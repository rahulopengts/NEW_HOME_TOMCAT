/*
 * File Name : mds.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : Sep/14/2015
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

#ifndef __MDS_H__
#define __MDS_H__

#include <typedefs.h>
#include <pltfrm.h>

#define MDS_STATE_ON   0xff
#define MDS_STATE_OFF  0x00
#define MDS_STATE_UNKNOWN  0xa5


#define MDS_OUTPUT_VALUE_SZ  1

typedef struct
{
   UINT8_t devId;
#ifdef MDS_USING_OPAMP_COMPARATOR_ENA
   PLTFRM_gpioPortId_t portId;
   PLTFRM_gpioPortPin_t portPinNr;
#elif defined(MDS_USING_ADC_ENA)
   UINT8_t adcChannId;
#else
#error method not specified !!
#endif
   UINT8_t evtPending;
} MDS_cntxt_s;


extern PLTFRM_sts_t MDS_init(UINT8_t devId);
extern PLTFRM_sts_t MDS_getCurrState(UINT8_t devId, UINT8_t *state_p);


#endif
