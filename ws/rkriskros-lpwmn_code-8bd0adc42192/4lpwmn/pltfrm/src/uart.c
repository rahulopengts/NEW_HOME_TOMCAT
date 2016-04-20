/*
 * File Name: uart.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Oct/3/2013
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

#ifdef PLTFRM_UART_ENA

#include <stdio.h>
#include <string.h>
#include <uart.h>
#include <system.h>
#include <pltfrm.h>
#include <util.h>

#include <gw.h>
#ifdef NODE_HOST_IF_ENA
#include <node_host_if.h>
#endif




#ifdef PLTFRM_LOG_ENA


SINT8_t UART_logBuff[UART_LOG_BUFF_LEN];

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void UART_init(void)
{


   /*
    * USCI_Ax - (UART/SPI/IrDA/LIN)
    * USCI_Bx - (SPI/I2C)
    */

   /*
    * Using the USCI_A0 USCI module.
    *
    * USCI Introduction: UART Mode
    * In asynchronous mode, the USCI_Ax modules connect the MSP430 to an
    * external system via two external pins, UCAxRXD and UCAxTXD. UART mode
    * is selected when the UCSYNC bit is cleared.
    */


#ifdef __MSP430G2553__
   // P1.1 - UCA0RXD
   // P1.2 - UCA0TXD
   P1SEL |= BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2 = TXD
   P1SEL2 |= BIT1 + BIT2;
   P1DIR &= ~BIT1;                           // P1.1 = RXD
   P1DIR |= BIT2;                            // P1.2 = TXD
#elif defined(__MSP430G2955__)
   // P3.5 - UCA0RXD
   // P3.4 - UCA0TXD
   P3SEL |= BIT4 + BIT5;                    // P1.1 = RXD, P1.2 = TXD
   P3SEL2 &= ~(BIT4 + BIT5);
#else
#error micro variant not specified !!
#endif

#if 0
   UCA0CTL1 |= UCSSEL_1;        // CLK = ACLK
   UCA0BR0 = 0x3;               // 32kHz/9600 = 3.41
   UCA0MCTL = UCBRS0 + UCBRS1;  // Modulation UCBRSx = 3
#endif

   UCA0CTL1 |= UCSSEL_2;         // CLK = SMCLK ( 8 MHz)

#ifdef UART_BAUD_RATE_9600
   UCA0BR1 = 3;               // 56000
   UCA0BR0 = 65;
   UCA0MCTL = UCBRS1;         // Modulation UCBRSx = 2
#elif defined(UART_BAUD_RATE_56000)
   UCA0BR1 = 0;               // 56000
   UCA0BR0 = 142;
   UCA0MCTL = UCBRS2 + UCBRS1 + UCBRS0;  // Modulation UCBRSx = 7
#elif defined(UART_BAUD_RATE_115200)
   UCA0BR1 = 0;               // 56000
   UCA0BR0 = 69;
   UCA0MCTL = UCBRS2;  // Modulation UCBRSx = 4
#else
#error UART baud rate not specified !!
#endif
   /*
    * The USCI module is enabled by clearing the UCSWRST bit and the transmitter
    * is ready and in an idle state. The transmit baud rate generator is ready but
    * is not clocked nor producing any clocks.
    */
   UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**

   /*
    * The USCI has one interrupt vector for transmission and one interrupt
    * vector for reception.
    *
    * USCI_Ax and USCI_Bx share the same interrupt vectors. The receive interrupt
    * flags UCAxRXIFG and UCBxRXIFG are routed to one interrupt vector, the transmit
    * interrupt flags UCAxTXIFG and UCBxTXIFG share another interrupt vector.
    *
    * UCA0TXIFG Bit 1 : USCI_A0 transmit interrupt flag.
    *                   UCA0TXIFG is set when UCA0TXBUF is empty.
    *                   UCAxTXIFG is automatically reset if a character is written to UCAxTXBUF.
    *                   UCAxTXIFG is set after a PUC or when UCSWRST = 1.
    *                    - 0 No interrupt pending
    *                    - 1 Interrupt pending
    *
    * UCA0RXIFG Bit 0 : USCI_A0 receive interrupt flag.
    *                   UCA0RXIFG is set when UCA0RXBUF has received a complete character.
    *                   UCAxRXIFG is automatically reset when UCAxRXBUF is read.
    *                   UCAxRXIFG is reset by a system reset PUC signal or when UCSWRST = 1.
    *                    - 0 No interrupt pending
    *                    - 1 Interrupt pending
    */

   IFG2 &= ~(UCA0RXIFG);

   IE2 &= ~UCA0RXIE;  // Disable USCI_A0 RX interrupt
   IE2 &= ~UCA0TXIE;  // Disable USCI_A0 RX interrupt



   // IFG2 &= ~(UCA0TXIFG | UCA0RXIFG);
   // IFG2 &= ~(UCB0TXIFG | UCB0RXIFG);

}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void UART_log(char *fmt_p, ...)
{
   va_list argPtr;
   UINT16_t logLen, offset = 0;

   // In CCSv5, enable support for GCC extensions in "build settings -> language options"

   va_start(argPtr, fmt_p);
   logLen = vsnprintf(UART_logBuff, UART_LOG_BUFF_LEN, fmt_p, argPtr);
   va_end(argPtr);

   for (offset=0; offset < logLen; offset++)
   {
       while ((UCA0STAT & UCBUSY) || (!(IFG2 & UCA0TXIFG)));
       // UCAxTXBUF ready to receive another byte
       UART_wrByte(UART_logBuff[offset]);
   }

   return;
}

#if 0
void UART_log(SINT8_t *logStr_p)
{
   if (logStr_p == NULL)
       SYS_fatal(SYS_FATAL_ERR_259);

   while (*(logStr_p) != '\0')
   {
       while ((UCA0STAT & UCBUSY) || (!(IFG2 & UCA0TXIFG)));
       // UCAxTXBUF ready to receive another byte
       UART_wrByte(*(logStr_p));
       logStr_p ++;
   }

   return;
}
#endif
#else

UART_cntxt_s UART_cntxt;


/*
 * USCI_Ax - (UART/SPI/IrDA/LIN)
 * USCI_Bx - (SPI/I2C)
 */

#ifdef DEV_TYPE_RFD
#ifdef ECLIPSE_IDE
static void __attribute__((__interrupt__(USCIAB0TX_VECTOR))) UART_usci0TxISR (void)
#else
#pragma vector=USCIAB0TX_VECTOR
__interrupt void UART_usci0TxISR(void)
#endif
{
#ifdef UART_STATS_EN
   UART_cntxt.UART_txIntCnt ++;
#endif
}
#else
#ifdef UART_TX_INT_ENA
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
#ifdef ECLIPSE_IDE
static void __attribute__((__interrupt__(USCIAB0TX_VECTOR))) UART_usci0TxISR (void)
#else
#pragma vector=USCIAB0TX_VECTOR
__interrupt void UART_usci0TxISR(void)
#endif
{
   /*
    * The UCAxTXIFG interrupt flag is set by the transmitter to indicate
    * that UCAxTXBUF is ready to accept another character. An interrupt
    * request is generated if UCAxTXIE and GIE are also set. UCAxTXIFG is
    * automatically reset if a character is written to UCAxTXBUF. UCAxTXIFG
    * is set after a PUC or when UCSWRST = 1. UCAxTXIE is reset after a PUC
    * or when UCSWRST = 1.
    */
   if (!(IFG2 & UCA0TXIFG))
   {
#ifdef UART_STATS_EN
       UART_cntxt.spuriousTxIntCnt ++;
#endif
       return;
   }

   IFG2 &= ~UCA0TXIFG;

#ifdef UART_FLOW_CONTROL_ON_TX
   if (UART_cntxt.flowIntPending)
       return;
#endif

   if (UART_cntxt.txBuffLen > 0)
   {
       UART_cntxt.txBuffOff ++;

       if (UART_cntxt.txBuffOff < UART_cntxt.txBuffLen)
       {
#ifdef UART_STATS_EN
           UART_cntxt.byteTxCnt ++;
#endif

           UART_wrByte(UART_cntxt.txBuff_p[UART_cntxt.txBuffOff]);

#ifdef UART_FLOW_CONTROL_ON_TX
           if (UART_cntxt.txBuffOff == (UART_FRAME_HDR_LEN - 1))
           {
               if (UART_cntxt.txBuffLen > UART_FRAME_HDR_LEN)
               {
                  // Just transmitted last header byte ...
                  // Wait for flow interrupt from receiver before sending payload.
                  UART_cntxt.flowIntPending = 1;
               }
           }
#endif
       }
       else
       {
           UART_cntxt.txBuffLen = 0;
           SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_UART_BLOCK_TX_DONE;
#ifdef UART_STATS_EN
           UART_cntxt.frameTxDoneCnt ++;
#endif
           PLTFRM_exitLPM();  // rkris, dec/30/2013 - wake up micro if sleeping in LPM3
       }
   }

#ifdef UART_STATS_EN
   UART_cntxt.UART_txIntCnt ++;
#endif
}
#endif
#endif

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void UART_flushRcvdData(void)
{
#ifdef DEV_TYPE_RFD
   IE2 &= ~UCA0RXIE;  // Disable USCI_A0 RX interrupt
   UART_cntxt.rxCircBuffNew = 0;
   UART_cntxt.rxCircBuffOld = 0;
   UART_cntxt.rxCircBuffEntries = 0;
   IE2 |= UCA0RXIE;  // Enable USCI_A0 RX interrupt
#else
#endif
}

#if defined(DEV_TYPE_RFD)
UINT8_t UART_getRxBuffData(UINT8_t *data_p)
{
   UINT8_t idx, rc;

   IE2 &= ~UCA0RXIE;

   for (idx=0; idx<UART_cntxt.rxCircBuffEntries; idx++)
   {
       data_p[idx] = UART_cntxt.rxCircBuff[UART_cntxt.rxCircBuffOld ++];
       if (UART_cntxt.rxCircBuffOld >= UART_RX_CIRC_BUFF_SZ)
           UART_cntxt.rxCircBuffOld ++;
   }

   rc = UART_cntxt.rxCircBuffEntries;

   UART_cntxt.rxCircBuffEntries = 0;
   UART_cntxt.rxCircBuffOld = 0;
   UART_cntxt.rxCircBuffNew = 0;

   IE2 |= UCA0RXIE;

   return rc;
}
#endif


#ifdef DEV_TYPE_RFD
#ifdef ECLIPSE_IDE
static void __attribute__((__interrupt__(USCIAB0RX_VECTOR))) UART_usci0RxISR (void)
#else
#pragma vector=USCIAB0RX_VECTOR
__interrupt void UART_usci0RxISR(void)
#endif
{
   /*
    * Note that interrupts are disabled at this point since GIE bit in SR is
    * zero.
    */

   /*
    * The UCAxRXIFG interrupt flag is set each time a character is received
    * and loaded into UCAxRXBUF. An interrupt request is generated if UCAxRXIE
    * and GIE are also set. UCAxRXIFG is automatically reset when UCAxRXBUF is
    * read.
    *
    * Note that the USCI_Ax and USCI_Bx share the same interrupt vectors. The
    * receive interrupt flags UCAxRXIFG and UCBxRXIFG are routed to one interrupt
    * vector, the transmit interrupt flags UCAxTXIFG and UCBxTXIFG share another
    * interrupt vector.
    */

#ifdef UART_STATS_EN
   UART_cntxt.UART_rxIntCnt ++;
#endif

   // UCAxRXIFG is automatically reset when UCAxRXBUF is read
   UART_cntxt.rxCircBuff[UART_cntxt.rxCircBuffNew] = UCA0RXBUF;
   UART_cntxt.rxCircBuffNew ++;
   if (UART_cntxt.rxCircBuffNew >= UART_RX_CIRC_BUFF_SZ)
	   UART_cntxt.rxCircBuffNew = 0;
   if (UART_cntxt.rxCircBuffEntries >= UART_RX_CIRC_BUFF_SZ)
   {
	   UART_cntxt.rxCircBuffOld ++;
	   if (UART_cntxt.rxCircBuffOld >= UART_RX_CIRC_BUFF_SZ)
		   UART_cntxt.rxCircBuffOld = 0;
   }
   else
	   UART_cntxt.rxCircBuffEntries ++;

#if 0
   // Customer specific
   if (UART_cntxt.rxCircBuffEntries >= 12)
   {
	   IE2 &= ~UCA0RXIE;  // Disable rx interrupt
	   PLTFRM_exitLPM();
   }
#endif
}
#else
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
#ifdef ECLIPSE_IDE
static void __attribute__((__interrupt__(USCIAB0RX_VECTOR))) UART_usci0RxISR (void)
#else
#pragma vector=USCIAB0RX_VECTOR
__interrupt void UART_usci0RxISR(void)
#endif
{
   UINT8_t UART_rxByte;

   /*
    * Note that interrupts are disabled at this point since GIE bit in SR is
    * zero.
    */

   /*
    * The UCAxRXIFG interrupt flag is set each time a character is received
    * and loaded into UCAxRXBUF. An interrupt request is generated if UCAxRXIE
    * and GIE are also set. UCAxRXIFG is automatically reset when UCAxRXBUF is
    * read.
    *
    * Note that the USCI_Ax and USCI_Bx share the same interrupt vectors. The
    * receive interrupt flags UCAxRXIFG and UCBxRXIFG are routed to one interrupt
    * vector, the transmit interrupt flags UCAxTXIFG and UCBxTXIFG share another
    * interrupt vector.
    */

#ifdef UART_STATS_EN
   UART_cntxt.UART_rxIntCnt ++;
#endif

#if 0
   if (!(IFG2 & UCA0RXIFG))
   {
#ifdef UART_STATS_EN
       if (IFG2 & UCB0RXIFG)
           UART_cntxt.UART_ucb0RxIntCnt ++;
#endif
       return;
   }
#endif

   // UCAxRXIFG is automatically reset when UCAxRXBUF is read
   UART_rxByte = UCA0RXBUF;

   if (UART_cntxt.rxFrameOff < UART_FRAME_HDR_LEN)
   {
       if (UART_cntxt.rxFrameOff == 0x0)
           UART_cntxt.currRcvdFrameSeqNr ++;

       UART_cntxt.rxHdrBuff[UART_cntxt.rxFrameOff] = UART_rxByte;
       UART_cntxt.rxFrameOff ++;
       if (UART_cntxt.rxFrameOff >= UART_FRAME_HDR_LEN)
       {
#ifdef UART_STATS_EN
           UART_cntxt.hdrRcvdCnt ++;
#endif
           SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_UART_FRAME_HDR_RCVD;
           IE2 &= ~UCA0RXIE;  // Disable rx interrupt
           PLTFRM_exitLPM();  // rkris, dec/30/2013 - wake up micro if sleeping in LPM3
       }
   }
   else
   {
       // Receiving payload
       UART_cntxt.rxFrameBuff_p[UART_cntxt.rxFrameOff - UART_FRAME_HDR_LEN] = UART_rxByte;
       UART_cntxt.rxFrameOff ++;
       if (UART_cntxt.rxFrameOff >= (UART_cntxt.rxFramePyldLen + UART_FRAME_HDR_LEN))
       {
           SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_UART_FRAME_PYLD_RCVD;
           IE2 &= ~UCA0RXIE;  // Disable rx interrupt
           PLTFRM_exitLPM();  // rkris, dec/30/2013 - wake up micro if sleeping in LPM3
       }
   }
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void UART_buildMsgHdr(UINT16_t msgType, UINT8_t *buff_p, UINT16_t pyldLen)
{
   UINT16_t calcCrc;

   UTIL_htons(buff_p, msgType);

   buff_p[UART_FRAME_HDR_FLAGS_FIELD_OFF] = 0x0;

   buff_p[UART_FRAME_HDR_SEQ_NR_FIELD_OFF] = UART_cntxt.txSeqNr;
   UART_cntxt.txSeqNr ++;

   UTIL_htons(buff_p + UART_FRAME_HDR_PYLD_LEN_FIELD_OFF, pyldLen);

   calcCrc = PLTFRM_calcCkSum16(buff_p, UART_FRAME_HDR_LEN - UART_FRAME_HDR_CRC_FIELD_LEN*2);
   UTIL_htons(buff_p + UART_FRAME_HDR_HDR_CRC_FIELD_OFF, calcCrc);

   if (pyldLen > 0)
       calcCrc = PLTFRM_calcCkSum16(buff_p + UART_FRAME_HDR_LEN, pyldLen);
   else
       calcCrc = 0x0;
   UTIL_htons(buff_p + UART_FRAME_HDR_PYLD_CRC_FIELD_OFF, calcCrc);

   return;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void UART_schedAckTx(UINT8_t sts)
{
    UINT8_t bdHndl;
    UINT8_t *rxHdr_p = UART_cntxt.rxHdrBuff;

    // ACK is sent with message type 0x0, received sequence number and no
    // payload.

    bdHndl = SYS_allocMem(SYS_TRSCN_Q_HNDL_FIELD_LEN + UART_FRAME_HDR_LEN);
    if (SYS_BD_HNDL_IS_VALID(bdHndl))
    {
        UINT8_t *respHdr_p = SYS_GET_TRSCN_Q_BD_BUFF(bdHndl);
        UINT16_t calcCrc;

        UTIL_htons(respHdr_p, UART_MSG_TYPE_ACK);

        respHdr_p[UART_FRAME_HDR_FLAGS_FIELD_OFF] = sts;

        respHdr_p[UART_FRAME_HDR_SEQ_NR_FIELD_OFF] = rxHdr_p[UART_FRAME_HDR_SEQ_NR_FIELD_OFF];

        UTIL_htons(respHdr_p + UART_FRAME_HDR_PYLD_LEN_FIELD_OFF, 0x0);  // no payload

        calcCrc = PLTFRM_calcCkSum16(respHdr_p, UART_FRAME_HDR_LEN - UART_FRAME_HDR_CRC_FIELD_LEN*2);
        UTIL_htons(respHdr_p + UART_FRAME_HDR_HDR_CRC_FIELD_OFF, calcCrc);

        UTIL_htons(respHdr_p + UART_FRAME_HDR_PYLD_CRC_FIELD_OFF, 0x0);

        UART_addToTxPendQ(bdHndl);

#ifdef UART_STATS_EN
        UART_cntxt.ackMemAllocOk ++;
#endif
    }
    else
    {
#ifdef UART_STATS_EN
        UART_cntxt.ackMemAllocFlr ++;
#endif
    }

    return;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void UART_checkRcvdFramePyld(void)
{
   UINT8_t *hdr_p = UART_cntxt.rxHdrBuff;
   UINT8_t *pyld_p = UART_cntxt.rxFrameBuff_p;
   UINT16_t calcCkSum, rcvdCkSum;

   if (UART_cntxt.rxFramePyldLen == 0x0
       || (!(SYS_BD_HNDL_IS_VALID(UART_cntxt.rxFrameBdHndl))))
   {
       SYS_fatal(SYS_FATAL_ERR_260);
   }

   calcCkSum = PLTFRM_calcCkSum16(pyld_p, UART_cntxt.rxFramePyldLen);
   rcvdCkSum = UTIL_ntohs(hdr_p + UART_FRAME_HDR_PYLD_CRC_FIELD_OFF);

   if (calcCkSum == rcvdCkSum)
   {
       UINT16_t msgType = UTIL_ntohs(hdr_p);
#ifdef UART_STATS_EN
       UART_cntxt.pyldCrcOkCnt ++;
#endif
       // Called function frees rxFrameBdHndl
#if defined(LPWMN_COORD) || defined(DEV_TYPE_SNIFFER)
       GW_rcvdMsgHndlr(msgType, UART_cntxt.rxFrameBdHndl);
#else
       NHI_procRcvdMsg(msgType, UART_cntxt.rxFrameBdHndl);
#endif
   }
   else
   {
       // Send NACK
       SYS_freeMem(UART_cntxt.rxFrameBdHndl);
       UART_schedAckTx(UART_ACK_STS_INV_CRC);
#ifdef UART_STATS_EN
       UART_cntxt.pyldCrcBadCnt ++;
#endif
   }
   
   UART_cntxt.rxFrameOff = 0;
   UART_cntxt.rxFramePyldLen = 0;
   UART_cntxt.rxFrameBdHndl = SYS_INV_BD_HDNL;

   IE2 |= UCA0RXIE;  // Enable rx interrupt
   
   return;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void UART_checkRcvrState(void)
{
   if (UART_cntxt.rxFrameOff > 0x0
       && UART_cntxt.currRcvdFrameSeqNr == UART_cntxt.lastCheckedRcvdFrameSeqNr)
   {
       // Frame not received completely and sequence number is same
       // Looks like some bytes are missing !!
       IE2 &= ~UCA0RXIE;  // Disable rx interrupt
       UART_cntxt.rcvrStuckFlag = 0x1;
       SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_UART_FRAME_HDR_RCVD;
       return;
   }

   UART_cntxt.lastCheckedRcvdFrameSeqNr = UART_cntxt.currRcvdFrameSeqNr;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void UART_checkRcvdFrameHdr(void)
{
   UINT8_t sts = UART_HDR_ACK_BM, errFlag = 0x1;
   UINT8_t *hdr_p = UART_cntxt.rxHdrBuff;
   UINT16_t calcCkSum, rcvdCkSum;

   if (UART_cntxt.rcvrStuckFlag)
   {
       // Some bytes in the header or payload are missing !!

       if (UART_cntxt.rxFrameOff >= UART_FRAME_HDR_LEN)
       {
           sts = UART_ACK_STS_PYLD_BYTES_MISSING;
           if (SYS_BD_HNDL_IS_VALID(UART_cntxt.rxFrameBdHndl))
           {
               SYS_freeMem(UART_cntxt.rxFrameBdHndl);
               UART_cntxt.rxFrameBdHndl = SYS_INV_BD_HDNL;
           }
           else
           {
               SYS_fatal(SYS_FATAL_ERR_261);
           }
       }
       else
       {
           sts = UART_ACK_STS_HDR_BYTES_MISSING;
       }

       UART_cntxt.rxFrameOff = 0x0;
       UART_cntxt.rxFramePyldLen = 0x0;
       UART_cntxt.currRcvdFrameSeqNr ++;
       UART_cntxt.rcvrStuckFlag = 0x0;

       UART_schedAckTx(sts);

       IE2 |= UCA0RXIE;  // Enable rx interrupt

       return;
   }

   calcCkSum = PLTFRM_calcCkSum16(hdr_p,
                                  UART_FRAME_HDR_LEN - UART_FRAME_HDR_CRC_FIELD_LEN*2);
   rcvdCkSum = UTIL_ntohs(hdr_p + UART_FRAME_HDR_HDR_CRC_FIELD_OFF);
   if (calcCkSum == rcvdCkSum)
   {
       UINT16_t msgType = UTIL_ntohs(hdr_p);

#ifdef UART_STATS_EN
       UART_cntxt.hdrCrcOkCnt ++;
#endif
       UART_cntxt.rxFramePyldLen = UTIL_ntohs(hdr_p + UART_FRAME_HDR_PYLD_LEN_FIELD_OFF);

       if (UART_cntxt.rxFramePyldLen == 0x0) 
       {
#if defined(LPWMN_COORD) || defined(DEV_TYPE_SNIFFER)
           GW_rcvdMsgHndlr(msgType, SYS_INV_BD_HDNL);
#else
           NHI_procRcvdMsg(msgType, SYS_INV_BD_HDNL);
#endif
           UART_cntxt.rxFrameOff = 0x0;
           errFlag = 0x0;
           // memset(UART_cntxt.rxHdrBuff, 0xCC, sizeof(UART_cntxt.rxHdrBuff));
       }
       else
       {
           if (UART_cntxt.rxFramePyldLen < UART_FRAME_MAX_PYLD_LEN)
           {
#ifdef LPWMN_COORD
               if (msgType != LPWMN_GW_MSG_TYPE_RELAY_TO_NODE
                   || (!(SYS_BD_HNDL_IS_VALID(GW_cntxt.relayFrameBdHndl))))
               {
                   UART_cntxt.rxFrameBdHndl = SYS_allocMem(UART_cntxt.rxFramePyldLen);
                   if (SYS_BD_HNDL_IS_VALID(UART_cntxt.rxFrameBdHndl))
                   {
                       UART_cntxt.rxFrameBuff_p = SYS_GET_BD_BUFF(UART_cntxt.rxFrameBdHndl);
                       sts |= UART_ACK_STS_OK_BM;
                       errFlag = 0x0;
                   }
                   else
                   {
                       sts |= UART_ACK_STS_OOM_BM;
                   }
               }
               else
               {
                   sts |= UART_ACK_STS_RELAY_IN_PROGRESS;
               }
#else
               if (1)
               {
                   UART_cntxt.rxFrameBdHndl = SYS_allocMem(UART_cntxt.rxFramePyldLen);
                   if (SYS_BD_HNDL_IS_VALID(UART_cntxt.rxFrameBdHndl))
                   {
                       UART_cntxt.rxFrameBuff_p = SYS_GET_BD_BUFF(UART_cntxt.rxFrameBdHndl);
                       sts |= UART_ACK_STS_OK_BM;
                       errFlag = 0x0;
                   }
                   else
                   {
                       sts |= UART_ACK_STS_OOM_BM;
                   }
               }
#endif
           }
           else
           {
               sts |= UART_ACK_STS_FRAME_TOO_LONG_BM;
           }
       }
   }
   else
   {
       sts |= UART_ACK_STS_INV_CRC;
#ifdef UART_STATS_EN
       UART_cntxt.hdrCrcBadCnt ++;
#endif
   }

   if ((errFlag) || (UART_cntxt.rxFramePyldLen > 0))
       UART_schedAckTx(sts);

   if (errFlag)
   {
       UART_cntxt.rxFrameOff = 0x0;
       UART_cntxt.rxFramePyldLen = 0x0;
   }

   IE2 |= UCA0RXIE;  // Enable rx interrupt

   return;
}

/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
void UART_procPendingTxReqs(void)
{
   SYS_trscnQ_s *trscnQ_p = &(UART_cntxt.pendTxtrscnQ);

   if (SYS_TRSCN_Q_IS_NOT_EMPTY(trscnQ_p))
   {
       UINT8_t *buff_p;
       UINT8_t bdHndl = SYS_GET_TRSCN_Q_HEAD(trscnQ_p);
       buff_p = SYS_GET_TRSCN_Q_BD_BUFF(bdHndl);
       UART_wrBlock(buff_p, SYS_GET_BD_LEN(bdHndl) - SYS_TRSCN_Q_HNDL_FIELD_LEN);
   }

   return;
}

/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
void UART_addToTxPendQ(UINT8_t bdHndl)
{
   SYS_trscnQ_s *trscnQ_p = &(UART_cntxt.pendTxtrscnQ);
   UINT8_t qEmpty = SYS_TRSCN_Q_IS_EMPTY(trscnQ_p) ? 0x1 : 0x0;

   // The message being currently transmitted is still at the head of the pendTxtrscnQ.
   // It will be removed only after the message has been completely transmitted.

   SYS_addToTrscnsQ(trscnQ_p, bdHndl);
   if (qEmpty)
       UART_procPendingTxReqs();

   return;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void UART_blockTxDoneEvtHndlr(void)
{
   SYS_trscnQ_s *trscnQ_p = &(UART_cntxt.pendTxtrscnQ);

   if (SYS_TRSCN_Q_IS_NOT_EMPTY(trscnQ_p))
   {
       UINT8_t bdHndl = SYS_GET_TRSCN_Q_HEAD(trscnQ_p);

       SYS_delFromTrscnsQ(trscnQ_p, bdHndl);
       SYS_freeMem(bdHndl);

       UART_cntxt.txBuffLen = 0x0;
       UART_cntxt.txBuffOff = 0x0;
       UART_cntxt.txBuff_p = (void *)0;

       // Check if there is a message waiting to be transmitted
       UART_procPendingTxReqs();
   }
   else
   {
       SYS_fatal(SYS_FATAL_ERR_262);
   }

   return;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void UART_pollTx(void)
{
   if (UART_cntxt.txBuffLen == 0)
       SYS_fatal(SYS_FATAL_ERR_263);

   /*
    * The UCAxTXIFG interrupt flag is set by the transmitter to indicate
    * that UCAxTXBUF is ready to accept another character. An interrupt
    * request is generated if UCAxTXIE and GIE are also set. UCAxTXIFG is
    * automatically reset if a character is written to UCAxTXBUF. UCAxTXIFG
    * is set after a PUC or when UCSWRST = 1. UCAxTXIE is reset after a PUC
    * or when UCSWRST = 1.
    */

#ifdef UART_FLOW_CONTROL_ON_TX
   if (UART_cntxt.flowIntPending)
       return;
#endif

   // Check the UCBUSY flag. Very important !!. Just checking for 
   // UCA0TXIFG is not enough.

   if ((!(UCA0STAT & UCBUSY)) && (IFG2 & UCA0TXIFG))
   {
       // UCAxTXBUF ready to receive another byte

       UART_cntxt.txBuffOff ++;
       if (UART_cntxt.txBuffOff < UART_cntxt.txBuffLen)
       {
           // UCAxTXIFG is automatically reset if a character is written to
           // UCAxTXBUF.
           UART_wrByte(UART_cntxt.txBuff_p[UART_cntxt.txBuffOff]);

#ifdef UART_FLOW_CONTROL_ON_TX
           if (UART_cntxt.txBuffOff >= (UART_FRAME_HDR_LEN - 1))
           {
               if (UART_cntxt.txBuffLen > UART_FRAME_HDR_LEN)
               {
                  // Just transmitted last header byte ...
                  // Wait for flow interrupt from receiver before sending payload.
                  UART_cntxt.flowIntPending = 1;
               }
           }
#endif

       }
       else
       {
#ifdef UART_STATS_EN
           UART_cntxt.frameTxDoneCnt ++;
#endif

#ifdef UART_TX_INT_ENA
           UART_cntxt.txBuffLen = 0;
           SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_UART_BLOCK_TX_DONE;
#else
           UART_blockTxDoneEvtHndlr( );
#endif
       }
   }

   return;
}
#endif

UINT8_t uartTestChar = 0;

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t UART_wrBlock(UINT8_t *buff_p,
                     UINT16_t buffLen)
{
   if (buff_p == ((void *)0) || buffLen == 0x0)
   {
       SYS_fatal(SYS_FATAL_ERR_264);
       return UART_STS_INV_PARAMS;
   }

   if (UART_cntxt.txBuffLen > 0)
       SYS_fatal(SYS_FATAL_ERR_265);

   UART_cntxt.txBuff_p = buff_p;
   UART_cntxt.txBuffLen = buffLen;

#if 0
   {
	  UINT16_t idx;
	  for (idx=0; idx<buffLen; idx++)
		  buff_p[idx] = uartTestChar + 0xa;
	  uartTestChar ++;
   }
#endif

   UART_cntxt.txBuffOff = 0x0;

#ifdef UART_STATS_EN
   UART_cntxt.frameTxStartCnt ++;
#endif

   // Send out the first byte
#ifdef UART_STATS_EN
   UART_cntxt.byteTxCnt ++;
#endif
   UART_wrByte(UART_cntxt.txBuff_p[0]);

   return UART_STS_TX_IN_PROGRESS;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void UART_start(UINT8_t devId)
{
   UINT8_t UART_rxByte = UCA0RXBUF;

   (void)devId;
   (void)UART_rxByte;

   /*
    * USCI_Ax - (UART/SPI/IrDA/LIN)
    * USCI_Bx - (SPI/I2C)
    */

   IFG2 &= ~(UCA0RXIFG);
#ifdef UART_TX_INT_ENA
   IFG2 &= ~(UCA0TXIFG);
#endif


   // IFG2 &= ~(UCA0TXIFG | UCA0RXIFG);
   // IFG2 &= ~(UCB0TXIFG | UCB0RXIFG);

   IE2 |= UCA0RXIE;  // Enable USCI_A0 RX interrupt

#ifdef UART_TX_INT_ENA
   IE2 |= UCA0TXIE;  // Enable USCI_A0 TX interrupt
#endif

#ifdef UART_FLOW_CONTROL_ON_TX
   PLTFRM_gpioPinIntCtrlOpn(PLTFRM_GPIO_PORT_2, PLTFRM_GPIO_PORT_PIN_0,
                            PLTFRM_ENABLE_GPIO_PIN_INT, 0x1);
#endif

   return;
}




#ifdef UART_FLOW_CONTROL_ON_TX
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void UART_flowCntrlIntHndlr(void)
{
   UART_cntxt.flowCntrlIntCnt ++;
   UART_cntxt.flowIntPending = 0;

#ifdef UART_TX_INT_ENA
   UART_cntxt.txBuffOff ++;
   UART_wrByte(UART_cntxt.txBuff_p[UART_cntxt.txBuffOff]);
#endif
}
#endif

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void UART_init(void)
{
   memset(&UART_cntxt, 0, sizeof(UART_cntxt));

#ifdef UART_FLOW_CONTROL_ON_TX
   {
      PLTFRM_sts_t sts;
      sts = PLTFRM_allocGPIOPin(PLTFRM_UART_HW_1_DEV_ID,
                                PLTFRM_GPIO_PORT_2,
                                PLTFRM_GPIO_PORT_PIN_0,
                                PLTFRM_GPIO_PIN_DIRECTION_INPUT,
                                PLTFRM_GPIO_OUTPUT_NA,
                                PLTFRM_GPIO_INT_EDGE_NEGATIVE,
                                UART_flowCntrlIntHndlr);
      if (sts != PLTFRM_STS_SUCCESS)
          SYS_fatal(SYS_FATAL_ERR_838);

      UART_cntxt.flowIntPending = 0;
   }
#endif


   /*
    * USCI_Ax - (UART/SPI/IrDA/LIN)
    * USCI_Bx - (SPI/I2C)
    */
#ifndef DEV_TYPE_RFD
   UART_cntxt.rxFrameBdHndl = SYS_INV_BD_HDNL;
   SYS_initTrscnsQ(&(UART_cntxt.pendTxtrscnQ));
#endif

   /*
    * USCI_Ax - (UART/SPI/IrDA/LIN)
    * USCI_Bx - (SPI/I2C)
    */

   /*
    * Using the USCI_A0 USCI module.
    *
    * USCI Introduction: UART Mode
    * In asynchronous mode, the USCI_Ax modules connect the MSP430 to an
    * external system via two external pins, UCAxRXD and UCAxTXD. UART mode
    * is selected when the UCSYNC bit is cleared.
    */


#ifdef __MSP430G2553__
   // P1.1 - UCA0RXD
   // P1.2 - UCA0TXD
   P1SEL |= BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2 = TXD
   P1SEL2 |= BIT1 + BIT2;
   P1DIR &= ~BIT1;                           // P1.1 = RXD
   P1DIR |= BIT2;                            // P1.2 = TXD
#elif defined(__MSP430G2955__) || defined(__MSP430F2274)
#ifdef GPIO_MGMT_ENA
   {
	  PLTFRM_sts_t sts;

      // P3.5 - UCA0RXD
      // P3.4 - UCA0TXD
      sts = PLTFRM_reserveGPIOPin(PLTFRM_UART_HW_1_DEV_ID,
                                  PLTFRM_GPIO_PORT_3,
                                  PLTFRM_GPIO_PORT_PIN_4);
      if (sts != PLTFRM_STS_SUCCESS)
          SYS_fatal(SYS_FATAL_ERR_520);

      sts = PLTFRM_reserveGPIOPin(PLTFRM_UART_HW_1_DEV_ID,
                                  PLTFRM_GPIO_PORT_3,
                                  PLTFRM_GPIO_PORT_PIN_5);
      if (sts != PLTFRM_STS_SUCCESS)
          SYS_fatal(SYS_FATAL_ERR_521);
   }
#endif
   P3SEL |= BIT4 + BIT5;
#ifdef __MSP430G2955__
   P3SEL2 &= ~(BIT4 + BIT5);
#endif
#else
#error micro variant not specified !!
#endif

#ifdef DEV_TYPE_RFD
   UCA0CTL1 |= UCSSEL_1;        // CLK = ACLK
#ifdef  UART_BAUD_RATE_9600
   UCA0BR0 = 0x3;               // 32kHz/9600 = 3.41
   UCA0BR1 = 0x0;
#elif defined(UART_BAUD_RATE_4800)
   UCA0BR0 = 0x6;               // 32kHz/4800 = 6.66666
   UCA0BR1 = 0x0;
#else
#error UART baud rate not specified !!
#endif
   UCA0MCTL = UCBRS0 + UCBRS1;  // Modulation UCBRSx = 3
#else

   UCA0CTL1 |= UCSSEL_2;      // CLK = SMCLK (8 or 16 MHz)

#ifdef UART_BAUD_RATE_9600
#if (CPU_ACTIVE_CLOCK_IN_MHZ == 8)
   UCA0BR1 = 3;
   UCA0BR0 = 65;
   UCA0MCTL = UCBRS1;         // Modulation UCBRSx
#elif (CPU_ACTIVE_CLOCK_IN_MHZ == 16)
   UCA0BR1 = 6;
   UCA0BR0 = 82;
   UCA0MCTL = UCBRS1;         // Modulation UCBRSx
#else
#error not supported !!
#endif
#elif defined(UART_BAUD_RATE_19200)
#if (CPU_ACTIVE_CLOCK_IN_MHZ == 8)
   UCA0BR1 = 0x01;         // 19200 (19230)
   UCA0BR0 = 0xa0;
   UCA0MCTL = UCBRS0;      // Modulation UCBRSx
#elif (CPU_ACTIVE_CLOCK_IN_MHZ == 16)
   UCA0BR1 = 3;            // 19200 (19207)
   UCA0BR0 = 65;
   UCA0MCTL = UCBRS1;      // Modulation UCBRSx
#else
#error not supported !!
#endif
#elif defined(UART_BAUD_RATE_38400)
#if (CPU_ACTIVE_CLOCK_IN_MHZ == 8)
   UCA0BR1 = 0x00;            // 38400 ((8*10^6 / 208) -> 38461)
   UCA0BR0 = 0xd0;
   UCA0MCTL = UCBRS0;         // Modulation UCBRSx
#elif (CPU_ACTIVE_CLOCK_IN_MHZ == 16)
   UCA0BR1 = 1;               // 38400 ((16*10^6 / 417) -> 38369.3)
   UCA0BR0 = 0xa0;
   UCA0MCTL = UCBRS0;         // Modulation UCBRSx
#else
#error not supported !!
#endif
#elif defined(UART_BAUD_RATE_56000)
   UCA0BR1 = 0;               // 56000
   UCA0BR0 = 142;
   UCA0MCTL = UCBRS2 + UCBRS1 + UCBRS0;  // Modulation UCBRSx = 7
#elif defined(UART_BAUD_RATE_115200)
   UCA0BR1 = 0;               // 56000
   UCA0BR0 = 69;
   UCA0MCTL = UCBRS2;  // Modulation UCBRSx = 4
#else
#error UART baud rate not specified !!
#endif
#endif
   /*
    * The USCI module is enabled by clearing the UCSWRST bit and the transmitter
    * is ready and in an idle state. The transmit baud rate generator is ready but
    * is not clocked nor producing any clocks.
    */
   UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**

   /*
    * The USCI has one interrupt vector for transmission and one interrupt
    * vector for reception.
    *
    * USCI_Ax and USCI_Bx share the same interrupt vectors. The receive interrupt
    * flags UCAxRXIFG and UCBxRXIFG are routed to one interrupt vector, the transmit
    * interrupt flags UCAxTXIFG and UCBxTXIFG share another interrupt vector.
    *
    * UCA0TXIFG Bit 1 : USCI_A0 transmit interrupt flag.
    *                   UCA0TXIFG is set when UCA0TXBUF is empty.
    *                   UCAxTXIFG is automatically reset if a character is written to UCAxTXBUF.
    *                   UCAxTXIFG is set after a PUC or when UCSWRST = 1.
    *                    - 0 No interrupt pending
    *                    - 1 Interrupt pending
    *
    * UCA0RXIFG Bit 0 : USCI_A0 receive interrupt flag.
    *                   UCA0RXIFG is set when UCA0RXBUF has received a complete character.
    *                   UCAxRXIFG is automatically reset when UCAxRXBUF is read.
    *                   UCAxRXIFG is reset by a system reset PUC signal or when UCSWRST = 1.
    *                    - 0 No interrupt pending
    *                    - 1 Interrupt pending
    */

   IFG2 &= ~(UCA0RXIFG);

   IE2 &= ~UCA0RXIE;  // Disable USCI_A0 RX interrupt
   IE2 &= ~UCA0TXIE;  // Disable USCI_A0 RX interrupt

   // IFG2 &= ~(UCA0TXIFG | UCA0RXIFG);
   // IFG2 &= ~(UCB0TXIFG | UCB0RXIFG);

}
#endif
#endif
