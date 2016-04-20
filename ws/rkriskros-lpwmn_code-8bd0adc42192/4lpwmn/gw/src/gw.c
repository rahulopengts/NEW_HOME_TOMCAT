/*
 * File Name: gw.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Oct/21/2013
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


#ifdef GW_ENA
#if defined(LPWMN_COORD) || defined(DEV_TYPE_SNIFFER)

#include <string.h>
#include <stdio.h>
#include <system.h>
#include <util.h>
#include <gw.h>
#include <node_mgr.h>
#include <ext_if.h>
#include <radio.h>
#include <phy_defs.h>
#include <phy_pib.h>
#include <mac_defs.h>
#include <mac.h>
#include <mac_pib.h>
#include <adp.h>
#include <aodvl.h>
#include <dis.h>
#ifdef PLTFRM_CC2D33S_SENSOR_ENA
#include <cc2d33s.h>
#endif
#ifdef CUSTOMER_EPOCH_SPECIFIC_BUILD
#include <ad7797.h>
#endif
GW_cntxt_s GW_cntxt;

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void GW_buildSendResp(UINT16_t msgType,
                      const UINT8_t *respVal_p,
                      UINT8_t respLen)
{
   UINT8_t bdHndl = SYS_allocMem(SYS_TRSCN_Q_HNDL_FIELD_LEN + EXT_IF_FRAME_HDR_LEN + respLen);
   if (SYS_BD_HNDL_IS_VALID(bdHndl))
   {
       UINT8_t *buff_p = SYS_GET_TRSCN_Q_BD_BUFF(bdHndl);

       switch (respLen)
       {
          case 1:
              *(buff_p + EXT_IF_FRAME_HDR_LEN) = *(respVal_p);
              break;

          case 2:
              UTIL_htons(buff_p + EXT_IF_FRAME_HDR_LEN, *((UINT16_t *)respVal_p));
              break;

          case 4:
              UTIL_htonl(buff_p + EXT_IF_FRAME_HDR_LEN, *((UINT32_t *)respVal_p));
              break;

          default:
              memcpy(buff_p + EXT_IF_FRAME_HDR_LEN, respVal_p, respLen);
              break;
       }

#if defined(COORD_EXT_IF_UART)
       UART_buildMsgHdr(msgType, buff_p, respLen);
       UART_addToTxPendQ(bdHndl);
#else
       SYNC_RT_buildMsgHdr(PLTFRM_SYNC_RT_1_DEV_ID, msgType, buff_p, respLen);
       SYNC_RT_addToTxPendQ(PLTFRM_SYNC_RT_1_DEV_ID, bdHndl);
#endif
   }
   else
   {
       // Drop the request silently !!
   }

   return;
}

#ifndef DEV_TYPE_SNIFFER

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void GW_relayMsgToRemoteNode(void)
{
#ifdef CUSTOMER_ANDY_SPECIFIC_COORD_BUILD
   MAC_sts_t macSts;

   macSts = MAC_txRawPkt(GW_cntxt.relayFrameBdHndl);
   if (macSts != MAC_STS_SUCCESS)
   {
       UINT16_t respVal = LPWMN_MAC_MODULE;
       respVal = (respVal << 8) | (UINT8_t)macSts;
       GW_buildSendResp(LPWMN_GW_MSG_TYPE_RELAY_TO_NODE,
                        (UINT8_t *)&respVal, 2);
       GW_cntxt.relayFrameBdHndl = SYS_MAX_BUFF_DESC_CNT;
   }
#else
   ADP_sts_t adpSts;

   // Send the packet received over the UART to the ADP layer
   ADP_cntxt.txParams.bdHndl = GW_cntxt.relayFrameBdHndl;
   ADP_cntxt.txParams.localOrig = 0x0;

   if ((GW_cntxt.relayTarget & LPWMN_GW_MESH_LEVEL_BROADCAST_MSK) == LPWMN_GW_MESH_LEVEL_BROADCAST_MSK)
   {
	   ADP_cntxt.txParams.meshBroadcastMaxHopCnt = GW_cntxt.relayTarget & LPWMN_GW_MESH_LEVEL_BROADCAST_HOP_CNT_MSK;
	   if (ADP_cntxt.txParams.meshBroadcastMaxHopCnt == 0)
       {
		   ADP_cntxt.txParams.meshBroadcastMaxHopCnt ++;
       }
       else
       {
	       if (ADP_cntxt.txParams.meshBroadcastMaxHopCnt > ADP_MAX_HOP_COUNT)
		       ADP_cntxt.txParams.meshBroadcastMaxHopCnt = ADP_MAX_HOP_COUNT;
       }

 	   ADP_cntxt.txParams.destShortAddr = MAC_BROADCAST_SHORT_ADDR;
   }
   else
       ADP_cntxt.txParams.destShortAddr = GW_cntxt.relayTarget;

   GW_cntxt.relayTarget = MAC_BROADCAST_SHORT_ADDR;

   adpSts = ADP_appPyldTxReq();
   if (adpSts != ADP_STS_SUCCESS)
   {
       UINT16_t respVal = LPWMN_ADP_MODULE;
       respVal = (respVal << 8) | (UINT8_t)adpSts;
       GW_buildSendResp(LPWMN_GW_MSG_TYPE_RELAY_TO_NODE,
                        (UINT8_t *)&respVal, 2);
       GW_cntxt.relayFrameBdHndl = SYS_MAX_BUFF_DESC_CNT;
   }
#endif

   return;
}
#endif

#ifdef CUSTOMER_EPOCH_SPECIFIC_BUILD
SINT8_t __outputBuff[16];
#endif

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void GW_relayMsgFromRemoteNode(UINT16_t srcShortAddr,
                               MCPS_dataIndParams_s *indParams_p)
{
   UINT8_t bdHndl;

#ifdef CUSTOMER_EPOCH_SPECIFIC_BUILD
   {
	   UINT16_t pyldLen = SYS_GET_BD_LEN(indParams_p->bdHndl);
	   UINT16_t expPyldLen = (DIS_MSG_TYPE_SZ
                              + (UTIL_TLV_HDR_SZ * 5)
                              + DIS_SENSOR_ID_TLV_SZ + PLTFRM_ON_CHIP_VCC_SENSOR_OUTPUT_LEN
                              + DIS_DATA_SCALE_FACTOR_TLV_SZ
                              + DIS_SENSOR_ID_TLV_SZ + AD7797_SENSOR_OUTPUT_VALUE_SZ);

       if (pyldLen >= DIS_MSG_TYPE_SZ)
       {
           UINT8_t *pyldBuff_p = SYS_GET_BD_BUFF(indParams_p->bdHndl);
	       if (*pyldBuff_p == DIS_MSG_TYPE_SENSOR_OUTPUT
	           && pyldLen >= expPyldLen)
           {
               SINT32_t adcVal;
               UINT32_t tempU32;
               UINT8_t len;

               pyldBuff_p += (expPyldLen - AD7797_SENSOR_OUTPUT_VALUE_SZ);
               tempU32 = UTIL_ntohl(pyldBuff_p);
               adcVal = (SINT32_t)tempU32;
               len = sprintf((char *)__outputBuff, "\n\r%ld\n", adcVal);
               bdHndl = SYS_allocMem(SYS_TRSCN_Q_HNDL_FIELD_LEN + len);
               if (SYS_BD_HNDL_IS_VALID(bdHndl))
               {
                   UINT8_t *msgBuff_p = SYS_GET_TRSCN_Q_BD_BUFF(bdHndl);
                   strcpy((char *)msgBuff_p, (char *)__outputBuff);
                   UART_addToTxPendQ(bdHndl);
               }
	       }
	   }

       SYS_freeMem(indParams_p->bdHndl);
       indParams_p->bdHndl = SYS_INV_BD_HDNL;
       return;
   }
#endif

#ifndef CUSTOMER_ANDY_SPECIFIC_COORD_BUILD
   NM_nodeListEntry_s *nodeInfo_p = NULL;
#endif
   UINT16_t pyldLen = SYS_GET_BD_LEN(indParams_p->bdHndl)
                      + MAC_SHORT_ADDR_LEN
#if !defined(CUSTOMER_ANDY_SPECIFIC_COORD_BUILD) && !defined(DEV_TYPE_SNIFFER)
                      + MAC_EXT_ADDR_LEN
#endif
                      + RADIO_RCVD_PKT_RSSI_INFO_LEN     // signed byte
#ifdef RADIO_CC2520
                      + RADIO_RCVD_PKT_CORRELATION_INFO_LEN;   // unsigned byte
#elif defined(RADIO_CC1200) || defined(RADIO_CC1101) || defined(RADIO_CC2500)
                      + RADIO_RCVD_PKT_LQI_INFO_LEN;    // unsigned byte
#else
#error Radio not specified !!
#endif

#if !defined(CUSTOMER_ANDY_SPECIFIC_COORD_BUILD) && !defined(DEV_TYPE_SNIFFER)
   if ((nodeInfo_p = NM_lookUpShortAddr(srcShortAddr)) == NULL)
   {
	   // Feb/4/16
	   // Mesh packet originated from a node which is not registered !!
	   GW_sendInvPktSrcEvt(srcShortAddr);
       goto _end;
   }
#endif

   nodeInfo_p->timeSinceLastRx = 0;

   bdHndl = SYS_allocMem(SYS_TRSCN_Q_HNDL_FIELD_LEN + EXT_IF_FRAME_HDR_LEN + pyldLen);
   if (SYS_BD_HNDL_IS_VALID(bdHndl))
   {
       UINT8_t *msgBuff_p = SYS_GET_TRSCN_Q_BD_BUFF(bdHndl);
       UINT8_t *pyldBuff_p = SYS_GET_BD_BUFF(indParams_p->bdHndl);
       UINT8_t *buff_p = msgBuff_p;

       buff_p = UTIL_htons(msgBuff_p + EXT_IF_FRAME_HDR_LEN, srcShortAddr);

       // rkris@wisense.in / may/7/15 - send ext address in each relay_from_node message
#if !defined(CUSTOMER_ANDY_SPECIFIC_COORD_BUILD) && !defined(DEV_TYPE_SNIFFER)
       memcpy(buff_p, nodeInfo_p->extAddr, MAC_EXT_ADDR_LEN);
       buff_p += MAC_EXT_ADDR_LEN;
#endif

       *(buff_p ++) = indParams_p->rssi;
#ifdef RADIO_CC2520
       *(buff_p ++) = indParams_p->correlation;
#elif defined(RADIO_CC1200) || defined(RADIO_CC1101)  || defined(RADIO_CC2500)
       *(buff_p ++) = indParams_p->lqi;
#else
#error Radio not specified !!
#endif
       // <TODO> avoid memcpy()
       memcpy(buff_p, pyldBuff_p, SYS_GET_BD_LEN(indParams_p->bdHndl));

#if defined(COORD_EXT_IF_UART)
       UART_buildMsgHdr(srcShortAddr != LPWMN_COORD_SHORT_ADDR ? LPWMN_GW_MSG_TYPE_RELAY_FROM_NODE : LPWMN_GW_MSG_TYPE_COORD_SENSOR_OUTPUT,
                        msgBuff_p, pyldLen);
       UART_addToTxPendQ(bdHndl);
#else
       SYNC_RT_buildMsgHdr(PLTFRM_SYNC_RT_1_DEV_ID,
    		               srcShortAddr != LPWMN_COORD_SHORT_ADDR ? LPWMN_GW_MSG_TYPE_RELAY_FROM_NODE : LPWMN_GW_MSG_TYPE_COORD_SENSOR_OUTPUT,
    		               msgBuff_p, pyldLen);
       SYNC_RT_addToTxPendQ(PLTFRM_SYNC_RT_1_DEV_ID, bdHndl);
#endif
       //  rkris@wisense.in / feb/5/2016
       GW_cntxt.lpwmnMsgsRcvdCnt ++;
   }
#ifndef CUSTOMER_ANDY_SPECIFIC_COORD_BUILD
_end:
#endif
   SYS_freeMem(indParams_p->bdHndl);
   indParams_p->bdHndl = SYS_INV_BD_HDNL;

   return;
}

#ifndef DEV_TYPE_SNIFFER
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void GW_relayCnfrm(UINT8_t moduleId, UINT8_t sts)
{
   UINT16_t respVal = moduleId;
   respVal = (respVal << 8) | (UINT8_t)sts;
   GW_buildSendResp(LPWMN_GW_MSG_TYPE_RELAY_TO_NODE,
                    (UINT8_t *)&respVal, 2);
   GW_cntxt.relayFrameBdHndl = SYS_MAX_BUFF_DESC_CNT;
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
void GW_sendPathDiscEvt(UINT8_t evtId,
                        UINT8_t sts,
                        UINT8_t bdHndl)
{
   UINT8_t evtHndl;
   UINT16_t msgLen = SYS_TRSCN_Q_HNDL_FIELD_LEN
                     + EXT_IF_FRAME_HDR_LEN
                     + LPWMN_GW_EVT_ID_LEN
                     + MAC_SHORT_ADDR_LEN
                     + LPMWN_GW_MSG_PATH_DISC_OPN_STS_FIELD_LEN;
   UINT16_t pathRepLen = 0;

   if (SYS_BD_HNDL_IS_VALID(bdHndl))
   {
       pathRepLen = SYS_GET_BD_LEN(bdHndl);
       msgLen += pathRepLen;
   }

   evtHndl = SYS_allocMem(msgLen);
   if (SYS_BD_HNDL_IS_VALID(evtHndl))
   {
       UINT8_t *hdr_p = SYS_GET_TRSCN_Q_BD_BUFF(evtHndl);
       UINT8_t *pyld_p = hdr_p + EXT_IF_FRAME_HDR_LEN;

       *(pyld_p ++) = evtId;
       pyld_p = UTIL_htons(pyld_p, AODVL_cntxt.pathDiscTgt);
       *(pyld_p ++) = sts;

       if (pathRepLen > 0)
       {
           UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);
           memcpy(pyld_p, buff_p, pathRepLen);
       }

#if defined(COORD_EXT_IF_UART)
       UART_buildMsgHdr(LPWMN_GW_MSG_TYPE_EVENT,
                        hdr_p,
                        msgLen - SYS_TRSCN_Q_HNDL_FIELD_LEN - EXT_IF_FRAME_HDR_LEN);
       UART_addToTxPendQ(evtHndl);
#else
       SYNC_RT_buildMsgHdr(PLTFRM_SYNC_RT_1_DEV_ID,
    		               LPWMN_GW_MSG_TYPE_EVENT, hdr_p,
                           msgLen - SYS_TRSCN_Q_HNDL_FIELD_LEN - EXT_IF_FRAME_HDR_LEN);
       SYNC_RT_addToTxPendQ(PLTFRM_SYNC_RT_1_DEV_ID, evtHndl);
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
void GW_sendLocalRtDiscStartedEvt(UINT16_t shortAddr, UINT16_t attemptNr)
{
   UINT8_t bdHndl;
   UINT16_t msgLen = SYS_TRSCN_Q_HNDL_FIELD_LEN + EXT_IF_FRAME_HDR_LEN;

   msgLen += (LPWMN_GW_EVT_ID_LEN + MAC_SHORT_ADDR_LEN + 2);

   bdHndl = SYS_allocMem(msgLen);
   if (SYS_BD_HNDL_IS_VALID(bdHndl))
   {
       UINT8_t *hdr_p = SYS_GET_TRSCN_Q_BD_BUFF(bdHndl);
       UINT8_t *pyld_p = hdr_p + EXT_IF_FRAME_HDR_LEN;

       *(pyld_p) = LPWMN_GW_EVT_TYPE_LOCAL_RT_DISC_STARTED;
       pyld_p ++;
       pyld_p = UTIL_htons(pyld_p, shortAddr);
       pyld_p = UTIL_htons(pyld_p, attemptNr);

#if defined(COORD_EXT_IF_UART)
       UART_buildMsgHdr(LPWMN_GW_MSG_TYPE_EVENT,
                        hdr_p,
                        msgLen - SYS_TRSCN_Q_HNDL_FIELD_LEN - EXT_IF_FRAME_HDR_LEN);
       UART_addToTxPendQ(bdHndl);
#else
       SYNC_RT_buildMsgHdr(PLTFRM_SYNC_RT_1_DEV_ID,
    		               LPWMN_GW_MSG_TYPE_EVENT, hdr_p,
                           msgLen - SYS_TRSCN_Q_HNDL_FIELD_LEN - EXT_IF_FRAME_HDR_LEN);
       SYNC_RT_addToTxPendQ(PLTFRM_SYNC_RT_1_DEV_ID, bdHndl);
#endif
   }

   return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void GW_sendEvtType_1(UINT8_t evtId, UINT16_t shortAddr)
{
   UINT8_t bdHndl;
   UINT16_t msgLen = SYS_TRSCN_Q_HNDL_FIELD_LEN + EXT_IF_FRAME_HDR_LEN;

   msgLen += (LPWMN_GW_EVT_ID_LEN + MAC_SHORT_ADDR_LEN);
   bdHndl = SYS_allocMem(msgLen);
   if (SYS_BD_HNDL_IS_VALID(bdHndl))
   {
       UINT8_t *hdr_p = SYS_GET_TRSCN_Q_BD_BUFF(bdHndl);
       UINT8_t *pyld_p = hdr_p + EXT_IF_FRAME_HDR_LEN;

       *(pyld_p) = evtId;
       pyld_p ++;
       pyld_p = UTIL_htons(pyld_p, shortAddr);

#if defined(COORD_EXT_IF_UART)
       UART_buildMsgHdr(LPWMN_GW_MSG_TYPE_EVENT,
                        hdr_p,
                        msgLen - SYS_TRSCN_Q_HNDL_FIELD_LEN - EXT_IF_FRAME_HDR_LEN);
       UART_addToTxPendQ(bdHndl);
#else
       SYNC_RT_buildMsgHdr(PLTFRM_SYNC_RT_1_DEV_ID,
    		               LPWMN_GW_MSG_TYPE_EVENT, hdr_p,
                           msgLen - SYS_TRSCN_Q_HNDL_FIELD_LEN - EXT_IF_FRAME_HDR_LEN);
       SYNC_RT_addToTxPendQ(PLTFRM_SYNC_RT_1_DEV_ID, bdHndl);
#endif
   }

   return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void GW_sendRtPurgeEvent(UINT16_t shortAddr)
{
   GW_sendEvtType_1(LPWMN_GW_EVT_TYPE_PURGE_RT_TBL, shortAddr);
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
void GW_sendInvPktSrcEvt(UINT16_t shortAddr)
{
   GW_sendEvtType_1(LPWMN_GW_EVT_TYPE_RCVD_PKT_WITH_INV_SRC_ADDR, shortAddr);
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
void GW_sendRtReqFromInvSrc(UINT16_t shortAddr)
{
   GW_sendEvtType_1(LPWMN_GW_EVT_TYPE_RCVD_RT_REQ_WITH_INV_SRC_ADDR, shortAddr);
   return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void GW_sendMacTxCnfrmEvt(UINT16_t dstAddr, UINT8_t macSts, UINT8_t srcModule)
{
   static UINT16_t macCnfrmCnt = 0;
   UINT8_t bdHndl;
   UINT16_t msgLen = SYS_TRSCN_Q_HNDL_FIELD_LEN + EXT_IF_FRAME_HDR_LEN;

   macCnfrmCnt ++;

   msgLen += (LPWMN_GW_EVT_ID_LEN + 2 + 1 + 1);

   bdHndl = SYS_allocMem(msgLen);
   if (SYS_BD_HNDL_IS_VALID(bdHndl))
   {
       UINT8_t *hdr_p = SYS_GET_TRSCN_Q_BD_BUFF(bdHndl);
       UINT8_t *pyld_p = hdr_p + EXT_IF_FRAME_HDR_LEN;

       *(pyld_p ++) = LPWMN_GW_EVT_TYPE_MAC_CNFRM;
       pyld_p = UTIL_htons(pyld_p, macCnfrmCnt);
       *(pyld_p ++) = macSts;
       *(pyld_p ++) = srcModule;

#if defined(COORD_EXT_IF_UART)
       UART_buildMsgHdr(LPWMN_GW_MSG_TYPE_EVENT,
                        hdr_p,
                        msgLen - SYS_TRSCN_Q_HNDL_FIELD_LEN - EXT_IF_FRAME_HDR_LEN);
       UART_addToTxPendQ(bdHndl);
#else
       SYNC_RT_buildMsgHdr(PLTFRM_SYNC_RT_1_DEV_ID,
    		               LPWMN_GW_MSG_TYPE_EVENT, hdr_p,
                           msgLen - SYS_TRSCN_Q_HNDL_FIELD_LEN - EXT_IF_FRAME_HDR_LEN);
       SYNC_RT_addToTxPendQ(PLTFRM_SYNC_RT_1_DEV_ID, bdHndl);
#endif
   }
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void GW_sendBcnReqRcvdEvt(const UINT8_t * const extAddr_p,
                          const SINT8_t rssi, const UINT8_t lqi)
{
   UINT8_t bdHndl;
   UINT16_t msgLen = SYS_TRSCN_Q_HNDL_FIELD_LEN
                     + EXT_IF_FRAME_HDR_LEN;
   static UINT16_t bcnReqCnt = 0;

   bcnReqCnt ++;

   msgLen += (LPWMN_GW_EVT_ID_LEN + MAC_EXT_ADDR_LEN + 1 + 1);

   bdHndl = SYS_allocMem(msgLen);
   if (SYS_BD_HNDL_IS_VALID(bdHndl))
   {
       UINT8_t *hdr_p = SYS_GET_TRSCN_Q_BD_BUFF(bdHndl);
       UINT8_t *pyld_p = hdr_p + EXT_IF_FRAME_HDR_LEN;

       *(pyld_p ++) = LPWMN_GW_EVT_TYPE_BCN_REQ_RCVD;
       pyld_p = UTIL_htons(pyld_p, bcnReqCnt);
       *(pyld_p ++) = rssi;
       *(pyld_p ++) = lqi;
       memcpy(pyld_p, extAddr_p, MAC_EXT_ADDR_LEN);

#if defined(COORD_EXT_IF_UART)
       UART_buildMsgHdr(LPWMN_GW_MSG_TYPE_EVENT,
                        hdr_p,
                        msgLen - SYS_TRSCN_Q_HNDL_FIELD_LEN - EXT_IF_FRAME_HDR_LEN);
       UART_addToTxPendQ(bdHndl);
#else
       SYNC_RT_buildMsgHdr(PLTFRM_SYNC_RT_1_DEV_ID,
    		               LPWMN_GW_MSG_TYPE_EVENT, hdr_p,
                           msgLen - SYS_TRSCN_Q_HNDL_FIELD_LEN - EXT_IF_FRAME_HDR_LEN);
       SYNC_RT_addToTxPendQ(PLTFRM_SYNC_RT_1_DEV_ID, bdHndl);
#endif
   }

   return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void GW_sendLocalRtDiscOverEvt(UINT16_t destAddr, UINT16_t nextHop)
{
   UINT8_t bdHndl;
   UINT16_t msgLen = SYS_TRSCN_Q_HNDL_FIELD_LEN + EXT_IF_FRAME_HDR_LEN;

   msgLen += (LPWMN_GW_EVT_ID_LEN + MAC_SHORT_ADDR_LEN + MAC_SHORT_ADDR_LEN);

   bdHndl = SYS_allocMem(msgLen);
   if (SYS_BD_HNDL_IS_VALID(bdHndl))
   {
       UINT8_t *hdr_p = SYS_GET_TRSCN_Q_BD_BUFF(bdHndl);
       UINT8_t *pyld_p = hdr_p + EXT_IF_FRAME_HDR_LEN;

       *(pyld_p) = LPWMN_GW_EVT_TYPE_LOCAL_RT_DISC_OVER;
       pyld_p ++;
       pyld_p = UTIL_htons(pyld_p, destAddr);
       pyld_p = UTIL_htons(pyld_p, nextHop);

#if defined(COORD_EXT_IF_UART)
       UART_buildMsgHdr(LPWMN_GW_MSG_TYPE_EVENT,
                        hdr_p,
                        msgLen - SYS_TRSCN_Q_HNDL_FIELD_LEN - EXT_IF_FRAME_HDR_LEN);
       UART_addToTxPendQ(bdHndl);
#else
       SYNC_RT_buildMsgHdr(PLTFRM_SYNC_RT_1_DEV_ID,
    		               LPWMN_GW_MSG_TYPE_EVENT, hdr_p,
                           msgLen - SYS_TRSCN_Q_HNDL_FIELD_LEN - EXT_IF_FRAME_HDR_LEN);
       SYNC_RT_addToTxPendQ(PLTFRM_SYNC_RT_1_DEV_ID, bdHndl);
#endif
   }

   return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void GW_sendAssocReqRcvdEvt(UINT8_t *extAddr_p)
{
   UINT8_t bdHndl;
   UINT16_t msgLen = SYS_TRSCN_Q_HNDL_FIELD_LEN + EXT_IF_FRAME_HDR_LEN;

   msgLen += (LPWMN_GW_EVT_ID_LEN + MAC_EXT_ADDR_LEN);

   bdHndl = SYS_allocMem(msgLen);
   if (SYS_BD_HNDL_IS_VALID(bdHndl))
   {
       UINT8_t *hdr_p = SYS_GET_TRSCN_Q_BD_BUFF(bdHndl);
       UINT8_t *pyld_p = hdr_p + EXT_IF_FRAME_HDR_LEN;

       *(pyld_p) = LPWMN_GW_EVT_TYPE_ASSOC_REQ_RCVD;
       pyld_p ++;

       memcpy(pyld_p, extAddr_p, MAC_EXT_ADDR_LEN);
       pyld_p += MAC_EXT_ADDR_LEN;

#if defined(COORD_EXT_IF_UART)
       UART_buildMsgHdr(LPWMN_GW_MSG_TYPE_EVENT,
                        hdr_p,
                        msgLen - SYS_TRSCN_Q_HNDL_FIELD_LEN - EXT_IF_FRAME_HDR_LEN);
       UART_addToTxPendQ(bdHndl);
#else
       SYNC_RT_buildMsgHdr(PLTFRM_SYNC_RT_1_DEV_ID,
    		               LPWMN_GW_MSG_TYPE_EVENT, hdr_p,
                           msgLen - SYS_TRSCN_Q_HNDL_FIELD_LEN - EXT_IF_FRAME_HDR_LEN);
       SYNC_RT_addToTxPendQ(PLTFRM_SYNC_RT_1_DEV_ID, bdHndl);
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
void GW_sendAssocReqDropEvt(UINT8_t *extAddr_p, SINT8_t timeLeftToAssoc)
{
   UINT8_t bdHndl;
   UINT16_t msgLen = SYS_TRSCN_Q_HNDL_FIELD_LEN + EXT_IF_FRAME_HDR_LEN;

   msgLen += (LPWMN_GW_EVT_ID_LEN + MAC_EXT_ADDR_LEN + 1);

   bdHndl = SYS_allocMem(msgLen);
   if (SYS_BD_HNDL_IS_VALID(bdHndl))
   {
       UINT8_t *hdr_p = SYS_GET_TRSCN_Q_BD_BUFF(bdHndl);
       UINT8_t *pyld_p = hdr_p + EXT_IF_FRAME_HDR_LEN;

       *(pyld_p) = LPWMN_GW_EVT_TYPE_ASSOC_REQ_DROPPED;
       pyld_p ++;

       memcpy(pyld_p, extAddr_p, MAC_EXT_ADDR_LEN);
       pyld_p += MAC_EXT_ADDR_LEN;

       *(pyld_p) = timeLeftToAssoc;

#if defined(COORD_EXT_IF_UART)
       UART_buildMsgHdr(LPWMN_GW_MSG_TYPE_EVENT,
                        hdr_p,
                        msgLen - SYS_TRSCN_Q_HNDL_FIELD_LEN - EXT_IF_FRAME_HDR_LEN);
       UART_addToTxPendQ(bdHndl);
#else
       SYNC_RT_buildMsgHdr(PLTFRM_SYNC_RT_1_DEV_ID,
    		               LPWMN_GW_MSG_TYPE_EVENT, hdr_p,
                           msgLen - SYS_TRSCN_Q_HNDL_FIELD_LEN - EXT_IF_FRAME_HDR_LEN);
       SYNC_RT_addToTxPendQ(PLTFRM_SYNC_RT_1_DEV_ID, bdHndl);
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
void GW_sendNodeEvt(NM_nodeListEntry_s *nodeInfo_p, UINT8_t eventId)
{
   UINT8_t bdHndl;
   UINT16_t msgLen = SYS_TRSCN_Q_HNDL_FIELD_LEN + EXT_IF_FRAME_HDR_LEN;

   msgLen += (LPWMN_GW_EVT_ID_LEN
              + MAC_EXT_ADDR_LEN
              + MAC_SHORT_ADDR_LEN
              + MAC_NODE_CAPABILITY_INFO_LEN);

   bdHndl = SYS_allocMem(msgLen);
   if (SYS_BD_HNDL_IS_VALID(bdHndl))
   {
       UINT8_t *hdr_p = SYS_GET_TRSCN_Q_BD_BUFF(bdHndl);
       UINT8_t *pyld_p = hdr_p + EXT_IF_FRAME_HDR_LEN;

       *(pyld_p) = eventId;
       pyld_p ++;
       pyld_p = UTIL_htons(pyld_p, nodeInfo_p->shortAddr);
       memcpy(pyld_p, nodeInfo_p->extAddr, MAC_EXT_ADDR_LEN);
       pyld_p += MAC_EXT_ADDR_LEN;
       *(pyld_p) = nodeInfo_p->nodeCapInfo;

#if defined(COORD_EXT_IF_UART)
       UART_buildMsgHdr(LPWMN_GW_MSG_TYPE_EVENT,
                        hdr_p,
                        msgLen - SYS_TRSCN_Q_HNDL_FIELD_LEN - EXT_IF_FRAME_HDR_LEN);
       UART_addToTxPendQ(bdHndl);
#else
       SYNC_RT_buildMsgHdr(PLTFRM_SYNC_RT_1_DEV_ID,
    		               LPWMN_GW_MSG_TYPE_EVENT, hdr_p,
                           msgLen - SYS_TRSCN_Q_HNDL_FIELD_LEN - EXT_IF_FRAME_HDR_LEN);
       SYNC_RT_addToTxPendQ(PLTFRM_SYNC_RT_1_DEV_ID, bdHndl);
#endif
   }

   return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void GW_sendNodeRegEvt(NM_nodeListEntry_s *nodeInfo_p)
{
#ifndef CUSTOMER_EPOCH_SPECIFIC_BUILD
   GW_sendNodeEvt(nodeInfo_p, LPWMN_GW_EVT_TYPE_NODE_REG);
#endif
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void GW_sendNodeDeRegEvt(NM_nodeListEntry_s *nodeInfo_p)
{
#ifndef CUSTOMER_EPOCH_SPECIFIC_BUILD
   GW_sendNodeEvt(nodeInfo_p, LPWMN_GW_EVT_TYPE_NODE_DEREG);
#endif
}



/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void GW_sendSysBootEvt(void)
{
#ifndef CUSTOMER_EPOCH_SPECIFIC_BUILD
   UINT8_t bdHndl;
   UINT16_t msgLen = SYS_TRSCN_Q_HNDL_FIELD_LEN + EXT_IF_FRAME_HDR_LEN;

   msgLen += (LPWMN_GW_EVT_ID_LEN);

   bdHndl = SYS_allocMem(msgLen);
   if (SYS_BD_HNDL_IS_VALID(bdHndl))
   {
       UINT8_t *hdr_p = SYS_GET_TRSCN_Q_BD_BUFF(bdHndl);
       UINT8_t *pyld_p = hdr_p + EXT_IF_FRAME_HDR_LEN;

       *(pyld_p) = LPWMN_GW_EVT_TYPE_SYS_BOOT;

#if defined(COORD_EXT_IF_UART)
       UART_buildMsgHdr(LPWMN_GW_MSG_TYPE_EVENT,
                        hdr_p,
                        msgLen - SYS_TRSCN_Q_HNDL_FIELD_LEN - EXT_IF_FRAME_HDR_LEN);
       UART_addToTxPendQ(bdHndl);
#else
       SYNC_RT_buildMsgHdr(PLTFRM_SYNC_RT_1_DEV_ID,
                           LPWMN_GW_MSG_TYPE_EVENT, hdr_p,
                           msgLen - SYS_TRSCN_Q_HNDL_FIELD_LEN - EXT_IF_FRAME_HDR_LEN);
       SYNC_RT_addToTxPendQ(PLTFRM_SYNC_RT_1_DEV_ID, bdHndl);
#endif
   }
#endif

   return;
}

#endif

const static SINT8_t __radioPartNr[ ] =
{
#ifdef RADIO_CC2520
   "CC2520"
#elif defined(RADIO_CC1200)
   "CC1200"
#elif defined(RADIO_CC1101)
   "CC1101"
#elif defined(RADIO_CC2500)
   "CC2500"
#else
#error RADIO not specified !!
#endif
};
                               

const static SINT8_t RADIO_freqBand[ ] =
{
#ifdef RADIO_FREQ_BAND_865_867_MHZ
  "865-867 MHz"
#elif defined(RADIO_FREQ_BAND_868_MHZ)
  "868 MHz"
#elif defined(RADIO_FREQ_BAND_2400_MHZ)
  "2.4 GHz"
#elif defined(RADIO_FREQ_BAND_315_MHZ)
  "315 MHz"
#elif defined(RADIO_FREQ_BAND_433_MHZ)
  "433 MHz"
#elif defined(RADIO_FREQ_BAND_915_MHZ)
  "915 MHz"
#else
#error radio frequency band not specified !!
#endif
};

const static SINT8_t RADIO_modFmt[ ] =
{
#ifdef RADIO_MOD_FMT_GFSK
   "GFSK",
#elif defined(RADIO_MOD_FMT_2FSK)
   "2-FSK",
#elif defined(RADIO_MOD_FMT_4FSK)
   "4-FSK",
#elif defined(RADIO_MOD_FMT_MSK)
   "MSK",
#elif defined(RADIO_MOD_FMT_ASK)
   "ASK",
#elif defined(RADIO_MOD_FMT_OOK)
   "OOK",
#else
#error radio modulation format specified !!
#endif
};

const static UINT32_t RADIO_baudRate = 
#ifdef RADIO_BAUD_RATE_1200
  1200;
#elif defined(RADIO_BAUD_RATE_10000)
  10000;
#elif defined(RADIO_BAUD_RATE_38383)
  38383;
#elif defined(RADIO_BAUD_RATE_250000)
  (UINT32_t)250000;
#else
#error radio baud rate not specified !!
#endif 

UINT8_t __attrValBuff[NM_MAX_ATTR_VAL_LEN];

UINT8_t GW_waitConvTimeSecs = 0;

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
#if defined(CUSTOMER_ANDY_SPECIFIC_COORD_BUILD) && defined(PLTFRM_CC2D33S_SENSOR_ENA)
void GW_getSnsrOp(void)
{
   if (GW_waitConvTimeSecs > 0)
   {
       GW_waitConvTimeSecs --;
       if (GW_waitConvTimeSecs == 0)
       {
           UINT16_t rhVal;
           SINT16_t tempVal;
           UINT8_t bdHndl;
           PLTFRM_sts_t sts;

           sts = CC2D33S_dataFetch(PLTFRM_CC2D33S_1_RH_DEV_ID,
                                   &rhVal,
                                   &tempVal);
           if (sts != PLTFRM_STS_SUCCESS)
           {
               rhVal = 0;
               tempVal = 0;
           }

           {
                const UINT8_t pktLen = DIS_MSG_TYPE_SZ
                                       + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT_LIST
                                       + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT   - CC2D33S relative humidity
                                       + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + CC2D33S_SENSOR_RH_OUTPUT_VALUE_SZ
                                       + UTIL_TLV_HDR_SZ
                                       + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + CC2D33S_SENSOR_TEMP_OUTPUT_VALUE_SZ;

                bdHndl = SYS_allocMem(pktLen);
                if (SYS_BD_HNDL_IS_VALID(bdHndl))
                {
                    UINT8_t *buff_p, *tlvList_p;
                    UINT8_t *tlvHdr_p;

                    buff_p = SYS_GET_BD_BUFF(bdHndl);

                    *buff_p = DIS_MSG_TYPE_SENSOR_OUTPUT;
                    buff_p += DIS_MSG_TYPE_SZ;

                    tlvList_p = buff_p;
                    buff_p += UTIL_TLV_HDR_SZ;

                    tlvHdr_p = buff_p;
                    buff_p += UTIL_TLV_HDR_SZ;
                    buff_p = UTIL_buildTlvHdr(buff_p,
                                              DIS_TLV_TYPE_SENSOR_ID,
                                              DIS_SENSOR_ID_FIELD_SZ);
                    *(buff_p ++) = PLTFRM_CC2D33S_1_RH_DEV_ID;

                    buff_p = UTIL_buildTlvHdr(buff_p,
                                              DIS_TLV_TYPE_VALUE,
                                              CC2D33S_SENSOR_RH_OUTPUT_VALUE_SZ);
                    buff_p = UTIL_htons(buff_p, rhVal);

                    UTIL_buildTlvHdr(tlvHdr_p,
                                     DIS_TLV_TYPE_SENSOR_OUTPUT,
                                     buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);

                    tlvHdr_p = buff_p;
                    buff_p += UTIL_TLV_HDR_SZ;

                    buff_p = UTIL_buildTlvHdr(buff_p,
                                              DIS_TLV_TYPE_SENSOR_ID,
                                              DIS_SENSOR_ID_FIELD_SZ);
                    *(buff_p ++) = PLTFRM_CC2D33S_1_TEMP_DEV_ID;

                    buff_p = UTIL_buildTlvHdr(buff_p,
                                              DIS_TLV_TYPE_VALUE,
                                              CC2D33S_SENSOR_TEMP_OUTPUT_VALUE_SZ);
                    buff_p = UTIL_htons(buff_p, tempVal);

                    UTIL_buildTlvHdr(tlvHdr_p,
                                     DIS_TLV_TYPE_SENSOR_OUTPUT,
                                     buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);

                    UTIL_buildTlvHdr(tlvList_p,
                                     DIS_TLV_TYPE_SENSOR_OUTPUT_LIST,
                                     buff_p - tlvList_p - UTIL_TLV_HDR_SZ);

                    {
                        MCPS_dataIndParams_s params;

                        params.rssi = 0;
                        params.lqi = 0;
                        params.bdHndl = bdHndl;

                        GW_relayMsgFromRemoteNode(LPWMN_COORD_SHORT_ADDR, &params);
                    }
                }
           }
       }
   }
}
#endif


UINT8_t GW_channMonResp[6 + 6*3];
RADIO_channMonReq_s GW_channMonReqParams;

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void GW_rcvdMsgHndlr(UINT16_t msgType, UINT8_t bdHndl)
{
   switch (msgType)
   {
#if defined(CUSTOMER_ANDY_SPECIFIC_COORD_BUILD) && defined(PLTFRM_CC2D33S_SENSOR_ENA)
      case LPWMN_GW_MSG_TYPE_TRIGGER_SENSOR_CONV:
           {
        	   PLTFRM_sts_t sts;
        	   UINT16_t tmoVal;
        	   UINT8_t rc = 0;

               if (GW_waitConvTimeSecs == 0)
               {
        	       sts = CC2D33S_startSingleConv(PLTFRM_CC2D33S_1_RH_DEV_ID, &tmoVal);
        	       if (sts == PLTFRM_STS_OPN_IN_PROGRESS)
        	       {
        	           GW_waitConvTimeSecs = 2;
        	           rc = 1;
        	       }
               }
               else
                   rc = 2;

               GW_buildSendResp(msgType, &rc, 1);

           }
           break;
#endif

#ifdef COORD_IDLE_UNTIL_TRIGGER
      case LPWMN_GW_MSG_TYPE_START_COORD:
           {
              UINT8_t sts = NM_startCoord();
              GW_buildSendResp(msgType, &sts, 1);   // 1 byte
           }
           break;
#endif

#ifdef RADIO_CC1101
#ifdef LPWMN_COORD
      case LPWMN_GW_MSG_TYPE_MON_CHANN:
           {
              UINT8_t error = 1;

              if (NM_cntxt.coordStarted == 0)
              {
                  UINT16_t pyldLen = SYS_GET_BD_LEN(bdHndl);

                  if (pyldLen == 8)
                  {
                      UINT8_t off, idx, *buff_p = SYS_GET_BD_BUFF(bdHndl);

                      GW_channMonReqParams.sampleCnt = UTIL_ntohl(buff_p);
                      GW_channMonReqParams.interval = UTIL_ntohl(buff_p + 4);

                      GW_channMonReqParams.maxMonRSSI =  -10;  // dBm
                      GW_channMonReqParams.minMonRSSI =  -140;  // dBm

                      CC1101_monRSSI(&GW_channMonReqParams);

                      error = 0;
                      UTIL_htons(GW_channMonResp, GW_channMonReqParams.minMeasRSSI);
                      UTIL_htons(GW_channMonResp + 2, GW_channMonReqParams.maxMeasRSSI);
                      UTIL_htons(GW_channMonResp + 4, GW_channMonReqParams.avgMeasRSSI);
                      off = 6;
                      for (idx=0; idx<3; idx++)
                      {
                    	  UTIL_htons(GW_channMonResp + off, GW_channMonReqParams._rsltArray[idx].start);
                    	  off += 2;
                    	  UTIL_htons(GW_channMonResp + off, GW_channMonReqParams._rsltArray[idx].start + GW_channMonReqParams.binSpan);
                    	  off += 2;
                    	  UTIL_htons(GW_channMonResp + off, GW_channMonReqParams._rsltArray[idx].percentage);
                    	  off += 2;
                      }
                  }
              }

              if (error)
              {
                  UTIL_htons(GW_channMonResp, 100);
                  UTIL_htons(GW_channMonResp + 2, 100);
                  UTIL_htons(GW_channMonResp + 4, 100);
              }

              GW_buildSendResp(msgType, GW_channMonResp, sizeof(GW_channMonResp));
           }
           break;
#endif
#endif

      case LPWMN_GW_MSG_TYPE_REBOOT_COORD:
           {
              PLTFRM_saveSysFEId(SYS_FATAL_ERR_REBOOT_ON_REQ);
              PLTFRM_mcuReset();
           }
    	   break;

      case LPWMN_GW_MSG_TYPE_GET_COORD_SW_BUILD_DATE:
           {
              GW_buildSendResp(msgType, __DATE__, strlen(__DATE__) + 1);
           }
           break;

      case LPWMN_GW_MSG_TYPE_GET_COORD_SW_BUILD_TIME:
           {
              GW_buildSendResp(msgType, __TIME__, strlen(__TIME__) + 1);
           }
           break;

      case LPMWN_GW_MSG_TYPE_GET_COORD_FREE_MEM_DESC_CNT:
           {
        	  UINT16_t cnt = SYS_findFreeDescCnt();
              GW_buildSendResp(msgType, (UINT8_t *)(&cnt), sizeof(UINT16_t));
           }
           break;

      case LPWMN_GW_MSG_TYPE_SET_RADIO_CHANNEL:
           {
              UINT8_t sts = 0;

              if (!SYS_BD_HNDL_IS_VALID(bdHndl))
                  break;

#ifdef LPWMN_COORD
              if (!(NM_coordStarted()))
#endif
              {
                  UINT16_t pyldLen = SYS_GET_BD_LEN(bdHndl);
                  if (pyldLen == LPWMN_GW_MSG_RADIO_CHANN_FIELD_LEN)
                  {
                      UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);
                      UINT16_t channIdx;

                      channIdx = UTIL_ntohs(buff_p);

#ifdef RADIO_CC1101
                      if (channIdx >= 1 && channIdx <= 9)
#else
                      if (channIdx >= 1 && channIdx <= 16)
#endif
                      {
                          channIdx --;
                          PHY_PIB.phyCurrentChannel = channIdx;
                          if (RADIO_setChan(PHY_PIB.phyCurrentChannel) != PLTFRM_STS_SUCCESS)
                              SYS_fatal(SYS_FATAL_ERR_69);
                          sts = 1;
                      }
                  }
              }
              GW_buildSendResp(msgType,  &sts, 1);   // 1 byte
           }
           break;

      case LPWMN_GW_MSG_TYPE_GET_RADIO_PART_NR:
           {
              GW_buildSendResp(msgType,
                               __radioPartNr,
                               sizeof(__radioPartNr));
           }
           break;

      case LPWMN_GW_MSG_TYPE_GET_RADIO_FREQ_BAND:
           {
              GW_buildSendResp(msgType,
                               RADIO_freqBand,
                               sizeof(RADIO_freqBand));
           }
           break;

      case LPWMN_GW_MSG_TYPE_GET_CARRIER_FREQ:
            {
               UINT32_t cfHz = RADIO_getCarrierFreq(PHY_PIB.phyCurrentChannel);

               GW_buildSendResp(msgType,
             		           (UINT8_t *)(&cfHz), sizeof(UINT32_t));
            }
            break;

       case LPWMN_GW_MSG_TYPE_GET_NWK_CHANNEL:
            {
               UINT16_t channNr = PHY_PIB.phyCurrentChannel + 1;

               GW_buildSendResp(msgType, (UINT8_t *)(&channNr), sizeof(channNr));
            }
            break;

       case LPWMN_GW_MSG_TYPE_GET_RADIO_MOD_FMT:
            {
               GW_buildSendResp(msgType, RADIO_modFmt, sizeof(RADIO_modFmt));
            }
            break;

       case LPWMN_GW_MSG_TYPE_GET_RADIO_BAUD_RATE:
            {
               GW_buildSendResp(msgType, (UINT8_t *)(&RADIO_baudRate), sizeof(RADIO_baudRate));
            }
            break;

#ifndef DEV_TYPE_SNIFFER
      case LPWMN_GW_MSG_TYPE_CFG_DROP_BCN_REQS:
           {
              MAC_cfgDropBcnReqs();
           }
           break;

      case LPWMN_GW_MSG_TYPE_CFG_ACCEPT_BCN_REQS:
           {
              MAC_cfgAcceptBcnReqs();
           }
           break;

      case LPWMN_GW_MSG_TYPE_ENABLE_NWK_JOINS:
           {
              MAC_allowNwkJoins();
           }
           break;

      case LPWMN_GW_MSG_TYPE_DISABLE_NWK_JOINS:
           {
              MAC_disAllowNwkJoins();
           }
           break;

      case LPWMN_GW_MSG_TYPE_GET_NWK_JOIN_CTRL_STATE:
           {
              UINT8_t flag = MAC_nwkJoinsAllowed() ? 0x1 : 0x0;
              GW_buildSendResp(msgType, &flag, 1);   // 1 byte
           }
           break;

      case LPWMN_GW_MSG_TYPE_RELAY_TO_NODE:
           {
              if (SYS_BD_HNDL_IS_VALID(bdHndl))
              {
                  UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);
                  if (SYS_GET_BD_LEN(bdHndl) > MAC_SHORT_ADDR_LEN)
                  {
                      GW_cntxt.relayTarget = UTIL_ntohs(buff_p);
                      SYS_ADJ_BD_HEAD(bdHndl, MAC_SHORT_ADDR_LEN);
                      GW_cntxt.relayFrameBdHndl = bdHndl;
                      bdHndl = SYS_INV_BD_HDNL;
                      SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_GW_RELAY_REQUEST;
                  }
              }
           }
           break;

      case LPWMN_GW_MSG_TYPE_GET_LPWMN_COORD_EXT_ADDR:
           {
              GW_buildSendResp(msgType,
                               MAC_PIB.macExtendedAddress,
                               MAC_EXT_ADDR_LEN);
           }
           break;

      case LPWMN_GW_MSG_TYPE_GET_NWK_LPWMN_ID:
           {
              GW_buildSendResp(msgType,
                               (UINT8_t *)&(MAC_PIB.macLPWMNId), MAC_LPWMN_ID_LEN);
           }
           break;

      case LPWMN_GW_MSG_TYPE_GET_COORD_UP_TIME:
           {
              GW_buildSendResp(msgType,
                               (UINT8_t *)&(SYS_upTimeInSecs), SYS_COORD_UP_TIME_FIELD_LEN);
           }
           break;

      case LPWMN_GW_MSG_TYPE_GET_RADIO_TX_PWR:
           {
              SINT16_t txPower = PHY_PIB.phyTXPower;
              txPower *= 100;
              if (txPower < 0)
            	  txPower -= PHY_PIB.phyTXPowerDecPart;
              else
            	  txPower += PHY_PIB.phyTXPowerDecPart;
              GW_buildSendResp(msgType,
                               (UINT8_t *)(&txPower), sizeof(txPower));
           }
           break;

      case LPWMN_GW_MSG_TYPE_SET_COORD_ATTR_VAL:
           {
               UINT16_t pyldLen;
               UINT16_t sts = LPWMN_GW_STS_INV_PARAMS;

               if (!SYS_BD_HNDL_IS_VALID(bdHndl))
                   break;

               pyldLen = SYS_GET_BD_LEN(bdHndl);
               if (pyldLen >= NM_ATTR_ID_LEN)
               {
            	   UINT16_t attrId;
                   UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl),
                		   attrValLen;

                   attrId = UTIL_ntohs(buff_p);

                   pyldLen -= NM_ATTR_ID_LEN;

                   attrValLen = NM_getAttrValSz(attrId);
                   if (attrValLen > 0)
                   {
                	   if (pyldLen >= attrValLen)
                	   {
                		   buff_p += NM_ATTR_ID_LEN;
                		   NM_setAttrVal(attrId, buff_p);
                		   sts = LPWMN_GW_STS_SUCCESS;
                	   }
                   }
               }

               GW_buildSendResp(msgType,
                                (UINT8_t *)(&sts), sizeof(UINT16_t));
           }
           break;

      case LPWMN_GW_MSG_TYPE_GET_COORD_ATTR_VAL:
           {
               UINT16_t pyldLen;

               if (!SYS_BD_HNDL_IS_VALID(bdHndl))
                   break;

               pyldLen = SYS_GET_BD_LEN(bdHndl);
               if (pyldLen == NM_ATTR_ID_LEN)
               {
            	   UINT16_t attrId;
                   UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl),
                		   attrValLen;
                   attrId = UTIL_ntohs(buff_p);

                   attrValLen = NM_getAttrValSz(attrId);
                   if (attrValLen > 0)
                	   NM_getAttrVal(attrId, __attrValBuff);
                   else
                   {
                       attrValLen = 1;
                       __attrValBuff[0] = 0;
                   }
                   GW_buildSendResp(msgType,
                                    (UINT8_t *)(__attrValBuff), attrValLen);
               }
           }
           break;

      case LPWMN_GW_MSG_TYPE_SET_RADIO_TX_PWR:
           {
              UINT16_t pyldLen;
              UINT16_t sts = LPWMN_GW_STS_SUCCESS;

              if (!SYS_BD_HNDL_IS_VALID(bdHndl))
                  break;

              pyldLen = SYS_GET_BD_LEN(bdHndl);

              if (pyldLen == 2)
              {
                  UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);
                  SINT16_t txPower;

                  buff_p += EXT_IF_FRAME_HDR_LEN;
                  txPower = UTIL_ntohs(buff_p);

                  if (txPower != PHY_PIB.phyTXPower)
                  {
                      // Check if txPower is valid
                      if (RADIO_txPowerSetPointValid(txPower))
                          PHY_PIB.changeReqBitMsk |= PHY_TX_POWER_CHANGE_REQ_BM;
                      else
                          sts = LPWMN_GW_STS_INV_RADIO_TX_PWR_SET_POINT;
                  }
              }
              else
                  sts = LPWMN_GW_STS_INV_PYLD_LEN;

              GW_buildSendResp(msgType,
                               (UINT8_t *)(&sts), sizeof(UINT16_t));
           }
           break;

      case LPWMN_GW_MSG_TYPE_NODE_CNT_REQ:
           {
              UINT16_t nodeCnt = NM_getJoinedNodeCnt();
              GW_buildSendResp(msgType,
                               (UINT8_t *)(&nodeCnt), sizeof(UINT16_t));
           }
           break;

      case LPWMN_GW_MSG_TYPE_GET_COORD_SNSR_OP:
           {
#if 0
              if (SYS_GET_BD_LEN(bdHndl) == DIS_SENSOR_ID_FIELD_SZ)
              {
                  UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);
                  UINT8_t devId = *buff_p;

                  // COORD_APP_getSnsrOp(devId);
              }
#endif
           }
           break;

      case LPWMN_GW_MSG_TYPE_GET_NODE_LAST_FATAL_ERR_ID:
           {
              UINT16_t lastFEId = SYS_RESEVED_FATAL_ERR;

              if (!SYS_BD_HNDL_IS_VALID(bdHndl))
                  break;

              if (SYS_GET_BD_LEN(bdHndl) == MAC_SHORT_ADDR_LEN)
              {
                  UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);
                  UINT16_t shortAddr = UTIL_ntohs(buff_p);

                  if (shortAddr == LPWMN_COORD_SHORT_ADDR)
                      lastFEId = PLTFRM_savedSysFEId;
                  else
                  {
                	  NM_nodeListEntry_s *ne_p = NM_lookUpShortAddr(shortAddr);
                      if (ne_p != NULL)
                          lastFEId = ne_p->lastSysFEId;
                  }
              }

              GW_buildSendResp(msgType, (UINT8_t *)(&lastFEId), SYSTEM_FATAL_ERROR_ID_LEN);
           }
           break;

#ifndef LPWMN_COORD_ASSOC_WHITE_LIST_DIS
      case LPWMN_GW_MSG_TYPE_DEL_NODE_FROM_WHITE_LIST:
      case LPWMN_GW_MSG_TYPE_ADD_NODE_TO_WHITE_LIST:
           {
              UINT16_t pyldLen;

              if (!SYS_BD_HNDL_IS_VALID(bdHndl))
                  break;

              pyldLen = SYS_GET_BD_LEN(bdHndl);
              if (pyldLen == MAC_EXT_ADDR_LEN)
              {
                  UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);
                  if (msgType == LPWMN_GW_MSG_TYPE_DEL_NODE_FROM_WHITE_LIST)
                      MAC_delNodeFromAssocWhiteList(buff_p);
                  else
                      MAC_addNodeToAssocWhiteList(buff_p);
              }
           }
           break;


      case LPWMN_GW_MSG_TYPE_DELETE_WHITE_LIST:
           {
              MAC_clearAssocWhiteList();
           }
           break;
#endif

#ifndef LPWMN_COORD_ASSOC_BLACK_LIST_DIS
      case LPWMN_GW_MSG_TYPE_DELETE_BLACK_LIST:
           {
              MAC_clearAssocBlackList();
           }
           break;

      case LPWMN_GW_MSG_TYPE_DEL_NODE_FROM_BLACK_LIST:
      case LPWMN_GW_MSG_TYPE_ADD_NODE_TO_BLACK_LIST:
           {
              UINT16_t pyldLen;

              if (!SYS_BD_HNDL_IS_VALID(bdHndl))
                  break;

              pyldLen = SYS_GET_BD_LEN(bdHndl);
              if (pyldLen == MAC_EXT_ADDR_LEN)
              {
                  UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);
                  if (msgType == LPWMN_GW_MSG_TYPE_DEL_NODE_FROM_BLACK_LIST)
                      MAC_delNodeFromAssocBlackList(buff_p);
                  else
                      MAC_addNodeToAssocBlackList(buff_p);
              }
           }
           break;
#endif

      case LPWMN_MSG_TYPE_GET_RT_NH_ON_COORD:
           {
              UINT16_t pyldLen, nextHop;

              // Added on april/1/2015, rkris@wisense.in

              if (!SYS_BD_HNDL_IS_VALID(bdHndl))
                  break;

              pyldLen = SYS_GET_BD_LEN(bdHndl);
              if (pyldLen == MAC_SHORT_ADDR_LEN)
              {
                  UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);
                  UINT16_t shortAddr = UTIL_ntohs(buff_p);

                  if (NM_lookUpLocalRFD(shortAddr))
                      nextHop = shortAddr;  // This Coordinator is the RFD's parent
                  else
                  {
                      // Called from system.c (via uart.c) so no issues
                      nextHop = AODVL_getRoute(shortAddr);
                  }
              }

              GW_buildSendResp(msgType,
                               (UINT8_t *)&(nextHop),
                               MAC_SHORT_ADDR_LEN);
           }
           break;

#ifndef LPWMN_COORD_ASSOC_WHITE_LIST_DIS
      case LPWMN_GW_MSG_TYPE_GET_WHITE_LIST_ENTRY:
           {
              UINT16_t pyldLen;

              if (!SYS_BD_HNDL_IS_VALID(bdHndl))
                  break;

              pyldLen = SYS_GET_BD_LEN(bdHndl);
              if (pyldLen == 2)
              {
                  UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);
                  UINT16_t tblIdx;
                  
                  tblIdx = UTIL_ntohs(buff_p);
                  
                  {
                      UINT8_t bdHndlResp;
                      UINT16_t idx, msgLen = SYS_TRSCN_Q_HNDL_FIELD_LEN
                                             + EXT_IF_FRAME_HDR_LEN
                                             + LPWMN_GW_MSG_TBL_IDX_FIELD_LEN;

                      for (idx=tblIdx; idx<MAC_ASSOC_WHITE_LIST_MAX_ENTRY_CNT; idx++)
                      {
                           if ((MAC_cntxt.assocWhiteList.whiteListBitMsk & (1 << idx)))
                               break;
                      }

                      if (idx < MAC_ASSOC_WHITE_LIST_MAX_ENTRY_CNT)
                      {
                          msgLen += MAC_EXT_ADDR_LEN;
                      }

                      bdHndlResp = SYS_allocMem(msgLen);
                      if (SYS_BD_HNDL_IS_VALID(bdHndlResp))
                      {
                          UINT8_t *hdr_p = SYS_GET_TRSCN_Q_BD_BUFF(bdHndlResp);
                          UINT8_t *pyld_p = hdr_p + EXT_IF_FRAME_HDR_LEN;

                          pyld_p = UTIL_htons(pyld_p, idx);
                          if (idx < MAC_ASSOC_WHITE_LIST_MAX_ENTRY_CNT)
                          {
                              UINT8_t *a_p = MAC_cntxt.assocWhiteList.extAddrArray + (idx*MAC_EXT_ADDR_LEN);
                              memcpy(pyld_p, a_p, MAC_EXT_ADDR_LEN);
                              pyld_p += MAC_EXT_ADDR_LEN;
                          }

#if defined(COORD_EXT_IF_UART)
                          UART_buildMsgHdr(msgType, hdr_p,
                                           msgLen - SYS_TRSCN_Q_HNDL_FIELD_LEN - EXT_IF_FRAME_HDR_LEN);
                          UART_addToTxPendQ(bdHndlResp);
#else
                          SYNC_RT_buildMsgHdr(PLTFRM_SYNC_RT_1_DEV_ID, msgType, hdr_p,
                                              msgLen - SYS_TRSCN_Q_HNDL_FIELD_LEN - EXT_IF_FRAME_HDR_LEN);
                          SYNC_RT_addToTxPendQ(PLTFRM_SYNC_RT_1_DEV_ID, bdHndlResp);
#endif
                      }
                  }
              }
           }
           break;
#endif

#ifndef LPWMN_COORD_ASSOC_BLACK_LIST_DIS
      case LPWMN_GW_MSG_TYPE_GET_BLACK_LIST_ENTRY:
           {
              UINT16_t pyldLen;

              if (!SYS_BD_HNDL_IS_VALID(bdHndl))
                  break;

              pyldLen = SYS_GET_BD_LEN(bdHndl);
              if (pyldLen == 2)
              {
                  UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);
                  UINT16_t tblIdx;

                  tblIdx = UTIL_ntohs(buff_p);

                  {
                      UINT8_t bdHndlResp;
                      UINT16_t idx, msgLen = SYS_TRSCN_Q_HNDL_FIELD_LEN
                                             + EXT_IF_FRAME_HDR_LEN
                                             + LPWMN_GW_MSG_TBL_IDX_FIELD_LEN;

                      for (idx=tblIdx; idx<MAC_ASSOC_BLACK_LIST_MAX_ENTRY_CNT; idx++)
                      {
                           if ((MAC_cntxt.assocBlackList.blackListBitMsk & (1 << idx)))
                               break;
                      }

                      if (idx < MAC_ASSOC_BLACK_LIST_MAX_ENTRY_CNT)
                      {
                          msgLen += MAC_EXT_ADDR_LEN;
                      }

                      bdHndlResp = SYS_allocMem(msgLen);
                      if (SYS_BD_HNDL_IS_VALID(bdHndlResp))
                      {
                          UINT8_t *hdr_p = SYS_GET_TRSCN_Q_BD_BUFF(bdHndlResp);
                          UINT8_t *pyld_p = hdr_p + EXT_IF_FRAME_HDR_LEN;

                          pyld_p = UTIL_htons(pyld_p, idx);
                          if (idx < MAC_ASSOC_BLACK_LIST_MAX_ENTRY_CNT)
                          {
                              UINT8_t *a_p = MAC_cntxt.assocBlackList.extAddrArray + (idx*MAC_EXT_ADDR_LEN);
                              memcpy(pyld_p, a_p, MAC_EXT_ADDR_LEN);
                              pyld_p += MAC_EXT_ADDR_LEN;
                          }

#if defined(COORD_EXT_IF_UART)
                          UART_buildMsgHdr(msgType, hdr_p,
                                           msgLen - SYS_TRSCN_Q_HNDL_FIELD_LEN - EXT_IF_FRAME_HDR_LEN);
                          UART_addToTxPendQ(bdHndlResp);
#else
                          SYNC_RT_buildMsgHdr(PLTFRM_SYNC_RT_1_DEV_ID, msgType, hdr_p,
                                              msgLen - SYS_TRSCN_Q_HNDL_FIELD_LEN - EXT_IF_FRAME_HDR_LEN);
                          SYNC_RT_addToTxPendQ(PLTFRM_SYNC_RT_1_DEV_ID, bdHndlResp);
#endif
                      }
                  }
              }
           }
           break;
#endif

      case LPWMN_GW_MSG_TYPE_NODE_LIST_REQ:
           {
              UINT16_t pyldLen;

              if (!SYS_BD_HNDL_IS_VALID(bdHndl))
                  break;

              pyldLen = SYS_GET_BD_LEN(bdHndl);
              if (pyldLen == 2)
              {
                  UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);
                  UINT16_t nodeTblIdx;

                  nodeTblIdx = UTIL_ntohs(buff_p);

                  {
                      UINT8_t bdHndlResp;
                      UINT16_t idx, msgLen = SYS_TRSCN_Q_HNDL_FIELD_LEN
                                             + EXT_IF_FRAME_HDR_LEN
                                             + LPWMN_GW_MSG_NODE_IDX_FIELD_LEN;

                      for (idx=nodeTblIdx; idx<NM_MAX_SUPPORTED_NODE_CNT; idx++)
                      {
                           if (NM_cntxt.NM_nodeList[idx].shortAddr != MAC_BROADCAST_SHORT_ADDR)
                           {
                               break;
                           }
                      }

                      if (idx < NM_MAX_SUPPORTED_NODE_CNT)
                      {
                          msgLen += MAC_EXT_ADDR_LEN + MAC_SHORT_ADDR_LEN
                        		    + MAC_SHORT_ADDR_LEN   // If RFD, this is the parent's short address
                                    + MAC_NODE_CAPABILITY_INFO_LEN
									+ 2;  // Time since last data packet received (in seconds)
                      }

                      bdHndlResp = SYS_allocMem(msgLen);
                      if (SYS_BD_HNDL_IS_VALID(bdHndlResp))
                      {
                          UINT8_t *hdr_p = SYS_GET_TRSCN_Q_BD_BUFF(bdHndlResp);
                          UINT8_t *pyld_p = hdr_p + EXT_IF_FRAME_HDR_LEN;

                          pyld_p = UTIL_htons(pyld_p, idx);
                          if (idx < NM_MAX_SUPPORTED_NODE_CNT)
                          {
                        	  UINT16_t parentSA = MAC_BROADCAST_SHORT_ADDR;
                              pyld_p = UTIL_htons(pyld_p, NM_cntxt.NM_nodeList[idx].shortAddr);
                              if (!(NM_cntxt.NM_nodeList[idx].nodeCapInfo & MAC_ASSOC_REQ_CAP_INFO_DEV_TYPE_BIT_SHIFT_MSK))
                              {
                                  UINT16_t parentIdx = NM_cntxt.NM_nodeList[idx].parentNodeIdx;
                                  if (parentIdx == NM_MAX_SUPPORTED_NODE_CNT)
                                      parentSA = LPWMN_COORD_SHORT_ADDR;
                                  else
                            	      parentSA = NM_cntxt.NM_nodeList[parentIdx].shortAddr;
                              }
                              pyld_p = UTIL_htons(pyld_p, parentSA);
                              memcpy(pyld_p, NM_cntxt.NM_nodeList[idx].extAddr, MAC_EXT_ADDR_LEN);
                              pyld_p += MAC_EXT_ADDR_LEN;
                              *(pyld_p ++) = NM_cntxt.NM_nodeList[idx].nodeCapInfo;
                              pyld_p = UTIL_htons(pyld_p, NM_cntxt.NM_nodeList[idx].timeSinceLastRx);
                          }

#if defined(COORD_EXT_IF_UART)
                          UART_buildMsgHdr(msgType,
                                           hdr_p,
                                           msgLen - SYS_TRSCN_Q_HNDL_FIELD_LEN - EXT_IF_FRAME_HDR_LEN);
                          UART_addToTxPendQ(bdHndlResp);
#else
                          SYNC_RT_buildMsgHdr(PLTFRM_SYNC_RT_1_DEV_ID, msgType, hdr_p,
                                              msgLen - SYS_TRSCN_Q_HNDL_FIELD_LEN - EXT_IF_FRAME_HDR_LEN);
                          SYNC_RT_addToTxPendQ(PLTFRM_SYNC_RT_1_DEV_ID, bdHndlResp);
#endif
                      }
                  }
              }
           }
           break;
#endif

      default:
           {
           }
           break;
   }

   if (SYS_BD_HNDL_IS_VALID(bdHndl))
       SYS_freeMem(bdHndl);

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
void GW_init(void)
{
    GW_cntxt.relayFrameBdHndl = SYS_MAX_BUFF_DESC_CNT;
    GW_cntxt.relayTarget = MAC_BROADCAST_SHORT_ADDR;
    GW_cntxt.lpwmnMsgsRcvdCnt = 0;

#if defined(COORD_EXT_IF_UART)
    UART_start(PLTFRM_UART_HW_1_DEV_ID);
#elif defined(COORD_EXT_IF_SYNC_RT)
    SYNC_RT_start(PLTFRM_SYNC_RT_1_DEV_ID);
#else
#error Coord external interface not defined !!
#endif
    return;
}

#endif
#endif
