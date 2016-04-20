/*
 * File Name: reed_switch.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Nov/23/2013
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

/*
 * Standex-Meder electronics
 */
#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <reed_switch.h>


#ifdef PLTFRM_REED_SWITCH_ENA


REED_SWITCH_cntxt_s REED_SWITCH_cntxtList[PLTFRM_REED_SWITCH_DEV_CNT] =
{
   {
      PLTFRM_REED_SWITCH_1_DEV_ID,
      PLTFRM_GPIO_PORT_1,
      PLTFRM_GPIO_PORT_PIN_3,
      REED_SWITCH_STATE_UNKNOWN
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
REED_SWITCH_cntxt_s *REED_SWITCH_getDevCntxt(UINT8_t devId)
{
#if PLTFRM_REED_SWITCH_DEV_CNT==1
   if (REED_SWITCH_cntxtList[0].devId == devId)
       return (&(REED_SWITCH_cntxtList[0]));
#else
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_REED_SWITCH_DEV_CNT; idx++)
   {
      if (REED_SWITCH_cntxtList[idx].devId == devId)
          return (&(REED_SWITCH_cntxtList[idx]));
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
void REED_SWITCH_updateState(REED_SWITCH_cntxt_s *devCntxt_p)
{
   UINT8_t regVal, pinMsk;

   regVal = P1IN;  // <TODO> Use API

   pinMsk = (1 << devCntxt_p->portPinNr);

   if (regVal & pinMsk)
       devCntxt_p->state = REED_SWITCH_STATE_CLOSED;
   else
       devCntxt_p->state = REED_SWITCH_STATE_OPEN;

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
void REED_SWITCH_debounceTmoCbFunc(void)
{
   UINT8_t devId = PLTFRM_REED_SWITCH_1_DEV_ID;
   REED_SWITCH_cntxt_s *devCntxt_p = REED_SWITCH_getDevCntxt(devId);
   PLTFRM_gpioIntEdge_t edge;

   if (devCntxt_p == NULL)
       SYS_fatal(SYS_FATAL_ERR_240);

   REED_SWITCH_updateState(devCntxt_p);

   if (devCntxt_p->state == REED_SWITCH_STATE_OPEN)
   {
       // look for low to high
       edge = PLTFRM_GPIO_INT_EDGE_POSITIVE;
   }
   else
   {
       // Look for high to low
       edge = PLTFRM_GPIO_INT_EDGE_NEGATIVE;
   }

   PLTFRM_gpioCfgIntEdge(devCntxt_p->portId,
                         devCntxt_p->portPinNr,
                         edge);

   devCntxt_p->evtPending = 0x1;

   REED_SWITCH_unMaskDevEvent(devId);

#ifdef REED_SWITCH_STATS_ENA
   devCntxt_p->evtCnt ++;
#endif

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
void REED_SWITCH_intCbFunc(UINT8_t devId)
{
   REED_SWITCH_cntxt_s *devCntxt_p;

   if (devId == PLTFRM_DUMMY_DEV_ID)
   {
       SYS_fatal(SYS_FATAL_ERR_241);
   }

   devCntxt_p = REED_SWITCH_getDevCntxt(devId);

   /*
    * The interrupt source has been masked at this point.
    */

   if (devCntxt_p != NULL)
   {
       // Start de-bounce timer
       msp430g2553_startTimerA1(0,
                                REED_SWITCH_DFLT_DEBOUNCE_TMO_MSECS,
                                REED_SWITCH_debounceTmoCbFunc);

#ifdef REED_SWITCH_STATS_ENA
       devCntxt_p->debounceCnt ++;
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
PLTFRM_sts_t REED_SWITCH_checkPendingEvt(UINT8_t devId, UINT8_t *evtFlag_p)
{
   REED_SWITCH_cntxt_s *devCntxt_p = REED_SWITCH_getDevCntxt(devId);

   // On an RFD, this function will be called by the APP after waking up.

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   *evtFlag_p = 0x0;

   if (devCntxt_p->evtPending)
   {
       // Note that further events are still masked. APP needs to
       // call REED_SWITCH_unMaskDevEvent() to accept future events.
       devCntxt_p->evtPending = 0x0;
       *evtFlag_p = 0x1;
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
PLTFRM_sts_t REED_SWITCH_maskDevEvent(UINT8_t devId)
{
   PLTFRM_sts_t sts;
   REED_SWITCH_cntxt_s *devCntxt_p = REED_SWITCH_getDevCntxt(devId);

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
PLTFRM_sts_t REED_SWITCH_unMaskDevEvent(UINT8_t devId)
{
   PLTFRM_sts_t sts;
   REED_SWITCH_cntxt_s *devCntxt_p = REED_SWITCH_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   // The clearIntFlag when set to 0x1, requests the function to clear
   // the interrupt pending flag before enabling the interrupt
   sts = PLTFRM_enaGPIOInt(devCntxt_p->portId,
                           devCntxt_p->portPinNr,
                           0x1);

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
PLTFRM_sts_t REED_SWITCH_getState(UINT8_t devId, REED_SWITCH_state_t *swState_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   REED_SWITCH_cntxt_s *devCntxt_p = REED_SWITCH_getDevCntxt(devId);

   if (devCntxt_p != NULL)
   {
       REED_SWITCH_updateState(devCntxt_p);

       *swState_p = devCntxt_p->state;
   }
   else
       sts = PLTFRM_STS_INV_DEV_ID;

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
PLTFRM_sts_t REED_SWITCH_ena(UINT8_t devId)
{
   REED_SWITCH_cntxt_s *devCntxt_p = REED_SWITCH_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   // Read current state of the switch
   REED_SWITCH_updateState(devCntxt_p);

   // Enable interrupt from the switch
   PLTFRM_gpioPinIntCtrlOpn(devCntxt_p->portId,
                            devCntxt_p->portPinNr,
                            PLTFRM_ENABLE_GPIO_PIN_INT, 0x1);

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
PLTFRM_sts_t REED_SWITCH_init(UINT8_t devId)
{
   PLTFRM_sts_t sts;
   REED_SWITCH_cntxt_s *devCntxt_p = REED_SWITCH_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

#ifdef REED_SWITCH_STATS_ENA
   devCntxt_p->evtCnt = 0x0;
   devCntxt_p->debounceCnt = 0x0;
#endif

   // This device has a single output pin which is connected to the MSP430.

   sts = PLTFRM_allocGPIOPin(devId,
                             devCntxt_p->portId,
                             devCntxt_p->portPinNr,
                             PLTFRM_GPIO_PIN_DIRECTION_INPUT,
                             PLTFRM_GPIO_OUTPUT_NA,
                             PLTFRM_GPIO_INT_EDGE_POSITIVE,
                             REED_SWITCH_intCbFunc);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       // Port and bit number combination has already been
       // reserved by another device !!
       SYS_fatal(SYS_FATAL_ERR_242);
   }
   else
   {
       // Switch interrupt has not been enabled yet.
       sts = PLTFRM_gpioEnablePullDown(devCntxt_p->portId, devCntxt_p->portPinNr);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_243);
       else
       {
           // Read current state of the switch
           REED_SWITCH_updateState(devCntxt_p);

#if 0
           // Enable interrupt from the switch
           PLTFRM_gpioPinIntCtrlOpn(devCntxt_p->portId,
                                    devCntxt_p->portPinNr,
                                    PLTFRM_ENABLE_GPIO_PIN_INT, 0x1);
#endif
       }
   }

   return PLTFRM_STS_SUCCESS;
}

#endif
