/*
 * File Name: cc2d33s.c
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

#ifdef PLTFRM_CC2D33S_SENSOR_ENA

/*
 * ChipCap2, digital, sleep mode, 3%, 3.3v
 *
 * Absolute max rating: Supply Voltage (VDD) : -0.3V to 6.0V
 * Supply Voltage: Min 2.7V to Max 5.5V
 * Supply current: 750 microamps
 * Sleep current: 0.6 microamps (typical)
 *
 *
 * The ChipCap 2 uses I2C-compatible communication protocol with support
 * for 100kHz and 400kHz bit rates. The I2C slave address (0x00 to 0x7F)
 * is selected by the Device_ID bits in the Cust_Config EEPROM word.
 *
 * Default I2C address is : 0101000
 *
 *
 * 5.3 Measurement Modes
 * The ChipCap 2 can be programmed to operate in either Sleep Mode or Update
 * Mode. The measurement mode is selected with the Measurement_Mode bit in the
 * ChipCap 2 Config Register word. In Sleep Mode, the part waits for commands
 * from the master before taking measurements (see section 5.3.2 below).
 */


#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <i2c_sw.h>
#include <cc2d33s.h>

UINT16_t CC2D33S_sensorInCmdModeCnt = 0;
UINT16_t CC2D33S_staleDataCnt = 0,
         CC2D33S_freshDataCnt = 0;


CC2D33S_cntxt_s CC2D33S_cntxtList[PLTFRM_LMP75B_DEV_CNT] =
{
   {PLTFRM_CC2D33S_1_RH_DEV_ID, CC2D33S_MEAS_MODE_UPDATE}
};


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
CC2D33S_cntxt_s *CC2D33S_getDevCntxt(UINT8_t devId)
{
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_CC2D33S_DEV_CNT; idx++)
   {
      if (CC2D33S_cntxtList[idx].devId == devId)
          return (&(CC2D33S_cntxtList[idx]));
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
PLTFRM_sts_t CC2D33S_startSingleConv(UINT8_t devId,
                                     UINT16_t *tmoVal_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   CC2D33S_cntxt_s *devCntxt_p;
   UINT8_t busId;
   const UINT8_t i2cAddr = CC2D33S_I2C_ADDR_PREFIX;

   /*
    * In Sleep Mode, the ChipCap 2 core will only perform conversions
    * when ChipCap 2 receives a Measurement Request command (MR); otherwise,
    * the ChipCap 2 is always powered down. Measurement Request commands can
    * only be sent using I2C, so Sleep Mode is not available for PDM.
    *
    * The master sends an MR command to wake the ChipCap 2 from power down. After
    * ChipCap 2 wakes up, a measurement cycle is performed consisting of both a
    * temperature and a capacitance conversion followed by the ChipCap 2 Core
    * correction calculations. At the end of a measurement cycle, the digital output
    * register and alarms will be updated before powering down. An I2C data fetch (DF)
    * is performed during the power-down period to fetch the data from the output register.
    */

   /*
    * MR command:
    * the communication contains only the slave address and the WRITE bit (0) sent by the
    * master. After the ChipCap 2 responds with the slave ACK, the master creates a stop
    * condition.
    */

   devCntxt_p = CC2D33S_getDevCntxt(devId);
   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   switch (devId)
   {
      case PLTFRM_CC2D33S_1_RH_DEV_ID:
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

   I2C_SW_stop(busId);

  *tmoVal_p = CC2D33S_CONV_TIME_MSECS;

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
PLTFRM_sts_t CC2D33S_dataFetch(UINT8_t devId,
                               UINT16_t *rh_p,
                               SINT16_t *tempVal_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   CC2D33S_cntxt_s *devCntxt_p;
   UINT8_t busId;
   const UINT8_t i2cAddr = CC2D33S_I2C_ADDR_PREFIX;
   UINT8_t rdBuff[4];
   UINT16_t valU16;
   SINT32_t valS32, tempFinal, rhFinal;

   /*
    * The Data Fetch (DF) command is used to fetch data in any digital output mode.
    */

   devCntxt_p = CC2D33S_getDevCntxt(devId);
   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   switch (devId)
   {
      case PLTFRM_CC2D33S_1_RH_DEV_ID:
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           break;

      default:
           sts = PLTFRM_STS_INV_DEV_ID;
           break;
   }

   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   I2C_SW_start(busId);

   /*
    * An I2C Data Fetch command starts with the 7-bit slave address and the 8th
    * bit = 1 (READ). The ChipCap 2 as the slave sends an acknowledgement (ACK)
    * indicating success.
    */

   if (I2C_SW_sendDevAddr_7(busId, i2cAddr, I2C_OPN_TYPE_READ) == 0x0)
   {
       I2C_SW_stop(busId);
       return PLTFRM_STS_I2C_BUS_WR_ERROR;
   }

   /*
    * The number of data bytes returned by the ChipCap 2 is determined by when the
    * master sends the NACK and stop condition.
    *
    * The full 14 bits of humidity data are fetched in the first two bytes. The MSBs
    * of the first byte are the status bits.
    *
    * If temperature data is needed, (up to two) additional temperature bytes can be
    * fetched.
    */

   I2C_SW_rdByte(busId, &rdBuff[0], I2C_ACK);   // RH Data [13:8]

   I2C_SW_rdByte(busId, &rdBuff[1], I2C_ACK);   // RH Data [7:0]

   I2C_SW_rdByte(busId, &rdBuff[2], I2C_ACK);   // Temp Data [13:6]

   I2C_SW_rdByte(busId, &rdBuff[3], I2C_NO_ACK);   // Temp Data [5:0]

   I2C_SW_stop(busId);

   valU16 = rdBuff[0];
   valU16 = (valU16 >> CC2D33S_RESP_STS_BITS_SHIFT) & CC2D33S_RESP_STS_SHIFT_BM;
   switch (valU16)
   {
      case CC2D33S_RESP_STS_DATA_FRESH:
           CC2D33S_freshDataCnt ++;
           break;

      case CC2D33S_RESP_STS_DATA_STALE:
           CC2D33S_staleDataCnt ++;
           break;

      default:
           CC2D33S_sensorInCmdModeCnt ++;
           break;
   }

   /*
    * <Relative humidity>
    * Resolution: 14 bit (0.01 % RH)
    * Accuracy: +/ 2.0 % RH (20 ~ 80 % RH)
    * Response time: 7 seconds
    * Conversion formula: ((RH_High [5:0] x 256 + RH_Low [7:0])/ 2^14) x 100
    */
   valU16 = rdBuff[0];
   valU16 = (valU16 << 8) & 0x3f00;
   valU16 |= rdBuff[1];
   valS32 = valU16;
   // Val * 100
   // = Val * (64 + 32 + 4)
   // = (Val << 6)  + (Val << 5) + (Val << 2)
   valS32 <<= 2;
   rhFinal = valS32;
   valS32 <<= 3;
   rhFinal += valS32;
   valS32 <<= 1;
   rhFinal += valS32;
   rhFinal >>= 14;
   *rh_p = (UINT16_t)rhFinal;

   /*
    * <Temperature>
    * Resolution: 14 bit (0.01°C)
    * Accuracy: +/- 0.3 deg C
    * Response Time: min 5 secs, max 20 secs
    * Operating Range: -40 deg C to +125 deg C
    * Conversion formula: ((Temp_High [7:0] x 64 + Temp_Low [7:2]/ 4)/ (2^14)) x 165 - 40
    */
   valU16 = rdBuff[3];
   valU16 = (valU16 >> 2) & 0x3f;
   valS32 = valU16;
   valU16 = rdBuff[2];
   valU16 = (valU16 << 6) & 0x3fc0;
   valS32 |= valU16;

   // tempVal * 165
   // = tempVal * (128 + 32 + 4 + 1)
   // = tempVal + (tempVal << 2)  + (tempVal << 5) + (tempVal << 7)
   tempFinal = valS32;
   tempFinal += (valS32 << 2);
   tempFinal += (valS32 << 5);
   tempFinal += (valS32 << 7);

   tempFinal >>= 14;
   tempFinal -= 40;

   *tempVal_p = (SINT16_t)tempFinal;

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
PLTFRM_sts_t CC2D33S_init(UINT8_t devId)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   CC2D33S_cntxt_s *devCntxt_p = CC2D33S_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   return sts;
}


#endif
