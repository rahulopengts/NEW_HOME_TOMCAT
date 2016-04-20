/*
 * File Name : mpl3115a2.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : Oct/2014
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

#ifndef __MPL3115A2_H__
#define __MPL3115A2_H__

#include <typedefs.h>
#include <i2c_sw.h>

#define MPL3115A2_I2C_ADDR_PREFIX   0x60    // 0 1100 000  (7 bit address)

/*
 * Device identification register contains the device identifier which is set
 * to 0xC4 by default. The value is factory programmed.
 */
#define MPL3115A2_DEV_ID  0xc4

#define MPL3115A2_STS_REG_ADDR  0x0
#define MPL3115A2_OUT_P_MSB_REG_ADDR  0x1
#define MPL3115A2_OUT_P_CSB_REG_ADDR  0x2
#define MPL3115A2_OUT_P_LSB_REG_ADDR  0x3
#define MPL3115A2_OUT_T_MSB_REG_ADDR  0x4
#define MPL3115A2_OUT_T_LSB_REG_ADDR  0x5
#define MPL3115A2_DEV_ID_REG_ADDR  0xc
#define MPL3115A2_SYSMOD_REG_ADDR  0x11
#define MPL3115A2_BAR_IN_MSB_REG_ADDR  0x14
#define MPL3115A2_BAR_IN_LSB_REG_ADDR  0x15
#define MPL3115A2_CTRL1_REG_ADDR  0x26
#define MPL3115A2_ALTITUDE_DATA_USER_OFF_REG_ADDR  0x2d


#define MPL3115A2_SYSMOD_REG_OPN_MODE_BIT  0x1    // 0x0 - standby / 0x1 - active




#define MPL3115A2_CTRL_REG1_RAW_BIT  (1 << 6)
#define MPL3115A2_CTRL_REG1_OS2_BIT  (1 << 5)
#define MPL3115A2_CTRL_REG1_OS1_BIT  (1 << 4)
#define MPL3115A2_CTRL_REG1_OS0_BIT  (1 << 3)
#define MPL3115A2_CTRL_REG1_OST_BIT  (1 << 1)
#define MPL3115A2_CTRL_REG1_SBYB_BIT (1 << 0)

#define MPL3115A2_OST_CONV_TMO_MSECS  100

#define MPL3115A2_TEMP_SENSOR_OUTPUT_VALUE_SZ   2
#define MPL3115A2_PRESSURE_SENSOR_OUTPUT_VALUE_SZ   4

typedef enum
{
	MPL3115A2_OPN_MODE_STANDBY,  // SBYB bit of CTRL_REG1 is cleared
	MPL3115A2_OPN_MODE_ACTIVE  // SBYB bit of CTRL_REG1 is set
} MPL3115A2_opnMode_t;


typedef struct
{
   UINT8_t devId;
   MPL3115A2_opnMode_t opnMode;
} MPL3115A2_cntxt_s;


extern PLTFRM_sts_t MPL3115A2_init(UINT8_t devId);

extern PLTFRM_sts_t MPL3115A2_startSingleConv(UINT8_t devId, UINT16_t *tmoVal_p);

extern PLTFRM_sts_t MPL3115A2_readTemp(UINT8_t devId, SINT16_t *tmoVal_p);

extern PLTFRM_sts_t MPL3115A2_readPressure(UINT8_t devId, UINT32_t *pressVal_p);

extern MPL3115A2_cntxt_s *MPL3115A2_getDevCntxt(UINT8_t devId);

#endif
