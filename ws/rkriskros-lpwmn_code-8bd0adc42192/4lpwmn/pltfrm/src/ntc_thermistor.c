/*
 * File Name: ntc_thermistor.c
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

#ifdef NTC_THERMISTOR_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <ntc_thermistor.h>

#if 0
static const char _snsrName_2[] = "103AT-4";
static const char _snsrMfr_2[] = "SEMITECH";
#else
static const char _snsrName_1[] = "NTCALUG02A103G";
static const char _snsrMfr_1[] = "VISHAY";
#endif

NTC_THERMISTOR_cntxt_s  NTC_THERMISTOR_cntxtList[PLTFRM_NTC_THERMISTOR_DEV_CNT] =
{
   {
      PLTFRM_NTCALUG02A_1_DEV_ID,  // Vishay - NTCALUG02A
      PLTFRM_ADC10_CHANN_0_ID,
      PLTFRM_GPIO_PORT_1,
      PLTFRM_GPIO_PORT_PIN_4,
	  NTC_THERM_NTCALUG02A_R25_VAL,   // R25 in ohms
	  NTC_THERM_NTCALUG02A_B_25_85_VAL,   // B value in kelvin
      {_snsrName_1, _snsrMfr_1}
   },

   {
      PLTFRM_NTCALUG02A_2_DEV_ID,   // Vishay - NTCALUG02A
      PLTFRM_ADC10_CHANN_1_ID,
      PLTFRM_GPIO_PORT_1,
      PLTFRM_GPIO_PORT_PIN_4,
	  NTC_THERM_NTCALUG02A_R25_VAL,   // R25 in ohms
	  NTC_THERM_NTCALUG02A_B_25_85_VAL,   // B value in kelvin
      {_snsrName_1, _snsrMfr_1}
   },
};

/*
 ********************************************************************************************
 * A thermistor is a type of resistor whose resistance varies significantly with
 * temperature, more so than in standard resistors.
 *
 * Assuming, as a first-order approximation, that the relationship between resistance
 * and temperature is linear, then:
 *
 *    Delta_R = K * Delta_T
 *    where
 *    Delta_R is change in resistance
 *    - and -
 *    Delta_T is change in temperature
 *    - and -
 *    K is first-order temperature coefficient of resistance
 *
 * Thermistors can be classified into two types, depending on the classification of K.
 * If K is positive, the resistance increases with increasing temperature, and the device
 * is called a positive temperature coefficient (PTC) thermistor, or posistor. If K is
 * negative, the resistance decreases with increasing temperature, and the device is
 * called a negative temperature coefficient (NTC) thermistor.
 *
 *
 * The equation for the NTC thermistor response is:
 *       RT = R25  /  (e ^ (B25_BY_85 * ((1/T25) - (1/T))))
 * where
 *       - RT is the thermistor resistance,
 *       - T is its temperature in Kelvin,
 *       - R25 is the rated zero-power resistance at 25 deg C (example 10000 ohms),
 *       - T25 is the Kelvin temperature at 25 deg C (298.15 K).
 *       - B25_BY_85 is the B value determined by rated zero-power
 *         resistance at 25 deg C and 85 deg C.
 *
 *
 * Interface circuit:
 *
 *     Vcc
 *     ---
 *      |       Si2323DS (P-channel mosfet)  Gating power to op-amp and resistor divider
 *      |       ________
 *      |______|Source  |
 *             |        |____ (Drain)
 * P2.2 \______|Gate    |    |
 * 1:off/      |________|    |
 * 0:on                      |
 *                           |
 *       ____________________|
 *      |            |
 *      \ Thermistor |
 *      / Eg:103AT-4 |
 *      \      ______|             ____________
 *      /  +  |\                  |            |
 *      |_____| \Op-amp buffer    |            |
 *      |     |  \ _______________| MSP430 ADC |
 *      |  _  |  /    |           |  channel   |
 *      |   __| /     |           |____________|
 *      |  |  |/      |
 *      \  |__________|
 *      /
 *      \ R25 (Example: 10K for the 103AT-4)
 *      /
 *      |
 *     _|_
 *     Gnd
 *
 *
 *     V_R25 = (Nadc * ADC_refV) / 1024
 *     V_R25 = (VCC * R25)/ (R25 + RT)
 *
 *     (Nadc * ADC_refV) / 1024 = VCC * R25 / (R25 + RT)
 *     R25 + RT = VCC * R25 * 1024 / (Nadc * ADC_refV)
 *     RT = (VCC * R25 * 1024 / (Nadc * ADC_refV)) - R25
 ********************************************************************************************
 */


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
NTC_THERMISTOR_cntxt_s *NTC_THERMISTOR_getDevCntxt(UINT8_t devId)
{
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_NTC_THERMISTOR_DEV_CNT; idx++)
   {
      if (NTC_THERMISTOR_cntxtList[idx].devId == devId)
          return (&(NTC_THERMISTOR_cntxtList[idx]));
   }

   return NULL;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t NTC_THERMISTOR_getSnsrOp(UINT8_t devId, UINT32_t *opVal_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   NTC_THERMISTOR_cntxt_s *devCntxt_p = NTC_THERMISTOR_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       sts = PLTFRM_STS_INV_DEV_ID;
   else
   {
       UINT16_t vcc, adcVal;
       UINT8_t refVEnc;

       // Power up op-amp and resistor divider
       PLTFRM_setGPIOPinOutput(devCntxt_p->sigPortId, devCntxt_p->sigPinId, 0);

       PLTFRM_delay1MilliSec();

       vcc = pltfrm_vccSense(1);   // In milli-volts

#if 0
       if (vcc >= (MSP_ADC10_REF_V_2_PT_5 + 100))
    	   refVEnc = PLTFRM_ADC10_REF_GEN_2PT5_VOLTS;
       else
    	   refVEnc = PLTFRM_ADC10_REF_GEN_1PT5_VOLTS;
#else
       refVEnc =  PLTFRM_ADC10_REF_VCC;
#endif
       // Cannot have ADC10 reference voltage greater than Vcc.

       sts = PLTFRM_doADC10SingleConv(devCntxt_p->adcChannId,
                                      refVEnc,
#ifdef TIMER_CLK_ACLK
                                      PLTFRM_ADC10_SH_CYCLE_COUNT_16,   // 32 KHz (31.25 microsecs * 4)
#else
#error Not Supported !!
#endif
                                      &adcVal);

       // Power down op-amp and resistor divider
       PLTFRM_setGPIOPinOutput(devCntxt_p->sigPortId, devCntxt_p->sigPinId, 1);

       if (sts == PLTFRM_STS_SUCCESS && adcVal > 0)
       {
           UINT32_t rt;

           if (refVEnc == PLTFRM_ADC10_REF_VCC)
           {
               rt = 1023;
               rt -= adcVal;
               rt *= devCntxt_p->r25;
               rt /= adcVal;
           }
           else
           {
               rt = vcc;
               rt *= devCntxt_p->r25;
               rt /= adcVal;
               rt <<= 10;
               rt = rt / (refVEnc ==  PLTFRM_ADC10_REF_GEN_2PT5_VOLTS ? MSP_ADC10_REF_V_2_PT_5 : MSP_ADC10_REF_V_1_PT_5);
               rt -= devCntxt_p->r25;
           }

           *opVal_p = rt;
       }
       else
    	   SYS_fatal(SYS_FATAL_ERR_878);
   }

   return sts;
}


// #define NTC_THERMISTOR_TEST_ENA

#ifdef NTC_THERMISTOR_TEST_ENA
UINT132_t NT_resVal, NT_resValMax = -999999, NT_resValMin = 999999;
UINT32_t NT_resValSampleCnt = 0;
#endif

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t NTC_THERMISTOR_init(UINT8_t devId)
{
   PLTFRM_sts_t sts;
   NTC_THERMISTOR_cntxt_s *devCntxt_p = NTC_THERMISTOR_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   // This device has a single analog output which is connected to the MSP430.

   sts = PLTFRM_allocADC10Chann(devCntxt_p->adcChannId, devId);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_812);
   }

   sts = PLTFRM_allocGPIOPin(devId,
                             devCntxt_p->sigPortId,
                             devCntxt_p->sigPinId,
                             PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                             PLTFRM_GPIO_OUTPUT_HIGH,    // Op-amp / divider ckt powered down
                             PLTFRM_GPIO_INT_EDGE_NA,
                             NULL);
   if (sts != PLTFRM_STS_SUCCESS)
   {
	   UINT8_t devId =  PLTFRM_getGPIOPinDevId(devCntxt_p->sigPortId, devCntxt_p->sigPinId);

	   // It is possible that multiple thermistors have the same power enable pin.

	   if (!((devId >= PLTFRM_NTCALUG02A_1_DEV_ID)
		      && (devId < (PLTFRM_NTCALUG02A_1_DEV_ID + PLTFRM_NTC_THERMISTOR_DEV_CNT))))
       {
          SYS_fatal(SYS_FATAL_ERR_815);
       }
   }

#ifdef NTC_THERMISTOR_TEST_ENA
   for (;;)
   {
      sts = NTC_THERMISTOR_getSnsrOp(PLTFRM_NTCALUG02A_1_DEV_ID, &NT_resVal);
      if (sts != PLTFRM_STS_SUCCESS)
          SYS_fatal(SYS_FATAL_ERR_814);

      if (NT_resVal > NT_resValMax)
          NT_resValMax = NT_resVal;

      if (NT_resVal < NT_resValMin)
          NT_resValMin = NT_resVal;

      NT_resValSampleCnt ++;

      PLTFRM_delay1MilliSec();
   }
#endif

   return PLTFRM_STS_SUCCESS;
}

#endif
