/*
 * File Name: cc1200.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Mar/4/2014
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


#ifdef RADIO_CC1200



#include <typedefs.h>
#ifdef __IAR__
#include "io430.h"
#else
#include <msp430.h>
#endif
#include <pltfrm.h>
#include <mac.h>
#include <system.h>
#include <phy_defs.h>
#include <phy_pib.h>
#include <cc1200.h>


#define CC1200_1200_BPS_CHAN_BW_20_KHZ


CC1200_cntxt_s  CC1200_cntxt;

const CC1200_syncMode_t CC1200_syncMode = CC1200_SYNC_MODE_32;

const UINT8_t CC1200_framePreambleLen = 24;  // In bits

#if (CC1200_CFG_SYM_RATE_SPS == 1200)

/*
 * Symbol rate - 1.2 ksps
 * Modulation - 2-FSK
 * Channel spacing - 12.5 kHz
 * Center frequency - 866 MHz
 * RX filter BW - 10.964912 kHz
 * Deviation - 3.9863 kHz
 * Xtal frequency - 40 MHz
 * Bit Rate - 1.2 kbps
 * Tx Power - 14 dBm
 * Performance mode - High
 *
 * <TI white paper>
 * (Long-range RF communication: Why narrowband is the de facto standard)
 *
 * Narrow band systems are defined as having less than 25 kHz bandwidth and
 * provide an excellent link budget due to low in-band receive noise (narrow
 * receive filters remove most of the noise). 12.5-kHz channel spacing with
 * 10-kHz receive bandwidth is commonly used. Examples of such systems are
 * police and safety radios, maritime communication systems, social alarms
 * and the new 169-MHz wireless M-bus (wM-Bus) standard for metering apps in
 * Europe. For wM-Bus, 169-MHz narrow band was chosen to get maximum range for
 * water and gas meters to enable fixed network deployments with very few
 * concentrators.
 *
 * For a given output power (often defined by governmental RF regulations), the
 * range of the RF link is determined by the data rate, i.e., lower rate provides
 * longer range due to increased sensitivity for the receiver. There is, of course,
 * also a trade-off, since very low rates mean very long time on air, which will
 * in turn reduce the battery lifetime. Having very long telegrams also  increases
 * the probability of interference/collisions with other wireless systems. So in
 * practical installations, long-range systems typically use a reasonably low data
 * rate, typically down to ~1 kbps, to optimize the range and transmission time
 * balance. Narrow band technique for long range and reasonably low date rate is
 * widely accepted by the industry since it gives the optimum tradeoff between range
 * and the transmission time.
 *
 * The drawback of a narrow band system has traditionally been the higher
 * requirements on the RF crystal. A frequency error on the RF crystal
 * leads to an offset on the programmed RF frequency. If the offset gets
 * too big, the signal will fall outside the channel, and be filtered out
 * by the strong receive filters. Legacy narrow band systems typically use
 * temperature-controlled oscillators (TCXOs). These have been more expensive
 * than standard crystals, but the difference has been drastically reduced.
 * Today, however, the accuracy of standard crystals are greatly improved, and
 * when combined with the novel WaveMatch and feedback frequency offset
 * to phase-lock loop (PLL) functionality of the CC1120/CC1200 Sub-1GHz smart
 * RF transceiver family, a narrow band system can be designed with a standard
 * crystal. Given the typical star network topology, it is also simple to
 * compensate for frequency errors from a tighter tolerance reference in the
 * concentrator.
 *
 *
 * Refer to http://e2e.ti.com/support/low_power_rf/f/155/t/151112.aspx
 *
 * Required RX filter bandwidth can be approximated as -
 *    (Signal BW) + 4x(ppm xtal)x(Frequency of operation)
 *    where Signal BW of = Data rate + 2 x frequency deviation.
 * In this case, Data rate is 1.2 kbps, freq dev is  3.9863 kHz.
 * Freq of operation is 40 MHz and ppm xtal is +/- 15 x 10^-6.
 * RX filter bw = 1200 + 2x3986 + 4 x 15x10^-6 x 40x10^6 = 11572
 *
 *
 * After sync is found the frequency offset estimate is saved in the FREQOFF_EST
 * registers. When SAFC is strobed in IDLE the content of FREQOFF_EST is copied
 * to FREQ_OFF. This value is used to calculate the RF frequency.
 * A SAFC command strobe can be issued in any state but does not take effect until
 * the next time the radio enters active mode (TX or RX).
 *
 * SYNC found ----> FREQOFF_EST ---------> FREQ_OFF
 */

const CC1200_chipRegInfo_s CC1200_chipRegInfoList[  ] =
{
   /*
    * SYNC_MODE - 101 (32 bits)
    * SYNC_THR  - 1010
    */
   // {CC1200_SYNC_CFG1_REG_ADDR,            0x0,   0xaa},

   /*
    * AUTO CLEAR        - 0
    * RX_CONFIG_LIMITATION  - 0
    * PQT_GATING_EN     - 0  (disabled)
    * EXT_SYNC_DETECT   - 0  (disabled)
    * STRICT_SYNC_CHECK - 11 (strict sync word check disabled)
    */
   // {CC1200_SYNC_CFG0_REG_ADDR,            0x0,   0x03},

   {CC1200_DEVIATION_M_REG_ADDR,          0x0,   0xd1},

   // MODEM_MODE - 00 (Normal mode)
   // MOD_FORMAT - 000 (2-FSK)
   // DEV_E      - 000 (Freq deviation exponent part)
   {CC1200_MODCFG_DEV_E_REG_ADDR,         0x0,   0x00},

   {CC1200_DCFILT_CFG_REG_ADDR,           0x0,   0x5d},

   /*
    * NUM_PREAMBLE - 101 (3 bytes)
    * PREAMBLE_WORD - 00 (10101010 (0xAA))
    */
   // {CC1200_PREAMBLE_CFG1_REG_ADDR,        0x0,   0x14},

   /*
    * PQT_EN - 1 (Preamble detection enabled)
    * PQT_VALID_TIMEOUT - 000 (11)
    * PQT - 1010 (Soft decision PQT)
    */
   {CC1200_PREAMBLE_CFG0_REG_ADDR,        0x0,   0x8a},

   {CC1200_IQIC_REG_ADDR,                 0x0,   0xcb},

#ifdef CC1200_1200_BPS_CHAN_BW_20_KHZ
   {CC1200_CHAN_BW_REG_ADDR,              0x0,   0x95},   // 20 kHz
#else
   {CC1200_CHAN_BW_REG_ADDR,              0x0,   0xa6},   // 10.964912 kHz
#endif

   {CC1200_MDMCFG1_REG_ADDR,              0x0,   0x40},
   {CC1200_MDMCFG0_REG_ADDR,              0x0,   0x05},

   /*
    *
    * Rsym = (((2^20) + SRATE_M) * (2^SRATE_E) * Fosc) / (2^39)
    * where Fosc = 40 * (10^6)
    * Here symbol rate is 1200 sps.
    * 0x3f/0x75/0x10 -> exponent is 3 (SRATE_E)
    *                   and mantissa is 0xf7510 -> 1013008 (SRATE_M))
    */
   {CC1200_SYMBOL_RATE2_REG_ADDR,         0x0,   0x3f},
   {CC1200_SYMBOL_RATE1_REG_ADDR,         0x0,   0x75},
   {CC1200_SYMBOL_RATE0_REG_ADDR,         0x0,   0x10},


#ifdef CC1200_1200_BPS_CHAN_BW_20_KHZ
   {CC1200_AGC_REF_REG_ADDR,              0x0,   0x31},  // 20 kHz
#else
   {CC1200_AGC_REF_REG_ADDR,              0x0,   0x20},  // 10.9649 kHz
#endif

   {CC1200_AGC_CS_THR_REG_ADDR,           0x0,   0x0},    // 0xec
   // {CC1200_AGC_GAIN_ADJUST_REG_ADDR,   0x0,   0x00},
   // {CC1200_AGC_CFG3_REG_ADDR,          0x0,   0xb1},
   // {CC1200_AGC_CFG2_REG_ADDR,          0x0,   0x20},
   {CC1200_AGC_CFG1_REG_ADDR,             0x0,   0x51},
   {CC1200_AGC_CFG0_REG_ADDR,             0x0,   0x87},

   // CRC_AUTOFLUSH - 1 (Auto flush last packet with bad CRC)
   // FIFO_THR - 126 (TXFIFO_THR signal will be de-asserted
   //                 when TX FIFO is drained below
   //                 (127 - FIFO_CFG.FIFO_THR) bytes. Setting
   //                 FIFO_THR to 126 means TXFIFO_THR signal
   //                 will get de-asserted when TX FIFO drains
   //                 below 1 byte (i.e TX FIFO becomes empty)).
   {CC1200_FIFO_CFG_REG_ADDR,             0x0,   0xfe},

   // {CC1200_DEV_ADDR_REG_ADDR,          0x0,   0x00},

   /*
    * FS_AUTOCAL - 01 (Auto calibration is performed when going from IDLE
    *                  to RX or TX (or FSTXON)
    * LOCK_TIME - 01 (75/30 microsecs)
    * FSREG_TIME - 1 (60 microsecs)
    */
   // {CC1200_SETTLING_CFG_REG_ADDR,         0x0,   0x0b},

   // FS_LOCK_EN - 1 (Out of lock detector enabled)
   // FSD_BANDSELECT - 2 (820 MHz - 960 MHz)
   {CC1200_FS_CFG_REG_ADDR,               0x0,   0x12},

   /*
    * RX_DUTY_CYCLE_MODE - 00 (disabled)
    */
   // {CC1200_RXDCM_TIME_REG_ADDR,           0x0,   0x00},

   // CCA_MODE - 011 (rssi below threshold and not receiving packet)
   // PKT_FORMAT - 00 (Normal/FIFO)
   {CC1200_PKT_CFG2_REG_ADDR,             0x0,   0x0c},

   // FEC_EN - 0 (disabled)
   // WHITE_DATA - 0 (disabled)
   // PN9_SWAP_EN - 0 (disabled)
   // ADDR_CHECK_CFG - 00 (disabled)
   // CRC CFG - 01 (CRC enabled and initialized to 0xffff)
   // APPEND_STATUS - 1
   // {CC1200_PKT_CFG1_REG_ADDR,             0x0,   0x03},

   // LENGTH_CONFIG 01 (Variable, length field included in packet)
   // UART_MODE_LEN 0 (UART mode disabled)
   // UART_SWAP_EN 0 (disabled)
   {CC1200_PKT_CFG0_REG_ADDR,             0x0,   0x20},

   // RXOFF_MODE   - 11 (RX - mode entered after receiving a good packet)
   // RX_TIME      - 111
   // RX_TIME_QUAL - 1
   {CC1200_RFEND_CFG1_REG_ADDR,           0x0,   0x3f},

   // ANT_DIV_RX_TERM_CFG   - 000 (Antenna diversity and termination based on
   //                              CS/PQT are disabled)
   // TERM_ON_BAD_PACKET_EN - 0   (Terminate on bad packet disabled)
   // TXOFF_MODE            - 11  (RX - mode entered after packet transmission)
   // CAL_END_WAKE_UP_EN    - 0
   // Not used              - 0
   {CC1200_RFEND_CFG0_REG_ADDR,           0x0,   0x30},

   {CC1200_PA_CFG1_REG_ADDR,              0x0,   0x7f},
   {CC1200_PA_CFG0_REG_ADDR,              0x0,   0x56},

   // {CC1200_ASK_CFG_REG_ADDR,              0x0,   0x0f},

   {CC1200_PKT_LEN_REG_ADDR,              0x0,   CC1200_MAX_MAC_PDU_LEN},

   /*
    * CC120X supports Low-IF and Zero-IF receiver architecture, which is set by the
    * IF_MIX_CFG.CMIX_CFG register field. For more information see section 9.12.
    * For best performance, use Low-IF receiver architecture when possible.
    */
   {CC1200_IF_MIX_CFG_EXT_REG_ADDR,       0x1,   0x1c},

   // FOC_EN - 0 (Frequency offset correction enabled)
   {CC1200_FREQOFF_CFG_EXT_REG_ADDR,      0x1,   0x22},

   //{CC1200_TOC_CFG_EXT_REG_ADDR,          0x1,   0x0b},
   //{CC1200_MARC_SPARE_EXT_REG_ADDR,       0x1,   0x00},

   //{CC1200_ECG_CFG_EXT_REG_ADDR,          0x1,   0x00},
   {CC1200_MDMCFG2_EXT_REG_ADDR,          0x1,   0x0c},

   //{CC1200_EXT_CTRL_EXT_REG_ADDR,         0x1,   0x01},

   //{CC1200_RCCAL_FINE_EXT_REG_ADDR,       0x1,   0x00},
   //{CC1200_RCCAL_COARSE_EXT_REG_ADDR,     0x1,   0x00},
   //{CC1200_RCCAL_OFFSET_EXT_REG_ADDR,     0x1,   0x00},


   /*
    * Frequency offset registers. Updated by user or SAFC strobe. The value is in
    * two's complement format.
    * SAFC strobe - Automatic frequency compensation.
    */
   //{CC1200_FREQOFF1_EXT_REG_ADDR,         0x1,   0x00},
   //{CC1200_FREQOFF0_EXT_REG_ADDR,         0x1,   0x00},

   //{CC1200_IF_ADC2_EXT_REG_ADDR,          0x1,   0x02},
   {CC1200_IF_ADC1_EXT_REG_ADDR,          0x1,   0xee},
   {CC1200_IF_ADC0_EXT_REG_ADDR,          0x1,   0x10},

   {CC1200_FS_DIG1_EXT_REG_ADDR,          0x1,   0x07},
   {CC1200_FS_DIG0_EXT_REG_ADDR,          0x1,   0xaf},

   //{CC1200_FS_CAL3_EXT_REG_ADDR,          0x1,   0x00},
   //{CC1200_FS_CAL2_EXT_REG_ADDR,          0x1,   0x20},
   {CC1200_FS_CAL1_EXT_REG_ADDR,          0x1,   0x40},
   {CC1200_FS_CAL0_EXT_REG_ADDR,          0x1,   0x0e},

   // {CC1200_FS_CHP_EXT_REG_ADDR,           0x1,   0x28},

   {CC1200_FS_DIVTWO_EXT_REG_ADDR,        0x1,   0x03},

   //{CC1200_FS_DSM1_EXT_REG_ADDR,          0x1,   0x00},
   {CC1200_FS_DSM0_EXT_REG_ADDR,          0x1,   0x33},

   //{CC1200_FS_DVC1_EXT_REG_ADDR,          0x1,   0xff},
   {CC1200_FS_DVC0_EXT_REG_ADDR,          0x1,   0x17},

   //{CC1200_FS_LBI_EXT_REG_ADDR,           0x1,   0x00},
   {CC1200_FS_PFD_EXT_REG_ADDR,           0x1,   0x00},
   {CC1200_FS_PRE_EXT_REG_ADDR,           0x1,   0x6e},
   {CC1200_FS_REG_DIV_CML_EXT_REG_ADDR,   0x1,   0x1c},
   {CC1200_FS_SPARE_EXT_REG_ADDR,         0x1,   0xac},

   //{CC1200_FS_VCO4_EXT_REG_ADDR,          0x1,   0x14},
   //{CC1200_FS_VCO3_EXT_REG_ADDR,          0x1,   0x00},
   //{CC1200_FS_VCO2_EXT_REG_ADDR,          0x1,   0x00},
   //{CC1200_FS_VCO1_EXT_REG_ADDR,          0x1,   0x00},
   {CC1200_FS_VCO0_EXT_REG_ADDR,          0x1,   0xb5},

   //{CC1200_IFAMP_EXT_REG_ADDR,            0x1,   0x01},
   //{CC1200_LNA_EXT_REG_ADDR,              0x1,   0x01},
   //{CC1200_RXMIX_EXT_REG_ADDR,            0x1,   0x01},

   {CC1200_XOSC5_EXT_REG_ADDR,            0x1,   0x0e},
   //{CC1200_XOSC4_EXT_REG_ADDR,            0x1,   0xa0},
   //{CC1200_XOSC3_EXT_REG_ADDR,            0x1,   0x03},
   //{CC1200_XOSC2_EXT_REG_ADDR,            0x1,   0x04},
   {CC1200_XOSC1_EXT_REG_ADDR,            0x1,   0x03},
   // {CC1200_XOSC0_EXT_REG_ADDR,         0x1,   0x00},

   //{CC1200_ANALOG_SPARE_EXT_REG_ADDR,     0x1,   0x00},
   //{CC1200_PA_CFG3_EXT_REG_ADDR,          0x1,   0x00},

   //{CC1200_AGC_GAIN2_EXT_REG_ADDR,        0x1,   0xd1},
   //{CC1200_AGC_GAIN1_EXT_REG_ADDR,        0x1,   0x00},
   //{CC1200_AGC_GAIN0_EXT_REG_ADDR,        0x1,   0x3f},

   //{CC1200_CFM_TX_DATA_IN_EXT_REG_ADDR,   0x1,   0x00},

   {0xff,                                 0x0,   0x00}     // Last entry
 };
#elif (CC1200_CFG_SYM_RATE_SPS == 38400)
const CC1200_chipRegInfo_s CC1200_chipRegInfoList[  ] =
{
   /*
    * SYNC_MODE - 101 (32 bits)
    * SYNC_THR  - 1001
    */
   {CC1200_SYNC_CFG1_REG_ADDR,            0x0,   0xa9},


   /*
    * AUTO CLEAR        - 0
    * RX_CONFIG_LIMITATION  - 0
    * PQT_GATING_EN     - 0  (disabled)
    * EXT_SYNC_DETECT   - 0  (disabled)
    * STRICT_SYNC_CHECK - 11 (strict sync word check disabled)
    */
   {CC1200_SYNC_CFG0_REG_ADDR,            0x0,   0x03},

   {CC1200_DEVIATION_M_REG_ADDR,          0x0,   0x06},

   // MODEM_MODE - 00 (Normal mode)
   // MOD_FORMAT - 001 (2-GFSK)
   // DEV_E      - 003 (Freq deviation exponent part)
   {CC1200_MODCFG_DEV_E_REG_ADDR,         0x0,   0x0B},

   {CC1200_DCFILT_CFG_REG_ADDR,           0x0,   0x4c},

   /*
    * NUM_PREAMBLE - 101 (3 bytes)
    * PREAMBLE_WORD - 00 (10101010 (0xAA))
    */
   {CC1200_PREAMBLE_CFG1_REG_ADDR,        0x0,   0x14},

   /*
    * PQT_EN - 1 (Preamble detection enabled)
    * PQT_VALID_TIMEOUT - 000 (11)
    * PQT - 1010 (Soft decision PQT)
    */
   {CC1200_PREAMBLE_CFG0_REG_ADDR,        0x0,   0x8a},

   {CC1200_IQIC_REG_ADDR,                 0x0,   0xc8},
   {CC1200_CHAN_BW_REG_ADDR,              0x0,   0x10},
   {CC1200_MDMCFG1_REG_ADDR,              0x0,   0x42},
   {CC1200_MDMCFG0_REG_ADDR,              0x0,   0x05},

   /*
    *
    * Rsym = (((2^20) + SRATE_M) * (2^SRATE_E) * Fosc) / (2^39)
    * where Fosc = 40 * (10^6)
    * Here symbol rate is 38400 sps.
    * 0x8f/0x75/0x10 -> exponent is 8 (SRATE_E)
    *                   and mantissa is 0xf7510 -> 1013008 (SRATE_M))
    */
   {CC1200_SYMBOL_RATE2_REG_ADDR,         0x0,   0x8f},
   {CC1200_SYMBOL_RATE1_REG_ADDR,         0x0,   0x75},
   {CC1200_SYMBOL_RATE0_REG_ADDR,         0x0,   0x10},

   {CC1200_AGC_REF_REG_ADDR,              0x0,   0x27},
   {CC1200_AGC_CS_THR_REG_ADDR,           0x0,   0xee},
   {CC1200_AGC_GAIN_ADJUST_REG_ADDR,      0x0,   0x00},
   {CC1200_AGC_CFG3_REG_ADDR,             0x0,   0xb1},
   {CC1200_AGC_CFG2_REG_ADDR,             0x0,   0x20},
   {CC1200_AGC_CFG1_REG_ADDR,             0x0,   0x11},
   {CC1200_AGC_CFG0_REG_ADDR,             0x0,   0x94},

   // CRC_AUTOFLUSH - 1 (Auto flush last packet with bad CRC)
   // FIFO_THR - 126 (TXFIFO_THR signal will be de-asserted
   //                 when TX FIFO is drained below
   //                 (127 - FIFO_CFG.FIFO_THR) bytes. Setting
   //                 FIFO_THR to 126 means TXFIFO_THR signal
   //                 will get de-asserted when TX FIFO drains
   //                 below 1 byte (i.e TX FIFO becomes empty)).
   {CC1200_FIFO_CFG_REG_ADDR,             0x0,   0xfe},

   // {CC1200_DEV_ADDR_REG_ADDR,          0x0,   0x00},

   {CC1200_SETTLING_CFG_REG_ADDR,         0x0,   0x0b},

   // FS_LOCK_EN - 1 (Out of lock detector enabled)
   // FSD_BANDSELECT - 2 (820 MHz - 960 MHz)
   {CC1200_FS_CFG_REG_ADDR,               0x0,   0x12},

   // CCA_MODE - 011 (rssi below threshold and not receiving packet)
   // PKT_FORMAT - 00 (Normal/FIFO)
   {CC1200_PKT_CFG2_REG_ADDR,             0x0,   0x0c},

   // FEC_EN - 0 (disabled)
   // WHITE_DATA - 0 (disabled)
   // PN9_SWAP_EN - 0 (disabled)
   // ADDR_CHECK_CFG - 00 (disabled)
   // CRC CFG - 01 (CRC enabled and initialized to 0xffff)
   // APPEND_STATUS - 1
   {CC1200_PKT_CFG1_REG_ADDR,             0x0,   0x03},

   // LENGTH_CONFIG 01 (Variable, length field included in packet)
   // UART_MODE_LEN 0 (UART mode disabled)
   // UART_SWAP_EN 0 (disabled)
   {CC1200_PKT_CFG0_REG_ADDR,             0x0,   0x20},

   // RXOFF_MODE   - 11 (RX - mode entered after receiving a good packet)
   // RX_TIME      - 111
   // RX_TIME_QUAL - 1
   {CC1200_RFEND_CFG1_REG_ADDR,           0x0,   0x3f},

   // ANT_DIV_RX_TERM_CFG   - 000 (Antenna diversity and termination based on
   //                              CS/PQT are disabled)
   // TERM_ON_BAD_PACKET_EN - 0   (Terminate on bad packet disabled)
   // TXOFF_MODE            - 11  (RX - mode entered after packet transmission)
   // CAL_END_WAKE_UP_EN    - 0
   {CC1200_RFEND_CFG0_REG_ADDR,           0x0,   0x30},

   /*
    * When using the auto flush function, the maximum packet length is 127
    * bytes in variable packet length mode and 128 bytes in fixed packet
    * length mode. Note that when PKT_CFG1.APPEND_STATUS is enabled, the
    * maximum allowed packet length is reduced by two bytes in order to make
    * room in the RX FIFO for the two status bytes appended at the end of the
    * packet. The MCU must not read from the current packet until the CRC has
    * been checked as OK.
    */

   {CC1200_PKT_LEN_REG_ADDR,              0x0,   CC1200_MAX_MAC_PDU_LEN},

   {CC1200_IF_MIX_CFG_EXT_REG_ADDR,       0x1,   0x1c},
   {CC1200_FREQOFF_CFG_EXT_REG_ADDR,      0x1,   0x20},

   {CC1200_TOC_CFG_EXT_REG_ADDR,          0x1,   0x03},

   {CC1200_MDMCFG2_EXT_REG_ADDR,          0x1,   0x02},

   {CC1200_IF_ADC2_EXT_REG_ADDR,          0x1,   0x02},
   {CC1200_IF_ADC1_EXT_REG_ADDR,          0x1,   0xee},
   {CC1200_IF_ADC0_EXT_REG_ADDR,          0x1,   0x10},

   {CC1200_FS_DIG1_EXT_REG_ADDR,          0x1,   0x07},
   {CC1200_FS_DIG0_EXT_REG_ADDR,          0x1,   0xaf},

   {CC1200_FS_CAL3_EXT_REG_ADDR,          0x1,   0x00},
   {CC1200_FS_CAL2_EXT_REG_ADDR,          0x1,   0x20},
   {CC1200_FS_CAL1_EXT_REG_ADDR,          0x1,   0x40},
   {CC1200_FS_CAL0_EXT_REG_ADDR,          0x1,   0x0e},
   {CC1200_FS_CHP_EXT_REG_ADDR,           0x1,   0x28},
   {CC1200_FS_DIVTWO_EXT_REG_ADDR,        0x1,   0x03},

   {CC1200_FS_DSM1_EXT_REG_ADDR,          0x1,   0x00},
   {CC1200_FS_DSM0_EXT_REG_ADDR,          0x1,   0x33},

   {CC1200_FS_DVC1_EXT_REG_ADDR,          0x1,   0xff},
   {CC1200_FS_DVC0_EXT_REG_ADDR,          0x1,   0x17},

   {CC1200_FS_LBI_EXT_REG_ADDR,           0x1,   0x00},
   {CC1200_FS_PFD_EXT_REG_ADDR,           0x1,   0x00},
   {CC1200_FS_PRE_EXT_REG_ADDR,           0x1,   0x6e},
   {CC1200_FS_REG_DIV_CML_EXT_REG_ADDR,   0x1,   0x1c},
   {CC1200_FS_SPARE_EXT_REG_ADDR,         0x1,   0xac},
   {CC1200_FS_VCO0_EXT_REG_ADDR,          0x1,   0xb5},
   {CC1200_IFAMP_EXT_REG_ADDR,            0x1,   0x09},
   {CC1200_XOSC5_EXT_REG_ADDR,            0x1,   0x0e},
   {CC1200_XOSC1_EXT_REG_ADDR,            0x1,   0x03},
   {CC1200_XOSC0_EXT_REG_ADDR,            0x1,   0x00},



   {0xff,                                 0x0,   0x00}     // Last entry
 };
#else
#error PHY symbol rate not supported !!
#endif

UINT16_t CC1200_865_MHZ_channMap[PHY_865_MHZ_BAND_CHANNEL_CNT] = {866, 868};


const CC1200_state_t CC1200_FSM_STATE_MAPPING[ ] =
{
   CC1200_STATE_IDLE,
   CC1200_STATE_RX,
   CC1200_STATE_TX_IN_PROGRESS,
   CC1200_STATE_FSTXON,
   CC1200_STATE_CALIBRATE,
   CC1200_STATE_SETTLING,
   CC1200_STATE_TX_FIFO_ERR,
   CC1200_STATE_RX_FIFO_ERR
};


/*
 * Section 9.5 - FS (Frequency Synthesizer)
 * The internal on-chip FS characteristics will vary with temperature and
 * supply voltage changes as well as the desired operating frequency. In
 * order to ensure reliable operation, CC120X includes frequency synthesizer
 * self-calibration circuitry. This calibration should be done regularly, and
 * ***MUST*** be performed after turning on power and before using a new radio
 * frequency.
 */


const UINT8_t CC1200_bandSelectEnc[CC1200_FREQ_BAND_CNT] =
{
   0x2,   // 820 - 960  MHz
   0x4,   // 410 - 480  MHz
   0x6,   // 273.3 - 320  MHz
   0x8,   // 205 - 240  MHz
   0xa,   // 164 - 192  MHz
   0xb    // 136.7 - 160  MHz
};


const UINT8_t CC1200_syncModeBitCntList[ ] =
{
   0,
   11,
   16,
   18,
   24,
   32,
   16,
   16
};



const CC1200_preambleLenEncListEntry_s CC1200_preambleLengthEncList[CC1200_PREAMBLE_LENGTH_ENC_LIST_SZ] =
{
    {0,   0x0},  {4,   0x1},  {8,   0x2},  {12,  0x3},
    {16,  0x4},  {24,  0x5},  {32,  0x6},  {40,  0x7},
    {48,  0x8},  {56,  0x9},  {64,  0xa},  {96,  0xb},
    {192, 0xc},  {240, 0xd}
};


#define CC1200_hwResetEna() \
do                          \
{                           \
    P4OUT &= ~(BIT1);       \
} while (0)


#define CC1200_hwResetDis() \
do                          \
{                           \
    P4OUT |= (BIT1);        \
} while (0)



#ifdef ECLIPSE_IDE
static void __attribute__((__interrupt__(PORT1_VECTOR))) Port_1 (void)
#else
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
#endif
{
    UINT8_t reg = P1IFG;

    // P1.1 <- GPIO_2
    // TXONCCA_DONE
    if (reg & BIT1)
    {
#ifdef CC1200_TIMING_TEST
        P1OUT &= ~BIT5;
#endif
        P1IE &= ~BIT1;
        P1IFG &= ~BIT1;
#ifdef CC1200_INTR_STATS_ENA
        CC1200_cntxt.CC1200_txOnCCADoneIntCnt ++;
#endif
        CC1200_cntxt.txOnCCADoneFlag = 1;
    }

    // P1.0 <- GPIO_0
    // PKT_CRC_OK
    if (reg & BIT0)
    {
        // P1IE &= ~BIT0;
        P1IFG &= ~BIT0;
#ifdef CC1200_INTR_STATS_ENA
        CC1200_cntxt.CC1200_pktCrcOkIntCnt ++;
#endif

        if (CC1200_cntxt.state == CC1200_STATE_TX_IN_PROGRESS)
        {
#ifdef CC1200_TIMING_TEST
            P1OUT &= ~BIT6;
#endif
            SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_FRAME_TX_DONE;
            // Change the interrupt edge to active high so that
            // we get an interrupt when a packet with valid CRC is
            // received
            CC1200_setUpPktRxInt();
        }
        else
            SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_FRAME_RX_DONE;
    }

    if (SYS_globalEvtMsk0 != 0x0)
    {
        PLTFRM_exitLPM();
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
PLTFRM_sts_t CC1200_cfgGPIO(const UINT8_t regAddr,
                            const UINT8_t sigNr)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;

   sts = CC1200_writeReg(regAddr, sigNr);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       sts = CC1200_readReg(regAddr, &regVal);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           if (regVal != sigNr)
               sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;
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
void CC1200_gpioInit(void)
{
   PLTFRM_sts_t sts;

   // Disable interrupts on all pins connected to cc1200 GPIO pins
   P1IE &= ~(BIT0 | BIT1 | BIT2);

   /*
    * The four digital I/O pins GPIO0, GPIO1, GPIO2 and GPIO3 are general
    * control pins configured with IOCFGx.GPIOx_CFG (where x is 0, 1, 2,
    * or 3).
    *
    * GPIO1 is shared with the SO pin in the SPI interface. In this
    * implementation, GPIO1 will not be used since the SPI interface will be
    * shared with other devices (sensors).
    *
    * When the IOCFGx.GPIOx_CFG setting is less than 0x30 and IOCFGx.GPIOx_INV
    * is 0 (1) the GPIO0 and GPIO2 pin will be hard wired to 0 (1), and GPIO1
    * and GPIO3 will be hard wired to 1 (0) in the SLEEP state. These signals will
    * be hard wired until the CHIP_RDYn signal goes low. If the IOCFGx.GPIOx_CFG
    * setting is 0x30 or higher, the GPIO pins will work as programmed also in
    * SLEEP state. The GPIOs can also be used as inputs by setting IOCFGx.GPIOx_CFG
    * = HIGHZ (48). Table 9 shows which signals can be input to the CC112X.
    *
    *
    * <TXFIFO_THR> - Asserted when the TX FIFO is filled above (or is equal to)
    * (127 - FIFO_CFG.FIFO_THR). De-asserted when the TX FIFO is drained below the
    * same threshold.
    *
    * <PKT_CRC_OK> - Asserted in RX when PKT_CFG1.CRC_CFG = 1 or 10b and a good
    * packet is received. The signal is de-asserted when RX mode is entered and
    * PKT_CFG1.CRC_CFG != 0. This signal is always on if the radio is in TX or
    * if the radio is in RX and PKT_CFG1.CRC_CFG = 0.
    *
    * <TXONCCA_DONE> - A pulse occurring when a decision has been made as to whether
    * the channel is busy or not. This signal must be used as an interrupt to the
    * MCU. When this signal is asserted/de-asserted, TXONCCA_FAILED can be checked.
    * Only available on GPIO 2.
    */

   // GPIO 0 (PKT_CRC_OK available on any GPIO)
   sts = CC1200_cfgGPIO(CC1200_IOCFG0_REG_ADDR,
                        CC1200_GPIOx_CFG_SIG_PKT_CRC_OK);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);

   // GPIO 2 (TXONCCA_DONE only available on GPIO_2)
   sts = CC1200_cfgGPIO(CC1200_IOCFG2_REG_ADDR,
                        CC1200_GPIOx_CFG_SIG_TXONCCA_DONE);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);


   // Globally interrupts are still disabled ...

   sts = PLTFRM_reserveGPIOPin(PLTFRM_CC1200_1_DEV_ID,
                               PLTFRM_GPIO_PORT_1,
                               PLTFRM_GPIO_PORT_PIN_0);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);

   // P1.0 <- GPIO_0
   P1DIR &= ~(BIT0);  // Input direction
   P1SEL  &= ~(BIT0); // I/O functionality
   P1SEL2 &= ~(BIT0);
   P1REN &= ~(BIT0);  // Pull up resistor disabled
   P1IFG &= ~(BIT0);  // clear interrupt flag
   P1IES &= ~(BIT0);  // Active high
   P1IE |= BIT0;      // Interrupt enable


   sts = PLTFRM_reserveGPIOPin(PLTFRM_CC1200_1_DEV_ID,
                               PLTFRM_GPIO_PORT_1,
                               PLTFRM_GPIO_PORT_PIN_1);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);

   // P1.1 <- GPIO_2
   P1DIR &= ~(BIT1);  // Input direction
   P1SEL  &= ~(BIT1); // I/O functionality
   P1SEL2 &= ~(BIT1);
   P1REN &= ~(BIT1);  // Pull up resistor disabled
   P1IE &= ~(BIT1);   // ***** Interrupt disabled ******
   P1IES &= ~(BIT1);  // Active high
   P1IFG &= ~(BIT1);  // clear interrupt flag

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
PLTFRM_sts_t CC1200_spiBegin(void)
{
  PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

  /*
   * When CSn is pulled low, the MCU must wait until CC120X SO pin goes
   * low before starting to transfer the header byte. This indicates that
   * the crystal is stable. Unless the chip was just reset or was in SLEEP
   * or XOFF state, or the XOSC configuration has been altered, the SO pin
   * will always go low immediately after pulling CSn low.
   */

  P4OUT &= ~(BIT0);

  // Loop while the SO pin is high
  while (P3IN & BIT2);

  return sts;
}

#define CC1200_spiEnd( )  \
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
UINT8_t CC1200_spiTxRx(UINT8_t val)
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
PLTFRM_sts_t CC1200_writeReg(UINT8_t regAddr, UINT8_t regVal)
{
   PLTFRM_sts_t sts;
   UINT8_t hdrByte = 0x0;
   
   hdrByte |= regAddr;

   sts = CC1200_spiBegin();
   if (sts != PLTFRM_STS_SUCCESS)
       return sts; 
   
   /*
    * All transfers on the SPI interface are done most significant bit 
    * first.  All transactions on the SPI interface start with a header 
    * byte containing a R/WÂ¯ bit, a burst access bit (B), and a 6-bit 
    * address (A5 - A0).
    */

   /*
    * CSn low to positive edge on SCLK - 50 nanosecs minimum
    */  
   PLTFRM_delay1MicroSecs();
   
   CC1200_spiTxRx(hdrByte);

   PLTFRM_delay1MicroSecs();

   CC1200_spiTxRx(regVal);

   // Tns- Negative edge on SCLK to CSn high - minimum 200 nanosecs
   PLTFRM_delay1MicroSecs();

   CC1200_spiEnd();
  
   // CSn high time, time from CSn has been pulled high until it can be pulled 
   // low again  - 50 nanosecs minimum
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
PLTFRM_sts_t CC1200_sendCmdStrobe(UINT8_t cmdStrobeId, CC1200_state_t *fsmState_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t stsByte, hdrByte = 0x0;

   /*
    * Command Strobes may be viewed as single byte instructions to CC120X. By 
    * addressing a command strobe register, internal sequences will be started. 
    * These commands are used to enable receive and transmit mode, enter SLEEP 
    * mode, disable the crystal oscillator, etc. The command strobes are listed 
    * in Table 6. The command strobe registers are accessed by transferring a 
    * single header byte (no data is being transferred). That is, only the R/W 
    * bit, the burst access bit (set to 0), and the six address bits (in the 
    * range 0x30 through 0x3D) are written. When sending a strobe, the R/W bit 
    * can be either one or zero. The status byte is available on the SO pin when 
    * a command strobe is being sent.
    */


   if (cmdStrobeId < CC1200_CMD_STROBE_SRES
       || cmdStrobeId > CC1200_CMD_STROBE_SNOP)
   {
       SYS_fatal(SYS_FATAL_ERR_1);
       return PLTFRM_STS_INV_PARAM;

   }

   hdrByte = cmdStrobeId;

   sts = CC1200_spiBegin();
   if (sts != PLTFRM_STS_SUCCESS)
       return sts; 

   /*
    * CSn low to positive edge on SCLK - 50 nanosecs minimum
    */  
   PLTFRM_delay1MicroSecs();


   // The status byte is available on the SO pin when a command strobe is being sent.
   // S6, S5, and S4 comprise the STATE value which reflects the state of the chip.

#ifdef CC1200_TIMING_TEST
   if (cmdStrobeId == CC1200_CMD_STROBE_STX)
   {
       P1OUT |= (BIT5 | BIT6);
   }
#endif

   stsByte = CC1200_spiTxRx(hdrByte);

   // Tns- Negative edge on SCLK to CSn high - minimum 200 nanosecs
   PLTFRM_delay1MicroSecs();

   CC1200_spiEnd();

   if (fsmState_p != NULL)
   {
       stsByte >>= CC1200_STS_BYTE_CHIP_STATE_SHIFT;
       stsByte &= CC1200_STS_BYTE_CHIP_STATE_BIT_MSK_SHIFT;
       *fsmState_p = CC1200_FSM_STATE_MAPPING[stsByte];
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
PLTFRM_sts_t CC1200_getCurrState(CC1200_state_t *state_p)
{
   return CC1200_sendCmdStrobe(CC1200_CMD_STROBE_SNOP, state_p);
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
PLTFRM_sts_t CC1200_forceIdle(void)
{
   CC1200_state_t state;
   PLTFRM_sts_t sts;

   // The SIDLE command strobe can always be used to force the radio
   // controller to go to the IDLE state.

   sts = CC1200_sendCmdStrobe(CC1200_CMD_STROBE_SIDLE, NULL);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   do
   {
       sts = CC1200_getCurrState(&state);
       if (sts != PLTFRM_STS_SUCCESS)
           break;
       PLTFRM_delay100MicroSecs();
   } while (state != CC1200_STATE_IDLE);

   if (sts == PLTFRM_STS_SUCCESS)
       CC1200_setCurrState(CC1200_STATE_IDLE);

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
PLTFRM_sts_t CC1200_flushTxFIFO(void)
{
   /*
    * Flush the TX FIFO. Only issue in IDLE or TX_FIFO_ERR states 
    */
   return CC1200_sendCmdStrobe(CC1200_CMD_STROBE_SFTX, NULL);
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
PLTFRM_sts_t CC1200_flushRxFIFO(void)
{
   /*
    * Flush the RX FIFO. Only issue in IDLE or RX_FIFO_ERR states 
    */
   return CC1200_sendCmdStrobe(CC1200_CMD_STROBE_SFRX, NULL);
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
PLTFRM_sts_t CC1200_readExtAddrSpaceReg(UINT8_t regAddr, UINT8_t *regVal_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t hdrByte = CC1200_SPI_HDR_RW_BIT;
   
   /*
    * <Single Register Access (extended register space)>
    * Command: R/W  0  1  0  1  1  1  1
    * Address:  A7 A6 A5 A4 A3 A2 A1 A0
    * 
    * This access mode starts with a specific command (0x2F). The first byte 
    * following this command is interpreted as the extended address. Exactly 
    * one data byte is expected after the extended address byte. When the 
    * extended address is sent on the SI line, SO will return all zeros. The 
    * chip status byte is returned on the SO line when the command is transmitted 
    * as well as when data are written to the extended address.
    */ 

   hdrByte |= CC1200_EXT_REG_SPACE_ACCESS_CMD;

   sts = CC1200_spiBegin();
   if (sts != PLTFRM_STS_SUCCESS)
       return sts; 

   /*
    * CSn low to positive edge on SCLK - 50 nanosecs minimum
    */  
   PLTFRM_delay1MicroSecs();

   CC1200_spiTxRx(hdrByte);   // Send 0x2f
   
   PLTFRM_delay1MicroSecs();
   
   CC1200_spiTxRx(regAddr);   // Send the specified extended address
   
   PLTFRM_delay1MicroSecs();
   
   *(regVal_p) = CC1200_spiTxRx(0x0);  // Read 1 byte

   // Tns- Negative edge on SCLK to CSn high - minimum 200 nanosecs
   PLTFRM_delay1MicroSecs();

   CC1200_spiEnd();

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
PLTFRM_sts_t CC1200_writeExtAddrSpaceReg(UINT8_t regAddr, UINT8_t regVal)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t hdrByte = 0;

   /*
    * <Single Register Access (extended register space)>
    * Command: R/W  0  1  0  1  1  1  1
    * Address:  A7 A6 A5 A4 A3 A2 A1 A0
    *
    * This access mode starts with a specific command (0x2F). The first byte
    * following this command is interpreted as the extended address. Exactly
    * one data byte is expected after the extended address byte. When the
    * extended address is sent on the SI line, SO will return all zeros. The
    * chip status byte is returned on the SO line when the command is transmitted
    * as well as when data are written to the extended address.
    */

   hdrByte |= CC1200_EXT_REG_SPACE_ACCESS_CMD;

   sts = CC1200_spiBegin();
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   /*
    * CSn low to positive edge on SCLK - 50 nanosecs minimum
    */
   PLTFRM_delay1MicroSecs();

   CC1200_spiTxRx(hdrByte);   // Send 0x2f

   PLTFRM_delay1MicroSecs();

   CC1200_spiTxRx(regAddr);   // Send the specified extended address

   PLTFRM_delay1MicroSecs();

   CC1200_spiTxRx(regVal);    // Send the value to be written to specified register

   PLTFRM_delay1MicroSecs();

   // Tns- Negative edge on SCLK to CSn high - minimum 200 nanosecs
   PLTFRM_delay1MicroSecs();

   CC1200_spiEnd();

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
PLTFRM_sts_t CC1200_readReg(UINT8_t regAddr, UINT8_t *regVal_p)
{
   PLTFRM_sts_t sts;
   UINT8_t hdrByte;

   hdrByte = CC1200_SPI_HDR_RW_BIT;

   hdrByte |= regAddr;

   sts = CC1200_spiBegin();
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   /*
    * All transfers on the SPI interface are done most significant bit
    * first.  All transactions on the SPI interface start with a header
    * byte containing a R/WÂ¯ bit, a burst access bit (B), and a 6-bit
    * address (A5 - A0).
    */

   /*
    * CSn low to positive edge on SCLK - 50 nanosecs minimum
    */
   PLTFRM_delay1MicroSecs();

   CC1200_spiTxRx(hdrByte);

   PLTFRM_delay1MicroSecs();

   *(regVal_p) = CC1200_spiTxRx(0x0);

   // Tns- Negative edge on SCLK to CSn high - minimum 200 nanosecs
   PLTFRM_delay1MicroSecs();

   CC1200_spiEnd();

   // CSn high time, time from CSn has been pulled high until it can be pulled
   // low again  - 50 nanosecs minimum
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
PLTFRM_sts_t CC1200_writeByteToTxFIFO(UINT8_t byte)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t hdrByte = 0x0;


   /*
    * 3.2.4 Standard FIFO Access
    * Using the standard FIFO push/pop interface the 128-byte TX FIFO 
    * and the 128-byte RX FIFO are accessed through the 0x3F address. 
    * When the R/W bit is zero the TX FIFO is accessed, and the RX
    * FIFO is accessed when the R/W bit is one. Using this type of
    * access, the TX FIFO is write-only, while the RX FIFO is read-only. 
    * The burst bit is used to determine if the FIFO access is a single  
    * byte access or a burst access. The single byte access method expects 
    * a header byte with the burst bit set to zero and one data byte. After 
    * the data byte, a new header byte is expected; hence CSn can remain low.
    * The burst access method expects one header byte and then consecutive 
    * data bytes until terminating the access by setting CSn high.
    */

   hdrByte |= CC1200_STD_FIFO_ACCESS_CMD;

   sts = CC1200_spiBegin();
   if (sts != PLTFRM_STS_SUCCESS)
       return sts; 

   /*
    * CSn low to positive edge on SCLK - 50 nanosecs minimum
    */  
   PLTFRM_delay1MicroSecs();

   CC1200_spiTxRx(hdrByte);   // Send 0x3f
   
   PLTFRM_delay1MicroSecs();
   
   CC1200_spiTxRx(byte);  // Write 1 byte

   // Tns- Negative edge on SCLK to CSn high - minimum 200 nanosecs
   PLTFRM_delay1MicroSecs();

   CC1200_spiEnd();

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
PLTFRM_sts_t CC1200_getRxFIFONextByte(UINT8_t *buff_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t hdrByte = CC1200_SPI_HDR_RW_BIT;

   /*
    * 3.2.4 Standard FIFO Access
    * Using the standard FIFO push/pop interface the 128-byte TX FIFO 
    * and the 128-byte RX FIFO are accessed through the 0x3F address. 
    * When the R/W bit is zero the TX FIFO is accessed, and the RX
    * FIFO is accessed when the R/WÂ¯ bit is one. Using this type of 
    * access, the TX FIFO is write-only, while the RX FIFO is read-only. 
    * The burst bit is used to determine if the FIFO access is a single  
    * byte access or a burst access. The single byte access method expects 
    * a header byte with the burst bit set to zero and one data byte. After 
    * the data byte, a new header byte is expected; hence CSn can remain low.
    * The burst access method expects one header byte and then consecutive 
    * data bytes until terminating the access by setting CSn high.
    */

   hdrByte |= CC1200_STD_FIFO_ACCESS_CMD;

   sts = CC1200_spiBegin();
   if (sts != PLTFRM_STS_SUCCESS)
       return sts; 

   /*
    * CSn low to positive edge on SCLK - 50 nanosecs minimum
    */  
   PLTFRM_delay1MicroSecs();

   CC1200_spiTxRx(hdrByte);   // Send 0x3f
   
   PLTFRM_delay1MicroSecs();
   
   *(buff_p) = CC1200_spiTxRx(0x0);  // Read 1 byte

   // Tns- Negative edge on SCLK to CSn high - minimum 200 nanosecs
   PLTFRM_delay1MicroSecs();

   CC1200_spiEnd();

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
PLTFRM_sts_t CC1200_getTXFIFOByteCnt(UINT8_t *byteCnt_p)
{
   return CC1200_readExtAddrSpaceReg(CC1200_NUM_TXBYTES_EXT_REG_ADDR,
                                     byteCnt_p);
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
PLTFRM_sts_t CC1200_getRXFIFOByteCnt(UINT8_t *byteCnt_p)
{
   return CC1200_readExtAddrSpaceReg(CC1200_NUM_RXBYTES_EXT_REG_ADDR,
                                     byteCnt_p);
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
PLTFRM_sts_t CC1200_readRxFIFO(UINT8_t cnt, UINT8_t *buff_p)
{
   PLTFRM_sts_t sts;
   SINT16_t idx;

   for (idx=0; idx<cnt; idx++)
   {
       sts = CC1200_getRxFIFONextByte(buff_p);
       if (sts != PLTFRM_STS_SUCCESS)
           break;
       buff_p ++;
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
PLTFRM_sts_t CC1200_writeBuffToTxFIFO(UINT8_t *buff_p, UINT8_t buffLen)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   SINT16_t idx;

   if (CC1200_cntxt.pktLenMode == CC1200_VARIABLE_PACKET_LENGTH_MODE_0)
   {
       UINT8_t byteCnt0, byteCnt1;
       
       if (buffLen > (CC1200_TX_FIFO_LEN - CC1200_FRAME_LENGTH_FIELD_LEN))
       {
           SYS_fatal(SYS_FATAL_ERR_1);
           return PLTFRM_STS_FRAME_TOO_LONG_FOR_TX_FIFO;
       }

       sts = CC1200_getTXFIFOByteCnt(&byteCnt0);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_1);

       if (byteCnt0 > 0)
       {
           // UINT8_t txLastRegVal, txFirstRegVal;

           // TX FIFO is not empty !! Need to empty the FIFO
           // Cannot use the SFTX strobe since it can be used only in IDLE and
           // TX_FIFO_ERR states !!
#if 0
           CC1200_readExtAddrSpaceReg(CC1200_TXLAST_EXT_REG_ADDR, &txLastRegVal);
           CC1200_readExtAddrSpaceReg(CC1200_TXFIRST_EXT_REG_ADDR, &txFirstRegVal);
#endif
           sts = CC1200_writeExtAddrSpaceReg(CC1200_TXFIRST_EXT_REG_ADDR, 0x0);
           if (sts != PLTFRM_STS_SUCCESS)
               SYS_fatal(SYS_FATAL_ERR_1);

           sts = CC1200_writeExtAddrSpaceReg(CC1200_TXLAST_EXT_REG_ADDR, 0x0);
           if (sts != PLTFRM_STS_SUCCESS)
               SYS_fatal(SYS_FATAL_ERR_1);

           sts = CC1200_getTXFIFOByteCnt(&byteCnt0);
           if (sts != PLTFRM_STS_SUCCESS)
               SYS_fatal(SYS_FATAL_ERR_1);

           if (byteCnt0 != 0)
              SYS_fatal(SYS_FATAL_ERR_1);
       }

       if (buffLen > (CC1200_TX_FIFO_LEN - byteCnt0 - CC1200_FRAME_LENGTH_FIELD_LEN))
       {
           SYS_fatal(SYS_FATAL_ERR_1);
           return PLTFRM_STS_NOT_ENOUGH_SPACE_TX_FIFO;
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

       sts = CC1200_writeByteToTxFIFO(buffLen);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_1);

        // Now write the actual payload
       for (idx=0; idx<buffLen; idx++)
       {
            sts = CC1200_writeByteToTxFIFO(*(buff_p + idx));
            if (sts !=  PLTFRM_STS_SUCCESS)
                SYS_fatal(SYS_FATAL_ERR_1);
       }

       sts = CC1200_getTXFIFOByteCnt(&byteCnt1);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_1);

       if ((byteCnt1 - byteCnt0) != (buffLen + 1))
           SYS_fatal(SYS_FATAL_ERR_1);
   }
   else
   {
       // Other modes not supported yet !!
       SYS_fatal(SYS_FATAL_ERR_1);
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
PLTFRM_sts_t CC1200_transferPktToTxFifo(UINT8_t bdHndl)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   if (!(SYS_BD_HNDL_IS_VALID(bdHndl)))
   {
       SYS_fatal(SYS_FATAL_ERR_1);
   }

   if (CC1200_cntxt.pktLenMode == CC1200_VARIABLE_PACKET_LENGTH_MODE_0)
   {
       UINT8_t pyldLen = SYS_GET_BD_LEN(bdHndl), byteCnt0, byteCnt1;
       UINT8_t *buff_p;
       SINT16_t idx;

       if (pyldLen > (CC1200_TX_FIFO_LEN - CC1200_FRAME_LENGTH_FIELD_LEN))
       {
           SYS_fatal(SYS_FATAL_ERR_1);
           return PLTFRM_STS_FRAME_TOO_LONG_FOR_TX_FIFO;
       }

       sts = CC1200_getTXFIFOByteCnt(&byteCnt0);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_1);

       if (byteCnt0 > CC1200_TX_FIFO_LEN)
           SYS_fatal(SYS_FATAL_ERR_1);

       if (byteCnt0 > 0)
       {
           // UINT8_t txLastRegVal, txFirstRegVal;

           // TX FIFO is not empty !! Need to empty the FIFO
           // Cannot use the SFTX strobe since it can be used only in IDLE and
           // TX_FIFO_ERR states !!
#if 0
           CC1200_readExtAddrSpaceReg(CC1200_TXLAST_EXT_REG_ADDR, &txLastRegVal);
           CC1200_readExtAddrSpaceReg(CC1200_TXFIRST_EXT_REG_ADDR, &txFirstRegVal);
#endif
           sts = CC1200_writeExtAddrSpaceReg(CC1200_TXFIRST_EXT_REG_ADDR, 0x0);
           if (sts != PLTFRM_STS_SUCCESS)
               SYS_fatal(SYS_FATAL_ERR_1);

           sts = CC1200_writeExtAddrSpaceReg(CC1200_TXLAST_EXT_REG_ADDR, 0x0);
           if (sts != PLTFRM_STS_SUCCESS)
               SYS_fatal(SYS_FATAL_ERR_1);

           sts = CC1200_getTXFIFOByteCnt(&byteCnt0);
           if (sts != PLTFRM_STS_SUCCESS)
               SYS_fatal(SYS_FATAL_ERR_1);

           if (byteCnt0 != 0)
              SYS_fatal(SYS_FATAL_ERR_1);
       }

       if (pyldLen > (CC1200_TX_FIFO_LEN - byteCnt0 - CC1200_FRAME_LENGTH_FIELD_LEN))
       {
           SYS_fatal(SYS_FATAL_ERR_1);
           return PLTFRM_STS_NOT_ENOUGH_SPACE_TX_FIFO;
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

       sts = CC1200_writeByteToTxFIFO(pyldLen);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_1);

       buff_p = SYS_GET_BD_BUFF(bdHndl);

        // Now write the actual payload
       for (idx=0; idx<pyldLen; idx++)
       {
            sts = CC1200_writeByteToTxFIFO(*(buff_p + idx));
            if (sts !=  PLTFRM_STS_SUCCESS)
                SYS_fatal(SYS_FATAL_ERR_1);
       }

       sts = CC1200_getTXFIFOByteCnt(&byteCnt1);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_1);

       if ((byteCnt1 - byteCnt0) != (pyldLen + 1))
           SYS_fatal(SYS_FATAL_ERR_1);
   }
   else
   {
       // Other modes not supported yet !!
       SYS_fatal(SYS_FATAL_ERR_1);
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
PLTFRM_sts_t CC1200_cfgFreqBand(CC1200_freqBand_t freqBand)
{
   UINT8_t regVal;
   PLTFRM_sts_t sts;
   const UINT8_t regAddr = CC1200_FS_CFG_REG_ADDR;

   /*
    * If any frequency programming registers are altered when the
    * frequency synthesizer is running, the synthesizer may give an
    * undesired response. Hence, the frequency programming should only
    * be updated when the radio is in the IDLE state.
    */

   if (freqBand > CC1200_FREQ_BAND_136PT7_160_MHZ)
       return PLTFRM_STS_INV_PARAM;

   /*
    * FDS_BANDSELECT (FS_CFG register)
    *
    * 0010 >  820.0 - 960.0 MHz band (LO divider = 4)
    * 0100 >  410.0 - 480.0 MHz band (LO divider = 8)
    * 0110 >  273.3 - 320.0 MHz band (LO divider = 12)
    * 1000 >  205.0 - 240.0 MHz band (LO divider = 16)
    * 1010 >  164.0 - 192.0 MHz band (LO divider = 20)
    * 1011 >  136.7 - 160.0 MHz band (LO divider = 24)
    */

   sts = CC1200_readReg(regAddr, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   regVal &= CC1200_FS_CFG_BAND_SELECT_BIT_MSK;
   if (regVal == CC1200_bandSelectEnc[freqBand])
       return sts;

   regVal &= ~(CC1200_FS_CFG_BAND_SELECT_BIT_MSK);
   regVal |= CC1200_bandSelectEnc[freqBand];

   sts = CC1200_writeReg(regAddr, regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   regVal = 0x0;
   sts = CC1200_readReg(regAddr, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   regVal &= CC1200_FS_CFG_BAND_SELECT_BIT_MSK;
   if (regVal != CC1200_bandSelectEnc[freqBand])
       sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;

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
PLTFRM_sts_t CC1200_setTxPower(SINT16_t powerInDBm)
{
   UINT8_t regVal;
   PLTFRM_sts_t sts;
   SINT16_t powerEnc;
   const UINT8_t regAddr = CC1200_PA_CFG1_REG_ADDR;
   
   /*
    * For -17.5 dBm, powerInDBm should be -175
    * For +14 dBm, powerInDBm should be 140
    */

   /*
    * The RF output power level from the device is programmed with the 
    * PA_CFG1.PA_POWER_RAMP register field. The power level resolution 
    * is 0.5 dB. 
    *
    * Output-Power = ((PA_POWER_RAMP + 1) / 2) - 18 [dBm]
    * where
    *       Where 3 <= PA_POWER_RAMP <= 64
    *
    * When PA_POWER_RAMP is 0, Output-Power = -17.5 dBm
    * When PA_POWER_RAMP is 63, Output-Power = +14.0 dBm
    */

   if (powerInDBm < CC1200_PA_MIN_OUTPUT_PWR_DBM_10
       && powerInDBm > CC1200_PA_MAX_OUTPUT_PWR_DBM_10)
   {
       return PLTFRM_STS_PARAM_OUT_OF_RANGE;
   }

   powerEnc = powerInDBm;

   powerEnc += 180;
   powerEnc /= 5;
   powerEnc -= 1;

   sts = CC1200_readReg(regAddr, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;
   
   regVal &= CC1200_PA_CFG1_POWER_RAMP_BIT_MSK;
   if (regVal == powerEnc)  
       return sts;

   regVal &= ~(CC1200_PA_CFG1_POWER_RAMP_BIT_MSK);
   regVal |= powerEnc;
   
   sts = CC1200_writeReg(regAddr, regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = CC1200_readReg(regAddr, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   regVal &= CC1200_PA_CFG1_POWER_RAMP_BIT_MSK;
   if (regVal != powerEnc)
       sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;

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
PLTFRM_sts_t CC1200_cfgDevAddr(UINT8_t devAddr)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;
   const UINT8_t regAddr = CC1200_DEV_ADDR_REG_ADDR;

   // Address used for packet filtering in RX

   sts = CC1200_writeReg(regAddr, devAddr);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = CC1200_readReg(regAddr, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   if (regVal != devAddr)
       sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;

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
PLTFRM_sts_t CC1200_cfgFreq(UINT8_t channIdx)
{
   UINT32_t freqEnc;
   UINT8_t regAddr;
   PLTFRM_sts_t sts;
   UINT16_t freqMHz;

   if (channIdx >= PHY_865_MHZ_BAND_CHANNEL_CNT)
       return PLTFRM_STS_INV_PARAM;

   freqMHz = CC1200_865_MHZ_channMap[channIdx];

   /*
    * 9.12 RF Programming
    *
    *   Fout = Fvco / (LO Divider)   --- eqn 1
    *
    * LO Divider is 4 for the 820 MHz - 960 MHz band
    *
    *   Fout = Fvco / (LO_DIV)  --- eqn 2
    *
    * The VCO frequency is given by the 24 bit (unsigned) frequency word
    * "FREQ" located in the FREQ2, FREQ1, and FREQ0 registers. There is also
    * a possibility to perform VCO frequency offset programming, given by the
    * 16 bit (signed) frequency offset word "FREQOFF" located in the FREQOFF1
    * and FREQOFF0 registers. This is intended to adjust for crystal intolerance
    * or fine adjustments of the RF programming.
    *
    *   Fvco = (FREQ * Fosc)/(2^16) + ((FREQOFF * Fosc)/(2^18)) -- eqn 3
    *
    * In our case, Fosc is 40 MHz
    *
    * From eqns 2 and 3, we get -
    *    Fout = (Fosc * (FREQ + (FREQOFF/4))) / ((2^16) * LO_DIV)
    *
    * In case we don't need frequency offset programming ....
    *     Fout = Fosc * FREQ / ((2^16) * LO_DIV)
    *     FREQ = Fout * ((2^16) * LO_DIV) / (40 * (10^6))
    *
    * For Fout of 866 MHz,
    *     FREQ = Fout * (10^6) * ((2^16) * LO_DIV)  /  (40 * (10^6))
    *     FREQ = Fout * 1638.4 * LO_DIV
    *     LO_DIV is 4 for 866 MHz
    *     FREQ = Fout * 6553.6
    *
    * Selecting FREQ value of 5675417, we get a frequency of 865999908.4 MHz.
    */
#if CC1200_OSC_FREQ_IN_MHZ == 40

   if (freqMHz >= 820 && freqMHz <= 960)
   {
       // FREQ = Fout * 1638.4 * LO_DIV  where Fout is in MHz

       freqEnc = freqMHz;
       freqEnc *= CC1200_820_960_MHZ_LO_DIVIDER;
       freqEnc = (freqEnc << 10) + (freqEnc << 9) + (freqEnc << 6)
                 + (freqEnc << 5) + (freqEnc << 2) + (freqEnc << 1)
                 + ((freqEnc << 1)/5);
   }
   else
   {
       sts = PLTFRM_STS_DEV_CFG_NOT_SUPPORTED;
   }

   // if (freqMHz == 866)
   //     freqEnc += 0x15;

#else
#error Specified crystal oscillator frequency not supported !!
#endif

   for (regAddr = CC1200_FREQ0_EXT_REG_ADDR;
        regAddr >= CC1200_FREQ2_EXT_REG_ADDR;
        regAddr --)
   {
        UINT8_t regVal0, regVal1;

        regVal0 = (UINT8_t)freqEnc;
        freqEnc >>= 8;

        sts = CC1200_writeExtAddrSpaceReg(regAddr, regVal0);
        if (sts != PLTFRM_STS_SUCCESS)
            break;

        sts = CC1200_readExtAddrSpaceReg(regAddr, &regVal1);
        if (sts != PLTFRM_STS_SUCCESS)
            break;

        if (regVal1 != regVal0)
        {
            sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;
            break;
        }
   }

   return sts;
}

/*
 * Optionally, the data stream can be Manchester encoded by the modulator
 * and decoded by the demodulator. This option is enabled by setting
 * MDMCFG1.MANCHESTER_EN = 1. Note that Manchester encoding/decoding is
 * only performed on the payload (including optional length and address
 * field) and the CRC and that all packet handling features are still
 * available. In applications where preamble and sync word also need to be
 * Manchester encoded, this can be achieved by selecting
 * PREAMBLE_CFG1.PREAMBLE_WORD = 10b or 11b and manually encoding a two byte
 * long sync word and write it to SYNC3/2/1/0.
 */


/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t CC1200_enableManchesterEnc(void)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;
   const UINT8_t regAddr = CC1200_MDMCFG1_REG_ADDR;

   /*
    * Manchester mode enable. Manchester encoding/decoding is only applicable
    * to payload data including optional CRC. Manchester encoding/decoding is
    * not supported for 4-(G)FSK
    */

   sts = CC1200_readReg(regAddr, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   if (regVal & CC1200_MDMCFG1_REG_MANCHESTER_EN_BIT)
       return sts;

   regVal |= (CC1200_MDMCFG1_REG_MANCHESTER_EN_BIT);

   sts = CC1200_writeReg(regAddr, regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = CC1200_readReg(regAddr, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   if ((regVal & CC1200_MDMCFG1_REG_MANCHESTER_EN_BIT) == 0x0)
       sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;

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
PLTFRM_sts_t CC1200_disableManchesterEnc(void)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;
   const UINT8_t regAddr = CC1200_MDMCFG1_REG_ADDR;

   sts = CC1200_readReg(regAddr, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   if ((regVal & CC1200_MDMCFG1_REG_MANCHESTER_EN_BIT) == 0x0)
       return sts;

   regVal &= ~(CC1200_MDMCFG1_REG_MANCHESTER_EN_BIT);

   sts = CC1200_writeReg(regAddr, regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = CC1200_readReg(regAddr, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   if ((regVal & CC1200_MDMCFG1_REG_MANCHESTER_EN_BIT))
       sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;

   return sts;
}


/*
 * Refer to http://e2e.ti.com/support/low_power_rf/f/155/t/151112.aspx
 *
 * Required RX filter bandwidth can be approximated as -
 *    (Signal BW) + 4x(ppm xtal)x(Frequency of operation)
 *    where Signal BW of = Data rate + 2 x frequency deviation.
 *
 * Example: 38.4 kbps data rate and +/-19.2 kHz deviation gives Signal BW
 * of 38.4 + 2 x 19.2 = 76.8 kHz
 * For modulation index m=1, the frequency separation, which is
 * 2x(frequency deviation), is equal to the data rate.
 * I.e m = 2 x frequency deviation / data rate
 *
 * A narrow bandwidth gives better sensitivity and selectivity at the
 * cost of more accurate RF crystals.
 */


/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t CC1200_cfgSymbolRate(UINT16_t symRateKsps)
{
   UINT32_t symbolRateEnc = 6872;
   UINT8_t regAddr;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   /*
    * 5.4 Symbol rate programming
    *
    * The symbol rate used in transmit and the symbol rate expected in
    * receive is programmed by the SYMBOL_RATE_M and the SYMBOL_RATE_E
    * configuration settings. The symbol rate, Rsymbol, is given by
    * Equation 6 and Equation 7 and is in ksps. Note that SYMBOL_RATE_M
    * is 20 bits wide and consists of the register fields SRATE_M_19_16,
    * SRATE_M_15_8 and SRATE_M_7_0 found in SYMBOL_RATE2, SYMBOL_RATE1,
    * and SYMBOL_RATE0 respectively.
    *
    * Eqn 6: When Symbol Rate (SRATE_E > 0)
    *        Rsym = ((2^20) + SRATE_M)x(2^(SRATE_E)) x Fosc) / (2^39)
    *
    * Eqn 6: When Symbol Rate (SRATE_E = 0)
    *        Rsym = (SRATE_M x Fosc) / (2^38)
    */

   // For now this code supports symbol rate up to 152 ksps.
   // According to Eqn 6, SRATE_M needs to be increased by
   // 6871.947 for every 1 ksps.
   // We will use a value of 6872 for every 1 ksps.
   if (symRateKsps > 152)
       return PLTFRM_STS_PARAM_OUT_OF_RANGE;

   symbolRateEnc *= symRateKsps;

   for (regAddr = CC1200_SYMBOL_RATE0_REG_ADDR;
        regAddr >= CC1200_SYMBOL_RATE2_REG_ADDR;
        regAddr --)
   {
        UINT8_t regVal0, regVal1;

        regVal0 = (UINT8_t)symbolRateEnc;
        symbolRateEnc >>= 8;

        if (regAddr == CC1200_SYMBOL_RATE2_REG_ADDR)
        {
            sts = CC1200_readReg(regAddr, &regVal1);
            if (sts != PLTFRM_STS_SUCCESS)
                break;

            regVal1 &= ~(CC1200_SYMBOL_RATE2_SRATE_M_19_16_BIT_MSK);
            regVal1 |= (regVal0 & CC1200_SYMBOL_RATE2_SRATE_M_19_16_BIT_MSK);

            regVal0 = regVal1;
        }

        sts = CC1200_writeReg(regAddr, regVal0);
        if (sts != PLTFRM_STS_SUCCESS)
            break;

        sts = CC1200_readReg(regAddr, &regVal1);
        if (sts != PLTFRM_STS_SUCCESS)
            break;

        if (regVal1 != regVal0)
        {
            sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;
            break;
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
 *
 ********************************************************************
 */
PLTFRM_sts_t CC1200_updateExtAddrSpaceRegField(const UINT8_t regAddr,
                                               const UINT8_t newVal,
                                               const UINT8_t bitMsk,
                                               const UINT8_t bitShift,
                                               const UINT8_t bitMskShifted)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;

   sts = CC1200_readExtAddrSpaceReg(regAddr, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       UINT8_t encByte;
       regVal &= ~(bitMsk);

       encByte = (UINT8_t)newVal;
       encByte <<= bitShift;
       encByte &= bitMsk;

       regVal |= encByte;

       sts = CC1200_writeExtAddrSpaceReg(regAddr, regVal);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           sts = CC1200_readExtAddrSpaceReg(regAddr, &regVal);
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
 *
 ********************************************************************
 */
PLTFRM_sts_t CC1200_updateRegField(const UINT8_t regAddr,
                                   const UINT8_t newVal,
                                   const UINT8_t bitMsk,
                                   const UINT8_t bitShift,
                                   const UINT8_t bitMskShifted)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;

   sts = CC1200_readReg(regAddr, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       UINT8_t encByte;
       regVal &= ~(bitMsk);

       encByte = (UINT8_t)newVal;
       encByte <<= bitShift;
       encByte &= bitMsk;

       regVal |= encByte;

       sts = CC1200_writeReg(regAddr, regVal);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           sts = CC1200_readReg(regAddr, &regVal);
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
 *
 ********************************************************************
 */
PLTFRM_sts_t CC1200_cfgFreqDevExp(UINT8_t devE)
{
   PLTFRM_sts_t sts;
   sts = CC1200_updateRegField(CC1200_MODCFG_DEV_E_REG_ADDR,
                               devE,
                               CC1200_MODCFG_DEV_E_BIT_MSK,
                               CC1200_MODCFG_DEV_E_SHIFT,
                               CC1200_MODCFG_DEV_E_BIT_MSK_SHIFT);
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
PLTFRM_sts_t CC1200_cfgPreambleDetection(CC1200_preambleDetectionCfg cfg)
{
   PLTFRM_sts_t sts;
   sts = CC1200_updateRegField(CC1200_PREAMBLE_CFG0_REG_ADDR,
                               cfg == CC1200_PREAMBLE_DETECTION_DISABLED ? 0x0 : 0x1,
                               CC1200_PREAMBLE_CFG0_PQT_EN_BIT_MSK,
                               CC1200_PREAMBLE_CFG0_PQT_EN_SHIFT,
                               CC1200_PREAMBLE_CFG0_PQT_EN_BIT_MSK_SHIFT);
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
PLTFRM_sts_t CC1200_cfgPQT(UINT8_t pqtVal)
{
   PLTFRM_sts_t sts;
   sts = CC1200_updateRegField(CC1200_PREAMBLE_CFG0_REG_ADDR,
                               pqtVal,
                               CC1200_PREAMBLE_CFG0_PQT_BIT_MSK,
                               CC1200_PREAMBLE_CFG0_PQT_SHIFT,
                               CC1200_PREAMBLE_CFG0_PQT_BIT_MSK_SHIFT);
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
PLTFRM_sts_t CC1200_cfgModemMode(CC1200_modemMode_t newMode)
{
   PLTFRM_sts_t sts;
   sts = CC1200_updateRegField(CC1200_MODCFG_DEV_E_REG_ADDR,
                               (UINT8_t)newMode,
                               CC1200_MODCFG_MODEM_MODE_BIT_MSK,
                               CC1200_MODCFG_MODEM_MODE_SHIFT,
                               CC1200_MODCFG_MODEM_MODE_BIT_MSK_SHIFT);
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
PLTFRM_sts_t CC1200_cfgModulationScheme(CC1200_modulationScheme_t newMod)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal, currMod;
   const UINT8_t regAddr = CC1200_MODCFG_DEV_E_REG_ADDR;

   /*
    * CC120X supports amplitude and frequency shift modulation formats. The
    * desired modulation format is set in the MODCFG_DEV_E.MOD_FORMAT register.
    */

   sts = CC1200_readReg(regAddr, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   currMod = (regVal >> CC1200_MODCFG_MOD_FMT_SHIFT);
   currMod &= CC1200_MODCFG_MOD_FMT_BIT_MSK_SHIFT;

   if (currMod == (UINT8_t)newMod)
       return sts;

   regVal &= ~(CC1200_MODCFG_MOD_FMT_BIT_MSK);
   currMod = (UINT8_t)newMod;
   currMod = (currMod << CC1200_MODCFG_MOD_FMT_SHIFT) & CC1200_MODCFG_MOD_FMT_BIT_MSK;
   regVal |= currMod;

   sts = CC1200_writeReg(regAddr, regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = CC1200_readReg(regAddr, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   currMod = (regVal >> CC1200_MODCFG_MOD_FMT_SHIFT);
   currMod &= CC1200_MODCFG_MOD_FMT_BIT_MSK_SHIFT;

   if (currMod != (UINT8_t)newMod)
       return PLTFRM_STS_DEV_REG_UPDATE_ERR;

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
PLTFRM_sts_t CC1200_cfgPktLenMode(CC1200_pktLenMode_t newMode)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal, currMode;
   const UINT8_t regAddr = CC1200_PKT_CFG0_REG_ADDR;

   // Get the current mode

   /*
    * CC120X supports both fixed packet length protocols and variable packet
    * length protocols. Variable or fixed packet length mode can be used for
    * packets up to 255 bytes.
    */

   sts = CC1200_readReg(regAddr, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   currMode = (regVal >> CC1200_PKT_CFG0_LEN_CFG_SHIFT);
   currMode &= CC1200_PKT_CFG0_LEN_CFG_BIT_MSK_SHIFT;

   if (currMode == (UINT8_t)newMode)
   {
       CC1200_cntxt.pktLenMode = newMode;
       return sts;
   }

   currMode = (UINT8_t)newMode;
   currMode = (currMode << CC1200_PKT_CFG0_LEN_CFG_SHIFT) & CC1200_PKT_CFG0_LEN_CFG_BIT_MSK;
   regVal &= ~(CC1200_PKT_CFG0_LEN_CFG_BIT_MSK);
   regVal |= currMode;

   sts = CC1200_writeReg(regAddr, regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = CC1200_readReg(regAddr, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   currMode = (regVal >> CC1200_PKT_CFG0_LEN_CFG_SHIFT);
   currMode &= CC1200_PKT_CFG0_LEN_CFG_BIT_MSK_SHIFT;

   if (currMode != (UINT8_t)newMode)
       sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;

   CC1200_cntxt.pktLenMode = newMode;

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
PLTFRM_sts_t CC1200_cfgSyncWordThreshold(UINT8_t thresholdVal)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;
   const UINT8_t regAddr = CC1200_SYNC_CFG1_REG_ADDR;

   /*
    * Soft decision sync word threshold. A sync word is accepted when the
    * calculated sync word qualifier value (PQT_SYNC_ERR.SYNC_ERROR) is less
    * than SYNC_THR/2). A low threshold value means a strict sync word qualifier
    * (sync word must be of high quality to be accepted) while a high threshold
    * value will accept sync word of a poorer quality (increased probability of
    * detecting ‘false’ sync words)
    */


   sts = CC1200_readReg(regAddr, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       UINT8_t encByte;
       regVal &= ~(CC1200_SYNC_CFG1_SYNC_THR_BIT_MSK);

       encByte = (UINT8_t)thresholdVal;
       encByte &= CC1200_SYNC_CFG1_SYNC_THR_BIT_MSK;

       regVal |= encByte;

       sts = CC1200_writeReg(regAddr, regVal);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           sts = CC1200_readReg(regAddr, &regVal);
           regVal &= CC1200_SYNC_CFG1_SYNC_THR_BIT_MSK;
           if (regVal != (UINT8_t)thresholdVal)
               sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;
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
 *
 ********************************************************************
 */
PLTFRM_sts_t CC1200_cfgSyncWord(CC1200_syncMode_t syncMode)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;
   const UINT8_t regAddr = CC1200_SYNC_CFG1_REG_ADDR;

   /*
    * The sync word is set in the SYNC3/2/1/0 registers. The sync word
    * provides byte synchronization of the incoming packet. Non-supported
    * sync word lengths can be emulated by using parts of the preamble
    * pattern in the SYNC registers.
    */

   /*
	* Packet format -
	* > Preamble Bits      (Upto 240 bits) - Inserted by the CC1200
    * > Sync word          (Upto 32 bits)  - Inserted by the CC1200
    * > Length byte field  (optional)
    * > Address byte field (optional)
    * > Data field
    * > CRC-16 field       (optional)      - Inserted by the CC1200
    */

   /*
    * In receive mode, the radio will search for a valid sync word (if
    * SYNC_CFG1.SYNC_MODE != 0) and when found, the demodulator has obtained
    * both bit and byte synchronization and will receive the first payload
    * byte. For more details on byte synchronization/sync word detection,
    * please see Section 6.7.
    *
    * 6.7 Byte Synchronization, Sync Word Detection
    * Byte synchronization is achieved by a continuous sync word search using
    * the novel WaveMatch capture logic (correlation filter). The sync word is
    * configured through the SYNC3/2/1/0 registers and can be programmed to be
    * 11, 16, 18, 24 or 32 bits. This is done through the SYNC_CFG1.SYNC_MODE
    * register field. In TX mode, these bits are automatically inserted at the
    * start of the packet by the modulator. The MSB in the sync word is sent
    * first. In RX mode, the demodulator uses the sync word to find the start
    * of the incoming packet.
    *
    * The GPIO pins can be used in both RX and TX to give an interrupt when a
    * sync word has been received/transmitted.
    */

   sts = CC1200_readReg(regAddr, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       UINT8_t encByte;
       regVal &= ~(CC1200_SYNC_CFG1_SYNC_MODE_BIT_MSK);

       encByte = (UINT8_t)syncMode;
       encByte <<= CC1200_SYNC_CFG1_SYNC_MODE_SHIFT;
       encByte &= CC1200_SYNC_CFG1_SYNC_MODE_BIT_MSK;

       regVal |= encByte;

       sts = CC1200_writeReg(regAddr, regVal);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           sts = CC1200_readReg(regAddr, &regVal);
           regVal >>= CC1200_SYNC_CFG1_SYNC_MODE_SHIFT;
           regVal &= CC1200_SYNC_CFG1_SYNC_MODE_BIT_MSK_SHIFT;
           if (regVal != (UINT8_t)syncMode)
               sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;
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
 *
 ********************************************************************
 */
PLTFRM_sts_t CC1200_cfgPreambleLen(UINT8_t lenInBits)
{
   PLTFRM_sts_t sts;
   SINT16_t idx;

   /*
    * The preamble pattern is an alternating sequence of ones and zeros
    * (1010../0101../00110011../11001100..) programmable through the
    * PREAMBLE_CFG1.PREAMBLE_WORD register field. The minimum length of
    * the preamble is programmable through the PREAMBLE_CFG1.NUM_PREAMBLE
    * register field. When strobing TX, the modulator will start transmitting
    * a preamble. When the programmed number of preamble bytes has been
    * transmitted, the modulator will send the sync word and then data from
    * the TX FIFO. If the TX FIFO is empty, the modulator will continue to
    * send preamble bytes until the first byte is written to the TX FIFO. The
    * modulator will then send the sync word and then the data bytes.
    *
    * Default preamble length is 24 bits.
    *
    * The CC120X family use strong DSP logic to detect a sync word and the
    * preamble is only needed for AGC settling, i.e. settling the gain of the
    * front end. A 4 bits preamble is enough for settling including frequency
    * offset compensation (AFC).
    */

   for (idx=0; idx<CC1200_PREAMBLE_LENGTH_ENC_LIST_SZ; idx++)
   {
        if (CC1200_preambleLengthEncList[idx].lenInBits == lenInBits)
            break;
   }

   if (idx >= CC1200_PREAMBLE_LENGTH_ENC_LIST_SZ)
       return PLTFRM_STS_INV_PARAM;

   sts = CC1200_updateRegField(CC1200_PREAMBLE_CFG1_REG_ADDR,
                               CC1200_preambleLengthEncList[idx].enc,
                               CC1200_PREAMBLE_CFG1_NUM_PREAMBLE_BIT_MSK,
                               CC1200_PREAMBLE_CFG1_NUM_PREAMBLE_SHIFT,
                               CC1200_PREAMBLE_CFG1_NUM_PREAMBLE_BIT_MSK_SHIFT);
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
PLTFRM_sts_t CC1200_disableCCA(void)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;
   const UINT8_t regAddr = CC1200_PKT_CFG2_REG_ADDR;

   sts = CC1200_readReg(regAddr, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);
   else
   {
       regVal &= ~(CC1200_PKT_CFG2_CCA_MODE_BIT_MSK);

       // Mode 000 - always give clear channel indication

       sts = CC1200_writeReg(regAddr, regVal);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_1);

       sts = CC1200_readReg(regAddr, &regVal);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_1);
       else
       {
           if (regVal & CC1200_PKT_CFG2_CCA_MODE_BIT_MSK)
           {
               sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;
               SYS_fatal(SYS_FATAL_ERR_1);
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
PLTFRM_sts_t CC1200_enableCCA(void)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;
   const UINT8_t regAddr = CC1200_PKT_CFG2_REG_ADDR;

   sts = CC1200_readReg(regAddr, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       UINT8_t newMode;

       regVal &= ~(CC1200_PKT_CFG2_CCA_MODE_BIT_MSK);

       newMode = CC1200_CCA_MODE_3;  // Indicates clear channel when RSSI is below
                                     // threshold and currently not receiving a packet

       newMode <<= CC1200_PKT_CFG2_CCA_MODE_SHIFT;
       newMode &= CC1200_PKT_CFG2_CCA_MODE_BIT_MSK;

       regVal |= newMode;

       sts = CC1200_writeReg(regAddr, regVal);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           sts = CC1200_readReg(regAddr, &regVal);
           if (sts == PLTFRM_STS_SUCCESS)
           {
               newMode = (regVal >> CC1200_PKT_CFG2_CCA_MODE_SHIFT);
               newMode &= CC1200_PKT_CFG2_CCA_MODE_BIT_MSK_SHIFT;
               if (newMode != CC1200_CCA_MODE_3)
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
 *
 ********************************************************************
 */
PLTFRM_sts_t CC1200_cfgDataCommMode(UINT8_t newMode)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;

   // Get the current mode

   sts = CC1200_readReg(CC1200_PKT_CFG2_REG_ADDR, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       UINT8_t currMode = regVal & CC1200_PKT_CFG2_REG_PKT_FMT_BIT_MSK;

       if (currMode == newMode)
           return sts;

       switch (newMode)
       {
           case CC1200_DATA_COMM_MODE_FIFO:
                {

                   regVal &= ~CC1200_PKT_CFG2_REG_PKT_FMT_BIT_MSK;
                   regVal |= newMode;
                   sts = CC1200_writeReg(CC1200_PKT_CFG2_REG_ADDR, regVal);
                   if (sts != PLTFRM_STS_SUCCESS)
                       break;

                   // Normal mode/FIFO mode (MDMCFG1.FIFO_EN must be set to 1
                   // and MDMCFG0.TRANSPARENT_MODE_EN must be set to 0)

                   sts = CC1200_readReg(CC1200_MDMCFG1_REG_ADDR, &regVal);
                   if (sts != PLTFRM_STS_SUCCESS)
                       break;
                   regVal |= CC1200_MDMCFG1_REG_FIFO_EN_BIT;
                   sts = CC1200_writeReg(CC1200_MDMCFG1_REG_ADDR, regVal);
                   if (sts != PLTFRM_STS_SUCCESS)
                       break;

                   sts = CC1200_readReg(CC1200_MDMCFG0_REG_ADDR, &regVal);
                   if (sts != PLTFRM_STS_SUCCESS)
                       break;
                   regVal &= ~(CC1200_MDMCFG0_TRANSPARENT_MODE_EN_BIT);
                   sts = CC1200_writeReg(CC1200_MDMCFG0_REG_ADDR, regVal);
                   if (sts != PLTFRM_STS_SUCCESS)
                       break;
                }
                break;

           case CC1200_DATA_COMM_MODE_SYNCH_SERIAL:
                {
                   // TODO
                }
                break;

           case CC1200_DATA_COMM_MODE_RANDOM:
                {

                   // TODO
                }
                break;

           case CC1200_DATA_COMM_MODE_TRANSPARENT_SERIAL:
                {

                }
                break;

           default:
                {
                   SYS_fatal(SYS_FATAL_ERR_1);
                }
                break;
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
PLTFRM_sts_t CC1200_cfgUARTMode(CC1200_uartModeCfg_t cfg)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;
   const UINT8_t regAddr = CC1200_PKT_CFG0_REG_ADDR;

   sts = CC1200_readReg(regAddr, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       UINT8_t newMode;

       regVal &= ~(CC1200_PKT_CFG0_UART_MODE_CFG_BIT_MSK);

       newMode = (UINT8_t)cfg;

       newMode <<= CC1200_PKT_CFG0_UART_MODE_CFG_SHIFT;
       newMode &= CC1200_PKT_CFG0_UART_MODE_CFG_BIT_MSK;

       regVal |= newMode;

       sts = CC1200_writeReg(regAddr, regVal);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           sts = CC1200_readReg(regAddr, &regVal);
           if (sts == PLTFRM_STS_SUCCESS)
           {
               newMode = (regVal >> CC1200_PKT_CFG0_UART_MODE_CFG_SHIFT);
               newMode &= CC1200_PKT_CFG0_UART_MODE_CFG_BIT_MSK_SHIFT;
               if (newMode != (UINT8_t)cfg)
                   sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;
           }
       }
   }

   return sts;
}


/*
 * Packet format -
 * > Preamble Bits      (Upto 240 bits) - Inserted by the CC1200
 * > Sync word          (Upto 32 bits)  - Inserted by the CC1200
 * > Length byte field  (optional)
 * > Address byte field (optional)
 * > Data field
 * > CRC-16 field       (optional)      - Inserted by the CC1200
 *
 *
 * Variable or fixed packet length mode can be used for packets up
 * to 255 bytes. For longer packets, infinite packet length mode must
 * be used. The packet length is defined as the payload data and the
 * optional address byte, excluding the optional length byte, the
 * optional CRC, and the optional append status.
 *
 *
 * If the radio controller is in RX when the STX or SFSTXON command
 * strobes are used, the TX-on-CCA function will be used. If the chan
 * is clear, TX (or FSTXON state) is entered. The PKT_CFG2.CCA_MODE
 * setting controls the conditions for clear channel assessment (see
 * Section 6.11 for more details).
 *
 * The SIDLE command strobe can always be used to force the radio
 * controller to go to the IDLE state.
 *
 *
 * STX command probe
 * In IDLE state: Enable TX. Perform calibration first if SETTLING_CFG.FS_AUTOCAL = 1.
 * If in RX state and PKT_CFG2.CCA_MODE != 0: Only go to TX if channel is clear.
 */


/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t CC1200_cfgTxOffMode(CC1200_txOffMode_t newMode)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;
   const UINT8_t regAddr = CC1200_RFEND_CFG0_REG_ADDR;

   // Determines the state the radio will enter after transmitting a packet

   sts = CC1200_readReg(regAddr, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       UINT8_t currMode = (regVal >> CC1200_RFEND_CFG0_TXOFF_MODE_SHIFT);
       currMode &= CC1200_RFEND_CFG0_TXOFF_MODE_BIT_MSK_SHIFT;

       if (currMode != (UINT8_t)newMode)
       {
           currMode = (UINT8_t)newMode;
           currMode <<= CC1200_RFEND_CFG0_TXOFF_MODE_SHIFT;
           currMode &= CC1200_RFEND_CFG0_TXOFF_MODE_BIT_MSK;

           regVal &= ~(CC1200_RFEND_CFG0_TXOFF_MODE_BIT_MSK);
           regVal |= currMode;

           sts = CC1200_writeReg(regAddr, regVal);
           if (sts == PLTFRM_STS_SUCCESS)
           {
               sts = CC1200_readReg(regAddr, &regVal);
               if (sts == PLTFRM_STS_SUCCESS)
               {
                   currMode = (regVal >> CC1200_RFEND_CFG0_TXOFF_MODE_SHIFT);
                   currMode &= CC1200_RFEND_CFG0_TXOFF_MODE_BIT_MSK_SHIFT;

                   if (currMode != (UINT8_t)newMode)
                       sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;
               }
           }
       }
   }

   if (sts == PLTFRM_STS_SUCCESS)
       CC1200_cntxt.txOffMode = newMode;

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
PLTFRM_sts_t CC1200_cfgRxOffMode(CC1200_rxOffMode_t newMode)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;
   const UINT8_t regAddr = CC1200_RFEND_CFG1_REG_ADDR;

   // Determines the state the radio will enter after receiving a good packet
   // When a packet is successfully received, the radio controller goes to the
   // state indicated by the RFEND_CFG1.RXOFF_MODE setting, i.e. IDLE, FSTXON,
   // TX or RX.

   sts = CC1200_readReg(regAddr, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       UINT8_t currMode = (regVal >> CC1200_RFEND_CFG1_RXOFF_MODE_SHIFT);
       currMode &= CC1200_RFEND_CFG1_RXOFF_MODE_BIT_MSK_SHIFT;

       if (currMode != (UINT8_t)newMode)
       {
           currMode = (UINT8_t)newMode;
           currMode <<= CC1200_RFEND_CFG1_RXOFF_MODE_SHIFT;
           currMode &= CC1200_RFEND_CFG1_RXOFF_MODE_BIT_MSK;

           regVal &= ~(CC1200_RFEND_CFG1_RXOFF_MODE_BIT_MSK);
           regVal |= currMode;

           sts = CC1200_writeReg(regAddr, regVal);
           if (sts == PLTFRM_STS_SUCCESS)
           {
               sts = CC1200_readReg(regAddr, &regVal);
               if (sts == PLTFRM_STS_SUCCESS)
               {
                   currMode = (regVal >> CC1200_RFEND_CFG1_RXOFF_MODE_SHIFT);
                   currMode &= CC1200_RFEND_CFG1_RXOFF_MODE_BIT_MSK_SHIFT;

                   if (currMode != (UINT8_t)newMode)
                       sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;
               }
           }
       }
   }

   if (sts == PLTFRM_STS_SUCCESS)
       CC1200_cntxt.rxOffMode = newMode;

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
PLTFRM_sts_t CC1200_badPktRxActionCfg(CC1200_termOnBadPktCfg_t newCfg)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;
   const UINT8_t regAddr = CC1200_RFEND_CFG0_REG_ADDR;

   /*
    * TERM_ON_BAD_PACKET_EN bit in RFEND_CFG0 register
    * ----------------------------------------------------------------------------
    * 0x0 - Terminate on bad packet disabled. When a bad packet is received (address,
    *       length, or CRC error) the radio stays in RX regardless of the RFEND_CFG1.
    *       RXOFF_MODE
    *
    * 0x1 - Terminate on bad packet enabled. RFEND_CFG1.RXOFF_MODE is ignored and the
    *       radio enters IDLE mode (or SLEEP mode if eWOR is used) when a bad packet
    *       has been received
    * ----------------------------------------------------------------------------
    */

   sts = CC1200_readReg(regAddr, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       UINT8_t currCfg = regVal >> CC1200_RFEND_CFG0_TERM_ON_BAD_PKT_EN_SHIFT;
       currCfg &= CC1200_RFEND_CFG0_TERM_ON_BAD_PKT_EN_BIT_MSK_SHIFT;

       if (currCfg != (UINT8_t)newCfg)
       {
           regVal &= ~(CC1200_RFEND_CFG0_TERM_ON_BAD_PKT_EN_BIT_MSK);
           currCfg = (UINT8_t)newCfg;
           currCfg <<= CC1200_RFEND_CFG0_TERM_ON_BAD_PKT_EN_BIT_MSK_SHIFT;
           currCfg &= CC1200_RFEND_CFG0_TERM_ON_BAD_PKT_EN_BIT_MSK;
           regVal |= currCfg;

           sts = CC1200_writeReg(regAddr, regVal);
           if (sts == PLTFRM_STS_SUCCESS)
           {
               sts = CC1200_readReg(regAddr, &regVal);
               if (sts == PLTFRM_STS_SUCCESS)
               {
                   currCfg = regVal >> CC1200_RFEND_CFG0_TERM_ON_BAD_PKT_EN_SHIFT;
                   currCfg &= CC1200_RFEND_CFG0_TERM_ON_BAD_PKT_EN_BIT_MSK_SHIFT;
                   if (currCfg != (UINT8_t)newCfg)
                       sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;
               }
           }
       }
   }

   if (sts == PLTFRM_STS_SUCCESS)
       CC1200_cntxt.termOnBadPktCfg = newCfg;

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
PLTFRM_sts_t CC1200_cfgCRCAutoFlush(CC1200_crcAutoFlushCfg_t newCfg)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;
   const UINT8_t regAddr = CC1200_FIFO_CFG_REG_ADDR;

   // CRC_AUTOFLUSH bit in FIFO_CFG
   // 1 - Automatically flushes the last packet received in the RX FIFO
   // if a CRC error occurred. If this bit has been turned off and should
   // be turned on again, an SFRX strobe must first be issued.
   // On reset, this bit is 0x1

   sts = CC1200_readReg(regAddr, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       regVal &= ~(CC1200_FIFO_CFG_CRC_AUTOFLUSH_BIT);
       if (newCfg == CC1200_ENABLE_CRC_AUTO_FLUSH)
           regVal |= CC1200_FIFO_CFG_CRC_AUTOFLUSH_BIT;

       sts = CC1200_writeReg(regAddr, regVal);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           sts = CC1200_readReg(regAddr, &regVal);
           regVal &= CC1200_FIFO_CFG_CRC_AUTOFLUSH_BIT;

           if (((newCfg == CC1200_ENABLE_CRC_AUTO_FLUSH) && (regVal == 0x0))
               || ((newCfg == CC1200_DISABLE_CRC_AUTO_FLUSH) && (regVal != 0x0)))
           {
               sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;
           }
           else
               CC1200_cntxt.crcAutoFlushCfg = newCfg;
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
PLTFRM_sts_t CC1200_cfgMaxRxPktLen(UINT8_t pyldLen)
{
   PLTFRM_sts_t sts;
   const UINT8_t regAddr = CC1200_PKT_LEN_REG_ADDR;

   /*
    * The PKT_LEN register is used to set the maximum packet length allowed in RX.
    * Any packet received with a length byte with a value greater than PKT_LEN will
    * be discarded.
    */

   sts = CC1200_writeReg(regAddr, pyldLen);
   if (sts == PLTFRM_STS_SUCCESS)
       CC1200_cntxt.rxMaxFramePyldLen = pyldLen;

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
void CC1200_swReset(void)
{
   PLTFRM_sts_t sts;
   CC1200_state_t state;

   /*
    * <0x30 SRES Reset chip>
    * All internal registers are set to their default values and the radio
    * will enter IDLE state.
    * When a SRES strobe is issued the CSn pin must be kept low and wait
    * for SO to go low again before the next header byte can be issued,
    * as shown in Figure 5.
    */

   sts = CC1200_sendCmdStrobe(CC1200_CMD_STROBE_SRES, NULL);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);

   sts = CC1200_getCurrState(&state);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);

   if (state != CC1200_STATE_IDLE);
       SYS_fatal(SYS_FATAL_ERR_1);

   CC1200_setCurrState(CC1200_STATE_IDLE);

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
PLTFRM_sts_t CC1200_enaRndGen(void)
{
   return CC1200_updateExtAddrSpaceRegField(CC1200_RNDGEN_EXT_REG_ADDR,
                                            0x1, CC1200_RNDGEN_EN_BIT_MSK,
                                            CC1200_RNDGEN_EN_SHIFT,
                                            CC1200_RNDGEN_EN_BIT_MSK_SHIFT);
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void CC1200_wakeUp(void)
{
   PLTFRM_sts_t sts;
   CC1200_state_t state;

   /*
    * When the CC1200 is in sleep state, setting CSn low again will turn
    * on the regulator and crystal oscillator and make the chip enter the
    * IDLE state.
    *
    *
    * When CSn is pulled low, the MCU must wait until CC120X SO pin goes
    * low before starting to transfer the header byte. This indicates that
    * the crystal is stable. Unless the chip was just reset or was in SLEEP
    * or XOFF state, or the XOSC configuration has been altered, the SO pin
    * will always go low immediately after pulling CSn low.
    */

   // CC1200_spiBegin( ) waits for the SO pin to go low.

   sts = CC1200_sendCmdStrobe(CC1200_CMD_STROBE_SNOP, NULL);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);

   sts = CC1200_getCurrState(&state);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);

   if (state != CC1200_STATE_IDLE);
       SYS_fatal(SYS_FATAL_ERR_1);


   // The "RNDGEN" register value is not retained. It needs to be reconfigured.

   if (CC1200_enaRndGen() != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);

   CC1200_setCurrState(CC1200_STATE_IDLE);

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
PLTFRM_sts_t CC1200_outOfLockDetectorCfg(CC1200_outOfLockDetectorCfg_t cfg)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;
   const UINT8_t regAddr = CC1200_XOSC2_EXT_REG_ADDR;

   sts = CC1200_readExtAddrSpaceReg(regAddr, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       regVal &= ~CC1200_FS_CFG_LOCK_EN_BIT_MSK;

       /*
        * 0 - The XOSC will be turned off if the SXOFF, SPWD, or SWOR command strobes
        *     are issued
        * 1 - The XOSC is forced on even if an SXOFF, SPWD, or SWOR command strobe has
        *     been issued. This can be used to enable fast start-up from SLEEP/XOFF on
        *     the expense of a higher current consumption
        */
       if (cfg == CC1200_OUT_OF_LOCK_DETECTOR_ENABLED)
           regVal |= CC1200_FS_CFG_LOCK_EN_BIT_MSK;

       sts = CC1200_writeExtAddrSpaceReg(regAddr, regVal);
       if (sts != PLTFRM_STS_SUCCESS)
       {
           UINT8_t currCfg;
           sts = CC1200_readExtAddrSpaceReg(regAddr, &currCfg);
           if (sts == PLTFRM_STS_SUCCESS)
           {
               currCfg >>= CC1200_FS_CFG_LOCK_EN_SHIFT;
               currCfg &= CC1200_FS_CFG_LOCK_EN_BIT_MSK_SHIFT;

               if (currCfg != (UINT8_t)cfg)
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
PLTFRM_sts_t CC1200_xoscPowerSaveCfg(CC1200_XoscPowerSaveCfg_t cfg)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;
   const UINT8_t regAddr = CC1200_XOSC2_EXT_REG_ADDR;

   /*
    * The crystal oscillator (XOSC) is either automatically controlled or
    * always on, if .XOSC_CORE_PD_OVERRIDE = 1. If the XOSC is forced
    * on, the crystal will always stay on even if an SXOFF, SPWD, or SWOR
    * command strobe has been issued. This can be used to enable fast start-up
    * from SLEEP/XOFF at the expense of a higher current consumption. If
    * XOSC2.XOSC_CORE_PD_OVERRIDE = 0, the XOSC will be turned off if the
    * SXOFF, SPWD, or SWOR command strobes are issued; the state machine then
    * goes to XOFF or SLEEP state. This can only be done from the IDLE state.
    * The XOSC will be automatically turned on again when CSn goes low, and
    * the radio will enter IDLE state.
    */

   sts = CC1200_readExtAddrSpaceReg(regAddr, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       regVal &= ~CC1200_XOSC2_CORE_PD_OVERRIDE_BIT;

       /*
        * 0 - The XOSC will be turned off if the SXOFF, SPWD, or SWOR command strobes
        *     are issued
        * 1 - The XOSC is forced on even if an SXOFF, SPWD, or SWOR command strobe has
        *     been issued. This can be used to enable fast start-up from SLEEP/XOFF on
        *     the expense of a higher current consumption
        */
       if (cfg == CC1200_XOSC_POWER_SAVE_CFG_DIS)
           regVal |= CC1200_XOSC2_CORE_PD_OVERRIDE_BIT;

       sts = CC1200_writeExtAddrSpaceReg(regAddr, regVal);
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
void CC1200_enterLPM(void)
{
   PLTFRM_sts_t sts;

   // SPWD strobe - Enter SLEEP mode when CSn is de-asserted
   // Both FIFOs are flushed when going to the SLEEP state.

   // 9.2 Crystal Control
   // The crystal oscillator (XOSC) is either automatically controlled
   // or always on, if XOSC2.XOSC_CORE_PD_OVERRIDE = 1. If the XOSC is
   // forced on, the crystal will always stay on even if an SXOFF, SPWD,
   // or SWOR command strobe has been issued. This can be used to enable
   // fast start-up from SLEEP/XOFF at the expense of a higher current
   // consumption. If XOSC2.XOSC_CORE_PD_OVERRIDE = 0, the XOSC will be
   // turned off if the SXOFF, SPWD, or SWOR command strobes are issued;
   // the state machine then goes to XOFF or SLEEP state. This can only be
   // done from the IDLE state.
   // Crystal oscillator start-up time depends on crystal ESR and load
   // capacitances.

   // The voltage regulator to the digital core is controlled by the radio
   // controller. When the chip enters the SLEEP state which is the state
   // with the lowest current consumption, the voltage regulator is disabled.
   // This occurs after CSn is released when a SPWD or SWOR command strobe
   // has been sent on the SPI interface. The chip is then in the SLEEP state.
   // Setting CSn low again will turn on the regulator and crystal oscillator
   // and make the chip enter the IDLE state.

   if (CC1200_forceIdle( ) != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);

   // The SPWD, SWOR, and the SXOFF command strobes are not executed before
   // the CSn goes high.

   sts = CC1200_sendCmdStrobe(CC1200_CMD_STROBE_SPWD, NULL);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);

   CC1200_setCurrState(CC1200_STATE_SLEEPING);

   /*
    * Note that all registers in register space (address 0x00 - 0x2E) have retention.
    * In extended register space, the status registers, FIFO pointers, and the AES
    * work space do not have retention. Please see Table 5 for details.
    */

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
PLTFRM_sts_t CC1200_startRx(void)
{
   PLTFRM_sts_t sts;
   CC1200_state_t cc1200State;

   /*
    * <SRX cmd strobe>
    * Enable RX. Perform calibration first if coming from IDLE and
    * SETTLING_CFG.FS_AUTOCAL = 1.
    *
    * IDLE -> Frequency synthesizer is turned on, can optionally be
    * calibrated, and then settles to the correct frequency -> RX.
    */

   sts = CC1200_sendCmdStrobe(CC1200_CMD_STROBE_SRX, NULL);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_1);
   }
   else
   {
       do
       {
           sts = CC1200_sendCmdStrobe(CC1200_CMD_STROBE_SNOP, &cc1200State);
           if (sts != PLTFRM_STS_SUCCESS)
           {
               SYS_fatal(SYS_FATAL_ERR_1);
           }
       }  while (cc1200State != CC1200_STATE_RX);

       CC1200_setCurrState(CC1200_STATE_RX);
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
PLTFRM_sts_t CC1200_cfgFIFOThreshold(UINT8_t thVal)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;
   const UINT8_t regAddr = CC1200_FIFO_CFG_REG_ADDR;

   if (thVal > 127)
       return PLTFRM_STS_INV_PARAM;

   /*
    * Threshold value for the RX and TX FIFO. The threshold value is
    * coded in opposite directions for the two FIFOs to give equal
    * margin to the overflow and underflow conditions when the threshold
    * is reached. I.e.; FIFO_THR = 0 means that there are 127 bytes in
    * the TX FIFO and 1 byte in the RX FIFO, while FIFO_THR = 127 means
    * that there are 0 bytes in the TX FIFO and 128 bytes in the RX FIFO
    * when the thresholds are reached
    */

   sts = CC1200_readReg(regAddr, &regVal);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       regVal &= ~CC1200_FIFO_CFG_FIFO_THR_BIT_MSK;

       regVal |= thVal;

       sts = CC1200_writeReg(regAddr, regVal);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           regVal = 0;
           sts = CC1200_readReg(regAddr, &regVal);
           if (sts == PLTFRM_STS_SUCCESS)
    	   {
               regVal &= CC1200_FIFO_CFG_FIFO_THR_BIT_MSK;
               if (regVal != thVal)
                   sts = PLTFRM_STS_DEV_REG_UPDATE_ERR;
           }
       }
   }

   return sts;
}


/*
 * RX Sniff Mode
 *
 * The RX Sniff Mode is enabled by using the eWOR timer together with the 
 * RX termination based on carrier sense (CS) or preamble quality threshold 
 * (PQT).
 *
 * It is recommended that both MDMCFG1.CARRIER_SENSE_GATE and 
 * SYNC_CFGx.PQT_GATING_EN = 0 when using the RX Sniff Mode to simplify how 
 * Tevent0 should be determined.
 * 
 * PQT_GATING_EN: (PQT gating enabled) - The demodulator will not start to 
 * look for a sync word before a preamble is detected (i.e. PQT_REACHED is 
 * asserted). The preamble detector must be enabled for this feature to work
 * (PREAMBLE_CFG0.PQT_EN = 1)
 *
 * CARRIER_SENSE_GATE: When CARRIER_SENSE_GATE is 1, the demodulator will not 
 * start to look for a sync word before CARRIER_SENSE is asserted.
 *
 * RX timeout must be disabled when running the RX Sniff Mode (RFEND_CFG1.RX_TIME 
 * = 111b).
 */


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t CC1200_txPowerSetPointValid(SINT16_t powerInDbm)
{
   UINT8_t rc = 0;

   if (powerInDbm >= CC1200_MIN_OP_POWER_DBM
       && powerInDbm <= CC1200_MAX_OP_POWER_DBM)
   {
       rc = 0x1;
   }

   return rc;
}



/*
 * CC1200 configuration -
 * > Modulation (2-FSK/2-GFSK etc)
 * > Number of bits in the preamble (0 to 240 bits)
 * > Preamble byte (01010101 / 10101010 / 00110011 / 11001100)
 * > PQT_EN (Preamble detection enable / disable) dflt - enabled
 * > SYNC_MODE (Sync word config (0/11/16/18/24/32/..) dflt - 32
 * > SYNC_THR (Sync word quality threshold) dflt - 10
 * > PQT_VALID_TIMEOUT
 * > PQT
 * > Symbol rate
 * > Device address (8 bits)
 * > FIFO auto flush on bad crc
 * > FIFO tx/rx thresholds
 * > Freq Synth band selection (820 / 410 etc)
 * > Freq Synth out of lock detection enable / disable (dflt = disabled)
 * > CCA mode (always clear/clear when rssi < th / rssi < thr and !rxing pkt)
 * > PKT_FORMAT (Normal/TSM/SSM/Random)
 * > FEC (enable/disable) dflt-disabled
 * > WHITE_DATA (enable/disable) dflt-disabled
 * > Address check on receive (enable/disable) dflt-disabled
 * > CRC_CFG (both tx and rx) (enabled/disabled) dflt-enabled (0x1)
 * > APPEND_STATUS (Append CRC/RSSI/LQI to rcvd packet) dflt-enabled
 * > LENGTH_CONFIG (fixed/variable/infinite) dflt-fixed
 * > PKT_BIT_LEN
 * > UART_MODE_EN (UART mode enable) dflt-disabled
 * > UART_SWAP_EN
 * > RXOFF_MODE (state of radio after rcving a good pkt) dflt - IDLE
 * > RX_TIME
 * > TX_TIME_QUAL
 * > CAL_END_WAKE_UP_EN
 * > TXOFF_MODE (state of radio after txing a packet) dflt - IDLE
 * > TERM_IN_BAD_PACKET_EN (state of radio on rcving a bad packet) dflt - RX
 * > ANT_DIV_RX_TERM_CFG   dflt-disabled (0x00)
 * > PA_RAMP_SHAPE_EN (PA ramping and ASK/OOK shaping enabled) dflt-enabled
 * > PA_POWER_RAMP (PA power output) dflt - +14 dBm
 * > FIRST_IPL  dflt - 2
 * > SECOND_IPL  dflt - 5
 * > RAMP_SHAPE  dflt - 2
 * > AGC_ASK_BW
 * > ASK_DEPTH
 * > PKT_LEN (In variable pkt len mode, max allowed length) dflt - 0x0  [************]  --- Done
 *
 * > eWOR
 *
 * > RX Sniff Mode
 * >
 * > XOSC_CORE_PD_OVERRIDE (XOSC power down control) dflt - 0x0 (can be turned off to save power)  --- Done
 */


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void CC1200_recoverFromFIFOErr(void)
{
   CC1200_state_t cc1200State;

   do
   {
       PLTFRM_delay1MilliSec();
       CC1200_getCurrState(&cc1200State);
   } while (cc1200State != CC1200_STATE_IDLE);

   // Put the radio back in RX state
   if (CC1200_startRx() != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);

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
void CC1200_checkRadioErrors(void)
{
   CC1200_state_t cc1200State;

   do
   {
      if (CC1200_getCurrState(&cc1200State) != PLTFRM_STS_SUCCESS)
      {
          SYS_fatal(SYS_FATAL_ERR_1);
      }
      else
      {
          switch (cc1200State)
          {
             case CC1200_STATE_RX_FIFO_ERR:
                  {
                     /*
                      * If the radio tries to write to the RX FIFO after it is full, the
                      * the RXFIFO_OVERFLOW signal will be asserted and the radio will
                      * enter the RX_FIFO_ERR state.
                      * We will flush the RX FIFO here by issuing the SFRX strobe which
                      * will put the radio in IDLE state.
                      */

#ifdef MAC_STATS_ENA
                     UINT8_t regVal;
                     if (CC1200_readExtAddrSpaceReg(CC1200_MODEM_STATUS1_EXT_REG_ADDR,
                                                    &regVal) != PLTFRM_STS_SUCCESS)
                         SYS_fatal(SYS_FATAL_ERR_1);
                     if (regVal & CC1200_MODEM_STATUS1_RXFIFO_OVERFLOW_BIT)
                         MAC_cntxt.rxFifoOvflErrCnt ++;
                     else
                     {
                         if (regVal & CC1200_MODEM_STATUS1_RXFIFO_UNDERFLOW_BIT)
                             MAC_cntxt.rxFifoUnflErrCnt ++;
                     }
#endif
                     if (CC1200_flushRxFIFO() != PLTFRM_STS_SUCCESS)
                         SYS_fatal(SYS_FATAL_ERR_1);
                     if (CC1200_flushTxFIFO() != PLTFRM_STS_SUCCESS)
                         SYS_fatal(SYS_FATAL_ERR_1);

                     CC1200_recoverFromFIFOErr();
                  }
                  break;

             case CC1200_STATE_TX_FIFO_ERR:
                  {
                     /*
                      * If the TX FIFO is tried written when it is full or if the TX FIFO
                      * runs empty in the middle of a packet, the TXFIFO_OVERFLOW and
                      * TXFIFO_UNDERFLOW signals will be asserted and the radio will enter
                      * the TX_FIFO_ERR state.
                      * We will flush the TX FIFO here by issuing the SFTX strobe which
                      * will put the radio in IDLE state.
                      * This should never occur !!
                      */
#ifdef MAC_STATS_ENA
                     UINT8_t regVal;
                     if (CC1200_readExtAddrSpaceReg(CC1200_MODEM_STATUS1_EXT_REG_ADDR,
                                                    &regVal) != PLTFRM_STS_SUCCESS)
                         SYS_fatal(SYS_FATAL_ERR_1);
                     if (regVal & CC1200_MODEM_STATUS0_TXFIFO_OVERFLOW_BIT)
                         MAC_cntxt.txFifoOvflErrCnt ++;
                     else
                     {
                         if (regVal & CC1200_MODEM_STATUS0_TXFIFO_UNDERFLOW_BIT)
                             MAC_cntxt.txFifoUnflErrCnt ++;
                     }
#endif
                     if (CC1200_flushTxFIFO() != PLTFRM_STS_SUCCESS)
                         SYS_fatal(SYS_FATAL_ERR_1);
                     if (CC1200_flushRxFIFO() != PLTFRM_STS_SUCCESS)
                         SYS_fatal(SYS_FATAL_ERR_1);

                     CC1200_recoverFromFIFOErr();
                  }
                  break;

             default:
                  break;
         }
      }
   } while (0);

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
PLTFRM_sts_t CC1200_cfgContTrxMode(void)
{
   PLTFRM_sts_t sts;
   UINT8_t byteCnt0;

   /*
    * After putting the device into un-modulated, continuous TX mode with
    * the settings found using SmartRF Studio, use a spectrum analyzer to
    * measure the exact carrier frequency coming out of the chip.
    */

   /*
    * Custom frequency modulation enable
    */
   sts = CC1200_updateRegField(CC1200_MDMCFG2_EXT_REG_ADDR,
                               0x1, CC1200_MDMCFG2_CFM_DATA_EN_BIT_MSK,
                               CC1200_MDMCFG2_CFM_DATA_EN_SHIFT,
                               CC1200_MDMCFG2_CFM_DATA_EN_BIT_MSK_SHIFT);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   /*
    * Disable PA ramping.
    */
   sts = CC1200_updateRegField(CC1200_PA_CFG1_REG_ADDR,
                               0x0, CC1200_PA_CFG1_RAMP_SHAPE_EN_BIT_MSK,
                               CC1200_PA_CFG1_RAMP_SHAPE_EN_BIT_SHIFT,
                               CC1200_PA_CFG1_RAMP_SHAPE_EN_BIT_MSK_SHIFT);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   /*
    * Set the packet length config to "infinite packet length".
    */
   sts = CC1200_updateRegField(CC1200_PKT_CFG0_REG_ADDR,
                               (UINT8_t)CC1200_INFINITE_PACKET_LENGTH_MODE,
                               CC1200_PKT_CFG0_LEN_CFG_BIT_MSK,
                               CC1200_PKT_CFG0_LEN_CFG_SHIFT,
                               CC1200_PKT_CFG0_LEN_CFG_BIT_MSK_SHIFT);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   /*
    * Disable CRC
    */
   sts = CC1200_updateRegField(CC1200_PKT_CFG1_REG_ADDR,
                               0x0,
                               CC1200_PKT_CFG1_CRC_CFG_BIT_MSK,
                               CC1200_PKT_CFG1_CRC_CFG_BIT_SHIFT,
                               CC1200_PKT_CFG1_CRC_CFG_BIT_MSK_SHIFT);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   /*
    * Set PKT_FORMAT to random mode.
    * Send random data using PN9 generator (Set TXLAST != TXFIRST before
    * strobing STX)
    */
   sts = CC1200_updateRegField(CC1200_PKT_CFG2_REG_ADDR,
                               CC1200_PKT_FORMAT_RANDOM_MODE,
                               CC1200_PKT_CFG2_REG_PKT_FMT_BIT_MSK,
                               CC1200_PKT_CFG2_REG_PKT_FMT_SHIFT,
                               CC1200_PKT_CFG2_REG_PKT_FMT_BIT_MSK_SHIFT);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   /*
    * Set preamble length to 0
    */
   sts = CC1200_cfgPreambleLen(0x0);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = CC1200_writeExtAddrSpaceReg(CC1200_TXFIRST_EXT_REG_ADDR, 0x0);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);

   sts = CC1200_writeExtAddrSpaceReg(CC1200_TXLAST_EXT_REG_ADDR, 0x1);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);

   sts = CC1200_getTXFIFOByteCnt(&byteCnt0);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);

   if (byteCnt0 != 1)
       SYS_fatal(SYS_FATAL_ERR_1);

   // Now strobe TX
   sts = CC1200_sendCmdStrobe(CC1200_CMD_STROBE_STX, NULL);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_1);
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
PLTFRM_sts_t CC1200_cfgChipRegs(void)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   const CC1200_chipRegInfo_s *info_p = &(CC1200_chipRegInfoList[0]);

   while (info_p->regAddr != 0xff)
   {
       if (info_p->isExtReg)
           sts = CC1200_updateExtAddrSpaceRegField(info_p->regAddr,
                                                   info_p->regVal,
                                                   0xff, 0x0, 0xff);
       else
           sts = CC1200_updateRegField(info_p->regAddr,
                                       info_p->regVal,
                                       0xff, 0x0, 0xff);

       if (sts != PLTFRM_STS_SUCCESS)
           break;

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
UINT8_t CC1200_getRandNr(void)
{
   UINT8_t regVal;
   PLTFRM_sts_t sts;

   /*
    * < 9.11 Random Number Generator >
    * A random number generator is available and can be enabled by setting
    * RNDGEN.RNDGEN_EN = 1. A random number can be read from
    * RNDGEN.RNDGEN_VALUE in any state, but the number will be further
    * randomized when in RX by XORing the feedback with receiver noise.
    *
    * This register is not retained in sleep mode.
    */
   sts = CC1200_readExtAddrSpaceReg(CC1200_RNDGEN_EXT_REG_ADDR, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);

   if ((regVal & CC1200_RNDGEN_EN_BIT_MSK) == 0x0)
       SYS_fatal(SYS_FATAL_ERR_1);

   regVal &= CC1200_RNDGEN_VALUE_BIT_MSK;

   return regVal;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT16_t CC1200_calcPktTrxTime(UINT16_t pduLen)
{
   UINT16_t pktTrxTime;

   if (pduLen == 0)
   {
       if (!(SYS_BD_HNDL_IS_VALID(MAC_cntxt.currTxPduHndl)))
       {
           SYS_fatal(SYS_FATAL_ERR_1);
       }

       pduLen = SYS_GET_BD_LEN(MAC_cntxt.currTxPduHndl);
   }

   if (pduLen == 0)
       SYS_fatal(SYS_FATAL_ERR_1);

   pduLen <<= 3;
   pduLen += ((CC1200_FRAME_CRC_FIELD_LEN * 8)
		      + (CC1200_framePreambleLen)
		      + (CC1200_FRAME_LENGTH_FIELD_LEN * 8)
		      + CC1200_syncModeBitCntList[(UINT8_t)CC1200_syncMode]);

   // Assume data rate = symbol rate

#if (CC1200_CFG_SYM_RATE_SPS == 1200)
   // 8 bits takes (8*1000)/1200 = 6.66 milli seconds ~ 7 milli-seconds
   pktTrxTime = pduLen;
   pktTrxTime >>= 3;
   pktTrxTime *= 7;
#elif (CC1200_CFG_SYM_RATE_SPS == 38400)
   // n bits take (n x 1000000)/38400 microsecs = n * 26.041 microsecs
   // = n*26 + n*.041 < n*26 + 54
   // assuming n can be at most (30 + 4 + 128 + 2)*8 -> 164*8 bits
   // Note that preamble length can be at most 30 bytes. Sync word can be up to 4 bytes long.
   pktTrxTime = pduLen;
   pktTrxTime *= 26;
   pktTrxTime += 54;
   pktTrxTime >>= 10;  // In millisecs
   pktTrxTime += 2;
#else
#error Symbol rate not configured !!
#endif

   return pktTrxTime;
}


UINT8_t CC1200_dbgfreqOff0, CC1200_dbgfreqOff1;
UINT8_t CC1200_dbgReadAllRegs = 1;
UINT8_t CC1200_regList[48];
UINT8_t CC1200_extRegList[0xdb];

void CC1200_dbgReadRegs()
{
   if (CC1200_dbgReadAllRegs)
   {
       UINT8_t addrReg;

       CC1200_dbgReadAllRegs = 0;

	   for (addrReg=0x0; addrReg <= 0x2f; addrReg++)
       {
		    if (CC1200_readReg(addrReg, &CC1200_regList[addrReg]) != PLTFRM_STS_SUCCESS)
                SYS_fatal(SYS_FATAL_ERR_1);
       }

	   for (addrReg=0x0; addrReg <= 0xda; addrReg++)
       {
		    if (CC1200_readExtAddrSpaceReg(addrReg, &CC1200_extRegList[addrReg]) != PLTFRM_STS_SUCCESS)
                SYS_fatal(SYS_FATAL_ERR_1);
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
void CC1200_writeToTxFifo(UINT8_t count, UINT8_t *data_p)
{
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
PLTFRM_sts_t CC1200_spiInit(void)
{
   PLTFRM_sts_t sts;

   // CC1200 CSn - default HIGH

   sts = PLTFRM_allocGPIOPin(PLTFRM_CC1200_1_DEV_ID,
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
PLTFRM_sts_t CC1200_init(void)
{
   PLTFRM_sts_t sts;
   UINT8_t regVal;
   CC1200_state_t state;

   if ((sts = CC1200_spiInit( )) != PLTFRM_STS_SUCCESS)
       return sts;

   /*
    * The four digital I/O pins GPIO0, GPIO1, GPIO2 and GPIO3 are general
    * control pins configured with IOCFGx.GPIOx_CFG (where x is 0, 1, 2, or
    * 3). Table 10 shows the different signals that can be monitored on the
    * GPIO pins.
    *
    * IOCFG2 - GPIO2 Pin Configuration
    * GPIO2_CFG bits - Output selection (Default: PKT_CRC_OK)
    *
    * GPIO1 is shared with the SO pin in the SPI interface.
    *
    * CC120X can be configured to signal the MCU that a packet has been
    * received by using a GPIO pin.
    *
    * CRC_OK - Asserted simultaneously as PKT_CRC_OK. De-asserted when the
    * first byte is read from the RX FIFO.
    *
    * PKT_CRC_OK - Asserted in RX when PKT_CFG1.CRC_CFG != 1 and a good pkt
    * is received. The signal is de-asserted when RX mode is entered and
    * PKT_CFG1.CRC_CFG != 0.
    */

   // RESETn pin
   sts = PLTFRM_allocGPIOPin(PLTFRM_CC1200_1_DEV_ID,
                             PLTFRM_GPIO_PORT_4,
                             PLTFRM_GPIO_PORT_PIN_1,
                             PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                             PLTFRM_GPIO_OUTPUT_LOW,
                             PLTFRM_GPIO_INT_EDGE_NA,
                             NULL);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   do
   {
#ifdef CC1200_HW_RESET_FUNC_ENA
      CC1200_hwResetEna();

      {
         SINT16_t idx;
         for (idx=0; idx<10; idx++)
            PLTFRM_delay1MilliSec();
      }

      CC1200_hwResetDis();
#else
      // Reset chip
      sts = CC1200_sendCmdStrobe(CC1200_CMD_STROBE_SRES, NULL);
      if (sts != PLTFRM_STS_SUCCESS)
    	  SYS_fatal(SYS_FATAL_ERR_1);
#endif

      {
         SINT16_t idx;
         for (idx=0; idx<10; idx++)
             PLTFRM_delay1MilliSec();
      }

      if (CC1200_getCurrState(&state) == PLTFRM_STS_SUCCESS)
      {
          if (state == CC1200_STATE_IDLE)
          {
              CC1200_setCurrState(CC1200_STATE_IDLE);
          }
      }
   } while (state != CC1200_STATE_IDLE);


   CC1200_gpioInit();

   do
   {
      sts = CC1200_readExtAddrSpaceReg(CC1200_PART_NUMBER_EXT_REG_ADDR, &regVal);
      if (sts != PLTFRM_STS_SUCCESS)
          SYS_fatal(SYS_FATAL_ERR_1);
      if (regVal == CC1200_PART_NUMBER)
          break;
   } while (1);

   // Configure all the registers other than the center frequency and output
   // power level.
   sts = CC1200_cfgChipRegs();
   if (sts != PLTFRM_STS_SUCCESS)
	   SYS_fatal(SYS_FATAL_ERR_1);

   sts = CC1200_cfgPreambleLen(CC1200_framePreambleLen);
   if (sts != PLTFRM_STS_SUCCESS)
	   SYS_fatal(SYS_FATAL_ERR_1);

   sts = CC1200_cfgSyncWord(CC1200_syncMode);
   if (sts != PLTFRM_STS_SUCCESS)
	   SYS_fatal(SYS_FATAL_ERR_1);

   sts = CC1200_enaRndGen();
   if (sts != PLTFRM_STS_SUCCESS)
	   SYS_fatal(SYS_FATAL_ERR_1);

   CC1200_cntxt.pktLenMode = CC1200_VARIABLE_PACKET_LENGTH_MODE_0;

   CC1200_dbgReadRegs();

   return sts;
}

#endif
