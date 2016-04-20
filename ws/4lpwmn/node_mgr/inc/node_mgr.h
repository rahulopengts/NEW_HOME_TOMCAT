/*
 * File Name: node_mgr.h
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Sep/3/2013
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

#ifndef __NODE_MGR_H__
#define __NODE_MGR_H__

#include <typedefs.h>
#include <mesh.h>
#include <mac.h>


#define NM_MAX_SCAN_FLR_CNT_TO_RESET      10    // Back to back failed scans

#define NM_MAX_JOIN_FLR_CNT  3

#define NM_RFD_DFLT_INTER_SCAN_INTERVAL_SECS  5

#define NM_TLV_TYPE_CFG_DATA  0xc0
#define NM_TLV_TYPE_LAST_ALLOC_MAC_SHORT_ADDR  0xc1

#define NM_TLV_TYPE_FIELD_SZ  1
#define NM_TLV_LEN_FIELD_SZ  1
#define NM_TLV_HDR_SZ (NM_TLV_TYPE_FIELD_SZ + NM_TLV_LEN_FIELD_SZ)

#define RFD_DFLT_KA_TMO_SECS  30

#define NM_ATTR_ID_LEN  2

#define NM_MAX_ATTR_VAL_LEN  4

#define SYS_UP_TIME_IN_SECONDS_ATTR_VAL_LEN  4

#define NM_REASSOC_TMO_CHECK_INTERVAL_SECS  5

#define NM_ASSOC_BLACK_LIST_SZ   8

// Assoc black list support
#define NM_MAX_ASSOC_ATTEMPTS_ALLOWED   3


typedef enum
{
    MAC_ACK_TMO_DELTA_ATTR_ID = 1,
    MAC_CCA_FLR_CNT_ATTR_ID = 2,
    MAC_CSMA_FLR_CNT_ATTR_ID = 3,
    MAC_ACK_TMO_CNT_ATTR_ID = 4,
    MAC_CCA_OK_CNT_ATTR_ID = 5,
    MAC_VALID_ACK_RCVD_CNT_ATTR_ID = 6,
    MAC_LAST_CSMA_BACK_OFF_ATTR_ID = 7,
    MAC_TX_ON_CCA_ENA_FLAG_ATTR_ID = 8,     // Enable / Disable TX_ON_CCA on a node
    MAC_LPWMN_ID_ATTR_ID = 9,
    PHY_RF_CHANN_ID_ATTR_ID = 10,
    APP_SENSOR_DATA_PUSH_INTERVAL_SECS_ATTR_ID = 11,
    FW_BUILD_DATE_ATTR_ID = 12,
    FW_BUILD_TIME_ATTR_ID = 13,
    SYS_FREE_MEM_DESC_CNT_ATTR_ID = 14,
    MAC_ACK_FRAME_RAW_TRX_TIME = 15,

    FU_IMAGE_STORE_IMAGE_FLAGS_ATTR_ID = 16,
    FU_IMAGE_STORE_IMAGE_INFO_ATTR_ID = 17,

    FU_IMAGE_STORE_IMAGE_1_LEN_ATTR_ID = 20,
    FU_IMAGE_STORE_IMAGE_1_SEG_CNT_ATTR_ID = 21,
    FU_IMAGE_STORE_IMAGE_1_TYPE_ATTR_ID = 22,
    FU_IMAGE_STORE_IMAGE_1_IMAGE_RCVD_TIME_STAMP_ATTR_ID = 23,

    FU_IMAGE_STORE_IMAGE_2_LEN_ATTR_ID = 40,
    FU_IMAGE_STORE_IMAGE_2_SEG_CNT_ATTR_ID = 41,
    FU_IMAGE_STORE_IMAGE_2_TYPE_ATTR_ID = 42,
    FU_IMAGE_STORE_IMAGE_2_IMAGE_RCVD_TIME_STAMP_ATTR_ID = 43,

    SYS_UP_TIME_IN_SECONDS_ATTR_ID = 50,

	MAC_BAD_UC_SA_RCVD_CNT_ATTR_ID = 55,
	MAC_GOOD_UC_SA_RCVD_CNT_ATTR_ID = 56,

    NWK_ROUTING_TABLE_MAX_ENTRY_CNT_ATTR_ID = 60,
    NWK_ROUTING_TABLE_VALID_ENTRY_CNT_ATTR_ID = 61,

    AODVL_RT_DISCOVERY_TABLE_MAX_ENTRY_CNT_ATTR_ID = 62,
    AODVL_RT_DISCOVERY_TABLE_VALID_ENTRY_CNT_ATTR_ID = 63,

    NWK_ROUTING_TABLE_ENTRY_ATTR_ID = 100,
	AODVL_RT_DISCOVERY_TABLE_ENTRY_ATTR_ID = 101,

	AODVL_RT_DISCOVERY_TABLE_FULL_CNT_ATTR_ID = 102,

    NWK_RT_DISC_STARTED_CNT_ATTR_ID = 120,
    NWK_RT_DISC_SUCCESS_CNT_ATTR_ID = 121,
    NWK_RT_DISC_FLR_CNT_ATTR_ID = 122,
    NWK_RT_DISC_ERR_CNT_ATTR_ID = 123,

    MAC_BCN_REQ_RCVD_CNT_ATTR_ID = 124,
    MAC_TIME_SINCE_LAST_BCN_REQ_RCVD_ATTR_ID = 125,
    MAC_LAST_BCN_REQ_SRC_EXT_ADDR_ATTR_ID = 126,

    MAC_ACK_TX_CNT_ATTR_ID = 200,

    SYS_MEM_ALLOC_SUCCESS_CNT_ATTR_ID = 300,
    SYS_BD_ALLOC_FLR_CNT_ATTR_ID = 301,
    SYS_BUFF_ALLOC_FLR_CNT_ATTR_ID = 302,
    SYS_MEM_FREE_CNT_ATTR_ID = 303,
    SYS_REBOOT_DELAY_ATTR_ID = 304,

    ADP_MESH_BC_PKT_RCVD_CNT_ATTR_ID = 400,
    ADP_MESH_DUP_BC_PKT_DROPPED_CNT_ATTR_ID = 401,
	NWK_MESH_LOOP_DETECTED_CNT_ATTR_ID = 402,
	ADP_APP_PYLD_TX_PROC_FLR_CNT_ATTR_ID = 403,
    ADP_RELAYED_ASSOC_RESP_RCVD_CNT_ATTR_ID =  404,

	MAC_ASSOC_REQ_RECEIVED_CNT_ATTR_ID = 410,
	MAC_ASSOC_RESP_TRX_CNT_ATTR_ID = 411,
	MAC_ASSOC_REQ_RELAYED_CNT_ATTR_ID = 412,
	MAC_MAX_RFD_SUPPORT_CNT_PER_FFD_ATTR_ID = 415,
	MAC_FFD_ASSOC_RFD_CNT_ATTR_ID = 416,

	MAC_FFD_ASSOC_RFD_TABLE_ENTRY_ATTR_ID = 417,


    APP_NO_OP_CMD_RCVD_CNT_ATTR_ID = 500,

	APP_DATA_CONFIRM_SUCCESS_CNT_ATTR_ID = 510,
	APP_DATA_CONFIRM_FAILURE_CNT_ATTR_ID = 511,

	APP_ADP_DATA_REQ_CNT_ATTR_ID = 512,
	APP_ADP_DATA_REQ_FLR_CNT_ATTR_ID =  513,

	APP_SENSOR_DATA_TX_INTERVAL_ATTR_ID = 514,

	APP_MAX_SENSOR_DATA_TX_INTERVAL_ATTR_ID =  515,

    MESH_TRIGGER_PATH_DISC_ATTR_ID = 600,

	NWK_TRIGGER_RT_TABLE_PURGE_ATTR_ID = 601,

	APP_PIR_SENSOR_ENA_DIS_CTRL_ATTR_ID = 700,  // read/write

	APP_VEH_DET_MFS_HPF_DETECTION_THRESHOLD = 750,  // read/write
	APP_VEH_DET_MFS_HPF_SETTLING_THRESHOLD = 751,
	APP_VEH_DET_MFS_HPF_DETECTION_ALPHA = 752,

	GW_LPWMN_MSGS_RCVD_CNT_ATTR_ID = 800,   // Coord  only

} NM_attrId_t;


#ifdef __MSP430F2274__
#define NM_MAX_SUPPORTED_NODE_CNT 5
#else
#define NM_MAX_SUPPORTED_NODE_CNT  30
#endif

// rkris@wisense.in / april/21/15
// <TODO> Make this proportional to distance from coordinator
#define NM_ASSOC_TMO_SECS   10

#define NM_TIME_TO_NEXT_REASSOC_SECS  60

   
typedef struct 
{
   UINT8_t nodeCapInfo;
   UINT8_t parentNodeIdx;  // If RFD, parent node's index in the node table
   SINT8_t reassocTmo;  // rkris@wisense.in - sep/23/15
   UINT16_t lastSysFEId; // rkris@wisense.in - dec/27/14
#ifdef RFD_KA_ENA
   UINT16_t kaTTE;
#endif
   UINT16_t timeSinceLastRx;   // Time in seconds since last data packet received from this node
   MAC_shortAddr_t shortAddr;
   SYS_trscnQ_s pendingFramesQ;
   UINT8_t extAddr[MAC_EXT_ADDR_LEN];
} NM_nodeListEntry_s;


typedef struct 
{
#ifdef LPWMN_COORD     
  UINT8_t assocDoneCnt;
  UINT8_t assocFlrCnt;
#else
  UINT8_t scanCnfrmCnt;
  UINT8_t assocDoneCnt;     
  UINT16_t assocTmoCnt;
  UINT8_t scanOkCnt;
  UINT8_t scanFlrCnt;
  UINT16_t assocCnfrmCnt;
#endif      
#ifdef DEV_TYPE_RFD
  UINT16_t coordPollStartCnt;
  UINT16_t coordPollSuccessCnt;
#endif
} NM_stats_s;


#define NM_ASSOC_LIST_SZ  8

typedef struct
{
  UINT8_t attemptCnt;
  UINT16_t lpwmnId;
  UINT16_t shortAddr;
} NM_assocListEntry_s;


typedef struct
{
   UINT16_t currAssocLPWMNId;
   UINT16_t currAssocParentShortAddr;
   UINT8_t assocListEntryCnt;
   NM_assocListEntry_s  assocList[NM_ASSOC_LIST_SZ];
} NM_assocCntxt_s;


typedef struct
{   
#ifdef LPWMN_COORD 
   UINT8_t coordStarted:1;
   UINT8_t rsvd:7;
   UINT16_t  NM_nextAllocShortAddr;
   NM_nodeListEntry_s  NM_nodeList[NM_MAX_SUPPORTED_NODE_CNT];
#endif

#ifdef RFD_KA_ENA
   UINT16_t rfdKeepAliveTmoSecs;  // rkris@wisense.in / June/10/15
#endif
   
#ifdef RFD_NO_RESCAN_ON_REASSOC
   UINT8_t joinFlrCnt;
#endif

#if defined(DEV_TYPE_RFD) || defined(DEV_TYPE_FFD)
   UINT8_t assocListBdHndl;
#endif

#ifndef LPWMN_COORD
   UINT16_t scanFlrCnt;   // rkris@wisense.in / march/12/16 - recovery from coninuous scan failures
                          // Problem seen in field. One FFD periodically sending BCN-REQs and sniffer
                          // shows BCNs being transmitted in response. No recovery even after coord was
                          // rebooted. Could be some bug in active-scan module or radio rx path broken ?
#endif

#ifdef NM_STATS_ENA   
   NM_stats_s stats;
#endif   
} NM_cntxt_s;

#define NM_coordStarted( )  (NM_cntxt.coordStarted)

extern NM_cntxt_s NM_cntxt;        

#define NM_nodeAddr(idx) NM_cntxt.NM_nodeList[(idx)].shortAddr

extern void NM_init(void);

extern void NM_evtHndlr(UINT16_t globalEvtId, void *params_p);

extern void NM_start(void);

extern void NM_relayCmdProc(void);

extern void NWM_procRelayedAssocResp(UINT8_t *buff_p);

extern void NWM_procPendingTrscns(void);
   
extern void NWM_procRelayedAssocReq(UINT8_t *rxBuff_p);

extern NM_nodeListEntry_s *NM_lookUpNode(UINT8_t *nodeExdtAddr_p);
  
extern NM_nodeListEntry_s *NM_allocNodeTblEntry(void);
  
extern MAC_shortAddr_t NM_allocShortAddr(void);

extern NM_nodeListEntry_s *NM_lookUpShortAddr(MAC_shortAddr_t shortAddr);

extern UINT16_t NM_getJoinedNodeCnt(void);

extern void NM_freeNodeTblEntry(NM_nodeListEntry_s *entry_p);

extern UINT16_t NM_getNodeIdx(MAC_shortAddr_t shortAddr);

extern UINT8_t NM_lookUpLocalRFD(MAC_shortAddr_t shortAddr);

extern void NM_startScan(void);

extern void NM_startCoordPoll(void);

extern void NM_coordPollCnfrmHndlr(void);

extern SYS_trscnQ_s *NM_checkPendingFrame(MAC_shortAddr_t shortAddr);

extern UINT8_t NM_startCoord(void);

extern SYS_trscnQ_s *NM_getRFDPendTrscnQ(MAC_shortAddr_t shortAddr);

extern void NM_purgeRFDPendTrscnQ(NM_nodeListEntry_s *entry_p);

extern void NM_procPendScanReq(void);

extern UINT8_t NM_getAttrVal(UINT16_t attrId, UINT8_t *buff_p);

extern UINT8_t NM_getAttrValSz(UINT16_t attrId);

extern UINT8_t NM_setAttrVal(UINT16_t attrId, UINT8_t *buff_p);

extern void NM_checkRFDKATmo(void);

extern UINT8_t NM_checkResetKATMO(MAC_shortAddr_t shortAddr);

extern void NM_updateTrafficTS(UINT8_t interval);

extern void NM_deleteRFDs(UINT16_t nodeTblIdx);

extern void NM_checkReassocTimeOuts(SINT8_t elapsedTime);

extern UINT8_t NM_getListAttrVal(UINT16_t attrId, UINT16_t attrIdx, UINT8_t *buff_p);

#endif
