/*
 * File Name : mag3110.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : 11/19/2015
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

#ifndef __MAG3110_H__
#define __MAG3110_H__

#include <typedefs.h>
#include <i2c_sw.h>

/*
 * Freescale's MAG3110 is a small, low-power, digital 3-axis magnetometer.
 */

#define MAG3110_DEVICE_ID_NUMBER   0xc4

#define MAG3110_I2C_ADDR_PREFIX  0x0e    // 0000 1110  (7 bit address)


#define MAG3110_DR_STATUS_REG_ADDR    0x0
#define MAG3110_OUT_X_MSB_REG_ADDR    0x1
#define MAG3110_OUT_X_LSB_REG_ADDR    0x2
#define MAG3110_OUT_Y_MSB_REG_ADDR    0x3
#define MAG3110_OUT_Y_LSB_REG_ADDR    0x4
#define MAG3110_OUT_Z_MSB_REG_ADDR    0x5
#define MAG3110_OUT_Z_LSB_REG_ADDR    0x6
#define MAG3110_WHO_AM_I_REG_ADDR     0x7
#define MAG3110_SYSMOD_REG_ADDR       0x8
#define MAG3110_OFF_X_MSB_REG_ADDR    0x9
#define MAG3110_OFF_X_LSB_REG_ADDR    0xa
#define MAG3110_OFF_Y_MSB_REG_ADDR    0xb
#define MAG3110_OFF_Y_LSB_REG_ADDR    0xc
#define MAG3110_OFF_Z_MSB_REG_ADDR    0xd
#define MAG3110_OFF_Z_LSB_REG_ADDR    0xe
#define MAG3110_DIE_TEMP_REG_ADDR     0xf
#define MAG3110_CTRL_REG1_ADDR        0x10
#define MAG3110_CTRL_REG2_ADDR        0x11


#define MAG3110_X_AXIS_ID  0x1
#define MAG3110_Y_AXIS_ID  0x2
#define MAG3110_Z_AXIS_ID  0x3

#define MAG3110_OUTPUT_CORRECTED  1
#define MAG3110_OUTPUT_UNCORRECTED  2

#define MAG3110_DEV_MODE_STANDBY            0x1
#define MAG3110_DEV_MODE_ACTIVE_RAW         0x2
#define MAG3110_DEV_MODE_ACTIVE_CORRECTED   0x3


#define MAG3110_SYSMOD_REG_SYSMOD0_BIT  0x1
#define MAG3110_SYSMOD_REG_SYSMOD1_BIT  0x2

#define MAG3110_CTRL_REG1_DR_FIELD_BIT_MSK   0xe0
#define MAG3110_CTRL_REG1_ADC_RATE_FIELD_BIT_MSK   MAG3110_CTRL_REG1_DR_FIELD_BIT_MSK
#define MAG3110_CTRL_REG1_OSR_FIELD_BIT_MSK  0x18
#define MAG3110_CTRL_REG1_FR_FIELD_BIT_MSK   0x04
#define MAG3110_CTRL_REG1_TM_FIELD_BIT_MSK   0x02
#define MAG3110_CTRL_REG1_AC_FIELD_BIT_MSK   0x01

#define MAG3110_CTRL_REG1_DR_FIELD_BIT_SHIFT   5
#define MAG3110_CTRL_REG1_ADC_RATE_FIELD_BIT_SHIFT  MAG3110_CTRL_REG1_DR_FIELD_BIT_SHIFT
#define MAG3110_CTRL_REG1_OSR_FIELD_BIT_SHIFT  3
#define MAG3110_CTRL_REG1_FR_FIELD_BIT_SHIFT   2
#define MAG3110_CTRL_REG1_TM_FIELD_BIT_SHIFT   1
#define MAG3110_CTRL_REG1_AC_FIELD_BIT_SHIFT   0

#define MAG3110_CTRL_REG2_AUTO_MRST_EN_FIELD_BIT_MSK  0x80
#define MAG3110_CTRL_REG2_RAW_FIELD_BIT_MSK  0x20
#define MAG3110_CTRL_REG2_MAG_RST_FIELD_BIT_MSK  0x10

// Output data rate = ADC_RATE / Over sampling ratio

#define MAG3110_ADC_RATE_1280_HZ_ENC    0x0
#define MAG3110_ADC_RATE_0640_HZ_ENC    0x1
#define MAG3110_ADC_RATE_0320_HZ_ENC    0x2
#define MAG3110_ADC_RATE_0160_HZ_ENC    0x3
#define MAG3110_ADC_RATE_0080_HZ_ENC    0x4
#define MAG3110_ADC_RATE_0040_HZ_ENC    0x5
#define MAG3110_ADC_RATE_0020_HZ_ENC    0x6
#define MAG3110_ADC_RATE_0010_HZ_ENC    0x7

#define MAG3110_ADC_RATE_1280_HZ  1280
#define MAG3110_ADC_RATE_0640_HZ  640
#define MAG3110_ADC_RATE_0320_HZ  320
#define MAG3110_ADC_RATE_0160_HZ  160
#define MAG3110_ADC_RATE_0080_HZ  80
#define MAG3110_ADC_RATE_0040_HZ  40
#define MAG3110_ADC_RATE_0020_HZ  20
#define MAG3110_ADC_RATE_0010_HZ  10

#define MAG3110_OSR_016_ENC    0x0
#define MAG3110_OSR_032_ENC    0x1
#define MAG3110_OSR_064_ENC    0x2
#define MAG3110_OSR_128_ENC    0x3

#define MAG3110_OUTPUT_DATA_MODE_RAW  0x1
#define MAG3110_OUTPUT_DATA_MODE_CORRECTED  0x2

// Sensor data for all 3-axis sent together
#define MAG3110_SENSOR_OUTPUT_LEN   (2 + 2 + 2)

typedef struct
{
  UINT8_t devId;
  UINT8_t portId;
  UINT8_t portPinNr;
  UINT8_t i2cBusId;
  UINT8_t i2cDevAddr;
  UINT8_t dataMode;
  PLTFRM_snsrDevInfo_s info;
  UINT8_t adcRateEnc;
  UINT8_t osrEnc;
  UINT16_t tmoMilliSec;
} MAG3110_cntxt_s;

extern PLTFRM_sts_t MAG3110_init(UINT8_t devId);

extern PLTFRM_sts_t MAG3110_getMeasData(UINT8_t devId,
                                        UINT8_t axisId,
                                        SINT16_t *out_p);

extern PLTFRM_sts_t MAG3110_triggerMeas(UINT8_t devId,
		                                UINT16_t *tmoMilliSecs_p);

#endif
