/*
 * File Name : ntc_thermistor.h
 * Author :ram krishnan (rkris@wisense.in)
 * Created : March/24/2015
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

#ifndef __NTC_THERMISTOR_H__
#define __NTC_THERMISTOR_H__

#include <typedefs.h>
#include <pltfrm.h>

#define NTC_THERM_103AT_4_R25_VAL  10000
#define NTC_THERM_103AT_4_B_25_85_VAL   3435

#define NTC_THERM_NTCALUG02A_R25_VAL  10000
#define NTC_THERM_NTCALUG02A_B_25_85_VAL   3984

#define NTC_THERMISTOR_SENSOR_OUTPUT_VALUE_SZ  4  // Returns thermistor resistance (not temperature)

typedef struct
{
   UINT8_t devId;
   UINT8_t adcChannId;
   UINT8_t sigPortId;  // Gates Vcc to op amp and resistor divider
   UINT8_t sigPinId;  // Gates Vcc to op amp and resistor divider
   UINT16_t r25;
   UINT16_t bVal;
   PLTFRM_snsrDevInfo_s info;
} NTC_THERMISTOR_cntxt_s;

extern PLTFRM_sts_t NTC_THERMISTOR_init(UINT8_t devId);
extern PLTFRM_sts_t NTC_THERMISTOR_getSnsrOp(UINT8_t devId, UINT32_t *snsrOp_p);  // Bug-fix / feb/17/16 / rkris@wisense.in

#endif
