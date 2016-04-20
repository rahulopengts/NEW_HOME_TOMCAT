/*
 * File Name : mpu6050.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : Jan/10/2014
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

#ifndef __MPU6050_H__
#define __MPU6050_H__

#include <typedefs.h>
#include <i2c_sw.h>


/*
 * I2C Address is 110100X where X determined by the logic on pin AD0.
 */
#define MPU6050_I2C_ADDR_PREFIX  0x68    // 0 1101 00X  (7 bit address)


#define MPU6050_SELF_TEST_X_REG_ADDR   0xd
#define MPU6050_SELF_TEST_Y_REG_ADDR   0xe
#define MPU6050_SELF_TEST_Z_REG_ADDR   0xf
#define MPU6050_SELF_TEST_A_REG_ADDR   0x10

/*
 * This register specifies the divider from the gyroscope output
 * rate used to generate the Sample Rate for the MPU-60X0. The
 * sensor register output, FIFO output, and DMP sampling are all
 * based on the Sample Rate.
 * Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV)
 */
#define MPU6050_SMPLRT_DIV_REG_ADDR    0x19

/*
 * This register configures the external Frame Synchronization (FSYNC) pin
 * sampling and the Digital Low Pass Filter (DLPF) setting for both the
 * gyroscopes and accelerometers.
 */
#define MPU6050_CONFIG_REG_ADDR         0x1a

/*
 * This register is used to trigger accelerometer self test and configure
 * the accelerometer full scale range. This register also configures the
 * Digital High Pass Filter (DHPF).
 */
#define MPU6050_ACCEL_CONFIG_REG_ADDR   0x1c

/*
 * This register determines which sensor measurements are loaded into the
 * FIFO buffer.
 */
#define MPU6050_FIFO_EN_REG_ADDR          0x23

#define MPU6050_I2C_MST_CTRL_REG_ADDR     0x24

#define MPU6050_I2C_PIN_CFG_REG_ADDR      0x37

#define MPU6050_I2C_ENABLE_REG_ADDR       0x38

#define MPU6050_I2C_INT_STATUS_REG_ADDR   0x39

/*
 * Accelerometer Measurements (X, Y and Z)
 * These registers store the most recent accelerometer measurements.
 * Accelerometer measurements are written to these registers at the Sample Rate
 * as defined in Register 25.
 */
#define MPU6050_ACCEL_XOUT_H_REG_ADDR     0x3b

#define MPU6050_ACCEL_XOUT_L_REG_ADDR     0x3c

#define MPU6050_ACCEL_YOUT_H_REG_ADDR     0x3d

#define MPU6050_ACCEL_YOUT_L_REG_ADDR     0x3e

#define MPU6050_ACCEL_ZOUT_H_REG_ADDR     0x3f

#define MPU6050_ACCEL_ZOUT_L_REG_ADDR     0x40

/*
 * These registers store the most recent temperature sensor measurement.
 * Temperature measurements are written to these registers at the Sample Rate
 * as defined in Register 25.
 */
#define MPU6050_TEMP_OUT_H_REG_ADDR       0x41

#define MPU6050_TEMP_OUT_L_REG_ADDR       0x42

#define MPU6050_USER_CTRL_REG_ADDR        0x6a

#define MPU6050_PWR_MGMT_1_REG_ADDR       0x6b

#define MPU6050_PWR_MGMT_2_REG_ADDR       0x6c

#define MPU6050_FIFO_COUNT_H_REG_ADDR     0x72

#define MPU6050_FIFO_COUNT_L_REG_ADDR     0x73

#define MPU6050_FIFO_R_W_REG_ADDR         0x74

/*
 * This register is used to verify the identity of the device. The contents of
 * WHO_AM_I are the upper 6 bits of the MPU-60X0’s 7-bit I2C address. The least
 * significant bit of the MPU-60X0’s I2C address is determined by the value of
 * the AD0 pin. The value of the AD0 pin is not reflected in this register.
 * The default value of the register is 0x68. Bits 0 and 7 are reserved. (Hard
 * coded to 0)
 */
#define MPU6050_WHO_AM_I_REG_ADDR         0x75
#define MPU6050_WHO_AM_I_REG_VAL          0x68


// Power Management 1 Register
#define MPU6050_PWR_MGMT_REG_1_SLEEP_BM     (BIT6)
#define MPU6050_PWR_MGMT_REG_1_CYCLE_BM     (BIT5)
#define MPU6050_PWR_MGMT_REG_1_TEMP_DIS_BM  (BIT3)
#define MPU6050_PWR_MGMT_REG_1_CLKSEL_BM    (BIT0 | BIT1 | BIT2)


// Acceleration config register
#define MPU6050_ACCEL_CFG_REG_AFS_SEL_BM   (BIT3 | BIT4)

#define MPU6050_ACCEL_ABS_FSR_2G    0x0   // +/- 2g
#define MPU6050_ACCEL_ABS_FSR_4G    0x1   // +/- 4g
#define MPU6050_ACCEL_ABS_FSR_8G    0x2   // +/- 8g
#define MPU6050_ACCEL_ABS_FSR_16G   0x3   // +/- 16g

typedef struct
{
   UINT8_t devId;
} MPU6050_cntxt_s;


extern PLTFRM_sts_t MPU6050_init(UINT8_t devId);

#endif
