/*
 * File Name: cc1101.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Sep/14/2014
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

#ifdef RADIO_CC1101

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
 * Setting #1 (based on 1.2 kBaud, 5.2 kHz dev, Rx BW 58 kHz, GFSK,
 *             optimized for sensitivity
 * ---------------------------------------------------------------
 * Values from smartRF studio 7
 * Settings optimized for sensitivity
 * OSC freq: 26 MHz
 * Band: 865 - 867 MHz
 * Base frequency: 865.199829 MHz
 * Channels: 9 - First: 865.199829 MHz, last: 866.799437 MHz
 * Data rate: 1.19948 KBaud
 * Deviation: 5.157471 KHz
 * RX filter BW: 58.035714 KHz
 * Channel spacing: 199.951172 KHz
 * Mod format: GFSK
 * Whitening: No 
 * Manchester enable: No
 * PA ramping: No
 * Tx power: 10.8 dBm
 *
 * ---------------------------------------------------------------
 * Setting #2 (based on 10 kBaud, 19 kHz dev, Rx BW 100 kHz, GFSK,
 *             optimized for sensitivity
 * ---------------------------------------------------------------
 * Values from smartRF studio 7
 * Settings optimized for sensitivity
 * OSC freq: 26 MHz
 * Band: 865 - 867 MHz
 * Base frequency: 865.199829 MHz
 * Channels: 9 - First: 865.199829 MHz, last: 866.799437 MHz
 * Data rate: 9.9926 KBaud
 * Deviation: 19.042969 KHz
 * RX filter BW: 101.562500 KHz
 * Channel spacing: 199.951172 KHz
 * Mod format: GFSK
 * Whitening: No 
 * Manchester enable: No
 * PA ramping: No
 * Tx power: 10.8 dBm
 *
 * -----------------------------------------------------------------
 * Setting #3 (based on 38.4 kBaud, 20 kHz dev, Rx BW 100 kHz, GFSK,
 *             optimized for sensitivity)
 * -----------------------------------------------------------------
 * Values from smartRF studio 7
 * Settings optimized for sensitivity
 * OSC freq: 26 MHz
 * Band: 865 - 867 MHz
 * Base frequency: 865.199829 MHz
 * Channels: 9 - First: 865.199829 MHz, last: 866.799437 MHz
 * Data rate: 38383.5 Baud
 * Deviation: 20.629883 KHz
 * RX filter BW: 101.562500 KHz
 * Channel spacing: 199.951172 KHz
 * Mod format: GFSK
 * Whitening: Nos
 * Tx power: 10.8 dBm
 */


#ifdef RADIO_FREQ_BAND_868_MHZ

#ifdef CUSTOMER_ANDY_SPECIFIC_COORD_BUILD
UINT32_t CC1101_baseFreqHz = 867999938;
UINT32_t CC1101_channelSpacingHz = 199951;
const CC1101_chipRegInfo_s CC1101_chipRegInfoList[  ] =
{
   // {CC1101_FIFOTHR_REG_ADDR, 0x47},
   // {CC1101_SYNC1_REG_ADDR, 0xd3},
   // {CC1101_SYNC0_REG_ADDR, 0x91},
   {CC1101_PKTLEN_REG_ADDR, 0x61},

   // Two status bytes will be appended to the payload of the packet
   // Address check, no broadcast
   {CC1101_PKTCTRL1_REG_ADDR, 0x05},

   // WHITE_DATA[6] - Whitening Off (0)
   // PKT_FORMAT[4:5] - Normal mode, use FIFOs for RX and TX (00)
   // CRC_EN[2] - CRC calculation in TX and CRC check in RX enabled (1)
   // LENGTH_CONFIG[0:1] - Variable packet length mode (01)
   {CC1101_PKTCTRL0_REG_ADDR, 0x05},


   // Local address is 0x3
   {CC1101_ADDR_REG_ADDR, 0x03},

   // The 8-bit unsigned channel number, which is multiplied by the channel
   // spacing setting and added to the base frequency.
   {CC1101_CHANNR_REG_ADDR, 0x00},

   // The desired IF frequency to employ in RX. Subtracted from FS base
   // frequency in RX and controls the digital complex mixer in the demodulator.
   // F(if) =  (Fosc) * FSCTRL1.FREQ_IF / (2 ^ 10)
   // F(if) = ((26 * (10 ^ 6)) * 11) / (2 ^ 10) = 279.296875 KHz
   {CC1101_FSCTRL1_REG_ADDR, 0x0b},

   // Frequency offset added to the base frequency before being used by the
   // frequency synthesizer. (2s-complement).
   {CC1101_FSCTRL0_REG_ADDR, 0x00},

   // Carrier frequency
   // Value set to 0x216276 which gives a carrier frequency of 867999938.9648438 HZ
   // assuming crystal freq is 26 MHz.
   {CC1101_FREQ2_REG_ADDR, 0x21},
   {CC1101_FREQ1_REG_ADDR, 0x62},
   {CC1101_FREQ0_REG_ADDR, 0x76},

   // RX filter bandwidth
   // CHANBW_E[1:0] = 3
   // CHANBW_M[1:0] = 0
   // CHAN_BW = (Fosc) / (8 * (4 + 0) * (2 ^ 3))
   // CHAN_BW = (26 * 10^6) / (8 * 4 * 8) = 101.562 KHz
   // Comes out to 101.562 kHz given crystal frequency of 26 MHz

   // Baud rate
   // DRATE_E[3:0] - 0xa
   {CC1101_MDMCFG4_REG_ADDR, 0xca},

   // DRATE_M[7:0] - 0x83
   // D_RATE = (256 + 0x83) * (2^(0xa)) * (Fosc) / (2^28)
   // D_RATE = 387 * (1024) * (26 * 10^6) / (2^28) =
   // Comes out to 38383.484 baud
   {CC1101_MDMCFG3_REG_ADDR, 0x83},

   // DEM_DCFILT_OFF - 0x0 (Disable digital DC blocking filter before demodulator)
   // 0 = Enable (better sensitivity)
   // MOD_FORMAT[2:0] - 1 (GFSK)
   // MANCHESTER_EN - Disabled (0)
   // SYNC_MODE[2:0] - 30/32 sync word bits detected (011)
   {CC1101_MDMCFG2_REG_ADDR, 0x13},

   // FEC_EN - FEC disabled (0)
   // NUM_PREAMBLE[2:0] - Pramble bytes count (4 bytes)
   // CHANSPC_E[1:0] - 2 (2 bit exponent of channel spacing)
   {CC1101_MDMCFG1_REG_ADDR, 0x22},

   // CHANSPC_M[7:0] - 2 (8 bit mantissa)
   // Channel spacing comes out to 199.951172 kHz
   {CC1101_MDMCFG0_REG_ADDR, 0xf8},

   // DEVIATN - Modem deviation setting
   // Specifies the nominal frequency deviation from the carrier for a
   // '0' (-DEVIATN) and '1' (+DEVIATN)
   // DEVIATION_E[2:0] - 3
   // DEVIATION_M[2:0] - 5
   {CC1101_DEVIATN_REG_ADDR, 0x35},

   // {CC1101_MCSM2_REG_ADDR, 0x07},

   // CCA_MODE[1:0] - 0x3 (If RSSI below threshold unless currently receiving a packet
   // RXOFF_MODE[1:0] - 0x3 (Stay in Rx after finishing packet reception)
   // TXOFF_MODE[1:0] - 0x3 (Go to RX after finishing packet transmission)
   {CC1101_MCSM1_REG_ADDR, 0x3f},

   // FS:AUTOCAL[5:4] - 0x1 (Automatically calibrate when going from IDLE to RX/TX)
   // PIN_CTRL_EN[1] - 0x0 (Pin radio control option disabled)
   // XOSC_FORCE_ON[0] - 0x0 (not forcing XOSC to stay on in sleep state)
   {CC1101_MCSM0_REG_ADDR, 0x18},

   {CC1101_FOCCFG_REG_ADDR, 0x1d},

   {CC1101_BSCFG_REG_ADDR, 0x1c},

   {CC1101_AGCTRL2_REG_ADDR, 0xc7},
   {CC1101_AGCTRL1_REG_ADDR, 0x00},
   {CC1101_AGCTRL0_REG_ADDR, 0xb2},

   //{CC1101_WOREVT1_REG_ADDR, 0x87},
   //{CC1101_WOREVT0_REG_ADDR, 0x6b},
   //{CC1101_WORCTRL_REG_ADDR, 0xfb},

   {CC1101_FREND1_REG_ADDR, 0xb6},

   // PA_POWER[0:2] - 0x0
   // The PATABLE settings from index ‘0’ to the PA_POWER value are used for
   // for power ramp-up/ramp-down at the start/end of transmission in all TX
   // modulation formats.
   // In our case, PA_POWER is set to 0x0 since ramping is not enabled.
   {CC1101_FREND0_REG_ADDR, 0x10},

   {CC1101_FSCAL3_REG_ADDR, 0xe9},
   {CC1101_FSCAL2_REG_ADDR, 0x2a},
   {CC1101_FSCAL1_REG_ADDR, 0x00},
   {CC1101_FSCAL0_REG_ADDR, 0x1f},

   // {CC1101_RCCTRL1_REG_ADDR, 0x41},
   // {CC1101_RCCTRL0_REG_ADDR, 0x00},

   {CC1101_TEST2_REG_ADDR, 0x81},
   {CC1101_TEST1_REG_ADDR, 0x35},
   {CC1101_TEST0_REG_ADDR, 0x09},

   {0xff,                  0xff}
};

#else

const CC1101_chipRegInfo_s CC1101_chipRegInfoList[  ] =
{
   {CC1101_FIFOTHR_REG_ADDR, 0x47},
   {CC1101_SYNC1_REG_ADDR, 0xd3},
   {CC1101_SYNC0_REG_ADDR, 0x91},

   // PACKET_LENGTH[0:7] - If variable packet length mode is used, this value
   // indicates the maximum packet length allowed.
   {CC1101_PKTLEN_REG_ADDR, CC1101_MAX_RX_PKT_LEN},   //

   // PQT[5:7] = 0 (sync word always accepted)
   // CRC_AUTOFLUSH[3] - Enable automatic flush of RX FIFO when CRC is not OK (1)
   // APPEND_STATUS[2] - Two status bytes appended to the pyld of the pkt (1)
   // ADR_CHK[0:1] - No address check (00)
   {CC1101_PKTCTRL1_REG_ADDR, 0x0c},

   // WHITE_DATA[6] - Whitening Off (0)
   // PKT_FORMAT[4:5] - Normal mode, use FIFOs for RX and TX (00)
   // CRC_EN[2] - CRC calculation in TX and CRC check in RX enabled (1)
   // LENGTH_CONFIG[0:1] - Variable packet length mode (01)
   {CC1101_PKTCTRL0_REG_ADDR, 0x05},


   // Not applicable in our case (no address check)
   {CC1101_ADDR_REG_ADDR, 0x00},

   // The 8-bit unsigned channel number, which is multiplied by the channel
   // spacing setting and added to the base frequency.
   {CC1101_CHANNR_REG_ADDR, 0x00},

   // The desired IF frequency to employ in RX. Subtracted from FS base
   // frequency in RX and controls the digital complex mixer in the demodulator.
   {CC1101_FSCTRL1_REG_ADDR, 0x06},

   // Frequency offset added to the base frequency before being used by the
   // frequency synthesizer. (2s-complement).
   {CC1101_FSCTRL0_REG_ADDR, 0x00},

   // Carrier frequency
   // Value set to 0x216276 which gives a carrier frequency of 867999938.9648438 HZ
   // assuming crystal freq is 26 MHz.
   {CC1101_FREQ2_REG_ADDR, 0x21},
   {CC1101_FREQ1_REG_ADDR, 0x62},
   {CC1101_FREQ0_REG_ADDR, 0x76},

   // RX filter bandwidth
   // CHANBW_E[1:0]
   // CHANBW_M[1:0]
   // Comes out to 58.035714 kHz given crystal frequency of 26 MHz

   // Baud rate
   // DRATE_E[3:0] - 0x08
   {CC1101_MDMCFG4_REG_ADDR, 0xd5},
   // DRATE_M[7:0] - 0x93
   // Comes out to 9992.599 baud
   {CC1101_MDMCFG3_REG_ADDR, 0x83},

   // DEM_DCFILT_OFF - 0x0 (Disable digital DC blocking filter before demodulator)
   // 0 = Enable (better sensitivity)
   // MOD_FORMAT[2:0] - GFSK
   // MANCHESTER_EN - Disabled (0)
   // SYNC_MODE[2:0] - 30/32 sync word bits detected (011)
   {CC1101_MDMCFG2_REG_ADDR, 0x13},

   // FEC_EN - FEC disabled (0)
   // NUM_PREAMBLE[2:0] - Pramble bytes count (4 bytes)
   // CHANSPC_E[1:0] - 2 (2 bit exponent of channel spacing)
   {CC1101_MDMCFG1_REG_ADDR, 0x22},
   // CHANSPC_M[7:0] - 2 (8 bit mantissa)
   // Channel spacing comes out to 199.951172 kHz
   {CC1101_MDMCFG0_REG_ADDR, 0xf8},

   // DEVIATN - Modem deviation setting
   // Specifies the nominal frequency deviation from the carrier for a
   // ‘0’ (-DEVIATN) and ‘1’ (+DEVIATN)
   // DEVIATION_E[2:0]
   // DEVIATION_M[2:0]
   // 5.157471 KHz
   {CC1101_DEVIATN_REG_ADDR, 0x15},

   {CC1101_MCSM2_REG_ADDR, 0x07},

   // CCA_MODE[1:0] - 0x3 (If RSSI below threshold unless currently receiving a packet
   // RXOFF_MODE[1:0] - 0x3 (Stay in Rx after finishing packet reception)
   // TXOFF_MODE[1:0] - 0x0 (Go to IDLE after finishing packet transmission)
   {CC1101_MCSM1_REG_ADDR, 0x3c},

   // FS:AUTOCAL[5:4] - 0x1 (Automatically calibrate when going from IDLE to RX/TX)
   // PIN_CTRL_EN[1] - 0x0 (Pin radio control option disabled)
   // XOSC_FORCE_ON[0] - 0x0 (not forcing XOSC to stay on in sleep state)
   {CC1101_MCSM0_REG_ADDR, 0x18},

   {CC1101_FOCCFG_REG_ADDR, 0x16},

   {CC1101_BSCFG_REG_ADDR, 0x6c},

   {CC1101_AGCTRL2_REG_ADDR, 0x03},
   {CC1101_AGCTRL1_REG_ADDR, 0x40},
   {CC1101_AGCTRL0_REG_ADDR, 0x91},

   {CC1101_WOREVT1_REG_ADDR, 0x87},
   {CC1101_WOREVT0_REG_ADDR, 0x6b},
   {CC1101_WORCTRL_REG_ADDR, 0xfb},

   {CC1101_FREND1_REG_ADDR, 0x56},

   // PA_POWER[0:2] - 0x0
   // The PATABLE settings from index ‘0’ to the PA_POWER value are used for
   // for power ramp-up/ramp-down at the start/end of transmission in all TX
   // modulation formats.
   // In our case, PA_POWER is set to 0x0 since ramping is not enabled.
   {CC1101_FREND0_REG_ADDR, 0x10},

   {CC1101_FSCAL3_REG_ADDR, 0xe9},
   {CC1101_FSCAL2_REG_ADDR, 0x2a},
   {CC1101_FSCAL1_REG_ADDR, 0x00},
   {CC1101_FSCAL0_REG_ADDR, 0x1f},

   {CC1101_RCCTRL1_REG_ADDR, 0x41},
   {CC1101_RCCTRL0_REG_ADDR, 0x00},

   {CC1101_TEST2_REG_ADDR, 0x81},
   {CC1101_TEST1_REG_ADDR, 0x35},
   {CC1101_TEST0_REG_ADDR, 0x09},

   {0xff,                  0xff}
};
#endif
#elif defined(RADIO_FREQ_BAND_865_867_MHZ)

#if defined(CC1101_CW_MOD_CFG_ENA) || defined(CC1101_CW_UNMOD_CFG_ENA)

const CC1101_chipRegInfo_s CC1101_contWaveRandRegList[ ] =
{
    {CC1101_IOCFG2_REG_ADDR, 0x0B},  // IOCFG2        GDO2 Output Pin Configuration
    {CC1101_IOCFG0_REG_ADDR, 0x0C},  // IOCFG0        GDO0 Output Pin Configuration
    {CC1101_FIFOTHR_REG_ADDR, 0x47},  // FIFOTHR       RX FIFO and TX FIFO Thresholds
    {CC1101_FSCTRL1_REG_ADDR, 0x06},  // FSCTRL1       Frequency Synthesizer Control
    {CC1101_FREQ2_REG_ADDR, 0x21},  // FREQ2         Frequency Control Word, High Byte
    {CC1101_FREQ1_REG_ADDR, 0x46},  // FREQ1         Frequency Control Word, Middle Byte
    {CC1101_FREQ0_REG_ADDR, 0xE4},  // FREQ0         Frequency Control Word, Low Byte
    {CC1101_MDMCFG4_REG_ADDR, 0xC8},  // MDMCFG4       Modem Configuration
    {CC1101_MDMCFG3_REG_ADDR, 0x93},  // MDMCFG3       Modem Configuration
    {CC1101_DEVIATN_REG_ADDR, 0x34},  // DEVIATN       Modem Deviation Setting
    {CC1101_MCSM0_REG_ADDR, 0x18},  // MCSM0         Main Radio Control State Machine Configuration
    {CC1101_FOCCFG_REG_ADDR, 0x16},  // FOCCFG        Frequency Offset Compensation Configuration
    {CC1101_AGCTRL2_REG_ADDR, 0x43},  // AGCCTRL2      AGC Control
    {CC1101_WORCTRL_REG_ADDR, 0xFB},  // WORCTRL       Wake On Radio Control
    {CC1101_FREND0_REG_ADDR, 0x11},  // FREND0        Front End TX Configuration
    {CC1101_FSCAL3_REG_ADDR, 0xE9},  // FSCAL3        Frequency Synthesizer Calibration
    {CC1101_FSCAL2_REG_ADDR, 0x2A},  // FSCAL2        Frequency Synthesizer Calibration
    {CC1101_FSCAL1_REG_ADDR, 0x00},  // FSCAL1        Frequency Synthesizer Calibration
    {CC1101_FSCAL0_REG_ADDR, 0x1F},  // FSCAL0        Frequency Synthesizer Calibration
    {CC1101_TEST2_REG_ADDR, 0x81},  // TEST2         Various Test Settings
    {CC1101_TEST1_REG_ADDR, 0x35},  // TEST1         Various Test Settings
    {CC1101_TEST0_REG_ADDR, 0x09},  // TEST0         Various Test Settings
#ifdef CC1101_CW_MOD_CFG_ENA
    // Modulated (GFSK)
    {CC1101_MDMCFG2_REG_ADDR, 0x10},  // MDMCFG2       Modem Configuration
#else
    // Unmodulated (pure sine wave)
    {CC1101_MDMCFG2_REG_ADDR, 0x30},  // MDMCFG2       Modem Configuration
#endif
    {CC1101_PKTCTRL0_REG_ADDR, 0x22},  // PKTCTRL0      Packet Automation Control
    {0xff, 0xff}
};
#endif

UINT32_t CC1101_baseFreqHz = 865199829;
UINT32_t CC1101_channelSpacingHz = 199951;
const CC1101_chipRegInfo_s CC1101_chipRegInfoList[  ] =
{
   {CC1101_FIFOTHR_REG_ADDR, 0x47}, 
   {CC1101_SYNC1_REG_ADDR, 0xd3},
   {CC1101_SYNC0_REG_ADDR, 0x91},

   // PACKET_LENGTH[0:7] - If variable packet length mode is used, this value
   // indicates the maximum packet length allowed.
   {CC1101_PKTLEN_REG_ADDR, CC1101_MAX_RX_PKT_LEN},


   // CRC_AUTOFLUSH[3] - Enable automatic flush of RX FIFO when CRC is not OK (1)
   // APPEND_STATUS[2] - Two status bytes appended to the pyld of the pkt (1)
   // ADR_CHK[0:1] - No address check (00)
#ifdef CC1101_WOR_ENA
   // PQT[5:7] = 2 - Threshold set to 2*4 = 8.
   {CC1101_PKTCTRL1_REG_ADDR, 0x4c},
#else
   // PQT[5:7] = 0 (sync word always accepted)
   {CC1101_PKTCTRL1_REG_ADDR, 0x0c},
#endif
   // WHITE_DATA[6] - Whitening Off (0)
   // PKT_FORMAT[4:5] - Normal mode, use FIFOs for RX and TX (00)
   // CRC_EN[2] - CRC calculation in TX and CRC check in RX enabled (1)
   // LENGTH_CONFIG[0:1] - Variable packet length mode (01)
   {CC1101_PKTCTRL0_REG_ADDR, 0x05},

   // Not applicable in our case (no address check)
   {CC1101_ADDR_REG_ADDR, 0x00},

   // The 8-bit unsigned channel number, which is multiplied by the channel
   // spacing setting and added to the base frequency.
   {CC1101_CHANNR_REG_ADDR, 0x00},

   // The desired IF frequency to employ in RX. Subtracted from FS base
   // frequency in RX and controls the digital complex mixer in the demodulator.
   {CC1101_FSCTRL1_REG_ADDR, 0x06},

   // Frequency offset added to the base frequency before being used by the
   // frequency synthesizer. (2s-complement).
   {CC1101_FSCTRL0_REG_ADDR, 0x00},

   // Base frequency
   // Value set to 0x2146e4 which gives a base frequency of 865.199829 MHz
   // assuming crystal freq is 26 MHz.
#if 0
   {CC1101_FREQ2_REG_ADDR, 0x21},
   {CC1101_FREQ1_REG_ADDR, 0x4e},
   {CC1101_FREQ0_REG_ADDR, 0xc4},
#else
   {CC1101_FREQ2_REG_ADDR, 0x21},
   {CC1101_FREQ1_REG_ADDR, 0x46},
   {CC1101_FREQ0_REG_ADDR, 0xe4},
#endif
   // RX filter bandwidth
   // CHANBW_E[1:0]
   // CHANBW_M[1:0]
   // Comes out to 58.035714 kHz given crystal frequency of 26 MHz

   // Baud rate
#ifdef RADIO_BAUD_RATE_1200
   // Comes out to 1.19948
   {CC1101_MDMCFG4_REG_ADDR, 0xf5},
   {CC1101_MDMCFG3_REG_ADDR, 0x83},
#elif defined(RADIO_BAUD_RATE_10000)
   //  9992.6 Baud
   {CC1101_MDMCFG4_REG_ADDR, 0xc8}, 
   {CC1101_MDMCFG3_REG_ADDR, 0x93},
#elif defined(RADIO_BAUD_RATE_38383)
   {CC1101_MDMCFG4_REG_ADDR, 0xca},
   {CC1101_MDMCFG3_REG_ADDR, 0x83},
#else
#error radio baud rate not specified !! 
#endif

   // DEM_DCFILT_OFF - 0x0 (Disable digital DC blocking filter before demodulator)
   // 0 = Enable (better sensitivity)
   // MOD_FORMAT[2:0] - GFSK
   // MANCHESTER_EN - Disabled (0)
   // SYNC_MODE[2:0] - 30/32 sync word bits detected (011)
#ifdef RADIO_MOD_FMT_GFSK
   {CC1101_MDMCFG2_REG_ADDR, 0x13},
#else
#error specified modulation format not specified !!
#endif

   // FEC_EN - FEC disabled (0)

   // CHANSPC_E[1:0] - 2 (2 bit exponent of channel spacing)
#ifdef CC1101_WOR_ENA
   // NUM_PREAMBLE[2:0] - Pramble bytes count (24 bytes)
   {CC1101_MDMCFG1_REG_ADDR, 0x72},
#else
   // NUM_PREAMBLE[2:0] - Pramble bytes count (4 bytes)
   {CC1101_MDMCFG1_REG_ADDR, 0x22},
#endif
   // CHANSPC_M[7:0] - 2 (8 bit mantissa)
   // Channel spacing comes out to 199.951172 kHz
   {CC1101_MDMCFG0_REG_ADDR, 0xf8},

   // DEVIATN - Modem deviation setting
   // Specifies the nominal frequency deviation from the carrier for a
   // ‘0’ (-DEVIATN) and ‘1’ (+DEVIATN)
   // DEVIATION_E[2:0]
   // DEVIATION_M[2:0]
   // 5.157471 KHz
#ifdef RADIO_BAUD_RATE_1200
   {CC1101_DEVIATN_REG_ADDR, 0x15},
#elif defined(RADIO_BAUD_RATE_10000)
   // Deviation: 19.042969 kHz
   {CC1101_DEVIATN_REG_ADDR, 0x34},
#elif defined(RADIO_BAUD_RATE_38383)
   {CC1101_DEVIATN_REG_ADDR, 0x35},
#else
#error radio baud rate not specified !! 
#endif

#ifndef CC1101_WOR_ENA
   {CC1101_MCSM2_REG_ADDR, 0x07},
#else
   /*
    * RC_PD[7]: 0
    * When written to 0, automatic initial calibration will be performed. The
    * RC_PD bit is the power down signal for the RC oscillator. This bit must
    * be cleared for the RC oscillator to run.
    *
    * RX_TIME_RSSI[4]: 1
    * When the RX_TIME_RSSI bit is set to one, the RSSI level is compared
    * to a programmable threshold a given time after RX mode has been entered.
    * If the RSSI level is below the programmed threshold at this time, the
    * radio will exit RX mode regardless of the RX timeout. If it is above the
    * same threshold (carrier sense is asserted), it will stay in RX until a
    * packet has been received or until it returns to SLEEP due to an RX
    * timeout. The time it takes from entering RX mode until the RSSI level is
    * valid is given by equation 5 in the TI app note "AN047".
    *
    * RX_TIME_QUAL[3]: 1
    * When RX_TIME_QUAL = 1, the radio will stay in RX when the RX_TIME timer
    * expires if a sync word is found or PQI is set (the preamble quality threshold
    * is reached).
    *
    * RX_TIME[2:0]: 0
    * The RX_TIME bits set the timeout for sync word search. When using WOR mode,
    * RX_TIME can have a value between 0 and 6 when WORCTRL.WOR_RES = 0, and between
    * 0 and 3 when WORCTRL.WOR_RES = 1. RX_TIME = 7 means that the radio will stay
    * in RX mode until a packet is received, regardless of the WORCTRL.WOR_RES setting.
    * The timeout is relative to tEvent0.
    *
    * The RX timeout in microsecs is given by EVENT0�C(RX_TIME, WOR_RES) �26/X
    * C(0, 0) is 3.6058 from the CC1101 datasheet and X is 26.
    * RX_timeout =  4506 * 3.6058 * 26/26 = 16.247 milli-secs
    *
    *     RX_timeout = Tevent0 * Duty_cycle
    *
    * Duty_cycle = RX_timeout / Tevent0 =  16.247 / 130 = 12.5 %
    */
   {CC1101_MCSM2_REG_ADDR, 0x18},
#endif

   // CCA_MODE[1:0] - 0x3 (If RSSI below threshold unless currently receiving a packet
   // RXOFF_MODE[1:0] - 0x3 (Stay in Rx after finishing packet reception)
   // TXOFF_MODE[1:0] - 0x0 (Go to IDLE after finishing packet transmission)
#ifdef DEV_TYPE_SNIFFER
   {CC1101_MCSM1_REG_ADDR, 0x3c},
#else
#ifdef CC1101_WOR_ENA
   {CC1101_MCSM1_REG_ADDR, 0x30},
#else
   {CC1101_MCSM1_REG_ADDR, 0x3c},
#endif
#endif


   // FS:AUTOCAL[5:4] - 0x1 (Automatically calibrate when going from IDLE to RX/TX)
   // PIN_CTRL_EN[1] - 0x0 (Pin radio control option disabled)
   // XOSC_FORCE_ON[0] - 0x0 (not forcing XOSC to stay on in sleep state)
   {CC1101_MCSM0_REG_ADDR, 0x18},

   {CC1101_FOCCFG_REG_ADDR, 0x16},

   {CC1101_BSCFG_REG_ADDR, 0x6c},

#ifdef RADIO_BAUD_RATE_1200
   {CC1101_AGCTRL2_REG_ADDR, 0x3},
#elif defined(RADIO_BAUD_RATE_10000)
   {CC1101_AGCTRL2_REG_ADDR, 0x43},
#elif defined(RADIO_BAUD_RATE_38383)
   {CC1101_AGCTRL2_REG_ADDR, 0x43},
#else
#error radio baud rate not specified !! 
#endif
#ifdef CC1101_SKIP_CCA
   {CC1101_AGCTRL1_REG_ADDR, 0x48},  // For testing throughput
#else
   {CC1101_AGCTRL1_REG_ADDR, 0x40},
#endif
   {CC1101_AGCTRL0_REG_ADDR, 0x91},


#ifdef CC1101_WOR_ENA

   // EVENT0 timeout : 0x119a (4506)
   {CC1101_WOREVT1_REG_ADDR, 0x11},
   {CC1101_WOREVT0_REG_ADDR, 0x9a},

   /*
    * EVENT1[6:4] -> 0x7
    * Number of clock periods after event 0 before event 1 times out.
    * RC oscillator clock frequency equals
    *    FXosc/750 = (26 * 10^6) / 750 = 34666.67 Hz
    * 1 clock cycle = 1 / 34666.67 = 2.89 * 10^-5
    * EVENT1 = 7 translates to 48 clock cycles (from table)
    *        = 48 * 2.89 * 10^-5 = 1.385 millisecs
    *
    * Measured value of Tevent1 = 1.4 millisecs
    * Measured value of IDLE->RX with Cal = 740 microsecs
    *
    * RC_CAL[3] -> 0x1
    * Enables the RC oscillator calibration.
    *
    * WOR_RES[1:0] -> 0x0  Resolution: 1 period (28 � 29 microseconds)
    *                     Max timeout: 1.8 � 1.9 seconds
    *
    * Controls the Event 0 resolution as well as maximum timeout of the WOR module.
    *
    *
    * WOREVT1 and WOREVT0 is 4506
    * WOR_RES is 0
    *
    * Tevent0 = (750 * 4506) / (26 * 10^6) = 129.98 millisecs
    */
   {CC1101_WORCTRL_REG_ADDR, 0x78},
#else
   {CC1101_WOREVT1_REG_ADDR, 0x87},
   {CC1101_WOREVT0_REG_ADDR, 0x6b},
   {CC1101_WORCTRL_REG_ADDR, 0xfb},
#endif

   {CC1101_FREND1_REG_ADDR, 0x56},

   // PA_POWER[0:2] - 0x0
   // The PATABLE settings from index ‘0’ to the PA_POWER value are used for
   // for power ramp-up/ramp-down at the start/end of transmission in all TX
   // modulation formats.
   // In our case, PA_POWER is set to 0x0 since ramping is not enabled.
   {CC1101_FREND0_REG_ADDR, 0x10},

   {CC1101_FSCAL3_REG_ADDR, 0xe9},
   {CC1101_FSCAL2_REG_ADDR, 0x2a},
   {CC1101_FSCAL1_REG_ADDR, 0x00},
   {CC1101_FSCAL0_REG_ADDR, 0x1f},

   // {CC1101_RCCTRL1_REG_ADDR, 0x41},
   // {CC1101_RCCTRL0_REG_ADDR, 0x00},

   {CC1101_TEST2_REG_ADDR, 0x81},
   {CC1101_TEST1_REG_ADDR, 0x35},
   {CC1101_TEST0_REG_ADDR, 0x09},

   {0xff,                  0xff}
};

#else
#error CC1101 carrier frequency not specified !!
#endif

#ifndef RADIO_MOD_FMT_OOK
/*
 * The special PATABLE register can hold up to eight user selected output 
 * power settings. All the PA power settings in the PATABLE from index 0 
 * up to the FREND0.PA_POWER value are used.
 */
UINT8_t CC1101_paTableCfgTbl[CC1101_PA_TABLE_LEN] =
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

const CC1101_modFormat_t CC1101_modFormat = CC1101_MOD_FORMAT_GSK; // MDMCFG2.MOD_FORMAT[2:0]
#ifdef CC1101_WOR_ENA
const UINT16_t CC1101_preambleLen = 24;
#else
const UINT16_t CC1101_preambleLen = 4;   // MDMCFG1.NUM_PREAMBLE[2:0]
#endif
const UINT16_t CC1101_syncLen = 4;   // MDMCFG2.SYNC_MODE[2:0]
const UINT32_t CC1101_baudRate = 1200;   // MDMCFG4.DRATE_E[3:0] and MDMCFG4.DRATE_M[7:0]
#ifdef RADIO_BAUD_RATE_1200
const UINT32_t CC1101_microSecsPerSym = 834;
const UINT16_t CC1101_txToIdleTransitionTimeMilli = 1;  // (834/4 -> 209 micros) <Calc .25/baud_rate>
#elif defined(RADIO_BAUD_RATE_10000)
// Programmed baud rate is 9992.6 baud
const UINT32_t CC1101_microSecsPerSym = 101;
const UINT16_t CC1101_txToIdleTransitionTimeMilli = 1;  // (100/4 -> 25 micros) <Calc .25/baud_rate>
#elif defined(RADIO_BAUD_RATE_38383)
const UINT32_t CC1101_microSecsPerSym = 26;
const UINT16_t CC1101_txToIdleTransitionTimeMilli = 1;  // (26/4 -> 6.5 micros) <Calc .25/baud_rate>
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


const CC1101_state_t CC1101_FSM_STATE_MAPPING[ ] =
{
   CC1101_STATE_IDLE,
   CC1101_STATE_RX,
   CC1101_STATE_TX_IN_PROGRESS,
   CC1101_STATE_FSTXON,
   CC1101_STATE_CALIBRATE,
   CC1101_STATE_SETTLING,
   CC1101_STATE_RXFIFO_OVERFLOW,
   CC1101_STATE_TXFIFO_UNDERFLOW,
};


CC1101_cntxt_s  CC1101_cntxt;



/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
#define CC1101_spiEnd( )  \
do \
{  \
   P4OUT |= (BIT0);  \
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
PLTFRM_sts_t CC1101_spiBegin(void)
{
  PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

  /*
   * When CSn is pulled low, the MCU must wait until CC1101 SO pin goes
   * low before starting to transfer the header byte. This indicates that
   * the crystal is running. Unless the chip was just reset or was in SLEEP
   * or XOFF state, the SO pin will always go low immediately after taking
   * CSn low.
   */

  P4OUT &= ~(BIT0);

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
#define CC1101_spiTxRxNoInt(wrVal, rdVal) \
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
UINT8_t CC1101_spiTxRx(UINT8_t val)
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
PLTFRM_sts_t CC1101_readReg(UINT8_t regAddr, UINT8_t *regVal_p)
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

   hdrByte = CC1101_SPI_HDR_RW_BIT;

   hdrByte |= regAddr;

   if (regAddr >= CC1101_PARTNUM_REG_ADDR)
       hdrByte |= CC1101_SPI_HDR_BURST_BIT;

   sts = CC1101_spiBegin();
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   /*
    * Table 22 
    * (Tsp) CSn low to positive edge on SCLK - 20 nanosecs minimum
    */
   PLTFRM_delay1MicroSecs();

   CC1101_spiTxRx(hdrByte);

   // PLTFRM_delay1MicroSecs();

   *(regVal_p) = CC1101_spiTxRx(0x0);

   /*
    * Table 22 
    * (Tns) - Negative edge on SCLK to CSn high - minimum 20 nanosecs
    */
   PLTFRM_delay1MicroSecs();

   CC1101_spiEnd();

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
PLTFRM_sts_t CC1101_writeReg(UINT8_t regAddr, UINT8_t regVal)
{
   PLTFRM_sts_t sts;
   UINT8_t hdrByte = 0x0;
   
   hdrByte |= regAddr;
   
   if (regAddr >= CC1101_PARTNUM_REG_ADDR)
       hdrByte |= CC1101_SPI_HDR_BURST_BIT;

   sts = CC1101_spiBegin();
   if (sts != PLTFRM_STS_SUCCESS)
       return sts; 
   
   /*
    * All transfers on the SPI interface are done most significant bit 
    * first.  All transactions on the SPI interface start with a header 
    * byte containing a R/WÂ¯ bit, a burst access bit (B), and a 6-bit 
    * address (A5 - A0).
    */

   /*
    * Tsp - CSn low to positive edge on SCLK - 20 nanosecs minimum
    */  
   PLTFRM_delay1MicroSecs();
   
   CC1101_spiTxRx(hdrByte);

   PLTFRM_delay1MicroSecs();

   CC1101_spiTxRx(regVal);

   /*
    * Tns - Negative edge on SCLK to CSn high - minimum 20 nanosecs
    */  
   PLTFRM_delay1MicroSecs();

   CC1101_spiEnd();
  
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
PLTFRM_sts_t CC1101_sendCmdStrobe(UINT8_t cmdStrobeId, CC1101_state_t *fsmState_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t stsByte, hdrByte = 0x0;

   /*
    * Command Strobes may be viewed as single byte instructions to CC1101. By 
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

   if (cmdStrobeId < CC1101_CMD_STROBE_SRES
       || cmdStrobeId > CC1101_CMD_STROBE_SNOP)
   {
       SYS_fatal(SYS_FATAL_ERR_150);
       return PLTFRM_STS_INV_PARAM;
   }

   hdrByte = cmdStrobeId;

   sts = CC1101_spiBegin();
   if (sts != PLTFRM_STS_SUCCESS)
       return sts; 

   /*
    * CSn low to positive edge on SCLK - 20 nanosecs minimum
    */  
   PLTFRM_delay1MicroSecs();


   // The status byte is available on the SO pin when a command strobe is being sent.
   // S6, S5, and S4 comprise the STATE value which reflects the state of the chip.

#ifdef CC1101_TIMING_TEST
   if (cmdStrobeId == CC1101_CMD_STROBE_STX)
   {
       P1OUT |= (BIT5 | BIT6);
   }
#endif

   stsByte = CC1101_spiTxRx(hdrByte);

   // Tns- Negative edge on SCLK to CSn high - minimum 20 nanosecs
   PLTFRM_delay1MicroSecs();

   CC1101_spiEnd();

   if (fsmState_p != NULL)
   {
       stsByte >>= CC1101_STS_BYTE_CHIP_STATE_SHIFT;
       stsByte &= CC1101_STS_BYTE_CHIP_STATE_BIT_MSK_SHIFT;
       *fsmState_p = CC1101_FSM_STATE_MAPPING[stsByte];
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
PLTFRM_sts_t CC1101_updateRegField(const UINT8_t regAddr,
                                   const UINT8_t newVal,
                                   const UINT8_t bitMsk,
                                   const UINT8_t bitShift,
                                   const UINT8_t bitMskShifted)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;

   sts = CC1101_readReg(regAddr, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       UINT8_t encByte;
       regVal &= ~(bitMsk);

       encByte = (UINT8_t)newVal;
       encByte <<= bitShift;
       encByte &= bitMsk;

       regVal |= encByte;

       sts = CC1101_writeReg(regAddr, regVal);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           sts = CC1101_readReg(regAddr, &regVal);
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
PLTFRM_sts_t CC1101_cfgChipRegs(const CC1101_chipRegInfo_s *reg_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   // const CC1101_chipRegInfo_s *info_p = &(CC1101_chipRegInfoList[0]);

   while (reg_p->regAddr != 0xff)
   {
       sts = CC1101_updateRegField(reg_p->regAddr,
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
PLTFRM_sts_t CC1101_cfgRegOnWakeUp(UINT8_t regAddr)
{
   PLTFRM_sts_t sts = PLTFRM_STS_REG_NOT_FOUND;
   const CC1101_chipRegInfo_s *info_p = &(CC1101_chipRegInfoList[0]);

   while (info_p->regAddr != 0xff)
   {
      if (info_p->regAddr == regAddr)
      {
          sts = CC1101_updateRegField(info_p->regAddr,
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
PLTFRM_sts_t CC1101_verifyPATable(void)
{
   UINT16_t hdrByte = CC1101_SPI_HDR_RW_BIT;
   UINT16_t tblIdx;
   PLTFRM_sts_t sts;

   hdrByte |= CC1101_PA_TABLE_ADDR;
   
   sts = CC1101_spiBegin();
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   for (tblIdx=0; tblIdx<CC1101_PA_TABLE_LEN; tblIdx++)
   {
       UINT8_t regVal;

       PLTFRM_delay10MicroSecs();
       CC1101_spiTxRx(hdrByte);
       PLTFRM_delay10MicroSecs();
       regVal = CC1101_spiTxRx(0x0);
       if (regVal != CC1101_paTableCfgTbl[tblIdx])
       {
           sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;
           break;
       }
   }

   PLTFRM_delay10MicroSecs();

   CC1101_spiEnd();

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
PLTFRM_sts_t CC1101_cfgPATable(void)
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
   
   hdrByte |= CC1101_PA_TABLE_ADDR; 
   
   sts = CC1101_spiBegin();
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;
       
   for (tblIdx=0; tblIdx<CC1101_PA_TABLE_LEN; tblIdx++)
   {
       PLTFRM_delay10MicroSecs();
       CC1101_spiTxRx(hdrByte);
       PLTFRM_delay10MicroSecs();
       CC1101_spiTxRx(CC1101_paTableCfgTbl[tblIdx]);
   }

   PLTFRM_delay10MicroSecs();

   CC1101_spiEnd();

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
PLTFRM_sts_t CC1101_waitTillRSSIValid(void)
{
   UINT16_t idx;

   /*
    * It takes some time from the radio enters RX mode until a valid RSSI value is
    * present in the RSSI register. Please see DN505 [12] for details on how the RSSI
    * response time can be estimated.
    *
    * Refer to swra114d.pdf for details.
    *
    * For now, just wait for x millisecs while the CC1101 starts getting valid RSSI
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
SINT16_t CC1101_calcPktRSSI(UINT8_t rssiRaw)
{
   SINT16_t rssi;

   /*
    * The RSSI value read from the RSSI status register is a 2’s complement
    * number. The following procedure can be used to convert the RSSI reading
    * to an absolute power level (RSSI_dBm)
    *
    * 1) Read the RSSI status register
    * 2) Convert the reading from a hexadecimal number to a decimal number (RSSI_dec)
    * 3) If RSSI_dec ≥ 128 then RSSI_dBm = (RSSI_dec - 256)/2 – RSSI_offset
    * 4) Else if RSSI_dec < 128 then RSSI_dBm = (RSSI_dec)/2 – RSSI_offset
    *
    * Ref Table 31
    * RSSI_offset is 74 dB (868 MHz, 1.2 kBaud/38.4 kBaud/250 kBaud/500 kBud)
    */

   if (rssiRaw >= 128)
       rssi = (rssiRaw - 256);
   else
       rssi = rssiRaw;
   rssi >>= 1;
   rssi -= CC1101_RSSI_OFFSET_IN_DB;

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
PLTFRM_sts_t CC1101_getCurrState(CC1101_state_t *state_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

#ifdef CC1101_WOR_ENA
   if (CC1101_cntxt.inWOR)
 	   *state_p = CC1101_STATE_WOR;
   else
#endif
   {
#if 0
       if (CC1101_cntxt.inLPM)
           *state_p = CC1101_STATE_SLEEP;
       else
#endif
       sts = CC1101_sendCmdStrobe(CC1101_CMD_STROBE_SNOP, state_p);
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
PLTFRM_sts_t CC1101_forceIdle(void)
{
   CC1101_state_t state;
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
   sts = CC1101_sendCmdStrobe(CC1101_CMD_STROBE_SIDLE, NULL);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   do
   {
       sts = CC1101_getCurrState(&state);
       if (sts != PLTFRM_STS_SUCCESS)
           break;
       PLTFRM_delay100MicroSecs();
   } while (state != CC1101_STATE_IDLE);

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
PLTFRM_sts_t CC1101_disableCCA(void)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;
   const UINT8_t regAddr = CC1101_MCSM1_REG_ADDR;

   sts = CC1101_readReg(regAddr, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_151);
   else
   {
       regVal &= ~(CC1101_MCSM1_REG_CCA_MODE_FIELD_BIT_MSK);

       // Mode 00 - always give clear channel indication

       sts = CC1101_writeReg(regAddr, regVal);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_152);

       sts = CC1101_readReg(regAddr, &regVal);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_153);
       else
       {
           if (regVal & CC1101_MCSM1_REG_CCA_MODE_FIELD_BIT_MSK)
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
UINT16_t CC1101_calcPktTrxTime(UINT16_t pktLen)
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

   temp16 = CC1101_preambleLen;
   temp16 <<= 3;   // In bits
   totSymCnt = temp16;

   temp16 = CC1101_syncLen;
   temp16 <<= 3;   // In bits
   totSymCnt += temp16;

   temp16 = pktLen + CC1101_FRAME_CRC_FIELD_LEN;
   temp16 <<= 3;   // In bits
   if (CC1101_modFormat == CC1101_MOD_FORMAT_4_FSK)
       temp16 >>= 1;  // 2 bits per symbol
   totSymCnt += temp16; 
   
   pktTxTime = totSymCnt;
   pktTxTime *= CC1101_microSecsPerSym;
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
PLTFRM_sts_t CC1101_enableCCA(void)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;
   const UINT8_t regAddr = CC1101_MCSM1_REG_ADDR;

   sts = CC1101_readReg(regAddr, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       UINT8_t newMode;

       regVal &= ~(CC1101_MCSM1_REG_CCA_MODE_FIELD_BIT_MSK);

       newMode = CC1101_CCA_MODE_3;  // Indicates clear channel when RSSI is below
                                     // threshold and currently not receiving a packet

       newMode <<= CC1101_MCSM1_REG_CCA_MODE_FIELD_SHIFT;
       newMode &= CC1101_MCSM1_REG_CCA_MODE_FIELD_BIT_MSK;

       regVal |= newMode;

       sts = CC1101_writeReg(regAddr, regVal);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           sts = CC1101_readReg(regAddr, &regVal);
           if (sts == PLTFRM_STS_SUCCESS)
           {
               newMode = (regVal >> CC1101_MCSM1_REG_CCA_MODE_FIELD_SHIFT);
               newMode &= CC1101_MCSM1_REG_CCA_MODE_FIELD_SHIFT_MSK;
               if (newMode != CC1101_CCA_MODE_3)
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
PLTFRM_sts_t CC1101_startRx(void)
{
   PLTFRM_sts_t sts;
   CC1101_state_t cc1101State;

   /*
    * <SRX cmd strobe>
    * Enable RX. Perform calibration first if coming from IDLE and
    * SETTLING_CFG.FS_AUTOCAL = 1.
    *
    * IDLE -> Frequency synthesizer is turned on, can optionally be
    * calibrated, and then settles to the correct frequency -> RX.
    */

   sts = CC1101_sendCmdStrobe(CC1101_CMD_STROBE_SRX, NULL);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_155);
   }
   else
   {
       do
       {
           sts = CC1101_sendCmdStrobe(CC1101_CMD_STROBE_SNOP, &cc1101State);
           if (sts != PLTFRM_STS_SUCCESS)
           {
               SYS_fatal(SYS_FATAL_ERR_156);
           }
       }  while (cc1101State != CC1101_STATE_RX);
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
PLTFRM_sts_t CC1101_getRxFIFONextByte(UINT8_t *buff_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t hdrByte = CC1101_SPI_HDR_RW_BIT;
   
   /*
    * The 64-byte TX FIFO and the 64-byte RX FIFO are accessed through 
    * the 0x3F address. When the R/W bit is zero, the TX FIFO is accessed, 
    * and the RX FIFO is accessed when the R/W bit is one.
    */

   hdrByte |= CC1101_RX_FIFO_ADDR;

   sts = CC1101_spiBegin();
   if (sts != PLTFRM_STS_SUCCESS)
       return sts; 

   /*
    * Table 22 
    * (Tsp) CSn low to positive edge on SCLK - 20 nanosecs minimum
    */
   PLTFRM_delay1MicroSecs();

   CC1101_spiTxRx(hdrByte);   // Send 0x3f
   
   PLTFRM_delay1MicroSecs();
   
   *(buff_p) = CC1101_spiTxRx(0x0);  // Read 1 byte

   /*
    * Table 22 
    * (Tns) - Negative edge on SCLK to CSn high - minimum 20 nanosecs
    */
   PLTFRM_delay1MicroSecs();

   CC1101_spiEnd();

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
PLTFRM_sts_t CC1101_flushTxFIFO(void)
{
   /*
    * Flush the TX FIFO. Only issue in IDLE or TX_FIFO_UNDERFLOW states
    */
   return CC1101_sendCmdStrobe(CC1101_CMD_STROBE_SFTX, NULL);
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
PLTFRM_sts_t CC1101_flushRxFIFO(void)
{
   /*
    * Flush the RX FIFO. Only issue in IDLE or RX_FIFO_OVERFLOW states
    */
   return CC1101_sendCmdStrobe(CC1101_CMD_STROBE_SFRX, NULL);
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
PLTFRM_sts_t CC1101_writeByteToTxFIFO(UINT8_t byte)
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

   hdrByte |= CC1101_TX_FIFO_ADDR;

   sts = CC1101_spiBegin();
   if (sts != PLTFRM_STS_SUCCESS)
       return sts; 

   /*
    * Table 22 
    * (Tsp) CSn low to positive edge on SCLK - 20 nanosecs minimum
    */
   PLTFRM_delay1MicroSecs();

   CC1101_spiTxRx(hdrByte);   // Send 0x3f
   
   PLTFRM_delay1MicroSecs();
   
   CC1101_spiTxRx(byte);  // Write 1 byte

   /*
    * Table 22 
    * (Tns) - Negative edge on SCLK to CSn high - minimum 20 nanosecs
    */
   PLTFRM_delay1MicroSecs();

   CC1101_spiEnd();

   return sts;
}


#if 0
UINT8_t __rxFifoByteCnt;
UINT8_t CC1101_rxFifoOvflBuff[CC1101_RX_FIFO_LEN];
#endif

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void CC1101_rxOvflHndlr(void)
{
   PLTFRM_sts_t sts;

#if 0
   sts = CC1101_getRXFIFOByteCnt(&__rxFifoByteCnt);
   if (sts != PLTFRM_STS_SUCCESS || __rxFifoByteCnt == 0)
   {
       SYS_fatal(SYS_FATAL_ERR_157);
   }

   sts = CC1101_readRxFIFO(__rxFifoByteCnt, CC1101_rxFifoOvflBuff);
   if (sts != PLTFRM_STS_SUCCESS || __rxFifoByteCnt == 0)
   {
       SYS_fatal(SYS_FATAL_ERR_158);
   }

   if (CC1101_forceIdle() != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_159);
#endif

   // State will change from RX_OVERFLOW to IDLE
   if (CC1101_flushRxFIFO() != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_160);

   sts = CC1101_startRx();
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_161);

   return;
}

UINT8_t __rxFIFOByteCnt = 0, __rxFIFOHasDataCnt = 0;
CC1101_state_t  CC1101_radioCheckState;
UINT16_t CC1101_rxFIFOOvflCnt = 0;
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void CC1101_checkRadioErrors(void)
{
   PLTFRM_sts_t sts;

#ifdef DEV_TYPE_RFD
   sts = CC1101_getRXFIFOByteCnt(&__rxFIFOByteCnt);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_162);

   if (__rxFIFOByteCnt)
       __rxFIFOHasDataCnt ++;
#endif

   sts = CC1101_getCurrState(&CC1101_radioCheckState);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       switch (CC1101_radioCheckState)
       {
          case CC1101_STATE_RXFIFO_OVERFLOW:
               {
                  CC1101_rxFIFOOvflCnt ++;
                  CC1101_rxOvflHndlr();
               }
               break;

          case CC1101_STATE_TXFIFO_UNDERFLOW:
               // Can never happen !!
               SYS_fatal(SYS_FATAL_ERR_163);
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
UINT8_t CC1101_txPowerSetPointValid(SINT16_t powerInDbm)
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
UINT32_t CC1101_getCarrierFreq(UINT8_t channIdx)
{
   UINT32_t cf = CC1101_channelSpacingHz;
   cf *= channIdx;
   cf += CC1101_baseFreqHz;
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
PLTFRM_sts_t CC1101_setTxPower(SINT16_t intPart,
                               SINT16_t decPart)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t enc;
   SINT32_t pwr = intPart;

   // Allowed values - 10.80 / 10.00 / 1.50 / 0.30
   pwr *= 100;
   if (pwr < 0)
       pwr -= decPart;
   else
       pwr += decPart;

   if (pwr == 1080)
       enc = CC1101_TX_PWR_10_POINT_8_DBM_PATABLE_ENC;
   else
   {
       if (pwr == 1000)
           enc = CC1101_TX_PWR_10_POINT_0_DBM_PATABLE_ENC;
       else
       {
           if (pwr == 150)
               enc = CC1101_TX_PWR_1_POINT_5_DBM_PATABLE_ENC;
           else
           {
               if (pwr == 30)
                   enc = CC1101_TX_PWR_0_POINT_3_DBM_PATABLE_ENC;
               else
                   sts = PLTFRM_STS_INV_PARAM;
           }
       }
   }

   if (sts == PLTFRM_STS_SUCCESS)
   {
       CC1101_paTableCfgTbl[0] = enc;
       sts = CC1101_cfgPATable();
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_164);

       sts = CC1101_verifyPATable();
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
PLTFRM_sts_t CC1101_getTXFIFOByteCnt(UINT8_t *byteCnt_p)
{
   PLTFRM_sts_t sts;

   sts = CC1101_readReg(CC1101_TXBYTES_REG_ADDR, byteCnt_p);
   if (sts == PLTFRM_STS_SUCCESS)
       (*byteCnt_p) &= CC1101_NUM_TXBYTES_BIT_MSK;

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
PLTFRM_sts_t CC1101_readRxFIFO(UINT8_t cnt, UINT8_t *buff_p)
{
   PLTFRM_sts_t sts;
   SINT16_t idx;

#if 1
   // Read the RX FIFO using burst access mode.
   {
      UINT8_t hdrByte = CC1101_SPI_HDR_RW_BIT | CC1101_SPI_HDR_BURST_BIT | CC1101_RX_FIFO_ADDR;
      UINT8_t dummyByte, globalIntFlag;

      PLTFRM_intSaveDisable(globalIntFlag);

      /*
       * The 64-byte TX FIFO and the 64-byte RX FIFO are accessed through
       * the 0x3F address. When the R/W bit is zero, the TX FIFO is accessed,
       * and the RX FIFO is accessed when the R/W bit is one.
       *
       * 0xFF: Burst access to RX FIFO
       */
      sts = CC1101_spiBegin();
      if (sts != PLTFRM_STS_SUCCESS)
          return sts;

      /*
       * Table 22
       * (Tsp) CSn low to positive edge on SCLK - 20 nanosecs minimum
       */
      PLTFRM_delay1MicroSecs();

      CC1101_spiTxRxNoInt(hdrByte, dummyByte);

      (void)dummyByte;

      for (idx=0; idx<cnt; idx++)
      {
    	  UINT8_t byte;
          CC1101_spiTxRxNoInt(0x0, byte);
          if (buff_p != NULL)
        	  buff_p[idx] = byte;
      }
      /*
       * Table 22
       * (Tns) - Negative edge on SCLK to CSn high - minimum 20 nanosecs
       */
      PLTFRM_delay1MicroSecs();

      CC1101_spiEnd();

      PLTFRM_intRestore(globalIntFlag);
   }
#else
   for (idx=0; idx<cnt; idx++)
   {
       sts = CC1101_getRxFIFONextByte(buff_p);
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
PLTFRM_sts_t CC1101_writeBuffToTxFIFO(UINT8_t *buff_p, UINT8_t buffLen)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t globalIntFlag, dummyByte,
           hdrByte =  CC1101_SPI_HDR_BURST_BIT | CC1101_TX_FIFO_ADDR;

   if (buffLen > CC1101_MAX_TX_PKT_LEN)
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

   sts = CC1101_spiBegin();
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

   CC1101_spiTxRxNoInt(hdrByte, dummyByte);
   CC1101_spiTxRxNoInt(buffLen, dummyByte);

   (void)dummyByte;

   {
      SINT16_t idx;
      for (idx=0; idx<buffLen; idx++)
           CC1101_spiTxRxNoInt(buff_p[idx], dummyByte);
   }

   /*
    * Table 22
    * (Tns) - Negative edge on SCLK to CSn high - minimum 20 nanosecs
    */
   PLTFRM_delay1MicroSecs();

   CC1101_spiEnd();

   PLTFRM_intRestore(globalIntFlag);

   PLTFRM_delay1MicroSecs();

   {
      UINT8_t byteCnt;

      sts = CC1101_getTXFIFOByteCnt(&byteCnt);
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
PLTFRM_sts_t CC1101_writeBuffToTxFIFO(UINT8_t *buff_p, UINT8_t buffLen)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   if (buffLen > (CC1101_TX_FIFO_LEN - CC1101_FRAME_LENGTH_FIELD_LEN))
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

       if (buffLen > (CC1101_TX_FIFO_LEN - CC1101_FRAME_LENGTH_FIELD_LEN))
       {
           SYS_fatal(SYS_FATAL_ERR_167);
           return PLTFRM_STS_FRAME_TOO_LONG_FOR_TX_FIFO;
       }

       sts = CC1101_getTXFIFOByteCnt(&byteCnt);
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

       sts = CC1101_writeByteToTxFIFO(buffLen);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_169);

        // Now write the actual payload
       for (idx=0; idx<buffLen; idx++)
       {
            sts = CC1101_writeByteToTxFIFO(*(buff_p + idx));
            if (sts !=  PLTFRM_STS_SUCCESS)
                SYS_fatal(SYS_FATAL_ERR_170);
       }

       sts = CC1101_getTXFIFOByteCnt(&byteCnt);
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
PLTFRM_sts_t CC1101_transferPktToTxFifo(UINT8_t bdHndl)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   if (!(SYS_BD_HNDL_IS_VALID(bdHndl)))
   {
       SYS_fatal(SYS_FATAL_ERR_173);
   }

   sts = CC1101_writeBuffToTxFIFO(SYS_GET_BD_BUFF(bdHndl),
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
PLTFRM_sts_t CC1101_getRXFIFOByteCnt(UINT8_t *byteCnt_p)
{
   PLTFRM_sts_t sts;

   sts = CC1101_readReg(CC1101_RXBYTES_REG_ADDR, byteCnt_p);
   if (sts == PLTFRM_STS_SUCCESS)
       (*byteCnt_p) &= CC1101_NUM_RXBYTES_BIT_MSK;
 
   return sts;
}

#ifdef CC1101_WOR_ENA
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t CC1101_exitWOR(void)
{
   PLTFRM_sts_t sts;

   // To exit WOR mode, an SIDLE strobe must be issued.
   // The SIDLE command strobe can always be used to force the radio
   // controller to go to the IDLE state.
   sts = CC1101_sendCmdStrobe(CC1101_CMD_STROBE_SIDLE, NULL);
   if (sts == PLTFRM_STS_SUCCESS)
       CC1101_cntxt.inWOR = 0;
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
PLTFRM_sts_t CC1101_enterWOR(void)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;

   sts = CC1101_sendCmdStrobe(CC1101_CMD_STROBE_SWOR, NULL);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_807);

   regVal = 0x6 | CC1101_IOCFGX_IRQ_INVERT_BIT;

   // Asserts when sync word has been sent / received, and de-asserts at the
   // end of the packet. In RX, the pin will also deassert when a packet is
   // discarded due to address or maximum length filtering or when the radio
   // enters RXFIFO_OVERFLOW state. In TX the pin will de-assert if the TX
   // FIFO underflows.

   sts = CC1101_updateRegField(CC1101_IOCFG0_REG_ADDR,
                               regVal, 0xff, 0x0, 0xff);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_808);

   /*
    * A �Preamble Quality Reached� signal can be observed on one of the GDO pins by
    * setting IOCFGx.GDOx_CFG=8.
    * We have set PKTCTRL1.PQT to 2. As a result, the preamble quality qualifier of
    * the sync word is enabled. This adds the requirement that the received sync word
    * must be preceded with a preamble with a quality above the programmed threshold.
    * The preamble quality estimator increases an internal counter by one each time a
    * bit is received that is different from the previous bit, and decreases the counter
    * by eight each time a bit is received that is the same as the last bit.
    */

   CC1101_cntxt.inWOR = 1;

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
PLTFRM_sts_t CC1101_cfgChan(UINT8_t channIdx)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;
   /*
    * Channel index range (0 - 8)
    */
   if (channIdx > PHY_865_867_MHZ_BAND_CHAN_9)
       return PLTFRM_STS_INV_PARAM;

   regVal = channIdx;

   sts =  CC1101_writeReg(CC1101_CHANNR_REG_ADDR, regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_174);
   regVal = 0x0;
   sts = CC1101_readReg(CC1101_CHANNR_REG_ADDR, &regVal);
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
PLTFRM_sts_t CC1101_spiInit(void)
{
   PLTFRM_sts_t sts;

   // CC1101 CSn - default HIGH

   sts = PLTFRM_allocGPIOPin(PLTFRM_CC1101_1_DEV_ID,
                             PLTFRM_GPIO_PORT_4,
                             PLTFRM_GPIO_PORT_PIN_0,
                             PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                             PLTFRM_GPIO_OUTPUT_HIGH,
                             PLTFRM_GPIO_INT_EDGE_NA,
                             NULL);
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
void CC1101_irqHndlr(void)
{
#ifdef CC1101_WOR_ENA
    UINT8_t reg = P1IFG;

    /*
     * > GDO0  (Connected to micro P1_1) : 0x06
     *   Asserts when sync word has been sent / received, and de-asserts at
     *   the end of the packet. In RX, the pin will also deassert when a pkt
     *   is discarded due to address or maximum length filtering or when the
     *   radio enters RXFIFO_OVERFLOW state. In TX the pin will de-assert if
     *   the TX FIFO underflows.
     */
    if (reg & BIT1)
    {
#ifdef CC1101_TIMING_TEST
        P1OUT &= ~BIT5;
#endif

        P1IFG &= ~BIT1;

        if (CC1101_cntxt.inWOR)
        {
#ifdef CC1101_INTR_STATS_ENA
            CC1101_cntxt.syncRcvdIntCnt ++;
#endif
            CC1101_cntxt.syncRcvdFlag = 1;
            CC1101_cntxt.inWOR = 0;   // Actively receiving
        }
        else
        {
#ifdef CC1101_INTR_STATS_ENA
            CC1101_cntxt.txStartedFlagIntCnt ++;
#endif
            CC1101_cntxt.txStartedFlag = 1;
        }
    }

    /*
     * There are two GPIO pins available.
     * > GDO2  (Connected to micro P1_0)
     *   Digital output.
     *   Packet with good CRC received
     */
    if (reg & BIT0)
    {
        P1IFG &= ~BIT0;
#ifdef CC1101_INTR_STATS_ENA
        CC1101_cntxt.crcOkPktRcvdIntCnt ++;
#endif
        CC1101_cntxt.crcOkPktInRxFIFOCnt ++;

        CC1101_cntxt.syncRcvdFlag = 0;

#ifdef MAC_TRACE_ENA
        UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_105);
#endif

        SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_FRAME_RX_DONE;
    }
#else
    UINT8_t reg = P1IFG;

    /*
     * > GDO0  (Connected to micro P1_1)
     *   Digital output.
     *   PA_PD
     */
    if (reg & BIT1)
    {
#ifdef CC1101_TIMING_TEST
        P1OUT &= ~BIT5;
#endif

        P1IFG &= ~BIT1;

#ifdef CC1101_INTR_STATS_ENA
        CC1101_cntxt.txStartedFlagIntCnt ++;
#endif
        CC1101_cntxt.txStartedFlag = 1;
    }

    /*
     * There are two GPIO pins available.
     * > GDO2  (Connected to micro P1_0)
     *   Digital output.
     *   Packet with good CRC received
     */
    if (reg & BIT0)
    {
        P1IFG &= ~BIT0;

#ifdef CC1101_INTR_STATS_ENA
        CC1101_cntxt.crcOkPktRcvdIntCnt ++;
#endif
        CC1101_cntxt.crcOkPktInRxFIFOCnt ++;

#ifdef MAC_TRACE_ENA
        UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_105);
#endif

        SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_FRAME_RX_DONE;
    }
#endif
}



#if 0
#ifdef CC1101_WOR_ENA
#ifdef ECLIPSE_IDE
static void __attribute__((__interrupt__(PORT1_VECTOR))) Port_1 (void)
#else
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
#endif
{
    UINT8_t reg = P1IFG;

    /*
     * > GDO0  (Connected to micro P1_1) : 0x06
     *   Asserts when sync word has been sent / received, and de-asserts at
     *   the end of the packet. In RX, the pin will also deassert when a pkt
     *   is discarded due to address or maximum length filtering or when the
     *   radio enters RXFIFO_OVERFLOW state. In TX the pin will de-assert if
     *   the TX FIFO underflows.
     */
    if (reg & BIT1)
    {
#ifdef CC1101_TIMING_TEST
        P1OUT &= ~BIT5;
#endif

        P1IFG &= ~BIT1;

        if (CC1101_cntxt.inWOR)
        {
#ifdef CC1101_INTR_STATS_ENA
            CC1101_cntxt.syncRcvdIntCnt ++;
#endif
            CC1101_cntxt.syncRcvdFlag = 1;
            CC1101_cntxt.inWOR = 0;   // Actively receiving
        }
        else
        {
#ifdef CC1101_INTR_STATS_ENA
            CC1101_cntxt.txStartedFlagIntCnt ++;
#endif
            CC1101_cntxt.txStartedFlag = 1;
        }
    }

    /*
     * There are two GPIO pins available.
     * > GDO2  (Connected to micro P1_0)
     *   Digital output.
     *   Packet with good CRC received
     */
    if (reg & BIT0)
    {
        P1IFG &= ~BIT0;
#ifdef CC1101_INTR_STATS_ENA
        CC1101_cntxt.crcOkPktRcvdIntCnt ++;
#endif
        CC1101_cntxt.crcOkPktInRxFIFOCnt ++;

        CC1101_cntxt.syncRcvdFlag = 0;

#ifdef MAC_TRACE_ENA
        UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_105);
#endif

        SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_FRAME_RX_DONE;
    }

    if (SYS_globalEvtMsk0 != 0x0)
    {
        PLTFRM_exitLPM();
    }
}

#else

#ifdef ECLIPSE_IDE
static void __attribute__((__interrupt__(PORT1_VECTOR))) Port_1 (void)
#else
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
#endif
{
    UINT8_t reg = P1IFG;

    /*
     * > GDO0  (Connected to micro P1_1)
     *   Digital output.
     *   PA_PD
     */
    if (reg & BIT1)
    {
#ifdef CC1101_TIMING_TEST
        P1OUT &= ~BIT5;
#endif

        P1IFG &= ~BIT1;
#ifdef CC1101_INTR_STATS_ENA
        CC1101_cntxt.txStartedFlagIntCnt ++;
#endif
        CC1101_cntxt.txStartedFlag = 1;
    }

    /*
     * There are two GPIO pins available.
     * > GDO2  (Connected to micro P1_0)
     *   Digital output.
     *   Packet with good CRC received
     */
    if (reg & BIT0)
    {
        P1IFG &= ~BIT0;
#ifdef CC1101_INTR_STATS_ENA
        CC1101_cntxt.crcOkPktRcvdIntCnt ++;
#endif
        CC1101_cntxt.crcOkPktInRxFIFOCnt ++;

#ifdef MAC_TRACE_ENA
        UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_105);
#endif

        SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_FRAME_RX_DONE;
    }

    if (SYS_globalEvtMsk0 != 0x0)
    {
        PLTFRM_exitLPM();
    }
}

#endif
#endif


#define CC1101_RSSI_MON_MAX_BIN_CNT  32

UINT16_t CC1101_rssiMonBins[CC1101_RSSI_MON_MAX_BIN_CNT];
SINT16_t CC1101_rssiMonBinIdx[CC1101_RSSI_MON_MAX_BIN_CNT];

/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
void CC1101_monRSSI(RADIO_channMonReq_s *reqParams_p)
{
  UINT16_t idx;
  SINT32_t totalRSSI = 0;
  UINT8_t binsInUse, mdmCfg2RegVal;
  SINT16_t binSpan, rssiRange;
  PLTFRM_sts_t sts;

  reqParams_p->minMeasRSSI = 1000;
  reqParams_p->maxMeasRSSI = -1000;

  memset(CC1101_rssiMonBins, 0, sizeof(CC1101_rssiMonBins));

  for (idx=0; idx<CC1101_RSSI_MON_MAX_BIN_CNT; idx++)
       CC1101_rssiMonBinIdx[idx] = idx;

#if 0
  sts = CC1101_cfgChan(reqParams_p->channIdx);
  if (sts != PLTFRM_STS_SUCCESS)
      SYS_fatal(SYS_FATAL_ERR_848);
#endif

  // Disable SYNC word detection
  {
     UINT8_t regVal;

     // The values 0 (000) and 4 (100) disables preamble and sync word transmission
     // in TX and preamble and sync word detection in RX.

     sts = CC1101_readReg(CC1101_MDMCFG2_REG_ADDR, &mdmCfg2RegVal);
     if (sts != PLTFRM_STS_SUCCESS)
         SYS_fatal(SYS_FATAL_ERR_847);

     regVal = mdmCfg2RegVal;

     regVal &= ~(0x7);
     sts = CC1101_writeReg(CC1101_MDMCFG2_REG_ADDR, regVal);
     if (sts != PLTFRM_STS_SUCCESS)
         SYS_fatal(SYS_FATAL_ERR_845);
  }

  if (CC1101_startRx() != PLTFRM_STS_SUCCESS)
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
     if (binsInUse <= (CC1101_RSSI_MON_MAX_BIN_CNT - 2))
         break;
     binSpan ++;
  } while (binSpan <= rssiRange);

  for (idx=0; idx<reqParams_p->sampleCnt; idx++)
  {
      UINT8_t rssiU8;
      SINT16_t rssiS16;

      if (CC1101_readReg(CC1101_RSSI_REG_ADDR, &rssiU8) != PLTFRM_STS_SUCCESS)
          SYS_fatal(SYS_FATAL_ERR_843);
      rssiS16 = CC1101_calcPktRSSI(rssiU8);
      totalRSSI += rssiS16;

      if (rssiS16 < reqParams_p->minMeasRSSI)
    	  reqParams_p->minMeasRSSI = rssiS16;

      if (rssiS16 > reqParams_p->maxMeasRSSI)
    	  reqParams_p->maxMeasRSSI = rssiS16;

	  if (rssiS16 < reqParams_p->minMonRSSI)
          CC1101_rssiMonBins[0] ++;
	  else
	  {
	      if (rssiS16 > reqParams_p->maxMonRSSI)
              CC1101_rssiMonBins[binsInUse-1] ++;
          else
          {
              SINT16_t binIdx = rssiS16;
              binIdx -=  reqParams_p->minMonRSSI;
              binIdx /= binSpan;
              CC1101_rssiMonBins[binIdx + 1] ++;
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
               if (CC1101_rssiMonBins[idy] > CC1101_rssiMonBins[idx])
               {
                   SINT16_t tmp = CC1101_rssiMonBins[idy];
                   CC1101_rssiMonBins[idy] = CC1101_rssiMonBins[idx];
                   CC1101_rssiMonBins[idx] = tmp;
                   tmp = CC1101_rssiMonBinIdx[idy];
                   CC1101_rssiMonBinIdx[idy] = CC1101_rssiMonBinIdx[idx];
                   CC1101_rssiMonBinIdx[idx] = tmp;
               }
          }
      }


      for (idx=0; idx<binsInUse; idx++)
      {
           UINT32_t perc;

           if (CC1101_rssiMonBins[idx] == 0)
               break;

           perc = CC1101_rssiMonBins[idx];
           perc *= 100;
           perc /= reqParams_p->sampleCnt;

           CC1101_rssiMonBins[idx] = perc;

           if (CC1101_rssiMonBinIdx[idx] == 0)
               CC1101_rssiMonBinIdx[idx] = reqParams_p->minMonRSSI - 1;
           else
           {
               if (CC1101_rssiMonBinIdx[idx] == binsInUse - 1)
                   CC1101_rssiMonBinIdx[idx] = reqParams_p->maxMonRSSI + 1;
               else
               {
                   SINT16_t tmp = CC1101_rssiMonBinIdx[idx];
                   tmp *= binSpan;
                   tmp += reqParams_p->minMonRSSI;
                   CC1101_rssiMonBinIdx[idx] = tmp;
               }
           }
      }

      reqParams_p->binSpan = binSpan;
      reqParams_p->_rsltArray[0].percentage = CC1101_rssiMonBins[0];
      reqParams_p->_rsltArray[0].start = CC1101_rssiMonBinIdx[0];
      reqParams_p->_rsltArray[1].percentage = CC1101_rssiMonBins[1];
      reqParams_p->_rsltArray[1].start = CC1101_rssiMonBinIdx[1];
      reqParams_p->_rsltArray[2].percentage = CC1101_rssiMonBins[2];
      reqParams_p->_rsltArray[2].start = CC1101_rssiMonBinIdx[2];
  }

  PLTFRM_OFF_LED_1();
  PLTFRM_OFF_LED_2();

  {
     // restore mdmcfg2
     sts = CC1101_writeReg(CC1101_MDMCFG2_REG_ADDR, mdmCfg2RegVal);
     if (sts != PLTFRM_STS_SUCCESS)
         SYS_fatal(SYS_FATAL_ERR_846);
  }

  if (CC1101_forceIdle() != PLTFRM_STS_SUCCESS)
      SYS_fatal(SYS_FATAL_ERR_856);

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
PLTFRM_sts_t CC1101_gpioInit(void)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
 
   /*
    * There are two GPIO pins available.
    * > GDO2  (Connected to micro P1_0) 
    *   Digital output.
    *   Packet with good CRC received
    *
    * > GDO0  (Connected to micro P1_1) 
    *   Digital output.  
    *   PA_PD
    */

   // Globally interrupts are still disabled ...

   sts = PLTFRM_reserveGPIOPin(PLTFRM_CC1101_1_DEV_ID,
                               PLTFRM_GPIO_PORT_1,
                               PLTFRM_GPIO_PORT_PIN_0);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_177);

   // P1.0 <- GDO2
   P1DIR &= ~(BIT0);  // Input direction
   P1SEL  &= ~(BIT0); // I/O functionality
   P1SEL2 &= ~(BIT0);
   P1REN &= ~(BIT0);  // Pull up resistor disabled
   P1IFG &= ~(BIT0);  // clear interrupt flag
   // P1IE |= (BIT0);   // Interrupt enabled
   P1IES |= (BIT0);  // Active low


   sts = PLTFRM_reserveGPIOPin(PLTFRM_CC1101_1_DEV_ID,
                               PLTFRM_GPIO_PORT_1,
                               PLTFRM_GPIO_PORT_PIN_1);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_178);

   // P1.1 <- GDO0
   P1DIR &= ~(BIT1);  // Input direction
   P1SEL  &= ~(BIT1); // I/O functionality
   P1SEL2 &= ~(BIT1);
   P1REN &= ~(BIT1);  // Pull up resistor disabled
   P1IFG &= ~(BIT1);  // clear interrupt flag
   // P1IE |= (BIT1);   // Interrupt enabled
   P1IES &= ~(BIT1);  // Active high

   return sts;
}

#define CC1101_REG_READ_TEST
#ifdef CC1101_REG_READ_TEST
UINT8_t CC1101_regTableVal[CC1101_RCCTRL0_STATUS_REG_ADDR - CC1101_IOCFG2_REG_ADDR + 1];
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
PLTFRM_sts_t CC1101_waitChipRdy(void)
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
UINT8_t CC1101_swRst(void)
{
   /*
    * The other global reset possibility on CC1101 uses the SRES command strobe. 
    * By issuing this strobe, all internal registers and states are set to the 
    * default, IDLE state. The manual power-up sequence is as follows -
    * 
    */

    // Set SCLK = 1 and SI = 0, to avoid potential problems with pin control mode
    // (see Section 11.3).
    P3OUT |= BIT3;
    P3OUT &= ~BIT1;

    P4OUT |= BIT0;
    PLTFRM_delay10MicroSecs();
    // Strobe CSn low / high.
    // Hold CSn low and then high for at least 40 microsecs relative to pulling CSn low
    P4OUT &= ~BIT0;
    PLTFRM_delay10MicroSecs();
    P4OUT |= BIT0;
    PLTFRM_delay100MicroSecs();


    // Pull CSn low and wait for SO to go low (CHIP_RDYn).
    P4OUT &= ~BIT0; 

    if (CC1101_waitChipRdy() != PLTFRM_STS_SUCCESS)
        return 1;

    // Issue the SRES strobe on the SI line.
    if (CC1101_sendCmdStrobe(CC1101_CMD_STROBE_SRES, NULL) != PLTFRM_STS_SUCCESS)
    {
        return 2;
    }

    // When SO goes low again, reset is complete and the chip is in the IDLE state.
    if (CC1101_waitChipRdy() != PLTFRM_STS_SUCCESS)
    {
        return 3;
    }

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
PLTFRM_sts_t CC1101_exitLPM(void)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   /*
    * Setting CSn low again will turn on the regulator and crystal
    * oscillator and make the chip enter the IDLE state. The SO pin 
    * on the SPI interface must be pulled low before the SPI interface 
    * is ready to be used as described in Section 10.1.
    */

   P4OUT &= ~BIT0;

   sts = CC1101_waitChipRdy(); 
   if (sts == PLTFRM_STS_SUCCESS)
   {
       CC1101_state_t state;

       sts = CC1101_getCurrState(&state);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           if (state == CC1101_STATE_IDLE)
           {
               P1REN &= ~(BIT1);  // pull up disabled

               // Enable GPIO interrupts after clearing the interrupt flags
               P1IFG &= ~(BIT0 | BIT1);

               P1IE |= (BIT0 | BIT1);

               // Flush the FIFOs
               if (CC1101_flushTxFIFO() != PLTFRM_STS_SUCCESS)
                   SYS_fatal(SYS_FATAL_ERR_182);

               if (CC1101_flushRxFIFO() != PLTFRM_STS_SUCCESS)
                   SYS_fatal(SYS_FATAL_ERR_183);

               if (CC1101_cfgRegOnWakeUp(CC1101_TEST2_REG_ADDR) != PLTFRM_STS_SUCCESS)
                   SYS_fatal(SYS_FATAL_ERR_184);

               if (CC1101_cfgRegOnWakeUp(CC1101_TEST1_REG_ADDR) != PLTFRM_STS_SUCCESS)
                   SYS_fatal(SYS_FATAL_ERR_185);

               if (CC1101_cfgRegOnWakeUp(CC1101_TEST0_REG_ADDR) != PLTFRM_STS_SUCCESS)
                   SYS_fatal(SYS_FATAL_ERR_186);

               CC1101_cntxt.inLPM = 0;
               CC1101_cntxt.exitedLPMCnt ++;
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
PLTFRM_sts_t CC1101_enterLPM(void)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   CC1101_state_t state;

   /*
    * In low power applications, the SLEEP state with the crystal oscillator 
    * core switched off should be used when the CC1101 is not active.
    * 
    * When CSn is released ater a SPWD command strobe has been sent on the 
    * SPI interface, the CC1101 enters the SLEEP state which is the state
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

   // Disable the CC1101 interrupts

   if (CC1101_getCurrState(&state) == PLTFRM_STS_SUCCESS)
   {
       if (state != CC1101_STATE_IDLE)
       {
           if (CC1101_forceIdle() != PLTFRM_STS_SUCCESS)
               SYS_fatal(SYS_FATAL_ERR_187);
       }
   }
   else
       SYS_fatal(SYS_FATAL_ERR_188);

   P1IE &= ~(BIT0 | BIT1);

   P1REN |= (BIT1);  // pull up enabled
   P1OUT |= BIT1;  // Pulled high

   sts = CC1101_sendCmdStrobe(CC1101_CMD_STROBE_SPWD, NULL);
   
   if (sts == PLTFRM_STS_SUCCESS)
   {
       CC1101_cntxt.enteredLPMCnt ++;
       CC1101_cntxt.inLPM = 1;
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
void CC1101_insertTurnAroundDelay(void)
{
   // Give enough time for the transmitter to go from TX->IDLE->RX.
   // TX->IDLE with no cal takes .25/baud_rate
   // IDLE->RX with cal takes 1953/Xosc +  FS cal time (under 1 millisec)
   // At 1200 baud, total time will be 1.3 millisecs
   // At 10000 baud, total time will be 1.1 millisecs
   // FS cal time is around
#if defined(RADIO_BAUD_RATE_1200)  \
    || defined(RADIO_BAUD_RATE_10000)  \
    || defined(RADIO_BAUD_RATE_38383)
   {
      UINT16_t _idx;
      for (_idx=0; _idx<CC1101_TX_TO_RX_TURN_AROUND_DELAY_MILLI_SECS; _idx++)
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

#ifdef CC1101_LOAD_SWITCH_ENA
void CC1101_switchOn(void)
{
   PLTFRM_sts_t sts;

   sts = PLTFRM_allocGPIOPin(PLTFRM_CC1101_1_DEV_ID,
                             PLTFRM_GPIO_PORT_4,
                             PLTFRM_GPIO_PORT_PIN_1,
                             PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                             PLTFRM_GPIO_OUTPUT_LOW,
                             PLTFRM_GPIO_INT_EDGE_NA,
                             NULL);
   if (sts != PLTFRM_STS_SUCCESS)
	   SYS_fatal(SYS_FATAL_ERR_606);

   // Power off the CC1101
   sts = PLTFRM_setGPIOPinOutput(PLTFRM_GPIO_PORT_4, PLTFRM_GPIO_PORT_PIN_1, 0);
   if (sts != PLTFRM_STS_SUCCESS)
	   SYS_fatal(SYS_FATAL_ERR_607);

   __delay100MilliSecs();

   // Power on the CC1101
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
void CC1101_cfgCWMode(void)
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
    *   RX mode, data is set up on the falling edge by CC1101 when GDOx_INV=0.
    *   In TX mode, data is sampled by CC1101 on the rising edge of the serial
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

#if defined(CC1101_CW_MOD_CFG_ENA) || defined(CC1101_CW_UNMOD_CFG_ENA)
CC1101_state_t state_0, state_1;
#endif

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t CC1101_init(UINT8_t onPwrUp)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t regVal;
   UINT16_t idx;

   CC1101_cntxt.inLPM = 0;
#ifdef CC1101_WOR_ENA
   CC1101_cntxt.inWOR = 0;
#endif

   if (onPwrUp)
      CC1101_gpioInit();

   /*
    * 19.1.1 Automatic POR 
    * A power-on reset circuit is included in the CC1101. The minimum requirements 
    * stated in Table 18 must be followed for the power-on reset to function properly. 
    * The internal powerup sequence is completed when CHIP_RDYn goes low. CHIP_RDYn is 
    * observed on the SO pin after CSn is pulled low. See Section 10.1 for more details 
    * on CHIP_RDYn.
    *
    * When the CC1101 reset is completed, the chip will be in the IDLE state and the 
    * crystal oscillator will be running. If the chip has had sufficient time for the 
    * crystal oscillator to stabilize after the power-on-reset, the SO pin will go low 
    * immediately after taking CSn low. If CSn is taken low before reset is completed,
    * the SO pin will first go high, indicating that the crystal oscillator is not 
    * stabilized, before going low as shown in Figure 26.
    */

   // if ((sts = CC1101_spiInit( )) != PLTFRM_STS_SUCCESS)
   //    return sts;
   
   {
       UINT16_t rc;

       // Retry CC1101 reset multiple times ...

       for (idx=0; idx<10; idx++)
       {
           UINT16_t j;

           if ((rc = CC1101_swRst()) == 0x0)
               break;

           for (j=0; j<10; j++)
                PLTFRM_delay1MilliSec();
       }

       if (idx >= 10)
           SYS_fatal(SYS_FATAL_ERR_179 + rc - 1);
   }

   PLTFRM_delay1MilliSec();

   do
   {
      CC1101_state_t state;

      if (CC1101_getCurrState(&state) == PLTFRM_STS_SUCCESS)
      {
          if (state == CC1101_STATE_IDLE)
          {
              // Read the part number reg
              sts = CC1101_readReg(CC1101_PARTNUM_REG_ADDR, &regVal);
              if (sts == PLTFRM_STS_SUCCESS)
              {
                   if (regVal == CC1101_PART_NUMBER)
                   {
                       sts = CC1101_readReg(CC1101_VERSION_REG_ADDR, &regVal);
                       if (sts == PLTFRM_STS_SUCCESS)
                       {
                           if (regVal == CC1101_VERSION_NUMBER_0
                               || regVal == CC1101_VERSION_NUMBER_1)
                               break;
                       }
                   }
              }
          }
      }
   
      {
         UINT16_t idx;
         for (idx=0; idx<1; idx++)
             PLTFRM_delay1MilliSec();
      }


   } while (1);


#if defined(CC1101_CW_MOD_CFG_ENA) || defined(CC1101_CW_UNMOD_CFG_ENA)
   // rkris@wisense.in / june/9/15 - support for antenna performance testing
   // Unmodulated CW is a pure sine wave.


   sts = CC1101_setTxPower(10, 80);  // +10.8 dBm
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_867);

   sts = CC1101_getCurrState(&state_0);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_868);

   sts = CC1101_cfgChan(PHY_865_867_MHZ_BAND_CHAN_5);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_869);

   sts = CC1101_cfgChipRegs(CC1101_contWaveRandRegList);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_866);

   sts = CC1101_sendCmdStrobe(CC1101_CMD_STROBE_STX, NULL);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_870);

   sts = CC1101_getCurrState(&state_1);
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

       sts = CC1101_getCurrState(&state_1);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_870);
   }
#endif


   // Configure IOCFG0 - set to 0x1b (PA_PD)
   // This will get asserted 782/fRef us after the STX strobe has been issued
   // (fRef is the reference frequency for the synthesizer in MHz)
   // Configure the interrupt as active low
   regVal = 0x1b | CC1101_IOCFGX_IRQ_INVERT_BIT;
   sts =  CC1101_writeReg(CC1101_IOCFG0_REG_ADDR, regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_190);
   regVal = 0x0;
   sts = CC1101_readReg(CC1101_IOCFG0_REG_ADDR, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_191);
   if (regVal != (0x1b | CC1101_IOCFGX_IRQ_INVERT_BIT))
       SYS_fatal(SYS_FATAL_ERR_192);

   // Configure IOCFG2 - set to 0x7
   // Asserts when a packet has been received with CRC OK. De-asserts when the
   // first byte is read from the RX FIFO.
   // Configure the interrupt as active low
   regVal = 0x7 | CC1101_IOCFGX_IRQ_INVERT_BIT;
   sts =  CC1101_writeReg(CC1101_IOCFG2_REG_ADDR, regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_193);
   regVal = 0x0;
   sts = CC1101_readReg(CC1101_IOCFG2_REG_ADDR, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_194);
   if (regVal != (0x7 | CC1101_IOCFGX_IRQ_INVERT_BIT))
       SYS_fatal(SYS_FATAL_ERR_195);

   // Configure all the registers other than the center frequency and output
   // power level.
   sts = CC1101_cfgChipRegs(CC1101_chipRegInfoList);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_196);

#ifdef CC1101_REG_READ_TEST
   {
      UINT8_t regAddr;
      for (regAddr=CC1101_IOCFG2_REG_ADDR;
           regAddr<= CC1101_RCCTRL0_STATUS_REG_ADDR;
           regAddr++)
      {
           sts = CC1101_readReg(regAddr, &regVal);
           if (sts == PLTFRM_STS_SUCCESS)
               CC1101_regTableVal[regAddr - CC1101_IOCFG2_REG_ADDR] = regVal;
           else
               SYS_fatal(SYS_FATAL_ERR_189);
           PLTFRM_delay1MilliSec();
      }
   }
#endif

#if 0
   sts = CC1101_cfgPATable();
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_197);

   sts = CC1101_verifyPATable();
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_198);
#endif

   // Clear the interrupt flags (they seem to be getting set after being cleared in
   // CC1101_gpioInit()

   // Global interrupts are still disabled.
   P1IE |= (BIT0 | BIT1);

   P1IFG &= ~(BIT0 | BIT1);

   return sts;
}
#endif
