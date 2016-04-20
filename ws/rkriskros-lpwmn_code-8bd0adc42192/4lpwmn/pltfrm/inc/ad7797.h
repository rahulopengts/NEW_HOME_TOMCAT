/*
 * File Name : ad7797.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : Jan/16/2014
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

#ifndef __AD7797_H__
#define __AD7797_H__


#define AD7797_ADC_OUTPUT_VAL_SZ  4

#define AD7797_SENSOR_OUTPUT_VALUE_SZ  AD7797_ADC_OUTPUT_VAL_SZ

/*
 * The communication register is an 8-bit write-only register. All communication to
 * the part must start with a write operation to this register. The data written to
 * the communication register determines whether the next operation is a read or write
 * operation, and selects the register where this operation takes place.
 */
#define AD7797_COMM_REG_WEN_BM     (1 << 7)
#define AD7797_COMM_REG_RW_BM      (1 << 6)
#define AD7797_COMM_REG_RA_BM      ((1 << 5) | (1 << 4) | (1 << 3))
#define AD7797_COMM_REG_CREAD_BM   (1 << 2)

#define AD7797_COMM_REG_RA_SHIFT  3

#define AD7797_MODE_REG_H_MODE_SEL_SHIFT  5  // Bits 7, 6 and 5
#define AD7797_MODE_REG_H_MODE_SEL_SHIFT_BM  0x7
#define AD7797_MODE_REG_H_MODE_SEL_BM  0xe

#define AD7797_MODE_REG_MODE_SEL_BM  0xe000   // Bits 15, 14 and 13
#define AD7797_MODE_REG_MODE_SEL_SHIFT  13
#define AD7797_MODE_REG_MODE_SEL_SHIFT_BM  0x7


#define AD7797_STS_REG_ADDR         0x0   // 8 bits, r/o
#define AD7797_MODE_REG_ADDR        0x1   // 16 bits, r/w
#define AD7797_CFG_REG_ADDR         0x2   // 16 bits, r/w
#define AD7797_DATA_REG_ADDR        0x3   // 24 bits, r/o
#define AD7797_ID_REG_ADDR          0x4   // 8 bits, r/o
#define AD7797_OFFSET_REG_ADDR      0x6   // 16 bits, r/w
#define AD7797_FULL_SCALE_REG_ADDR  0x7   // 16 bits, r/w


/*
 * ID register (Value - 0x5b)
 * The identification number for the AD7796/AD7797 is stored in the
 * ID register. This is a read-only register.
 */
#define AD7797_ID_REG_VAL  0x5b


/*
 * Mode register
 */

#define AD7797_MODE_REG_L_CLK_SRC_BM  0xc  // Bit 7 and Bit 6
#define AD7797_MODE_REG_L_CLK_SRC_SHIFT  6
#define AD7797_MODE_REG_L_CLK_SRC_SHIFT_BM  0x3   // 2 bits

#define AD7797_MODE_REG_L_FSX_BM  0xf
#define AD7797_MODE_REG_L_FSX_SHIFT  0x0
#define AD7797_MODE_REG_L_FSX_SHIFT_BM  0xf

#define AD7797_MODE_REG_FSX_BM  0xf
#define AD7797_MODE_REG_FSX_SHIFT  0x0
#define AD7797_MODE_REG_FSX_SHIFT_BM  0xf

#define AD7797_MODE_REG_CLK_SRC_BM  0xc  // Bit 7 and Bit 6
#define AD7797_MODE_REG_CLK_SRC_SHIFT  6
#define AD7797_MODE_REG_CLK_SRC_SHIFT_BM  0x3   // 2 bits

/*
 * Status register.
 */
#define AD7797_STS_REG_RDY_BM  0x80   // Bit 7

/*
 * Config register.
 */
#define AD7797_CFG_REG_BO_BM  0x2000
#define AD7797_CFG_REG_BO_SHIFT  13
#define AD7797_CFG_REG_BO_SHIFT_BM  0x1

#define AD7797_CFG_REG_POLARITY_BM  0x1000
#define AD7797_CFG_REG_POLARITY_SHIFT  12
#define AD7797_CFG_REG_POLARITY_SHIFT_BM  0x1

#define AD7797_CFG_REG_CHAN_BM  0x7
#define AD7797_CFG_REG_CHAN_SHIFT  0
#define AD7797_CFG_REG_CHAN_SHIFT_BM  0x7

typedef enum
{
  AD7797_MODE_CONT_CONV = 0,
  AD7797_MODE_SINGLE_COV = 1,
  AD7797_MODE_IDLE = 2,
  AD7797_MODE_POWER_DOWN = 3,
  AD7797_MODE_INTERNAL_ZERO_SCALE_CAL = 4,
  AD7797_MODE_RSVD = 5,
  AD7797_MODE_ZERO_SCALE_CAL = 6,
  AD7797_MODE_FULL_SCALE_CAL = 7
} AD7797_opnMode_t;

typedef enum
{
  AD7797_UPDATE_RATE_123_HZ = 3,
  AD7797_UPDATE_RATE_62_HZ = 4,
  AD7797_UPDATE_RATE_50_HZ = 5,
  AD7797_UPDATE_RATE_33_POINT_2_HZ = 7,
  AD7797_UPDATE_RATE_16_POINT_7_HZ = 9,
  AD7797_UPDATE_RATE_12_POINT_5_HZ = 11,
  AD7797_UPDATE_RATE_10_HZ = 12,
  AD7797_UPDATE_RATE_8_POINT_33_HZ = 13,
  AD7797_UPDATE_RATE_6_POINT_25_HZ = 14,
  AD7797_UPDATE_RATE_4_POINT_17_HZ = 15
} AD7797_updateRate_t;

typedef enum
{
  // Internal 64 kHz Clock. Internal clock is not available at the CLK pin.
  AD7797_CLK_SRC_INT_64_KHZ = 0,

  // Internal 64 kHz Clock. This clock is made available at the CLK pin.
  AD7797_CLK_SRC_INT_64_KHZ_EXT_AVAIL = 1,

  // External 64 kHz Clock Used. An external clock gives better 50 Hz/60 Hz rejection.
  AD7797_CLK_SRC_EXT_64_KHZ = 2,

  // External Clock Used. The external clock is divided by 2 within the AD7796/AD7797.
  AD7797_CLK_SRC_EXT = 3
} AD7797_clkSrc_t;


/*
 * The analog input to the AD7796/AD7797 can accept either uni-polar or bi-polar input
 * voltage ranges.
 * The amplifier is configured to have a gain of 128. Therefore, with an external Vref
 * reference, the uni-polar range is 0 to Vref/128 Volts and the bi-polar range is
 * -Vref/128 to to +Vref/128 Volts.
 * For example, with an external 2.5 V reference, the uni-polar range is 0 mV to 20 mV
 * while the bi-polar range is -20 mV to +20 mV. Note that 2.5v/128 -> 19.53 mV ~ 20 mV.
 *
 * This implies that the differential input has to be within +/- Vref/128 V.
 */
typedef enum
{
   AD7797_ADC_INPUT_BI_POLAR,
   AD7797_ADC_INPUT_UNI_POLAR
} AD7797_adcInpPolarity_t;



/*
 * The amplifier is configured to have a gain of 128.
 */
#define AD7797_AMPLIFIER_GAIN_FACTOR  128

/*
 * When single conversion mode is selected, the ADC powers up and performs
 * a single conversion. The oscillator requires 1 ms to power up and settle.
 */
#define ADC7797_SINGLE_CONV_SETTLING_TIME_MSECS  1


typedef enum
{
   AD7797_CHANN_AINP_MINUS_AINN = 0,  // AIN+ - AIN-
   AD7797_CHANN_AINN_MINUS_AINN = 3,  // AIN- - AIN-
   AD7797_CHANN_TEMP_SENSOR = 6,
   AD7797_CHANN_AVDD_MON = 7
} AD7797_chann_t;

typedef struct
{
  UINT8_t devId;
  UINT8_t busId;
#ifdef AD7797_DIRECT_INTERFACE
  PLTFRM_gpioPortId_t portId;
  PLTFRM_gpioPortPin_t portPinNr;
#else
  PLTFRM_gpioPortId_t convTriggerPortId;
  PLTFRM_gpioPortId_t convTriggerPortPinNr;
#endif
  AD7797_clkSrc_t clkSrc;
  AD7797_updateRate_t updateRate;
  AD7797_adcInpPolarity_t polarity;
  AD7797_chann_t adcChann;
#ifdef AD7797_DIRECT_INTERFACE
  volatile UINT8_t *csReg_p;
#else
  volatile UINT8_t *conTriggerReg_p;
#endif
  UINT16_t adcVRefMv;  // ADC reference voltage in milli-volts
} AD7797_cntxt_s;

extern PLTFRM_sts_t AD7797_init(UINT8_t devId);

extern PLTFRM_sts_t AD7797_getADCVal(UINT8_t devId, SINT32_t *adcVal_p);

extern PLTFRM_sts_t AD7797_startSingleConv(UINT8_t devId, UINT16_t *tmo_p);

extern PLTFRM_sts_t AD7797_startSystemOffsetCal(UINT8_t devId, SINT32_t *offsetRegVal_p);

extern PLTFRM_sts_t AD7797_spiInit(UINT8_t devId);

#endif
