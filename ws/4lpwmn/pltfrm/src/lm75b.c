/*
 * File Name: lm75b.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Oct/23/2013
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

#ifdef PLTFRM_LM75B_TEMP_SENSOR_ENA


#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <i2c_sw.h>
#include <lm75b.h>

static const char _snsrName[] = "LM75B";
static const char _snsrMfr[] = "NXP";

LM75B_cntxt_s LM75B_cntxtList[PLTFRM_LMP75B_DEV_CNT] =
{
   {PLTFRM_LM75B_1_DEV_ID, LM75B_OPN_MODE_UNKNOWN, {_snsrName, _snsrMfr}},
};

/*
 * Data sheet: http://www.nxp.com/documents/data_sheet/LM75B.pdf
 */

/*
 * Usage on an RFD:
 *
 * <<<<<<  On wake up >>>>>>>>
 *
 * sts = LM75B_startSingleConv(devId, &tempVal, &timeOut);
 * if (sts == PLTFRM_STS_OPN_IN_PROGRESS)
 * {
 *     start timer with timeOut returned by function
 * }
 *
 *
 * <<<<<< On timer expiry >>>>>>>>
 * void timerCbFunc( )
 * {
 *    sts =  LM75B_getTempVal(devId, &tempVal);
 *    if (sts == PLTFRM_STS_SUCCESS)
 *    {
 *        // temp returned in tempVal
 *    }
 *    LM75B_shutDown(devId);
 * }
 */



/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
LM75B_cntxt_s *LM75B_getDevCntxt(UINT8_t devId)
{
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_LMP75B_DEV_CNT; idx++)
   {
      if (LM75B_cntxtList[idx].devId == devId)
          return (&(LM75B_cntxtList[idx]));
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
PLTFRM_sts_t LM75B_writeReg(LM75B_cntxt_s *devCntxt_p,
                            UINT8_t regAddr,
                            UINT8_t *wrBuff_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t busId, i2cAddr = LM75B_I2C_ADDR_PREFIX, wrCnt;

   if (regAddr > LM75B_TOS_REG_ADDR)
       return PLTFRM_STS_INV_PARAM;

   if (regAddr == LM75B_CONF_REG_ADDR)
       wrCnt = 0x1;
   else
       wrCnt = 0x2;

   /*
    * Write configuration register (1-byte data)
    *
    * > START                               (from micro)
    * > 7 bit device address | Write bit    (from micro)
    * > ACK                                 (from LM75B)
    * > 8 bit register address              (from micro)
    * > ACK                                 (from LM75B)
    * > Write Byte                          (from micro)
    * > ACK                                 (from LM75B)
    * > STOP                                (from micro)
    *
    *
    * Write Tos or Thyst register (2-byte data)
    *
    * > START                               (from micro)
    * > 7 bit device address | Write bit    (from micro)
    * > ACK                                 (from LM75B)
    * > 8 bit register address              (from micro)
    * > ACK                                 (from LM75B)
    * > Write MSB                           (from micro)
    * > ACK                                 (from LM75B)
    * > Write LSB                           (from micro)
    * > ACK                                 (from LM75B)
    * > STOP                                (from micro)
    */

   switch (devCntxt_p->devId)
   {
      case PLTFRM_LM75B_1_DEV_ID:
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           i2cAddr |= PLTFRM_LM75B_1_I2C_ADDR;
           break;

      default:
           sts = PLTFRM_STS_INV_DEV_ID;
           break;
   }

   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   I2C_SW_start(busId);

   if (I2C_SW_sendDevAddr_7(busId, i2cAddr, I2C_OPN_TYPE_WRITE) == 0x0)
       return PLTFRM_STS_I2C_BUS_WR_ERROR;

   if (I2C_SW_wrByte(busId, regAddr) == 0x0)
       return PLTFRM_STS_I2C_BUS_WR_ERROR;

   if (wrCnt == 2)
   {
       if (I2C_SW_wrByte(busId, *wrBuff_p) == 0x0)
       {
           I2C_SW_stop(busId);
           return PLTFRM_STS_I2C_BUS_WR_ERROR;
       }
       wrBuff_p ++;
   }

   if (I2C_SW_wrByte(busId, *wrBuff_p) == 0x0)
       sts = PLTFRM_STS_I2C_BUS_WR_ERROR;

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
PLTFRM_sts_t LM75B_readReg(LM75B_cntxt_s *devCntxt_p,
                           UINT8_t regAddr,
                           UINT8_t *rdBuff_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t busId, i2cAddr = LM75B_I2C_ADDR_PREFIX, rdCnt;

   if (regAddr > LM75B_TOS_REG_ADDR)
      return PLTFRM_STS_INV_PARAM;

   if (regAddr == LM75B_CONF_REG_ADDR)
       rdCnt = 0x1;
   else
       rdCnt = 0x2;

   /*
    * Read configuration register including pointer byte (1-byte data)
    *
    * > START                               (from micro)
    * > 7 bit device address | Write bit    (from micro)
    * > ACK                                 (from LM75B)
    * > 8 bit register address              (from micro)
    * > ACK                                 (from LM75B)
    * > RE-START                            (from micro)
    * > 7 bit device address | Read bit     (from micro)
    * > Read Byte                           (from LM75B)
    * > No Ack bit (SDA remains high)
    * > STOP                                (from micro)
    */

   /*
    * Read Temp, Tos or Thyst register including pointer byte (2-byte data)
    *
    * > START                               (from micro)
    * > 7 bit device address | Write bit    (from micro)
    * > ACK                                 (from LM75B)
    * > 8 bit register address              (from micro)
    * > ACK                                 (from LM75B)
    * > RE-START                            (from micro)
    * > 7 bit device address | Read bit     (from micro)
    * > Read MSB                            (from LM75B)
    * > Ack bit                             (from micro)
    * > Read LSB                            (from LM75B)
    * > No Ack bit (SDA remains high)
    * > STOP                                (from micro)
    */

   switch (devCntxt_p->devId)
   {
      case PLTFRM_LM75B_1_DEV_ID:
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           i2cAddr |= PLTFRM_LM75B_1_I2C_ADDR;
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

   if (rdCnt == 2)
   {
       I2C_SW_rdByte(busId, rdBuff_p, I2C_ACK);
       rdBuff_p ++;
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
PLTFRM_sts_t LM75B_cfg(UINT8_t devId,
                       LM75B_mode_t reqOpnMode)
{
   UINT8_t regVal;
   PLTFRM_sts_t sts;
   LM75B_cntxt_s *devCntxt_p;

   devCntxt_p = LM75B_getDevCntxt(devId);
   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   /*
    * The Configuration register (Conf) is a write/read register and
    * contains an 8-bit non-complement data byte that is used to
    * configure the device for different operation conditions.
    */

   sts = LM75B_readReg(devCntxt_p, LM75B_CONF_REG_ADDR, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   if ((reqOpnMode == LM75B_OPN_MODE_NORMAL
       && ((regVal & LM75B_CFG_REG_OPN_MODE_BM) == 0x1))
       || (reqOpnMode == LM75B_OPN_MODE_SHUT_DOWN
           && ((regVal & LM75B_CFG_REG_OPN_MODE_BM) == 0x0)))
   {
       // Device is not currently in the requested mode !!
       regVal ^= LM75B_CFG_REG_OPN_MODE_BM;

       sts = LM75B_writeReg(devCntxt_p, LM75B_CONF_REG_ADDR, &regVal);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           UINT8_t regRdBack;
           sts = LM75B_readReg(devCntxt_p, LM75B_CONF_REG_ADDR, &regRdBack);
           if (sts == PLTFRM_STS_SUCCESS)
           {
               if (regRdBack != regVal)
                   sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;
               else
                   devCntxt_p->opnMode = reqOpnMode;
           }
       }
   }
   else
   {
       devCntxt_p->opnMode = reqOpnMode;
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
PLTFRM_sts_t LM75B_getTempVal(UINT8_t devId,
                              SINT16_t *tempVal_p)
{
   PLTFRM_sts_t sts;
   UINT8_t tempBuff[LM75B_TEMP_REG_SZ];
   SINT16_t tempVal;
   LM75B_cntxt_s *devCntxt_p;

   devCntxt_p = LM75B_getDevCntxt(devId);
   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   /*
    * The LM75B uses the on-chip band gap sensor to measure the device
    * temperature with the resolution of 0.125 deg C and stores the 11-
    * bit two�s complement digital data, resulted from 11-bit A-to-D
    * conversion, into the device "Temp" register. This "Temp" register can be
    * read at any time by a controller on the I2C-bus. Reading temperature
    * data does not affect the conversion in progress during the read operation.
    */

   /*
    * When reading register Temp, all 16 bits of the two data bytes (MSByte and LSByte)
    * are provided to the bus and must be all collected by the controller to complete the
    * bus operation. However, only the 11 most significant bits should be used, and the 5
    * least significant bits of the LSByte are zero and should be ignored. One of the ways
    * to calculate the temperature value in degree C from the 11-bit "Temp" data is:
    * 1. If the Temp data MSByte bit D10 = 0, then the temperature is positive and Temp value
    *    Temperature in degree C = +(Temp data) * 0.125
    * 2. If the Temp data MSByte bit D10 = 1, then the temperature is negative and
    *    Temperature in degree C = -(two�s complement of Temp data) * 0.125
    */

   /*
    * MSB contains the bits D10 to D3.
    * LSB contains the bits D2 to D0.
    */

   sts = LM75B_readReg(devCntxt_p, LM75B_TEMP_REG_ADDR, tempBuff);

   if (sts == PLTFRM_STS_SUCCESS)
   {
       SINT32_t tempS32;

       tempVal = tempBuff[0];
       tempVal = (tempVal << 3) & 0x7f8;
       tempBuff[1] >>= 5;
       tempVal |= (tempBuff[1] & 0x7);

       if (tempBuff[0] & 0x80)
           tempVal |= 0xf800;

       tempS32 = tempVal;

       // Calc (tempVal * 100)/8

       tempS32 *= 25;
       tempS32 /= 2;

       *tempVal_p = (SINT16_t)tempS32;  // Scale is centi
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
PLTFRM_sts_t LM75B_shutDown(UINT8_t devId)
{
   return LM75B_cfg(devId, LM75B_OPN_MODE_SHUT_DOWN);
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t LM75B_startSingleConv(UINT8_t devId,
                                   SINT16_t *tempVal_p,
                                   UINT16_t *tmoVal_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   LM75B_cntxt_s *devCntxt_p;

   devCntxt_p = LM75B_getDevCntxt(devId);
   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   // Get the current mode of operation
   if (devCntxt_p->opnMode == LM75B_OPN_MODE_NORMAL)
   {
       /*
        * In normal operation mode, the temp-to-digital conversion is executed
        * every 100 msecs and the "Temp" register is updated at the end of each
        * conversion.
        */

       sts = LM75B_getTempVal(devId, tempVal_p);
   }
   else
   {
       /*
        * In shutdown mode, the device becomes idle, data conversion is disabled
        * and the Temp register holds the latest result; however, the device I2C-bus
        * interface is still active and register write/read operation can be performed.
        * The device operation mode is controllable by programming bit B0 of the config
        * register. The temperature conversion is initiated when the device is powered
        * -up or put back into normal mode from shutdown. The temperature conversion is
        * initiated when the device is powered-up or put back into normal mode from
        * shutdown.
        */

       if (tmoVal_p == NULL)
           sts = PLTFRM_STS_INV_PARAM;
       else
       {        
           sts = LM75B_cfg(devId, LM75B_OPN_MODE_NORMAL);
           if (sts != PLTFRM_STS_SUCCESS)
               return sts;
           else
           {
               *tmoVal_p = LM75B_TEMP_SENSING_TIME * 2;
               sts = PLTFRM_STS_OPN_IN_PROGRESS;
           }
       }
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
PLTFRM_sts_t LM75B_init(UINT8_t devId)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   LM75B_cntxt_s *devCntxt_p = LM75B_getDevCntxt(devId);
   UINT8_t i2cAddr, busId;

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   devCntxt_p->opnMode = LM75B_OPN_MODE_UNKNOWN;

   switch (devId)
   {
      case PLTFRM_LM75B_1_DEV_ID:
           i2cAddr = LM75B_I2C_ADDR_PREFIX | PLTFRM_LM75B_1_I2C_ADDR;
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           break;

      default:
           SYS_fatal(SYS_FATAL_ERR_531);
           break;
   }

   sts = PLTFRM_regI2CDev(devId, busId, i2cAddr);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = LM75B_cfg(devId, LM75B_OPN_MODE_SHUT_DOWN);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       devCntxt_p->opnMode = LM75B_OPN_MODE_SHUT_DOWN;
       sts = PLTFRM_regSnsrDev(PLTFRM_LM75B_1_DEV_ID, &devCntxt_p->info);
   }

   return sts;
}

#endif
