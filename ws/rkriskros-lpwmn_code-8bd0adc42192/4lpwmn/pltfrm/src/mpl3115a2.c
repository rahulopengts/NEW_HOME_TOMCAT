/*
 * File Name: mpl3115a2.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Feb/19/2014
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

#ifdef PLTFRM_MPL3115A2_SENSOR_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <i2c_sw.h>
#include <mpl3115a2.h>

/*
 * The device is a low-power, high accuracy digital output altimeter, barometer and
 * thermometer, packaged in a 3 x 5 x 1.1 mm form factor. The complete device includes
 * a sensing element, analog and digital signal processing and an I2C interface.
 */

UINT16_t MPL3115A2_readRegFlr1Cnt = 0, MPL3115A2_readRegFlr2Cnt = 0,
         MPL3115A2_readRegFlr3Cnt = 0, MPL3115A2_readRegOkCnt = 0;


MPL3115A2_cntxt_s MPL3115A2_cntxtList[PLTFRM_MPL3115A2_DEV_CNT] =
{
   {PLTFRM_MPL3115A2_1_DEV_ID}
};


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
MPL3115A2_cntxt_s *MPL3115A2_getDevCntxt(UINT8_t devId)
{
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_MPL3115A2_DEV_CNT; idx++)
   {
      if (MPL3115A2_cntxtList[idx].devId == devId)
          return (&(MPL3115A2_cntxtList[idx]));
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
PLTFRM_sts_t MPL3115A2_readReg(MPL3115A2_cntxt_s *devCntxt_p,
                               UINT8_t regAddr,
                               UINT8_t *rdBuff_p)
{
   UINT8_t busId;
   const UINT8_t i2cAddr = MPL3115A2_I2C_ADDR_PREFIX;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   if (regAddr > MPL3115A2_ALTITUDE_DATA_USER_OFF_REG_ADDR)
       return PLTFRM_STS_INV_PARAM;

   // All registers are 8 bits long

   switch (devCntxt_p->devId)
   {
      case PLTFRM_MPL3115A2_1_DEV_ID:
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
       MPL3115A2_readRegFlr1Cnt ++;
       return PLTFRM_STS_I2C_BUS_WR_ERROR;
   }

   if (I2C_SW_wrByte(busId, regAddr) == 0x0)
   {
       I2C_SW_stop(busId);
       MPL3115A2_readRegFlr2Cnt ++;
       return PLTFRM_STS_I2C_BUS_WR_ERROR;
   }

   I2C_SW_start(busId);

   if (I2C_SW_sendDevAddr_7(busId, i2cAddr, I2C_OPN_TYPE_READ) == 0x0)
   {
       I2C_SW_stop(busId);
       MPL3115A2_readRegFlr3Cnt ++;
       return PLTFRM_STS_I2C_BUS_WR_ERROR;
   }

   I2C_SW_rdByte(busId, rdBuff_p, I2C_NO_ACK);

   I2C_SW_stop(busId);

   MPL3115A2_readRegOkCnt ++;

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
PLTFRM_sts_t MPL3115A2_writeReg(MPL3115A2_cntxt_s *devCntxt_p,
                                UINT8_t regAddr,
                                UINT8_t regVal)
{
   UINT8_t busId;
   const UINT8_t i2cAddr = MPL3115A2_I2C_ADDR_PREFIX;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   if (regAddr > MPL3115A2_ALTITUDE_DATA_USER_OFF_REG_ADDR)
       return PLTFRM_STS_INV_PARAM;

   // All registers are 8 bits long

   switch (devCntxt_p->devId)
   {
      case PLTFRM_MPL3115A2_1_DEV_ID:
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


UINT8_t regValBarInMSB = 0x0, regValBarInLSB;
UINT8_t regValTmpMSB = 0, regValTmpLSB = 0;
UINT8_t sysModRegVal = 0, ctrl1RegVal = 0;
UINT16_t delayCnt = 0, loopCnt = 0, validTmpCnt = 0;
UINT8_t regValPressureMSB, regValPressureCSB, regValPressureLSB;

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t MPL3115A2_startSingleConv(UINT8_t devId,
                                       UINT16_t *tmoVal_p)
{
	PLTFRM_sts_t sts;
	MPL3115A2_cntxt_s *devCntxt_p = MPL3115A2_getDevCntxt(devId);

   /*
    * One Shot: When SBYB is 0, the OST bit is an auto-clear bit. When OST is set,
    * the device initiates a measurement by going into active mode. Once a Pressure
    * /Altitude and Temperature measurement is completed, it clears the OST bit and
    * comes back to STANDBY mode. User shall read the value of the OST bit before
    * writing to this bit again.
    */

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   if (devCntxt_p->opnMode != MPL3115A2_OPN_MODE_STANDBY)
   {
	   return PLTFRM_STS_DEV_INV_OPN_MODE;
   }

   sts = MPL3115A2_readReg(devCntxt_p,
                           MPL3115A2_CTRL1_REG_ADDR,
                           &ctrl1RegVal);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       return sts;
   }

   if (ctrl1RegVal & MPL3115A2_CTRL_REG1_OST_BIT)
   {
	   return PLTFRM_STS_DEV_INV_OPN_MODE;
   }

   ctrl1RegVal |= MPL3115A2_CTRL_REG1_OST_BIT;

   sts = MPL3115A2_writeReg(devCntxt_p,
                            MPL3115A2_CTRL1_REG_ADDR,
                            ctrl1RegVal);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       return sts;
   }

   *tmoVal_p = MPL3115A2_OST_CONV_TMO_MSECS;

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
PLTFRM_sts_t MPL3115A2_readPressure(UINT8_t devId,
                                    UINT32_t *pressVal_p)
{
   PLTFRM_sts_t sts;
   MPL3115A2_cntxt_s *devCntxt_p = MPL3115A2_getDevCntxt(devId);
   UINT32_t pressureVal;
   UINT32_t tmpU32;

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   /*
    * The Pressure data is arranged as 20-bit unsigned data in Pascals. The
    * first 18 bits are located in OUT_P_MSB, OUT_P_CSB and bits 7-6 of OUT_P_LSB.
    * The 2 bits in position 5-4 of OUT_P_LSB represent the fractional component.
    */

   sts = MPL3115A2_readReg(devCntxt_p,
                           MPL3115A2_OUT_P_MSB_REG_ADDR,
                           &regValPressureMSB);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       return sts;
   }

   sts = MPL3115A2_readReg(devCntxt_p,
                           MPL3115A2_OUT_P_CSB_REG_ADDR,
                           &regValPressureCSB);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       return sts;
   }

   sts = MPL3115A2_readReg(devCntxt_p,
                           MPL3115A2_OUT_P_LSB_REG_ADDR,
                           &regValPressureLSB);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       return sts;
   }

   tmpU32 = regValPressureMSB;
   tmpU32 = (tmpU32 << 10);
   pressureVal = tmpU32;

   tmpU32 = regValPressureCSB;
   tmpU32 = (tmpU32 << 2);
   pressureVal |= tmpU32;

   tmpU32 = regValPressureLSB;
   tmpU32 = (tmpU32 >> 6) & 0x3;
   pressureVal |= tmpU32;

   *pressVal_p = pressureVal;

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
PLTFRM_sts_t MPL3115A2_readTemp(UINT8_t devId,
                                SINT16_t *tmoVal_p)
{
   PLTFRM_sts_t sts;
   MPL3115A2_cntxt_s *devCntxt_p = MPL3115A2_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   /*
    * The Temperature data is arranged as 12-bit 2’s complement value in
    * degrees C. The 8 bits of OUT_T_MSB representing degrees and with
    * fractions of a degree are stored in 4 bits in position 7-4 of OUT_T_LSB.
    * Be aware that the fractional bits are not signed, therefore, they are
    * not represented in 2’s complement. When RAW is selected then the RAW
    * value is stored in all 16 bits of OUT_T_MSB and OUT_T_LSB.
    */

   sts = MPL3115A2_readReg(devCntxt_p,
                           MPL3115A2_OUT_T_MSB_REG_ADDR,
                           &regValTmpMSB);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       return sts;
   }

   sts = MPL3115A2_readReg(devCntxt_p,
                           MPL3115A2_OUT_T_LSB_REG_ADDR,
                           &regValTmpLSB);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       return sts;
   }

   *(tmoVal_p) = (SINT8_t)regValTmpMSB;

   return PLTFRM_STS_SUCCESS;
}


UINT8_t idVal = 0xa5;

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t MPL3115A2_init(UINT8_t devId)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   MPL3115A2_cntxt_s *devCntxt_p = MPL3115A2_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   sts = PLTFRM_allocGPIOPin(PLTFRM_GENERIC_DEV_ID,
                             PLTFRM_GPIO_PORT_2,
                             PLTFRM_GPIO_PORT_PIN_4,
                             PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                             PLTFRM_GPIO_OUTPUT_HIGH,
                             PLTFRM_GPIO_INT_EDGE_NA,
                             NULL);

   if (sts != PLTFRM_STS_SUCCESS)
   {
       // Port and bit number combination has already been
       // reserved by another device !!
       SYS_fatal(SYS_FATAL_ERR_238);
   }

   sts = PLTFRM_allocGPIOPin(PLTFRM_GENERIC_DEV_ID,
                             PLTFRM_GPIO_PORT_2,
                             PLTFRM_GPIO_PORT_PIN_5,
                             PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                             PLTFRM_GPIO_OUTPUT_HIGH,
                             PLTFRM_GPIO_INT_EDGE_NA,
                             NULL);

   if (sts != PLTFRM_STS_SUCCESS)
   {
       // Port and bit number combination has already been
       // reserved by another device !!
       SYS_fatal(SYS_FATAL_ERR_239);
   }

   sts = MPL3115A2_readReg(devCntxt_p,
                           MPL3115A2_DEV_ID_REG_ADDR,
                           &idVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       if (idVal != MPL3115A2_DEV_ID)
       {
           return PLTFRM_STS_DEV_PART_NR_MISMATCH;
       }
   }

   sts = MPL3115A2_readReg(devCntxt_p,
                           MPL3115A2_SYSMOD_REG_ADDR,
	                       &sysModRegVal);
   if (sts != PLTFRM_STS_SUCCESS)
   {
	   return sts;
   }

   if (sysModRegVal & MPL3115A2_SYSMOD_REG_OPN_MODE_BIT)
       return PLTFRM_STS_DEV_INV_OPN_MODE;

   sts = MPL3115A2_readReg(devCntxt_p,
                           MPL3115A2_CTRL1_REG_ADDR,
                           &ctrl1RegVal);
   if (sts != PLTFRM_STS_SUCCESS)
   {
	   return sts;
   }

   ctrl1RegVal |= MPL3115A2_CTRL_REG1_RAW_BIT;
   ctrl1RegVal |= MPL3115A2_CTRL_REG1_OS2_BIT;
   ctrl1RegVal |= MPL3115A2_CTRL_REG1_OS1_BIT;
   ctrl1RegVal |= MPL3115A2_CTRL_REG1_OS0_BIT;

   sts = MPL3115A2_writeReg(devCntxt_p,
                            MPL3115A2_CTRL1_REG_ADDR,
                            ctrl1RegVal);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       return sts;
   }

   sts = MPL3115A2_readReg(devCntxt_p,
                           MPL3115A2_CTRL1_REG_ADDR,
                           &ctrl1RegVal);
   if (sts != PLTFRM_STS_SUCCESS)
   {
	   return sts;
   }

   if ((ctrl1RegVal & MPL3115A2_CTRL_REG1_RAW_BIT) == 0x0)
       return PLTFRM_STS_DEV_REG_UPDATE_ERR;

   devCntxt_p->opnMode = MPL3115A2_OPN_MODE_STANDBY;

#ifdef MPL3115A2_UT_ENA
   sts = MPL3115A2_readReg(devCntxt_p,
                           MPL3115A2_BAR_IN_MSB_REG_ADDR,
                           &regValBarInMSB);
   if (sts != PLTFRM_STS_SUCCESS)
   {
	   return sts;
   }

   sts = MPL3115A2_readReg(devCntxt_p,
                           MPL3115A2_BAR_IN_LSB_REG_ADDR,
                           &regValBarInLSB);
   if (sts != PLTFRM_STS_SUCCESS)
   {
	   return sts;
   }

   sts = MPL3115A2_readReg(devCntxt_p,
                           MPL3115A2_SYSMOD_REG_ADDR,
	                       &sysModRegVal);
   if (sts != PLTFRM_STS_SUCCESS)
   {
	   return sts;
   }

   if (sysModRegVal & MPL3115A2_SYSMOD_REG_OPN_MODE_BIT)
       return PLTFRM_STS_DEV_INV_OPN_MODE;

   for (;;)
   {
       PLTFRM_TOGGLE_LED_1();

       for (;;)
       {
	       sts = MPL3115A2_readReg(devCntxt_p,
                                   MPL3115A2_CTRL1_REG_ADDR,
	                               &ctrl1RegVal);
           if (sts != PLTFRM_STS_SUCCESS)
           {
	           return sts;
           }

           if (ctrl1RegVal & MPL3115A2_CTRL_REG1_OST_BIT)
           {
               PLTFRM_TOGGLE_LED_2();
               delayCnt ++;
           }
           else
           {
               if (validTmpCnt > 0)
               {

               }

               PLTFRM_delay1MilliSec();

               ctrl1RegVal |= MPL3115A2_CTRL_REG1_OST_BIT;

	           sts = MPL3115A2_writeReg(devCntxt_p,
                                        MPL3115A2_CTRL1_REG_ADDR,
	                                    ctrl1RegVal);
               if (sts != PLTFRM_STS_SUCCESS)
               {
	               return sts;
               }

               validTmpCnt ++;

               break;
           }
       }

       loopCnt ++;
       delayCnt = 0;
   }
#endif

   return sts;
}


#endif
