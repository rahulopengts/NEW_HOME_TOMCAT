/*
 * File Name : MAX_SONAR.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : 10/23/2013
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

#ifndef __MAX_SONAR_H__
#define __MAX_SONAR_H__

#include <typedefs.h>
#include <pltfrm.h>

/*
 *  'R' D2 D1 D1 \13
 */
#define MAX_SONAR_SERIAL_OP_LEN  5
#define MAX_SONAR_RANGE_DIGIT_CNT  3    // 3 digits

#define MAX_SONAR_RANGING_TIME_MILLISECS  500

#define PLTFRM_MAX_SONAR_RANGE_OUTPUT_LEN 2

typedef struct
{
   UINT8_t devId;
   PLTFRM_gpioPortId_t port;
   PLTFRM_gpioPortPin_t pin;
   UINT8_t initDone:1;
   UINT8_t opBuffIdx:5;
   UINT8_t opRcvd:1;
   UINT8_t rangingInitiated:1;
   PLTFRM_snsrDevInfo_s info;
   UINT8_t opBuff[MAX_SONAR_SERIAL_OP_LEN];
} MAX_SONAR_cntxt_s;

extern PLTFRM_sts_t MAX_SONAR_init(UINT8_t devId);

extern PLTFRM_sts_t MAX_SONAR_getLatestRangeReading(const UINT8_t devId,
                                                    UINT16_t *rangeInInches);

extern PLTFRM_sts_t MAX_SONAR_startRanging(const UINT8_t devId,
		                                   UINT16_t *tmoMilliSecs_p);

extern PLTFRM_sts_t MAX_SONAR_UART_start(UINT8_t devId);


#endif
