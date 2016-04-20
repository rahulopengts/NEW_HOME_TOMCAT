/*
 * File Name: wsms100.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: March/24/2015
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

#ifdef WSMS100_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <wsms100.h>

static const char _snsrName_1[] = "WSMS100";
static const char _snsrMfr_1[] = "WISENSE";

WSMS100_cntxt_s  WSMS100_cntxtList[PLTFRM_WSMS100_DEV_CNT] =
{
   {
      PLTFRM_WSMS100_1_DEV_ID,
      PLTFRM_ADC10_CHANN_2_ID,
      PLTFRM_GPIO_PORT_4,   // Square wave o/p
      PLTFRM_GPIO_PORT_PIN_4,
      {_snsrName_1, _snsrMfr_1}
   }
};


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
WSMS100_cntxt_s *WSMS100_getDevCntxt(UINT8_t devId)
{
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_WSMS100_DEV_CNT; idx++)
   {
      if (WSMS100_cntxtList[idx].devId == devId)
          return (&(WSMS100_cntxtList[idx]));
   }

   return NULL;
}

// For 333.333 KHz and  duty cycle of 50 %  TBCCR0=24-1 / TBCCR1=12
// For 666.666 KHz and  duty cycle of 50 %  TBCCR0=12-1 / TBCCR1=6


void WSM100_startSqWave(void)
{
  P4DIR |= BIT4;                            // P4.4 and P4.5 output
  P4SEL |= BIT4;                            // P4.4 and P4.5 TA1/2 otions
  TBCCR0 = 24 -1; //  8 - 1; // 512 - 1;    // PWM Period
  TBCCTL1 = OUTMOD_7;                       // TACCR1 reset/set
  TBCCR1 = 12;                        // TACCR1 PWM duty cycle
  TBCTL = TBSSEL_2 | MC_1;                  // SMCLK, up mode
}

void WSM100_stopSqWave(void)
{
  TBCTL = 0x0;
  P4SEL &= ~BIT4;
  P4OUT &= ~BIT4;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t WSMS100_getSnsrOp(UINT8_t devId, UINT16_t *opVal_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   WSMS100_cntxt_s *devCntxt_p = WSMS100_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       sts = PLTFRM_STS_INV_DEV_ID;
   else
   {
       UINT16_t vcc, adcVal;
       UINT8_t refVEnc;

       // Start the square wave ...

#ifndef WSM100_CONT_TEST
       WSM100_startSqWave();
#endif

       vcc = pltfrm_vccSense(1);   // In milli-volts
       if (vcc >= (MSP_ADC10_REF_V_2_PT_5 + 100))
    	   refVEnc = PLTFRM_ADC10_REF_GEN_2PT5_VOLTS;
       else
    	   refVEnc = PLTFRM_ADC10_REF_GEN_1PT5_VOLTS;

       {
          UINT16_t idx;
          for (idx=0; idx<200; idx++)
               PLTFRM_delay1MilliSec();
       }
       // Cannot have ADC10 reference voltage greater than Vcc.

       sts = PLTFRM_doADC10SingleConv(devCntxt_p->adcChannId,
                                      refVEnc,
#ifdef TIMER_CLK_ACLK
                                      PLTFRM_ADC10_SH_CYCLE_COUNT_16,   // 32 KHz (31.25 microsecs * 4)
#else
#error Not Supported !!
#endif
                                      &adcVal);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           UINT32_t rt;

           rt = adcVal;
           rt *= (refVEnc ==  PLTFRM_ADC10_REF_GEN_2PT5_VOLTS ? MSP_ADC10_REF_V_2_PT_5 : MSP_ADC10_REF_V_1_PT_5);
           rt = rt / 1023;

           rt <<= 1;   // sensor output is divided by 2 before being fed to the ADC10

           // Take ratio of filter o/p to supply voltage
           rt *= 10000;
           rt /= vcc;

           *opVal_p = (UINT16_t)rt;
           *opVal_p = 10000 - *opVal_p;
       }

#ifndef WSM100_CONT_TEST
       WSM100_stopSqWave();
#endif
   }

   return sts;
}

#ifdef WSM100_CONT_TEST
UINT32_t _senseCnt = 0, _maxOpVal = 0, _minOpVal = 100000;
#endif

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t WSMS100_init(UINT8_t devId)
{
   PLTFRM_sts_t sts;
   WSMS100_cntxt_s *devCntxt_p = WSMS100_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   // This device has a single analog output which is connected to the MSP430.

   sts = PLTFRM_allocADC10Chann(devCntxt_p->adcChannId, devId);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_812);
   }

   sts = PLTFRM_reserveGPIOPin(PLTFRM_WSMS100_1_DEV_ID,
                               devCntxt_p->sqWavePortId,
                               devCntxt_p->sqWavePinId);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_815);


#ifdef WSM100_CONT_TEST
   WSM100_startSqWave();
   for (;;)
   {
	   UINT16_t opVal = 0;
	   WSMS100_getSnsrOp(PLTFRM_WSMS100_1_DEV_ID, &opVal);
	   if (opVal > _maxOpVal)
		   _maxOpVal = opVal;
	   if (opVal < _minOpVal)
		   _minOpVal = opVal;
	   _senseCnt++;
   }
#endif

   return PLTFRM_STS_SUCCESS;
}

#endif
