/*
 * File Name: mpl115a2.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Oct/18/2014
 *
 *
 * Copyright (c) <2014>, <ram krishnan>
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

// #define MPL115A2_UT_ENA

#ifdef PLTFRM_MPL115A2_SENSOR_ENA

#define MPL115A2_SHUTDOWN_CNTRL_ENA

// #define MPL115A2_UT_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <i2c_sw.h>
#include <mpl115a2.h>

UINT16_t MPL115A2_readRegFlr1Cnt = 0,
         MPL115A2_readRegFlr2Cnt = 0,
         MPL115A2_readRegOkCnt = 0,
         MPL115A2_readRegFlr3Cnt = 0;


MPL115A2_cntxt_s MPL115A2_cntxtList[PLTFRM_MPL115A2_DEV_CNT] =
{
   {
     PLTFRM_MPL115A2_1_DEV_ID,
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
MPL115A2_cntxt_s *MPL115A2_getDevCntxt(UINT8_t devId)
{
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_MPL115A2_DEV_CNT; idx++)
   {
      if (MPL115A2_cntxtList[idx].devId == devId)
          return (&(MPL115A2_cntxtList[idx]));
   }

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
PLTFRM_sts_t MPL115A2_readReg(MPL115A2_cntxt_s *devCntxt_p,
                              UINT8_t regAddr,
                              UINT8_t *rdBuff_p)
{
   UINT8_t busId;
   const UINT8_t i2cAddr = MPL115A2_I2C_ADDR_PREFIX;  // 7 bit
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   // All registers are 8 bits long

   switch (devCntxt_p->devId)
   {
      case PLTFRM_MPL115A2_1_DEV_ID:
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           break;

      default:
           sts = PLTFRM_STS_INV_DEV_ID;
           break;
   }

   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   I2C_SW_start(busId);

   if (I2C_SW_sendDevAddr_7(busId, i2cAddr, I2C_OPN_TYPE_WRITE) == 0x0)
   {
       I2C_SW_stop(busId);
       MPL115A2_readRegFlr1Cnt ++;
       return PLTFRM_STS_I2C_BUS_WR_ERROR;
   }

   if (I2C_SW_wrByte(busId, regAddr) == 0x0)
   {
       I2C_SW_stop(busId);
       MPL115A2_readRegFlr2Cnt ++;
       return PLTFRM_STS_I2C_BUS_WR_ERROR;
   }

   I2C_SW_start(busId);

   if (I2C_SW_sendDevAddr_7(busId, i2cAddr, I2C_OPN_TYPE_READ) == 0x0)
   {
       I2C_SW_stop(busId);
       MPL115A2_readRegFlr3Cnt ++;
       return PLTFRM_STS_I2C_BUS_WR_ERROR;
   }

   I2C_SW_rdByte(busId, rdBuff_p, I2C_NO_ACK);

   I2C_SW_stop(busId);

   MPL115A2_readRegOkCnt ++;

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
PLTFRM_sts_t MPL115A2_writeReg(MPL115A2_cntxt_s *devCntxt_p,
                                UINT8_t regAddr,
                                UINT8_t regVal)
{
   UINT8_t busId;
   const UINT8_t i2cAddr = MPL115A2_I2C_ADDR_PREFIX;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   // All registers are 8 bits long

   switch (devCntxt_p->devId)
   {
      case PLTFRM_MPL115A2_1_DEV_ID:
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           break;

      default:
           sts = PLTFRM_STS_INV_DEV_ID;
           break;
   }

   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   I2C_SW_start(busId);

   if (I2C_SW_sendDevAddr_7(busId, i2cAddr, I2C_OPN_TYPE_WRITE) == 0x0)
   {
       I2C_SW_stop(busId);
       return PLTFRM_STS_I2C_BUS_WR_ERROR;
   }

   if (I2C_SW_wrByte(busId, regAddr) == 0x0)
   {
       I2C_SW_stop(busId);
       return PLTFRM_STS_I2C_BUS_WR_ERROR;
   }

   if (I2C_SW_wrByte(busId, regVal) == 0x0)
   {
       I2C_SW_stop(busId);
       return PLTFRM_STS_I2C_BUS_WR_ERROR;
   }

   I2C_SW_stop(busId);

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
PLTFRM_sts_t MPL115A2_readCoefficents(MPL115A2_cntxt_s *devCntxt_p)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal, neg = 0;
   UINT16_t tempU16;

   // a0
   // Sign bit - 1
   // Total bits - 16
   // Integer bits - 12
   // Fractional bits - 3
   sts = MPL115A2_readReg(devCntxt_p,
                          MPL115A2_A0_MSB_REG_ADDR,
                          &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   if (regVal & 0x80)
   {
	   regVal &= ~0x80;
       neg = 1;
   }
   tempU16 = regVal;
   tempU16 <<= 8;

   sts = MPL115A2_readReg(devCntxt_p,
                          MPL115A2_A0_LSB_REG_ADDR,
                          &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   tempU16 |= regVal;
   devCntxt_p->a0 = (tempU16 & 0x7);
   tempU16 >>= 3;
   devCntxt_p->a0 /= 8;
   devCntxt_p->a0 += tempU16;
   if (neg)
       devCntxt_p->a0 = -devCntxt_p->a0;

   // b1
   // Total bits - 16
   // Sign bit - 1
   // Integer bits - 2
   // Fractional bits - 13
   neg = 0;
   sts = MPL115A2_readReg(devCntxt_p,
                          MPL115A2_B1_MSB_REG_ADDR,
                          &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   if (regVal & 0x80)
   {
	   regVal &= ~0x80;
       neg = 1;
   }
   tempU16 = regVal;
   tempU16 <<= 8;

   sts = MPL115A2_readReg(devCntxt_p,
                          MPL115A2_B1_LSB_REG_ADDR,
                          &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   tempU16 |= regVal;
   devCntxt_p->b1 = (tempU16 & 0x1fff);
   tempU16 >>= 13;
   devCntxt_p->b1 /= 8192;
   devCntxt_p->b1 += tempU16;
   if (neg)
       devCntxt_p->b1 = -devCntxt_p->b1;

   // b2
   // Total bits - 16
   // Sign bit - 1
   // Integer bits - 1
   // Fractional bits - 14
   neg = 0;
   sts = MPL115A2_readReg(devCntxt_p,
                          MPL115A2_B2_MSB_REG_ADDR,
                          &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   if (regVal & 0x80)
   {
	   regVal &= ~0x80;
       neg = 1;
   }
   tempU16 = regVal;
   tempU16 <<= 8;

   sts = MPL115A2_readReg(devCntxt_p,
                          MPL115A2_B2_LSB_REG_ADDR,
                          &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   tempU16 |= regVal;
   devCntxt_p->b2 = (tempU16 & 0x3fff);
   tempU16 >>= 14;
   devCntxt_p->b2 /= 16384;
   devCntxt_p->b2 += tempU16;
   if (neg)
       devCntxt_p->b2 = -devCntxt_p->b2;

   // c12
   // Total bits - 14
   // Sign bits - 1
   // Integer bits - 0
   // Fractional bits - 13
   // Padding after decimal point - 9
   neg = 0;
   sts = MPL115A2_readReg(devCntxt_p,
                          MPL115A2_C12_MSB_REG_ADDR,
                          &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   if (regVal & 0x80)
   {
	   regVal &= ~0x80;
       neg = 1;
   }
   tempU16 = regVal;
   tempU16 <<= 8;

   sts = MPL115A2_readReg(devCntxt_p,
                          MPL115A2_C12_LSB_REG_ADDR,
                          &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   tempU16 |= regVal;
   // The 2 least significant bits are 0
   tempU16 >>= 2;

   // Divide by 2^22
   devCntxt_p->c12 = tempU16;
   devCntxt_p->c12 /= 1024;
   devCntxt_p->c12 /= 4096;
   if (neg)
       devCntxt_p->c12 = -devCntxt_p->c12;

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
PLTFRM_sts_t MPL115A2_startSingleConv(UINT8_t devId,
                                      UINT16_t *tmoVal_p)
{
	PLTFRM_sts_t sts;
	MPL115A2_cntxt_s *devCntxt_p = MPL115A2_getDevCntxt(devId);

   /*
    * Data Conversion
    * This is the first step that is performed each time a new pressure reading
    * is required which is initiated by the host sending the CONVERT command. The
    * main system circuits are activated (wake) in response to the command and after
    * the conversion completes, the result is placed into the Pressure and Temperature
    * ADC output registers. The conversion completes within the maximum conversion time,
    * tc (see Row 7, in the Operating Characteristics Table). The device then enters
    * standby mode.
    */

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   if (devCntxt_p->opnMode == MPL115A2_OPN_MODE_SHUTDOWN)
   {
	   UINT8_t idx;

	   sts = PLTFRM_setGPIOPinOutput(devCntxt_p->shutDownCtrlPortId,
                                     devCntxt_p->shutDownCtrlPinId,
									 0x1);
	   if (sts != PLTFRM_STS_SUCCESS)
		   SYS_fatal(SYS_FATAL_ERR_4011);

	   /*
	    * Wait for the maximum wakeup time, tw (see Row 8, in the Operating
	    * Characteristics Table), after which another pressure reading can be
	    * taken by transitioning to the data Conversion step.
	    * "tw" is typically 3 milli-secs and max 5 milli-secs.
	    */

	   for (idx=0; idx<MPL115A2_MAX_WAKEUP_TIME_MILLI_SECS; idx++)
	        PLTFRM_delay1MilliSec();

	   devCntxt_p->opnMode = MPL115A2_OPN_MODE_ACTIVE;
	   devCntxt_p->shutDownPending = 1;
   }

   /*
    * The command byte needs to be paired with a 0x00 as part of the I2C exchange
    * to complete the passing of Start Conversions.
    * - START
    * - 0xc0
    * - 0x12
    * - 0x00
    * - STOP
    */

   sts = MPL115A2_writeReg(devCntxt_p,
                           MPL115A2_CONVERT_REG_ADDR,
                           0x0);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       return sts;
   }

   if (tmoVal_p != NULL)
       *tmoVal_p = MPL115A2_CONVERSION_TIME_MILLISECS;

   return PLTFRM_STS_OPN_IN_PROGRESS;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t MPL115A2_readPressure(UINT8_t devId,
                                   UINT32_t *pressVal_p)
{
   PLTFRM_sts_t sts;
   MPL115A2_cntxt_s *devCntxt_p = MPL115A2_getDevCntxt(devId);
   UINT16_t pADCVal, tADCVal;
   UINT8_t regVal;

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   /*
    * Padc is the 10-bit pressure ADC output of the MPL115A
    */

   sts = MPL115A2_readReg(devCntxt_p,
                           MPL115A2_P_ADC_MSB_REG_ADDR,
                           &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       goto _end;
   }

   pADCVal = regVal;
   pADCVal <<= 8;

   sts = MPL115A2_readReg(devCntxt_p,
                           MPL115A2_P_ADC_LSB_REG_ADDR,
                           &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       goto _end;
   }

   pADCVal |= regVal;
   pADCVal >>= 6; // least significant 6 bits are zero

   /*
    * Tadc is the 10-bit temperature ADC output of the MPL115A
    */

   sts = MPL115A2_readReg(devCntxt_p,
                           MPL115A2_T_ADC_MSB_REG_ADDR,
                           &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       goto _end;
   }

   tADCVal = regVal;
   tADCVal <<= 8;

   sts = MPL115A2_readReg(devCntxt_p,
                           MPL115A2_T_ADC_LSB_REG_ADDR,
                           &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       goto _end;
   }

   tADCVal |= regVal;
   tADCVal >>= 6; // least significant 6 bits are zero

   devCntxt_p->pComp = devCntxt_p->c12;
   devCntxt_p->pComp *= tADCVal;
   devCntxt_p->pComp += devCntxt_p->b1;
   devCntxt_p->pComp *= pADCVal;
   devCntxt_p->pComp += devCntxt_p->a0;

   {
      float tempFloat = devCntxt_p->b2;
      tempFloat *= tADCVal;
      devCntxt_p->pComp += tempFloat;  // in kilo-pascals
   }

   devCntxt_p->pFinal = MPL115A2_MAX_PRESSURE_OUTPUT_KPA;
   devCntxt_p->pFinal -= MPL115A2_MIN_PRESSURE_OUTPUT_KPA;
   devCntxt_p->pFinal *= devCntxt_p->pComp;
   devCntxt_p->pFinal /= 1023;
   devCntxt_p->pFinal += MPL115A2_MIN_PRESSURE_OUTPUT_KPA;
   devCntxt_p->pFinal *= 1000;  // in pascals

   *(pressVal_p) = (UINT32_t)(devCntxt_p->pFinal);

_end:
   if (devCntxt_p->shutDownPending)
   {
	   sts = PLTFRM_setGPIOPinOutput(devCntxt_p->shutDownCtrlPortId,
                                     devCntxt_p->shutDownCtrlPinId,
									 0x0);
	   if (sts != PLTFRM_STS_SUCCESS)
		   SYS_fatal(SYS_FATAL_ERR_4012);

	   devCntxt_p->opnMode = MPL115A2_OPN_MODE_SHUTDOWN;
	   devCntxt_p->shutDownPending = 0;
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
PLTFRM_sts_t MPL115A2_init(UINT8_t devId)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   MPL115A2_cntxt_s *devCntxt_p = MPL115A2_getDevCntxt(devId);

   // Allocate two GPIO pins for MP115A2's shutdown pin and reset pin
   // For now, both are pulled high. Shutdown high means the chip
   // will remain in standby until a conversion is triggered. After
   // the conversion, the chip will go back to standby mode.

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

#ifdef MPL115A2_RESET_CNTRL_ENA
   // Take chip out of reset.
   sts = PLTFRM_allocGPIOPin(PLTFRM_GENERIC_DEV_ID,
                             devCntxt_p->resetCtrlPortId,
                             devCntxt_p->resetCtrlPinId,
                             PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                             PLTFRM_GPIO_OUTPUT_HIGH,  // Active low
                             PLTFRM_GPIO_INT_EDGE_NA,
                             NULL);

   if (sts != PLTFRM_STS_SUCCESS)
   {
       // Port and bit number combination has already been
       // reserved by another device !!
       SYS_fatal(SYS_FATAL_ERR_234);
   }
#endif



#ifdef MPL115A2_SHUTDOWN_CNTRL_ENA

   /*
    * For longer periods of inactivity the user may assert the SHDN input by
    * driving this pin low to reduce system power consumption. This removes
    * power from all internal circuits, including any registers. In the shutdown
    * state, the Pressure and Temperature registers will be reset, losing any
    * previous ADC output values.
    */
   sts = PLTFRM_allocGPIOPin(PLTFRM_GENERIC_DEV_ID,
                             devCntxt_p->shutDownCtrlPortId,
							 devCntxt_p->shutDownCtrlPinId,
                             PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                             PLTFRM_GPIO_OUTPUT_HIGH,   // Active low signal
                             PLTFRM_GPIO_INT_EDGE_NA,
                             NULL);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       // Port and bit number combination has already been
       // reserved by another device !!
       SYS_fatal(SYS_FATAL_ERR_233);
   }

   devCntxt_p->opnMode = MPL115A2_OPN_MODE_ACTIVE;
#endif

   devCntxt_p->shutDownPending = 0;

   if (MPL115A2_readCoefficents(devCntxt_p) != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_235);

#ifdef MPL115A2_SHUTDOWN_CNTRL_ENA
   sts = PLTFRM_setGPIOPinOutput(devCntxt_p->shutDownCtrlPortId,
                                 devCntxt_p->shutDownCtrlPinId,
								 0x0);
   if (sts != PLTFRM_STS_SUCCESS)
	   SYS_fatal(SYS_FATAL_ERR_4013);

   devCntxt_p->opnMode = MPL115A2_OPN_MODE_SHUTDOWN;
#endif

#ifdef MPL115A2_UT_ENA
   for (;;)
   {
      UINT32_t _d, pressVal;
      UINT16_t tmoMilliSecs;

      sts = MPL115A2_startSingleConv(devId, &tmoMilliSecs);
      if (sts != PLTFRM_STS_OPN_IN_PROGRESS)
      {
          SYS_fatal(SYS_FATAL_ERR_236);
      }

      for (_d=0; _d<tmoMilliSecs + 5; _d++)
           PLTFRM_delay1MilliSec();

      sts = MPL115A2_readPressure(devId, &pressVal);
      if (sts != PLTFRM_STS_SUCCESS)
      {
          SYS_fatal(SYS_FATAL_ERR_237);
      }
   }
#endif

   return sts;
}


#endif
