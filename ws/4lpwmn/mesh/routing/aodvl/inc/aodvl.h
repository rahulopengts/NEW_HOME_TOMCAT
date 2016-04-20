/*
 * File Name: aodvl.h
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

#ifndef __AODVL_H__
#define __AODVL_H__

#include <typedefs.h>
#include <pltfrm.h>


#define AODVL_MAX_RT_DISC_TRY_CNT  3

#define AODVL_PERIODIC_TIMER_TMO_INTERVAL  1   // in seconds

#define AODVL_ROUTE_MAX_HOP_COUNT  6
   
#define AODVL_RT_COST_TYPE_HOP_COUNT_AVOIDING_WEAK_LINKS  0x0   

#define AODVL_DFLT_RT_COST_TYPE  AODVL_RT_COST_TYPE_HOP_COUNT_AVOIDING_WEAK_LINKS   
   
#define AODVL_WEAK_LINK_THRESHOLD_LQI  -80   // In dBM

#define AODVL_RT_DISC_HOLD_TIME  3

#define AOVL_PATH_RESP_TMO_SECS  4


#define AODVL_MAX_RT_COST_VAL  255
#define AODVL_MAX_WLC  15

#ifdef __MSP430F2274__
#define AODVL_RT_DISC_TBL_SZ  1
#define AODVL_ROUTING_TBL_SZ  1
#else
#define AODVL_RT_DISC_TBL_SZ  8
#define AODVL_ROUTING_TBL_SZ  32
#endif

#define AODVL_MSG_TYPE_RT_REQ    0x1
#define AODVL_MSG_TYPE_RT_REP    0x2
#define AODVL_MSG_TYPE_PATH_REQ  0x3
#define AODVL_MSG_TYPE_PATH_REP  0x4
#define AODVL_MSG_TYPE_RT_ERR    0x5

#define AODVL_WLC_FIELD_LEN  0x1
#define AODVL_MSG_FLAGS_FIELD_LEN  0x1
#define AODVL_MSG_RT_COST_FIELD_LEN  0x1
#define AODVL_MSG_REQ_ID_FIELD_LEN  0x1

#define AODVL_RT_REQ_MSG_LEN  (AODVL_MSG_FLAGS_FIELD_LEN \
                               + AODVL_WLC_FIELD_LEN \
                               + AODVL_MSG_RT_COST_FIELD_LEN \
                               + AODVL_MSG_REQ_ID_FIELD_LEN \
                               + MAC_SHORT_ADDR_LEN \
                               + MAC_SHORT_ADDR_LEN)
   
#define AODVL_RT_REP_MSG_LEN  (AODVL_MSG_FLAGS_FIELD_LEN \
                               + AODVL_WLC_FIELD_LEN \
                               + AODVL_MSG_RT_COST_FIELD_LEN \
                               + AODVL_MSG_REQ_ID_FIELD_LEN \
                               + MAC_SHORT_ADDR_LEN \
                               + MAC_SHORT_ADDR_LEN)

#define AODVL_RT_REQ_MSG_FLAGS_FIELD_OFF   0x0
#define AODVL_RT_REQ_MSG_WLC_FIELD_OFF  (AODVL_RT_REQ_MSG_FLAGS_FIELD_OFF \
                                         + AODVL_MSG_FLAGS_FIELD_LEN)
#define AODVL_RT_REQ_MSG_RT_COST_FIELD_OFF  (AODVL_RT_REQ_MSG_WLC_FIELD_OFF \
                                             + AODVL_WLC_FIELD_LEN)
#define AODVL_RT_REQ_MSG_REQ_ID_FIELD_OFF  (AODVL_RT_REQ_MSG_RT_COST_FIELD_OFF \
                                            + AODVL_MSG_RT_COST_FIELD_LEN)
#define AODVL_RT_REQ_MSG_DADDR_FIELD_OFF  (AODVL_RT_REQ_MSG_REQ_ID_FIELD_OFF \
                                           + AODVL_MSG_REQ_ID_FIELD_LEN)
#define AODVL_RT_REQ_MSG_OADDR_FIELD_OFF  (AODVL_RT_REQ_MSG_DADDR_FIELD_OFF \
                                           + MAC_SHORT_ADDR_LEN)
   
 
#define AODVL_RT_REP_MSG_FLAGS_FIELD_OFF   0x0
#define AODVL_RT_REP_MSG_WLC_FIELD_OFF  (AODVL_RT_REP_MSG_FLAGS_FIELD_OFF \
                                         + AODVL_MSG_FLAGS_FIELD_LEN)
#define AODVL_RT_REP_MSG_RT_COST_FIELD_OFF  (AODVL_RT_REP_MSG_WLC_FIELD_OFF \
                                             + AODVL_WLC_FIELD_LEN)
#define AODVL_RT_REP_MSG_REQ_ID_FIELD_OFF  (AODVL_RT_REP_MSG_RT_COST_FIELD_OFF \
                                            + AODVL_MSG_RT_COST_FIELD_LEN)
#define AODVL_RT_REP_MSG_DADDR_FIELD_OFF  (AODVL_RT_REP_MSG_REQ_ID_FIELD_OFF \
                                           + AODVL_MSG_REQ_ID_FIELD_LEN)
#define AODVL_RT_REP_MSG_OADDR_FIELD_OFF  (AODVL_RT_REP_MSG_DADDR_FIELD_OFF \
                                           + MAC_SHORT_ADDR_LEN)   



#define AODVL_RT_REQ_MSG_TYPE_SHIFT      0x0
#define AODVL_RT_REQ_MSG_TYPE_BM         0x7
#define AODVL_RT_REQ_MSG_TYPE_SHIFT_BM   0x7

#define AODVL_RT_REQ_MSG_RT_REPAIR_FLAG_SHIFT     0x3
#define AODVL_RT_REQ_MSG_RT_REPAIR_FLAG_SHIFT_BM  0x8
#define AODVL_RT_REQ_MSG_RT_REPAIR_FLAG_BM        0x1

#define AODVL_RT_REQ_MSG_COST_TYPE_SHIFT     0x4
#define AODVL_RT_REQ_MSG_COST_TYPE_SHIFT_BM  0xf0
#define AODVL_RT_REQ_MSG_COST_TYPE_BM        0x0f

#define AODVL_RT_REQ_MSG_WLC_SHIFT     0x0
#define AODVL_RT_REQ_MSG_WLC_SHIFT_BM  0xf
#define AODVL_RT_REQ_MSG_WLC_BM        0xf
   
   
#define AODVL_RT_REP_MSG_TYPE_SHIFT      0x0
#define AODVL_RT_REP_MSG_TYPE_BM         0x7
#define AODVL_RT_REP_MSG_TYPE_SHIFT_BM   0x7

#define AODVL_RT_REP_MSG_RT_REPAIR_FLAG_SHIFT     0x3
#define AODVL_RT_REP_MSG_RT_REPAIR_FLAG_SHIFT_BM  0x8
#define AODVL_RT_REP_MSG_RT_REPAIR_FLAG_BM        0x1

#define AODVL_RT_REP_MSG_COST_TYPE_SHIFT     0x4
#define AODVL_RT_REP_MSG_COST_TYPE_SHIFT_BM  0xf0
#define AODVL_RT_REP_MSG_COST_TYPE_BM        0x0f

#define AODVL_RT_REP_MSG_WLC_SHIFT     0x0
#define AODVL_RT_REP_MSG_WLC_SHIFT_BM  0xf
#define AODVL_RT_REP_MSG_WLC_BM        0xf   


#define AODVL_PATH_REQ_MSG_HOP_CNT_FIELD_LEN  0x1
#define AODVL_PATH_REP_MSG_HOP_CNT_FIELD_LEN  0x1

#define AODVL_PATH_REP_MSG_STS_FIELD_LEN  0x1

#define AODVL_PATH_REQ_MSG_MIN_LEN  (AODVL_MSG_FLAGS_FIELD_LEN  \
                                     + MAC_SHORT_ADDR_LEN  \
                                     + MAC_SHORT_ADDR_LEN  \
                                     + AODVL_PATH_REQ_MSG_HOP_CNT_FIELD_LEN)

#define AODVL_PATH_REP_MSG_MIN_LEN  (AODVL_MSG_FLAGS_FIELD_LEN  \
                                     + MAC_SHORT_ADDR_LEN  \
                                     + MAC_SHORT_ADDR_LEN  \
                                     + AODVL_PATH_REP_MSG_HOP_CNT_FIELD_LEN  \
                                     + AODVL_PATH_REP_MSG_STS_FIELD_LEN  \
                                     + AODVL_PATH_REP_MSG_HOP_CNT_FIELD_LEN)

#define AODVL_RT_ERR_MSG_LEN  (AODVL_MSG_FLAGS_FIELD_LEN  \
                               + MAC_SHORT_ADDR_LEN)


#define AODVL_RT_TBL_INV_IDX   0xff


#if AODVL_ROUTING_TBL_SZ > 256
#error 
#endif

typedef struct
{
   UINT8_t bdHndl;
   SINT8_t measLQI;
   UINT16_t senderAddr;
} AODVL_rcvdMsgInfo_s;

typedef struct
{
   UINT8_t nextIdx;
   UINT16_t destAddr;
   UINT16_t nextHop; 
} AODVL_routingTblEntry_s;

typedef struct
{
   UINT8_t rtReqId;
   
   // On the RREQ originating node, fwdRtCost is not used. "rtRepRcvdCnt"
   // is valid only on the RREQ originating node.
   union
   {
      UINT8_t fwdRtCost;
      UINT8_t rtRepRcvdCnt;
   } _union_0;
   UINT8_t residualRtCost;
   UINT8_t rtDiscTryCnt:3;
   UINT8_t rtRepairFlag:1;
   UINT8_t rtReqPending:1;
   UINT8_t rtRepPending:1;
   UINT8_t rsvd:2;   
   UINT8_t fwdWLC:4;
   UINT8_t residualWLC:4; 
   UINT8_t timeToExpiry;
   UINT16_t destAddr;
   UINT16_t origAddr;
   UINT16_t downStreamHop; 
   UINT16_t upStreamHop;
} AODVL_routeDiscTblEntry_s;  


typedef void (*AODVL_pathDiscCnfrmCbFunc_p)(UINT8_t sts, UINT8_t bdHndl);   

typedef struct
{
   UINT16_t destAddr;
   AODVL_pathDiscCnfrmCbFunc_p cbFunc_p;
} AODVL_pathDiscParams_s; 

typedef struct
{
   UINT8_t nextRtReqId;
#ifdef RADIO_CC2500
   UINT8_t pathDiscRtTblIdx:6;
#else
   UINT8_t rsvd:6;
#endif
   UINT8_t pathDiscInProgress:1;
   UINT8_t timerActive:1;
   UINT8_t pathDiscTmrTTE;
   UINT8_t netTraversalTime;
   UINT8_t rtTblHeadIdx;

#ifdef LPWMN_COORD
   UINT16_t pathDiscTgt;
#endif

#ifdef DEV_TYPE_FFD
   // rkris@wisense.in - june/8/15
   UINT8_t coordRtCost;
   UINT8_t coordWLC;
#endif

   UINT16_t rtDiscTblFullCnt;

   AODVL_pathDiscCnfrmCbFunc_p pathDiscCnfrmCbFunc_p;
   AODVL_routingTblEntry_s  routingTbl[AODVL_ROUTING_TBL_SZ];
   AODVL_routeDiscTblEntry_s  rtDiscTbl[AODVL_RT_DISC_TBL_SZ];
} AODVL_cntxt_s;

extern AODVL_cntxt_s AODVL_cntxt;

extern void AODVL_pktTxCnfrmHndlr(void);

extern void AODVL_pktTxProc(void);

extern UINT16_t AODVL_getRoute(UINT16_t shortAddr);

extern void AODVL_clearRoutes(UINT16_t shortAddr);

extern UINT8_t AODVL_rtDiscRequest(UINT16_t destAddr);

extern UINT16_t AODVL_getRoute(UINT16_t destAddr);   

extern void AODVL_init(void);

extern void AODVL_processRtReqMsg(AODVL_rcvdMsgInfo_s *);

extern void AODVL_processPathReqMsg(UINT8_t bdHndl);

extern void AODVL_processPathRepMsg(UINT8_t bdHndl);

extern void AODVL_rcvdMsgHndlr(AODVL_rcvdMsgInfo_s *info_p);

extern UINT8_t AODVL_pathDiscRequest(AODVL_pathDiscParams_s *params_p);

extern void AODVL_purgeRtTbls(UINT16_t shortAddr);

extern void AODVL_buildSendRtErrMsg(UINT16_t destAddr, UINT16_t pktOrigAddr);

extern void AODVL_periodicTmrExpCbFunc(void);

extern UINT8_t AODVL_lookUpNodeInRtDiscTbl(UINT16_t nodeAddr);

extern UINT16_t AODVL_getValidRtTblEntryCnt(void);

extern UINT8_t AODVL_checkRt(UINT16_t destAddr);

extern UINT8_t AODVL_lookUpNodeInRtDiscTbl(UINT16_t nodeAddr);

extern UINT16_t AODVL_getValidRtDiscTblEntryCnt(void);
   
#endif
