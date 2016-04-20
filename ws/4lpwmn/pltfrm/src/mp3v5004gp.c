/*
 * File Name: mp3v5004gp.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: April/23/2015
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

#ifdef PLTFRM_MP3V5004GP_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <mp3v5004gp.h>


#define MP3V5004GP_SNSR_OP_SCALED


MP3V5004GP_cntxt_s MP3V5004GP_cntxtList[PLTFRM_MP3V5004GP_DEV_CNT] =
{
   {
      PLTFRM_MP3V5004GP_1_DEV_ID,
      PLTFRM_ADC10_CHANN_0_ID,
      PLTFRM_GPIO_PORT_1,
      PLTFRM_GPIO_PORT_PIN_4
   }
};

/*
 * Problem with using ported pressure sensors for applications such as water
 * level monitoring is the leakage of air inside the hosing which conveys the
 * water column pressure to the sensor. As this air leaks out, water enters
 * the tube and because of surface tension, the sensor no longer sees the
 * pressure of the water column in the tank. So it is important to make sure
 * that this trapped air does not leak. I have seen sensor output drop
 * signifiantly in just 1 day. So unless this problem is fixed, these sensors
 * are useless in this application.
 *
 * Hose and clamps:
 * Check out AN1513. This document recommends a variety of hoses, hose clamps, and
 * their respective suppliers.
 *
 * Hose: Flexible
 * Tube: Non-flexible
 *
 * By using a hose, a sensor can be located in a convenient place away from the
 * actual sensing location which could be a hazardous and difficult area to reach.
 * There are many types of hoses on the market. They have different wall thicknesses,
 * working pressures, working temperatures, material compositions, and media
 * compatibilities.
 *
 * At WiSense, we are using Tygon tubing
 * - Tygon Lab Tubing, Non-DEHP, 3/32"ID X 5/32"OD from coleparmer.com
 *
 * According to AN1513, this tubing should also be used with a hose clamp.
 *
 * Hose clamps provide a strong connection with the sensor which prevents the hose
 * from working itself off, and also reduces the chance of leakage. There are many
 * types of hose clamps that can be used with the ported sensors.
 *
 * We will be using the "Clippard, reusable hose clamp (5000-2)".
 * > http://www.clippard.com/
 * > Material: Brass, self-threading, reusable
 * > Installation: Slip sleeve over hose, push onto fitting, then screw into place;
 *   tighten with wrench.
 * > The Clippard, reusable hose clamp is a brass, self–threading clamp, which provides
 *   an equally strong grip as the crimp–on type just described. The drawback is the
 *   reusable clamp is considerably more expensive.
 *
 */

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
MP3V5004GP_cntxt_s *MP3V5004GP_getDevCntxt(UINT8_t devId)
{
#if PLTFRM_MP3V5004GP_DEV_CNT==1
   if (MP3V5004GP_cntxtList[0].devId == devId)
       return (&(MP3V5004GP_cntxtList[0]));
#else
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_MP3V5004GP_DEV_CNT; idx++)
   {
      if (MP3V5004GP_cntxtList[idx].devId == devId)
          return (&(MP3V5004GP_cntxtList[idx]));
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
PLTFRM_sts_t MP3V5004GP_getSensorOutput(UINT8_t devId, UINT32_t *snsrOp_p)
{
   UINT16_t adcVal16;
   UINT32_t snsrOp;
   MP3V5004GP_cntxt_s *devCntxt_p = MP3V5004GP_getDevCntxt(devId);
   PLTFRM_sts_t sts;
   UINT16_t battVoltage, refV;

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   battVoltage = pltfrm_vccSense(1);

   refV = battVoltage >= (MSP_ADC10_REF_V_2_PT_5 + 100) \
		  ? PLTFRM_ADC10_REF_GEN_2PT5_VOLTS : PLTFRM_ADC10_REF_GEN_1PT5_VOLTS;

#ifdef MP3V5004GP_PWR_SAVING_ENA
   // Power up the sensor and the op-amp
   sts = PLTFRM_setGPIOPinOutput(devCntxt_p->sigPortId, devCntxt_p->sigPinId, 0x0);
   if (sts != PLTFRM_STS_SUCCESS)
	   SYS_fatal(SYS_FATAL_ERR_828);
#endif

   // Response time is defined as the time for the incremental change in output to go from
   // 10 % to 90 % ...
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

#ifdef MP3V5004GP_PWR_SAVING_ENA
   // Power down the sensor and the op-amp
   sts = PLTFRM_setGPIOPinOutput(devCntxt_p->sigPortId, devCntxt_p->sigPinId, 0x1);
   if (sts != PLTFRM_STS_SUCCESS)
	   SYS_fatal(SYS_FATAL_ERR_837);
#endif

   snsrOp = adcVal16;
   snsrOp *= (refV == PLTFRM_ADC10_REF_GEN_2PT5_VOLTS ? MSP_ADC10_REF_V_2_PT_5 : MSP_ADC10_REF_V_1_PT_5);
   snsrOp /= 1023;
#ifdef MP3V5004GP_SNSR_OP_SCALED
   snsrOp <<= 1;   // Sensor o/p is divided by 2 using a resistor divider (2 100k resistors)
#endif
   // SnsrOp has the sensor output in milli-volts

   // Transfer function:
   //   Vout = Vcc*(0.2*P + 0.2) +/- (2.5 % of VFSS)
   //   where P is pressure in kPa
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
PLTFRM_sts_t MP3V5004GP_init(UINT8_t devId)
{
   PLTFRM_sts_t sts;
   MP3V5004GP_cntxt_s *devCntxt_p = MP3V5004GP_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

#ifdef MP3V5004GP_STATS_ENA
   devCntxt_p->evtCnt = 0x0;
#endif

#ifdef MP3V5004GP_PWR_SAVING_ENA

   // Allocate a GPIO pin to gate power to the MP3V5050GP and op-amp
   sts = PLTFRM_allocGPIOPin(PLTFRM_MP3V5004GP_1_DEV_ID,
                             devCntxt_p->sigPortId,
                             devCntxt_p->sigPinId,
                             PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                             PLTFRM_GPIO_OUTPUT_HIGH,    // Op-amp / MP3V5004GP ckt powered down
                             PLTFRM_GPIO_INT_EDGE_NA,
                             NULL);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_836);
#endif

   // This device has a single analog output which is connected to the MSP430.

   sts = PLTFRM_allocADC10Chann(devCntxt_p->adcChan, devId);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_231);
   }

   return PLTFRM_STS_SUCCESS;
}

#endif
