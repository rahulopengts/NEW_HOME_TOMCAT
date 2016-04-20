/*
 * File Name: udp.h
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Sep/9/2013
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

#ifndef __UDP_H__
#define __UDP_H__

#define UDP_PORT_LEN  2

#define UDP_HDR_SRC_PORT_FIELD LEN  UDP_PORT_LEN
#define UDP_HDR_DEST_PORT_FIELD LEN  UDP_PORT_LEN
#define UDP_HDR_LENGTH_FIELD_LEN  2
#define UDP_HDR_CRC_FIELD_LEN  2 


#define UDP_PORT_COMP_RANGE_MIN_VAL  0xf0b0
#define UDP_PORT_COMP_RANGE_MAX_VAL  0xf0bf

#define UDP_COMPRESSED_SRC_PORT_BIT_MSK   0x0f
#define UDP_COMPRESSED_DEST_PORT_BIT_MSK  0xf0

#define UDP_COMPRESSED_SRC_PORT_BIT_SHIFT   0
#define UDP_COMPRESSED_DEST_PORT_BIT_SHIFT  4

#endif
