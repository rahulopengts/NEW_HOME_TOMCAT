/*
 * File Name : sync_rt.c
 * Author : ram krishnan (rkris@wisense.in)
 * Created : 9/28/2014
 *
 *
 * Copyright (c) <2014>, <ram krishnan>
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

#ifdef LPWMN_COORD

#include <system.h>
#include <sync_rt.h>
#include <util.h>
#include <gw.h>

void __SYNC_RT_addToTxPendQ(SYNC_RT_cntxt_s *dev_p, UINT8_t bdHndl);

SYNC_RT_cntxt_s SYNC_RT_cntxtList[PLTFRM_SYNC_RT_DEV_CNT] =
{
   {
      PLTFRM_SYNC_RT_1_DEV_ID,
      {
         {PLTFRM_GPIO_PORT_1, PLTFRM_GPIO_PORT_PIN_5},
         {PLTFRM_GPIO_PORT_1, PLTFRM_GPIO_PORT_PIN_5},
         {PLTFRM_GPIO_PORT_1, PLTFRM_GPIO_PORT_PIN_5},
         {PLTFRM_GPIO_PORT_1, PLTFRM_GPIO_PORT_PIN_5},
         {PLTFRM_GPIO_PORT_1, PLTFRM_GPIO_PORT_PIN_5},
         {PLTFRM_GPIO_PORT_1, PLTFRM_GPIO_PORT_PIN_5},
      },
   }
};


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
SYNC_RT_cntxt_s *SYNC_RT_getDevCntxt(UINT8_t devId)
{
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_SYNC_RT_DEV_CNT; idx++)
   {
      if (SYNC_RT_cntxtList[idx].devId == devId)
          return (&(SYNC_RT_cntxtList[idx]));
   }

   return NULL;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void SYNC_RT_wrBit(SYNC_RT_cntxt_s *dev_p)
{
  SYNC_RT_sigInfo_s *sigInfo_p = &(dev_p->sigList[SYNC_RT_TX_DATA_OUT_SIG_ID]);
  UINT8_t bitVal;

  bitVal = dev_p->txBuff_p[dev_p->txBuffOff];
  bitVal &= (1 << dev_p->txBitOff);

  // Put data bit on the line
  PLTFRM_setGPIOPinOutput(sigInfo_p->portNr,
                          sigInfo_p->pinNr,
                          bitVal ? 0x1 : 0x0);

  PLTFRM_delay10MicroSecs();  // Set up time

  sigInfo_p = &(dev_p->sigList[SYNC_RT_TX_ACK_IN_SIG_ID]);

  // Enable interrupt on the ACK pin ...
  PLTFRM_gpioPinIntCtrlOpn(sigInfo_p->portNr, sigInfo_p->pinNr,
                           PLTFRM_ENABLE_GPIO_PIN_INT, 0x1);

  sigInfo_p = &(dev_p->sigList[SYNC_RT_TX_CLK_OUT_SIG_ID]);

  // Strobe CLK
  PLTFRM_setGPIOPinOutput(sigInfo_p->portNr, sigInfo_p->pinNr, 0x0);
  PLTFRM_delay10MicroSecs();
  PLTFRM_setGPIOPinOutput(sigInfo_p->portNr, sigInfo_p->pinNr, 0x1);

  return;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void  SYNC_RT_wrBlock(SYNC_RT_cntxt_s *dev_p,
                      UINT8_t *buff_p, UINT16_t buffLen)
{
   if (buff_p == ((void *)0) || buffLen == 0x0)
   {
       SYS_fatal(SYS_FATAL_ERR_247);
       return;
   }

   if (dev_p->txBuffLen > 0)
       SYS_fatal(SYS_FATAL_ERR_248);

   dev_p->txBuff_p = buff_p;
   dev_p->txBuffLen = buffLen;
   dev_p->txBuffOff = 0x0;
   dev_p->txBitOff = 0x0;

#ifdef SYNC_RT_STATS_EN
   dev_p->frameTxStartCnt ++;
#endif

   // Send out the first bit
#ifdef SYNC_RT_STATS_EN
   dev_p->bitTxCnt ++;
#endif

   SYNC_RT_wrBit(dev_p);

   return;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void SYNC_RT_blockTxDoneHndlr(SYNC_RT_cntxt_s *dev_p)
{
   SYS_trscnQ_s *trscnQ_p = &(dev_p->pendTxtrscnQ);

   if (SYS_TRSCN_Q_IS_NOT_EMPTY(trscnQ_p))
   {
       UINT8_t bdHndl = SYS_GET_TRSCN_Q_HEAD(trscnQ_p);

       SYS_delFromTrscnsQ(trscnQ_p, bdHndl);
       SYS_freeMem(bdHndl);

       dev_p->txBuffLen = 0x0;
       dev_p->txBuffOff = 0x0;
       dev_p->txBuff_p = (void *)0;

       // Check if there is a message waiting to be transmitted
       if (!(SYS_TRSCN_Q_IS_EMPTY(trscnQ_p)))
       {
           UINT8_t bdHndl = SYS_GET_TRSCN_Q_HEAD(trscnQ_p);
           UINT8_t *buff_p = SYS_GET_TRSCN_Q_BD_BUFF(bdHndl);
           SYNC_RT_wrBlock(dev_p, buff_p,
                          SYS_GET_BD_LEN(bdHndl) - SYS_TRSCN_Q_HNDL_FIELD_LEN);
       }
   }
   else
   {
       SYS_fatal(SYS_FATAL_ERR_249);
   }

   return;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void SYNC_RT_schedAckTx(SYNC_RT_cntxt_s *dev_p, UINT8_t sts)
{
    UINT8_t bdHndl;
    UINT8_t *rxHdr_p = dev_p->rxHdrBuff;

    // ACK is sent with message type 0x0, received sequence number and no
    // payload.

    bdHndl = SYS_allocMem(SYS_TRSCN_Q_HNDL_FIELD_LEN + SYNC_RT_FRAME_HDR_LEN);
    if (SYS_BD_HNDL_IS_VALID(bdHndl))
    {
        UINT8_t *respHdr_p = SYS_GET_TRSCN_Q_BD_BUFF(bdHndl);
        UINT16_t calcCrc;

        UTIL_htons(respHdr_p, SYNC_RT_MSG_TYPE_ACK);

        respHdr_p[SYNC_RT_FRAME_HDR_FLAGS_FIELD_OFF] = sts;

        respHdr_p[SYNC_RT_FRAME_HDR_SEQ_NR_FIELD_OFF] = rxHdr_p[SYNC_RT_FRAME_HDR_SEQ_NR_FIELD_OFF];

        UTIL_htons(respHdr_p + SYNC_RT_FRAME_HDR_PYLD_LEN_FIELD_OFF, 0x0);  // no payload

        calcCrc = PLTFRM_calcCkSum16(respHdr_p, SYNC_RT_FRAME_HDR_LEN - SYNC_RT_FRAME_HDR_CRC_FIELD_LEN*2);
        UTIL_htons(respHdr_p + SYNC_RT_FRAME_HDR_HDR_CRC_FIELD_OFF, calcCrc);

        UTIL_htons(respHdr_p + SYNC_RT_FRAME_HDR_PYLD_CRC_FIELD_OFF, 0x0);

        __SYNC_RT_addToTxPendQ(dev_p, bdHndl);

#ifdef SYNC_RT_STATS_EN
        SYNC_RT_cntxt.ackMemAllocOk ++;
#endif
    }
    else
    {
#ifdef SYNC_RT_STATS_EN
        SYNC_RT_cntxt.ackMemAllocFlr ++;
#endif
    }

    return;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void SYNC_RT_checkRcvdFrameHdr(SYNC_RT_cntxt_s *dev_p)
{
   UINT8_t *hdr_p = dev_p->rxHdrBuff, errFlag = 1;
   UINT8_t sts = SYNC_RT_HDR_ACK_BM;
   UINT16_t calcCkSum, rcvdCkSum;

   calcCkSum = PLTFRM_calcCkSum16(hdr_p,
                                  SYNC_RT_FRAME_HDR_LEN - SYNC_RT_FRAME_HDR_CRC_FIELD_LEN*2);
   rcvdCkSum = UTIL_ntohs(hdr_p + SYNC_RT_FRAME_HDR_HDR_CRC_FIELD_OFF);
   if (calcCkSum == rcvdCkSum)
   {
       UINT16_t msgType = UTIL_ntohs(hdr_p);

#ifdef SYNC_RT_STATS_EN
       dev_p->hdrCrcOkCnt ++;
#endif
       dev_p->rxFramePyldLen = UTIL_ntohs(hdr_p + SYNC_RT_FRAME_HDR_PYLD_LEN_FIELD_OFF);
       if (dev_p->rxFramePyldLen == 0x0)
       {
           GW_rcvdMsgHndlr(msgType, SYS_INV_BD_HDNL);
           dev_p->rxFrameOff = 0x0;
           errFlag = 0x0;
       }
       else
       {
           if (dev_p->rxFramePyldLen < SYNC_RT_FRAME_MAX_PYLD_LEN)
           {
               if (msgType != LPWMN_GW_MSG_TYPE_RELAY_TO_NODE
                   || (!(SYS_BD_HNDL_IS_VALID(GW_cntxt.relayFrameBdHndl))))
               {
                   dev_p->rxFrameBdHndl = SYS_allocMem(dev_p->rxFramePyldLen);
                   if (SYS_BD_HNDL_IS_VALID(dev_p->rxFrameBdHndl))
                   {
                       dev_p->rxFrameBuff_p = SYS_GET_BD_BUFF(dev_p->rxFrameBdHndl);
                       sts |= SYNC_RT_ACK_STS_OK_BM;
                       errFlag = 0x0;
                   }
                   else
                   {
                       sts |= SYNC_RT_ACK_STS_OOM_BM;
                   }
               }
               else
               {
                   sts |= SYNC_RT_ACK_STS_RELAY_IN_PROGRESS;
               }
           }
           else
           {
               sts |= SYNC_RT_ACK_STS_FRAME_TOO_LONG_BM;
           }
       }
   }
   else
   {
       sts |= SYNC_RT_ACK_STS_INV_CRC;
#ifdef SYNC_RT_STATS_EN
       dev_p->hdrCrcBadCnt ++;
#endif
   }

   if ((errFlag) || (dev_p->rxFramePyldLen > 0))
       SYNC_RT_schedAckTx(dev_p, sts);

   if (errFlag)
   {
       dev_p->rxFrameOff = 0x0;
       dev_p->rxFramePyldLen = 0x0;
   }

   return;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void SYNC_RT_checkRcvdFramePyld(SYNC_RT_cntxt_s *dev_p)
{
   UINT8_t *hdr_p = dev_p->rxHdrBuff;
   UINT8_t *pyld_p = dev_p->rxFrameBuff_p;
   UINT16_t calcCkSum, rcvdCkSum;

   if (dev_p->rxFramePyldLen == 0x0
       || (!(SYS_BD_HNDL_IS_VALID(dev_p->rxFrameBdHndl))))
   {
       SYS_fatal(SYS_FATAL_ERR_250);
   }

   calcCkSum = PLTFRM_calcCkSum16(pyld_p, dev_p->rxFramePyldLen);
   rcvdCkSum = UTIL_ntohs(hdr_p + SYNC_RT_FRAME_HDR_PYLD_CRC_FIELD_OFF);

   if (calcCkSum == rcvdCkSum)
   {
       UINT16_t msgType = UTIL_ntohs(hdr_p);
#ifdef SYNC_RT_STATS_EN
       dev_p->pyldCrcOkCnt ++;
#endif
       // Called function frees rxFrameBdHndl
       GW_rcvdMsgHndlr(msgType, dev_p->rxFrameBdHndl);
   }
   else
   {
       // Send NACK
       SYS_freeMem(dev_p->rxFrameBdHndl);
       SYNC_RT_schedAckTx(dev_p, SYNC_RT_ACK_STS_INV_CRC);
#ifdef SYNC_RT_STATS_EN
       dev_p->pyldCrcBadCnt ++;
#endif
   }

   dev_p->rxFrameOff = 0;
   dev_p->rxFramePyldLen = 0;
   dev_p->rxFrameBdHndl = SYS_INV_BD_HDNL;

   return;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void SYNC_RT_rxClkInIntFn(UINT8_t devId)
{
  SYNC_RT_cntxt_s *dev_p;

  /*
   * This function is called from the main loop. It is not called
   * in interrupt context.
   */

  dev_p = SYNC_RT_getDevCntxt(devId);
  if (dev_p == NULL)
  {
      SYS_fatal(SYS_FATAL_ERR_251);
  }
  else
  {
     UINT8_t pinVal;
     PLTFRM_sts_t sts;
     SYNC_RT_sigInfo_s *sigInfo_p = &(dev_p->sigList[SYNC_RT_RX_DATA_IN_SIG_ID]);

     sts = PLTFRM_getGPIOPinInput(sigInfo_p->portNr,
                                  sigInfo_p->pinNr,
                                  &pinVal);
     if (sts != PLTFRM_STS_SUCCESS)
     {
         SYS_fatal(SYS_FATAL_ERR_252);
     }
     else
     {
         if (dev_p->rxFrameOff < SYNC_RT_FRAME_HDR_LEN)
         {
             dev_p->rxHdrBuff[dev_p->rxFrameOff] |= (pinVal << dev_p->rxBitOff);
             dev_p->rxBitOff ++;
             if (dev_p->rxBitOff >= 8)
             {
                 dev_p->rxFrameOff ++;
                 if (dev_p->rxFrameOff >= SYNC_RT_FRAME_HDR_LEN)
                 {
#ifdef SYNC_RT_STATS_EN
                     dev_p->hdrRcvdCnt ++;
#endif
                     SYNC_RT_checkRcvdFrameHdr(dev_p);
                 }
             }
         }
         else
         {
             dev_p->rxFrameBuff_p[dev_p->rxFrameOff - SYNC_RT_FRAME_HDR_LEN] |= (pinVal << dev_p->rxBitOff);
             dev_p->rxBitOff ++;
             if (dev_p->rxBitOff >= 8)
             {
                 dev_p->rxBitOff = 0;
                 dev_p->rxFrameOff ++;
                 if (dev_p->rxFrameOff >= (dev_p->rxFramePyldLen + SYNC_RT_FRAME_HDR_LEN))
                 {
                     // Complete payload received ..
                     SYNC_RT_checkRcvdFramePyld(dev_p);
                 }
             }
         }
     }
  }

  return;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void SYNC_RT_txAckInIntFn(UINT8_t devId)
{
  SYNC_RT_cntxt_s *dev_p;

  /*
   * This function is called from the main loop. It is not called
   * in interrupt context.
   */

  dev_p = SYNC_RT_getDevCntxt(devId);
  if (dev_p == NULL)
  {
      SYS_fatal(SYS_FATAL_ERR_253);
  }
  else
  {
      // Ack received ...
      dev_p->txBitOff ++;
      if (dev_p->txBitOff >= 8)
      {
          dev_p->txBitOff = 0;
          dev_p->txBuffOff ++;
          if (dev_p->txBuffOff >= dev_p->txBuffLen)
          {
              SYNC_RT_blockTxDoneHndlr(dev_p);
          }
          else
          {
              SYNC_RT_wrBit(dev_p);
          }
      }
      else
      {
          SYNC_RT_wrBit(dev_p);
      }
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
void __SYNC_RT_addToTxPendQ(SYNC_RT_cntxt_s *dev_p, UINT8_t bdHndl)
{
   SYS_trscnQ_s *trscnQ_p;
   UINT8_t qEmpty;

   trscnQ_p = &(dev_p->pendTxtrscnQ);
   qEmpty = SYS_TRSCN_Q_IS_EMPTY(trscnQ_p) ? 0x1 : 0x0;

   // The message being currently transmitted is still at the head of the pendTxtrscnQ.
   // It will be removed only after the message has been completely transmitted.

   SYS_addToTrscnsQ(trscnQ_p, bdHndl);
   if (qEmpty)
   {
       UINT8_t bdHndl = SYS_GET_TRSCN_Q_HEAD(trscnQ_p);
       UINT8_t *buff_p = SYS_GET_TRSCN_Q_BD_BUFF(bdHndl);
       SYNC_RT_wrBlock(dev_p, buff_p,
                       SYS_GET_BD_LEN(bdHndl) - SYS_TRSCN_Q_HNDL_FIELD_LEN);
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
void SYNC_RT_addToTxPendQ(UINT8_t devId, UINT8_t bdHndl)
{
   SYNC_RT_cntxt_s *dev_p = SYNC_RT_getDevCntxt(devId);

   if (dev_p == NULL)
   {
       SYS_fatal(SYS_FATAL_ERR_254);
       return;
   }

   __SYNC_RT_addToTxPendQ(dev_p, bdHndl);

   return;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void SYNC_RT_buildMsgHdr(UINT8_t devId,
                         UINT16_t msgType,
                         UINT8_t *buff_p,
                         UINT16_t pyldLen)
{
   UINT16_t calcCrc;
   SYNC_RT_cntxt_s *dev_p = SYNC_RT_getDevCntxt(devId);

   if (dev_p == NULL)
   {
       SYS_fatal(SYS_FATAL_ERR_255);
       return;
   }

   UTIL_htons(buff_p, msgType);

   buff_p[SYNC_RT_FRAME_HDR_FLAGS_FIELD_OFF] = 0x0;

   buff_p[SYNC_RT_FRAME_HDR_SEQ_NR_FIELD_OFF] = dev_p->txSeqNr;
   dev_p->txSeqNr ++;

   UTIL_htons(buff_p + SYNC_RT_FRAME_HDR_PYLD_LEN_FIELD_OFF, pyldLen);

   calcCrc = PLTFRM_calcCkSum16(buff_p, SYNC_RT_FRAME_HDR_LEN - SYNC_RT_FRAME_HDR_CRC_FIELD_LEN*2);
   UTIL_htons(buff_p + SYNC_RT_FRAME_HDR_HDR_CRC_FIELD_OFF, calcCrc);

   if (pyldLen > 0)
       calcCrc = PLTFRM_calcCkSum16(buff_p + SYNC_RT_FRAME_HDR_LEN, pyldLen);
   else
       calcCrc = 0x0;

   UTIL_htons(buff_p + SYNC_RT_FRAME_HDR_PYLD_CRC_FIELD_OFF, calcCrc);

   return;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void SYNC_RT_start(UINT8_t devId)
{
   SYNC_RT_cntxt_s *dev_p = SYNC_RT_getDevCntxt(devId);
   SYNC_RT_sigInfo_s *sigInfo_p;

   if (dev_p == NULL)
   {
       SYS_fatal(SYS_FATAL_ERR_256);
   }

   sigInfo_p = &(dev_p->sigList[SYNC_RT_RX_CLK_IN_SIG_ID]);

   // Enable interrupt on the rx clock in pin ...
   PLTFRM_gpioPinIntCtrlOpn(sigInfo_p->portNr, sigInfo_p->pinNr,
                            PLTFRM_ENABLE_GPIO_PIN_INT, 0x1);

   return;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t SYNC_RT_devInit(UINT8_t devId)
{
  PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
  SYNC_RT_cntxt_s *dev_p = SYNC_RT_getDevCntxt(devId);
  UINT8_t sigId;

  if (dev_p == NULL)
      return PLTFRM_STS_INV_DEV_ID;

  for (sigId = SYNC_RT_RX_DATA_IN_SIG_ID;
       sigId <= SYNC_RT_TX_ACK_IN_SIG_ID;
       sigId ++)
  {
	  SYNC_RT_sigInfo_s *sigInfo_p;
      PLTFRM_gpioDirn_t dirn = PLTFRM_GPIO_PIN_DIRECTION_OUTPUT;
      PLTFRM_gpioIntEdge_t intEdge = PLTFRM_GPIO_INT_EDGE_NA;
      PLTRM_gpioOutVal_t dfltOp = PLTFRM_GPIO_OUTPUT_NA;
      PLTFRM_intCbFunc_t intCbFn_p = NULL;

	  if (sigId == SYNC_RT_RX_DATA_IN_SIG_ID
          || sigId == SYNC_RT_RX_CLK_IN_SIG_ID
          || sigId == SYNC_RT_TX_ACK_IN_SIG_ID)
      {
		  dirn = PLTFRM_GPIO_PIN_DIRECTION_INPUT;
		  if (sigId != SYNC_RT_RX_DATA_IN_SIG_ID)
		  {
		      intEdge = PLTFRM_GPIO_INT_EDGE_NEGATIVE;
		      if (sigId == SYNC_RT_RX_CLK_IN_SIG_ID)
		          intCbFn_p = SYNC_RT_txAckInIntFn;
		      else
		          intCbFn_p = SYNC_RT_rxClkInIntFn;
		  }
      }
      else
      {
          dfltOp = PLTFRM_GPIO_OUTPUT_HIGH;
      }

	  sigInfo_p = &(dev_p->sigList[sigId]);
	  sts = PLTFRM_allocGPIOPin(devId,
	                            sigInfo_p->portNr,
	                            sigInfo_p->pinNr,
	                            dirn, dfltOp, intEdge,
	                            intCbFn_p);
	  if (sts != PLTFRM_STS_SUCCESS)
	      SYS_fatal(SYS_FATAL_ERR_257);
  }

  dev_p->rxFrameOff = 0;
  dev_p->rxBitOff = 0;
  dev_p->rxFramePyldLen = 0;
  dev_p->rxFrameBdHndl = SYS_INV_BD_HDNL;
  dev_p->rxFrameBuff_p = (void *)0;
  dev_p->txBuffLen = 0;
  dev_p->txBuffOff = 0;
  dev_p->txBitOff = 0;
  dev_p->txBuff_p = (void *)0;
  dev_p->txSeqNr = 0x80;

  SYS_initTrscnsQ(&(dev_p->pendTxtrscnQ));

  return sts;
}

#endif
