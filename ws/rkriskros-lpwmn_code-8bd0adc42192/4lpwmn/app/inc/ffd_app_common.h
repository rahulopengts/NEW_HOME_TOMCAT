/*
 * File Name: app.h
 * Author: ram krishnan (rkris@wisense.in)
 * Created: April/2/2015
 *
 *
 * Copyright (c) <2015>, <ram krishnan>
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

#ifndef __FFD_APP_COMMON_H__
#define __FFD_APP_COMMON_H__

#include <typedefs.h>

#define APP_TLV_TYPE_CFG_DATA   0xa0
#define APP_TLV_TYPE_DATA_PUSH_INTERVAL  0xa1

#define APP_TLV_TYPE_NV_ATTR_LIST  0xa2
#define APP_TLV_TYPE_NV_ATTR  0xa3

#define APP_DATA_PUSH_INTERVAL_VALUE_FIELD_SZ  4

#define APP_TLV_TYPE_FIELD_SZ  1
#define APP_TLV_LEN_FIELD_SZ  1
#define APP_TLV_HDR_SZ (APP_TLV_TYPE_FIELD_SZ + APP_TLV_LEN_FIELD_SZ)



extern UINT32_t FFD_APP_commIntervalInSecs;
extern UINT8_t APP_nodeJoinDoneIndCnt;
extern UINT32_t FFD_APP_dataTxReqCnt;
extern UINT32_t FFD_APP_dataRxReqFlrCnt;
extern UINT32_t FFD_APP_cnfrmOkCnt;
extern UINT32_t FFD_APP_cnfrmFlrCnt;
extern UINT16_t FFD_APP_memAllocFlrCnt;
extern UINT16_t FFD_APP_dataIndCnt;
extern UINT16_t FFD_APP_dataIndDropCnt;
extern UINT8_t APP_nodeJoined;
extern UINT32_t APP_txInterval;
extern UINT8_t APP_pendingTxMsgHndl;
extern UINT16_t APP_pendingTxMsgDestAddr;
extern UINT8_t FFD_appRespTxFlag, FFD_snsrDataTxPendFlag;
extern UINT16_t FFD_APP_noOpReqRcvdCnt;

extern void FFD_APP_commonInit(void);
extern void APP_nodeJoinDoneInd(void);
extern void FFD_APP_rcvdMsgHndlr(UINT8_t bdHndl, UINT16_t srcAddr);
extern void APP_evtHndlr(UINT16_t globalEvtId, void *params_p);
extern void APP_procGPIOEvt(void);
extern void FFD_APP_procPendDataReq(void);
extern void FFD_APP_buildSendSnsrUpdate(void);
extern void FFD_APP_collectSensorData(void);

extern UINT32_t FFD_APP_commIntervalInSecs;

extern void FFD_APP_periodicTmrExpCbFunc(void);
extern void APP_COMMON_procGPIOEvt();
extern void FFD_APP_stopPeriodicPush(void);

#endif
