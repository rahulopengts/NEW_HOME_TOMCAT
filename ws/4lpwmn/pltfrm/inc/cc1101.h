/*
 * File Name : cc1101.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : Sep/15/2014
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

#ifndef __CC1101_H__
#define __CC1101_H__

#include <typedefs.h>
#include <pltfrm.h>

#define CC1101_PART_NUMBER  0x00
#define CC1101_VERSION_NUMBER_0  0x04
#define CC1101_VERSION_NUMBER_1  0x14

#define CC1101_PA_TABLE_LEN  8

#define CC1101_TX_FIFO_LEN  64
#define CC1101_RX_FIFO_LEN  64

#define CC1101_TX_FIFO_ADDR  0x3f
#define CC1101_RX_FIFO_ADDR  0x3f

#define CC1101_SPI_HDR_RW_BIT        0x80
#define CC1101_SPI_HDR_BURST_BIT     0x40
#define CC1101_SPI_HDR_ADDR_BIT_MSK  0x3f

// Table 3 
#define CC1101_2FSK_MIN_BAUD_RATE  600
#define CC1101_GFSK_MIN_BAUD_RATE  600
#define CC1101_4FSK_MIN_BAUD_RATE  600 
#define CC1101_MSK_MIN_BAUD_RATE_KBAUD  26

#define CC1101_2FSK_MAX_BAUD_RATE_KBAUD  500
#define CC1101_GFSK_MAX_BAUD_RATE_KBAUD  250
#define CC1101_4FSK_MAX_BAUD_RATE_KBAUD  300
#define CC1101_MSK_MAX_BAUD_RATE_KBAUD  500

/*
 * Table 31: Typical RSSI_offset Values.
 */
#define CC1101_RSSI_OFFSET_IN_DB  74

/*
 * > Preamble bits (8xn bits)
 * > Sync word (16/32 bits) 
 * > Length field (8 bits)
 * > Address field (8 bits)
 * > Data field (8xn bits)
 * > CRC-16 (16 bits)
 */

// A 2 byte CRC checksum computed over the data field
#define CC1101_FRAME_CRC_FIELD_LEN  2

// Optional 1 byte address field
#define CC1101_FRAME_ADDRESS_FIELD_LEN  1

// Optional 1 byte frame length field
#define CC1101_FRAME_LENGTH_FIELD_LEN  1

#define CC1101_APPENDED_RSSI_INFO_LEN  1
#define CC1101_APPENDED_CRC_LQI_INFO_LEN   1

#if defined(RADIO_BAUD_RATE_38383)
/*
 * Give enough time for the transmitter to go from TX->IDLE->RX.
 * TX->IDLE with no cal takes .25/baud_rate
 * IDLE->RX with cal takes 1953/Xosc +  FS cal time (under 1 millisec)
 * At 1200 baud, total time will be 1.3 millisecs
 * At 10000 baud, total time will be 1.1 millisecs
 *
 * FS cal time is 712 micro-secs as CC1101 TEST0 is set to 0x9 and
 * FSCAL3.CHP_CURR_CAL_EN is set to 2. See table 35 in the CC1101 spec.
 * So total time to go from TX->RX->IDLE at 38400 bps will be
 * .25/baud_rate + 1953/Xosc + FS cal time
 *  -> (.25/38383) + (1953/(26 * 10^6)) + 712 microsecs
 *  -> 6.51 + 75.2 + 712 -> 794 microsecs
 *
 *  So total time to go from TX->RX->IDLE at 1200 bps will be
 * .25/baud_rate + 1953/Xosc + FS cal time
 *  -> (.25/1200) + (1953/(26 * 10^6)) + 712 microsecs
 *  -> 209 + 75.2 + 712 -> 997 microsecs
 */

// This macro is used by a node when it receives a data packet and wants
// to give the transmitter some time for the latter to go from TX->IDLE->RX.
// The CC1101 is configured to FS cal when going from IDLE to TX or RX.
// This node (which has just received a data packet) will take some time
// to read the data packet from the RX-FIFO, process it and then send the
// ACK immediately. All this processing (up to ACK transmission on the air)
// will take some time. So 1 milli-sec extra delay should be enough.
// RX->TX switch takes 782/Xosc -> 30.1 microsecs
// Time to send ACK packet to the TX FIFO is taking around 100 microsecs
// on FFD/Coord.
#define CC1101_TX_TO_RX_TURN_AROUND_DELAY_MILLI_SECS  1

/*
 * RX to TX switch (Without PA ramping) - 30.1 microsecs (negligible)
 * Include time to decode the packet, and schedule immediate ACK (time
 * to transfer ACK PDU to CC1101) etc.
 */
#define CC1101_RX_TO_TX_ACK_TURN_AROUND_DELAY_MILLI_SECS  1

#elif defined(RADIO_BAUD_RATE_1200)
#define CC1101_TX_TO_RX_TURN_AROUND_DELAY_MILLI_SECS  20
#define CC1101_RX_TO_TX_ACK_TURN_AROUND_DELAY_MILLI_SECS  4
#elif defined(RADIO_BAUD_RATE_10000)
#define CC1101_TX_TO_RX_TURN_AROUND_DELAY_MILLI_SECS  2
#define CC1101_RX_TO_TX_ACK_TURN_AROUND_DELAY_MILLI_SECS  2
#else
#error not supported !!
#endif

/*
 * When using the auto flush function, the maximum packet length is 63
 * bytes in variable packet length mode. Note that when PKTCTRL1.APPEND_STATUS
 * is enabled, the maximum allowed packet length is reduced by two bytes in
 * order to make room in the RX FIFO for the two status bytes appended at the
 * end of the packet.
 */
#define CC1101_MAX_RX_PKT_LEN (CC1101_RX_FIFO_LEN \
                               - CC1101_FRAME_LENGTH_FIELD_LEN \
                               - CC1101_APPENDED_RSSI_INFO_LEN \
                               - CC1101_APPENDED_CRC_LQI_INFO_LEN)

#define CC1101_MAX_TX_PKT_LEN CC1101_MAX_RX_PKT_LEN

// Sync word
#define CC1101_FRAME_SYNC_FIELD_MAX_LEN  4
#define CC1101_FRAME_SYNC_FIELD_MIN_LEN  2

#define CC1101_FRAME_PREAMBLE_LEN  3

#define CC1101_FRAME_SYNC_WORD_LEN_BITS  32

/*
 * When using the auto flush function, the maximum packet length is 63 bytes 
 * in variable packet length mode and 64 bytes in fixed packet length mode. 
 * Note that when PKTCTRL1.APPEND_STATUS is enabled, the maximum allowed pkt 
 * length is reduced by two bytes in order to make room in the RX FIFO for the 
 * two status bytes appended at the end of the packet.
 */
#define CC1101_MAX_MAC_PDU_LEN  (CC1101_RX_FIFO_LEN                 \
                                 - CC1101_FRAME_LENGTH_FIELD_LEN    \
                                 - CC1101_APPENDED_RSSI_INFO_LEN    \
                                 - CC1101_APPENDED_CRC_LQI_INFO_LEN)

#define CC1101_PIN_GDO0  0x0   // Pin #6 
#define CC1101_PIN_GDO1  0x1   // Pin #2 (Also MISO)
#define CC1101_PIN_GDO2  0x2   // Pin #3

/*
 * Table 42: Command strobes
 */
#define CC1101_CMD_STROBE_SRES     0x30
#define CC1101_CMD_STROBE_SFSTXON  0x31
#define CC1101_CMD_STROBE_SXOFF    0x32
#define CC1101_CMD_STROBE_SCAL     0x33
#define CC1101_CMD_STROBE_SRX      0x34
#define CC1101_CMD_STROBE_STX      0x35
#define CC1101_CMD_STROBE_SIDLE    0x36
#define CC1101_CMD_STROBE_SWOR     0x38
#define CC1101_CMD_STROBE_SPWD     0x39
#define CC1101_CMD_STROBE_SFRX     0x3a
#define CC1101_CMD_STROBE_SFTX     0x3b
#define CC1101_CMD_STROBE_SWORRST  0x3c
#define CC1101_CMD_STROBE_SNOP     0x3d


/*
 * Table 45: SPI address space.
 */
#define CC1101_IOCFG2_REG_ADDR           0x00
#define CC1101_IOCFG1_REG_ADDR           0x01
#define CC1101_IOCFG0_REG_ADDR           0x02
#define CC1101_FIFOTHR_REG_ADDR          0x03
#define CC1101_SYNC1_REG_ADDR            0x04
#define CC1101_SYNC0_REG_ADDR            0x05
#define CC1101_PKTLEN_REG_ADDR           0x06
#define CC1101_PKTCTRL1_REG_ADDR         0x07
#define CC1101_PKTCTRL0_REG_ADDR         0x08
#define CC1101_ADDR_REG_ADDR             0x09
#define CC1101_CHANNR_REG_ADDR           0x0a
#define CC1101_FSCTRL1_REG_ADDR          0x0b
#define CC1101_FSCTRL0_REG_ADDR          0x0c
#define CC1101_FREQ2_REG_ADDR            0x0d
#define CC1101_FREQ1_REG_ADDR            0x0e
#define CC1101_FREQ0_REG_ADDR            0x0f
#define CC1101_MDMCFG4_REG_ADDR          0x10
#define CC1101_MDMCFG3_REG_ADDR          0x11
#define CC1101_MDMCFG2_REG_ADDR          0x12
#define CC1101_MDMCFG1_REG_ADDR          0x13
#define CC1101_MDMCFG0_REG_ADDR          0x14
#define CC1101_DEVIATN_REG_ADDR          0x15
#define CC1101_MCSM2_REG_ADDR            0x16
#define CC1101_MCSM1_REG_ADDR            0x17
#define CC1101_MCSM0_REG_ADDR            0x18
#define CC1101_FOCCFG_REG_ADDR           0x19
#define CC1101_BSCFG_REG_ADDR            0x1a
#define CC1101_AGCTRL2_REG_ADDR          0x1b
#define CC1101_AGCTRL1_REG_ADDR          0x1c
#define CC1101_AGCTRL0_REG_ADDR          0x1d
#define CC1101_WOREVT1_REG_ADDR          0x1e
#define CC1101_WOREVT0_REG_ADDR          0x1f
#define CC1101_WORCTRL_REG_ADDR          0x20
#define CC1101_FREND1_REG_ADDR           0x21
#define CC1101_FREND0_REG_ADDR           0x22
#define CC1101_FSCAL3_REG_ADDR           0x23
#define CC1101_FSCAL2_REG_ADDR           0x24
#define CC1101_FSCAL1_REG_ADDR           0x25
#define CC1101_FSCAL0_REG_ADDR           0x26
#define CC1101_RCCTRL1_REG_ADDR          0x27
#define CC1101_RCCTRL0_REG_ADDR          0x28
#define CC1101_FSTEST_REG_ADDR           0x29
#define CC1101_PTEST_REG_ADDR            0x2a
#define CC1101_AGCTEST_REG_ADDR          0x2b
#define CC1101_TEST2_REG_ADDR            0x2c
#define CC1101_TEST1_REG_ADDR            0x2d
#define CC1101_TEST0_REG_ADDR            0x2e
#define CC1101_PARTNUM_REG_ADDR          0x30
#define CC1101_VERSION_REG_ADDR          0x31
#define CC1101_FREQEST_REG_ADDR          0x32
#define CC1101_LQI_REG_ADDR              0x33
#define CC1101_RSSI_REG_ADDR             0x34
#define CC1101_MARCSTATE_REG_ADDR        0x35
#define CC1101_WORTIME1_REG_ADDR         0x36
#define CC1101_WORTIME0_REG_ADDR         0x37
#define CC1101_PKTSTATUS_REG_ADDR        0x38
#define CC1101_VCO_VC_DAC_REG_ADDR       0x39
#define CC1101_TXBYTES_REG_ADDR          0x3a
#define CC1101_RXBYTES_REG_ADDR          0x3b
#define CC1101_RCCTRL1_STATUS_REG_ADDR   0x3c
#define CC1101_RCCTRL0_STATUS_REG_ADDR   0x3d
#define CC1101_PA_TABLE_ADDR             0x3e

/*
 * From Design Note DN025 -
 * JTI Matched Filter Balun (0868BM15C0001) at 868 MHz
 * Balun + LC(filter) + C(blocking cap)
 * Section 3.3.2.2
 */
#if defined(RADIO_FREQ_BAND_865_867_MHZ) || defined(RADIO_FREQ_BAND_868_MHZ)
#define CC1101_TX_PWR_10_POINT_8_DBM_PATABLE_ENC   0xc0
#define CC1101_TX_PWR_10_POINT_0_DBM_PATABLE_ENC   0xc2
#define CC1101_TX_PWR_1_POINT_5_DBM_PATABLE_ENC   0x8e
#define CC1101_TX_PWR_0_POINT_3_DBM_PATABLE_ENC   0x50
#else
#error radio operating frequency band not specified !!
#endif


// IOCFGX registers
#define  CC1101_IOCFGX_IRQ_INVERT_BIT   0x40   // 0 - active high / 1 - active low

/*
 * Status byte
 */
#define CC1101_STS_BYTE_CHIP_STATE_SHIFT  4
#define CC1101_STS_BYTE_CHIP_STATE_BIT_MSK_SHIFT 0x7  // Bits 6:4

#define CC11O1_PKT_STS_REG_CS_BIT_MSK  0x40
#define CC11O1_PKT_STS_REG_CCA_BIT_MSK  0x10

/*
 * TXBYTES (0x3a)
 */
#define CC1101_TXFIFO_UNDERFLOW_BIT_MSK  0x80
#define CC1101_NUM_TXBYTES_BIT_MSK  0x7f

/*
 * RXBYTES (0x3b)
 */
#define CC1101_RXFIFO_OVERFLOW_BIT_MSK  0x80
#define CC1101_NUM_RXBYTES_BIT_MSK  0x7f


/*
 * MCSM1 (0x17)
 */
#define CC1101_MCSM1_REG_CCA_MODE_FIELD_BIT_MSK  0x30
#define CC1101_MCSM1_REG_CCA_MODE_FIELD_SHIFT 4
#define CC1101_MCSM1_REG_CCA_MODE_FIELD_SHIFT_MSK  0x03

typedef enum
{
   CC1101_CCA_MODE_0 = 0,  // Always
   CC1101_CCA_MODE_1 = 1,  // If RSSI < threshold
   CC1101_CCA_MODE_2 = 2,  // If not receving a packet
   CC1101_CCA_MODE_3 = 3   // if RSSI < threshold and not currently receiving a packet
} CC1101_ccaMode_t;

typedef enum
{
   CC1101_MOD_FORMAT_2_FSK = 0,
   CC1101_MOD_FORMAT_GSK = 1,
   CC1101_MOD_FORMAT_ASK_OOK = 3,
   CC1101_MOD_FORMAT_4_FSK = 4,
   CC1101_MOD_FORMAT_MSK = 7
} CC1101_modFormat_t;


#define CC1101_2_FSK_MOD_BITS_PER_SYMBOL  1
#define CC1101_GFSK_MOD_BITS_PER_SYMBOL  1
#define CC1101_4_FSK_MOD_BITS_PER_SYMBOL  2

typedef enum
{
   // Fixed packet length mode. Length configured in PKTLEN
   // register.
   CC1101_FIXED_PACKET_LENGTH_MODE = 0,

   // Variable packet length mode. Packet length configured by
   // the first byte after sync word.
   CC1101_VARIABLE_PACKET_LENGTH_MODE = 1,

   // Infinite packet length mode.
   CC1101_INFINITE_PACKET_LENGTH_MODE = 2,
} CC1101_pktLenMode_t;



typedef struct
{
   UINT8_t regAddr;
   UINT8_t regVal;
} CC1101_chipRegInfo_s;


/*
 * <Static modes>
 * Power down with retention - Low-power RC oscillator running (0.3 uA to 0.5 uA)
 * XOFF mode - Crystal oscillator / TCXO disabled (180 microamps)
 * IDLE mode - Clock running, system waiting with no radio activity (1.5 mA)
 */

/*
 * Table 23 - list of exposed states
 */
typedef enum
{
   CC1101_STATE_IDLE,
   CC1101_STATE_RX,
   CC1101_STATE_TX_IN_PROGRESS,
   CC1101_STATE_FSTXON,
   CC1101_STATE_CALIBRATE,
   CC1101_STATE_SETTLING,
   CC1101_STATE_RXFIFO_OVERFLOW,
   CC1101_STATE_TXFIFO_UNDERFLOW,
   CC1101_STATE_WOR,
   CC1101_STATE_SLEEP
} CC1101_state_t;



typedef struct
{
   volatile UINT8_t txStartedFlag:1;
   volatile UINT8_t inLPM:1;     // 1 if CC1101 is in LPM, 0 otherwise ..
#ifdef CC1101_WOR_ENA
   volatile UINT8_t syncRcvdFlag:1;
   volatile UINT8_t inWOR:1;
   volatile UINT8_t rsvd:4;
#else
   volatile UINT8_t rsvd:6;
#endif

   SINT8_t crcOkPktInRxFIFOCnt;  // Ok packets in RX FIFO

   UINT16_t enteredLPMCnt, exitedLPMCnt;

#ifdef CC1101_INTR_STATS_ENA
   UINT16_t txStartedFlagIntCnt;
   UINT16_t crcOkPktRcvdIntCnt;
#ifdef CC1101_WOR_ENA
   UINT8_t syncRcvdIntCnt;
#endif
#endif
} CC1101_cntxt_s;

extern CC1101_cntxt_s CC1101_cntxt;

extern PLTFRM_sts_t CC1101_flushRxFIFO(void);
extern PLTFRM_sts_t CC1101_flushTxFIFO(void);
extern PLTFRM_sts_t CC1101_getRXFIFOByteCnt(UINT8_t *byteCnt_p);
extern void CC1101_checkRadioErrors(void);
extern UINT8_t CC1101_txPowerSetPointValid(SINT16_t powerInDbm);
extern PLTFRM_sts_t CC1101_enterLPM(void);
extern PLTFRM_sts_t CC1101_wakeUp(void);
extern PLTFRM_sts_t CC1200_setTxPower(SINT16_t powerInDBm);
extern PLTFRM_sts_t CC1101_startRx(void);
extern PLTFRM_sts_t CC1101_forceIdle(void);
extern PLTFRM_sts_t CC1101_getCurrState(CC1101_state_t *state_p);
extern PLTFRM_sts_t CC1101_getTXFIFOByteCnt(UINT8_t *byteCnt_p);
extern PLTFRM_sts_t CC1101_sendCmdStrobe(UINT8_t cmdStrobeId, CC1101_state_t *fsmState_p);
extern PLTFRM_sts_t CC1101_waitTillRSSIValid(void);
extern PLTFRM_sts_t CC1101_transferPktToTxFifo(UINT8_t bdHndl);
extern UINT16_t CC1101_calcPktTrxTime(UINT16_t pktLen);
extern PLTFRM_sts_t CC1101_writeBuffToTxFIFO(UINT8_t *buff_p, UINT8_t buffLen);
extern PLTFRM_sts_t CC1101_cfgChan(UINT8_t channIdx);
extern PLTFRM_sts_t CC1101_readRxFIFO(UINT8_t cnt, UINT8_t *buff_p);
extern PLTFRM_sts_t CC1101_init(UINT8_t);
extern PLTFRM_sts_t CC1101_exitLPM(void);
extern PLTFRM_sts_t CC1101_setTxPower(SINT16_t intPart, SINT16_t decPart);
extern UINT32_t CC1101_getCarrierFreq(UINT8_t channIdx);
extern CC1101_cntxt_s  CC1101_cntxt;
extern PLTFRM_sts_t CC1101_readRxFIFO(UINT8_t cnt, UINT8_t *buff_p);
extern PLTFRM_sts_t CC1101_readReg(UINT8_t regAddr, UINT8_t *regVal_p);

extern const UINT16_t CC1101_txToIdleTransitionTimeMilli;

extern void CC1101_insertTurnAroundDelay(void);

extern PLTFRM_sts_t CC1101_enterWOR(void);
extern PLTFRM_sts_t CC1101_exitWOR(void);

extern PLTFRM_sts_t CC1101_updateRegField(const UINT8_t regAddr,
                                          const UINT8_t newVal,
                                          const UINT8_t bitMsk,
                                          const UINT8_t bitShift,
                                          const UINT8_t bitMskShifted);

extern PLTFRM_sts_t CC1101_spiInit(void);

extern void CC1101_switchOn(void);

extern void CC1101_irqHndlr(void);

extern SINT16_t CC1101_calcPktRSSI(UINT8_t rssiRaw);

extern PLTFRM_sts_t CC1101_enableCCA(void);
extern PLTFRM_sts_t CC1101_disableCCA(void);

#endif
