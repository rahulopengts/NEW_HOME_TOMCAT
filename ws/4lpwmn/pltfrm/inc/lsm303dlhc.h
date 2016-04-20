/*
 * File Name : lsm303dlhc.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : jan/5/2015
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

#ifndef __LSM303DLHC_H__
#define __LSM303DLHC_H__

#include <typedefs.h>
#include <i2c_sw.h>

#define LSM303DLHC_I2C_ADDR 0x19    // 0 0011001  (7 bit address)


/*
 * LSM303DLHC register list
 */

#define LSM303DLHC_CTRL1_REG_ADDR           0x20
#define LSM303DLHC_CTRL2_REG_ADDR           0x21
#define LSM303DLHC_CTRL3_REG_ADDR           0x22
#define LSM303DLHC_CTRL4_REG_ADDR           0x23
#define LSM303DLHC_CTRL5_REG_ADDR           0x24
#define LSM303DLHC_CTRL6_REG_ADDR           0x25
#define LSM303DLHC_REF_A_REG_ADDR           0x26
#define LSM303DLHC_STS_REG_ADDR             0x27
#define LSM303DLHC_OUT_X_L_REG_ADDR         0x28
#define LSM303DLHC_OUT_X_H_REG_ADDR         0x29
#define LSM303DLHC_OUT_Y_L_REG_ADDR         0x2a
#define LSM303DLHC_OUT_Y_H_REG_ADDR         0x2b
#define LSM303DLHC_OUT_Z_L_REG_ADDR         0x2c
#define LSM303DLHC_OUT_Z_H_REG_ADDR         0x2d
#define LSM303DLHC_FIFO_CTRL_REG_ADDR       0x2e
#define LSM303DLHC_FIFO_SRC_REG_ADDR        0x2f
#define LSM303DLHC_INT1_CFG_REG_ADDR        0x30
#define LSM303DLHC_INT1_SRC_REG_ADDR        0x31
#define LSM303DLHC_INT1_THS_REG_ADDR        0x32
#define LSM303DLHC_INT1_DURATION_REG_ADDR   0x33
#define LSM303DLHC_INT2_CFG_REG_ADDR        0x34
#define LSM303DLHC_INT2_SRC_REG_ADDR        0x35
#define LSM303DLHC_INT2_THS_REG_ADDR        0x36
#define LSM303DLHC_INT2_DURATION_REG_ADDR   0x37
#define LSM303DLHC_CLICK_CFG_REG_ADDR       0x38
#define LSM303DLHC_CLICK_SRC_REG_ADDR       0x39
#define LSM303DLHC_CLICK_THS_REG_ADDR       0x3a
#define LSM303DLHC_TIME_LIMIT_REG_ADDR      0x3b
#define LSM303DLHC_TIME_LATENCY_REG_ADDR    0x3c
#define LSM303DLHC_TIME_WINDOW_REG_ADDR     0x3d


typedef enum
{
   LSM303DLHC_ACCEL_OPN_MODE_POWER_DOWN,
   LSM303DLHC_ACCEL_OPN_MODE_LPM,
   LSM303DLHC_ACCEL_OPN_MODE_NORMAL
} LSM303DLHC_accelOpnMode_t;


#endif

