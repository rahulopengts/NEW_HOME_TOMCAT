/*
 * File Name: pltfrm.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Aug/12/2013
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

#include <string.h>

#include <typedefs.h>
#include <util.h>
#ifdef PLTFRM_AT24C10C_ENA
#include <at24c10c.h>
#endif
#include <uart.h>
#include <pltfrm.h>
#include <radio.h>
#include <sfh_7773.h>
#include <lm75b.h>
#include <mp3v5050gp.h>
#ifdef PLTFRM_TMP102_TEMP_SENSOR_ENA
#include <tmp102.h>
#endif
#ifdef PLTFRM_BATTV_SENSE_ENA
#include <battv.h>
#endif
#ifdef PLTFRM_AT24MAC602_ENA
#include <at24mac602.h>
#endif
#ifdef PLTFRM_TSL45315_ENA
#include <tsl45315.h>
#endif
#ifdef PLTFRM_AD7797_ENA
#include <ad7797.h>
#endif
#ifdef PLTFRM_CC2D33S_SENSOR_ENA
#include <cc2d33s.h>
#endif
#ifdef PLTFRM_MPL3115A2_SENSOR_ENA
#include <mpl3115a2.h>
#endif
#ifdef PLTFRM_MMA7660FC_SENSOR_ENA
#include <mma7660fc.h>
#endif
#ifdef PLTFRM_ADXL345_ENA
#include <adxl345.h>
#endif
#ifdef PLTFRM_SHT10_ENA
#include <sht10.h>
#endif
#ifdef PLTFRM_LIS3MDLTR_ENA
#include <lis3mdltr.h>
#endif
#ifdef PLTFRM_HE055T01_SENSOR_ENA
#include <he055t01.h>
#endif
#ifdef PLTFRM_M24M01_ENA
#include <m24m01.h>
#endif
#ifdef PLTFRM_MAX_SONAR_ENA
#include <max_sonar.h>
#endif
#ifdef NTC_THERMISTOR_ENA
#include <ntc_thermistor.h>
#endif
#ifdef PLTFRM_MQ5_ENA
#include <mq5.h>
#endif
#ifdef PLTFRM_EKMC160111X_ENA
#include <ekmc160111x.h>
#endif
#ifdef PLTFRM_INA219_ENA
#include <ina219.h>
#endif
#ifdef PLTFRM_MP3V5004GP_ENA
#include <mp3v5004gp.h>
#endif
#ifdef PLTFRM_MPXV5010G_ENA
#include <mpxv5010g.h>
#endif
#ifdef PLTFRM_MP3V5010_ENA
#include <mp3v5010.h>
#endif
#ifdef WSMS100_ENA
#include <wsms100.h>
#endif
#ifdef PLTFRM_DHT11_ENA
#include <dht11.h>
#endif
#ifdef PLTFRM_MAINS_DETECT_SNSR_ENA
#include <mds.h>
#endif
#ifdef PLTFRM_MAG3110_SENSOR_ENA
#include <mag3110.h>
#endif

UINT8_t PLTFRM_saveFEIdEna = 0;
UINT16_t PLTFRM_savedSysFEId = SYS_FATAL_ERR_0;


#ifdef GPIO_MGMT_ENA
PLTFRM_intCapGPIOPortInfo_s PLTFRM_intCapGPIOPortList[PLTFRM_INT_CAP_GPIO_PORT_COUNT];
PLTFRM_gpioPortInfo_s PLTFRM_gpioPortList[PLTFRM_GPIO_PORT_COUNT];
#endif


UINT8_t PLTFRM_nodeSnsrCnt = 0;
PLTFRM_nodeSnsrListEntry_s PLTFRM_nodeSnsrList[PLTFRM_MAX_NODE_SENSOR_CNT];


#ifndef PLTFRM_I2C_SUPPORT_DISABLED
PLTFRM_i2cBusListEntry_s PLTFRM_i2cBusList[PLTFRM_MAX_I2C_DEV_CNT];
#endif

// #define MP3V5050GP_UT

/*
 ********************************************************************
 *                        HW_REV_7       HW_REV_8
 ********************************************************************
 * P1_0 -                                GDO1
 * P1_1 -                                GDO0
 * P1_2 - 
 * P1_3 - 
 * P1_4 - 
 * P1_5 - 
 * P1_6 - 
 * P1_7 - 
 *
 * P2_0 - 
 * P2_1 - 
 * P2_2 - 
 * P2_3 - 
 * P2_4 - 
 * P2_5 - 
 * P2_6 -                 XIN            XIN
 * P2_7 -                 XOUT           XOUT
 *
 * P3_0 -                 LED            LED
 * P3_1 -                 SPI SIMO       SPI SIMO
 * P3_2 -                 SPI SOMI       SPI SOMI
 * P3_3 -                 SPI SCLK       SPI SCLK
 * P3_4 -                 UART TXD       UART TXD
 * P3_5 -                 UART RXD       UART RXD
 * P3_6 -                 I2C SDA        I2C SDA
 * P3_7 -                 I2C SCL        I2C SCL
 *
 * P4_0 -                 SPI CSn        SPI CSn
 * P4_1 - 
 * P4_2 - 
 * P4_3 - 
 * P4_4 - 
 * P4_5 -                 LED            LED
 * P4_6 - 
 * P4_7 - 
 ********************************************************************
 */
// #define PLTFRM_TMP102_UT_ENA

#if 0
/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */   
void pltfrm_delay(UINT16_t microsecs)
{
   UINT16_t i;
   
   // MCLK 16 MHz
   // Measured LED toggle frequency with scope.
   // 1 > 2.5 microsecs
   // 5 > 6.6 microsecs
   // 10 > 11.7 microsecs
   // 20 > 22 microsecs
   // 40 > 42 microsecs
   // 80 > 82 microsecs
   // 100 > 102 microsecs
   // 160 > 164 microsecs
   // 320 > 325 microsecs 
#if 0
   for (i=0; i<microsecs; i++)
        __delay_cycles(1);
#else
   microsecs <<= 2;
   for (i=0; i<microsecs; i++)
       __delay_cycles(1);
#endif
}
#endif
   


/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
void pltfrm_randInit(void)
{
   /*
    * void srand(unsigned int seed);   
    * <stdlib.h>
    * The function stores the seed value seed in a static-duration object that 
    * rand uses to compute a pseudo-random number. From a given seed value, that 
    * function always generates the same sequence of return values. The program 
    * behaves as if the target environment calls srand(1) at program startup.
    */

#ifdef RADIO_CC2520
   UINT8_t seedVal;

   cc2520_rand(&seedVal);  // Get a random number in the range 0 - 255 from 
                           // the cc2520

   srand(seedVal);
#endif
   
   return;
}

/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t PLTFRM_setMacExtAddr(UINT8_t *extAddr_p)
{
   UINT8_t rc = 1;
   
   // The MAC extended address is stored in external eeprom
  
#ifdef PLTFRM_AT24C01C_ENA
   rc = AT24C01C_write(PLTFRM_AT24C01C_1_DEV_ID, 
                       PLTFRM_AT24C01C_1_OFF_MAC_EXT_ADDR,
                       10,
                       extAddr_p);
#endif

   return rc;  
}


/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
UINT16_t PLTFRM_calcCkSum16(UINT8_t *buff_p, UINT8_t len)
{
   UINT32_t ckSum = 0;

   while (len > 1)
   {
      UINT16_t tmp = *buff_p;
      tmp = (tmp << 8) | (*(buff_p + 1));
      ckSum = ckSum + tmp;
      buff_p += 2;
      len -= 2;
   }

   if (len > 0)
       ckSum += (*buff_p);

   while (ckSum >> 16)
   {
      ckSum = (ckSum & 0xffff) + (ckSum >> 16);
   }

   return (~ckSum);
}

/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */      
PLTFRM_sts_t PLTFRM_getMacExtAddr(UINT8_t *extAddr_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
#ifdef PLTFRM_AT24MAC602_ENA
   sts = AT24MAC602_readEUI64(PLTFRM_AT24MAC602_1_DEV_ID, extAddr_p);
#elif defined(PLTFRM_AT24C01C_ENA)
   UINT8_t rc = 1;
   UINT16_t calcCkSum, storedCkSum;
   UINT8_t ckSumBuff[2];
   
   // The MAC extended address is stored in external eeprom
  

   rc = AT24C01C_read(PLTFRM_AT24C01C_1_DEV_ID, 
                      PLTFRM_AT24C01C_1_OFF_MAC_EXT_ADDR,
                      8,
                      extAddr_p);
   if (rc == 0)
       return PLTFRM_STS_OPN_FAILED;
   
   rc = AT24C01C_read(PLTFRM_AT24C01C_1_DEV_ID, 
                      PLTFRM_AT24C01C_1_OFF_MAC_EXT_ADDR_CKSUM,
                      PLTFRM_MAC_EXT_ADDR_CKSUM_LEN,
                      ckSumBuff);
   if (rc == 0)
       return PLTFRM_STS_OPN_FAILED;

   storedCkSum = *ckSumBuff;
   storedCkSum = (storedCkSum << 8) | (*(ckSumBuff + 1));

   calcCkSum = PLTFRM_calcCkSum16(extAddr_p, 8);
   
   if (calcCkSum != storedCkSum)
       sts = PLTFRM_STS_OPN_FAILED;
#else
   {
       SINT16_t idx;
       // Hack !!
       for (idx=0; idx<MAC_EXT_ADDR_LEN; idx++)
            extAddr_p[idx] = 0x47;
   }
#endif

   return sts;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */      
void PLTFRM_ledInit(void)
{
   PLTFRM_sts_t sts;

#if defined(WISENSE_HW_REV_2)  || defined(WISENSE_HW_REV_3)
   P2DIR |= 0x1;  // LED   
   P2OUT &= ~0x1;
#elif defined(WISENSE_HW_REV_4)

#ifdef GPIO_MGMT_ENA
   sts = PLTFRM_allocGPIOPin(PLTFRM_LED_1_DEV_ID,
                             PLTFRM_GPIO_PORT_3,
                             PLTFRM_GPIO_PORT_PIN_4,
                             PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                             PLTFRM_GPIO_OUTPUT_LOW,
                             PLTFRM_GPIO_INT_EDGE_NA,
                             NULL);
   if (sts != PLTFRM_STS_SUCCESS)
   {
      SYS_fatal(SYS_FATAL_ERR_100);
   }

   sts = PLTFRM_allocGPIOPin(PLTFRM_LED_2_DEV_ID,
                             PLTFRM_GPIO_PORT_3,
                             PLTFRM_GPIO_PORT_PIN_6,
                             PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                             PLTFRM_GPIO_OUTPUT_LOW,
                             PLTFRM_GPIO_INT_EDGE_NA,
                             NULL);
   if (sts != PLTFRM_STS_SUCCESS)
   {
      SYS_fatal(SYS_FATAL_ERR_101);
   }
#else
   // LED_1
   P3DIR |= 0x10;  
   P3OUT &= ~0x10;
   
   // LED_2
   P3DIR |= 0x40;  
   P3OUT &= ~0x40;
#endif
#elif defined(WISENSE_HW_REV_5)
   sts = PLTFRM_allocGPIOPin(PLTFRM_LED_1_DEV_ID,
                              PLTFRM_GPIO_PORT_4,
                              PLTFRM_GPIO_PORT_PIN_7,
                              PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                              PLTFRM_GPIO_OUTPUT_LOW,
                              PLTFRM_GPIO_INT_EDGE_NA,
                              NULL);
    if (sts != PLTFRM_STS_SUCCESS)
    {
       SYS_fatal(SYS_FATAL_ERR_102);
    }

    sts = PLTFRM_allocGPIOPin(PLTFRM_LED_2_DEV_ID,
                              PLTFRM_GPIO_PORT_3,
                              PLTFRM_GPIO_PORT_PIN_6,
                              PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                              PLTFRM_GPIO_OUTPUT_LOW,
                              PLTFRM_GPIO_INT_EDGE_NA,
                              NULL);
    if (sts != PLTFRM_STS_SUCCESS)
    {
       SYS_fatal(SYS_FATAL_ERR_103);
    }
#elif defined(WISENSE_HW_REV_6)
#ifdef PLTFRM_OFF_BOARD_LEDS_ENA
     sts = PLTFRM_allocGPIOPin(PLTFRM_LED_1_DEV_ID,
                               PLTFRM_GPIO_PORT_1,
                               PLTFRM_GPIO_PORT_PIN_2,
                               PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                               PLTFRM_GPIO_OUTPUT_LOW,
                               PLTFRM_GPIO_INT_EDGE_NA,
                               NULL);
     if (sts != PLTFRM_STS_SUCCESS)
     {
        SYS_fatal(SYS_FATAL_ERR_104);
     }

     sts = PLTFRM_allocGPIOPin(PLTFRM_LED_2_DEV_ID,
                               PLTFRM_GPIO_PORT_1,
                               PLTFRM_GPIO_PORT_PIN_3,
                               PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                               PLTFRM_GPIO_OUTPUT_LOW,
                               PLTFRM_GPIO_INT_EDGE_NA,
                               NULL);
     if (sts != PLTFRM_STS_SUCCESS)
     {
        SYS_fatal(SYS_FATAL_ERR_105);
     }
#else
    sts = PLTFRM_allocGPIOPin(PLTFRM_LED_1_DEV_ID,
                               PLTFRM_GPIO_PORT_3,
                               PLTFRM_GPIO_PORT_PIN_0,
                               PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                               PLTFRM_GPIO_OUTPUT_LOW,
                               PLTFRM_GPIO_INT_EDGE_NA,
                               NULL);
     if (sts != PLTFRM_STS_SUCCESS)
     {
        SYS_fatal(SYS_FATAL_ERR_104);
     }

     sts = PLTFRM_allocGPIOPin(PLTFRM_LED_2_DEV_ID,
                               PLTFRM_GPIO_PORT_4,
                               PLTFRM_GPIO_PORT_PIN_5,
                               PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                               PLTFRM_GPIO_OUTPUT_LOW,
                               PLTFRM_GPIO_INT_EDGE_NA,
                               NULL);
     if (sts != PLTFRM_STS_SUCCESS)
     {
        SYS_fatal(SYS_FATAL_ERR_105);
     }
#endif
#elif defined(EZ430_RF2500)
#ifdef GPIO_MGMT_ENA
    sts = PLTFRM_allocGPIOPin(PLTFRM_LED_1_DEV_ID,
                               PLTFRM_GPIO_PORT_1,
                               PLTFRM_GPIO_PORT_PIN_0,
                               PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                               PLTFRM_GPIO_OUTPUT_LOW,
                               PLTFRM_GPIO_INT_EDGE_NA,
                               NULL);
     if (sts != PLTFRM_STS_SUCCESS)
     {
        SYS_fatal(SYS_FATAL_ERR_104);
     }

     sts = PLTFRM_allocGPIOPin(PLTFRM_LED_2_DEV_ID,
                               PLTFRM_GPIO_PORT_1,
                               PLTFRM_GPIO_PORT_PIN_1,
                               PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                               PLTFRM_GPIO_OUTPUT_LOW,
                               PLTFRM_GPIO_INT_EDGE_NA,
                               NULL);
     if (sts != PLTFRM_STS_SUCCESS)
     {
        SYS_fatal(SYS_FATAL_ERR_105);
     }
#else
     P1DIR |= (BIT0 | BIT1);
     P1SEL &= (BIT0 | BIT1);
     P1OUT &= (BIT0 | BIT1);
#endif
#else
#error hardware version not specified   
#endif   
}   


#ifdef PLTFRM_ON_CHIP_VCC_SENSE_ENA
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT16_t PLTFRM_getAvgSupplyVoltage(void)
{
   UINT32_t val = 0;
   UINT8_t idx;

   for (idx=0; idx<(1 << PLTFRM_SUPPLY_VOLTAGE_SAMPLE_CNT_SHIFT); idx++)
   {
        val += pltfrm_vccSense(1);
        __delay_cycles(50*MSP430_START_UP_CLK_MHZ);
   }

   val >>= PLTFRM_SUPPLY_VOLTAGE_SAMPLE_CNT_SHIFT;
   return (UINT16_t)val;
}
#endif


#ifdef PLTFRM_BATTV_SENSE_ENA
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT16_t PLTFRM_getAvgBattVoltage(void)
{
   UINT32_t val = 0;
   UINT8_t idx;

   for (idx=0; idx<(1 << PLTFRM_BATT_VOLTAGE_SAMPLE_CNT_SHIFT); idx++)
   {
	   UINT32_t battMilliVolts;
      // if (PLTFRM_getBattV(&battMilliVolts) != PLTFRM_STS_SUCCESS)
      if (BATT_getVal(PLTFRM_BATT_1_DEV_ID, &battMilliVolts))
          SYS_fatal(SYS_FATAL_ERR_811);
      val += battMilliVolts;
      __delay_cycles(50*MSP430_START_UP_CLK_MHZ);
   }

   val >>= PLTFRM_BATT_VOLTAGE_SAMPLE_CNT_SHIFT;
   return (UINT16_t)val;
}
#endif


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void PLTFRM_setActiveModeClock(void)
{
   /*
    * MCLK: Master clock. MCLK is software selectable as LFXT1CLK, VLOCLK,
    * XT2CLK (if available on chip), or DCOCLK. MCLK is divided by 1, 2, 4,
    * or 8. MCLK is used by the CPU and system. The MCLK can be configured
    * to operate from the on-chip DCO that can be activated when requested
    * by interrupt-driven events.
    *
    * The DCO is an integrated digitally controlled oscillator. The DCO
    * frequency can be adjusted by software using the DCOx, MODx, and RSELx
    * bits.
    *
    * After a PUC, MCLK and SMCLK are sourced from DCOCLK at ~1.1 MHz.
    *
    * <TODO>
    * DCOCLK frequency should be set according to the current available power
    * supply voltage. For example, supply voltage should be at least 3.0 volts
    * for the MCU to run at 16 Mhz. Supply voltage should be at least 2.2 volts
    * for the MCU to run at 9.6  Mhz
    * Refer to the "DCO Frequency" table on page 29 of the MSP430Gx53 data sheet.
    */
#if defined(DEV_TYPE_RFD) || defined(DEV_TYPE_FFD)
#if defined(PWR_SRC_SOLAR_SUPERCAP) || defined(PWR_SRC_SOLAR_BATT)
#ifndef PLTFRM_PWR_THROUGH_LDO
   // Node powered by super-cap + solar cell combo.
   // There is no dc-dc converter/LDO between battery and micro/radio.
   // CPU is running at 1.1 MHz (DCOCLK)
   __bis_SR_register(GIE);
   do
   {
      UINT16_t svMV = PLTFRM_getAvgSupplyVoltage();
      if (svMV >= PLTFRM_MIN_SUPPLY_VOLTAGE_IN_MV)
      {
          __bic_SR_register(GIE);
          break;
      }

      PLTFRM_startTimerA1(PLTFRM_SUPPLY_VOLTAGE_MON_INTERVAL_SECS, 0, NULL);
      PLTFRM_enterLPM();
      SYS_globalEvtMsk0 &= ~SYS_GLOBAL_EVT_TIMER_A1_EXPIRY;

      PLTFRM_ON_LED_1();
      __delay_cycles(100*MSP430_START_UP_CLK_MHZ);
      PLTFRM_OFF_LED_1();

   } while (1);
#else
   // There is a dc-dc converter/LDO between battery/cap  and micro/radio.
   __bis_SR_register(GIE);
   do
   {
      UINT16_t svMV = PLTFRM_getAvgBattVoltage();
      if (svMV >= PLTFRM_MIN_SUPPLY_VOLTAGE_IN_MV)
      {
          __bic_SR_register(GIE);
          break;
      }

      PLTFRM_startTimerA1(PLTFRM_SUPPLY_VOLTAGE_MON_INTERVAL_SECS, 0, NULL);
      PLTFRM_enterLPM();
      SYS_globalEvtMsk0 &= ~SYS_GLOBAL_EVT_TIMER_A1_EXPIRY;

      PLTFRM_ON_LED_1();
      __delay_cycles(100*MSP430_START_UP_CLK_MHZ);
      PLTFRM_OFF_LED_1();

   } while (1);
#endif
#endif
#endif
  
#if (CPU_ACTIVE_CLOCK_IN_MHZ == 16)
   if (CALBC1_16MHZ == 0xFF)   // If calibration constant erased
   {
       while (1);
   }

   DCOCTL = 0;    // Select lowest DCOx and MODx settings
   BCSCTL1 = CALBC1_16MHZ;     // Set range
   DCOCTL = CALDCO_16MHZ;
   // BCSCTL2 |= DIVS_3;
#elif (CPU_ACTIVE_CLOCK_IN_MHZ == 8)
   if (CALBC1_8MHZ == 0xFF)   // If calibration constant erased
   {
       while (1);
   }

   DCOCTL = 0;    // Select lowest DCOx and MODx settings
   BCSCTL1 = CALBC1_8MHZ;     // Set range
   DCOCTL = CALDCO_8MHZ;
   // BCSCTL2 |= DIVS_3;
#else
#error CPU frequency not specified !!
#endif

   return;
}


#ifdef GPIO_MGMT_ENA

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t PLTFRM_getGPIOPinInput(PLTFRM_gpioPortId_t portId,
                                    PLTFRM_gpioPortPin_t portPinNr,
                                    UINT8_t *pinVal_p)
{
   volatile UINT8_t *regAddr_p;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   switch (portId)
   {
      case PLTFRM_GPIO_PORT_1:
           regAddr_p = &(P1IN);
           break;

      case PLTFRM_GPIO_PORT_2:
           regAddr_p = &(P2IN);
           break;

      case PLTFRM_GPIO_PORT_3:
           regAddr_p = &(P3IN);
           break;

#ifdef __MSP430G2955__
      case PLTFRM_GPIO_PORT_4:
           regAddr_p = &(P4IN);
           break;
#endif

      default:
          SYS_fatal(SYS_FATAL_ERR_106);
           sts = PLTFRM_STS_INV_PARAM;
           break;
   }

   if (sts == PLTFRM_STS_SUCCESS)
   {
       UINT8_t regVal = *regAddr_p;
       *pinVal_p = (regVal & (1 << portPinNr)) ? 0x1 : 0x0;
   }

   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t PLTFRM_getGPIOPortOp(PLTFRM_gpioPortId_t portId, UINT8_t *val_p)
{
   volatile UINT8_t *regAddr_p;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   switch (portId)
   {
	  case PLTFRM_GPIO_PORT_1:
	       regAddr_p = &(P1OUT);
	       break;

	  case PLTFRM_GPIO_PORT_2:
	       regAddr_p = &(P2OUT);
	       break;

	  case PLTFRM_GPIO_PORT_3:
	       regAddr_p = &(P3OUT);
	       break;

#ifdef __MSP430G2955__
	  case PLTFRM_GPIO_PORT_4:
	       regAddr_p = &(P4OUT);
	       break;
#endif

	  default:
	       SYS_fatal(SYS_FATAL_ERR_107);
	       sts = PLTFRM_STS_INV_PARAM;
	       break;
   }

   if (sts == PLTFRM_STS_SUCCESS)
       *val_p = *(regAddr_p);

   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t PLTFRM_setGPIOPinOutput(PLTFRM_gpioPortId_t portId,
                                     PLTFRM_gpioPortPin_t portPinNr,
                                     UINT8_t pinVal)
{
   volatile UINT8_t *regAddr_p;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   switch (portId)
   {
      case PLTFRM_GPIO_PORT_1:
           regAddr_p = &(P1OUT);
           break;

      case PLTFRM_GPIO_PORT_2:
           regAddr_p = &(P2OUT);
           break;

      case PLTFRM_GPIO_PORT_3:
           regAddr_p = &(P3OUT);
           break;

#ifdef __MSP430G2955__
      case PLTFRM_GPIO_PORT_4:
           regAddr_p = &(P4OUT);
           break;
#endif

      default:
           SYS_fatal(SYS_FATAL_ERR_107);
           sts = PLTFRM_STS_INV_PARAM;
           break;
   }

   if (sts == PLTFRM_STS_SUCCESS)
   {
       if (pinVal)
           (*(regAddr_p)) |= (1 << portPinNr);
       else
           (*(regAddr_p)) &= ~(1 << portPinNr);
   }

   return sts;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t PLTFRM_gpioPinIntCtrlOpn(PLTFRM_gpioPortId_t portId,
                                      PLTFRM_gpioPortPin_t portPinNr,
                                      PLTFRM_gpioIntCtrlOpn_t opn,
                                      UINT8_t clearIntFlag)
{
   volatile UINT8_t *regIEAddr_p;
   volatile UINT8_t *regIFGAddr_p;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   switch (portId)
   {
      case PLTFRM_GPIO_PORT_1:
           regIEAddr_p = &(P1IE);
           regIFGAddr_p = &(P1IFG);
           break;

      case PLTFRM_GPIO_PORT_2:
           regIEAddr_p = &(P2IE);
           regIFGAddr_p = &(P2IFG);
           break;

      default:
          SYS_fatal(SYS_FATAL_ERR_108);
           sts = PLTFRM_STS_INV_PARAM;
           break;
   }

   if (sts == PLTFRM_STS_SUCCESS)
   {
       if (opn == PLTFRM_ENABLE_GPIO_PIN_INT)
       {
           if (clearIntFlag)
               *(regIFGAddr_p) &= ~(1 << portPinNr);
           *(regIEAddr_p) |= (1 << portPinNr);
       }
       else
           *(regIEAddr_p) &= ~(1 << portPinNr);
   }

   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t PLTFRM_checkGPIOEvt(void)
{
   UINT16_t portIdx;
   UINT8_t gpioEvtCnt = 0;

   for (portIdx=0; portIdx<PLTFRM_INT_CAP_GPIO_PORT_COUNT; portIdx++)
   {
      UINT16_t pinIdx;
      PLTFRM_intCapGPIOPortInfo_s *portInfo_p = &PLTFRM_intCapGPIOPortList[portIdx];

      if (portInfo_p->intFlags == 0x0)
          continue;

      for (pinIdx=0; pinIdx<PLTFRM_GPIO_PORT_SIZE; pinIdx++)
      {
          if (portInfo_p->intFlags & (1 << pinIdx))
          {
              PLTFRM_intCapGPIOPortPinInfo_s *pinInfo_p = &(portInfo_p->pinInfo[pinIdx]);
              portInfo_p->intFlags &= ~(1 << pinIdx);
              if (pinInfo_p->cbFunc_p != NULL)
              {
                  (*(pinInfo_p->cbFunc_p))(pinInfo_p->devId);
                  gpioEvtCnt = 1;
              }
              if (portInfo_p->intFlags == 0x0)
                  break;
          }
      }
   }

   return gpioEvtCnt;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
volatile UINT8_t *PLTFRM_getGPIOPortBaseAddr(PLTFRM_gpioPortId_t portId)
{
   volatile UINT8_t *baseAddr_p;

   if (portId > PLTFRM_GPIO_PORT_3)
   {
       return NULL;
   }

   switch (portId)
   {
       case PLTFRM_GPIO_PORT_1:
            baseAddr_p = &(P1IN);
            break;

       case PLTFRM_GPIO_PORT_2:
            baseAddr_p = &(P2IN);
            break;

       case PLTFRM_GPIO_PORT_3:
            baseAddr_p = &(P3IN);
            break;

       default:
            baseAddr_p = NULL;
            break;
   }

   return baseAddr_p;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t PLTFRM_gpioCfgIntEdge(PLTFRM_gpioPortId_t portId,
                                   PLTFRM_gpioPortPin_t portPinNr,
                                   PLTFRM_gpioIntEdge_t edgeType)
{
   volatile UINT8_t *regAddr_p;
   UINT8_t portBitMsk;
   PLTFRM_globalIntState_t intState;

   if (portId > PLTFRM_GPIO_PORT_2
       || portPinNr >= PLTFRM_GPIO_PORT_SIZE)
   {
      SYS_fatal(SYS_FATAL_ERR_109);
   }

   regAddr_p = PLTFRM_getGPIOPortBaseAddr(portId);
   if (regAddr_p == NULL)
      SYS_fatal(SYS_FATAL_ERR_110);

   portBitMsk = (1 << portPinNr);

   /*
    * Each PxIES bit selects the interrupt edge for the corresponding I/O pin.
    * > Bit = 0: The PxIFGx flag is set with a low-to-high transition
    * > Bit = 1: The PxIFGx flag is set with a high-to-low transition
    *
    * Writing to P1IES, or P2IES can result in setting the corresponding interrupt
    * flags.
    *   -----------------------------
    *   PxIESx   PxINx   PxIFGx
    *   -----------------------------
    *   0 -> 1    0      May be set
    *   0 -> 1    1      Unchanged
    *   1 -> 0    0      Unchanged
    *   1 -> 0    1      May be set
    *   -----------------------------
    */

   PLTFRM_saveIntStateAndDisable(intState);

   if (edgeType == PLTFRM_GPIO_INT_EDGE_POSITIVE)
       *(regAddr_p + (&(P1IES) - &(P1IN))) &= ~(portBitMsk);
   else
       *(regAddr_p + (&(P1IES) - &(P1IN))) |= (portBitMsk);

   PLTFRM_delay5MicroSecs();  // Is this required ?

   // Clear IFG in case it has been set
   *(regAddr_p + (&(P1IFG) - &(P1IN))) &= ~(portBitMsk);

   PLTFRM_restoreIntState(intState);

   return PLTFRM_STS_SUCCESS;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t PLTFRM_gpioEnablePull(PLTFRM_gpioPortId_t portId,
                                   PLTFRM_gpioPortPin_t portPinNr,
                                   PLTFRM_gpioPullType_t pullType)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   volatile UINT8_t *regAddr_p;

   if (portPinNr >= PLTFRM_GPIO_PORT_SIZE
       || ((regAddr_p = PLTFRM_getGPIOPortBaseAddr(portId)) == NULL))
   {
      SYS_fatal(SYS_FATAL_ERR_111);
       sts = PLTFRM_STS_INV_PARAM;
   }
   else
   {
       UINT8_t portBitMsk = (1 << portPinNr);

       /*
        * Each bit in each PxREN register enables or disables the pullup/pulldown
        * resistor of the corresponding I/O pin. The corresponding bit in the PxOUT
        * register selects if the pin is pulled up or pulled down.
        *
        * If the pin's pull-up/pull-down resistor is enabled, the corresponding bit in
        * the PxOUT register selects pull-up or pull-down.
        * > Bit = 0: The pin is pulled down
        * > Bit = 1: The pin is pulled up
        */
       *(regAddr_p + (&(P1REN) - &(P1IN))) |= (portBitMsk);

       if (pullType == PLTFRM_GPIO_INPUT_PULL_DOWN)
           *(regAddr_p + (&(P1OUT) - &(P1IN))) &= ~(portBitMsk);
       else
           *(regAddr_p + (&(P1OUT) - &(P1IN))) |= (portBitMsk);
   }

   return sts;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t PLTFRM_getGPIOPinDevId(PLTFRM_gpioPortId_t portId,
                               PLTFRM_gpioPortPin_t portPinNr)
{
   UINT8_t devId;

   if (portPinNr >= PLTFRM_GPIO_PORT_SIZE
       || portId > PLTFRM_GPIO_PORT_4)
   {
       devId = PLTFRM_DUMMY_DEV_ID;
   }
   else
   {
       PLTFRM_gpioPortInfo_s *portInfo_p = &PLTFRM_gpioPortList[portId];
       devId = portInfo_p->pinInfo[portPinNr].devId;
   }

   return devId;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t PLTFRM_reserveGPIOPin(UINT8_t devId,
                                   PLTFRM_gpioPortId_t portId,
                                   PLTFRM_gpioPortPin_t portPinNr)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   if (portPinNr >= PLTFRM_GPIO_PORT_SIZE
       || portId > PLTFRM_GPIO_PORT_4)
   {
       sts = PLTFRM_STS_INV_PARAM;
   }
   else
   {
       PLTFRM_gpioPortInfo_s *portInfo_p = &PLTFRM_gpioPortList[portId];
       if (portInfo_p->pinInfo[portPinNr].devId != PLTFRM_DUMMY_DEV_ID)
           sts = PLTFRM_STS_RESOURCE_CONFLICT;
       else
       {
           portInfo_p->pinInfo[portPinNr].devId = devId;
           if (portId <= PLTFRM_GPIO_PORT_2)
           {
               PLTFRM_intCapGPIOPortInfo_s *intCapPortInfo_p = &PLTFRM_intCapGPIOPortList[portId];
               if (intCapPortInfo_p->pinInfo[portPinNr].devId != PLTFRM_DUMMY_DEV_ID)
                   SYS_fatal(SYS_FATAL_ERR_112);
               intCapPortInfo_p->pinInfo[portPinNr].devId = devId;
           }
       }
   }

   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t PLTFRM_allocGPIOPin(UINT8_t devId,
                                 PLTFRM_gpioPortId_t portId,
                                 PLTFRM_gpioPortPin_t portPinNr,
                                 PLTFRM_gpioDirn_t pinDirn,
                                 PLTRM_gpioOutVal_t initOutVal,
                                 PLTFRM_gpioIntEdge_t intEdge,
                                 PLTFRM_intCbFunc_t cbFunc_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   volatile UINT8_t *portBaseAddr_p;

   // If cbFunc_p is NULL, it means this pin is not an interrupt source

   if (portPinNr >= PLTFRM_GPIO_PORT_SIZE
       || portId > PLTFRM_GPIO_PORT_4)
   {
      SYS_fatal(SYS_FATAL_ERR_113);
   }

   /*
    * GPIO ports P1 and P2 have these associated registers -
    * ----------------------
    * Offset    Description
    * ----------------------
    * 0x0        IN
    * 0x1        OUT
    * 0x2        DIR
    * 0x3        IFG
    * 0x4        IES
    * 0x5        IE
    * 0x6        SEL
    * 0x7        REN
    * ---------------------
    * P1 base address is 0x20. P2 base address is 0x28
    * P1SEL2 is at 0x41 and P2SEL2 is at 0x42
    */

   switch (portId)
   {
       case PLTFRM_GPIO_PORT_1:
            portBaseAddr_p = &(P1IN);
            break;

       case PLTFRM_GPIO_PORT_2:
            portBaseAddr_p = &(P2IN);
            break;

       case PLTFRM_GPIO_PORT_3:
            if (cbFunc_p != NULL || intEdge != PLTFRM_GPIO_INT_EDGE_NA)
                sts = PLTFRM_STS_INV_HW_CONN;
            else
                portBaseAddr_p = &(P3IN);
            break;

#ifdef __MSP430G2955__
       case PLTFRM_GPIO_PORT_4:
            if (cbFunc_p != NULL || intEdge != PLTFRM_GPIO_INT_EDGE_NA)
                sts = PLTFRM_STS_INV_HW_CONN;
            else
                portBaseAddr_p = &(P4IN);
            break;
#endif

       default:
            sts = PLTFRM_STS_INV_HW_RESOURCE;
            break;
   }

   if (sts == PLTFRM_STS_SUCCESS)
   {
       PLTFRM_gpioPortInfo_s *portInfo_p = &PLTFRM_gpioPortList[portId];
       UINT8_t portBitMsk = (1 << portPinNr);

       if (portInfo_p->pinInfo[portPinNr].devId != PLTFRM_DUMMY_DEV_ID)
           return PLTFRM_STS_RESOURCE_CONFLICT;

       if (portId <= PLTFRM_GPIO_PORT_2)
       {
           PLTFRM_intCapGPIOPortInfo_s *intCapPortInfo_p = &PLTFRM_intCapGPIOPortList[portId];

           intCapPortInfo_p->intFlags &= ~portBitMsk;
           if ((intEdge != PLTFRM_GPIO_INT_EDGE_NA && cbFunc_p == NULL)
               || (intEdge == PLTFRM_GPIO_INT_EDGE_NA && cbFunc_p != NULL))
           {
               return PLTFRM_STS_INV_PARAM;
           }

           intCapPortInfo_p->pinInfo[portPinNr].cbFunc_p = cbFunc_p;
           intCapPortInfo_p->pinInfo[portPinNr].devId = devId;
       }

       portInfo_p->pinInfo[portPinNr].devId = devId;


       // I/O functionality
       if (portId <= PLTFRM_GPIO_PORT_2)
           *(portBaseAddr_p + (&(P1SEL) - &(P1IN))) &= ~(portBitMsk);
       else
           *(portBaseAddr_p + (&(P3SEL) - &(P3IN))) &= ~(portBitMsk);

#ifdef __MSP430G2955__
       // P1SEL2 (0x41) / P1SEL2 (0x42) / P1SEL2 (0x43) / P1SEL2 (0x44)
       *((&(P1SEL2)) + portId) &= ~(portBitMsk);
#endif

       // Pull up/down resistor disabled
       if (portId <= PLTFRM_GPIO_PORT_2)
           *(portBaseAddr_p + (&(P1REN) - &(P1IN))) &= ~(portBitMsk);
       else
           *(portBaseAddr_p + (&(P3REN) - &(P3IN))) &= ~(portBitMsk);

       if (pinDirn == PLTFRM_GPIO_PIN_DIRECTION_INPUT)
       {
           if (portId <= PLTFRM_GPIO_PORT_2)
               *(portBaseAddr_p + (&(P1DIR) - &(P1IN))) &= ~(portBitMsk);
           else
               *(portBaseAddr_p + (&(P3DIR) - &(P3IN))) &= ~(portBitMsk);

           if ((portId <= PLTFRM_GPIO_PORT_2) && (intEdge != PLTFRM_GPIO_INT_EDGE_NA))
           {
               // Configure edge
               if (intEdge == PLTFRM_GPIO_INT_EDGE_POSITIVE)
                   *(portBaseAddr_p + (&(P1IES) - &(P1IN))) &= ~(portBitMsk);
               else
                   *(portBaseAddr_p + (&(P1IES) - &(P1IN))) |= (portBitMsk);

               // clear interrupt flag
               *(portBaseAddr_p + (&(P1IFG) - &(P1IN))) &= ~(portBitMsk);

               // Enable interrupt
               // *(portBaseAddr_p + (&(P1IE) - &(P1IN))) |= (portBitMsk);
           }
       }
       else
       {
           if (initOutVal == PLTFRM_GPIO_OUTPUT_LOW)
           {
               if (portId <= PLTFRM_GPIO_PORT_2)
                   *(portBaseAddr_p + (&(P1OUT) - &(P1IN))) &= ~(portBitMsk);
               else
                   *(portBaseAddr_p + (&(P3OUT) - &(P3IN))) &= ~(portBitMsk);
           }
           else
           {
               if (portId <= PLTFRM_GPIO_PORT_2)
                   *(portBaseAddr_p + (&(P1OUT) - &(P1IN))) |= portBitMsk;
               else
                   *(portBaseAddr_p + (&(P3OUT) - &(P3IN))) |= portBitMsk;
           }

           if (portId <= PLTFRM_GPIO_PORT_2)
               *(portBaseAddr_p + (&(P1DIR) - &(P1IN))) |= (portBitMsk);
           else
               *(portBaseAddr_p + (&(P3DIR) - &(P3IN))) |= (portBitMsk);
       }
   }

   return sts;
}
#endif


#if defined(__MSP430G2955__) || defined(__MSP430F2274__)
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t PLTFRM_allocADC10Chann(UINT8_t adcChannId,
                                    UINT8_t devId)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   PLTFRM_gpioPortId_t portId;
   PLTFRM_gpioPortPin_t pinNr;
   volatile UINT8_t *reg_p;

   /*
    * MSP430G2955 has eight external input channels.
    *
    * P2.0 / A0 - Analog input A0  - Free
    * P2.1 / A1 - Analog input A1  - Free
    * P2.2 / A2 - Analog input A2  - Free
    * P2.3 / A3 - Analog input A3  - Free
    * P2.4 / A4 - Analog input A4  - Free
    * P3.0 / A5 - Analog input A5  - Free
    * P3.6 / A6 - Analog input A6  - Used by software I2C
    * P3.7 / A6 - Analog input A7  - Used by software I2C
    *
    */
   if (adcChannId > PLTFRM_ADC10_CHANN_5_ID)
       return PLTFRM_STS_INV_PARAM;

   if (adcChannId <= PLTFRM_ADC10_CHANN_4_ID)
   {
       portId = PLTFRM_GPIO_PORT_2;
       pinNr = PLTFRM_GPIO_PORT_PIN_0 + (adcChannId - PLTFRM_ADC10_CHANN_0_ID);
       reg_p = &(P2DIR);
   }
   else
   {
       portId = PLTFRM_GPIO_PORT_3;
       pinNr = PLTFRM_GPIO_PORT_PIN_0;
       reg_p = &(P3DIR);
   }

   sts = PLTFRM_reserveGPIOPin(devId, portId, pinNr);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   *(reg_p) |= (1 << pinNr);  // Set port pin to output direction

   /*
    * ADC10 analog enable. These bits enable the corresponding pin for
    * analog input. BIT0 corresponds to A0, BIT1 corresponds to A1, etc.
    */
   ADC10AE0 |= (1 << pinNr);

   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t PLTFRM_doADC10SingleConv(UINT8_t adcChannId,
                                      UINT8_t refGenVEnc,
                                      UINT8_t waitCycleEnc,
                                      UINT16_t *adcVal_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   if (refGenVEnc > PLTFRM_ADC10_REF_VCC)
       return PLTFRM_STS_INV_PARAM;

   /*
    * With few exceptions the ADC10 control bits can only be modified when ENC = 0.
    * ENC must be set to 1 before any conversion can take place.
    */
   ADC10CTL0 &= ~(ENC);

#ifdef TIMER_CLK_ACLK

   /*
    * When using ACLK (32 kHz), one clock cycle is 31.25 microsecs.
    * For now, all conversions are give 31.25 * 64 microsecs to complete.
    */

  ADC10CTL1 = (adcChannId*INCH_1) // Channel
              + ADC10SSEL_1 // ADC10 clock source select (ACLK)
              + SHS_0  // Sample-and-hold source select (00 ADC10SC bit)
              + CONSEQ_0;  // Single channel - single conversion

  ADC10CTL0 = ADC10ON
              + (ADC10SHT_1 * (waitCycleEnc & 0x3));  // clock cycles

  if (refGenVEnc == PLTFRM_ADC10_REF_VCC)
  {
      ADC10CTL0 += SREF_0;  // Vr+ = Vcc and Vr- = Vss
  }
  else
  {
      ADC10CTL0 += (SREF_1
                    + REFON    // Reference generator on
                    + (refGenVEnc == PLTFRM_ADC10_REF_GEN_2PT5_VOLTS ? REF2_5V : 0x0)); // Reference-generator voltage is 2.5 volts
  }
#else
#error Not Supported !!
#endif

   PLTFRM_delay1MilliSec();

   /*
    * ADC10BUSY (Bit 0 in ADC10CTL1)
    * This bit indicates an active sample or conversion operation
    *   - 0 No operation is active.
    *   - 1 A sequence, sample, or conversion is active.
    */
   while (ADC10CTL1 & ADC10BUSY);

   ADC10CTL0 &= ~ADC10IFG;

   ADC10CTL0 |= ENC + ADC10SC;   // Sampling and conversion start

   /*
    * ADC10IFG (Bit 2 in ADC10CTL0)
    * ADC10 interrupt flag. This bit is set if ADC10MEM is loaded with a
    * conversion result. It is automatically reset when the interrupt
    * request is accepted, or it may be reset by software.
    *   - 0 No interrupt pending
    *   - 1 Interrupt pending
    */

   while (!(ADC10IFG & ADC10CTL0));

   *adcVal_p = ADC10MEM;

   /*
    * With few exceptions the ADC10 control bits can only be modified when ENC = 0.
    */
   ADC10CTL0 &= ~(ENC);

   ADC10CTL0 &= ~ADC10IFG;

   // ADC core and reference voltage can be powered down separately.
   ADC10CTL0 &= ~(REFON | ADC10ON);  // Required to reduce power consumption

   return sts;
}
#elif defined(__MSP430G2553__)
#else
#error Not Supported !!
#endif


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t PLTFRM_regSnsrDev(UINT8_t devId, PLTFRM_snsrDevInfo_s *devInfo_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   if (PLTFRM_nodeSnsrCnt >= PLTFRM_MAX_NODE_SENSOR_CNT)
       sts = PLTFRM_STS_SENSOR_LIMIT_REACHED;
   else
   {
       PLTFRM_nodeSnsrListEntry_s *entry_p = PLTFRM_nodeSnsrList + PLTFRM_nodeSnsrCnt;
       entry_p->devId = devId;
       entry_p->devInfo_p = devInfo_p;
       PLTFRM_nodeSnsrCnt ++;
   }

   return sts;
}

#ifndef PLTFRM_I2C_SUPPORT_DISABLED
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t PLTFRM_regI2CDev(UINT8_t devId, UINT8_t busId, UINT8_t i2cAddr)
{
   UINT16_t idx;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   // rkris@wisense.in / jan/10/2015

   for (idx=0; idx<PLTFRM_MAX_I2C_DEV_CNT; idx++)
   {
       PLTFRM_i2cBusListEntry_s *dev_p = &PLTFRM_i2cBusList[idx];

       if (dev_p->devId != PLTFRM_DUMMY_DEV_ID)
       {
           if (dev_p->i2cAddr == i2cAddr
#if (PLTFRM_I2C_BUS_CNT > 1)
               && dev_p->busId == busId
#endif
              )
           {
               sts = PLTFRM_STS_I2C_ADDR_IN_USE;
               break;
           }
       }
       else
       {
           dev_p->devId = devId;
           dev_p->i2cAddr = i2cAddr;
#if (PLTFRM_I2C_BUS_CNT > 1)
           dev_p->busId = busId;
#endif
           break;
       }
   }

   if (idx >= PLTFRM_MAX_I2C_DEV_CNT)
       sts = PLTFRM_STS_I2C_DEV_LIST_FULL;

   return sts;
}
#endif

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t PLTFRM_getNodeSnsrCnt(void)
{
   return PLTFRM_nodeSnsrCnt;
}

#if 0
void __test(void)
{
   P1DIR |= 0x1;
   for (;;)
   {
       P1OUT ^= 0x1;
       PLTFRM_delay100MicroSecs();
   }
}
#endif



UINT16_t P2_intCnt = 0, P1_intCnt = 0;

/*
 ********************************************************************
 *
 *  April/17/15 - moved here from cc1101.c
 *
 *
 ********************************************************************
 */
#ifdef ECLIPSE_IDE
static void __attribute__((__interrupt__(PORT1_VECTOR))) Port_1 (void)
#else
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
#endif
{
    UINT16_t reg = P1IFG;
#ifdef GPIO_MGMT_ENA
    UINT16_t bitIdx;
    PLTFRM_intCapGPIOPortInfo_s *portInfo_p = &PLTFRM_intCapGPIOPortList[0];
#endif

    P1_intCnt ++;

#ifdef RADIO_CC1101
    CC1101_irqHndlr();
    // P1_0 and P1_1 are used by CC1101
    if (!(reg & 0xfc))
    {
    	PLTFRM_exitLPM();
    	return;
    }
#endif

#ifdef GPIO_MGMT_ENA
#ifdef RADIO_CC1101
    // P1_0 and P1_1 are used by CC1101
    for (bitIdx=2; bitIdx<8; bitIdx++)
#else
    for (bitIdx=0; bitIdx<8; bitIdx++)
#endif
    {
        UINT16_t bitMsk = (1 << bitIdx);
        if (reg & bitMsk)
        {
            PLTFRM_intCapGPIOPortPinInfo_s *pinInfo_p = &(portInfo_p->pinInfo[bitIdx]);

            // Each PxIFG flag must be reset with software
            P1IFG &= ~(bitMsk);

            // Disable further interrupts on this pin
            // P2IE &= ~(bitMsk);

            // rkris@wisense.in, april/17/15
            // Do not invoke callback function if particular interrupt
            // has been masked since IFG flag will be set irrespective
            // of whether corresponding pin's interrupt is masked or not.

            if ((P1IE & bitMsk) && (pinInfo_p->cbFunc_p != NULL))
            {
                (*(pinInfo_p->cbFunc_p))(pinInfo_p->devId);
            }

            // portInfo_p->intFlags |= bitMsk;
        }
    }
#else
    P1IFG &= ~(reg);
#endif

    // Irrespective of whether MSP430 was in low power mode before or not
    PLTFRM_exitLPM();
}



/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
#ifdef ECLIPSE_IDE
static void __attribute__((__interrupt__(PORT2_VECTOR))) Port_2(void)
#else
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
#endif
{
    UINT16_t reg = P2IFG;
#ifdef GPIO_MGMT_ENA
    UINT16_t bitIdx;
    PLTFRM_intCapGPIOPortInfo_s *portInfo_p = &PLTFRM_intCapGPIOPortList[1];
#endif

    P2_intCnt ++;

#ifdef EZ430_RF2500
    CC2500_irqHndlr();
    // GDO0 : P2.6,  GDO1 : P2.7
    if (!(reg & 0x3f))
    {
    	PLTFRM_exitLPM();
    	return;
    }
#endif

    // At this point interrupts are disabled globally

    // Each PxIFGx bit is the interrupt flag for its corresponding I/O pin and is set
    // when the selected input signal edge occurs at the pin. All PxIFGx interrupt flags
    // request an interrupt when their corresponding PxIE bit and the GIE bit are set.
#ifdef GPIO_MGMT_ENA
#ifdef EZ430_RF2500
    // P2_6 and P2_7 used by CC2500 (EZ430_RF2500 board)
    for (bitIdx=0; bitIdx<6; bitIdx++)
#else
    for (bitIdx=0; bitIdx<8; bitIdx++)
#endif
    {
        UINT16_t bitMsk = (1 << bitIdx);
        if (reg & bitMsk)
        {
            PLTFRM_intCapGPIOPortPinInfo_s *pinInfo_p = &(portInfo_p->pinInfo[bitIdx]);

            // Each PxIFG flag must be reset with software
            P2IFG &= ~(bitMsk);

            // Disable further interrupts on this pin
            // P2IE &= ~(bitMsk);

            // rkris@wisense.in, april/17/15
            // Do not invoke callback function if particular interrupt
            // has been masked since IFG flag will be set irrespective
            // of whether corresponding pin's interrupt is masked or not.

            if ((P2IE & bitMsk) && (pinInfo_p->cbFunc_p != NULL))
            {
                (*(pinInfo_p->cbFunc_p))(pinInfo_p->devId);
            }

            // portInfo_p->intFlags |= bitMsk;
        }
    }
#else
    P2IFG &= ~(reg);
#endif

    // Irrespective of whether MSP430 was in low power mode before or not
    PLTFRM_exitLPM();
    return;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t PLTFRM_spiInit(UINT8_t devId)
{
  PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
  PLTFRM_gpioPortPin_t portPin;

  // <TODO> - Add parameters for clock polarity and clock phase
  //          and SPI interrupts should be enabled or disabled
  // P3.1    UCB0SIMO
  // P3.2    UCB0SOMI
  // P3.3    UCB0CLK  /  UCA0STE

#ifdef GPIO_MGMT_ENA
  for (portPin = PLTFRM_GPIO_PORT_PIN_1;
       portPin <= PLTFRM_GPIO_PORT_PIN_3;
       portPin++)
  {
      sts = PLTFRM_reserveGPIOPin(devId,
                                  PLTFRM_GPIO_PORT_3, portPin);
      if (sts != PLTFRM_STS_SUCCESS)
          return sts;
  }
#endif


#if 0
  P3DIR |= (BIT1 | BIT3);
  P3DIR &= ~(BIT2);
  P3REN |= (BIT2);
#endif


  P3SEL |= (BIT1 | BIT2 | BIT3);  // 3.1, 3.2 and 3.3

  UCB0CTL1 |= UCSWRST;
  // Clock polarity (0) - low
  // Clock phase (1) - capture on first edge, change on subsequent edge.
  UCB0CTL0 |= UCCKPH + UCMSB + UCMST + UCSYNC; // 3-pin, 8-bit SPI master
  UCB0CTL1 |= UCSSEL_2;                        // SMCLK (8 or 16 MHz)
  UCB0BR0 |= 0x2;                              // 8/2 or 16/2 -> 4 or 8MHz
  UCB0BR1 = 0;                                 //
  // UCB0MCTL = 0;                             // No modulation
  UCB0CTL1 &= ~UCSWRST;                        // **Initialize USCI state machine**
  // IE2 |= UCB0RXIE;                          // Enable USCI0 RX interrupt

  return sts;
}



#ifdef PLTFRM_TMP102_TEMP_SENSOR_ENA
#ifdef PLTFRM_TMP102_UT_ENA
SINT32_t tempVal;
UINT16_t tmoVal;

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void PLTFRM_tmp102UT(void)
{
  for (;;)
  {
     int idx;

#if 0
     if (TMP102_startSingleConv(PLTFRM_TMP102_1_DEV_ID,
                                &tempVal,
                                &tmoVal) != PLTFRM_STS_OPN_IN_PROGRESS)
     {
        SYS_fatal(SYS_FATAL_ERR_114);
     }

     for (idx=0; idx<50; idx++)
     {
        PLTFRM_delay1000MicroSecs();
     }
#endif

     if (TMP102_getTempVal(PLTFRM_TMP102_1_DEV_ID, &tempVal) != PLTFRM_STS_SUCCESS)
     {
        SYS_fatal(SYS_FATAL_ERR_115);
     }
  }
}
#endif
#endif

// #define _CHECK_
#define PLTFRM_AT24MAC602_TEST_ENA
// #define PLTFRM_LM75B_TEMP_SENSOR_TEST_ENA

#if defined(PLTFRM_LM75B_TEMP_SENSOR_ENA) && defined(PLTFRM_LM75B_TEMP_SENSOR_TEST_ENA)
SINT16_t LM75B_temp16;
#endif

#if defined(PLTFRM_MP3V5050GP_ENA) && defined(MP3V5050GP_UT)
UINT32_t snsrOp32, snsrVcc;
#endif

#ifdef PLTFRM_AT24MAC602_TEST_ENA
UINT8_t PLTFRM_at24mac602ExtAddr[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
#endif

#if 0
void junkFunc(UINT8_t devId)
{

}
#endif

// #define PLTFRM_CC2D33S_SENSOR_TEST_ENA


#if defined(PLTFRM_CC2D33S_SENSOR_ENA) && defined(PLTFRM_CC2D33S_SENSOR_TEST_ENA)
UINT16_t CC2D33S_rhVal = 0;
SINT16_t CC2D33S_tempVal = 0;
#endif

#ifdef ADC10_TEST
UINT32_t snsrVal = 0;
UINT32_t minSnsrVal = (UINT32_t)5000;
UINT32_t maxSnsrVal = 0;
UINT16_t sampleCnt = 0;
#endif

// #define PLTFRM_MPL3115A2_SENSOR_TEST_ENA
// #define PLTFRM_LM75B_TEMP_SENSOR_TEST_ENA
// #define PLTFRM_MPL3115A2_SENSOR_TEST_ENA

volatile UINT32_t dummy = 0, done = 0;

#ifdef CC1101_TIMING_DBG_ENA
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void CC1101_timingDbgInit()
{
   PLTFRM_sts_t sts;
   sts = PLTFRM_allocGPIOPin(PLTFRM_GENERIC_DEV_ID,
                             PLTFRM_GPIO_PORT_1,
                             PLTFRM_GPIO_PORT_PIN_6,
                             PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                             PLTFRM_GPIO_OUTPUT_LOW,
                             PLTFRM_GPIO_INT_EDGE_NA,
                             NULL);

   if (sts != PLTFRM_STS_SUCCESS)
   {
       // Port and bit number combination has already been
       // reserved by another device !!
      SYS_fatal(SYS_FATAL_ERR_116);
   }

   sts = PLTFRM_allocGPIOPin(PLTFRM_GENERIC_DEV_ID,
                             PLTFRM_GPIO_PORT_1,
                             PLTFRM_GPIO_PORT_PIN_7,
                             PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                             PLTFRM_GPIO_OUTPUT_LOW,
                             PLTFRM_GPIO_INT_EDGE_NA,
                             NULL);

   if (sts != PLTFRM_STS_SUCCESS)
   {
       // Port and bit number combination has already been
       // reserved by another device !!
      SYS_fatal(SYS_FATAL_ERR_117);
   }
}
#endif


#ifndef MICRO_ONLY
/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t PLTFRM_readSavedSysFEId(UINT16_t *fatalErrorId_p)
{
   UINT8_t bArray[NVM_FE_ID_FIELD_LEN];
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

#ifdef PLTFRM_AT24MAC602_ENA
   sts = AT24MAC602_readMem(PLTFRM_AT24MAC602_1_DEV_ID,
                            NVM_FE_ID_FIELD_OFFSET,
                            NVM_FE_ID_FIELD_LEN,
                            bArray);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       *fatalErrorId_p = UTIL_ntohs(bArray);
   }
#endif

   return sts;
}

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t PLTFRM_clearSavedSysFaultId(void)
{
   UINT16_t faultId = 0;

#ifdef PLTFRM_AT24MAC602_ENA
   return AT24MAC602_write(PLTFRM_AT24MAC602_1_DEV_ID,
                           NVM_FE_ID_FIELD_OFFSET,
                           NVM_FE_ID_FIELD_LEN,
                           (UINT8_t *)&faultId);
#else
   return PLTFRM_STS_SUCCESS;
#endif
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void PLTFRM_saveSysFEId(UINT16_t fatalErrId)
{
#ifndef DEV_TYPE_SNIFFER
#ifdef PLTFRM_AT24MAC602_ENA
   if (PLTFRM_saveFEIdEna && fatalErrId != SYS_FATAL_ERR_0)
   {
       PLTFRM_sts_t sts;
       UINT8_t bArray[2];

       UTIL_htons(bArray, fatalErrId);

       sts = AT24MAC602_write(PLTFRM_AT24MAC602_1_DEV_ID,
                              NVM_FE_ID_FIELD_OFFSET,
                              NVM_FE_ID_FIELD_LEN,
                              bArray);

       if (sts != PLTFRM_STS_SUCCESS)
           return;

#if 0
       bArray[0] = 0x0;
       bArray[1] = 0x0;
                          
       sts = AT24MAC602_readMem(PLTFRM_AT24MAC602_1_DEV_ID,
                                NVM_FE_ID_FIELD_OFFSET,
                                NVM_FE_ID_FIELD_LEN,
                                bArray);
       if (sts != PLTFRM_STS_SUCCESS)
           return;
#endif
   }
#else
#if !defined(MICRO_ONLY) && !defined(EZ430_RF2500)
//#error No NVM on the node !!
#endif
#endif
#endif
}
#endif

#ifdef PLTRM_SYS_FATAL_ID_ON_GPIO_ENA

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void PLTFRM_outputSysFEId(UINT8_t id)
{
   UINT8_t regVal;

   id = id & PLTFRM_SYS_FATAL_ID_MSK;

   regVal = P2OUT;
   regVal &= ~PLTFRM_SYS_FATAL_ID_MSK;
   regVal |= id;
   P2OUT = regVal;

   return;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t PLTFRM_cfgSysFatalOp(void)
{
   PLTFRM_sts_t sts;
   PLTFRM_gpioPortPin_t portPinNr;

   // Send the SYS_FATAL source id on five GPIO pins

   for (portPinNr = PLTFRM_GPIO_PORT_PIN_0;
        portPinNr <= PLTFRM_GPIO_PORT_PIN_4;
        portPinNr ++)

   {
       sts = PLTFRM_allocGPIOPin(PLTFRM_GENERIC_DEV_ID,
                                 PLTFRM_GPIO_PORT_2,
                                 portPinNr,
                                 PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                                 PLTFRM_GPIO_OUTPUT_LOW,
                                 PLTFRM_GPIO_INT_EDGE_NA,
                                 NULL);
       if (sts != PLTFRM_STS_SUCCESS)
           break;
   }

   return sts;
}
#endif

#ifdef MICRO_ONLY

UINT32_t MO_convTriggerCnt = 0;
/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
void MO_convTrigger(UINT8_t  devId)
{
	MO_convTriggerCnt ++;
	SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_GPIO_INT_EVT;
}
#endif


static const char _onChipTempSnsrName[] = "ON_CHIP_TEMP";
static const char _onChipVccSnsrName[] = "ON_CHIP_VCC";
static const char _onChipSnsrMfr[] = "TI";

PLTFRM_snsrDevInfo_s ON_CHIP_TEMP_SENSOR_devInfo =
{
  _onChipTempSnsrName, _onChipSnsrMfr
};

PLTFRM_snsrDevInfo_s ON_CHIP_VCC_SENSOR_devInfo =
{
  _onChipVccSnsrName, _onChipSnsrMfr
};


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void PLTFRM_resetSelf(void)
{
   // Use invalid password
   WDTCTL = WDTPW + 10;  // valid password (0x5a00)
}

UINT32_t HE055T01_snsrOp = 0;



#ifdef PLTFRM_EXT_VOLTAGE_MON_ENA
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t PLTFRM_extVoltMonInit(void)
{
   PLTFRM_sts_t sts;
   sts = PLTFRM_allocADC10Chann(PLTFRM_ADC10_CHANN_1_ID, PLTFRM_EXT_VOLTAGE_MON_DEV_ID);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;
   return sts;
}
#endif



#ifdef __MSP430G2955__
// 8 - 1 > 2.18 to 1.62
// 12 - 1 > 2.42 to 1.62
// 18 - 1 > 2.64 to 1.64 @ 441 kHz
// 24 - 1 > 2.77 to 1.66 @ 330 kHz
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void PLTFRM_testPWM(void)
{
  // P4.4 - TB0.1
  // P4.5 - TB0.2

  // P1.2 - TA0.1
  // P1.3 - TA0.2

#if 0
  P1DIR |= 0x04;                            // P1.2 and P1.3 output
  P1SEL |= 0x04;                            // P1.2 and P1.3 TA1/2 otions
  TACCR0 = 24 -1; //  8 - 1; // 512 - 1;    // PWM Period
  TACCTL1 = OUTMOD_7;                       // TACCR1 reset/set
  TACCR1 = 12;                              // TACCR1 PWM duty cycle
  // TACCTL2 = OUTMOD_7;                    // TACCR2 reset/set
  // TACCR2 = 64;                           // TACCR2 PWM duty cycle
  TACTL = TASSEL_2 | MC_1;                  // SMCLK, up mode
#else
  P4DIR |= 0x10;                            // P4.4 and P4.5 output
  P4SEL |= 0x10;                            // P4.4 and P4.5 TA1/2 otions
  TBCCR0 = 24 -1; //  8 - 1; // 512 - 1;    // PWM Period
  TBCCTL1 = OUTMOD_7;                       // TACCR1 reset/set
  TBCCR1 = 12;                              // TACCR1 PWM duty cycle
  // TACCTL2 = OUTMOD_7;                    // TACCR2 reset/set
  // TACCR2 = 64;                           // TACCR2 PWM duty cycle
  TBCTL = TBSSEL_2 | MC_1;                  // SMCLK, up mode
#endif
  __bis_SR_register(CPUOFF);                // Enter LPM0
}
#endif

UINT32_t __DCOPerVLO = 0;

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void PLTFRM_cfgLPMClock(void)
{
  /*
   * In low-power mode 3 (LPM3)
   *   – CPU is disabled.
   *   – MCLK and SMCLK are disabled.
   *   – DCO dc-generator is disabled.
   *   – ACLK remains active.
   *
   * Auxiliary clock (ACLK) can be sourced from
   *   - A 32768-Hz watch crystal,
   *   - A high-frequency crystal
   *   - Or the internal very low-power LF oscillator.
   *
   *
   * <From the f2274 data sheet, page 41:
   * Internal VLO clock frequency can range from 4 kHz to 20 kHz with
   * a typical value of 12 kHz.
   */
#ifdef EZ430_RF2500
  // No 32 KHz crystal on the board !!. Use the VLO.
  BCSCTL3 |= LFXT1S_2;    // ACLK = VLO

  // __DCOPerVLO = TI_measureVLO();
#endif
}


UINT32_t __dbgIntCnt = 0;

void __dbgCbFn(void)
{
   __dbgIntCnt = 0;
}

// #define TEST_RADIO_INTF


#if 0
void __loopTest(void)
{
   P2DIR |= BIT0;
   while (1)
   {
	  UINT16_t idx;
      for (idx=0; idx<15; idx++);
      P2OUT ^= BIT0;
   }
}
#endif



/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void PLTFRM_init(void)
{
  // Stop watch-dog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;

#ifdef GPIO_MGMT_ENA
  memset(PLTFRM_intCapGPIOPortList, 0, sizeof(PLTFRM_intCapGPIOPortList));
  memset(PLTFRM_gpioPortList, 0, sizeof(PLTFRM_gpioPortList));  // rkris@wisense.in / feb/23/15
#endif

#if defined(WISENSE_HW_REV_2) || defined(WISENSE_HW_REV_3)\
        || defined(WISENSE_HW_REV_4) || defined(WISENSE_HW_REV_5)\
        || defined(WISENSE_HW_REV_6) || defined(EZ430_RF2500)
  PLTFRM_ledInit();
#endif

#ifdef TEST_RADIO_INTF
  {
	  P3DIR = 0xe;
	  P1DIR = 0x3;
	  P4DIR = 0x3;

	  for (;;)
	  {
		 P3OUT ^= 0xe;  // 3.1, 3.2 and 3.3
		 P1OUT ^= 0x3;  // 1.0 and 1.1
		 P4OUT ^= 0x3;  // 4.0 and 4.1
	  }

  }
#endif

#ifndef DEV_TYPE_RFD
#ifndef MICRO_ONLY
  // Blink the LEDs to indicate boot up ..
  {
     UINT16_t __i;
     PLTFRM_OFF_LED_1();
     PLTFRM_OFF_LED_2();
     for (__i=0; __i<20; __i++)
     {
        UINT16_t __j;
        for (__j=0; __j<50; __j++)
             __delay_cycles(1000*MSP430_START_UP_CLK_MHZ);
        PLTFRM_TOGGLE_LED_1();
        PLTFRM_TOGGLE_LED_2();
     }
     PLTFRM_OFF_LED_1();
     PLTFRM_OFF_LED_2();
  }
#endif
#endif

  PLTFRM_cfgLPMClock();

  memset(PLTFRM_nodeSnsrList, 0, sizeof(PLTFRM_nodeSnsrList));

  PLTFRM_setActiveModeClock();

  PLTFRM_delay1MilliSec();

#ifndef PLTFRM_I2C_SUPPORT_DISABLED
  {
     UINT16_t idx;
     for (idx=0; idx<PLTFRM_MAX_I2C_DEV_CNT; idx++)
     {
          PLTFRM_i2cBusList[idx].devId = PLTFRM_DUMMY_DEV_ID;
          PLTFRM_i2cBusList[idx].i2cAddr = PLTFRM_INV_I2C_ADDR;
#if (PLTFRM_I2C_BUS_CNT > 1)
          PLTFRM_i2cBusList[idx].busId = PLTFRM_INV_I2C_BUS_ID;
#endif
     }
  }
#endif

#ifdef CC1101_TIMING_DBG_ENA
  CC1101_timingDbgInit();
#endif

#ifdef TEST_TIMER_A0
  __bis_SR_register(GIE);
  msp430g2553_startTimerA0(10, 0, dummyTA0CbFunc);
  for (;;);
#endif

#ifdef ADC10_TEST
  for (;;)
  {
     int idx;

     PLTFRM_sts_t sts = PLTFRM_adc10Init(MSP430_ADC10_CHAN_3, 250, MSP430_ADC10_REF_V_2_5);

     if (sts == PLTFRM_STS_SUCCESS)
     {
         snsrVal = PLTFRM_adc10Sense();
         if (snsrVal > maxSnsrVal)
             maxSnsrVal = snsrVal;
         if (snsrVal < minSnsrVal)
             minSnsrVal = snsrVal;
     }

     for (idx=0; idx<100; idx++)
     {
         PLTFRM_delay1MilliSec();
     }

     sampleCnt ++;
  }
#endif

#ifndef EZ430_RF2500
#ifdef GPIO_MGMT_ENA
   {
      PLTFRM_sts_t sts;

      // Reserve the XIN/XOUT pins

      sts = PLTFRM_reserveGPIOPin(PLTFRM_32KHZ_CRYSTAL_DEV_ID,
                                  PLTFRM_GPIO_PORT_2,
                                  PLTFRM_GPIO_PORT_PIN_6);
      if (sts != PLTFRM_STS_SUCCESS)
         SYS_fatal(SYS_FATAL_ERR_118);

      sts = PLTFRM_reserveGPIOPin(PLTFRM_32KHZ_CRYSTAL_DEV_ID,
                                  PLTFRM_GPIO_PORT_2,
                                  PLTFRM_GPIO_PORT_PIN_7);
      if (sts != PLTFRM_STS_SUCCESS)
         SYS_fatal(SYS_FATAL_ERR_119);
   }
#endif
#endif

#ifdef PLTRM_SYS_FATAL_ID_ON_GPIO_ENA
  PLTFRM_cfgSysFatalOp();
#endif

#ifndef PLTFRM_I2C_SUPPORT_DISABLED
  if (I2C_SW_init(PLTFRM_I2C_SW_BUS_1_ID) == 0x0)
  {
      SYS_fatal(SYS_FATAL_ERR_535);
  }
#endif

#ifdef PLTFRM_AT24MAC602_ENA
  if (AT24MAC602_init(PLTFRM_AT24MAC602_1_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
      SYS_fatal(SYS_FATAL_ERR_536);
  }

#ifdef PLTFRM_AT24MAC602_TEST_ENA
  if (PLTFRM_getMacExtAddr(PLTFRM_at24mac602ExtAddr) != PLTFRM_STS_SUCCESS)
  {
      {
         PLTFRM_OFF_LED_1();
         PLTFRM_OFF_LED_2();
         for (;;)
         {
             UINT16_t __j;
             for (__j=0; __j<50; __j++)
                  __delay_cycles(1000*MSP430_START_UP_CLK_MHZ);
             PLTFRM_TOGGLE_LED_1();
         }
      }

      // SYS_fatal(SYS_FATAL_ERR_537);
  }
#endif

  if (PLTFRM_readSavedSysFEId(&PLTFRM_savedSysFEId) != PLTFRM_STS_SUCCESS)
  {
      {
         PLTFRM_OFF_LED_1();
         PLTFRM_ON_LED_2();
         for (;;)
         {
             UINT16_t __j;
             for (__j=0; __j<50; __j++)
                  __delay_cycles(1000*MSP430_START_UP_CLK_MHZ);
             PLTFRM_TOGGLE_LED_1();
             PLTFRM_TOGGLE_LED_2();
         }
      }

      // SYS_fatal(SYS_FATAL_ERR_538)
  }

#ifdef PLTFRM_EXT_VOLTAGE_MON_ENA
  if (PLTFRM_extVoltMonInit() != PLTFRM_STS_SUCCESS)
	  SYS_fatal(SYS_FATAL_ERR_818);
#endif

  __bis_SR_register(GIE);

  // EEPROM write uses a platform timer ...

  if (PLTFRM_clearSavedSysFaultId() != PLTFRM_STS_SUCCESS)
  {
      // SYS_fatal(SYS_FATAL_ERR_539);
      PLTFRM_savedSysFEId = SYS_RESEVED_EEPROM_ERR_2;
      {
         PLTFRM_OFF_LED_1();
         PLTFRM_OFF_LED_2();
         for (;;)
         {
             UINT16_t __j;
             for (__j=0; __j<50; __j++)
                  __delay_cycles(1000*MSP430_START_UP_CLK_MHZ);
             PLTFRM_TOGGLE_LED_2();
         }
      }
  }

  __bic_SR_register(GIE);
#endif

#ifdef PLTFRM_BATTV_SENSE_ENA
  if (BATT_init(PLTFRM_BATT_1_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
     SYS_fatal(SYS_FATAL_ERR_135);
  }
#endif

#ifdef PLTFRM_M24M01_ENA
  if (M24M01_init(PLTFRM_M24M01_1_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
      SYS_fatal(SYS_FATAL_ERR_547);
  }
#endif

  PLTFRM_saveFEIdEna = 1;

#ifdef PLTFRM_CHIRP_PWLA_ENA
  CHIRP_PWLA_init(PLTFRM_CHIRP_PWLA_1_DEV_ID);
#endif

#ifdef FC_28_MOISTURE_SENSOR_ENA
  if (FC_28_init(PLTFRM_FC_28_1_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
       SYS_fatal(SYS_FATAL_ERR_120);
  }
#endif

#ifdef WSMS100_ENA
  if (WSMS100_init(PLTFRM_WSMS100_1_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
       SYS_fatal(SYS_FATAL_ERR_120);
  }
#endif

#ifdef PLTFRM_LDR_ENA
  if (LDR_init(PLTFRM_LDR_1_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
       SYS_fatal(SYS_FATAL_ERR_831);
  }
#endif

#ifdef __MSP430G2955__
  {
#ifdef PLTFRM_ON_CHIP_VCC_SENSE_ENA
     if (PLTFRM_regSnsrDev(PLTFRM_ON_CHIP_VCC_SENSOR_DEV_ID,
                           &ON_CHIP_VCC_SENSOR_devInfo) != PLTFRM_STS_SUCCESS)
         SYS_fatal(SYS_FATAL_ERR_120);
#endif

#ifdef PLTFRM_ON_CHIP_TEMP_SENSOR_ENA
     if (PLTFRM_regSnsrDev(PLTFRM_ON_CHIP_TEMP_SENSOR_DEV_ID,
                           &ON_CHIP_TEMP_SENSOR_devInfo) != PLTFRM_STS_SUCCESS)
         SYS_fatal(SYS_FATAL_ERR_121);
#endif
  }
#endif
// #endif


#ifdef PLTFRM_REMOTE_GPIO_CTRL_ENA
  {
      // Allocate the GPIOs which will be remotely controlled here.
	  PLTFRM_sts_t sts;
	  sts = PLTFRM_allocGPIOPin(PLTFRM_GPIO_REMOTE_CTRL_DEV_ID,
                                PLTFRM_GPIO_PORT_1,
                                PLTFRM_GPIO_PORT_PIN_4,
                                PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                                PLTFRM_GPIO_OUTPUT_LOW,
                                PLTFRM_GPIO_INT_EDGE_NA,
                                NULL);
	  if (sts != PLTFRM_STS_SUCCESS)
		  SYS_fatal(SYS_FATAL_ERR_544);


#if 0
	  sts = PLTFRM_allocGPIOPin(PLTFRM_GPIO_REMOTE_CTRL_DEV_ID,
                                PLTFRM_GPIO_PORT_1,
                                PLTFRM_GPIO_PORT_PIN_5,
                                PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                                PLTFRM_GPIO_OUTPUT_LOW,
                                PLTFRM_GPIO_INT_EDGE_NA,
                                NULL);
	  if (sts != PLTFRM_STS_SUCCESS)
		  SYS_fatal(SYS_FATAL_ERR_545);

	  sts = PLTFRM_allocGPIOPin(PLTFRM_GPIO_REMOTE_CTRL_DEV_ID,
                                PLTFRM_GPIO_PORT_1,
                                PLTFRM_GPIO_PORT_PIN_6,
                                PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                                PLTFRM_GPIO_OUTPUT_LOW,
                                PLTFRM_GPIO_INT_EDGE_NA,
                                NULL);
	  if (sts != PLTFRM_STS_SUCCESS)
		  SYS_fatal(SYS_FATAL_ERR_545);
#endif
   }
#endif

#ifdef PLTFRM_MAINS_DETECT_SNSR_ENA
  {
      if (MDS_init(PLTFRM_MDS_1_DEV_ID) != PLTFRM_STS_SUCCESS)
		  SYS_fatal(SYS_FATAL_ERR_3030);
  }
#endif

#ifdef PLTFRM_ADXL345_ENA
  if (ADXL345_init(PLTFRM_ADXL345_1_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
     SYS_fatal(SYS_FATAL_ERR_123);
  }
#endif

#ifdef PLTFRM_ACS712_ENA
   if (ACS712_init(PLTFRM_ACS712_1_CURRENT_DEV_ID) != PLTFRM_STS_SUCCESS)
   {
	   SYS_fatal(SYS_FATAL_ERR_840);
   }

   if (ACS712_init(PLTFRM_ACS712_2_CURRENT_DEV_ID) != PLTFRM_STS_SUCCESS)
   {
	   SYS_fatal(SYS_FATAL_ERR_841);
   }

   if (ACS712_init(PLTFRM_ACS712_3_CURRENT_DEV_ID) != PLTFRM_STS_SUCCESS)
   {
	   SYS_fatal(SYS_FATAL_ERR_842);
   }
#endif

#ifdef PLTFRM_MAG3110_SENSOR_ENA
   if  (MAG3110_init(PLTFRM_MAG3110_1_DEV_ID) != PLTFRM_STS_SUCCESS)
   {
	   SYS_fatal(SYS_FATAL_ERR_3303);
   }
#endif

#ifdef PLTFRM_INA219_ENA
#if 0
  if (INA219_init(PLTFRM_INA219_1_BV_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
      // SYS_fatal(SYS_FATAL_ERR_826);
  }

  if (INA219_init(PLTFRM_INA219_2_BV_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
      // SYS_fatal(SYS_FATAL_ERR_832);
  }
#endif

  if (INA219_init(PLTFRM_INA219_3_BV_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
      SYS_fatal(SYS_FATAL_ERR_833);
  }

#if 0
  if (INA219_init(PLTFRM_INA219_4_BV_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
      // SYS_fatal(SYS_FATAL_ERR_834);
  }
#endif
#endif

#ifdef PLTFRM_LM75B_TEMP_SENSOR_ENA

  if (LM75B_init(PLTFRM_LM75B_1_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
      SYS_fatal(SYS_FATAL_ERR_430);
  }

#ifdef PLTFRM_LM75B_TEMP_SENSOR_TEST_ENA
  {
     UINT16_t idx;
     SINT16_t LM75B_temp16;
     for (idx=0; idx<10; idx++)
     {
        PLTFRM_TOGGLE_LED_1();
        PLTFRM_TOGGLE_LED_2();
        if (LM75B_getTempVal(PLTFRM_LM75B_1_DEV_ID, &LM75B_temp16) != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_401);

     }
  }
#endif
#endif

#ifdef PLTFRM_LIS3MDLTR_ENA
  if (LIS3MDLTR_init(PLTFRM_LIS3MDLTR_1_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
      SYS_fatal(SYS_FATAL_ERR_534);
  }
#endif

#ifdef PLTFRM_DHT11_ENA
  if (DHT11_init(PLTFRM_DHT11_1_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
      SYS_fatal(SYS_FATAL_ERR_879);
  }
#endif

#ifdef PLTFRM_SHT10_ENA
  if (SHT10_init(PLTFRM_SHT10_1_TEMP_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
      SYS_fatal(SYS_FATAL_ERR_402);
  }

#ifdef PLTFRM_SHT10_TEST_ENA
  for (;;)
  {
     UINT16_t idx, tmoValMilliSecs, rawRH;
     SINT16_t measTemp;
     PLTFRM_sts_t sts;

     sts = SHT10_startTempMeas(PLTFRM_SHT10_1_DEV_ID, &tmoValMilliSecs);
     if (sts != PLTFRM_STS_SUCCESS)
         SYS_fatal(SYS_FATAL_ERR_402);

     for (idx=0; idx<tmoValMilliSecs; idx++)
          PLTFRM_delay1MilliSec();

     sts = SHT10_readMeasTempVal(PLTFRM_SHT10_1_DEV_ID, &measTemp);
     if (sts != PLTFRM_STS_SUCCESS)
         SYS_fatal(SYS_FATAL_ERR_402);
     
     sts = SHT10_startRHMeas(PLTFRM_SHT10_1_DEV_ID, &tmoValMilliSecs);
     if (sts != PLTFRM_STS_SUCCESS)
         SYS_fatal(SYS_FATAL_ERR_402);

     for (idx=0; idx<tmoValMilliSecs; idx++)
          PLTFRM_delay1MilliSec();

     sts = SHT10_readMeasRHVal(PLTFRM_SHT10_1_DEV_ID, &rawRH);
     if (sts != PLTFRM_STS_SUCCESS)
         SYS_fatal(SYS_FATAL_ERR_402);
  }
#endif
#endif

#ifdef PLTFRM_HE055T01_SENSOR_ENA
  if (HE055T01_init(PLTFRM_HE055T01_1_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
      SYS_fatal(SYS_FATAL_ERR_540);
  }

#ifdef PLTFRM_HE055T01_SENSOR_TEST_ENA
  for (;;)
  {
      UINT16_t idx=0;
      if (HE055T01_getSnsrOp(PLTFRM_HE055T01_1_DEV_ID, &HE055T01_snsrOp) != PLTFRM_STS_SUCCESS)
          break;
      for (idx=0; idx<100; idx++)
           PLTFRM_delay1MilliSec();
  }
#endif
#endif

#ifdef PLTFRM_TSL45315_ENA
  if (TSL45315_init(PLTFRM_TSL45315_1_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
      SYS_fatal(SYS_FATAL_ERR_402);
  }
#endif


#ifdef PLTFRM_MPL115A2_SENSOR_ENA
  if (MPL115A2_init(PLTFRM_MPL115A2_1_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
      SYS_fatal(SYS_FATAL_ERR_124);
  }
#endif

#ifdef PLTFRM_MPL3115A2_SENSOR_ENA
  if (MPL3115A2_init(PLTFRM_MPL3115A2_1_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
      SYS_fatal(SYS_FATAL_ERR_125);
  }
#endif

#ifdef PLTFRM_MPL3115A2_SENSOR_TEST_ENA
  for (;;)
  {
      SINT16_t tempVal;
      UINT16_t tmoVal, x;
      UINT32_t pressVal;
      UINT16_t delayCnt = 0;
     
      PLTFRM_sts_t sts = MPL3115A2_startSingleConv(PLTFRM_MPL3115A2_1_DEV_ID, &tmoVal);
      if (sts != PLTFRM_STS_OPN_IN_PROGRESS)
      {
          SYS_fatal(SYS_FATAL_ERR_126);
      }

      // for (x=0; x<(tmoVal+1); x++)
      for (;;)
      {
           UINT8_t ctrl1RegVal;
           MPL3115A2_cntxt_s *devCntxt_p = MPL3115A2_getDevCntxt(PLTFRM_MPL3115A2_1_DEV_ID);
           if (devCntxt_p == NULL)
           {
              SYS_fatal(SYS_FATAL_ERR_127);
           }

           sts = MPL3115A2_readReg(devCntxt_p,
                                   MPL3115A2_CTRL1_REG_ADDR,
                                   &ctrl1RegVal);
           if (sts != PLTFRM_STS_SUCCESS)
               SYS_fatal(SYS_FATAL_ERR_127);

           if (!(ctrl1RegVal & MPL3115A2_CTRL_REG1_OST_BIT))
               break;

           delayCnt ++;

           PLTFRM_delay1MilliSec();
      }

      sts = MPL3115A2_readTemp(PLTFRM_MPL3115A2_1_DEV_ID, &tempVal);
      if (sts != PLTFRM_STS_SUCCESS)
      {
         SYS_fatal(SYS_FATAL_ERR_128);
      }

      sts = MPL3115A2_readPressure(PLTFRM_MPL3115A2_1_DEV_ID, &pressVal);
      if (sts != PLTFRM_STS_SUCCESS)
      {
         SYS_fatal(SYS_FATAL_ERR_129);
      }
  }
#endif

#if 1
#ifdef PLTFRM_CC2D33S_SENSOR_ENA
  if (CC2D33S_init(PLTFRM_CC2D33S_1_RH_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
     SYS_fatal(SYS_FATAL_ERR_130);
  }

#ifdef PLTFRM_CC2D33S_SENSOR_TEST_ENA
  for (;;) // idx=0; idx<10; idx++)
  {
     UINT16_t tmoVal, x;
     PLTFRM_sts_t sts = CC2D33S_startSingleConv(PLTFRM_CC2D33S_1_RH_DEV_ID, &tmoVal);
     if (sts != PLTFRM_STS_OPN_IN_PROGRESS)
     {
        SYS_fatal(SYS_FATAL_ERR_131);
     }

     for (x=0; x<50; x++)
          PLTFRM_delay1MilliSec();

     sts = CC2D33S_dataFetch(PLTFRM_CC2D33S_1_DEV_ID, &CC2D33S_rhVal, &CC2D33S_tempVal);
     if (sts != PLTFRM_STS_SUCCESS)
     {
        SYS_fatal(SYS_FATAL_ERR_132);
     }
  }
#endif
#endif
#endif

#ifdef PLTFRM_MMA7660FC_SENSOR_ENA
  if (MMA7660FC_init(PLTFRM_MMA7660FC_1_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
     SYS_fatal(SYS_FATAL_ERR_133);
  }
#endif

#ifdef _CHECK_
  PLTFRM_ON_LED_1();
  PLTFRM_OFF_LED_1();
  for (;;)
  {
     int idx;
     PLTFRM_TOGGLE_LED_1();
     PLTFRM_TOGGLE_LED_2();
     for (idx=0; idx<100; idx++)
         PLTFRM_delay1000MicroSecs();
  }
#endif

#ifdef PLTFRM_AT24C01C_ENA
  AT24C01C_init(PLTFRM_AT24C01C_1_DEV_ID);
#endif

#ifdef PLTFRM_TMP102_TEMP_SENSOR_ENA
  if (TMP102_init(PLTFRM_TMP102_1_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
     SYS_fatal(SYS_FATAL_ERR_134);
  }
#ifdef PLTFRM_TMP102_UT_ENA
  PLTFRM_tmp102UT();
#endif
#endif

  
#ifdef PLTFRM_MPXV5010G_ENA
  if (MPXV5010G_init(PLTFRM_MPXV5010G_1_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
     SYS_fatal(SYS_FATAL_ERR_136);
  }
#endif

#ifdef PLTFRM_MP3V5010_ENA
  if (MP3V5010_init(PLTFRM_MP3V5010_1_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
     SYS_fatal(SYS_FATAL_ERR_4010);
  }
#endif

#ifdef PLTFRM_MP3V5050GP_ENA
  if (MP3V5050GP_init(PLTFRM_MP3V5050GP_1_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
     SYS_fatal(SYS_FATAL_ERR_136);
  }

#ifdef MP3V5050GP_UT
  for (;;)
  {
      if (MP3V5050GP_getSensorOutput(PLTFRM_MP3V5050GP_1_DEV_ID, &snsrOp32) != PLTFRM_STS_SUCCESS)
          SYS_fatal(SYS_FATAL_ERR_137);
  }
#endif
#endif

#ifdef PLTFRM_MP3V5004GP_ENA
  if (MP3V5004GP_init(PLTFRM_MP3V5004GP_1_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
     SYS_fatal(SYS_FATAL_ERR_136);
  }
#endif

#ifdef PLTFRM_LLS_ENA
  if (LLS_init(PLTFRM_LLS_1_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
     SYS_fatal(SYS_FATAL_ERR_854);
  }
#endif

#ifdef NTC_THERMISTOR_ENA
  {
     UINT8_t devIdx;

     for (devIdx=0; devIdx < PLTFRM_NTC_THERMISTOR_DEV_CNT; devIdx++)
	 {
        if (NTC_THERMISTOR_init((PLTFRM_NTCALUG02A_1_DEV_ID + devIdx)) != PLTFRM_STS_SUCCESS)
	        SYS_fatal(SYS_FATAL_ERR_877);

	 }
  }
#endif

#ifdef PLTFRM_MQ5_ENA
  if (MQ5_init(PLTFRM_MQ5_1_DEV_ID) != PLTFRM_STS_SUCCESS)
	  SYS_fatal(SYS_FATAL_ERR_882);
#endif

  if (PLTFRM_spiInit(PLFRM_SPI_HW_BUS_1_ID) != PLTFRM_STS_SUCCESS)
      SYS_fatal(SYS_FATAL_ERR_138);

#if defined(RADIO_CC1101)
  if (CC1101_spiInit() != PLTFRM_STS_SUCCESS)
#elif defined(RADIO_CC2500)
  if (CC2500_spiInit() != PLTFRM_STS_SUCCESS)
#elif defined(RADIO_CC3100)
  if (CC3100_spiInit() != PLTFRM_STS_SUCCESS)
#elif defined(RADIO_NONE)
  if (0)
#else
#error Specified radio not supported !!
#endif
     return;

#ifdef PLTFRM_AD7797_ENA
#ifdef AD7797_DIRECT_INTERFACE
   if (AD7797_spiInit(PLTFRM_AD7797_1_DEV_ID) != PLTFRM_STS_SUCCESS)
       return;
#endif
#endif

#ifdef PLTFRM_AD7797_ENA
  if (AD7797_init(PLTFRM_AD7797_1_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
      SYS_fatal(SYS_FATAL_ERR_140);
  }
#endif

#ifdef MICRO_ONLY
  {
    PLTFRM_sts_t sts;
    sts = PLTFRM_allocGPIOPin(PLTFRM_GPIO_REMOTE_CTRL_DEV_ID,
                              PLTFRM_GPIO_PORT_2,
                              PLTFRM_GPIO_PORT_PIN_1,
                              PLTFRM_GPIO_PIN_DIRECTION_INPUT,
                              PLTFRM_GPIO_OUTPUT_NA,
                              PLTFRM_GPIO_INT_EDGE_POSITIVE,
                              MO_convTrigger);
    if (sts != PLTFRM_STS_SUCCESS)
        SYS_fatal(SYS_FATAL_ERR_611);
  }
#endif

#ifdef RADIO_CC1101
#ifdef CC1101_LOAD_SWITCH_ENA
  CC1101_switchOn();
#endif
#elif defined(EZ430_RF2500)
#elif defined(RADIO_CC3100)
#elif defined(RADIO_NONE)
#else
#error Specified radio not supported !!
#endif

  // Do not change order of initialization (first cc2520, then UART) !!

#if defined(RADIO_CC2520)
  cc2520_init();
#elif defined(RADIO_CC1200)
  if (CC1200_init() != PLTFRM_STS_SUCCESS)
      SYS_fatal(SYS_FATAL_ERR_141);
#elif defined(RADIO_CC1101)
  if (CC1101_init(1) != PLTFRM_STS_SUCCESS)
      SYS_fatal(SYS_FATAL_ERR_421);
#elif defined(RADIO_CC2500)
  if (CC2500_init() != PLTFRM_STS_SUCCESS)
      SYS_fatal(SYS_FATAL_ERR_421);
#elif defined(RADIO_CC3100)
  if (CC3100_init() != PLTFRM_STS_SUCCESS)
      SYS_fatal(SYS_FATAL_ERR_876);
#elif defined(RADIO_NONE)
#else
#error Radio not defined !!
#endif

#ifdef PLTFRM_UART_ENA
  UART_init();
#endif

#if 0
#if defined(LPWMN_COORD) || defined(DEV_TYPE_SNIFFER)
#if defined(GW_ENA) || defined(PLTFRM_LOG_ENA)
  UART_init();
#endif
#elif defined(MICRO_ONLY)
  UART_init();
#endif
#endif


#ifdef PLTFRM_MAX_SONAR_ENA
  {
     PLTFRM_sts_t sts = MAX_SONAR_init(PLTFRM_MAX_SONAR_1_DEV_ID);
     if (sts != PLTFRM_STS_SUCCESS)
         SYS_fatal(SYS_FATAL_ERR_552);
  }
#endif

#ifdef PLTFRM_EKMC160111X_ENA
  EKMC160111X_init(PLTFRM_EKMC160111X_1_DEV_ID);
#endif

#ifdef PLTFRM_SFH_7773_SENSOR_ENA
  SFH_7773_init(PLTFRM_SFH_7773_1_DEV_ID);
#endif

#ifdef PLTFRM_REED_SWITCH_ENA
  REED_SWITCH_init(PLTFRM_REED_SWITCH_1_DEV_ID);
#endif


#ifdef PLTFRM_MPU6050_SENSOR_ENA
  if (MPU6050_init(PLTFRM_MPU6050_1_DEV_ID) != PLTFRM_STS_SUCCESS)
  {
      SYS_fatal(SYS_FATAL_ERR_143);
  }
#endif

#ifdef GPIO_MGMT_ENA
  // Do not call any driver init functions beyond this point

  // All unused GPIO pins should be configured as outputs and set to low.
  { 
     PLTFRM_gpioPortId_t portId; 

     for (portId = PLTFRM_GPIO_PORT_1; 
          portId <= PLTFRM_GPIO_PORT_4; 
          portId ++)
     {        
          PLTFRM_gpioPortPin_t portPinNr;
          PLTFRM_gpioPortInfo_s *portInfo_p = &PLTFRM_gpioPortList[portId];
     
          for (portPinNr = PLTFRM_GPIO_PORT_PIN_0;
               portPinNr <= PLTFRM_GPIO_PORT_PIN_7;
               portPinNr ++)
          {
               if (portInfo_p->pinInfo[portPinNr].devId == PLTFRM_DUMMY_DEV_ID)
               {
                   PLTFRM_sts_t sts;
                   sts = PLTFRM_allocGPIOPin(PLTFRM_GENERIC_DEV_ID,
                                             portId,
                                             portPinNr,
                                             PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                                             PLTFRM_GPIO_OUTPUT_LOW,
                                             PLTFRM_GPIO_INT_EDGE_NA,
                                             NULL);
                   if (sts != PLTFRM_STS_SUCCESS)
                      SYS_fatal(SYS_FATAL_ERR_142);
               }
          }
     }
  } 
#endif

  __bis_SR_register(GIE);
  
#ifdef EXT_EEPROM_EN
  {
     UINT8_t macExtAddr[10] = {0x11, 0x33, 0x55, 0x77, 0x99, 0xbb, 0xdd, 0xff, 0x00, 0x00};
     UINT16_t ckSum;
     ckSum = PLTFRM_calcCkSum16(macExtAddr, 8);
     macExtAddr[8] = ((ckSum >> 8) & 0xff);
     macExtAddr[9] = (ckSum & 0xff);
     PLTFRM_setMacExtAddr(macExtAddr);
  }
#endif


  // cc2520_softRxStart(); // Put radio into receive state
  
#ifdef CC1200_TIMING_TEST
  {
     PLTFRM_sts_t sts;
     sts = PLTFRM_allocGPIOPin(PLTFRM_CC1200_1_DEV_ID,
                               PLTFRM_GPIO_PORT_1,
                               PLTFRM_GPIO_PORT_PIN_5,
                               PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                               PLTFRM_GPIO_OUTPUT_LOW,
                               PLTFRM_GPIO_INT_EDGE_NA,
                               NULL);
     if (sts != PLTFRM_STS_SUCCESS)
        SYS_fatal(SYS_FATAL_ERR_144);

     sts = PLTFRM_allocGPIOPin(PLTFRM_CC1200_1_DEV_ID,

                               PLTFRM_GPIO_PORT_1,
                               PLTFRM_GPIO_PORT_PIN_6,
                               PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                               PLTFRM_GPIO_OUTPUT_LOW,
                               PLTFRM_GPIO_INT_EDGE_NA,
                               NULL);
     if (sts != PLTFRM_STS_SUCCESS)
        SYS_fatal(SYS_FATAL_ERR_145);

  }
#endif

  return;
}
