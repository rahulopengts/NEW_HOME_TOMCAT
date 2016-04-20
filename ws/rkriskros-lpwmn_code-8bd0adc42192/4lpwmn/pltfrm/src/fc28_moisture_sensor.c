/*
 * File Name: fc28_moisture_sensor.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: May/7/2015
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

#ifdef FC_28_MOISTURE_SENSOR_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <fc28_moisture_sensor.h>

static const char _snsrName_1[] = "FC_28";
static const char _snsrMfr_1[] = "China";

FC_28_cntxt_s  FC_28_cntxtList[PLTFRM_FC_28_DEV_CNT] =
{
   {
      PLTFRM_FC_28_1_DEV_ID,
      PLTFRM_ADC10_CHANN_0_ID,
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
FC_28_cntxt_s *FC_28_getDevCntxt(UINT8_t devId)
{
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_FC_28_DEV_CNT; idx++)
   {
      if (FC_28_cntxtList[idx].devId == devId)
          return (&(FC_28_cntxtList[idx]));
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
PLTFRM_sts_t FC_28_getSnsrOp(UINT8_t devId, UINT16_t *opVal_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   FC_28_cntxt_s *devCntxt_p = FC_28_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       sts = PLTFRM_STS_INV_DEV_ID;
   else
   {
       UINT16_t adcVal, vcc;

       vcc = pltfrm_vccSense(1);   // In milli-volts

       // Cannot have ADC10 reference voltage greater than Vcc.

       sts = PLTFRM_doADC10SingleConv(devCntxt_p->adcChannId,
                                      vcc >= (MSP_ADC10_REF_V_2_PT_5 + 100) ? PLTFRM_ADC10_REF_GEN_2PT5_VOLTS : PLTFRM_ADC10_REF_GEN_1PT5_VOLTS,
#ifdef TIMER_CLK_ACLK
                                      PLTFRM_ADC10_SH_CYCLE_COUNT_64,   // 32 KHz (31.25 microsecs * 4)
#else
#error Not Supported !!
#endif
                                      &adcVal);
       if (sts == PLTFRM_STS_SUCCESS)
       {
    	   UINT32_t op = adcVal;
    	   op *= (vcc >= MSP_ADC10_REF_V_2_PT_5 ? MSP_ADC10_REF_V_2_PT_5 : MSP_ADC10_REF_V_1_PT_5);
    	   op /= 1023;
           *opVal_p = (UINT16_t)op;
       }

   }

   return sts;
}


// #define FC_28_TEST_ENA

#ifdef FC_28_TEST_ENA
UINT16_t FC_28_resVal, FC_28_resValMax = 0, FC_28_resValMin = 65535;
UINT32_t FC_28_resValSampleCnt = 0;
#endif

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t FC_28_init(UINT8_t devId)
{
   PLTFRM_sts_t sts;
   FC_28_cntxt_s *devCntxt_p = FC_28_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   // This device has a single analog output which is connected to the MSP430.

   sts = PLTFRM_allocADC10Chann(devCntxt_p->adcChannId, devId);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_812);
   }


#ifdef FC_28_TEST_ENA
   for (;;)
   {
      sts = FC_28_getSnsrOp(PLTFRM_FC_28_1_DEV_ID, &FC_28_resVal);
      if (sts != PLTFRM_STS_SUCCESS)
          SYS_fatal(SYS_FATAL_ERR_814);

      if (FC_28_resVal > FC_28_resValMax)
          FC_28_resValMax = FC_28_resVal;

      if (FC_28_resVal < FC_28_resValMin)
          FC_28_resValMin = FC_28_resVal;

      FC_28_resValSampleCnt ++;

      PLTFRM_delay1MilliSec();
   }
#endif

   return PLTFRM_STS_SUCCESS;
}

#endif
