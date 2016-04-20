/*
 * File Name: tsl45315.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Nov/10/2013
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


#ifdef PLTFRM_TSL45315_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <tsl45315.h>

TSL45315_cntxt_s TSL45315_cntxtList[PLTFRM_TSL45315_DEV_CNT] =
{
   {PLTFRM_TSL45315_1_DEV_ID, TSL45315_OPN_MODE_UNKNOWN}
};


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
TSL45315_cntxt_s *TSL45315_getDevCntxt(UINT8_t devId)
{
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_TSL45315_DEV_CNT; idx++)
   {
      if (TSL45315_cntxtList[idx].devId == devId)
          return (&(TSL45315_cntxtList[idx]));
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
PLTFRM_sts_t TSL45315_writeReg(TSL45315_cntxt_s *devCntxt_p,
                               UINT8_t regAddr,
                               UINT8_t wrByte)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t busId, i2cAddr = TSL45315_I2C_ADDR_PREFIX;

   /*
    * Write register (1-byte data)
    *
    * > START                               (from micro)
    * > 7 bit device address | Write bit    (from micro)
    * > ACK                                 (from TSL45315)
    * > 8 bit register address              (from micro)
    * > ACK                                 (from TSL45315)
    * > Write Byte                          (from micro)
    * > ACK                                 (from TSL45315)
    * > STOP                                (from micro)
    */

   switch (devCntxt_p->devId)
   {
      case PLTFRM_TSL45315_1_DEV_ID:
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           i2cAddr |= PLTFRM_TSL45315_1_I2C_ADDR;
           break;

      default:
           sts = PLTFRM_STS_INV_DEV_ID;
           break;
   }

   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   regAddr |= TSL45315_COMMAND_FMT_BM;

   I2C_SW_start(busId);

   if (I2C_SW_sendDevAddr_7(busId, i2cAddr, I2C_OPN_TYPE_WRITE) == 0x0)
       return PLTFRM_STS_I2C_BUS_WR_ERROR;

   if (I2C_SW_wrByte(busId, regAddr) == 0x0)
       return PLTFRM_STS_I2C_BUS_WR_ERROR;

   if (I2C_SW_wrByte(busId, wrByte) == 0x0)
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
PLTFRM_sts_t TSL45315_readReg(TSL45315_cntxt_s *devCntxt_p,
                              UINT8_t regAddr,
                              UINT8_t *rdBuff_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t busId, i2cAddr = TSL45315_I2C_ADDR_PREFIX;

   /*
    * Read configuration register including pointer byte (1-byte data)
    *
    * > START                               (from micro)
    * > 7 bit device address | Write bit    (from micro)
    * > ACK                                 (from sensor)
    * > 8 bit register address              (from micro)
    * > ACK                                 (from sensor)
    * > RE-START                            (from micro)
    * > 7 bit device address | Read bit     (from micro)
    * > Read Byte                           (from sensor)
    * > No Ack bit (SDA remains high)
    * > STOP                                (from micro)
    */


   switch (devCntxt_p->devId)
   {
      case PLTFRM_TSL45315_1_DEV_ID:
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           i2cAddr |= PLTFRM_TSL45315_1_I2C_ADDR;
           break;

      default:
           sts = PLTFRM_STS_INV_DEV_ID;
           break;
   }

   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   regAddr |= TSL45315_COMMAND_FMT_BM;

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
 ************************************************************
 *
 *
 *
 ************************************************************
 */
PLTFRM_sts_t TSL45315_getALSOutput(UINT8_t devId,
                                   SINT32_t *luxVal_p)
{
   TSL45315_cntxt_s *devCntxt_p;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t regVal;

   devCntxt_p = TSL45315_getDevCntxt(devId);
   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   /*
    * The ADC data is expressed as a 16-bit word stored in two 8-bit registers.
    * The read-only ADC data registers DATALOW and DATAHIGH provide the low and
    * high bytes, respectively, of the 16-bit ADC conversion value. The conversion
    * value translates directly to units of lux.
    */

   sts = TSL45315_readReg(devCntxt_p, TSL45315_DATA_LOW_REG_ADDR, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       *luxVal_p = regVal;

       sts = TSL45315_readReg(devCntxt_p, TSL45315_DATA_HIGH_REG_ADDR, &regVal);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           UINT16_t tempU16;

           tempU16 = regVal;
           tempU16 = (tempU16 << 8) & 0xff00;
           (*luxVal_p) |= tempU16;

           /*
            * The ADC output is a 16-bit number that is directly proportional to the
            * value that approximates the human eye response in the commonly used
            * illuminance unit of lux. The light level can be calculated using the
            * following expression.
            *
            *    Light Level (lux) = MULTIPLIER × ADC_VALUE
            *    where
            *     MULTIPLIER = 1 for TCNTRL = 00 (Tint = 400 ms)
            *     ULTMIPLIER = 2 for TCNTRL = 01 (Tint = 200 ms)
            *     MULTIPLIER = 4 for TCNTRL = 10 (Tint = 100 ms)
            */

           (*luxVal_p) <<= devCntxt_p->tcntrlEnc;
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
PLTFRM_sts_t TSL45315_triggerALSMeas(TSL45315_cntxt_s *devCntxt_p)
{
   UINT8_t regVal;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   sts = TSL45315_readReg(devCntxt_p, TSL45315_CONFIG_REG_ADDR, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       if (devCntxt_p->tcntrlEnc != (regVal & TSL45315_CONFIG_REG_TCNTRL_BIT_MSK))
       {
           regVal &= ~(TSL45315_CONFIG_REG_TCNTRL_BIT_MSK);
           regVal |= devCntxt_p->tcntrlEnc;
           sts = TSL45315_writeReg(devCntxt_p, TSL45315_CONFIG_REG_ADDR, regVal);
           if (sts == PLTFRM_STS_SUCCESS)
           {
               UINT8_t regVal1;
               sts = TSL45315_readReg(devCntxt_p, TSL45315_CONFIG_REG_ADDR, &regVal1);
               regVal1 &= TSL45315_CONFIG_REG_TCNTRL_BIT_MSK;
               if (regVal1 != devCntxt_p->tcntrlEnc)
                   return PLTFRM_STS_DEV_REG_UPDATE_ERR;
           }
       }
   }

   regVal = TSL45315_SINGLE_CYCLE_MODE_ENC;
   sts = TSL45315_writeReg(devCntxt_p, TSL45315_CONTROL_REG_ADDR, regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       UINT8_t regVal1;
       sts = TSL45315_readReg(devCntxt_p, TSL45315_CONTROL_REG_ADDR, &regVal1);
       if (sts == PLTFRM_STS_SUCCESS)
       {
    	   regVal1 &= TSL45315_CONTROL_REG_MODE_BIT_MSK;
           if (regVal1 != regVal)
               sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;
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
PLTFRM_sts_t TSL45315_startALSConv(UINT8_t devId,
                                   SINT32_t *luxVal_p,
                                   UINT16_t *tmoVal_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   TSL45315_cntxt_s *devCntxt_p;

   devCntxt_p = TSL45315_getDevCntxt(devId);
   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   switch (devCntxt_p->opnMode)
   {
      case TSL45315_OPN_MODE_NORMAL:
           {
              // Free running at a rate which is configurable.
              sts = TSL45315_getALSOutput(devId, luxVal_p);
           }
           break;

      case TSL45315_OPN_MODE_POWER_DOWN:
           {
              // Trigger single cycle through the control register
              sts = TSL45315_triggerALSMeas(devCntxt_p);
              if (sts == PLTFRM_STS_SUCCESS)
              {
                  if (devCntxt_p->tcntrlEnc == TSL45315_400_MS_INTEGRATION_TIME_ENC)
                      *tmoVal_p = 400;
                  else
                  {
                      if (devCntxt_p->tcntrlEnc == TSL45315_200_MS_INTEGRATION_TIME_ENC)
                          *tmoVal_p = 200;
                      else
                          *tmoVal_p = 100;
                  }
                  sts = PLTFRM_STS_OPN_IN_PROGRESS;
              }
           }
           break;

      default:
           sts = PLTFRM_STS_DEV_NOT_READY;
           break;
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
PLTFRM_sts_t TSL45315_cfgALSOpnMode(UINT8_t devId,
                                    TSL45315_mode_t reqOpnMode,
                                    UINT16_t *opnTime_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   TSL45315_cntxt_s *devCntxt_p;
   UINT8_t newModeEnc = 0, regVal;

   if (reqOpnMode != TSL45315_OPN_MODE_POWER_DOWN
       && reqOpnMode != TSL45315_OPN_MODE_NORMAL)
   {
       return PLTFRM_STS_INV_PARAM;
   }

   devCntxt_p = TSL45315_getDevCntxt(devId);
   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   if (reqOpnMode != devCntxt_p->opnMode)
   {
       if (reqOpnMode == TSL45315_OPN_MODE_POWER_DOWN)
           newModeEnc = TSL45315_PWR_DOWN_MODE_ENC;
       else
           newModeEnc = TSL45315_NORMAL_MODE_ENC;

       regVal = newModeEnc;

       sts = TSL45315_writeReg(devCntxt_p, TSL45315_CONTROL_REG_ADDR, regVal);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           UINT8_t regVal1;
           sts = TSL45315_readReg(devCntxt_p, TSL45315_CONTROL_REG_ADDR, &regVal1);
           regVal1 &= TSL45315_CONTROL_REG_MODE_BIT_MSK;
           if (regVal1 != regVal)
               sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;
       }

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
PLTFRM_sts_t TSL45315_checkPresence(TSL45315_cntxt_s *devCntxt_p)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;

   sts = TSL45315_readReg(devCntxt_p,
                          TSL45315_ID_REG_ADDR,
                          &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       regVal = (regVal >> TSL45315_PART_NR_SHIFT) & TSL45315_PART_NR_SHIFT_BIT_MSK;
       if (regVal == TSL45315_PART_NUMBER_ID)
           sts = PLTFRM_STS_SUCCESS;
       else
           sts = PLTFRM_STS_DEV_PART_NR_MISMATCH;
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
PLTFRM_sts_t TSL45315_init(UINT8_t devId)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   TSL45315_cntxt_s *devCntxt_p = TSL45315_getDevCntxt(devId);
   UINT8_t i2cAddr;

   devCntxt_p->opnMode = TSL45315_OPN_MODE_UNKNOWN;
   devCntxt_p->tcntrlEnc = TSL45315_100_MS_INTEGRATION_TIME_ENC;

   if (devCntxt_p == NULL)
   {
       return PLTFRM_STS_INV_DEV_ID;
   }

   switch (devId)
   {
       case PLTFRM_TSL45315_1_DEV_ID:
            i2cAddr = TSL45315_I2C_ADDR_PREFIX | PLTFRM_TSL45315_1_I2C_ADDR;
            break;

       default:
            SYS_fatal(SYS_FATAL_ERR_532);
            break;
   }

   sts = PLTFRM_regI2CDev(devId, PLTFRM_I2C_SW_BUS_1_ID, i2cAddr);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = TSL45315_checkPresence(devCntxt_p);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       UINT8_t regVal;
       sts = TSL45315_readReg(devCntxt_p, TSL45315_CONTROL_REG_ADDR, &regVal);
       regVal &= TSL45315_CONTROL_REG_MODE_BIT_MSK;
       switch (regVal)
       {
          case TSL45315_PWR_DOWN_MODE_ENC:
               devCntxt_p->opnMode = TSL45315_OPN_MODE_POWER_DOWN;
               break;

          case TSL45315_NORMAL_MODE_ENC:
               devCntxt_p->opnMode = TSL45315_OPN_MODE_NORMAL;
               break;

          case TSL45315_SINGLE_CYCLE_MODE_ENC:
               // This is a transient state. After the integration time, the device goes
               // back to power down mode.
          default:
               sts = PLTFRM_STS_UNKNOWN_DEV_OPN_MODE;
               break;
       }

       if (devCntxt_p->opnMode == TSL45315_OPN_MODE_NORMAL)
       {
           regVal = TSL45315_PWR_DOWN_MODE_ENC;
           sts = TSL45315_writeReg(devCntxt_p, TSL45315_CONTROL_REG_ADDR, regVal);
           if (sts == PLTFRM_STS_SUCCESS)
           {
               UINT8_t regVal1;
               sts = TSL45315_readReg(devCntxt_p, TSL45315_CONTROL_REG_ADDR, &regVal1);
               regVal1 &= TSL45315_CONTROL_REG_MODE_BIT_MSK;
               if (regVal1 != regVal)
                   sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;
           }
       }
   }

   return sts;
}
#endif
