/*
 * File Name : sfh_7773.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : 10/31/2013
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

#ifndef __SFH_7773_H__
#define __SFH_7773_H__

#include <typedefs.h>
#include <i2c_sw.h>

/*
 * ALS - Ambient light sensor
 * PS - Proximity sensor
 */

/*
 * Looks like there can be only one SFH 7773 on a bus at address
 * 000b.
 */
#define SFH_7773_I2C_ADDR_PREFIX   0x38   // 0 0111 000

#define SFH_7773_PART_NUMBER_ID  0x9

#define SFH_7773_SENSOR_OUTPUT_VALUE_SZ   4

/*
 * SFH 7773 register list
 */

#define SFH_773_PART_NR_AND_REV_REG_ADDR         0x8a    // R/O
#define SFH_773_MFR_ID_REG_ADDR                  0x8b    // R/O
#define SFH_773_INTERRUPT_REG_ADDR               0x92    // R/W
#define SFH_7773_SW_RST_CTRL_REG_ADDR            0x80    // R/W

#define SFH_7773_ALS_CTRL_REG_ADDR               0x80    // R/W
#define SFH_7773_ALS_MCU_TRIGGER_REG_ADDR        0x84    // R/W
#define SFH_7773_ALS_TIME_INTERVAL_REG_ADDR      0x86    // R/W
#define SFH_7773_ALS_DATA_LSB_REG_ADDR           0x8c    // R/O
#define SFH_7773_ALS_DATA_MSB_REG_ADDR           0x8d    // R/O
#define SFH_773_ALS_DATA_STATUS_REG_ADDR         0x8e    // R/O
#define SFH_773_ALS_UPPER_THLD_LSB_REG_ADDR      0x96    // R/W
#define SFH_773_ALS_UPPER_THLD_MSB_REG_ADDR      0x97    // R/W
#define SFH_773_ALS_LOWER_THLD_LSB_REG_ADDR      0x98    // R/W
#define SFH_773_ALS_LOWER_THLD_MSB_REG_ADDR      0x99    // R/W


#define SFH_7773_PS_CTRL_REG_ADDR                0x81    // R/W
#define SFH_7773_EMITTER_CURRENT_CTRL_REG_ADDR   0x82    // R/W
#define SFH_7773_PS_MCU_TRIGGER_REG_ADDR         0x84    // R/W
#define SFH_7773_PS_TIME_INTERVAL_REG_ADDR       0x86    // R/W
#define SFH_7773_PS_DATA_STATUS_REG_ADDR         0x8e    // R/O
#define SFH_7773_PS_MEAS_DATA_REG_ADDR           0x8f    // R/O
#define SFH_7773_PS_THLD_LEVEL_REG_ADDR          0x93    // R/W

#define SFH_7773_ALS_OPN_MODE_TRANSITION_TIME    10   // In milliseconds

#define SFH_7773_ALS_DFLT_INTEGRATION_TIME   100     // In milliseconds

#define SFH_7773_ALS_MIN_INTEGRATION_TIME    10     // In milliseconds
#define SFH_7773_ALS_MAX_INTEGRATION_TIME    1000   // In milliseconds

#define SFH_7773_PS_MIN_INTEGRATION_TIME    100     // In microseconds
#define SFH_7773_PS_MAX_INTEGRATION_TIME    2500    // In microseconds

#define SFH_7773_STANDBY_CURRENT_TYPICAL   2   // In micro amps
#define SFH_7773_STANDBY_CURRENT_MAX       2   // In micro amps

// Register 0x80 (RST_AND_ALS_CTRL_REG_ADDR)
#define SFH_7773_ALS_MODE_BIT_MSK   0x3
#define SFH_7773_SW_RESET_TRIGGER_BIT_MSK  0x4

// Register 0x84
#define SFH_7773_ALS_MEAS_TRIGGER_BIT_MSK  0x2
#define SFH_7773_PS_MEAS_TRIGGER_BIT_MSK   0x1

// Register 0x8a
#define SFH_7773_PART_NR_SHIFT  4
#define SFH_7773_PART_NR_SHIFT_BIT_MSK  0xf

#define SFH_7773_ALS_OPN_MODE_STAND_BY_0_ENC     0x0
#define SFH_7773_ALS_OPN_MODE_STAND_BY_1_ENC     0x1
#define SFH_7773_ALS_OPN_MODE_TRIGGERED_ENC      0x2
#define SFH_7773_ALS_OPN_MODE_FREE_RUNNING_ENC   0x3

// Register 0x81 (PS_CTRL_REG_ADDR)
#define SFH_7773_PS_OPN_MODE_STAND_BY_0_ENC     0x0
#define SFH_7773_PS_OPN_MODE_STAND_BY_1_ENC     0x1
#define SFH_7773_PS_OPN_MODE_TRIGGERED_ENC      0x2
#define SFH_7773_PS_OPN_MODE_FREE_RUNNING_ENC   0x3



typedef enum
{
   SFH_7773_OPN_MODE_UNKNOWN,       // Implementation specific

   /*
    * IDD is below 2mA and the device is inactive. Other units may use
    * the I2C bus without any restrictions; I/O pins and INT are in Z state.
    * There is no sink current through the LED
    */
   SFH_7773_OPN_MODE_OFF,

   /*
    * This is the initial mode after power-up. IDD is below 5mA. No
    * measurement is performed. Device can be activated by I2C bus
    * communication. Data registers can be read and written. The data
    * will be stored in the registers when the device goes from TRIGGERED
    * or FREE-RUNNING to STAND-BY.
    */
   SFH_7773_OPN_MODE_STAND_BY,

   /*
    * Every measurement is separately initiated by MCU. This mode
    * can be used for ambient light sensor and proximity sensor.
    * Measurement data are available in the registers after a
    * defined delay time.
    */
   SFH_7773_OPN_MODE_TRIGGERED,

   /*
    * Measurements are triggered internally by SFH7773. This mode can
    * also be used for ambient light sensor and proximity sensor. The
    * measurement repetition rate and current through the LED is defined
    * by MCU. Measurement results can be read from the data register, the
    * status from the interrupt register.
    */
   SFH_7773_OPN_MODE_FREE_RUNNING
} SFH_7773_mode_t;


typedef struct
{
   UINT8_t devId;
   SFH_7773_mode_t opnMode;
} SFH_7773_cntxt_s;


extern PLTFRM_sts_t SFH_7773_init(UINT8_t devId);

extern PLTFRM_sts_t SFH_7773_swReset(UINT8_t devId);

extern PLTFRM_sts_t SFH_7773_startALSConv(UINT8_t devId,
                                          SINT32_t *luxVal_p,
                                          UINT16_t *tmoVal_p);

extern PLTFRM_sts_t SFH_7773_cfgALSOpnMode(UINT8_t devId,
                                           SFH_7773_mode_t reqOpnMode,
                                           UINT16_t *opnTime_p);

extern SFH_7773_mode_t SFH_7773_getOpnMode(UINT8_t devId);

extern PLTFRM_sts_t SFH_7773_getALSOutput(UINT8_t devId,
                                          SINT32_t *luxVal_p);


#endif
