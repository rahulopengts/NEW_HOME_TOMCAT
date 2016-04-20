/*
 * File Name: app.c
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

#ifdef LPWMN_COORD

#include <stdlib.h>
#include <string.h>
#include <typedefs.h>
#include <pltfrm.h>
#include <adp.h>
#include <system.h>
#include <util.h>
#include <app.h>
#include <aodvl.h>
#include <node_mgr.h>
#include <pltfrm_log.h>

#ifndef LPWMN_COORD
// #define APP_STATS_ENA
#endif

#ifndef LPWMN_COORD
  
#ifdef APP_STATS_ENA
UINT8_t APP_cnfrmOkCnt = 0, APP_cnfrmFlrCnt = 0, APP_dataIndCnt = 0;
#endif


#ifdef CHAN_HOP_TEST
UINT8_t chanNr = PHY_2450_MHZ_BAND_PAGE_0_CHAN_11;
#endif


// extern UINT16_t ADP_dataRcvdCnt;


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */  
void APP_timerExpHndlr(void)
{   
   SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_APP_DATA_REQUEST;
}


UINT16_t APP_dataPktRxCnt = 0, APP_dataPktTxCnt = 0;
UINT16_t APP_dataReqSeqNr = 0;

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */ 
void APP_reportSensorVal(void)
{   
   ADP_cntxt.txParams.bdHndl = SYS_allocMem(6);
   if (SYS_BD_HNDL_IS_VALID(ADP_cntxt.txParams.bdHndl))
   {
        UINT8_t *buff_p = SYS_GET_BD_BUFF(ADP_cntxt.txParams.bdHndl);
#if 0
#if 0
        SINT16_t temp16;
        // UINT16_t vcc = pltfrm_vccSense();
      
        pltfrm_tempSensorInit();
        temp16 = pltfrm_readTempSensor();
 
        // buff_p[0] = (vcc >> 8) & 0xff;
        // buff_p[1] = vcc & 0xff;
      
        // buff_p[] = 0x0;
        
        buff_p[0] = (temp16 >> 8) & 0xff;
        buff_p[1] = temp16 & 0xff;   
#else        
        {
           uint8_t idx;
           buff_p[0] = (ADP_dataRcvdCnt >> 8) & 0xff;
           buff_p[1] = (UINT8_t)(ADP_dataRcvdCnt);
           buff_p[2] = 0x00;
           buff_p[3] = 0xde;
           buff_p[4] = 0xad;
           for (idx=5; idx<6; idx++)
                buff_p[idx] = 0x0;
        }
#endif
#endif
        APP_dataPktTxCnt ++;
        UTIL_htons(buff_p, APP_dataReqSeqNr);
        UTIL_htons(buff_p + 2, APP_dataPktTxCnt);
        UTIL_htons(buff_p + 4, APP_dataPktRxCnt);
   }
   else
   {
       // <TODO> App will wait for memory to be freed later !!
       SYS_fatal(SYS_FATAL_ERR_1);
   }
  
#ifdef IPv4_SUPPORT_ENA
   ADP_cntxt.txParams.destIp = 0xc0a80001;
   ADP_cntxt.txParams.destUdpPort = 0xf0b7;
#endif

   if (ADP_appPyldTxReq() != ADP_STS_SUCCESS)
       PLTFRM_startTimerA1(1, 0, APP_timerExpHndlr);
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */  
void APP_init(void)
{  
   // SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_APP_DATA_REQUEST;
}


#if 0
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void APP_nodeJoinDoneInd(void)
{
   // APP_timerExpHndlr();
}
#endif


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void APP_evtHndlr(UINT16_t globalEvtId, void *params_p)
{          
   switch (globalEvtId)
   {  
      case SYS_GLOBAL_EVT_ADP_DATA_INDICATION:
           {
              ADP_dataIndParams_s *indParams_p = (ADP_dataIndParams_s *)params_p;
              PLTFRM_TOGGLE_LED_1();
              UINT8_t *buff_p = SYS_GET_BD_BUFF(indParams_p->bdHndl);
              if (SYS_GET_BD_LEN(indParams_p->bdHndl) == 6)
              {
                  APP_dataReqSeqNr = UTIL_ntohs(buff_p);
                  APP_dataPktRxCnt ++;
              }
              SYS_freeMem(indParams_p->bdHndl);
              SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_APP_DATA_REQUEST;
           }
           break;
          
      case SYS_GLOBAL_EVT_ADP_DATA_CONFIRM:
           {
#if 0
              ADP_dataCnfrmParams_s *dcParams_p = (ADP_dataCnfrmParams_s *)params_p;
              if (dcParams_p->sts == ADP_STS_SUCCESS)
              {
#ifdef APP_STATS_ENA               
                  APP_cnfrmOkCnt ++;
#endif                  
              }
              else
              {
#ifdef APP_STATS_ENA                
                  APP_cnfrmFlrCnt ++;
#endif                  
              }
#endif
              
#ifdef CHAN_HOP_TEST              
              cc2520_setPhyChan(chanNr);
              chanNr ++;
              if (chanNr == PHY_2450_MHZ_BAND_PAGE_0_CHAN_26)
                  chanNr = PHY_2450_MHZ_BAND_PAGE_0_CHAN_11;
#endif              
           }
           break;
          
      default:
          break;
   }
}
#endif

// #define NC_STATS_ENA

#ifdef LPWMN_COORD

#if 0
UINT16_t NC_nodeAddr[4] = {0x0, 0x0, 0x0, 0x0};
UINT16_t NC_nodeTxStats[4] = {0x0, 0x0, 0x0, 0x0};
UINT16_t NC_nodeRxStats[4] = {0x0, 0x0, 0x0, 0x0};
UINT16_t NC_sentSeqNr = 0;
#endif

#ifdef NC_STATS_ENA
UINT16_t NC_dataCnfrmFlrCnt = 0;
UINT16_t NC_dataCnfrmOkCnt = 0;
UINT16_t NC_dataIndCnt = 0;
UINT16_t NC_appPyldTxReqOkCnt = 0;
UINT16_t NC_appPyldTxReqFlrCnt = 0;
UINT16_t NC_nodeRespTmoCnt = 0;
UINT16_t NC_indBeforeCnfrmCnt = 0;
#endif

UINT8_t lastNodePathDiscIdx = NM_MAX_SUPPORTED_NODE_CNT;

#ifndef LPWMN_COORD
void NC_appLoop(UINT8_t lastIdx);
#endif


#if 0
UINT8_t NC_appPathDiscCnfrmOkCnt = 0,
        NC_appPathDiscCnfrmFlrCnt = 0;
#endif


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void NC_appTimerExpHndlr(void)
{   
#ifndef LPWMN_COORD
   NC_appLoop(lastNodePathDiscIdx);
#endif
}

#if 0
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void NC_appPathDiscCnfrmCbFunc(UINT8_t sts, UINT8_t bdHndl)
{
   // Do not free the bdHndl !!
  
   if (sts == ADP_STS_SUCCESS)
       NC_appPathDiscCnfrmOkCnt ++;
   else
       NC_appPathDiscCnfrmFlrCnt ++;  
   
   if (bdHndl < SYS_MAX_BUFF_DESC_CNT)
       SYS_freeMem(bdHndl);
   
   PLTFRM_startTimerA1(2, 0, NC_appTimerExpHndlr);
}
#endif


#if 0
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void NC_waitNodeRespTmoCbFunc(void)
{
#ifdef NC_STATS_ENA
   NC_nodeRespTmoCnt ++;
#endif
#ifndef LPWMN_COORD
   NC_appLoop(lastNodePathDiscIdx);
#endif
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t NC_sendAppMsgToNode(UINT8_t nodeIdx)
{
    UINT8_t rc;

    // APP should not send another data request until it gets a confirm
    // for the previous request.

    if (ADP_cntxt.txParams.bdHndl != SYS_MAX_BUFF_DESC_CNT
        || ADP_cntxt.txParams.destShortAddr != MAC_BROADCAST_SHORT_ADDR)
    {
        // Looks like indication came before confirm !!
#ifdef NC_STATS_ENA
        NC_indBeforeCnfrmCnt ++;
#endif
        return 0;
    }

#ifdef IPv4_SUPPORT_ENA
    ADP_cntxt.txParams.destUdpPort = 0xf0b7;
    ADP_cntxt.txParams.bdHndl = SYS_allocMem(6);
#else
    ADP_cntxt.txParams.bdHndl = SYS_allocMem(MAC_SHORT_ADDR_LEN
                                             + ADP_NON_IP_DEST_PORT_FIELD_LEN
                                             + 6);
#endif
    if (SYS_BD_HNDL_IS_VALID(ADP_cntxt.txParams.bdHndl))
    {
        UINT8_t *buff_p = SYS_GET_BD_BUFF(ADP_cntxt.txParams.bdHndl);

        if (NC_nodeAddr[nodeIdx] != NM_cntxt.NM_nodeList[nodeIdx].shortAddr)
        {
            NC_nodeTxStats[nodeIdx] = 0;
            NC_nodeRxStats[nodeIdx] = 0;
            NC_nodeAddr[nodeIdx] = NM_cntxt.NM_nodeList[nodeIdx].shortAddr;
        }

        NC_nodeTxStats[nodeIdx] ++;

#ifdef IPv4_SUPPORT_ENA
        ADP_cntxt.txParams.destIp = 0xc0a80000 | NM_cntxt.NM_nodeList[nodeIdx].shortAddr;
#else
        buff_p = UTIL_htons(buff_p, NM_cntxt.NM_nodeList[nodeIdx].shortAddr);
        *buff_p = 0x7;   // src port 0, dest port 7
        buff_p ++;
#endif

        NC_sentSeqNr ++;

        UTIL_htons(buff_p, NC_sentSeqNr);
        UTIL_htons(buff_p + 2, NC_nodeTxStats[nodeIdx]);
        UTIL_htons(buff_p + 4, NC_nodeRxStats[nodeIdx]);

        if (ADP_appPyldTxReq() != ADP_STS_SUCCESS)
        {
            NC_appPyldTxReqFlrCnt ++;
            rc = 0;
        }
        else
        {
            NC_appPyldTxReqOkCnt ++;
            rc = 1;
        }
    }
    else
    {
        SYS_fatal(SYS_FATAL_ERR_1);
    }

    return rc;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void NC_dataReqProc(void)
{
    if (NC_sendAppMsgToNode(lastNodePathDiscIdx) == 0x0)
        PLTFRM_startTimerA1(5, 0, NC_appTimerExpHndlr);
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void NC_appLoop(UINT8_t lastIdx)
{
    UINT8_t idx, done = 0;
 
    if (NM_getJoinedNodeCnt( ) < 1)
    {
        PLTFRM_startTimerA1(2, 0, NC_appTimerExpHndlr);
        return;
    }
    
    lastIdx ++;
    
    if (lastIdx >= NM_MAX_SUPPORTED_NODE_CNT)
        lastIdx = 0;
    
    do
    {
       for (idx=lastIdx; idx<NM_MAX_SUPPORTED_NODE_CNT; idx++)
       {
            // AODVL_pathDiscParams_s params;
      
            if (NM_cntxt.NM_nodeList[idx].shortAddr == MAC_BROADCAST_SHORT_ADDR)
                continue;

            lastNodePathDiscIdx = idx;

#if 0
            params.cbFunc_p = NC_appPathDiscCnfrmCbFunc;
            params.destAddr = NM_cntxt.NM_nodeList[idx].shortAddr;
         
            if (AODVL_pathDiscRequest(&params) == 0x0)
            {
                PLTFRM_startTimerA1(5, 0, NC_appTimerExpHndlr);
            }
#else
            SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_APP_DATA_REQUEST;
#endif
            
            done = 1;
       
            break;
       }
       
       if (done == 1
           || lastIdx == 0)
           break;
       
       lastIdx = 0;
       
    }  while (1);
    
    if (done == 0)
        PLTFRM_startTimerA1(3, 0, NC_appTimerExpHndlr);
    
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
void APP_evtHndlr(UINT16_t globalEvtId, void *params_p)
{
   switch (globalEvtId)
   {
      case SYS_GLOBAL_EVT_ADP_DATA_INDICATION:
           {
              ADP_dataIndParams_s *indParams_p = (ADP_dataIndParams_s *)params_p;
              UINT8_t *buff_p = SYS_GET_BD_BUFF(indParams_p->bdHndl);
#ifdef NC_STATS_ENA
              NC_dataIndCnt ++;
#endif
              if (SYS_GET_BD_LEN(indParams_p->bdHndl) == 6)
              {
                  UINT16_t seqNr = UTIL_ntohs(buff_p);
                  if (seqNr == NC_sentSeqNr)
                  {
                      NC_nodeRxStats[lastNodePathDiscIdx] ++;
                      PLTFRM_stopTimerA1();
                      PLTFRM_startTimerA1(0, 500, NC_appTimerExpHndlr);
                  }
              }

              PLTFRM_TOGGLE_LED_1();
              SYS_freeMem(indParams_p->bdHndl);
           }
           break;

      case SYS_GLOBAL_EVT_ADP_DATA_CONFIRM:
           {
              ADP_dataCnfrmParams_s *dcParams_p = (ADP_dataCnfrmParams_s *)params_p;
              if (dcParams_p->sts == ADP_STS_SUCCESS)
              {
 #ifdef NC_STATS_ENA
                  NC_dataCnfrmOkCnt ++;
 #endif
              }
              else
              {
 #ifdef NC_STATS_ENA
                  NC_dataCnfrmFlrCnt ++;
 #endif
              }

              PLTFRM_startTimerA1(10, 0, NC_waitNodeRespTmoCbFunc);
           }
           break;

      default:
           break;
   }
}
#endif


UINT16_t pktSeqNr = 0x0;

SINT32_t luxVal = 0x0;

UINT16_t nodeVcc = 0x0;
SINT16_t nodeTemp = 0x0;
UINT16_t extTempH, extTempL;
UINT16_t alsVal16;
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void NC_dataReqProc(void)
{
   SYS_fatal(SYS_FATAL_ERR_1);
}


void APP_evtHndlr(UINT16_t globalEvtId, void *params_p)
{
#if 0
   switch (globalEvtId)
   {
      case SYS_GLOBAL_EVT_ADP_DATA_INDICATION:
           {
              ADP_dataIndParams_s *indParams_p = (ADP_dataIndParams_s *)params_p;
              UINT8_t *buff_p = SYS_GET_BD_BUFF(indParams_p->bdHndl);

              if (NC_dataIndCnt == 0x0)
              {
            	  PLTFRM_ON_LED_1();
            	  PLTFRM_OFF_LED_2();
              }

#ifdef NC_STATS_ENA
              NC_dataIndCnt ++;
#endif

              PLTFRM_TOGGLE_LED_1();
              PLTFRM_TOGGLE_LED_2();

              if (SYS_GET_BD_LEN(indParams_p->bdHndl) == 9) // 13)  // 6)
              {
                  UINT32_t alsVal32;
                  pktSeqNr = UTIL_ntohs(buff_p);
                  buff_p += 3;  // 1 byte sensor flag field
                  nodeVcc = UTIL_ntohs(buff_p);
                  buff_p += 2;
                  alsVal32 = (UINT32_t)UTIL_ntohl(buff_p);
                  alsVal16 = (UINT16_t)alsVal32;
                  PLTFRM_LOG("sn<%x> vcc<%d> alsVal<0x%x>\n\r",
                             pktSeqNr, nodeVcc, alsVal16);
                  goto _free;
              }

              if (SYS_GET_BD_LEN(indParams_p->bdHndl) == 13) // 13)  // 6)
              {
                  UINT32_t llSnsrVout, llSnsrVcc;
                  UINT16_t llSnsrVoutL, llSnsrVoutH, llSnsrVccL, llSnsrVccH;
                  pktSeqNr = UTIL_ntohs(buff_p);
                  buff_p += 3;  // 1 byte sensor flag field
                  nodeVcc = UTIL_ntohs(buff_p);
                  buff_p += 2;
                  llSnsrVout = UTIL_ntohl(buff_p);
                  buff_p += 4;
                  llSnsrVcc = UTIL_ntohl(buff_p);

                  llSnsrVoutL = llSnsrVout & 0xffff;
                  llSnsrVoutH = (llSnsrVout >> 16) & 0xffff;
                  llSnsrVccL = llSnsrVcc & 0xffff;
                  llSnsrVccH = (llSnsrVcc >> 16) & 0xffff;
                  PLTFRM_LOG("llSnsrVoH<0x%x> llSnsrVoL<0x%x> llSnsrVsH<0x%x> llSnsrVsL<0x%x>\n\r",
                		  llSnsrVoutH, llSnsrVoutL, llSnsrVccH, llSnsrVccL);
                  (void)llSnsrVccL;
                  (void)llSnsrVccH;
                  (void)llSnsrVoutL;
                  (void)llSnsrVoutH;
                  goto _free;
              }

              if (SYS_GET_BD_LEN(indParams_p->bdHndl) == 15) // 13)  // 6)
              {
                  pktSeqNr = UTIL_ntohs(buff_p);
                  buff_p += 3;  // 1 byte sensor flag field
                  nodeVcc = UTIL_ntohs(buff_p);
                  buff_p += 2;  // 2 bytes filler (0x0000)
                  nodeTemp = (SINT16_t)UTIL_ntohs(buff_p);
                  buff_p += 2;
                  extTempH = UTIL_ntohs(buff_p);
                  buff_p += 2;
                  extTempL = UTIL_ntohs(buff_p);
                  buff_p += 2;
                  PLTFRM_LOG("sn<%x> temp<%d> tmp102<0x%x%x>\n\r",
                             pktSeqNr, nodeTemp, extTempH, extTempL);

#if 0
                  luxVal = UTIL_ntohl(buff_p + 2);

                  if (luxVal > 1000)


                  if (nodeTemp > 2800)    // 10  degC
                  {
                      PLTFRM_ON_LED_1();
                      PLTFRM_ON_LED_2();
                  }
                  else
                  {
                      PLTFRM_OFF_LED_1();
                      PLTFRM_OFF_LED_2();
                  }
#endif
              }
_free:
              SYS_freeMem(indParams_p->bdHndl);
           }
           break;

      case SYS_GLOBAL_EVT_ADP_DATA_CONFIRM:
           {
              ADP_dataCnfrmParams_s *dcParams_p = (ADP_dataCnfrmParams_s *)params_p;
              if (dcParams_p->sts == ADP_STS_SUCCESS)
              {
 #ifdef NC_STATS_ENA
                  NC_dataCnfrmOkCnt ++;
 #endif
              }
              else
              {
 #ifdef NC_STATS_ENA
                  NC_dataCnfrmFlrCnt ++;
 #endif
              }
           }
           break;

      default:
           break;
   }
#endif
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */  
void APP_procGPIOEvt()
{

}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void APP_init(void)
{  
   // PLTFRM_startTimerA1(2, 0, NC_appTimerExpHndlr);
   ADP_cntxt.txParams.bdHndl = SYS_MAX_BUFF_DESC_CNT;
   ADP_cntxt.txParams.destShortAddr = MAC_BROADCAST_SHORT_ADDR;
   return;
}

    
#endif

#endif
