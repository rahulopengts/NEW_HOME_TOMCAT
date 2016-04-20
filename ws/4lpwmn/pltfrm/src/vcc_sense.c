/*
 * File Name: vcc_sense.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Aug/11/2013
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


#ifdef __IAR__
#include "io430.h"
#else
#include <msp430.h>
#endif
#include <typedefs.h>
#include <pltfrm.h>


#ifdef PLTFRM_ON_CHIP_VCC_SENSE_ENA

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
void pltfrm_vccSenseInit(void)
{
   /*
    * The ADC10OSC, generated internally, is in the 5-MHz range, but varies 
    * with individual devices, supply voltage, and temperature. See the 
    * device-specific data sheet for the ADC10OSC specification.
    */ 

   /*
    * Tsample > (Rs + 2000) × 7.625 × 27 pF
    * When Rs = 0 (measuring vcc),
    *
    * From SLAS735J.pdf (MSP430G2553 data sheet), page 40.
    *   VMID(sample)
    *   Sample time required if channel 11 is selected (ADC10ON = 1, INCHx = 0Bh) is
    *   minimum 1220 nano seconds -> 1.220 microsec.
    *
    * When using ACLK (32 kHz), one clock cycle is 31.25 microsecs. Setting ADC10SHTx
    * to the minimum value (00) gives 4*31.25 microsecs -> 125 microsecs.
    */ 

   /*
    * The ADC10 module contains a built-in voltage reference with two selectable voltage levels.
    * Setting REFON = 1 enables the internal reference. When REF2_5V = 1, the internal reference
    * is 2.5 V. When REF2_5V = 0, the reference is 1.5 V.
    */

   /*
    * With few exceptions the ADC10 control bits can only be modified when ENC = 0.
    * ENC must be set to 1 before any conversion can take place.
    */
   ADC10CTL0 &= ~(ENC);

   // Here we are measuring (Vcc - Vss)/2 using the internal 2.5 volts reference
   // voltage.

   /*
    * ADC10SHTx (Bits 12-11) -  ADC10 sample-and-hold time
    * > 00     4 × ADC10CLKs
    * > 01     8 × ADC10CLKs
    * > 10    16 × ADC10CLKs
    * > 11    64 × ADC10CLKs
    */

#ifdef TIMER_CLK_ACLK
   ADC10CTL1 = INCH_11  // Channel 11 ((Vcc - Vss/2)
               + ADC10SSEL_1 // ADC10 clock source select (ACLK)
               + SHS_0  // Sample-and-hold source select (00 ADC10SC bit)
               + CONSEQ_0;  // Single channel - single conversion

   // ADC10 sample-and-hold time (4 x ADC10CLKS)
   ADC10CTL0 = SREF_1  // Vr+ = Vref+ and Vr- = Vss
#if (MSP_VCC >= 2500)
               + REF2_5V  // Reference-generator voltage is 2.5 volts
#endif
               + REFON   // Reference generator on
               + ADC10ON;
#else
   ADC10CTL1 = ADC10DIV_7 // ADC10 clock divider (/3)
               + INCH_11  // Channel 11 ((Vcc - Vss/2)
               + ADC10SSEL_0 // ADC10 clock source select (ADC10OSC)
               + SHS_0  // Sample-and-hold source select (00 ADC10SC bit)
               + CONSEQ_0;  // Single channel - single conversion

   ADC10CTL0 = SREF_1  // Vr+ = Vref+ and Vr- = Vss
               + ADC10SHT_3  // ADC10 sample-and-hold time (64 x ADC10CLKS)
#if (MSP_VCC >= 2500)
               + REF2_5V  // Reference-generator voltage is 2.5 volts
#endif
               + REFON   // Reference generator on
               + ADC10ON;
#endif


   return;
}
#else
void pltfrm_vccSenseInit(void)
{
   return;
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
void __vccSenseInit(UINT8_t refVHi)
{
   /*
    * The ADC10OSC, generated internally, is in the 5-MHz range, but varies
    * with individual devices, supply voltage, and temperature. See the
    * device-specific data sheet for the ADC10OSC specification.
    */

   /*
    * Tsample > (Rs + 2000) × 7.625 × 27 pF
    * When Rs = 0 (measuring vcc),
    *
    * From SLAS735J.pdf (MSP430G2553 data sheet), page 40.
    *   VMID(sample)
    *   Sample time required if channel 11 is selected (ADC10ON = 1, INCHx = 0Bh) is
    *   minimum 1220 nano seconds -> 1.220 microsec.
    *
    * When using ACLK (32 kHz), one clock cycle is 31.25 microsecs. Setting ADC10SHTx
    * to the minimum value (00) gives 4*31.25 microsecs -> 125 microsecs.
    */

   /*
    * The ADC10 module contains a built-in voltage reference with two selectable voltage levels.
    * Setting REFON = 1 enables the internal reference. When REF2_5V = 1, the internal reference
    * is 2.5 V. When REF2_5V = 0, the reference is 1.5 V.
    */

   /*
    * With few exceptions the ADC10 control bits can only be modified when ENC = 0.
    * ENC must be set to 1 before any conversion can take place.
    */
   ADC10CTL0 &= ~(ENC);

   // Here we are measuring (Vcc - Vss)/2 using the internal 2.5 volts reference
   // voltage.

   /*
    * ADC10SHTx (Bits 12-11) -  ADC10 sample-and-hold time
    * > 00     4 × ADC10CLKs
    * > 01     8 × ADC10CLKs
    * > 10    16 × ADC10CLKs
    * > 11    64 × ADC10CLKs
    */

#ifdef TIMER_CLK_ACLK
   ADC10CTL1 = INCH_11  // Channel 11 ((Vcc - Vss/2)
               + ADC10SSEL_1 // ADC10 clock source select (ACLK)
               + SHS_0  // Sample-and-hold source select (00 ADC10SC bit)
               + CONSEQ_0;  // Single channel - single conversion

   // ADC10 sample-and-hold time (4 x ADC10CLKS)
   ADC10CTL0 = SREF_1  // Vr+ = Vref+ and Vr- = Vss
               + (refVHi ? REF2_5V : 0)  // Reference-generator voltage is 2.5 or 1.5 volts
               + REFON   // Reference generator on
               + ADC10ON;
#else
   ADC10CTL1 = ADC10DIV_7 // ADC10 clock divider (/3)
               + INCH_11  // Channel 11 ((Vcc - Vss/2)
               + ADC10SSEL_0 // ADC10 clock source select (ADC10OSC)
               + SHS_0  // Sample-and-hold source select (00 ADC10SC bit)
               + CONSEQ_0;  // Single channel - single conversion

   ADC10CTL0 = SREF_1  // Vr+ = Vref+ and Vr- = Vss
               + ADC10SHT_3  // ADC10 sample-and-hold time (64 x ADC10CLKS)
               + (refVHi ? REF2_5V : 0)  // Reference-generator voltage is 2.5 volts
               + REFON   // Reference generator on
               + ADC10ON;
#endif


   return;
}


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
UINT32_t pltfrm_vccSense(UINT8_t shutDownADC)
{
   UINT32_t tmpVal = 0x0;

   /*
    * Returns Vcc in millivolts.
    */

   /*
    * ADC10 channel 11 measures (Vcc - Vss)/2.
    */

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

   tmpVal = ADC10MEM;

#if (MSP_VCC >= 2500)
   tmpVal *= 5; // Vcc/2 = (2.5 * ADCval)/1023 => Vcc ~ (5 * ADCval)/1024
#else
   tmpVal *= 3; // Vcc/2 = (1.5 * ADCval)/1023 => Vcc ~ (3 * ADCval)/1024
#endif
   tmpVal *= 1000;  // In millivolts
   tmpVal >>= 10;

   /*
    * The ADC10 core is configured by two control registers, ADC10CTL0 and
    * ADC10CTL1. The core is enabled with the ADC10ON bit. With few exceptions
    * the ADC10 control bits can only be modified when ENC = 0. ENC must be set
    * to 1 before any conversion can take place.
    */
   ADC10CTL0 &= ~(ENC);

   if (shutDownADC)
   {
       // ADC core and reference voltage can be powered down separately.
       ADC10CTL0 &= ~(REFON | ADC10ON);  // Required to reduce power consumption
   }

   return tmpVal;
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
UINT32_t __vccSense(UINT8_t refVHi, UINT8_t shutDownADC)
{
   UINT32_t tmpVal = 0x0;

   /*
    * Returns Vcc in millivolts.
    */

   /*
    * ADC10 channel 11 measures (Vcc - Vss)/2.
    */

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

   tmpVal = ADC10MEM;

   if (refVHi)
       tmpVal *= 5; // Vcc/2 = (2.5 * ADCval)/1023 => Vcc ~ (5 * ADCval)/1024
   else
       tmpVal *= 3; // Vcc/2 = (1.5 * ADCval)/1023 => Vcc ~ (3 * ADCval)/1024

   tmpVal *= 1000;  // In millivolts
   tmpVal >>= 10;

   /*
    * The ADC10 core is configured by two control registers, ADC10CTL0 and
    * ADC10CTL1. The core is enabled with the ADC10ON bit. With few exceptions
    * the ADC10 control bits can only be modified when ENC = 0. ENC must be set
    * to 1 before any conversion can take place.
    */
   ADC10CTL0 &= ~(ENC);

   if (shutDownADC)
   {
       // ADC core and reference voltage can be powered down separately.
       ADC10CTL0 &= ~(REFON | ADC10ON);  // Required to reduce power consumption
   }

   return tmpVal;
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
UINT32_t pltfrm_vccSense(UINT8_t shutDownADC)
{
   UINT32_t vcc;

   __vccSenseInit(0);   // Select 1.5 V reference

   // The total settling time when REFON becomes set is approximately 30
   // microsecs.
   PLTFRM_delay1MilliSec();
   vcc = __vccSense(0, shutDownADC);

   if (vcc > 2900)
   {
       __vccSenseInit(1);  // Select 2.5 V reference
       // The total settling time when REFON becomes set is approximately 30
       // microsecs.
       PLTFRM_delay1MilliSec();
       vcc = __vccSense(1, shutDownADC);
   }

   return vcc;
}

#endif
