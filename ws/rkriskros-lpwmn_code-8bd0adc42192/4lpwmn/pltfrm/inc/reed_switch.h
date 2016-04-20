/*
 * File Name : reed_switch.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : Nov/24/2013
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

#ifndef __REED_SWITCH_H__
#define __REED_SWITCH_H__

#include <typedefs.h>
#include <pltfrm.h>

#define REED_SWITCH_STATE_OUTPUT_SZ  1

#define REED_SWITCH_DFLT_DEBOUNCE_TMO_MSECS  200

typedef enum
{
   REED_SWITCH_STATE_UNKNOWN = 0,
   REED_SWITCH_STATE_OPEN = 1,
   REED_SWITCH_STATE_CLOSED = 2
} REED_SWITCH_state_t;

typedef struct
{
   UINT8_t devId;
   PLTFRM_gpioPortId_t portId;
   PLTFRM_gpioPortPin_t portPinNr;
   REED_SWITCH_state_t state;
   UINT8_t evtPending;
#ifdef REED_SWITCH_STATS_ENA
   UINT16_t evtCnt;
   UINT16_t debounceCnt;
#endif
} REED_SWITCH_cntxt_s;

extern PLTFRM_sts_t REED_SWITCH_unMaskDevEvent(UINT8_t devId);
extern PLTFRM_sts_t REED_SWITCH_init(UINT8_t devId);
extern PLTFRM_sts_t REED_SWITCH_maskDevEvent(UINT8_t devId);
extern PLTFRM_sts_t REED_SWITCH_checkPendingEvt(UINT8_t devId, UINT8_t *evtFlag_p);
extern PLTFRM_sts_t REED_SWITCH_getState(UINT8_t devId, REED_SWITCH_state_t *swState_p);
extern PLTFRM_sts_t REED_SWITCH_ena(UINT8_t devId);

#endif

