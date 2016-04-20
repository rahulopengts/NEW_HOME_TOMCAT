/*
 * File Name : i2c_sw.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : 9/5/2013
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

#ifndef __I2C_SW_H__
#define __I2C_SW_H__

#include <typedefs.h>
#include <pltfrm.h>

#define I2C_OPN_TYPE_READ   0x1
#define I2C_OPN_TYPE_WRITE  0x2

#define I2C_ACK      0x1
#define I2C_NO_ACK   0x2

typedef struct
{
   volatile UINT8_t *dirReg_p;
   volatile UINT8_t *outReg_p;
   volatile UINT8_t *inReg_p;
   UINT8_t sclBitNr; 
   UINT8_t sdaBitNr;
} I2C_SW_devInfo_s;


extern UINT8_t I2C_SW_start(UINT8_t busId);

extern UINT8_t I2C_SW_stop(UINT8_t busId);

extern UINT8_t I2C_SW_sendDevAddr_7(UINT8_t busId, UINT8_t devAddr, UINT8_t opn);

extern UINT8_t I2C_SW_wrByte(UINT8_t busId, UINT8_t opn);

extern UINT8_t I2C_SW_rdByte(UINT8_t busId, UINT8_t *buff_p, UINT8_t ackCntrl); 

extern UINT8_t I2C_SW_init(UINT8_t busId);

// rkris@wisense.in - Jan/7/15 - Added these three to support Sensiron SHT-10
extern PLTFRM_sts_t I2C_SW_readDataLine(UINT8_t busId, UINT8_t *sigVal_p);
extern PLTFRM_sts_t I2C_SW_clkLowDataLow(UINT8_t busId);
extern PLTFRM_sts_t I2C_SW_clkHiDataHi(UINT8_t busId);

#endif
