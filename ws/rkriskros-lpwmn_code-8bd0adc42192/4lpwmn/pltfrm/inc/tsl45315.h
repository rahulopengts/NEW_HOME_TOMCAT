/*
 * File Name : tsl45315.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : Nov/10/2013
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

#ifndef __TSL45315_H__
#define __TSL45315_H__

#include <typedefs.h>
#include <i2c_sw.h>

#define TSL45315_SENSOR_OUTPUT_VALUE_SZ  4

// See table 2 (Command register)
#define TSL45315_COMMAND_FMT_BM  0x80


#define TSL45315_I2C_ADDR_PREFIX   0x29    // 0101 001  (7 bit address)

#define TSL45315_CONTROL_REG_ADDR    0x0
#define TSL45315_CONFIG_REG_ADDR     0x1
#define TSL45315_DATA_LOW_REG_ADDR   0x4
#define TSL45315_DATA_HIGH_REG_ADDR  0x5
#define TSL45315_ID_REG_ADDR         0xa

/*
 * ID Register
 */
#define TSL45315_PART_NR_SHIFT  0x4
#define TSL45315_PART_NR_SHIFT_BIT_MSK  0xf

#define TSL45311_PART_NUMBER_ID   0xb
#define TSL45313_PART_NUMBER_ID   0x9
#define TSL45315_PART_NUMBER_ID   0xa
#define TSL45317_PART_NUMBER_ID   0x8


/*
 * Control register
 */
#define TSL45315_PWR_DOWN_MODE_ENC      0x0
#define TSL45315_SINGLE_CYCLE_MODE_ENC  0x2
#define TSL45315_NORMAL_MODE_ENC        0x3

#define TSL45315_CONTROL_REG_MODE_BIT_MSK  0x3

/*
 * Configuration register
 */
#define TSL45315_DFLT_INTEGRATION_TIME  400

#define TSL45315_CONFIG_REG_TCNTRL_BIT_MSK  0x3

#define TSL45315_400_MS_INTEGRATION_TIME_ENC  0x0
#define TSL45315_200_MS_INTEGRATION_TIME_ENC  0x1
#define TSL45315_100_MS_INTEGRATION_TIME_ENC  0x2


/*
 * The device allows the user to control the integration time. This
 * enables the user to control the sensitivity of the device to allow
 * for the greater dynamic range needed in bright lighting conditions
 * such as sunlight. Integration times of 400 ms, 200 ms, or 100 ms
 * are available.
 */

typedef enum
{
   // Implementation specific
   TSL45315_OPN_MODE_UNKNOWN,

   TSL45315_OPN_MODE_POWER_DOWN,

   TSL45315_OPN_MODE_NORMAL
} TSL45315_mode_t;


typedef struct
{
   UINT8_t devId;
   UINT8_t tcntrlEnc;  // 0 - 400 msecs, 1 - 200 msecs, 2 - 100 msecs
   TSL45315_mode_t opnMode;
} TSL45315_cntxt_s;


extern PLTFRM_sts_t TSL45315_init(UINT8_t devId);

extern PLTFRM_sts_t TSL45315_startALSConv(UINT8_t devId,
                                          SINT32_t *luxVal_p,
                                          UINT16_t *tmoVal_p);

extern PLTFRM_sts_t TSL45315_cfgALSOpnMode(UINT8_t devId,
                                           TSL45315_mode_t reqOpnMode,
                                           UINT16_t *opnTime_p);

extern TSL45315_mode_t TSL45315_getOpnMode(UINT8_t devId);

extern PLTFRM_sts_t TSL45315_getALSOutput(UINT8_t devId,
                                          SINT32_t *luxVal_p);

#endif
