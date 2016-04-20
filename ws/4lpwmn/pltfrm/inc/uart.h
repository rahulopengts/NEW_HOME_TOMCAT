/*
 * File Name : uart.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : 10/3/2013
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
 *
 */

#ifndef __UART_H__
#define __UART_H__

#include <typedefs.h>
#include <system.h>
#ifdef __IAR__
#include "io430.h"
#else
#include <stdarg.h>
#include <msp430.h>
#endif


/*
 * A transmission is initiated by writing data to UCAxTXBUF. When
 * this occurs, the baud rate generator is enabled and the data in
 * UCAxTXBUF is moved to the transmit shift register on the next
 * BITCLK after the transmit shift register is empty. UCAxTXIFG is
 * set when new data can be written into UCAxTXBUF. Transmission
 * continues as long as new data is available in UCAxTXBUF at the
 * end of the previous byte transmission. If new data is not in
 * UCAxTXBUF when the previous byte has transmitted, the transmitter
 * returns to its idle state and the baud rate generator is turned off.
 */
#define UART_wrByte(byte)  UCA0TXBUF = (byte)


#define UART_LOG_BUFF_LEN   256

#define UART_STS_SUCCESS         0x0
#define UART_STS_DEV_BUSY        0x1
#define UART_STS_HW_TX_ERR       0x2
#define UART_STS_INV_PARAMS      0x3
#define UART_STS_TX_IN_PROGRESS  0x4
#define UART_STS_IDLE            0x5

// #define UART_STATS_EN

#define UART_MSG_TYPE_ACK   0x0


// Bit definitions of the flags field in the message header
#define UART_HDR_ACK_BM  (1 << 7)
#define UART_PYLD_ACK_BM  (~(1 << 7))

#define UART_ACK_STS_OK_BM               (1 << 0)
#define UART_ACK_STS_OOM_BM              (1 << 1)
#define UART_ACK_STS_FRAME_TOO_LONG_BM   (1 << 2)
#define UART_ACK_STS_INV_CRC             (1 << 3)
#define UART_ACK_STS_RELAY_IN_PROGRESS   (1 << 4)
#define UART_ACK_STS_HDR_BYTES_MISSING   (1 << 5)
#define UART_ACK_STS_PYLD_BYTES_MISSING  (1 << 6)

#define UART_FRAME_HDR_MSG_TYPE_FIELD_LEN   2
#define UART_FRAME_HDR_FLAGS_FIELD_LEN      1
#define UART_FRAME_HDR_SEQ_NR_FIELD_LEN     1
#define UART_FRAME_HDR_PYLD_LEN_FIELD_LEN   2
#define UART_FRAME_HDR_CRC_FIELD_LEN        2

#define UART_FRAME_HDR_HDR_CRC_FIELD_LEN   UART_FRAME_HDR_CRC_FIELD_LEN
#define UART_FRAME_HDR_PYLD_CRC_FIELD_LEN  UART_FRAME_HDR_CRC_FIELD_LEN

#define UART_FRAME_HDR_MSG_TYPE_FIELD_OFF   0

#define UART_FRAME_HDR_FLAGS_FIELD_OFF \
        UART_FRAME_HDR_MSG_TYPE_FIELD_LEN

#define UART_FRAME_HDR_SEQ_NR_FIELD_OFF \
        (UART_FRAME_HDR_FLAGS_FIELD_OFF + UART_FRAME_HDR_FLAGS_FIELD_LEN)

#define UART_FRAME_HDR_PYLD_LEN_FIELD_OFF \
        (UART_FRAME_HDR_SEQ_NR_FIELD_OFF + UART_FRAME_HDR_SEQ_NR_FIELD_LEN)

#define UART_FRAME_HDR_HDR_CRC_FIELD_OFF  \
        (UART_FRAME_HDR_PYLD_LEN_FIELD_OFF + UART_FRAME_HDR_PYLD_LEN_FIELD_LEN)

#define UART_FRAME_HDR_PYLD_CRC_FIELD_OFF \
        (UART_FRAME_HDR_HDR_CRC_FIELD_OFF + UART_FRAME_HDR_HDR_CRC_FIELD_LEN)

#define UART_FRAME_MAX_PYLD_LEN  128

#define UART_FRAME_HDR_LEN  (UART_FRAME_HDR_MSG_TYPE_FIELD_LEN \
                             + UART_FRAME_HDR_FLAGS_FIELD_LEN \
                             + UART_FRAME_HDR_SEQ_NR_FIELD_LEN \
                             + UART_FRAME_HDR_PYLD_LEN_FIELD_LEN \
                             + UART_FRAME_HDR_HDR_CRC_FIELD_LEN \
                             + UART_FRAME_HDR_PYLD_CRC_FIELD_LEN)

#ifndef UART_TX_INT_ENA
#define UART_POLL_TX( )  \
do { \
  if (UART_cntxt.txBuffLen > 0) \
      UART_pollTx(); \
} while (0)
#endif

#define UART_RX_CIRC_BUFF_SZ  16

typedef void (*UART_txDoneCbFunc_t)(UINT8_t sts);

typedef struct
{
#if defined(DEV_TYPE_RFD)
  // Simple circular buffer
  UINT8_t rxCircBuffNew;
  UINT8_t rxCircBuffOld;
  UINT8_t rxCircBuffEntries;
  UINT8_t rxCircBuff[UART_RX_CIRC_BUFF_SZ];
#else
  UINT8_t rxFrameBdHndl;
  UINT8_t rcvrStuckFlag;
  UINT16_t currRcvdFrameSeqNr;
  UINT16_t lastCheckedRcvdFrameSeqNr;
  UINT8_t stsTxPending;
  UINT8_t rxFramePyldLen;
  UINT8_t rxFrameOff;
  UINT8_t *rxFrameBuff_p;
  UINT8_t rxHdrBuff[UART_FRAME_HDR_LEN];
  SYS_trscnQ_s pendTxtrscnQ;
#endif

  UINT16_t txBuffLen;
  UINT16_t txBuffOff;
  UINT8_t *txBuff_p;
  UINT8_t txSeqNr;

#ifdef UART_FLOW_CONTROL_ON_TX
  UINT8_t flowIntPending;
#endif

#ifdef UART_STATS_EN
  UINT16_t UART_txIntCnt;
  UINT16_t UART_rxIntCnt;
  UINT16_t UART_ucb0RxIntCnt;
  UINT16_t byteTxCnt;
  UINT16_t spuriousTxIntCnt;
  UINT8_t pyldCrcBadCnt;
  UINT8_t pyldCrcOkCnt;
  UINT8_t hdrRcvdCnt;
  UINT8_t hdrCrcOkCnt;
  UINT8_t hdrCrcBadCnt;
  UINT8_t frameTxStartCnt;
  UINT8_t frameTxDoneCnt;
  UINT8_t ackMemAllocOk;
  UINT8_t ackMemAllocFlr;
  UINT16_t flowCntrlIntCnt;
#endif
} UART_cntxt_s;


void UART_procPendingTxReqs(void);

extern void UART_blockTxDoneEvtHndlr(void);

extern void UART_addToTxPendQ(UINT8_t bdHndl);

extern void UART_buildMsgHdr(UINT16_t msgType, UINT8_t *buff_p, UINT16_t pyldLen);

extern void UART_init(void);

extern void UART_start(UINT8_t devId);

extern void UART_checkRcvdFramePyld(void);

extern void UART_checkRcvdFrameHdr(void);

extern void UART_pollTx(void);

extern void UART_checkRcvrState(void);

extern UINT8_t UART_wrBlock(UINT8_t *buff_p, UINT16_t buffLen);

void UART_log(char *fmt_p, ...);

// void UART_log(SINT8_t *logStr_p);

extern UART_cntxt_s UART_cntxt;

extern SINT8_t UART_logBuff[UART_LOG_BUFF_LEN];

extern void UART_flushRcvdData(void);

extern UINT8_t UART_getRxBuffData(UINT8_t *data_p);

#endif /* __UART_H__ */
