/*
 * File Name: cc2500.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: July/15/2015
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

#ifdef RADIO_CC2500

#include <string.h>
#include <typedefs.h>
#ifdef __IAR__
#include "io430.h"
#else
#include <msp430.h>
#endif
#include <pltfrm.h>
#include <util.h>
#include <mac.h>
#include <system.h>
#include <phy_defs.h>
#include <phy_pib.h>
#include <radio.h>



/*
 * Programmable output power up to +12 dBm for all supported frequencies
 *
 * Modulation format
 * 2-FSK / GFSK / ASK/OOK / 4-FSK / MSK
 * MOD_FORMAT[2:0] bits in MDMCFG2
 *
 * Manchester Encoding (Yes / No)
 *
 * FEC (On / Off)   [FEC_EN bit in MDMCFG1]
 *
 * Preamble length (2 to 24 bytes) [NUM_PREAMBLE[2:0] in MDMCFG1] 
 *
 * Packet length mode (fixed/variable/infinite)  [LENGTH_CONFIG in PKTCTRLO]
 * This implementation uses varibale length packet mode.
 *
 * Address filtering (Yes/No) [ADR_CHK in PKTCTRL1]
 * This implementation does not use address filtering
 *
 * Maximum allowed length of incoming packet [PACKET_LENGTH in PKTLEN]
 * This implementation sets this value to 64 -1(length field) -2(status info appended). 
 *
 * CRC filtering (CRC_AUTOFLUSH in PKTCTRL1)
 * This implmentation uses CRC filtering
 * 
 */


/*
 * ---------------------------------------------------------------
 * Setting #1 (optimized for sensitivity)
 * ---------------------------------------------------------------
 * Baud rate: 250 kBaud
 * Base freq: 2432.999908
 * Channel #: 0
 * Channel spacing: 199.951172 kHz
 * RX filter BW: 541.666667 kHz
 * Tx Power: +1 dBm
 * Mod: GFSK
 * Deviation: 1.586914 kHz
 * Xtal freq: 26 MHz
 * Manchester enable: No
 * Whitening: No
 * ---------------------------------------------------------------
 */


UINT32_t CC2500_baseFreqHzOff = 32999908;  // offset from 2.4 GHz
UINT32_t CC2500_channelSpacingHz = 199951;


const CC2500_chipRegInfo_s CC2500_chipRegInfoList[  ] =
{
  // IOCFG2
  // IOCFG1
  // IOCFG0

  {CC2500_FIFOTHR_REG_ADDR, 0x07},

  // 8 MSB of 16-bit sync word (0xd391)
  {CC2500_SYNC1_REG_ADDR,   0xd3},

  // 8 LSB of 16-bit sync word (0xd391)
  {CC2500_SYNC0_REG_ADDR,   0x91},

  // PACKET_LENGTH[0:7] - If variable packet length mode is used, this value
  // indicates the maximum packet length allowed.
  {CC2500_PKTLEN_REG_ADDR, CC2500_MAX_RX_PKT_LEN},

  // PQT: 0 (SYNC word always accepted)
  // CRC_AUTOFLUSH: 1 (enabled - must be 0 if CC2400_EN is 1)
  // APPEND_STATUS: 1 (enabled)
  // ADR_CHK: 0 (Not enabled)
  {CC2500_PKTCTRL1_REG_ADDR,    0x0c},

  // WHITE_DATA: 0 (off)
  // PKT_FORMAT:0 (normal mode)
  // CC2400_EN: 0 (Must be 1 in our case)
  // CRC_EN: 1 (CRC calc in TX/RX enabled)
  // LENGTH_CONFIG: 1 (Variable pkt len mode)
  {CC2500_PKTCTRL0_REG_ADDR,  0x05},

  // Not applicable in our case (no address check)
  {CC2500_ADDR_REG_ADDR, 0x00},

  // The 8-bit unsigned channel number, which is multiplied by the channel
  // spacing setting and added to the base frequency.
  {CC2500_CHANNR_REG_ADDR, 0x00},

  // Intermediate frequency of 253.90625 kHz
  {CC2500_FSCTRL1_REG_ADDR,     0x07},  // from smart-rf

  // Frequency offset
  {CC2500_FSCTRL0_REG_ADDR,     0x00},

  {CC2500_FREQ2_REG_ADDR,       0x5d},  // 0x5c}, // 0x5D},    // base frequency
  {CC2500_FREQ1_REG_ADDR,       0x44},  // 0x80}, // 0x93},    // ok
  {CC2500_FREQ0_REG_ADDR,       0xec},  // 0x00}, // 0xB1},    // ok

  // Baud rate : 250 kbaud
  {CC2500_MDMCFG4_REG_ADDR,     0x2D},
  {CC2500_MDMCFG3_REG_ADDR,     0x3B},

  // Mod: MSK
  // Sync Mode: 30/32 sync word bits detected
  {CC2500_MDMCFG2_REG_ADDR,     0x73},  // 0x13

  // Preamble count : 4 bytes
  {CC2500_MDMCFG1_REG_ADDR,     0x23},  // 0x22

  // CHANSPC_M[7:0] - 2 (8 bit mantissa)
  // Channel spacing comes out to 249.938965 kHz  // 199.951172 kHz
  {CC2500_MDMCFG0_REG_ADDR,     0x3b},  // 0xf8},

  // DEVIATION_E: 0
  // DEVIATION_M: 0
  // 1.586914 kHz
  {CC2500_DEVIATN_REG_ADDR,     0x01},  // 0x00},

  {CC2500_MCSM2_REG_ADDR,       0x07},

  // CCA_MODE: 3 (If RSSI below threshold unless currently receiving a packet)
  // RXOFF_MODE: 3 (Stay in RX)
  // TXOFF_MODE: 0 (Go to IDLE)
  {CC2500_MCSM1_REG_ADDR,       0x3c},

  // FS:AUTOCAL[5:4] - 0x1 (Automatically calibrate when going from IDLE to RX/TX)
  // PIN_CTRL_EN[1] - 0x0 (Pin radio control option disabled)
  // XOSC_FORCE_ON[0] - 0x0 (not forcing XOSC to stay on in sleep state)
  {CC2500_MCSM0_REG_ADDR,       0x18},  // from smart-rf

  {CC2500_FOCCFG_REG_ADDR,      0x1D},  // from smart-rf

  {CC2500_BSCFG_REG_ADDR,       0x1C},  // from smart-rf

  {CC2500_AGCCTRL2_REG_ADDR,    0xC7},  // from smart-rf
  {CC2500_AGCCTRL1_REG_ADDR,    0x00},  // from smart-rf
  {CC2500_AGCCTRL0_REG_ADDR,    0xB0},  // from smart-rf

  {CC2500_FREND1_REG_ADDR,      0xB6},  // from smart-rf
  {CC2500_FREND0_REG_ADDR,      0x10},  // from smart-rf

  {CC2500_FSCAL3_REG_ADDR,      0xEA},  // from smart-rf
  {CC2500_FSCAL2_REG_ADDR,      0x0A},  // from smart-rf
  {CC2500_FSCAL1_REG_ADDR,      0x00},  // from smart-rf
  {CC2500_FSCAL0_REG_ADDR,      0x11},  // from smart-rf

  {CC2500_TEST2_REG_ADDR,       0x88},  // from smart-rf
  {CC2500_TEST1_REG_ADDR,       0x31},  // from smart-rf
  {CC2500_TEST0_REG_ADDR,       0x0b},  // from smart-rf

  {0xff,  0xff}
};

#ifndef RADIO_MOD_FMT_OOK
/*
 * The special PATABLE register can hold up to eight user selected output 
 * power settings. All the PA power settings in the PATABLE from index 0 
 * up to the FREND0.PA_POWER value are used.
 */
UINT8_t CC2500_paTableCfgTbl[CC2500_PA_TABLE_LEN] =
{
   0xc0,
   0x00,
   0x00,
   0x00,
   0x00,
   0x00,
   0x00,
   0x00
};
#else
#error specified modulation not supported !!
#endif

const CC2500_modFormat_t CC2500_modFormat = CC2500_MOD_FORMAT_GSK; // MDMCFG2.MOD_FORMAT[2:0]
#ifdef CC2500_WOR_ENA
const UINT16_t CC2500_preambleLen = 24;
#else
const UINT16_t CC2500_preambleLen = 4;   // MDMCFG1.NUM_PREAMBLE[2:0]
#endif
const UINT16_t CC2500_syncLen = 4;   // MDMCFG2.SYNC_MODE[2:0]
const UINT32_t CC2500_baudRate = 1200;   // MDMCFG4.DRATE_E[3:0] and MDMCFG4.DRATE_M[7:0]

/*
 * Note that TX to IDLE and TX to RX transition times are functions of data rate (fbaudrate).
 */

#ifdef RADIO_BAUD_RATE_1200
const UINT32_t CC2500_microSecsPerSym = 834;
const UINT16_t CC2500_txToIdleTransitionTimeMilli = 1;  // (834/4 -> 209 micros) <Calc .25/baud_rate>
#elif defined(RADIO_BAUD_RATE_10000)
// Programmed baud rate is 9992.6 baud
const UINT32_t CC2500_microSecsPerSym = 101;
const UINT16_t CC2500_txToIdleTransitionTimeMilli = 1;  // (100/4 -> 25 micros) <Calc .25/baud_rate>
#elif defined(RADIO_BAUD_RATE_38383)
const UINT32_t CC2500_microSecsPerSym = 26;
const UINT16_t CC2500_txToIdleTransitionTimeMilli = 1;  // (26/4 -> 6.5 micros) <Calc .25/baud_rate>
#elif defined(RADIO_BAUD_RATE_250000)
//
const UINT32_t CC2500_microSecsPerSym = 4;
const UINT16_t CC2500_txToIdleTransitionTimeMilli = 1;  // (4/4 -> 1 microsecond) <Calc .25/baud_rate>
#else
#error radio baud rate not specified !!
#endif


/*
 * WOR timing (with above settings) measured on scope
 *
 * Tevent1 = 1.390 millisecs
 * IDLE->RX with cal = 790 microsecs
 * RSSI response time = 1.460 millisec
 * RX -> IDLE -> SLEEP = 1.4 millisec
 */


const CC2500_state_t CC2500_FSM_STATE_MAPPING[ ] =
{
   CC2500_STATE_IDLE,
   CC2500_STATE_RX,
   CC2500_STATE_TX_IN_PROGRESS,
   CC2500_STATE_FSTXON,
   CC2500_STATE_CALIBRATE,
   CC2500_STATE_SETTLING,
   CC2500_STATE_RXFIFO_OVERFLOW,
   CC2500_STATE_TXFIFO_UNDERFLOW,
};


CC2500_cntxt_s  CC2500_cntxt;



/*
 ********************************************************************
 *
 * P3.0 is connected to CC2500 CSn
 *
 *
 *
 ********************************************************************
 */
#define CC2500_spiEnd( )  \
do \
{  \
   P3OUT |= (BIT0);  \
} while (0)


/*
 ********************************************************************
 *
 * P3.0 is connected to CC2500 CSn
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t CC2500_spiBegin(void)
{
  PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

  /*
   * When CSn is pulled low, the MCU must wait until CC2500 SO pin goes
   * low before starting to transfer the header byte. This indicates that
   * the crystal is running. Unless the chip was just reset or was in SLEEP
   * or XOFF state, the SO pin will always go low immediately after taking
   * CSn low.
   */

  P3OUT &= ~(BIT0);

  // Loop while the SO pin is high
  while (P3IN & BIT2);

  return sts;
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
#define CC2500_spiTxRxNoInt(wrVal, rdVal) \
do                               \
{                                \
   while ((UCB0STAT & UCBUSY));  \
   while (!(IFG2 & UCB0TXIFG));  \
   UCB0TXBUF = wrVal;            \
   while (!(IFG2 & UCB0RXIFG));  \
   rdVal = UCB0RXBUF;            \
} while (0)

/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t CC2500_spiTxRx(UINT8_t val)
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
PLTFRM_sts_t CC2500_readReg(UINT8_t regAddr, UINT8_t *regVal_p)
{
   PLTFRM_sts_t sts;
   UINT8_t hdrByte;

   /*
    * All transfers on the SPI interface are done most significant bit
    * first.  All transactions on the SPI interface start with a header
    * byte containing a R/W bit, a burst access bit (B), and a 6-bit
    * address (A5 - A0).
    *
    * For register addresses in the range 0x30-0x3D, the burst bit is used
    * to select between status registers when burst bit is one, and between
    * command strobes when burst bit is zero.
    */

   hdrByte = CC2500_SPI_HDR_RW_BIT;

   hdrByte |= regAddr;

   if (regAddr >= CC2500_PARTNUM_REG_ADDR)
       hdrByte |= CC2500_SPI_HDR_BURST_BIT;

   sts = CC2500_spiBegin();
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   /*
    * Table 22 
    * (Tsp) CSn low to positive edge on SCLK - 20 nanosecs minimum
    */
   PLTFRM_delay1MicroSecs();

   CC2500_spiTxRx(hdrByte);

   // PLTFRM_delay1MicroSecs();

   *(regVal_p) = CC2500_spiTxRx(0x0);

   /*
    * Table 22 
    * (Tns) - Negative edge on SCLK to CSn high - minimum 20 nanosecs
    */
   PLTFRM_delay1MicroSecs();

   CC2500_spiEnd();

   PLTFRM_delay1MicroSecs();

   return sts;
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
PLTFRM_sts_t CC2500_writeReg(UINT8_t regAddr, UINT8_t regVal)
{
   PLTFRM_sts_t sts;
   UINT8_t hdrByte = 0x0;
   
   hdrByte |= regAddr;
   
   if (regAddr >= CC2500_PARTNUM_REG_ADDR)
       hdrByte |= CC2500_SPI_HDR_BURST_BIT;

   sts = CC2500_spiBegin();
   if (sts != PLTFRM_STS_SUCCESS)
       return sts; 
   
   /*
    * All transfers on the SPI interface are done most significant bit 
    * first.  All transactions on the SPI interface start with a header 
    * byte containing a R/WÃ‚Â¯ bit, a burst access bit (B), and a 6-bit 
    * address (A5 - A0).
    */

   /*
    * Tsp - CSn low to positive edge on SCLK - 20 nanosecs minimum
    */  
   PLTFRM_delay1MicroSecs();
   
   CC2500_spiTxRx(hdrByte);

   PLTFRM_delay1MicroSecs();

   CC2500_spiTxRx(regVal);

   /*
    * Tns - Negative edge on SCLK to CSn high - minimum 20 nanosecs
    */  
   PLTFRM_delay1MicroSecs();

   CC2500_spiEnd();
  
   PLTFRM_delay1MicroSecs();

   return sts;
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
PLTFRM_sts_t CC2500_sendCmdStrobe(UINT8_t cmdStrobeId, CC2500_state_t *fsmState_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t stsByte, hdrByte = 0x0;

   /*
    * Command Strobes may be viewed as single byte instructions to CC2500. By
    * addressing a command strobe register, internal sequences will be started. 
    * These commands are used to enable disable the crystal oscillator, enable
    * receive mode, enable wake-on-radio etc. The 13 command strobes are listed 
    * in Table 42. The command strobe registers are accessed by transferring a 
    * single header byte (no data is being transferred). That is, only the R/W 
    * bit, the burst access bit (set to 0), and the six address bits (in the 
    * range 0x30 through 0x3D) are written. The R/W bit can be either one or 
    * zero and will determine how the FIFO_BYTES_AVAILABLE field in the status 
    * byte should be interpreted.
    *
    * When writing command strobes, the status byte is sent on the SO pin.
    */

   if (cmdStrobeId < CC2500_CMD_STROBE_SRES
       || cmdStrobeId > CC2500_CMD_STROBE_SNOP)
   {
       SYS_fatal(SYS_FATAL_ERR_150);
       return PLTFRM_STS_INV_PARAM;
   }

   hdrByte = cmdStrobeId;

   sts = CC2500_spiBegin();
   if (sts != PLTFRM_STS_SUCCESS)
       return sts; 

   /*
    * CSn low to positive edge on SCLK - 20 nanosecs minimum
    */  
   PLTFRM_delay1MicroSecs();


   // The status byte is available on the SO pin when a command strobe is being sent.
   // S6, S5, and S4 comprise the STATE value which reflects the state of the chip.

   stsByte = CC2500_spiTxRx(hdrByte);

   // Tns- Negative edge on SCLK to CSn high - minimum 20 nanosecs
   PLTFRM_delay1MicroSecs();

   CC2500_spiEnd();

   if (fsmState_p != NULL)
   {
       stsByte >>= CC2500_STS_BYTE_CHIP_STATE_SHIFT;
       stsByte &= CC2500_STS_BYTE_CHIP_STATE_BIT_MSK_SHIFT;
       *fsmState_p = CC2500_FSM_STATE_MAPPING[stsByte];
   }

   return sts;
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
PLTFRM_sts_t CC2500_updateRegField(const UINT8_t regAddr,
                                   const UINT8_t newVal,
                                   const UINT8_t bitMsk,
                                   const UINT8_t bitShift,
                                   const UINT8_t bitMskShifted)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;

   sts = CC2500_readReg(regAddr, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       UINT8_t encByte;
       regVal &= ~(bitMsk);

       encByte = (UINT8_t)newVal;
       encByte <<= bitShift;
       encByte &= bitMsk;

       regVal |= encByte;

       sts = CC2500_writeReg(regAddr, regVal);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           sts = CC2500_readReg(regAddr, &regVal);
           if (sts == PLTFRM_STS_SUCCESS)
           {
              regVal >>= bitShift;
              regVal &= bitMskShifted;
              if (regVal != (UINT8_t)newVal)
                  sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;
           }
       }
   }

   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t CC2500_cfgChipRegs(const CC2500_chipRegInfo_s *reg_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   while (reg_p->regAddr != 0xff)
   {
       sts = CC2500_updateRegField(reg_p->regAddr,
                                   reg_p->regVal,
                                   0xff, 0x0, 0xff);

       if (sts != PLTFRM_STS_SUCCESS)
           break;

       reg_p ++;
   }

   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t CC2500_cfgRegOnWakeUp(UINT8_t regAddr)
{
   PLTFRM_sts_t sts = PLTFRM_STS_REG_NOT_FOUND;
   const CC2500_chipRegInfo_s *info_p = &(CC2500_chipRegInfoList[0]);

   while (info_p->regAddr != 0xff)
   {
      if (info_p->regAddr == regAddr)
      {
          sts = CC2500_updateRegField(info_p->regAddr,
                                      info_p->regVal,
                                      0xff, 0x0, 0xff);
          break;
      }
      else
          info_p ++;
   }

   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t CC2500_verifyPATable(void)
{
   UINT16_t hdrByte = CC2500_SPI_HDR_RW_BIT;
   UINT16_t tblIdx;
   PLTFRM_sts_t sts;

   hdrByte |= CC2500_PA_TABLE_ADDR;
   
   sts = CC2500_spiBegin();
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   for (tblIdx=0; tblIdx<CC2500_PA_TABLE_LEN; tblIdx++)
   {
       UINT8_t regVal;

       PLTFRM_delay10MicroSecs();
       CC2500_spiTxRx(hdrByte);
       PLTFRM_delay10MicroSecs();
       regVal = CC2500_spiTxRx(0x0);
       if (regVal != CC2500_paTableCfgTbl[tblIdx])
       {
           sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;
           break;
       }
   }

   PLTFRM_delay10MicroSecs();

   CC2500_spiEnd();

   PLTFRM_delay10MicroSecs();
   
   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t CC2500_cfgPATable(void)
{
   UINT8_t hdrByte = 0x0;
   UINT16_t tblIdx;
   PLTFRM_sts_t sts;

   /*
    * The SPI expects up to eight data bytes after receiving the address.
    *
    * The table is written and read from the lowest setting (0) to the 
    * highest (7), one byte at a time. An index counter is used to control 
    * the access to the table. This counter is incremented each time a
    * byte is read or written to the table, and set to the lowest index when 
    * CSn is high. When the
    *
    * The access to the PATABLE is either single byte or burst access 
    * depending on the burst bit.
    */
   
   hdrByte |= CC2500_PA_TABLE_ADDR;
   
   sts = CC2500_spiBegin();
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;
       
   for (tblIdx=0; tblIdx<CC2500_PA_TABLE_LEN; tblIdx++)
   {
       PLTFRM_delay10MicroSecs();
       CC2500_spiTxRx(hdrByte);
       PLTFRM_delay10MicroSecs();
       CC2500_spiTxRx(CC2500_paTableCfgTbl[tblIdx]);
   }

   PLTFRM_delay10MicroSecs();

   CC2500_spiEnd();

   PLTFRM_delay10MicroSecs();
   
   return sts;
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
PLTFRM_sts_t CC2500_waitTillRSSIValid(void)
{
   UINT16_t idx;

   /*
    * It takes some time from the radio enters RX mode until a valid RSSI value is
    * present in the RSSI register. Please see DN505 [12] for details on how the RSSI
    * response time can be estimated.
    *
    * Refer to swra114d.pdf for details.
    *
    * For now, just wait for x millisecs while the CC2500 starts getting valid RSSI
    * values.
    *
    * If this delay is not correct (less than required), tx-on-cca will not succeed.
    */

#ifdef RADIO_BAUD_RATE_1200
   for (idx=0; idx<2; idx++)    // 1.5234 milli
#elif defined(RADIO_BAUD_RATE_38383)  // 462 micros
   for (idx=0; idx<1; idx++)
#elif defined(RADIO_BAUD_RATE_10000)  // 462 micros
   for (idx=0; idx<1; idx++)
#elif defined(RADIO_BAUD_RATE_250000)  // 462 micros
   for (idx=0; idx<1; idx++)
#else
#error baud rate not supported !!
#endif
	   PLTFRM_delay1MilliSec();

   return PLTFRM_STS_SUCCESS;
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
SINT16_t CC2500_calcPktRSSI(UINT8_t rssiRaw)
{
   SINT16_t rssi;

   /*
    * The RSSI value read from the RSSI status register is a 2â€™s complement
    * number. The following procedure can be used to convert the RSSI reading
    * to an absolute power level (RSSI_dBm)
    *
    * 1) Read the RSSI status register
    * 2) Convert the reading from a hexadecimal number to a decimal number (RSSI_dec)
    * 3) If RSSI_dec >= 128 then RSSI_dBm = (RSSI_dec - 256)/2 - RSSI_offset
    * 4) Else if RSSI_dec < 128 then RSSI_dBm = (RSSI_dec)/2 - RSSI_offset
    *
    * Ref Table 25
    * -------------------------------------
    * Data Rate [kBaud]    RSSI_offset [dB]
    * -------------------------------------
    *          2.4            71
    *          10             69
    *          250            72
    *          500            72
    * -------------------------------------
    */

   if (rssiRaw >= 128)
       rssi = (rssiRaw - 256);
   else
       rssi = rssiRaw;
   rssi >>= 1;
#ifdef RADIO_BAUD_RATE_250000
   rssi -= 72;
#elif RADIO_BAUD_RATE_10000
   rssi -= 69;
#elif RADIO_BAUD_RATE_2400
   rssi -= 71;
#else
   rssi = -70;
#endif

   return rssi;
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
PLTFRM_sts_t CC2500_getCurrState(CC2500_state_t *state_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

#ifdef CC2500_WOR_ENA
   if (CC2500_cntxt.inWOR)
 	   *state_p = CC2500_STATE_WOR;
   else
#endif
   {
#if 0
       if (CC2500_cntxt.inLPM)
           *state_p = CC2500_STATE_SLEEP;
       else
#endif
       sts = CC2500_sendCmdStrobe(CC2500_CMD_STROBE_SNOP, state_p);
   }

   return sts;
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
PLTFRM_sts_t CC2500_forceIdle(void)
{
   CC2500_state_t state;
   PLTFRM_sts_t sts;

   /*
    * The SIDLE command strobe can always be used to force the radio
    * controller to go to the IDLE state.
    * If the radio goes from TX or RX to IDLE by issuing an SIDLE 
    * strobe, calibration will not be performed.  
    * When the SIDLE probe is issued, the radio will exit RX / TX, 
    * turn off frequency synthesizer and exit Wake-On-Radio mode if 
    * applicable.
    */
   sts = CC2500_sendCmdStrobe(CC2500_CMD_STROBE_SIDLE, NULL);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   do
   {
       sts = CC2500_getCurrState(&state);
       if (sts != PLTFRM_STS_SUCCESS)
           break;
       PLTFRM_delay100MicroSecs();
   } while (state != CC2500_STATE_IDLE);

   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t CC2500_disableCCA(void)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;
   const UINT8_t regAddr = CC2500_MCSM1_REG_ADDR;

   sts = CC2500_readReg(regAddr, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_151);
   else
   {
       regVal &= ~(CC2500_MCSM1_REG_CCA_MODE_FIELD_BIT_MSK);

       // Mode 00 - always give clear channel indication

       sts = CC2500_writeReg(regAddr, regVal);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_152);

       sts = CC2500_readReg(regAddr, &regVal);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_153);
       else
       {
           if (regVal & CC2500_MCSM1_REG_CCA_MODE_FIELD_BIT_MSK)
           {
               sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;
               SYS_fatal(SYS_FATAL_ERR_154);
           }
       }
   }

   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT16_t CC2500_calcPktTrxTime(UINT16_t pktLen)
{
   UINT16_t temp16, totSymCnt;
   UINT32_t pktTxTime;

   /*
    * Returns packet transmission time in milli-secs.
    *
    * The modulator will first send the programmed number of preamble bytes.
    * If data is available in the TX FIFO, the modulator will send the two-byte 
    * (optionally 4-byte) sync word followed by the payload in the TX FIFO.
    * If CRC is enabled, the checksum is calculated over all the data pulled 
    * from the TX FIFO, and the result is sent as two extra bytes following the
    * payload data.
    *
    * Symbol rate is configured using MDMCFG4.DRATE_E[3:0] & MDMCFG3.DRATE_M[7:0]
    *
    * When selecting 4-FSK, the preamble and sync word is sent using 2-FSK (see 
    * Figure 21).
    */

   temp16 = CC2500_preambleLen;
   temp16 <<= 3;   // In bits
   totSymCnt = temp16;

   temp16 = CC2500_syncLen;
   temp16 <<= 3;   // In bits
   totSymCnt += temp16;

   temp16 = pktLen + CC2500_FRAME_CRC_FIELD_LEN;
   temp16 <<= 3;   // In bits
   if (CC2500_modFormat == CC2500_MOD_FORMAT_4_FSK)
       temp16 >>= 1;  // 2 bits per symbol
   totSymCnt += temp16; 
   
   pktTxTime = totSymCnt;
   pktTxTime *= CC2500_microSecsPerSym;
   pktTxTime /= 1000;
   pktTxTime ++;

   return pktTxTime;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t CC2500_enableCCA(void)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;
   const UINT8_t regAddr = CC2500_MCSM1_REG_ADDR;

   sts = CC2500_readReg(regAddr, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       UINT8_t newMode;

       regVal &= ~(CC2500_MCSM1_REG_CCA_MODE_FIELD_BIT_MSK);

       newMode = CC2500_CCA_MODE_3;  // Indicates clear channel when RSSI is below
                                     // threshold and currently not receiving a packet

       newMode <<= CC2500_MCSM1_REG_CCA_MODE_FIELD_SHIFT;
       newMode &= CC2500_MCSM1_REG_CCA_MODE_FIELD_BIT_MSK;

       regVal |= newMode;

       sts = CC2500_writeReg(regAddr, regVal);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           sts = CC2500_readReg(regAddr, &regVal);
           if (sts == PLTFRM_STS_SUCCESS)
           {
               newMode = (regVal >> CC2500_MCSM1_REG_CCA_MODE_FIELD_SHIFT);
               newMode &= CC2500_MCSM1_REG_CCA_MODE_FIELD_SHIFT_MSK;
               if (newMode != CC2500_CCA_MODE_3)
                   sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;
           }
       }
   }

   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t CC2500_startRx(void)
{
   PLTFRM_sts_t sts;
   CC2500_state_t cc2500State;

   /*
    * <SRX cmd strobe>
    * Enable RX. Perform calibration first if coming from IDLE and
    * SETTLING_CFG.FS_AUTOCAL = 1.
    *
    * IDLE -> Frequency synthesizer is turned on, can optionally be
    * calibrated, and then settles to the correct frequency -> RX.
    */

   sts = CC2500_sendCmdStrobe(CC2500_CMD_STROBE_SRX, NULL);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_155);
   }
   else
   {
       do
       {
           sts = CC2500_sendCmdStrobe(CC2500_CMD_STROBE_SNOP, &cc2500State);
           if (sts != PLTFRM_STS_SUCCESS)
           {
               SYS_fatal(SYS_FATAL_ERR_156);
           }
       }  while (cc2500State != CC2500_STATE_RX);
   }

   return sts;
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
PLTFRM_sts_t CC2500_getRxFIFONextByte(UINT8_t *buff_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t hdrByte = CC2500_SPI_HDR_RW_BIT;
   
   /*
    * The 64-byte TX FIFO and the 64-byte RX FIFO are accessed through 
    * the 0x3F address. When the R/W bit is zero, the TX FIFO is accessed, 
    * and the RX FIFO is accessed when the R/W bit is one.
    */

   hdrByte |= CC2500_RX_FIFO_ADDR;

   sts = CC2500_spiBegin();
   if (sts != PLTFRM_STS_SUCCESS)
       return sts; 

   /*
    * Table 22 
    * (Tsp) CSn low to positive edge on SCLK - 20 nanosecs minimum
    */
   PLTFRM_delay1MicroSecs();

   CC2500_spiTxRx(hdrByte);   // Send 0x3f
   
   PLTFRM_delay1MicroSecs();
   
   *(buff_p) = CC2500_spiTxRx(0x0);  // Read 1 byte

   /*
    * Table 22 
    * (Tns) - Negative edge on SCLK to CSn high - minimum 20 nanosecs
    */
   PLTFRM_delay1MicroSecs();

   CC2500_spiEnd();

   return sts;
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
PLTFRM_sts_t CC2500_flushTxFIFO(void)
{
   /*
    * Flush the TX FIFO. Only issue in IDLE or TX_FIFO_UNDERFLOW states
    */
   return CC2500_sendCmdStrobe(CC2500_CMD_STROBE_SFTX, NULL);
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
PLTFRM_sts_t CC2500_flushRxFIFO(void)
{
   /*
    * Flush the RX FIFO. Only issue in IDLE or RX_FIFO_OVERFLOW states
    */
   return CC2500_sendCmdStrobe(CC2500_CMD_STROBE_SFRX, NULL);
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
PLTFRM_sts_t CC2500_writeByteToTxFIFO(UINT8_t byte)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t hdrByte = 0x0;

   /*
    * The 64-byte TX FIFO and the 64-byte RX FIFO are accessed through 
    * the 0x3F address. When the R/W bit is zero, the TX FIFO is accessed, 
    * and the RX FIFO is accessed when the R/W bit is one.
    *
    * The burst bit is used to determine if the FIFO access is a single byte 
    * access or a burst access. The single byte access method expects a header 
    * byte with the burst bit set to zero and one data byte. After the data byte, 
    * a new header byte is expected; hence, CSn can remain low.
    */

   hdrByte |= CC2500_TX_FIFO_ADDR;

   sts = CC2500_spiBegin();
   if (sts != PLTFRM_STS_SUCCESS)
       return sts; 

   /*
    * Table 22 
    * (Tsp) CSn low to positive edge on SCLK - 20 nanosecs minimum
    */
   PLTFRM_delay1MicroSecs();

   CC2500_spiTxRx(hdrByte);   // Send 0x3f
   
   PLTFRM_delay1MicroSecs();
   
   CC2500_spiTxRx(byte);  // Write 1 byte

   /*
    * Table 22 
    * (Tns) - Negative edge on SCLK to CSn high - minimum 20 nanosecs
    */
   PLTFRM_delay1MicroSecs();

   CC2500_spiEnd();

   return sts;
}


#if 0
UINT8_t __rxFifoByteCnt;
UINT8_t CC2500_rxFifoOvflBuff[CC2500_RX_FIFO_LEN];
#endif

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void CC2500_rxOvflHndlr(void)
{
   PLTFRM_sts_t sts;

#if 0
   sts = CC2500_getRXFIFOByteCnt(&__rxFifoByteCnt);
   if (sts != PLTFRM_STS_SUCCESS || __rxFifoByteCnt == 0)
   {
       SYS_fatal(SYS_FATAL_ERR_157);
   }

   sts = CC2500_readRxFIFO(__rxFifoByteCnt, CC2500_rxFifoOvflBuff);
   if (sts != PLTFRM_STS_SUCCESS || __rxFifoByteCnt == 0)
   {
       SYS_fatal(SYS_FATAL_ERR_158);
   }

   if (CC2500_forceIdle() != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_159);
#endif

   // State will change from RX_OVERFLOW to IDLE
   if (CC2500_flushRxFIFO() != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_160);

   sts = CC2500_startRx();
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_161);

   return;
}

UINT8_t __rxFIFOByteCnt = 0, __rxFIFOHasDataCnt = 0;
CC2500_state_t  CC2500_radioCheckState;
UINT16_t CC2500_rxFIFOOvflCnt = 0;

void __junkF(void)
{
   CC2500_rxFIFOOvflCnt ++;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void CC2500_checkRadioErrors(void)
{
   PLTFRM_sts_t sts;

#ifdef DEV_TYPE_RFD
   sts = CC2500_getRXFIFOByteCnt(&__rxFIFOByteCnt);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_162);

   if (__rxFIFOByteCnt)
       __rxFIFOHasDataCnt ++;
#endif

   sts = CC2500_getCurrState(&CC2500_radioCheckState);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       switch (CC2500_radioCheckState)
       {
          case CC2500_STATE_RXFIFO_OVERFLOW:
               {
                  CC2500_rxFIFOOvflCnt ++;
                  CC2500_rxOvflHndlr();
               }
               break;

          case CC2500_STATE_TXFIFO_UNDERFLOW:
               // Can never happen !!
               SYS_fatal(SYS_FATAL_ERR_163);
               break;

          case CC2500_STATE_RX:
               {
                  UINT8_t rxFifoByteCnt;
                  sts = CC2500_getRXFIFOByteCnt(&rxFifoByteCnt);
                  if (rxFifoByteCnt > 0)
                  {
                      __junkF();
                  }
               }
               break;

          default:
               break;
       }
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
UINT8_t CC2500_txPowerSetPointValid(SINT16_t powerInDbm)
{
   UINT8_t rc = 0;

   // TODO

   return rc;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT32_t CC2500_getCarrierFreq(UINT8_t channIdx)
{
   UINT32_t cf = CC2500_channelSpacingHz;
   cf *= channIdx;
   cf += CC2500_baseFreqHzOff;
   return cf;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t CC2500_setTxPower(SINT16_t intPart,
                               SINT16_t decPart)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t enc = 0;
   SINT32_t pwr = intPart;

   pwr *= 100;
   if (pwr < 0)
       pwr -= decPart;
   else
       pwr += decPart;

   if (pwr == 100)
       enc = CC2500_TX_PWR_1_DBM_PATABLE_ENC;
   else
       sts = PLTFRM_STS_INV_PARAM;

   if (sts == PLTFRM_STS_SUCCESS)
   {
       CC2500_paTableCfgTbl[0] = 0xfe;  // enc;
       sts = CC2500_cfgPATable();
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_164);

       sts = CC2500_verifyPATable();
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_165);
   }

   return sts;
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
PLTFRM_sts_t CC2500_getTXFIFOByteCnt(UINT8_t *byteCnt_p)
{
   PLTFRM_sts_t sts;

   sts = CC2500_readReg(CC2500_TXBYTES_REG_ADDR, byteCnt_p);
   if (sts == PLTFRM_STS_SUCCESS)
       (*byteCnt_p) &= CC2500_NUM_TXBYTES_BIT_MSK;

   return sts;
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
PLTFRM_sts_t CC2500_readRxFIFO(UINT8_t cnt, UINT8_t *buff_p)
{
   PLTFRM_sts_t sts;
   SINT16_t idx;

#if 1
   // Read the RX FIFO using burst access mode.
   {
      UINT8_t hdrByte = CC2500_SPI_HDR_RW_BIT | CC2500_SPI_HDR_BURST_BIT | CC2500_RX_FIFO_ADDR;
      UINT8_t dummyByte, globalIntFlag;

      PLTFRM_intSaveDisable(globalIntFlag);

      /*
       * The 64-byte TX FIFO and the 64-byte RX FIFO are accessed through
       * the 0x3F address. When the R/W bit is zero, the TX FIFO is accessed,
       * and the RX FIFO is accessed when the R/W bit is one.
       *
       * 0xFF: Burst access to RX FIFO
       */
      sts = CC2500_spiBegin();
      if (sts != PLTFRM_STS_SUCCESS)
          return sts;

      /*
       * Table 22
       * (Tsp) CSn low to positive edge on SCLK - 20 nanosecs minimum
       */
      PLTFRM_delay1MicroSecs();

      CC2500_spiTxRxNoInt(hdrByte, dummyByte);

      (void)dummyByte;

      for (idx=0; idx<cnt; idx++)
      {
    	  UINT8_t byte;
          CC2500_spiTxRxNoInt(0x0, byte);
          if (buff_p != NULL)
        	  buff_p[idx] = byte;
      }
      /*
       * Table 22
       * (Tns) - Negative edge on SCLK to CSn high - minimum 20 nanosecs
       */
      PLTFRM_delay1MicroSecs();

      CC2500_spiEnd();

      PLTFRM_intRestore(globalIntFlag);
   }
#else
   for (idx=0; idx<cnt; idx++)
   {
       sts = CC2500_getRxFIFONextByte(buff_p);
       if (sts != PLTFRM_STS_SUCCESS)
           break;
       buff_p ++;
   }
#endif

   return sts;
}


#if 1
/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t CC2500_writeBuffToTxFIFO(UINT8_t *buff_p, UINT8_t buffLen)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t globalIntFlag, dummyByte,
           hdrByte =  CC2500_SPI_HDR_BURST_BIT | CC2500_TX_FIFO_ADDR;

   if (buffLen > CC2500_MAX_TX_PKT_LEN)
   {
       SYS_fatal(SYS_FATAL_ERR_166);
       // return PLTFRM_STS_FRAME_TOO_LONG_FOR_TX_FIFO;
   }

   /* 
    * The 64-byte TX FIFO and the 64-byte RX FIFO are accessed through the 
    * 0x3F address. When the R/W bit is zero, the TX FIFO is accessed, and 
    * the RX FIFO is accessed when the R/W bit is one.
    * The TX FIFO is write-only, while the RX FIFO is read-only.
    * 
    * Note that in cc1200, the FIFOs are 128 bytes long :-(.
    *
    * This implementation does not use address filtering.
    *
    * The payload that is to be transmitted must be written into the TX FIFO. 
    * The first byte written must be the length byte when variable packet 
    * length is enabled. The length byte has a value equal to the payload of 
    * the packet 
    */

   /*
    * The payload that is to be transmitted must be written into the TX
    * FIFO. The first byte written must be the length byte when variable
    * packet length is enabled (PKT_CFG0.LENGTH_CONFIG = 1). The length
    * byte has a value equal to the payload of the packet (including the
    * optional address byte). If address recognition is enabled in the
    * receiver (PKT_CFG1.ADDR_CHECK_CFG != 0) the second byte written to
    * the TX FIFO must be the address byte (as the address in not
    * automatically inserted).
    *
    * Note that the length byte is sent over the air.
    *
    * We are not going to enable address recognition in the receiver.
    *
    * 0x7F: Burst access to TX FIFO
    */

   PLTFRM_intSaveDisable(globalIntFlag);

   sts = CC2500_spiBegin();
   if (sts != PLTFRM_STS_SUCCESS)
   {
       PLTFRM_intRestore(globalIntFlag);
       return sts;
   }

   /*
    * Table 22
    * (Tsp) CSn low to positive edge on SCLK - 20 nanosecs minimum
    */
   PLTFRM_delay1MicroSecs();

   CC2500_spiTxRxNoInt(hdrByte, dummyByte);
   CC2500_spiTxRxNoInt(buffLen, dummyByte);

   (void)dummyByte;

   {
      SINT16_t idx;
      for (idx=0; idx<buffLen; idx++)
           CC2500_spiTxRxNoInt(buff_p[idx], dummyByte);
   }

   /*
    * Table 22
    * (Tns) - Negative edge on SCLK to CSn high - minimum 20 nanosecs
    */
   PLTFRM_delay1MicroSecs();

   CC2500_spiEnd();

   PLTFRM_intRestore(globalIntFlag);

   PLTFRM_delay1MicroSecs();

   {
      UINT8_t byteCnt;

      sts = CC2500_getTXFIFOByteCnt(&byteCnt);
      if (sts != PLTFRM_STS_SUCCESS)
          SYS_fatal(SYS_FATAL_ERR_171);

      if (byteCnt != (buffLen + 1))
          SYS_fatal(SYS_FATAL_ERR_172);
   }

   return sts;
}
#else
/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t CC2500_writeBuffToTxFIFO(UINT8_t *buff_p, UINT8_t buffLen)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   if (buffLen > (CC2500_TX_FIFO_LEN - CC2500_FRAME_LENGTH_FIELD_LEN))
   {
       SYS_fatal(SYS_FATAL_ERR_166);
       return PLTFRM_STS_FRAME_TOO_LONG_FOR_TX_FIFO;
   }

   /*
    * The 64-byte TX FIFO and the 64-byte RX FIFO are accessed through the
    * 0x3F address. When the R/W bit is zero, the TX FIFO is accessed, and
    * the RX FIFO is accessed when the R/W bit is one.
    * The TX FIFO is write-only, while the RX FIFO is read-only.
    *
    * Note that in cc1200, the FIFOs are 128 bytes long :-(.
    *
    * This implementation does not use address filtering.
    *
    * The payload that is to be transmitted must be written into the TX FIFO.
    * The first byte written must be the length byte when variable packet
    * length is enabled. The length byte has a value equal to the payload of
    * the packet
    */

   {
       UINT8_t byteCnt;
       SINT16_t idx;

       if (buffLen > (CC2500_TX_FIFO_LEN - CC2500_FRAME_LENGTH_FIELD_LEN))
       {
           SYS_fatal(SYS_FATAL_ERR_167);
           return PLTFRM_STS_FRAME_TOO_LONG_FOR_TX_FIFO;
       }

       sts = CC2500_getTXFIFOByteCnt(&byteCnt);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_168);

       if (byteCnt > 0)
       {
           // TX FIFO is not empty !! Need to empty the FIFO
           // Cannot use the SFTX strobe since it can be used only in IDLE and
           // TX_FIFO_ERR states !!

           return PLTFRM_STS_RADIO_TX_FIFO_NOT_EMPTY;
       }

       /*
        * The payload that is to be transmitted must be written into the TX
        * FIFO. The first byte written must be the length byte when variable
        * packet length is enabled (PKT_CFG0.LENGTH_CONFIG = 1). The length
        * byte has a value equal to the payload of the packet (including the
        * optional address byte). If address recognition is enabled in the
        * receiver (PKT_CFG1.ADDR_CHECK_CFG != 0) the second byte written to
        * the TX FIFO must be the address byte (as the address in not
        * automatically inserted).
        *
        * Note that the length byte is sent over the air.
        *
        * We are not going to enable address recognition in the receiver.
        */

       sts = CC2500_writeByteToTxFIFO(buffLen);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_169);

        // Now write the actual payload
       for (idx=0; idx<buffLen; idx++)
       {
            sts = CC2500_writeByteToTxFIFO(*(buff_p + idx));
            if (sts !=  PLTFRM_STS_SUCCESS)
                SYS_fatal(SYS_FATAL_ERR_170);
       }

       sts = CC2500_getTXFIFOByteCnt(&byteCnt);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_171);

       if (byteCnt != (buffLen + 1))
           SYS_fatal(SYS_FATAL_ERR_172);
   }

   return sts;
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
PLTFRM_sts_t CC2500_transferPktToTxFifo(UINT8_t bdHndl)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   if (!(SYS_BD_HNDL_IS_VALID(bdHndl)))
   {
       SYS_fatal(SYS_FATAL_ERR_173);
   }

   sts = CC2500_writeBuffToTxFIFO(SYS_GET_BD_BUFF(bdHndl),
                                  SYS_GET_BD_LEN(bdHndl));

   return sts;
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
PLTFRM_sts_t CC2500_getRXFIFOByteCnt(UINT8_t *byteCnt_p)
{
   PLTFRM_sts_t sts;

   sts = CC2500_readReg(CC2500_RXBYTES_REG_ADDR, byteCnt_p);
   if (sts == PLTFRM_STS_SUCCESS)
       (*byteCnt_p) &= CC2500_NUM_RXBYTES_BIT_MSK;
 
   return sts;
}

#ifdef CC2500_WOR_ENA
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t CC2500_exitWOR(void)
{
   PLTFRM_sts_t sts;

   // To exit WOR mode, an SIDLE strobe must be issued.
   // The SIDLE command strobe can always be used to force the radio
   // controller to go to the IDLE state.
   sts = CC2500_sendCmdStrobe(CC2500_CMD_STROBE_SIDLE, NULL);
   if (sts == PLTFRM_STS_SUCCESS)
       CC2500_cntxt.inWOR = 0;
   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t CC2500_enterWOR(void)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;

   sts = CC2500_sendCmdStrobe(CC2500_CMD_STROBE_SWOR, NULL);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_807);

   regVal = 0x6 | CC2500_IOCFGX_IRQ_INVERT_BIT;

   // Asserts when sync word has been sent / received, and de-asserts at the
   // end of the packet. In RX, the pin will also deassert when a packet is
   // discarded due to address or maximum length filtering or when the radio
   // enters RXFIFO_OVERFLOW state. In TX the pin will de-assert if the TX
   // FIFO underflows.

   sts = CC2500_updateRegField(CC2500_IOCFG0_REG_ADDR,
                               regVal, 0xff, 0x0, 0xff);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_808);

   /*
    * A “Preamble Quality Reached” signal can be observed on one of the GDO pins by
    * setting IOCFGx.GDOx_CFG=8.
    * We have set PKTCTRL1.PQT to 2. As a result, the preamble quality qualifier of
    * the sync word is enabled. This adds the requirement that the received sync word
    * must be preceded with a preamble with a quality above the programmed threshold.
    * The preamble quality estimator increases an internal counter by one each time a
    * bit is received that is different from the previous bit, and decreases the counter
    * by eight each time a bit is received that is the same as the last bit.
    */

   CC2500_cntxt.inWOR = 1;

   return sts;
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
PLTFRM_sts_t CC2500_cfgChan(UINT8_t channIdx)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;
   /*
    * Channel index range (0 - 8)
    */
   if (channIdx > PHY_2400_MHZ_BAND_CHAN_6)
       return PLTFRM_STS_INV_PARAM;

   regVal = channIdx;

   sts =  CC2500_writeReg(CC2500_CHANNR_REG_ADDR, regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_174);
   regVal = 0x0;
   sts = CC2500_readReg(CC2500_CHANNR_REG_ADDR, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_175);
   if (regVal != channIdx)
       SYS_fatal(SYS_FATAL_ERR_176);

   return PLTFRM_STS_SUCCESS;
}

#ifdef MAC_TRACE_ENA
extern UTIL_traceCntxt_s MAC_traceCntxt;
#endif

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t CC2500_spiInit(void)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   // CC2500 CSn - default HIGH

#ifdef GPIO_MGMT_ENA
   sts = PLTFRM_allocGPIOPin(PLTFRM_CC2500_1_DEV_ID,
                             PLTFRM_GPIO_PORT_3,
                             PLTFRM_GPIO_PORT_PIN_0,
                             PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                             PLTFRM_GPIO_OUTPUT_HIGH,
                             PLTFRM_GPIO_INT_EDGE_NA,
                             NULL);
#else
   P3DIR |= BIT0;
   P3OUT |= BIT0;
   P3SEL &= ~BIT0;
#endif

   return sts;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void CC2500_irqHndlr(void)
{
#ifdef CC2500_WOR_ENA
    UINT8_t reg = P2IFG;

    /*
     * > GDO0  (Connected to micro P2_6) : 0x06
     *   Asserts when sync word has been sent / received, and de-asserts at
     *   the end of the packet. In RX, the pin will also deassert when a pkt
     *   is discarded due to address or maximum length filtering or when the
     *   radio enters RXFIFO_OVERFLOW state. In TX the pin will de-assert if
     *   the TX FIFO underflows.
     */
    if (reg & BIT6)
    {
        P2IFG &= ~BIT6;

        if (CC2500_cntxt.inWOR)
        {
#ifdef CC2500_INTR_STATS_ENA
            CC2500_cntxt.syncRcvdIntCnt ++;
#endif
            CC2500_cntxt.syncRcvdFlag = 1;
            CC2500_cntxt.inWOR = 0;   // Actively receiving
        }
        else
        {
#ifdef CC2500_INTR_STATS_ENA
            CC2500_cntxt.txStartedFlagIntCnt ++;
#endif
            CC2500_cntxt.txStartedFlag = 1;
        }
    }

    /*
     * There are two GPIO pins available.
     * > GDO2  (Connected to micro P2_7)
     *   Digital output.
     *   Packet with good CRC received
     */
    if (reg & BIT7)
    {
        P2IFG &= ~BIT7;
#ifdef CC2500_INTR_STATS_ENA
        CC2500_cntxt.crcOkPktRcvdIntCnt ++;
#endif
        CC2500_cntxt.crcOkPktInRxFIFOCnt ++;

        CC2500_cntxt.syncRcvdFlag = 0;

#ifdef MAC_TRACE_ENA
        UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_105);
#endif

        SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_FRAME_RX_DONE;
    }
#else
    UINT8_t reg = P2IFG;

    /*
     * > GDO0  (Connected to micro P2_6)
     *   Digital output.
     *   PA_PD
     */
    if (reg & BIT6)
    {
        P2IFG &= ~BIT6;

#ifdef CC2500_INTR_STATS_ENA
        CC2500_cntxt.txStartedFlagIntCnt ++;
#endif
        CC2500_cntxt.txStartedFlag = 1;
    }

    /*
     * There are two GPIO pins available.
     * > GDO2  (Connected to micro P2_7)
     *   Digital output.
     *   Packet with good CRC received
     */
    if (reg & BIT7)
    {
        P2IFG &= ~BIT7;

#ifdef CC2500_INTR_STATS_ENA
        CC2500_cntxt.crcOkPktRcvdIntCnt ++;
#endif
        CC2500_cntxt.crcOkPktInRxFIFOCnt ++;

#ifdef MAC_TRACE_ENA
        UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_105);
#endif

        SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_FRAME_RX_DONE;
    }
#endif
}


#ifdef PLTFRM_CC2500_RSSI_MON_ENA
#define CC2500_RSSI_MON_MAX_BIN_CNT  32

UINT16_t CC2500_rssiMonBins[CC2500_RSSI_MON_MAX_BIN_CNT];
SINT16_t CC2500_rssiMonBinIdx[CC2500_RSSI_MON_MAX_BIN_CNT];


/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
void CC2500_monRSSI(RADIO_channMonReq_s *reqParams_p)
{
  UINT16_t idx;
  SINT32_t totalRSSI = 0;
  UINT8_t binsInUse, mdmCfg2RegVal;
  SINT16_t binSpan, rssiRange;
  PLTFRM_sts_t sts;

  reqParams_p->minMeasRSSI = 1000;
  reqParams_p->maxMeasRSSI = -1000;

#ifdef PLTFRM_CC2500_RSSI_MON_ENA
  memset(CC2500_rssiMonBins, 0, sizeof(CC2500_rssiMonBins));

  for (idx=0; idx<CC2500_RSSI_MON_MAX_BIN_CNT; idx++)
       CC2500_rssiMonBinIdx[idx] = idx;
#endif

#if 0
  sts = CC2500_cfgChan(reqParams_p->channIdx);
  if (sts != PLTFRM_STS_SUCCESS)
      SYS_fatal(SYS_FATAL_ERR_848);
#endif

  // Disable SYNC word detection
  {
     UINT8_t regVal;

     // The values 0 (000) and 4 (100) disables preamble and sync word transmission
     // in TX and preamble and sync word detection in RX.

     sts = CC2500_readReg(CC2500_MDMCFG2_REG_ADDR, &mdmCfg2RegVal);
     if (sts != PLTFRM_STS_SUCCESS)
         SYS_fatal(SYS_FATAL_ERR_847);

     regVal = mdmCfg2RegVal;

     regVal &= ~(0x7);
     sts = CC2500_writeReg(CC2500_MDMCFG2_REG_ADDR, regVal);
     if (sts != PLTFRM_STS_SUCCESS)
         SYS_fatal(SYS_FATAL_ERR_845);
  }

  if (CC2500_startRx() != PLTFRM_STS_SUCCESS)
      SYS_fatal(SYS_FATAL_ERR_855);

  for (idx=0; idx<100; idx++)
       PLTFRM_delay1MilliSec();

  PLTFRM_OFF_LED_1();
  PLTFRM_OFF_LED_2();

  /*
   * In RX mode, the RSSI value can be read continuously from the RSSI status
   * register until the demodulator detects a sync word (when sync word detection
   * is enabled). At that point the RSSI readout value is frozen until the next
   * time the chip enters RX mode. If sync word detection is disabled, the RSSI
   * register will be continuously updated.
   *
   * SPI read synchronization results in incorrect read values for register fields
   * that are continuously updated. The RSSI register read out is affected by this
   * bug.
   *
   * Workaround: Always average RSSI and LQI values over several packets before
   * using them in decision algorithms (that is, for FH channel selection)
   */

  binSpan = 1;
  rssiRange = reqParams_p->maxMonRSSI - reqParams_p->minMonRSSI + 1;
  do
  {
     binsInUse = rssiRange / binSpan;
     if (binsInUse <= (CC2500_RSSI_MON_MAX_BIN_CNT - 2))
         break;
     binSpan ++;
  } while (binSpan <= rssiRange);

  for (idx=0; idx<reqParams_p->sampleCnt; idx++)
  {
      UINT8_t rssiU8;
      SINT16_t rssiS16;

      if (CC2500_readReg(CC2500_RSSI_REG_ADDR, &rssiU8) != PLTFRM_STS_SUCCESS)
          SYS_fatal(SYS_FATAL_ERR_843);
      rssiS16 = CC2500_calcPktRSSI(rssiU8);
      totalRSSI += rssiS16;

      if (rssiS16 < reqParams_p->minMeasRSSI)
    	  reqParams_p->minMeasRSSI = rssiS16;

      if (rssiS16 > reqParams_p->maxMeasRSSI)
    	  reqParams_p->maxMeasRSSI = rssiS16;

	  if (rssiS16 < reqParams_p->minMonRSSI)
          CC2500_rssiMonBins[0] ++;
	  else
	  {
	      if (rssiS16 > reqParams_p->maxMonRSSI)
              CC2500_rssiMonBins[binsInUse-1] ++;
          else
          {
              SINT16_t binIdx = rssiS16;
              binIdx -=  reqParams_p->minMonRSSI;
              binIdx /= binSpan;
              CC2500_rssiMonBins[binIdx + 1] ++;
          }
	  }

	  PLTFRM_TOGGLE_LED_1();
	  PLTFRM_TOGGLE_LED_2();

      {
         UINT16_t dy;
	     for (dy=0; dy<reqParams_p->interval; dy++)
	          PLTFRM_delay1MilliSec();
	  }
  }

  if (reqParams_p->sampleCnt > 0)
  {
      UINT16_t idy;
	  SINT32_t avgRSSI;

	  avgRSSI = totalRSSI;
	  avgRSSI /= reqParams_p->sampleCnt;
      reqParams_p->avgMeasRSSI = avgRSSI;

      // sort the bins

      for (idx=0; idx<binsInUse; idx++)
      {
          for (idy=idx+1; idy<binsInUse; idy++)
          {
               if (CC2500_rssiMonBins[idy] > CC2500_rssiMonBins[idx])
               {
                   SINT16_t tmp = CC2500_rssiMonBins[idy];
                   CC2500_rssiMonBins[idy] = CC2500_rssiMonBins[idx];
                   CC2500_rssiMonBins[idx] = tmp;
                   tmp = CC2500_rssiMonBinIdx[idy];
                   CC2500_rssiMonBinIdx[idy] = CC2500_rssiMonBinIdx[idx];
                   CC2500_rssiMonBinIdx[idx] = tmp;
               }
          }
      }


      for (idx=0; idx<binsInUse; idx++)
      {
           UINT32_t perc;

           if (CC2500_rssiMonBins[idx] == 0)
               break;

           perc = CC2500_rssiMonBins[idx];
           perc *= 100;
           perc /= reqParams_p->sampleCnt;

           CC2500_rssiMonBins[idx] = perc;

           if (CC2500_rssiMonBinIdx[idx] == 0)
               CC2500_rssiMonBinIdx[idx] = reqParams_p->minMonRSSI - 1;
           else
           {
               if (CC2500_rssiMonBinIdx[idx] == binsInUse - 1)
                   CC2500_rssiMonBinIdx[idx] = reqParams_p->maxMonRSSI + 1;
               else
               {
                   SINT16_t tmp = CC2500_rssiMonBinIdx[idx];
                   tmp *= binSpan;
                   tmp += reqParams_p->minMonRSSI;
                   CC2500_rssiMonBinIdx[idx] = tmp;
               }
           }
      }

      reqParams_p->binSpan = binSpan;
      reqParams_p->_rsltArray[0].percentage = CC2500_rssiMonBins[0];
      reqParams_p->_rsltArray[0].start = CC2500_rssiMonBinIdx[0];
      reqParams_p->_rsltArray[1].percentage = CC2500_rssiMonBins[1];
      reqParams_p->_rsltArray[1].start = CC2500_rssiMonBinIdx[1];
      reqParams_p->_rsltArray[2].percentage = CC2500_rssiMonBins[2];
      reqParams_p->_rsltArray[2].start = CC2500_rssiMonBinIdx[2];
  }

  PLTFRM_OFF_LED_1();
  PLTFRM_OFF_LED_2();

  {
     // restore mdmcfg2
     sts = CC2500_writeReg(CC2500_MDMCFG2_REG_ADDR, mdmCfg2RegVal);
     if (sts != PLTFRM_STS_SUCCESS)
         SYS_fatal(SYS_FATAL_ERR_846);
  }

  if (CC2500_forceIdle() != PLTFRM_STS_SUCCESS)
      SYS_fatal(SYS_FATAL_ERR_856);

  return;
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
PLTFRM_sts_t CC2500_gpioInit(void)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
 
   /*
    * There are two GPIO pins available.
    * > GDO2  (Connected to micro P2_7)
    *   Digital output.
    *   Packet with good CRC received
    *
    * > GDO0  (Connected to micro P2_6)
    *   Digital output.  
    *   PA_PD
    */

   // Globally interrupts are still disabled ...
#ifdef GPIO_MGMT_ENA
   sts = PLTFRM_reserveGPIOPin(PLTFRM_CC2500_1_DEV_ID,
                               PLTFRM_GPIO_PORT_2,
                               PLTFRM_GPIO_PORT_PIN_7);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_177);
#endif

   // P2.7 <- GDO2
   P2DIR &= ~(BIT7);  // Input direction
   P2SEL  &= ~(BIT7); // I/O functionality
   // P2SEL2 &= ~(BIT7);
   P2REN &= ~(BIT7);  // Pull up resistor disabled
   P2IFG &= ~(BIT7);  // clear interrupt flag
   // P2IE |= (BIT7);   // Interrupt enabled
   P2IES |= (BIT7);  // Active low

#ifdef GPIO_MGMT_ENA
   sts = PLTFRM_reserveGPIOPin(PLTFRM_CC2500_1_DEV_ID,
                               PLTFRM_GPIO_PORT_2,
                               PLTFRM_GPIO_PORT_PIN_6);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_178);
#endif

   // P2.6 <- GDO0
   P2DIR &= ~(BIT6);  // Input direction
   P2SEL  &= ~(BIT6); // I/O functionality
   // P2SEL2 &= ~(BIT6);
   P2REN &= ~(BIT6);  // Pull up resistor disabled
   P2IFG &= ~(BIT6);  // clear interrupt flag
   // P2IE |= (BIT6);   // Interrupt enabled
   P2IES &= ~(BIT6);  // Active high

   return sts;
}

// #define CC2500_REG_READ_TEST
#ifdef CC2500_REG_READ_TEST
const UINT8_t CC2500_regTableVal[CC2500_RCCTRL0_STATUS_REG_ADDR - CC2500_IOCFG2_REG_ADDR + 1];
#endif
    
UINT16_t wIdx;

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t CC2500_waitChipRdy(void)
{
   // UINT16_t wIdx;

   for (wIdx=0; wIdx<500; wIdx++)
   {
       if (!(P3IN & BIT2))
           break;
       PLTFRM_delay100MicroSecs();
   }
      
   return wIdx >= 500 ? PLTFRM_STS_DEV_NOT_READY : PLTFRM_STS_SUCCESS;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t CC2500_swRst(void)
{
   /*
    * The other global reset possibility on CC2500 uses the SRES command strobe.
    * By issuing this strobe, all internal registers and states are set to the 
    * default, IDLE state. The manual power-up sequence is as follows -
    * 
    */

    // Set SCLK = 1 and SI = 0, to avoid potential problems with pin control mode
    // (see Section 11.3).
    P3OUT |= BIT3;
    P3OUT &= ~BIT1;

    P3OUT |= BIT0;   // CSn -> P3.0
    PLTFRM_delay10MicroSecs();

    // Strobe CSn low / high.
    // Hold CSn low and then high for at least 40 microsecs relative to pulling CSn low
    P3OUT &= ~BIT0;  // CSn -> P3.0
    PLTFRM_delay10MicroSecs();
    P3OUT |= BIT0;   // CSn -> P3.0
    PLTFRM_delay100MicroSecs();
    
    // Pull CSn low and wait for SO to go low (CHIP_RDYn).
    P3OUT &= ~BIT0;

    if (CC2500_waitChipRdy() != PLTFRM_STS_SUCCESS)
        return 1;

    CC2500_spiTxRx(CC2500_CMD_STROBE_SRES);

    PLTFRM_delay100MicroSecs();

#if 0
    // Issue the SRES strobe on the SI line.
    if (CC2500_sendCmdStrobe(CC2500_CMD_STROBE_SRES, NULL) != PLTFRM_STS_SUCCESS)
    {
        return 2;
    }
#endif

    // When SO goes low again, reset is complete and the chip is in the IDLE state.
    if (CC2500_waitChipRdy() != PLTFRM_STS_SUCCESS)
    {
        return 3;
    }

    // Bring CSn high
    P3OUT |= BIT0;

    return 0;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t CC2500_exitLPM(void)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   /*
    * Setting CSn low again will turn on the regulator and crystal
    * oscillator and make the chip enter the IDLE state. The SO pin 
    * on the SPI interface must be pulled low before the SPI interface 
    * is ready to be used as described in Section 10.1.
    */

   P3OUT &= ~BIT0;   // CSn -> P3.0

   sts = CC2500_waitChipRdy();
   if (sts == PLTFRM_STS_SUCCESS)
   {
       CC2500_state_t state;

       sts = CC2500_getCurrState(&state);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           if (state == CC2500_STATE_IDLE)
           {
               P2REN &= ~(BIT6);  // pull up disabled

               // Enable GPIO interrupts after clearing the interrupt flags
               P2IFG &= ~(BIT6 | BIT7);

               P2IE |= (BIT6 | BIT7);

               // Flush the FIFOs
               if (CC2500_flushTxFIFO() != PLTFRM_STS_SUCCESS)
                   SYS_fatal(SYS_FATAL_ERR_182);

               if (CC2500_flushRxFIFO() != PLTFRM_STS_SUCCESS)
                   SYS_fatal(SYS_FATAL_ERR_183);

               if (CC2500_cfgRegOnWakeUp(CC2500_TEST2_REG_ADDR) != PLTFRM_STS_SUCCESS)
                   SYS_fatal(SYS_FATAL_ERR_184);

               if (CC2500_cfgRegOnWakeUp(CC2500_TEST1_REG_ADDR) != PLTFRM_STS_SUCCESS)
                   SYS_fatal(SYS_FATAL_ERR_185);

               if (CC2500_cfgRegOnWakeUp(CC2500_TEST0_REG_ADDR) != PLTFRM_STS_SUCCESS)
                   SYS_fatal(SYS_FATAL_ERR_186);

               CC2500_cntxt.inLPM = 0;
               CC2500_cntxt.exitedLPMCnt ++;
           } 
           else
           {
               sts = PLTFRM_STS_DEV_INV_OPN_MODE;
           }
       }
   }    
  
   return sts; 
} 


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t CC2500_enterLPM(void)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   CC2500_state_t state;

   /*
    * In low power applications, the SLEEP state with the crystal oscillator 
    * core switched off should be used when the CC2500 is not active.
    * 
    * When CSn is released ater a SPWD command strobe has been sent on the 
    * SPI interface, the CC2500 enters the SLEEP state which is the state
    * with the lowest power consumption.
    *
    * > The voltage regulator (to the digital core) is disabled in the SLEEP
    *   state. 
    * > Most register values are retained. Current consumption is typically 
    *   200 nA.
    * > If MCSM0.XOSC_FORCE_ON is not set, the crystal is automatically
    *   controlled. In this mode, the crystal oscillator (XOSC) will be
    *   turned off if the SXOFF or SPWD command strobes are issued; the state 
    *   machine then goes to XOFF or SLEEP respectively. This can only be done 
    *   from the IDLE state. The XOSC will be turned off when CSn is released
    *   (goes high). 
    * > If MCSM0.XOSC_FORCE_ON is set (to 1), the crystal will always stay on
    *   even in the SLEEP state.
    * > Crystal oscillator start-up time depends on crystal ESR and load 
    *   capacitances. The electrical specification for the crystal oscillator
    *   can be found in Section 4.4.
    * > The content in the TESTx registers is not retained in SLEEP state, thus 
    *   it is necessary to re-write these registers when returning from the SLEEP 
    *   state. 
    * > Note that the content of the PATABLE is lost when entering the SLEEP state, 
    *   except for the first byte (index 0). Note that if ramping is not enabled, 
    *   only the first entry is valid.
    */

   // Disable the CC2500 interrupts

   if (CC2500_getCurrState(&state) == PLTFRM_STS_SUCCESS)
   {
       if (state != CC2500_STATE_IDLE)
       {
           if (CC2500_forceIdle() != PLTFRM_STS_SUCCESS)
               SYS_fatal(SYS_FATAL_ERR_187);
       }
   }
   else
       SYS_fatal(SYS_FATAL_ERR_188);

   P2IE &= ~(BIT6 | BIT7);

   P2REN |= (BIT6);  // pull up enabled
   P2OUT |= BIT6;  // Pulled high

   sts = CC2500_sendCmdStrobe(CC2500_CMD_STROBE_SPWD, NULL);
   
   if (sts == PLTFRM_STS_SUCCESS)
   {
       CC2500_cntxt.enteredLPMCnt ++;
       CC2500_cntxt.inLPM = 1;
   }
   return sts;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void CC2500_insertTurnAroundDelay(void)
{
   // Give enough time for the transmitter to go from TX->IDLE->RX.
   // TX->IDLE with no cal takes .25/baud_rate
   // IDLE->RX with cal takes 1953/Xosc +  FS cal time (under 1 millisec)
   // At 1200 baud, total time will be 1.3 millisecs
   // At 10000 baud, total time will be 1.1 millisecs
   // FS cal time is around
#if defined(RADIO_BAUD_RATE_1200)  \
    || defined(RADIO_BAUD_RATE_10000)  \
    || defined(RADIO_BAUD_RATE_38383) \
    || defined(RADIO_BAUD_RATE_250000)
   {
      UINT16_t _idx;
      for (_idx=0; _idx<CC2500_TX_TO_RX_TURN_AROUND_DELAY_MILLI_SECS; _idx++)
           PLTFRM_delay1MilliSec();
   }
#else
#error radio baud rate not specified !!
#endif
}


void __delay100MilliSecs(void)
{
   UINT16_t idx;
   for (idx=0; idx<100; idx++)
        PLTFRM_delay1MilliSec();
}

#ifdef CC2500_LOAD_SWITCH_ENA
void CC2500_switchOn(void)
{
   PLTFRM_sts_t sts;

   sts = PLTFRM_allocGPIOPin(PLTFRM_CC2500_1_DEV_ID,
                             PLTFRM_GPIO_PORT_4,
                             PLTFRM_GPIO_PORT_PIN_1,
                             PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                             PLTFRM_GPIO_OUTPUT_LOW,
                             PLTFRM_GPIO_INT_EDGE_NA,
                             NULL);
   if (sts != PLTFRM_STS_SUCCESS)
	   SYS_fatal(SYS_FATAL_ERR_606);

   // Power off the CC2500
   sts = PLTFRM_setGPIOPinOutput(PLTFRM_GPIO_PORT_4, PLTFRM_GPIO_PORT_PIN_1, 0);
   if (sts != PLTFRM_STS_SUCCESS)
	   SYS_fatal(SYS_FATAL_ERR_607);

   __delay100MilliSecs();

   // Power on the CC2500
   sts = PLTFRM_setGPIOPinOutput(PLTFRM_GPIO_PORT_4, PLTFRM_GPIO_PORT_PIN_1, 1);
   if (sts != PLTFRM_STS_SUCCESS)
	   SYS_fatal(SYS_FATAL_ERR_608);

   __delay100MilliSecs();
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
void CC2500_cfgCWMode(void)
{
   /*
    * Put radio in RANDOM umodulated or modulated continuous transmission mode.
    * Random TX mode; sends random data using PN9 generator. Used for test.
    * Works as normal mode, setting 0 (00), in RX.
    *
    * Unmodulated random mode:
    * > PKTCTRL0 - 0x22
    *   0x2 - RANDOM TX mode
    *   0x2 - Infinite packet length mode
    *
    * > MDMCFG2: 0x30
    *   Mod_format (0x3) - ASK/OOK
    *   Manchester_enc (0x0) - Disable
    *   SYNC_MODE (0x0) - No preamble/Sync
    *
    *
    * > IOCFG2: 0xb
    *   Serial Clock. Synchronous to the data in synchronous serial mode. In
    *   RX mode, data is set up on the falling edge by CC2500 when GDOx_INV=0.
    *   In TX mode, data is sampled by CC2500 on the rising edge of the serial
    *   clock when GDOx_INV=0.
    *
    * > IOCFG0: 0xc
    *   Serial Synchronous Data Output. Used for synchronous serial mode.
    *
    * > PKTLEN: 0xff
    *   Modulated mode:
    *
    */

}

#if defined(CC2500_CW_MOD_CFG_ENA) || defined(CC2500_CW_UNMOD_CFG_ENA)
CC2500_state_t state_0, state_1;
#endif

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t CC2500_init(void)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t regVal;
   UINT16_t idx;

   CC2500_cntxt.inLPM = 0;
#ifdef CC2500_WOR_ENA
   CC2500_cntxt.inWOR = 0;
#endif

   CC2500_gpioInit();

   /*
    * 19.1.1 Automatic POR 
    * A power-on reset circuit is included in the CC2500. The minimum requirements
    * stated in Table 18 must be followed for the power-on reset to function properly. 
    * The internal powerup sequence is completed when CHIP_RDYn goes low. CHIP_RDYn is 
    * observed on the SO pin after CSn is pulled low. See Section 10.1 for more details 
    * on CHIP_RDYn.
    *
    * When the CC2500 reset is completed, the chip will be in the IDLE state and the
    * crystal oscillator will be running. If the chip has had sufficient time for the 
    * crystal oscillator to stabilize after the power-on-reset, the SO pin will go low 
    * immediately after taking CSn low. If CSn is taken low before reset is completed,
    * the SO pin will first go high, indicating that the crystal oscillator is not 
    * stabilized, before going low as shown in Figure 26.
    */
   
   {
       UINT16_t rc;

       // Retry CC2500 reset multiple times ...

       for (idx=0; idx<10; idx++)
       {
           UINT16_t j;

           if ((rc = CC2500_swRst()) == 0x0)
               break;

           for (j=0; j<10; j++)
                PLTFRM_delay1MilliSec();
       }

       if (idx >= 10)
           SYS_fatal(SYS_FATAL_ERR_179 + rc - 1);
   }

#if 0
   P3OUT |= BIT0;

   {
      UINT16_t idx;
      for (idx=0; idx<10; idx++)
          PLTFRM_delay1MilliSec();
   }
  
   // Strobe CSn 
   P3OUT &= ~BIT0;
  
   // Loop while the SO pin is high
   while (P3IN & BIT2);
     
   // de-assert CSn
   P3OUT |= BIT0;
#endif

   PLTFRM_delay1MilliSec();

   do
   {
      CC2500_state_t state;

      if (CC2500_getCurrState(&state) == PLTFRM_STS_SUCCESS)
      {
          if (state == CC2500_STATE_IDLE)
          {
              // Read the part number reg
              sts = CC2500_readReg(CC2500_PARTNUM_REG_ADDR, &regVal);
              if (sts == PLTFRM_STS_SUCCESS)
              {
                   if (regVal == CC2500_PART_NUMBER)
                   {
                       sts = CC2500_readReg(CC2500_VERSION_REG_ADDR, &regVal);
                       if (sts == PLTFRM_STS_SUCCESS)
                       {
                           if (regVal == CC2500_VERSION_NUMBER)
                               break;
                       }
                   }
              }
          }
          else
          {
        	  CC2500_forceIdle();
          }
      }
   
      {
         UINT16_t idx;
         for (idx=0; idx<1; idx++)
             PLTFRM_delay1MilliSec();
      }
   } while (1);


#if defined(CC2500_CW_MOD_CFG_ENA) || defined(CC2500_CW_UNMOD_CFG_ENA)
   // rkris@wisense.in / june/9/15 - support for antenna performance testing
   // Unmodulated CW is a pure sine wave.


   sts = CC2500_setTxPower(10, 80);  // +10.8 dBm
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_867);

   sts = CC2500_getCurrState(&state_0);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_868);

   sts = CC2500_cfgChan(PHY_2400_MHZ_BAND_CHAN_1);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_869);

   sts = CC2500_cfgChipRegs(CC2500_contWaveRandRegList);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_866);

   sts = CC2500_sendCmdStrobe(CC2500_CMD_STROBE_STX, NULL);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_870);

   sts = CC2500_getCurrState(&state_1);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_871);

   PLTFRM_OFF_LED_1();
   PLTFRM_OFF_LED_2();

   for (;;)
   {
       UINT16_t idx;

       PLTFRM_TOGGLE_LED_1();
       PLTFRM_TOGGLE_LED_2();

       for (idx=0; idx<500; idx++)
         PLTFRM_delay1MilliSec();

       sts = CC2500_getCurrState(&state_1);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_870);
   }
#endif


   // Configure IOCFG0 - set to 0x1b (PA_PD)
   // This will get asserted 782/fRef us after the STX strobe has been issued
   // (fRef is the reference frequency for the synthesizer in MHz)
   // Configure the interrupt as active low
   regVal = 0x1b | CC2500_IOCFGX_IRQ_INVERT_BIT;
   sts =  CC2500_writeReg(CC2500_IOCFG0_REG_ADDR, regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_190);
   regVal = 0x0;
   sts = CC2500_readReg(CC2500_IOCFG0_REG_ADDR, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_191);
   if (regVal != (0x1b | CC2500_IOCFGX_IRQ_INVERT_BIT))
       SYS_fatal(SYS_FATAL_ERR_192);

   // Configure IOCFG2 - set to 0x7
   // Asserts when a packet has been received with CRC OK. De-asserts when the
   // first byte is read from the RX FIFO.
   // Configure the interrupt as active low
   regVal = 0x7 | CC2500_IOCFGX_IRQ_INVERT_BIT;
   sts =  CC2500_writeReg(CC2500_IOCFG2_REG_ADDR, regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_193);
   regVal = 0x0;
   sts = CC2500_readReg(CC2500_IOCFG2_REG_ADDR, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_194);
   if (regVal != (0x7 | CC2500_IOCFGX_IRQ_INVERT_BIT))
       SYS_fatal(SYS_FATAL_ERR_195);

   // Configure all the registers other than the center frequency and output
   // power level.
   sts = CC2500_cfgChipRegs(CC2500_chipRegInfoList);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_196);

#ifdef CC2500_REG_READ_TEST
   {
      UINT8_t regAddr;
      for (regAddr=CC2500_IOCFG2_REG_ADDR;
           regAddr<= CC2500_RCCTRL0_STATUS_REG_ADDR;
           regAddr++)
      {
           sts = CC2500_readReg(regAddr, &regVal);
           if (sts == PLTFRM_STS_SUCCESS)
               CC2500_regTableVal[regAddr - CC2500_IOCFG2_REG_ADDR] = regVal;
           else
               SYS_fatal(SYS_FATAL_ERR_189);
           PLTFRM_delay1MilliSec();
      }
   }
#endif

#if 0
   sts = CC2500_cfgPATable();
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_197);

   sts = CC2500_verifyPATable();
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_198);
#endif

   // Clear the interrupt flags (they seem to be getting set after being cleared in
   // CC2500_gpioInit()

   P2IFG &= ~(BIT6 | BIT7);

   // Global interrupts are still disabled.
   P2IE |= (BIT6 | BIT7);

   return sts;
}
#endif
