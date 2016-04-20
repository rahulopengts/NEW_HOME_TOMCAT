
#ifndef __PLTFRM_H__
#define __PLTFRM_H__
/*
 * File Name : pltfrm.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : 8/12/2013
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

#include <stdlib.h>   
#ifdef __IAR__
#include "io430.h"
#else
#include <msp430.h>
#endif
#include <msp430.h>
#include <typedefs.h>


#define PLTFRM_I2C_BUS_CNT   1
#define PLTFRM_MAX_I2C_DEV_CNT  12

#define MSP430_START_UP_CLK_MHZ  1  // ~ 1.1 MHz

#define PLTFRM_SUPPLY_VOLTAGE_SAMPLE_CNT_SHIFT  1   // (1 << 2 -> 4)
#define PLTFRM_BATT_VOLTAGE_SAMPLE_CNT_SHIFT  1   // (1 << 2 -> 4)

#define PLTFRM_SUPPLY_VOLTAGE_MON_INTERVAL_SECS  5

#if defined(PWR_SRC_SOLAR_SUPERCAP) || defined(PWR_SRC_SOLAR_BATT)
#ifdef PLTFRM_STM1061N25_ENA
#define PLTFRM_MON_IC_THRESHOLD_VOLTAGE  2500
#define PLTFRM_MIN_SUPPLY_VOLTAGE_IN_MV  \
	    (PLTFRM_MON_IC_THRESHOLD_VOLTAGE + 600)
#define PLTFRM_LOW_VOLTAGE_TH_IN_MV (PLTFRM_MON_IC_THRESHOLD_VOLTAGE + 100)
#elif defined(PLTFRM_STM1061N27_ENA)
#define PLTFRM_MON_IC_THRESHOLD_VOLTAGE  2700
#define PLTFRM_MIN_SUPPLY_VOLTAGE_IN_MV  \
	    (PLTFRM_MON_IC_THRESHOLD_VOLTAGE + 400)
#define PLTFRM_LOW_VOLTAGE_TH_IN_MV (PLTFRM_MON_IC_THRESHOLD_VOLTAGE + 100)
#else
#error Voltage mon chip not specified !!
#endif
#endif


// AT24MAC602 size - 256 bytes (0x00 - 0xff)
#define AT24MAC602_FE_ID_FIELD_OFFSET  0
#define AT24MAC602_FE_ID_FIELD_LEN  SYSTEM_FATAL_ERROR_ID_LEN

#define AT24MAC602_APP_CFG_DATA_OFFSET  16
#define AT24MAC602_APP_CFG_DATA_LEN  32

#define AT24MAC602_MAC_CFG_DATA_OFFSET  \
        (AT24MAC602_APP_CFG_DATA_OFFSET + AT24MAC602_APP_CFG_DATA_LEN)
#define AT24MAC602_MAC_CFG_DATA_LEN  16

#define AT24MAC602_PHY_CFG_DATA_OFFSET  \
        (AT24MAC602_MAC_CFG_DATA_OFFSET + AT24MAC602_MAC_CFG_DATA_LEN)
#define AT24MAC602_PHY_CFG_DATA_LEN  16

#define AT24MAC602_NM_CFG_DATA_OFFSET  \
        (AT24MAC602_PHY_CFG_DATA_OFFSET + AT24MAC602_PHY_CFG_DATA_LEN)
#define AT24MAC602_NM_CFG_DATA_LEN  8

#ifndef MICRO_ONLY
#ifdef PLTFRM_AT24MAC602_ENA
#define NVM_FE_ID_FIELD_OFFSET  AT24MAC602_FE_ID_FIELD_OFFSET
#define NVM_FE_ID_FIELD_LEN  AT24MAC602_FE_ID_FIELD_LEN

#define NVM_APP_CFG_DATA_OFFSET  AT24MAC602_APP_CFG_DATA_OFFSET
#define NVM_APP_CFG_DATA_LEN  AT24MAC602_APP_CFG_DATA_LEN

#define NVM_MAC_CFG_DATA_OFFSET  AT24MAC602_MAC_CFG_DATA_OFFSET
#define NVM_MAC_CFG_DATA_LEN  AT24MAC602_MAC_CFG_DATA_LEN

#define NVM_PHY_CFG_DATA_OFFSET  AT24MAC602_PHY_CFG_DATA_OFFSET
#define NVM_PHY_CFG_DATA_LEN  AT24MAC602_PHY_CFG_DATA_LEN

#define NVM_NM_CFG_DATA_OFFSET  AT24MAC602_NM_CFG_DATA_OFFSET
#define NVM_NM_CFG_DATA_LEN  AT24MAC602_NM_CFG_DATA_LEN

#define PLTFRM_NVM_DEV_ID  PLTFRM_AT24MAC602_1_DEV_ID

#else
#error No NVM on board !!
#endif
#endif

#ifdef EZ430_RF2500
#define PLTFRM_MAX_NODE_SENSOR_CNT  1
#else
#define PLTFRM_MAX_NODE_SENSOR_CNT  8
#endif

// Interrupt capable ports (Port 1 and Port 2)
#define PLTFRM_INT_CAP_GPIO_PORT_COUNT  2




#ifdef __MSP430G2553__
#define PLTFRM_GPIO_PORT_COUNT  2
#elif defined(__MSP430G2955__)
#define PLTFRM_GPIO_PORT_COUNT  4
#elif defined(__MSP430F2274__)
#define PLTFRM_GPIO_PORT_COUNT  4
#else
#error micro variant not specified !!
#endif

#define PLTFRM_GPIO_PORT_SIZE  8


typedef enum
{
   PLTFRM_GPIO_INT_EDGE_NA = 0,
   PLTFRM_GPIO_INT_EDGE_POSITIVE = 1,
   PLTFRM_GPIO_INT_EDGE_NEGATIVE = 2
} PLTFRM_gpioIntEdge_t;

typedef enum
{
   PLTFRM_GPIO_PIN_DIRECTION_INPUT = 0,
   PLTFRM_GPIO_PIN_DIRECTION_OUTPUT = 1
} PLTFRM_gpioDirn_t;

typedef enum
{
   PLTFRM_GPIO_PORT_1 = 0,
   PLTFRM_GPIO_PORT_2 = 1,
   PLTFRM_GPIO_PORT_3 = 2,
   PLTFRM_GPIO_PORT_4 = 3
} PLTFRM_gpioPortId_t;

typedef enum
{
   PLTFRM_GPIO_OUTPUT_LOW = 0,
   PLTFRM_GPIO_OUTPUT_HIGH = 1,
   PLTFRM_GPIO_OUTPUT_NA = 2   // Not applicable
} PLTRM_gpioOutVal_t;

typedef enum
{
   PLTFRM_GPIO_INPUT_LOW = 0,
   PLTFRM_GPIO_INPUT_HIGH = 1
} PLTRM_gpioInVal_t;

typedef enum
{
   PLTFRM_GPIO_INPUT_PULL_DOWN,
   PLTFRM_GPIO_INPUT_PULL_UP
} PLTFRM_gpioPullType_t;

typedef enum
{
   PLTFRM_GPIO_PORT_PIN_0,
   PLTFRM_GPIO_PORT_PIN_1,
   PLTFRM_GPIO_PORT_PIN_2,
   PLTFRM_GPIO_PORT_PIN_3,
   PLTFRM_GPIO_PORT_PIN_4,
   PLTFRM_GPIO_PORT_PIN_5,
   PLTFRM_GPIO_PORT_PIN_6,
   PLTFRM_GPIO_PORT_PIN_7
} PLTFRM_gpioPortPin_t;

typedef enum
{
   PLTFRM_ENABLE_GPIO_PIN_INT,
   PLTFRM_DISABLE_GPIO_PIN_INT
} PLTFRM_gpioIntCtrlOpn_t;

typedef enum
{
   PLTFRM_GLOBAL_INT_ENABLED,
   PLTFRM_GLOBAL_INT_DISABLED
} PLTFRM_globalIntState_t;

#define PLTFRM_disGPIOInt(portId, portPinNr) \
        PLTFRM_gpioPinIntCtrlOpn(portId, portPinNr, PLTFRM_DISABLE_GPIO_PIN_INT, 0x0)

#define PLTFRM_enaGPIOInt(portId, portPinNr, clearIntFlag) \
        PLTFRM_gpioPinIntCtrlOpn(portId, portPinNr, PLTFRM_ENABLE_GPIO_PIN_INT, clearIntFlag)

#define PLTFRM_gpioEnablePullDown(portId, portPinNr) \
		PLTFRM_gpioEnablePull(portId, portPinNr, PLTFRM_GPIO_INPUT_PULL_DOWN)

#define PLTFRM_gpioEnablePullUp(portId, portPinNr) \
		PLTFRM_gpioEnablePull(portId, portPinNr, PLTFRM_GPIO_INPUT_PULL_UP)

// On chip sensing of Vcc
#define PLTFRM_ON_CHIP_VCC_SENSOR_OUTPUT_LEN   2
#define PLTFRM_ON_CHIP_VCC_SENSOR_ACTIVE_TIME_MICROSECS   125

// On chip temperature sensor
#define PLTFRM_ON_CHIP_TEMP_SENSOR_OUTPUT_LEN  2
#define PLTFRM_ON_CHIP_TEMP_SENSOR_ACTIVE_TIME_MICROSECS   125

// A simple switch (such as a light switch). The two terminals of
// this switch are either connected (closed) or not connected (open).
#define PLTFRM_SPST_SWITCH_STATE_SZ  0x1

#define PLTFRM_BATT_VOLT_OUTPUT_LEN  2

typedef enum
{
   PLTFRM_SPST_SWITCH_STATE_OPEN,
   PLTFRM_SPST_SWITCH_STATE_CLOSED
} PLTFRM_spstSwitchState_t;


#ifdef __MSP430G2955__
typedef enum
{
   PLTFRM_ADC10_CHANN_0_ID,   // A0 (p2.0)
   PLTFRM_ADC10_CHANN_1_ID,   // A1 (p2.1)
   PLTFRM_ADC10_CHANN_2_ID,   // A2 (p2.2)
   PLTFRM_ADC10_CHANN_3_ID,   // A3 (p2.3)
   PLTFRM_ADC10_CHANN_4_ID,   // A4 (p2.4)
   PLTFRM_ADC10_CHANN_5_ID,   // A5 (p2.5)
   PLTFRM_ADC10_CHANN_6_ID,   // A6 (p2.6)
   PLTFRM_ADC10_CHANN_7_ID,   // A7 (p2.7)
   PLTFRM_ADC10_CHANN_8_ID,   // VeREF+
   PLTFRM_ADC10_CHANN_9_ID,   // VREF-/VeREF-
   PLTFRM_ADC10_CHANN_10_ID,  // Temperature Sensor
   PLTFRM_ADC10_CHANN_11_ID   // (Vcc - Vss) / 2
} PLTFRM_adc10ChannId_t;
#elif defined(__MSP430F2274__)
typedef enum
{
   PLTFRM_ADC10_CHANN_0_ID,   // A0 (p2.0)
   PLTFRM_ADC10_CHANN_1_ID,   // A1 (p2.1)
   PLTFRM_ADC10_CHANN_2_ID,   // A2 (p2.2)
   PLTFRM_ADC10_CHANN_3_ID,   // A3 (p2.3)
   PLTFRM_ADC10_CHANN_4_ID,   // A4 (p2.4)
   PLTFRM_ADC10_CHANN_5_ID,   // A5 (p3.0)
   PLTFRM_ADC10_CHANN_6_ID,   // A6 (p3.6)
   PLTFRM_ADC10_CHANN_7_ID,   // A7 (p3.7)
   PLTFRM_ADC10_CHANN_8_ID,   // VeREF+
   PLTFRM_ADC10_CHANN_9_ID,   // VREF-/VeREF-
   PLTFRM_ADC10_CHANN_10_ID,  // Temperature Sensor
   PLTFRM_ADC10_CHANN_11_ID   // (Vcc - Vss) / 2
} PLTFRM_adc10ChannId_t;
#elif defined(__MSP430G2553__)
#else
#error not supported !!
#endif

#define MSP_ADC10_REF_V_2_PT_5  2500
#define MSP_ADC10_REF_V_1_PT_5  1500

/*
 * Software selectable on-chip reference voltage generation (1.5 V or 2.5 V)
 */
typedef enum
{
   PLTFRM_ADC10_REF_GEN_1PT5_VOLTS,
   PLTFRM_ADC10_REF_GEN_2PT5_VOLTS,
   PLTFRM_ADC10_REF_VCC
} PLTFRM_adc10RefGenV_t;

typedef enum
{
   PLTFRM_ADC10_SH_CYCLE_COUNT_4 = 0,
   PLTFRM_ADC10_SH_CYCLE_COUNT_8,
   PLTFRM_ADC10_SH_CYCLE_COUNT_16,
   PLTFRM_ADC10_SH_CYCLE_COUNT_64
} PLTFRM_adc10SHCycles_t;


// --------------------- Devices --------------------------

#define PLTFRM_DUMMY_DEV_ID        0x0

#define PLTFRM_EXT_VOLT_MON_OUTPUT_LEN  2

#define PLTFRM_LOW_PRESSURE_LEVEL_TIME_VALUE_SZ   2

#define PLTFRM_EXT_VOLTAGE_MON_OP_LEN  2

// Serial EEPROM
#define PLTFRM_AT24C01C_1_DEV_ID    0x1           // I2C
#define PLTFRM_AT24C01C_1_I2C_ADDR  0x0  

// Serial EEPROM with EUI-64 and 128 bit serial number
#define PLTFRM_AT24MAC602_DEV_CNT  1
#define PLTFRM_AT24MAC602_1_DEV_ID    0x2
#define PLTFRM_AT24MAC602_1_I2C_ADDR  0x0

#define PLTFRM_M24M01_1_DEV_ID  0x4
#define PLTFRM_M24M01_DEV_CNT  1
   
// Temperature sensors   
#define PLTFRM_LM75B_1_DEV_ID       0x9            // I2C
#define PLTFRM_LM75B_1_I2C_ADDR     0x0
#define PLTFRM_LMP75B_DEV_CNT       0x1            // Number of such devices in the system

#define PLTFRM_TMP102_1_DEV_ID      0xa            // I2C
#define PLTFRM_TMP102_1_I2C_ADDR    0x0            // A0 pin connected to ground
#define PLTFRM_TMP102_DEV_CNT       0x1

#define PLTFRM_NTCALUG02A_1_DEV_ID   0xb       // Vishay - NTCALUG02A (analog)
#define PLTFRM_NTCALUG02A_2_DEV_ID   0xc       //
#define PLTFRM_NTC_THERMISTOR_DEV_CNT    0x2

// Light sensor
#define PLTFRM_SFH_7773_1_DEV_ID    0x11           // I2C
#define PLTFRM_SFH_7773_1_I2C_ADDR  0x0
#define PLTFRM_SFH_7773_DEV_CNT     0x1            // Number of such devices in the system

// Light sensor
#define PLTFRM_TSL45315_1_DEV_ID    0x12           // I2C
#define PLTFRM_TSL45315_1_I2C_ADDR  0x0
#define PLTFRM_TSL45315_DEV_CNT     0x1            // Number of such devices in the system
   
// Gyro + Accelerometer + Temp
#define PLTFRM_MPU6050_1_DEV_ID     0x13           // I2C
#define PLTFRM_MPU6050_1_I2C_ADDR   0x0            // LSB is 0 (AD0 is 0)
#define PLTFRM_MPU6050_DEV_CNT      0x1            // Number of such devices in the system

// Accelerometer
#define PLTFRM_ADXL345_1_DEV_ID     0x14           // I2C and SPI
#define PLTFRM_ADXL345_DEV_CNT      0x1            // Number of such devices in the system

// SHT-10 (humidity + temperature)
#define PLTFRM_SHT10_1_TEMP_DEV_ID       0x15           // I2C
#define PLTFRM_SHT10_DEV_CNT        0x1

// rkris@wisense.in / july/4/15
#define PLTFRM_DHT11_1_DEV_ID       0x16
#define PLTFRM_DHT11_DEV_CNT        0x1

#define LIS3MDLTR_I2C_ADDR_PREFIX   0x1c           // 0 0011100
// LIS3MDLTR (Magnetometer)
#define PLTFRM_LIS3MDLTR_1_DEV_ID   0x16           // I2C
#define PLTFRM_LIS3MDLTR_1_I2C_ADDR (LIS3MDLTR_I2C_ADDR_PREFIX | 0x0)  // 00111x0 , x is 0
#define PLTFRM_LIS3MDLTR_DEV_CNT    0x1


#define PLTFRM_LDR_1_DEV_ID         0x17           // Analog
#define PLTFRM_LDR_DEV_CNT          1

#define PLTFRM_SHT10_1_RH_DEV_ID       0x18           // I2C

// RADIO
#define PLTFRM_CC2520_1_DEV_ID      0x19           // SPI
#define PLTFRM_CC1200_1_DEV_ID      0x1a           // SPI
#define PLTFRM_CC1101_1_DEV_ID      0x1b           // SPI
#define PLTFRM_CC3100_1_DEV_ID      0x1c           // SPI
#define PLTFRM_CC2500_1_DEV_ID      0x1d           // SPI

// Pressure sensor
#define PLTFRM_MPL115A1_1_DEV_ID    0x20           // SPI

#define PLTFRM_MAX_SONAR_DEV_CNT    1              // MAXBOTIX ultrasonic
#define PLTFRM_MAX_SONAR_1_DEV_ID   0x28


#define PLTFRM_EKMC160111X_1_DEV_ID   0x30         // GPIO
#define PLTFRM_EKMC160111X_DEV_CNT    0x1          // Number of such devices in the system

#define PLTFRM_GEN_VOLT_GEN_DEV_ID    0x38
#define PLTFRM_CURR_MON_DEV_ID        0x39
#define PLTFRM_EXT_VOLTAGE_MON_DEV_ID  0x3a


// LEDs
#define PLTFRM_LED_1_DEV_ID           0x40         // GPIO
#define PLTFRM_LED_2_DEV_ID           0x41         // GPIO


#define PLTFRM_BAR_CODE_SCANNER_1_DEV_ID   0x48

// Switches (On/Off)
#define PLTFRM_REED_SWITCH_1_DEV_ID   0x50         // GPIO
#define PLTFRM_REED_SWITCH_DEV_CNT    0x1

#define PLTFRM_SPST_SWITCH_1_DEV_ID   0x51         // GPIO
#define PLTFRM_SPST_SWITCH_DEV_CNT    0x1

#define PLTFRM_ACS712_1_CURRENT_DEV_ID   0x54      // Analog
#define PLTFRM_ACS712_2_CURRENT_DEV_ID   0x55      // Analog
#define PLTFRM_ACS712_3_CURRENT_DEV_ID   0x56      // Analog
#define PLTFRM_ACS712_DEV_CNT  3

#define PLTFRM_INA219_1_BV_DEV_ID     0x58         // I2C   INA219 bus voltage
#define PLTFRM_INA219_1_SV_DEV_ID     0x59         // I2C   INA219 shunt voltage
#define PLTFRM_INA219_2_BV_DEV_ID     0x5a         // I2C   INA219 bus voltage
#define PLTFRM_INA219_2_SV_DEV_ID     0x5b         // I2C   INA219 shunt voltage
#define PLTFRM_INA219_3_BV_DEV_ID     0x5c         // I2C   INA219 bus voltage
#define PLTFRM_INA219_3_SV_DEV_ID     0x5d         // I2C   INA219 shunt voltage
#define PLTFRM_INA219_4_BV_DEV_ID     0x5e         // I2C   INA219 bus voltage
#define PLTFRM_INA219_4_SV_DEV_ID     0x5f         // I2C   INA219 shunt voltage
#define PLTFRM_INA219_DEV_CNT         0x4

// Analog Pressure Sensor
#define PLTFRM_MP3V5050GP_1_DEV_ID    0x60         // Analog
#define PLTFRM_MP3V5050GP_DEV_CNT     0x1

#define PLTFRM_HE055T01_1_DEV_ID      0x61         // Analog
#define PLTFRM_HE055T01_DEV_CNT       0x1

#define PLTFRM_MP3V5004GP_1_DEV_ID    0x62         // Analog
#define PLTFRM_MP3V5004GP_DEV_CNT     0x1

#define PLTFRM_MPXV5010G_1_DEV_ID     0x63         // Analog
#define PLTFRM_MPXV5010G_DEV_CNT      1

#define PLTFRM_MP3V5010_1_DEV_ID      0x64
#define PLTFRM_MP3V5010_DEV_CNT       1

#define PLTFRM_LLS_1_DEV_ID           0x65
#define PLTFRM_LLS_DEV_CNT            0x1

#define PLTFRM_LOW_PRESSURE_TRACKER_LOGICAL_DEV  0x66

#define PLTFRM_BATT_1_DEV_ID          0x68         // Analog
#define PLTFRM_BATT_DEV_CNT           0x1

#define PLTFRM_4_20_CURRENT_SENSOR_ID    0x69       // Analog
#define PLTFRM_4_20_CURRENT_SENSOR_CNT   0x1

// AD7797 (Bridge sensor)
#define PLTFRM_AD7797_1_DEV_ID        0x70
#define PLTFRM_AD7797_DEV_CNT         0x1          // SPI bus

#define PLTFRM_ON_CHIP_VCC_SENSOR_DEV_ID    0x78      // ADC_10 channel
#define PLTFRM_ON_CHIP_VCC_SENSOR_DEV_CNT   0x1

#define PLTFRM_ON_CHIP_TEMP_SENSOR_DEV_ID    0x79      // ADC_10 channel
#define PLTFRM_ON_CHIP_TEMP_SENSOR_DEV_CNT   0x1

#define PLTFRM_SYNC_RT_1_DEV_ID  0x80
#define PLTFRM_SYNC_RT_DEV_CNT  0x1

#define PLTFRM_MAG3110_1_DEV_ID  0x88
#define PLTFRM_MAG3110_DEV_CNT  0x1

#define PLTFRM_GPIO_REMOTE_CTRL_DEV_ID  0x90

#define PLTFRM_CHIRP_PWLA_1_DEV_ID  0x91
#define PLTFRM_CHIRP_PWLA_DEV_CNT 1

#define PLTFRM_FC_28_1_DEV_ID  0x92  // Analog (soil moisture sensor)
#define PLTFRM_FC_28_DEV_CNT  1

// June/28/2015 - rkris@wisense.in
#define PLTFRM_WSMS100_1_DEV_ID  0x93
#define PLTFRM_WSMS100_DEV_CNT 1

// CC2D33S measures both RH and temperature
#define PLTFRM_CC2D33S_1_RH_DEV_ID      0xb0  // I2C
#define PLTFRM_CC2D33S_1_TEMP_DEV_ID    0xb1  // I2C
#define PLTFRM_CC2D33S_DEV_CNT     0x1

#define PLTFRM_MPL3115A2_1_DEV_ID  0xb8  // I2C
#define PLTFRM_MPL3115A2_DEV_CNT   0x1

#define PLTFRM_MPL115A2_1_DEV_ID  0xb9  // I2C
#define PLTFRM_MPL115A2_DEV_CNT  0x1

#define PLTFRM_BMP180_1_DEV_ID  0xba  // I2C
#define PLTFRM_BMP180_DEV_CNT  0x1

#define PLTFRM_MMA7660FC_1_DEV_ID  0xc0  // I2C
#define PLTFRM_MMA7660FC_DEV_CNT   0x1

#define PLTFRM_MDS_1_DEV_ID  0xca   // Digital (Mains detection sensor)
#define PLTFRM_MDS_DEV_CNT  0x1

#define PLTFRM_VIBRATION_SNSR_1_DEV_ID  0xcc
#define PLTFRM_VIBRATION_SNSR_DEV_CNT  0x1

#define PLTFRM_MQ5_1_DEV_ID  0xc8  // Analog
#define PLTFRM_MQ5_DEV_CNT  0x1




#define PLTFRM_I2C_SW_BUS_1_DEV_ID  0xe0
#define PLTFRM_I2C_SW_BUS_2_DEV_ID  0xe1

#define PLFRM_SPI_HW_BUS_1_DEV_ID   0xe8

#define PLTFRM_WS_VEH_DET_1_DEV_ID  0xea  // Veh detection (digital)

// Dummy device ids
#define PLTFRM_COORD_KA_DEV_ID   0xfc
#define PLTFRM_EXT_IF_KA_DEV_ID  0xfd

#define PLTFRM_32KHZ_CRYSTAL_DEV_ID  0xfe

#define PLTFRM_GENERIC_DEV_ID      0xff

// --------------------------------------------------------- 
   
   
// ------------- Buses (I2C, SPI, 1-WIRE etc) --------------   
// I2C (software)
#define PLTFRM_I2C_SW_BUS_1_ID      0x80

   
// I2C (hardware) 
#define PLFRM_I2C_HW_BUS_1_ID      0x88

#define PLTFRM_INV_I2C_BUS_ID      0xff
#define PLTFRM_INV_I2C_ADDR        0xff
 
// SPI (hardware)
#define PLFRM_SPI_HW_BUS_1_ID      0x90

// 1 WIRE (software)
#define PLTFRM_1WIRE_SW_BUS_1_ID   0x98
   
// UART
#define PLTFRM_UART_HW_1_DEV_ID    0xa0
// ---------------------------------------------------------   
   

   

// ---------------------------------------------------------
// Layout of EEPROM (device id 0x1)
// ---------------------------------------------------------

#define PLTFRM_AT24C01C_1_OFF_MAC_EXT_ADDR         0x0
#define PLTFRM_AT24C01C_1_OFF_MAC_EXT_ADDR_CKSUM   8
   
#define PLTFRM_MAC_EXT_ADDR_CKSUM_LEN   0x2

#define PLTFRM_AT24C01C_1_OFF_PHY_CHAN_NR          (PLTFRM_AT24C01C_1_OFF_MAC_EXT_ADDR_CKSUM  \
                                                    + PLTFRM_MAC_EXT_ADDR_CKSUM_LEN)


// ---------------------------------------------------------   


#define PLTFRM_4_20_CURRENT_OUTPUT_VALUE_SZ  4

   
extern volatile UINT16_t PLTFRM_globalEvtMsk;
      
#define PLTRM_TIMER_A0_ACTIVE_BIT  (1 << 0)
#define PLTRM_TIMER_A1_ACTIVE_BIT  (1 << 1)   
#define PLTRM_TIMER_1HZ_PERIODIC_ACTIVE_BIT  (1 << 2)
#define PLTRM_TIMER_WAKEUP_ACTIVE_BIT  (1 << 3)

#define PLTFRM_intDisable( ) \
do \
{  \
   __bic_SR_register(GIE); \
} while (0)

#define PLTFRM_intSaveDisable(flag) \
do \
{  \
   UINT8_t stsReg = __bic_SR_register(GIE); \
   flag = (stsReg & GIE) ? 0x1 : 0x0; \
} while (0)


#define PLTFRM_intRestore(flag) \
do \
{  \
   if (flag) \
   { \
       __bis_SR_register(GIE); \
   } \
} while (0)

#define PLTFRM_intEnable( ) \
do \
{  \
   __bis_SR_register(GIE); \
} while (0)


#define PLTFRM_saveIntStateAndDisable(intState) \
do \
{  \
   UINT16_t statusReg = _get_SR_register(); \
   intState = statusReg & GIE ? \
              PLTFRM_GLOBAL_INT_ENABLED : PLTFRM_GLOBAL_INT_DISABLED; \
   __bic_SR_register(GIE); \
} while (0)

#define PLTFRM_restoreIntState(savedIntState) \
do \
{  \
   if (savedIntState == PLTFRM_GLOBAL_INT_ENABLED) \
       __bis_SR_register(GIE); \
   else \
	   __bic_SR_register(GIE); \
} while (0)

#define PLTFRM_enterLPM( )  \
do \
{  \
   _BIS_SR(LPM3_bits + GIE); \
} while (0)

#define PLTFRM_exitLPM( )  \
do \
{  \
   _BIC_SR_IRQ(LPM3_bits); \
} while (0)

#if 1
#if defined(WISENSE_HW_REV_2)  || defined(WISENSE_HW_REV_3)

#define PLTFRM_TOGGLE_LED_1()  \
do                             \
{                              \
   P2OUT ^= 0x1;               \
} while (0)

#elif defined(WISENSE_HW_REV_4)

#define PLTFRM_TOGGLE_LED_1()  \
do                             \
{                              \
   P3OUT ^= 0x10;              \
} while (0)

#define PLTFRM_TOGGLE_LED_2()  \
do                             \
{                              \
   P3OUT ^= 0x40;               \
} while (0)

#define PLTFRM_ON_LED_1()   P3OUT |= 0x10
#define PLTFRM_ON_LED_2()   P3OUT |= 0x40

#define PLTFRM_OFF_LED_1()   P3OUT &= ~(0x10)
#define PLTFRM_OFF_LED_2()   P3OUT &= ~(0x40)

#elif defined(WISENSE_HW_REV_5)

#define PLTFRM_TOGGLE_LED_1()  \
do                             \
{                              \
   P4OUT ^= BIT7;              \
} while (0)

#define PLTFRM_TOGGLE_LED_2()  \
do                             \
{                              \
   P3OUT ^= BIT6;               \
} while (0)

#define PLTFRM_ON_LED_1()   P4OUT |= BIT7
#define PLTFRM_ON_LED_2()   P3OUT |= BIT6

#define PLTFRM_OFF_LED_1()   P4OUT &= ~(BIT7)
#define PLTFRM_OFF_LED_2()   P3OUT &= ~(BIT6)

#elif defined(WISENSE_HW_REV_6)

#ifdef PLTFRM_OFF_BOARD_LEDS_ENA
#define PLTFRM_TOGGLE_LED_1()  \
do                             \
{                              \
   P1OUT ^= BIT2;              \
} while (0)

#define PLTFRM_TOGGLE_LED_2()  \
do                             \
{                              \
   P1OUT ^= BIT3;               \
} while (0)

#define PLTFRM_ON_LED_1()   P1OUT |= BIT2
#define PLTFRM_ON_LED_2()   P1OUT |= BIT3

#define PLTFRM_OFF_LED_1()   P1OUT &= ~(BIT2)
#define PLTFRM_OFF_LED_2()   P1OUT &= ~(BIT3)

#else
#define PLTFRM_TOGGLE_LED_1()  \
do                             \
{                              \
   P3OUT ^= BIT0;              \
} while (0)

#define PLTFRM_TOGGLE_LED_2()  \
do                             \
{                              \
   P4OUT ^= BIT5;               \
} while (0)

#define PLTFRM_ON_LED_1()   P3OUT |= BIT0
#define PLTFRM_ON_LED_2()   P4OUT |= BIT5

#define PLTFRM_OFF_LED_1()   P3OUT &= ~(BIT0)
#define PLTFRM_OFF_LED_2()   P4OUT &= ~(BIT5)
#endif

#elif defined(EZ430_RF2500)

#define PLTFRM_TOGGLE_LED_1()  P1OUT ^= BIT0
#define PLTFRM_TOGGLE_LED_2()  P1OUT ^= BIT1

#define PLTFRM_ON_LED_1()   P1OUT |= BIT0
#define PLTFRM_ON_LED_2()   P1OUT |= BIT1

#define PLTFRM_OFF_LED_1()  P1OUT &= ~(BIT0)
#define PLTFRM_OFF_LED_2()  P1OUT &= ~(BIT1)

#else

#define PLTFRM_TOGGLE_LED_1()  \
do                             \
{                              \
} while (0)

#endif
#endif


#define PLTFRM_TIMER_TYPE_PERIODIC  0x1
#define PLTFRM_TIMER_TYPE_ONE_SHOT  0x2

// IAR compiler - returns a random number in the range 0 to RAND_MAX (32767)
#define pltfrm_randU16( )   rand()

#ifdef __MSP430G2955__

#define PLTFRM_startTimerA0(milliSecs, microSecs, cbFunc_p) \
        msp430g2553_startTimerA0((milliSecs),  \
                                 (microSecs),  \
                                 (PLTFRM_timerCbFunc_t)(cbFunc_p))
          
#define PLTFRM_startTimerA1(secs, MilliSecs, cbFunc_p) \
        msp430g2553_startTimerA1((secs),  \
                                 (MilliSecs),  \
                                 (PLTFRM_timerCbFunc_t)(cbFunc_p))

#define PLTFRM_start1HzPeriodicTimer(cbFunc_p) \
        msp430g2553_startWDTimer((PLTFRM_timerCbFunc_t)(cbFunc_p))          
          
#define PLTFRM_stopTimerA0(  )  msp430g2553_stopTimerA0( )
          
#define PLTFRM_stopTimerA1(  )  msp430g2553_stopTimerA1( )
          
#define PLTFRM_stop1HzPeriodicTimer(  )  msp430g2553_stopWDTimer( )
          
#elif defined(__MSP430F2274__)

#define PLTFRM_startTimerA0(milliSecs, microSecs, cbFunc_p) \
        msp430g2553_startTimerA0((milliSecs),  \
                                 (microSecs),  \
                                 (PLTFRM_timerCbFunc_t)(cbFunc_p))

#define PLTFRM_startTimerA1(secs, MilliSecs, cbFunc_p) \
        msp430g2553_startTimerA1((secs),  \
                                 (MilliSecs),  \
                                 (PLTFRM_timerCbFunc_t)(cbFunc_p))

#define PLTFRM_start1HzPeriodicTimer(cbFunc_p)   PLTFRM_STS_SUCCESS


#define PLTFRM_stopTimerA0(  )   msp430g2553_stopTimerA0( )


#define PLTFRM_stopTimerA1(  ) \
do { \
} while (0)

#define PLTFRM_stop1HzPeriodicTimer(  ) \
do { \
} while (0)
#elif defined(__MSP430G2553__)
#else
#error micro not supported !!
#endif

#define PLTFRM_getTimeStamp(time_p) \
do { \
} while (0)

/*
 * WDTCTL is a 16-bit, password-protected, read/write register. Any read
 * or write access must use word instructions and write accesses must
 * include the write password 05Ah in the upper byte. Any write to WDTCTL
 * with any value other than 05Ah in the upper byte is a security key
 * violation and triggers a PUC system reset regardless of timer mode.
 */
#define PLTFRM_mcuReset()  \
do {                       \
   WDTCTL = ~WDTPW;        \
} while (1)

/*
 * Setting WDTCNTCL = 1 clears the count value to 0000h. WDTCNTCL is
 * automatically reset.
 */
#define PLTFRM_kickWatchDog( )   \
do {                             \
   WDTCTL = (WDTPW + WDTCNTCL + WDTSSEL);  \
} while (0)


// CPU_ACTIVE_CLOCK_IN_MHZ is fixed at compile time
// For now, the MSP always runs at a fixed clock rate
// when in active mode.

#define PLTFRM_delay1MicroSecs()  \
        __delay_cycles(1*CPU_ACTIVE_CLOCK_IN_MHZ)

#define PLTFRM_delay5MicroSecs()  \
        __delay_cycles(5*CPU_ACTIVE_CLOCK_IN_MHZ)

#define PLTFRM_delay10MicroSecs()  \
        __delay_cycles(10*CPU_ACTIVE_CLOCK_IN_MHZ)

#define PLTFRM_delay50MicroSecs()  \
        __delay_cycles(50*CPU_ACTIVE_CLOCK_IN_MHZ)

#define PLTFRM_delay80MicroSecs() \
		__delay_cycles(80*CPU_ACTIVE_CLOCK_IN_MHZ)

#define PLTFRM_delay100MicroSecs()  \
        __delay_cycles(100*CPU_ACTIVE_CLOCK_IN_MHZ)

#define PLTFRM_delay500MicroSecs()  \
        __delay_cycles(500*CPU_ACTIVE_CLOCK_IN_MHZ)

#define PLTFRM_delay625MicroSecs() \
        __delay_cycles(625*CPU_ACTIVE_CLOCK_IN_MHZ)

#define PLTFRM_delay312MicroSecs() \
        __delay_cycles(312*CPU_ACTIVE_CLOCK_IN_MHZ)

#define PLTFRM_delay1000MicroSecs()  \
        __delay_cycles(1000*CPU_ACTIVE_CLOCK_IN_MHZ)

#define PLTFRM_delay1250MicroSecs()  \
        __delay_cycles(1250*CPU_ACTIVE_CLOCK_IN_MHZ)

#define PLTFRM_delay1MilliSec()  \
        __delay_cycles(1000*CPU_ACTIVE_CLOCK_IN_MHZ)

#define PLTFRM_SYS_FATAL_ID_MSK  0x1f


typedef enum
{
	PLTFRM_DEVICE_STATE_UNINITIALIZED = 1,
	PLTFRM_DEVICE_STATE_READY =  2
}PLTFRM_devState_t;


typedef enum
{
   PLTFRM_STS_SUCCESS = 0,
   PLTFRM_STS_OPN_IN_PROGRESS = 1,
   PLTFRM_STS_INV_DEV_ID = 2,
   PLTFRM_STS_TIMER_ALREADY_ACTIVE = 3,
   PLTFRM_STS_INV_PARAM = 4,
   PLTFRM_STS_I2C_BUS_WR_ERROR = 5,
   PLTFRM_STS_I2C_BUS_RD_ERROR = 6,
   PLTFRM_STS_DEV_REG_UPDATE_ERR = 7,
   PLTFRM_STS_DEV_STATE_UNKNOWN = 8,
   PLTFRM_STS_DEV_NOT_READY = 9,
   PLTFRM_STS_DEV_MISSING = 10,
   PLTFRM_STS_DEV_PART_NR_MISMATCH = 11,
   PLTFRM_STS_UNKNOWN_DEV_OPN_MODE = 12,
   PLTFRM_STS_INV_HW_CONN = 13,
   PLTFRM_STS_FEATURE_NOT_ENA = 14,
   PLTFRM_STS_RESOURCE_CONFLICT = 15,
   PLTFRM_STS_INV_HW_RESOURCE = 16,
   PLTFRM_STS_NO_PENDING_EVT = 17,
   PLTFRM_STS_INV_DEV_OPN_MODE = 18,
   PLTFRM_STS_OPN_FAILED = 19,
   PLTFRM_STS_DEV_DATA_NOT_READY = 20,
   PLTFRM_STS_DEV_INV_OPN_MODE = 21,
   PLTFRM_STS_PARAM_OUT_OF_RANGE = 22,
   PLTFRM_STS_DEV_CFG_NOT_SUPPORTED = 23,
   PLTFRM_STS_FRAME_TOO_LONG_FOR_TX_FIFO = 24,
   PLTFRM_STS_NOT_ENOUGH_SPACE_TX_FIFO = 25,
   PLTFRM_STS_RADIO_RX_FIFO_ERR = 26,
   PLTFRM_STS_OOM = 27,
   PLTFRM_STS_HW_REG_IS_READ_ONLY = 28,
   PLTFRM_STS_RADIO_TX_FIFO_NOT_EMPTY = 29,
   PLTFRM_STS_REG_NOT_FOUND = 30,
   PLTFRM_STS_SENSOR_LIMIT_REACHED = 31,
   PLTFRM_STS_DEV_UNINITIALIZED = 32,
   PLTFRM_STS_INV_I2C_BUS_ID = 33,
   PLTFRM_STS_I2C_ADDR_IN_USE = 34,
   PLTFRM_STS_I2C_DEV_LIST_FULL = 35,
   PLTFRM_STS_DEV_ID_REG_VAL_MISMATCH = 36,
   PLTFRM_STS_CC1101_RST_ERR = 37,
   PLTFRM_STS_DEV_NOT_INITIALIZED = 38,
   PLTFRM_STS_DEV_ALREADY_INITIALIZED = 39,
   PLTFRM_STS_DEV_DRIVER_INTERNAL_ERROR = 40,
   PLTFRM_STS_DEV_OP_ERR = 41,
   PLTFRM_STS_DEV_NOT_IN_SBY_MODE  = 42,
   PLTFRM_STS_NO_NEW_MEAS_DATA = 43,
   PLTFRM_STS_SNSR_OUTPUT_OVFL = 44
} PLTFRM_sts_t;

typedef UINT32_t PLTFRM_timeStamp_t;

typedef void (*PLTFRM_intCbFunc_t)(UINT8_t devId);

typedef struct
{
   UINT8_t devId;
   PLTFRM_intCbFunc_t cbFunc_p;
} PLTFRM_intCapGPIOPortPinInfo_s;

typedef struct
{
   UINT8_t intFlags;
   PLTFRM_intCapGPIOPortPinInfo_s pinInfo[PLTFRM_GPIO_PORT_SIZE];
} PLTFRM_intCapGPIOPortInfo_s;

typedef struct
{
   UINT8_t devId;
} PLTFRM_gpioPortPinInfo_s;

typedef struct
{
  PLTFRM_gpioPortPinInfo_s pinInfo[PLTFRM_GPIO_PORT_SIZE];
} PLTFRM_gpioPortInfo_s;

typedef struct
{
  const char *_name_p;
  const char *_mfr_p;
} PLTFRM_snsrDevInfo_s;

typedef struct
{
  UINT8_t devId;
  PLTFRM_snsrDevInfo_s *devInfo_p;
} PLTFRM_nodeSnsrListEntry_s;


typedef struct
{
  UINT8_t devId;
#if (PLTFRM_I2C_BUS_CNT > 1)
  UINT8_t busId;
#endif
  UINT8_t i2cAddr;
} PLTFRM_i2cBusListEntry_s;

extern UINT16_t PLTFRM_savedSysFEId;

typedef void (*PLTFRM_timerCbFunc_t)(void);   

extern void pltfrm_delay(UINT16_t microsecs);

extern PLTFRM_sts_t msp430g2553_startTimerA(UINT16_t milliSecs, 
                                            UINT16_t microSecs,
                                            PLTFRM_timerCbFunc_t cbFunc_p);

extern void PLTFRM_timerA0ExpHndlr(void);

extern void PLTFRM_timerA1ExpHndlr(void);

extern void PLTFRM_timer1HzPeriodicExpHndlr(void);

extern void PLTFRM_init(void);

extern PLTFRM_sts_t msp430g2553_startTimerA0(UINT16_t milliSecs, 
                                             UINT16_t microSecs,
                                             PLTFRM_timerCbFunc_t cbFunc_p);

extern PLTFRM_sts_t msp430g2553_startTimerA1(UINT16_t secs,
                                             UINT16_t milliSecs,
                                             PLTFRM_timerCbFunc_t cbFunc_p); 

extern void msp430g2553_start32KHzTimerA0(UINT8_t val, 
                                          PLTFRM_timerCbFunc_t cbFunc_p);


extern PLTFRM_sts_t msp430g2553_startWDTimer(PLTFRM_timerCbFunc_t cbFunc_p);

extern PLTFRM_sts_t PLTFRM_startWakeUpTimer(SINT32_t tmoSecs);

#define PLTFRM_stopWakeUpTimer()  msp430g2553_stopWDTimer()

extern void msp430g2553_stopTimerA0(void);

extern void msp430g2553_stopTimerA1(void);  

extern void msp430g2553_stopWDTimer(void);

extern SINT16_t PLTFRM_readOnChipTempSensor(void);

extern void PLTFRM_onChipTempSensorInit(void);

extern UINT32_t pltfrm_vccSense(UINT8_t shutDownADC);

extern PLTFRM_sts_t PLTFRM_getMacExtAddr(UINT8_t *extAddr_p);

extern UINT16_t PLTFRM_calcCkSum16(UINT8_t *buff_p, UINT8_t len);

extern void pltfrm_mcuReset(void);

extern UINT8_t PLTFRM_checkGPIOEvt(void);

extern PLTFRM_sts_t PLTFRM_reserveGPIOPin(UINT8_t devId,
                                          PLTFRM_gpioPortId_t portId,
                                          PLTFRM_gpioPortPin_t portPinNr);

extern PLTFRM_sts_t PLTFRM_allocGPIOPin(UINT8_t devId,
                                        PLTFRM_gpioPortId_t portId,
                                        PLTFRM_gpioPortPin_t portPinNr,
                                        PLTFRM_gpioDirn_t pinDirn,
                                        PLTRM_gpioOutVal_t initOutVal,
                                        PLTFRM_gpioIntEdge_t intEdge,
                                        PLTFRM_intCbFunc_t cbFunc_p);

extern PLTFRM_sts_t PLTFRM_gpioPinIntCtrlOpn(PLTFRM_gpioPortId_t portId,
                                             PLTFRM_gpioPortPin_t portPinNr,
                                             PLTFRM_gpioIntCtrlOpn_t opn,
                                             UINT8_t clearIntFlag);

extern PLTFRM_sts_t PLTFRM_gpioEnablePull(PLTFRM_gpioPortId_t portId,
                                          PLTFRM_gpioPortPin_t portPinNr,
                                          PLTFRM_gpioPullType_t pullType);

extern PLTFRM_sts_t PLTFRM_gpioCfgIntEdge(PLTFRM_gpioPortId_t portId,
                                          PLTFRM_gpioPortPin_t portPinNr,
                                          PLTFRM_gpioIntEdge_t edgeType);

extern PLTFRM_sts_t PLTFRM_getGPIOPinInput(PLTFRM_gpioPortId_t portId,
                                           PLTFRM_gpioPortPin_t portPinNr,
                                           UINT8_t *pinVal_p);

extern PLTFRM_sts_t PLTFRM_setGPIOPinOutput(PLTFRM_gpioPortId_t portId,
                                            PLTFRM_gpioPortPin_t portPinNr,
                                            UINT8_t pinVal);

extern PLTFRM_sts_t PLTFRM_getGPIOPortOp(PLTFRM_gpioPortId_t portId,
                                         UINT8_t *val_p);

#if 0
extern UINT32_t PLTFRM_adc10Sense(void);

extern PLTFRM_sts_t  PLTFRM_adc10Init(MSP430_adc10ChannId_t channelId,
                                      UINT16_t shTimeMicroSecs,
                                      MSP430_adc10RefV_t refV);
#endif

extern PLTFRM_sts_t PLTFRM_spiInit(UINT8_t devId);

extern void PLTFRM_outputSysFatalId(UINT8_t id);

extern PLTFRM_sts_t PLTFRM_regSnsrDev(UINT8_t devId, PLTFRM_snsrDevInfo_s *devInfo_p);

extern UINT8_t PLTFRM_getNodeSnsrCnt(void);
   
extern void PLTFRM_saveSysFEId(UINT16_t faultId);

extern void PLTFRM_resetSelf(void);

// rkris@wisense.in / jan/10/2015
extern PLTFRM_sts_t PLTFRM_regI2CDev(UINT8_t devId, UINT8_t busId, UINT8_t i2cAddr);

// rkris@wisense.in / jan/12/2015
extern UINT16_t PLTFRM_getAvgSupplyVoltage(void);

// rkris@wisense.in / jan/19/2015
extern PLTFRM_sts_t PLTFRM_doADC10SingleConv(UINT8_t adcChannId,
                                             UINT8_t refGenVEnc,
                                             UINT8_t waitCycleEnc,
                                             UINT16_t *adcVal_p);

extern PLTFRM_sts_t PLTFRM_allocADC10Chann(UINT8_t adcChannId,
		                                   UINT8_t devId);

// rkris@wisense.in / feb/1/2015
extern UINT8_t PLTFRM_getGPIOPinDevId(PLTFRM_gpioPortId_t portId,
                                      PLTFRM_gpioPortPin_t portPinNr);

extern PLTFRM_sts_t PLTFRM_getBattV(UINT16_t *battV_p);

// rkris@wisense.in / feb/6/2016
extern PLTFRM_sts_t PLTFRM_getLithiumSCBattV(UINT16_t *battV_p);

// rkris@wisense.in / march/6/2016
extern UINT32_t PLTFRM_perSecTickCnt;

#define PLTFRM_currTimeStamp( )  PLTFRM_perSecTickCnt

#define PLTFRM_secsElapsed(prevTS)  \
		(UINT32_t)(PLTFRM_currTimeStamp() - (prevTS))

#endif
