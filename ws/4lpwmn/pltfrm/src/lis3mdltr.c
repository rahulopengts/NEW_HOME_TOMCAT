/*
 * File Name: lism3dltr.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Jan/10/2015
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

#ifdef PLTFRM_LIS3MDLTR_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <i2c_sw.h>
#include <lis3mdltr.h>

/*
 * The LIS3MDL is an ultra low-power high performance three axis magnetic
 * sensor. The LIS3MDL has user selectable full-scales of ±4/ ±8/ ±12/ ±16
 * gauss. The device may be configured to generate interrupt signals for
 * magnetic field detection
 */

static const char _snsrName[] = "LIS3MDLTR";
static const char _snsrMfr[] = "ST";

LIS3MDLTR_cntxt_s LIS3MDLTR_cntxtList[PLTFRM_LIS3MDLTR_DEV_CNT] =
{
   {PLTFRM_LIS3MDLTR_1_DEV_ID, {_snsrName, _snsrMfr}},
};

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
LIS3MDLTR_cntxt_s *LIS3MDLTR_getDevCntxt(UINT8_t devId)
{
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_LIS3MDLTR_DEV_CNT; idx++)
   {
      if (LIS3MDLTR_cntxtList[idx].devId == devId)
          return (&(LIS3MDLTR_cntxtList[idx]));
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
PLTFRM_sts_t LIS3MDLTR_readReg(LIS3MDLTR_cntxt_s *devCntxt_p,
                               UINT8_t regAddr,
                               UINT8_t *rdBuff_p)
{
   UINT8_t busId, i2cAddr;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   /*
    * Table 14: Transfer when master is receiving (reading) one
    *           byte of data from slave
    *
    * > START                               (from micro)
    * > 7 bit device address | Write bit    (from micro)
    * > ACK                                 (from LIS3MDLTR)
    * > 8 bit register address              (from micro)
    * > ACK                                 (from LIS3MDLTR)
    * > RE-START                            (from micro)
    * > 7 bit device address | Read bit     (from micro)
    * > ACK                                 (from LIS3MDLTR)
    * > Read Byte                           (from LIS3MDLTR)
    * > No Ack bit (SDA remains high)
    * > STOP                                (from micro)
    */

   switch (devCntxt_p->devId)
   {
      case PLTFRM_LIS3MDLTR_1_DEV_ID:
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           i2cAddr = PLTFRM_LIS3MDLTR_1_I2C_ADDR;
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

   I2C_SW_start(busId);

   if (I2C_SW_sendDevAddr_7(busId, i2cAddr, I2C_OPN_TYPE_READ) == 0x0)
   {
       I2C_SW_stop(busId);
       return PLTFRM_STS_I2C_BUS_WR_ERROR;
   }

   I2C_SW_rdByte(busId, rdBuff_p, I2C_NO_ACK);

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
PLTFRM_sts_t LIS3MDLTR_writeReg(LIS3MDLTR_cntxt_s *devCntxt_p,
                                UINT8_t regAddr,
                                UINT8_t val)
{
   UINT8_t busId, i2cAddr;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   /*
    * Table 14: Transfer when master is writing one byte to a slave
    *
    * > START                               (from micro)
    * > 7 bit device address | Write bit    (from micro)
    * > ACK                                 (from LIS3MDLTR)
    * > 8 bit register address              (from micro)
    * > ACK                                 (from LIS3MDLTR)
    * > Write data byte                     (from micro)
    * > Ack                                 (from LIS3MDLTR)
    * > STOP                                (from micro)
    */

   switch (devCntxt_p->devId)
   {
      case PLTFRM_LIS3MDLTR_1_DEV_ID:
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           i2cAddr = PLTFRM_LIS3MDLTR_1_I2C_ADDR;
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

   I2C_SW_wrByte(busId, val);

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
PLTFRM_sts_t LIS3MDLTR_readAxisOp(UINT8_t devId,
                                  LIS3MDLTR_axis_t axis,
                                  SINT16_t *opVal_p)
{
   LIS3MDLTR_cntxt_s *devCntxt_p = LIS3MDLTR_getDevCntxt(devId);
   UINT8_t lsbVal, msbVal, regAddr = LIS3MDLTR_OUT_X_L_REG_ADDR + (axis << 1);
   UINT16_t u16Val;
   PLTFRM_sts_t sts;

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   sts = LIS3MDLTR_readReg(devCntxt_p, regAddr, &lsbVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = LIS3MDLTR_readReg(devCntxt_p, regAddr + 1, &msbVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   u16Val = msbVal;
   u16Val = (u16Val << 8) | lsbVal;

   *opVal_p = (SINT16_t)u16Val;

   return sts;
}

#ifdef PLTFRM_LIS3MDLTR_TEST_ENA
UINT8_t LIS3MDLTR_stsReg_1, LIS3MDLTR_ctrlReg_3;
UINT32_t __loopCnt = 0;
SINT16_t LIS3MDLTR_xOp = 0, LIS3MDLTR_yOp = 0, LIS3MDLTR_zOp = 0;
#endif

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t LIS3MDLTR_init(UINT8_t devId)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   LIS3MDLTR_cntxt_s *devCntxt_p = LIS3MDLTR_getDevCntxt(devId);
   UINT8_t busId, i2cAddr, regVal;

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   switch (devId)
   {
      case PLTFRM_LIS3MDLTR_1_DEV_ID:
           i2cAddr = PLTFRM_LIS3MDLTR_1_I2C_ADDR;
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           break;

      default:
           SYS_fatal(SYS_FATAL_ERR_531);
           break;
   }

   sts = PLTFRM_regI2CDev(devId, busId, i2cAddr);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = LIS3MDLTR_readReg(devCntxt_p, LIS3MDLTR_WHO_AM_I_REG_ADDR, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   if (regVal != LIS3MDLTR_WHO_AM_I_REG_VALUE)
       return PLTFRM_STS_DEV_ID_REG_VAL_MISMATCH;

#ifdef PLTFRM_LIS3MDLTR_TEST_ENA
  for (;;)
  {
     sts = LIS3MDLTR_readReg(devCntxt_p, LIS3MDLTR_STATUS_REG1_ADDR, &LIS3MDLTR_stsReg_1);
     if (sts != PLTFRM_STS_SUCCESS)
         return sts;

     sts = LIS3MDLTR_readReg(devCntxt_p, LIS3MDLTR_CTRL_REG3_ADDR, &LIS3MDLTR_ctrlReg_3);
     if (sts != PLTFRM_STS_SUCCESS)
         return sts;

     LIS3MDLTR_ctrlReg_3 &= ~(LIS3MDLTR_CTRL_REG3_MDX_BIT_MSK);
     LIS3MDLTR_ctrlReg_3 |= LIS3MDLTR_SYS_OPN_MODE_SINGLE_CONV;

     sts = LIS3MDLTR_writeReg(devCntxt_p, LIS3MDLTR_CTRL_REG3_ADDR, LIS3MDLTR_ctrlReg_3);
     if (sts != PLTFRM_STS_SUCCESS)
         return sts;

     __loopCnt = 0;

     for (;;)
     {
        LIS3MDLTR_ctrlReg_3 = 0;
        sts = LIS3MDLTR_readReg(devCntxt_p, LIS3MDLTR_CTRL_REG3_ADDR, &LIS3MDLTR_ctrlReg_3);
        if (sts != PLTFRM_STS_SUCCESS)
            return sts;

        LIS3MDLTR_stsReg_1 = 0;
        sts = LIS3MDLTR_readReg(devCntxt_p, LIS3MDLTR_STATUS_REG1_ADDR, &LIS3MDLTR_stsReg_1);
        if (sts != PLTFRM_STS_SUCCESS)
            return sts;

        if (LIS3MDLTR_stsReg_1 & LIS3MDLTR_STATUS_1_REG_ZYXDA_BIT)
        {
            sts = LIS3MDLTR_readAxisOp(devId, LIS3MDLTR_X_AXIS, &LIS3MDLTR_xOp);
            if (sts != PLTFRM_STS_SUCCESS)
                break;

            sts = LIS3MDLTR_readAxisOp(devId, LIS3MDLTR_Y_AXIS, &LIS3MDLTR_yOp);
            if (sts != PLTFRM_STS_SUCCESS)
                break;

            sts = LIS3MDLTR_readAxisOp(devId, LIS3MDLTR_Z_AXIS, &LIS3MDLTR_zOp);
            if (sts != PLTFRM_STS_SUCCESS)
                break;

            __loopCnt ++;

            break;
        }
     }

     if (sts != PLTFRM_STS_SUCCESS)
         break;
   }
#endif

   return sts;
}

#endif
