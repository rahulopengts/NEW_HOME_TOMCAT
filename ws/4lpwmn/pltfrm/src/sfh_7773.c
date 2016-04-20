/*
 * File Name: sfh_7773.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Oct/31/2013
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


#ifdef PLTFRM_SFH_7773_SENSOR_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <i2c_sw.h>
#include <sfh_7773.h>


SFH_7773_cntxt_s SFH_7773_cntxtList[PLTFRM_SFH_7773_DEV_CNT] =
{
   {PLTFRM_SFH_7773_1_DEV_ID, SFH_7773_OPN_MODE_UNKNOWN}
};


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
SFH_7773_cntxt_s *SFH_7773_getDevCntxt(UINT8_t devId)
{
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_SFH_7773_DEV_CNT; idx++)
   {
      if (SFH_7773_cntxtList[idx].devId == devId)
          return (&(SFH_7773_cntxtList[idx]));
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
PLTFRM_sts_t SFH_7773_writeReg(SFH_7773_cntxt_s *devCntxt_p,
                               UINT8_t regAddr,
                               UINT8_t wrByte)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t busId, i2cAddr = SFH_7773_I2C_ADDR_PREFIX;

   if (regAddr < SFH_7773_SW_RST_CTRL_REG_ADDR
       || regAddr > SFH_773_ALS_LOWER_THLD_MSB_REG_ADDR)
   {
       return PLTFRM_STS_INV_PARAM;
   }

   /*
    * Write register (1-byte data)
    *
    * > START                               (from micro)
    * > 7 bit device address | Write bit    (from micro)
    * > ACK                                 (from SFH_7773)
    * > 8 bit register address              (from micro)
    * > ACK                                 (from SFH_7773)
    * > Write Byte                          (from micro)
    * > ACK                                 (from SFH_7773)
    * > STOP                                (from micro)
    */

   switch (devCntxt_p->devId)
   {
      case PLTFRM_SFH_7773_1_DEV_ID:
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           i2cAddr |= PLTFRM_SFH_7773_1_I2C_ADDR;
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
PLTFRM_sts_t SFH_7773_readReg(SFH_7773_cntxt_s *devCntxt_p,
                              UINT8_t regAddr,
                              UINT8_t *rdBuff_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t busId, i2cAddr = SFH_7773_I2C_ADDR_PREFIX;

   if (regAddr < SFH_7773_SW_RST_CTRL_REG_ADDR
       || regAddr > SFH_773_ALS_LOWER_THLD_MSB_REG_ADDR)
   {
       return PLTFRM_STS_INV_PARAM;
   }

   /*
    * Read 8 bits register
    * > START                               (from micro)
    * > 7 bit device address | Write bit    (from micro)
    * > ACK                                 (from SFH_7773)
    * > 8 bit register address              (from micro)
    * > ACK                                 (from SFH_7773)
    * > RE-START                            (from micro)
    * > 7 bit device address | Read bit     (from micro)
    * > Read Byte                           (from SFH_7773)
    * > No Ack bit (SDA remains high)
    * > STOP                                (from micro)
    */

   switch (devCntxt_p->devId)
   {
      case PLTFRM_SFH_7773_1_DEV_ID:
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           i2cAddr |= PLTFRM_SFH_7773_1_I2C_ADDR;
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

   I2C_SW_start(devCntxt_p->devId);

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
PLTFRM_sts_t SFH_7773_cfgALSOpnMode(UINT8_t devId,
                                    SFH_7773_mode_t reqOpnMode,
                                    UINT16_t *opnTime_p)
{
   UINT8_t reqModeEnc, currModeEnc;
   PLTFRM_sts_t sts;
   SFH_7773_cntxt_s *devCntxt_p;

   devCntxt_p = SFH_7773_getDevCntxt(devId);
   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   switch (reqOpnMode)
   {
       case SFH_7773_OPN_MODE_TRIGGERED:
            reqModeEnc = SFH_7773_ALS_OPN_MODE_TRIGGERED_ENC;
            break;

       case SFH_7773_OPN_MODE_FREE_RUNNING:
            reqModeEnc = SFH_7773_ALS_OPN_MODE_FREE_RUNNING_ENC;
            break;

       default:
            reqModeEnc = SFH_7773_PS_OPN_MODE_STAND_BY_1_ENC;
            break;
   }

   sts = SFH_7773_readReg(devCntxt_p, SFH_7773_ALS_CTRL_REG_ADDR, &currModeEnc);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   currModeEnc &= SFH_7773_ALS_MODE_BIT_MSK;

   if (currModeEnc != reqModeEnc)
   {
       // As shown in the transition-diagram it is possible to switch
       // between all modes without any restriction. The transition time
       // between modes (ttrans) is < 10ms. The delay time between STAND-BY
       // and start of measurement is max. 10ms for the ambient light sensor.

       sts = SFH_7773_writeReg(devCntxt_p, SFH_7773_ALS_CTRL_REG_ADDR, reqModeEnc);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           // Need to wait for the transition time (max 10 millisecs)
           *opnTime_p = SFH_7773_ALS_OPN_MODE_TRANSITION_TIME;
           currModeEnc = reqModeEnc;
           sts = PLTFRM_STS_OPN_IN_PROGRESS;
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
PLTFRM_sts_t SFH_7773_getALSOutput(UINT8_t devId, SINT32_t *luxVal_p)
{
   UINT8_t regVal;
   PLTFRM_sts_t sts;
   SFH_7773_cntxt_s *devCntxt_p;

   devCntxt_p = SFH_7773_getDevCntxt(devId);
   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;
   /*
    * The 16 bit ALS data output can be converted to a lux value as
    * shown below :-
    *    lux value = (16 bit data output) * 0.9
    *
    * Read the two registers in this order (LSB, then MSB)
    */

    sts = SFH_7773_readReg(devCntxt_p, SFH_7773_ALS_DATA_LSB_REG_ADDR, &regVal);
    if (sts == PLTFRM_STS_SUCCESS)
    {
        UINT16_t alsDataOut;

        alsDataOut = regVal;

        sts = SFH_7773_readReg(devCntxt_p, SFH_7773_ALS_DATA_MSB_REG_ADDR, &regVal);
        if (sts == PLTFRM_STS_SUCCESS)
        {
            UINT16_t luxVal, tempU16;

            tempU16 = regVal;
            tempU16 = (tempU16 << 8) & 0xff00;

            alsDataOut |= tempU16;

            // 0.5 + 0.25 + 0.125 + 0.03125 is 0.90625 (close to 0.9)

            luxVal = (alsDataOut >> 1);   // 0.5
            luxVal += (alsDataOut >> 2);  // 0.25
            luxVal += (alsDataOut >> 3);  // 0.125
            luxVal += (alsDataOut >> 5);  // 0.03125

            *luxVal_p = luxVal;
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
PLTFRM_sts_t SFH_7773_triggerALSMeas(SFH_7773_cntxt_s *devCntxt_p)
{
   UINT8_t regVal = SFH_7773_ALS_MEAS_TRIGGER_BIT_MSK;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   sts = SFH_7773_writeReg(devCntxt_p,
                           SFH_7773_ALS_MCU_TRIGGER_REG_ADDR,
                           regVal);

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
PLTFRM_sts_t SFH_7773_startALSConv(UINT8_t devId,
                                   SINT32_t *luxVal_p,
                                   UINT16_t *tmoVal_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   SFH_7773_cntxt_s *devCntxt_p;

   devCntxt_p = SFH_7773_getDevCntxt(devId);
   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   switch (devCntxt_p->opnMode)
   {
      case SFH_7773_OPN_MODE_FREE_RUNNING:
           {
              // Free running at a rate which is configurable.
              sts = SFH_7773_getALSOutput(devId, luxVal_p);
           }
           break;

      case SFH_7773_OPN_MODE_TRIGGERED:
           {
              // The measurements are initiated via I2C-bus instruction.
              // Data available after processing is finished (default 100
              // milli-seconds for ALS).
              sts = SFH_7773_triggerALSMeas(devCntxt_p);
              if (sts == PLTFRM_STS_SUCCESS)
                  *tmoVal_p = SFH_7773_ALS_DFLT_INTEGRATION_TIME;
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
PLTFRM_sts_t SFH_7773_checkPresence(SFH_7773_cntxt_s *devCntxt_p)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;

   sts = SFH_7773_readReg(devCntxt_p,
                          SFH_773_PART_NR_AND_REV_REG_ADDR,
                          &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       regVal = (regVal >> SFH_7773_PART_NR_SHIFT) & SFH_7773_PART_NR_SHIFT_BIT_MSK;
       if (regVal == SFH_7773_PART_NUMBER_ID)
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
PLTFRM_sts_t SFH_7773_swReset(UINT8_t devId)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t regVal = SFH_7773_SW_RESET_TRIGGER_BIT_MSK;
   SFH_7773_cntxt_s *devCntxt_p = SFH_7773_getDevCntxt(devId);
   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   sts = SFH_7773_writeReg(devCntxt_p,
                           SFH_7773_SW_RST_CTRL_REG_ADDR,
                           regVal);

   if (sts == PLTFRM_STS_SUCCESS)
   {
       sts = SFH_7773_checkPresence(devCntxt_p);
       if (sts == PLTFRM_STS_SUCCESS)
           devCntxt_p->opnMode = SFH_7773_OPN_MODE_STAND_BY;
       else
           devCntxt_p->opnMode = SFH_7773_OPN_MODE_UNKNOWN;
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
SFH_7773_mode_t SFH_7773_getOpnMode(UINT8_t devId)
{
   SFH_7773_cntxt_s *devCntxt_p = SFH_7773_getDevCntxt(devId);
   return (devCntxt_p == NULL) ? SFH_7773_OPN_MODE_UNKNOWN :  devCntxt_p->opnMode;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t SFH_7773_init(UINT8_t devId)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   SFH_7773_cntxt_s *devCntxt_p = SFH_7773_getDevCntxt(devId);

   devCntxt_p->opnMode = SFH_7773_OPN_MODE_UNKNOWN;

   if (devCntxt_p == NULL)
   {
       SYS_fatal(SYS_FATAL_ERR_244);
       return PLTFRM_STS_INV_DEV_ID;
   }

   sts = SFH_7773_checkPresence(devCntxt_p);
   if (sts == PLTFRM_STS_SUCCESS)
       devCntxt_p->opnMode = SFH_7773_OPN_MODE_STAND_BY;
   else
   {
       SYS_fatal(SYS_FATAL_ERR_245);
   }

   return sts;
}

#endif
