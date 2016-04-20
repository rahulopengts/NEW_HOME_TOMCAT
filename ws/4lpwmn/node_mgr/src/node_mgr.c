/*
 * File Name: node_mgr.c
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

#include <string.h>
#include <typedefs.h>
#include <util.h>
#include <pltfrm.h>
#include <radio.h>
#include <system.h>
#include <mac.h>
#include <phy_pib.h>
#include <mac_pib.h>
#include <node_mgr.h>
#ifdef DEV_TYPE_RFD
#include <rfd_app.h>
#else
// #include <app.h>
#include <ffd_app_common.h>
#endif
#include <adp.h>
#ifdef LPWMN_COORD
#include <nvm.h>
#endif
#include <gw.h>
#ifdef UC_FU_ENA
#include <fu.h>
#endif
#ifndef DEV_TYPE_RFD
#include <aodvl.h>
#endif

NM_cntxt_s NM_cntxt;


#ifndef DEV_TYPE_SNIFFER

#ifndef LPWMN_COORD   
MLME_scanRequestParams_s NM_scanReqParams;
#endif


#ifdef LPWMN_COORD
#define __setAttrVal16(val) \
      attrValLen = 2; \
      *(UINT16_t *)(buff_p) = (val);
#else
#define __setAttrVal16(val) \
      attrValLen = 2; \
      UTIL_htons(buff_p, (val));
#endif


#ifdef LPWMN_COORD
#define __setAttrVal32(val) \
      attrValLen = 4; \
      *(UINT32_t *)(buff_p) = (val);
#else
#define __setAttrVal32(val) \
      attrValLen = 4; \
      UTIL_htonl(buff_p, (val));
#endif



/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t NM_getAttrValSz(UINT16_t attrId)
{
   UINT8_t attrValLen = 0;

   switch (attrId)
   {
      case APP_SENSOR_DATA_PUSH_INTERVAL_SECS_ATTR_ID:
           attrValLen = APP_DATA_PUSH_INTERVAL_VALUE_FIELD_SZ;
           break;

      case PHY_RF_CHANN_ID_ATTR_ID:
      case MAC_TX_ON_CCA_ENA_FLAG_ATTR_ID:
      case FU_IMAGE_STORE_IMAGE_1_SEG_CNT_ATTR_ID:
      case FU_IMAGE_STORE_IMAGE_2_SEG_CNT_ATTR_ID:
      case FU_IMAGE_STORE_IMAGE_1_TYPE_ATTR_ID:
      case FU_IMAGE_STORE_IMAGE_2_TYPE_ATTR_ID:

#ifdef WISENSE_PIR_SENSOR_NODE
      case APP_PIR_SENSOR_ENA_DIS_CTRL_ATTR_ID:
#endif
           attrValLen = 1;
           break;

      case MAC_ACK_TMO_DELTA_ATTR_ID:
      case SYS_FREE_MEM_DESC_CNT_ATTR_ID:
      case MAC_ACK_FRAME_RAW_TRX_TIME:
      case FU_IMAGE_STORE_IMAGE_1_LEN_ATTR_ID:
      case FU_IMAGE_STORE_IMAGE_2_LEN_ATTR_ID:
      case NWK_RT_DISC_STARTED_CNT_ATTR_ID:
      case ADP_APP_PYLD_TX_PROC_FLR_CNT_ATTR_ID:
      case NWK_RT_DISC_SUCCESS_CNT_ATTR_ID:
      case NWK_RT_DISC_FLR_CNT_ATTR_ID:
      case NWK_RT_DISC_ERR_CNT_ATTR_ID:
      case MAC_TIME_SINCE_LAST_BCN_REQ_RCVD_ATTR_ID:
      case MAC_BCN_REQ_RCVD_CNT_ATTR_ID:
      case MAC_ASSOC_REQ_RECEIVED_CNT_ATTR_ID:
      case MAC_ASSOC_RESP_TRX_CNT_ATTR_ID:
      case MAC_MAX_RFD_SUPPORT_CNT_PER_FFD_ATTR_ID:
      case MAC_FFD_ASSOC_RFD_CNT_ATTR_ID:
      case MAC_ASSOC_REQ_RELAYED_CNT_ATTR_ID:
      case ADP_RELAYED_ASSOC_RESP_RCVD_CNT_ATTR_ID:
      case MAC_LAST_BCN_REQ_SRC_EXT_ADDR_ATTR_ID:
      case MAC_ACK_TX_CNT_ATTR_ID:
      case MAC_BAD_UC_SA_RCVD_CNT_ATTR_ID:
      case MAC_GOOD_UC_SA_RCVD_CNT_ATTR_ID:
      case SYS_BD_ALLOC_FLR_CNT_ATTR_ID:
      case SYS_BUFF_ALLOC_FLR_CNT_ATTR_ID:
      case ADP_MESH_BC_PKT_RCVD_CNT_ATTR_ID:
      case ADP_MESH_DUP_BC_PKT_DROPPED_CNT_ATTR_ID:
      case SYS_REBOOT_DELAY_ATTR_ID:
      case APP_NO_OP_CMD_RCVD_CNT_ATTR_ID:
      case MESH_TRIGGER_PATH_DISC_ATTR_ID:
           attrValLen = 2;
           break;

#ifdef VEHICLE_DETECTION_USING_HIGH_PASS_FILTER_ENA
      case APP_VEH_DET_MFS_HPF_DETECTION_THRESHOLD:
      case APP_VEH_DET_MFS_HPF_SETTLING_THRESHOLD:
           attrValLen = 2;
           break;

      case APP_VEH_DET_MFS_HPF_DETECTION_ALPHA:
          attrValLen = 1;
          break;
#endif



      case MAC_LPWMN_ID_ATTR_ID:
      case NWK_ROUTING_TABLE_MAX_ENTRY_CNT_ATTR_ID:
      case NWK_ROUTING_TABLE_VALID_ENTRY_CNT_ATTR_ID:
      case AODVL_RT_DISCOVERY_TABLE_MAX_ENTRY_CNT_ATTR_ID:
      case AODVL_RT_DISCOVERY_TABLE_VALID_ENTRY_CNT_ATTR_ID:
#ifndef DEV_TYPE_RFD
      case NWK_MESH_LOOP_DETECTED_CNT_ATTR_ID:
      case AODVL_RT_DISCOVERY_TABLE_FULL_CNT_ATTR_ID:
      // case NWK_TRIGGER_RT_TABLE_PURGE_ATTR_ID:
#endif
#ifdef DEV_TYPE_FFD
      case MAC_FFD_ASSOC_RFD_TABLE_ENTRY_ATTR_ID:
#endif
           // If this attribute is set to some value other than 0xffff on an RFD or FFD,
           // the device will only join the specified LPMWN. If set on a coordinator node,
           // the coord will always create an LPWMN with this id.
           attrValLen = 2;
           break;

      case MAC_CSMA_FLR_CNT_ATTR_ID:
      case MAC_CCA_FLR_CNT_ATTR_ID:
      case MAC_ACK_TMO_CNT_ATTR_ID:
      case MAC_CCA_OK_CNT_ATTR_ID:
      case MAC_VALID_ACK_RCVD_CNT_ATTR_ID:
      case MAC_LAST_CSMA_BACK_OFF_ATTR_ID:
      case FU_IMAGE_STORE_IMAGE_1_IMAGE_RCVD_TIME_STAMP_ATTR_ID:
      case FU_IMAGE_STORE_IMAGE_2_IMAGE_RCVD_TIME_STAMP_ATTR_ID:
      case SYS_UP_TIME_IN_SECONDS_ATTR_ID:
#ifndef DEV_TYPE_RFD
      case NWK_ROUTING_TABLE_ENTRY_ATTR_ID:
      case AODVL_RT_DISCOVERY_TABLE_ENTRY_ATTR_ID:
#endif
      case SYS_MEM_ALLOC_SUCCESS_CNT_ATTR_ID:
      case SYS_MEM_FREE_CNT_ATTR_ID:
#ifdef LPWMN_COORD
      case GW_LPWMN_MSGS_RCVD_CNT_ATTR_ID:
#endif
#ifndef LPWMN_COORD
      case APP_DATA_CONFIRM_SUCCESS_CNT_ATTR_ID:
      case APP_DATA_CONFIRM_FAILURE_CNT_ATTR_ID:
#endif
#ifdef DEV_TYPE_FFD
      case APP_ADP_DATA_REQ_CNT_ATTR_ID:
      case APP_ADP_DATA_REQ_FLR_CNT_ATTR_ID:
      case APP_SENSOR_DATA_TX_INTERVAL_ATTR_ID:
#endif
#ifdef DEV_TYPE_RFD
      case APP_MAX_SENSOR_DATA_TX_INTERVAL_ATTR_ID:
#endif
           attrValLen = 4;
           break;

      case FW_BUILD_DATE_ATTR_ID:
           {
              attrValLen = strlen(__DATE__) + 1;
           }
           break;

      case FW_BUILD_TIME_ATTR_ID:
           {
              attrValLen = strlen(__TIME__) + 1;
           }
           break;

#ifdef UC_FU_ENA
      case FU_IMAGE_STORE_IMAGE_FLAGS_ATTR_ID:
           {
              attrValLen = FU_IMAGE_STORE_IMAGE_FLAGS_ATTR_VAL_LEN;
           }
           break;
#endif

      default:
           break;
   }

   return attrValLen;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t NM_setAttrVal(UINT16_t attrId, UINT8_t *buff_p)
{
   UINT8_t attrValLen = 0;

   switch (attrId)
   {
#ifndef DEV_TYPE_RFD
#if  0
      case NWK_TRIGGER_RT_TABLE_PURGE_ATTR_ID:
           {
              destAddr = UTIL_ntohs(buff_p);
           }
           break;
#endif
#endif

#ifdef LPWMN_COORD
      case MESH_TRIGGER_PATH_DISC_ATTR_ID:
           {
              AODVL_pathDiscParams_s params;
              NM_nodeListEntry_s *node_p;

              attrValLen = MAC_SHORT_ADDR_LEN;
              params.destAddr = UTIL_ntohs(buff_p);
              node_p = NM_lookUpShortAddr(params.destAddr);
              if (AODVL_cntxt.pathDiscInProgress == 0x0
                  && node_p != NULL
                  && (node_p->nodeCapInfo & MAC_ASSOC_REQ_CAP_INFO_DEV_TYPE_BIT_SHIFT_MSK))
              {
                  // PATH discovery is only allowed to registered FFDs for which a route exists
                  // on this coordinator assuming path discovery is not running already.

                  params.cbFunc_p = NULL;
                  AODVL_pathDiscRequest(&params);
              }
           }
           break;

      case PHY_RF_CHANN_ID_ATTR_ID:
           {
              UINT8_t attrVal = 1;

              attrValLen = 1;
              attrVal = *buff_p;

              // Called function will reboot the micro !!
              PHY_updateCfgInfo(PHY_TLV_TYPE_RF_CHANN_ID,
                                PHY_RF_CHANN_ID_TLV_LEN,
                                &attrVal);
           }
           break;
#endif

#ifdef WISENSE_PIR_SENSOR_NODE
      case APP_PIR_SENSOR_ENA_DIS_CTRL_ATTR_ID:
           {
              UINT8_t attrVal = *buff_p;

              attrValLen = 1;

              // Called function will reboot the micro !!
              RFD_APP_snsrCtrlUpdate(attrVal);
           }
           break;
#endif

#ifdef DEV_TYPE_RFD
      case APP_MAX_SENSOR_DATA_TX_INTERVAL_ATTR_ID:
           {
              UINT32_t attrVal;

              attrValLen = 4;
              attrVal = UTIL_ntohl(buff_p);

              if (attrVal > 0)
                  RFD_APP_maxSnsrMsgTxInterval = attrVal;
           }
           break;
#endif

#ifdef VEHICLE_DETECTION_USING_HIGH_PASS_FILTER_ENA
      case APP_VEH_DET_MFS_HPF_DETECTION_ALPHA:
           {
              UINT8_t attrVal;

              attrValLen = 1;
              attrVal = *(buff_p);

              if (attrVal > 0)
                  VD_HPF_alpha = attrVal;
           }
           break;

      case APP_VEH_DET_MFS_HPF_DETECTION_THRESHOLD:
           {
              UINT16_t attrVal;

              attrValLen = 2;
              attrVal = UTIL_ntohs(buff_p);

              if (attrVal > 0)
                  VD_HPF_detectionTh = attrVal;
           }
           break;

      case APP_VEH_DET_MFS_HPF_SETTLING_THRESHOLD:
           {
              UINT16_t attrVal;

              attrValLen = 2;
              attrVal = UTIL_ntohs(buff_p);

              if (attrVal > 0)
                  VD_HPF_settlingTh = attrVal;
           }
           break;
#endif

      case MAC_LPWMN_ID_ATTR_ID:
           {
              UINT16_t attrVal;

              attrValLen = 2;
              attrVal = UTIL_ntohs(buff_p);

              // Called function will reboot the micro !!
              MAC_updateCfgInfo(MAC_TLV_TYPE_LPWMN_ID,
                                MAC_LPWMN_ID_LEN,
                                (UINT8_t *)&attrVal);
           }
           break;

      case MAC_TX_ON_CCA_ENA_FLAG_ATTR_ID:
           {
              attrValLen = 1;
              MAC_cntxt.txOnCCAEna = ((*buff_p) == 0 ? 0 : 1);
              MAC_cntxt.pendingAction |= MAC_PENDING_ACTION_BIT_TX_ON_CCA_CTRL;
           }
           break;

      case MAC_ACK_TMO_DELTA_ATTR_ID:
           {
              UINT16_t attrVal;

              attrValLen = 2;
              attrVal = UTIL_ntohs(buff_p);
              MAC_cntxt.ackTmoExtraMilliSecs = attrVal;
           }
           break;

      default:
           break;
   }

   return attrValLen;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t NM_getListAttrVal(UINT16_t attrId, UINT16_t attrIdx, UINT8_t *buff_p)
{
   UINT8_t attrValLen = 0;

   switch (attrId)
   {
#ifdef DEV_TYPE_FFD
      case MAC_FFD_ASSOC_RFD_TABLE_ENTRY_ATTR_ID:
           {
               UINT16_t attrVal = 0;

               attrValLen = MAC_SHORT_ADDR_LEN;
               if (attrIdx < MAC_MAX_ASSOC_RFD_CNT)
               {
                   attrVal = MAC_cntxt.assocRFDList[attrIdx];
               }

               UTIL_htons(buff_p, attrVal);
           }
           break;
#endif

#ifndef DEV_TYPE_RFD
      case AODVL_RT_DISCOVERY_TABLE_ENTRY_ATTR_ID:
           {
              UINT32_t attrVal = 0;

              attrValLen = sizeof(attrVal);

              if (attrIdx < AODVL_RT_DISC_TBL_SZ)
              {
                  attrVal = AODVL_cntxt.rtDiscTbl[attrIdx].destAddr;
                  attrVal <<= 16;
                  if (AODVL_cntxt.rtDiscTbl[attrIdx].destAddr == MAC_BROADCAST_SHORT_ADDR)
                      attrVal |= MAC_BROADCAST_SHORT_ADDR;
                  else 
                      attrVal |= AODVL_cntxt.rtDiscTbl[attrIdx].origAddr;
              }

#ifdef LPWMN_COORD
              *(UINT32_t *)(buff_p) = attrVal;
#else
              UTIL_htonl(buff_p, attrVal);
#endif
           }
           break;

      case NWK_ROUTING_TABLE_ENTRY_ATTR_ID:
           {
              UINT32_t attrVal = 0;

              attrValLen = sizeof(attrVal);

              if (attrIdx < AODVL_ROUTING_TBL_SZ)
              {
                  attrVal = AODVL_cntxt.routingTbl[attrIdx].destAddr;
                  attrVal <<= 16;
                  if (AODVL_cntxt.routingTbl[attrIdx].destAddr == MAC_BROADCAST_SHORT_ADDR)
                      attrVal |= MAC_BROADCAST_SHORT_ADDR;
                  else
                      attrVal |= AODVL_cntxt.routingTbl[attrIdx].nextHop;
              }

#ifdef LPWMN_COORD
              *(UINT32_t *)(buff_p) = attrVal;
#else
              UTIL_htonl(buff_p, attrVal);
#endif

           }
           break;
#endif

      default:
           break;
   }

   return attrValLen;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t NM_getAttrVal(UINT16_t attrId, UINT8_t *buff_p)
{
   UINT8_t attrValLen = 0;

   switch (attrId)
   {
#ifdef DEV_TYPE_RFD
      case APP_MAX_SENSOR_DATA_TX_INTERVAL_ATTR_ID:
           {
        	   __setAttrVal32(RFD_APP_maxSnsrMsgTxInterval);
              attrValLen = 4;
           }
           break;
#endif

#ifdef VEHICLE_DETECTION_USING_HIGH_PASS_FILTER_ENA
      case APP_VEH_DET_MFS_HPF_DETECTION_ALPHA:
           {
              *(buff_p) = VD_HPF_alpha;
              attrValLen = 1;
           }
           break;

      case APP_VEH_DET_MFS_HPF_DETECTION_THRESHOLD:
           __setAttrVal16(VD_HPF_detectionTh);
           break;

      case APP_VEH_DET_MFS_HPF_SETTLING_THRESHOLD:
           __setAttrVal16(VD_HPF_settlingTh);
           break;
#endif

      case SYS_BD_ALLOC_FLR_CNT_ATTR_ID:
          __setAttrVal16(SYS_bdAllocFlrCnt);
          break;

      case SYS_BUFF_ALLOC_FLR_CNT_ATTR_ID:
          __setAttrVal16(SYS_buffAllocFlrCnt);
          break;

      case SYS_MEM_ALLOC_SUCCESS_CNT_ATTR_ID:
          __setAttrVal32(SYS_memAllocDoneCnt);
          break;

      case SYS_MEM_FREE_CNT_ATTR_ID:
          __setAttrVal32(SYS_memFreeCnt);
          break;

      case MAC_ACK_TX_CNT_ATTR_ID:
           __setAttrVal16(MAC_cntxt.macAckTxCnt);
           break;

#ifndef DEV_TYPE_RFD
      case ADP_APP_PYLD_TX_PROC_FLR_CNT_ATTR_ID:
    	   __setAttrVal16(ADP_cntxt.appPyldTxProcFlrCnt);
    	   break;

      case NWK_RT_DISC_STARTED_CNT_ATTR_ID:
           __setAttrVal16(ADP_cntxt.rtDiscStartedCnt);
           break;

      case NWK_RT_DISC_SUCCESS_CNT_ATTR_ID:
           __setAttrVal16(ADP_cntxt.rtDiscSuccessCnt);
           break;

      case NWK_RT_DISC_FLR_CNT_ATTR_ID:
           __setAttrVal16(ADP_cntxt.rtDiscFlrCnt);
           break;

      case NWK_RT_DISC_ERR_CNT_ATTR_ID:
           __setAttrVal16(ADP_cntxt.rtDiscErrCnt);
           break;

      case MAC_BCN_REQ_RCVD_CNT_ATTR_ID:
           __setAttrVal16(MAC_cntxt.bcnReqRcvdCnt);
           break;

      case MAC_ASSOC_REQ_RECEIVED_CNT_ATTR_ID:
           __setAttrVal16(MAC_cntxt.assocReqRcvdCnt);
           break;

      case MAC_ASSOC_RESP_TRX_CNT_ATTR_ID:
	       __setAttrVal16(MAC_cntxt.assocRespTxCnt);
           break;

#ifdef DEV_TYPE_FFD
      case ADP_RELAYED_ASSOC_RESP_RCVD_CNT_ATTR_ID:
           __setAttrVal16(ADP_cntxt.relayedAssocRespRcvdCnt);
           break;

      case MAC_MAX_RFD_SUPPORT_CNT_PER_FFD_ATTR_ID:
	       __setAttrVal16(MAC_MAX_ASSOC_RFD_CNT);
           break;

      case MAC_FFD_ASSOC_RFD_CNT_ATTR_ID:
           __setAttrVal16(MAC_cntxt.assocRFDCnt);
           break;

      case MAC_ASSOC_REQ_RELAYED_CNT_ATTR_ID:
	       __setAttrVal16(ADP_cntxt.assocReqRelayedCnt);
           break;
#endif

      case MAC_TIME_SINCE_LAST_BCN_REQ_RCVD_ATTR_ID:
           __setAttrVal16(MAC_cntxt.timeSinceLastBncReq);
           break;

      case MAC_LAST_BCN_REQ_SRC_EXT_ADDR_ATTR_ID:
           __setAttrVal16(MAC_cntxt.bcnReqSrcExtAddr);
           break;

      case NWK_ROUTING_TABLE_VALID_ENTRY_CNT_ATTR_ID:
           {
               UINT16_t validEntryCnt = AODVL_getValidRtTblEntryCnt();
               __setAttrVal16(validEntryCnt);
           }
           break;

      case NWK_ROUTING_TABLE_MAX_ENTRY_CNT_ATTR_ID:
           {
               __setAttrVal16(AODVL_ROUTING_TBL_SZ);
           }
           break;

      case AODVL_RT_DISCOVERY_TABLE_VALID_ENTRY_CNT_ATTR_ID:
           {
               UINT16_t validEntryCnt = AODVL_getValidRtDiscTblEntryCnt();
               __setAttrVal16(validEntryCnt);
           }
           break;

      case AODVL_RT_DISCOVERY_TABLE_MAX_ENTRY_CNT_ATTR_ID:
           {
               __setAttrVal16(AODVL_RT_DISC_TBL_SZ);
           }
           break;

      case NWK_MESH_LOOP_DETECTED_CNT_ATTR_ID:
           {
        	   __setAttrVal16(ADP_cntxt.meshLoopDetectCnt);
           }
           break;

      case AODVL_RT_DISCOVERY_TABLE_FULL_CNT_ATTR_ID:
           {
   	           __setAttrVal16(AODVL_cntxt.rtDiscTblFullCnt);
           }
           break;
#endif

#ifdef DEV_TYPE_FFD
      case SYS_REBOOT_DELAY_ATTR_ID:
           {
               __setAttrVal16(SYS_rebootDelaySecs);
           }
           break;

      case APP_NO_OP_CMD_RCVD_CNT_ATTR_ID:
           {
              __setAttrVal16(FFD_APP_noOpReqRcvdCnt);
           }
           break;

      case APP_ADP_DATA_REQ_CNT_ATTR_ID:
           {
              __setAttrVal32(FFD_APP_dataTxReqCnt);
           }
           break;

      case APP_ADP_DATA_REQ_FLR_CNT_ATTR_ID:
           {
              __setAttrVal32(FFD_APP_dataRxReqFlrCnt);
           }
           break;

      case APP_SENSOR_DATA_TX_INTERVAL_ATTR_ID:
	       {
    	      __setAttrVal32(APP_txInterval);
	       }
           break;
#endif

      case MAC_ACK_FRAME_RAW_TRX_TIME:
           {
               __setAttrVal16(MAC_cntxt.ackPktTrxTimeMilliSecs);
           }
           break;

#if defined(LPWMN_COORD) || defined(DEV_TYPE_FFD)
      case ADP_MESH_BC_PKT_RCVD_CNT_ATTR_ID:
           {
               __setAttrVal16(ADP_cntxt.meshBroadcastPktRcvdCnt);
           }
           break;

      case ADP_MESH_DUP_BC_PKT_DROPPED_CNT_ATTR_ID:
           {
               __setAttrVal16(ADP_cntxt.meshBroadcastDupPktDroppedCnt);
           }
           break;
#endif

#ifndef LPWMN_COORD
      case APP_DATA_CONFIRM_SUCCESS_CNT_ATTR_ID:
           {
#ifdef DEV_TYPE_FFD
        	   __setAttrVal32(FFD_APP_cnfrmOkCnt);
#elif defined(DEV_TYPE_RFD)
        	   __setAttrVal32(RFD_APP_cnfrmOkCnt);
#endif
           }
           break;

      case APP_DATA_CONFIRM_FAILURE_CNT_ATTR_ID:
           {
#ifdef DEV_TYPE_FFD
        	   __setAttrVal32(FFD_APP_cnfrmFlrCnt);
#elif defined(DEV_TYPE_RFD)
        	   __setAttrVal32(RFD_APP_cnfrmFlrCnt);
#endif
           }
           break;
#endif

#ifdef LPWMN_COORD
      case GW_LPWMN_MSGS_RCVD_CNT_ATTR_ID:
           {
              attrValLen = 4;
              *(buff_p) = GW_cntxt.lpwmnMsgsRcvdCnt;
           }
           break;

      case PHY_RF_CHANN_ID_ATTR_ID:
           {
              attrValLen = 1;
              *(buff_p) = PHY_PIB.phyCurrentChannel;
           }
           break;
#endif

      case MAC_LPWMN_ID_ATTR_ID:
           {
        	   attrValLen = 2;
              __setAttrVal16(MAC_PIB.macLPWMNId);
           }
           break;

#ifdef WISENSE_PIR_SENSOR_NODE
      case APP_PIR_SENSOR_ENA_DIS_CTRL_ATTR_ID:
           {
              attrValLen = 1;
              *buff_p = 0;
              EKMC160111X_getSensorState(PLTFRM_EKMC160111X_1_DEV_ID, buff_p);
           }
           break;
#endif

      case MAC_TX_ON_CCA_ENA_FLAG_ATTR_ID:
           {
              attrValLen = 1;
              *buff_p = MAC_cntxt.txOnCCAEna;
           }
           break;

      case MAC_ACK_TMO_DELTA_ATTR_ID:
           {
              __setAttrVal16(MAC_cntxt.ackTmoExtraMilliSecs);
           }
           break;

      case MAC_BAD_UC_SA_RCVD_CNT_ATTR_ID:
           {
        	   __setAttrVal16(MAC_cntxt.badUCSAVerifCnt);
           }
           break;

      case MAC_GOOD_UC_SA_RCVD_CNT_ATTR_ID:
           {
        	   __setAttrVal16(MAC_cntxt.goodUCSAVerifCnt);
           }
           break;

#ifndef DEV_TYPE_RFD
      case SYS_UP_TIME_IN_SECONDS_ATTR_ID:
           {
              attrValLen = SYS_UP_TIME_IN_SECONDS_ATTR_VAL_LEN;
#ifdef LPWMN_COORD
              *(UINT32_t *)(buff_p) = SYS_upTimeInSecs;
#else
              UTIL_htonl(buff_p, SYS_upTimeInSecs);
#endif
           }
           break;
#endif

      case MAC_CSMA_FLR_CNT_ATTR_ID:
           {
              attrValLen = 4;
#ifdef LPWMN_COORD
              *(UINT32_t *)(buff_p) = MAC_cntxt.csmaFlrCnt;
#else
              UTIL_htonl(buff_p, MAC_cntxt.csmaFlrCnt);
#endif
           }
           break;

      case MAC_CCA_FLR_CNT_ATTR_ID:
           {
              attrValLen = 4;
#ifdef LPWMN_COORD
              *(UINT32_t *)(buff_p) = MAC_cntxt.ccaFlrCnt;
#else
              UTIL_htonl(buff_p, MAC_cntxt.ccaFlrCnt);
#endif
           }
           break;

      case MAC_ACK_TMO_CNT_ATTR_ID:
           {
              attrValLen = 4;
#ifdef LPWMN_COORD
              *(UINT32_t *)(buff_p) = MAC_cntxt.ackTmoCnt;
#else
              UTIL_htonl(buff_p, MAC_cntxt.ackTmoCnt);
#endif
           }
           break;

      case MAC_VALID_ACK_RCVD_CNT_ATTR_ID:
           {
              UINT32_t rslt;

              attrValLen = 4;

#ifdef MAC_STATS_ENA
              rslt = MAC_cntxt.stats.validAckRxCnt;
#else
              rslt = -1;
#endif

#ifdef LPWMN_COORD
              *(UINT32_t *)(buff_p) = rslt;
#else
              UTIL_htonl(buff_p, rslt);
#endif
           }
           break;

      case MAC_LAST_CSMA_BACK_OFF_ATTR_ID:
           {
              UINT32_t rslt;

              attrValLen = 4;

#ifdef MAC_STATS_ENA
              rslt = MAC_cntxt.stats.lastBackOffDelay;
#else
              rslt = -1;
#endif

#ifdef LPWMN_COORD
              *(UINT32_t *)(buff_p) = rslt;
#else
              UTIL_htonl(buff_p, rslt);
#endif
           }
           break;

      case MAC_CCA_OK_CNT_ATTR_ID:
           {
              UINT32_t rslt;

              attrValLen = 4;

#ifdef MAC_STATS_ENA
              rslt = MAC_cntxt.stats.ccaOkCnt;
#else
              rslt = -1;
#endif

#ifdef LPWMN_COORD
              *(UINT32_t *)(buff_p) = rslt;
#else
              UTIL_htonl(buff_p, rslt);
#endif
           }
           break;

#ifndef LPWMN_COORD
      case APP_SENSOR_DATA_PUSH_INTERVAL_SECS_ATTR_ID:
           {
              attrValLen = 4;

              UTIL_htonl(buff_p,
#ifdef DEV_TYPE_RFD
                         RFD_APP_snsrRepInterval
#else
                         FFD_APP_commIntervalInSecs
#endif
                        );
           }
           break;
#endif

      case FW_BUILD_DATE_ATTR_ID:
           {
              attrValLen = strlen(__DATE__) + 1;
              memcpy(buff_p, __DATE__, attrValLen);
           }
           break;

      case FW_BUILD_TIME_ATTR_ID:
           {
              attrValLen = strlen(__TIME__) + 1;
              memcpy(buff_p, __TIME__, attrValLen);
           }
           break;

      case SYS_FREE_MEM_DESC_CNT_ATTR_ID:
           {
              UINT16_t freeCnt = SYS_findFreeDescCnt();

              __setAttrVal16(freeCnt);
           }
           break;

#ifdef UC_FU_ENA

      case FU_IMAGE_STORE_IMAGE_FLAGS_ATTR_ID:
           {
              attrValLen = FU_IMAGE_STORE_IMAGE_FLAGS_ATTR_VAL_LEN;

              FU_getImageStoreFlags(buff_p);

              // First byte shows availability of storage for up to 8 images.
              // second byte contains "valid" bit for up to 8 images.
              // third byte contains "active" bit for up to 8 images.
              // Right now only 2 images can be stored.
           }
           break;

      case FU_IMAGE_STORE_IMAGE_1_IMAGE_RCVD_TIME_STAMP_ATTR_ID:
      case FU_IMAGE_STORE_IMAGE_2_IMAGE_RCVD_TIME_STAMP_ATTR_ID:
           {
              attrValLen = FW_STORE_IMAGE_RCVD_TIME_STAMP_LEN;

              FU_getImageRcvdTS(attrId == FU_IMAGE_STORE_IMAGE_1_IMAGE_RCVD_TIME_STAMP_ATTR_ID ? \
                                FW_STORE_IMAGE_1 :  FW_STORE_IMAGE_2,
                                buff_p);
           }
           break;

      case FU_IMAGE_STORE_IMAGE_1_LEN_ATTR_ID:
      case FU_IMAGE_STORE_IMAGE_2_LEN_ATTR_ID:
           {
              FU_imageInfo_s imageInfo;

              attrValLen = FU_IMAGE_STORE_IMAGE_LEN_ATTR_VAL_LEN;

              FU_getImageInfo(attrId == FU_IMAGE_STORE_IMAGE_1_LEN_ATTR_ID ? \
                              FW_STORE_IMAGE_1 :  FW_STORE_IMAGE_2,
                              &imageInfo);

#ifdef LPWMN_COORD
              *(UINT16_t *)(buff_p) = imageInfo.len;
#else
              UTIL_htons(buff_p, imageInfo.len);
#endif
           }
           break;

      case FU_IMAGE_STORE_IMAGE_1_SEG_CNT_ATTR_ID:
      case FU_IMAGE_STORE_IMAGE_2_SEG_CNT_ATTR_ID:
           {
              FU_imageInfo_s imageInfo;

              attrValLen = FU_IMAGE_STORE_IMAGE_SEG_CNT_ATTR_VAL_LEN;

              FU_getImageInfo(attrId == FU_IMAGE_STORE_IMAGE_1_SEG_CNT_ATTR_ID ? \
                              FW_STORE_IMAGE_1 :  FW_STORE_IMAGE_2,
                                 &imageInfo);

              *buff_p = imageInfo.segCnt;
           }
           break;

      case FU_IMAGE_STORE_IMAGE_1_TYPE_ATTR_ID:
      case FU_IMAGE_STORE_IMAGE_2_TYPE_ATTR_ID:
           {
              FU_imageInfo_s imageInfo;

              attrValLen = FU_IMAGE_STORE_IMAGE_TYPE_ATTR_VAL_LEN;

              FU_getImageInfo(attrId == FU_IMAGE_STORE_IMAGE_1_TYPE_ATTR_ID ? \
                              FW_STORE_IMAGE_1 :  FW_STORE_IMAGE_2,
                                 &imageInfo);

              *buff_p = imageInfo.type;
           }
           break;
#endif

      default:
           break;
   }

   return attrValLen;
}


#ifdef LPWMN_COORD

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
NM_nodeListEntry_s *NM_lookUpShortAddr(MAC_shortAddr_t shortAddr)
{
   UINT16_t idx;
   
   for (idx=0; idx<NM_MAX_SUPPORTED_NODE_CNT; idx++)
   {
       if (NM_cntxt.NM_nodeList[idx].shortAddr == shortAddr)
           return &(NM_cntxt.NM_nodeList[idx]); 
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
void NM_checkReassocTimeOuts(SINT8_t elapsedTime)
{
   UINT16_t idx;
   NM_nodeListEntry_s *entry_p = &(NM_cntxt.NM_nodeList[0]);

   for (idx=0; idx<NM_MAX_SUPPORTED_NODE_CNT; idx++)
   {
        if (entry_p->shortAddr != MAC_BROADCAST_SHORT_ADDR
            && entry_p->reassocTmo > 0)
        {
            entry_p->reassocTmo -= elapsedTime;
        }

        entry_p ++;
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
void NM_updateTrafficTS(UINT8_t interval)
{
   UINT16_t idx;
   NM_nodeListEntry_s *entry_p = &(NM_cntxt.NM_nodeList[0]);

   for (idx=0; idx<NM_MAX_SUPPORTED_NODE_CNT; idx++)
   {
        if (entry_p->shortAddr != MAC_BROADCAST_SHORT_ADDR
            && entry_p->timeSinceLastRx < 65535)
        {
            UINT16_t tmpU16 = entry_p->timeSinceLastRx;
            tmpU16 += interval;
            if (tmpU16 < entry_p->timeSinceLastRx)
                entry_p->timeSinceLastRx = 65535;
            else
                entry_p->timeSinceLastRx = tmpU16;
        }

        entry_p ++;
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
UINT16_t NM_getNodeIdx(MAC_shortAddr_t shortAddr)
{
   UINT16_t idx;

   for (idx=0; idx<NM_MAX_SUPPORTED_NODE_CNT; idx++)
   {
       if (NM_cntxt.NM_nodeList[idx].shortAddr == shortAddr)
           return idx;
   }

   return NM_MAX_SUPPORTED_NODE_CNT;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t NM_lookUpLocalRFD(MAC_shortAddr_t shortAddr)
{
   UINT16_t idx;
   NM_nodeListEntry_s *entry_p = &(NM_cntxt.NM_nodeList[0]);

   for (idx=0; idx<NM_MAX_SUPPORTED_NODE_CNT; idx++)
   {
       if (entry_p->shortAddr == shortAddr
           && ((entry_p->nodeCapInfo & MAC_ASSOC_REQ_CAP_INFO_DEV_TYPE_BIT_SHIFT_MSK) == 0)
           && entry_p->parentNodeIdx == NM_MAX_SUPPORTED_NODE_CNT)
       {
           return 1;
       }
       entry_p ++;
   }

   return 0;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t NM_checkResetKATMO(MAC_shortAddr_t shortAddr)
{
   UINT16_t idx;
   NM_nodeListEntry_s *entry_p = &(NM_cntxt.NM_nodeList[0]);

   for (idx=0; idx<NM_MAX_SUPPORTED_NODE_CNT; idx++)
   {
       if (entry_p->shortAddr == shortAddr
           && ((entry_p->nodeCapInfo & MAC_ASSOC_REQ_CAP_INFO_DEV_TYPE_BIT_SHIFT_MSK) == 0)
           && entry_p->parentNodeIdx == NM_MAX_SUPPORTED_NODE_CNT)
       {
#ifdef RFD_KA_ENA
           entry_p->kaTTE = NM_cntxt.rfdKeepAliveTmoSecs;
#endif
           return 1;
       }
       entry_p ++;
   }

   return 0;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void NM_freeNodeTblEntry(NM_nodeListEntry_s *entry_p)
{
   GW_sendNodeDeRegEvt(entry_p);
   SYS_purgeTrscnsQ(&(entry_p->pendingFramesQ));
   entry_p->shortAddr = MAC_BROADCAST_SHORT_ADDR;
   entry_p->parentNodeIdx = NM_MAX_SUPPORTED_NODE_CNT;
   entry_p->nodeCapInfo = 0x0;
   entry_p->lastSysFEId = 0x0;
#ifdef RFD_KA_ENA
   entry_p->kaTTE = 0x0;
#endif
   memset(entry_p->extAddr, 0, MAC_EXT_ADDR_LEN);
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
void NM_deleteRFDs(UINT16_t nodeTblIdx)
{
   UINT16_t idx;
   NM_nodeListEntry_s *entry_p = &(NM_cntxt.NM_nodeList[0]);

   // Delete all RFDs which share the parent corresponding to "nodeTblIdx"

   for (idx=0; idx<NM_MAX_SUPPORTED_NODE_CNT; idx++)
   {
       if (entry_p->shortAddr != MAC_BROADCAST_SHORT_ADDR
           && ((entry_p->nodeCapInfo & MAC_ASSOC_REQ_CAP_INFO_DEV_TYPE_BIT_SHIFT_MSK) == 0)
           && entry_p->parentNodeIdx == nodeTblIdx)
       {
           UINT8_t flag = 0;

           if (AODVL_lookUpNodeInRtDiscTbl(entry_p->shortAddr))
               flag = 1;

           // Remove node from routing table and route discovery table.
           AODVL_purgeRtTbls(entry_p->shortAddr);

           if (flag)
               ADP_procRtDiscOpnRslt(entry_p->shortAddr, MAC_BROADCAST_SHORT_ADDR);

           NM_freeNodeTblEntry(entry_p);
       }
       entry_p ++;
   }

   return;
}


#ifdef RFD_KA_ENA

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void NM_checkRFDKATmo(void)
{
   if (1)
   {
       UINT16_t idx;
       NM_nodeListEntry_s *entry_p = NM_cntxt.NM_nodeList;

       for (idx=0; idx<NM_MAX_SUPPORTED_NODE_CNT; idx++)
       {
            if (entry_p->shortAddr == MAC_BROADCAST_SHORT_ADDR)
                continue;

            if ((!(entry_p->nodeCapInfo & MAC_ASSOC_REQ_CAP_INFO_DEV_TYPE_BIT_SHIFT_MSK))
                && (entry_p->parentNodeIdx == NM_MAX_SUPPORTED_NODE_CNT))
            {
                if (entry_p->kaTTE == 0x0)
                    SYS_fatal(SYS_FATAL_ERR_875);

                entry_p->kaTTE --;
                if (entry_p->kaTTE == 0x0)
                {
                    NM_freeNodeTblEntry(entry_p);
                }
            }

            entry_p ++;
       }
   }

   return;
}

#endif


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
SYS_trscnQ_s *NM_checkPendingFrame(MAC_shortAddr_t shortAddr)
{
   UINT16_t idx;
   NM_nodeListEntry_s *entry_p = &(NM_cntxt.NM_nodeList[0]);
   SYS_trscnQ_s *trscnQ_p = NULL;

   for (idx=0; idx<NM_MAX_SUPPORTED_NODE_CNT; idx++)
   {
      if (entry_p->shortAddr == shortAddr)
      {
           if (((entry_p->nodeCapInfo & MAC_ASSOC_REQ_CAP_INFO_DEV_TYPE_BIT_SHIFT_MSK) == 0)  // RFD
               && (entry_p->parentNodeIdx == NM_MAX_SUPPORTED_NODE_CNT))  // Coordinator is this node's parent
           {
               if (!(SYS_TRSCN_Q_IS_EMPTY(&(entry_p->pendingFramesQ))))
               {
                   trscnQ_p = &(entry_p->pendingFramesQ);
               }
           }
           break;
       }
       entry_p ++;
   }

   return trscnQ_p;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
SYS_trscnQ_s *NM_getRFDPendTrscnQ(MAC_shortAddr_t shortAddr)
{
   UINT16_t idx;
   NM_nodeListEntry_s *entry_p = &(NM_cntxt.NM_nodeList[0]);
   SYS_trscnQ_s *trscnQ_p = NULL;

   for (idx=0; idx<NM_MAX_SUPPORTED_NODE_CNT; idx++)
   {
       if (entry_p->shortAddr == shortAddr)
       {
           if (((entry_p->nodeCapInfo & MAC_ASSOC_REQ_CAP_INFO_DEV_TYPE_BIT_SHIFT_MSK) == 0x0)
               && (entry_p->parentNodeIdx == NM_MAX_SUPPORTED_NODE_CNT))
           {

               // Node is an RFD and it's parent is this Coord.
               trscnQ_p = &(entry_p->pendingFramesQ);
           }

           break;
       }

       entry_p ++;
   }

   return trscnQ_p;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void NM_purgeRFDPendTrscnQ(NM_nodeListEntry_s *entry_p)
{
   SYS_purgeTrscnsQ(&(entry_p->pendingFramesQ));
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
NM_nodeListEntry_s *NM_lookUpNode(UINT8_t *nodeExdtAddr_p)
{
   UINT16_t idx;
   NM_nodeListEntry_s *entry_p = &(NM_cntxt.NM_nodeList[0]);
   
   for (idx=0; idx<NM_MAX_SUPPORTED_NODE_CNT; idx++)
   {
        if (entry_p->shortAddr != MAC_BROADCAST_SHORT_ADDR)
        {
            if (memcmp(nodeExdtAddr_p, entry_p->extAddr, MAC_EXT_ADDR_LEN) == 0x0)
                return entry_p;
        }

        entry_p ++;
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
NM_nodeListEntry_s *NM_allocNodeTblEntry(void)
{
   UINT16_t idx;
   NM_nodeListEntry_s *entry_p = &(NM_cntxt.NM_nodeList[0]);
    
   for (idx=0; idx<NM_MAX_SUPPORTED_NODE_CNT; idx++)
   {
      if (entry_p->shortAddr == MAC_BROADCAST_SHORT_ADDR)
      {
          entry_p->parentNodeIdx = NM_MAX_SUPPORTED_NODE_CNT;
          entry_p->nodeCapInfo = 0x0;
          entry_p->reassocTmo = 0;  // rkris@wisense.in / sep/23/15

#ifdef RFD_KA_ENA
          NM_cntxt.NM_nodeList[idx].kaTTE = NM_cntxt.rfdKeepAliveTmoSecs;
#endif
          return entry_p;
      }

      entry_p ++;
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
UINT16_t NM_getJoinedNodeCnt(void)
{
   UINT16_t idx;
   UINT16_t nodeCnt = 0;
    
   for (idx=0; idx<NM_MAX_SUPPORTED_NODE_CNT; idx++)
   {
      if (NM_cntxt.NM_nodeList[idx].shortAddr != MAC_BROADCAST_SHORT_ADDR)
      {
          nodeCnt ++;
      }
   }
   
   return nodeCnt;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void NM_updateCfgInfo(UINT8_t tlvId,
                      UINT8_t tlvValLen,
                      UINT8_t *val_p)
{
   UINT8_t tempBd = SYS_allocMem(NVM_NM_CFG_DATA_LEN);

   if (SYS_BD_HNDL_IS_VALID(tempBd))
   {
       PLTFRM_sts_t sts;
       UINT8_t *buff_p = SYS_GET_BD_BUFF(tempBd);

       sts = NVM_readMem(PLTFRM_NVM_DEV_ID,
                         NVM_NM_CFG_DATA_OFFSET,
                         NVM_NM_CFG_DATA_LEN,
                         buff_p);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           UINT8_t tlvLen1, rc, *buff1_p;

           rc = UTIL_getTlv(buff_p, NVM_NM_CFG_DATA_LEN,
                            NM_TLV_TYPE_CFG_DATA,
                            &tlvLen1, &buff1_p);
           if (rc)
           {
               UINT8_t tlvLen2, *buff2_p;

               rc = UTIL_getTlv(buff1_p, tlvLen1,
                                NM_TLV_TYPE_LAST_ALLOC_MAC_SHORT_ADDR,
                                &tlvLen2, &buff2_p);
               if (rc)
               {
                   if (tlvLen2 == tlvValLen)
                   {
                       switch (tlvValLen)
                       {
                          case 4:
                              UTIL_htonl(buff2_p, *((UINT32_t *)val_p));
                              break;

                          case 2:
                              UTIL_htons(buff2_p, *((UINT16_t *)val_p));
                              break;

                          default:
                              memcpy(buff2_p, val_p, tlvValLen);
                              break;
                       }

                       sts = NVM_writeMem(PLTFRM_NVM_DEV_ID,
                                          NVM_NM_CFG_DATA_OFFSET + (buff2_p - buff_p),
                                          tlvValLen, buff2_p);
                       if (sts != PLTFRM_STS_SUCCESS)
                           SYS_fatal(SYS_FATAL_ERR_908);
                   }
               }
           }
       }

       SYS_freeMem(tempBd);
   }
   else
   {
      SYS_fatal(SYS_FATAL_ERR_909);
   }
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
MAC_shortAddr_t NM_allocShortAddr(void)
{
  MAC_shortAddr_t shortAddr = NM_cntxt.NM_nextAllocShortAddr;

  if (shortAddr >= MAC_MAX_UNICAST_SHORT_ADDR)
  {
      UINT16_t idx, shortAddr = MAC_BROADCAST_SHORT_ADDR;

      // All unicast addresses allocated once ... restart the network
      NM_updateCfgInfo(NM_TLV_TYPE_LAST_ALLOC_MAC_SHORT_ADDR,
                       MAC_SHORT_ADDR_LEN,
                       (UINT8_t *)&shortAddr);

      for (idx=0; idx<100; idx++)
           PLTFRM_delay1MilliSec();

      // The coordinator will reboot now
      PLTFRM_mcuReset();
  }

  // Short address 0x0 is not allocated to any node
  // Short address 0x1 is assigned to the LPWMN coordinator
  // Short address 0xffff is also not assigned to any node
  
  NM_cntxt.NM_nextAllocShortAddr ++;
  if (NM_cntxt.NM_nextAllocShortAddr > MAC_MAX_UNICAST_SHORT_ADDR)
      NM_cntxt.NM_nextAllocShortAddr = LPWMN_COORD_SHORT_ADDR + 1;

  NM_updateCfgInfo(NM_TLV_TYPE_LAST_ALLOC_MAC_SHORT_ADDR,
                   MAC_SHORT_ADDR_LEN,
                   (UINT8_t *)&shortAddr);

  // <TODO> Make sure address is not in use by any node !!
    
  return shortAddr;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void NM_evtHndlr(UINT16_t globalEvtId, void *params_p)
{

}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void NM_readCfgInfo(void)
{
   UINT8_t tempBd = SYS_allocMem(NVM_NM_CFG_DATA_LEN);

   if (SYS_BD_HNDL_IS_VALID(tempBd))
   {
       PLTFRM_sts_t sts;
       UINT8_t *buff_p = SYS_GET_BD_BUFF(tempBd);

       sts = NVM_readMem(PLTFRM_NVM_DEV_ID,
                         NVM_NM_CFG_DATA_OFFSET,
                         NVM_NM_CFG_DATA_LEN,
                         buff_p);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           UINT8_t tlvLen1, rc, *buff1_p;

           rc = UTIL_getTlv(buff_p, NVM_NM_CFG_DATA_LEN, NM_TLV_TYPE_CFG_DATA, &tlvLen1, &buff1_p);
           if (rc && (tlvLen1 > 0) && (tlvLen1 <= (NVM_NM_CFG_DATA_LEN - NM_TLV_HDR_SZ)))
           {
               UINT8_t tlvLen2, *buff2_p;

               rc = UTIL_getTlv(buff1_p, tlvLen1, NM_TLV_TYPE_LAST_ALLOC_MAC_SHORT_ADDR, &tlvLen2, &buff2_p);
               if (rc)
               {
                   if (tlvLen2 == MAC_SHORT_ADDR_LEN)
                   {
                       NM_cntxt.NM_nextAllocShortAddr = UTIL_ntohs(buff2_p);
                       if (NM_cntxt.NM_nextAllocShortAddr > MAC_MAX_UNICAST_SHORT_ADDR)
                           NM_cntxt.NM_nextAllocShortAddr = LPWMN_COORD_SHORT_ADDR + 1;
                       else
                       {
                          NM_cntxt.NM_nextAllocShortAddr += 5;
                          if (NM_cntxt.NM_nextAllocShortAddr > MAC_MAX_UNICAST_SHORT_ADDR)
                              NM_cntxt.NM_nextAllocShortAddr = LPWMN_COORD_SHORT_ADDR + 1;
                       }
                   }
               }
           }
           else
           {
               UINT8_t *buff1_p = buff_p;

               // Initialize the EEPROM ...
               *(buff1_p ++) = NM_TLV_TYPE_CFG_DATA;
               *(buff1_p ++) = NM_TLV_HDR_SZ + MAC_SHORT_ADDR_LEN;
               *(buff1_p ++) = NM_TLV_TYPE_LAST_ALLOC_MAC_SHORT_ADDR;
               *(buff1_p ++) = MAC_SHORT_ADDR_LEN;
               buff1_p = UTIL_htons(buff1_p, MAC_BROADCAST_SHORT_ADDR);

               sts = NVM_writeMem(PLTFRM_NVM_DEV_ID,
                                  NVM_NM_CFG_DATA_OFFSET,
                                  buff1_p - buff_p,
                                  buff_p);
               if (sts != PLTFRM_STS_SUCCESS)
                   SYS_fatal(SYS_FATAL_ERR_906);
           }
       }

       SYS_freeMem(tempBd);
   }
   else
   {
      SYS_fatal(SYS_FATAL_ERR_907);
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
void NM_bcNwkRestart(void)
{
   UINT16_t idx;

   // Broadcast a network restart message couple of times
   for (idx=0; idx<3; idx++)
   {
       UINT16_t t;

       MAC_buildSendNwkRestartCmd();

       for (t=0; t<2000; t++)
            PLTFRM_delay1MilliSec();
   }
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t NM_startCoord(void)
{
   UINT8_t rc = 0;

   if (NM_cntxt.coordStarted == 0)
   {
       NM_bcNwkRestart();

       RADIO_flushRx();

       RADIO_hardRxStart();

#ifdef RADIO_CC2520
       RADIO_setCurrState(RADIO_STATE_RX_ON);
#endif
       NM_cntxt.coordStarted = 1;
       rc = 1;
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
void NM_start(void)
{
   UINT16_t idx;
   
   NM_cntxt.coordStarted = 0;

#ifdef RFD_KA_ENA
   NM_cntxt.rfdKeepAliveTmoSecs = RFD_DFLT_KA_TMO_SECS;
#endif

   NM_cntxt.NM_nextAllocShortAddr = LPWMN_COORD_SHORT_ADDR + 1;
   NM_readCfgInfo();

   for (idx=0; idx<NM_MAX_SUPPORTED_NODE_CNT; idx++)
   {
       NM_cntxt.NM_nodeList[idx].shortAddr = MAC_BROADCAST_SHORT_ADDR;
       SYS_initTrscnsQ(&(NM_cntxt.NM_nodeList[idx].pendingFramesQ));
   }
   
#ifdef EXT_EEPROM_EN
   if (PLTFRM_getMacExtAddr(MAC_PIB.macExtendedAddress) != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_62);
   }
#else
   MAC_PIB.macExtendedAddress[0] = 0xde;
   MAC_PIB.macExtendedAddress[1] = 0xad;
   MAC_PIB.macExtendedAddress[2] = 0xbe;
   MAC_PIB.macExtendedAddress[3] = 0xef;
   MAC_PIB.macExtendedAddress[4] = 0xfe;
   MAC_PIB.macExtendedAddress[5] = 0xed;
   MAC_PIB.macExtendedAddress[6] = 0xda;
   MAC_PIB.macExtendedAddress[7] = 0xdd;   
#endif
   
#ifdef RADIO_CC2520
   RADIO_cfgShortAddrFilter(MAC_PIB.macShortAddress);
   RADIO_cfgLPWMNIdFilter(MAC_PIB.macLPWMNId);
#endif
   
   RADIO_setChan(PHY_PIB.phyCurrentChannel);
   RADIO_setTxPower(PHY_PIB.phyTXPower, PHY_PIB.phyTXPowerDecPart);

#ifdef COORD_IDLE_UNTIL_TRIGGER
   if (RADIO_hardStop() != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_63);
#else
   NM_bcNwkRestart();

   RADIO_flushRx();

   RADIO_hardRxStart(); 
#ifdef RADIO_CC2520
   RADIO_setCurrState(RADIO_STATE_RX_ON);
#endif
   NM_cntxt.coordStarted = 1;
#endif

   return;
}

#else

#ifdef RFD_NO_RESCAN_ON_REASSOC
void NM_checkJoinFlrThreshold(void)
{
#ifdef RADIO_CC1101
   // rkris@wisense.in / Sep/7/2015
   // If join fails "NM_MAX_JOIN_FLR_CNT" consecutive times, reset the
   // scanChannelsMsk to include all channels in the band.
   if (NM_cntxt.joinFlrCnt > NM_MAX_JOIN_FLR_CNT)
   {
       NM_scanReqParams.scanChannelsMsk  = PHY_865_867_MHZ_BAND_CHANNEL_MSK;
       PHY_PIB.workingChannel = PHY_865_867_MHZ_BAND_CHAN_INV;
       NM_cntxt.joinFlrCnt = 0;
   }
#else
#error Specfied radio not supported !!
#endif
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
void NM_procPendScanReq(void)
{
#ifdef RFD_NO_RESCAN_ON_REASSOC
   NM_checkJoinFlrThreshold();
#endif

   MLME_scanRequest(&NM_scanReqParams);
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void NM_timerExpHndlr(void)
{
   if (MAC_PIB.macShortAddress == MAC_BROADCAST_SHORT_ADDR)
   {
       // rkris@wisense.in - march/1/15
       // Cannot call MLME_scanRequest() directly in case prior assoc-req
       // is still running (beacuse of busy channel at low data rates such
       // as 1200 bps)

       SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_NODE_SCAN_REQ;
   }
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
NM_assocCntxt_s *NM_getAssocCntxt(void)
{
   UINT8_t *b_p = SYS_GET_BD_BUFF(NM_cntxt.assocListBdHndl);

   if (((UINT16_t)b_p) & 0x1)
       b_p ++;

   return (NM_assocCntxt_s *)b_p;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void NM_assocTmoHndlr(void)
{
   // This function is called usually when there is no response to
   // ASSOC.REQ message sent to the selected parent.

#ifdef NM_STATS_ENA
   NM_cntxt.stats.assocTmoCnt ++;
#endif

   if (MAC_PIB.macShortAddress == MAC_BROADCAST_SHORT_ADDR)
   {
       // rkris@wisense.in - march/1/15
       // Cannot call MLME_scanRequest() directly in case prior assoc-req
       // is still running (beacuse of busy channel at low data rates such
       // as 1200 bps)

#ifdef RFD_NO_RESCAN_ON_REASSOC
       NM_checkJoinFlrThreshold();
#endif

       // rkris@wisense.in / oct/14/15 - assoc black list
       // Record association failure through this node.

       if (!(SYS_BD_HNDL_IS_VALID(NM_cntxt.assocListBdHndl)))
       {
           SYS_fatal(SYS_FATAL_ERR_3201);
       }
       else
       {
           UINT8_t idx;
           NM_assocCntxt_s *assocCntxt_p = NM_getAssocCntxt();
           NM_assocListEntry_s *entry_p = assocCntxt_p->assocList;

           if (assocCntxt_p->assocListEntryCnt > NM_ASSOC_LIST_SZ)
           {
               SYS_fatal(SYS_FATAL_ERR_3202);
           }

           for (idx=0; idx<assocCntxt_p->assocListEntryCnt; idx++)
           {
                if (entry_p->lpwmnId == assocCntxt_p->currAssocLPWMNId
                    && entry_p->shortAddr == assocCntxt_p->currAssocParentShortAddr)
                {
                    entry_p->attemptCnt ++;
                    break;
                }
                else
                    entry_p ++;
           }

           if (idx >= assocCntxt_p->assocListEntryCnt)
           {
               // Node not already in list ..

               if (idx < NM_ASSOC_LIST_SZ)
               {
                   entry_p = assocCntxt_p->assocList + idx;
                   entry_p->lpwmnId = assocCntxt_p->currAssocLPWMNId;
                   entry_p->shortAddr = assocCntxt_p->currAssocParentShortAddr;
                   entry_p->attemptCnt = 1;
                   assocCntxt_p->assocListEntryCnt ++;
               }
               else
               {
                   // List is full !! - cannot add this node !!
               }
           }
       }

       NM_startScan();
   }  
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */  
UINT8_t NM_checkNodeBlackListed(UINT16_t lpwmnId, UINT16_t shortAddr)
{
   UINT8_t rc = 0;

   if (SYS_BD_HNDL_IS_VALID(NM_cntxt.assocListBdHndl))
   {
       UINT8_t idx;
       NM_assocCntxt_s *assocCntxt_p = NM_getAssocCntxt();
       NM_assocListEntry_s *entry_p = assocCntxt_p->assocList;

       if (assocCntxt_p->assocListEntryCnt > NM_ASSOC_LIST_SZ)
       {
           SYS_fatal(SYS_FATAL_ERR_3205);
       }

       for (idx=0; idx<assocCntxt_p->assocListEntryCnt; idx++)
       {
            if (lpwmnId == entry_p->lpwmnId
                && shortAddr == entry_p->shortAddr)
            {
                if (entry_p->attemptCnt >= NM_MAX_ASSOC_ATTEMPTS_ALLOWED)
                    rc = 1;
                break;
            }
       }
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
void NM_startAssocProc(MAC_lpwmnDescriptor_s *selLPWMNDesc_p)
{
   MLME_assocRequestParams_s assocParams;
   UINT8_t capInfo = 0;
   NM_assocCntxt_s *assocCntxt_p = NULL;
   
   MAC_PIB.macLPWMNId = selLPWMNDesc_p->coordLPWMNId;

#ifdef RADIO_CC2520
   RADIO_cfgLPWMNIdFilter(MAC_PIB.macLPWMNId);
#endif
   
   assocParams.channelNumber = selLPWMNDesc_p->channelNumber;
   assocParams.coordLPWMNId = selLPWMNDesc_p->coordLPWMNId;
   assocParams.coordShortAddress = selLPWMNDesc_p->coordShortAddress;

   // rkris@wisense.in / oct/14/15 - assoc black list
   if (!(SYS_BD_HNDL_IS_VALID(NM_cntxt.assocListBdHndl)))
   {
       NM_cntxt.assocListBdHndl = SYS_allocMem(sizeof(NM_assocCntxt_s) + 1);
       if (!(SYS_BD_HNDL_IS_VALID(NM_cntxt.assocListBdHndl)))
       {
           SYS_fatal(SYS_FATAL_ERR_3200);
       }
       else
       {
           assocCntxt_p = NM_getAssocCntxt();
           memset(assocCntxt_p, 0, sizeof(NM_assocCntxt_s));
       }
   }
   else
       assocCntxt_p = NM_getAssocCntxt();

   assocCntxt_p->currAssocLPWMNId = selLPWMNDesc_p->coordLPWMNId;
   assocCntxt_p->currAssocParentShortAddr = selLPWMNDesc_p->coordShortAddress;

   /*
    * The Device Type field shall be set to one if the device is an FFD. Otherwise, 
    * the Device Type field shall be set to zero to indicate an RFD.
    */
#ifndef DEV_TYPE_RFD
   capInfo |= MAC_ASSOC_REQ_CAP_INFO_DEV_TYPE_BIT_SHIFT_MSK;
#endif   

   /*
    * The Power Source field shall be set to one if the device is receiving power 
    * from the alternating current mains. Otherwise, the Power Source field shall be 
    * set to zero.
    */   
#ifndef DEV_TYPE_RFD
   capInfo |= MAC_ASSOC_REQ_CAP_INFO_POWER_SOURCE_BIT_SHIFT_MSK;
#endif
    
   /*
    * The Receiver On When Idle field shall be set to one if the device does not 
    * disable its receiver to conserve power during idle periods. Otherwise, the 
    * "Receiver On When Idle" field shall be set to zero.
    */
#ifndef DEV_TYPE_RFD
   capInfo |= MAC_ASSOC_REQ_CAP_INFO_RX_ON_IDLE_BIT_SHIFT_MSK;
#endif

   /*
    * The Security Capability field shall be set to one if the device is capable 
    * of sending and receiving cryptographically protected MAC frames as specified 
    * in 7.2; it shall be set to zero otherwise.
    */
#ifdef MAC_SEC_ENA
   capInfo |= MAC_ASSOC_REQ_CAP_INFO_SEC_CAP_BIT_SHIFT_MSK;
#endif   
   
   capInfo |= MAC_ASSOC_REQ_CAP_INFO_ALLOC_ADDR_BIT_SHIFT_MSK;
   
   assocParams.CapabilityInformation = capInfo;
      
   MLME_assocRequest(&assocParams);
}


#ifdef DEV_TYPE_RFD
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void NM_startCoordPoll(void)
{
   /*
    * Poll the coordinator. On an RFD, this is done after app has requested
    * that the node be put to deep sleep.
    */
#ifdef NM_STATS_ENA
   NM_cntxt.stats.coordPollStartCnt ++;
#endif

   SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_COORD_POLL_IN_PROGRESS;
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
void NM_evtHndlr(UINT16_t globalEvtId, void *params_p)
{
   switch (globalEvtId)
   {                   
       case SYS_GLOBAL_EVT_MAC_ASSOC_CONFIRM:
            {
                MLME_assocCnfrmParams_s *cnfrmParams_p = (MLME_assocCnfrmParams_s *)params_p;
                
#ifdef NM_STATS_ENA
                NM_cntxt.stats.assocCnfrmCnt ++;
#endif

                if (cnfrmParams_p->status == MAC_STS_SUCCESS)                    
                {
#ifdef NM_STATS_ENA                  
                    NM_cntxt.stats.assocDoneCnt ++;
#endif                    
                    PLTFRM_stopTimerA1();

                    if (!(SYS_BD_HNDL_IS_VALID(NM_cntxt.assocListBdHndl)))
                    {
                        SYS_fatal(SYS_FATAL_ERR_3203);
                    }
                    else
                    {
                        SYS_freeMem(NM_cntxt.assocListBdHndl);
                        NM_cntxt.assocListBdHndl = SYS_INV_BD_HDNL;
                    }

#ifdef RFD_NO_RESCAN_ON_REASSOC
                    NM_cntxt.joinFlrCnt = 0;
#endif

#ifdef RFD_NO_RESCAN_ON_REASSOC
#ifdef RADIO_CC1101
                    if (PHY_PIB.workingChannel == PHY_865_867_MHZ_BAND_CHAN_INV)
                        PHY_PIB.workingChannel = PHY_PIB.phyCurrentChannel;
                    else
                    {
                        if (PHY_PIB.workingChannel != PHY_PIB.phyCurrentChannel)
                        {
                            SYS_fatal(SYS_FATAL_ERR_824);
                        }
                    }
#elif defined(RADIO_CC2500)
                    if (PHY_PIB.workingChannel == PHY_2400_MHZ_BAND_CHAN_INV)
                        PHY_PIB.workingChannel = PHY_PIB.phyCurrentChannel;
                    else
                    {
                        if (PHY_PIB.workingChannel != PHY_PIB.phyCurrentChannel)
                        {
                            SYS_fatal(SYS_FATAL_ERR_824);
                        }
                    }
#else
#error radio not specified !!
#endif
#endif

#ifndef NODE_HOST_IF_ENA
                    APP_nodeJoinDoneInd( );
#endif
                }
                else
                {

                    // Association failed !!
#ifdef RFD_NO_RESCAN_ON_REASSOC
                    NM_cntxt.joinFlrCnt ++;
#endif
                }
            }
            break;
     
       case SYS_GLOBAL_EVT_MAC_SCAN_CONFIRM:
            {
               // UINT8_t maxLQI = 0x0;
               SINT16_t maxRSSI = -129;
               MLME_scanCnfrmParams_s *cnfrmParams_p = (MLME_scanCnfrmParams_s *)params_p;
               MAC_lpwmnDescriptor_s *selLPWMNDesc_p = NULL;
               
#ifdef NM_STATS_ENA
               NM_cntxt.stats.scanCnfrmCnt ++;
#endif                          
               if (cnfrmParams_p->status == MAC_STS_SUCCESS)
               {
                   UINT8_t idx;
                   MAC_lpwmnDescriptor_s *lpwmnDesc_p = cnfrmParams_p->lpwmnDescriptorList_p;

#ifdef NM_STATS_ENA
                   NM_cntxt.stats.scanOkCnt ++;
#endif
                   // rkris@wisense.in  / mar/12/16 - recovery from continuous scan failures.
                   // See comment in node_mgr.h.
                   NM_cntxt.scanFlrCnt = 0;

                   for (idx=0; idx<cnfrmParams_p->resultListSize; idx++, lpwmnDesc_p++)
                   {
                        // rkris@wisense.in - aug/8/15
                        // If this node has been configured to join a specific LPWMN id,
                        // only join specified LPWMN id.
                        if (MAC_cntxt.preConfLPWMNId != MAC_BROADCAST_LPWMN_ID
                            && MAC_cntxt.preConfLPWMNId != lpwmnDesc_p->coordLPWMNId)
                        {
                            continue;
                        }

                        // Make sure this node has not been blacklisted ...
                        if (NM_checkNodeBlackListed(lpwmnDesc_p->coordLPWMNId, lpwmnDesc_p->coordShortAddress))
                        {
                            continue;
                        }

                        if (lpwmnDesc_p->rssi > maxRSSI)
                        {
                            // Bug fix (17/dec/14) - maxRSSI was not being updated.
                            maxRSSI = lpwmnDesc_p->rssi;
                            selLPWMNDesc_p = lpwmnDesc_p;
                        }
                   }
               }
#ifdef NM_STATS_ENA
               else
                   NM_cntxt.stats.scanFlrCnt ++;
#endif
               if (selLPWMNDesc_p != NULL)
               {
                   NM_startAssocProc(selLPWMNDesc_p);

                   // Timeout in seconds
                   PLTFRM_startTimerA1(NM_ASSOC_TMO_SECS, 0, NM_assocTmoHndlr);
               }   
               else
               { 
                   if (cnfrmParams_p->resultListSize > 0)
                   {
                       // Scan returned all black listed or disallowed nodes !!
                       SYS_fatal(SYS_FATAL_ERR_3204);
                   }


                   // What to do if scan fails ?
#ifdef DEV_TYPE_RFD
                   // On an RFD, put the system into LPM before waking up to re-scan
                   SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ENTER_LPM_REQUEST;
#endif

#ifdef RFD_NO_RESCAN_ON_REASSOC
                   NM_cntxt.joinFlrCnt ++;
#endif

                   // rkris@wisense.in  / mar/12/16 - recovery from continuous scan failures.
                   // See comment in node_mgr.h.
                   NM_cntxt.scanFlrCnt ++;
                   if (NM_cntxt.scanFlrCnt >=  NM_MAX_SCAN_FLR_CNT_TO_RESET)
                   {
                       SYS_fatal(SYS_FATAL_ERR_3206);
                   }

                   PLTFRM_startTimerA1(NM_RFD_DFLT_INTER_SCAN_INTERVAL_SECS, 0, NM_timerExpHndlr);
               }
            }
            break;
         
       default:
            {
            }
            break;          
   }
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void NM_startScan()
{
   // Bug fix - rkris@wisense.in - march/28/15
   MAC_PIB.macCoordShortAddress = MAC_BROADCAST_SHORT_ADDR;
   MAC_PIB.macShortAddress = MAC_BROADCAST_SHORT_ADDR;
   MAC_PIB.macLPWMNId = MAC_BROADCAST_LPWMN_ID;


   // rkris@wisense.in  / mar/12/16 - recovery from continuous scan failures.
   // See comment in node_mgr.h.
   NM_cntxt.scanFlrCnt = 0;

#ifdef RADIO_CC2520
   NM_scanReqParams.scanChannelsMsk = 0x80;
#elif defined(RADIO_CC1200)
   NM_scanReqParams.scanChannelsMsk = 0x1;
#elif defined(RADIO_CC1101)
   {
      UINT16_t scanMsk = PHY_865_867_MHZ_BAND_CHANNEL_MSK;
      // rkris@wisense.in / april/18/2015
      // If RFD_RESCAN_ON_REASSOC is enabled, RFD will only scan the last
      // "working" channel

#ifdef RFD_NO_RESCAN_ON_REASSOC
      if (PHY_PIB.workingChannel != PHY_865_867_MHZ_BAND_CHAN_INV)
      {
          scanMsk = 1;
          scanMsk <<= PHY_PIB.workingChannel;
      }
#endif

      NM_scanReqParams.scanChannelsMsk = scanMsk;
   }
#elif defined(RADIO_CC2500)
   NM_scanReqParams.scanChannelsMsk = PHY_2400_MHZ_BAND_CHANNEL_MSK;
#else
#error radio not specified !!
#endif

   NM_scanReqParams.scanDurationSecs = 1;

   // Call indirectly (march/1/15 - rkris@wisense.in)
   SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_NODE_SCAN_REQ;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */ 
void NM_start(void)
{
#ifdef RFD_KA_ENA
  NM_cntxt.rfdKeepAliveTmoSecs = RFD_DFLT_KA_TMO_SECS;  // Will get nwk wide keep alive time from beacon
#endif

  NM_cntxt.assocListBdHndl = SYS_INV_BD_HDNL;

  if (PLTFRM_getMacExtAddr(MAC_PIB.macExtendedAddress) != PLTFRM_STS_SUCCESS)
      SYS_fatal(SYS_FATAL_ERR_64);

#ifdef RADIO_CC2520
  RADIO_cfgExtAddrFilter(MAC_PIB.macExtendedAddress);
#endif

#ifdef RFD_NO_RESCAN_ON_REASSOC
  NM_cntxt.joinFlrCnt = 0;
#endif

  // Issue #25
  // Fixed on nov 16, 2013 by rkris
  RADIO_setTxPower(PHY_PIB.phyTXPower, PHY_PIB.phyTXPowerDecPart);

#ifdef STATIC_ASSOC_ENA
  MLME_staticAssoc();
#else
  NM_startScan();
#endif
}

#endif




#else
/*
 ********************************************************************
 *
 * SNIFFER
 *
 *
 ********************************************************************
 */
void NM_start(void)
{
  RADIO_setChan(PHY_PIB.phyCurrentChannel);

#ifdef CC1101_WOR_ENA
  // if (CC1101_enterLPM() != PLTFRM_STS_SUCCESS)
  //    SYS_fatal(SYS_FATAL_ERR_800);

  if (CC1101_enterWOR() != PLTFRM_STS_SUCCESS)
      SYS_fatal(SYS_FATAL_ERR_800);
#else
  RADIO_hardRxStart();
#endif
}

#endif
