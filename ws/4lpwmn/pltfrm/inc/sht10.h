/*
 * File Name : sht10.h
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

#ifndef __SHT10_H__
#define __SHT10_H__

#include <typedefs.h>
#include <pltfrm.h>
#include <i2c_sw.h>


#define SHT10_TEMP_OUTPUT_LEN  2
#define SHT10_RH_OUTPUT_LEN  2

/*
 * Datasheet:
 * http://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/Humidity/Sensirion_Humidity_SHT1x_Datasheet_V5.pdf
 */

/*
 * After power-up the sensor needs 11ms to get to Sleep State. No commands must be sent before
 * that time. 
 */


#define SHT10_TEMP_MEAS_ADDR      0x01   // 0 000 0001
#define SHT10_RH_MEAS_ADDR        0x02   // 0 000 0010
#define SHT10_STATUS_REG_RW_ADDR  0x03   // 0 000 0011

#define SHT10_STS_REG_RES_BIT_MSK  (1 << 0)
#define SHT10_STS_REG_RES_BIT_SHIFT (0)

#define SHT10_STS_REG_HEATER_BIT_MSK  (1 << 2)
#define SHT10_STS_REG_HEATER_BIT_SHIFT (2)

#define SHT10_STS_REG_EOBI_BIT_MSK  (1 << 6)
#define SHT10_STS_REG_EOBI_BIT_SHIFT (6)

#define SHT10_STS_REG_NRFO_BIT_MSK  (1 << 1)
#define SHT10_STS_REG_NRFO_BIT_SHIFT  (1)


#define SHT10_8_BIT_MEAS_TIME_MILLISECS  20
#define SHT10_12_BIT_MEAS_TIME_MILLISECS  80
#define SHT10_14_BIT_MEAS_TIME_MILLISECS  320

typedef enum
{
   SHT10_OP_RES_LOW,
   SHT10_OP_RES_HIGH,
   SHT10_OP_DFLT_RES = SHT10_OP_RES_HIGH
} SHT10_opRes_t;

typedef struct
{
   UINT8_t devId;
   UINT8_t res;  // High / low
   PLTFRM_snsrDevInfo_s info;
} SHT10_cntxt_s;



extern PLTFRM_sts_t SHT10_init(UINT8_t devId);
extern PLTFRM_sts_t SHT10_readMeasRHVal(UINT8_t devId, UINT16_t *snsrRawOp_p);
extern PLTFRM_sts_t SHT10_readMeasTempVal(UINT8_t devId, SINT16_t *tempVal_p);
extern PLTFRM_sts_t SHT10_startRHMeas(UINT8_t devId, UINT16_t *tmoVal_p);
extern PLTFRM_sts_t SHT10_startTempMeas(UINT8_t devId, UINT16_t *tmoVal_p);

#endif

