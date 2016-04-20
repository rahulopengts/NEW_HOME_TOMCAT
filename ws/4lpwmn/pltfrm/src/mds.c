/*
 * File Name: mds.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Sep/14/2015
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

#ifdef PLTFRM_MAINS_DETECT_SNSR_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <mds.h>



MDS_cntxt_s MDS_cntxtList[PLTFRM_MDS_DEV_CNT] =
{
   {
      PLTFRM_MDS_1_DEV_ID,
#ifdef MDS_USING_OPAMP_COMPARATOR_ENA
      PLTFRM_GPIO_PORT_1,
      PLTFRM_GPIO_PORT_PIN_4,
#elif MDS_USING_ADC_ENA
	  PLTFRM_ADC10_CHANN_0_ID,
#else
#error method not specified !!
#endif
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
MDS_cntxt_s *MDS_getDevCntxt(UINT8_t devId)
{
#if PLTFRM_MDS_DEV_CNT==1
   if (MDS_cntxtList[0].devId == devId)
       return (&(MDS_cntxtList[0]));
#else
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_MDS_DEV_CNT; idx++)
   {
      if (MDS_cntxtList[idx].devId == devId)
          return (&(MDS_cntxtList[idx]));
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
PLTFRM_sts_t MDS_getCurrState(UINT8_t devId, UINT8_t *state_p)
{
   PLTFRM_sts_t sts;
   MDS_cntxt_s *devCntxt_p = MDS_getDevCntxt(devId);
   UINT8_t pinVal;

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

#ifdef MDS_USING_OPAMP_COMPARATOR_ENA
   sts = PLTFRM_getGPIOPinInput(devCntxt_p->portId, devCntxt_p->portPinNr, &pinVal);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_3029);
   *state_p = pinVal == 1 ? MDS_STATE_ON : MDS_STATE_OFF;
#elif defined(MDS_USING_ADC_ENA)
   {
	   UINT16_t adcVal;

       sts = PLTFRM_doADC10SingleConv(devCntxt_p->adcChannId,
                                      PLTFRM_ADC10_REF_VCC,
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
           rt *= 1000;
           rt /= 1023;

           *state_p = (rt > 500) ? MDS_STATE_ON : MDS_STATE_OFF;
       }
   }
#else
#error method not specified !!
#endif

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
PLTFRM_sts_t MDS_init(UINT8_t devId)
{
   PLTFRM_sts_t sts;
   MDS_cntxt_s *devCntxt_p = MDS_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

#ifdef MDS_USING_OPAMP_COMPARATOR_ENA
   // This sensor has a single output pin which is connected to the MSP430.
   sts = PLTFRM_allocGPIOPin(devId,
                             devCntxt_p->portId,
                             devCntxt_p->portPinNr,
                             PLTFRM_GPIO_PIN_DIRECTION_INPUT,
                             PLTFRM_GPIO_OUTPUT_NA,
                             PLTFRM_GPIO_INT_EDGE_NA,
                             NULL);
#elif defined(MDS_USING_ADC_ENA)
   sts = PLTFRM_allocADC10Chann(devCntxt_p->adcChannId, devId);
#else
#error method not specified !!
#endif
   if (sts != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_4016);
   }
   return PLTFRM_STS_SUCCESS;
}

#endif
