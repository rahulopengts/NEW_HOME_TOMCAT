/*
 * File Name : ina219.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : april/25/2015
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

#ifndef __INA219_H__
#define __INA219_H__

#include <typedefs.h>
#include <pltfrm.h>
#include <i2c_sw.h>

#define IN219_BUS_VOLTAGE_OUTPUT_VAL_SZ  2
#define IN219_SHUNT_VOLTAGE_OUTPUT_VAL_SZ   2


#define INA219_REG_LEN  2

typedef enum
{
   INA219_BUS_VOLTAGE_RANGE_UNKNOWN,
   INA219_BUS_VOLTAGE_RANGE_16V,
   INA219_BUS_VOLTAGE_RANGE_32V
} INA219_busVRCfg_t;

typedef enum
{
   INA219_MODE_POWER_DOWN,
   INA219_MODE_SHUNT_VOLTAGE_TRIGGERED,
   INA219_MODE_BUS_VOLTAGE_TRIGGERED,
   INA219_MODE_SHUNT_AND_BUS_TRIGGERED,
   INA219_MODE_ADC_OFF,
   INA219_MODE_SHUNT_VOLTAGE_CONTINUOUS,
   INA219_MODE_BUS_VOLTAGE_CONTINUOUS,
   INA219_MODE_SHUNT_AND_BUS_CONTINUOUS,  // default
} INA219_mode_t;


/*
 * INA219 register list.
 */
#define INA219_CONF_REG_ADDR       0x0   // R/W
#define INA219_SHUNT_V_REG_ADDR    0x1   // RO
#define INA219_BUS_V_REG_ADDR      0x2   // RO
#define INA219_POWER_REG_ADDR      0x3   // RO
#define INA219_CURRENT_REG_ADDR    0x4   // RO
#define INA219_CAL_REG_ADDR        0x4   // RW

#define INA219_SV_RANGE_SETTING_40_MV   0x0   // +/- 40 mV
#define INA219_SV_RANGE_SETTING_80_MV   0x1   // +/- 80 mV
#define INA219_SV_RANGE_SETTING_160_MV   0x2   // +/- 160 mV
#define INA219_SV_RANGE_SETTING_320_MV   0x3   // +/- 320 mV


/*
 * 7.4.2 Configuration register
 */
#define INA219_CONF_REG_POR_VAL   0x399f


#define INA219_CONF_REG_RST_BIT_MSK   0x8000
#define INA219_CONF_REG_BRNG_BIT_MSK  0x4000
#define INA219_CONF_REG_PG_BIT_MSK    0x1800
#define INA219_CONF_REG_BADC_BIT_MSK  0x0780
#define INA219_CONF_REG_SADC_BIT_MSK  0x0078
#define INA219_CONF_REG_MODE_BIT_MSK  0x0007

#define INA219_CONF_REG_RST_BIT_SHIFT  15
#define INA219_CONF_REG_BRNG_BIT_SHIFT  13
#define INA219_CONF_REG_PG_BIT_SHIFT    11
#define INA219_CONF_REG_BADC_BIT_SHIFT  7
#define INA219_CONF_REG_SADC_BIT_SHIFT  3
#define INA219_CONF_REG_MODE_BIT_SHIFT  0


typedef struct
{
   UINT8_t devId1;  // Bus voltage sensor
   UINT8_t devId2;  // shunt voltage sensor
   UINT8_t i2cDevId;
   UINT8_t i2cBusId;
   UINT8_t busVRCfg;
   UINT8_t mode;
   UINT8_t shuntVRCfg;
   PLTFRM_snsrDevInfo_s info;
} INA219_cntxt_s;



extern PLTFRM_sts_t INA219_init(UINT8_t devId);
extern PLTFRM_sts_t INA219_readBusVoltage(UINT8_t devId, SINT16_t *voltage_p);
extern PLTFRM_sts_t INA219_readShuntVoltage(UINT8_t devId, SINT16_t *voltage_p);


#endif
