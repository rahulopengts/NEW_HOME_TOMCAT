/*
 * File Name: mpu6050.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Jan/10/2014
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

#ifdef PLTFRM_MPU6050_SENSOR_ENA


#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <i2c_sw.h>
#include <mpu6050.h>


MPU6050_cntxt_s MPU6050_cntxtList[PLTFRM_MPU6050_DEV_CNT] =
{
   {PLTFRM_MPU6050_1_DEV_ID}
};


#define MPU6050_UT

#ifdef MPU6050_UT
SINT32_t MPU6050_tempVal = 0;
UINT8_t MPU6050_pwrMgmt1Reg = 0;
UINT8_t MPU6050_pwrMgmt2Reg = 0;
UINT8_t MPU6050_usrCtrlReg = 0;
UINT8_t MPU6050_configReg = 0;
UINT16_t MPU6050_loopCount = 0;
SINT16_t MPU6050_xData, MPU6050_yData, MPU6050_zData;
UINT8_t MPU6050_accelConfigReg;
#endif



/*
 * <Accelerometers>
 * The MPU-60X0 features three 16-bit analog-to-digital converters (ADCs) for digitizing
 * the the accelerometer outputs. For precision tracking of both fast and slow motions, the
 * parts feature a user-programmable accelerometer full-scale range of +/- 2g, +/- 4g, +/- 8g
 * and +/- 16g.
 *
 * <Accelerometer Features (5.2)>
 * The triple-axis MEMS accelerometer in MPU-60X0 includes a wide range of features:
 *  > Digital-output triple-axis accelerometer with a programmable full scale range of ±2g, ±4g, ±8g and ±16g
 *  > Integrated 16-bit ADCs enable simultaneous sampling of accelerometers while requiring no external multiplexer
 *  > Accelerometer normal operating current: 500 uA
 *  > Low power accelerometer mode current: 10uA at 1.25Hz, 20uA at 5Hz, 60uA at 20Hz, 110uA at 40Hz
 *  > Orientation detection and signaling
 *  > Tap detection
 *  > User-programmable interrupts
 *  > High-G interrupt
 *  > User self-test
 *
 * <Accelerometer output rate>
 * Programmable range (4 Hz to 1000 Hz)
 * Sample Rate = Gyro output rate / (1 + SMPRT)
 * where SMPRT is the value of the 8 bit SMPRT_DIV register.
 * Gyro output rate is 8 kHZ when DLPF is disabled and is 1 kHZ
 * when DLPF is enabled.
 * At 1 kHZ Gyro o/p and SMPRT value of 0, Sample Rate = 1000/(1) => 1000
 * At 1 kHZ Gyro o/p and SMPRT value of 255, Sample Rate = 1000/(1 + 255) => 4
 * The max accelerometer output rate is 1kHz. This means that for a Sample Rate greater than
 * 1kHz, the same accelerometer sample may be output to the FIFO, DMP, and sensor registers
 * more than once.
 *
 * <Accelerometer Low Power Mode Current>
 *  > 1.25 Hz update rate   (10 uA)
 *  > 5 Hz update rate      (20 uA)
 *  > 20 Hz update rate     (70 uA)
 *  > 40 Hz update rate     (140 uA)
 *
 * <FIFO>
 * An on-chip 1024 Byte FIFO buffer helps lower system power consumption by allowing the system
 * processor to read the sensor data in bursts and then enter a low-power mode as the MPU collects
 * more data.
 *
 * <I2C>
 * Communication with all registers of the device is performed using either I2C at 400kHz.
 * I2C Address is 110100X where X determined by the logic on pin AD0.
 *
 * <Additional features>
 * Additional features include an embedded temperature sensor and an on-chip oscillator with +/- 1%
 * variation over the operating temperature range.
 *
 * The part features a robust 10,000g shock tolerance, and has programmable low-pass filters for the
 * gyroscopes, accelerometers, and the on-chip temperature sensor.
 *
 * <Power supply>
 * For power supply flexibility, the MPU-60X0 operates from VDD power supply voltage range of
 * 2.375V-3.46V. Additionally, the MPU-6050 provides a VLOGIC reference pin (in addition to its analog
 * supply pin: VDD), which sets the logic levels of its I2C interface. The VLOGIC voltage may be
 * 1.8V +/- 5% or VDD.
 *
 * <Digital filter>
 * User-programmable digital filters for gyroscope, accelerometer, and temp sensor.
 *
 * <Clocking (5.5)>
 *  > On-chip timing generator ±1% frequency variation over full temperature range
 *  > Optional external clock inputs of 32.768kHz or 19.2MHz
 *
 * <Interrupts (7.18)>
 * One of the configurable sources of interrupts is accelerometer event interrupts.
 *
 */


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
MPU6050_cntxt_s *MPU6050_getDevCntxt(UINT8_t devId)
{
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_MPU6050_DEV_CNT; idx++)
   {
      if (MPU6050_cntxtList[idx].devId == devId)
          return (&(MPU6050_cntxtList[idx]));
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
PLTFRM_sts_t MPU6050_readReg(MPU6050_cntxt_s *devCntxt_p,
                             UINT8_t regAddr,
                             UINT8_t *rdBuff_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t busId, i2cAddr = MPU6050_I2C_ADDR_PREFIX;

   if (regAddr < MPU6050_SELF_TEST_X_REG_ADDR
       || regAddr > MPU6050_WHO_AM_I_REG_ADDR)
   {
       return PLTFRM_STS_INV_PARAM;
   }

   /*
    * Read any register
    *
    * > START (S)                                   (from micro)
    * > 7 bit device address | Write bit (AD+W)     (from micro)
    * > ACK                                         (from MPU6050)
    * > 8 bit register address (RA)                 (from micro)
    * > ACK                                         (from MPU6050)
    * > RE-START (S)                                (from micro)
    * > 7 bit device address | Read bit (AD+R)      (from micro)
    * > ACK                                         (from MPU6050)
    * > Read Byte (DATA)                            (from MPU6050)
    * > No Ack bit (SDA remains high) (NACK)
    * > STOP (P)                                    (from micro)
    */

   switch (devCntxt_p->devId)
   {
      case PLTFRM_MPU6050_1_DEV_ID:
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           i2cAddr |= PLTFRM_MPU6050_1_I2C_ADDR;
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

   // ACK -->
   I2C_SW_rdByte(busId, rdBuff_p, I2C_ACK);
   rdBuff_p ++;

   // <--- DATA
   I2C_SW_rdByte(busId, rdBuff_p, I2C_NO_ACK);

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
PLTFRM_sts_t MPU6050_writeReg(MPU6050_cntxt_s *devCntxt_p,
                              UINT8_t regAddr,
                              UINT8_t dataByte)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t busId, i2cAddr = MPU6050_I2C_ADDR_PREFIX;

   if (regAddr < MPU6050_SELF_TEST_X_REG_ADDR
       || regAddr > MPU6050_WHO_AM_I_REG_ADDR)
   {
       return PLTFRM_STS_INV_PARAM;
   }

   /*
    * Write single byte to any register
    *
    * > START (S)                                   (from micro)
    * > 7 bit device address | Write bit (AD+W)     (from micro)
    * > ACK                                         (from MPU6050)
    * > 8 bit register address (RA)                 (from micro)
    * > ACK                                         (from MPU6050)
    * > Write Byte (DATA)                           (from micro)
    * > ACK                                         (from MPU6050)
    * > STOP (P)                                    (from micro)
    */

   switch (devCntxt_p->devId)
   {
      case PLTFRM_MPU6050_1_DEV_ID:
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           i2cAddr |= PLTFRM_MPU6050_1_I2C_ADDR;
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

   // DATA --->
   if (I2C_SW_wrByte(busId, dataByte) == 0x0)
   {
       I2C_SW_stop(busId);
       return PLTFRM_STS_I2C_BUS_WR_ERROR;
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
PLTFRM_sts_t MPU6050_getTempData(UINT8_t devId, SINT32_t *tempData_p)
{
   MPU6050_cntxt_s *devCntxt_p = MPU6050_getDevCntxt(devId);
   UINT8_t regVal8;
   SINT16_t tempS16;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   sts = MPU6050_readReg(devCntxt_p, MPU6050_TEMP_OUT_H_REG_ADDR, &regVal8);
   if (sts != PLTFRM_STS_SUCCESS)
       return PLTFRM_STS_I2C_BUS_RD_ERROR;
   tempS16 = regVal8;
   tempS16 <<= 8;

   sts = MPU6050_readReg(devCntxt_p, MPU6050_TEMP_OUT_L_REG_ADDR, &regVal8);
   if (sts != PLTFRM_STS_SUCCESS)
       return PLTFRM_STS_I2C_BUS_RD_ERROR;
   tempS16 |= regVal8;

   /*
    * The temperature in degrees C for a given register value may be computed as:
    * Temperature in degrees is
    *        C = (TEMP_OUT Register Value as a signed quantity)/340 + 36.53
    */

   *tempData_p = tempS16;
   (*(tempData_p)) *= 100;
   (*(tempData_p)) /= 340;
   (*(tempData_p)) += 3653;

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
PLTFRM_sts_t MPU6050_getAccelData(UINT8_t devId, axis_t axis, SINT16_t *accelData_p)
{
   MPU6050_cntxt_s *devCntxt_p = MPU6050_getDevCntxt(devId);
   UINT8_t regVal8, regAddr;
   SINT16_t accelData;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;


   regAddr = (axis == X_AXIS) ? MPU6050_ACCEL_XOUT_H_REG_ADDR : \
              ((axis == Y_AXIS) ? MPU6050_ACCEL_YOUT_H_REG_ADDR : MPU6050_ACCEL_ZOUT_H_REG_ADDR);

   /*
    * Digital-output triple-axis accelerometer with a programmable full
    * scale range of ±2g, ±4g, ±8g and ±16g.
    */

   sts = MPU6050_readReg(devCntxt_p, regAddr, &regVal8);
   if (sts != PLTFRM_STS_SUCCESS)
       return PLTFRM_STS_I2C_BUS_RD_ERROR;
   accelData = regVal8;
   accelData <<= 8;

   sts = MPU6050_readReg(devCntxt_p, regAddr + 1, &regVal8);
   if (sts != PLTFRM_STS_SUCCESS)
       return PLTFRM_STS_I2C_BUS_RD_ERROR;
   accelData |= regVal8;

   *accelData_p = accelData;

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
PLTFRM_sts_t MPU6050_init(UINT8_t devId)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   MPU6050_cntxt_s *devCntxt_p = MPU6050_getDevCntxt(devId);
   UINT8_t regVal8;

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   // Read the "who-am-i" register first

   sts = MPU6050_readReg(devCntxt_p, MPU6050_WHO_AM_I_REG_ADDR, &regVal8);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   if (regVal8 != MPU6050_WHO_AM_I_REG_VAL)
   {
       return PLTFRM_STS_DEV_MISSING;
   }


#ifdef MPU6050_UT

   sts = MPU6050_readReg(devCntxt_p, MPU6050_PWR_MGMT_1_REG_ADDR, &MPU6050_pwrMgmt1Reg);
   if (sts != PLTFRM_STS_SUCCESS)
       return PLTFRM_STS_I2C_BUS_RD_ERROR;

   if (MPU6050_pwrMgmt1Reg & MPU6050_PWR_MGMT_REG_1_SLEEP_BM)
   {
       /*
        *  When set to 1, this bit puts the MPU-60X0 into sleep mode.
        */
       MPU6050_pwrMgmt1Reg &= ~(MPU6050_PWR_MGMT_REG_1_SLEEP_BM);
   }

   if (MPU6050_pwrMgmt1Reg & MPU6050_PWR_MGMT_REG_1_TEMP_DIS_BM)
   {
       /*
        *  When set to 1, this bit disables the temperature sensor.
        */
       MPU6050_pwrMgmt1Reg &= ~(MPU6050_PWR_MGMT_REG_1_TEMP_DIS_BM);
   }

   sts = MPU6050_writeReg(devCntxt_p, MPU6050_PWR_MGMT_1_REG_ADDR, MPU6050_pwrMgmt1Reg);
   if (sts != PLTFRM_STS_SUCCESS)
       return PLTFRM_STS_I2C_BUS_WR_ERROR;

   PLTFRM_delay1000MicroSecs();

   sts = MPU6050_readReg(devCntxt_p, MPU6050_PWR_MGMT_1_REG_ADDR, &MPU6050_pwrMgmt1Reg);
   if (sts != PLTFRM_STS_SUCCESS)
       return PLTFRM_STS_I2C_BUS_RD_ERROR;

   sts = MPU6050_readReg(devCntxt_p, MPU6050_PWR_MGMT_2_REG_ADDR, &MPU6050_pwrMgmt2Reg);
   if (sts != PLTFRM_STS_SUCCESS)
       return PLTFRM_STS_I2C_BUS_RD_ERROR;

   sts = MPU6050_readReg(devCntxt_p, MPU6050_USER_CTRL_REG_ADDR, &MPU6050_usrCtrlReg);
   if (sts != PLTFRM_STS_SUCCESS)
       return PLTFRM_STS_I2C_BUS_RD_ERROR;

   sts = MPU6050_readReg(devCntxt_p, MPU6050_CONFIG_REG_ADDR, &MPU6050_configReg);
   if (sts != PLTFRM_STS_SUCCESS)
       return PLTFRM_STS_I2C_BUS_RD_ERROR;

   sts = MPU6050_readReg(devCntxt_p, MPU6050_ACCEL_CONFIG_REG_ADDR, &MPU6050_accelConfigReg);
   if (sts != PLTFRM_STS_SUCCESS)
       return PLTFRM_STS_I2C_BUS_RD_ERROR;

   for (;;)
   {
       if (MPU6050_getAccelData(devId, X_AXIS, &MPU6050_xData) != PLTFRM_STS_SUCCESS)
           break;

       if (MPU6050_getAccelData(devId, Y_AXIS, &MPU6050_yData) != PLTFRM_STS_SUCCESS)
           break;

       if (MPU6050_getAccelData(devId, Z_AXIS, &MPU6050_zData) != PLTFRM_STS_SUCCESS)
           break;

       MPU6050_loopCount ++;
   }

   for (;;)
   {
       if ((sts = MPU6050_getTempData(devId, &MPU6050_tempVal)) != PLTFRM_STS_SUCCESS)
           break;
   }
#endif

   return sts;
}


#endif
