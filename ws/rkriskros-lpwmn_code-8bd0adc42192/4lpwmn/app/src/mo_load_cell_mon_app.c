/*
 * File Name: rfd_app_load_cell_mon.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: March/5/2015
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



#ifdef MICRO_ONLY_LOAD_CELL_MON_APP_ENA
#ifdef PLTFRM_AD7797_ENA

#include <stdio.h>
#include <typedefs.h>
#include <system.h>
#include <pltfrm.h>
#include <uart.h>
#include <util.h>
#include <mo_app.h>
#include <dis.h>
#include <ad7797.h>

char MO_serialTxBuff[16];

UINT32_t MO_convTmoHndlr = 0,
         MO_uartTXDoneEvtCnt = 0,
         MO_txCnt = 0;

UINT8_t MO_ad7797ConvInProgress = 0;



/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
void MO_appInit(void)
{
   PLTFRM_sts_t sts;
   SINT32_t calOffVal;

#ifdef PLTFRM_UART_ENA
   UART_start(PLTFRM_UART_HW_1_DEV_ID);
#endif

   sts = AD7797_startSystemOffsetCal(PLTFRM_AD7797_1_DEV_ID, &calOffVal);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_616);

   sts = PLTFRM_gpioPinIntCtrlOpn(PLTFRM_GPIO_PORT_2, PLTFRM_GPIO_PORT_PIN_1,
                                  PLTFRM_ENABLE_GPIO_PIN_INT, 1);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_617);

   return;
}


/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
void MO_uartTxDoneHndlr(void)
{
   MO_uartTXDoneEvtCnt ++;
   SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ENTER_LPM_REQUEST;
}


/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
void MO_ad7797ConvTmoTmoHndlr(void)
{
   PLTFRM_sts_t sts;
   SINT32_t adcVal;

   MO_convTmoHndlr ++;

   if (MO_ad7797ConvInProgress == 0)
       SYS_fatal(SYS_FATAL_ERR_615);

   MO_ad7797ConvInProgress = 0;

   sts = AD7797_getADCVal(PLTFRM_AD7797_1_DEV_ID, &adcVal);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_614);
   }
   else
   {
       // Send the conversion result to the host node
       UINT8_t strLen;

       strLen = sprintf(MO_serialTxBuff, "<%lu", ++ MO_txCnt);
       strLen += sprintf(MO_serialTxBuff + strLen, ":%ld>", adcVal);
       UART_wrBlock((UINT8_t *)MO_serialTxBuff, strLen);
   }
}


/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
void MO_appGPIOEvtHndlr(void)
{
   if (MO_ad7797ConvInProgress)
	   return;
   else
   {
       UINT16_t tmoVal;
       PLTFRM_sts_t sts;

       MO_ad7797ConvInProgress = 1;

       sts = AD7797_startSingleConv(PLTFRM_AD7797_1_DEV_ID, &tmoVal);
       if (sts != PLTFRM_STS_OPN_IN_PROGRESS)
       {
           SYS_fatal(SYS_FATAL_ERR_612);
       }
       else
       {
           if (PLTFRM_startTimerA1(0, tmoVal, MO_ad7797ConvTmoTmoHndlr) != PLTFRM_STS_SUCCESS)
           {
               SYS_fatal(SYS_FATAL_ERR_613);
           }
           else
           {
               SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ENTER_LPM_REQUEST;
           }
       }
   }
}

#endif
#endif


