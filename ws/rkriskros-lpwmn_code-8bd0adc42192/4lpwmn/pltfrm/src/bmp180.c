/*
 * File Name: bmp180.c
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

#define BMP180_UT_ENA

#ifdef PLTFRM_BMP180_SENSOR_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <i2c_sw.h>
#include <BMP180.h>

/*
 * The BMP180 consists of a piezo-resistive sensor, an analog to digital converter
 * and a control unit with E2PROM and a serial I2C interface. The BMP180 delivers
 * the uncompensated value of pressure and temperature. The E2PROM has stored 176
 * bit of individual calibration data. This is used to compensate offset, temperature
 * dependence and other parameters of the sensor.
 *   > UP = pressure data (16 to 19 bit)
 *   > UT = temperature data (16 bit)
 */

BMP180_cntxt_s BMP180_cntxtList[PLTFRM_BMP180_DEV_CNT] =
{
   {PLTFRM_BMP180_1_DEV_ID, PLTFRM_I2C_SW_BUS_1_ID}
};


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
BMP180_cntxt_s *BMP180_getDevCntxt(UINT8_t devId)
{
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_BMP180_DEV_CNT; idx++)
   {
      if (BMP180_cntxtList[idx].devId == devId)
          return (&(BMP180_cntxtList[idx]));
   }

   return NULL;
}


/*
 * 3.4 Calibration coefficients
 * The 176 bit E2PROM is partitioned in 11 words of 16 bit each. These contain 11
 * calibration coefficients. Every sensor module has individual coefficients.
 * > AC1 (short)
 * > AC2 (short)
 * > AC3 (short)
 * > AC4 (unsigned short)
 * > AC5 (unsigned short)
 * > AC6 (unsigned short)
 * > B1 (short)
 * > B2 (short)
 * > MB (short)
 * > MC (short)
 * > MD (short)
 */


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t BMP180_read8BitReg(BMP180_cntxt_s *devCntxt_p,
                                UINT8_t regAddr,
                                UINT8_t *regVal_p)
{
   const UINT8_t i2cAddr = BMP180_I2C_ADDR_PREFIX;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t busId = devCntxt_p->busId;

   /*
    * <START>
    * <Dev Addr | 0x0>
    * <Reg Addr>
    * <RESTART>
    * <Dev Addr | 0x01>
    * <BYTE from BMP180>
    * NACK from micro
    * <STOP>
    */

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

   I2C_SW_start(busId);

   if (I2C_SW_sendDevAddr_7(busId, i2cAddr, I2C_OPN_TYPE_READ) == 0x0)
   {
       I2C_SW_stop(busId);
       return PLTFRM_STS_I2C_BUS_WR_ERROR;
   }

   I2C_SW_rdByte(busId, regVal_p, I2C_NO_ACK);

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
PLTFRM_sts_t BMP180_read16BitReg(BMP180_cntxt_s *devCntxt_p,
                                 UINT8_t regAddr,
                                 SINT32_t *regVal_p)
{
   const UINT8_t i2cAddr = BMP180_I2C_ADDR_PREFIX;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t msb, lsb, busId = devCntxt_p->busId;
   UINT16_t u16;

   /*
    * <START>
    * <Dev Addr | 0x0>
    * <Reg Addr>
    * <RESTART>
    * <Dev Addr | 0x01>
    * <MSB from BMP180>
    * ACK from micro
    * <LSB from BMP180>
    * NACK from micro
    * <STOP>
    */

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

   I2C_SW_start(busId);

   if (I2C_SW_sendDevAddr_7(busId, i2cAddr, I2C_OPN_TYPE_READ) == 0x0)
   {
       I2C_SW_stop(busId);
       return PLTFRM_STS_I2C_BUS_WR_ERROR;
   }

   I2C_SW_rdByte(busId, &msb, I2C_ACK);

   I2C_SW_rdByte(busId, &lsb, I2C_NO_ACK);

   I2C_SW_stop(busId);

   u16 = msb;
   u16 <<= 8;
   u16 |= lsb;

   if (regAddr < BMP180_AC4_MSB_REG_ADDR
       || regAddr > BMP180_AC6_LSB_REG_ADDR)
       *regVal_p = (SINT16_t)u16;
   else
       *regVal_p = (UINT16_t)u16;

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
PLTFRM_sts_t BMP180_startTempConv(BMP180_cntxt_s *devCntxt_p,
                                  UINT16_t tmoMilliSec)
{
   I2C_SW_start(busId);

   if (I2C_SW_sendDevAddr_7(busId, i2cAddr, I2C_OPN_TYPE_WRITE) == 0x0)
   {
       I2C_SW_stop(busId);
       return PLTFRM_STS_I2C_BUS_WR_ERROR;
   }

   if (I2C_SW_wrByte(busId, BMP180_CTRL_MEAS_REG_ADDR) == 0x0)
   {
       I2C_SW_stop(busId);
       return PLTFRM_STS_I2C_BUS_WR_ERROR;
   }

    if (I2C_SW_wrByte(busId, BMP180_START_TEMP_CONV_CODE) == 0x0)
   {
       I2C_SW_stop(busId);
       return PLTFRM_STS_I2C_BUS_WR_ERROR;
   }

}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t BMP180_init(UINT8_t devId)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   BMP180_cntxt_s *devCntxt_p = BMP180_getDevCntxt(devId);
   UINT8_t regVal;

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   sts = BMP180_read8BitReg(devCntxt_p, BMP180_ID_REG_ADDR, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   if (regVal != BMP180_CHIP_ID)
       return PLTFRM_STS_DEV_PART_NR_MISMATCH;

   sts = BMP180_read16BitReg(devCntxt_p,
                             BMP180_AC1_MSB_REG_ADDR,
                             &(devCntxt_p->ac1));
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = BMP180_read16BitReg(devCntxt_p,
                             BMP180_AC2_MSB_REG_ADDR,
                             &(devCntxt_p->ac2));
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = BMP180_read16BitReg(devCntxt_p,
                             BMP180_AC3_MSB_REG_ADDR,
                             &(devCntxt_p->ac3));
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = BMP180_read16BitReg(devCntxt_p,
                             BMP180_AC4_MSB_REG_ADDR,
                             &(devCntxt_p->ac4));
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = BMP180_read16BitReg(devCntxt_p,
                             BMP180_AC5_MSB_REG_ADDR,
                             &(devCntxt_p->ac5));
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = BMP180_read16BitReg(devCntxt_p,
                             BMP180_AC6_MSB_REG_ADDR,
                             &(devCntxt_p->ac6));
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = BMP180_read16BitReg(devCntxt_p,
                             BMP180_B1_MSB_REG_ADDR,
                             &(devCntxt_p->b1));
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = BMP180_read16BitReg(devCntxt_p,
                             BMP180_B2_MSB_REG_ADDR,
                             &(devCntxt_p->b2));
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = BMP180_read16BitReg(devCntxt_p,
                             BMP180_MB_MSB_REG_ADDR,
                             &(devCntxt_p->mb));
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = BMP180_read16BitReg(devCntxt_p,
                             BMP180_MC_MSB_REG_ADDR,
                             &(devCntxt_p->mc));
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = BMP180_read16BitReg(devCntxt_p,
                             BMP180_MD_MSB_REG_ADDR,
                             &(devCntxt_p->md));
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

#ifdef BMP180_UT_ENA
   for (;;)
   {
      // Read uncompensated temperature value
      sts = BMP180_startTempConv(devCntxt_p, &tmoVal);
      if (sts != PLTFRM_STS_SUCCESS)
          SYS_fatal(SYS_FATAL_ERR_218);

      // Read uncompensated pressure value
      sts = BMP180_startPressureConv(devCntxt_p, &tmoVal);
      if (sts != PLTFRM_STS_SUCCESS)
          SYS_fatal(SYS_FATAL_ERR_219);

      // Calculate true temperature

      // Calculate true pressure
   }
#endif

   return sts;
}

#endif
