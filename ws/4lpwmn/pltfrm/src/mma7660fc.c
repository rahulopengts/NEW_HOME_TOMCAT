/*
 * File Name: mma7660fc.c
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

#ifdef PLTFRM_MMA7660FC_SENSOR_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <i2c_sw.h>
#include <mma7660fc.h>

/*
 * The device is a low-power, high accuracy digital output altimeter, barometer and
 * thermometer, packaged in a 3 x 5 x 1.1 mm form factor. The complete device includes
 * a sensing element, analog and digital signal processing and an I2C interface.
 *
 * The device can be used for sensor data changes, product orientation, and gesture
 * detection through an interrupt pin (INT). The device is housed in a small 3mm x 3mm x
 * 0.9mm DFN package.
 */


MMA7660FC_cntxt_s MMA7660FC_cntxtList[PLTFRM_MMA7660FC_DEV_CNT] =
{
   {PLTFRM_MMA7660FC_1_DEV_ID}
};


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
MMA7660FC_cntxt_s *MMA7660FC_getDevCntxt(UINT8_t devId)
{
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_MMA7660FC_DEV_CNT; idx++)
   {
      if (MMA7660FC_cntxtList[idx].devId == devId)
          return (&(MMA7660FC_cntxtList[idx]));
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
PLTFRM_sts_t MMA7660FC_writeReg(MMA7660FC_cntxt_s *devCntxt_p,
                               UINT8_t regAddr,
                               UINT8_t regVal)
{
   UINT8_t busId;
   const UINT8_t i2cAddr = MMA7660FC_I2C_ADDR_PREFIX;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   if (regAddr > MMA7660FC_PD_REG_ADDR)
       return PLTFRM_STS_INV_PARAM;

   // All registers are 8 bits long

   switch (devCntxt_p->devId)
   {
      case PLTFRM_MMA7660FC_1_DEV_ID:
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
PLTFRM_sts_t MMA7660FC_readReg(MMA7660FC_cntxt_s *devCntxt_p,
                               UINT8_t regAddr,
                               UINT8_t *rdBuff_p)
{
   UINT8_t busId;
   const UINT8_t i2cAddr = MMA7660FC_I2C_ADDR_PREFIX;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   /*
    * A read is initiated by first configuring the device’s register address
    * by performing a write followed by a repeated start.
    */

   if (regAddr > MMA7660FC_PD_REG_ADDR)
       return PLTFRM_STS_INV_PARAM;

   // All registers are 8 bits long

   switch (devCntxt_p->devId)
   {
      case PLTFRM_MMA7660FC_1_DEV_ID:
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
PLTFRM_sts_t MMA7660FC_getTiltSts(UINT8_t devId,
                                  UINT8_t *tiltSts_p)
{
   PLTFRM_sts_t sts;
   MMA7660FC_cntxt_s *devCntxt_p = MMA7660FC_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   sts = MMA7660FC_readReg(devCntxt_p,
                           MMA7660FC_TILT_REG_ADDR,
                           tiltSts_p);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_224);
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
PLTFRM_sts_t MMA7660FC_getCurrOpnMode(MMA7660FC_cntxt_s *devCntxt_p,
                                      UINT8_t *modeRegVal_p)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;

   sts = MMA7660FC_readReg(devCntxt_p,
                           MMA7660FC_MODE_REG_ADDR,
                           &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       /*
        * Mode
        * 0: Standby mode or Test Mode depending on state of TON
        * 1: Active mode
        */
       if (regVal & MMA7660FC_MODE_REG_MODE_BIT)
       {
           // Mode - 1
           devCntxt_p->opnMode = MMA7660FC_OPN_MODE_ACTIVE;
       }
       else
       {
           /*
            * TON
            * 0: Standby Mode or Active Mode depending on state of MODE
            * 1: Test Mode
            */
           if (regVal & MMA7660FC_MODE_REG_TON_BIT)
           {
               // Mode - 0, TON - 1
               devCntxt_p->opnMode = MMA7660FC_OPN_MODE_TEST;
           }
           else
           {
               // Mode - 0, TON - 0
               devCntxt_p->opnMode = MMA7660FC_OPN_MODE_STANDBY;
           }
       }

       if (modeRegVal_p != NULL)
           *modeRegVal_p = regVal;
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
PLTFRM_sts_t MMA7660FC_cfgOpnMode(MMA7660FC_cntxt_s *devCntxt_p,
                                  MMA7660FC_opnMode_t newMode)
{
   PLTFRM_sts_t sts;
   UINT8_t modeRegVal, confirmFlag = 0, viaStandByFlag = 0;

   do
   {
       sts = MMA7660FC_getCurrOpnMode(devCntxt_p, &modeRegVal);
       if (sts != PLTFRM_STS_SUCCESS)
       {
           SYS_fatal(SYS_FATAL_ERR_225);
       }

       if (viaStandByFlag)
       {
           if (devCntxt_p->opnMode != MMA7660FC_OPN_MODE_STANDBY)
           {
               SYS_fatal(SYS_FATAL_ERR_226);
           }
           else
               viaStandByFlag = 0;
       }
       else
       {
           if (devCntxt_p->opnMode == newMode)
               break;
           else
           {
               if (confirmFlag)
               {
                   SYS_fatal(SYS_FATAL_ERR_227);
               }
           }
       }

       switch (newMode)
       {
          case MMA7660FC_OPN_MODE_ACTIVE:
               {
                  if (devCntxt_p->opnMode == MMA7660FC_OPN_MODE_STANDBY)
                  {
                      // STANDBY -> ACTIVE
                      modeRegVal |= MMA7660FC_MODE_REG_MODE_BIT;
                  }
                  else
                  {
                      // TEST -> ACTIVE
                      // Existing state of TON bit must be 0, to write MODE = 1.
                      // Test Mode must not be enabled.

                      // First change state from TEST to STANDBY

                      modeRegVal &= ~(MMA7660FC_MODE_REG_TON_BIT);
                      viaStandByFlag = 1;
                  }
              }
              break;

         case MMA7660FC_OPN_MODE_STANDBY:
              {
                  if (devCntxt_p->opnMode == MMA7660FC_OPN_MODE_ACTIVE)
                  {
                      // ACTIVE -> STANDBY
                      modeRegVal &= ~(MMA7660FC_MODE_REG_MODE_BIT);
                  }
                  else
                  {
                      // TEST -> STANDBY
                      modeRegVal &= ~(MMA7660FC_MODE_REG_TON_BIT);
                  }
              }
              break;

         case MMA7660FC_OPN_MODE_TEST:
              {
                  if (devCntxt_p->opnMode == MMA7660FC_OPN_MODE_STANDBY)
                  {
                      // STANDBY -> TEST
                      modeRegVal |= MMA7660FC_MODE_REG_TON_BIT;
                  }
                  else
                  {
                      // ACTIVE -> TEST
                      // Existing state of MODE bit must be 0, to write TON = 1.
                      // Device must be in Standby Mode.

                      // First change state from ACTIVE to STANDBY

                      modeRegVal &= ~(MMA7660FC_MODE_REG_MODE_BIT);
                      viaStandByFlag = 1;
                  }
              }
              break;

         default:
              {
                 SYS_fatal(SYS_FATAL_ERR_228);
              }
              break;
       }

       if (1)
       {
           sts = MMA7660FC_writeReg(devCntxt_p, MMA7660FC_MODE_REG_ADDR, modeRegVal);
           if (sts != PLTFRM_STS_SUCCESS)
           {
               SYS_fatal(SYS_FATAL_ERR_229);
           }
           else
           {
               PLTFRM_delay1MilliSec();
               confirmFlag = 1;
           }
       }
   } while (1);

   return sts;
}

UINT8_t MMA7660FC_tiltReg = 0;

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t MMA7660FC_init(UINT8_t devId)
{
   PLTFRM_sts_t sts;
   MMA7660FC_cntxt_s *devCntxt_p = MMA7660FC_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   devCntxt_p->opnMode = MMA7660FC_OPN_MODE_UNKNOWN;

   sts = MMA7660FC_getCurrOpnMode(devCntxt_p, NULL);
   if (sts != PLTFRM_STS_SUCCESS
       || devCntxt_p->opnMode == MMA7660FC_OPN_MODE_UNKNOWN)
   {
       SYS_fatal(SYS_FATAL_ERR_230);
   }

   sts = MMA7660FC_cfgOpnMode(devCntxt_p, MMA7660FC_OPN_MODE_ACTIVE);

   return sts;
}


#endif
