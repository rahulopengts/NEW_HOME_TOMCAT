/*
 * File Name : tmp102.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : 12/29/2013
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

#ifndef __TMP102_H__
#define __TMP102_H__

#include <typedefs.h>
#include <i2c_sw.h>


#define TMP102_OUTPUT_SCALE_CENTI

#ifdef TMP102_OUTPUT_SCALE_MILLI
#define TMP102_SENSOR_OUTPUT_VALUE_SZ   4
#elif defined(TMP102_OUTPUT_SCALE_CENTI)
#define TMP102_SENSOR_OUTPUT_VALUE_SZ   2
#else
#error Sensor output scale not specified !!
#endif

/*
 * The TMP102 features an address pin to allow up to four devices to
 * be addressed on a single bus. Table 12 describes the pin logic
 * levels used to properly connect up to four devices.
 */
#define TMP102_I2C_ADDR_PREFIX  0x48    // 0 1001 xxx

/*
 * Table 2. Pointer Addresses
 */
#define TMP102_TEMP_REG_ADDR      0x0   // RO
#define TMP102_CONFIG_REG_ADDR    0x1   // RW
#define TMP102_LOW_TH_REG_ADDR    0x2   // RW
#define TMP102_HIGH_TH_REG_ADDR   0x3   // RW

#define TMP102_TEMP_REG_SZ  0x2
#define TMP102_CONFIG_REG_SZ  0x2
#define TMP102_LOW_TH_REG_SZ  0x2
#define TMP102_HIGH_TH_REG_SZ  0x2


/*
 * Config register description
 * Table 7
 */
#define TMP102_CONFIG_REG_EM_BIT  (1 << 4)
#define TMP102_CONFIG_REG_AL_BIT  (1 << 5)
#define TMP102_CONFIG_REG_CR0_BIT (1 << 6)
#define TMP102_CONFIG_REG_CR1_BIT (1 << 7)

#define TMP102_CONFIG_REG_SD_BIT  (1 << 0)
#define TMP102_CONFIG_REG_TM_BIT  (1 << 1)
#define TMP102_CONFIG_REG_POL_BIT (1 << 2)
#define TMP102_CONFIG_REG_F0_BIT  (1 << 3)
#define TMP102_CONFIG_REG_F1_BIT  (1 << 4)
#define TMP102_CONFIG_REG_R0_BIT  (1 << 5)
#define TMP102_CONFIG_REG_R1_BIT  (1 << 6)
#define TMP102_CONFIG_REG_OS_BIT  (1 << 7)



/*
 * Bit D0 of byte 2 indicates Normal mode (EM bit = '0') or Extended
 * mode (EM bit = '1') and can be used to distinguish between the two
 * temperature register data formats.
 */
#define TMP102_TEMP_REG_MODE_BIT  (1 << 0)


/*
 * The TMP102 has a typical conversion time of 26ms. To achieve
 * different conversion rates, the TMP102 makes a conversion and
 * after that powers down and waits for the appropriate delay set
 * by CR1 and CR0.
 */
#define TMP102_CONVERSION_TIME_MILLISECS   26

typedef enum
{
  TMP102_DATA_FMT_MODE_UNKNOWN =  0,
  TMP102_DATA_FMT_MODE_NORMAL = 1,   // EM - 0 (12 bit data)
  TMP102_DATA_FMT_MODE_EXTENDED = 2  // EM - 1 (13 bit data)
} TMP102_dataFmtMode_t;

/*
 * The conversion rate bits, CR1 and CR0, configure the TMP102 for
 * conversion rates of 8Hz, 4Hz, 1Hz, or 0.25Hz. The default rate
 * is 4Hz.
 */
typedef enum
{
   TMP102_CONV_RATE_QUARTER_HZ = 0,
   TMP102_CONV_RATE_ONE_HZ = 1,
   TMP102_CONV_RATE_FOUR_HZ = 2,
   TMP102_CONV_RATE_EIGHT_HZ = 3,
} TMP102_convRate_t;


/*
 * < SHUTDOWN MODE (SD) >
 * The Shutdown mode bit saves maximum power by shutting down all device
 * circuitry other than the serial interface, reducing current consumption
 * to typically less than 0.5 micro-amps. Shutdown mode is enabled when the
 * SD bit is '1'; the device shuts down when current conversion is completed.
 * When SD is equal to '0', the device maintains a continuous conversion state.
 *
 * After power-up or general-call reset, the TMP102 immediately starts a conversion.
 */
typedef enum
{
   TMP102_OPN_MODE_UNKNOWN = 0,
   TMP102_OPN_MODE_ONE_SHOT = 1,
   TMP102_OPN_MODE_PERIODIC = 2
} TMP102_opnMode_t;


typedef struct
{
   UINT8_t devId;
   TMP102_opnMode_t opnMode;
   TMP102_dataFmtMode_t dataFmtMode;
} TMP102_cntxt_s;


extern PLTFRM_sts_t TMP102_init(UINT8_t devId);

extern PLTFRM_sts_t TMP102_startSingleConv(UINT8_t devId,
                                           SINT32_t *tempVal_p,
                                           UINT16_t *tmoVal_p);

extern PLTFRM_sts_t TMP102_getTempVal(UINT8_t devId,
#ifdef TMP102_OUTPUT_SCALE_MILLI
                                      SINT32_t *tempVal_p
#else
                                      SINT16_t *tempVal_p);
#endif


extern PLTFRM_sts_t TMP102_readReg(TMP102_cntxt_s *devCntxt_p,
                                   UINT8_t ptrRegVal,
                                   UINT8_t *rdBuff_p);

extern PLTFRM_sts_t TMP102_writeReg(TMP102_cntxt_s *devCntxt_p,
                                    UINT8_t ptrRegVal,
                                    UINT8_t *wrBuff_p);

#endif
