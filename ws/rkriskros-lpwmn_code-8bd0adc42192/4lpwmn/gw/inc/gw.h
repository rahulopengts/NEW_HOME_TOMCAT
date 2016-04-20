/*
 * File Name: gw.h
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

#ifndef __GW_H__
#define __GW_H__

#include <typedefs.h>
#include <mac.h>
#include <node_mgr.h>

#define LPWMN_GW_MESH_LEVEL_BROADCAST_MSK  0xfff0
#define LPWMN_GW_MESH_LEVEL_BROADCAST_HOP_CNT_MSK  0xf

// Message type 0x0 is reserved for ACKs
#define LPWMN_GW_MSG_TYPE_NODE_CNT_REQ     0x1
#define LPWMN_GW_MSG_TYPE_NODE_LIST_REQ    0x3
#define LPWMN_GW_MSG_TYPE_GET_NODE_LAST_FATAL_ERR_ID  0x4
#define LPWMN_GW_MSG_TYPE_RELAY_TO_NODE    0x5
#define LPWMN_GW_MSG_TYPE_RELAY_FROM_NODE  0x6
#define LPWMN_GW_MSG_TYPE_KEEP_ALIVE_REQ   0x7
#define LPWMN_GW_MSG_TYPE_KEEP_ALIVE_RESP  0x8
#define LPWMN_GW_MSG_TYPE_PING_NODE_REQ    0x9
#define LPWMN_GW_MSG_TYPE_NODE_PING_RESP   0xa

// rkris@wisense.in / april/20/15
#define LPWMN_GW_MSG_TYPE_REBOOT_COORD   0xb

#define LPWMN_GW_MSG_TYPE_NODE_RT_DISC_STS   0x10

#define LPWMN_GW_MSG_TYPE_PATH_DISC_REQ    0x13
#define LPWMN_GW_MSG_TYPE_PATH_DISC_RESP   0x14

#define LPWMN_GW_MSG_TYPE_NODE_TRAFFIC_STATS_REQ   0x18

#define LPWMN_GW_MSG_TYPE_RESET_NODE_TRAFFIC_STATS  0x1a

#define LPWMN_GW_MSG_TYPE_STOP_TRAFFIC_REQ   0x1e
#define LPWMN_GW_MSG_TYPE_START_TRAFFIC_REQ  0x1f


#define LPWMN_GW_MSG_TYPE_GET_COORD_STATS    0x20
#define LPWMN_GW_MSG_TYPE_RESET_COORD_STATS  0x21

#define LPWMN_GW_MSG_TYPE_STOP_NWK   0x30
#define LPWMN_GW_MSG_TYPE_START_NWK  0x31


#define LPWMN_GW_MSG_TYPE_ENABLE_NWK_JOINS  0x40
#define LPWMN_GW_MSG_TYPE_DISABLE_NWK_JOINS  0x41
#define LPWMN_GW_MSG_TYPE_GET_NWK_JOIN_CTRL_STATE  0x42
#define LPWMN_GW_MSG_TYPE_GET_NWK_JOIN_CTRL_STATE  0x42
#define LPWMN_GW_MSG_TYPE_ADD_NODE_TO_WHITE_LIST  0x43
#define LPWMN_GW_MSG_TYPE_DEL_NODE_FROM_WHITE_LIST  0x44
#define LPWMN_GW_MSG_TYPE_DELETE_WHITE_LIST  0x45
#define LPWMN_GW_MSG_TYPE_GET_WHITE_LIST_ENTRY  0x46

#define LPWMN_GW_MSG_TYPE_ADD_NODE_TO_BLACK_LIST  0x47
#define LPWMN_GW_MSG_TYPE_DEL_NODE_FROM_BLACK_LIST  0x48
#define LPWMN_GW_MSG_TYPE_DELETE_BLACK_LIST  0x49



#define LPWMN_GW_MSG_TYPE_CFG_DROP_BCN_REQS  0x4a
#define LPWMN_GW_MSG_TYPE_CFG_ACCEPT_BCN_REQS  0x4b
#define LPWMN_GW_MSG_TYPE_GET_BCN_REQ_HANDLING_CFG  0x4c

#define LPWMN_GW_MSG_TYPE_GET_BLACK_LIST_ENTRY  0x4d

#define LPWMN_GW_MSG_TYPE_SET_NWK_LPWMN_ID          0x50
#define LPWMN_GW_MSG_TYPE_SET_RADIO_CHANNEL         0x51
#define LPWMN_GW_MSG_TYPE_SET_LPWMN_COORD_EXT_ADDR  0x52
#define LPWMN_GW_MSG_TYPE_SET_RADIO_TX_PWR          0x53
#define LPWMN_GW_MSG_TYPE_START_COORD               0x54
#define LPWMN_GW_MSG_TYPE_STOP_COORD                0x55

#define LPWMN_GW_MSG_TYPE_GET_NWK_LPWMN_ID          0x59
#define LPWMN_GW_MSG_TYPE_GET_NWK_CHANNEL           0x5a
#define LPWMN_GW_MSG_TYPE_GET_LPWMN_COORD_EXT_ADDR  0x5b
#define LPWMN_GW_MSG_TYPE_GET_RADIO_TX_PWR          0x5c
#define LPWMN_GW_MSG_TYPE_GET_RADIO_PART_NR         0x5d
#define LPWMN_GW_MSG_TYPE_GET_RADIO_FREQ_BAND       0x5e
#define LPWMN_GW_MSG_TYPE_GET_CARRIER_FREQ          0x5f
#define LPWMN_GW_MSG_TYPE_GET_RADIO_BAUD_RATE       0x60
#define LPWMN_GW_MSG_TYPE_GET_RADIO_MOD_FMT         0x61

#define LPWMN_GW_MSG_TYPE_SET_WCT  0x70   // set wall clock time
#define LPWMN_GW_MSG_TYPE_GET_WCT  0x71   // get wall clock time

#define LPWMN_MSG_TYPE_GET_UP_TIME  0x80  // in seconds




#define LPWMN_GW_MSG_TYPE_COORD_LOG           0x90
#define LPWMN_GW_MSG_TYPE_ENABLE_COORD_LOG    0x91
#define LPWMN_GW_MSG_TYPE_DISABLE_COORD_LOG   0x92

#define LPMWN_GW_MSG_TYPE_GET_COORD_FREE_MEM_DESC_CNT  0x93

#define LPWMN_GW_MSG_TYPE_DEREG_NODE   0xa0

#define LPWMN_GW_MSG_TYPE_EVENT   0xb0

#define LPWMN_GW_MSG_TYPE_GET_COORD_SW_BUILD_DATE  0xc0
#define LPWMN_GW_MSG_TYPE_GET_COORD_SW_BUILD_TIME  0xc1

#define LPWMN_GW_MSG_TYPE_GET_COORD_ATTR_VAL  0xc8
#define LPWMN_GW_MSG_TYPE_SET_COORD_ATTR_VAL  0xc9

// Get next hop in route to specified node
#define LPWMN_MSG_TYPE_GET_RT_NH_ON_COORD  0xd0

#define LPWMN_GW_MSG_TYPE_GET_COORD_SNSR_OP  0xd8

#define LPWMN_GW_MSG_TYPE_GET_COORD_UP_TIME  0xda

#define LPWMN_GW_MSG_TYPE_TRIGGER_SENSOR_CONV  0xe0
#define LPWMN_GW_MSG_TYPE_COORD_SENSOR_OUTPUT  0xe1

#define LPWMN_GW_MSG_TYPE_MON_CHANN  0xe8

// Events sent to host by the LPWMN coordinator
#define LPWMN_GW_EVT_TYPE_NODE_REG               0x1
#define LPWMN_GW_EVT_TYPE_LOCAL_RT_DISC_STARTED  0x2
#define LPWMN_GW_EVT_TYPE_SYS_BOOT  			 0x3
#define LPWMN_GW_EVT_TYPE_NODE_DEREG             0x4
#define LPWMN_GW_EVT_TYPE_LOCAL_RT_DISC_OVER     0x5
#define LPWMN_GW_EVT_TYPE_BCN_REQ_RCVD           0x6
#define LPWMN_GW_EVT_TYPE_MAC_CNFRM              0x7
#define LPWMN_GW_EVT_TYPE_PURGE_RT_TBL           0x8
#define LPWMN_GW_EVT_TYPE_RCVD_PKT_WITH_INV_SRC_ADDR   0x9
#define LPWMN_GW_EVT_TYPE_ASSOC_REQ_DROPPED      0xa
#define LPWMN_GW_EVT_TYPE_ASSOC_REQ_RCVD         0xb
#define LPWMN_GW_EVT_TYPE_PATH_DISC_STARTED      0xc
#define LPWMN_GW_EVT_TYPE_PATH_DISC_OVER         0xd
#define LPWMN_GW_EVT_TYPE_RCVD_RT_REQ_WITH_INV_SRC_ADDR   0xe


#define LPWMN_GW_EVT_ID_LEN  0x1
#define LPWMN_GW_NODE_CNT_RESP_PYLD_LEN  2
#define LPWMN_GW_MSG_NODE_IDX_FIELD_LEN  2
#define LPWMN_GW_MSG_TBL_IDX_FIELD_LEN  2
#define LPWMN_GW_MSG_RADIO_CHANN_FIELD_LEN  2
#define LPMWN_GW_MSG_PATH_DISC_OPN_STS_FIELD_LEN  1

#define LPWMN_GW_STS_SUCCESS               0x0
#define LPWMN_GW_STS_INV_REQ               0x1
#define LPWMN_GW_STS_INV_PARAMS            0x2
#define LPWMN_GW_STS_PREV_REQ_IN_PROGRESS  0x3
#define LPWMN_GW_STS_NWK_STOPPED           0x4
#define LPWMN_GW_STS_NO_ROUTE_TO_NODE      0x5
#define LPWMN_GW_STS_INV_PYLD_LEN          0x6
#define LPWMN_GW_STS_INV_RADIO_TX_PWR_SET_POINT   0x7


typedef struct
{
   UINT8_t relayFrameBdHndl;
   MAC_shortAddr_t relayTarget;
   UINT32_t lpwmnMsgsRcvdCnt;   // Attribute id <GW_LPWMN_MSGS_RCVD_CNT_ATTR_ID>
} GW_cntxt_s;

extern GW_cntxt_s GW_cntxt;

extern void GW_rcvdMsgHndlr(UINT16_t msgType, UINT8_t bdHndl);

extern void GW_init(void);

extern void GW_relayCnfrm(UINT8_t moduleId, UINT8_t sts);

extern void GW_relayMsgFromRemoteNode(UINT16_t srcShortAddr,
                                      MCPS_dataIndParams_s *dataIndParams_p);

extern void GW_sendNodeRegEvt(NM_nodeListEntry_s *nodeInfo_p);
extern void GW_sendNodeDeRegEvt(NM_nodeListEntry_s *nodeInfo_p);
extern void GW_sendSysBootEvt(void);
extern void GW_sendLocalRtDiscStartedEvt(UINT16_t shortAddr, UINT16_t attemptNr);
extern void GW_sendLocalRtDiscOverEvt(UINT16_t destAddr, UINT16_t nextHop);
extern void GW_relayMsgToRemoteNode(void);
extern void GW_sendBcnReqRcvdEvt(const UINT8_t * const extAddr_p, const SINT8_t rssi, const UINT8_t lqi);
extern void GW_sendMacTxCnfrmEvt(UINT16_t dstAddr, UINT8_t macSts, UINT8_t srcModule);
extern void GW_sendRtPurgeEvent(UINT16_t shortAddr);
extern void GW_sendInvPktSrcEvt(UINT16_t shortAddr);
extern void GW_sendAssocReqDropEvt(UINT8_t *extAddr_p, SINT8_t timeInSecs);
extern void GW_sendAssocReqRcvdEvt(UINT8_t *extAddr_p);
extern void GW_sendPathDiscEvt(UINT8_t evtId, UINT8_t sts, UINT8_t bdHndl);
extern void GW_sendRtReqFromInvSrc(UINT16_t shortAddr);
#endif
