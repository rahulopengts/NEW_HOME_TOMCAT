/*
 * File Name: system.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Aug/21/2013
 *
 *
 * Copyright (c) <2013>, <ram krishnan>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the names of its contributors
 *     may be used to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>

#include <typedefs.h>
#include <pltfrm.h>
#include <pltfrm_log.h>
#include <uart.h>
#include <radio.h>
#include <phy_pib.h>
#include <mac.h>
#include <mac_pib.h>
#ifdef DEV_TYPE_RFD
#include <rfd_app.h>
#elif defined(LPWMN_COORD)
#include <app.h>
#elif defined(DEV_TYPE_FFD)
#include <ffd_app_common.h>
#elif defined(MICRO_ONLY)
#include <mo_app.h>
#else
#include <lpwmn_app.h>
#endif
#include <system.h>
#include <node_mgr.h>
#include <aodvl.h>
#include <adp.h>
#include <gw.h>
#ifdef UC_FU_ENA
#include <fu.h>
#endif

#if defined(LPWMN_COORD) || defined(DEV_TYPE_FFD)
UINT32_t SYS_upTimeInSecs = 0;
#endif


#ifdef PLTFRM_EKMC160111X_ENA
#include <ekmc160111x.h>
#endif
#ifdef PLTFRM_REED_SWITCH_ENA
#include <reed_switch.h>
#endif

#ifdef DEV_TYPE_RFD
SINT32_t SYS_wakeUpIntervalSecs = 0;
#endif

volatile UINT16_t SYS_globalEvtMsk0 = 0x0,
                  SYS_globalEvtMsk1 = 0x0,
                  SYS_globalEvtMsk2 = 0x0;
SYS_buffDesc_s SYS_buffDescList[SYS_MAX_BUFF_DESC_CNT];
UINT8_t SYS_buff[SYS_BUFF_LEN];

UINT32_t SYS_memAllocDoneCnt = 0;
UINT32_t SYS_memFreeCnt = 0;
UINT16_t SYS_buffAllocFlrCnt = 0;
UINT16_t SYS_bdAllocFlrCnt = 0;
UINT8_t SYS_contBuffAllocFlrCnt = 0;

UINT8_t SYS_fatalInvoked = 0;
UINT16_t SYS_rebootDelaySecs = 0;


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
UINT16_t SYS_findFreeDescCnt(void)
{
   SYS_buffDesc_s *bd_p = SYS_buffDescList;
   UINT16_t idx, freeCnt = 0;

   for (idx=0; idx<SYS_MAX_BUFF_DESC_CNT; idx++)
   {
       if (bd_p->len == 0)
           freeCnt ++;
       bd_p ++;
   }

   return freeCnt;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void SYS_init(void)
{
   memset(SYS_buffDescList, 0, sizeof(SYS_buffDescList));
}

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
UINT8_t SYS_findNextInUseBlock(UINT16_t currOff)
{
   SYS_buffDesc_s *bd_p = SYS_buffDescList;
   UINT16_t idx, minOffDiff = 0xffff, minOffIdx = SYS_MAX_BUFF_DESC_CNT;

   for (idx=0; idx<SYS_MAX_BUFF_DESC_CNT; idx++)
   {
      if (bd_p->len != 0x0
          && (bd_p->offset >= currOff))
      {
          if ((bd_p->offset - currOff) <= minOffDiff)
          {
              minOffDiff = bd_p->offset - currOff;
              if (minOffDiff == 0x0)
                  return (UINT8_t)idx;
              else
                  minOffIdx = idx;
          }
      }

      bd_p ++;
   }

   return (UINT8_t)minOffIdx;
}

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
UINT8_t SYS_allocMem(UINT16_t allocLen)
{
   SYS_buffDesc_s *bd_p = SYS_buffDescList;
   UINT16_t idx, freeIdx = SYS_MAX_BUFF_DESC_CNT, inUseLen = 0x0, currOff = 0x0;

   for (idx=0; idx<SYS_MAX_BUFF_DESC_CNT; idx++)
   {
       if (bd_p->len > 0)
       {
           inUseLen += bd_p->len;
       }
       else
       {
           if (freeIdx == SYS_MAX_BUFF_DESC_CNT)
               freeIdx = idx;
       }
       bd_p ++;
   }

   if (freeIdx == SYS_MAX_BUFF_DESC_CNT
       || ((SYS_BUFF_LEN - inUseLen) < allocLen))
   {
	   goto _flr;
   }

   if (inUseLen == 0x0)
   {
       SYS_buffDescList[0].len = allocLen;
       SYS_buffDescList[0].offset = 0x0;
       SYS_memAllocDoneCnt ++;
       SYS_contBuffAllocFlrCnt = 0;
       return 0;
   }

   do
   {
       UINT8_t bdHndl = SYS_findNextInUseBlock(currOff);
       if (bdHndl == SYS_MAX_BUFF_DESC_CNT)
       {
           // Next in use block not found
           if ((SYS_BUFF_LEN - currOff) >= allocLen)
           {
               SYS_buffDescList[freeIdx].len = allocLen;
               SYS_buffDescList[freeIdx].offset = currOff;
               SYS_memAllocDoneCnt ++;
               SYS_contBuffAllocFlrCnt = 0;
               return (UINT8_t)freeIdx;
           }
           else
               break;
       }
       else
       {
           bd_p = &(SYS_buffDescList[bdHndl]);
           if ((bd_p->offset - currOff) >= allocLen)
           {
               SYS_buffDescList[freeIdx].len = allocLen;
               SYS_buffDescList[freeIdx].offset = currOff;
               SYS_memAllocDoneCnt ++;
               SYS_contBuffAllocFlrCnt = 0;
               return (UINT8_t)freeIdx;
           }

           currOff = bd_p->offset + bd_p->len;
           if (currOff >= SYS_BUFF_LEN)
               break;
       }
   } while(1);


_flr:
   SYS_buffAllocFlrCnt ++;

   // If mem allocation fails continuously for 128 requests, reset the node !!
   SYS_contBuffAllocFlrCnt ++;
   if (SYS_contBuffAllocFlrCnt > SYS_BACK_TO_BACK_MEM_ALLOC_FLR_CNT_THRESHOLD)
   {
	   SYS_fatal(SYS_FATAL_ERR_5000);
   }
   return SYS_MAX_BUFF_DESC_CNT;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void SYS_freeMem(UINT8_t bdHndl)
{
   if (bdHndl >=  SYS_MAX_BUFF_DESC_CNT
       || SYS_buffDescList[bdHndl].len == 0
       || SYS_buffDescList[bdHndl].offset >= SYS_BUFF_LEN)
   {
       SYS_fatal(SYS_FATAL_ERR_1);
   }
   else
   {
       SYS_buffDescList[bdHndl].len = 0x0;
       SYS_buffDescList[bdHndl].offset = 0x0;

       SYS_memFreeCnt ++;
   }

   return;
}


/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
void SYS_initTrscnsQ(SYS_trscnQ_s *queue_p)
{
   if (queue_p == NULL)
       SYS_fatal(SYS_FATAL_ERR_2);

   queue_p->qHead = SYS_MAX_BUFF_DESC_CNT;
   queue_p->qTail = SYS_MAX_BUFF_DESC_CNT;

   return;
}

/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
void SYS_delFromTrscnsQ(SYS_trscnQ_s *queue_p, UINT8_t bdHndl)
{
   UINT8_t inQHndl, prevInQHndl = SYS_MAX_BUFF_DESC_CNT, found = 0;

   // Does not free the memory associated with the bdHndl

   if (queue_p == NULL
       || ((inQHndl = queue_p->qHead) == SYS_MAX_BUFF_DESC_CNT)
       || (queue_p->qTail == SYS_MAX_BUFF_DESC_CNT)
       || (SYS_GET_BD_LEN(bdHndl) < 2))
   {
       SYS_fatal(SYS_FATAL_ERR_3);
   }

   do
   {
      if (bdHndl == inQHndl)
      {
          found = 1;
          if (prevInQHndl == SYS_MAX_BUFF_DESC_CNT)
          {
              // Remove the head of the queue
              if (SYS_GET_BD_LEN(inQHndl) < 2)
                  SYS_fatal(SYS_FATAL_ERR_4);

              queue_p->qHead = *(SYS_GET_BD_BUFF(inQHndl));
              if (queue_p->qHead > SYS_MAX_BUFF_DESC_CNT)
                  SYS_fatal(SYS_FATAL_ERR_5);

              if (queue_p->qHead == SYS_MAX_BUFF_DESC_CNT)
              {
                  if (queue_p->qTail != bdHndl)
                      SYS_fatal(SYS_FATAL_ERR_6);

                  queue_p->qTail = SYS_MAX_BUFF_DESC_CNT;
              }

              inQHndl = SYS_MAX_BUFF_DESC_CNT;
          }
          else
          {
              *(SYS_GET_BD_BUFF(prevInQHndl)) = *(SYS_GET_BD_BUFF(inQHndl));
              if (queue_p->qTail == bdHndl)
                  queue_p->qTail = prevInQHndl;
              inQHndl = SYS_MAX_BUFF_DESC_CNT;
          }
      }
      else
      {
          prevInQHndl = inQHndl;
          inQHndl = *(SYS_GET_BD_BUFF(inQHndl));
          if (inQHndl > SYS_MAX_BUFF_DESC_CNT)
              SYS_fatal(SYS_FATAL_ERR_7);
      }
   } while (inQHndl < SYS_MAX_BUFF_DESC_CNT);

   if (found == 0)
       SYS_fatal(SYS_FATAL_ERR_8);
   else
   {
       *(SYS_GET_BD_BUFF(bdHndl)) = SYS_MAX_BUFF_DESC_CNT;
   }

   return;
}


/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
void SYS_purgeTrscnsQ(SYS_trscnQ_s *queue_p)
{
   UINT8_t bdHndl;

   if (queue_p == NULL)
       SYS_fatal(SYS_FATAL_ERR_9);
  
   if (queue_p->qHead == SYS_MAX_BUFF_DESC_CNT)
   {
       if (queue_p->qTail != SYS_MAX_BUFF_DESC_CNT)
           SYS_fatal(SYS_FATAL_ERR_10);
        return;
   }

   bdHndl = queue_p->qHead;

   while (bdHndl < SYS_MAX_BUFF_DESC_CNT)
   {
       UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);
       UINT8_t nextBdHndl = *(buff_p);

       if (SYS_GET_BD_LEN(bdHndl) < 2)
           SYS_fatal(SYS_FATAL_ERR_11);

       SYS_freeMem(bdHndl);
      
       bdHndl = nextBdHndl; 
   }

   queue_p->qHead = SYS_MAX_BUFF_DESC_CNT;
   queue_p->qTail = SYS_MAX_BUFF_DESC_CNT;

   return;
}


/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
void SYS_addToTrscnsQ(SYS_trscnQ_s *queue_p, UINT8_t bdHndl)
{
   UINT8_t *buff_p;

   if (queue_p == NULL)
       SYS_fatal(SYS_FATAL_ERR_12);

   if (queue_p->qTail == SYS_MAX_BUFF_DESC_CNT)
   {
       if (queue_p->qHead != SYS_MAX_BUFF_DESC_CNT)
           SYS_fatal(SYS_FATAL_ERR_13);

       queue_p->qTail = queue_p->qHead = bdHndl;
   }
   else
   {
       // Add to tail
       buff_p = SYS_GET_BD_BUFF(queue_p->qTail);
       *(buff_p) = bdHndl;
       queue_p->qTail = bdHndl;
   }

   if (SYS_GET_BD_LEN(bdHndl) < 2)
       SYS_fatal(SYS_FATAL_ERR_14);

   buff_p = SYS_GET_BD_BUFF(bdHndl);
   *(buff_p) = SYS_MAX_BUFF_DESC_CNT;

   return;
}

#ifndef RADIO_TYPE_WIFI
#if !defined(DEV_TYPE_RFD) && !defined(DEV_TYPE_SNIFFER)
/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void SYS_periodicTmrExpCbFunc(void)
{
   static UINT8_t traffTSUpdate = 0;

   SYS_upTimeInSecs ++;

#ifdef DEV_TYPE_FFD
   if (SYS_rebootDelaySecs > 0)
   {
	   SYS_rebootDelaySecs --;
	   if (SYS_rebootDelaySecs == 0)
	   {
           PLTFRM_saveSysFEId(SYS_FATAL_ERR_REBOOT_ON_REQ);
           PLTFRM_mcuReset();
	   }
   }
#endif

#ifdef UC_FU_ENA
   FU_checkStartReflash();
#endif

#if defined(LPWMN_COORD) || defined(MAC_NEIGHBOR_TBL_ENA)
   {
      traffTSUpdate ++;
      if (traffTSUpdate >= SYS_TRAFF_UPDATE_INTERVAL)
      {
#ifdef LPWMN_COORD
          NM_updateTrafficTS(SYS_TRAFF_UPDATE_INTERVAL);
#endif
#ifdef MAC_NEIGHBOR_TBL_ENA
          MAC_updNeighborTblPktTS(SYS_TRAFF_UPDATE_INTERVAL);
#endif
          traffTSUpdate = 0;
      }
   }
#endif

#if defined(LPWMN_COORD) && defined(PLTFRM_UART_ENA)
   UART_checkRcvrState();
#endif

   AODVL_periodicTmrExpCbFunc();

#if defined(DEV_TYPE_FFD) && !defined(NODE_HOST_IF_ENA)
   FFD_APP_periodicTmrExpCbFunc();
#endif

#if defined(DEV_TYPE_FFD) || defined(LPWMN_COORD)
   // Dec/9/14 - duplicate check algo
   MAC_checkRcvdPktInfoList();

   if (MAC_cntxt.timeSinceLastBncReq != 0xffff)
       MAC_cntxt.timeSinceLastBncReq ++;
#endif

#ifdef RFD_KA_ENA
#ifdef LPWMN_COORD
   NM_checkRFDKATmo();
#else
   MAC_checkRFDKATmo();
#endif
#endif

#ifdef CUSTOMER_ANDY_SPECIFIC_COORD_BUILD
   GW_getSnsrOp();
#endif

#ifdef LPWMN_COORD
#ifdef ADP_FRAG_ENA
   if (ADP_cntxt.reassyCntxtsInUse > 0)
       ADP_checkReassyTmo();
#endif
#endif

#ifdef LPWMN_COORD
   if ((SYS_upTimeInSecs % NM_REASSOC_TMO_CHECK_INTERVAL_SECS) == 0)
   {
	   NM_checkReassocTimeOuts(NM_REASSOC_TMO_CHECK_INTERVAL_SECS);
   }
#endif

#if defined(DEV_TYPE_FFD) || defined(LPWMN_COORD)
   if ((SYS_upTimeInSecs & 0x3)  == 0x0)
	   ADP_checkBcLogTbl(4);
#endif

}
#endif


#if defined(DEV_TYPE_FFD) || defined(DEV_TYPE_SNIFFER)
/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void SYS_checkEnterLPM(void)
{
   PLTFRM_globalIntState_t intState;

   // This function will put the MSP430 into LPM3 if no event flag bit is set.

   // Disable interrupts
   PLTFRM_saveIntStateAndDisable(intState);

   if (SYS_globalEvtMsk0 == 0x0
       && SYS_globalEvtMsk1 == 0x0
       && SYS_globalEvtMsk2 == 0x0)
   {
       // No event pending. Enable interrupts and enter LPM at the same time.
       PLTFRM_enterLPM();
   }
   else
   {
       // Restore interrupt state
       PLTFRM_restoreIntState(intState);
   }

   return;
}
#endif



#ifdef CUSTOMER_ANDY_SPECIFIC_COORD_BUILD
UINT16_t __rxPktStuckInRxFifoCnt = 0;
#endif

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void SYS_rcvdPktCheck(void)
{
#ifdef CUSTOMER_ANDY_SPECIFIC_COORD_BUILD
   if ((SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_MAC_FRAME_RX_DONE) == 0)
   {
	   UINT8_t __rxFifoByteCnt = 0;
       CC1101_getRXFIFOByteCnt(&__rxFifoByteCnt);
       if (__rxFifoByteCnt >= 15)
       {
    	   __rxPktStuckInRxFifoCnt ++;
	       SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_FRAME_RX_DONE;
       }
   }
#endif


   if (SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_MAC_FRAME_RX_DONE
       && (!(SYS_globalEvtMsk2 & SYS_GLOBAL_EVT_NODE_RESET_PEND)))
   {
       SYS_globalEvtMsk0 &= ~(SYS_GLOBAL_EVT_MAC_FRAME_RX_DONE);
       MAC_evtHndlr(SYS_GLOBAL_EVT_MAC_FRAME_RX_DONE);
   }
}
#endif


#ifdef MICRO_ONLY
/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void SYS_loop(void)
{
    SYS_init();

    MO_appInit();

    for (;;)
    {
       if (SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_GPIO_INT_EVT)
       {
           SYS_globalEvtMsk1 &= ~(SYS_GLOBAL_EVT_GPIO_INT_EVT);
           MO_appGPIOEvtHndlr();
       }

       if (SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_TIMER_A0_EXPIRY)
       {
           SYS_globalEvtMsk0 &= ~(SYS_GLOBAL_EVT_TIMER_A0_EXPIRY);
           PLTFRM_timerA1ExpHndlr();
       }

       if (SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_TIMER_A1_EXPIRY)
       {
           SYS_globalEvtMsk0 &= ~(SYS_GLOBAL_EVT_TIMER_A1_EXPIRY);
           PLTFRM_timerA1ExpHndlr();
       }

#ifdef PLTFRM_UART_ENA
#ifdef UART_TX_INT_ENA
       if (SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_UART_BLOCK_TX_DONE)
       {
           SYS_globalEvtMsk1 &= ~(SYS_GLOBAL_EVT_UART_BLOCK_TX_DONE);
           MO_uartTxDoneHndlr();
       }
#endif
#endif

       if (SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_ENTER_LPM_REQUEST)
       {
           SYS_globalEvtMsk1 &= ~(SYS_GLOBAL_EVT_ENTER_LPM_REQUEST);
           PLTFRM_enterLPM();
       }
    }
}

#else

#if 0
UINT16_t _cntr = 0;
volatile UINT8_t _junkTF = 0;

void __junkTestFunc(void)
{
	_junkTF = 1;
	_cntr ++;
	P2OUT ^= BIT0;
}
#endif

#ifdef RADIO_TYPE_WIFI
/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void SYS_loop(void)
{
    SYS_init();

    __udpMain();
}

#else
/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void SYS_loop(void)
{
    SYS_init();

    PHY_PIB_init();

    MAC_init();

    ADP_init();

#if !defined(DEV_TYPE_RFD) && !defined(DEV_TYPE_SNIFFER)
    AODVL_init();
#endif

#ifndef DEV_TYPE_SNIFFER
#ifdef DEV_TYPE_RFD
    RFD_APP_COMMON_init();
#elif defined (DEV_TYPE_FFD) && !defined(NODE_HOST_IF_ENA)
    FFD_APP_commonInit();
#endif
#endif

#ifdef UC_FU_ENA
    FU_init();
#endif


#ifdef GW_ENA
    GW_init();
#endif

#ifndef EKM160111X_ENA_ON_PWR_UP
    NM_start();
#endif

#if !defined(DEV_TYPE_RFD) && !defined(DEV_TYPE_SNIFFER)
    PLTFRM_start1HzPeriodicTimer(SYS_periodicTmrExpCbFunc);
#endif

#if defined(PLTFRM_UART_ENA) || defined(NODE_HOST_IF_ENA)
    UART_start(PLTFRM_UART_HW_1_DEV_ID);
#endif

#ifdef LPWMN_COORD
#ifdef GW_ENA
#ifndef GREENOPIA_COORD_BUILD
    GW_sendSysBootEvt();
#endif
#endif
#endif

#ifdef TEST_TIMER_A0
    P2DIR |= BIT0;
    for (;;)
    {
        if (PLTFRM_startTimerA0(100, 0, __junkTestFunc) != PLTFRM_STS_SUCCESS)
            SYS_fatal(SYS_FATAL_ERR_34);

        while (_junkTF == 0)
        {
            if (SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_TIMER_A0_EXPIRY)
            {
                SYS_globalEvtMsk0 &= ~(SYS_GLOBAL_EVT_TIMER_A0_EXPIRY);
                PLTFRM_timerA0ExpHndlr();
            }
        }

        _junkTF = 0;
    }
#endif

    for (;;)
    {
#if defined(DEV_TYPE_RFD) || defined(DEV_TYPE_FFD) && !defined(NODE_HOST_IF_ENA)
       if (SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_GPIO_INT_EVT)
       {
           SYS_globalEvtMsk1 &= ~(SYS_GLOBAL_EVT_GPIO_INT_EVT);
           APP_COMMON_procGPIOEvt();
       }
#endif

#if defined(DEV_TYPE_FFD) || defined(DEV_TYPE_SNIFFER)
       // SYS_checkEnterLPM();
#endif

#if 0
#ifdef GPIO_MGMT_ENA
       if (PLTFRM_checkGPIOEvt())
           APP_procGPIOEvt();
#endif
#endif

#ifndef CC1101_WOR_ENA
       RADIO_checkRadioErrors( );
#endif

#if defined(LPWMN_COORD) || defined(DEV_TYPE_SNIFFER)
#ifdef PLTFRM_UART_ENA
#ifndef UART_TX_INT_ENA
       // Not using UART transmit done interrupt since interrupt interferes with SPI
       // connection to cc2520. Polling UART transmit over condition instead.
       UART_POLL_TX();
#endif
#endif
#endif

       if (MAC_TX_MODULE_IS_IDLE())
    	   MAC_checkPendAction();

#ifdef DEV_TYPE_RFD
       // Do not go into deep sleep if coordinator poll is pending or is in progress
       while ((!(SYS_globalEvtMsk2 & SYS_GLOBAL_EVT_IMAGE_REFLASH_PENDING))
    		   && (SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_ENTER_LPM_REQUEST)
               && MAC_TX_MODULE_IS_IDLE())
       {
#ifdef RFD_KA_ENA
    	   UINT8_t kaSent = 0;

           RFD_APP_sleepTimeElapsed = 0;
#endif
           SYS_globalEvtMsk1 &= ~(SYS_GLOBAL_EVT_ENTER_LPM_REQUEST);

#ifdef RADIO_CC2520
           if (RADIO_state != RADIO_STATE_LPM)
#else
           if (!(RADIO_inLPM()))
#endif
           {
               RADIO_enterLPM();

               // Clear all radio related flags since these are no longer valid
               // as the radio is in low power mode.
               SYS_globalEvtMsk0 &= ~SYS_GLOBAL_EVT_RADIO_RX_FIFO_OVFL;
               SYS_globalEvtMsk0 &= ~SYS_GLOBAL_EVT_MAC_FRAME_TX_DONE;
               SYS_globalEvtMsk0 &= ~SYS_GLOBAL_EVT_MAC_FRAME_RX_DONE;
           }

           PLTFRM_stopWakeUpTimer();

           if (SYS_wakeUpIntervalSecs > 0)
               PLTFRM_startWakeUpTimer(SYS_wakeUpIntervalSecs);
#ifdef RFD_KA_ENA
           else
           {

               if (NM_cntxt.rfdKeepAliveTmoSecs > 0)
        	       PLTFRM_startWakeUpTimer((NM_cntxt.rfdKeepAliveTmoSecs << 1));  // Yes, twice not half
           }
#endif

#ifndef RFD_KA_ENA
           PLTFRM_intDisable();
           RFD_APP_prepareToEnterLPM();  // Let APP know that micro is going to sleep.
           PLTFRM_enterLPM();

#else
           PLTFRM_intDisable();
           RFD_APP_prepareToEnterLPM();  // Let APP know that micro is going to sleep.

           do
           {
               PLTFRM_enterLPM();

               if (!(SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_WAKEUP_TIMER_EXPIRY_EVT)
                   && (!(SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_GPIO_INT_EVT))
                   && MAC_nodeJoined())
               {
                   // Check if keep alive needs to be sent to the parent.
                   if (RFD_APP_checkKAProc())
                   {
                       MAC_wakeUpProc();
                       RFD_APP_keepAliveTmrExpEvt();
                       kaSent = 1;
                       break;
                   }
               }
               else
                   break;

           } while (1);

           if (kaSent)
        	   break;
#endif

#if 0
           {
               UINT16_t sv = PLTFRM_getAvgSupplyVoltage();
               if (sv < PLTFRM_LOW_VOLTAGE_TH_IN_MV)
               {
                   // Supply voltage is too low for normal operations.
                   // Indicate this to the app and let it decide whether
                   // to continue or go back to sleep. APP can choose
                   // to change SYS_wakeUpIntervalSecs.
                   if (!RFD_APP_continueWithLowSupplyV())
                   {
                       SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ENTER_LPM_REQUEST;
                       continue;
                   }
               }
           }
#endif

           MAC_wakeUpProc();


           if (SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_WAKEUP_TIMER_EXPIRY_EVT)
           {
               // rkris@wisense.in / Jan/12/15 / commented out since RFD_APP_wakeUpTimerExpEvt()
               // does not seem to be doing anything.
               SYS_globalEvtMsk1 &= ~SYS_GLOBAL_EVT_WAKEUP_TIMER_EXPIRY_EVT;
               // RFD_APP_wakeUpTimerExpEvt();
           }

           if (SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_TIMER_A1_EXPIRY)
           {
               SYS_globalEvtMsk0 &= ~(SYS_GLOBAL_EVT_TIMER_A1_EXPIRY);
               PLTFRM_timerA1ExpHndlr();
           }

           if (SYS_wakeUpIntervalSecs > 0)
               PLTFRM_stopWakeUpTimer();  // In case micro is woken by a GPIO interrupt

#if 0
#ifdef GPIO_MGMT_ENA
           PLTFRM_checkGPIOEvt();
#endif
#endif

           RFD_APP_wakeUpProc();

#ifdef PLTFRM_EKMC160111X_ENA
           if (SYS_globalEvtMsk2 & SYS_GLOBAL_EVT_JOIN_NWK_REQ)
           {
               SYS_globalEvtMsk2 &= ~SYS_GLOBAL_EVT_JOIN_NWK_REQ;
               NM_start();
           }
#endif
       }  // while()
#endif

#ifdef LPWMN_COORD
#ifdef GPIO_MGMT_ENA
       PLTFRM_checkGPIOEvt();
#endif
#endif

       if (SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_TIMER_A0_EXPIRY)
       {
           SYS_globalEvtMsk0 &= ~(SYS_GLOBAL_EVT_TIMER_A0_EXPIRY);
           PLTFRM_timerA0ExpHndlr();
       }

       if (SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_TIMER_A1_EXPIRY)
       {
           SYS_globalEvtMsk0 &= ~(SYS_GLOBAL_EVT_TIMER_A1_EXPIRY);
           PLTFRM_timerA1ExpHndlr();
       }

#ifdef DEV_TYPE_RFD
       if (SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_RFD_REASSOC_REQ)
       {
    	   SYS_wakeUpIntervalSecs = 0;  // rkris@wisense.in / april/18/15
           SYS_globalEvtMsk1 &= ~SYS_GLOBAL_EVT_RFD_REASSOC_REQ;
           NM_startScan();
       }
#endif

       if (SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_TIMER_1HZ_PERIODIC_EXPIRY)
       {
           SYS_globalEvtMsk0 &= ~(SYS_GLOBAL_EVT_TIMER_1HZ_PERIODIC_EXPIRY);
           PLTFRM_timer1HzPeriodicExpHndlr();
       }

#if defined(LPWMN_COORD) || defined(DEV_TYPE_SNIFFER) || defined(DEV_TYPE_FFD)
#ifdef PLTFRM_UART_ENA
       if (SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_UART_FRAME_HDR_RCVD)
       {
           SYS_globalEvtMsk1 &= ~(SYS_GLOBAL_EVT_UART_FRAME_HDR_RCVD);
           UART_checkRcvdFrameHdr();
       }

       if (SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_UART_FRAME_PYLD_RCVD)
       {
           SYS_globalEvtMsk1 &= ~(SYS_GLOBAL_EVT_UART_FRAME_PYLD_RCVD);
           UART_checkRcvdFramePyld();
       }

#ifdef UART_TX_INT_ENA
       if (SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_UART_BLOCK_TX_DONE)
       {
           SYS_globalEvtMsk1 &= ~(SYS_GLOBAL_EVT_UART_BLOCK_TX_DONE);
           UART_blockTxDoneEvtHndlr();
       }
#endif
#endif
#endif

#ifndef DEV_TYPE_SNIFFER
       if (SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_MAC_DATA_CONFIRM
           && (MAC_TX_MODULE_CAN_CONFIRM()))
       {
#ifdef CUSTOMER_ANDY_SPECIFIC_COORD_BUILD
           GW_relayCnfrm(LPWMN_MAC_MODULE,
                         MAC_cntxt.datacnfrmParams.status);
#else
           if (!(SYS_globalEvtMsk2 & SYS_GLOBAL_EVT_NODE_RESET_PEND))
               ADP_evtHndlr(SYS_GLOBAL_EVT_MAC_DATA_CONFIRM,
                            (void *)&(MAC_cntxt.datacnfrmParams));
#endif
           SYS_globalEvtMsk0 &= ~(SYS_GLOBAL_EVT_MAC_DATA_CONFIRM);
           MAC_resetTxModState();
       }

       SYS_rcvdPktCheck();

       if (SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_MAC_FRAME_TX_DONE)
       {
           SYS_globalEvtMsk0 &= ~(SYS_GLOBAL_EVT_MAC_FRAME_TX_DONE);
           MAC_evtHndlr(SYS_GLOBAL_EVT_MAC_FRAME_TX_DONE);
       }

       SYS_rcvdPktCheck();

       if ((SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_APP_DATA_REQ_POST_RT_DISC)
           && (!(SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_PENDING_FRAME_TRX_REQ))  // Pending frame trx has highest priority
           && (!(SYS_globalEvtMsk2 & SYS_GLOBAL_EVT_NODE_RESET_PEND)))
       {
           // rkris@wisense.in - dec/25/14 - bug fix (MAC-IDLE-CHECK was missing)
           if (MAC_TX_MODULE_IS_IDLE())
           {
               SYS_globalEvtMsk1 &= ~(SYS_GLOBAL_EVT_APP_DATA_REQ_POST_RT_DISC);
               ADP_retryAppDataTxPostRtDisc();
           }
       }

       SYS_rcvdPktCheck();

#ifndef DEV_TYPE_RFD
       if (SYS_globalEvtMsk0 & AODVL_CTRL_PKT_TX_REQUEST
           && (!(SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_PENDING_FRAME_TRX_REQ))  // Pending frame trx has highest priority
           && (!(SYS_globalEvtMsk2 & SYS_GLOBAL_EVT_NODE_RESET_PEND)))
       {
           if (MAC_TX_MODULE_IS_IDLE())
           {
               // Called function will update the AODVL_CTRL_PKT_TX_REQUEST bit
               AODVL_pktTxProc(  );
           }
       }
#endif
#endif

#if defined(DEV_TYPE_RFD)  || defined(DEV_TYPE_FFD)
       /*
        * rkris@wisense.in - march/1/15
        * MLME_scanRequest() cannot be called directly
        */
       if ((SYS_globalEvtMsk2 & SYS_GLOBAL_EVT_NODE_SCAN_REQ)
           && MAC_TX_MODULE_IS_IDLE())
       {
           SYS_globalEvtMsk2 &= ~(SYS_GLOBAL_EVT_NODE_SCAN_REQ);
           NM_procPendScanReq();
       }
#endif

       SYS_rcvdPktCheck();

#if defined(DEV_TYPE_FFD) || defined(DEV_TYPE_RFD)
       if ((SYS_globalEvtMsk2 & SYS_GLOBAL_EVT_NWK_RESTART_CMD_RCVD)
           && MAC_TX_MODULE_IS_IDLE())
       {
           UINT8_t t;
           SYS_globalEvtMsk2 &= ~(SYS_GLOBAL_EVT_NWK_RESTART_CMD_RCVD);
           MAC_buildSendNwkRestartCmd();
           PLTFRM_saveSysFEId(SYS_FATAL_ERR_NWK_RESTART);
           for (t=0; t<100; t++) PLTFRM_delay1MilliSec();
           PLTFRM_resetSelf();
       }
#endif

#ifndef DEV_TYPE_SNIFFER
#ifndef DEV_TYPE_RFD
       if ((SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_PENDING_FRAME_TRX_REQ)
           && (!(SYS_globalEvtMsk2 & SYS_GLOBAL_EVT_NODE_RESET_PEND))
           && (MAC_TX_MODULE_IS_IDLE()))
       {
           SYS_globalEvtMsk1 &= ~(SYS_GLOBAL_EVT_PENDING_FRAME_TRX_REQ);
           MAC_sendPendFrame();
       }
#endif

       SYS_rcvdPktCheck();

#ifdef DEV_TYPE_RFD
#ifdef RFD_COORD_POLL_ENA
       if ((SYS_globalEvtMsk2 & SYS_GLOBAL_EVT_COORD_POLL_PENDING)
           && (!(SYS_globalEvtMsk2 & SYS_GLOBAL_EVT_NODE_RESET_PEND))
           && (MAC_TX_MODULE_IS_IDLE()))
       {
           SYS_globalEvtMsk2 &= ~(SYS_GLOBAL_EVT_COORD_POLL_PENDING);

           if (MLME_pollRequest() == MAC_STS_SUCCESS)
           {
               SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_COORD_POLL_IN_PROGRESS;
           }
           else
           {
               // Inform APP about this failure !!
               RFD_APP_coordPollStartFlr();
           }
       }

       if (SYS_globalEvtMsk2 & SYS_GLOBAL_EVT_MLME_POLL_REQ_CONFIRM)
       {
           SYS_globalEvtMsk2 &= ~SYS_GLOBAL_EVT_COORD_POLL_IN_PROGRESS;
           SYS_globalEvtMsk2 &= ~SYS_GLOBAL_EVT_MLME_POLL_REQ_CONFIRM;
           RFD_APP_pollReqCnfrmHndlr();
       }
#endif
#endif

#ifndef DEV_TYPE_RFD
       if (SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_APP_DATA_REQUEST
           && (!(SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_PENDING_FRAME_TRX_REQ))  // Pending frame trx has highest priority
           && (!(SYS_globalEvtMsk2 & SYS_GLOBAL_EVT_NODE_RESET_PEND)))
       {
           if (MAC_TX_MODULE_IS_IDLE())
           {
#ifdef LPWMN_COORD
               NC_dataReqProc();
#else
#ifdef DIS_ENA
               APP_buildSendPendDataReqs();
#endif

#endif
               SYS_globalEvtMsk0 &= ~(SYS_GLOBAL_EVT_APP_DATA_REQUEST);
           }
       }
#endif

       SYS_rcvdPktCheck();

#ifdef LPWMN_COORD
#ifdef GW_ENA
       if (SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_GW_RELAY_REQUEST
           && (!(SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_PENDING_FRAME_TRX_REQ)))  // Pending frame trx has highest priority
       {
           if (MAC_TX_MODULE_IS_IDLE())
           {
               GW_relayMsgToRemoteNode();
               SYS_globalEvtMsk1 &= ~(SYS_GLOBAL_EVT_GW_RELAY_REQUEST);
           }
       }
#endif
#endif
#ifdef NODE_HOST_IF_ENA
       if (SYS_globalEvtMsk2 & SYS_GLOBAL_EVT_NHI_PING_REQ
           && (!(SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_PENDING_FRAME_TRX_REQ))  // Pending frame trx has highest priority
           && (!(SYS_globalEvtMsk2 & SYS_GLOBAL_EVT_NODE_RESET_PEND)))
       {
           if (MAC_TX_MODULE_IS_IDLE())
           {
        	   SYS_globalEvtMsk2 &= ~(SYS_GLOBAL_EVT_NHI_PING_REQ);
        	   NHI_sendPing();
           }
       }
#else

#ifdef UC_FU_ENA
#if defined(DEV_TYPE_RFD) || defined(DEV_TYPE_FFD)
       if (SYS_globalEvtMsk2 & SYS_GLOBAL_EVT_PENDING_UC_FU_COMM
           && (!(SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_PENDING_FRAME_TRX_REQ))  // Pending frame trx has highest priority
           && (!(SYS_globalEvtMsk2 & SYS_GLOBAL_EVT_NODE_RESET_PEND)))
       {
           if (MAC_TX_MODULE_IS_IDLE())
           {
               SYS_globalEvtMsk2 &= ~SYS_GLOBAL_EVT_PENDING_UC_FU_COMM;
               UC_FU_sendResp();
           }
       }
#endif
#endif

       // Check if RFD/FFD app wants to send out any data
       if (SYS_globalEvtMsk2 & SYS_GLOBAL_EVT_PENDING_APP_DATA_REQ
           && (!(SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_PENDING_FRAME_TRX_REQ))  // Pending frame trx has highest priority
           && (!(SYS_globalEvtMsk2 & SYS_GLOBAL_EVT_NODE_RESET_PEND)))
       {
           if (MAC_TX_MODULE_IS_IDLE())
           {
               SYS_globalEvtMsk2 &= ~(SYS_GLOBAL_EVT_PENDING_APP_DATA_REQ);
#ifdef DEV_TYPE_RFD
               RFD_APP_procPendDataReq();
#elif defined (DEV_TYPE_FFD)
               FFD_APP_procPendDataReq();
#endif
           }
       }
#endif

       SYS_rcvdPktCheck();

       if (SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_ADP_DATA_CONFIRM
           && (!(SYS_globalEvtMsk2 & SYS_GLOBAL_EVT_NODE_RESET_PEND)))
       {
           SYS_globalEvtMsk0 &= ~(SYS_GLOBAL_EVT_ADP_DATA_CONFIRM);
#ifdef DEV_TYPE_RFD
           RFD_APP_evtHndlr(SYS_GLOBAL_EVT_ADP_DATA_CONFIRM,
                            &(ADP_cntxt.cnfrmSts));
#else
#ifdef NODE_HOST_IF_ENA
           NHI_evtHndlr(SYS_GLOBAL_EVT_ADP_DATA_CONFIRM,
                        &(ADP_cntxt.cnfrmSts));
#else
           APP_evtHndlr(SYS_GLOBAL_EVT_ADP_DATA_CONFIRM,
                        &(ADP_cntxt.cnfrmSts));
#endif
#endif
       }

       SYS_rcvdPktCheck();

       if (SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_ADP_DATA_INDICATION
           && (!(SYS_globalEvtMsk2 & SYS_GLOBAL_EVT_NODE_RESET_PEND)))
       {
           SYS_globalEvtMsk0 &= ~(SYS_GLOBAL_EVT_ADP_DATA_INDICATION);
#ifdef DEV_TYPE_RFD
           RFD_APP_evtHndlr(SYS_GLOBAL_EVT_ADP_DATA_INDICATION,
                            &(ADP_cntxt.dataIndParams));

           SYS_rcvdPktCheck();
#else
#ifndef NODE_HOST_IF_ENA
           APP_evtHndlr(SYS_GLOBAL_EVT_ADP_DATA_INDICATION,
                        &(ADP_cntxt.dataIndParams));
#endif
#endif
       }

       SYS_rcvdPktCheck();

       if (SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_ADP_TRSCN_PENDING
           && (!(SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_PENDING_FRAME_TRX_REQ))  // Pending frame trx has highest priority
           && (!(SYS_globalEvtMsk2 & SYS_GLOBAL_EVT_NODE_RESET_PEND)))
       {
           if (MAC_TX_MODULE_IS_IDLE())
           {
               ADP_procPendingTrscns();
           }
       }

       SYS_rcvdPktCheck();

       // Bug fix - rkris@wisense.in - dec/23/14
       // MAC_TX_MODULE_CAN_RETRY_CSMA() check was missing
       if ((SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_MAC_RETRY_CCA)
           && (MAC_TX_MODULE_CAN_RETRY_CSMA()))
       {
           if (SYS_globalEvtMsk2 & SYS_GLOBAL_EVT_NODE_RESET_PEND)
               MAC_resetTxModState();
           else
           {
               SYS_globalEvtMsk0 &= ~(SYS_GLOBAL_EVT_MAC_RETRY_CCA);
               MAC_evtHndlr(SYS_GLOBAL_EVT_MAC_RETRY_CCA);
           }
       }
#endif

       if (SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_MAC_DATA_INDICATION)
       {
#ifdef DEV_TYPE_RFD
    	   if (SYS_globalEvtMsk2 & SYS_GLOBAL_EVT_IMAGE_REFLASH_PENDING)
    	   {
    		   SYS_globalEvtMsk2 &= ~SYS_GLOBAL_EVT_IMAGE_REFLASH_PENDING;
    		   if (SYS_BD_HNDL_IS_VALID(MAC_cntxt.rcvdPktInfo.bdHndl))
    		   {
    			   SYS_freeMem(MAC_cntxt.rcvdPktInfo.bdHndl);
    			   MAC_cntxt.rcvdPktInfo.bdHndl = SYS_INV_BD_HDNL;
    		   }
    	   }
#endif

#ifdef DEV_TYPE_SNIFFER


#else
           ADP_evtHndlr(SYS_GLOBAL_EVT_MAC_DATA_INDICATION,
                        &(MAC_cntxt.rcvdPktInfo));

           SYS_globalEvtMsk0 &= ~(SYS_GLOBAL_EVT_MAC_DATA_INDICATION);
#endif
       }

#ifndef DEV_TYPE_SNIFFER
#ifndef LPWMN_COORD
       if (SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_MAC_SCAN_CONFIRM)
       {
            SYS_globalEvtMsk0 &= ~(SYS_GLOBAL_EVT_MAC_SCAN_CONFIRM);
            NM_evtHndlr(SYS_GLOBAL_EVT_MAC_SCAN_CONFIRM,
                        (void *)&(MAC_cntxt.scanCnfrmParams));
       }

       if (SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_MAC_ASSOC_CONFIRM)
       {
            SYS_globalEvtMsk1 &= ~(SYS_GLOBAL_EVT_MAC_ASSOC_CONFIRM);
            NM_evtHndlr(SYS_GLOBAL_EVT_MAC_ASSOC_CONFIRM,
                        (void *)&(MAC_cntxt.assocCnfrmParams));
       }
#endif

#ifndef DEV_TYPE_RFD
       if (SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_MAC_BCN_REQUEST
           && (!(SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_PENDING_FRAME_TRX_REQ))  // Pending frame trx has highest priority
           && (!(SYS_globalEvtMsk2 & SYS_GLOBAL_EVT_NODE_RESET_PEND)))
       {
           if (MAC_TX_MODULE_IS_IDLE())
           {
#ifdef RADIO_CC1101
               // <TODO> Find another way !!
               CC1101_insertTurnAroundDelay();
#endif
               SYS_globalEvtMsk0 &= ~(SYS_GLOBAL_EVT_MAC_BCN_REQUEST);
               MAC_evtHndlr(SYS_GLOBAL_EVT_MAC_BCN_REQUEST);
           }
       }

       SYS_rcvdPktCheck();

       if (SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_MAC_DATA_REQ_CMD_RCVD
           && (!(SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_PENDING_FRAME_TRX_REQ)))  // Pending frame trx has highest priority
       {
           if (MAC_TX_MODULE_IS_IDLE())
           {
               SYS_globalEvtMsk0 &= ~(SYS_GLOBAL_EVT_MAC_DATA_REQ_CMD_RCVD);
               MAC_evtHndlr(SYS_GLOBAL_EVT_MAC_DATA_REQ_CMD_RCVD);
           }
       }

       SYS_rcvdPktCheck();
#endif
#endif
    }
}
#endif
#endif
/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void SYS_fatal(UINT16_t sysFEId)
{
	__bic_SR_register(GIE);

   if (SYS_fatalInvoked == 0)
   {
       SYS_fatalInvoked = 1;

       PLTFRM_ON_LED_1();
       PLTFRM_OFF_LED_2();

       // Write the fault identifier to on board NVM.
       PLTFRM_saveSysFEId(sysFEId);

#ifdef SYS_REBOOT_ON_ASSERT
       {
          UINT16_t t;
          for (t=0; t<100; t++)
        	   PLTFRM_delay1MilliSec();
          PLTFRM_resetSelf();
       }
#endif

       for(;;)
       {
           UINT16_t idx;

#ifdef PLTRM_SYS_FATAL_ID_ON_GPIO_ENA
           PLTFRM_outputSysFEId(sysFEId);
#endif

           for (idx=0; idx<500; idx++)
           {
               PLTFRM_delay1MilliSec();
           }
           PLTFRM_TOGGLE_LED_1();
           PLTFRM_TOGGLE_LED_2();
       }
   }
}
