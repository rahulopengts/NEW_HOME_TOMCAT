/*
 * File Name: liquid_level_sensor.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: June/5/2015
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

#ifdef PLTFRM_LLS_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <lls.h>




LLS_cntxt_s LLS_cntxtList[PLTFRM_LLS_DEV_CNT] =
{
   {
      PLTFRM_LLS_1_DEV_ID,
      PLTFRM_ADC10_CHANN_0_ID,
      PLTFRM_GPIO_PORT_1,
      PLTFRM_GPIO_PORT_PIN_4
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
LLS_cntxt_s *LLS_getDevCntxt(UINT8_t devId)
{
#if PLTFRM_LLS_DEV_CNT==1
   if (LLS_cntxtList[0].devId == devId)
       return (&(LLS_cntxtList[0]));
#else
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_LLS_DEV_CNT; idx++)
   {
      if (LLS_cntxtList[idx].devId == devId)
          return (&(LLS_cntxtList[idx]));
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
PLTFRM_sts_t LLS_getSensorOutput(UINT8_t devId, UINT32_t *snsrOp_p)
{
   UINT16_t adcVal16;
   UINT32_t snsrOp, potRes;
   LLS_cntxt_s *devCntxt_p = LLS_getDevCntxt(devId);
   UINT16_t battVoltage, refV;
   PLTFRM_sts_t sts;

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   battVoltage = pltfrm_vccSense(1);

   refV = battVoltage >= (MSP_ADC10_REF_V_2_PT_5 + 100) \
		  ? PLTFRM_ADC10_REF_GEN_2PT5_VOLTS : PLTFRM_ADC10_REF_GEN_1PT5_VOLTS;

#ifdef LLS_PWR_SAVING_ENA
   // Power up the sensor and the op-amp
   sts = PLTFRM_setGPIOPinOutput(devCntxt_p->sigPortId, devCntxt_p->sigPinId, 0x0);
   if (sts != PLTFRM_STS_SUCCESS)
	   SYS_fatal(SYS_FATAL_ERR_851);
#endif

   {
      UINT8_t idx;
      for (idx=0; idx<10; idx++)
           PLTFRM_delay1MilliSec();
   }


   sts = PLTFRM_doADC10SingleConv(PLTFRM_ADC10_CHANN_0_ID,
		                          refV,
#ifdef TIMER_CLK_ACLK
                                  PLTFRM_ADC10_SH_CYCLE_COUNT_64,   // 32 KHz (31.25 microsecs * 4)
#else
#error Not Supported !!
#endif
                                  &adcVal16);
   if (sts != PLTFRM_STS_SUCCESS)
	   SYS_fatal(SYS_FATAL_ERR_849);

#ifdef LLS_PWR_SAVING_ENA
   // Power down the sensor and the op-amp
   sts = PLTFRM_setGPIOPinOutput(devCntxt_p->sigPortId, devCntxt_p->sigPinId, 0x1);
   if (sts != PLTFRM_STS_SUCCESS)
	   SYS_fatal(SYS_FATAL_ERR_850);
#endif

   snsrOp = adcVal16;
   snsrOp *= (refV == PLTFRM_ADC10_REF_GEN_2PT5_VOLTS ? MSP_ADC10_REF_V_2_PT_5 : MSP_ADC10_REF_V_1_PT_5);
   snsrOp /= 1023;

   // Voltage accross the fixed resistance (98.2 ohms) is Vf. If = Vf / 98.2.
   // Potentiometer resistance (Rp) = (Vp / Ip) = (Vcc - Vf) / (Ip) = ((Vcc - Vf) / If) = ((Vcc - Vf) * 98.2 / Vf)
   // Rp = (((Vcc - Vf) * 98.2) / Vf)

   potRes = battVoltage - snsrOp;
   potRes *= 982;
   potRes /= snsrOp;

   // SnsrOp has the sensor output in milli-volts

   // Transfer function:
   //   Vout = Vcc*(0.2*P + 0.2) +/- (2.5 % of VFSS)
   //   where P is pressure in kPa
   *snsrOp_p = potRes;

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
PLTFRM_sts_t LLS_init(UINT8_t devId)
{
   PLTFRM_sts_t sts;
   LLS_cntxt_s *devCntxt_p = LLS_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

#ifdef LLS_PWR_SAVING_ENA
   // Allocate a GPIO pin to gate power to the MP3V5050GP and op-amp
   sts = PLTFRM_allocGPIOPin(PLTFRM_LLS_1_DEV_ID,
                             devCntxt_p->sigPortId,
                             devCntxt_p->sigPinId,
                             PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                             PLTFRM_GPIO_OUTPUT_HIGH,    // Op-amp / LLS ckt powered down
                             PLTFRM_GPIO_INT_EDGE_NA,
                             NULL);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_852);
#endif

   // This device has a single analog output which is connected to the MSP430.

   sts = PLTFRM_allocADC10Chann(devCntxt_p->adcChan, devId);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_853);
   }

   return PLTFRM_STS_SUCCESS;
}

#endif
