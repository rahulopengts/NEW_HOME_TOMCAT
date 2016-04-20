/*
 * File Name: acs712.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: may/30/2015
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

#ifdef PLTFRM_ACS712_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <acs712.h>

// #define ACS712_UT_ENA

static const char _snsrName_1[] = "ACS712";
static const char _snsrMfr_1[] = "Allegro";

ACS712_cntxt_s  ACS712_cntxtList[PLTFRM_ACS712_DEV_CNT] =
{
   {
      PLTFRM_ACS712_1_CURRENT_DEV_ID,
      PLTFRM_ADC10_CHANN_0_ID,
      {_snsrName_1, _snsrMfr_1}
   },

   {
      PLTFRM_ACS712_2_CURRENT_DEV_ID,
      PLTFRM_ADC10_CHANN_1_ID,
      {_snsrName_1, _snsrMfr_1}
   },

   {
      PLTFRM_ACS712_3_CURRENT_DEV_ID,
      PLTFRM_ADC10_CHANN_2_ID,
      {_snsrName_1, _snsrMfr_1}
   },
};


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
ACS712_cntxt_s *ACS712_getDevCntxt(UINT8_t devId)
{
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_ACS712_DEV_CNT; idx++)
   {
      if (ACS712_cntxtList[idx].devId == devId)
          return (&(ACS712_cntxtList[idx]));
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
PLTFRM_sts_t ACS712_getSensorOutput(UINT8_t devId, UINT32_t *snsrOp_p)
{
   UINT16_t adcVal16;
   UINT32_t snsrOp;
   ACS712_cntxt_s *devCntxt_p = ACS712_getDevCntxt(devId);
   PLTFRM_sts_t sts;
   UINT16_t battVoltage, refV;

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   battVoltage = pltfrm_vccSense(1);

   refV = battVoltage >= (MSP_ADC10_REF_V_2_PT_5 + 100) \
		  ? PLTFRM_ADC10_REF_GEN_2PT5_VOLTS : PLTFRM_ADC10_REF_GEN_1PT5_VOLTS;


   sts = PLTFRM_doADC10SingleConv(devCntxt_p->adcChan,
		                          refV,
#ifdef TIMER_CLK_ACLK
                                  PLTFRM_ADC10_SH_CYCLE_COUNT_64,   // 32 KHz (31.25 microsecs * 4)
#else
#error Not Supported !!
#endif
                                  &adcVal16);

   snsrOp = adcVal16;
   snsrOp *= (refV == PLTFRM_ADC10_REF_GEN_2PT5_VOLTS ? MSP_ADC10_REF_V_2_PT_5 : MSP_ADC10_REF_V_1_PT_5);
   snsrOp /= 1023;

   *snsrOp_p = snsrOp;  // in milli-volts

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
PLTFRM_sts_t ACS712_init(UINT8_t devId)
{
   PLTFRM_sts_t sts;
   ACS712_cntxt_s *devCntxt_p = ACS712_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   // This device has a single analog output which is connected to the MSP430.

   sts = PLTFRM_allocADC10Chann(devCntxt_p->adcChan, devId);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_839);
   }

   return PLTFRM_STS_SUCCESS;
}


#endif

