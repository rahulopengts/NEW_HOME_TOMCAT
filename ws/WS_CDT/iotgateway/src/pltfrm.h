/*
 * File Name : pltfrm.h
 * Author : ram krishnan
 * Created : 10/21/2013
 */

#ifndef __PLTFRM_H__
#define __PLTFRM_H__


// --------------------- Devices --------------------------

#define PLTFRM_DUMMY_DEV_ID        0x0

// Serial EEPROM
#define PLTFRM_AT24C01C_1_DEV_ID    0x1           // I2C
#define PLTFRM_AT24C01C_1_I2C_ADDR  0x0  

// Serial EEPROM with EUI-64 and 128 bit serial number
#define PLTFRM_AT24MAC602_DEV_CNT  1
#define PLTFRM_AT24MAC602_1_DEV_ID    0x2
#define PLTFRM_AT24MAC602_1_I2C_ADDR  0x0
   
// Temperature sensors   
#define PLTFRM_LM75B_1_DEV_ID       0x9            // I2C
#define PLTFRM_LM75B_1_I2C_ADDR     0x0
#define PLTFRM_LMP75B_DEV_CNT       0x1            // Number of such devices in the system

#define PLTFRM_TMP102_1_DEV_ID      0xa            // I2C
#define PLTFRM_TMP102_1_I2C_ADDR    0x0            // A0 pin connected to ground
#define PLTFRM_TMP102_DEV_CNT       0x1

#define PLTFRM_NTCALUG02A_1_DEV_ID   0xb       // Vishay - NTCALUG02A (analog)
#define PLTFRM_NTCALUG02A_2_DEV_ID   0xc       // Vishay - NTCALUG02A (analog)

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
#define PLTFRM_SHT10_1_RH_DEV_ID       0x18           // I2C
#define PLTFRM_SHT10_DEV_CNT        0x1

#define LIS3MDLTR_I2C_ADDR_PREFIX   0x1c           // 0 0011100
// LIS3MDLTR (Magnetometer)
#define PLTFRM_LIS3MDLTR_1_DEV_ID   0x16           // I2C
#define PLTFRM_LIS3MDLTR_1_I2C_ADDR (LIS3MDLTR_I2C_ADDR_PREFIX | 0x0)  // 00111x0 , x is 0
#define PLTFRM_LIS3MDLTR_DEV_CNT    0x1


// RADIO
#define PLTFRM_CC2520_1_DEV_ID      0x19           // SPI
#define PLTFRM_CC1200_1_DEV_ID      0x1a           // SPI
#define PLTFRM_CC1101_1_DEV_ID      0x1b           // SPI

// Pressure sensor
#define PLTFRM_MPL115A1_1_DEV_ID    0x20           // SPI

#define PLTFRM_MAX_SONAR_DEV_CNT    1              // MAXBOTIX ultrasonic
#define PLTFRM_MAX_SONAR_1_DEV_ID   0x28

#define PLTFRM_EKMC160111X_1_DEV_ID   0x30         // GPIO
#define PLTFRM_EKMC160111X_DEV_CNT    0x1          // Number of such devices in the system

#define PLTFRM_GEN_VOLT_MON_DEV_ID   0x38 
#define PLTFRM_GEN_CURRENT_MON_DEV_ID   0x39 

#define PLTFRM_EXT_VOLTAGE_MON_DEV_ID   0x3a

// LEDs
#define PLTFRM_LED_1_DEV_ID           0x40         // GPIO
#define PLTFRM_LED_2_DEV_ID           0x41         // GPIO

#define PLTFRM_BAR_CODE_SCANNER_1_DEV_ID   0x48    // UART


// Switches (On/Off)
#define PLTFRM_REED_SWITCH_1_DEV_ID   0x50         // GPIO
#define PLTFRM_REED_SWITCH_DEV_CNT    0x1

#define PLTFRM_SPST_SWITCH_1_DEV_ID   0x51         // GPIO
#define PLTFRM_SPST_SWITCH_DEV_CNT    0x1

#define PLTFRM_ACS712_1_CURRENT_DEV_ID   0x54      // Analog
#define PLTFRM_ACS712_2_CURRENT_DEV_ID   0x55      // Analog
#define PLTFRM_ACS712_3_CURRENT_DEV_ID   0x56      // Analog
#define PLTFRM_ACS712_DEV_CNT  3

// Analog Pressure Sensor
#define PLTFRM_MP3V5050GP_1_DEV_ID    0x60         // Analog input
#define PLTFRM_MP3V5050GP_DEV_CNT     0x1

#define PLTFRM_HE055T01_1_DEV_ID      0x61         // Analog input
#define PLTFRM_HE055T01_DEV_CNT       0x1

#define PLTFRM_MP3V5004GP_1_DEV_ID    0x62         // Analog input
#define PLTFRM_MP3V5004GP_DEV_CNT     0x1

#define PLTFRM_MPXV5010G_1_DEV_ID    0x63         // Analog input
#define PLTFRM_MPXV5010G_DEV_CNT     0x1

#define PLTFRM_MP3V5010_1_DEV_ID      0x64
#define PLTFRM_MP3V5010_DEV_CNT       1

#define PLTFRM_LLS_1_DEV_ID           0x65

#define PLTFRM_BATT_1_DEV_ID          0x68         // Analog input
#define PLTFRM_BATT_DEV_CNT           0x1

#define PLTFRM_4_20_CURRENT_SENSOR_ID    0x69       // Analog input
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

#define PLTFRM_INA219_1_BV_DEV_ID     0x58         // I2C   INA219 bus voltage
#define PLTFRM_INA219_1_SV_DEV_ID     0x59         // I2C   INA219 shunt voltage
#define PLTFRM_INA219_DEV_CNT         0x1

#define PLTFRM_GPIO_LOGIC_SZ  0x1
#define PLTFRM_LOGIC_1_DEV_ID  0x90
// #define PLTFRM_LOGIC_2_DEV_ID  0x91

#define PLTFRM_CHIRP_PWLA_1_DEV_ID  0x91
#define PLTFRM_CHIRP_PWLA_DEV_CNT 1

#define PLTFRM_FC_28_1_DEV_ID  0x92
#define PLTFRM_FC_28_DEV_CNT  1

#define PLTFRM_WSMS100_1_DEV_ID  0x93

#define PLTFRM_CC2D33S_1_DEV_ID    0xb0  // I2C
#define PLTFRM_CC2D33S_DEV_CNT     0x1

#define PLTFRM_MPL3115A2_1_DEV_ID  0xb8  // I2C
#define PLTFRM_MPL3115A2_DEV_CNT   0x1

#define PLTFRM_MPL115A2_1_DEV_ID  0xb9  // I2C
#define PLTFRM_MPL115A2_DEV_CNT  0x1

#define PLTFRM_BMP180_1_DEV_ID  0xba  // I2C
#define PLTFRM_BMP180_DEV_CNT  0x1

#define PLTFRM_MMA7660FC_1_DEV_ID  0xc0  // I2C
#define PLTFRM_MMA7660FC_DEV_CNT   0x1

#define PLTFRM_MDS_1_DEV_ID  0xca

#define PLTFRM_VIBRATION_SNSR_1_DEV_ID  0xcc

#define PLTFRM_I2C_SW_BUS_1_DEV_ID  0xe0
#define PLTFRM_I2C_SW_BUS_2_DEV_ID  0xe1

#define PLFRM_SPI_HW_BUS_1_DEV_ID   0xe8

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


#define NTC_THERM_103AT_4_R25_VAL  10000
#define NTC_THERM_103AT_4_B_25_85_VAL   3435

#define NTC_THERM_NTCALUG02A_R25_VAL  10000
#define NTC_THERM_NTCALUG02A_B_25_85_VAL   3984
   

#endif
