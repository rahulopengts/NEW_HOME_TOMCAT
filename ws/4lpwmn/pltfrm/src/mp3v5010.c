/*
 * File Name: mp3v5010.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Oct/31/2015
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
#ifdef PLTFRM_MP3V5010_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>

#define MP3V5010_PWR_GATING_ENA

#include <mp3v5010.h>

/*
 * ----------------------------------------------------------------------------------------
* > Type:
 *    Differential/Gauge ported pressure sensor (Requires tubing)
 * > Data sheet:
 *    http://www.freescale.com/files/sensors/doc/data_sheet/MPX5010.pdf
 * > Vendor:
 *    Freescale
 * > Description:
 *    Integrated Silicon Pressure Sensor On-Chip Signal Conditioned,
 *    Temperature Compensated and Calibrated.
 * > Application:
 *    Monitor liquid level in a bottle/bucket/tank etc
 * > Range:
 *    Min (0 kPa), Max (10 kPa)
 * > Interface:
 *    Analog
 * > Pin out:
 *    SOP (Small outline package)
 *    8 pin device with 3 in use pins
 *     - Pin #2 : Vcc
 *     - Pin #3 : Gnd
 *     - Pin #4 : Vout
 *     - Remaining pins are "no connects"
 * > Power Supply:
 *    Min (2.7v volts), Typical (3.0 volts), Max (3.3 volts)
 * > Current consumption:
 *    Typical (7 milli-amps), Max (10 milli-amps)
 * > Response Time
 *    1 millisec
 * ----------------------------------------------------------------------------------------
 */


MP3V5010_cntxt_s MP3V5010_cntxtList[PLTFRM_MP3V5010_DEV_CNT] =
{
   {
      PLTFRM_DEVICE_STATE_UNINITIALIZED,   // initialized or not
      PLTFRM_MP3V5010_1_DEV_ID,
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
MP3V5010_cntxt_s *MP3V5010_getDevCntxt(UINT8_t devId)
{
#if PLTFRM_MP3V5010_DEV_CNT==1
   if (MP3V5010_cntxtList[0].devId == devId)
       return (&(MP3V5010_cntxtList[0]));
#else
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_MP3V5010_DEV_CNT; idx++)
   {
      if (MP3V5010_cntxtList[idx].devId == devId)
          return (&(MP3V5010_cntxtList[idx]));
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
PLTFRM_sts_t MP3V5010_getSensorOutput(UINT8_t devId, UINT32_t *snsrOp_p)
{
   UINT16_t adcVal16;
   UINT32_t snsrOp;
   MP3V5010_cntxt_s *devCntxt_p = MP3V5010_getDevCntxt(devId);
   PLTFRM_sts_t sts;

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   // Sensor/op-amp gated through mosfet.
   // To power the sensor/op-amp, set gpio (connected to transistor base) to 1.
   // To power the sensor/op-amp, set gpio (connected to transistor base) to 0.

#ifdef MP3V5010_PWR_GATING_ENA
   // Power up the sensor and the op-amp
   sts = PLTFRM_setGPIOPinOutput(devCntxt_p->sigPortId, devCntxt_p->sigPinId, 0x0);
   if (sts != PLTFRM_STS_SUCCESS)
	   SYS_fatal(SYS_FATAL_ERR_4003);
#endif

   // Response time is defined as the time for the incremental change in output to go from
   // 10 % to 90 % ...
   {
      UINT8_t idx;
      for (idx=0; idx<10; idx++)
           PLTFRM_delay1MilliSec();
   }

   sts = PLTFRM_doADC10SingleConv(devCntxt_p->adcChan,
                                  PLTFRM_ADC10_REF_VCC,
#ifdef TIMER_CLK_ACLK
                                  PLTFRM_ADC10_SH_CYCLE_COUNT_16,   // 32 KHz (31.25 microsecs * 4)
#else
#error Not Supported !!
#endif
                                  &adcVal16);
   if (sts != PLTFRM_STS_SUCCESS)
	   SYS_fatal(SYS_FATAL_ERR_4004);

#ifdef MP3V5010_PWR_GATING_ENA
   // Power down the sensor and the op-amp
   sts = PLTFRM_setGPIOPinOutput(devCntxt_p->sigPortId, devCntxt_p->sigPinId, 0x1);
   if (sts != PLTFRM_STS_SUCCESS)
	   SYS_fatal(SYS_FATAL_ERR_4005);
#endif

   snsrOp = adcVal16;

   // Sensor o/p is divided by 2 before being fed to op-amp
   snsrOp *= 2;

   snsrOp *= 10000;
   snsrOp /= 1023;

#if 0
   if (snsrOp < MP3V5010_LOW_PRESSURE_THRESHOLD)
   {
	   if (0 == devCntxt_p->timeInLowPressureState)
	   {
	       devCntxt_p->lowPressureTimeStamp = SYS_upTimeInSecs;
	       devCntxt_p->timeInLowPressureState = 1;
	   }
	   else
	   {
		   UINT32_t delta = SYS_upTimeInSecs - devCntxt_p->lowPressureTimeStamp;
		   if (delta > 65535)
			   delta = 65535;
	       devCntxt_p->timeInLowPressureState = delta;
	   }
   }
   else
   {
	   devCntxt_p->lowPressureTimeStamp = (UINT32_t)0xffffffff;
	   devCntxt_p->timeInLowPressureState = 0;
   }
#endif

   // SnsrOp has the sensor output in centi-percent

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
PLTFRM_sts_t MP3V5010_init(UINT8_t devId)
{
   PLTFRM_sts_t sts;
   MP3V5010_cntxt_s *devCntxt_p = MP3V5010_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

#ifdef MP3V5010_PWR_GATING_ENA
   // Allocate a GPIO pin to gate power to the MP3V5010 and op-amp
   sts = PLTFRM_allocGPIOPin(PLTFRM_MP3V5010_1_DEV_ID,
                             devCntxt_p->sigPortId,
                             devCntxt_p->sigPinId,
                             PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                             PLTFRM_GPIO_OUTPUT_LOW,    // Op-amp / MP3V5010 ckt powered down
                             PLTFRM_GPIO_INT_EDGE_NA,
                             NULL);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_4001);
#endif

#ifdef MP3V5010_STATS_ENA
   devCntxt_p->evtCnt = 0x0;
#endif

   // This device has a single analog output which is connected to the MSP430.

   sts = PLTFRM_allocADC10Chann(devCntxt_p->adcChan, devId);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_4002);
   }

   devCntxt_p->devState = PLTFRM_DEVICE_STATE_READY;

#if 0
   devCntxt_p->timeInLowPressureState = 0;
   devCntxt_p->lowPressureTimeStamp = (UINT32_t)0xffffffff;
#endif

   return PLTFRM_STS_SUCCESS;
}

#endif




