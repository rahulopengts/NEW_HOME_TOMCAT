/*
 * File Name: tmp102.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Dec/29/2013
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


#ifdef PLTFRM_TMP102_TEMP_SENSOR_ENA


#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <i2c_sw.h>
#include <tmp102.h>


// #define TMP102_CFG_SHUTDOWN_MODE_ON_BOOTUP
#define TMP102_CFG_NORMAL_MODE_ON_BOOTUP

TMP102_cntxt_s TMP102_cntxtList[PLTFRM_TMP102_DEV_CNT] =
{
   {PLTFRM_TMP102_1_DEV_ID, TMP102_OPN_MODE_UNKNOWN}
};

/*
 * Data sheet: http://www.ti.com/lit/ds/symlink/tmp102.pdf
 *
 * Power supply: 1.4 to 3.6 volts
 */


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
TMP102_cntxt_s *TMP102_getDevCntxt(UINT8_t devId)
{
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_TMP102_DEV_CNT; idx++)
   {
      if (TMP102_cntxtList[idx].devId == devId)
          return (&(TMP102_cntxtList[idx]));
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
PLTFRM_sts_t TMP102_writeReg(TMP102_cntxt_s *devCntxt_p,
                             UINT8_t ptrRegVal,
                             UINT8_t *wrBuff_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t busId, i2cAddr = TMP102_I2C_ADDR_PREFIX;

   if (ptrRegVal > TMP102_HIGH_TH_REG_ADDR)
      return PLTFRM_STS_INV_PARAM;

   /*
    * Every write operation to the TMP102 requires a value for the Pointer
    * Register.
    */

   /*
    * Figure 13. Two-Wire Timing Diagram for Write Word Format
    *
    * Write to any register (2-bytes of data)
    *
    * > START                               (from micro)
    * > 7 bit device address | Write bit    (from micro)
    * > ACK                                 (from TMP102)
    * > 8 bit pointer register value        (from micro)
    * > ACK                                 (from TMP102)
    * > 8 bit data (MSB)                    (from micro)
    * > ACK                                 (from TMP102)
    * > 8 bit data (LSB)                    (from micro)
    * > ACK                                 (from TMP102)
    * > STOP                                (from micro)
    */

   switch (devCntxt_p->devId)
   {
      case PLTFRM_TMP102_1_DEV_ID:
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           i2cAddr |= PLTFRM_TMP102_1_I2C_ADDR;
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

   if (I2C_SW_wrByte(busId, ptrRegVal) == 0x0)
   {
       I2C_SW_stop(busId);
       return PLTFRM_STS_I2C_BUS_WR_ERROR;
   }

   if (I2C_SW_wrByte(busId, wrBuff_p[0]) == 0x0)
   {
       I2C_SW_stop(busId);
       return PLTFRM_STS_I2C_BUS_WR_ERROR;
   }

   if (I2C_SW_wrByte(busId, wrBuff_p[1]) == 0x0)
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
PLTFRM_sts_t TMP102_readReg(TMP102_cntxt_s *devCntxt_p,
                            UINT8_t ptrRegVal,
                            UINT8_t *rdBuff_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t busId, i2cAddr = TMP102_I2C_ADDR_PREFIX;

   if (ptrRegVal > TMP102_HIGH_TH_REG_ADDR)
      return PLTFRM_STS_INV_PARAM;

   /*
    * Accessing a particular register on the TMP102 is accomplished by
    * writing the appropriate value to the Pointer Register. The value
    * for the Pointer Register is the first byte transferred after the
    * slave address byte with the R/W bit low. Every write operation to
    * the TMP102 requires a value for the Pointer Register.
    *
    * The master can then generate a START condition and send the slave
    * address byte with the R/W bit high to initiate the read command.
    */

   /*
    * Figure 14. Two-Wire Timing Diagram for Read Word Format
    *
    * Read any register (2-bytes of data)
    *
    * > START                               (from micro)
    * > 7 bit device address | Write bit    (from micro)
    * > ACK                                 (from TMP102)
    * > 8 bit pointer register value        (from micro)
    * > ACK                                 (from TMP102)
    * > RE-START                            (from micro)
    * > 7 bit device address | Read bit     (from micro)
    * > Read Byte                           (from TMP102)
    * > No Ack bit (SDA remains high)
    * > STOP                                (from micro)
    */

   switch (devCntxt_p->devId)
   {
      case PLTFRM_TMP102_1_DEV_ID:
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           i2cAddr |= PLTFRM_TMP102_1_I2C_ADDR;
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

   if (I2C_SW_wrByte(busId, ptrRegVal) == 0x0)
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

   I2C_SW_rdByte(busId, rdBuff_p, I2C_ACK);
   rdBuff_p ++;

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
PLTFRM_sts_t TMP102_getTempVal(UINT8_t devId,
#ifdef TMP102_OUTPUT_SCALE_MILLI
                               SINT32_t *tempVal_p
#else
                               SINT16_t *tempVal_p
#endif
                               )
{
   PLTFRM_sts_t sts;
   UINT8_t tempBuff[TMP102_TEMP_REG_SZ];
   TMP102_cntxt_s *devCntxt_p;

   devCntxt_p = TMP102_getDevCntxt(devId);
   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   /*
    * The Temperature Register of the TMP102 is configured as a 12-bit, read-
    * only register (Configuration Register EM bit = '0', see the Extended Mode
    * section), or as a 13-bit, read-only register (Configuration Register EM
    * bit = '1') that stores the output of the most recent conversion. Two bytes
    * must be read to obtain data, and are described in Table 3 and Table 4. Note
    * that byte 1 is the most significant byte, followed by byte 2, the least
    * significant byte. The first 12 bits (13 bits in Extended mode) are used to
    * indicate temperature. The least significant byte does not have to be read
    * if that information is not needed. The data format for temperature is
    * summarized in Table 5 and Table 6. One LSB equals 0.0625 deg C. Negative
    * numbers are represented in binary twos complement format. Following power-up
    * or reset, the Temperature Register will read 0 deg C until the first conversion
    * is complete. Bit D0 of byte 2 indicates Normal mode (EM bit = '0') or Extended
    * mode (EM bit = '1') and can be used to distinguish between the two temperature
    * register data formats. The unused bits in the Temperature Register always read
    * '0'.
    */

   sts = TMP102_readReg(devCntxt_p, TMP102_TEMP_REG_ADDR, tempBuff);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       SINT16_t snsrVal = tempBuff[0];
       SINT32_t s32Val, tempVal;

       snsrVal = (snsrVal << 8) | tempBuff[1];
       snsrVal &= ~(TMP102_TEMP_REG_MODE_BIT);

       // Temperature in deg C = value * .0625 = value / 16

       if (tempBuff[1] & TMP102_TEMP_REG_MODE_BIT)
       {
           // Extended mode
           snsrVal = (snsrVal >> 3) & 0x1fff;
           if (tempBuff[0] & 0x80)
               snsrVal |= 0xe000;
       }
       else
       {
           // Normal mode
           snsrVal = (snsrVal >> 4) & 0xfff;
           if (tempBuff[0] & 0x80)
               snsrVal |= 0xf000;
       }

#ifdef TMP102_OUTPUT_SCALE_MILLI
       // Report in milli scale (25 deg C will be reported as 25000)
       // (snsrVal * 1000)/16 => snsrVal * 62.5 => (snsrVal * (64 - 2)) + (snsrVal / 2)

       s32Val = snsrVal;
       s32Val <<= 6;
       tempVal = s32Val; // snsrVal*64

       s32Val = snsrVal;
       s32Val <<= 1;  // snsrVal*2

       tempVal -= s32Val;  // snsrVal * (64 - 2)

       s32Val = snsrVal;
       s32Val >>= 1;
       tempVal += s32Val;
#else
       // Report in centi scale (25 deg C will be reported as 2500)
       // (snsrVal * 100)/16 => snsrVal * 6.25 => (snsrVal * (4 + 2)) + (snsrVal / 4)

       s32Val = snsrVal;
       s32Val <<= 2;  // snsrVal*4
       tempVal = s32Val;

       s32Val = snsrVal;
       s32Val <<= 1;  // snsrVal*2

       tempVal += s32Val;  // snsrVal*6

       snsrVal >>= 2;  // snsrVal/4
       tempVal += snsrVal;  // sensrVal*(6.25)
#endif

#ifdef TMP102_OUTPUT_SCALE_MILLI
       *tempVal_p = tempVal;
#else
       *tempVal_p = (SINT16_t)tempVal;
#endif
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
PLTFRM_sts_t TMP102_startSingleConv(UINT8_t devId,
                                    SINT32_t *tempVal_p,
                                    UINT16_t *tmoVal_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   TMP102_cntxt_s *devCntxt_p;

   devCntxt_p = TMP102_getDevCntxt(devId);
   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   if (devCntxt_p->opnMode == TMP102_OPN_MODE_UNKNOWN)
       return PLTFRM_STS_DEV_STATE_UNKNOWN;

   // Get the current mode of operation
   if (devCntxt_p->opnMode == TMP102_OPN_MODE_PERIODIC)
   {
       sts = TMP102_getTempVal(devId, tempVal_p);
   }
   else
   {
       UINT8_t tempBuff[TMP102_CONFIG_REG_SZ];

       /*
        * The TMP102 features a One-Shot Temperature Measurement mode. When the
        * device is in Shutdown mode, writing a ‘1’ to the OS bit starts a single
        * temperature conversion. During the conversion, the OS bit reads '0'. The
        * device returns to the shutdown state at the completion of the single
        * conversion. After the conversion, the OS bit reads '1'. This feature is
        * useful for reducing power consumption in the TMP102 when continuous temp
        * monitoring is not required. As a result of the short conversion time, the
        * TMP102 can achieve a higher conversion rate. A single conversion typically
        * takes 26ms and a read can take place in less than 20 micro-secs. When using
        * One-Shot mode, more conversions per second are possible.
        */
       if (tmoVal_p == NULL)
           SYS_fatal(SYS_FATAL_ERR_258);

       sts = TMP102_readReg(devCntxt_p, TMP102_CONFIG_REG_ADDR, tempBuff);
       if (sts != PLTFRM_STS_SUCCESS)
           return sts;

       if (!(tempBuff[0] & TMP102_CONFIG_REG_SD_BIT))
           return PLTFRM_STS_INV_DEV_OPN_MODE;

       // After the conversion, the OS bit reads '1'. During the conversion, the OS bit
       // reads '0'.
       if (tempBuff[0] & TMP102_CONFIG_REG_OS_BIT)
       {
           // Conversion not in progress, start one.
           sts = TMP102_writeReg(devCntxt_p, TMP102_CONFIG_REG_ADDR, tempBuff);
           if (sts != PLTFRM_STS_SUCCESS)
               return sts;
       }

       *tmoVal_p = TMP102_CONVERSION_TIME_MILLISECS * 2;
       sts = PLTFRM_STS_OPN_IN_PROGRESS;
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
PLTFRM_sts_t TMP102_init(UINT8_t devId)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   TMP102_cntxt_s *devCntxt_p = TMP102_getDevCntxt(devId);
   UINT8_t tempBuff[TMP102_CONFIG_REG_SZ];

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   /*
    * After power-up or general-call reset, the TMP102 immediately starts a
    * conversion. The TMP102 is in continuous conversion state (periodicity
    * depends on the CR01/CR1 bits in the configuration register).
    */

   /*
    * Shutdown mode is enabled when the SD bit is '1'; the device shuts down
    * when current conversion is completed. When SD is equal to '0', the device
    * maintains a continuous conversion state.
    */

   /*
    * The Configuration Register is a 16-bit read/write register used to store
    * bits that control the operational modes of the temperature sensor. Read/
    * write operations are performed MSB first.
    */
   sts = TMP102_readReg(devCntxt_p, TMP102_CONFIG_REG_ADDR, tempBuff);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

#ifdef TMP102_IN_SHUTDOWN_MODE_ON_BOOTUP
   /*
    * Shutdown mode is enabled when the SD bit is '1'; the device shuts down when
    * current conversion is completed.
    */
   if (!(tempBuff[0] & TMP102_CONFIG_REG_SD_BIT))
   {
       tempBuff[0] |= TMP102_CONFIG_REG_SD_BIT;

       // Setting OS bit to 1 will trigger a conversion in shutdown mode
       tempBuff[0] &= ~(TMP102_CONFIG_REG_OS_BIT);

       sts = TMP102_writeReg(devCntxt_p, TMP102_CONFIG_REG_ADDR, tempBuff);
   }

   if (sts == PLTFRM_STS_SUCCESS)
   {
       SINT16_t tmoVal = TMP102_CONVERSION_TIME_MILLISECS;

       tmoVal *= 2;

       // Wait for device to shut down ...

       while (tmoVal > 0)
       {
           PLTFRM_delay1000MicroSecs();
           tmoVal --;
       }

       devCntxt_p->opnMode = TMP102_OPN_MODE_ONE_SHOT;
   }
#elif defined(TMP102_CFG_NORMAL_MODE_ON_BOOTUP)
   /*
    * Shutdown mode is enabled when the SD bit is '1'. When SD is equal to '0', the
    * device maintains a continuous conversion state. After power-up or general-call
    * reset, the TMP102 immediately starts a conversion. The first result is available
    * after 26ms (typical).
    */
   if (tempBuff[0] & TMP102_CONFIG_REG_SD_BIT)
   {
       tempBuff[0] &= ~(TMP102_CONFIG_REG_SD_BIT + TMP102_CONFIG_REG_OS_BIT);

       sts = TMP102_writeReg(devCntxt_p, TMP102_CONFIG_REG_ADDR, tempBuff);

       if (sts == PLTFRM_STS_SUCCESS)
       {
    	   PLTFRM_delay1MilliSec();
           sts = TMP102_readReg(devCntxt_p, TMP102_TEMP_REG_ADDR, tempBuff);
           if (sts == PLTFRM_STS_SUCCESS)
           {
        	   if (tempBuff[0] & TMP102_CONFIG_REG_SD_BIT)
        		   sts = PLTFRM_STS_INV_DEV_OPN_MODE;
        	   else
               {
                   SINT16_t idx;
                   // Wait for first conversion to be over
                   for (idx=0; idx<30; idx++)
                   {
                	   PLTFRM_delay1MilliSec();
                   }

                   devCntxt_p->opnMode = TMP102_OPN_MODE_PERIODIC;
               }
           }
       }
   }
   else
   {
	   devCntxt_p->opnMode = TMP102_OPN_MODE_PERIODIC;
   }
#else
#error TMP102 boot up mode configuration not specified !!
#endif

   return sts;
}

#endif
