/*
 * File Name : ext_if.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : Oct/1/2014
 *
 *
 * Copyright (c) <2014>, <ram krishnan>
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

#ifndef __EXT_IF_H__
#define __EXT_IF_H__

#if defined(COORD_EXT_IF_UART)
#include <uart.h>

#define EXT_IF_buildMsgHdr(msgType, buff_p, pyldLen) \
	    UART_buildMsgHdr(msgType, buff_p, pyldLen)

#define EXT_IF_addToTxPendQ(bdHndl) \
	    UART_addToTxPendQ(bdHndl)

#elif defined(COORD_EXT_IF_SYNC_RT)
#include <sync_rt.h>

#define EXT_IF_buildMsgHdr(msgType, buff_p, pyldLen) \
	    SYNC_RT_buildMsgHdr(msgType, buff_p, pyldLen)

#define EXT_IF_addToTxPendQ(bdHndl) \
	    SYNC_RT_addToTxPendQ(bdHndl)

#else
#error Coord external interface not defined !!
#endif


#define EXT_IF_FRAME_HDR_MSG_TYPE_FIELD_LEN   2
#define EXT_IF_FRAME_HDR_FLAGS_FIELD_LEN      1
#define EXT_IF_FRAME_HDR_SEQ_NR_FIELD_LEN     1
#define EXT_IF_FRAME_HDR_PYLD_LEN_FIELD_LEN   2
#define EXT_IF_FRAME_HDR_CRC_FIELD_LEN        2

#define EXT_IF_FRAME_HDR_HDR_CRC_FIELD_LEN   EXT_IF_FRAME_HDR_CRC_FIELD_LEN
#define EXT_IF_FRAME_HDR_PYLD_CRC_FIELD_LEN  EXT_IF_FRAME_HDR_CRC_FIELD_LEN

#define EXT_IF_FRAME_HDR_MSG_TYPE_FIELD_OFF   0

#define EXT_IF_FRAME_HDR_FLAGS_FIELD_OFF \
        EXT_IF_FRAME_HDR_MSG_TYPE_FIELD_LEN

#define EXT_IF_FRAME_HDR_SEQ_NR_FIELD_OFF \
        (EXT_IF_FRAME_HDR_FLAGS_FIELD_OFF + EXT_IF_FRAME_HDR_FLAGS_FIELD_LEN)

#define EXT_IF_FRAME_HDR_PYLD_LEN_FIELD_OFF \
        (EXT_IF_FRAME_HDR_SEQ_NR_FIELD_OFF + EXT_IF_FRAME_HDR_SEQ_NR_FIELD_LEN)

#define EXT_IF_FRAME_HDR_HDR_CRC_FIELD_OFF  \
        (EXT_IF_FRAME_HDR_PYLD_LEN_FIELD_OFF + EXT_IF_FRAME_HDR_PYLD_LEN_FIELD_LEN)

#define EXT_IF_FRAME_HDR_PYLD_CRC_FIELD_OFF \
        (EXT_IF_FRAME_HDR_HDR_CRC_FIELD_OFF + EXT_IF_FRAME_HDR_HDR_CRC_FIELD_LEN)

#define EXT_IF_FRAME_MAX_PYLD_LEN  128

#define EXT_IF_FRAME_HDR_LEN  (EXT_IF_FRAME_HDR_MSG_TYPE_FIELD_LEN \
                             + EXT_IF_FRAME_HDR_FLAGS_FIELD_LEN \
                             + EXT_IF_FRAME_HDR_SEQ_NR_FIELD_LEN \
                             + EXT_IF_FRAME_HDR_PYLD_LEN_FIELD_LEN \
                             + EXT_IF_FRAME_HDR_HDR_CRC_FIELD_LEN \
                             + EXT_IF_FRAME_HDR_PYLD_CRC_FIELD_LEN)

#endif

