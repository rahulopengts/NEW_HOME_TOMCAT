/*
 * File Name : mpl115a2.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : Oct/17/2014
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

#ifndef __MPL115A2_H__
#define __MPL115A2_H__

#include <typedefs.h>
#include <i2c_sw.h>

#define MPL115A2_I2C_ADDR_PREFIX   0x60    // 0 1100 000  (7 bit address)

#define MPL115A2_P_ADC_MSB_REG_ADDR  0x00
#define MPL115A2_P_ADC_LSB_REG_ADDR  0x01
#define MPL115A2_T_ADC_MSB_REG_ADDR  0x02
#define MPL115A2_T_ADC_LSB_REG_ADDR  0x03
#define MPL115A2_A0_MSB_REG_ADDR     0x04
#define MPL115A2_A0_LSB_REG_ADDR     0x05
#define MPL115A2_B1_MSB_REG_ADDR     0x06
#define MPL115A2_B1_LSB_REG_ADDR     0x07
#define MPL115A2_B2_MSB_REG_ADDR     0x08
#define MPL115A2_B2_LSB_REG_ADDR     0x09
#define MPL115A2_C12_MSB_REG_ADDR    0x0a
#define MPL115A2_C12_LSB_REG_ADDR    0x0b
#define MPL115A2_CONVERT_REG_ADDR    0x12

#define MPL115A2_MIN_PRESSURE_OUTPUT_KPA  50
#define MPL115A2_MAX_PRESSURE_OUTPUT_KPA  115

/*
 * Time between start convert command and data available in the
 * Pressure and Temperature registers.
 * - Typical - 1.6 millisecs
 * - Max - 4 millisecs
 */
#define MPL115A2_CONVERSION_TIME_MILLISECS  3

#define MPL115A2_COEFF_TABLE_SZ  8

#define MPL115A2_MAX_WAKEUP_TIME_MILLI_SECS  (5 + 1)

#define MPL115A2_PRESSURE_SENSOR_OUTPUT_VALUE_SZ   4


typedef enum
{
   MPL115A2_OPN_MODE_SHUTDOWN,
   MPL115A2_OPN_MODE_ACTIVE
} MPL115A2_opnMode_t;


typedef struct
{
   UINT8_t devId;
   UINT8_t shutDownCtrlPortId;
   UINT8_t shutDownCtrlPinId;
   UINT8_t resetCtrlPortId;
   UINT8_t reseteCtrlPinId;
   MPL115A2_opnMode_t opnMode;
   float a0, b1, b2, c12;
   float pComp, pFinal;
   UINT8_t shutDownPending;
} MPL115A2_cntxt_s;


extern PLTFRM_sts_t MPL115A2_init(UINT8_t devId);
extern PLTFRM_sts_t MPL115A2_readPressure(UINT8_t devId,
                                          UINT32_t *pressVal_p);
extern PLTFRM_sts_t MPL115A2_startSingleConv(UINT8_t devId,
                                             UINT16_t *tmoVal_p);

#endif
