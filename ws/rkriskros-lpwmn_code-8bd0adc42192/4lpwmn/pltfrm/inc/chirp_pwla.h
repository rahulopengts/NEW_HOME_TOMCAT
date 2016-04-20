/*
 * File Name : lm75b.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : 10/23/2013
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

#ifndef __CHIRP_WLA_H__
#define __CHIRP_WLA_H__

#include <typedefs.h>
#include <i2c_sw.h>

#define CHIRP_PWLA_I2C_ADDR  0x20    // 0010 0000 ( 7 bit address)

#define CHIRP_PWLA_RESET_TIME_MILLISECS  30
#define CHIRP_PWLA_RESET_HOLD_TIME_MILLISECS  2000

#define CHIRP_PWLA_CMD_MOISTURE_MEAS  0x0
#define CHIRP_PWLA_CMD_SET_I2C_ADDR  0x1
#define CHIRP_PWLA_CMD_GET_I2C_ADDR  0x2
#define CHIRP_PWLA_CMD_START_LIGHT_MEAS  0x3
#define CHIRP_PWLA_CMD_GET_LIGHT_OUTPUT  0x4

#define PLTFRM_CHIRP_PWLA_MOISTURE_LEVEL_OUTPUT_LEN  2


typedef struct
{
   UINT8_t devId;
   UINT8_t i2cBusId;
   UINT8_t i2cAddr;
   PLTFRM_gpioPortId_t resetPort;
   PLTFRM_gpioPortPin_t resetPin;
   PLTFRM_snsrDevInfo_s info;
} CHIRP_PWLA_cntxt_s;

extern PLTFRM_sts_t CHIRP_PWLA_getMoistureLevel(UINT16_t devId, UINT16_t *opVal_p);

#endif
