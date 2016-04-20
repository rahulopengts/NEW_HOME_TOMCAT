/*
 * File Name : cc2d33s.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : Feb/19/2014
 *
 *
 * Copyright (c) <2014>, <ram krishnan>
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

#ifndef __CC2D33S_H__
#define __CC2D33S_H__

#include <typedefs.h>
#include <i2c_sw.h>

#define CC2D33S_CONV_TIME_MSECS  100

#define CC2D33S_I2C_ADDR_PREFIX   0x28    // 0101 000  (7 bit address)

#define CC2D33S_RESP_STS_BITS_SHIFT  6
#define CC2D33S_RESP_STS_SHIFT_BM  0x3

#define CC2D33S_RESP_STS_DATA_FRESH           0x0
#define CC2D33S_RESP_STS_DATA_STALE           0x1
#define CC2D33S_RESP_STS_SENSOR_IN_CMD_MODE   0x2

#define CC2D33S_SENSOR_RH_OUTPUT_VALUE_SZ  2
#define CC2D33S_SENSOR_TEMP_OUTPUT_VALUE_SZ  2

typedef enum
{
  CC2D33S_MEAS_MODE_UPDATE,
  CC2D33S_MEAS_MODE_SLEEP
} CC2D33S_measMode_t;

typedef struct
{
   UINT8_t devId;
   CC2D33S_measMode_t measMode;
} CC2D33S_cntxt_s;


extern PLTFRM_sts_t CC2D33S_init(UINT8_t devId);

extern PLTFRM_sts_t CC2D33S_dataFetch(UINT8_t devId, UINT16_t *rh_p, SINT16_t *tempVal_p);

extern PLTFRM_sts_t CC2D33S_startSingleConv(UINT8_t devId, UINT16_t *tmoVal_p);

#endif
