/*
 * File Name: adxl345b.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: April/1/2014
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

#ifdef PLTFRM_ADXL345_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <i2c_sw.h>
#include <adxl345.h>

// #define ADXL345_UT_ENA

#ifdef ADXL345_UT_ENA
#define ZACCEL_ARRAY_SZ  (512)
SINT16_t zAccelArray[ZACCEL_ARRAY_SZ];
UINT16_t zAccelIdx = 0;
SINT16_t minAccelVal = 32767;
SINT16_t maxAccelVal = -32768;
UINT8_t __resetArray = 0, __doneCnt = 0;
UINT16_t minAccelidx = 2048, maxAccelidx = 2048;
#endif



/*
 * - Activity -
 * The activity bit is set when acceleration greater than the value stored
 * in the THRESH_ACT register (Address 0x24) is experienced on any participating
 * axis, set by the ACT_INACT_CTL register (Address 0x27).
 *
 * - Inactivity -
 * The inactivity bit is set when acceleration of less than the value stored in
 * the THRESH_INACT register (Address 0x25) is experienced for more time than is
 * specified in the TIME_INACT register (Address 0x26) on all participating axes,
 * as set by the ACT_INACT_CTL register (Address 0x27). The maximum value for
 * TIME_INACT is 255 sec.
 */


ADXL345_cntxt_s ADXL345_cntxtList[PLTFRM_ADXL345_DEV_CNT] =
{
  {
     PLTFRM_ADXL345_1_DEV_ID,
     ADXL345_ALT_ADDR_LOGIC_HIGH,
     0,
     ADXL345_RANGE_UNKNOWN,
     ADXL345_RESOLUTION_UNKNOWN,
     ADXL345_ACCEL_DATA_JUSTIFICATION_UNKNOWN
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
PLTFRM_sts_t ADXL345_readReg(ADXL345_cntxt_s *devCntxt_p,
                             UINT8_t regAddr,
                             UINT8_t regCnt,
                             UINT8_t *rdBuff_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t busId, i2cAddr = devCntxt_p->i2cAddrPrefix;
   SINT16_t idx;

   if (!((regAddr == ADXL345_DEVID_REG_ADDR)
         || (regAddr >= ADXL345_THRESH_TAP_REG_ADDR
             && regAddr <= ADXL345_FIFO_STATUS_REG_ADDR)))
   {
       return PLTFRM_STS_INV_PARAM;
   }

   /*
    * Read any register
    *
    * > START (S)                                   (from micro)
    * > 7 bit device address | Write bit (AD+W)     (from micro)
    * > ACK                                         (from ADXL345)
    * > 8 bit register address (RA)                 (from micro)
    * > ACK                                         (from ADXL345)
    * > RE-START (S)                                (from micro)
    * > 7 bit device address | Read bit (AD+R)      (from micro)
    * > ACK                                         (from ADXL345)
    * > Read Byte (DATA)                            (from ADXL345)
    * > No Ack bit (SDA remains high) (NACK)
    * > STOP (P)                                    (from micro)
    */

   switch (devCntxt_p->devId)
   {
      case PLTFRM_ADXL345_1_DEV_ID:
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           break;

      default:
           sts = PLTFRM_STS_INV_DEV_ID;
           break;
   }

   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   // S --->
   I2C_SW_start(busId);

   // AD+W --->
   if (I2C_SW_sendDevAddr_7(busId, i2cAddr, I2C_OPN_TYPE_WRITE) == 0x0)
   {
       I2C_SW_stop(busId);
       return PLTFRM_STS_I2C_BUS_WR_ERROR;
   }

   // RA --->
   if (I2C_SW_wrByte(busId, regAddr) == 0x0)
   {
       I2C_SW_stop(busId);
       return PLTFRM_STS_I2C_BUS_WR_ERROR;
   }

   // S --->
   I2C_SW_start(busId);

   // AD+R --->
   if (I2C_SW_sendDevAddr_7(busId, i2cAddr, I2C_OPN_TYPE_READ) == 0x0)
   {
       I2C_SW_stop(busId);
       return PLTFRM_STS_I2C_BUS_WR_ERROR;
   }

   for (idx=0; idx<regCnt; idx++)
   {
       // ACK -->
       I2C_SW_rdByte(busId, rdBuff_p, idx == (regCnt - 1) ? I2C_NO_ACK : I2C_ACK);
       rdBuff_p ++;
   }

   // P --->
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
PLTFRM_sts_t ADXL345_writeReg(ADXL345_cntxt_s *devCntxt_p,
                              UINT8_t regAddr,
                              UINT8_t regVal)
{
   UINT8_t busId;
   const UINT8_t i2cAddr = devCntxt_p->i2cAddrPrefix;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   if (regAddr < ADXL345_THRESH_TAP_REG_ADDR
       || regAddr > ADXL345_FIFO_CTL_REG_ADDR)
   {
       return PLTFRM_STS_INV_PARAM;
   }

   if (regAddr == ADXL345_ACT_TAP_STATUS_REG_ADDR
       || regAddr == ADXL345_INT_SOURCE_REG_ADDR
       || (regAddr >= ADXL345_DATAX0_REG_ADDR
           && regAddr <= ADXL345_DATAZ1_REG_ADDR))
   {
       return PLTFRM_STS_HW_REG_IS_READ_ONLY;
   }
   // All registers are 8 bits long

   switch (devCntxt_p->devId)
   {
      case PLTFRM_ADXL345_1_DEV_ID:
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
 ********************************************************************
 */
ADXL345_cntxt_s *ADXL345_getDevCntxt(UINT8_t devId)
{
#if PLTFRM_ADXL345_DEV_CNT==1
   if (ADXL345_cntxtList[0].devId == devId)
       return (&(ADXL345_cntxtList[0]));
#else
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_ADXL345_DEV_CNT; idx++)
   {
       if (ADXL345_cntxtList[idx].devId == devId)
           return (&(ADXL345_cntxtList[idx]));
   }
#endif

   return NULL;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t ADXL345_cfgFIFO(UINT8_t devId,
                             ADXL345_fifoMode_t fifoMode)
{
   UINT8_t regVal;
   PLTFRM_sts_t sts;
   ADXL345_cntxt_s *devCntxt_p = ADXL345_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   sts = ADXL345_readReg(devCntxt_p,
                         ADXL345_FIFO_CTL_REG_ADDR,
                         1, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       UINT8_t newCfg = (UINT8_t)fifoMode;
       newCfg <<= ADXL345_FIFO_CTL_FIFO_MODE_BIT_SHIFT;
       newCfg &= ADXL345_FIFO_CTL_FIFO_MODE_BIT_MSK;

       regVal &= ~(ADXL345_FIFO_CTL_FIFO_MODE_BIT_MSK);
       regVal |= newCfg;

       sts = ADXL345_writeReg(devCntxt_p,
                              ADXL345_POWER_CTL_REG_ADDR,
                              regVal);
   }

   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t ADXL345_startMeas(UINT8_t devId)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;
   ADXL345_cntxt_s *devCntxt_p = ADXL345_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   /*
    * <Measure Bit>
    * A setting of 0 in the measure bit places the part into standby mode,
    * and a setting of 1 places the part into measurement mode. The ADXL345
    * powers up in standby mode with minimum power consumption.
    */

   sts = ADXL345_readReg(devCntxt_p,
                         ADXL345_POWER_CTL_REG_ADDR,
                         1, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       if (!(regVal & ADXL345_POWER_CTL_REG_MEASURE_BIT))
       {
           regVal |= ADXL345_POWER_CTL_REG_MEASURE_BIT;

           sts = ADXL345_writeReg(devCntxt_p,
                                  ADXL345_POWER_CTL_REG_ADDR,
                                  regVal);
       }
   }

   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t ADXL345_stopMeas(UINT8_t devId)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;
   ADXL345_cntxt_s *devCntxt_p = ADXL345_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   /*
    * <Measure Bit>
    * A setting of 0 in the measure bit places the part into standby mode,
    * and a setting of 1 places the part into measurement mode. The ADXL345
    * powers up in standby mode with minimum power consumption.
    */

   sts = ADXL345_readReg(devCntxt_p,
                         ADXL345_POWER_CTL_REG_ADDR,
                         1, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       if (regVal & ADXL345_POWER_CTL_REG_MEASURE_BIT)
       {
           regVal &= ~ADXL345_POWER_CTL_REG_MEASURE_BIT;

           sts = ADXL345_writeReg(devCntxt_p,
                                  ADXL345_POWER_CTL_REG_ADDR,
                                  regVal);
       }
   }

   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t ADXL345_putToSleep(UINT8_t devId)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;
   ADXL345_cntxt_s *devCntxt_p = ADXL345_getDevCntxt(devId);

   /*
    * <Sleep Bit>
    * A setting of 0 in the sleep bit puts the part into the normal mode
    * of operation, and a setting of 1 places the part into sleep mode.
    * Sleep mode suppresses DATA_READY, stops transmission of data to FIFO,
    * and switches the sampling rate to one specified by the wakeup bits.
    * In sleep mode, only the activity function can be used. When the
    * DATA_READY interrupt is suppressed, the output data registers
    * (Register 0x32 to Register 0x37) are still updated at the sampling
    * rate set by the wakeup bits (D1:D0).
    */

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   sts = ADXL345_readReg(devCntxt_p,
                         ADXL345_POWER_CTL_REG_ADDR,
                         1, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       if (!(regVal & ADXL345_POWER_CTL_REG_SLEEP_BIT))
       {
           // Not sleeping currently
           regVal |= ADXL345_POWER_CTL_REG_SLEEP_BIT;
           sts = ADXL345_writeReg(devCntxt_p,
                                  ADXL345_POWER_CTL_REG_ADDR,
                                  regVal);
       }
   }

   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t ADXL345_wakeUp(UINT8_t devId)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;
   ADXL345_cntxt_s *devCntxt_p = ADXL345_getDevCntxt(devId);
   const UINT8_t regAddr = ADXL345_POWER_CTL_REG_ADDR;

   /*
    * <Sleep Bit>
    * A setting of 0 in the sleep bit puts the part into the normal mode
    * of operation, and a setting of 1 places the part into sleep mode.
    *
    * When clearing the sleep bit, it is recommended that the part be
    * placed into standby mode and then set back to measurement mode
    * with a subsequent write. This is done to ensure that the device
    * is properly biased if sleep mode is manually disabled; otherwise,
    * the first few samples of data after the sleep bit is cleared may
    * have additional noise, especially if the device was asleep when the
    * bit was cleared.
    */

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   sts = ADXL345_readReg(devCntxt_p,
                         regAddr, 1, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       if (regVal & ADXL345_POWER_CTL_REG_SLEEP_BIT)
       {
           UINT8_t measFlag;

           // Sleeping currently
           if (regVal & ADXL345_POWER_CTL_REG_MEASURE_BIT)
               measFlag = 0x1;

           regVal &= ~(ADXL345_POWER_CTL_REG_SLEEP_BIT
                       | ADXL345_POWER_CTL_REG_MEASURE_BIT);
           sts = ADXL345_writeReg(devCntxt_p, regAddr, regVal);
           if (sts == PLTFRM_STS_SUCCESS
               && measFlag)
           {
               regVal |= ADXL345_POWER_CTL_REG_MEASURE_BIT;
               sts = ADXL345_writeReg(devCntxt_p, regAddr, regVal);
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
 ********************************************************************
 */
PLTFRM_sts_t ADXL345_setRange(UINT8_t devId,
                              const ADXL345_accelRange_t range)
{
   UINT8_t regVal;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   ADXL345_cntxt_s *devCntxt_p = ADXL345_getDevCntxt(devId);
   const UINT8_t regAddr = ADXL345_DATA_FORMAT_REG_ADDR;

   sts = ADXL345_readReg(devCntxt_p, regAddr, 1, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       if ((regVal & ADXL345_DATA_FORMAT_RANGE_BIT_MSK) != (UINT8_t)range)
       {
           regVal &= ~(ADXL345_DATA_FORMAT_RANGE_BIT_MSK);
           regVal |= (UINT8_t)range;
           sts = ADXL345_writeReg(devCntxt_p, regAddr, regVal);
           devCntxt_p->range = range;
       }
   }

   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t ADXL345_setDataLayoutMode(UINT8_t devId,
                                       ADXL345_dataLayoutMode_t layoutMode)
{
   UINT8_t regVal;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   const UINT8_t regAddr = ADXL345_DATA_FORMAT_REG_ADDR;
   ADXL345_cntxt_s *devCntxt_p = ADXL345_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   sts = ADXL345_readReg(devCntxt_p, regAddr, 1, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       regVal &= ~(ADXL345_DATA_FORMAT_JUSTIFY_BIT_MSK);

       if (layoutMode == ADXL345_ACCEL_DATA_LEFT_JUSTIFIED)
           regVal |= ADXL345_DATA_FORMAT_JUSTIFY_BIT_MSK;

       sts = ADXL345_writeReg(devCntxt_p, regAddr, regVal);
       devCntxt_p->dataLayOutMode = layoutMode;
   }

   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t ADXL345_setOutputRate(UINT8_t devId,
                                   ADXL345_outputDataRate_t opRate)
{
   UINT8_t regVal;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   const UINT8_t regAddr = ADXL345_BW_RATE_REG_ADDR;
   ADXL345_cntxt_s *devCntxt_p = ADXL345_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   sts = ADXL345_readReg(devCntxt_p, regAddr, 1, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       regVal &= ~(ADXL345_BW_RATE_REG_RATE_BIT_MSK);
       regVal |= (UINT8_t)opRate;
       sts = ADXL345_writeReg(devCntxt_p, regAddr, regVal);
   }

   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t ADXL345_setResolution(UINT8_t devId,
                                   ADXL345_resolution_t res)
{
   UINT8_t regVal;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   const UINT8_t regAddr = ADXL345_DATA_FORMAT_REG_ADDR;
   ADXL345_cntxt_s *devCntxt_p = ADXL345_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   sts = ADXL345_readReg(devCntxt_p, regAddr, 1, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       regVal &= ~(ADXL345_DATA_FORMAT_FULL_RES_BIT_MSK);

       if (res == ADXL345_RESOLUTION_FULL_SCALE)
           regVal |= ADXL345_DATA_FORMAT_FULL_RES_BIT_MSK;

       sts = ADXL345_writeReg(devCntxt_p, regAddr, regVal);
       devCntxt_p->resolution = res;
   }

   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
SINT16_t ADXL345_getAccelData(ADXL345_cntxt_s *devCntxt_p, UINT8_t *buff_p)
{
   SINT16_t accelVal = 0xffff;

   if (devCntxt_p->resolution == ADXL345_RESOLUTION_10_BITS_FIXED)
   {
       if (devCntxt_p->dataLayOutMode == ADXL345_ACCEL_DATA_RIGHT_JUSTIFIED)
       {
           // Offset 0 - LSB, Offset 1 - MSB
           accelVal = buff_p[1];
           accelVal = (accelVal << 8) & 0xff00;
           accelVal |= buff_p[0];
       }
       else
       {
           // <TODO>
           SYS_fatal(SYS_FATAL_ERR_213);
       }
   }
   else
   {
       // <TODO>
       SYS_fatal(SYS_FATAL_ERR_214);
   }

   return accelVal;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t ADXL345_readAccelData(UINT8_t devId,
                                   SINT16_t *xAccel_p,
                                   SINT16_t *yAccel_p,
                                   SINT16_t *zAccel_p)

{
   UINT8_t accelDataArray[ADXL345_ACCEL_DATA_REG_CNT];
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   ADXL345_cntxt_s *devCntxt_p = ADXL345_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   /*
    * These six bytes (Register 0x32 to Register 0x37) are eight bits
    * each and hold the output data for each axis. The output data is
    * twos complement, with DATAx0 as the least significant byte and
    * DATAx1 as the most significant byte, where x represent X, Y, or
    * Z. The DATA_FORMAT register (Address 0x31) controls the format
    * of the data. It is recommended that a multiple-byte read of all
    * registers be performed to prevent a change in data between reads
    * of sequential registers
    */

   sts = ADXL345_readReg(devCntxt_p,
                         ADXL345_DATAX0_REG_ADDR,
                         ADXL345_ACCEL_DATA_REG_CNT,
                         accelDataArray);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       /*
        * The output data is twos complement, with DATAx0 as the least
        * significant byte and DATAx1 as the most significant byte, where
        * x represents X, Y, or Z.
        */

       if (xAccel_p != NULL)
           *(xAccel_p) = ADXL345_getAccelData(devCntxt_p, accelDataArray + ADXL345_X_AXIS_DATA_OFF);

       if (yAccel_p != NULL)
           *(yAccel_p) = ADXL345_getAccelData(devCntxt_p, accelDataArray + ADXL345_Y_AXIS_DATA_OFF);

       if (zAccel_p != NULL)
       {
           *(zAccel_p) = ADXL345_getAccelData(devCntxt_p, accelDataArray + ADXL345_Z_AXIS_DATA_OFF);
       }
    }

    return sts;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t ADXL345_init(UINT8_t devId)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   ADXL345_cntxt_s *devCntxt_p = ADXL345_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   devCntxt_p->i2cAddrPrefix = devCntxt_p->altAddrPinVal == ADXL345_ALT_ADDR_LOGIC_HIGH \
                               ? ADXL345_ALT_HIGH_I2C_ADDR_PREFIX : ADXL345_ALT_LOW_I2C_ADDR_PREFIX;

   {
       UINT8_t regVal;
       sts = ADXL345_readReg(devCntxt_p,
                             ADXL345_DEVID_REG_ADDR,
                             1, &regVal);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           if (regVal != ADXL345_DEV_ID)
               sts = PLTFRM_STS_DEV_PART_NR_MISMATCH;
       }
   }


#ifdef ADXL345_UT_ENA

   sts = ADXL345_setRange(devId, ADXL345_RANGE_16G);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_3035);

   sts = ADXL345_setResolution(devId, ADXL345_RESOLUTION_10_BITS_FIXED);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_3036);

   sts = ADXL345_setDataLayoutMode(devId, ADXL345_ACCEL_DATA_RIGHT_JUSTIFIED);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_3037);

   sts = ADXL345_cfgFIFO(devId, ADXL345_FIFO_MODE_BYPASS);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_3038);

   sts = ADXL345_setOutputRate(devId, ADXL345_OUTPUT_DATA_RATE_1600_HZ);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_3039);

   sts = ADXL345_wakeUp(devId);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = ADXL345_startMeas(devId);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   do
   {
       SINT16_t accelVal, idx;

       sts = ADXL345_readAccelData(devId, NULL, NULL, &accelVal);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_215);

       if (accelVal > maxAccelVal)
       {
           maxAccelVal = accelVal;
           maxAccelidx = zAccelIdx;
       }

       if (accelVal < minAccelVal)
       {
           minAccelidx = zAccelIdx;
           minAccelVal = accelVal;
       }

       if (zAccelIdx < ZACCEL_ARRAY_SZ)
       {
           zAccelArray[zAccelIdx] = accelVal;
           zAccelIdx ++;
       }
       else
       {
           __doneCnt ++;
       }

       PLTFRM_delay625MicroSecs();

       if (__resetArray)
       {
           for(idx=0; idx<ZACCEL_ARRAY_SZ; idx++)
               zAccelArray[idx] = 32767;
           zAccelIdx = 0;
           maxAccelVal = -32768;
           minAccelVal = 32767;
           __resetArray = 0;
       }

   } while (sts == PLTFRM_STS_SUCCESS);
#endif

   return sts;
}

#endif
