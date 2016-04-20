/*
 * File Name : lis3mdltr.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : Jan/7/2015
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

#ifndef __LIS3MDLTR_H__
#define __LIS3MDLTR_H__

#include <typedefs.h>
#include <pltfrm.h>
#include <i2c_sw.h>

/*
 * Datasheet:
 * http://www.mouser.com/ds/2/389/DM00075867-490662.pdf
 */

// 8 bit registers
#define LIS3MDLTR_WHO_AM_I_REG_ADDR   0x0f
#define LIS3MDLTR_CTRL_REG1_ADDR      0x20
#define LIS3MDLTR_CTRL_REG2_ADDR      0x21
#define LIS3MDLTR_CTRL_REG3_ADDR      0x22
#define LIS3MDLTR_CTRL_REG4_ADDR      0x23
#define LIS3MDLTR_CTRL_REG5_ADDR      0x24
#define LIS3MDLTR_STATUS_REG1_ADDR    0x27
#define LIS3MDLTR_OUT_X_L_REG_ADDR    0x28
#define LIS3MDLTR_OUT_X_H_REG_ADDR    0x29
#define LIS3MDLTR_OUT_Y_L_REG_ADDR    0x2a
#define LIS3MDLTR_OUT_Y_H_REG_ADDR    0x2b
#define LIS3MDLTR_OUT_Z_L_REG_ADDR    0x2c
#define LIS3MDLTR_OUT_Z_H_REG_ADDR    0x2d
#define LIS3MDLTR_STATUS_REG2_ADDR    0x30
#define LIS3MDLTR_INT_SRC_REG_ADDR    0x31
#define LIS3MDLTR_INT_THS_L_REG_ADDR  0x32
#define LIS3MDLTR_INT_THS_H_REG_ADDR  0x33


#define LIS3MDLTR_CTRL_REG3_MDX_BIT_MSK  0x3

#define LIS3MDLTR_STATUS_1_REG_ZYXOR_BIT BIT7
#define LIS3MDLTR_STATUS_1_REG_ZOR_BIT   BIT6
#define LIS3MDLTR_STATUS_1_REG_YOR_BIT   BIT5
#define LIS3MDLTR_STATUS_1_REG_XOR_BIT   BIT4
#define LIS3MDLTR_STATUS_1_REG_ZYXDA_BIT BIT3
#define LIS3MDLTR_STATUS_1_REG_ZDA_BIT   BIT2
#define LIS3MDLTR_STATUS_1_REG_YDA_BIT   BIT1
#define LIS3MDLTR_STATUS_1_REG_XDA_BIT   BIT0



#define LIS3MDLTR_WHO_AM_I_REG_VALUE  0x3d   // 00111101


typedef enum
{
   LIS3MDLTR_X_AXIS = 0,
   LIS3MDLTR_Y_AXIS,
   LIS3MDLTR_Z_AXIS
} LIS3MDLTR_axis_t;


typedef enum
{
   LIS3MDLTR_SYS_OPN_MODE_CONT_CONV = 0,
   LIS3MDLTR_SYS_OPN_MODE_SINGLE_CONV = 1,
   LIS3MDLTR_SYS_OPN_MODE_POWERED_DOWN = 2
} LIS3MDLTR_sysOpnMode_t;

typedef enum
{
   LIS3MDLTR_FULL_SCALE_SEL_4_GAUSS = 0,  // +/- 4 Gauss
   LIS3MDLTR_FULL_SCALE_SEL_8_GAUSS,  // +/- 8 Gauss
   LIS3MDLTR_FULL_SCALE_SEL_12_GAUSS,  // +/- 12 Gauss
   LIS3MDLTR_FULL_SCALE_SEL_16_GAUSS,  // +/- 16 Gauss
} LIS3MDLTR_fullScaleSel_t;

typedef enum
{
   LIS3MDLTR_OP_DATA_RATE_PT_625_HZ = 0,
   LIS3MDLTR_OP_DATA_RATE_1_PT_25_HZ ,
   LIS3MDLTR_OP_DATA_RATE_2_PT_5_HZ,
   LIS3MDLTR_OP_DATA_RATE_5_HZ,
   LIS3MDLTR_OP_DATA_RATE_10_HZ,
   LIS3MDLTR_OP_DATA_RATE_20_HZ,
   LIS3MDLTR_OP_DATA_RATE_40_HZ,
   LIS3MDLTR_OP_DATA_RATE_80_HZ
} LIS3MDLTR_outputDataRate_t;

typedef enum
{
   LIS3MDLTR_AXIS_OPN_MODE_LOW_POWER = 0,
   LIS3MDLTR_AXIS_OPN_MODE_MEDIUM_PERFORMANCE,
   LIS3MDLTR_AXIS_OPN_MODE_HIGH_PERFORMANCE,
   LIS3MDLTR_AXIS_OPN_MODE_ULTRA_HIGH_PERFORMANCE
} LIS3MDLTR_axisOpnMode_t;

typedef struct
{
   UINT8_t devId;
   PLTFRM_snsrDevInfo_s info;
} LIS3MDLTR_cntxt_s;


extern PLTFRM_sts_t LIS3MDLTR_init(UINT8_t devId);

extern PLTFRM_sts_t LIS3MDLTR_readAxisOp(UINT8_t devId,
                                         LIS3MDLTR_axis_t axis,
                                         SINT16_t *opVal_p);

#endif
