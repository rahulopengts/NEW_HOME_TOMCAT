/*
 * File Name : cc1200.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : march/4/2014
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

#ifndef __CC1200_H__
#define __CC1200_H__

#include <typedefs.h>
#include <pltfrm.h>

#define CC1200_INTR_STATS_ENA


#define CC1200_TX_TO_RX_TURN_AROUND_TIME_MSECS   20    // Measured value < 20 msecs


// --------------------------------------------------
// TXFIFO_THR
// --------------------------------------------------
#define CC1200_enaGPIO0Int(  )  \
do                              \
{                               \
   P1IFG &= ~(BIT0);            \
   P1IE |= BIT0;                \
} while (0)

#define CC1200_disGPIO0Int(  )  \
do                              \
{                               \
   P1IE &= ~BIT0;               \
} while (0)
// --------------------------------------------------


// --------------------------------------------------
// TXONCCA_DONE
// --------------------------------------------------
#define CC1200_enaGPIO2Int(  )  \
do                              \
{                               \
   P1IFG &= ~(BIT1);            \
   P1IE |= BIT1;                \
} while (0)

#define CC1200_disGPIO2Int(  )  \
do                              \
{                               \
   P1IE &= ~BIT1;               \
} while (0)
// --------------------------------------------------


// --------------------------------------------------
// PKT_CRC_OK
// --------------------------------------------------
#define CC1200_enaGPIO3Int(  )  \
do                              \
{                               \
   P1IFG &= ~(BIT2);            \
   P1IE |= BIT2;                \
} while (0)

#define CC1200_disGPIO3Int(  )  \
do                              \
{                               \
   P1IE &= ~BIT2;               \
} while (0)
// --------------------------------------------------

/*
 * <PKT_CRC_OK> - Asserted in RX when PKT_CFG1.CRC_CFG = 1 or 10b
 * and a good packet is received. The signal is de-asserted when RX
 * mode is entered and PKT_CFG1.CRC_CFG != 0. This signal is always
 * on if the radio is in TX or if the radio is in RX and
 * PKT_CFG1.CRC_CFG = 0.
 *
 * At this point, TX has been entered so the signal is asserted. We
 * will wait for the signal to be de-asserted indicating that the
 * CC1200 has gone back to RX state. This macro will change the
 * interrupt edge to active low.
 */
#define CC1200_setUpRxEntryInt( )   \
do                                  \
{                                   \
   P1IE &= ~(BIT0);                 \
   P1IES |= (BIT0);                 \
   P1IFG &= ~(BIT0);                \
   P1IE |= BIT0;                    \
} while (0)

#if 0
   if ((P1IN & BIT0) == 0x0)
       SYS_fatal(SYS_FATAL_ERR_1);
#endif
/*
 * Configure the interrupt edge active high so that the micro gets an
 * interrupt when a packet with good CRC is received in the RX FIFO.
 * Active high - Clear PxIES.bity
 * Active low  - Set PxIES.bity
 */
#define CC1200_setUpPktRxInt( )     \
do                                  \
{                                   \
   P1IE &= ~BIT0;                   \
   P1IES &= ~(BIT0);                \
   P1IFG &= ~(BIT0);                \
   P1IE |= BIT0;                    \
} while (0)

/*
 * Output-Power = ((PA_POWER_RAMP + 1)/2) - 18  (In dBm)
 * 3 <= PA_POWER_RAMP <= 63
 * 0x0, 0x1 and 0x2 are special power levels.
 */
#define CC1200_MIN_OP_POWER_DBM  ((SINT16_t)-16)
#define CC1200_MAX_OP_POWER_DBM  ((SINT16_t)14)

#define CC1200_PART_NUMBER  0x20
#define CC1201_PART_NUMBER  0x21

#define PHY_865_MHZ_BAND_CHAN_1  0
#define PHY_865_MHZ_BAND_CHANNEL_CNT  2

/*
 * The Offset is dependent on the RX settings on the CC1200.
 * This value "99" has been obtained from smartRF studio
 * (continuous RX screen).
 *
 */
#define CC1200_RSSI_OFFSET_VALUE  99

#define CC1200_OSC_FREQ_IN_MHZ  40


#define CC1200_TX_FIFO_EMPTY_THRESHOLD   126

#define PHY_SYMBOL_LENGTH_USECS_SHIFT  4   // <TODO>

#define CC1200_PACKET_MODE_MIN_DATA_RATE_KBPS   0
#define CC1200_PACKET_MODE_MAX_DATA_RATE_KBPS   1250

#define CC1200_TRANSPARENT_MODE_MIN_DATA_RATE_KBPS   0
#define CC1200_TRANSPARENT_MODE_MAX_DATA_RATE_KBPS   1250

// A 2 byte CRC checksum computed over the data field
#define CC1200_FRAME_CRC_FIELD_LEN  2

// Optional 1 byte address field
#define CC1200_FRAME_ADDRESS_FIELD_LEN  1

// Optional 1 byte frame length field
#define CC1200_FRAME_LENGTH_FIELD_LEN  1

// Sync word
#define CC1200_FRAME_SYNC_FIELD_MAX_LEN  4

#define CC1200_FRAME_PREAMBLE_LEN  3

#define CC1200_FRAME_SYNC_WORD_LEN_BITS  32

/*
 * When using the auto flush function, the maximum packet length is 127
 * bytes in variable packet length mode and 128 bytes in fixed packet
 * length mode. Note that when PKT_CFG1.APPEND_STATUS is enabled, the
 * maximum allowed packet length is reduced by two bytes in order to make
 * room in the RX FIFO for the two status bytes appended at the end of the
 * packet. The MCU must not read from the current packet until the CRC has
 * been checked as OK.
 */
#define CC1200_MAX_MAC_PDU_LEN  (CC1200_RX_FIFO_LEN                 \
                                 - CC1200_FRAME_LENGTH_FIELD_LEN    \
                                 - CC1200_APPENDED_RSSI_INFO_LEN    \
                                 - CC1200_APPENDED_CRC_LQI_INFO_LEN)

   
// CC120X supports both fixed packet length protocols and variable
// packet length protocols. Variable or fixed packet length mode can
// be used for packets up to 255 bytes. For longer packets, infinite
// packet length mode must be used.
// The packet length is defined as the payload data and the optional
// address byte, excluding the optional length byte, the optional CRC,
// and the optional append status.
#define CC1200_MAX_PKT_LEN  255
#define CC1200_MAX_PYLD_LEN  (CC1200_MAX_PKT_LEN - CC1200_FRAME_ADDRESS_FIELD_LEN)

#define CC1200_PA_MIN_OUTPUT_PWR_DBM_10  ((SINT16_t)-175)    // -17.5 dBm
#define CC1200_PA_MAX_OUTPUT_PWR_DBM_10  ((SINT16_t)140)     // +14.0 dBm

#define CC1200_SPI_HDR_RW_BIT        0x80
#define CC1200_SPI_HDR_BURST_BIT     0x40
#define CC1200_SPI_HDR_ADDR_BIT_MSK  0x3f

// Normal mode/FIFO mode (MDMCFG1.FIFO_EN must be set to 1
// and MDMCFG0.TRANSPARENT_MODE_EN must be set to 0)
#define CC1200_DATA_COMM_MODE_FIFO                0x0

// Synchronous serial mode (MDMCFG1.FIFO_EN must be set to 
// 0 and MDMCFG0.TRANSPARENT_MODE_EN must be set to 0).
#define CC1200_DATA_COMM_MODE_SYNCH_SERIAL        0x1

// Random mode. Send random data using PN9 generator
#define CC1200_DATA_COMM_MODE_RANDOM              0x2

// Transparent serial mode (MDMCFG1.FIFO_EN must be set to 0 and
// MDMCFG0.TRANSPARENT_MODE_EN must be set to 1). This mode is
// only supported for 2’ary modulations formats.
#define CC1200_DATA_COMM_MODE_TRANSPARENT_SERIAL  0x3

// FS_CFG register
#define CC1200_FS_CFG_LOCK_EN_BIT_MSK      0x10
#define CC1200_FS_CFG_LOCK_EN_SHIFT        4
#define CC1200_FS_CFG_LOCK_EN_BIT_MSK_SHIFT  0x1

#define CC1200_FS_CFG_BAND_SELECT_BIT_MSK  0x0f

// PKT_CFG2 register
#define CC1200_PKT_CFG2_REG_PKT_FMT_BIT_MSK  0x3
#define CC1200_PKT_CFG2_REG_PKT_FMT_BIT_MSK_SHIFT  0x3
#define CC1200_PKT_CFG2_REG_PKT_FMT_SHIFT  0x0

#define CC1200_PKT_CFG2_CCA_MODE_SHIFT  2
#define CC1200_PKT_CFG2_CCA_MODE_BIT_MSK_SHIFT 0x7
#define CC1200_PKT_CFG2_CCA_MODE_BIT_MSK  0x1c

// MDMCFG1 register
#define CC1200_MDMCFG1_REG_CS_GATE_BIT              0x80   // Bit 7
#define CC1200_MDMCFG1_REG_FIFO_EN_BIT              0x40   // Bit 6
#define CC1200_MDMCFG1_REG_MANCHESTER_EN_BIT        0x20   // Bit 5
#define CC1200_MDMCFG1_REG_INVERT_DATA_EN_BIT       0x10   // Bit 4
#define CC1200_MDMCFG1_REG_COLLISION_DETECT_EN_BIT  0x08   // Bit 3
#define CC1200_MDMCFG1_REG_DVGA_GAIN_BITS           0x06   // Bits 2:1
#define CC1200_MDMCFG1_REG_SINGLE_ADC_EN_BIT        0x01   // Bit 0

// MDMCFG0 register
#define CC1200_MDMCFG0_TRANSPARENT_MODE_EN_BIT      0x40
#define CC1200_MDMCFG0_TRANSPARENT_INTFACT_BIT_MSK  0x30
#define CC1200_MDMCFG0_DATA_FILTER_EN_BIT           0x08
#define CC1200_MDMCFG0_VITERBI_EN_BIT               0x04


/*
 * <Static modes>
 * Power down with retention - Low-power RC oscillator running (0.3 uA to 0.5 uA)
 * XOFF mode - Crystal oscillator / TCXO disabled (180 microamps)
 * IDLE mode - Clock running, system waiting with no radio activity (1.5 mA)
 */

typedef enum
{
   CC1200_STATE_UNKNOWN,
   CC1200_STATE_POWER_DOWN,
   CC1200_STATE_XOFF_MODE,
   CC1200_STATE_SLEEPING,
   CC1200_STATE_IDLE,
   CC1200_STATE_FSTXON,
   CC1200_STATE_CALIBRATE,
   CC1200_STATE_SETTLING,
   CC1200_STATE_RX,
   CC1200_STATE_TX_IN_PROGRESS,
   CC1200_STATE_TX_FIFO_ERR,
   CC1200_STATE_RX_FIFO_ERR
} CC1200_state_t;


// Table 10
#define CC1200_GPIOx_CFG_SIG_RXFIFO_THR             0
#define CC1200_GPIOx_CFG_SIG_RXFIFO_THR_PKT         1
#define CC1200_GPIOx_CFG_SIG_TXFIFO_THR             2
#define CC1200_GPIOx_CFG_SIG_TXFIFO_THR_PKT         3
#define CC1200_GPIOx_CFG_SIG_RXFIFO_OVERFLOW        4
#define CC1200_GPIOx_CFG_SIG_TXFIFO_UNDERFLOW       5
#define CC1200_GPIOx_CFG_SIG_PKT_SYNC_RXTX          6
#define CC1200_GPIOx_CFG_SIG_CRC_OK                 7
#define CC1200_GPIOx_CFG_SIG_SERIAL_CLK             8
#define CC1200_GPIOx_CFG_SIG_SERIAL_RX              9
#define CC1200_GPIOx_CFG_SIG_RSVD_1                 10
#define CC1200_GPIOx_CFG_SIG_PQT_REACHED            11
#define CC1200_GPIOx_CFG_SIG_PQT_VALID              12
#define CC1200_GPIOx_CFG_SIG_RSSI_VALID             13
#define CC1200_GPIOx_CFG_SIG_RSSI_UPDATE            14    // 3, 2
#define CC1200_GPIOx_CFG_SIG_AGC_HOLD               14    // 1
#define CC1200_GPIOx_CFG_SIG_AGC_UPDATE_0           14    // 0
#define CC1200_GPIOx_CFG_SIG_CCA_STATUS             15    // 3
#define CC1200_GPIOx_CFG_SIG_TXONCCA_DONE           15    // 2
#define CC1200_GPIOx_CFG_SIG_CCA_STATUS             15    // 1
#define CC1200_GPIOx_CFG_SIG_TXONCCA_FAILED         15    // 0
#define CC1200_GPIOx_CFG_SIG_CARRIER_SENSE_VALID    16
#define CC1200_GPIOx_CFG_SIG_CARRIER_SENSE          17
#define CC1200_GPIOx_CFG_SIG_DSSS_CLK               18  // 3
#define CC1200_GPIOx_CFG_SIG_DSSS_DATA0             18  // 2
#define CC1200_GPIOx_CFG_SIG_DSSS_CLK               18  // 1
#define CC1200_GPIOx_CFG_SIG_DSSS_DATA1             18  // 0
#define CC1200_GPIOx_CFG_SIG_PKT_CRC_OK             19
#define CC1200_GPIOx_CFG_SIG_MCU_WAKEUP             20
#define CC1200_GPIOx_CFG_SIG_SYNC_LOW0_HIGH1        21
#define CC1200_GPIOx_CFG_SIG_AES_COMMAND_ACTIVE     22  // 0
#define CC1200_GPIOx_CFG_SIG_LNA_PA_REG_PD          23
#define CC1200_GPIOx_CFG_SIG_LNA_PD                 24
#define CC1200_GPIOx_CFG_SIG_PA_PD                  25
#define CC1200_GPIOx_CFG_SIG_RX0TX1_CFG             26
#define CC1200_GPIOx_CFG_SIG_RSVD_2                 27
#define CC1200_GPIOx_CFG_SIG_IMAGE_FOUND            28
#define CC1200_GPIOx_CFG_SIG_CLKEN_CFM              29
#define CC1200_GPIOx_CFG_SIG_CFM_TX_DATA_CLK        30
#define CC1200_GPIOx_CFG_SIG_RSSI_STEP_FOUND        33
#define CC1200_GPIOx_CFG_SIG_AES_RUN                34  // 3, 2
#define CC1200_GPIOx_CFG_SIG_RSSI_STEP_EVENT        34  // 1, 0
#define CC1200_GPIOx_CFG_SIG_LOCK                   35  // 1, 0
#define CC1200_GPIOx_CFG_SIG_ANTENNA_SELECT         36
#define CC1200_GPIOx_CFG_SIG_MARC_2PIN_STATUS_1     37
#define CC1200_GPIOx_CFG_SIG_MARC_2PIN_STATUS_0     38
#define CC1200_GPIOx_CFG_SIG_TXFIFO_OVERFLOW        39  // 2
#define CC1200_GPIOx_CFG_SIG_RXFIFO_UNDERFLOW       39  // 0
#define CC1200_GPIOx_CFG_SIG_MAGN_VALID             40  // 3
#define CC1200_GPIOx_CFG_SIG_CHFILT_VALID           40  // 2
#define CC1200_GPIOx_CFG_SIG_RCC_CAL_VALID          40  // 1
#define CC1200_GPIOx_CFG_SIG_CHFILT_STARTUP_VALID   40  // 0
#define CC1200_GPIOx_CFG_SIG_COLLISION_FOUND        41  // 3, 1
#define CC1200_GPIOx_CFG_SIG_SYNC_EVENT             41  // 2
#define CC1200_GPIOx_CFG_SIG_COLLISION_EVENT        41  // 0
#define CC1200_GPIOx_CFG_SIG_PA_RAMP_UP             42
#define CC1200_GPIOx_CFG_SIG_CRC_FAILED             43  // 3
#define CC1200_GPIOx_CFG_SIG_LENGTH_FAILED          43  // 2
#define CC1200_GPIOx_CFG_SIG_ADDR_FAILED            43  // 1
#define CC1200_GPIOx_CFG_SIG_UART_FRAMING_ERROR     43  // 0
#define CC1200_GPIOx_CFG_SIG_AGC_STABLE_GAIN        44
#define CC1200_GPIOx_CFG_SIG_AGC_UPDATE_1           45
#define CC1200_GPIOx_CFG_SIG_ADC_CLOCK              46  // 3, 1
#define CC1200_GPIOx_CFG_SIG_ADC_Q_DATA_SAMPLE      46  // 2
#define CC1200_GPIOx_CFG_SIG_ADC_I_DATA_SAMPLE      46  // 0
#define CC1200_GPIOx_CFG_SIG_HIGHZ                  48
#define CC1200_GPIOx_CFG_SIG_EXT_CLOCK              49
#define CC1200_GPIOx_CFG_SIG_CHIP_RDYn              50
#define CC1200_GPIOx_CFG_SIG_HW0                    51
#define CC1200_GPIOx_CFG_SIG_CLOCK_40K              54
#define CC1200_GPIOx_CFG_SIG_WOR_EVENT0             55
#define CC1200_GPIOx_CFG_SIG_WOR_EVENT1             56
#define CC1200_GPIOx_CFG_SIG_WOR_EVENT2             57
#define CC1200_GPIOx_CFG_SIG_XOSC_STABLE            59
#define CC1200_GPIOx_CFG_SIG_EXT_OSC_EN             60



// Register
#define CC1200_MAIN_FSM_STATE_IDLE         0x0
#define CC1200_MAIN_FSM_STATE_RX           0x1
#define CC1200_MAIN_FSM_STATE_TX           0x2
#define CC1200_MAIN_FSM_STATE_FSTXON       0x3
#define CC1200_MAIN_FSM_STATE_CALIBRATE    0x4
#define CC1200_MAIN_FSM_STATE_SETTLING     0x5
#define CC1200_MAIN_FSM_STATE_TX_FIFO_ERR  0x6
#define CC1200_MAIN_FSM_STATE_RX_FIFO_ERR  0x7
#define CC1200_MAIN_FSM_STATE_UNKNOWN      0xf

// Table 34: RF Band Selection Decoding
#define CC1200_820_960_MHZ_LO_DIVIDER     4
#define CC1200_410_480_MHZ_LO_DIVIDER     8
#define CC1200_273PT3_320_MHZ_LO_DIVIDER  12
#define CC1200_205_240_MHZ_LO_DIVIDER     16
#define CC1200_164_192_MHZ_LO_DIVIDER     20
#define CC1200_136PT7_160_MHZ_LO_DIVIDER  24


// PA_CFG1
#define CC1200_PA_CFG1_RAMP_SHAPE_EN_BIT_MSK    0x40
#define CC1200_PA_CFG1_RAMP_SHAPE_EN_BIT_SHIFT  0x06
#define CC1200_PA_CFG1_RAMP_SHAPE_EN_BIT_MSK_SHIFT 0x1

#define CC1200_PA_CFG1_POWER_RAMP_BIT_MSK  0x3f   


// PA_CFG0
#define CC1200_PA_CFG0_FIRST_IPL_BIT_MSK    0xe0   
#define CC1200_PA_CFG0_SECOND_IPL_BIT_MSK   0x1c
#define CC1200_PA_CFG0_RAMP_SHAPE_BIT_MSK   0x03


// PKT_CFG0
#define CC1200_PKT_CFG0_LEN_CFG_SHIFT  5
#define CC1200_PKT_CFG0_LEN_CFG_BIT_MSK  0x60   // Bits 6 and 5
#define CC1200_PKT_CFG0_LEN_CFG_BIT_MSK_SHIFT  0x03

#define CC1200_PKT_CFG0_UART_MODE_CFG_BIT_MSK  0x02   // Bit 2
#define CC1200_PKT_CFG0_UART_MODE_CFG_BIT_MSK_SHIFT  0x1
#define CC1200_PKT_CFG0_UART_MODE_CFG_SHIFT  1


// MODCFG_DEV_E
#define CC1200_MODCFG_MODEM_MODE_BIT_MSK  0xc0  // Bits 7:6
#define CC1200_MODCFG_MODEM_MODE_BIT_MSK_SHIFT  0x3
#define CC1200_MODCFG_MODEM_MODE_SHIFT  6

#define CC1200_MODCFG_MOD_FMT_BIT_MSK  0x38   // Bits 5:3
#define CC1200_MODCFG_MOD_FMT_BIT_MSK_SHIFT  0x7
#define CC1200_MODCFG_MOD_FMT_SHIFT  3

#define CC1200_MODCFG_DEV_E_BIT_MSK  0x7  // Bits 2:0
#define CC1200_MODCFG_DEV_E_BIT_MSK_SHIFT  0x7
#define CC1200_MODCFG_DEV_E_SHIFT  0


#define CC1200_FIFO_CFG_CRC_AUTOFLUSH_BIT  0x80
#define CC1200_FIFO_CFG_CRC_AUTOFLUSH_SHIFT  7
#define CC1200_FIFO_CFG_CRC_AUTOFLUSH_BIT_MSK_SHIFT  0x1

#define CC1200_FIFO_CFG_FIFO_THR_BIT_MSK 0x7f


// SYMBOL_RATE2
#define CC1200_SYMBOL_RATE2_SRATE_E_BIT_MSK  0xf0
#define CC1200_SYMBOL_RATE2_SRATE_M_19_16_BIT_MSK  0x0f

// PREAMBLE_CFG1
#define CC1200_PREAMBLE_CFG1_NUM_PREAMBLE_SHIFT 2
#define CC1200_PREAMBLE_CFG1_NUM_PREAMBLE_BIT_MSK_SHIFT  0xf
#define CC1200_PREAMBLE_CFG1_NUM_PREAMBLE_BIT_MSK 0x3c  // Bits 5:2

// RFEND_CFG1
#define CC1200_RFEND_CFG1_RXOFF_MODE_SHIFT  4
#define CC1200_RFEND_CFG1_RXOFF_MODE_BIT_MSK_SHIFT  0x3
#define CC1200_RFEND_CFG1_RXOFF_MODE_BIT_MSK  0x30  // Bits 5:4

// RFEND_CFG0
#define CC1200_RFEND_CFG0_TXOFF_MODE_SHIFT  4
#define CC1200_RFEND_CFG0_TXOFF_MODE_BIT_MSK_SHIFT  0x3
#define CC1200_RFEND_CFG0_TXOFF_MODE_BIT_MSK  0x30  // Bits 5:4

#define CC1200_RFEND_CFG0_TERM_ON_BAD_PKT_EN_SHIFT  3
#define CC1200_RFEND_CFG0_TERM_ON_BAD_PKT_EN_BIT_MSK  0x8
#define CC1200_RFEND_CFG0_TERM_ON_BAD_PKT_EN_BIT_MSK_SHIFT 0x1

// XOSC2
#define CC1200_XOSC2_CORE_PD_OVERRIDE_BIT  0x1

// SYNC_CFG1
#define CC1200_SYNC_CFG1_SYNC_MODE_BIT_MSK  0xe0
#define CC1200_SYNC_CFG1_SYNC_MODE_SHIFT 5
#define CC1200_SYNC_CFG1_SYNC_MODE_BIT_MSK_SHIFT 0x7

#define CC1200_SYNC_CFG1_SYNC_THR_BIT_MSK  0x1f
#define CC1200_SYNC_CFG1_SYNC_THR_BIT_MSK_SHIFT  0x1f
#define CC1200_SYNC_CFG1_SYNC_THR_SHIFT  0

// Status byte
#define CC1200_STS_BYTE_CHIP_STATE_SHIFT  4
#define CC1200_STS_BYTE_CHIP_STATE_BIT_MSK_SHIFT 0x7  // Bits 6:4

#define CC1200_MARC_STATUS0_TXONCCA_FAILED_BIT  0x4  // Bit 2


// PREAMBLE_CFG0
#define CC1200_PREAMBLE_CFG0_PQT_EN_BIT_MSK  0x80
#define CC1200_PREAMBLE_CFG0_PQT_EN_SHIFT 7
#define CC1200_PREAMBLE_CFG0_PQT_EN_BIT_MSK_SHIFT  0x1

#define CC1200_PREAMBLE_CFG0_PQT_BIT_MSK  0x7f
#define CC1200_PREAMBLE_CFG0_PQT_SHIFT  0x0
#define CC1200_PREAMBLE_CFG0_PQT_BIT_MSK_SHIFT  0x7f

// RNDGEN
#define CC1200_RNDGEN_VALUE_BIT_MSK    0x7f

// PKT_CFG1
#define CC1200_PKT_CFG1_CRC_CFG_BIT_MSK  0x6
#define CC1200_PKT_CFG1_CRC_CFG_BIT_SHIFT  1
#define CC1200_PKT_CFG1_CRC_CFG_BIT_MSK_SHIFT  0x3


#define CC1200_RNDGEN_EN_BIT_MSK       0x80
#define CC1200_RNDGEN_EN_SHIFT          0x7
#define CC1200_RNDGEN_EN_BIT_MSK_SHIFT  0x1

// MDMCFG2
#define CC1200_MDMCFG2_CFM_DATA_EN_BIT_MSK  0x1
#define CC1200_MDMCFG2_CFM_DATA_EN_BIT_MSK_SHIFT  0x1
#define CC1200_MDMCFG2_CFM_DATA_EN_SHIFT  0x0

typedef enum
{
   CC1200_MODULATION_2_FSK = 0,
   CC1200_MODULATION_2_GFSK = 1,
   CC1200_MODULATION_ASK_OOK = 3,
   CC1200_MODULATION_4_FSK = 4,
   CC1200_MODULATION_4_GFSK = 5
} CC1200_modulationScheme_t;

typedef enum
{
   CC1200_FREQ_BAND_820_960_MHZ    = 0,
   CC1200_FREQ_BAND_410_480_MHZ    = 1,
   CC1200_FREQ_BAND_273PT3_320_MHZ = 2,
   CC1200_FREQ_BAND_205_240_MHZ    = 3,
   CC1200_FREQ_BAND_164_192_MHZ    = 4,
   CC1200_FREQ_BAND_136PT7_160_MHZ = 5,
   CC1200_FREQ_BAND_CNT            = 6
} CC1200_freqBand_t;

/*
 * The CC120X contains two 128 byte FIFOs, one for received data and one for 
 * transmit data. The SPI interface is used to read from the RX FIFO and write 
 * to the TX FIFO.
 */ 
#define CC1200_TX_FIFO_LEN  128
#define CC1200_RX_FIFO_LEN  128

#define CC1200_APPENDED_RSSI_INFO_LEN  1
#define CC1200_APPENDED_CRC_LQI_INFO_LEN   1


// Table 6: Command strobes
#define CC1200_CMD_STROBE_SRES     0x30
#define CC1200_CMD_STROBE_SFSTXON  0x31
#define CC1200_CMD_STROBE_SXOFF    0x32
#define CC1200_CMD_STROBE_SCAL     0x33
#define CC1200_CMD_STROBE_SRX      0x34
#define CC1200_CMD_STROBE_STX      0x35
#define CC1200_CMD_STROBE_SIDLE    0x36
#define CC1200_CMD_STROBE_SAFC     0x37
#define CC1200_CMD_STROBE_SWOR     0x38
#define CC1200_CMD_STROBE_SPWD     0x39
#define CC1200_CMD_STROBE_SFRX     0x3a
#define CC1200_CMD_STROBE_SFTX     0x3b
#define CC1200_CMD_STROBE_SWORRST  0x3c
#define CC1200_CMD_STROBE_SNOP     0x3d


// Figure 4: SPI Memory Map
#define CC1200_IOCFG3_REG_ADDR          0x00
#define CC1200_IOCFG2_REG_ADDR          0x01
#define CC1200_IOCFG1_REG_ADDR          0x02
#define CC1200_IOCFG0_REG_ADDR          0x03
#define CC1200_SYNC3_REG_ADDR           0x04
#define CC1200_SYNC2_REG_ADDR           0x05
#define CC1200_SYNC1_REG_ADDR           0x06
#define CC1200_SYNC0_REG_ADDR           0x07
#define CC1200_SYNC_CFG1_REG_ADDR       0x08
#define CC1200_SYNC_CFG0_REG_ADDR       0x09
#define CC1200_DEVIATION_M_REG_ADDR     0x0a
#define CC1200_MODCFG_DEV_E_REG_ADDR    0x0b
#define CC1200_DCFILT_CFG_REG_ADDR      0x0c
#define CC1200_PREAMBLE_CFG1_REG_ADDR   0x0d
#define CC1200_PREAMBLE_CFG0_REG_ADDR   0x0e
#define CC1200_IQIC_REG_ADDR            0x0f
#define CC1200_CHAN_BW_REG_ADDR         0x10
#define CC1200_MDMCFG1_REG_ADDR         0x11
#define CC1200_MDMCFG0_REG_ADDR         0x12
#define CC1200_SYMBOL_RATE2_REG_ADDR    0x13  // Mantissa [19:16]
#define CC1200_SYMBOL_RATE1_REG_ADDR    0x14  // Mantissa [15:8]
#define CC1200_SYMBOL_RATE0_REG_ADDR    0x15  // Mantissa [7:0]
#define CC1200_AGC_REF_REG_ADDR         0x16
#define CC1200_AGC_CS_THR_REG_ADDR      0x17
#define CC1200_AGC_GAIN_ADJUST_REG_ADDR 0x18
#define CC1200_AGC_CFG3_REG_ADDR        0x19
#define CC1200_AGC_CFG2_REG_ADDR        0x1a
#define CC1200_AGC_CFG1_REG_ADDR        0x1b
#define CC1200_AGC_CFG0_REG_ADDR        0x1c
#define CC1200_FIFO_CFG_REG_ADDR        0x1d	
#define CC1200_DEV_ADDR_REG_ADDR        0x1e
#define CC1200_SETTLING_CFG_REG_ADDR    0x1f
#define CC1200_FS_CFG_REG_ADDR          0x20
#define CC1200_WOR_CFG1_REG_ADDR        0x21
#define CC1200_WOR_CFG0_REG_ADDR        0x22
#define CC1200_WOR_EVENT0_MSB_REG_ADDR  0x23
#define CC1200_WOR_EVENT0_LSB_REG_ADDR  0x24
#define CC1200_RXDCM_TIME_REG_ADDR      0x25
#define CC1200_PKT_CFG2_REG_ADDR        0x26
#define CC1200_PKT_CFG1_REG_ADDR        0x27
#define CC1200_PKT_CFG0_REG_ADDR        0x28
#define CC1200_RFEND_CFG1_REG_ADDR      0x29
#define CC1200_RFEND_CFG0_REG_ADDR      0x2a
#define CC1200_PA_CFG1_REG_ADDR         0x2b
#define CC1200_PA_CFG0_REG_ADDR         0x2c
#define CC1200_ASK_CFG_REG_ADDR         0x2d
#define CC1200_PKT_LEN_REG_ADDR         0x2e


#define CC1200_EXT_REG_SPACE_ACCESS_CMD  0x2f

#define CC1200_STD_FIFO_ACCESS_CMD  0x3f


// Extended register space
#define CC1200_IF_MIX_CFG_EXT_REG_ADDR        0x00
#define CC1200_FREQOFF_CFG_EXT_REG_ADDR       0x01
#define CC1200_TOC_CFG_EXT_REG_ADDR           0x02
#define CC1200_MARC_SPARE_EXT_REG_ADDR        0x03
#define CC1200_ECG_CFG_EXT_REG_ADDR           0x04
#define CC1200_MDMCFG2_EXT_REG_ADDR           0x05
#define CC1200_EXT_CTRL_EXT_REG_ADDR          0x06
#define CC1200_RCCAL_FINE_EXT_REG_ADDR        0x07
#define CC1200_RCCAL_COARSE_EXT_REG_ADDR      0x08
#define CC1200_RCCAL_OFFSET_EXT_REG_ADDR      0x09
#define CC1200_FREQOFF1_EXT_REG_ADDR          0x0a
#define CC1200_FREQOFF0_EXT_REG_ADDR          0x0b
#define CC1200_FREQ2_EXT_REG_ADDR             0x0c
#define CC1200_FREQ1_EXT_REG_ADDR             0x0d
#define CC1200_FREQ0_EXT_REG_ADDR             0x0e
#define CC1200_IF_ADC2_EXT_REG_ADDR           0x0f
#define CC1200_IF_ADC1_EXT_REG_ADDR           0x10
#define CC1200_IF_ADC0_EXT_REG_ADDR           0x11
#define CC1200_FS_DIG1_EXT_REG_ADDR           0x12
#define CC1200_FS_DIG0_EXT_REG_ADDR           0x13
#define CC1200_FS_CAL3_EXT_REG_ADDR           0x14
#define CC1200_FS_CAL2_EXT_REG_ADDR           0x15
#define CC1200_FS_CAL1_EXT_REG_ADDR           0x16
#define CC1200_FS_CAL0_EXT_REG_ADDR           0x17
#define CC1200_FS_CHP_EXT_REG_ADDR            0x18
#define CC1200_FS_DIVTWO_EXT_REG_ADDR         0x19
#define CC1200_FS_DSM1_EXT_REG_ADDR           0x1a
#define CC1200_FS_DSM0_EXT_REG_ADDR           0x1b
#define CC1200_FS_DVC1_EXT_REG_ADDR           0x1c
#define CC1200_FS_DVC0_EXT_REG_ADDR           0x1d
#define CC1200_FS_LBI_EXT_REG_ADDR            0x1e
#define CC1200_FS_PFD_EXT_REG_ADDR            0x1f
#define CC1200_FS_PRE_EXT_REG_ADDR            0x20
#define CC1200_FS_REG_DIV_CML_EXT_REG_ADDR    0x21
#define CC1200_FS_SPARE_EXT_REG_ADDR          0x22
#define CC1200_FS_VCO4_EXT_REG_ADDR           0x23
#define CC1200_FS_VCO3_EXT_REG_ADDR           0x24
#define CC1200_FS_VCO2_EXT_REG_ADDR           0x25
#define CC1200_FS_VCO1_EXT_REG_ADDR           0x26
#define CC1200_FS_VCO0_EXT_REG_ADDR           0x27
#define CC1200_GBIAS6_EXT_REG_ADDR            0x28
#define CC1200_GBIAS5_EXT_REG_ADDR            0x29
#define CC1200_GBIAS4_EXT_REG_ADDR            0x2a
#define CC1200_GBIAS3_EXT_REG_ADDR            0x2b
#define CC1200_GBIAS2_EXT_REG_ADDR            0x2c
#define CC1200_GBIAS1_EXT_REG_ADDR            0x2d
#define CC1200_GBIAS0_EXT_REG_ADDR            0x2e
#define CC1200_IFAMP_EXT_REG_ADDR             0x2f
#define CC1200_LNA_EXT_REG_ADDR               0x30
#define CC1200_RXMIX_EXT_REG_ADDR             0x31
#define CC1200_XOSC5_EXT_REG_ADDR             0x32
#define CC1200_XOSC4_EXT_REG_ADDR             0x33
#define CC1200_XOSC3_EXT_REG_ADDR             0x34
#define CC1200_XOSC2_EXT_REG_ADDR             0x35
#define CC1200_XOSC1_EXT_REG_ADDR             0x36
#define CC1200_XOSC0_EXT_REG_ADDR             0x37
#define CC1200_ANALOG_SPARE_EXT_REG_ADDR      0x38
#define CC1200_PA_CFG3_EXT_REG_ADDR           0x39

#define CC1200_FREQOFF_EST1_EXT_REG_ADDR      0x77
#define CC1200_FREQOFF_EST0_EXT_REG_ADDR      0x78
#define CC1200_AGC_GAIN3_EXT_REG_ADDR         0x79
#define CC1200_AGC_GAIN2_EXT_REG_ADDR         0x7a
#define CC1200_AGC_GAIN1_EXT_REG_ADDR         0x7b
#define CC1200_AGC_GAIN0_EXT_REG_ADDR         0x7c
#define CC1200_CFM_RX_DATA_OUT_EXT_REG_ADDR   0x7d
#define CC1200_CFM_TX_DATA_IN_EXT_REG_ADDR    0x7e

#define CC1200_RNDGEN_EXT_REG_ADDR         0x80

#define CC1200_PART_NUMBER_EXT_REG_ADDR    0x8f
#define CC1200_PART_VERSION_EXT_REG_ADDR   0x90

#define CC1200_MODEM_STATUS1_EXT_REG_ADDR  0x92
#define CC1200_MODEM_STATUS0_EXT_REG_ADDR  0x93
#define CC1200_MARC_STATUS1_EXT_REG_ADDR   0x95
#define CC1200_MARC_STATUS0_EXT_REG_ADDR   0x95

#define CC1200_RXFIRST_EXT_REG_ADDR      0xd2
#define CC1200_TXFIRST_EXT_REG_ADDR      0xd3
#define CC1200_RXLAST_EXT_REG_ADDR       0xd4
#define CC1200_TXLAST_EXT_REG_ADDR       0xd5
#define CC1200_NUM_TXBYTES_EXT_REG_ADDR  0xd6
#define CC1200_NUM_RXBYTES_EXT_REG_ADDR  0xd7


#define CC1200_MODEM_STATUS1_RXFIFO_OVERFLOW_BIT   0x8
#define CC1200_MODEM_STATUS1_RXFIFO_UNDERFLOW_BIT  0x4

#define CC1200_MODEM_STATUS0_TXFIFO_OVERFLOW_BIT    0x2
#define CC1200_MODEM_STATUS0_TXFIFO_UNDERFLOW_BIT   0x1



// PKT_CFG2 - Packet configuration register 2
// -----------------------------------------------------------------------
// Always give a clear channel indication
#define CC1200_CCA_MODE_0  0x0

// Indicates clear channel when RSSI is below threshold
#define CC1200_CCA_MODE_1  0x1

// Indicates clear channel unless currently receiving a packet
#define CC1200_CCA_MODE_2  0x2

// Indicates clear channel when RSSI is below threshold and currently
// not receiving a packet
#define CC1200_CCA_MODE_3  0x3

// Indicates clear channel when RSSI is below threshold and ETSI LBT
// requirements are met
#define CC1200_CCA_MODE_4  0x4
// -----------------------------------------------------------------------

typedef enum
{
	CC1200_PKT_FORMAT_NORMAL_FIFO_MODE = 0,
	CC1200_PKT_FORMAT_SYNCHRONOUS_SERIAL_MODE = 1,
	CC1200_PKT_FORMAT_RANDOM_MODE = 2,
	CC1200_PKT_FORMAT_TRANSPARENT_SERIAL_MODE = 3
} CC1200_pktFormat_t;

typedef enum
{
   CC1200_FIXED_PACKET_LENGTH_MODE = 0,
   CC1200_VARIABLE_PACKET_LENGTH_MODE_0 = 1,
   CC1200_INFINITE_PACKET_LENGTH_MODE = 2,
   CC1200_VARIABLE_PACKET_LENGTH_MODE_1 = 3
} CC1200_pktLenMode_t;

// TXOFF mode - Determines the state the radio will enter after
// transmitting a packet
typedef enum
{
   CC1200_TX_OFF_MODE_IDLE = 0,
   CC1200_TX_OFF_MODE_FSTXON = 1,
   CC1200_TX_OFF_MODE_TX = 2,
   CC1200_TX_OFF_MODE_RX = 3
} CC1200_txOffMode_t;

// RXOFF mode. Determines the state the radio will enter after
// receiving a good packet
typedef enum
{
   CC1200_RX_OFF_MODE_IDLE = 0,
   CC1200_RX_OFF_MODE_FSTXON = 1,
   CC1200_RX_OFF_MODE_TX = 2,
   CC1200_RX_OFF_MODE_RX = 3
} CC1200_rxOffMode_t;

// Modem mode
typedef enum
{
   CC1200_MODEM_MODE_NORMAL = 0x0,
   CC1200_MODEM_MODE_DSSS_REPEAT_MODE = 0x1,
   CC1200_MODEM_MODE_DSSS_PN_MODE = 02,
   CC1200_MODEM_MODE_CARRIER_SENSE_MODE = 0x3
} CC1200_modemMode_t;

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
typedef enum
{
   CC1200_TERM_ON_BAD_PKT_DISABLED = 0,
   CC1200_TERM_ON_BAD_PKT_ENABLED = 1
} CC1200_termOnBadPktCfg_t;

typedef enum
{
   CC1200_DISABLE_CRC_AUTO_FLUSH = 0,
   CC1200_ENABLE_CRC_AUTO_FLUSH = 1
} CC1200_crcAutoFlushCfg_t;

typedef enum
{
   CC1200_XOSC_POWER_SAVE_CFG_ENA = 0,
   CC1200_XOSC_POWER_SAVE_CFG_DIS = 1
} CC1200_XoscPowerSaveCfg_t;

typedef enum
{
   CC1200_SYNC_MODE_0,
   CC1200_SYNC_MODE_11,
   CC1200_SYNC_MODE_16,
   CC1200_SYNC_MODE_18,
   CC1200_SYNC_MODE_24,
   CC1200_SYNC_MODE_32,
   CC1200_SYNC_MODE_16H,
   CC1200_SYNC_MODE_16D
} CC1200_syncMode_t;

typedef enum
{
	CC1200_PREAMBLE_DETECTION_DISABLED = 0,
	CC1200_PREAMBLE_DETECTION_ENABLED = 1
} CC1200_preambleDetectionCfg;

typedef enum
{
    CC1200_OUT_OF_LOCK_DETECTOR_DISABLED = 0,
    CC1200_OUT_OF_LOCK_DETECTOR_ENABLED = 1
} CC1200_outOfLockDetectorCfg_t;

typedef struct
{
   CC1200_syncMode_t mode;
   UINT8_t encoding;
} CC1200_syncModeEncListEntry_s;

typedef enum
{
   CC1200_UART_MODE_DISABLED = 0,
   CC1200_UART_MODE_ENABLED = 1
} CC1200_uartModeCfg_t;

#define CC1200_PREAMBLE_LENGTH_ENC_LIST_SZ  14

typedef struct
{
   UINT8_t  lenInBits;
   UINT8_t  enc;
} CC1200_preambleLenEncListEntry_s;


typedef struct
{
   UINT8_t regAddr;
   UINT8_t isExtReg;
   UINT8_t regVal;
} CC1200_chipRegInfo_s;


typedef struct
{
   CC1200_state_t  state;

   UINT8_t rxMaxFramePyldLen;

   CC1200_pktLenMode_t  pktLenMode;

   // TXOFF mode - Determines the state the radio will enter after
   // transmitting a packet
   CC1200_txOffMode_t txOffMode;

   // RXOFF mode. Determines the state the radio will enter after
   // receiving a good packet
   CC1200_rxOffMode_t rxOffMode;

   volatile UINT8_t txOnCCADoneFlag;

   CC1200_crcAutoFlushCfg_t crcAutoFlushCfg;

   CC1200_termOnBadPktCfg_t termOnBadPktCfg;

#ifdef CC1200_INTR_STATS_ENA
   UINT16_t CC1200_txFIFOThresholdIntCnt;
   UINT16_t CC1200_txOnCCADoneIntCnt;
   UINT16_t CC1200_pktCrcOkIntCnt;
#endif
} CC1200_cntxt_s;


extern CC1200_cntxt_s CC1200_cntxt;



#define CC1200_setCurrState(newState)  CC1200_cntxt.state = (newState)

extern void CC1200_freeTxFifoBD();

extern PLTFRM_sts_t CC1200_forceIdle(void);

extern PLTFRM_sts_t CC1200_getCurrState(CC1200_state_t *state_p);

extern PLTFRM_sts_t CC1200_sendCmdStrobe(UINT8_t cmdStrobeId, CC1200_state_t *state_p);

extern PLTFRM_sts_t CC1200_readExtAddrSpaceReg(UINT8_t regAddr, UINT8_t *regVal_p);

extern PLTFRM_sts_t CC1200_writeExtAddrSpaceReg(UINT8_t regAddr, UINT8_t regVal);

extern PLTFRM_sts_t CC1200_readReg(UINT8_t regAddr, UINT8_t *regVal_p);

extern PLTFRM_sts_t CC1200_transferPktToTxFifo(UINT8_t bdHndl);

extern PLTFRM_sts_t CC1200_readRxFIFO(UINT8_t cnt, UINT8_t *buff_p);

extern PLTFRM_sts_t CC1200_getRXFIFOByteCnt(UINT8_t *byteCnt_p);

extern PLTFRM_sts_t CC1200_flushRxFIFO(void);

extern PLTFRM_sts_t CC1200_startRx(void);

extern PLTFRM_sts_t CC1200_writeReg(UINT8_t regAddr, UINT8_t regVal);

extern PLTFRM_sts_t CC1200_readReg(UINT8_t regAddr, UINT8_t *regVal_p);

extern PLTFRM_sts_t CC1200_enableCCA(void);

extern PLTFRM_sts_t CC1200_disableCCA(void);

extern PLTFRM_sts_t CC1200_cfgFreq(UINT8_t channIdx);

extern void CC1200_wakeUp(void);

extern PLTFRM_sts_t CC1200_cfgXoscDuringSleep(CC1200_XoscPowerSaveCfg_t cfg);

extern PLTFRM_sts_t CC1200_spiInit(void);

extern PLTFRM_sts_t CC1200_init(void);

extern PLTFRM_sts_t CC1200_setTxPower(SINT16_t powerInDBm);

extern PLTFRM_sts_t CC1200_getTXFIFOByteCnt(UINT8_t *byteCnt_p);

extern UINT8_t CC1200_txPowerSetPointValid(SINT16_t powerInDbm);

extern void CC1200_writeToTxFifo(UINT8_t count, UINT8_t *data_p);

extern PLTFRM_sts_t CC1200_cfgChipRegs(void);

extern void CC1200_dbgReadRegs();

extern UINT16_t CC1200_calcPktTrxTime(UINT16_t pduLen);

extern PLTFRM_sts_t CC1200_writeBuffToTxFIFO(UINT8_t *buff_p, UINT8_t buffLen);

extern void CC1200_checkRadioErrors(void);

extern PLTFRM_sts_t CC1200_setUpTxDoneInt(void);

extern PLTFRM_sts_t CC1200_updateRegField(const UINT8_t regAddr,
                                          const UINT8_t newVal,
                                          const UINT8_t bitMsk,
                                          const UINT8_t bitShift,
                                          const UINT8_t bitMskShifted);

extern PLTFRM_sts_t CC1200_enaRndGen(void);

extern UINT8_t CC1200_getRandNr(void);

extern void CC1200_enterLPM(void);

#endif

