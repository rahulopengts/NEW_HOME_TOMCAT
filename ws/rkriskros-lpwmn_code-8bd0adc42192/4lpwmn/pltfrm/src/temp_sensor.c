/*
 * File Name: temp_sensor.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Aug/10/2013
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


#ifdef PLTFRM_ON_CHIP_TEMP_SENSOR_ENA

#ifdef __IAR__
#include "io430.h"
#else
#include <msp430.h>
#endif
#include <typedefs.h>
#include <pltfrm.h>



/*
 * MSP430G2553 internal temperature sensor
 *
 * The ADC10 module supports fast, 10-bit analog-to-digital 
 * conversions. ADC10 features include a conversion channel
 * for internal temperature sensor.
 *
 *
 * > ADC10MEM
 *   The ADC core converts an analog input to its 10-bit digital 
 *   representation and stores the result in the ADC10MEM register.
 *  
 * The ADC10 core is configured by two control registers, ADC10CTL0 
 * and ADC10CTL1. The core is enabled with the ADC10ON bit. With few 
 * exceptions the ADC10 control bits can only be modified when ENC = 0. 
 * ENC must be set to 1 before any conversion can take place.
 *
 *
 * The ADC10CLK is used both as the conversion clock and to generate the 
 * sampling period. The ADC10 source clock is selected using the ADC10SSELx 
 * bits and can be divided from 1 to 8 using the ADC10DIVx bits. Possible 
 * ADC10CLK sources are SMCLK, MCLK, ACLK, and internal oscillator ADC10OSC .
 * The ADC10OSC, generated internally, is in the 5-MHz range, but varies with 
 * individual devices, supply voltage, and temperature. See the device-specific 
 * data sheet for the ADC10OSC specification.  The user must ensure that the 
 * clock chosen for ADC10CLK remains active until the end of a conversion. If 
 * the clock is removed during a conversion, the operation does not complete, 
 * and any result is invalid.
 * 
 * 22.2.8 Using the Integrated Temperature Sensor
 * To use the on-chip temperature sensor, select the analog input channel 
 * INCHx = 1010. Any other configuration is done as if an external channel was 
 * selected, including reference selection, conversion memory selection, etc.
 * The typical temperature sensor transfer function is shown in Figure 22-13. 
 * When using the temperature sensor, the sample period must be greater than 
 * 30 µs. The temperature sensor offset error is large. Deriving absolute 
 * temperature values in the application requires calibration. See the device-
 * specific data sheet for the parameters. See Section 24.2.2.1 for the calibration 
 * equations. Selecting the temperature sensor automatically turns on the on-chip 
 * reference generator as a voltage source for the temperature sensor. However, it 
 * does not enable the VREF+ output or affect the reference selections for the 
 * conversion. The reference choices for converting the temperature sensor are the 
 * same as with any other channel.
 * 
 */

#if 0   
UINT16_t adc10MinVal = 1024;
UINT16_t adc10MaxVal = 0;

SINT32_t minTemp = 1000;
SINT32_t maxTemp = -100;

UINT16_t lastAdcVal;

SINT32_t last10TempValArray[5];
UINT16_t tempIdx = 0;

SINT32_t runningAvgTemp = 0;


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
static void PLTFRM_onChipTempSnsrValProc(SINT16 tempVal)
{
   if (tempVal < minTemp)
       minTemp = tempVal;

   if (tempVal > maxTemp)
       maxTemp = tempVal;

   /*
    * The temperature sensor offset error can be large and needs to be
    * calibrated for most applications. See the device-specific data
    * sheet for parameters. See Section 24.2.2.1 for the calibration
    * equations.
    */

   last10TempValArray[tempIdx] = tempVal;
   tempIdx = (tempIdx + 1) % 5;

   {
     UINT16_t i;
     SINT32_t total = 0;

     for (i=0; i<5; i++)
         total += last10TempValArray[i];
     runningAvgTemp = total;
     runningAvgTemp /= 5;
   }

   return;
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
SINT16_t PLTFRM_readOnChipTempSensor(void)
{
   SINT32_t tmpVal = 0x0, adcVal;
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

   adcVal = ADC10MEM;
   
#if 0   
   lastAdcVal = tmpVal;
   
   if (tmpVal > adc10MaxVal)
       adc10MaxVal = tmpVal;
   
   if (tmpVal < adc10MinVal)
       adc10MinVal = tmpVal;   
#endif
   
   /*
    * With few exceptions the ADC10 control bits can only be modified when ENC = 0.
    */
   ADC10CTL0 &= ~(ENC);

   ADC10CTL0 &= ~ADC10IFG;

   /*
    * Vt =  0.00355*(T) + 0.986
    *
    * T = (Vt - .986) / 0.00355
    * where Vt = (2.5 * adcVal)/1023
    * 
    * T = 2.5 * adcVal/(1023 * 0.00355) - 277.746
    * T = 2.5 * adcVal/3.63165 - 277.746
    * T = ((2.5 * 10000 * adcVal/363) - 27775)
    * T = (68.839 * adcVal - 27775)/100
    * T = ((68.8125 * adcVal) - 27775)/100 (approximate)
    * T = (((adcVal*68) + (adcVal/2) + (adcVal/4) + (adcVal/16)) - 27775)/100
    * T = (((adcVal*64) + (adcVal*4) + (adcVal/2) + (adcVal/4) + (adcVal/16)) - 27775)/100
    * T = (((adcVal << 6) + (adcVAal << 2) + (adcVal >> 1) + (adcVal >> 2) + (adcVal >> 4)) - 27775)/100
    */
   
   tmpVal = (adcVal << 6) + (adcVal << 2);
   adcVal >>= 1;
   tmpVal += adcVal;
   adcVal >>= 1;
   tmpVal += adcVal;
   adcVal >>= 2;
   tmpVal += adcVal;
   tmpVal -= 27775;
   
   // PLTFRM_onChipTempSnsrValProc(tmpVal);

   // ADC core and reference voltage can be powered down separately.
   ADC10CTL0 &= ~(REFON | ADC10ON);  // Required to reduce power consumption

   return tmpVal;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void PLTFRM_onChipTempSensorCalib(void)
{
  /*
   * The characteristic equation of the temperature sensor voltage, in mV, is:
   * Vsense = TCsensor × Temp + Vsensor
   */
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void PLTFRM_onChipTempSensorInit(void)
{
  /*
   * With few exceptions the ADC10 control bits can only be modified when ENC = 0.
   * ENC must be set to 1 before any conversion can take place.
   */
  ADC10CTL0 &= ~(ENC);

  /*
   * Tsample > (Rs + 2000) × 7.625 × 27 pF
   * When Rs = 0 (measuring vcc),
   *
   * From SLAS735J.pdf (MSP430G2553 data sheet), page 40 :
   *   Sample time required if channel 10 is selected (ADC10ON = 1, INCHx = 0Ah)
   *   is 30 microseconds minimum (assuming Vcc is 3.0 volts).
   *   The typical equivalent impedance of the sensor is 51 Kilo-ohms. The sample time
   *   required includes the sensor-on time.
   *
   * When using ACLK (32 kHz), one clock cycle is 31.25 microsecs. Setting ADC10SHTx
   * to the minimum value (00) gives 4*31.25 microsecs -> 125 microsecs.
   */

#ifdef TIMER_CLK_ACLK
  ADC10CTL1 = INCH_10  // Channel 10 (temperature sensor)
              + ADC10SSEL_1 // ADC10 clock source select (ACLK)
              + SHS_0  // Sample-and-hold source select (00 ADC10SC bit)
              + CONSEQ_0;  // Single channel - single conversion

  // ADC10 sample-and-hold time (4 x ADC10CLKS)
  ADC10CTL0 = SREF_1  // Vr+ = Vref+ and Vr- = Vss
              + REF2_5V  // Reference-generator voltage is 2.5 volts
              + REFON   // Reference generator on
              + ADC10ON;
#else
  /*
   * The ADC10OSC, generated internally, is in the 5-MHz range, but varies 
   * with individual devices, supply voltage, and temperature. See the 
   * device-specific data sheet for the ADC10OSC specification.
   */ 
  ADC10CTL1 = INCH_10  // Channel 10 (temperature sensor)
              + ADC10DIV_7  // ADC10 clock divider (/3)
              + ADC10SSEL_0  // ADC10 clock source select (ADC10OSC)
              + SHS_0  // Sample-and-hold source select (00 ADC10SC bit)
              + CONSEQ_0;  // Single channel - single conversion

  ADC10CTL0 = SREF_1  // Vr+ = Vref+ and Vr- = Vss
              + ADC10SHT_3  // ADC10 sample-and-hold time (64 x ADC10CLKS)
              + REF2_5V  // Reference-generator voltage is 2.5 volts
              + REFON   // Reference generator on
              + ADC10ON;
#endif

  return;
}

#endif
