/*
 * File Name : m2m01.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : Feb/7/2015
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

#ifndef __M24M01_H__
#define __M24M01_H__

#include <typedefs.h>
#include <i2c_sw.h>


#define M24M01_MEM_SIZE_KB  128
#define M24M01_PAGE_SZ  256


/*
 * When accessing the memory (assuming E1 and E2 are x and y resp)
 *    1 0 1 0 E2 E1 A16  R/W  -> 1 0 1 0 1 1 0/1 [R/W]  -> 0 1 0 1  0 x y 0/1
 *    0x56 and 0x57 when E1 and E2 are both 1
 *
 * When accessing the Identification page (assuming E1 and E2 are x and y resp)
 *    1 0 1 1 E2 E1 DC   R/W  -> 1 0 1 1 1 1  DC [R/W]  -> 0 1 0 1  1 x y  DC
 *    0x5e and 0x5f when E1 and E2 are both 1
 */
#define M24M01_I2C_ADDR_PREFIX_1   0x50   // When accessing the memory
#define M24M01_I2C_ADDR_PREFIX_2   0x58   // When accessing the Identification page


// Both durations are same !!
#define M24M01_BYTE_WRITE_DURATION_MILLISECS  5
#define M24M01_PAGE_WRITE_DURATION_MILLISECS  5

typedef struct
{
   UINT8_t devId;
   UINT8_t busId;
   UINT8_t chipEnaAddr;   // Bits E2 and E1
} M24M01_cntxt_s;

extern PLTFRM_sts_t M24M01_init(const UINT8_t devId);

extern PLTFRM_sts_t M24M01_read(UINT8_t devId,
                                UINT32_t addr,
		                        UINT16_t len,
                                UINT8_t *byte_p);

extern PLTFRM_sts_t M24M01_write(UINT8_t devId,
                                 UINT32_t addr,
		                         UINT16_t len,
                                 UINT8_t *byte_p);

#endif
