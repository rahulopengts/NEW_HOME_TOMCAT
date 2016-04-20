/*
 * File Name: battv.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Jan/7/2014
 *
 *
 * Copyright (c) <2014>, <ram krishnan>
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

#ifdef PLTFRM_BATTV_SENSE_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <battv.h>

BATT_cntxt_s BATT_cntxtList[PLTFRM_BATT_DEV_CNT] =
{
   {
      PLTFRM_BATT_1_DEV_ID,
	  PLTFRM_ADC10_CHANN_1_ID
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
BATT_cntxt_s *BATT_getDevCntxt(UINT8_t devId)
{
#if PLTFRM_BATT_DEV_CNT==1
   if (BATT_cntxtList[0].devId == devId)
       return (&(BATT_cntxtList[0]));
#else
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_BATT_DEV_CNT; idx++)
   {
      if (BATT_cntxtList[idx].devId == devId)
          return (&(BATT_cntxtList[idx]));
   }
#endif

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
PLTFRM_sts_t BATT_getVal(UINT8_t devId, UINT32_t *snsrOp_p)
{
   UINT16_t adcVal16, battVoltage, refV, idx;
   UINT32_t snsrOp;
   BATT_cntxt_s *devCntxt_p;
   PLTFRM_sts_t sts;

   devCntxt_p = BATT_getDevCntxt(devId);
   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

#ifdef PWR_SRC_SOLAR_SUPERCAP
   refV = PLTFRM_ADC10_REF_GEN_1PT5_VOLTS;
#elif defined(BATT_TYPE_LITHIUM_SINGLE_CELL)
   refV = PLTFRM_ADC10_REF_GEN_2PT5_VOLTS;
#else
   battVoltage = pltfrm_vccSense(1);
   refV = battVoltage >= (MSP_ADC10_REF_V_2_PT_5 + 100) \
		  ? PLTFRM_ADC10_REF_GEN_2PT5_VOLTS : PLTFRM_ADC10_REF_GEN_1PT5_VOLTS;
#endif

   for (idx=0; idx<10; idx++)
   PLTFRM_delay1MilliSec();

   sts = PLTFRM_doADC10SingleConv(devCntxt_p->adcChan,
		                          refV,
#ifdef TIMER_CLK_ACLK
                                  PLTFRM_ADC10_SH_CYCLE_COUNT_64,   // 32 KHz (31.25 microsecs * 4)
#else
#error Not Supported !!
#endif
                                  &adcVal16);
   if (sts != PLTFRM_STS_SUCCESS)
	   SYS_fatal(SYS_FATAL_ERR_3026);

   snsrOp = adcVal16;
   snsrOp *= (refV == PLTFRM_ADC10_REF_GEN_2PT5_VOLTS ? MSP_ADC10_REF_V_2_PT_5 : MSP_ADC10_REF_V_1_PT_5);
   snsrOp /= 1023;

#ifdef PWR_SRC_SOLAR_SUPERCAP
   // Battery o/p is divided by 11 using a resistor divider before being fed to op-amp
   snsrOp *= 11;
#elif defined(BATT_TYPE_LITHIUM_SINGLE_CELL)
   // Single cell lithium batt voltage divided by 2 and buffered by op-amp
   snsrOp *= 2;
#else
   // Battery o/p is divided by 6 using a resistor divider before being fed to op-amp
   snsrOp *= 6;

   // Handle Error of -2 %
   snsrOp += (snsrOp/50);
#endif

   *snsrOp_p = snsrOp;

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
PLTFRM_sts_t BATT_init(UINT8_t devId)
{
   PLTFRM_sts_t sts;
   BATT_cntxt_s *devCntxt_p;

   devCntxt_p = BATT_getDevCntxt(devId);
   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   sts = PLTFRM_allocADC10Chann(devCntxt_p->adcChan, devId);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_231);
   }

   return PLTFRM_STS_SUCCESS;
}

#endif
