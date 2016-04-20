/*
 * File Name : sync_rt.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : 9/28/2014
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

#ifndef __SYNC_RT_H__
#define __SYNC_RT_H__

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>

#define SYNC_RT_STS_SUCCESS         0x0
#define SYNC_RT_STS_DEV_BUSY        0x1
#define SYNC_RT_STS_HW_TX_ERR       0x2
#define SYNC_RT_STS_INV_PARAMS      0x3
#define SYNC_RT_STS_TX_IN_PROGRESS  0x4
#define SYNC_RT_STS_IDLE            0x5

#define SYNC_RT_MSG_TYPE_ACK   0x0

// Bit definitions of the flags field in the message header
#define SYNC_RT_HDR_ACK_BM  (1 << 7)
#define SYNC_RT_PYLD_ACK_BM  (~(1 << 7))

#define SYNC_RT_ACK_STS_OK_BM               (1 << 0)
#define SYNC_RT_ACK_STS_OOM_BM              (1 << 1)
#define SYNC_RT_ACK_STS_FRAME_TOO_LONG_BM   (1 << 2)
#define SYNC_RT_ACK_STS_INV_CRC             (1 << 3)
#define SYNC_RT_ACK_STS_RELAY_IN_PROGRESS   (1 << 4)
#define SYNC_RT_ACK_STS_HDR_BYTES_MISSING   (1 << 5)
#define SYNC_RT_ACK_STS_PYLD_BYTES_MISSING  (1 << 6)

#define SYNC_RT_FRAME_HDR_MSG_TYPE_FIELD_LEN   2
#define SYNC_RT_FRAME_HDR_FLAGS_FIELD_LEN      1
#define SYNC_RT_FRAME_HDR_SEQ_NR_FIELD_LEN     1
#define SYNC_RT_FRAME_HDR_PYLD_LEN_FIELD_LEN   2
#define SYNC_RT_FRAME_HDR_CRC_FIELD_LEN        2

#define SYNC_RT_FRAME_HDR_HDR_CRC_FIELD_LEN   SYNC_RT_FRAME_HDR_CRC_FIELD_LEN
#define SYNC_RT_FRAME_HDR_PYLD_CRC_FIELD_LEN  SYNC_RT_FRAME_HDR_CRC_FIELD_LEN

#define SYNC_RT_FRAME_HDR_MSG_TYPE_FIELD_OFF   0

#define SYNC_RT_FRAME_HDR_FLAGS_FIELD_OFF \
        SYNC_RT_FRAME_HDR_MSG_TYPE_FIELD_LEN

#define SYNC_RT_FRAME_HDR_SEQ_NR_FIELD_OFF \
        (SYNC_RT_FRAME_HDR_FLAGS_FIELD_OFF + SYNC_RT_FRAME_HDR_FLAGS_FIELD_LEN)

#define SYNC_RT_FRAME_HDR_PYLD_LEN_FIELD_OFF \
        (SYNC_RT_FRAME_HDR_SEQ_NR_FIELD_OFF + SYNC_RT_FRAME_HDR_SEQ_NR_FIELD_LEN)

#define SYNC_RT_FRAME_HDR_HDR_CRC_FIELD_OFF  \
        (SYNC_RT_FRAME_HDR_PYLD_LEN_FIELD_OFF + SYNC_RT_FRAME_HDR_PYLD_LEN_FIELD_LEN)

#define SYNC_RT_FRAME_HDR_PYLD_CRC_FIELD_OFF \
        (SYNC_RT_FRAME_HDR_HDR_CRC_FIELD_OFF + SYNC_RT_FRAME_HDR_HDR_CRC_FIELD_LEN)

#define SYNC_RT_FRAME_MAX_PYLD_LEN  128

#define SYNC_RT_FRAME_HDR_LEN  (SYNC_RT_FRAME_HDR_MSG_TYPE_FIELD_LEN \
                                + SYNC_RT_FRAME_HDR_FLAGS_FIELD_LEN \
                                + SYNC_RT_FRAME_HDR_SEQ_NR_FIELD_LEN \
                                + SYNC_RT_FRAME_HDR_PYLD_LEN_FIELD_LEN \
                                + SYNC_RT_FRAME_HDR_HDR_CRC_FIELD_LEN \
                                + SYNC_RT_FRAME_HDR_PYLD_CRC_FIELD_LEN)


#define SYNC_RT_RX_SIG_CNT  3   // Data in, Clock in, Ack out
#define SYNC_RT_TX_SIG_CNT  3   // Data out, Clock out, Ack in
#define SYNC_RT_SIG_CNT  (SYNC_RT_RX_SIG_CNT \
		                  + SYNC_RT_TX_SIG_CNT)

typedef struct
{
  UINT8_t portNr:4;
  UINT8_t pinNr:4;
} SYNC_RT_sigInfo_s;

typedef enum
{
  SYNC_RT_RX_DATA_IN_SIG_ID = 0,
  SYNC_RT_RX_CLK_IN_SIG_ID = 1,
  SYNC_RT_RX_ACK_OUT_SIG_ID = 2,
  SYNC_RT_TX_DATA_OUT_SIG_ID = 3,
  SYNC_RT_TX_CLK_OUT_SIG_ID = 4,
  SYNC_RT_TX_ACK_IN_SIG_ID = 5
} SYNC_RT_sigId_t;

typedef struct
{
   UINT8_t devId;
   SYNC_RT_sigInfo_s sigList[SYNC_RT_SIG_CNT];
   UINT8_t rxFrameBdHndl;
   UINT8_t txSeqNr;
   UINT8_t txBitOff, *txBuff_p;
   UINT8_t rxBitOff;
   UINT16_t txBuffLen, txBuffOff;
   UINT8_t rxFrameOff;
   UINT8_t rxFramePyldLen;
   UINT8_t *rxFrameBuff_p;
   UINT8_t rxHdrBuff[SYNC_RT_FRAME_HDR_LEN];
   SYS_trscnQ_s pendTxtrscnQ;
} SYNC_RT_cntxt_s;


extern PLTFRM_sts_t SYNC_RT_init(UINT8_t devId);

extern void SYNC_RT_addToTxPendQ(UINT8_t devId, UINT8_t bdHndl);

extern void SYNC_RT_buildMsgHdr(UINT8_t devId,
		                        UINT16_t msgType,
                                UINT8_t *buff_p,
                                UINT16_t pyldLen);

extern void SYNC_RT_start(UINT8_t devId);

#endif
