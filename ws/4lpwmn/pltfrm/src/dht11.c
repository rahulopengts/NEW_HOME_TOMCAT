/*
 * File Name: dht11.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: July/4/2015
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

#ifdef PLTFRM_DHT11_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <i2c_sw.h>
#include <dht11.h>


const SINT16_t DHT11_d1CoeffValAt3Volts = -3960;  // -39.6 * 100

static const char _snsrName[] = "DHT11";
static const char _snsrMfr[] = "SENSIRON";

DHT11_cntxt_s DHT11_cntxtList[PLTFRM_DHT11_DEV_CNT] =
{
   {
     PLTFRM_DHT11_1_DEV_ID,
     PLTFRM_GPIO_PORT_3,
     PLTFRM_GPIO_PORT_PIN_4,
     {_snsrName, _snsrMfr}
   },
};


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
DHT11_cntxt_s *DHT11_getDevCntxt(UINT8_t devId)
{
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_DHT11_DEV_CNT; idx++)
   {
      if (DHT11_cntxtList[idx].devId == devId)
          return (&(DHT11_cntxtList[idx]));
   }

   return NULL;
}


UINT16_t highCnt = 0, lowCnt = 0;

typedef struct
{
  UINT16_t highCnt;
  UINT16_t lowCnt;
  UINT8_t bitVal;
} STH11_bitTiming_s;

STH11_bitTiming_s DTH11_bitTimingList[DHT11_SNSR_BIT_STREAM_LEN];

UINT8_t DHT11_byteList[5];

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t DHT11_readSnsr(UINT8_t devId, SINT16_t *temp_p, SINT16_t *humidity_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   DHT11_cntxt_s *devCntxt_p = DHT11_getDevCntxt(devId);
   UINT8_t intFlag, sigVal, respRcvd = 0;
   SINT16_t idx, bitIdx;
   STH11_bitTiming_s *bitTimingEntry_p = &DTH11_bitTimingList[0];

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   sts = PLTFRM_getGPIOPinInput(devCntxt_p->port, devCntxt_p->pin, &sigVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   // Data signal-bus free status is at high voltage level.
   if (sigVal != 0x1)
       return PLTFRM_STS_DEV_OP_ERR;

   // Bring the data line low for at least 18 milli-secs to ensure DHT’s detection of MCU's
   // start signal.
   // sts = PLTFRM_setGPIOPinOutput(devCntxt_p->port, devCntxt_p->pin, 0);
   // if (sts != PLTFRM_STS_SUCCESS)
   //    return sts;

   P3OUT &= ~BIT4;
   P3DIR |= BIT4;  // output low

   {
      for (idx=0; idx<DHT11_START_SIG_MIN_TIME_MSECS + 1; idx++)
           PLTFRM_delay1MilliSec();
   }

   PLTFRM_intSaveDisable(intFlag);

   // Bring the data line low for at least 18 milli-secs to ensure DHT’s detection of MCU's
   // start signal.

   P3DIR &= ~BIT4;  // back to input

   // Need to wait 20 to 40 microsecs ...
   // Once DHT11 brings signal low, it will keep it low for 80 microsecs

   respRcvd = 0;

   PLTFRM_delay10MicroSecs();

   for (idx=0; idx<2000; idx++)
   {
      // PLTFRM_delay1MicroSecs();

      __delay_cycles(1);
      sigVal = P3IN;
      if (!(sigVal & BIT4))
      {
          respRcvd = 1;
          break;
      }
   }

   if (respRcvd == 0)
       return PLTFRM_STS_DEV_OP_ERR;

   respRcvd = 0;
   // Gone low ..  should stay low for 80 micros

   for (lowCnt=0; lowCnt<2000; lowCnt++)
   {
      __delay_cycles(1);
      sigVal = P3IN;
      if ((sigVal & BIT4))
      {
          respRcvd = 1;  // gone high
          break;
      }
   }

   if (respRcvd == 0)
       return PLTFRM_STS_DEV_OP_ERR;

   respRcvd = 0;  // Should stay high for at least 80 micros

   for (highCnt=0; highCnt<2000; highCnt++)
   {
      __delay_cycles(1);
      sigVal = P3IN;
      if (!(sigVal & BIT4))
      {
          respRcvd = 1;
          break;
      }
   }

   if (respRcvd == 0)
       return PLTFRM_STS_DEV_OP_ERR;


   // Now wait for 40 bits
   /*
    * Data consists of decimal and integral parts. A complete data transmission
    * is 40bit, and the sensor sends higher data bit first.
    * Data format: 8-bit integral RH data + 8-bit decimal RH data
    *              + 8-bit integral T data + 8-bit decimal T data + 8-bit check sum.
    * If the data transmission is right, the check-sum should be the last 8bit of
    * "8bit integral RH data + 8bit decimal RH data + 8bit integral T data + 8 bit
    * decimal T data".
    */

   for (bitIdx=0; bitIdx<DHT11_SNSR_BIT_STREAM_LEN; bitIdx++)
   {
       // Wait for signal to go high and then measure the high part.
       respRcvd = 0;

       for (lowCnt=0; lowCnt<1000; lowCnt++)
       {
          sigVal = P3IN;
          if (sigVal & BIT4)
          {
             respRcvd = 1;
             break;
          }
       }

       if (respRcvd == 0)
           return PLTFRM_STS_DEV_OP_ERR;
       respRcvd = 0;

       // Now count the high duration

       for (highCnt=0; highCnt<1000; highCnt++)
       {
          sigVal = P3IN;
          if (!(sigVal & BIT4))
          {
             respRcvd = 1;
             break;
          }
       }

       if (respRcvd == 0)
           return PLTFRM_STS_DEV_OP_ERR;


       bitTimingEntry_p->highCnt = highCnt;
       bitTimingEntry_p->lowCnt = lowCnt;
       bitTimingEntry_p ++;
   }

   PLTFRM_intRestore(intFlag);


   bitTimingEntry_p = &DTH11_bitTimingList[0];

   respRcvd = 0;

   idx = 0;

   for (bitIdx=0; bitIdx<DHT11_SNSR_BIT_STREAM_LEN; bitIdx++)
   {
      if (bitTimingEntry_p->highCnt < 2 || bitTimingEntry_p->highCnt > 40)
          break;

      if  (bitTimingEntry_p->highCnt < 14)
           bitTimingEntry_p->bitVal = 0;
      else
           bitTimingEntry_p->bitVal = 1;

      if (bitIdx > 0 && bitIdx % 8 == 0)
      {
          idx ++;
          DHT11_byteList[idx] = 0;
      }

      DHT11_byteList[idx] <<= 1;
      DHT11_byteList[idx] |= bitTimingEntry_p->bitVal;

      bitTimingEntry_p ++;
   }

   {
      UINT16_t ckSum = 0;
      for (idx=0; idx<4; idx++)
           ckSum += DHT11_byteList[idx];

      ckSum &= 0xff;

      if (ckSum != DHT11_byteList[4])
          sts = PLTFRM_STS_DEV_OP_ERR;
      else
      {
          *temp_p = DHT11_byteList[2];
          *humidity_p = DHT11_byteList[0];
      }
   }

   return sts;
}



// #define DHT11_TEST_ENA


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t DHT11_init(UINT8_t devId)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   DHT11_cntxt_s *devCntxt_p = DHT11_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   sts = PLTFRM_reserveGPIOPin(PLTFRM_DHT11_1_DEV_ID,
                               devCntxt_p->port,
                               devCntxt_p->pin);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_211);
   }

   P3SEL &= ~BIT4;
   P3SEL2 &= ~BIT4;
   P3DIR &= ~BIT4;    // Input - data line pulled high

   PLTFRM_delay1MilliSec();

   return sts;
}

#endif
