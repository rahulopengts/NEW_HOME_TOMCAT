/*
 * File Name: mp3v5050gp.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Nov/25/2013
 *
 *
 * Copyright (c) <2013>, <ram krishnan>
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
#ifdef PLTFRM_MP3V5050GP_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <mp3v5050gp.h>

/*
 * ----------------------------------------------------------------------------------------
* > Type:
 *    Differential/Gauge ported pressure sensor (Requires tubing)
 * > Data sheet:
 *    http://cache.freescale.com/files/sensors/doc/data_sheet/MP3V5050.pdf?fsrch=1&sr=2
 * > Vendor:
 *    Freescale
 * > Description:
 *    Integrated Silicon Pressure Sensor On-Chip Signal Conditioned,
 *    Temperature Compensated and Calibrated.
 * > Application:
 *    Monitor liquid level in a bottle/bucket/tank etc
 * > Range:
 *    Min (0 kPa), Max (50 kPa)
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
 *    Min (2.7 volts), Typical (3 volts), Max (3.3 volts)
 * > Current consumption:
 *    Typical (7 milli-amps), Max (10 milli-amps)
 * > Sensitivity:
 *    Typical (54 milli-volts/kPa)
 * > Error:
 *    2.5% maximum error over 0 deg C to 85 deg C
 * ----------------------------------------------------------------------------------------
 */


MP3V5050GP_cntxt_s MP3V5050GP_cntxtList[PLTFRM_MP3V5050GP_DEV_CNT] =
{
   {
      PLTFRM_MP3V5050GP_1_DEV_ID,
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
MP3V5050GP_cntxt_s *MP3V5050GP_getDevCntxt(UINT8_t devId)
{
#if PLTFRM_MP3V5050GP_DEV_CNT==1
   if (MP3V5050GP_cntxtList[0].devId == devId)
       return (&(MP3V5050GP_cntxtList[0]));
#else
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_MP3V5050GP_DEV_CNT; idx++)
   {
      if (MP3V5050GP_cntxtList[idx].devId == devId)
          return (&(MP3V5050GP_cntxtList[idx]));
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
PLTFRM_sts_t MP3V5050GP_getSensorOutput(UINT8_t devId, UINT32_t *snsrOp_p)
{
   UINT16_t adcVal16;
   UINT32_t snsrOp;
   MP3V5050GP_cntxt_s *devCntxt_p = MP3V5050GP_getDevCntxt(devId);
   PLTFRM_sts_t sts;
   UINT16_t battVoltage, refV;

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   battVoltage = pltfrm_vccSense(1);

   refV = battVoltage >= (MSP_ADC10_REF_V_2_PT_5 + 100) \
		  ? PLTFRM_ADC10_REF_GEN_2PT5_VOLTS : PLTFRM_ADC10_REF_GEN_1PT5_VOLTS;

   // Power up the sensor and the op-amp

   sts = PLTFRM_setGPIOPinOutput(devCntxt_p->sigPortId, devCntxt_p->sigPinId, 0x0);
   if (sts != PLTFRM_STS_SUCCESS)
	   SYS_fatal(SYS_FATAL_ERR_828);

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
                                  PLTFRM_ADC10_SH_CYCLE_COUNT_16,   // 32 KHz (31.25 microsecs * 4)
#else
#error Not Supported !!
#endif
                                  &adcVal16);

   // Power down the sensor and the op-amp
   sts = PLTFRM_setGPIOPinOutput(devCntxt_p->sigPortId, devCntxt_p->sigPinId, 0x1);
   if (sts != PLTFRM_STS_SUCCESS)
	   SYS_fatal(SYS_FATAL_ERR_829);

   snsrOp = adcVal16;
   snsrOp *= (refV == PLTFRM_ADC10_REF_GEN_2PT5_VOLTS ? MSP_ADC10_REF_V_2_PT_5 : MSP_ADC10_REF_V_1_PT_5);
   snsrOp /= 1023;

   // SnsrOp has the sensor output in milli-volts

   // Transfer function:
   //   Vout = Vcc*(0.018*P  +  0.04) + ERRORv
   //   where P is pressure in kPa
   //   Vout = 0.018*Vcc*P  +  0.04*Vcc  + ERRORv
   // P (in kPa) = (Vout -(Vcc*0.04) - ERRORv)/(0.018*Vcc)
   // P (in kPa) = ((Vout - (Vcc*0.04) - ERRORv) * 55.555) / Vcc
   // P (in Pa) = ((Voutmv - (Vccmv/25) - ERRORv*1000) * 55.555) / Vcc
   // P (in Pa) = ((Voutmv - (Vccmv/25) - ERRORmv) * 55555) / Vccmv
   // P (in Pa) = ((Voutmv*55555) - (Vccmv*2222) - (ERRORmv*55555))/Vccmv
   // P (in Pa) = ((Voutmv*55555) - (Vccmv*2222) - (ERRORmv*55555))/Vccmv
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
PLTFRM_sts_t MP3V5050GP_init(UINT8_t devId)
{
   PLTFRM_sts_t sts;
   MP3V5050GP_cntxt_s *devCntxt_p = MP3V5050GP_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   // Allocate a GPIO pin to gate power to the MP3V5050GP and op-amp
   sts = PLTFRM_allocGPIOPin(PLTFRM_MP3V5050GP_1_DEV_ID,
                             devCntxt_p->sigPortId,
                             devCntxt_p->sigPinId,
                             PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                             PLTFRM_GPIO_OUTPUT_HIGH,    // Op-amp / MP3V5050GP ckt powered down
                             PLTFRM_GPIO_INT_EDGE_NA,
                             NULL);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_827);

#ifdef MP3V5050GP_STATS_ENA
   devCntxt_p->evtCnt = 0x0;
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
