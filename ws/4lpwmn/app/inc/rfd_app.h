/*
 * File Name: rfd_app.h
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Oct/1/2013
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


#ifndef __RFD_APP_H__
#define __RFD_APP_H__

#include <typedefs.h>

#define APP_TLV_TYPE_CFG_DATA   0xa0
#define APP_TLV_TYPE_DATA_PUSH_INTERVAL  0xa1

#define APP_TLV_TYPE_NV_ATTR_LIST  0xa2
#define APP_TLV_TYPE_NV_ATTR  0xa3

#define APP_DATA_PUSH_INTERVAL_VALUE_FIELD_SZ  4

#define PIR_SENSOR_PWR_DOWN_PUSH_INTERVAL  300
#define PIR_SENSOR_PWR_UP_PUSH_INTERVAL  120

#define APP_TLV_TYPE_FIELD_SZ  1
#define APP_TLV_LEN_FIELD_SZ  1
#define APP_TLV_HDR_SZ (APP_TLV_TYPE_FIELD_SZ + APP_TLV_LEN_FIELD_SZ)


#define RFD_KEEP_ALIVE_RETRY_TIME_SECS  5  // retry every 5 seconds

#if defined(RFD_APP_PIR_MS) || defined(RFD_APP_BAR_CODE_SCANNER)
#define RFD_APP_DFLT_WAKE_UP_INTERVAL_SECS  0
#elif defined(RFD_APP_4)
#define RFD_APP_DFLT_WAKE_UP_INTERVAL_SECS  30
#elif defined(RFD_VIBRATION_SNSR_APP_ENA)
#define  RFD_APP_DFLT_WAKE_UP_INTERVAL_SECS  180
#else
#define RFD_APP_DFLT_WAKE_UP_INTERVAL_SECS  60
#endif


#define RFD_APP_FAST_RETRY_ON_TX_FLR_INTERVAL_SECS   5

#define RFD_APP_POLL_TMO_MSECS  100

// rkris@wisense.in - march/2/15 - increase the timeout for 1200 bps
// Consider this scenario where FFD has a packet pending for an RFD
// RFD wakes up, sends sensor data to coord via parent FFD.
// Parent FFD sends ack to RFD with data pending bit set.
// Then parent FFD wants to forward sensor data to next hop while
// at the same time RFD wants to send data-req command to the parent
// FFD. If the parent's CSMA/CA looses and the FFD sends data-req cmd
// to the parent, the parent will ack the data-req cmd but it will
// then forward the sensor data to the next hop before sending the
// pending pkt to the RFD. If the RFD is waiting for only 1 second
// for pending data from parent FFD, the former will time out at
// low data rates.

#ifdef RADIO_BAUD_RATE_1200
#define RFD_APP_WAIT_COORD_DATA_TMO_MSECS 3000
#elif defined(RADIO_BAUD_RATE_38383)
#define RFD_APP_WAIT_COORD_DATA_TMO_MSECS 1000
#elif defined(RADIO_BAUD_RATE_10000)
#define RFD_APP_WAIT_COORD_DATA_TMO_MSECS 2000
#elif defined(RADIO_BAUD_RATE_250000)
#define RFD_APP_WAIT_COORD_DATA_TMO_MSECS 1000
#else
#error baud rate not supported !!
#endif
typedef enum
{
	RFD_APP_STATE_WAIT_CONVERSION,
	RFD_APP_STATE_WAIT_CONVERSION_1,
	RFD_APP_STATE_WAIT_CONVERSION_2,
	RFD_APP_STATE_WAIT_CONVERSION_3,
	RFD_APP_STATE_CONVERSION_DONE,
	RFD_APP_STATE_POLL_REQ_IN_PROGRESS,
	RFD_APP_STATE_WAIT_DATA_FROM_COORD
} RFD_APP_state_t;

extern SINT32_t RFD_APP_sleepTimeElapsed;

extern RFD_APP_state_t RFD_APP_state;
extern SINT32_t RFD_APP_snsrRepInterval;
extern UINT8_t APP_pendingTxMsgHndl;
extern UINT16_t APP_pendingTxMsgDestAddr;

extern UINT8_t RFD_APP_lastTxMsgType;

extern void RFD_APP_sendSensorVal(void);

extern void RFD_APP_wakeUpProc(void);

extern void RFD_APP_init(void);

extern void RFD_APP_COMMON_init(void);

extern void RFD_APP_evtHndlr(UINT16_t globalEvtId, void *params_p);

extern void APP_nodeJoinDoneInd(void);

extern void RFD_APP_prepareToEnterLPM(void);

extern void RFD_APP_wakeUpTimerExpEvt(void);

extern void APP_procGPIOEvt(void);

extern void RFD_APP_coordPollStartFlr(void);

extern void RFD_APP_pollCnfrmHndlr(MAC_sts_t sts);

extern void RFD_APP_pollReqCnfrmHndlr(void);

extern void RFD_APP_procPendDataReq(void);

extern UINT8_t RFD_APP_continueWithLowSupplyV(void);

extern UINT8_t RFD_APP_checkKAProc(void);

extern void RFD_APP_keepAliveTmrExpEvt(void);

extern void APP_COMMON_procGPIOEvt(void);

extern void MDS_APP_cnfrmFlrInd(void);

extern UINT32_t RFD_APP_cnfrmOkCnt;
extern UINT32_t RFD_APP_cnfrmFlrCnt;

extern void RFD_APP_snsrCtrlUpdate(UINT8_t snsrCntrlUpdate);

extern UINT32_t RFD_APP_lastSnsrUpdateSentTS;
extern UINT32_t RFD_APP_maxSnsrMsgTxInterval;

#ifdef VEHICLE_DETECTION_USING_HIGH_PASS_FILTER_ENA
extern UINT8_t VD_HPF_alpha;
extern UINT16_t VD_HPF_detectionTh;
extern UINT16_t VD_HPF_settlingTh;
#endif

#endif /* __RFD_APP_H__ */
