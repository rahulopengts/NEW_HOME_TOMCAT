/*
 * File Name: cc2520.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Aug/2/2013
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


#ifdef RADIO_CC2520


#include <typedefs.h>
#ifdef __IAR__
#include "io430.h"
#else
#endif
#include <pltfrm.h>
#include <cc2520.h>
#include <msp430.h>
#include <radio.h>
#include <mac.h>
#include <system.h>
#include <phy_defs.h>
#include <phy_pib.h>



#define CC2520_INTR_STATS_ENA
// #define CC2520_STATS_ENA

#ifdef CC2520_INTR_STATS_ENA
UINT8_t rxOverFlowCnt = 0, rxDoneCnt = 0, txDoneCnt = 0;
#endif


#define HI_UINT16(a) (((unsigned int)(a) >> 8) & 0xFF)
#define LO_UINT16(a) ((unsigned int)(a) & 0xFF)


const cc2520_txPowerEnc_s cc2520_txPowerEncTable[CC2520_TX_POWER_ENC_TBL_SZ] =
{
  {5,    CC2520_TXPOWER_PLUS_5_DBM},
  {3,    CC2520_TXPOWER_PLUS_3_DBM},
  {2,    CC2520_TXPOWER_PLUS_2_DBM},
  {1,    CC2520_TXPOWER_PLUS_1_DBM},
  {0,         CC2520_TXPOWER_0_DBM},
  {(SINT8_t)-2,   CC2520_TXPOWER_MINUS_2_DBM},
  {(SINT8_t)-4,   CC2520_TXPOWER_MINUS_4_DBM},
  {(SINT8_t)-7,   CC2520_TXPOWER_MINUS_7_DBM},
  {(SINT8_t)-18,  CC2520_TXPOWER_MINUS_18_DBM}
};


/*
 * CSn is an input enable signal for the SPI and is controlled 
 * by the external MCU. The CSn signal is used as an asynchronous 
 * active high reset to the SPI module.
 * CSn must be held low during all SPI operations and must also be 
 * held low for more than two periods of XOSC before the first 
 * positive edge of SCLK and more than two periods of XOSC after 
 * the last negative edge of SCLK.
 * When CSn is high it must be held high for at least 2 periods of 
 * XOSC.
 */

#ifdef WISENSE_HW_REV_2

#define cc2520_spiBegin( )  \
do \
{  \
   P1OUT &= ~(0x10);  \
} while (0)

#define cc2520_spiEnd( )  \
do \
{  \
   P1OUT |= (0x10);  \
} while (0)

#elif defined (WISENSE_HW_REV_3) || (WISENSE_HW_REV_4)

#define cc2520_spiBegin( )  \
do \
{  \
   P3OUT &= ~(0x80);  \
} while (0)

#define cc2520_spiEnd( )  \
do \
{  \
   P3OUT |= (0x80);  \
} while (0)

#elif defined (WISENSE_HW_REV_5) || defined (WISENSE_HW_REV_6)

#define cc2520_spiBegin( ) \
do \
{  \
   P4OUT &= ~(BIT0);  \
} while (0)

#define cc2520_spiEnd( )  \
do \
{  \
   P4OUT |= (BIT0);  \
} while (0)
#else
#error hardware revision not specified !!
#endif




#ifdef WISENSE_HW_REV_2
/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
void cc2520_spiInit(void)
{
  // P1.7    UCB0SIMO
  // P1.6    UCB0SOMI
  // P1.4    UCB0STE
  // P1.5    UCB0CLK  /  UCA0STE
  
  P1DIR |= 0x10;  // CC2520 CSn
  P1DIR |= 0x20;  // CC2520 CLK
  P1DIR |= 0x80;  // CC2520 SIMO
  P1DIR &= ~(0x40); // CC2520 SOMI  
  P1SEL = 0xe0;  // 7, 6 and 5 
  P1SEL2 = 0xe0;
  P1OUT |= 0x10; // CSn high
  P1OUT &= ~(0x20 | 0x80);  // CLK and SIMO low
  
  
  UCB0CTL1 |= UCSWRST;
  UCB0CTL0 |= UCCKPL + UCMSB + UCMST + UCSYNC; // 3-pin, 8-bit SPI master
  UCB0CTL1 |= UCSSEL_2;                        // SMCLK (16 MHz)
  UCB0BR0 |= 0x02;                             // 16/2 MHz
  UCB0BR1 = 0;                                 //
  // UCB0MCTL = 0;                             // No modulation
  UCB0CTL1 &= ~UCSWRST;                        // **Initialize USCI state machine**
  // IE2 |= UCB0RXIE;                          // Enable USCI0 RX interrupt
  
}
#elif defined(WISENSE_HW_REV_3) || defined(WISENSE_HW_REV_4)
void cc2520_spiInit(void)
{
  // P1.7    UCB0SIMO
  // P1.6    UCB0SOMI
  // P1.5    UCB0CLK  /  UCA0STE
  
  // P3.7    CSn
  // P3.0    VREG_EN
  // P3.1    RESETn

  P1DIR |= 0x20;  // CC2520 CLK
  P1DIR |= 0x80;  // CC2520 SIMO
  P1DIR &= ~(0x40); // CC2520 SOMI  
  P1SEL = 0xe0;  // 7, 6 and 5 
  P1SEL2 = 0xe0;  
  
  P3DIR |= 0x80; // CC2520 CSn
  P3OUT |= 0x80; // CSn high
  
  P1OUT &= ~(0x20 | 0x80);  // CLK and SIMO low
  
  UCB0CTL1 |= UCSWRST;
  UCB0CTL0 |= UCCKPL + UCMSB + UCMST + UCSYNC; // 3-pin, 8-bit SPI master
  UCB0CTL1 |= UCSSEL_2;                        // SMCLK (16 MHz)
  UCB0BR0 |= 0x02;                             // 16/2 MHz
  UCB0BR1 = 0;                                 //
  // UCB0MCTL = 0;                             // No modulation
  UCB0CTL1 &= ~UCSWRST;                        // **Initialize USCI state machine**
  // IE2 |= UCB0RXIE;                          // Enable USCI0 RX interrupt
  
}
#elif defined(WISENSE_HW_REV_5) || defined(WISENSE_HW_REV_6)
void cc2520_spiInit(void)
{
  // P3.1    UCB0SIMO
  // P3.2    UCB0SOMI
  // P3.3    UCB0CLK  /  UCA0STE

  // P4.0    CSn
  // P4.2    VREG_EN
  // P4.1    RESETn

  // P3DIR |= BIT3;  // CC2520 CLK
  // P3DIR |= BIT1;  // CC2520 SIMO
  // P3DIR &= ~(BIT2); // CC2520 SOMI

  P3SEL |= (BIT1 | BIT2 | BIT3);  // 3.1, 3.2 and 3.3

  P4DIR |= BIT0; // CC2520 CSn
  P4OUT |= BIT0; // CSn high

  // P3OUT &= ~(BIT3 | BIT1);  // CLK and SIMO low

  UCB0CTL1 |= UCSWRST;
  UCB0CTL0 |= UCCKPL + UCMSB + UCMST + UCSYNC; // 3-pin, 8-bit SPI master
  UCB0CTL1 |= UCSSEL_2;                        // SMCLK (16 MHz)
  UCB0BR0 |= 0x02;                             // 16/2 MHz
  UCB0BR1 = 0;                                 //
  // UCB0MCTL = 0;                             // No modulation
  UCB0CTL1 &= ~UCSWRST;                        // **Initialize USCI state machine**
  // IE2 |= UCB0RXIE;                          // Enable USCI0 RX interrupt

}
#else
#error hardware revision not specified !!
#endif

/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t cc2520_spiTxRx(UINT8_t val)
{
    UINT8_t rdVal, globalIntFlag;
    
    PLTFRM_intSaveDisable(globalIntFlag);

    // USCI busy. This bit indicates if a transmit or receive 
    // operation is in progress.
    //  > 0b = USCI inactive
    //  > 1b = USCI transmitting or receiving
    while ((UCB0STAT & UCBUSY));
    
    /*
     * If this line is uncommented, UART is getting stuck !!
     */
    // IFG2 &= ~(UCB0RXIFG);  // clear the char received bit
    
    // UCTXIFG is set when UCAxTXBUF empty. 
    // Data written to UCxTXBUF when UCTXIFG = 0 may result in 
    // erroneous data transmission.
    // A set transmit interrupt flag, UCTXIFG, indicates that data 
    // has moved from UCxTXBUF to the TX shift register and UCxTXBUF 
    // is ready for new data. It does not indicate RX/TX completion.
    while (!(IFG2 & UCB0TXIFG));     // USCI_A3 TX buffer ready?
    
    // UCTXIFG is automatically reset if a character is written to UCxTXBUF
    UCB0TXBUF = val;
    
    // UCRXIFG is set when UCAxRXBUF has received a complete character.
    // Data on UCxSOMI is shifted into the receive shift register on the 
    // opposite clock edge. When the character is received, the receive 
    // data is moved from the receive (RX) shift register to the received 
    // data buffer UCxRXBUF and the receive interrupt flag UCRXIFG is set, 
    // indicating the RX/TX operation is complete
    while (!(IFG2 & UCB0RXIFG)); 
    
    // UCRXIFG is automatically reset when UCxRXBUF is read.
    rdVal = UCB0RXBUF;
    
    PLTFRM_intRestore(globalIntFlag);

    return rdVal;
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
void cc2520_spiTx(UINT8_t val)
{
   while ((UCB0STAT & UCBUSY));
   while (!(IFG2 & UCB0TXIFG));    // USCI_B0 TX buffer ready?
   UCB0TXBUF = val;
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
UINT8_t cc2520_SpiRx(void)
{
   UINT8_t rdVal;
   rdVal = UCB0RXBUF;
   return rdVal;
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
void cc2520_spiWaitRxRdy(void)
{
   while (!(IFG2 & UCB0RXIFG)); 
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
void cc2520_insRdArray(UINT16_t count,
                       UINT8_t *data_p)
{
    while (count--) 
    {
        UINT8_t globalIntFlag;
    	PLTFRM_intSaveDisable(globalIntFlag);
        cc2520_spiTx(0x00);
        cc2520_spiWaitRxRdy();
        *data_p = (cc2520_SpiRx());
        PLTFRM_intRestore(globalIntFlag);
        data_p ++;
    }
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
void cc2520_insWrArray(UINT16_t count, 
                       UINT8_t *data_p)
{
    while (count--) 
    {
        UINT8_t globalIntFlag;
    	PLTFRM_intSaveDisable(globalIntFlag);
        cc2520_spiTx(*data_p);
        data_p ++;
        cc2520_spiWaitRxRdy();
        PLTFRM_intRestore(globalIntFlag);
    }
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
UINT8_t cc2520_memRead(UINT16_t addr, 
                       UINT16_t count, 
                       UINT8_t  *data_p)
{
    UINT8_t s;
    cc2520_spiBegin();
    PLTFRM_delay1MicroSecs();
    s = cc2520_spiTxRx(CC2520_INS_MEMRD | HI_UINT16(addr));
    // PLTFRM_delay1MicroSecs();
    cc2520_spiTxRx(LO_UINT16(addr));
    // PLTFRM_delay1MicroSecs();
    cc2520_insRdArray(count, data_p);
    PLTFRM_delay1MicroSecs();
    cc2520_spiEnd();
    //PLTFRM_delay1MicroSecs();
    return s;
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
UINT8_t cc2520_memWrite(UINT16_t addr, 
                        UINT16_t count, 
                        UINT8_t *data_p)
{
    UINT8_t s;
    
    cc2520_spiBegin();
    PLTFRM_delay1MicroSecs();
    s = cc2520_spiTxRx(CC2520_INS_MEMWR | HI_UINT16(addr));
    PLTFRM_delay1MicroSecs();
    cc2520_spiTxRx(LO_UINT16(addr));
    PLTFRM_delay1MicroSecs();
    cc2520_insWrArray(count, data_p);
    PLTFRM_delay1MicroSecs();
    cc2520_spiEnd();
    PLTFRM_delay1MicroSecs();
    return s;
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
void cc2520_bitClear(UINT8_t bitAddr)
{
    /* 
     * BCLR - A[7:3] B[2:0] S[7:0]
     * Clear a single bit. Writes 0 to bit B in address A. This is
     * done without affecting the value of, or triggering sideeffects
     * of other bits at the same address. Only the address range [0, 31] 
     * is accessible with this instruction.
     */
    cc2520_spiBegin();
    PLTFRM_delay1MicroSecs();
    cc2520_spiTxRx(CC2520_INS_BCLR);
    cc2520_spiTxRx(bitAddr);
    PLTFRM_delay1MicroSecs();
    cc2520_spiEnd();
    //PLTFRM_delay1MicroSecs();
    return ;
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
void cc2520_bitSet(UINT8_t bitAddr)
{
    /*
     * BSET - A[7:3] B[2:0] S[7:0]
     * Set a single bit. Writes 1 to bit B in address A. This is
     * done without affecting the value of, or triggering side effects
     * of other bits at the same address. Only the address range [0, 31] 
     * is accessible with this instruction.
     */  
    cc2520_spiBegin();
    PLTFRM_delay1MicroSecs();
    cc2520_spiTxRx(CC2520_INS_BSET);
    cc2520_spiTxRx(bitAddr);
    PLTFRM_delay1MicroSecs();
    cc2520_spiEnd();
    //PLTFRM_delay1MicroSecs();
    return ;
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
void cc2520_insStrobe(UINT8_t strobe)
{
    cc2520_spiBegin();
    PLTFRM_delay1MicroSecs();
    cc2520_spiTxRx(strobe);
    PLTFRM_delay1MicroSecs();
    cc2520_spiEnd();
    //PLTFRM_delay1MicroSecs();
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
UINT8_t cc2520_regRead8(UINT8_t addr)
{
    UINT8_t val;
    
    cc2520_spiBegin();
    
    PLTFRM_delay1MicroSecs();
    
    cc2520_spiTxRx(CC2520_INS_REGRD | addr);
    
    PLTFRM_delay1MicroSecs();
    
    val = cc2520_spiTxRx(0x00);
    
    PLTFRM_delay1MicroSecs();
    
    cc2520_spiEnd();
    
    //PLTFRM_delay1MicroSecs();
    
    return val;
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
void cc2520_writeToTxFifo(UINT8_t count, 
                          UINT8_t *data_p)
{
    cc2520_spiBegin();
    PLTFRM_delay1MicroSecs();
    cc2520_spiTxRx(CC2520_INS_TXBUF);
    cc2520_insWrArray(count, data_p);

    {
       int cnt;
       for (cnt=0; cnt<4; cnt++)  // 32
           PLTFRM_delay1MicroSecs();
    }
    
    cc2520_spiEnd();
    
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
void CC2520_readRxFifo(UINT8_t count, UINT8_t *data_p)
{
    /*
     * The RX FIFO memory area is located at addresses 0x180 to 0x1FF and is thus 
     * 128 bytes. The RX FIFO can contain more than one frame at a time.
     *
     * 20.4 RX FIFO Access
     * The RX FIFO can hold one or more received frames, provided that the total 
     * number of bytes is 128 or less. There are two ways to determine the number 
     * of bytes in the RX FIFO:
     *  > Reading RXFIFOCNT register
     *  > Using the FIFOP and FIFO signals in combination with the FIFOPCTRL.FIFOPTHR 
     *    setting.
     *
     * There are several ways to access the RX FIFO:
     *  > The RXBUF instruction transfers received bytes from CC2520 to the micro.
     */
    cc2520_spiBegin();
    PLTFRM_delay1MicroSecs();
    
    /*
     * Read the oldest byte in the RX FIFO. At the first data transfer the oldest 
     * byte in the RX FIFO is read and removed from the RX FIFO. This operation is 
     * repeated for subsequent SPI transfers.
     * If this instruction is performed when the RX FIFO is empty, an RX_UNDERFLOW 
     * exception is raised.
     * Note: Do not execute RXBUF while RXBUFMOV is in progress. It could result 
     * in loss of data.
     */
    cc2520_spiTxRx(CC2520_INS_RXBUF);
    
    cc2520_insRdArray(count, data_p);
    PLTFRM_delay1MicroSecs();
    cc2520_spiEnd();
    //PLTFRM_delay1MicroSecs();
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
void cc2520_hardRxStart(void)
{
   UINT8_t rssiStat = 0x0;
   
   /*
    * The CC2520 receiver is turned on and off with the SRXON and SRFOFF 
    * command strobes, and with the RXENABLE registers. The command strobes 
    * provide a "hard" on/off mechanism, while RXENABLE manipulation provides 
    * a "soft" on/off mechanism.
    *
    * - The SRXON strobe:
    *    > Sets RXENABLE[15]
    *    > Aborts ongoing transmission/reception by forcing a transition to RX 
    *      calibration.
    */
  
   cc2520_memRead(CC2520_RSSISTAT, 0x1, &rssiStat);    
  
   cc2520_insStrobe(CC2520_INS_SRXON);
   
   // waitCnt = 0;
    
   do
   {  
      // waitCnt ++;
#if 0      
      cc2520_memRead(CC2520_CHIPID, 0x1, &_MAC_dbgPreChipId);   
      cc2520_memRead(CC2520_FSMSTAT0, 0x1, &_MAC_dbgPreFSMSTAT0);
      cc2520_memRead(CC2520_FSMSTAT1, 0x1, &_MAC_dbgPreFSMSTAT1);
      cc2520_memRead(CC2520_TXFIFOCNT, 0x1, &_MAC_dbgPreTXFIFOCNT);
#endif    
      cc2520_memRead(CC2520_RSSISTAT, 0x1, &rssiStat);
      
      // cc2520_memRead(CC2520_RXENABLE1, 0x1, &____RXENABLE1);  
      
   } while (!(rssiStat & CC2520_RSSISTAT_RSSI_VALID_BIT_MSK));    
       
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
void cc2520_hardRxStop(void)
{
   UINT8_t fsmStat0;
   
   /*
    * The CC2520 receiver is turned on and off with the SRXON and SRFOFF 
    * command strobes, and with the RXENABLE registers. The command strobes 
    * provide a "hard" on/off mechanism, while RXENABLE manipulation provides 
    * a "soft" on/off mechanism.
    *
    * The "SRFOFF" command strobe aborts ongoing transmission/reception and 
    * forces the FSM to the IDLE state.  
    */
    
   cc2520_insStrobe(CC2520_INS_SRFOFF);

   do
   {  
      cc2520_memRead(CC2520_FSMSTAT0, 0x1, &fsmStat0);
   } while ((fsmStat0 & CC2520_FSMSTAT0_FSM_FFCTRL_STATE_BIT_MSK) != CC2520_RADIO_STATE_IDLE);
    
   return;
}

#if 0
/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
void cc2520_softRxStart(void)
{
   UINT8_t rssiStat;
   /*
    * The CC2520 receiver is turned on and off with the SRXON and SRFOFF 
    * command strobes, and with the RXENABLE registers. The command strobes 
    * provide a "hard" on/off mechanism, while RXENABLE manipulation provides 
    * a "soft" on/off mechanism.
    *
    * - Setting RXENABLE != 0x0000:
    *   > Does not abort ongoing transmission/reception.
    */
  
    cc2520_bitSet(CC2520_MAKE_BIT_ADDR(CC2520_RXENABLE0, 0x0));
    
   do
   {  
      // waitCnt ++;
      cc2520_memRead(CC2520_RSSISTAT, 0x1, &rssiStat);      
   } while (!(rssiStat & CC2520_RSSISTAT_RSSI_VALID_BIT_MSK));    
    
}
#endif


/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
void cc2520_transferPktBuffToTxFifo(UINT8_t *pktBuff_p,
                                    UINT16_t pktLen)
{
   UINT8_t txFifoCnt = 0,
           txFifoLenFld = pktLen + MAC_FCS_LEN;
  
   do
   {
      /*
       * The TX FIFO memory area is located at addresses 0x100 to 0x17F and is thus 
       * 128 bytes. The TX FIFO can only contain one frame at a time. The frame can 
       * be buffered before or after the TX command strobe is executed, as long as 
       * it does not generate TX underflow.
       * The number of bytes in the TX FIFO is stored in the TXFIFOCNT register. The 
       * TX FIFO can be emptied manually with the SFLUSHTX command strobe.
       */
     
      cc2520_insStrobe(CC2520_INS_SFLUSHTX);
      
      // Make sure tx fifo is empty
      cc2520_memRead(CC2520_TXFIFOCNT, 0x1, &txFifoCnt);
      if (txFifoCnt > 0)
      {
          SYS_fatal(SYS_FATAL_ERR_1);
          continue;
      }
      
      /*
       * Clear all transmit related exceptions.
       * Whenever an exception occurs this bit will be set by hardware. Only software
       * can clear this bit. 
       *
       * 14.1
       * EXCFLAGn register bits that are high can only be cleared by writing zero to 
       * the bit.
       */
      
      /*
       * When AUTOCRC = 1, the contents of the tx fifo should be:
       *---------------------------------------------------------
       * LEN (1 byte)  |  MAC PDU (LEN - 2 bytes)  | ignored
       *---------------------------------------------------------
       */
   
      cc2520_writeToTxFifo(1, &txFifoLenFld);
      
      cc2520_writeToTxFifo(pktLen, pktBuff_p);
     
      cc2520_memRead(CC2520_TXFIFOCNT, 0x1, &txFifoCnt);
   
      if (txFifoCnt == (1 + pktLen))
          break;  
      else
          SYS_fatal(SYS_FATAL_ERR_1);
   } while (1);
     
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
void cc2520_transferPktToTxFifo(UINT8_t bdHndl)
{
   UINT8_t txFifoCnt = 0,
           txFifoLenFld = SYS_GET_BD_LEN(bdHndl) + MAC_FCS_LEN;
  
   do
   {
      /*
       * The TX FIFO memory area is located at addresses 0x100 to 0x17F and is thus 
       * 128 bytes. The TX FIFO can only contain one frame at a time. The frame can 
       * be buffered before or after the TX command strobe is executed, as long as 
       * it does not generate TX underflow.
       * The number of bytes in the TX FIFO is stored in the TXFIFOCNT register. The 
       * TX FIFO can be emptied manually with the SFLUSHTX command strobe.
       */
     
      cc2520_insStrobe(CC2520_INS_SFLUSHTX);
      
      // Make sure tx fifo is empty
      cc2520_memRead(CC2520_TXFIFOCNT, 0x1, &txFifoCnt);
      if (txFifoCnt > 0)
      {
          SYS_fatal(SYS_FATAL_ERR_1);
          continue;
      }
      
      /*
       * Clear all transmit related exceptions.
       * Whenever an exception occurs this bit will be set by hardware. Only software
       * can clear this bit. 
       *
       * 14.1
       * EXCFLAGn register bits that are high can only be cleared by writing zero to 
       * the bit.
       */
      
#if 0      
      regVal = (~(CC2520_EXCFLAG0_TX_FRM_DONE_BIT
                  | CC2520_EXCFLAG0_TX_ACK_DONE
                  | CC2520_EXCFLAG0_TX_UNDERFLOW
                  | CC2520_EXCFLAG0_TX_OVERFLOW));
   
      cc2520_memWrite(CC2520_EXCFLAG0, 0x1, &regVal);
#endif
      
      
      /*
       * When AUTOCRC = 1, the contents of the tx fifo should be:
       *---------------------------------------------------------
       * LEN (1 byte)  |  MAC PDU (LEN - 2 bytes)  | ignored
       *---------------------------------------------------------
       */
   
      // cc2520_memRead(CC2520_FSMSTAT0, 0x1, &MAC_preTrxToFifoFSMSTAT0);
      cc2520_writeToTxFifo(1, &txFifoLenFld);
      
      // cc2520_memRead(CC2520_FSMSTAT0, 0x1, &MAC_midTrxToFifoFSMSTAT0);
  
      cc2520_writeToTxFifo(SYS_GET_BD_LEN(bdHndl), 
                           SYS_GET_BD_BUFF(bdHndl));
     
      cc2520_memRead(CC2520_TXFIFOCNT, 0x1, &txFifoCnt);
   
      if (txFifoCnt == (1 + SYS_GET_BD_LEN(bdHndl)))
          break;  
      else
          SYS_fatal(SYS_FATAL_ERR_1);
   } while (1);
     
   return;
}


/*
 * During operation in a busy IEEE 802.15.4 environment, CC2520 will receive large 
 * numbers of non-intended acknowledgment frames. To effectively block reception 
 * of these frames, use the FRMFILT1.ACCEPT_FT2_ACK bit to control when ack frames 
 * should be received:
 *
 *  - Set FRMFILT1.ACCEPT_FT2_ACK after successfully starting a transmission with 
 *    acknowledgment request, and clear the bit again after the ack frame has 
 *    been received, or the timeout has been reached.
 *
 *  - Keep the bit cleared otherwise.
 */

   
/*
 * 15.5 MEM
 * The MEM memory area from address 0x200 to 0x37F is 384 bytes long. The two 
 * 16-byte temporary areas CBCTEMPH and CBCTEMPL are used for CBCMAC, UCBCMAC, 
 * CCM and UCCM instructions, with high and low priority respectively. The 
 * remaining MEM area is general purpose memory.   
 *
 * < Local address information> 
 * - 0x3F4-0x3F5   SHORT_ADDR     Little Endian    
 *   The short address used during destination address filtering.
 *
 * - 0x3F2-0x3F3   LPWMN_ID         Little Endian 
 *   The LPWMN ID used during destination address filtering.
 * 
 * - 0x3EA-0x3F1   EXT_ADDR       Little Endian 
 *   The IEEE extended address used during destination address filtering.
 */
void CC2520_writeShort(UINT16_t addr, UINT16_t val)
{
   UINT8_t regVal;
   
   // local short address is stored in little endian format
   regVal = val & 0xff;
   cc2520_memWrite(addr, 0x1, &regVal);
   regVal = (val >> 8) & 0xff;
   cc2520_memWrite(addr + 1, 0x1, &regVal);
  
   do
   { 
      cc2520_memRead(addr, 0x1, &regVal);
      if (regVal == (val & 0xff))
      {
          cc2520_memRead(addr + 1, 0x1, &regVal);
          if (regVal == ((val >> 8) & 0xff))
              break;
      }
   } while (1);
   
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
void cc2520_cfgEnaAckFramesRx(void)
{
   /*
    * Acknowledgment frames are only accepted when:
    *  > FRMFILT1.ACCEPT_FT2_ACK = 1
    *  > Length byte = 5  
    */
   cc2520_bitSet(CC2520_MAKE_BIT_ADDR(CC2520_FRMFILT1, 
                                      CC2520_FRMFILT1_ACCEPT_FT_2_ACK_BIT_NR));
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */                          
void cc2520_cfgDisAckFramesRx(void)
{
   cc2520_bitClear(CC2520_MAKE_BIT_ADDR(CC2520_FRMFILT1, 
                                        CC2520_FRMFILT1_ACCEPT_FT_2_ACK_BIT_NR));
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void cc2520_postResetWait( )
{
   SYS_globalEvtMsk0 &= ~SYS_GLOBAL_EVT_TIMER_A0_EXPIRY;

   // Wait for 1 millisecond
   if (PLTFRM_startTimerA0(0, 1000, NULL) != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_1);
   }

   while ((SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_TIMER_A0_EXPIRY) == 0x0);

   SYS_globalEvtMsk0 &= ~SYS_GLOBAL_EVT_TIMER_A0_EXPIRY;

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
UINT8_t cc2520_rand(UINT8_t *randVal_p)
{
   UINT8_t rc = 0, rssiSts;
   
   /*
    * CC2520 can output random bits in two different ways. Common for these are 
    * that the chip should be in RX when generation of random bits are required. 
    * One must also make sure that the chip has been in RX long enough for the 
    * transients to have died out. A convenient way to do this is to wait for the 
    * RSSI valid signal to go high.
    * > Single random bits from either the I or Q channel (configurable) can be 
    *   output on GPIO pins at a rate of 8MHz. One can also select to xor the I 
    *   and Q bits before they are output on a GPIO pin. These bits are taken 
    *   from the least significant bit in the I and/or Q channel after the 
    *   decimation filter in the demodulator.
    * > CC2520 supports an instruction called RANDOM that allows the user to read 
    *   randomly generated bytes over the SPI. These bytes are generated from the 
    *   least significant bit of the I channel output from the channel filter in 
    *   the demodulator.  
    */
  
  
   /*
    * The status bit RSSI_VALID should be checked before reading the RSSI value 
    * register. RSSI_VALID indicates that the RSSI value in the register is in 
    * fact valid, which means that the receiver has been enabled for at least 8 
    * symbol periods.
    */
   
   cc2520_memRead(CC2520_RSSISTAT, 0x1, &rssiSts);
  
   if (rssiSts & CC2520_RSSISTAT_RSSI_VALID_BIT_MSK)
   {
       cc2520_memRead(CC2520_INS_RANDOM, 0x1, randVal_p);
       rc = 1;
   }   
   
   return rc;
}


#ifdef WISENSE_HW_REV_2
// Port 2 interrupt service routine
#ifdef ECLIPSE_IDE
static void __attribute__((__interrupt__(PORT2_VECTOR))) Port_2 (void)
#else
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
#endif
{
  
    if (P2IFG & BIT3)
    {  
        P2IFG &= ~BIT3;
    }
  
    if (P2IFG & BIT4)
    {  
        P2IFG &= ~BIT4;
        SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_FRAME_TX_DONE;
    }

    if (P2IFG & BIT5)
    {  
        P2IFG &= ~BIT5;
        SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_FRAME_RX_DONE;
    }
}
#elif defined (WISENSE_HW_REV_3)
// Port 2 interrupt service routine
#ifdef ECLIPSE_IDE
static void __attribute__((__interrupt__(PORT2_VECTOR))) Port_2 (void)
#else
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
#endif
{
    UINT8_t reg = P2IFG;

    // P2.3 -> GPIO_1
    // TX_DONE
    if (reg & BIT3)
    {  
        P2IFG &= ~BIT3;
#ifdef CC2520_INTR_STATS_ENA        
        txDoneCnt ++;
#endif         
        SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_FRAME_TX_DONE;
    }

    // P2.5 -> GPIO_3
    // RX_DONE
    if (reg & BIT4)
    {  
        P2IFG &= ~BIT4;
#ifdef CC2520_INTR_STATS_ENA        
        rxDoneCnt ++;
#endif         
        SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_FRAME_RX_DONE;
    }    
    
    // P2.4 -> GPIO_2
    // RX_OVERFLOW    
    if (reg & BIT5)
    {  
        P2IFG &= ~BIT5;
#ifdef CC2520_INTR_STATS_ENA        
        rxOverFlowCnt ++;
#endif        
        SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_RADIO_RX_FIFO_OVFL;
    } 
}
#elif defined (WISENSE_HW_REV_4)
// Port 2 interrupt service routine
#ifdef ECLIPSE_IDE
static void __attribute__((__interrupt__(PORT2_VECTOR))) Port_2 (void)
#else
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
#endif
{
    UINT8_t reg = P2IFG;

    // At this point interrupts are disabled globally
    // This does not prevent bits being set in P2IFG

    // P2.1 -> GPIO_1
    // TX_DONE 
    if (reg & BIT1)
    {  
        P2IFG &= ~BIT1;
#ifdef CC2520_INTR_STATS_ENA        
        txDoneCnt ++;
#endif         
        SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_FRAME_TX_DONE;
    }    
    
    // P2.2 -> GPIO_2
    // RX_DONE     
    if (reg & BIT3)
    {  
        P2IFG &= ~BIT3;
#ifdef CC2520_INTR_STATS_ENA        
        rxDoneCnt ++;
#endif        
        SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_FRAME_RX_DONE;
    } 
    
#if 0    
    // P2.3 -> GPIO_3
    // RX_OVERFLOW     
    if (reg & BIT3)
    {  
        P2IFG &= ~BIT3;
#ifdef CC2520_INTR_STATS_ENA        
        rxOverFlowCnt ++;
#endif        
        SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_RADIO_RX_FIFO_OVFL;
    }
#endif    
    
    // In all modes (RFD, FFD and Coord), exit LPM mode
    if (SYS_globalEvtMsk0 != 0x0)
    {
        PLTFRM_exitLPM();
    }


    // Zero out bits which we are not interested in
    P2IFG = ~(BIT1 | BIT3);
}
#elif defined(WISENSE_HW_REV_5) || defined(WISENSE_HW_REV_6)
// Port 1 interrupt service routine
#ifdef ECLIPSE_IDE
static void __attribute__((__interrupt__(PORT1_VECTOR))) Port_1 (void)
#else
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
#endif
{
    UINT8_t reg = P1IFG;

    // P1.4 <- GPIO_1
    // TX_DONE
    if (reg & BIT4)
    {
        P1IFG &= ~BIT4;
#ifdef CC2520_INTR_STATS_ENA
        txDoneCnt ++;
#endif
        SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_FRAME_TX_DONE;
    }

    // P1.2 <- GPIO_3
    // RX_DONE
    if (reg & BIT2)
    {
        P1IFG &= ~BIT2;
#ifdef CC2520_INTR_STATS_ENA
        rxDoneCnt ++;
#endif
        SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_FRAME_RX_DONE;
    }

    if (SYS_globalEvtMsk0 != 0x0)
    {
        PLTFRM_exitLPM();
    }

    // Zero out bits which we are not interested in
    P1IFG &= (BIT2 | BIT4);
}
#else
#error board hardware revision not specified !!
#endif 
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */ 
UINT8_t cc2520_setPhyChan(UINT8_t chanNr)
{
   UINT8_t regVal;
   
   /*
    * IEEE802.15.4-2006 specifies a frequency range from 2405MHz to 2480MHz 
    * with 16 channels 5 MHz apart. The channels are numbered 11 through 26. 
    * For an IEEE802.15.4-2006 compliant system, the only valid settings are 
    * thus :  
    *       freq[6:0] = 11 + 5(channel number - 11)
    */
   
   regVal = 11 + 5*(chanNr - 11);
   
   cc2520_memWrite(CC2520_FREQCTRL, 0x1, &regVal);
   
   return 1;
}

void cc2520_clearIntrFlags(void)
{
   // Clear interrupt flags
#if defined(WISENSE_HW_REV_2) || defined(WISENSE_HW_REV_3)
   P2IFG &= (~(BIT3 | BIT4 | BIT5));
#elif defined(WISENSE_HW_REV_4)
   P2IFG &= (~(BIT1 | BIT3));
#elif defined(WISENSE_HW_REV_5)
   P2IFG &= (~(BIT2 | BIT4));
#elif defined(WISENSE_HW_REV_6)
   P1IFG &= (~(BIT2 | BIT4));
#else
#error hardware revision not specifed !!
#endif
}



/*
 * Automatic acknowledgment transmission with correct setting of the frame
 * pending bit: When using indirect frame transmission, the devices will
 * send data requests to poll frames stored on the coordinator. To indicate
 * whether it actually has a frame stored for the device, the coordinator
 * must set or clear the frame pending bit in the returned acknowledgment
 * frame. On most 8- and 16-bit MCUs, however, there is not enough time to
 * determine this, and so the coordinator ends up setting the pending bit
 * regardless of whether there are pending frames for the device (as required
 * by IEEE 802.15.4 [2]). This is wasteful in terms of power consumption,
 * because the polling device will have to keep its receiver enabled for a
 * considerable period of time, even if there are no frames for it. By loading
 * the destination addresses in the indirect frame queue into the source address
 * table and enabling the AUTOPEND function, CC2520 will set the pending bit
 * in outgoing acknowledgment frames automatically. This way the operation is no
 * longer timing critical, as the effort done by the microcontroller is when
 * adding or removing frames in the indirect frame queue and updating the source
 * address table accordingly.
 */


#if defined(DEV_TYPE_RFD) || defined(DEV_TYPE_FFD)
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void cc2520_exitLPM1(void)
{
   UINT16_t waitCnt = 0;

   /*
    * > SXOSCON + NOP
    * > Set CSn=0 and wait until SO=1
    * > Set CSn=1
    *
    * Turn on the crystal oscillator. If this instruction is executed when the
    * XOSC is already on, the instruction has no effect. This instruction can
    * only be run as the first instruction after CSn has been pulled low. Must
    * be immediately followed by a SNOP instruction in order to terminate properly.
    */

    cc2520_insStrobe(CC2520_INS_SXOSCON);
    cc2520_insStrobe(CC2520_INS_SNOP);

    /*
     * "Txr" is the time for internal XOSC to stabilize after RESETn is released
     * or SXOSCON strobe is issued. Typical value is 0.2 milli-secs (is crystal
     * dependent).
     */
    cc2520_spiBegin();

#if defined(WISENSE_HW_REV_3) || defined(WISENSE_HW_REV_4)
    while ((P1IN & 0x40) == 0x0)
#elif defined(WISENSE_HW_REV_5) || defined(WISENSE_HW_REV_6)
    while ((P3IN & BIT2) == 0x0)
#else
#error hardware version not specified !!
#endif
    {
        waitCnt ++;
    }

    cc2520_spiEnd();

    PLTFRM_delay1000MicroSecs();

    {
       UINT8_t chipId = 0x0;
       do
       {
          cc2520_memRead(CC2520_CHIPID, 0x1, &chipId);
       } while (chipId != 0x84);
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
void cc2520_enterLPM1(void)
{
   /*
    * In Low Power Mode 1 (LPM1) the digital voltage regulator is on (VREG_EN=1),
    * but no clocks are running. Data is retained. The power down signals to the
    * analog modules are controlled by the digital part.
    *
    * Active --> SXOSCOFF (Radio must be idle) --> LPM1
    */

   /*
    * The receiver will be turned off by the following actions:
    * The SRFOFF strobe:
    *   > Clears RXENABLE[15:0]
    *   > Aborts ongoing transmission/reception by forcing the transition
    *     to IDLE mode.
    */

    PLTFRM_intDisable();

    // Put cc2520 in idle mode
    cc2520_hardRxStop();

    // Flush the receive and transmit FIFOs
    cc2520_insStrobe(CC2520_INS_SFLUSHRX);
    cc2520_insStrobe(CC2520_INS_SFLUSHTX);

    cc2520_clearIntrFlags();

    PLTFRM_intEnable();

    // Put the cc2520 in LPM1 mode (turn off the crystal oscillator)
    cc2520_insStrobe(CC2520_INS_SXOSCOFF);

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
void cc2520_enterLPM2(void)
{
   /*
    * In Low Power Mode 2 (LPM2) the digital voltage regulator is turned off
    * (VREG_EN=0) and no clocks are running. No data is retained. All analog
    * modules are in power down state.
    *
    * Before entering LPM2, it is strongly recommended that the device is reset.
    * This way, the configuration will always be the same when the power to the
    * digital part is removed, and it is less likely that there will be issues
    * with current spikes or other side effects of the power being removed.
    *
    * SRES command : Reset the device except the SPI interface. This instruction
    * can only be run as the first instruction after CSn has been pulled low.
    *
    * Entering LPM2 mode
    * > Active mode -> SRES -> Set VREG_EN = 0, Set GPIO5 = 0 -> LPM2
    */


    PLTFRM_intDisable();

#if defined(WISENSE_HW_REV_4)
     P2IE &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5);
#elif defined(WISENSE_HW_REV_5) || defined(WISENSE_HW_REV_6)
     P1IE &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5);
#else
#error TODO !!
#endif

    cc2520_insStrobe(CC2520_INS_SRES);

    PLTFRM_delay1000MicroSecs();

    VREG_OFF();

    PLTFRM_delay1000MicroSecs();

    RESET_ENA();
    cc2520_spiBegin();


    /*
     * In LPM2 GPIO5 (which is configured as an input) should be connected to either CC2520
     * ground or VDD. The other GPIO pins should be grounded or high impedance. Failing to
     * do this, will result in significantly higher current consumption than necessary.
     */

#if defined(WISENSE_HW_REV_4)
    // P2.5 <- GPIO_5
    P2DIR |= (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5);  // Output direction
    P2OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5);  // Output 0
#elif defined(WISENSE_HW_REV_5) || defined(WISENSE_HW_REV_6)
    P1DIR |= (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5);  // Output direction
    P1OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5);  // Output 0
#else
#error TODO !!
#endif

#if 0
    // rkris, Jan/20/2014
    // SPI bus may be in use by some SPI enabled sensors. So do not change the SPI interface.

    /*
     * SO is configured as an input when CSn is high or RESETn is low. Note that the SO pin
     * should not be left floating while in LPM1 or LPM2, as this will result in higher current
     * consumption than necessary.
     */
#if defined(WISENSE_HW_REV_3) || defined(WISENSE_HW_REV_4)

    // P1.7    UCB0SIMO
    // P1.6    UCB0SOMI
    // P1.5    UCB0CLK  /  UCA0STE

    P1SEL &= ~(0xe0);  // 7, 6 and 5
    P1SEL2 &= ~(0xe0);
    P1DIR |= (0xe0);  // SIMO, SOMI and CLK
    P1OUT &= ~(0xe0); // All low
#elif defined(WISENSE_HW_REV_5) || defined(WISENSE_HW_REV_6)
    // P3.1    UCB0SIMO
    // P3.2    UCB0SOMI
    // P3.3    UCB0CLK  /  UCA0STE

    P3SEL &= ~(BIT1 | BIT2 | BIT3);  // 7, 6 and 5
    P3DIR |= (BIT1 | BIT2 | BIT3);   // SIMO, SOMI and CLK
    P3OUT &= ~(BIT1 | BIT2 | BIT3);  // All low
#else
#error TODO !!
#endif
#endif

    PLTFRM_intEnable();

    // Bug fixed, rkris@wisense.in, jan/20/14
    cc2520_spiEnd();

    return;
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
UINT8_t cc2520_txPowerSetPointValid(SINT16_t powerInDbm)
{
   UINT16_t idx;

   for (idx=0; idx<CC2520_TX_POWER_ENC_TBL_SZ; idx++)
   {
        if (cc2520_txPowerEncTable[idx].txPower == powerInDbm)
            break;
   }

   return ((idx >= CC2520_TX_POWER_ENC_TBL_SZ) ? 0x0 : 0x1);
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t cc2520_setTxPower(SINT16_t powerInDbm)
{
   UINT8_t regVal;
   UINT16_t idx;

   /*
    * The RF output power of CC2520 is controlled by the 7 bit value in the
    * TXPOWER register. Table 17 shows the typical output power and current
    * consumption for the recommended settings when the centre frequency
    * is set to 2440 GHz.
    *
    * !!!!!!!!!!!!!!!!!!!!!  VERY IMPORTANT NOTE !!!!!!!!!!!!!!!!!!!!!!!!
    * Note that the recommended settings are only a small subset of all the
    * possible register settings. Using other settings than those in Table
    * 17 might result in very high current consumption and generally poor
    * performance.
    * ###################################################################
    *
    * Please refer to section 5.11 for details on the optional temperature
    * compensated TX.
    *
    * !!!!!!!!!!!!!!!!!!!!!  VERY IMPORTANT NOTE !!!!!!!!!!!!!!!!!!!!!!!!
    * TXPOWER is one of the registers which needs to be updated from its
    * default value. The default value is 0x6 which is not in table 17.
    * ###################################################################
    */

   for (idx=0; idx<CC2520_TX_POWER_ENC_TBL_SZ; idx++)
   {
        if (cc2520_txPowerEncTable[idx].txPower == powerInDbm)
            break;
   }

   if (idx >= CC2520_TX_POWER_ENC_TBL_SZ)
       return 0;

   regVal = cc2520_txPowerEncTable[idx].enc;

   cc2520_memWrite(CC2520_TXPOWER, 0x1, &regVal);
   do
   {
      cc2520_memRead(CC2520_TXPOWER, 0x1, &regVal);
   } while (regVal != cc2520_txPowerEncTable[idx].enc);

   return 1;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void cc2520_cfg(void)
{
      /*
       * CCACTRL0, A 0x036, R 0xE0, CCA threshold
       * Clear Channel Assessment threshold value, signed 2�s complement number for
       * comparison with the RSSI. The unit is 1 dB, offset is about 76dBm. The CCA
       * signal goes high when the received signal is below this val. The CCA signal
       * is available on the CCA pin and in FSMSTAT1 register. Note that the value
       * should never be set lower than CCA_HYST-128 in order to avoid erroneous
       * behavior of the CCA signal.
       * NOTE
       * The reset value translates to an input level of approximately -32 �76 = -108
       * dBm, which is well below the sensitivity limit. That means the CCA signal will
       * never indicate a clear channel. This register should be updated to 0xF8, which
       * translates to an input level of about -8 - 76 = -84dBm.
       */

      do
      {
         UINT8_t ccaCtrl0 = 0x0;
         cc2520_memRead(CC2520_CCACTRL0, 0x1, &ccaCtrl0);
         ccaCtrl0 = 0xf8;
         cc2520_memWrite(CC2520_CCACTRL0, 0x1, &ccaCtrl0);
         ccaCtrl0 = 0x0;
         cc2520_memRead(CC2520_CCACTRL0, 0x1, &ccaCtrl0);
         if (ccaCtrl0 == 0xf8)
             break;
      } while (1);


#if 1
      // Enable frame filtering. When disabled, the CC2520 will accept all received
      // frames.
      cc2520_bitSet(CC2520_MAKE_BIT_ADDR(CC2520_FRMFILT0,
                                         CC2520_FRMFILT0_FRAME_FILTER_EN_BIT_NR));
#else
      // Frame filtering is enabled by default after reset
      cc2520_bitClear(CC2520_MAKE_BIT_ADDR(CC2520_FRMFILT0,
                                           CC2520_FRMFILT0_FRAME_FILTER_EN_BIT_NR));
#endif

#ifdef LPWMN_COORD
      /*
       * Should be set high when the device is a LPWMN coordinator, to accept frames
       * with no destination address (as specified in section 7.5.6.2 in 802.15.4(b))
       *  > 0 - Device is not LPWMN coordinator
       *  > 1 - Device is LPWMN coordinator
       */
      cc2520_bitSet(CC2520_MAKE_BIT_ADDR(CC2520_FRMFILT0,
                                         CC2520_FRMFILT0_LPWMN_COORDINATOR_BIT_NR));
#else
      cc2520_bitClear(CC2520_MAKE_BIT_ADDR(CC2520_FRMFILT0,
                                           CC2520_FRMFILT0_LPWMN_COORDINATOR_BIT_NR));
#endif


      // Configure the cc2520 GPIOs
      {
         UINT8_t regVal;

#ifdef WISENSE_HW_REV_2
         // Pin 2.5
         P2DIR &= ~(BIT5);  // Input direction
         P2SEL  &= ~(BIT5); // I/O functionality
         P2SEL2 &= ~(BIT5);
         P2REN &= ~(BIT5);  // Pull up resistor disabled
         P2IE |= BIT5;      // Interrupt enable
         P2IES &= ~(BIT5);  // Active high
         P2IFG &= ~(BIT5);  // clear interrupt flag

         // Pin 2.4
         P2DIR &= ~(BIT4);  // Input direction
         P2SEL  &= ~(BIT4); // I/O functionality
         P2SEL2 &= ~(BIT4);
         P2REN &= ~(BIT4);  // Pull up resistor disabled
         P2IE |= BIT4;      // Interrupt enable
         P2IES &= ~(BIT4);  // Active high
         P2IFG &= ~(BIT4);  // clear interrupt flag

#if 0
         // Pin 2.3
         P2DIR &= ~(BIT3);  // Input direction
         P2SEL  &= ~(BIT3); // I/O functionality
         P2SEL2 &= ~(BIT3);
         P2REN &= ~(BIT3);  // Pull up resistor disabled
         P2IE |= BIT3;      // Interrupt enable
         P2IES &= ~(BIT3);  // Active high
         P2IFG &= ~(BIT3);  // clear interrupt flag
#endif

#elif defined(WISENSE_HW_REV_3)

         // P3.6 -> GPIO_5
         // P3.5 -> GPIO_4
         // P2.5 -> GPIO_3
         // P3.4 -> GPIO_0

         // P2.3 -> GPIO_1
         P2DIR &= ~(BIT3);  // Input direction
         P2SEL  &= ~(BIT3); // I/O functionality
         P2SEL2 &= ~(BIT3);
         P2REN &= ~(BIT3);  // Pull up resistor disabled
         P2IE |= BIT3;      // Interrupt enable
         P2IES &= ~(BIT3);  // Active high
         P2IFG &= ~(BIT3);  // clear interrupt flag

         // P2.4 -> GPIO_2
         P2DIR &= ~(BIT4);  // Input direction
         P2SEL  &= ~(BIT4); // I/O functionality
         P2SEL2 &= ~(BIT4);
         P2REN &= ~(BIT4);  // Pull up resistor disabled
         P2IE |= BIT4;      // Interrupt enable
         P2IES &= ~(BIT4);  // Active high
         P2IFG &= ~(BIT4);  // clear interrupt flag

         // P2.5 -> GPIO_3
         P2DIR &= ~(BIT5);  // Input direction
         P2SEL  &= ~(BIT5); // I/O functionality
         P2SEL2 &= ~(BIT5);
         P2REN &= ~(BIT5);  // Pull up resistor disabled
         P2IE |= BIT5;      // Interrupt enable
         P2IES &= ~(BIT5);  // Active high
         P2IFG &= ~(BIT5);  // clear interrupt flag

#elif defined(WISENSE_HW_REV_4)

#if 0
         // P2.0 <- GPIO_0
         P2DIR &= ~(BIT0);  // Input direction
         P2SEL  &= ~(BIT0); // I/O functionality
         P2SEL2 &= ~(BIT0);
         P2REN &= ~(BIT0);  // Pull up resistor disabled
         P2IE |= BIT0;      // Interrupt enable
         P2IES &= ~(BIT0);  // Active high
         P2IFG &= ~(BIT0);  // clear interrupt flag
#endif

         // GPIO0 outputs 1 mhz clock on reset !!
         P2IE &= ~BIT0;     // Interrupt disable
         P2IFG &= ~(BIT0);  // clear interrupt flag

         // P2.1 <- GPIO_1
         P2DIR &= ~(BIT1);  // Input direction
         P2SEL  &= ~(BIT1); // I/O functionality
         P2SEL2 &= ~(BIT1);
         P2REN &= ~(BIT1);  // Pull up resistor disabled
         P2IE |= BIT1;      // Interrupt enable
         P2IES &= ~(BIT1);  // Active high
         P2IFG &= ~(BIT1);  // clear interrupt flag

#if 0
         // P2.2 <- GPIO_2
         P2DIR &= ~(BIT2);  // Input direction
         P2SEL  &= ~(BIT2); // I/O functionality
         P2SEL2 &= ~(BIT2);
         P2REN &= ~(BIT2);  // Pull up resistor disabled
         P2IE |= BIT2;      // Interrupt enable
         P2IES &= ~(BIT2);  // Active high
         P2IFG &= ~(BIT2);  // clear interrupt flag
#endif

         // P2.3 <- GPIO_3
         P2DIR &= ~(BIT3);  // Input direction
         P2SEL  &= ~(BIT3); // I/O functionality
         P2SEL2 &= ~(BIT3);
         P2REN &= ~(BIT3);  // Pull up resistor disabled
         P2IE |= BIT3;      // Interrupt enable
         P2IES &= ~(BIT3);  // Active high
         P2IFG &= ~(BIT3);  // clear interrupt flag

#elif defined(WISENSE_HW_REV_5) || defined(WISENSE_HW_REV_6)
         // GPIO5 -> P1.0
         // GPIO4 -> P1.1
         // GPIO3 -> P1.2
         // GPIO2 -> P1.3
         // GPIO1 -> P1.4
         // GPIO0 -> P1.5

         // GPIO0 outputs 1 mhz clock on reset !!
         // P2IE &= ~BIT5;     // Interrupt disable
         // P2IFG &= ~(BIT5);  // clear interrupt flag

         // P1.4 <- GPIO_1
         P1DIR &= ~(BIT4);  // Input direction
         P1SEL  &= ~(BIT4); // I/O functionality
         P1SEL2 &= ~(BIT4);
         P1REN &= ~(BIT4);  // Pull up resistor disabled
         P1IE |= BIT4;      // Interrupt enable
         P1IES &= ~(BIT4);  // Active high
         P1IFG &= ~(BIT4);  // clear interrupt flag

         // P1.2 <- GPIO_3
         P1DIR &= ~(BIT2);  // Input direction
         P1SEL  &= ~(BIT2); // I/O functionality
         P1SEL2 &= ~(BIT2);
         P1REN &= ~(BIT2);  // Pull up resistor disabled
         P1IE |= BIT2;      // Interrupt enable
         P1IES &= ~(BIT2);  // Active high
         P1IFG &= ~(BIT2);  // clear interrupt flag

#else
#error board hardware revision not specified !!
#endif

         /*
          * Each GPIO has an associated register, GPIOCTRLn, where the MSB configure
          * the pin to either input or output.
          * When a GPIO pin is configured as an output, the signal corresponding to
          * the CTRLn setting in GPIOCTRLn register (CTRLn values are shown in Table 9
          * - section 12.6). The polarity of the pin is set in the GPIOPOLARITY reg.
          */
#if defined(WISENSE_HW_REV_2) || defined(WISENSE_HW_REV_3)
         do
         {
            regVal = CC2520_CTRLn_OUT_UNUSED | 0x80;
            cc2520_memWrite(CC2520_GPIOCTRL0, 0x1, &regVal);
            regVal = 0;
            cc2520_memRead(CC2520_GPIOCTRL0, 0x1, &regVal);
         } while (regVal != (CC2520_CTRLn_OUT_UNUSED | 0x80));

         regVal = CC2520_CTRLn_OUT_TX_FRM_DONE;  // ;
         cc2520_memWrite(CC2520_GPIOCTRL1, 0x1, &regVal);

         regVal = CC2520_CTRLn_OUT_RX_FRM_DONE;  // CC2520_CTRLn_OUT_RX_FRM_DONE;  // ;
         cc2520_memWrite(CC2520_GPIOCTRL2, 0x1, &regVal);

         regVal = CC2520_CTRLn_OUT_UNUSED | 0x80;  // CC2520_CTRLn_OUT_RX_OVERFLOW;  // CC2520_CTRLn_OUT_RX_OVERFLOW;  // ;
         cc2520_memWrite(CC2520_GPIOCTRL3, 0x1, &regVal);

         regVal = CC2520_CTRLn_OUT_UNUSED | 0x80;
         cc2520_memWrite(CC2520_GPIOCTRL4, 0x1, &regVal);

         regVal = CC2520_CTRLn_OUT_UNUSED | 0x80;
         cc2520_memWrite(CC2520_GPIOCTRL5, 0x1, &regVal);

         cc2520_memRead(CC2520_GPIOPOLARITY, 0x1, &regVal);
         regVal |= CC2520_GPIOPOLARITY_GPIO1_BIT_MSK;
         regVal |= CC2520_GPIOPOLARITY_GPIO2_BIT_MSK;
         cc2520_memWrite(CC2520_GPIOPOLARITY, 0x1, &regVal);

         while (1)
         {
            regVal = 0xbf;
            cc2520_memWrite(CC2520_GPIOCTRL, 0x1, &regVal);
            cc2520_memRead(CC2520_GPIOCTRL, 0x1, &regVal);
            if (regVal == 0xbf)
                break;
         }

#elif defined(WISENSE_HW_REV_4) || defined(WISENSE_HW_REV_5) || defined(WISENSE_HW_REV_6)

         regVal = CC2520_CTRLn_OUT_UNUSED | 0x80;
         cc2520_memWrite(CC2520_GPIOCTRL0, 0x1, &regVal);

         regVal = CC2520_CTRLn_OUT_TX_FRM_DONE;
         cc2520_memWrite(CC2520_GPIOCTRL1, 0x1, &regVal);

         regVal = CC2520_CTRLn_OUT_UNUSED | 0x80;
         cc2520_memWrite(CC2520_GPIOCTRL2, 0x1, &regVal);

         // regVal = CC2520_CTRLn_OUT_RX_FRM_DONE;
         // cc2520_memWrite(CC2520_GPIOCTRL2, 0x1, &regVal);

         regVal = CC2520_CTRLn_OUT_RX_FRM_DONE; // CC2520_CTRLn_OUT_RX_OVERFLOW;
         cc2520_memWrite(CC2520_GPIOCTRL3, 0x1, &regVal);

         regVal = CC2520_CTRLn_OUT_UNUSED | 0x80;
         cc2520_memWrite(CC2520_GPIOCTRL4, 0x1, &regVal);

         regVal = CC2520_CTRLn_OUT_UNUSED | 0x80;
         cc2520_memWrite(CC2520_GPIOCTRL5, 0x1, &regVal);


#else
#error hardware revision not specifed !!
#endif
      }

      /*
       * Automatic ACK
       * When FRMFILT0.FRM_FILTER_EN and FRMCTRL0.AUTOACK are both enabled, the
       * CC2520 will determine automatically whether or not to transmit acknowledgment
       * frames:
       *  > The RX frame must be accepted by frame filtering (indicated by the
       *    RX_FRM_ACCEPTED exception)
       *  > The acknowledgment request bit must be set in the RX frame
       *  > The RX frame must not be a beacon or an acknowledgment frame
       *  > The FCS of the RX frame must be correct
       *
       * ACUTOACK bit in FRMCTRL0 (Reset value is 0x0)
       * Defines whether CC2520 automatically transmits acknowledge frames or not.
       * When autoack is enabled, all frames that are accepted by address filtering,
       * have the acknowledge request flag set and have a valid CRC, are automatically
       * acknowledged 12 symbol periods after being received.
       *  > 0 - Autoack disabled
       *  > 1 - Autoack enabled
       */
      cc2520_bitSet(CC2520_MAKE_BIT_ADDR(CC2520_FRMCTRL0,
                                         CC2520_FRMCTRL0_AUTOACK_BIT_NR));


      // Disable source matching
      cc2520_bitClear(CC2520_MAKE_BIT_ADDR(CC2520_SRCMATCH,
                                           CC2520_SRCMATCH_SRC_MATCH_EN_BIT_NR));


      cc2520_cfgDisAckFramesRx();  // Disable ACK reception

      // Flush the RX FIFO
      cc2520_insStrobe(CC2520_INS_SFLUSHRX);

      if (cc2520_setTxPower(0) == 0)
          SYS_fatal(SYS_FATAL_ERR_1);

#if 0
      cc2520_memRead(CC2520_FRMFILT1, 0x1, &__frmFilt1);
      cc2520_bitClear(CC25f20_MAKE_BIT_ADDR(CC2520_FRMFILT1,
                                            CC2520_FRMFILT1_ACCEPT_FT_1_DATA_BIT_NR));
      cc2520_bitClear(CC2520_MAKE_BIT_ADDR(CC2520_FRMFILT1,
                                            CC2520_FRMFILT1_ACCEPT_FT_3_MAC_CMD_BIT_NR));
      cc2520_bitClear(CC2520_MAKE_BIT_ADDR(CC2520_FRMFILT1,
                                            CC2520_FRMFILT1_ACCEPT_FT_0_BEACON_BIT_NR));
      cc2520_bitClear(CC2520_MAKE_BIT_ADDR(CC2520_FRMFILT1,
                                            CC2520_FRMFILT1_ACCEPT_FT_2_ACK_BIT_NR));
      cc2520_memRead(CC2520_FRMFILT1, 0x1, &__frmFilt1);
#endif

      /*
       * The function is controlled by:
       *  > The FRMFILT0 and FRMFILT1 registers
       *  > The LOCAL_LPWMN_ID, LOCAL_SHORT_ADDR and LOCAL_EXT_ADDR values in RAM.
       */


#if defined(WISENSE_HW_REV_2) || defined(WISENSE_HW_REV_3)
      P2IFG &= (~(BIT3 | BIT4 | BIT5));
#elif defined(WISENSE_HW_REV_4)
      P2IFG &= (~(BIT1 | BIT3));
#elif defined(WISENSE_HW_REV_5) || defined(WISENSE_HW_REV_6)
      // GPIO1 -> P1.4
      // GPIO3 -> P1.2
      P1IFG &= (~(BIT2 | BIT4));
#else
#error hardware revision not specifed !!
#endif

      return;
}

#if defined(DEV_TYPE_RFD) || defined(DEV_TYPE_FFD)

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void cc2520_exitLPM2(void)
{
   UINT16_t waitCnt = 0;

   /*
    * When the device has been in LPM2, all register content is lost. To bring
    * the device up to active mode, a reset is required or the device will be in
    * an unknown state. The reset can be applied either by setting the RESETn pin
    * low, or issuing a reset instruction (SRES) over the SPI. It is recommended
    * that the RESETn method is used, because it will give a controlled start and
    * automatic start of the crystal oscillator.
    */

#if defined(WISENSE_HW_REV_3) || defined(WISENSE_HW_REV_4)
     // Configure all GPIO connected pins as input
     P2DIR &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5);
     P2IE &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5);

     // Configure pin connecting to cc2520 SOMI as input
     // P1.6  UCB0SOMI
     P1DIR &= ~(0x40);
#elif defined(WISENSE_HW_REV_5) || defined(WISENSE_HW_REV_6)
     // Configure all GPIO connected pins as input
     P1DIR &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5);
     P1IE &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5);

     // As of Jan/20/14, SPI interface is not touched when entering/exiting LPM2
#if 0
     // Configure pin connecting to cc2520 SOMI as input
     // P3.2  UCB0SOMI
     P3DIR &= ~(BIT2);
#endif
#else
#error TODO !!
#endif

    // CSn = 1
    cc2520_spiEnd( );

    /*
     * Set RESETn = 0.
     * Set VREG_EN = 1.
     */
    RESET_ENA();
    VREG_ON();

    /*
     * In LPM2, the digital voltage regulator is turned off. Now it
     * has been turned on. Wait until regulator has stabilized. Use a
     * timeout.
     *
     * When the RESETn pin is used it must be held low until the internal
     * regulator has stabilized. This typically takes 0.1 milli-secs. When
     * the RESETn pin is set high, the crystal oscillator (in the CC2520
     * reference design) uses typically 0.2 milli-secs to start.
     *
     * From table 5.3 -
     * Internal regulator startup time + XOSC startup time  is typically 0.3
     * milli-secs.
     */

    cc2520_postResetWait( );

    PLTFRM_intDisable();

    /*
     * Set RESETn = 1
     * Set CSn = 0
     */
    RESET_DIS();
    cc2520_spiBegin( );

    /*
     * Wait until SO = 1
     */
#if defined(WISENSE_HW_REV_3) || defined(WISENSE_HW_REV_4)
    while ((P1IN & 0x40) == 0x0)
#elif defined(WISENSE_HW_REV_5) || defined(WISENSE_HW_REV_6)
    while ((P3IN & BIT2) == 0x0)
#elif defined(WISENSE_HW_REV_2)
#else
#error hardware version not specified !!
#endif
    {
        waitCnt ++;
    }

    // Set CSn = 1
    cc2520_spiEnd();

    cc2520_clearIntrFlags();

    PLTFRM_delay1000MicroSecs();

    PLTFRM_intEnable();

#if 0
    // // As of Jan/20/14, SPI interface is not touched when entering/exiting LPM2
    cc2520_spiInit();
#endif
    {
       UINT8_t chipId = 0x0;
       do
       {
          cc2520_memRead(CC2520_CHIPID, 0x1, &chipId);
       } while (chipId != 0x84);
    }

    cc2520_cfg();

    // rkris, jan/12/2014, set the PHY channel (default is chann 11)
    cc2520_setPhyChan(PHY_PIB.phyCurrentChannel);
}
#endif


#ifdef CC2520_STATS_ENA
UINT8_t cc2520_rxOvflCnt = 0, cc2520_rxOvflCorrCnt = 0;
#endif

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void cc2520_checkRadioErrors(void)
{
   UINT8_t fsmStat0;
   cc2520_memRead(CC2520_FSMSTAT0, 0x1, &fsmStat0);
   fsmStat0 &= CC2520_FSMSTAT0_FSM_FFCTRL_STATE_BIT_MSK;

   switch (fsmStat0)
   {
      case CC2520_RADIO_STATE_RX_OVERFLOW:
           {
#ifdef CC2520_STATS_ENA
              cc2520_rxOvflCnt ++;
#endif

              SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_RADIO_RX_FIFO_OVFL;
              /*
               * Reception of data is aborted and the FSM enters the rx_overflow
               * state. Recommended action is to issue a SFLUSHRX command strobe
               * to empty the RX FIFO and restart RX.
               */

              /*
               * Note that transmission is not stopped by the rx overflow condition.
               * Only difference is that the RAdio returns to 0x11 (rx overflow)
               * After transmission.
               */
           }
           break;

      default:
           break;
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
void cc2520_rxOvflProc(void)
{
   UINT8_t rxFifoCnt;

   cc2520_memRead(CC2520_RXFIFOCNT, 0x1, &rxFifoCnt);

   cc2520_insStrobe(CC2520_INS_SFLUSHRX);

   cc2520_memRead(CC2520_RXFIFOCNT, 0x1, &rxFifoCnt);
   if (rxFifoCnt != 0x0)
       SYS_fatal(SYS_FATAL_ERR_1);

   // restart RX
   cc2520_hardRxStart();

#ifdef CC2520_STATS_ENA
   cc2520_rxOvflCorrCnt ++;
#endif

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
void cc2520_init(void)
{
  unsigned char chipId = 0x0;
  
#ifdef WISENSE_HW_REV_4
  // Disable interrupts on all pins connected to cc2520 GPIO pins
  P2IE &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5);
#elif defined(WISENSE_HW_REV_5) || defined(WISENSE_HW_REV_6)
  // Disable interrupts on all pins connected to cc2520 GPIO pins
  P1IE &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5);
#else
#error TODO !!
#endif

#ifdef WISENSE_HW_REV_2  
  // RESETn : P2.2
  // VREGEN : P2.1
  P2DIR |= (0x2 | 0x4);
#elif defined(WISENSE_HW_REV_3) || defined(WISENSE_HW_REV_4)
  // RESETn : P3.1
  // VREGEN : P3.0  
  P3DIR |= (0x1 | 0x2);
#elif defined(WISENSE_HW_REV_5) || defined(WISENSE_HW_REV_6)
  // RESETn : P4.1
  // VREGEN : P4.2
  P4DIR |= (BIT1 | BIT2);
#else
#error board hardware revision not specified !!
#endif 
  
#if 0
  cc2520_spiInit();
#endif

#ifdef BOARD_TEST  
  for (;;)
  {
     UINT8_t idx;
     cc2520_spiBegin();
     // VREG_ON();
     // RESET_ENA();
     // for (idx=0; idx<10; idx++)
     //     PLTFRM_delay1000MicroSecs();
     // RESET_DIS();
     // VREG_OFF();
     cc2520_spiEnd();
  }
#endif  
      
  // Enable RESET and disable VREG_EN
  RESET_ENA();
  VREG_OFF();
  
  PLTFRM_delay1000MicroSecs();
  
  // Enable VREG_EN
  VREG_ON();
  
  // Delay for Tdres (0.3 millisecs)
  
  PLTFRM_delay1000MicroSecs();
  
  // Release reset
  RESET_DIS();

  PLTFRM_delay1000MicroSecs();
  
  chipId = 0x0;
  do
  {
     cc2520_memRead(CC2520_CHIPID, 0x1, &chipId);
  } while (chipId != 0x84);
  
  cc2520_cfg();

  return;
}


#endif
