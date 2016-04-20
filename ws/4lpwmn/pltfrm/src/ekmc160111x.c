/*
 * File Name: ekmc160111x.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Nov/18/2013
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

#ifdef PLTFRM_EKMC160111X_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <ekmc160111x.h>

SINT8_t EKMC160111X_debounceIntervalSecs = 0;

/*
 * EKMC160111X
 * > Panasonic PIR motion sensor (VZ series)
 * > Three pins (Vcc, GND, Signal(active high))
 * > Vcc range (3.0 volts to 6.0 volts)
 * > Current consumption (170 microamps + output current (max 100 microamps))
 *
 *
 * <Data sheets>
 * http://pewa.panasonic.com/assets/pcsd/catalog/papirs-ekmc-catalog.pdf
 * http://www1.futureelectronics.com/doc/PANASONIC%20ELECTRIC%20WORKS/EKMC1601112.pdf
 */


EKMC160111X_cntxt_s EKMC160111X_cntxtList[PLTFRM_EKMC160111X_DEV_CNT] =
{
   {
      PLTFRM_EKMC160111X_1_DEV_ID,
	  EKMC160111X_DEV_STATE_UNKNOWN,
      PLTFRM_GPIO_PORT_1,
      PLTFRM_GPIO_PORT_PIN_6,
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
EKMC160111X_cntxt_s *EKMC160111X_getDevCntxt(UINT8_t devId)
{
#if PLTFRM_EKMC160111X_DEV_CNT==1
   if (EKMC160111X_cntxtList[0].devId == devId)
       return (&(EKMC160111X_cntxtList[0]));
#else
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_EKMC160111X_DEV_CNT; idx++)
   {
      if (EKMC160111X_cntxtList[idx].devId == devId)
          return (&(EKMC160111X_cntxtList[idx]));
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
PLTFRM_sts_t EKMC160111X_maskDevEvent(UINT8_t devId)
{
   PLTFRM_sts_t sts;
   EKMC160111X_cntxt_s *devCntxt_p = EKMC160111X_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   sts = PLTFRM_disGPIOInt(devCntxt_p->portId, devCntxt_p->portPinNr);

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
PLTFRM_sts_t EKMC160111X_unMaskDevEvent(UINT8_t devId)
{
   PLTFRM_sts_t sts;
   EKMC160111X_cntxt_s *devCntxt_p = EKMC160111X_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   // The clearIntFlag when set to 0x1, requests the function to clear
   // the interrupt pending flag before enabling the interrupt
   sts = PLTFRM_enaGPIOInt(devCntxt_p->portId,
                           devCntxt_p->portPinNr,
                           0x1);

   return sts;
}


UINT16_t EKMC160111X_intCnt = 0;

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void EKMC160111X_intCbFunc(UINT8_t devId)
{
   EKMC160111X_cntxt_s *devCntxt_p;

   if (devId == PLTFRM_DUMMY_DEV_ID)
   {
       SYS_fatal(SYS_FATAL_ERR_220);
   }

   devCntxt_p = EKMC160111X_getDevCntxt(devId);

   /*
    * This function is called from .
    * The interrupt source has been masked at this point.
    */

   if (devCntxt_p != NULL)
   {
       // Motion detected by this device.
       devCntxt_p->evtPending = 0x1;

       // Mask further interrupts from this device !!
       PLTFRM_disGPIOInt(devCntxt_p->portId, devCntxt_p->portPinNr);

#ifdef EKMC160111X_STATS_ENA
       devCntxt_p->intCnt ++;
#endif
   }

   return;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t EKMC160111X_checkPendingEvt(UINT8_t devId, UINT8_t *evtFlag_p)
{
   EKMC160111X_cntxt_s *devCntxt_p = EKMC160111X_getDevCntxt(devId);

   // Micro is woken up by the interrupt from the PIR sensor.
   // On an RFD, this function will be called by the APP after waking up.

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   *evtFlag_p = 0x0;

   if (devCntxt_p->evtPending)
   {
       // Note that further events are still masked. APP needs to
       // call EKMC160111X_unMaskDevEvent() to accept future events.
       devCntxt_p->evtPending = 0x0;
       *evtFlag_p = 0x1;
#ifdef EKMC160111X_STATS_ENA
       devCntxt_p->evtPendCnt ++;
#endif
   }

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
PLTFRM_sts_t EKMC160111X_pwrOpn(EKMC160111X_cntxt_s *devCntxt_p, UINT8_t enable)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   if (devCntxt_p->devState == EKMC160111X_DEV_STATE_UNKNOWN)
	   SYS_fatal(SYS_FATAL_ERR_4015);

   if (devCntxt_p->devState == EKMC160111X_DEV_STATE_POWERED_OFF)
   {
	   if (enable == 0)
           return sts;
   }
   else
   {
	   // POWERED_ON or READY
       if (enable)
           return sts;
   }

   sts = PLTFRM_setGPIOPinOutput(devCntxt_p->pwrOnCntrlPortId,
                                 devCntxt_p->pwrOnCntrlPortPinNr,
                                 enable ? 0 : 1);
   if (sts == PLTFRM_STS_SUCCESS)
	   devCntxt_p->devState = enable ? EKMC160111X_DEV_STATE_POWERED_ON : EKMC160111X_DEV_STATE_POWERED_OFF;
   else
	   SYS_fatal(SYS_FATAL_ERR_4014);

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
PLTFRM_sts_t EKMC160111X_powerDevDown(UINT8_t devId)
{
   PLTFRM_sts_t sts;
   EKMC160111X_cntxt_s *devCntxt_p = EKMC160111X_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   PLTFRM_gpioPinIntCtrlOpn(devCntxt_p->portId,
                            devCntxt_p->portPinNr,
                            PLTFRM_DISABLE_GPIO_PIN_INT, 0x0);

   sts = EKMC160111X_pwrOpn(devCntxt_p, 0);

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
PLTFRM_sts_t EKMC160111X_getSensorState(UINT8_t devId, UINT8_t *state_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   EKMC160111X_cntxt_s *devCntxt_p = EKMC160111X_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   if (devCntxt_p->devState == EKMC160111X_DEV_STATE_POWERED_ON)
       *state_p = 1;
   else
       *state_p = 0;

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
PLTFRM_sts_t EKMC160111X_powerDevUp(UINT8_t devId)
{
   EKMC160111X_cntxt_s *devCntxt_p = EKMC160111X_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   // Disable PIR sensor interrupt
   PLTFRM_gpioPinIntCtrlOpn(devCntxt_p->portId,
                            devCntxt_p->portPinNr,
                            PLTFRM_DISABLE_GPIO_PIN_INT, 0x0);

   if (EKMC160111X_pwrOpn(devCntxt_p, 1) == PLTFRM_STS_SUCCESS)
   {
	   EKMC160111X_debounceIntervalSecs = EKMC160111X_CKT_STABILITY_TIME_SECS;
   }

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
PLTFRM_sts_t EKMC160111X_init(UINT8_t devId)
{
   PLTFRM_sts_t sts;
   EKMC160111X_cntxt_s *devCntxt_p = EKMC160111X_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

#ifdef EKMC160111X_STATS_ENA
   devCntxt_p->intCnt = 0x0;
   devCntxt_p->evtPendCnt = 0x0;
#endif

   devCntxt_p->devState = EKMC160111X_DEV_STATE_UNKNOWN;

   // This device has a single output pin which is connected to the MSP430.

   sts = PLTFRM_allocGPIOPin(devId,
                             devCntxt_p->portId,
                             devCntxt_p->portPinNr,
                             PLTFRM_GPIO_PIN_DIRECTION_INPUT,
                             PLTFRM_GPIO_OUTPUT_NA,
                             PLTFRM_GPIO_INT_EDGE_POSITIVE,
                             EKMC160111X_intCbFunc);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       // Port and bit number combination has already been
       // reserved by another device !!
       SYS_fatal(SYS_FATAL_ERR_221);
   }

   // Device power is gated through a mosfet controlled by a pin on the micro
   // This is because device current consumption is pretty high (around 100 microamps)
   sts = PLTFRM_allocGPIOPin(devId,
                             devCntxt_p->pwrOnCntrlPortId,
                             devCntxt_p->pwrOnCntrlPortPinNr,
                             PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                             PLTFRM_GPIO_OUTPUT_HIGH,   // sensor powered down
                             PLTFRM_GPIO_INT_EDGE_NA,
                             NULL);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       // Port and bit number combination has already been
       // reserved by another device !!
       SYS_fatal(SYS_FATAL_ERR_221);
   }

   devCntxt_p->devState = EKMC160111X_DEV_STATE_POWERED_OFF;

   return PLTFRM_STS_SUCCESS;
}

#endif
