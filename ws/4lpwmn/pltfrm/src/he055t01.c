/*
 * File Name: he055t01.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Jan/19/2015
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

#ifdef PLTFRM_HE055T01_SENSOR_ENA

#include <he055t01.h>

HE055T01_cntxt_s  HE055T01_cntxtList[PLTFRM_HE055T01_DEV_CNT] =
{
   {PLTFRM_HE055T01_1_DEV_ID, PLTFRM_ADC10_CHANN_0_ID}
};

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
HE055T01_cntxt_s *HE055T01_getDevCntxt(UINT8_t devId)
{
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_HE055T01_DEV_CNT; idx++)
   {
      if (HE055T01_cntxtList[idx].devId == devId)
          return (&(HE055T01_cntxtList[idx]));
   }

   return NULL;
}

UINT32_t minADCValSum = 0x100000, maxADCValSum = 0, adcConvCnt = 0;
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t HE055T01_getSnsrOp(UINT8_t devId, UINT32_t *opVal_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   HE055T01_cntxt_s *devCntxt_p = HE055T01_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       sts = PLTFRM_STS_INV_DEV_ID;
   else
   {
       UINT8_t idx;
       UINT32_t adcValSum = 0, minADCVal = 0x100000, maxADCVal = 0;

       for (idx=0; idx<HE055T01_SAMPLE_CNT; idx++)
       {
           UINT16_t adcVal;

           sts = PLTFRM_doADC10SingleConv(devCntxt_p->adcChannId,
                                          PLTFRM_ADC10_REF_GEN_2PT5_VOLTS,
#ifdef TIMER_CLK_ACLK
                                          PLTFRM_ADC10_SH_CYCLE_COUNT_16,   // 32 KHz (31.25 microsecs * 4)
#else
#error Not Supported !!
#endif
                                          &adcVal);
           if (sts != PLTFRM_STS_SUCCESS)
               break;

           if (adcVal < minADCVal)
               minADCVal = adcVal;
           if (adcVal > maxADCVal)
               maxADCVal = adcVal;

           adcValSum += adcVal;

           PLTFRM_delay1MilliSec();
       }

       if (sts == PLTFRM_STS_SUCCESS)
       {
           /*
            * Voltage = 2.5 * (adcVal) / 1023 = (2500 * 1000 * (adcVal)) / 1023 micro-volts
            */

           adcValSum *= 1000;
           adcValSum /= HE055T01_SAMPLE_CNT;
           adcValSum *= 2500;
           adcValSum <<= 1;  // Measured voltage is half of voltage accross burden resistor
           adcValSum /= 1023;
           adcValSum /= HE055T01_BURDEN_RESISTANCE;   // current in micro-amps

           // HE055T01 outputs 1 mA for every 1 A of sensed current.
           // 1000 micro-amps -> 1 A
           // x micro-amps -> x / 1000 amps -> x milli-amps
           // This means adcValSum contains the sensed current in milli-amps.

           adcValSum -= HE055T01_ZERO_CURRENT_OFF;

           *opVal_p = adcValSum;

           if (*opVal_p < minADCValSum)
               minADCValSum = *opVal_p;
           if (*opVal_p > maxADCValSum)
               maxADCValSum = *opVal_p;
           adcConvCnt ++;
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
PLTFRM_sts_t HE055T01_init(UINT8_t devId)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   HE055T01_cntxt_s *devCntxt_p = HE055T01_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   sts = PLTFRM_allocADC10Chann(PLTFRM_ADC10_CHANN_0_ID, devId);

   return sts;
}


#endif
