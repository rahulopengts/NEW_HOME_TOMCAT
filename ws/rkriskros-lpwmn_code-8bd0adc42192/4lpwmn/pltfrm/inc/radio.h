
#ifndef __RADIO_H__
#define __RADIO_H__
/*
 * File Name : radio.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : 9/1/2013
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

#include <typedefs.h>
#ifdef RADIO_CC2520
#include <cc2520.h>
#elif defined(RADIO_CC1200)
#include <cc1200.h>
#elif defined(RADIO_CC1101)
#include <cc1101.h>
#elif defined(RADIO_CC3100)
#include <cc3100.h>
#elif defined(RADIO_CC2500)
#include <cc2500.h>
#elif defined(RADIO_NONE)
#else
#error Radio not specified !!
#endif
#ifdef RADIO_CC1200
// #define RADIO_TX_POWER_LEVEL_PLUS_14_DBM    14
#endif
#if 0
#define RADIO_TX_POWER_LEVEL_PLUS_5_DBM     5
#define RADIO_TX_POWER_LEVEL_PLUS_3_DBM     3
#define RADIO_TX_POWER_LEVEL_PLUS_2_DBM     2
#define RADIO_TX_POWER_LEVEL_PLUS_1_DBM     1
#define RADIO_TX_POWER_LEVEL_0_DBM          0
#define RADIO_TX_POWER_LEVEL_MINUS_2_DBM   -2
#define RADIO_TX_POWER_LEVEL_MINUS_4_DBM   -4
#define RADIO_TX_POWER_LEVEL_MINUS_7_DBM   -7
#define RADIO_TX_POWER_LEVEL_MINUS_18_DBM  -18
#endif
#ifdef RADIO_CC2520
#define RADIO_RCVD_PKT_RSSI_INFO_LEN   CC2520_RCVD_PKT_RSSI_INFO_LEN
#define RADIO_RCVD_PKT_CORRELATION_INFO_LEN  CC2520_RCVD_PKT_CORRELATION_INFO_LEN
#elif defined(RADIO_CC1200)
#define RADIO_RCVD_PKT_RSSI_INFO_LEN   CC1200_APPENDED_RSSI_INFO_LEN
#define RADIO_RCVD_PKT_LQI_INFO_LEN   CC1200_APPENDED_CRC_LQI_INFO_LEN
#elif defined(RADIO_CC1101)
#define RADIO_RCVD_PKT_RSSI_INFO_LEN   CC1101_APPENDED_RSSI_INFO_LEN
#define RADIO_RCVD_PKT_LQI_INFO_LEN   CC1101_APPENDED_CRC_LQI_INFO_LEN
#elif defined(RADIO_CC2500)
#define RADIO_RCVD_PKT_RSSI_INFO_LEN   CC2500_APPENDED_RSSI_INFO_LEN
#define RADIO_RCVD_PKT_LQI_INFO_LEN   CC2500_APPENDED_CRC_LQI_INFO_LEN
#elif defined(RADIO_CC3100)
#elif defined(RADIO_NONE)
#else
#error Radio not specified !!
#endif

typedef enum
{
   RADIO_STATE_UNKNOWN,
   RADIO_STATE_LPM,
   RADIO_STATE_IDLE,   // receiver off
   RADIO_STATE_RX_ON,  // receiver on
   RADIO_STATE_TX_ON   // transmitting
} RADIO_state_t;      

extern RADIO_state_t RADIO_state;

#define RADIO_getCurrState()  RADIO_state

typedef struct
{
  UINT8_t percentage;
  SINT16_t start;
} RADIO_channMonRslt_s;


typedef struct
{
   // UINT8_t channIdx;
   UINT8_t binSpan;
   UINT16_t sampleCnt;
   UINT16_t interval;
   SINT16_t minMeasRSSI;
   SINT16_t maxMeasRSSI;
   SINT16_t avgMeasRSSI;
   SINT16_t minMonRSSI;
   SINT16_t maxMonRSSI;
   RADIO_channMonRslt_s  _rsltArray[3];
} RADIO_channMonReq_s;


#ifdef RADIO_CC2520

#define RADIO_enterWOR( )   0

#define RADIO_MAX_MAC_PDU_LEN  CC2520_MAX_MAC_PDU_LEN

#define RADIO_hardRxStart()  cc2520_hardRxStart()

#define RADIO_setCurrState(newState)  RADIO_state = (newState)

#define RADIO_hardRxStop()  cc2520_hardRxStop()

#define RADIO_softRxStart()  cc2520_softRxStart()

#define RADIO_startTxOnCCA() cc2520_insStrobe(CC2520_INS_STXONCCA)

#define RADIO_flushRx( ) cc2520_insStrobe(CC2520_INS_SFLUSHRX);

#define RADIO_flushTx( ) cc2520_insStrobe(CC2520_INS_SFLUSHTX);

#define RADIO_setChan(chan) cc2520_setPhyChan(chan)

#define RADIO_getCarrierFreq()  C2500_getCarrierFreq()

#define RADIO_enterLPM( )  \
do \
{  \
   cc2520_enterLPM( );  \
   RADIO_setCurrState(RADIO_STATE_LPM);  \
} while (0)


#define RADIO_exitLPM( ) \
do \
{  \
   cc2520_exitLPM( );  \
   RADIO_setCurrState(RADIO_STATE_IDLE);  \
} while (0)


#define RADIO_restart( )  \
do \
{  \
   RADIO_setCurrState(RADIO_STATE_UNKNOWN); \
   cc2520_exitLPM2( ); \
   RADIO_setCurrState(RADIO_STATE_IDLE); \
} while (0)

#define RADIO_cfgExtAddrFilter(extAddr_p)  \
        cc2520_memWrite(CC2520_RAM_LOCAL_EXT_ADDR, MAC_EXT_ADDR_LEN, extAddr_p)

#define RADIO_setTxPower(powerInDbm, powerInDbmDecPart) \
        cc2520_setTxPower((powerInDbm))

#define RADIO_txPowerSetPointValid(powerInDbm) \
        cc2520_txPowerSetPointValid((powerInDbm))

#define RADIO_sendToTxFifo(buff_p, buffLen) \
        cc2520_writeToTxFifo((buffLen), (buff_p))

#define RADIO_checkRadioErrors( ) cc2520_checkRadioErrors( )

#define RADIO_rxOvflProc( ) cc2520_rxOvflProc( )

#define RADIO_setFramePendingEveryAck( ) \
        cc2520_bitSet(CC2520_MAKE_BIT_ADDR(CC2520_FRMCTRL1, CC2520_FRMCTRL1_PENDING_OR_BIT_NR));

#define RADIO_clearFramePendingEveryAck( ) \
        cc2520_bitClear(CC2520_MAKE_BIT_ADDR(CC2520_FRMCTRL1, CC2520_FRMCTRL1_PENDING_OR_BIT_NR));

#define RADIO_cfgShortAddrFilter(shortAddr)  CC2520_setLocalShortAddr(shortAddr)

#define RADIO_cfgLPWMNIdFilter(lpwmnId)  CC2520_setLocalLPWMNId(lpwmnId)

#define RADIO_enableAutoAck( ) \
        cc2520_bitSet(CC2520_MAKE_BIT_ADDR(CC2520_FRMCTRL0, \
                                           CC2520_FRMCTRL0_AUTOACK_BIT_NR));


#elif defined(RADIO_CC1200)

#define RADIO_enterWOR( )   0

#define RADIO_MAX_MAC_PDU_LEN  CC1200_MAX_MAC_PDU_LEN

#define RADIO_setChan(chan) CC1200_cfgFreq(chan)

#define RADIO_hardStop( )  CC1200_forceIdle( )

#define RADIO_flushRx( )  CC1200_flushRxFIFO( )

#define RADIO_flushTx( )  CC1200_flushTxFIFO( )

#define RADIO_restart( )  CC1200_wakeUp( )

#define RADIO_enterLPM( )  CC1200_enterLPM( )

#define RADIO_hardRxStart()  CC1200_startRx()

#define RADIO_softRxStart()  CC1200_startRx()

#define RADIO_setTxPower(powerInDbm, powerInDbmDecPart) \
        CC1200_setTxPower((powerInDbm))

#define RADIO_txPowerSetPointValid(powerInDbm) \
        CC1200_txPowerSetPointValid(powerInDbm)

#define RADIO_sendToTxFifo(buff_p, buffLen) \
        CC1200_writeToTxFifo((buffLen), (buff_p))

#define RADIO_checkRadioErrors( ) CC1200_checkRadioErrors( )

#define RADIO_getTXFIFOByteCnt(p)  CC1200_getTXFIFOByteCnt(p)

#elif defined(RADIO_CC1101)

extern void CC1101_monRSSI(RADIO_channMonReq_s *reqParams_p);

#define RADIO_monRSSI(reqParams_p)  CC1101_monRSSI(reqParams_p)

#define RADIO_enterWOR( )  (CC1101_enterWOR() == PLTFRM_STS_SUCCESS)

#define RADIO_MAX_MAC_PDU_LEN  CC1101_MAX_MAC_PDU_LEN

#define RADIO_inLPM( )  (CC1101_cntxt.inLPM != 0)

#define RADIO_setChan(chan) CC1101_cfgChan(chan)

#define RADIO_hardStop( )  CC1101_forceIdle( )

#define RADIO_flushRx( )  CC1101_flushRxFIFO( )

#define RADIO_flushTx( )  CC1101_flushTxFIFO( )

#define RADIO_restart( )  CC1101_wakeUp( )

#define RADIO_enterLPM( )  CC1101_enterLPM( )

#define RADIO_hardRxStart()  CC1101_startRx()

#define RADIO_softRxStart()  CC1101_startRx()

#define RADIO_getCarrierFreq(channIdx)  CC1101_getCarrierFreq(channIdx)

#define RADIO_setTxPower(powerInDbm, powerInDbmDecPart) \
        CC1101_setTxPower(powerInDbm, powerInDbmDecPart)

#define RADIO_txPowerSetPointValid(powerInDbm) \
        CC1101_txPowerSetPointValid(powerInDbm)

#define RADIO_sendToTxFifo(buff_p, buffLen) \
		CC1101_writeBuffToTxFIFO(buff_p, buffLen)

#define RADIO_checkRadioErrors( ) CC1101_checkRadioErrors( )

#define RADIO_getTXFIFOByteCnt(p)  CC1101_getTXFIFOByteCnt(p)

#define RADIO_enableTxOnCCA( )  CC1101_enableCCA( )
#define RADIO_disableTxOnCCA( )  CC1101_disableCCA( )

#elif defined(RADIO_CC2500)

extern void CC2500_monRSSI(RADIO_channMonReq_s *reqParams_p);
#define RADIO_monRSSI(reqParams_p)  CC2500_monRSSI(reqParams_p)
#define RADIO_enterWOR( )  (CC2500_enterWOR() == PLTFRM_STS_SUCCESS)
#define RADIO_MAX_MAC_PDU_LEN  CC2500_MAX_MAC_PDU_LEN
#define RADIO_inLPM( )  (CC2500_cntxt.inLPM != 0)
#define RADIO_setChan(chan) CC2500_cfgChan(chan)
#define RADIO_hardStop( )  CC2500_forceIdle( )
#define RADIO_flushRx( )  CC2500_flushRxFIFO( )
#define RADIO_flushTx( )  CC2500_flushTxFIFO( )
#define RADIO_restart( )  CC2500_wakeUp( )
#define RADIO_enterLPM( )  CC2500_enterLPM( )
#define RADIO_hardRxStart()  CC2500_startRx()
#define RADIO_softRxStart()  CC2500_startRx()
#define RADIO_getCarrierFreq(channIdx)  CC2500_getCarrierFreq(channIdx)
#define RADIO_setTxPower(powerInDbm, powerInDbmDecPart) \
        CC2500_setTxPower(powerInDbm, powerInDbmDecPart)
#define RADIO_txPowerSetPointValid(powerInDbm) \
        CC2500_txPowerSetPointValid(powerInDbm)
#define RADIO_sendToTxFifo(buff_p, buffLen) \
		CC2500_writeBuffToTxFIFO(buff_p, buffLen)
#define RADIO_checkRadioErrors( ) CC2500_checkRadioErrors( )
#define RADIO_getTXFIFOByteCnt(p)  CC2500_getTXFIFOByteCnt(p)
#define RADIO_enableTxOnCCA( )  CC2500_enableCCA( )
#define RADIO_disableTxOnCCA( )  CC2500_disableCCA( )

#elif defined(RADIO_CC3100)
#elif defined(RADIO_NONE)
#else
#error radio not specified !!
#endif


extern void RADIO_transferPktToTxFifo(UINT8_t bdHndl);

extern void RADIO_transferPktBuffToTxFifo(UINT8_t *pktBuff_p, UINT16_t pktLen);
      
extern volatile RADIO_state_t cc2520_radioState;          
          
#endif
