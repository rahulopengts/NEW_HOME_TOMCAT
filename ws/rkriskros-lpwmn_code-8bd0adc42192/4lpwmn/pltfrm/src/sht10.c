/*
 * File Name: sht10.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Jan/7/2015
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

#ifdef PLTFRM_SHT10_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <i2c_sw.h>
#include <sht10.h>
       

const SINT16_t SHT10_d1CoeffValAt3Volts = -3960;  // -39.6 * 100

static const char _snsrName[] = "SHT10";
static const char _snsrMfr[] = "SENSIRON";

SHT10_cntxt_s SHT10_cntxtList[PLTFRM_SHT10_DEV_CNT] =
{
   {PLTFRM_SHT10_1_TEMP_DEV_ID, SHT10_OP_DFLT_RES, {_snsrName, _snsrMfr}},
};
   

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
SHT10_cntxt_s *SHT10_getDevCntxt(UINT8_t devId)
{
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_SHT10_DEV_CNT; idx++)
   {
      if (SHT10_cntxtList[idx].devId == devId)
          return (&(SHT10_cntxtList[idx]));
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
PLTFRM_sts_t SHT10_getOpRes(SHT10_cntxt_s *dev_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   // <TODO>
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
PLTFRM_sts_t SHT10_readMeasVal(SHT10_cntxt_s *dev_p, UINT16_t *snsrOp_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t busId, dataLine, rdBuff[2];

   switch (dev_p->devId)
   {
      case PLTFRM_SHT10_1_TEMP_DEV_ID:
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           break;

      default:
           sts = PLTFRM_STS_INV_DEV_ID;
           break;
   }

   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   // Check the data line. It should be low.

   I2C_SW_readDataLine(busId, &dataLine);

   if (dataLine == 0x0)
   {
       // Low. Measurement is done. Read the sensor output (2 bytes, no CRC).
       /*
        * Two bytes of measurement data and one byte of CRC checksum (optional) 
        * will then be transmitted. The micro controller must acknowledge each 
        * byte by pulling the DATA line low. All values are MSB first, right 
        * justified.
        */

       I2C_SW_rdByte(busId, rdBuff, I2C_ACK);
       I2C_SW_rdByte(busId, rdBuff + 1, I2C_NO_ACK);

       (*snsrOp_p) = rdBuff[0];
       (*snsrOp_p) = ((*snsrOp_p) << 8) | rdBuff[1];
   }
   else
   {
       sts = PLTFRM_STS_DEV_NOT_READY;
   }

   // In either case, release both I2C lines.
   sts = I2C_SW_clkHiDataHi(busId);

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
PLTFRM_sts_t SHT10_readMeasRHVal(UINT8_t devId, UINT16_t *snsrRawOp_p)
{
   SHT10_cntxt_s *dev_p = SHT10_getDevCntxt(devId);

   if (dev_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   /*
    * For compensating non-linearity of the humidity sensor and for
    * obtaining the full accuracy of the sensor it is recommended to
    * convert the humidity readout (Sout) with the following formula
    * with coefficients given below.
    *
    * RH = C1 + C2*(Sout) + C3*(Sout)*(Sout);
    *
    *  ---------------------------------------
    *   Sout       C1       C2         C3
    *  ---------------------------------------
    *  12 bit   -2.0468   0.0367   -1.5955E-6
    *  8 bit    -2.0468   0.5872   -4.0845E-4
    *  ---------------------------------------
    *
    *  Values higher than 99% RH indicate fully saturated air and must
    *  be processed  and displayed as 100%RH13. Please note that the
    *  humidity sensor has no significant voltage dependency.
    */

   return SHT10_readMeasVal(dev_p, snsrRawOp_p);
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t SHT10_readMeasTempVal(UINT8_t devId, SINT16_t *tempVal_p)
{
   PLTFRM_sts_t sts;
   UINT16_t snsrOp;
   SHT10_cntxt_s *dev_p = SHT10_getDevCntxt(devId);

   if (dev_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   sts = SHT10_readMeasVal(dev_p, &snsrOp);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       // T = d1  + d2 * (SO)
       // d1 is -39.6 deg C (When Vdd is 3.0V).
       // d2 is 0.01 deg C when resolution is 14 bits and 0.04 deg C when
       // resolution is 12 bits.

       // T = (100*d1 + 100*d2*(SO)) / 100
       // T = (100*d1 + SO)/100 for 14 bit resolution
       // T = (100*d1 + SO*4)/100 for 12 bit resolution
       if (dev_p->res == SHT10_OP_RES_HIGH)
       {
           snsrOp &= 0x3fff;   // 14 valid bits
       }
       else
       {
           snsrOp &= 0x0fff;   // 12 valid bits
           snsrOp <<= 2;       // * 0.04
       }

       *tempVal_p = (SINT16_t)snsrOp;

       // Actual temperature is tempVal/100
       *tempVal_p += SHT10_d1CoeffValAt3Volts;
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
PLTFRM_sts_t SHT10_startMeas(SHT10_cntxt_s *dev_p,
                             UINT8_t i2cAddr)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t busId;

   switch (dev_p->devId)
   {
      case PLTFRM_SHT10_1_TEMP_DEV_ID:
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           break;

      default:
           sts = PLTFRM_STS_INV_DEV_ID;
           break;
   }

   if (sts != PLTFRM_STS_SUCCESS)
       return sts;
   /*
    * After issuing a measurement command, the controller has to wait for the 
    * measurement to complete. This takes a maximum of 20/80/320 ms for a 8/12/14 bit
    * measurement. The time varies with the speed of the internal oscillator and can 
    * be lower by up to 30%. To signal the completion of a measurement, the SHT1x pulls
    * data line low and enters Idle Mode. The controller must wait for this Data Ready 
    * signal before restarting SCK to readout the data.
    */

   I2C_SW_start(busId);
   I2C_SW_stop(busId);
   I2C_SW_clkLowDataLow(busId);

   if (I2C_SW_sendDevAddr_7(busId, i2cAddr, I2C_OPN_TYPE_READ) == 0x0)
       sts = PLTFRM_STS_I2C_BUS_WR_ERROR;

   // Ack received from sensor
   // Data would have gone high now. Will go low at the end of the measurement.

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
PLTFRM_sts_t SHT10_startTempMeas(UINT8_t devId, UINT16_t *tmoVal_p)
{
   PLTFRM_sts_t sts;
   SHT10_cntxt_s *dev_p = SHT10_getDevCntxt(devId);

   if (dev_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   sts = SHT10_startMeas(dev_p, SHT10_TEMP_MEAS_ADDR);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       if (tmoVal_p != NULL)
       {
           *tmoVal_p = dev_p->res == SHT10_OP_RES_HIGH \
                       ? SHT10_14_BIT_MEAS_TIME_MILLISECS : SHT10_12_BIT_MEAS_TIME_MILLISECS;
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
PLTFRM_sts_t SHT10_startRHMeas(UINT8_t devId, UINT16_t *tmoVal_p)
{
   PLTFRM_sts_t sts;
   SHT10_cntxt_s *dev_p = SHT10_getDevCntxt(devId);

   if (dev_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   sts = SHT10_startMeas(dev_p, SHT10_RH_MEAS_ADDR);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       if (tmoVal_p != NULL)
       {
           *tmoVal_p = dev_p->res == SHT10_OP_RES_HIGH \
                       ? SHT10_12_BIT_MEAS_TIME_MILLISECS : SHT10_8_BIT_MEAS_TIME_MILLISECS;
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
PLTFRM_sts_t SHT10_init(UINT8_t devId)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   SHT10_cntxt_s *devCntxt_p = SHT10_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;
   
   sts = SHT10_getOpRes(devCntxt_p);
   if (sts == PLTFRM_STS_SUCCESS) 
   {
       sts = PLTFRM_regSnsrDev(PLTFRM_SHT10_1_TEMP_DEV_ID, &devCntxt_p->info);
   }

   return sts;
}

#endif
