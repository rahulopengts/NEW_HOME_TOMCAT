/*
 * File Name: adp.h
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Sep/5/2013
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


#ifndef __ADP_H__
#define __ADP_H__

#include <typedefs.h>
#include <mac.h>

#define ADP_ELEMENT_TYPE_LEN  0x1

#define ADP_MAX_HOP_COUNT  0x8


#define ADP_REASSY_PROC_TMO_SECS  16   // seconds


#define ADP_RT_DISC_TO_COORD_FLR_CNT_THRESHOLD  5


// source port (4 bits) | destination port (4 bits)
#define ADP_NON_IP_DEST_PORT_FIELD_LEN   0x1

#define ADP_ELEMENT_E2E_ACK_REQUEST_BIT   0x80

#define ADP_ELEMENT_TYPE_AODVL  0x10

#define ADP_ELEMENT_TYPE_MESH_HDR  0x20

#define ADP_ELEMENT_TYPE_FRAG_HDR  0x21

#define ADP_ELEMENT_TYPE_MESH_BC_HDR  0x22

#define ADP_ELEMENT_TYPE_URGENT_DATA  0x41
#define ADP_ELEMENT_TYPE_NORMAL_DATA  0x40

#define ADP_ELEMENT_TYPE_ASSOC_REQ       0x30    // COORD -> LPWMN COORD
#define ADP_ELEMENT_TYPE_ASSOC_RESP      0x31    // LPWMN COORD -> COORD
#define ADP_ELEMENT_TYPE_DISASSOC_REQ    0x32    // LPWMN COORD -> ANY NODE
#define ADP_ELEMENT_TYPE_DISASSOC_RESP   0x33    // ANY Node -> LPWMN COORD

#define ADP_ELEMENT_TYPE_FIRST_FRAG_HDR    0x60
#define ADP_ELEMENT_TYPE_MIDDLE_FRAG_HDR   0x61
#define ADP_ELEMENT_TYPE_LAST_FRAG_HDR     0x62

#define ADP_MESH_HDR_DEST_ADDR_FIELD_LEN  MAC_SHORT_ADDR_LEN
#define ADP_MESH_HDR_SRC_ADDR_FIELD_LEN  MAC_SHORT_ADDR_LEN

#define ADP_MESH_HDR_HOP_COUNT_FIELD_LEN  1

#define ADP_MESH_HDR_LEN  (ADP_MESH_HDR_HOP_COUNT_FIELD_LEN \
                           + ADP_MESH_HDR_DEST_ADDR_FIELD_LEN \
                           + ADP_MESH_HDR_SRC_ADDR_FIELD_LEN)

#define ADP_MESH_HDR_HOP_COUNT_OFF   0x0
#define ADP_MESH_HDR_DEST_ADDR_OFF  0x1
#define ADP_MESH_HDR_SRC_ADDR_OFF   0x3


#define ADP_MESH_BC_HDR_HOP_COUNT_FIELD_OFF  0x0
#define ADP_MESH_BC_HDR_SEQ_NR_FIELD_OFF  0x1
#define ADP_MESH_BC_HDR_SRC_ADDR_OFF  0x2

#define ADP_MESH_BC_HDR_HOP_COUNT_FIELD_LEN  1
#define ADP_MESH_BC_HDR_SRC_ADDR_FIELD_LEN  MAC_SHORT_ADDR_LEN
#define ADP_MESH_BC_HDR_SEQ_NR_FIELD_LEN  1

#define ADP_MESH_BC_HDR_LEN  (ADP_MESH_BC_HDR_HOP_COUNT_FIELD_LEN \
                              + ADP_MESH_BC_HDR_SRC_ADDR_FIELD_LEN \
                              + ADP_MESH_BC_HDR_SEQ_NR_FIELD_LEN)


// Fragmentation header
#define ADP_FRAG_HDR_LEN  1  //   (4 bits tag | 4 bits frag index)
#define ADP_FRAG_HDR_TAG_FIELD_MSK  0xf
#define ADP_FRAG_HDR_IDX_FIELD_MSK  0x7
#define ADP_FRAG_HDR_TAG_FIELD_SHIFT  4
#define ADP_FRAG_HDR_IDX_FIELD_SHIFT  0
#define ADP_FRAG_HDR_TAG_FIELD_SHIFT_MSK  0xf0
#define ADP_FRAG_HDR_IDX_FIELD_SHIFT_MSK  0x07
#define ADP_FRAG_HDR_LAST_FRAG_IND_BIT  0x8

#define ADP_MAX_REASSY_CNTXTS  8

#define ADP_MAX_FRAGS_PER_SDU  8  // (0x0 - 0x7)

#define ADP_ASSOC_RESP_ELEMENT_LEN   (MAC_EXT_ADDR_LEN \
                                      + MAC_ASSOC_RESP_STS_LEN \
                                      + MAC_SHORT_ADDR_LEN \
                                      + MAC_NODE_CAPABILITY_INFO_LEN)

#define ADP_ASSOC_REQ_ELEMENT_LEN    (MAC_EXT_ADDR_LEN \
                                      + MAC_NODE_CAPABILITY_INFO_LEN)
                                            

#define ADP_TX_REQ_SRC_NONE             0x0
#define ADP_TX_REQ_SRC_APP              0x1
#define ADP_TX_REQ_SRC_AODVL            0x2
#define ADP_TX_REQ_SRC_FWD_PATH         0x3
#define ADP_TX_REQ_SRC_ASSOC_REQ_RELAY  0x4
#define ADP_TX_REQ_SRC_ASSOC_RESP_TO_RELAY  0x5
#define ADP_TX_REQ_SRC_GW               0x6
#define ADP_TX_REQ_SRC_UC_FU            0x7





#define ADP_TRSCN_TYPE_ASSOC_REQ             0x1
#define ADP_TRSCN_TYPE_ASSOC_RESP            0x2
#define ADP_TRSCN_TYPE_ASSOC_RESP_TO_RELAY   0x3
#define ADP_TRSCN_TYPE_IPv4_PDU              0x4
#define ADP_TRSCN_TYPE_MESH_PDU              0x5
#define ADP_TRSCN_TYPE_AODVL_PATH_REQ        0x6
#define ADP_TRSCN_TYPE_AODVL_PATH_REP        0x7
#define ADP_TRSCN_TYPE_RERR_TRX_REQ          0x8
#define ADP_TRSCN_TYPE_RFD_REASSOC           0x9
#define ADP_TRSCN_TYPE_MESH_BC_PDU           0xa


#define ADP_TRSCN_TYPE_FIELD_LEN  0x1


#define ADP_FWD_PKT_Q_LEN  3


#define ADP_PKT_TYPE_IPv4_PDU    0x1
#define ADP_PKT_TYPE_MESH_PDU    0x2
#define ADP_PKT_TYPE_ASSOC_PDU   0x3

#define ADP_BC_LOG_TBL_SZ  4
#define ADP_BC_LOG_TBL_ENTRY_MAX_TTL_IN_SECS   32


typedef enum
{
   ADP_STS_SUCCESS,
   ADP_STS_NODE_NOT_FOUND,
   ADP_STS_OOM,
   ADP_STS_TX_FLR,
   ADP_STS_RT_DISC_OOM,
   ADP_STS_RT_DISC_FAILED,
   ADP_STS_ROUTING_LOOP,
   ADP_STS_MAX_HOPS_EXCEEDED,
   ADP_STS_NO_ROUTE,
   ADP_STS_PATH_DISC_TIMED_OUT,
   ADP_STS_NODE_NOT_ASSOCIATED,
   ADP_STS_TOO_MANY_FRAGMENTS ,
   ADP_STS_FRAG_NOT_SUPPORTED
} ADP_sts_t;

typedef struct
{
   UINT8_t bdHndl;
   UINT8_t localOrig;
   UINT8_t srcModId;
   UINT8_t meshBroadcastMaxHopCnt;
   UINT16_t destShortAddr;
   UINT16_t nextHopAddr;
#ifdef IPv4_SUPPORT_ENA
   UINT16_t destUdpPort;
   UINT32_t destIp;
#endif
} ADP_dataReqParams_s;

typedef struct
{
   UINT8_t fragCnt;
   UINT8_t fragLen;
   UINT8_t fragTag;
   UINT8_t currFragIdx;
   UINT8_t macHdrLen;
   UINT8_t adpHdrLen;
   UINT16_t currFragOffset;
   UINT16_t sduLenLeft;
} ADP_dataReqFragInfo_s;

typedef struct
{
   UINT8_t nextExpFragIdx;
   UINT8_t timeToExpiry;
   UINT8_t fragTag;
   UINT16_t srcShort;
   SYS_trscnQ_s reassyQ;
} ADP_reassmCntxt_s;

typedef struct
{
#ifdef DEV_TYPE_RFD
   UINT8_t framePending;
#endif
   ADP_sts_t sts; 
} ADP_dataCnfrmParams_s;

typedef struct
{
   UINT8_t bdHndl;
   UINT16_t srcAddr;
} ADP_dataIndParams_s; 

typedef struct
{
   UINT8_t bdHndl;
   UINT8_t nextIdx;
} ADP_fwdPktQEntry_s;

typedef struct
{
   UINT8_t seqNr;
   SINT8_t ttl;
   UINT16_t srcAddr;
} ADP_bcLogTblEntry_s;
  
typedef struct
{
#ifdef ADP_STATS_ENA  
   UINT8_t ADP_dataRcvdCnt;
#endif   
   UINT8_t pendTxReqSrc;
   UINT8_t fragTag;
   UINT8_t meshBroadcastSeqNr;
#ifndef DEV_TYPE_RFD 
   UINT8_t fwdPktQHead;
#endif
   ADP_dataCnfrmParams_s cnfrmSts;
#ifdef IPv4_SUPPORT_ENA
   UINT16_t ipv4NetAddr;
#endif

#if defined(DEV_TYPE_FFD)  || defined(LPWMN_COORD)
   UINT16_t rtDiscErrCnt;
   UINT16_t rtDiscStartedCnt;
   UINT16_t rtDiscSuccessCnt;
   UINT16_t rtDiscFlrCnt;
   UINT16_t appPyldTxProcFlrCnt;
   UINT16_t meshBroadcastPktRcvdCnt;
   UINT16_t meshBroadcastDupPktDroppedCnt;
   UINT16_t meshLoopDetectCnt;
#endif

#ifdef DEV_TYPE_FFD
   UINT16_t assocReqRelayedCnt;
   UINT16_t relayedAssocRespRcvdCnt;
#endif

   ADP_dataReqParams_s txParams;
   ADP_dataIndParams_s dataIndParams;
   ADP_dataReqFragInfo_s fragInfo;
   SYS_trscnQ_s trscnQ;
#ifndef DEV_TYPE_RFD 
   ADP_fwdPktQEntry_s fwdPktQ[ADP_FWD_PKT_Q_LEN];
   SYS_trscnQ_s rtPendFwdPktTrscnQ;
   SYS_trscnQ_s assocRtPendTrscnQ;
#endif

#if defined(DEV_TYPE_FFD) && defined(FFD_RBT_ON_RPT_RT_FLR_TO_COORD)
   UINT8_t rtDiscToCoordFlrCnt;
#endif

   ADP_bcLogTblEntry_s  bcLogTbl[ADP_BC_LOG_TBL_SZ];

#ifdef LPWMN_COORD
#ifdef ADP_FRAG_ENA
   UINT8_t reassyCntxtsInUse;
   ADP_reassmCntxt_s reAssyFragCntxtList[ADP_MAX_REASSY_CNTXTS];
#endif
#endif
} ADP_cntxt_s;

extern ADP_cntxt_s ADP_cntxt;


/*
 * This API is used to transmit app layer payload
 */
extern ADP_sts_t ADP_appPyldTxReq(void);

extern ADP_sts_t ADP_appPyldTxProc(void);

extern void ADP_evtHndlr(UINT16_t globalEvtId, void *params_p);

extern void ADP_init(void);

extern void ADP_fwdPktTxProc(void);

extern void ADP_procPendingTrscns(void);

extern void ADP_retryAppDataTxPostRtDisc(void);

extern void ADP_procRtDiscOpnRslt(UINT16_t, UINT16_t);

extern void ADP_sendSigPkt(UINT8_t bdHndl, UINT16_t nextHop, UINT8_t ackReqd);

extern UINT8_t * ADP_prefixMeshHdr(UINT8_t *buff_p, UINT16_t destAddr);

extern UINT16_t ADP_getRoute(UINT16_t destAddr);

extern void ADP_macAssocIndHndlr(UINT8_t *srcExtAddr_p, UINT8_t capInfo, UINT16_t lastSysFEId);

extern void ADP_checkBcLogTbl(UINT8_t elapsedTimeSecs);

#endif
