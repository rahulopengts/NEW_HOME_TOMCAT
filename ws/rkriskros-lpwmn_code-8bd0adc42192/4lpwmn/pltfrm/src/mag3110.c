/*
 * File Name: mag3110.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Nov/19/2015
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

#ifdef PLTFRM_MAG3110_SENSOR_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <i2c_sw.h>
#include <mag3110.h>

static const char _snsrName[] = "MAG3110";
static const char _snsrMfr[] = "FREESCALE";

MAG3110_cntxt_s MAG3110_cntxtList[PLTFRM_MAG3110_DEV_CNT] =
{
   {
     PLTFRM_MAG3110_1_DEV_ID,
     PLTFRM_GPIO_PORT_1,
     PLTFRM_GPIO_PORT_PIN_4,
     PLTFRM_I2C_SW_BUS_1_ID,
     MAG3110_I2C_ADDR_PREFIX,
     MAG3110_OUTPUT_UNCORRECTED,
     {_snsrName, _snsrMfr},
     MAG3110_ADC_RATE_1280_HZ_ENC, // 0  to 7
     0,  // osr encoding  (16)
     0
   },
};

/*
 * The MAG3110 is capable of measuring magnetic fields with an output data rate
 * (ODR) up to 80 Hz; these output data rates correspond to sample intervals from
 * 12.5 ms to several seconds.
 *
 *
 * Power supply: 1.95V to 3.6V
 * Has separate digital(VDDIO) and analog(VDD) supply pins.
 * Full-scale range: +/- 1000 uT (micro-tesla)
 * Resolution: 0.1 uT
 *
 * strength of Earth's magnetic field at 0 deg latitude, 0 deg longitude
 * 31.869 uT
 *
 *
 * <https://www.ngdc.noaa.gov/geomag/faqgeom.shtml#What_is_the_Earths_magnetic_field>
 *
 * At any point and time, the Earth's magnetic field is characterized by a
 * direction and intensity which can be measured. Often the parameters measured
 * are the magnetic declination, D, the horizontal intensity, H, and the vertical
 * intensity, Z. From these elements, all other parameters of the magnetic field
 * can be calculated.
 *
 * The Earth's magnetic field is described by seven parameters. These are -
 *
 * > declination (D),
 * > inclination (I),
 * > horizontal intensity (H),
 * > the north (X) and east (Y) components of the horizontal intensity,
 * > vertical intensity (Z), and
 * > total intensity (F).
 *
 * The parameters describing the direction of the magnetic field are declination (D)
 * and inclination (I). D and I are measured in units of degrees, positive east for
 * D and positive down for I. The intensity of the total field (F) is described by
 * the horizontal component (H), vertical component (Z), and the north (X) and
 * east (Y) components of the horizontal intensity. These components may be measured
 * in units of gauss but are generally reported in nanoTesla (1nT * 100,000 = 1 gauss).
 *
 *
 * The magnetic field is different in different places. In fact, the magnetic field
 * changes with both location and time. It is so irregular that it must be measured in
 * many places to get a satisfactory picture of its distribution.
 *
 * However, there are some regular features of the magnetic field. At the magnetic poles,
 * a dip needle stands vertical (dip=90 degrees), the horizontal intensity is zero, and
 * a compass does not show direction (D is undefined). At the north magnetic pole, the
 * north end of the dip needle is down; at the south magnetic pole, the north end is up.
 * At the magnetic equator the dip or inclination is zero. Unlike the Earth's geographic
 * equator, the magnetic equator is not fixed, but slowly changes.
 *
 * All magnets have two poles, where the lines of magnetic flux enter and emerge. By analogy
 * with the Earth's magnetic field, these are called the magnet's "north" and "south" poles.
 * The convention in early compasses was to call the end of the needle pointing to the Earth's
 * North Magnetic Pole the "north pole" (or "north-seeking pole") and the other end the "south
 * pole" (the names are often abbreviated to "N" and "S"). Because opposite poles attract,
 * this definition means that the Earth's North Magnetic Pole is actually a magnetic south
 * pole and the Earth's South Magnetic Pole is a magnetic north pole.
 *
 *
 * The horizontal component of the geomagnetic field always points to the magnetic north
 * pole. In the northern hemisphere, the vertical component also points downward with the
 * precise angle being dependent on location.
 *
 *
 * Soft-iron and hard-iron interference
 * > Soft-iron (think steel in EMI  sheilds, screws and battery contacts).  Steel provides a
 *   “lower resistance” path to the magnetic field than does the surrounding air.  So it’s
 *   natural for the field to be diverted
 * > Hard-iron (permanent magnets such as a speaker magnet). 'Hard-iron' magnetic fields are
 *   those which are generated by permanently magnetized ferromagnetic components on the PCB,
 *   such as audio speakers and buzzers. Permanently magnetized components also create permanent
 *   induced magnetic fields in normally unmagnetized ferromagnetic materials in their vicinity
 *   and these are also included as ‘hard-iron’ fields.
 *
 * It makes little sense for manufacturers to supply carefully calibrated magnetometers with
 * no zero field offset into the smartphone market since the magnetometer will be exposed to an
 * unknown additive hard-iron field. The manufacturer will typically specify only the limits
 * between which the zero field sensor offset will lie. The magnetometer zero field offset is
 * also independent of the smartphone orientation and therefore simply adds to the hard-iron
 * field. The calibration algorithm then adds the magnetometer zero field offset to the PCB
 * hard-iron interference and removes both together
 *
 * Several precautions should be observed when using magnetometers in general:
 * - The presence of ferrous materials, such as nickel, iron, steel, and cobalt near the
 * magnetometer will create disturbances in the earth’s magnetic field that will distort
 * the X, Y, and Z field measurements.
 * - The presence of the earth’s magnetic field must be taken into account when measuring other
 * magnetic fields.
 * - The variance of the earth’s magnetic field must be accounted for in different parts of the
 * world. Differences in the earth’s field are quite dramatic between N America, S America
 * and the Equator region.
 *
 * Non-ferromagnetic materials are all safe to use (e.g., aluminum, copper, brass, tin, silver,
 * and gold).
 *
 * - The Magnetometer calibration process enables the estimation and software subtraction of any
 * hard-iron field, but it’s good practice to minimize hard iron interference at the design stage.
 * Remember, a current trace will create a cylindrical magnetic field that falls off relatively
 * slowly with the inverse of distance, so place themagnetometer as far away from high current
 * traces as possible. A 0.1-A current trace at 10-mm distance will produce a 2-µT magnetic field,
 * four times our 0.5-uT error budget, only reducing to 0.5 uT at a 40-mm distance. Remember that
 * the earth's magnetic field at the surface is around 25 to 65 uTs.
 *
 *
 *
 */
const UINT16_t MAG3110_snsrMeasIntervalMilliSecs[ ] =
{
    13,   // 80 Hz  -> 12.5 milli-secs
    25,   // 40 Hz  -> 25 milli-secs
    50,   // 20 Hz  -> 50 milli-secs
    100,  // 10 Hz  -> 100 milli-secs

    25,   // 40 Hz  -> 25 milli-secs
    50,   // 20 Hz  -> 50 milli-secs
    100,  // 10 Hz  -> 100 milli-secs
    200,  //  5 Hz  -> 200 milli-secs

    50,   //  20 Hz  ->  50 milli-secs
    100,  //  10 Hz  -> 100 milli-secs
    200,  //   5 Hz  -> 200 milli-secs
    400,  // 2.5 Hz  -> 400 milli-secs

    100,  //   10 Hz  -> 100 milli-secs
    200,  //    5 Hz  -> 200 milli-secs
    400,  //  2.5 Hz  -> 400 milli-secs
    800,  // 1.25 Hz  -> 800 milli-secs

    200,  //     5 Hz  -> 200 milli-secs
    400,  //   2.5 Hz  -> 400 milli-secs
    800,  //  1.25 Hz  -> 800 milli-secs
   1600,  // 0.625 Hz  -> 1600 milli-secs

    400,  //    2.5 Hz  -> 400 milli-secs
    800,  //   1.25 Hz  -> 800 milli-secs
   1600,  //  0.625 Hz  -> 1600 milli-secs
   3200,  // 0.3125 Hz  ->  3200 milli-secs

    800,  //    1.25 Hz  -> 800 milli-secs
   1600,  //   0.625 Hz  -> 1600 milli-secs
   3200,  //  0.3125 Hz  -> 3200 milli-secs
   6400,  // 0.15625 Hz  -> 6400 milli-secs

   1600,  //    0.625 Hz  -> 1600 milli-secs
   3200,  //   0.3125 Hz  -> 3200 milli-secs
   6400,  //  0.15625 Hz  -> 6400 milli-secs
  12800,  // 0.078125 Hz  -> 12800 milli-secs
};


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
MAG3110_cntxt_s *MAG3110_getDevCntxt(UINT8_t devId)
{
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_LMP75B_DEV_CNT; idx++)
   {
      if (MAG3110_cntxtList[idx].devId == devId)
          return (&(MAG3110_cntxtList[idx]));
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
PLTFRM_sts_t MAG3110_writeReg(MAG3110_cntxt_s *devCntxt_p,
                              UINT8_t regAddr,
                              UINT8_t regVal)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t busId, i2cAddr;

   /*
    * To start a write command, the master transmits a start condition
    * (ST) to the MAG3110, followed by the slave address with the R/W
    * bit set to 0 for a write, and the MAG3110 sends an ack. Then
    * the master (or MCU) transmits the address of the register to write
    * to, and the MAG3110 sends an acknowledgement. Then the master (or
    * MCU) transmits the 8-bit data to write to the designated register
    * and the MAG3110 sends an acknowledgement that it has received the
    * data. Since this transmission is complete, the master transmits a
    * stop condition (SP) to end the data transfer. The data sent to the
    * MAG3110 is now stored in the appropriate register
    */

   if (devCntxt_p == NULL
       || regAddr > MAG3110_CTRL_REG2_ADDR)
   {
       return PLTFRM_STS_INV_PARAM;
   }

   busId = devCntxt_p->i2cBusId;
   i2cAddr = devCntxt_p->i2cDevAddr;

   I2C_SW_start(busId);

   if (I2C_SW_sendDevAddr_7(busId, i2cAddr, I2C_OPN_TYPE_WRITE) == 0x0)
       return PLTFRM_STS_I2C_BUS_WR_ERROR;

   if (I2C_SW_wrByte(busId, regAddr) == 0x0)
       return PLTFRM_STS_I2C_BUS_WR_ERROR;

   if (I2C_SW_wrByte(busId, regVal) == 0x0)
       sts = PLTFRM_STS_I2C_BUS_WR_ERROR;

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
PLTFRM_sts_t MAG3110_readReg(MAG3110_cntxt_s *devCntxt_p,
                             UINT8_t regAddr,
                             UINT8_t *regVal_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t busId, i2cAddr;

   if (devCntxt_p == NULL
       || regAddr > MAG3110_CTRL_REG2_ADDR
       || regVal_p == NULL)
   {
       return PLTFRM_STS_INV_PARAM;
   }

   busId = devCntxt_p->i2cBusId;
   i2cAddr = devCntxt_p->i2cDevAddr;

   /*
    * <Single byte read>
    *
    * The master (or MCU) transmits a start condition (ST) to the MAG3110,
    * followed by the slave address, with the R/W bit set to “0” for a write,
    * and the MAG3110 sends an acknowledgement. Then the master (or MCU)
    * transmits the address of the register to read and the MAG3110 sends an
    * acknowledgement. The master (or MCU) transmits a repeated start condition
    * (SR), followed by the slave address with the R/W bit set to “1” for a
    * read from the previously selected register. The MAG3110 then acknowledges
    * and transmits the data from the requested register. The master does not
    * acknowledge (NAK) the transmitted data, but transmits a stop condition to
    * end the data transfer.
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
 ********************************************************************
 */
static PLTFRM_sts_t __getDevOpnMode(MAG3110_cntxt_s *devCntxt_p,
                                     UINT8_t *opnMode_p)
{
   UINT8_t regVal;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   sts = MAG3110_readReg(devCntxt_p, MAG3110_SYSMOD_REG_ADDR, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_3302);
       return sts;
   }

   /*
    * System Mode. Default value: 00.
    * 00: STANDBY mode.
    * 01: ACTIVE mode, RAW data.
    * 10: ACTIVE mode, non-RAW user-corrected data
    */

   if (regVal & MAG3110_SYSMOD_REG_SYSMOD0_BIT)
       *opnMode_p = MAG3110_DEV_MODE_ACTIVE_RAW;
   else
   {
       if (regVal & MAG3110_SYSMOD_REG_SYSMOD1_BIT)
           *opnMode_p = MAG3110_DEV_MODE_ACTIVE_CORRECTED;
       else
           *opnMode_p = MAG3110_DEV_MODE_STANDBY;
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
PLTFRM_sts_t MAG3110_getDevOpnMode(UINT8_t devId, UINT8_t *opnMode_p)
{
   MAG3110_cntxt_s *devCntxt_p = MAG3110_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   return __getDevOpnMode(devCntxt_p, opnMode_p);
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
static PLTFRM_sts_t __devOpnModeIsStandby(MAG3110_cntxt_s *devCntxt_p)
{
   UINT8_t opnMode;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   sts = __getDevOpnMode(devCntxt_p, &opnMode);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       if (opnMode != MAG3110_DEV_MODE_STANDBY)
           sts = PLTFRM_STS_DEV_NOT_IN_SBY_MODE;
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
static PLTFRM_sts_t __setStandbyOpnMode(MAG3110_cntxt_s *devCntxt_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t regVal;

   /*
    * TM bit in the CNTRL_REG1
    * Operating mode selection. Default value: 0.
    *
    * 0: STANDBY mode.
    * 1: ACTIVE mode.
    *    ACTIVE mode will make periodic measurements based on values programmed
    *    in the Data Rate (DR) and Over Sampling Ratio bits (OS).
    */

   sts = MAG3110_readReg(devCntxt_p, MAG3110_CTRL_REG1_ADDR, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   regVal &= ~(MAG3110_CTRL_REG1_AC_FIELD_BIT_MSK
               | MAG3110_CTRL_REG1_TM_FIELD_BIT_MSK);
   sts = MAG3110_writeReg(devCntxt_p, MAG3110_CTRL_REG1_ADDR, regVal);

   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t MAG3110_setStandbyOpnMode(UINT8_t devId)
{
   MAG3110_cntxt_s *devCntxt_p = MAG3110_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   return __setStandbyOpnMode(devCntxt_p);
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t MAG3110_triggerMeas(UINT8_t devId, UINT16_t *tmoMilliSecs_p)
{
   MAG3110_cntxt_s *devCntxt_p = MAG3110_getDevCntxt(devId);
   UINT8_t regVal;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   /*
    * TM bit in the CNTRL_REG1
    * Trigger immediate measurement. Default value: 0
    *
    * 0: Normal operation based on AC condition.
    *    If part is in ACTIVE mode, any measurement in progress will continue with the
    *    highest ODR possible for the selected OSR.
    *
    * 1: Trigger measurement.
    *    In STANDBY mode triggered measurement will occur immediately and part will
    *    return to STANDBY mode as soon as the measurement is complete.
    */

   /*
    * Note: Except for STANDBY mode selection (Bit 0, AC), the device must be in
    * STANDBY mode to change any of the fields within CTRL_REG1 (0x10).
    */
   sts = __devOpnModeIsStandby(devCntxt_p);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = MAG3110_readReg(devCntxt_p, MAG3110_CTRL_REG1_ADDR, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   regVal |= MAG3110_CTRL_REG1_TM_FIELD_BIT_MSK;

   sts = MAG3110_writeReg(devCntxt_p, MAG3110_CTRL_REG1_ADDR, regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   if (tmoMilliSecs_p != NULL)
   {
       UINT8_t arrayIdx = devCntxt_p->adcRateEnc;
       arrayIdx <<= 2;
       arrayIdx |= devCntxt_p->osrEnc;
       *tmoMilliSecs_p = MAG3110_snsrMeasIntervalMilliSecs[arrayIdx];
   }

   return PLTFRM_STS_OPN_IN_PROGRESS;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
static PLTFRM_sts_t __setADCSamplingRate(MAG3110_cntxt_s *devCntxt_p,
                                         UINT8_t adcRateEnc)
{
   UINT8_t tempVal, regVal;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   if (adcRateEnc > MAG3110_ADC_RATE_0010_HZ_ENC)
       return PLTFRM_STS_INV_PARAM;

   sts = MAG3110_readReg(devCntxt_p, MAG3110_CTRL_REG1_ADDR, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   tempVal = adcRateEnc;
   tempVal <<= MAG3110_CTRL_REG1_DR_FIELD_BIT_SHIFT;

   regVal &= ~MAG3110_CTRL_REG1_DR_FIELD_BIT_MSK;
   regVal |= tempVal;

   sts = MAG3110_writeReg(devCntxt_p, MAG3110_CTRL_REG1_ADDR, regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = MAG3110_readReg(devCntxt_p, MAG3110_CTRL_REG1_ADDR, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;
   regVal &= MAG3110_CTRL_REG1_DR_FIELD_BIT_MSK;
   if (regVal != tempVal)
       sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;

   devCntxt_p->adcRateEnc = adcRateEnc;

   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t MAG3110_setADCSamplingRate(UINT8_t devId, UINT8_t adcRateEnc)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   MAG3110_cntxt_s *devCntxt_p = MAG3110_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   /*
    * Note: Except for STANDBY mode selection (Bit 0, AC), the device must be in
    * STANDBY mode to change any of the fields within CTRL_REG1 (0x10).
    */
   sts = __devOpnModeIsStandby(devCntxt_p);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   return __setADCSamplingRate(devCntxt_p, adcRateEnc);
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
static PLTFRM_sts_t __setDataOpMode(MAG3110_cntxt_s *devCntxt_p,
                                    UINT8_t dataMode)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t regVal;

   if (dataMode != MAG3110_OUTPUT_DATA_MODE_RAW
       && dataMode != MAG3110_OUTPUT_DATA_MODE_CORRECTED)
   {
       return PLTFRM_STS_INV_PARAM;
   }

   sts = MAG3110_readReg(devCntxt_p, MAG3110_CTRL_REG2_ADDR, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   if (dataMode == MAG3110_OUTPUT_DATA_MODE_RAW)
       regVal &= ~MAG3110_CTRL_REG2_RAW_FIELD_BIT_MSK;
   else
       regVal |= MAG3110_CTRL_REG2_RAW_FIELD_BIT_MSK;

   sts = MAG3110_writeReg(devCntxt_p, MAG3110_CTRL_REG2_ADDR, regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = MAG3110_readReg(devCntxt_p, MAG3110_CTRL_REG2_ADDR, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   regVal &= MAG3110_CTRL_REG2_RAW_FIELD_BIT_MSK;
   if ((regVal && dataMode == MAG3110_OUTPUT_DATA_MODE_RAW)
       || (regVal == 0 && dataMode == MAG3110_OUTPUT_DATA_MODE_CORRECTED))
   {
       sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;
   }

   devCntxt_p->dataMode = dataMode;

   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t MAG3110_setDataOpMode(UINT8_t devId, UINT8_t dataMode)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   MAG3110_cntxt_s *devCntxt_p = MAG3110_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   // Modification of CNTRL_REGx contents can only occur only when device is
   // “STANDBY” mode.
   sts = __devOpnModeIsStandby(devCntxt_p);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   return __setDataOpMode(devCntxt_p, dataMode);
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
static PLTFRM_sts_t __cfgAutoMagRstOption(MAG3110_cntxt_s *devCntxt_p,
                                          UINT8_t enaAutoMagRst)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t regVal;

   /*
    * Automatic Magnetic Sensor Reset. Default value: 0.
    *  0: Automatic magnetic sensor resets disabled.
    *  1: Automatic magnetic sensor resets enabled.
    * Similar to Mag_RST, however, the resets occur automatically before
    * each data acquisition. This bit is recommended to be always explicitly
    * enabled by the host application. This a WRITE ONLY bit and always reads
    * back as 0.
    */

   sts = MAG3110_readReg(devCntxt_p, MAG3110_CTRL_REG2_ADDR, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   if (enaAutoMagRst)
       regVal |= MAG3110_CTRL_REG2_AUTO_MRST_EN_FIELD_BIT_MSK;
   else
       regVal &= ~MAG3110_CTRL_REG2_AUTO_MRST_EN_FIELD_BIT_MSK;

   sts = MAG3110_writeReg(devCntxt_p, MAG3110_CTRL_REG2_ADDR, regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t MAG3110_cfgAutoMagRstOption(UINT8_t devId, UINT8_t enaAutoMagRst)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   MAG3110_cntxt_s *devCntxt_p = MAG3110_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   // Modification of CNTRL_REGx contents can only occur only when device is
   // “STANDBY” mode.
   sts = __devOpnModeIsStandby(devCntxt_p);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   return __cfgAutoMagRstOption(devCntxt_p, enaAutoMagRst);
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
static PLTFRM_sts_t __setOSR(MAG3110_cntxt_s *devCntxt_p,
                             UINT8_t overSampleRatio)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t tempVal, regVal;

   if (!(overSampleRatio == 16
         || overSampleRatio == 32
         || overSampleRatio == 64
         || overSampleRatio == 128))
   {
       return PLTFRM_STS_INV_PARAM;
   }

   sts = MAG3110_readReg(devCntxt_p, MAG3110_CTRL_REG1_ADDR, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   overSampleRatio >>= 4;
   for (tempVal=0; tempVal<4; tempVal++)
        if (overSampleRatio & (1 << tempVal))
            break;

   devCntxt_p->osrEnc = tempVal;

   tempVal <<= MAG3110_CTRL_REG1_OSR_FIELD_BIT_SHIFT;

   regVal &= ~MAG3110_CTRL_REG1_OSR_FIELD_BIT_MSK;
   regVal |= tempVal;

   sts = MAG3110_writeReg(devCntxt_p, MAG3110_CTRL_REG1_ADDR, regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = MAG3110_readReg(devCntxt_p, MAG3110_CTRL_REG1_ADDR, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   regVal &= MAG3110_CTRL_REG1_OSR_FIELD_BIT_MSK;
   if (regVal != tempVal)
       sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;

   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t MAG3110_setOSR(UINT8_t devId, UINT8_t overSampleRatio)
{
   MAG3110_cntxt_s *devCntxt_p = MAG3110_getDevCntxt(devId);
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;
  
   /*
    * Note: Except for STANDBY mode selection (Bit 0, AC), the device must be in 
    * STANDBY mode to change any of the fields within CTRL_REG1 (0x10).
    */
   sts = __devOpnModeIsStandby(devCntxt_p);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   return __setOSR(devCntxt_p, overSampleRatio);
}

/*
 * Full-scale range +/- 1000 uT
 * Sensitivity of 0.10 uT
 *
 * X-axis, Y-axis, and Z-axis 16-bit output sample data of the magnetic field
 * strength expressed as signed 2's complement numbers.
 *
 * The 2's complement user offset correction register values are used to compensate
 * for correcting the X, Y, and Z-axis after device board mount. These values may be
 * used to compensate for hard-iron interference and zero-flux offset of the sensor.
 *
 * Depending on the setting of the CTRL_REG2[RAW] bit, the magnetic field sample
 * data is corrected with the user offset values (CTRL_REG2[RAW] = 0), or can be
 * read out uncorrected for user offset values (CTRL_REG2[RAW] = 1)
 *
 * When RAW bit is set (CTRL_REG2[RAW] = 1), the output range is between -20,000
 * to 20,000 bit counts (the combination of the 1000 uT full scale range and the
 * zero-flux offset ranging up to 1000 uT). <Uncorrected output>
 *
 * When RAW bit is clear (CTRL_REG2[RAW] = 0), the output range is between
 * -30,000 to 30,000 bit counts when the user offset ranging between -10,000 to
 * 10,000 bit counts are included.  <Corrected output>
 */

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t MAG3110_getMeasData(UINT8_t devId,
                                 UINT8_t axisId,
                                 SINT16_t *out_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   MAG3110_cntxt_s *devCntxt_p = MAG3110_getDevCntxt(devId);
   UINT8_t regVal, regAddr, axisBitMsk;
   UINT16_t data;

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   if (axisId < MAG3110_X_AXIS_ID
       || axisId > MAG3110_Z_AXIS_ID
       || out_p == NULL)
   {
       return PLTFRM_STS_INV_PARAM;
   }

   // Check if new data is available for this axis.
   sts = MAG3110_readReg(devCntxt_p, MAG3110_DR_STATUS_REG_ADDR, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   axisBitMsk = (1 << (axisId - MAG3110_X_AXIS_ID));
   if ((regVal & axisBitMsk) == 0)
       return PLTFRM_STS_NO_NEW_MEAS_DATA;

   regAddr = MAG3110_OUT_X_MSB_REG_ADDR;
   regAddr += ((axisId  - MAG3110_X_AXIS_ID) * 2);

   // Read the MSB
   sts = MAG3110_readReg(devCntxt_p, regAddr, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   data = regVal;
   data <<= 8;

   // Read the LSB
   sts = MAG3110_readReg(devCntxt_p, regAddr + 1, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_3301);
       return sts;
   }

   data |= regVal;
   *out_p = (SINT16_t)data;

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
static PLTFRM_sts_t __getDieTemp(MAG3110_cntxt_s *devCntxt_p,
                                 SINT8_t *temp_p)
{
   UINT8_t regVal;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   /*
    * The register contains the die temperature in °C expressed as an 8-bit 2's
    * complement number. The sensitivity of the temperature sensor is factory
    * trimmed to 1°C/LSB. The temperature sensor offset is not factory trimmed
    * and must be calibrated by the user software if higher absolute accuracy is
    * required. Note: The register allows for temperature measurements from -128°C
    * to 127°C but the output range is limited to -40°C to 125°C.
    *
    * [****] The temperature data is updated on every measurement cycle.
    */

   sts = MAG3110_readReg(devCntxt_p, MAG3110_DIE_TEMP_REG_ADDR, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       *temp_p = (SINT8_t)regVal;
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
PLTFRM_sts_t MAG3110_getDieTemp(UINT8_t devId,
                                SINT8_t *temp_p)
{
   MAG3110_cntxt_s *devCntxt_p = MAG3110_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   return  __getDieTemp(devCntxt_p, temp_p);
}


#ifdef MAG3110_DBG_ENA
SINT8_t dbgDieTemp;
SINT16_t magX, magY, magZ;
UINT16_t dbgTmoVal;
UINT8_t _dbgDRStsRegVal = 0;
UINT32_t waitMeasLoopCnt = 0;
#endif


#ifdef MAG3110_IRQ_SUPPORT_ENA
UINT32_t MAG3110_intRcvdCnt = 0;

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void MAG3110_intCbFunc(UINT8_t devId)
{
   // Interrupt received when new measurement data is available.
   // Active high output. Interrupt is cleared when register 0x01
   // OUT_X_MSB is read.
   // The interrupt source has been masked at this point.

   MAG3110_intRcvdCnt ++;
}
#endif

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t MAG3110_init(UINT8_t devId)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   MAG3110_cntxt_s *devCntxt_p = MAG3110_getDevCntxt(devId);
   UINT8_t regVal, opMode, tempVal;

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   sts = PLTFRM_regI2CDev(devId, devCntxt_p->i2cBusId, devCntxt_p->i2cDevAddr);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = MAG3110_readReg(devCntxt_p, MAG3110_WHO_AM_I_REG_ADDR, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   if (regVal != MAG3110_DEVICE_ID_NUMBER)
       sts = PLTFRM_STS_DEV_ID_REG_VAL_MISMATCH;

   if (__getDevOpnMode(devCntxt_p, &opMode) == PLTFRM_STS_SUCCESS)
   {
       if (opMode != MAG3110_DEV_MODE_STANDBY)
       {
           sts = __setStandbyOpnMode(devCntxt_p);
           if (sts != PLTFRM_STS_SUCCESS)
               return sts;
       }
   }

   sts = MAG3110_readReg(devCntxt_p, MAG3110_CTRL_REG1_ADDR, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;
   tempVal = regVal;
   regVal &= MAG3110_CTRL_REG1_OSR_FIELD_BIT_MSK;
   regVal >>= MAG3110_CTRL_REG1_OSR_FIELD_BIT_SHIFT;
   devCntxt_p->osrEnc = regVal;

   regVal = tempVal;
   regVal &= MAG3110_CTRL_REG1_ADC_RATE_FIELD_BIT_MSK;
   regVal >>= MAG3110_CTRL_REG1_ADC_RATE_FIELD_BIT_SHIFT;
   devCntxt_p->adcRateEnc = regVal;

   sts = __setDataOpMode(devCntxt_p, MAG3110_OUTPUT_DATA_MODE_RAW);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = __setOSR(devCntxt_p, 128);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = __setADCSamplingRate(devCntxt_p, MAG3110_ADC_RATE_1280_HZ_ENC);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = __cfgAutoMagRstOption(devCntxt_p, 1);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

#ifdef MAG3110_IRQ_SUPPORT_ENA
   sts = PLTFRM_allocGPIOPin(devId,
                             devCntxt_p->portId,
                             devCntxt_p->portPinNr,
                             PLTFRM_GPIO_PIN_DIRECTION_INPUT,
                             PLTFRM_GPIO_OUTPUT_NA,
                             PLTFRM_GPIO_INT_EDGE_POSITIVE,
                             MAG3110_intCbFunc);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;
#endif

#ifdef MAG3110_DBG_ENA
   for (;;)
   {
      sts = __getDieTemp(devCntxt_p, &dbgDieTemp);
      if (sts != PLTFRM_STS_SUCCESS)
          break;

      _dbgDRStsRegVal = 0;
      sts = MAG3110_readReg(devCntxt_p, MAG3110_DR_STATUS_REG_ADDR, &_dbgDRStsRegVal);
      if (sts != PLTFRM_STS_SUCCESS)
          return sts;

      waitMeasLoopCnt = 0;

      sts = MAG3110_triggerMeas(devId, &dbgTmoVal);
      if (sts != PLTFRM_STS_OPN_IN_PROGRESS)
          break;

      while  (1)
      {
         _dbgDRStsRegVal = 0;
         sts = MAG3110_readReg(devCntxt_p, MAG3110_DR_STATUS_REG_ADDR, &_dbgDRStsRegVal);
         if (sts != PLTFRM_STS_SUCCESS)
             return sts;
         if (_dbgDRStsRegVal & 0x7 == 0x7)
             break;

         waitMeasLoopCnt ++;
      }

      sts = MAG3110_getMeasData(devId, MAG3110_X_AXIS_ID, &magX);
      if (sts != PLTFRM_STS_SUCCESS)
          break;

      sts = MAG3110_getMeasData(devId, MAG3110_Y_AXIS_ID, &magY);
      if (sts != PLTFRM_STS_SUCCESS)
          break;

      sts = MAG3110_getMeasData(devId, MAG3110_Z_AXIS_ID, &magZ);
      if (sts != PLTFRM_STS_SUCCESS)
          break;
   }
#endif

   return sts;
}


#endif
