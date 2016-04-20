/*
 * File Name : adxl345.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : April/1/2014
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

#ifndef __ADXL345_H__
#define __ADXL345_H__

#include <typedefs.h>
#include <i2c_sw.h>


#define ADXL345_DEV_ID  0xe5

#define ADXL345_ALT_ADDR_LOGIC_HIGH  0x1
#define ADXL345_ALT_ADDR_LOGIC_LOW   0x0


/*
 * With the ALT ADDRESS pin  high, the 7-bit I2C address for the device is 0x1D,
 * followed by the R/W bit. This translates to 0x3A for a write and 0x3B for a read.
 */
#define ADXL345_ALT_HIGH_I2C_ADDR_PREFIX  0x1d    // 001 1101 (7 bit address)

/*
 * An alternate I2C address of 0x53 (followed by the R/W bit)  can be chosen by
 * grounding the ALT ADDRESS pin (Pin 12). This translates to 0xA6 for a write and
 * 0xA7 for a read.
 */
#define ADXL345_ALT_LOW_I2C_ADDR_PREFIX   0x53    // 101 0011 (7 bit address)


#define ADXL345_X_AXIS  0x1
#define ADXL345_Y_AXIS  0x2
#define ADXL345_Z_AXIS  0x3

#define ADXL345_X_AXIS_DATA_OFF  0
#define ADXL345_Y_AXIS_DATA_OFF  2
#define ADXL345_Z_AXIS_DATA_OFF  4

#define ADXL345_ACCEL_DATA_REG_CNT (3*2)


typedef enum
{
  /*
   * The device is in 10-bit mode, and the range bits determine the maximum
   * g range and scale factor. For example, when range is +/- 2g, scale factor
   * is 4000mg/1024 -> 3.9 mg/LSB. Here 1 mg is g/1000.
   */
  ADXL345_RESOLUTION_10_BITS_FIXED = 0,

  /*
   * The device is in full resolution mode, where the output resolution increases
   * with the g range set by the range bits to maintain a 4 mg/LSB scale factor.
   * When range is +/- 2g, 4000 mg/4 -> 1000 -> 10 bits
   * When range is +/- 4g, 8000 mg/4 -> 2000 -> resolution increases to 11 bits
   * When range is +/- 8g, 16000 mg/4 -> 4000 -> resolution increases to 12 bits
   * When range is +/- 16g, 32000 mg/4 -> 8000 -> resolution increases to 13 bits
   */
  ADXL345_RESOLUTION_FULL_SCALE = 1,

  ADXL345_RESOLUTION_UNKNOWN = 2
} ADXL345_resolution_t;


typedef enum
{
   // +/-2g, 10-bit resolution
   // -2g to +2g -> 4g
   // 4000 mg/1024 -> 3.9 mg/LSB
   ADXL345_RANGE_2G = 0,

   // +/-4g, 10-bit resolution
   // -4g to +4g -> 8g
   // 8000 mg/1024 -> 7.8 mg/LSB
   ADXL345_RANGE_4G = 1,

   // +/-8g, 10-bit resolution
   // -8g to +8g -> 16g
   // 16000 mg/1024 -> 15.6 mg/LSB
   ADXL345_RANGE_8G = 2,

   // +/-16g, 10-bit resolution
   // -16g to +16g -> 32g
   // 32000 mg/1024 -> 31.2 mg/LSB
   ADXL345_RANGE_16G = 3,

   ADXL345_RANGE_UNKNOWN = 7
} ADXL345_accelRange_t;


typedef enum
{
  /*
   * In bypass mode, FIFO is not operational and, therefore, remains empty.
   */
  ADXL345_FIFO_MODE_BYPASS = 0x0,

  ADXL345_FIFO_MODE_FIFO = 0x1,

  ADXL345_FIFO_MODE_STREAM = 0x2,

  ADXL345_FIFO_MODE_TRIGGER = 0x3
} ADXL345_fifoMode_t;


typedef enum
{
   ADXL345_ACCEL_DATA_RIGHT_JUSTIFIED = 0,
   ADXL345_ACCEL_DATA_LEFT_JUSTIFIED = 1,
   ADXL345_ACCEL_DATA_JUSTIFICATION_UNKNOWN = 7
} ADXL345_dataLayoutMode_t;


typedef enum
{
   ADXL345_OUTPUT_DATA_RATE_0PT10_HZ = 0,
   ADXL345_OUTPUT_DATA_RATE_0PT20_HZ = 1,
   ADXL345_OUTPUT_DATA_RATE_0PT39_HZ = 2,
   ADXL345_OUTPUT_DATA_RATE_0PT78_HZ = 3,
   ADXL345_OUTPUT_DATA_RATE_1PT56_HZ = 4,
   ADXL345_OUTPUT_DATA_RATE_3PT13_HZ = 5,
   ADXL345_OUTPUT_DATA_RATE_6PT25_HZ = 6,
   ADXL345_OUTPUT_DATA_RATE_12PT5_HZ = 7,
   ADXL345_OUTPUT_DATA_RATE_25_HZ = 8,
   ADXL345_OUTPUT_DATA_RATE_50_HZ = 9,
   ADXL345_OUTPUT_DATA_RATE_100_HZ = 10,
   ADXL345_OUTPUT_DATA_RATE_200_HZ = 11,
   ADXL345_OUTPUT_DATA_RATE_400_HZ = 12,
   ADXL345_OUTPUT_DATA_RATE_800_HZ = 13,
   ADXL345_OUTPUT_DATA_RATE_1600_HZ = 14,
   ADXL345_OUTPUT_DATA_RATE_3200_HZ = 15
} ADXL345_outputDataRate_t;


// Table 19
// Register MAP

#define ADXL345_DEVID_REG_ADDR           0x00
#define ADXL345_THRESH_TAP_REG_ADDR      0x1d
#define ADXL345_OFSX_REG_ADDR            0x1e
#define ADXL345_OFSY_REG_ADDR            0x1f
#define ADXL345_OFSZ_REG_ADDR            0x20
#define ADXL345_DUR_REG_ADDR             0x21
#define ADXL345_LATENT_REG_ADDR          0x22
#define ADXL345_WINDOW_REG_ADDR          0x23
#define ADXL345_THRESH_ACT_REG_ADDR      0x24
#define ADXL345_THRESH_INACT_REG_ADDR    0x25
#define ADXL345_TIME_INACT_REG_ADDR      0x26
#define ADXL345_ACT_INACT_CTL_REG_ADDR   0x27
#define ADXL345_ACT_TAP_STATUS_REG_ADDR  0x2b
#define ADXL345_BW_RATE_REG_ADDR         0x2c
#define ADXL345_POWER_CTL_REG_ADDR       0x2d
#define ADXL345_INT_ENABLE_REG_ADDR      0x2e
#define ADXL345_INT_MAP_REG_ADDR         0x2f
#define ADXL345_INT_SOURCE_REG_ADDR      0x30
#define ADXL345_DATA_FORMAT_REG_ADDR     0x31
#define ADXL345_DATAX0_REG_ADDR          0x32
#define ADXL345_DATAX1_REG_ADDR          0x33
#define ADXL345_DATAY0_REG_ADDR          0x34
#define ADXL345_DATAY1_REG_ADDR          0x35
#define ADXL345_DATAZ0_REG_ADDR          0x36
#define ADXL345_DATAZ1_REG_ADDR          0x37
#define ADXL345_FIFO_CTL_REG_ADDR        0x38
#define ADXL345_FIFO_STATUS_REG_ADDR     0x39


// POWER_CTL (R/W)
#define ADXL345_POWER_CTL_REG_LINK_BIT         0x20
#define ADXL345_POWER_CTL_REG_AUTO_SLEEP_BIT   0x10
#define ADXL345_POWER_CTL_REG_MEASURE_BIT      0x08
#define ADXL345_POWER_CTL_REG_SLEEP_BIT        0x04
#define ADXL345_POWER_CTL_REG_WAKEUP_BIT_MSK   0x03

// FIFO_CTL
#define ADXL345_FIFO_CTL_FIFO_MODE_BIT_MSK          0xc0
#define ADXL345_FIFO_CTL_FIFO_MODE_BIT_SHIFT        0x06
#define ADXL345_FIFO_CTL_FIFO_MODE_BIT_MSK_SHIFTED  0x03


// DATA_FORMAT
#define ADXL345_DATA_FORMAT_RANGE_BIT_MSK  0x3
#define ADXL345_DATA_FORMAT_JUSTIFY_BIT_MSK  0x4
#define ADXL345_DATA_FORMAT_FULL_RES_BIT_MSK  0x8


// BW_RATE
#define ADXL345_BW_RATE_REG_RATE_BIT_MSK  0xf
#define ADXL345_BW_RATE_REG_LOW_POWER_BIT  0x10

typedef struct
{
   UINT8_t devId;
   UINT8_t altAddrPinVal;   // 1 - high, 0 - low
   UINT8_t i2cAddrPrefix;   // 8 bit (LSB is R/W bit)

   ADXL345_accelRange_t range;
   ADXL345_resolution_t resolution;
   ADXL345_dataLayoutMode_t dataLayOutMode;  // left justified or right justified
} ADXL345_cntxt_s;

extern PLTFRM_sts_t ADXL345_setOutputRate(UINT8_t devId,
                                          ADXL345_outputDataRate_t opRate);

extern PLTFRM_sts_t ADXL345_setResolution(UINT8_t devId,
                                          ADXL345_resolution_t res);

extern PLTFRM_sts_t ADXL345_readAccelData(UINT8_t devId,
                                          SINT16_t *xAccel_p,
                                          SINT16_t *yAccel_p,
                                          SINT16_t *zAccel_p);

extern PLTFRM_sts_t ADXL345_init(UINT8_t devId);

extern PLTFRM_sts_t ADXL345_checkVibration(UINT8_t devId, UINT8_t *vibState_p);

extern PLTFRM_sts_t ADXL345_wakeUp(UINT8_t devId);

extern PLTFRM_sts_t ADXL345_putToSleep(UINT8_t devId);

extern PLTFRM_sts_t ADXL345_setRange(UINT8_t devId, const ADXL345_accelRange_t range);

extern PLTFRM_sts_t ADXL345_cfgFIFO(UINT8_t devId, ADXL345_fifoMode_t fifoMode);

extern PLTFRM_sts_t ADXL345_setDataLayoutMode(UINT8_t devId, ADXL345_dataLayoutMode_t layoutMode);

extern PLTFRM_sts_t ADXL345_startMeas(UINT8_t devId);

extern PLTFRM_sts_t ADXL345_stopMeas(UINT8_t devId);

#endif
