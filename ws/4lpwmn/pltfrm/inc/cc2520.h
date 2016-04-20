/*
 * File Name : cc2520.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : 8/2013
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

#ifndef __CC2520_H__
#define __CC2520_H__

#include <typedefs.h>
#include <pltfrm.h>


// Make a BIT address from register address and bit number.
#define CC2520_MAKE_BIT_ADDR(regAddr, bitIndex) (((regAddr) << 3) | (bitIndex))


/*
 ***********************************
 * 
 *  P2.1 -> RESET
 *  P2.2 -> VREG_EN
 *  P6.1 -> LED
 *
 *  <UCB0>
 *  P10.0 -> SCLK
 *  P10.5 -> SO
 *  P10.4 -> SI
 *  P10.3 -> STE/CSn
 *
 * VREG_EN
 * In Active mode the digital voltage regulator is on (VREG_EN=1) and 
 * the crystal oscillator clock is running.
 * The power down signals to the analog modules are controlled by the 
 * digital part.
 *
 *
 * RESETn
 * Power Up Sequence Using RESETn (recommended)
 * When the RESETn pin is used it must be held low until the internal 
 * regulator has stabilized. This typically takes 0.1 ms. When the RESETn 
 * pin is set high, the crystal oscillator (in the CC2520 reference design) 
 * uses typically 0.2 ms to start. See section 6 for crystal specific params.
 *
 * - Tdres : Time required after VREG_EN is activated until RESETn is 
 *   released or CSn is set low.
 *
 * - Txr : Time required for internal XOSC to stabilize after RESETn is 
 *   released or SXOSCON strobe is issued
 ***********************************
 */

#ifdef WISENSE_HW_REV_2     
   
#define RESET_ENA()            \
do                             \
{                              \
    P2OUT &= ~(0x4);           \
} while (0)


#define RESET_DIS()            \
do                             \
{                              \
    P2OUT |= (0x4);            \
} while (0)


// When high, digital voltage regulator is active.
#define VREG_ON()              \
do                             \
{                              \
    P2OUT |= (0x2);            \
} while (0)


#define VREG_OFF()                \
do                                \
{                                 \
    P2OUT &= ~(0x2);              \
} while (0)

#elif defined(WISENSE_HW_REV_3) || defined(WISENSE_HW_REV_4)

#define RESET_ENA()            \
do                             \
{                              \
    P3OUT &= ~(0x2);           \
} while (0)


#define RESET_DIS()            \
do                             \
{                              \
    P3OUT |= (0x2);            \
} while (0)


// When high, digital voltage regulator is active.
#define VREG_ON()              \
do                             \
{                              \
    P3OUT |= (0x1);            \
} while (0)


#define VREG_OFF()                \
do                                \
{                                 \
    P3OUT &= ~(0x1);              \
} while (0)
#elif defined(WISENSE_HW_REV_5) || defined(WISENSE_HW_REV_6)

#define RESET_ENA()            \
do                             \
{                              \
    P4OUT &= ~(BIT1);          \
} while (0)


#define RESET_DIS()            \
do                             \
{                              \
    P4OUT |= (BIT1);            \
} while (0)


// When high, digital voltage regulator is active.
#define VREG_ON()              \
do                             \
{                              \
    P4OUT |= (BIT2);            \
} while (0)


#define VREG_OFF()                \
do                                \
{                                 \
    P4OUT &= ~(BIT2);              \
} while (0)
#else
#error board hardware revision not specified !!
#endif  

#define CC2520_RX_FIFO_SZ  128

#define CC2520_MAX_MAC_PDU_LEN  (CC2520_RX_FIFO_SZ - 3)

#define CC2520_RSSI_OFFSET_VAL  76    // In dBm

#define CC2520_RCVD_PKT_RSSI_INFO_LEN  1
#define CC2520_RCVD_PKT_CORRELATION_INFO_LEN  1

// RSSI (1 byte) + (CRC-OK flag (1 bit) | correlation (7 bits))
#define CC2520_RX_FIFO_SIG_INFO_LEN  2

// Instruction implementation
#define CC2520_INS_SNOP                0x00
#define CC2520_INS_IBUFLD              0x02
#define CC2520_INS_SIBUFEX             0x03
#define CC2520_INS_SSAMPLECCA          0x04
#define CC2520_INS_SRES                0x0F
#define CC2520_INS_MEMRD               0x10
#define CC2520_INS_MEMWR               0x20
#define CC2520_INS_RXBUF               0x30
#define CC2520_INS_RXBUFCP             0x38
#define CC2520_INS_RXBUFMOV            0x32
#define CC2520_INS_TXBUF               0x3A
#define CC2520_INS_TXBUFCP             0x3E
#define CC2520_INS_RANDOM              0x3C
#define CC2520_INS_SXOSCON             0x40
#define CC2520_INS_STXCAL              0x41
#define CC2520_INS_SRXON               0x42
#define CC2520_INS_STXON               0x43
#define CC2520_INS_STXONCCA            0x44
#define CC2520_INS_SRFOFF              0x45
#define CC2520_INS_SXOSCOFF            0x46
#define CC2520_INS_SFLUSHRX            0x47
#define CC2520_INS_SFLUSHTX            0x48
#define CC2520_INS_SACK                0x49
#define CC2520_INS_SACKPEND            0x4A
#define CC2520_INS_SNACK               0x4B
#define CC2520_INS_SRXMASKBITSET       0x4C
#define CC2520_INS_SRXMASKBITCLR       0x4D
#define CC2520_INS_RXMASKAND           0x4E
#define CC2520_INS_RXMASKOR            0x4F
#define CC2520_INS_MEMCP               0x50
#define CC2520_INS_MEMCPR              0x52
#define CC2520_INS_MEMXCP              0x54
#define CC2520_INS_MEMXWR              0x56
#define CC2520_INS_BCLR                0x58
#define CC2520_INS_BSET                0x59
#define CC2520_INS_CTR                 0x60
#define CC2520_INS_CBCMAC              0x64
#define CC2520_INS_UCBCMAC             0x66
#define CC2520_INS_CCM                 0x68
#define CC2520_INS_UCCM                0x6A
#define CC2520_INS_ECB                 0x70
#define CC2520_INS_ECBO                0x72
#define CC2520_INS_ECBX                0x74
#define CC2520_INS_ECBXO               0x76
#define CC2520_INS_INC                 0x78
#define CC2520_INS_ABORT               0x7F
#define CC2520_INS_REGRD               0x80
#define CC2520_INS_REGWR               0xC0


// FREG definitions (BSET/BCLR supported)
#define CC2520_FRMFILT0                0x000
#define CC2520_FRMFILT1                0x001
#define CC2520_SRCMATCH                0x002
#define CC2520_SRCSHORTEN0             0x004
#define CC2520_SRCSHORTEN1             0x005
#define CC2520_SRCSHORTEN2             0x006
#define CC2520_SRCEXTEN0               0x008
#define CC2520_SRCEXTEN1               0x009
#define CC2520_SRCEXTEN2               0x00A
#define CC2520_FRMCTRL0                0x00C
#define CC2520_FRMCTRL1                0x00D
#define CC2520_RXENABLE0               0x00E
#define CC2520_RXENABLE1               0x00F
#define CC2520_EXCFLAG0                0x010
#define CC2520_EXCFLAG1                0x011
#define CC2520_EXCFLAG2                0x012
#define CC2520_EXCMASKA0               0x014
#define CC2520_EXCMASKA1               0x015
#define CC2520_EXCMASKA2               0x016
#define CC2520_EXCMASKB0               0x018
#define CC2520_EXCMASKB1               0x019
#define CC2520_EXCMASKB2               0x01A
#define CC2520_EXCBINDX0               0x01C
#define CC2520_EXCBINDX1               0x01D
#define CC2520_EXCBINDY0               0x01E
#define CC2520_EXCBINDY1               0x01F
#define CC2520_GPIOCTRL0               0x020
#define CC2520_GPIOCTRL1               0x021
#define CC2520_GPIOCTRL2               0x022
#define CC2520_GPIOCTRL3               0x023
#define CC2520_GPIOCTRL4               0x024
#define CC2520_GPIOCTRL5               0x025
#define CC2520_GPIOPOLARITY            0x026
#define CC2520_GPIOCTRL                0x028
#define CC2520_DPUCON                  0x02A
#define CC2520_DPUSTAT                 0x02C
#define CC2520_FREQCTRL                0x02E
#define CC2520_FREQTUNE                0x02F
#define CC2520_TXPOWER                 0x030
#define CC2520_TXCTRL                  0x031
#define CC2520_FSMSTAT0                0x032
#define CC2520_FSMSTAT1                0x033
#define CC2520_FIFOPCTRL               0x034
#define CC2520_FSMCTRL                 0x035
#define CC2520_CCACTRL0                0x036
#define CC2520_CCACTRL1                0x037
#define CC2520_RSSI                    0x038
#define CC2520_RSSISTAT                0x039
#define CC2520_TXFIFO_BUF              0x03A
#define CC2520_RXFIRST                 0x03C
#define CC2520_RXFIFOCNT               0x03E
#define CC2520_TXFIFOCNT               0x03F

// SREG definitions (BSET/BCLR unsupported)
#define CC2520_CHIPID                  0x040
#define CC2520_VERSION                 0x042
#define CC2520_EXTCLOCK                0x044
#define CC2520_MDMCTRL0                0x046
#define CC2520_MDMCTRL1                0x047
#define CC2520_FREQEST                 0x048
#define CC2520_RXCTRL                  0x04A
#define CC2520_FSCTRL                  0x04C
#define CC2520_FSCAL0                  0x04E
#define CC2520_FSCAL1                  0x04F
#define CC2520_FSCAL2                  0x050
#define CC2520_FSCARL0 L3                  0x051
#define CC2520_AGCCT               0x052
#define CC2520_AGCCTRL1                0x053
#define CC2520_AGCCTRL2                0x054
#define CC2520_AGCCTRL3                0x055
#define CC2520_ADCTEST0                0x056
#define CC2520_ADCTEST1                0x057
#define CC2520_ADCTEST2                0x058
#define CC2520_MDMTEST0                0x05A
#define CC2520_MDMTEST1                0x05B
#define CC2520_DACTEST0                0x05C
#define CC2520_DACTEST1                0x05D
#define CC2520_ATEST                   0x05E
#define CC2520_DACTEST2                0x05F
#define CC2520_PTEST0                  0x060
#define CC2520_PTEST1                  0x061
#define CC2520_RESERVED                0x062
#define CC2520_DPUBIST                 0x07A
#define CC2520_ACTBIST                 0x07C
#define CC2520_RAMBIST                 0x07E

// RAM size definitions
#define CC2520_RAM_START_ADDR          0x100
#define CC2520_RAM_SIZE                640

// RX and TX buffer definitions
#define CC2520_RAM_TXBUF               0x100
#define CC2520_RAM_RXBUF               0x180

// Security work buffer definitions
#define CC2520_RAM_CBCTEMPL            0x360
#define CC2520_RAM_CBCTEMPH            0x370

// Source address matching definitions
#define CC2520_RAM_SRCTABLEBASE        0x380
#define CC2520_RAM_SRCRESMASK0         0x3E0
#define CC2520_RAM_SRCRESMASK1         0x3E1
#define CC2520_RAM_SRCRESMASK2         0x3E2
#define CC2520_RAM_SRCRESINDEX         0x3E3
#define CC2520_RAM_SRCEXTPENDEN0       0x3E4
#define CC2520_RAM_SRCEXTPENDEN1       0x3E5
#define CC2520_RAM_SRCEXTPENDEN2       0x3E6
#define CC2520_RAM_SRCSHORTPENDEN0     0x3E7
#define CC2520_RAM_SRCSHORTPENDEN1     0x3E8
#define CC2520_RAM_SRCSHORTPENDEN2     0x3E9

// Local address definitions
#define CC2520_RAM_LOCAL_EXT_ADDR             0x3EA
#define CC2520_RAM_LOCAL_LPWMN_ID               0x3F2
#define CC2520_RAM_LOCAL_SHORT_ADDR           0x3F4

// Command strobes (identifiers to be used with GPIO and exception binding)
#define CC2520_STR_SIBUFEX             0x00
#define CC2520_STR_SRXMASKBITCLR       0x01
#define CC2520_STR_SRXMASKBITSET       0x02
#define CC2520_STR_SRXON               0x03
#define CC2520_STR_SSAMPLECCA          0x04
#define CC2520_STR_SACK                0x05
#define CC2520_STR_SACKPEND            0x06
#define CC2520_STR_SNACK               0x07
#define CC2520_STR_STXON               0x08
#define CC2520_STR_STXONCCA            0x09
#define CC2520_STR_SFLUSHRX            0x0A
#define CC2520_STR_SFLUSHTX            0x0B
#define CC2520_STR_SRXFIFOPOP          0x0C
#define CC2520_STR_STXCAL              0x0D
#define CC2520_STR_SRFOFF              0x0E
#define CC2520_STR_SXOSCOFF            0x0F

// Exceptions
#define CC2520_EXC_RF_IDLE             0
#define CC2520_EXC_TX_FRM_DONE         1
#define CC2520_EXC_TX_ACK_DONE         2
#define CC2520_EXC_TX_UNDERFLOW        3
#define CC2520_EXC_TX_OVERFLOW         4
#define CC2520_EXC_RX_UNDERFLOW        5
#define CC2520_EXC_RX_OVERFLOW         6
#define CC2520_EXC_RXENABLE_ZERO       7
#define CC2520_EXC_RX_FRM_DONE         8
#define CC2520_EXC_RX_FRM_ACCEPTED     9
#define CC2520_EXC_SRC_MATCH_DONE      10
#define CC2520_EXC_SRC_MATCH_FOUND     11
#define CC2520_EXC_FIFOP               12
#define CC2520_EXC_SFD                 13
#define CC2520_EXC_DPU_DONE_L          14
#define CC2520_EXC_DPU_DONE_H          15
#define CC2520_EXC_MEMADDR_ERROR       16
#define CC2520_EXC_USAGE_ERROR         17
#define CC2520_EXC_OPERAND_ERROR       18
#define CC2520_EXC_SPI_ERROR           19
#define CC2520_EXC_RF_NO_LOCK          20
#define CC2520_EXC_RX_FRM_ABORTED      21
#define CC2520_EXC_RXBUFMOV_TIMEOUT    22



// Table 9

#define CC2520_CTRLn_OUT_RF_IDLE           0x1
#define CC2520_CTRLn_OUT_TX_FRM_DONE       0x2
#define CC2520_CTRLn_OUT_TX_ACK_DONE       0x3
#define CC2520_CTRLn_OUT_TX_UNDERFLOW      0x4
#define CC2520_CTRLn_OUT_TX_OVERFLOW       0x5
#define CC2520_CTRLn_OUT_RX_UNDERFLOW      0x6
#define CC2520_CTRLn_OUT_RX_OVERFLOW       0x7
#define CC2520_CTRLn_OUT_RXENABLE_ZERO     0x8
#define CC2520_CTRLn_OUT_RX_FRM_DONE       0x9
#define CC2520_CTRLn_OUT_RX_FRM_ACCEPTED   0xa
#define CC2520_CTRLn_OUT_SRC_MATCH_DONE    0xb
#define CC2520_CTRLn_OUT_SRC_MATCH_FOUND   0xc
#define CC2520_CTRLn_OUT_FIFOP_EXCEPTION   0xd
#define CC2520_CTRLn_OUT_SFD_EXCEPTION     0xe
#define CC2520_CTRLn_OUT_DPU_DONE_L        0xf
#define CC2520_CTRLn_OUT_DPU_DONE_H        0x10
#define CC2520_CTRLn_OUT_MEMADDR_ERROR     0x11
#define CC2520_CTRLn_OUT_USAGE_ERROR       0x12
#define CC2520_CTRLn_OUT_OPERAND_ERROR     0x13
#define CC2520_CTRLn_OUT_SPI_ERROR         0x14
#define CC2520_CTRLn_OUT_RF_NO_LOCK        0x15
#define CC2520_CTRLn_OUT_RX_FRM_ABORTED    0x16
#define CC2520_CTRLn_OUT_RXBUFMOV_TIMEOUT  0x17
#define CC2520_CTRLn_OUT_UNUSED            0x18
#define CC2520_CTRLn_OUT_EXC_CHAN_A        0x21
#define CC2520_CTRLn_OUT_EXC_CHAN_B        0x22
#define CC2520_CTRLn_OUT_EXC_CHAN_A_COMP   0x23
#define CC2520_CTRLn_OUT_EXC_CHAN_B_COMP   0x23

#define CC2520_CTRLn_OUT_FIFO_SIGNAL          0x27
#define CC2520_CTRLn_OUT_FIFOP_SIGNAL         0x28
#define CC2520_CTRLn_OUT_CCA_SIGNAL           0x29
#define CC2520_CTRLn_OUT_SFD_SIGNAL           0x2a                  
#define CC2520_CTRLn_OUT_LOCK_SIGNAL          0x2b
#define CC2520_CTRLn_OUT_RSSI_VALID_SIGNAL    0x2c
#define CC2520_CTRLn_OUT_SAMPLED_CCA_SIGNAL   0x2d
#define CC2520_CTRLn_OUT_RAND_I               0x2e
#define CC2520_CTRLn_OUT_RAND_Q               0x2f




// Predefined exception channels
#define CC2520_EXC_CH_RX_BV           \
    (BV(CC2520_EXC_RX_UNDERFLOW) | BV(CC2520_EXC_RX_OVERFLOW) \
        | BV(CC2520_EXC_RX_FRM_ABORTED) | BV(CC2520_EXC_RXBUFMOV_TIMEOUT))
#define CC2520_EXC_CH_ERR_BV          \
    (BV(CC2520_EXC_MEMADDR_ERROR) | BV(CC2520_EXC_USAGE_ERROR) \
        | BV(CC2520_EXC_OPERAND_ERROR) | BV(CC2520_EXC_SPI_ERROR))

      
      
// FSMSTAT1 register bits
#define CC2520_FSMSTAT1_FIFO_BIT          (1 <<  7)
#define CC2520_FSMSTAT1_FIFOP_BIT         (1 <<  6)
#define CC2520_FSMSTAT1_SFD_BIT           (1 <<  5)
#define CC2520_FSMSTAT1_CCA_BIT           (1 <<  4)
#define CC2520_FSMSTAT1_SAMPLED_CCA_BIT   (1 <<  3)
#define CC2520_FSMSTAT1_LOCK_STATUS_BIT   (1 <<  2)
#define CC2520_FSMSTAT1_TX_ACTIVE_BIT     (1 <<  1)
#define CC2520_FSMSTAT1_RX_ACTIVE_BIT     (1 <<  0)      
      
// FSMSTAT0 register bits
#define CC2520_FSMSTAT0_FSM_FFCTRL_STATE_BIT_MSK   0x3f
#define CC2520_FSMSTAT0_CAL_RUNNING_BIT_MSK  0x1
#define CC2520_FSMSTAT0_CAL_DONE_BIT_MSK  0x1

#define CC2520_FSMSTAT0_FSM_FFCTRL_STATE_BIT_NR   0
#define CC2520_FSMSTAT0_CAL_RUNNING_BIT_NR  6
#define CC2520_FSMSTAT0_CAL_DONE_BIT_NR  7
      
// Table 18: FSM State Mapping
#define CC2520_RADIO_STATE_IDLE  0x0
#define CC2520_RADIO_STATE_RX_CALIBRATION  0x2
#define CC2520_RADIO_STATE_RX_OVERFLOW     0x11
#define CC2520_RADIO_STATE_SFD_WAIT        0x6
     
      
      
// EXCFLAG0 register bits
#define CC2520_EXCFLAG0_RF_IDLE_BIT        (1 << 0)
#define CC2520_EXCFLAG0_TX_FRM_DONE_BIT    (1 << 1)
#define CC2520_EXCFLAG0_TX_ACK_DONE        (1 << 2)
#define CC2520_EXCFLAG0_TX_UNDERFLOW       (1 << 3)
#define CC2520_EXCFLAG0_TX_OVERFLOW        (1 << 4)
#define CC2520_EXCFLAG0_RX_UNDERFLOW       (1 << 5)
#define CC2520_EXCFLAG0_RX_OVERFLOW        (1 << 6)
#define CC2520_EXCFLAG0_RXENABLE_ZERO      (1 << 7)
      
      
// FRMFILT0 register bits
#define CC2520_FRMFILT0_FRAME_FILTER_EN_BIT_NR   0
#define CC2520_FRMFILT0_LPWMN_COORDINATOR_BIT_NR   1

#define CC2520_FRMFILT0_FRAME_FILTER_EN_BIT_MSK  (1 << 0)
#define CC2520_FRMFILT0_LPWMN_COORDINATOR_BIT_MSK  (1 << 1)   
      
// FRMFILT1 register bits
#define CC2520_FRMFILT1_ACCEPT_FT_4TO7_RESVD_BIT_MSK    (1 << 7)
#define CC2520_FRMFILT1_ACCEPT_FT_3_MAC_CMD_BIT_MSK     (1 << 6)
#define CC2520_FRMFILT1_ACCEPT_FT_2_ACK_BIT_MSK         (1 << 5)
#define CC2520_FRMFILT1_ACCEPT_FT_1_DATA_BIT_MSK        (1 << 4)
#define CC2520_FRMFILT1_ACCEPT_FT_0_BEACON_BIT_MSK      (1 << 3)
#define CC2520_FRMFILT1_ACCEPT_FT_FILTER_MSK_BIT_MSK    ((1 << 2) | (1 << 1))     
  
#define CC2520_FRMFILT1_ACCEPT_FT_4TO7_RESVD_BIT_NR    (7)
#define CC2520_FRMFILT1_ACCEPT_FT_3_MAC_CMD_BIT_NR     (6)
#define CC2520_FRMFILT1_ACCEPT_FT_2_ACK_BIT_NR         (5)
#define CC2520_FRMFILT1_ACCEPT_FT_1_DATA_BIT_NR        (4)
#define CC2520_FRMFILT1_ACCEPT_FT_0_BEACON_BIT_NR      (3)
     
        
// RSSISTAT register bits
#define CC2520_RSSISTAT_RSSI_VALID_BIT_MSK   (1 << 0)
#define CC2520_RSSISTAT_RSSI_VALID_BIT_NR   0     


// FRMCTRL0 register bits
#define CC2520_FRMCTRL0_AUTOACK_BIT_NR   5
#define CC2520_FRMCTRL0_AUTOACK_BIT_MSK  (1 << 5)      
      
// FRMCTRL1 register bits     
#define CC2520_FRMCTRL1_PENDING_OR_BIT_NR          2
#define CC2520_FRMCTRL1_IGNORE_TX_UNDERF_BIT_NR    1
#define CC2520_FRMCTRL1_SET_RXENMASK_ON_TX_BIT_NR  0      

// GPIOPOLARITY register bits
#define CC2520_GPIOPOLARITY_GPIO0_BIT_MSK    (1 << 0)
#define CC2520_GPIOPOLARITY_GPIO1_BIT_MSK    (1 << 1)
#define CC2520_GPIOPOLARITY_GPIO2_BIT_MSK    (1 << 2)
#define CC2520_GPIOPOLARITY_GPIO3_BIT_MSK    (1 << 3)
#define CC2520_GPIOPOLARITY_GPIO4_BIT_MSK    (1 << 4)
#define CC2520_GPIOPOLARITY_GPIO5_BIT_MSK    (1 << 5)      
      
#define CC2520_SRCMATCH_SRC_MATCH_EN_BIT_NR  0      
#define CC2520_SRCMATCH_SRC_MATCH_EN_BIT_MSK  (1 << 0)
      
// TXPOWER regiter values
#define CC2520_TXPOWER_PLUS_5_DBM     0xf7 
#define CC2520_TXPOWER_PLUS_3_DBM     0xf2
#define CC2520_TXPOWER_PLUS_2_DBM     0xab
#define CC2520_TXPOWER_PLUS_1_DBM     0x13
#define CC2520_TXPOWER_0_DBM          0x32     
#define CC2520_TXPOWER_MINUS_2_DBM    0x81
#define CC2520_TXPOWER_MINUS_4_DBM    0x88
#define CC2520_TXPOWER_MINUS_7_DBM    0x2c
#define CC2520_TXPOWER_MINUS_18_DBM   0x03      
      
/*
 * If a short destination address is included in the frame, it must match 
 * either LOCAL_SHORT_ADDR or the broadcast address (0xFFFF).
 */
#define CC2520_setLocalShortAddr(shortAddr)  \
   CC2520_writeShort(CC2520_RAM_LOCAL_SHORT_ADDR, shortAddr) 
  
/*
 * If a destination LPWMN ID is included in the frame, it must match LOCAL_LPWMN_ID
 * or must be the broadcast LPWMN identifier (0xFFFF).
 */     
#define CC2520_setLocalLPWMNId(lpwmnId)  \
   CC2520_writeShort(CC2520_RAM_LOCAL_LPWMN_ID, lpwmnId)

/*
 * If an extended destination address is included in the frame, it must match 
 * LOCAL_EXT_ADDR.
 */     
#define CC2520_setLocalExtAddr(extAddr_p)  \
   cc2520_memWrite(CC2520_RAM_LOCAL_EXT_ADDR, MAC_EXT_ADDR_LEN, extAddr_p)


#ifdef CC2520_DFLT_LPM_MODE_LPM1
#define cc2520_enterLPM( )  cc2520_enterLPM1( )
#define cc2520_exitLPM( )  cc2520_exitLPM1( )
#elif defined(CC2520_DFLT_LPM_MODE_LPM2)
#define cc2520_enterLPM(  )  cc2520_enterLPM2( )
#define cc2520_exitLPM( )  cc2520_exitLPM2( )
#else
#error CC2520 low power mode not specifed !!
#endif
     
      
extern void cc2520_softRxStart(void);
extern void cc2520_hardRxStart(void);
extern void CC2520_readRxFifo(UINT8_t count, UINT8_t *data_p);
extern void cc2520_writeToTxFifo(UINT8_t count, UINT8_t *data_p);
extern UINT8_t cc2520_regRead8(UINT8_t addr);
extern void cc2520_insStrobe(UINT8_t strobe);
extern void cc2520_bitSet(UINT8_t bitAddr);
extern void cc2520_bitClear(UINT8_t bitAddr);
extern UINT8_t cc2520_memWrite(UINT16_t addr, UINT16_t count, UINT8_t *data_p);
extern UINT8_t cc2520_memRead(UINT16_t addr, UINT16_t count, UINT8_t  *data_p);
extern void cc2520_insRdArray(UINT16_t count, UINT8_t *data_p);
extern void cc2520_insWrArray(UINT16_t count, UINT8_t *data_p);
extern void CC2520_insRdArray(UINT16_t count, UINT8_t *data_p);
extern void cc2520_spiWaitRxRdy(void);
extern UINT8_t cc2520_SpiRx(void);
extern void cc2520_spiTx(UINT8_t val);
extern UINT8_t cc2520_spiTxRx(UINT8_t val);
extern void cc2520_spiInit(void);

extern void cc2520_transferPktToTxFifo(UINT8_t pduBDHndl);
extern void cc2520_transferPktBuffToTxFifo(UINT8_t *pktBuff_p,
                                           UINT16_t pktLen);

extern void cc2520_cfgEnaAckFramesRx(void);
extern void cc2520_cfgDisAckFramesRx(void);
extern UINT8_t cc2520_rand(UINT8_t *randVal_p);
extern void cc2520_init(void);
extern UINT8_t cc2520_setPhyChan(UINT8_t chanNr);
extern void CC2520_writeShort(UINT16_t addr, UINT16_t val);
extern void cc2520_hardRxStop(void);
extern void cc2520_rxOvflProc(void);
extern void cc2520_checkRadioErrors(void);

extern void cc2520_enterLPM1(void);
extern void cc2520_enterLPM2(void);
extern void cc2520_exitLPM1(void);
extern void cc2520_exitLPM2(void);

extern UINT8_t cc2520_setTxPower(SINT16_t powerInDbm);

extern UINT8_t cc2520_txPowerSetPointValid(SINT16_t powerInDbm);

#define CC2520_TX_POWER_ENC_TBL_SZ  9

typedef struct
{
   SINT8_t txPower;  // in dBm
   UINT8_t enc;      // TXPOWER register encoding
} cc2520_txPowerEnc_s;

      
#endif
