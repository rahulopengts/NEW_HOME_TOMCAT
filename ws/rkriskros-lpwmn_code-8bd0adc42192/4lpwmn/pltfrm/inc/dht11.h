/*
 * File Name : dht11.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : Jan/7/2015
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

#ifndef __DHT11_H__
#define __DHT11_H__

#include <typedefs.h>
#include <pltfrm.h>

/*
 * Datasheet:
 * http://www.micropik.com/PDF/dht11.pdf
 */

#define DHT11_SNSR_BIT_STREAM_LEN  40

#define DHT11_BIT_0_OP_LEN_MICROS  27  // 26-28
#define DHT11_BIT_1_OP_LEN_MICROS  70  // 70

#define DHT11_START_SIG_MIN_TIME_MSECS  18

#define PLTFRM_DHT11_1_DEV_HUMIDITY_VAL_OP_LEN  2

typedef struct
{
   UINT8_t devId;
   PLTFRM_gpioPortId_t port;
   PLTFRM_gpioPortPin_t pin;
   PLTFRM_snsrDevInfo_s info;
} DHT11_cntxt_s;


extern PLTFRM_sts_t DHT11_init(UINT8_t devId);
extern PLTFRM_sts_t DHT11_readSnsr(UINT8_t devId, SINT16_t *temp_p, SINT16_t *humidity_p);

#endif
