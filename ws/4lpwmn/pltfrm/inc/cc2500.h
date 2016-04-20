/*
 * File Name : cc2500.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : July/15/2015
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

#ifndef __CC2500_H__
#define __CC2500_H__

#include <typedefs.h>
#include <pltfrm.h>

#define CC2500_PART_NUMBER  0x80
#define CC2500_VERSION_NUMBER  0x03

#define CC2500_PA_TABLE_LEN  8

#define CC2500_TX_FIFO_LEN  64
#define CC2500_RX_FIFO_LEN  64

#define CC2500_TX_FIFO_ADDR  0x3f
#define CC2500_RX_FIFO_ADDR  0x3f

#define CC2500_SPI_HDR_RW_BIT        0x80
#define CC2500_SPI_HDR_BURST_BIT     0x40
#define CC2500_SPI_HDR_ADDR_BIT_MSK  0x3f

// Table 3 
#define CC2500_2FSK_MIN_BAUD_RATE  600
#define CC2500_GFSK_MIN_BAUD_RATE  600
#define CC2500_4FSK_MIN_BAUD_RATE  600
#define CC2500_MSK_MIN_BAUD_RATE_KBAUD  26

#define CC2500_2FSK_MAX_BAUD_RATE_KBAUD  500
#define CC2500_GFSK_MAX_BAUD_RATE_KBAUD  250
#define CC2500_4FSK_MAX_BAUD_RATE_KBAUD  300
#define CC2500_MSK_MAX_BAUD_RATE_KBAUD  500

/*
 * Table 31: Typical RSSI_offset Values.
 */
#define CC2500_RSSI_OFFSET_IN_DB  74

/*
 * > Preamble bits (8xn bits)
 * > Sync word (16/32 bits) 
 * > Length field (8 bits)
 * > Address field (8 bits)
 * > Data field (8xn bits)
 * > CRC-16 (16 bits)
 */

// A 2 byte CRC checksum computed over the data field
#define CC2500_FRAME_CRC_FIELD_LEN  2

// Optional 1 byte address field
#define CC2500_FRAME_ADDRESS_FIELD_LEN  1

// Optional 1 byte frame length field
#define CC2500_FRAME_LENGTH_FIELD_LEN  1

#define CC2500_APPENDED_RSSI_INFO_LEN  1
#define CC2500_APPENDED_CRC_LQI_INFO_LEN   1

#if defined(RADIO_BAUD_RATE_38383)
/*
 * Give enough time for the transmitter to go from TX->IDLE->RX.
 * TX->IDLE with no cal takes .25/baud_rate
 * IDLE->RX with cal takes 1953/Xosc +  FS cal time (under 1 millisec)
 * At 1200 baud, total time will be 1.3 millisecs
 * At 10000 baud, total time will be 1.1 millisecs
 *
 * FS cal time is 712 micro-secs as CC2500 TEST0 is set to 0x9 and
 * FSCAL3.CHP_CURR_CAL_EN is set to 2. See table 35 in the CC2500 spec.
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
// The CC2500 is configured to FS cal when going from IDLE to TX or RX.
// This node (which has just received a data packet) will take some time
// to read the data packet from the RX-FIFO, process it and then send the
// ACK immediately. All this processing (up to ACK transmission on the air)
// will take some time. So 1 milli-sec extra delay should be enough.
// RX->TX switch takes 782/Xosc -> 30.1 microsecs
// Time to send ACK packet to the TX FIFO is taking around 100 microsecs
// on FFD/Coord.
#define CC2500_TX_TO_RX_TURN_AROUND_DELAY_MILLI_SECS  1

/*
 * RX to TX switch (Without PA ramping) - 30.1 microsecs (negligible)
 * Include time to decode the packet, and schedule immediate ACK (time
 * to transfer ACK PDU to CC2500) etc.
 */
#define CC2500_RX_TO_TX_ACK_TURN_AROUND_DELAY_MILLI_SECS  1

#elif defined(RADIO_BAUD_RATE_1200)
#define CC2500_TX_TO_RX_TURN_AROUND_DELAY_MILLI_SECS  20
#define CC2500_RX_TO_TX_ACK_TURN_AROUND_DELAY_MILLI_SECS  4
#elif defined(RADIO_BAUD_RATE_10000)
#define CC2500_TX_TO_RX_TURN_AROUND_DELAY_MILLI_SECS  2
#define CC2500_RX_TO_TX_ACK_TURN_AROUND_DELAY_MILLI_SECS  2
#elif defined(RADIO_BAUD_RATE_250000)
#define CC2500_TX_TO_RX_TURN_AROUND_DELAY_MILLI_SECS  1
#define CC2500_RX_TO_TX_ACK_TURN_AROUND_DELAY_MILLI_SECS  1
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
#define CC2500_MAX_RX_PKT_LEN (CC2500_RX_FIFO_LEN \
                               - CC2500_FRAME_LENGTH_FIELD_LEN \
                               - CC2500_APPENDED_RSSI_INFO_LEN \
                               - CC2500_APPENDED_CRC_LQI_INFO_LEN)

#define CC2500_MAX_TX_PKT_LEN CC2500_MAX_RX_PKT_LEN

// Sync word
#define CC2500_FRAME_SYNC_FIELD_MAX_LEN  4
#define CC2500_FRAME_SYNC_FIELD_MIN_LEN  2

#define CC2500_FRAME_PREAMBLE_LEN  3

#define CC2500_FRAME_SYNC_WORD_LEN_BITS  32

/*
 * When using the auto flush function, the maximum packet length is 63 bytes 
 * in variable packet length mode and 64 bytes in fixed packet length mode. 
 * Note that when PKTCTRL1.APPEND_STATUS is enabled, the maximum allowed pkt 
 * length is reduced by two bytes in order to make room in the RX FIFO for the 
 * two status bytes appended at the end of the packet.
 */
#define CC2500_MAX_MAC_PDU_LEN  (CC2500_RX_FIFO_LEN                 \
                                 - CC2500_FRAME_LENGTH_FIELD_LEN    \
                                 - CC2500_APPENDED_RSSI_INFO_LEN    \
                                 - CC2500_APPENDED_CRC_LQI_INFO_LEN)

#define CC2500_PIN_GDO0  0x0   // Pin #6
#define CC2500_PIN_GDO1  0x1   // Pin #2 (Also MISO)
#define CC2500_PIN_GDO2  0x2   // Pin #3

/*
 * Table 42: Command strobes
 */
#define CC2500_CMD_STROBE_SRES     0x30
#define CC2500_CMD_STROBE_SFSTXON  0x31
#define CC2500_CMD_STROBE_SXOFF    0x32
#define CC2500_CMD_STROBE_SCAL     0x33
#define CC2500_CMD_STROBE_SRX      0x34
#define CC2500_CMD_STROBE_STX      0x35
#define CC2500_CMD_STROBE_SIDLE    0x36
#define CC2500_CMD_STROBE_SWOR     0x38
#define CC2500_CMD_STROBE_SPWD     0x39
#define CC2500_CMD_STROBE_SFRX     0x3a
#define CC2500_CMD_STROBE_SFTX     0x3b
#define CC2500_CMD_STROBE_SWORRST  0x3c
#define CC2500_CMD_STROBE_SNOP     0x3d


/*
 * Table 45: SPI address space.
 */
#define CC2500_IOCFG2_REG_ADDR           0x00
#define CC2500_IOCFG1_REG_ADDR           0x01
#define CC2500_IOCFG0_REG_ADDR           0x02
#define CC2500_FIFOTHR_REG_ADDR          0x03
#define CC2500_SYNC1_REG_ADDR            0x04
#define CC2500_SYNC0_REG_ADDR            0x05
#define CC2500_PKTLEN_REG_ADDR           0x06
#define CC2500_PKTCTRL1_REG_ADDR         0x07
#define CC2500_PKTCTRL0_REG_ADDR         0x08
#define CC2500_ADDR_REG_ADDR             0x09
#define CC2500_CHANNR_REG_ADDR           0x0a
#define CC2500_FSCTRL1_REG_ADDR          0x0b
#define CC2500_FSCTRL0_REG_ADDR          0x0c
#define CC2500_FREQ2_REG_ADDR            0x0d
#define CC2500_FREQ1_REG_ADDR            0x0e
#define CC2500_FREQ0_REG_ADDR            0x0f
#define CC2500_MDMCFG4_REG_ADDR          0x10
#define CC2500_MDMCFG3_REG_ADDR          0x11
#define CC2500_MDMCFG2_REG_ADDR          0x12
#define CC2500_MDMCFG1_REG_ADDR          0x13
#define CC2500_MDMCFG0_REG_ADDR          0x14
#define CC2500_DEVIATN_REG_ADDR          0x15
#define CC2500_MCSM2_REG_ADDR            0x16
#define CC2500_MCSM1_REG_ADDR            0x17
#define CC2500_MCSM0_REG_ADDR            0x18
#define CC2500_FOCCFG_REG_ADDR           0x19
#define CC2500_BSCFG_REG_ADDR            0x1a
#define CC2500_AGCCTRL2_REG_ADDR          0x1b
#define CC2500_AGCCTRL1_REG_ADDR          0x1c
#define CC2500_AGCCTRL0_REG_ADDR          0x1d
#define CC2500_WOREVT1_REG_ADDR          0x1e
#define CC2500_WOREVT0_REG_ADDR          0x1f
#define CC2500_WORCTRL_REG_ADDR          0x20
#define CC2500_FREND1_REG_ADDR           0x21
#define CC2500_FREND0_REG_ADDR           0x22
#define CC2500_FSCAL3_REG_ADDR           0x23
#define CC2500_FSCAL2_REG_ADDR           0x24
#define CC2500_FSCAL1_REG_ADDR           0x25
#define CC2500_FSCAL0_REG_ADDR           0x26
#define CC2500_RCCTRL1_REG_ADDR          0x27
#define CC2500_RCCTRL0_REG_ADDR          0x28
#define CC2500_FSTEST_REG_ADDR           0x29
#define CC2500_PTEST_REG_ADDR            0x2a
#define CC2500_AGCTEST_REG_ADDR          0x2b
#define CC2500_TEST2_REG_ADDR            0x2c
#define CC2500_TEST1_REG_ADDR            0x2d
#define CC2500_TEST0_REG_ADDR            0x2e
#define CC2500_PARTNUM_REG_ADDR          0x30
#define CC2500_VERSION_REG_ADDR          0x31
#define CC2500_FREQEST_REG_ADDR          0x32
#define CC2500_LQI_REG_ADDR              0x33
#define CC2500_RSSI_REG_ADDR             0x34
#define CC2500_MARCSTATE_REG_ADDR        0x35
#define CC2500_WORTIME1_REG_ADDR         0x36
#define CC2500_WORTIME0_REG_ADDR         0x37
#define CC2500_PKTSTATUS_REG_ADDR        0x38
#define CC2500_VCO_VC_DAC_REG_ADDR       0x39
#define CC2500_TXBYTES_REG_ADDR          0x3a
#define CC2500_RXBYTES_REG_ADDR          0x3b
#define CC2500_RCCTRL1_STATUS_REG_ADDR   0x3c
#define CC2500_RCCTRL0_STATUS_REG_ADDR   0x3d
#define CC2500_PA_TABLE_ADDR             0x3e

// CC2500 data sheet
// Table 31: Optimum PATABLE Settings for Various Output Power Levels
#define CC2500_TX_PWR_1_DBM_PATABLE_ENC   0xff
#define CC2500_TX_PWR_0_DBM_PATABLE_ENC   0xfe

#define PHY_2400_MHZ_BAND_CHANNEL_CNT  8


// IOCFGX registers
#define  CC2500_IOCFGX_IRQ_INVERT_BIT   0x40   // 0 - active high / 1 - active low

/*
 * Status byte
 */
#define CC2500_STS_BYTE_CHIP_STATE_SHIFT  4
#define CC2500_STS_BYTE_CHIP_STATE_BIT_MSK_SHIFT 0x7  // Bits 6:4

#define CC11O1_PKT_STS_REG_CS_BIT_MSK  0x40
#define CC11O1_PKT_STS_REG_CCA_BIT_MSK  0x10

/*
 * TXBYTES (0x3a)
 */
#define CC2500_TXFIFO_UNDERFLOW_BIT_MSK  0x80
#define CC2500_NUM_TXBYTES_BIT_MSK  0x7f

/*
 * RXBYTES (0x3b)
 */
#define CC2500_RXFIFO_OVERFLOW_BIT_MSK  0x80
#define CC2500_NUM_RXBYTES_BIT_MSK  0x7f


/*
 * MCSM1 (0x17)
 */
#define CC2500_MCSM1_REG_CCA_MODE_FIELD_BIT_MSK  0x30
#define CC2500_MCSM1_REG_CCA_MODE_FIELD_SHIFT 4
#define CC2500_MCSM1_REG_CCA_MODE_FIELD_SHIFT_MSK  0x03

typedef enum
{
   CC2500_CCA_MODE_0 = 0,  // Always
   CC2500_CCA_MODE_1 = 1,  // If RSSI < threshold
   CC2500_CCA_MODE_2 = 2,  // If not receving a packet
   CC2500_CCA_MODE_3 = 3   // if RSSI < threshold and not currently receiving a packet
} CC2500_ccaMode_t;

typedef enum
{
   CC2500_MOD_FORMAT_2_FSK = 0,
   CC2500_MOD_FORMAT_GSK = 1,
   CC2500_MOD_FORMAT_ASK_OOK = 3,
   CC2500_MOD_FORMAT_4_FSK = 4,
   CC2500_MOD_FORMAT_MSK = 7
} CC2500_modFormat_t;


#define CC2500_2_FSK_MOD_BITS_PER_SYMBOL  1
#define CC2500_GFSK_MOD_BITS_PER_SYMBOL  1
#define CC2500_4_FSK_MOD_BITS_PER_SYMBOL  2

typedef enum
{
   // Fixed packet length mode. Length configured in PKTLEN
   // register.
   CC2500_FIXED_PACKET_LENGTH_MODE = 0,

   // Variable packet length mode. Packet length configured by
   // the first byte after sync word.
   CC2500_VARIABLE_PACKET_LENGTH_MODE = 1,

   // Infinite packet length mode.
   CC2500_INFINITE_PACKET_LENGTH_MODE = 2,
} CC2500_pktLenMode_t;



typedef struct
{
   UINT8_t regAddr;
   UINT8_t regVal;
} CC2500_chipRegInfo_s;


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
   CC2500_STATE_IDLE,
   CC2500_STATE_RX,
   CC2500_STATE_TX_IN_PROGRESS,
   CC2500_STATE_FSTXON,
   CC2500_STATE_CALIBRATE,
   CC2500_STATE_SETTLING,
   CC2500_STATE_RXFIFO_OVERFLOW,
   CC2500_STATE_TXFIFO_UNDERFLOW,
   CC2500_STATE_WOR,
   CC2500_STATE_SLEEP
} CC2500_state_t;



typedef struct
{
   volatile UINT8_t txStartedFlag:1;
   volatile UINT8_t inLPM:1;     // 1 if CC2500 is in LPM, 0 otherwise ..
#ifdef CC2500_WOR_ENA
   volatile UINT8_t syncRcvdFlag:1;
   volatile UINT8_t inWOR:1;
   volatile UINT8_t rsvd:4;
#else
   volatile UINT8_t rsvd:6;
#endif

   SINT8_t crcOkPktInRxFIFOCnt;  // Ok packets in RX FIFO

   UINT16_t enteredLPMCnt, exitedLPMCnt;

#ifdef CC2500_INTR_STATS_ENA
   UINT16_t txStartedFlagIntCnt;
   UINT16_t crcOkPktRcvdIntCnt;
#ifdef CC2500_WOR_ENA
   UINT8_t syncRcvdIntCnt;
#endif
#endif
} CC2500_cntxt_s;

extern CC2500_cntxt_s CC2500_cntxt;

extern PLTFRM_sts_t CC2500_flushRxFIFO(void);
extern PLTFRM_sts_t CC2500_flushTxFIFO(void);
extern PLTFRM_sts_t CC2500_getRXFIFOByteCnt(UINT8_t *byteCnt_p);
extern void CC2500_checkRadioErrors(void);
extern UINT8_t CC2500_txPowerSetPointValid(SINT16_t powerInDbm);
extern PLTFRM_sts_t CC2500_enterLPM(void);
extern PLTFRM_sts_t CC2500_wakeUp(void);
extern PLTFRM_sts_t CC1200_setTxPower(SINT16_t powerInDBm);
extern PLTFRM_sts_t CC2500_startRx(void);
extern PLTFRM_sts_t CC2500_forceIdle(void);
extern PLTFRM_sts_t CC2500_getCurrState(CC2500_state_t *state_p);
extern PLTFRM_sts_t CC2500_getTXFIFOByteCnt(UINT8_t *byteCnt_p);
extern PLTFRM_sts_t CC2500_sendCmdStrobe(UINT8_t cmdStrobeId, CC2500_state_t *fsmState_p);
extern PLTFRM_sts_t CC2500_waitTillRSSIValid(void);
extern PLTFRM_sts_t CC2500_transferPktToTxFifo(UINT8_t bdHndl);
extern UINT16_t CC2500_calcPktTrxTime(UINT16_t pktLen);
extern PLTFRM_sts_t CC2500_writeBuffToTxFIFO(UINT8_t *buff_p, UINT8_t buffLen);
extern PLTFRM_sts_t CC2500_cfgChan(UINT8_t channIdx);
extern PLTFRM_sts_t CC2500_readRxFIFO(UINT8_t cnt, UINT8_t *buff_p);
extern PLTFRM_sts_t CC2500_init(void);
extern PLTFRM_sts_t CC2500_exitLPM(void);
extern PLTFRM_sts_t CC2500_setTxPower(SINT16_t intPart, SINT16_t decPart);
extern UINT32_t CC2500_getCarrierFreq(UINT8_t channIdx);
extern CC2500_cntxt_s  CC2500_cntxt;
extern PLTFRM_sts_t CC2500_readRxFIFO(UINT8_t cnt, UINT8_t *buff_p);
extern PLTFRM_sts_t CC2500_readReg(UINT8_t regAddr, UINT8_t *regVal_p);

extern const UINT16_t CC2500_txToIdleTransitionTimeMilli;

extern void CC2500_insertTurnAroundDelay(void);

extern PLTFRM_sts_t CC2500_enterWOR(void);
extern PLTFRM_sts_t CC2500_exitWOR(void);

extern PLTFRM_sts_t CC2500_updateRegField(const UINT8_t regAddr,
                                          const UINT8_t newVal,
                                          const UINT8_t bitMsk,
                                          const UINT8_t bitShift,
                                          const UINT8_t bitMskShifted);

extern PLTFRM_sts_t CC2500_spiInit(void);

extern void CC2500_switchOn(void);

extern void CC2500_irqHndlr(void);

extern SINT16_t CC2500_calcPktRSSI(UINT8_t rssiRaw);

extern PLTFRM_sts_t CC2500_enableCCA(void);
extern PLTFRM_sts_t CC2500_disableCCA(void);

#endif
