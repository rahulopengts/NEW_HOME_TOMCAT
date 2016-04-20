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

#ifndef __LM75B_H__
#define __LM75B_H__

#include <typedefs.h>
#include <i2c_sw.h>

#define LM75B_SENSOR_OUTPUT_VALUE_SZ  2


/*
 * The LM75B slave address on the I2C-bus is partially defined
 * by the logic applied to the device address pins A2, A1 and A0.
 * Each of them is typically connected either to GND for logic 0,
 * or to VCC for logic 1. These pins represent the three LSB bits
 * of the device 7-bit address. The other four MSB bits of the
 * address data are preset to 1001 by hard wiring inside the LM75B.
 */
#define LM75B_I2C_ADDR_PREFIX  0x48    // 0 1001 xxx  (7 bit address)

/*
 * LM75B register list
 */
#define LM75B_TEMP_REG_ADDR   0x0   // RO
#define LM75B_CONF_REG_ADDR   0x1   // R/W
#define LM75B_THYST_REG_ADDR  0x2   // R/W
#define LM75B_TOS_REG_ADDR    0x3   // R/W

#define LM75B_CONF_REG_SZ     0x1
#define LM75B_TEMP_REG_SZ     0x2
#define LM75B_THYST_REG_SZ    0x2
#define LM75B_TOS_REG_SZ      0x2

/*
 * 7.4.2 Configuration register
 */
#define LM75B_CFG_REG_OPN_MODE_BM          (1 << 0)
#define LM75B_CFG_REG_OS_OPN_MODE_BM       (1 << 1)
#define LM75B_CFG_REG_POL_SELN_BM          (1 << 2)
#define LM75B_CFG_REG_OS_FLT_Q_PRGMG_BM    ((1 << 3) | (1 << 4))


/*
 * Temperatures range from -55 degree C to +125 degree C
 */
#define LM75B_MIN_OUTPUT_VAL  ((SINT8_t)-55)
#define LM75B_MAX_OUTPUT_VAL  ((SINT8_t)125)


typedef enum
{
   LM75B_OPN_MODE_UNKNOWN,      // Implementation specific
   LM75B_OPN_MODE_NORMAL,       // Periodically monitor the ambient temperature (every 100 milli-seconds)
   LM75B_OPN_MODE_SHUT_DOWN     // Shutdown mode to minimize power consumption
} LM75B_mode_t;


typedef struct
{
   UINT8_t devId;
   LM75B_mode_t opnMode;
   PLTFRM_snsrDevInfo_s info;
} LM75B_cntxt_s;

/*
 * In normal operation mode, the temp-to-digital conversion is executed
 * every 100 ms and the "Temp" register is updated at the end of each conversion.
 */
#define LM75B_PERIODIC_TEMP_SENSING_INTERVAL   100   // 100 milliseconds

/*
 * During each ‘conversion period’ (Tconv) of about 100 msecs the device takes only
 * about 10 msecs, called ‘temperature conversion time’ (tconv(T)), to complete a
 * temperature-to-data conversion and then becomes idle for the time remaining in
 * the period.
 */
#define LM75B_TEMP_SENSING_TIME  10   // 10 milliseconds

extern PLTFRM_sts_t LM75B_init(UINT8_t devId);
extern PLTFRM_sts_t LM75B_startSingleConv(UINT8_t devId, SINT16_t *tempVal_p, UINT16_t *tmoVal_p);
extern PLTFRM_sts_t LM75B_shutDown(UINT8_t devId);
extern PLTFRM_sts_t LM75B_getTempVal(UINT8_t devId, SINT16_t *tempVal_p);
extern PLTFRM_sts_t LM75B_cfg(UINT8_t devId, LM75B_mode_t reqOpnMode);

#endif
