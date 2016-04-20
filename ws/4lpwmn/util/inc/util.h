/*
 * File Name: util.h
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Sep/8/2013
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

#ifndef __UTIL_H__
#define __UTIL_H__
/*
 * File Name : util.h
 * Author : ram krishnan
 * Created : 9/8/2013
 */

#include <typedefs.h>

#define UTIL_TRACE_LOG_ENTRY_MAX_CNT  256

typedef struct
{
   UINT8_t _traceId;
} UTIL_traceLogEntry_s;

typedef struct
{
   UINT16_t _traceLogOldest;
   UINT16_t _traceLogLatest;
   UINT16_t _traceLogInUseCnt;
   UINT32_t _traceLogInsertCnt;
   UTIL_traceLogEntry_s _array[UTIL_TRACE_LOG_ENTRY_MAX_CNT];
} UTIL_traceCntxt_s;


#define UTIL_TLV_TYPE_FIELD_SZ   0x1
#define UTIL_TLV_LEN_FIELD_SZ    0x1

#define UTIL_TLV_HDR_SZ  \
        (UTIL_TLV_TYPE_FIELD_SZ + UTIL_TLV_LEN_FIELD_SZ)
   
extern UINT8_t *UTIL_htons(UINT8_t *buff_p, UINT16_t val);

extern UINT16_t UTIL_ntohs(UINT8_t *buff_p);

extern UINT32_t UTIL_ntohl(UINT8_t *buff_p);

extern UINT8_t *UTIL_htonl(UINT8_t *buff_p, UINT32_t val);

extern UINT8_t *UTIL_htole16(UINT8_t *buff_p, UINT16_t val);

extern UINT8_t *UTIL_htole32(UINT8_t *buff_p, UINT32_t val);

extern UINT8_t UTIL_getTlv(UINT8_t *buff_p,
                           UINT8_t buffLen,
                           UINT8_t tlvType,
                           UINT8_t *tlvValLen_p,
                           UINT8_t **tlvValBuff_pp);

extern UINT8_t *UTIL_buildTlvHdr(UINT8_t *buff_p,
                                 UINT8_t tlvType,
                                 UINT8_t tlvPyldLen);

extern UINT32_t UTIL_letoh32(UINT8_t *buff_p);

extern UINT16_t UTIL_letoh16(UINT8_t *buff_p);

extern void UTIL_initTraceCntxt(UTIL_traceCntxt_s *cntxt_p);
extern void UTIL_AddTrace(UTIL_traceCntxt_s *cntxt_p, UINT8_t traceId);
#endif
