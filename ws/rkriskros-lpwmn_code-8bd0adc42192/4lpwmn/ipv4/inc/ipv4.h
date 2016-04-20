/*
 * File Name : ipv4.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : 9/9/2013
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
 
#ifndef __IPv4_H__
#define __IPv4_H__

#define IPv4_ADDR_LEN  4

#define IPv4_HDR_LEN  20

#define IPv4_HDR_TOTAL_LEN_FIELD_OFF   2
#define IPv4_HDR_ID_FIELD_OFF          4
#define IPv4_HDR_FRAG_OFF_FIELD_OFF    6
#define IPv4_HDR_TTL_FIELD_OFF         8
#define IPv4_HDR_PROTO_FIELD_OFF       9
#define IPv4_HDR_CKSUM_FIELD_OFF       10
#define IPv4_HDR_SRC_ADDR_FIELD_OFF    12
#define IPv4_HDR_DEST_ADDR_FIELD_OFF   16

#define IPv4_NET_MASK  0xffff0000  // 255.255.0.0
#define IPv4_NET_ADDR_SHIFT  16

// IPv4 compression information byte

#define IPv4_COMP_ENC_FIELD_LEN  1


#define IPv4_COMP_BYTE_SAM_BIT_NR    0x0
#define IPv4_COMP_BYTE_DAM_BIT_NR    0x2
#define IPv4_COMP_BYTE_NHC_BIT_NR    0x4
#define IPv4_COMP_BYTE_CKSUM_BIT_NR  0x5
#define IPv4_COMP_BYTE_UDP_SRC_PORT_NR  0x6
#define IPv4_COMP_BYTE_UDP_DEST_PORT_NR 0x7

#define IPv4_COMP_BYTE_SAM_BIT_SHIFT_MSK    0x3
#define IPv4_COMP_BYTE_DAM_BIT_SHIFT_MSK    0xc
#define IPv4_COMP_BYTE_NHC_BIT_SHIFT_MSK    0x10
#define IPv4_COMP_BYTE_CKSUM_BIT_SHIFT_MSK  0x20
#define IPv4_COMP_BYTE_UDP_SRC_PORT_BIT_SHIFT_MSK  0x40
#define IPv4_COMP_BYTE_UDP_DEST_PORT_BIT_SHIFT_MSK 0x80

#define IPv4_COMP_BYTE_SAM_BIT_MSK   0x3
#define IPv4_COMP_BYTE_SAM_BIT_MSK   0x3
#define IPv4_COMP_BYTE_NHC_BIT_MSK   0x1
#define IPv4_COMP_BYTE_CKSUM_BIT_MSK  0x1
#define IPv4_COMP_BYTE_UDP_SRC_PORT_BIT_MSK  0x1
#define IPv4_COMP_BYTE_UDP_DEST_PORT_BIT_MSK 0x1




/*
 * DAM / SAM
 *   00 - Completely inline
 *   01 - 16 bit network prefix
 *   10 - 16 bit host id
 *   11 - Completely elided (Derieved from fixed prefix and short address)
 */
#define IPv4_COMP_BYTE_SAM_ENC_0  0x0
#define IPv4_COMP_BYTE_SAM_ENC_1  0x1
#define IPv4_COMP_BYTE_SAM_ENC_2  0x2
#define IPv4_COMP_BYTE_SAM_ENC_3  0x3

#define IPv4_COMP_BYTE_DAM_ENC_0  0x0
#define IPv4_COMP_BYTE_DAM_ENC_1  0x1
#define IPv4_COMP_BYTE_DAM_ENC_2  0x2
#define IPv4_COMP_BYTE_DAM_ENC_3  0x3


/*
 * NHC
 *  1 - Protocol field compressed (is UDP)
 *  0 - Protocol field carried inline (!= UDP)
 */
#define IPv4_COMP_BYTE_NHC_ENC_0  0x0
#define IPv4_COMP_BYTE_NHC_ENC_1  0x1

/*
 * CKSUM
 *  1 - IPv4 header checksum is compressed 
 *  0 - IPv4 header checksum is carried inline
 */
#define IPv4_COMP_BYTE_CKSUM_ENC_0  0x0
#define IPv4_COMP_BYTE_CKSUM_ENC_1  0x1


/*
 * UDP source & destination port 
 *  0: Not compressed, carried "in-line" 
 *  1: Compressed to 4 bits.  The actual 16-bit source port is
 *     obtained by calculating: P + short_port value.  The value of
 *     P is the number 61616 (0xF0B0).  The short_port is expressed
 *     as a 4-bit value which is carried "in-line" (Section 10.3.2)
 */
#define IPv4_COMP_BYTE_UDP_SRC_PORT_ENC_0   0x0
#define IPv4_COMP_BYTE_UDP_SRC_PORT_ENC_1   0x1

#define IPv4_COMP_BYTE_UDP_DEST_PORT_ENC_0   0x0
#define IPv4_COMP_BYTE_UDP_DEST_PORT_ENC_1   0x1
   
#define UDP_PORT_COMP_ENC_LEN  0x1     
#endif
