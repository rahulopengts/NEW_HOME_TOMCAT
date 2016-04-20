/*
 * File Name: pltfrm_timer.c
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


#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>

#ifdef __MSP430F2274__

#define PLTFRM_STATS_ENA


UINT8_t PLTFRM_timerSts = 0x0;
PLTFRM_timerCbFunc_t PLTFRM_timerA0CbFunc_p = NULL;
PLTFRM_timerCbFunc_t PLTFRM_timerA1CbFunc_p = NULL;
PLTFRM_timerCbFunc_t PLTFRM_timer1HzPeriodicCbFunc_p = NULL;

#ifdef DEV_TYPE_RFD
SINT32_t PLTFRM_timeLeftToWakeUp = 0;
#endif

#ifdef PLTFRM_STATS_ENA
UINT16_t timerA1IntCnt = 0, 
         timerA0IntCnt = 0,
         wdtTimerCnt = 0;
#endif

void PLTFRM_timerA0ExpHndlr(void)
{
   if (PLTFRM_timerA0CbFunc_p != NULL)
       (*PLTFRM_timerA0CbFunc_p)();
}

void PLTFRM_timerA1ExpHndlr(void)
{
   if (PLTFRM_timerA1CbFunc_p != NULL)
       (*PLTFRM_timerA1CbFunc_p)();
}

void PLTFRM_timer1HzPeriodicExpHndlr(void)
{
   if (PLTFRM_timer1HzPeriodicCbFunc_p != NULL)
       (*PLTFRM_timer1HzPeriodicCbFunc_p)();
}

#ifndef DEV_TYPE_RFD
#ifdef ECLIPSE_IDE
static void __attribute__((__interrupt__(WDT_VECTOR))) PLTFRM_watchDogIntHndlr (void)
#else
#pragma vector=WDT_VECTOR
__interrupt void PLTFRM_watchDogIntHndlr(void)
#endif
{
#ifdef PLTFRM_STATS_ENA  
   wdtTimerCnt ++;
#endif   
   
   if (PLTFRM_timerSts & PLTRM_TIMER_1HZ_PERIODIC_ACTIVE_BIT)
   {
       SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_TIMER_1HZ_PERIODIC_EXPIRY;  
#if 1
       PLTFRM_exitLPM();  // rkris, dec/30/2013 - wake up micro if sleeping in LPM3
#endif
   }   
}
#else
#ifdef ECLIPSE_IDE
static void __attribute__((__interrupt__(WDT_VECTOR))) PLTFRM_wakeUpTimerHndlr (void)
#else
#pragma vector=WDT_VECTOR
__interrupt void PLTFRM_wakeUpTimerHndlr(void)
#endif
{
#ifdef PLTFRM_STATS_ENA
   wdtTimerCnt ++;
#endif

   PLTFRM_timeLeftToWakeUp --;

   if (PLTFRM_timeLeftToWakeUp <= 0)
   {
       PLTFRM_timerSts &= ~PLTRM_TIMER_WAKEUP_ACTIVE_BIT;
       SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_WAKEUP_TIMER_EXPIRY_EVT;
#if 1
       PLTFRM_exitLPM();
#endif
   }

#if defined(DEV_TYPE_RFD) && defined(RFD_KA_ENA)
   PLTFRM_exitLPM();
#endif
}
#endif



// Timer A0 interrupt service routine
#ifdef ECLIPSE_IDE
static void __attribute__((__interrupt__(TIMERA0_VECTOR))) PLTFRM_timerA0IntHndlr (void)
#else
#pragma vector=TIMERA0_VECTOR
__interrupt void PLTFRM_timerA0IntHndlr(void)
#endif
{
   // All CCIFG flags request an interrupt when their corresponding CCIE bit and
   // the GIE bit are set.
   // The TACCR0 CCIFG flag is automatically reset when the TACCR0 interrupt
   // request is serviced.

#ifdef PLTFRM_STATS_ENA
   timerA0IntCnt ++;
#endif

   // Setting MCx = 00h when Timer_A is not in use conserves power.
   TACTL &= (~(BIT4 | BIT5));  // Set MC_x to 0x0
   TACCR0 = 0x0;
   TACCTL0 &= ~(CCIE | CCIFG);
   if (PLTFRM_timerSts & PLTRM_TIMER_A0_ACTIVE_BIT)
   {
       SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_TIMER_A0_EXPIRY;
       PLTFRM_timerSts &= ~PLTRM_TIMER_A0_ACTIVE_BIT;
#if 1
       PLTFRM_exitLPM();
#endif
   }
}



// Timer A1 interrupt service routine
#ifdef ECLIPSE_IDE
static void __attribute__((__interrupt__(TIMERB0_VECTOR))) PLTFRM_timerA1IntHndlr (void)
#else
#pragma vector=TIMERB0_VECTOR
__interrupt void PLTFRM_timerA1IntHndlr(void)
#endif
{
   // All CCIFG flags request an interrupt when their corresponding CCIE bit and
   // the GIE bit are set.
   // The TACCR0 CCIFG flag is automatically reset when the TACCR0 interrupt
   // request is serviced.

#ifdef PLTFRM_STATS_ENA
   timerA1IntCnt ++;
#endif

   // Setting MCx = 00h when Timer_A is not in use conserves power.
   TBCTL &= (~(BIT4 | BIT5));  // Set MC_x to 0x0
   TBCCR0 = 0x0;
   TBCCTL0 &= ~(CCIE | CCIFG);
   if (PLTFRM_timerSts & PLTRM_TIMER_A1_ACTIVE_BIT)
   {
       SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_TIMER_A1_EXPIRY;
       PLTFRM_timerSts &= ~PLTRM_TIMER_A1_ACTIVE_BIT;
#if 1
       PLTFRM_exitLPM();
#endif
   }
}


void msp430g2553_stopTimerA0(void)
{
   __bic_SR_register(GIE);
   // Setting MCx = 00h when Timer_A is not in use conserves power.
   TACTL &= (~(BIT4 | BIT5));  // Set MC_x to 0x0
   TACCR0 = 0x0;
   TACCTL0 &= ~(CCIE | CCIFG);
   PLTFRM_timerSts &= ~PLTRM_TIMER_A0_ACTIVE_BIT;
   SYS_globalEvtMsk0 &= ~(SYS_GLOBAL_EVT_TIMER_A0_EXPIRY);
   __bis_SR_register(GIE);
}


void msp430g2553_stopTimerA1(void)
{
   __bic_SR_register(GIE);
   // Setting MCx = 00h when Timer_A is not in use conserves power.
   TBCTL &= (~(BIT4 | BIT5));  // Set MC_x to 0x0
   TBCCR0 = 0x0;
   TBCCTL0 &= ~(CCIE | CCIFG);
   PLTFRM_timerSts &= ~PLTRM_TIMER_A1_ACTIVE_BIT;
   SYS_globalEvtMsk0 &= ~(SYS_GLOBAL_EVT_TIMER_A1_EXPIRY);
   __bis_SR_register(GIE);
}
   
void msp430g2553_start32KHzTimerA0(UINT8_t val, PLTFRM_timerCbFunc_t cbFunc_p)
{
}


#define __MICRO_USECS_PER_VLO_TICK  156


PLTFRM_sts_t msp430g2553_startTimerA1(UINT16_t secs,
                                      UINT16_t milliSecs,
                                      PLTFRM_timerCbFunc_t cbFunc_p)
{
   UINT32_t regVal = secs;

   /*
    * Timer_A is a 16-bit timer/counter. Timer_A also has extensive interrupt
    * capabilities.
    * Timer_A features include:
    * > Asynchronous 16-bit timer/counter with four operating modes
    * > Selectable and configurable clock source
    * > Two or three configurable capture/compare registers
    * > Configurable outputs with PWM capability
    * > Asynchronous input and output latching
    * > Interrupt vector register for fast decoding of all Timer_A interrupts
    */

   if (PLTFRM_timerSts & PLTRM_TIMER_A1_ACTIVE_BIT)
       return PLTFRM_STS_TIMER_ALREADY_ACTIVE;

   // if (cbFunc_p == NULL)
   //     return PLTFRM_STS_INV_PARAM;

   PLTFRM_timerA1CbFunc_p = cbFunc_p;

   if (regVal > 0)
       regVal *= 1000;
   regVal += milliSecs;

   regVal *= 1000;  // in micros

   regVal /= __MICRO_USECS_PER_VLO_TICK;
   regVal ++;

    if (regVal > 65536)
        regVal = 65536;

   // 12.2.4.2 Compare Mode
   // The compare mode is selected when CAP = 0. The compare mode is used to
   // generate PWM output signals or interrupts at specific time intervals. When
   // TAR counts to the value in a TACCRx:
   //   > Interrupt flag CCIFG is set
   //   > Internal signal EQUx = 1
   //   > EQUx affects the output according to the output mode
   //   > The input signal CCI is latched into SCCI

   TBR = 0x0;  // very important

   TBCCR0 = (UINT16_t)(regVal);

   // 12.2.6 Timer_A Interrupts
   // Two interrupt vectors are associated with the 16-bit Timer_A module:
   //  > TACCR0 interrupt vector for TACCR0 CCIFG
   //  > TAIV interrupt vector for all other CCIFG flags and TAIFG
   // In compare mode, any CCIFG flag is set if TAR counts to the associated
   // TACCRx value. Software may also set or clear any CCIFG flag. All CCIFG
   // flags request an interrupt when their corresponding CCIE bit and the GIE
   // bit are set.

   // Capture/compare interrupt enable. This bit enables the interrupt request
   // of the corresponding CCIFG flag.
   TBCCTL0 = CCIE;

#ifdef TIMER_CLK_ACLK
   TBCTL = TASSEL_1; // ACLK
#else
   TBCTL = TASSEL_2 + ID_3; // SMCLK/8
#endif

   PLTFRM_timerSts |= PLTRM_TIMER_A1_ACTIVE_BIT;

   // The timer counts when MCx > 0 and the clock source is active.
   TBCTL |= MC_1;

   return PLTFRM_STS_SUCCESS;
}


PLTFRM_sts_t msp430g2553_startTimerA0(UINT16_t milliSecs,
                                      UINT16_t microSecs,
                                      PLTFRM_timerCbFunc_t cbFunc_p)
{
   UINT32_t regVal = milliSecs;

   /*
    * Timer_A is a 16-bit timer/counter. Timer_A also has extensive interrupt
    * capabilities.
    * Timer_A features include:
    * > Asynchronous 16-bit timer/counter with four operating modes
    * > Selectable and configurable clock source
    * > Two or three configurable capture/compare registers
    * > Configurable outputs with PWM capability
    * > Asynchronous input and output latching
    * > Interrupt vector register for fast decoding of all Timer_A interrupts
    */

   if (PLTFRM_timerSts & PLTRM_TIMER_A0_ACTIVE_BIT)
       return PLTFRM_STS_TIMER_ALREADY_ACTIVE;

   // if (cbFunc_p == NULL)
   //     return PLTFRM_STS_INV_PARAM;

   PLTFRM_timerA0CbFunc_p = cbFunc_p;

   // Timer A0 clock source is ACLK - VLO (156 micros per tick)
   // ta0ccr0 = time_in_micros / 156 micros

   if (regVal > 0)
       regVal *= 1000;
   regVal += (microSecs);

   regVal /= __MICRO_USECS_PER_VLO_TICK;
   regVal ++;

   TAR = 0x0;  // very important

   TACCR0 = (UINT16_t)(regVal);

   // 12.2.6 Timer_A Interrupts
   // Two interrupt vectors are associated with the 16-bit Timer_A module:
   //  > TACCR0 interrupt vector for TACCR0 CCIFG
   //  > TAIV interrupt vector for all other CCIFG flags and TAIFG
   // In compare mode, any CCIFG flag is set if TAR counts to the associated
   // TACCRx value. Software may also set or clear any CCIFG flag. All CCIFG
   // flags request an interrupt when their corresponding CCIE bit and the GIE
   // bit are set.

   // Capture/compare interrupt enable. This bit enables the interrupt request
   // of the corresponding CCIFG flag.
   TACCTL0 = CCIE;

#ifdef TIMER_CLK_ACLK
   TACTL = TASSEL_1;  // ACLK
#else
   TACTL = TASSEL_2 + ID_3; // SMCLK/8
#endif

   PLTFRM_timerSts |= PLTRM_TIMER_A0_ACTIVE_BIT;

   // The timer counts when MCx > 0 and the clock source is active.
   TACTL |= MC_1;

   return PLTFRM_STS_SUCCESS;
}

/*
 * Setting the WDTTMSEL bit to 1 selects the interval timer mode. This mode can 
 * be used to provide periodic interrupts. In interval timer mode, the WDTIFG 
 * flag is set at the expiration of the selected time interval. A PUC is not 
 * generated in interval timer mode at expiration of the selected timer interval 
 * and the WDTIFG enable bit WDTIE remains unchanged. When the WDTIE bit and the 
 * GIE bit are set, the WDTIFG flag requests an interrupt. The WDTIFG interrupt 
 * flag is automatically reset when its interrupt request is serviced, or may be 
 * reset by software. The interrupt vector address in interval timer mode is 
 * different from that in watchdog mode.
 */
#ifndef DEV_TYPE_RFD
PLTFRM_sts_t msp430g2553_startWDTimer(PLTFRM_timerCbFunc_t cbFunc_p)
{  
   if (PLTFRM_timerSts & PLTRM_TIMER_1HZ_PERIODIC_ACTIVE_BIT)
       return PLTFRM_STS_TIMER_ALREADY_ACTIVE;
   
   PLTFRM_timer1HzPeriodicCbFunc_p = cbFunc_p;
   
    // 1 second interval
    WDTCTL = WDTSSEL        // clock source - ACLK  
             | WDTTMSEL     // interval timer mode
             | WDTCNTCL     // Set WDTCNT = 0000h
             | WDTPW;       // password (0x5a00)
    IE1 |= WDTIE;   
    
    PLTFRM_timerSts |= PLTRM_TIMER_1HZ_PERIODIC_ACTIVE_BIT;   
    return PLTFRM_STS_SUCCESS;
}
#else
PLTFRM_sts_t PLTFRM_startWakeUpTimer(SINT32_t tmoSecs)
{
    /*
     * WDTISx Bits 1-0 Watchdog timer+ interval select. These bits select the watch-dog
     * timer+ interval to set the WDTIFG flag and/or generate a PUC.
     *  > 00   Watchdog clock source /32768    (1 sec @ 32768 Hz)
     *  > 01   Watchdog clock source /8192     (1/4 secs @ 32768 Hz)
     *  > 10   Watchdog clock source /512      (1/64 secs @ 32768 Hz)
     *  > 11   Watchdog clock source /64       (1/512 secs @ 32768 Hz)
     */
    if (PLTFRM_timerSts & PLTRM_TIMER_WAKEUP_ACTIVE_BIT)
        return PLTFRM_STS_TIMER_ALREADY_ACTIVE;

    if (tmoSecs == 0)
        return PLTFRM_STS_INV_PARAM;

    PLTFRM_timeLeftToWakeUp = tmoSecs;

    /*
     * The WDTCTL register also contains control bits to configure the RST/NMI pin.
     * WDTCTL is a 16-bit, password-protected, read/write register. Any read or write
     * access must use word instructions and write accesses must include the write
     * password 05Ah (WDTPW) in the upper byte.
     */

    // clock source - ACLK, interval timer mode, set WDTCNT = 0000h,
    // password (0x5a00), 1 second period


    WDTCTL = (WDTSSEL | WDTTMSEL | WDTCNTCL | WDTPW);
    IE1 |= WDTIE;

    return PLTFRM_STS_SUCCESS;
}
#endif


void msp430g2553_stopWDTimer(void)
{
   /*
    * WDTHOLD Bit 7 Watchdog timer+ hold. This bit stops the watchdog timer+.
    * Setting WDTHOLD = 1 when the WDT+ is not in use conserves power.
    *  > 0 Watchdog timer+ is not stopped
    *  > 1 Watchdog timer+ is stopped
    */
   __bic_SR_register(GIE);

   WDTCTL = WDTPW | WDTHOLD | WDTCNTCL;

   PLTFRM_timerSts &= ~PLTRM_TIMER_1HZ_PERIODIC_ACTIVE_BIT;
   SYS_globalEvtMsk0 &= ~(SYS_GLOBAL_EVT_TIMER_1HZ_PERIODIC_EXPIRY);

   __bis_SR_register(GIE);
}
#endif
