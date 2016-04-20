/*
 * File Name : ekmc160111x.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : Nov/18/2013
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

#ifndef __EKMC160111X_H__
#define __EKMC160111X_H__

#include <typedefs.h>
#include <pltfrm.h>

#define EKMC160111X_STATS_ENA

#define EKMC160111X_CKT_STABILITY_TIME_SECS  30  // 30 seconds max

#define EKMC160111X_DFLT_DEBOUNCE_TIME_SECS  5

typedef enum
{
   EKMC160111X_DEV_STATE_UNKNOWN = 0,
   EKMC160111X_DEV_STATE_POWERED_OFF = 1,
   EKMC160111X_DEV_STATE_POWERED_ON = 2,
   EKMC160111X_DEV_STATE_READY = 3,
} EKMC160111X_devState_t;

typedef struct
{
   UINT8_t devId;
   UINT8_t devState;
   PLTFRM_gpioPortId_t portId;
   PLTFRM_gpioPortPin_t portPinNr;
   PLTFRM_gpioPortId_t pwrOnCntrlPortId;
   PLTFRM_gpioPortId_t pwrOnCntrlPortPinNr;
   UINT8_t evtPending;
#ifdef EKMC160111X_STATS_ENA
   UINT16_t intCnt;
   UINT16_t evtPendCnt;
#endif
} EKMC160111X_cntxt_s;

extern SINT8_t EKMC160111X_debounceIntervalSecs;

extern PLTFRM_sts_t EKMC160111X_unMaskDevEvent(UINT8_t devId);
extern PLTFRM_sts_t EKMC160111X_init(UINT8_t devId);
extern PLTFRM_sts_t EKMC160111X_maskDevEvent(UINT8_t devId);
extern PLTFRM_sts_t EKMC160111X_powerDevUp(UINT8_t devId);
extern PLTFRM_sts_t EKMC160111X_powerDevDown(UINT8_t devId);
extern PLTFRM_sts_t EKMC160111X_checkPendingEvt(UINT8_t devId, UINT8_t *evtFlag_p);
extern PLTFRM_sts_t EKMC160111X_getSensorState(UINT8_t devId, UINT8_t *state_p);

#endif
