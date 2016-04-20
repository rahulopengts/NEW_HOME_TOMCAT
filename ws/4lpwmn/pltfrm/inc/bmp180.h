/*
 * File Name : bmp180.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : Oct/18/2014
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

#ifndef __BMP180_H__
#define __BMP180_H__

#include <typedefs.h>
#include <i2c_sw.h>

#define BMP180_I2C_ADDR_PREFIX   0x77  // 0 1110 111 (7 bit address)

#define BMP180_OUT_XLSB_REG_ADDR  0xf8
#define BMP180_OUT_LSB_REG_ADDR   0xf7
#define BMP180_OUT_MSB_REG_ADDR   0xf6
#define BMP180_CTRL_MEAS_REG_ADDR  0xf4
#define BMP180_SOFT_RESET_REG_ADDR  0xe0
#define BMP180_ID_REG_ADDR  0xd0

#define BMP180_AC1_MSB_REG_ADDR  0xaa
#define BMP180_AC1_LSB_REG_ADDR  0xab

#define BMP180_AC2_MSB_REG_ADDR  0xac
#define BMP180_AC2_LSB_REG_ADDR  0xad

#define BMP180_AC3_MSB_REG_ADDR  0xae
#define BMP180_AC3_LSB_REG_ADDR  0xaf

#define BMP180_AC4_MSB_REG_ADDR  0xb0
#define BMP180_AC4_LSB_REG_ADDR  0xb1

#define BMP180_AC5_MSB_REG_ADDR  0xb2
#define BMP180_AC5_LSB_REG_ADDR  0xb3

#define BMP180_AC6_MSB_REG_ADDR  0xb4
#define BMP180_AC6_LSB_REG_ADDR  0xb5

#define BMP180_B1_MSB_REG_ADDR  0xb6
#define BMP180_B1_LSB_REG_ADDR  0xb7

#define BMP180_B2_MSB_REG_ADDR  0xb8
#define BMP180_B2_LSB_REG_ADDR  0xb9

#define BMP180_MB_MSB_REG_ADDR  0xba
#define BMP180_MB_LSB_REG_ADDR  0xbb

#define BMP180_MC_MSB_REG_ADDR  0xbc
#define BMP180_MC_LSB_REG_ADDR  0xbd

#define BMP180_MD_MSB_REG_ADDR  0xbe
#define BMP180_MD_LSB_REG_ADDR  0xbf


#define BMP180_CHIP_ID  0x55

#define BMP180_START_TEMP_CONV_CODE   0x0e
#define BMP180_START_PRESSURE_CONV_CODE  0x14

#define BMP180_CTRL_MEAS_SCO_BIT  0x20
#define BMP180_CTRL_MEAS_OSS_BIT_MSK  0xc0
#define BMP180_CTRL_MEAS_OSS_BIT_SHIFT 6


/*
 * By using different modes the optimum compromise between power
 * consumption, speed and resolution can be selected.
 */
typdef enum
{
   BMP180_OSS_ULTRA_LOW_POWER,
   BMP180_OSS_STANDARD,
   BMP180_OSS_HIGH_RES,
   BMP180_OSS_ULTRA_HIGH_RES,
} BMP180_oss_t;


// c_t = 1.5 + 3*n
// When n is 1, c_t is 4.5 msecs
// When n is 2, c_t is 7.5 msecs
// When n is 4, c_t is 13.5 msecs
// When n is 8, c_t is 25.5 msecs

#define BMP180_ULTRA_LOW_POWER_OSS_CONVERSION_TIME_MILLISECS  5  // 4.5 millisecs
#define BMP180_STANDARD_OSS_CONVERSION_TIME_MILLISECS  8  // 7.5 millisecs
#define BMP180_HIGH_RES_OSS_CONVERSION_TIME_MILLISECS  14  // 13.5 millisecs
#define BMP180_ULTRA_HIGH_RES_OSS_CONVERSION_TIME_MILLISECS  26  // 25.5 millisecs

#define BMP180_ULTRA_LOW_POWER_OSS_SAMPLE_CNT  1
#define BMP180_STANDARD_OSS_SAMPLE_CNT  2
#define BMP180_HIGH_RES_OSS_SAMPLE_CNT  4
#define BMP180_ULTRA_HIGH_RES_OSS_SAMPLE_CNT  8


#define BMP180_MAX_TEMP_CONVERSION_TIME  5  // 4.5 msecs

typedef struct
{
   UINT8_t devId;
   UINT8_t busId;
   int32_t ac1, ac2, ac3, b1, b2, mb, mc, md;
   uint32_t ac4, ac5, ac6;
} BMP180_cntxt_s;


#endif
