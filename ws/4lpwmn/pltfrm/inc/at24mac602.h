/*
 * File Name : at24mac602.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : Nov/9/2013
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

#ifndef __AT24MAC602_H__
#define __AT24MAC602_H__

#include <typedefs.h>
#include <i2c_sw.h>

#define AT24MAC602_EEPROM_LEN  256


#define AT24MAC602_WRITE_CYCLE_TMO_MSECS   6    // Spec says 5 milliseconds

/*
 * The device address word consists of a mandatory one-zero sequence for the
 * first four most-significant bits "1010" (Ah) for normal read and write operations
 */
#define AT24MAC602_I2C_ADDR_PREFIX_1   0x50    // 0 1010 xxx  (7 bit address)

/*
 * and "0110" (6h) for writing to the Software Write Protect Register.
 */
#define AT24MAC602_I2C_ADDR_PREFIX_2   0x30    // 0 0110 xxx  (7 bit address)

/*
 * The AT24MAC402/602 incorporates an extended memory block containing a factory-
 * programmed 128-bit serial number. Access to this memory location is obtained by
 * beginning the device address word with a "1011" (Bh) sequence.
 *
 * The AT24MAC602 utilizes an extended memory block containing a factory-programmed
 * read-only EUI-64 address respectively. Access to this memory block is obtained by
 * beginning the device address word with a "1011" (Bh) sequence.
 */
#define AT24MAC602_I2C_ADDR_PREFIX_3   0x58    // 0 1011 xxx  (7 bit address)

/*
 * Contents of the extended memory block.
 */
#define AT24MAC602_SERIAL_NR_OFF  0x80
#define AT24MAC602_SERIAL_NR_LEN  0x10   // 128 bits

#define AT24MAC602_EUI64_OFF  0x98
#define AT24MAC602_EUI64_LEN  0x8   // 64 bits

#define AT24MAC602_EUI48_OFF  0x9a
#define AT24MAC602_EUI48_LEN  0x6   // 48 bits

typedef struct
{ 
   UINT8_t devId;
   UINT8_t initDone;
} AT24MAC602_cntxt_s;


extern PLTFRM_sts_t AT24MAC602_readEUI64(UINT8_t devId, UINT8_t *buff_p);
extern PLTFRM_sts_t AT24MAC602_readSerialNr(UINT8_t devId, UINT8_t *buff_p);
extern PLTFRM_sts_t AT24MAC602_init(const UINT8_t devId);
extern PLTFRM_sts_t AT24MAC602_write(UINT8_t devId, UINT8_t wrOff,
                                     UINT8_t wrCnt, UINT8_t *buff_p);
extern PLTFRM_sts_t AT24MAC602_readMem(UINT8_t devId,
                                       UINT8_t rdOff,
                                       UINT8_t rdCnt,
                                       UINT8_t *buff_p);
#endif
