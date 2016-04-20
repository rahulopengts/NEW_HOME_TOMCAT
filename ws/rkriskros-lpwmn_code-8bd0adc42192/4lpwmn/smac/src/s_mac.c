/*
 * File Name: s_mac.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: March/6/2014
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

#include <string.h>
#include <pltfrm.h>
#include <mac.h>
#include <mac_pib.h>
#include <mlme.h>
#include <radio.h>
#include <phy_defs.h>
#include <phy_pib.h>
#include <system.h>
#if !defined(DEV_TYPE_RFD)
#include <aodvl.h>
#endif
#include <util.h>
#include <pltfrm_log.h>


MAC_cntxt_s MAC_cntxt;

void MAC_doCsmaProc(void);
void MAC_csmaInit(void);
void MAC_updateSrcInAckBuff(void);
void MAC_handleCsmaFlrDuringScan(void);
void MAC_assocReqAckHndlr(void);

extern UINT8_t ADP_pktFwdCheck(UINT8_t *adpPdu_p,
                               UINT8_t *dstShort_p,  // Points to dest short in the MAC header
                               UINT8_t *srcShort_p,
                               UINT8_t adpPduLen,
                               UINT8_t bdHndl);

extern void ADP_macAssocIndHndlr(UINT8_t *srcExtAddr_p,
                                 UINT8_t capInfo);

extern UINT8_t NM_lookUpLocalRFD(UINT16_t shortAddr);
extern SYS_trscnQ_s *NM_checkPendingFrame(MAC_shortAddr_t shortAddr);


#ifdef DEV_TYPE_RFD
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void MAC_sendPollCnfrm(MAC_sts_t sts)
{
   MAC_cntxt.pollReqSent = 0;
   MAC_cntxt.mlmePollCnfrmParams.status = sts;
   SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_MLME_POLL_CONFIRM;
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
#if (MAC_PDU_HDR_SEQ_NR_FIELD_LEN > 1)
UINT32_t MAC_getNextDSN(void)
{
   UINT32_t nextDSN;
#else
UINT8_t MAC_getNextDSN(void)
{
   UINT8_t nextDSN;
#endif

   nextDSN = MAC_PIB.macDSN;

   /*
    * <802.15.4-2006>
    * 5.1.6.1 Transmission
    * Each device shall store its current DSN value in the MAC PIB attribute
    * macDSN and initialize it to a random value; the algorithm for choosing a
    * random number is outside the scope of this standard. Each time a data or
    * a MAC command frame is generated, the MAC sub layer shall copy the value of
    * macDSN into the Sequence Number field of the MHR of the outgoing frame and
    * then increment it by one. Each device shall generate exactly one DSN
    * regardless of the number of unique devices with which it wishes to communicate.
    * The value of macDSN shall be permitted to roll over.
    */
   MAC_PIB.macDSN ++;

   return nextDSN;
}


#ifndef LPWMN_COORD
/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
void MAC_scanDurationTmoHndlr(void)
{
#ifdef MAC_STATS_ENA
   MAC_cntxt.scanTmoCnt ++;
#endif

   MAC_doActiveScan();
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
void MAC_sendDataCnfrm(void)
{
   if (MAC_cntxt.cnfrmReqd)
   {
       MAC_cntxt.txModState = MAC_TX_MODE_STATE_CONFIRM;

       SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_DATA_CONFIRM;
   }
   else
       MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE;

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
void MAC_freeCurrTxPkt(void)
{
   if (!(SYS_BD_HNDL_IS_VALID(MAC_cntxt.currTxPduHndl)))
       SYS_fatal(SYS_FATAL_ERR_1);

   SYS_freeMem(MAC_cntxt.currTxPduHndl);

   MAC_cntxt.currTxPduHndl = SYS_INV_BD_HDNL;

   return;
}


#ifndef LPWMN_COORD
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void MAC_sendAssocCnfrm(const UINT8_t sts, const MAC_shortAddr_t allocShortAddr)
{
   // Turn the radio off (switch to idle mode)
   // In the idle mode, clock is running and system is waiting with
   // no radio activity. CC1200 current consumption is 1.5 mA.

   if (sts != MAC_STS_SUCCESS)
   {
       if (CC1200_forceIdle( ) != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_1);

       // Reset related PIB attributes
       MAC_PIB.macLPWMNId = MAC_BROADCAST_LPWMN_ID;
       MAC_PIB.macCoordShortAddress = MAC_BROADCAST_SHORT_ADDR;
   }

   MAC_cntxt.assocReqInProgress = 0;

   MAC_cntxt.assocCnfrmParams.status = sts;
   MAC_cntxt.assocCnfrmParams.assocShortAddr = allocShortAddr;

   SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_MAC_ASSOC_CONFIRM;

#ifndef LPWMN_COORD
   if (sts == MAC_STS_SUCCESS)
   {
       MAC_updateSrcInAckBuff();
   }
#endif

   return;
}
#endif


#ifdef DEV_TYPE_RFD
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t MAC_checkReassocCondition(void)
{
   MAC_cntxt.txFlrCnt ++;

   if (MAC_cntxt.txFlrCnt > MAC_RFD_REASSOC_THRESHOLD)
   {
       MAC_cntxt.txFlrCnt = 0x0;

       MAC_PIB.macShortAddress = MAC_BROADCAST_SHORT_ADDR;
       MAC_PIB.macLPWMNId = MAC_BROADCAST_LPWMN_ID;

       // TODO - reset the cc1200
       // RADIO_restart();

       SYS_globalEvtMsk1  |= SYS_GLOBAL_EVT_RFD_REASSOC_REQ;

       return 1;
   }

   return 0;
}
#endif

#ifndef DEV_TYPE_RFD
/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
void MAC_stopPendingFrameTrx(void)
{
   if (!(SYS_globalEvtMsk1 & SYS_GLOBAL_EVT_PENDING_FRAME_TRX_REQ))
       SYS_fatal(SYS_FATAL_ERR_1);

   if (MAC_cntxt.pendIndTrxDestAddr == MAC_BROADCAST_SHORT_ADDR)
       SYS_fatal(SYS_FATAL_ERR_1);

   MAC_cntxt.pendingFrameTxInProgress = 0;
   SYS_globalEvtMsk1 &= ~SYS_GLOBAL_EVT_PENDING_FRAME_TRX_REQ;
   MAC_cntxt.pendIndTrxDestAddr = MAC_BROADCAST_SHORT_ADDR;

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
void MAC_ackTmoHndlr(void)
{
   /*
    * If an acknowledgment is not received within macAckWaitDuration or an ack
    * is rxd containing a DSN that was not the same as the original transmission,
    * the device shall conclude that the single transmission attempt has failed.
    */

#ifdef MAC_STATS_ENA
   MAC_cntxt.ackTmoCnt ++;
#endif

   {
       /*
        * If a single transmission attempt has failed and the transmission was
        * direct, the device shall repeat the process of transmitting the data
        * or MAC command frame and waiting for the acknowledgment, up to a max
        * of macMaxFrameRetries times. The retransmitted frame shall contain the
        * same DSN as was used in the original transmission.
        */

       if (MAC_cntxt.txAttempCnt > MAC_PIB.macMaxFrameRetries)
       {
           /*
            * If an acknowledgment is still not received after macMaxFrameRetries
            * retransmissions, the MAC sub-layer shall assume the transmission has
            * failed and notify the next higher layer of the failure.
            */

           MAC_cntxt.expAckSrcShortAddr = MAC_BROADCAST_SHORT_ADDR;

           // Free the BD (containing the unacked packet)
           MAC_freeCurrTxPkt();

#ifndef LPWMN_COORD
           if (MAC_cntxt.assocReqInProgress)
           {
               MAC_sendAssocCnfrm(MAC_STS_NO_ACK, MAC_BROADCAST_SHORT_ADDR);

               MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE;
           }
           else
#endif
           {
#ifdef DEV_TYPE_RFD
               if (MAC_cntxt.pollReqSent)
               {
                   /*
                    * 7.1.16.1.3
                    * If the MLME successfully transmits a data request command, the
                    * MLME will expect an acknowledgment in return. If an acknowledgment
                    * is not received, the MLME will issue the MLME-POLL.confirm primitive
                    * with a status of NO_ACK (see 7.5.6.4).
                    */
                   MAC_sendPollCnfrm(MAC_STS_NO_ACK);
                   MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE;

                   // Do not dis-associate now. When this RFD wakes up next time, it will
                   // send some data anyway.

                   return;
               }
#else
               if (MAC_cntxt.pendingFrameTxInProgress)
               {
                   MAC_stopPendingFrameTrx( );
                   MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE;
                   // Do not remove the RFD ...
                   return;
               }
#endif
               /*
                * If the TxOptions parameter specifies that a direct transmission is
                * required and the MAC sub-layer does not receive an acknowledgment
                * from the recipient after macMaxFrameRetries retransmissions, as
                * described in 5.1.6.4, it will discard the MSDU and issue the MCPS-
                * DATA.confirm primitive with a status of NO_ACK.
                */

               MAC_cntxt.datacnfrmParams.status = MAC_STS_NO_ACK;
               MAC_cntxt.datacnfrmParams.destShortAddr = MAC_cntxt.expAckSrcShortAddr;

#if !defined(DEV_TYPE_RFD)
               AODVL_purgeRtTbls(MAC_cntxt.expAckSrcShortAddr);
#endif

#ifdef DEV_TYPE_RFD
               if (MAC_checkReassocCondition() == 0x0)
                   MAC_sendDataCnfrm();
#else
               MAC_sendDataCnfrm();
#endif
           }
       }
       else
       {
           MAC_cntxt.txAttempCnt ++;

           MAC_csmaInit();
           MAC_doCsmaProc();
       }
   }

   return;
}

UINT8_t CC1200_dbgfreqOff1_1, CC1200_dbgfreqOff0_1;
UINT8_t CC1200_dbgTxFifoByteCnt;

#ifdef CC1200_TIMING_TEST
UINT16_t CC1200_txInProgCnt = 0;
UINT8_t preambleLenEncVal = 6;
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
void MAC_txOverEvtHndlr(const UINT8_t intRcvd)
{
   PLTFRM_sts_t sts;
   PLTFRM_stopTimerA0();


#ifdef MAC_STATS_ENA
   if (intRcvd)
   {
#ifdef CC1200_TIMING_TEST
       preambleLenEncVal ++;
       if (preambleLenEncVal > 13)
    	   preambleLenEncVal = 0;

       sts = CC1200_updateRegField(CC1200_PREAMBLE_CFG1_REG_ADDR,
    		                       preambleLenEncVal,
                                   CC1200_PREAMBLE_CFG1_NUM_PREAMBLE_BIT_MSK,
                                   CC1200_PREAMBLE_CFG1_NUM_PREAMBLE_SHIFT,
                                   CC1200_PREAMBLE_CFG1_NUM_PREAMBLE_BIT_MSK_SHIFT);
       if (sts != PLTFRM_STS_SUCCESS)
    	   SYS_fatal(SYS_FATAL_ERR_1);
#endif

       MAC_cntxt.frameTxDoneEvtCnt ++;
   }
#endif

   sts = CC1200_readExtAddrSpaceReg(CC1200_FREQOFF1_EXT_REG_ADDR, &CC1200_dbgfreqOff1_1);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);
   sts = CC1200_readExtAddrSpaceReg(CC1200_FREQOFF0_EXT_REG_ADDR, &CC1200_dbgfreqOff0_1);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);

   {

      PLTFRM_sts_t sts;
      // Check the TX FIFO. It should be empty !!
      sts = RADIO_getTXFIFOByteCnt(&CC1200_dbgTxFifoByteCnt);
      if (sts != PLTFRM_STS_SUCCESS || CC1200_dbgTxFifoByteCnt > 0)
          SYS_fatal(SYS_FATAL_ERR_1);
   }  

   /*
    * When a packet is successfully transmitted, the radio controller goes to
    * the state indicated by the RFEND_CFG0.TXOFF_MODE setting.
    *
    * In this implementation, the radio is configured to listen after a transmission
    */

   // TODO > Read state from device and confirm
   CC1200_setCurrState(CC1200_STATE_RX);

#ifdef DEV_TYPE_RFD
   if (MAC_cntxt.pollReqTxAttempt)
   {
       MAC_cntxt.pollReqTxAttempt = 0;
       MAC_cntxt.pollReqSent = 1;
   }
#endif

#ifndef LPWMN_COORD
   if (MAC_cntxt.scanInProgress)
   {
       UINT32_t scanDuration;

       /*
        * Upon successful transmission of the beacon request command for an
        * active scan or after switching to the channel for a passive scan,
        * the device shall enable its receiver for at most
        *              [aBaseSuperframeDuration �((2^n) + 1)],
        * where n is the value of the ScanDuration parameter.
        */

       MAC_freeCurrTxPkt();

#ifndef RFD_CONT_TX_TEST
       scanDuration = 1000;   // 1 second
       if (PLTFRM_startTimerA0(scanDuration, 0, MAC_scanDurationTmoHndlr) != PLTFRM_STS_SUCCESS)
    	   SYS_fatal(SYS_FATAL_ERR_1);
#else
       MAC_doActiveScan();
#endif

#ifdef MAC_STATS_ENA
       MAC_cntxt.scanTimerStartedCnt ++;
#endif

       MAC_cntxt.txModState = MAC_TX_MOD_STATE_CHANN_SCAN;

#ifdef FLASH_LED_ON_PKT_RX_TX
       if (intRcvd)
       {
           PLTFRM_OFF_LED_2();
       }
#endif
       return;
   }
#endif

   // PLTFRM_LOG("<%s> pkt (len - %d) trx over\n\r", __FUNCTION__, (SINT16_t)MAC_cntxt.macPduLen);

   if ((MAC_cntxt.immAckTxInProgress == 0x0) && (MAC_cntxt.ackRequired))
   {
       /*
        * A frame transmitted with the AR field set to request an acknowledgment,
        * as defined in 5.2.1.1.4, shall be acknowledged by the recipient. If the
        * intended recipient correctly receives the frame, it shall generate and
        * send an acknowledgment frame containing the same DSN from the data or
        * MAC command frame that is being acknowledged.
        *
        * The CC1200 does not have automatic ack transmission facility. As a
        * result, we need to wait for a worst case duration of say 'x' seconds
        * for the corresponding ack.
        *
        * This node will not transmit any non-ack packet till it receives an
        * ack for the last transmitted non-ack packet. The last transmitted
        * packet is still in the CC1200's transmit fifo and the corresponding
        * buffer descriptor (BD) is stored in the variable CC1200_cntxt.txFifoPktHndl.
        */

       sts = PLTFRM_startTimerA0(1000,   // HACK
                                 0,
                                 MAC_ackTmoHndlr);
       if (sts != PLTFRM_STS_SUCCESS)
    	   SYS_fatal(SYS_FATAL_ERR_1);

       MAC_cntxt.txModState = MAC_TX_MOD_STATE_WAIT_ACK;
   }
   else
   {
       /*
        * 5.1.6.4.1 No acknowledgment
        * A frame transmitted with its AR field set to indicate no acknowledgment
        * requested, as defined in 5.2.1.1.4, shall not be acknowledged by its
        * intended recipient. The originating device shall assume that the
        * transmission of the frame was successful.
        */

       if (MAC_cntxt.immAckTxInProgress == 0x0)
           MAC_freeCurrTxPkt();

       // Bug, fixed on feb/7/14, rkris@wisense.in
#if defined(DEV_TYPE_RFD) || defined(DEV_TYPE_FFD)
       if (MAC_cntxt.assocReqInProgress)
       {
           MAC_cntxt.assocReqInProgress = 0x0;
           MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE;

           // The node manager is already running a timer waiting for
           // association to complete successfully. So don't call
           // MAC_assocReqAckHndlr() here.

           // MAC_assocReqAckHndlr();
       }
       else
#endif
       {
           if (MAC_cntxt.immAckTxInProgress == 0x0)
           {
               MAC_cntxt.datacnfrmParams.status = MAC_STS_SUCCESS;
               MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE;
               MAC_sendDataCnfrm();
           }
           else
           {
               // Re-enable CCA which was disabled before sending ACK
               CC1200_enableCCA();

               if (MAC_cntxt.backOffInterrupted)
               {
                   // Restart CSMA-CA
                   MAC_cntxt.csmaCntxt.backOffExp = MAC_PIB.macMinBE;
                   MAC_cntxt.txModState = MAC_TX_MOD_STATE_CSMA_BACK_OFF;
                   SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_RETRY_CCA;
                   MAC_cntxt.backOffInterrupted = 0;
               }
               else
               {
                   if (MAC_cntxt.ackRxPending)
                   {
                       MAC_cntxt.txModState = MAC_TX_MOD_STATE_WAIT_ACK;
                       MAC_cntxt.ackRxPending = 0;
                       sts = PLTFRM_startTimerA0(5,   // HACK (5 millisecs)
                                                 0,   // Hack
                                                 MAC_ackTmoHndlr);
                       if (sts != PLTFRM_STS_SUCCESS)
                    	   SYS_fatal(SYS_FATAL_ERR_1);
                   }
                   else
                   {
                       // MAC Tx module was IDLE before this ack was sent
                       MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE;
                   }
               }
           }
       }

       MAC_cntxt.immAckTxInProgress = 0x0;
   }

#ifdef FLASH_LED_ON_PKT_RX_TX
   if (intRcvd)
   {
       PLTFRM_OFF_LED_2();
   }
#endif

   return;
}


CC1200_state_t __cc1200State;

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void MAC_txOverTmoCbFunc( )
{
#ifdef MAC_STATS_ENA
   MAC_cntxt.MAC_txOverTmoCnt ++;
#endif

   if (CC1200_getCurrState(&__cc1200State) != PLTFRM_STS_SUCCESS)
	   SYS_fatal(SYS_FATAL_ERR_1);

   // Since the "back to rx" interrupt did not occur, we need
   // to change the interrupt edge to positive to get an
   // interrupt when a packet is received
   CC1200_setUpPktRxInt();

   MAC_txOverEvtHndlr(0);
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
void MAC_csmaInit(void)
{
   /*
    * lastSentMacPduLen will be > 0 when
    * - This is a retransmission of a packet wth MPDU length > aMaxSIFSFrameSize
    *   (18) bytes.
    * - A back to back packet transmission attempt is being made where the previously
    *   transmitted packet's MPDU length exceeds aMaxSIFSFrameSize (18) bytes
    *   and the previous packet was acknowledged less than 40 symbols ago.
    */


   /*
    * Each device shall maintain two variables for each transmission attempt:
    *   NB, and BE.
    */

   /*
    * NB is the number of times the CSMA-CA algorithm was required to back off
    * while attempting the current transmission; this value shall be initialized
    * to zero before each new transmission attempt.
    *
    * The CSMA-CA algorithm is implemented using units of time called backoff
    * periods, where one backoff period shall be equal to aUnitBackoffPeriod.
    */
   MAC_cntxt.csmaCntxt.numBackOffs = 0x0;

   /*
    * BE is the backoff exponent, which is related to how many backoff periods a
    * device shall wait before attempting to assess a channel. In unslotted systems,
    * BE shall be initialized to the value of macMinBE.
    */
   MAC_cntxt.csmaCntxt.backOffExp = MAC_PIB.macMinBE;

   return;
}


#ifdef __SMAC_DBG__
UINT8_t CC1200_dbgFIFOByteCnt0 = 0, CC1200_dbgFIFOByteCnt1 = 0;
CC1200_state_t CC1200_dbgState;

UINT8_t CC1200_dbgMdmSts0_0, CC1200_dbgMdmSts1_0;
UINT8_t CC1200_dbgMdmSts0_1, CC1200_dbgMdmSts1_1;
UINT8_t CC1200_dbgMdmSts0_2, CC1200_dbgMdmSts1_2;

UINT8_t CC1200_dbgIOCFG0 = 0;
#endif

UINT8_t CC1200_safcDone = 0;


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void MAC_csmaAttemptTxOnCca(void)
{
  PLTFRM_sts_t sts;
  CC1200_state_t cc1200State;
  UINT8_t regVal, txOnCCALoopCnt;

#ifdef MAC_STATS_ENA
  MAC_cntxt.csmaAttemptTxOnCcaCnt ++;
#endif

  /*
   * <STX command probe>
   * In IDLE state: Enable TX. Perform calibration first if
   * SETTLING_CFG.FS_AUTOCAL = 1. If in RX state and PKT_CFG2.CCA_MODE
   * != 0: Only go to TX if channel is clear.
   *
   * If the radio controller is in RX when the STX or SFSTXON command
   * strobes are used, the TX-on-CCA function will be used. If the chan
   * is clear, TX (or FSTXON state) is entered. The PKT_CFG2.CCA_MODE
   * setting controls the conditions for clear channel assessment (see
   * Section 6.11 for more details).
   */

   // In this implementation, SETTLING_CFG.FS_AUTOCAL is always set to 1.

   /*
    * When an STX or SFSTXON command strobe is given while CC120X is in
    * the RX state, the TX or FSTXON state is only entered if the clear
    * channel requirements are fulfilled (CCA_STATUS is asserted). Otherwise,
    * the chip will remain in RX. If the channel then becomes available, the
    * radio will not enter TX or FSTXON state before a new strobe command is
    * sent on the SPI interface.
    *
    *
    * In this implementation, the radio is configured to go back to RX after
    * receiving a packet (good or bad). Further, the radio is configured to
    * go to RX after transmitting a packet. This means the radio state here
    * will be IDLE only on an RFD whose radio has been woken up from SLEEP.
    */

   // It is possible that the radio is an error state. Recover if so.
   CC1200_checkRadioErrors();

   if (CC1200_getCurrState(&cc1200State) != PLTFRM_STS_SUCCESS)
	   SYS_fatal(SYS_FATAL_ERR_1);

   if (cc1200State == CC1200_STATE_SLEEPING)
   {
       CC1200_wakeUp();
       cc1200State = CC1200_STATE_IDLE;
   }

   if (cc1200State == CC1200_STATE_IDLE)
   {
       sts = CC1200_startRx();
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_1);
   }

   CC1200_cntxt.txOnCCADoneFlag = 0;

#ifdef __SMAC_DBG__
   sts = CC1200_readExtAddrSpaceReg(CC1200_MODEM_STATUS0_EXT_REG_ADDR, &CC1200_dbgMdmSts0_0);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);

   sts = CC1200_readExtAddrSpaceReg(CC1200_MODEM_STATUS1_EXT_REG_ADDR, &CC1200_dbgMdmSts1_0);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);

   CC1200_readReg(CC1200_IOCFG0_REG_ADDR, &CC1200_dbgIOCFG0);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);
#endif

   // Send the packet to the FIFO before triggering transmission
   CC1200_transferPktToTxFifo(MAC_cntxt.currTxPduHndl);

#ifdef __SMAC_DBG__
   sts = CC1200_readExtAddrSpaceReg(CC1200_MODEM_STATUS0_EXT_REG_ADDR, &CC1200_dbgMdmSts0_1);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);

   sts = CC1200_readExtAddrSpaceReg(CC1200_MODEM_STATUS1_EXT_REG_ADDR, &CC1200_dbgMdmSts1_1);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);
#endif

   CC1200_dbgReadRegs();

#ifdef __SMAC_DBG__
   sts = CC1200_getTXFIFOByteCnt(&CC1200_dbgFIFOByteCnt0);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);
#endif

   /*
    * <TXONCCA_DONE>
    * A pulse occurring when a decision has been made as to whether the
    * channel is busy or not. This signal must be used as an interrupt to
    * the MCU. When this signal is asserted/de-asserted, TXONCCA_FAILED
    * can be checked.
    */


   // Enable "TXONCCA_DONE" signal to interrupt the micro
   CC1200_enaGPIO2Int();

   /*
    * Automatic frequency compensation.
    * A SAFC command strobe can be issued in any state but does
    * not take effect until the next time the radio enters active
    * mode (TX or RX).
    * The SAFC strobe moves the estimate in FREQOFF_EST over to
    * FREQ_OFF registers.
    */
#if 0
   if (CC1200_safcDone == 0)
   {
       UINT8_t delay;
       for (delay=0; delay<100; delay++)
            PLTFRM_delay1MilliSec();
       sts = CC1200_sendCmdStrobe(CC1200_CMD_STROBE_SAFC, NULL);
       if (sts != PLTFRM_STS_SUCCESS)
       {
           SYS_fatal(SYS_FATAL_ERR_1);
       }
       CC1200_safcDone = 1;
   }
#endif

   /*
    * If the radio controller is in RX when the STX or SFSTXON command strobes
    * are used, the TX-on-CCA function will be used. If the channel is clear,
    * TX (or FSTXON state) is entered. The PKT_CFG2.CCA_MODE setting controls
    * the conditions for clear channel assessment.
    * The CCA_MODE is set to 0x3 (Indicates clear channel when RSSI is below
    * threshold and currently not receiving a packet).
    *
    * Carrier Sense (CS) is asserted when the RSSI is above a programmable CS
    * threshold, AGC_CS_THR, and de-asserted when RSSI is below the same
    * threshold. The CS threshold should be set high enough so that CS is de-
    * asserted when only background noise is present and low enough so that CS
    * is asserted when a wanted signal is present. Different usage of CS includes:
    * - Clear Channel Assessment (CCA) and TX on CCA
    *
    * AGC_CS_THR - Carrier Sense Threshold Configuration
    * AGC carrier sense threshold. Two's complement number with 1 dB resolution.
    */

   sts = CC1200_sendCmdStrobe(CC1200_CMD_STROBE_STX, NULL);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_1);
   }

#ifdef MAC_STATS_ENA
   MAC_cntxt.txStrobeCnt ++;
#endif

   txOnCCALoopCnt = 0;

   do
   {
       PLTFRM_delay1MilliSec();
       if (CC1200_cntxt.txOnCCADoneFlag)
           break;
       txOnCCALoopCnt ++;
       if (txOnCCALoopCnt > 20)
           break;
   } while (1);


   CC1200_disGPIO2Int();

   if (txOnCCALoopCnt > 20)
   {
       // It is possible that the radio is an error state. Recover if so.
       CC1200_checkRadioErrors();

       SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_RETRY_CCA;

#ifdef MAC_STATS_ENA
       MAC_cntxt.ccaDoneTmoCnt ++;
#endif

       return;
   }

#ifdef __SMAC_DBG__
   sts = CC1200_readExtAddrSpaceReg(CC1200_MODEM_STATUS0_EXT_REG_ADDR, &CC1200_dbgMdmSts0_2);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);

   sts = CC1200_readExtAddrSpaceReg(CC1200_MODEM_STATUS1_EXT_REG_ADDR, &CC1200_dbgMdmSts1_2);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);

   sts = CC1200_getTXFIFOByteCnt(&CC1200_dbgFIFOByteCnt1);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);

   if (CC1200_getCurrState(&CC1200_dbgState) != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);
#endif

   sts = CC1200_readExtAddrSpaceReg(CC1200_MARC_STATUS0_EXT_REG_ADDR, &regVal);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_1);
   }

   if (regVal & CC1200_MARC_STATUS0_TXONCCA_FAILED_BIT)
   {
       // The channel was busy. The radio will remain in RX state.

       // CC1200_cntxt.txAttemptInProgress = 0;

#ifdef MAC_STATS_ENA
       MAC_cntxt.ccaFlrCnt ++;
#endif

       MAC_cntxt.csmaCntxt.numBackOffs ++;

       if (MAC_cntxt.csmaCntxt.numBackOffs > MAC_PIB.macMaxCSMABackoffs)
       {
           // All attempts exhausted !!
#ifdef MAC_STATS_ENA
           MAC_cntxt.csmaFlrCnt ++;
#endif

           // Free the BD (containing the un-acked packet)
           MAC_freeCurrTxPkt();

           /*
            * If the transmission uses CSMA-CA and the CSMA-CA algorithm failed due to
            * adverse conditions on the channel, and the TxOptions parameter specifies
            * that a direct transmission is required, the MAC sub-layer will discard the
            * MSDU and the status will be set to CHANNEL_ACCESS_FAILURE.
            */
#ifndef LPWMN_COORD
           if (MAC_cntxt.scanInProgress)
           {
               MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE;
               MAC_handleCsmaFlrDuringScan();
           }
           else
#endif
           {
#ifndef LPWMN_COORD
               if (MAC_cntxt.assocReqInProgress)
               {
                   /*
                    * if the association request command cannot be sent due to a channel
                    * access failure, the MAC sub-layer shall notify the next higher layer.
                    */
                   MAC_sendAssocCnfrm(MAC_STS_CHANNEL_ACCESS_FLR,
                                      MAC_BROADCAST_SHORT_ADDR);
               }
               else
#endif
               {
#ifdef DEV_TYPE_RFD
                   if (MAC_cntxt.pollReqTxAttempt)
                   {
                       MAC_sendPollCnfrm(MAC_STS_CHANNEL_ACCESS_FLR);
                       MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE;
                   }
                   else
#else
                   if (MAC_cntxt.pendingFrameTxInProgress)
                   {
                       MAC_stopPendingFrameTrx( );
                       MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE;
                       // Do not remove the RFD ...
                       return;
                   }
                   else
#endif
                   {
                       MAC_cntxt.datacnfrmParams.status = MAC_STS_CHANNEL_ACCESS_FLR;
                       MAC_sendDataCnfrm();
                   }
               }
           }
       }
       else
       {
           // BE = min(BE+1, macMaxBE)
           MAC_cntxt.csmaCntxt.backOffExp ++;

           if (MAC_cntxt.csmaCntxt.backOffExp > MAC_PIB.macMaxBE)
               MAC_cntxt.csmaCntxt.backOffExp = MAC_PIB.macMaxBE;

           SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_RETRY_CCA;
       }
   }
   else
   {
       UINT16_t tmoMilliSecs;

       // The channel was clear. The radio has entered TX state.

       // Wait for CC1200 to go back to RX state.
       CC1200_setUpRxEntryInt();

       CC1200_setCurrState(CC1200_STATE_TX_IN_PROGRESS);

#ifdef MAC_STATS_ENA
       MAC_cntxt.ccaOkCnt ++;
#endif

       tmoMilliSecs = CC1200_calcPktTrxTime(0);
       tmoMilliSecs += (CC1200_TX_TO_RX_TURN_AROUND_TIME_MSECS*2);
       if (PLTFRM_startTimerA0(tmoMilliSecs, 0, MAC_txOverTmoCbFunc) != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_1);

       // CCA passed. Transmission underway
       MAC_cntxt.txModState = MAC_TX_MOD_STATE_WAIT_TX_OVER;
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
void MAC_sendAdpFwdPkt(UINT8_t bdHndl)
{
   (void)bdHndl;
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
UINT16_t MAC_getCsmaBackOff(const UINT16_t backOffLimit)
{
   UINT16_t randVal;

#ifdef MAC_CSMA_SW_RAND_GEN
   randVal = pltfrm_randU16();
#else
   if (CC1200_cntxt.state == CC1200_STATE_SLEEPING)
       randVal = pltfrm_randU16();
   else
       randVal = CC1200_getRandNr();
#endif

   randVal = randVal % (backOffLimit + 1);

   return randVal;
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
void MAC_doCsmaProc(void)
{
   /*
    * Non beacon-enabled LPWMNs use an un-slotted CSMA-CA channel access mechanism,
    * as described in 5.1.1.4. Each time a device wishes to transmit data frames
    * or MAC commands, it waits for a random period. If the channel is found to
    * be idle, following the random back-off, the device transmits its data. If
    * the channel is found to be busy following the random backoff, the device
    * waits for another random period before trying to access the channel again.
    * Acknowledgment frames are sent without using a CSMACA mechanism.
    */

   /*
    * Note that if macMinBE is set to zero, collision avoidance will be disabled
    * during the first iteration of this algorithm.
    */

   do
   {
      if (MAC_cntxt.csmaCntxt.backOffExp > 0x0)
      {
          UINT32_t backOffDelay;

          /*
           * Delay for random((2^BE) - 1) backoff periods.
           */
#ifndef CSMA_TEST
          backOffDelay = MAC_getCsmaBackOff(((1 << MAC_cntxt.csmaCntxt.backOffExp) - 1));
          /*
           * One back off period shall be equal to aUnitBackoffPeriod (in symbols)
           */
          backOffDelay *= MAC_aUnitBackoffPeriod;

          /*
           * In microseconds.
           * macMinBE range is 0 to macMaxBE. macMaxBE  range is 3 to 8.
           * BE range is 0 to 8 which means delay range is 0 microseconds
           * to 255*20*16 microseconds => 0 usecs to 81.6 msecs.
           */
          backOffDelay <<= PHY_SYMBOL_LENGTH_USECS_SHIFT;
#else
          backOffDelay = 0;
#endif

          MAC_cntxt.txModState = MAC_TX_MOD_STATE_CSMA_BACK_OFF;

          if (backOffDelay > 0)
          {
              if (PLTFRM_startTimerA0(backOffDelay/1000,
                                      backOffDelay % 1000,
                                      MAC_csmaAttemptTxOnCca) != PLTFRM_STS_SUCCESS)
              {
            	  SYS_fatal(SYS_FATAL_ERR_1);
              }
          }
          else
          {
              MAC_csmaAttemptTxOnCca(  );
          }
      }
      else
      {
          MAC_csmaAttemptTxOnCca(  );
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
 *
 *
 ********************************************************************
 */
UINT8_t MAC_buildFrameCntrlHdr(UINT8_t *hdr_p,
                               const MAC_pduInfo_s * const frameInfo_p)
{
   UINT8_t byte;

   byte = frameInfo_p->frameType;

   if (frameInfo_p->secEna)
       byte |= MAC_FC_SEC_ENA_BIT_SHIFT_MSK;

   if (frameInfo_p->framePending)
       byte |= MAC_FC_PENDING_BIT_SHIFT_MSK;

   if (frameInfo_p->ackRequired)
       byte |= MAC_FC_ACK_REQ_BIT_SHIFT_MSK;

   if (frameInfo_p->lpwmnIdComp)
       byte |= MAC_FC_LPWMN_ID_COMP_BIT_SHIFT_MSK;
   hdr_p[0] = byte;

   byte = frameInfo_p->dam;
   byte = (byte << MAC_FC_DAM_BITS_SHIFT) & MAC_FC_DAM_BITS_SHIFT_MSK;
   hdr_p[1] = byte;

   byte = frameInfo_p->frameVersion;
   byte = (byte << MAC_FC_FRAME_VER_BITS_SHIFT) & MAC_FC_FRAME_VER_BITS_SHIFT_MSK;
   hdr_p[1] |= byte;

   byte = frameInfo_p->sam;
   byte = (byte << MAC_FC_SAM_BITS_SHIFT) & MAC_FC_SAM_BITS_SHIFT_MSK;
   hdr_p[1] |= byte;

   return MAC_PDU_HDR_FC_FIELD_MAX_LEN;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t MAC_calcHdrLen(const MAC_pduInfo_s * const pduInfo_p)
{
   UINT8_t hdrLen = MAC_PDU_HDR_FC_FIELD_LEN + MAC_PDU_HDR_SEQ_NR_FIELD_LEN;

   if (pduInfo_p->dam != MAC_ADDRESS_MODE_NO_ADDR)
   {
       hdrLen += MAC_LPWMN_ID_LEN;
   }

   if (pduInfo_p->dam != MAC_ADDRESS_MODE_NO_ADDR)
   {
       if (pduInfo_p->dam == MAC_ADDRESS_MODE_SHORT_ADDR)
           hdrLen += MAC_SHORT_ADDR_LEN;
       else
           hdrLen += MAC_EXT_ADDR_LEN;
   }

   if (pduInfo_p->sam != MAC_ADDRESS_MODE_NO_ADDR)
   {
       if (pduInfo_p->lpwmnIdComp == 0x0
           || (pduInfo_p->dam == MAC_ADDRESS_MODE_NO_ADDR))
       {
           hdrLen += MAC_LPWMN_ID_LEN;
       }

       if (pduInfo_p->sam == MAC_ADDRESS_MODE_SHORT_ADDR)
           hdrLen += MAC_SHORT_ADDR_LEN;
       else
           hdrLen += MAC_EXT_ADDR_LEN;
   }

#ifdef MAC_SEC_ENA
   /*
    * <802.15.4-2011>
    * 5.2.1.7 Auxiliary Security Header field
    * The Auxiliary Security Header field specifies information required for
    * security processing. This field shall be present only if the Security
    * Enabled field is set to one. The formatting of the Auxiliary Security
    * Header field is described in 7.4.
    */
   if (pduInfo_p->secEna)
   {

   }
#endif

   return hdrLen;
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
MAC_sts_t MAC_buildPDU(MAC_pduInfo_s * const pduInfo_p,
                       const MAC_addr_s * const destAddr_p,
                       const MAC_shortAddr_t destShortAddr,
                       UINT8_t indirectTx)
{
   UINT8_t *hdr_p = NULL;
   UINT8_t macHdrLen;
   UINT16_t offset = 0x0;
   MAC_sts_t sts = MAC_STS_SUCCESS;

   /*
    * General MAC PDU frame format -
    * - Frame control <2 bytes>
    * - DSN           <4 bytes>
    * - Dest LPWMN Id <0/2 bytes>
    * - Dest address  <0/2/8 bytes>
    * - Src LPWMN Id  <0/2 bytes>
    * - Src address   <0/2/8>
    * - Aux security hdr <optional>
    * - Payload       <N bytes>
    * - FCS           <2 bytes>
    */

   macHdrLen = MAC_calcHdrLen(pduInfo_p);

   {
      UINT8_t sduBDHndl = pduInfo_p->bdHndl;
      UINT8_t sduLen = SYS_GET_BD_LEN(sduBDHndl);
      UINT8_t pduBDHndl = SYS_allocMem(macHdrLen + sduLen + (indirectTx ? SYS_TRSCN_Q_HNDL_FIELD_LEN : 0));
      if (SYS_BD_HNDL_IS_VALID(pduBDHndl) == 0x0)
      {
          // OOM !!
          SYS_freeMem(sduBDHndl);

          // <TODO> - send confirm to APP
          return MAC_STS_OOM;
      }
      else
      {
          hdr_p = SYS_GET_BD_BUFF(pduBDHndl);
          if (indirectTx)
              hdr_p += SYS_TRSCN_Q_HNDL_FIELD_LEN;
          memcpy(hdr_p + macHdrLen,
                 SYS_GET_BD_BUFF(sduBDHndl),
                 sduLen);
          SYS_freeMem(sduBDHndl);
          pduInfo_p->bdHndl = pduBDHndl;
      }
   }

   /*
    * 802.15.4-2011
    * 5.2.1.1.7 Frame Version field
    * The Frame Version field specifies the version number corresponding to
    * the frame. This field shall be set to 0x00 to indicate a frame compatible
    * with 802.15.4-2003 and 0x01 to indicate an IEEE 802.15.4 frame. All other
    * field values are reserved. Details on frame compatibility are described in
    * 5.2.3.
    */

   if (pduInfo_p->frameType == MAC_FRAME_TYPE_BEACON)
   {
       // The Frame Version subfield shall be set to one only if the Security Enabled
       // subfield is set to one.
       if (pduInfo_p->secEna)
           pduInfo_p->frameVersion = 1;
       else
           pduInfo_p->frameVersion = 0;
   }
   else
   {
       pduInfo_p->frameVersion = MAC_FRAME_VER_802_15_4;
   }

   offset = MAC_buildFrameCntrlHdr(hdr_p, pduInfo_p);

   /*
    * Fields that are longer than a single octet are sent to the
    * PHY in the order from the octet containing the lowest numbered
    * bits to the octet containing the highest numbered bits.
    */

   /*
    * <802.15.4-2011>
    * 5.2.1.2 Sequence Number field
    * The Sequence Number field specifies the sequence identifier for
    * the frame. For a beacon frame, the Sequence Number field shall
    * specify a BSN. For a data, acknowledgment, or MAC command frame,
    * the Sequence Number field shall specify a DSN that is used to match
    * an acknowledgment frame to the data or MAC command frame.
    */
#if (MAC_PDU_HDR_SEQ_NR_FIELD_LEN > 1)
   UTIL_htole32(hdr_p + offset, pduInfo_p->seqNr);
#else
   hdr_p[offset] = pduInfo_p->seqNr;
#endif
   offset += MAC_PDU_HDR_SEQ_NR_FIELD_LEN;

   /*
    * <802.15.4-2011>
    * 5.2.1.1.5 LPWMN ID Compression field
    * The LPWMN ID Compression field specifies whether the MAC frame is to
    * be sent containing only one of the LPWMN identifier fields when both
    * source and destination addresses are present. If this field is set to
    * one and both the source and destination addresses are present, the frame
    * shall contain only the Destination LPWMN Identifier field, and the Source
    * LPWMN Identifier field shall be assumed equal to that of the destination.
    * If this field is set to zero, then the LPWMN Identifier field shall be
    * present if and only if the corresponding address is present.
    */
   if (pduInfo_p->lpwmnIdComp == 0x0)
   {
       if (pduInfo_p->dam != MAC_ADDRESS_MODE_NO_ADDR)
       {
           hdr_p[offset] = MAC_PIB.macLPWMNId & 0xff;
           hdr_p[offset + 1] = (MAC_PIB.macLPWMNId >> 8) & 0xff;
           offset += MAC_LPWMN_ID_LEN;
       }
   }

   /*
    * The Destination Address field, when present, with a length
    * specified in the Destination Addressing Mode field of the
    * Frame Control field as described in 5.2.1.1.6, specifies the
    * address of the intended recipient of the frame. A value of
    * 0xffff in this field shall represent the broadcast short
    * address, which shall be accepted as a valid address by all
    * devices currently listening to the channel.
    */
   if (pduInfo_p->dam != MAC_ADDRESS_MODE_NO_ADDR)
   {
       if (pduInfo_p->dam == MAC_ADDRESS_MODE_SHORT_ADDR)
       {
           UINT16_t da = destAddr_p != NULL ? destAddr_p->shortAddr : destShortAddr;
           hdr_p[offset] = (da) & 0xff;
           hdr_p[offset + 1] = ((da) >> 8) & 0xff;
           offset += MAC_SHORT_ADDR_LEN;
       }
       else
       {
           memcpy(hdr_p + offset, destAddr_p->extAddr, MAC_EXT_ADDR_LEN);
           offset += MAC_EXT_ADDR_LEN;
       }
   }

   /*
    * <802.15.4-2011>
    * 5.2.1.5 Source LPWMN Identifier field
    * The Source LPWMN Identifier field, when present, specifies the unique LPWMN
    * identifier of the originator of the frame. This field shall be included
    * in the MAC frame only if the Source Addressing Mode field is nonzero and
    * the LPWMN ID Compression field is equal to zero. The LPWMN identifier of a
    * device is initially determined during association on a LPWMN but may change
    * following a LPWMN identifier conflict resolution, as described in 5.1.2.2.
    */
   if (pduInfo_p->sam != MAC_ADDRESS_MODE_NO_ADDR)
   {
       if (pduInfo_p->lpwmnIdComp == 0x0
           || (pduInfo_p->dam == MAC_ADDRESS_MODE_NO_ADDR))
       {
           hdr_p[offset] = MAC_PIB.macLPWMNId & 0xff;
           hdr_p[offset + 1] = (MAC_PIB.macLPWMNId >> 8) & 0xff;
           offset += MAC_LPWMN_ID_LEN;
       }

       if (pduInfo_p->sam == MAC_ADDRESS_MODE_SHORT_ADDR)
       {
           hdr_p[offset] = (MAC_PIB.macShortAddress) & 0xff;
           hdr_p[offset + 1] = ((MAC_PIB.macShortAddress) >> 8) & 0xff;
           offset += MAC_SHORT_ADDR_LEN;
       }
       else
       {
           memcpy(hdr_p + offset,
                  MAC_PIB.macExtendedAddress,
                  MAC_EXT_ADDR_LEN);
           offset += MAC_EXT_ADDR_LEN;
       }
   }

#ifdef MAC_SEC_ENA
   /*
    * <802.15.4-2011>
    * 5.2.1.7 Auxiliary Security Header field
    * The Auxiliary Security Header field specifies information required for
    * security processing. This field shall be present only if the Security
    * Enabled field is set to one. The formatting of the Auxiliary Security
    * Header field is described in 7.4.
    */
   if (pduInfo_p->secEna)
   {

   }
#endif

   {
      UINT16_t pduLen = SYS_GET_BD_LEN(pduInfo_p->bdHndl);
      if (indirectTx)
          pduLen --;
      if (pduLen > CC1200_MAX_MAC_PDU_LEN)
      {
          SYS_freeMem(pduInfo_p->bdHndl);
          sts = MAC_STS_FRAME_TOO_LONG;
      }
   }

   return sts;
}


#ifndef DEV_TYPE_RFD
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
SYS_trscnQ_s *MAC_checkFrameIsPending(MAC_shortAddr_t shortAddr)
{
#ifdef LPWMN_COORD
   return NM_checkPendingFrame(shortAddr);
#else
   return NULL;  // <TODO>
#endif
}


#ifndef DEV_TYPE_RFD
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void MAC_sendPendFrame(void)
{
   UINT8_t bdHndl;
   SYS_trscnQ_s *trscnQ_p;

   if (MAC_cntxt.pendIndTrxDestAddr == MAC_BROADCAST_SHORT_ADDR)
       SYS_fatal(SYS_FATAL_ERR_1);

   // Make sure that this RFD has not disappeared ...

#ifdef LPWMN_COORD
   trscnQ_p = NM_checkPendingFrame(MAC_cntxt.pendIndTrxDestAddr);
#else
#error TODO
#endif

   if (trscnQ_p == NULL)
       SYS_fatal(SYS_FATAL_ERR_1);

   if (SYS_TRSCN_Q_IS_EMPTY(trscnQ_p))
       SYS_fatal(SYS_FATAL_ERR_1);

   bdHndl = SYS_GET_TRSCN_Q_HEAD(trscnQ_p);
   SYS_delFromTrscnsQ(trscnQ_p, bdHndl);

   // In this implementation, if this frame is not successfully transmitted, it
   // is discarded (instead of being put back at the head of the RFD's pending frame
   // queue).

   SYS_ADJ_BD_HEAD(bdHndl, SYS_TRSCN_Q_HNDL_FIELD_LEN);

   MAC_cntxt.ackRequired = 1;   // Unicast packets always require an ACK
   MAC_cntxt.txAttempCnt = 1;
   MAC_cntxt.macPduLen = SYS_GET_BD_LEN(bdHndl);
   {
      UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);
#if (MAC_PDU_HDR_SEQ_NR_FIELD_LEN > 1)
      MAC_cntxt.expAckDSN = UTIL_letoh32(buff_p + MAC_PDU_HDR_FC_FIELD_LEN);
#else
      MAC_cntxt.expAckDSN = buff_p[MAC_PDU_HDR_FC_FIELD_LEN];
#endif
   }

   MAC_cntxt.expAckSrcShortAddr = MAC_cntxt.pendIndTrxDestAddr;
   MAC_cntxt.cnfrmReqd = 0x0;  // No confirmation required !!

   if (SYS_BD_HNDL_IS_VALID(MAC_cntxt.currTxPduHndl))
   {
       SYS_fatal(SYS_FATAL_ERR_1);
   }

   MAC_cntxt.currTxPduHndl = bdHndl;

#ifdef MAC_STATS_ENA
   MAC_cntxt.pendingFrameTxCnt ++;
#endif

   MAC_cntxt.pendingFrameTxInProgress = 1;

   MAC_csmaInit();
   MAC_doCsmaProc();

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
UINT8_t MAC_addFrameToPendQ(MAC_shortAddr_t shortAddr, UINT8_t pdubdHndl)
{
   UINT8_t rc = 1;
   SYS_trscnQ_s *trscnQ_p;

#ifdef LPWMN_COORD
   trscnQ_p = NM_checkPendingFrame(shortAddr);
#else
#error TODO
#endif

   if (trscnQ_p == NULL)
   {
       SYS_fatal(SYS_FATAL_ERR_1);
   }
   else
   {
       SYS_addToTrscnsQ(trscnQ_p, pdubdHndl);
   }

   return rc;
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
UINT8_t MAC_DataReqHndlr(MAC_shortAddr_t rfdAddr)
{
   UINT8_t rc = 0;

   /*
    * The LPWMN coordinator or any router can have multiple frames pending
    * for one or more RFDs. But this implementation supports only one
    * "data-request" command at any time. This means if this (non RFD)
    * node receives a "data-request" command from RFD_X, this node will
    * set the SYS_GLOBAL_EVT_PENDING_FRAME_TRX_REQ bit in SYS_globalEvtMsk1
    * which will trigger the transmission of a pending frame to the
    * requesting RFD (RFD_X) when MAC-TX-MOD becomes free. In the meantime,
    * if another node RFD_Y sends a "data-request" command to this node, this
    * node will set the frame pending bit to 0 in the ACK to RFD_Y even if
    * there is a frame pending for RFD_Y on this node. In fact any acks to
    * any other RFDs will not have the frame pending bit set to 1 as long as
    * the SYS_GLOBAL_EVT_PENDING_FRAME_TRX_REQ remains set. This bit will get
    * cleared when this node was able to send a pending frame to RFD_X or
    * the transmission failed (channel access failure / no ack etc).
    * Pending frames have a life time. On expiration of this lifetime, the
    * pending frame is deleted. Pending frames are also removed if an RFD
    * rejoins through this node or this node receives a purge message from
    * the LPWMN coord if the RFD rejoins through some other non RFD.
    */

   if (MAC_cntxt.pendIndTrxDestAddr != MAC_BROADCAST_SHORT_ADDR)
       return rc;
   else
   {
       SYS_trscnQ_s *pendFrameQ_p = MAC_checkFrameIsPending(rfdAddr);
       if (pendFrameQ_p != NULL)
       {
           MAC_cntxt.timeSinceRFDDataReq = 0;
           MAC_cntxt.pendIndTrxDestAddr = rfdAddr;
           SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_PENDING_FRAME_TRX_REQ;
           rc = 1;
       }
   }

   return rc;
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
void MAC_cmdFrameHandler(UINT8_t *cmd_p,
                         UINT8_t cmdLen,
                         UINT8_t *dataPendFlag_p)
{
    if (cmdLen < MAC_CMD_ID_FIELD_LEN)
        return;

    *dataPendFlag_p = 0x0;

    switch (*cmd_p)
    {
#if !defined(DEV_TYPE_RFD) && !defined(DEV_TYPE_FFD_NO_MESH)
        case MAC_CMD_BEACON_REQUEST:
             {
                /*
                 * The beacon request command is used by a device to locate all
                 * coordinators within its radio communications range during an
                 * active scan. This command is optional for an RFD.
                 *
                 * An active scan uses the beacon request command to extract the
                 * beacon from a coordinator.
                 *
                 * If a coordinator of a non beacon-enabled LPWMN receives this cmd,
                 * it shall transmit a single beacon frame using unslotted CSMA-CA.
                 */

                // Accept beacons only when associated.
                if (MAC_PIB.macShortAddress != MAC_BROADCAST_SHORT_ADDR
#if !defined(LPWMN_COORD) && !defined(DEV_TYPE_RFD)
                    && MAC_cntxt.assocRFDCnt < MAC_MAX_ASSOC_RFD_CNT
#endif
                   )
                {
#ifdef LPWMN_COORD
#ifdef FORCE_HOP
                    // Remove --------------------------------------------------------------
                    {
                        UINT8_t extAddr[ ] = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
                        if (NM_lookUpNode(extAddr) != NULL)
                            break;
                    }
#endif
#endif
                    SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_BCN_REQUEST;
                }
             }
             break;
#endif

#ifndef LPWMN_COORD
        case MAC_CMD_ASSOCIATION_RESP:
             {
                UINT8_t assocSts;

                // Verify the command length
                if (cmdLen != MAC_ASSOC_RESP_CMD_PDU_PYLD_LEN)
                {
                    // Drop this frame
                    break;
                }

                // Looks like extended address conflict
                if (MAC_PIB.macShortAddress != MAC_BROADCAST_SHORT_ADDR)
                {
                    // Drop this frame since this node is already associated
                    break;
                }

                // Cancel the timer waiting for association response
                // PLTFRM_stopTimerA0( );

                MAC_PIB.macShortAddress = MAC_BROADCAST_SHORT_ADDR;

                /*
                 * If the Association Status field of the association response
                 * command indicates that the association was successful, the
                 * device shall store the address contained in the Short Address
                 * field of the command in "macShortAddress".
                 */
                assocSts = cmd_p[MAC_CMD_ID_FIELD_LEN + MAC_SHORT_ADDR_LEN];
                if (assocSts ==  MAC_ASSOCIATION_STS_SUCCESS)
                {
                    MAC_PIB.macShortAddress = cmd_p[MAC_CMD_ID_FIELD_LEN + 1];
                    MAC_PIB.macShortAddress <<= 8;
                    MAC_PIB.macShortAddress |= cmd_p[MAC_CMD_ID_FIELD_LEN];

                    MAC_cntxt.assocCnfrmParams.assocShortAddr = MAC_PIB.macShortAddress;

#ifndef LPWMN_COORD
                    MAC_updateSrcInAckBuff();
#endif
                }

                MAC_cntxt.assocCnfrmParams.status = assocSts;

                SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_MAC_ASSOC_CONFIRM;
             }
             break;
#endif


#ifndef DEV_TYPE_RFD
        case MAC_CMD_ASSOCIATION_REQ:
             {
                // Verify the command length
                if (cmdLen != MAC_ASSOC_REQ_CMD_PDU_PYLD_LEN
                    || MAC_cntxt.rcvdPktInfo.srcAddrMode != MAC_ADDRESS_MODE_EXTENDED_ADDR)
                {
                    // Drop this frame
                    break;
                }

                /*
                 * The MLME-ASSOCIATE.indication primitive is used to indicate the
                 * reception of an association request command.
                 * When the next higher layer of a coordinator receives the MLME-
                 * ASSOCIATE.indication primitive, the coordinator determines whether
                 * to accept or reject the un associated device using an algorithm
                 * outside the scope of this standard.
                 */
#ifdef MAC_STATS_ENA
                MAC_cntxt.assocReqRcvdCnt ++;
#endif

                ADP_macAssocIndHndlr(MAC_cntxt.rcvdPktInfo.srcAddr.extAddr,
                                     cmd_p[1]);
             }
             break;

        case MAC_CMD_DATA_REQUEST:
             {
                /*
                 * The data request command is sent by a device to request data
                 * from the LPWMN coordinator or a coordinator.
                 */

                if (MAC_cntxt.rcvdPktInfo.srcAddrMode == MAC_ADDRESS_MODE_SHORT_ADDR)
                {
                    *dataPendFlag_p = MAC_DataReqHndlr(MAC_cntxt.rcvdPktInfo.srcAddr.shortAddr);
                }
             }
             break;
#endif

        default:
        case MAC_CMD_DISASSOCIATION_NOTIF:

        case MAC_CMD_LPWMN_ID_CONFLICT_NOTIF:
        case MAC_CMD_ORPHAN_NOTIF:
        case MAC_CMD_COORD_REALIGNMENT:
        case MAC_CMD_GTS_REQUEST:
             break;

    }

    return;
}

#ifndef LPWMN_COORD
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void MAC_assocRespTmoHndlr(void)
{
  /*
   * If the device does not extract an association response command frame from
   * the coordinator within "macResponseWaitTime", the MLME shall issue the
   * MLME-ASSOCIATE.confirm primitive, as described in 6.2.2.4, with a status
   * of NO_DATA, and the association attempt shall be deemed a failure.
   */

  MAC_sendAssocCnfrm(MAC_STS_NO_DATA,
                     MAC_BROADCAST_SHORT_ADDR);
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void MAC_assocReqAckHndlr(void)
{
   UINT32_t tmoVal;

   /*
    * The acknowledgment to an association request command does not mean that
    * the device has associated. The next higher layer of the coordinator needs
    * time to determine whether the current resources available on the LPWMN are
    * sufficient to allow another device to associate. The next higher layer
    * should make this decision within "macResponseWaitTime".
    *
    * On receipt of the acknowledgment to the association request command, the
    * device shall wait for at most macResponseWaitTime for the coordinator to
    * make its association decision; the PIB attribute macResponseWaitTime is a
    * network-topology-dependent parameter and may be set to match the specific
    * requirements of the network that a device is trying to join.
    */

#if 0
   /*
    * "macResponseWaitTime" is the max time, in multiples of aBaseSuperframeDuration,
    * a device shall wait for a response command frame to be available following a
    * request command frame. Range is 2 to 64, default value is 32.
    */
   tmoVal = MAC_aBaseSuperframeDuration;  // In symbols
   tmoVal <<= PHY_SYMBOL_LENGTH_USECS_SHIFT; // In microsecs
   tmoVal *= MAC_PIB.macResponseWaitTime;

   // Convert to millisecs
   // X/1000 = (X/1024)*(1.024) = (X/1024) + ((X/1024)*(0.024))
   // < (X/1024) + ((X/1024)*(0.0625)) = ((X >> 10) + ((X >> 10) >> 4))
   tmoVal >>= 10;
   tmoVal += (tmoVal >> 4);
   tmoVal ++;
#else
   // In a mesh network, association request could trigger route discovery resulting in
   // a big delay.
   tmoVal = 10000;  // 10 seconds
#endif

   if (PLTFRM_startTimerA0(tmoVal, 0, MAC_assocRespTmoHndlr) != PLTFRM_STS_SUCCESS)
	   SYS_fatal(SYS_FATAL_ERR_1);

   return;
}
#endif


#ifdef DEV_TYPE_RFD
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void MAC_coordPollTmoExpCbFunc(void)
{
   if (MAC_cntxt.pollReqSent)
       SYS_fatal(SYS_FATAL_ERR_1);

   if (MAC_cntxt.pollTimerStarted == 0x0)
       SYS_fatal(SYS_FATAL_ERR_1);

   MAC_cntxt.pollTimerStarted = 0x0;

   // Timer has expired - no message from coordinator !!

   /*
    * If a frame is not received even though the acknowledgment to the
    * data request command has its Frame Pending subfield set to one, the
    * MLME will issue the MLME-POLL.confirm primitive with a status of
    * NO_DATA.
    */
   MAC_sendPollCnfrm(MAC_STS_NO_DATA);
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
void MAC_ackRcvdHndlr(const UINT8_t framePending)
{
   /*
    * This function is called when a valid ack has been received.
    */

   // Stop the ack-timeout timer
   PLTFRM_stopTimerA0();

#ifdef MAC_STATS_ENA
   MAC_cntxt.validAckRxCnt ++;
#endif

#ifdef DEV_TYPE_RFD
   if (MAC_cntxt.pollReqSent)
   {
       MAC_cntxt.pollReqSent = 0;

       if (framePending)
       {
           /*
            * On receipt of the acknowledgment frame with the Frame Pending
            * field set to one, a device shall enable its receiver for at
            * most macMaxFrameTotalWaitTime to receive the corresponding
            * data frame from the coordinator.
            */

           MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE;

           if (MAC_cntxt.pollTimerStarted)
               SYS_fatal(SYS_FATAL_ERR_1);

           // Wait MAC_COORD_POLL_TIMEOUT_SECS seconds
           if (PLTFRM_startTimerA1(MAC_COORD_POLL_TIMEOUT_SECS, 0, MAC_coordPollTmoExpCbFunc) != PLTFRM_STS_SUCCESS)
           {
               SYS_fatal(SYS_FATAL_ERR_1);
           }
           else
           {
               MAC_cntxt.pollTimerStarted = 1;
           }
       }
       else
       {
           /*
            * If the Frame Pending subfield of the acknowledgment frame is set
            * to zero, the MLME will issue the MLME-POLL.confirm primitive with a
            * status of NO_DATA.
            */
           MAC_sendPollCnfrm(MAC_STS_NO_DATA);
           MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE;
       }

       // Return here since since we do not want to send a data-confirm to
       // ADP/APP.
       return;
   }
   else
   {
       if (framePending)
           SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_COORD_POLL_PENDING;
   }
#else
   if (MAC_cntxt.pendingFrameTxInProgress)
   {
       MAC_stopPendingFrameTrx( );
   }
#endif

   MAC_cntxt.datacnfrmParams.status = MAC_STS_SUCCESS;
   MAC_sendDataCnfrm();

   return;
}


#ifndef DEV_TYPE_RFD
/*
 *******************************************************************
 *
 *
 *
 *
 *******************************************************************
 */
MAC_sts_t MAC_buildSendAssocnRespCmd(const UINT8_t * const extAddr_p,
                                     const UINT8_t status,
                                     const MAC_shortAddr_t allocShortAddr)
{
   MAC_pduInfo_s pduInfo;
   UINT8_t *buff_p;
   UINT8_t offset = 0x0, macHdrLen;
   MAC_sts_t sts = MAC_STS_SUCCESS;

   /*
    * 5.3.2 Association response command
    * The association response cmd allows the LPWMN coordinator or a coordinator
    * to communicate the results of an association attempt back to the device
    * requesting association.
    * This command shall only be sent by the LPWMN coordinator or coordinator to a
    * device that is currently trying to associate.
    * All devices shall be capable of receiving this command, although an RFD is
    * not required to be capable of transmitting it.
    * The association response command shall be formatted as illustrated in Fig
    * 51.
    *
    * ------------------------------------------------------------------------
    * Octets:       Variable           1               2             1
    * ------------------------------------------------------------------------
    *                 MHR        Cmd Frame Id    Short address    Assoc sts
    * ------------------------------------------------------------------------
    */

   pduInfo.frameVersion = MAC_FRAME_VER_802_15_4;
   pduInfo.frameType = MAC_FRAME_TYPE_CMD;
   pduInfo.secEna = 0x0;

   /*
    * The Destination Addressing Mode and Source Addressing Mode fields shall
    * each be set to indicate extended addressing.
    */
   pduInfo.dam = MAC_ADDRESS_MODE_EXTENDED_ADDR;

   // ACK to this packet requires this node's short address
   pduInfo.sam = MAC_ADDRESS_MODE_SHORT_ADDR;

   /*
    *  The Frame Pending field shall be set to zero and ignored upon reception,
    * and the AR field shall be set to one.
    */
   pduInfo.framePending = 0x0;
   pduInfo.ackRequired = 0x1;

   /*
    * The LPWMN ID Compression field shall be set to one. In accordance with this
    * value of the LPWMN ID Compression field, the Destination LPWMN Identifier field
    * shall contain the value of macLPWMNId, while the Source LPWMN Identifier field
    * shall be omitted.
    */
   pduInfo.lpwmnIdComp = 0x1;

   macHdrLen = MAC_calcHdrLen(&pduInfo);
   pduInfo.bdHndl = SYS_allocMem(macHdrLen + MAC_ASSOC_RESP_CMD_PDU_PYLD_LEN);
   if (SYS_BD_HNDL_IS_VALID(pduInfo.bdHndl) == 0x0)
       return MAC_STS_OOM;

   buff_p = SYS_GET_BD_BUFF(pduInfo.bdHndl);

   offset = MAC_buildFrameCntrlHdr(buff_p, &pduInfo);

   /*
    * The Sequence Number field shall contain the current value of macDSN.
    * macDSN is the sequence number added to the transmitted data or MAC
    * command frame.
    */
   pduInfo.seqNr = MAC_getNextDSN();
#if (MAC_PDU_HDR_SEQ_NR_FIELD_LEN > 1)
   UTIL_htole32(buff_p + offset, pduInfo.seqNr);
#else
   buff_p[offset] = pduInfo.seqNr;
#endif
   offset += MAC_PDU_HDR_SEQ_NR_FIELD_LEN;

   buff_p[offset] = MAC_PIB.macLPWMNId & 0xff;
   buff_p[offset + 1] = (MAC_PIB.macLPWMNId >> 8) & 0xff;
   offset += MAC_LPWMN_ID_LEN;

   /*
    * The Destination Address field shall contain the extended address of the
    * device requesting association.
    */
   memcpy(buff_p + offset, extAddr_p, MAC_EXT_ADDR_LEN);
   offset += MAC_EXT_ADDR_LEN;

   /*
    * The Source Address field shall contain the short address of this node.
    */
   buff_p[offset] =  MAC_PIB.macShortAddress & 0xff;
   buff_p[offset + 1] = (MAC_PIB.macShortAddress >> 8) & 0xff;
   offset += MAC_SHORT_ADDR_LEN;

   // pduInfo.hdr_p = hdr_p;
   // pduInfo.hdrLen = offset;

   buff_p[offset] = MAC_CMD_ASSOCIATION_RESP;
   offset ++;

   buff_p[offset] = allocShortAddr & 0xff;
   buff_p[offset + 1] = (allocShortAddr >> 8) & 0xff;
   offset += MAC_SHORT_ADDR_LEN;
   buff_p[offset] = status;
   offset ++;

   MAC_cntxt.ackRequired = 0x1;
   MAC_cntxt.txAttempCnt = 1;
   MAC_cntxt.expAckDSN =  pduInfo.seqNr;
   MAC_cntxt.expAckSrcShortAddr = allocShortAddr;
   MAC_cntxt.macPduLen = macHdrLen + MAC_ASSOC_RESP_CMD_PDU_PYLD_LEN;
   MAC_cntxt.cnfrmReqd = 0x0; // No confirmation required

   if (SYS_BD_HNDL_IS_VALID(MAC_cntxt.currTxPduHndl))
   {
       SYS_fatal(SYS_FATAL_ERR_1);
   }

   MAC_cntxt.currTxPduHndl = pduInfo.bdHndl;

   MAC_csmaInit();
   MAC_doCsmaProc();

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
void MAC_buildSigPktMacHdr(const MAC_shortAddr_t destAddr,
                           const UINT16_t sduLen)
{
#if 0
   const UINT8_t macHdrLen = MAC_PDU_HDR_FC_FIELD_LEN + MAC_PDU_HDR_SEQ_NR_FIELD_LEN
                       + MAC_LPWMN_ID_LEN + (MAC_SHORT_ADDR_LEN*2);
   UINT8_t macPduLen =  macHdrLen + sduLen + MAC_FCS_LEN;
#if (MAC_PDU_HDR_SEQ_NR_FIELD_LEN > 1)
   UINT32_t dsn = MAC_getNextDSN();
#else
   UINT8_t dsn = MAC_getNextDSN();
#endif

   do
   {
      UINT8_t byte, buff[2];

      RADIO_flushTx();
      cc2520_memRead(CC2520_TXFIFOCNT, 0x1, &byte);
      if (byte != 0x0)
      {
          SYS_fatal(SYS_FATAL_ERR_1);
          continue;
      }

      /*
       * When AUTOCRC = 1, the contents of the TX FIFO should be:
       *---------------------------------------------------------
       * LEN (1 byte)  |  MAC PDU (LEN - 2 bytes)  | ignored
       *---------------------------------------------------------
       */
      cc2520_writeToTxFifo(1, &macPduLen);

      // Frame control field
      buff[0] = MAC_FRAME_TYPE_DATA | MAC_FC_LPWMN_ID_COMP_BIT_SHIFT_MSK;

      if (destAddr != MAC_BROADCAST_SHORT_ADDR)
          buff[0] |= MAC_FC_ACK_REQ_BIT_SHIFT_MSK;

      byte = MAC_ADDRESS_MODE_SHORT_ADDR;
      byte = (byte << MAC_FC_DAM_BITS_SHIFT) & MAC_FC_DAM_BITS_SHIFT_MSK;
      buff[1] = byte;

      byte = MAC_FRAME_VER_802_15_4;
      byte = (byte << MAC_FC_FRAME_VER_BITS_SHIFT) & MAC_FC_FRAME_VER_BITS_SHIFT_MSK;
      buff[1] |= byte;

      byte = MAC_ADDRESS_MODE_SHORT_ADDR;
      byte = (byte << MAC_FC_SAM_BITS_SHIFT) & MAC_FC_SAM_BITS_SHIFT_MSK;
      buff[1] |= byte;

      cc2520_writeToTxFifo(MAC_PDU_HDR_FC_FIELD_LEN, buff);

      // DSN
      cc2520_writeToTxFifo(MAC_PDU_HDR_SEQ_NR_FIELD_LEN, &dsn);

      // Destination LPWMN-ID
      buff[0] = MAC_PIB.macLPWMNId & 0xff;
      buff[1] = (MAC_PIB.macLPWMNId >> 8) & 0xff;
      cc2520_writeToTxFifo(MAC_LPWMN_ID_LEN, buff);

      // Destination short address
      buff[0] = destAddr & 0xff;
      buff[1] = (destAddr >> 8) & 0xff;
      cc2520_writeToTxFifo(MAC_SHORT_ADDR_LEN, buff);

      // Source short address
      buff[0] = MAC_PIB.macShortAddress & 0xff;
      buff[1] = (MAC_PIB.macShortAddress >> 8) & 0xff;
      cc2520_writeToTxFifo(MAC_SHORT_ADDR_LEN, buff);

      cc2520_memRead(CC2520_TXFIFOCNT, 0x1, &byte);
      if (byte == (macHdrLen + 1))
          break;
      else
          SYS_fatal(SYS_FATAL_ERR_1);
   } while (1);

   MAC_cntxt.ackRequired = destAddr != MAC_BROADCAST_SHORT_ADDR ? 0x1 : 0x0;
   MAC_cntxt.txAttempCnt = 1;
   if (MAC_cntxt.ackRequired)
   {
       MAC_cntxt.expAckDSN =  dsn;
       MAC_cntxt.expAckSrcShortAddr = destAddr;
   }
   MAC_cntxt.cnfrmReqd = 0x0;  // Need confirm for RREP and PREQ/PREP messages
   MAC_cntxt.macPduLen = macPduLen;

   MAC_cntxt.datacnfrmParams.status = MAC_STS_TX_PENDING;
   MAC_cntxt.datacnfrmParams.destShortAddr = destAddr;
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
void MAC_startSigPktTx(void)
{
#if 0
   MAC_csmaInit();
   MAC_doCsmaProc(MAC_IFS_TYPE_NONE);
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
void MAC_buildFCInAckBuff(void)
{
   UINT8_t byte;
   UINT8_t *buff_p = MAC_cntxt.ackTxBuff;

   byte = MAC_FRAME_TYPE_ACK;

   // if (frameInfo_p->secEna)
   //     byte |= MAC_FC_SEC_ENA_BIT_SHIFT_MSK;

   // if (frameInfo_p->framePending)
   //     byte |= MAC_FC_PENDING_BIT_SHIFT_MSK;

   byte |= MAC_FC_LPWMN_ID_COMP_BIT_SHIFT_MSK;
   buff_p[0] = byte;

   byte = MAC_ADDRESS_MODE_SHORT_ADDR;
   byte = (byte << MAC_FC_DAM_BITS_SHIFT) & MAC_FC_DAM_BITS_SHIFT_MSK;
   buff_p[1] = byte;

   byte = MAC_FRAME_VER_802_15_4;
   byte = (byte << MAC_FC_FRAME_VER_BITS_SHIFT) & MAC_FC_FRAME_VER_BITS_SHIFT_MSK;
   buff_p[1] |= byte;

   byte = MAC_ADDRESS_MODE_SHORT_ADDR;
   byte = (byte << MAC_FC_SAM_BITS_SHIFT) & MAC_FC_SAM_BITS_SHIFT_MSK;
   buff_p[1] |= byte;

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
void MAC_updateSrcInAckBuff(void)
{
   UINT8_t *buff_p = MAC_cntxt.ackTxBuff + MAC_PDU_HDR_FC_FIELD_MAX_LEN + MAC_PDU_HDR_SEQ_NR_FIELD_LEN;

   buff_p = UTIL_htole16(buff_p, MAC_PIB.macLPWMNId);
   buff_p += MAC_SHORT_ADDR_LEN;

   UTIL_htole16(buff_p, MAC_PIB.macShortAddress);

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
void MAC_updateAckBuff(const MCPS_dataIndParams_s * const rcvdPktInfo_p,
                       const UINT8_t framePending)
{
   UINT8_t *buff_p = MAC_cntxt.ackTxBuff;
   if (framePending)
       buff_p[0] |= MAC_FC_PENDING_BIT_SHIFT_MSK;
   else
       buff_p[0] &= ~MAC_FC_PENDING_BIT_SHIFT_MSK;
   buff_p += MAC_PDU_HDR_FC_FIELD_MAX_LEN;
#if (MAC_PDU_HDR_SEQ_NR_FIELD_LEN > 1)
   UTIL_htole32(buff_p, rcvdPktInfo_p->dsn);
#else
   *(buff_p) = rcvdPktInfo_p->dsn;
#endif
   buff_p += (MAC_PDU_HDR_SEQ_NR_FIELD_LEN + MAC_LPWMN_ID_LEN);
   buff_p = UTIL_htole16(buff_p, rcvdPktInfo_p->srcAddr.shortAddr);
   *(buff_p ++) = rcvdPktInfo_p->rssi;
   *(buff_p) = rcvdPktInfo_p->lqi;

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
void MAC_sendAckNow(void)
{
   PLTFRM_sts_t sts;

   // Go to IDLE mode and then send STX strobe.
   sts = CC1200_forceIdle();
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);

   /*
    * Here we are sending an ACK packet after receiving an packet which needs to be
    * acknowledged.
    *
    * After sync is found the frequency offset estimate is saved in the FREQOFF_EST
    * registers. When SAFC is strobed in IDLE the content of FREQOFF_EST is copied
    * to FREQ_OFF. This value is used to calculate the RF frequency.
    * A SAFC command strobe can be issued in any state but does not take effect until
    * the next time the radio enters active mode (TX or RX).
    *
    * SYNC found ----> FREQOFF_EST ---------> FREQ_OFF
    *
    * Note that the FREQOFF programming and FREQOFF_EST (found in FREQOFF_EST1 and
    * FREQOFF_EST0) have identical formats hence the frequency estimate can be accumulated
    * directly to the FREQOFF programming. This can be done either manually or automatically
    * through the SAFC command strobe. A SAFC command strobe can be issued in any state
    * but does not take effect until the next time the radio enters active mode (TX or RX).
    *
    * Issue SAFC here since RADIO is in IDLE state right now and will be going to TX
    * next.
    */

   sts = CC1200_sendCmdStrobe(CC1200_CMD_STROBE_SAFC, NULL);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_1);
   }


#if 0
   // Change CCA mode to skip CCA before transmission
   sts = CC1200_disableCCA();
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);
   else
#endif
   {
       // *** Do not touch any of these variables below ****
       // -  MAC_cntxt.ackRequired = 0;
       // -  MAC_cntxt.txAttempCnt = 1;
       // -  MAC_cntxt.macPduLen = MAC_SIM_802_15_4_ACK_PDU_LEN;
       // -  MAC_cntxt.expAckDSN =  0;
       // -  MAC_cntxt.cnfrmReqd = 0;

       // This function below will first flush the TX FIFO if it not empty.
       sts = CC1200_writeBuffToTxFIFO(MAC_cntxt.ackTxBuff, 
                                      sizeof(MAC_cntxt.ackTxBuff));
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_1);

       // Enable "TXONCCA_DONE" signal to interrupt the micro
       CC1200_enaGPIO2Int();

       // STX strobe
       // In IDLE state: Enable TX. Perform calibration first if SETTLING_CFG.FS_AUTOCAL = 1.

       sts = CC1200_sendCmdStrobe(CC1200_CMD_STROBE_STX, NULL);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_1);
       else
       {
           UINT16_t tmoMilliSecs, txOnCCALoopCnt;

#ifdef MAC_STATS_ENA
           MAC_cntxt.txStrobeCnt ++;
#endif

           txOnCCALoopCnt = 0;

           do
           {
	          PLTFRM_delay1MilliSec();
	          if (CC1200_cntxt.txOnCCADoneFlag)
		          break;
	          txOnCCALoopCnt ++;
	          if (txOnCCALoopCnt > 20)
		          break;
           } while (1);


           CC1200_disGPIO2Int();

           if (txOnCCALoopCnt > 20)
           {
               // It is possible that the radio is an error state. Recover if so.
               CC1200_checkRadioErrors();

#ifdef MAC_STATS_ENA
               MAC_cntxt.ccaDoneTmoCnt ++;
#endif

               // Just pretend TX is in progress and let the tx-over-tmo func
               // do it's job
           }

           // Wait for CC1200 to go back to RX state.
           CC1200_setUpRxEntryInt();

           CC1200_setCurrState(CC1200_STATE_TX_IN_PROGRESS);

#ifdef MAC_STATS_ENA
           MAC_cntxt.ackTrxStartedCnt ++;
#endif

           tmoMilliSecs = CC1200_calcPktTrxTime(MAC_SIM_802_15_4_ACK_PDU_LEN);
           tmoMilliSecs += (CC1200_TX_TO_RX_TURN_AROUND_TIME_MSECS*2);
           if (PLTFRM_startTimerA0(tmoMilliSecs, 0, MAC_txOverTmoCbFunc) != PLTFRM_STS_SUCCESS)
               SYS_fatal(SYS_FATAL_ERR_1);

           // CCA passed. Transmission underway
           MAC_cntxt.txModState = MAC_TX_MOD_STATE_WAIT_TX_OVER;

           MAC_cntxt.immAckTxInProgress = 0x1;
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
void MAC_schedAckTrx(const MCPS_dataIndParams_s * const rcvdPktInfo_p,
                     const UINT8_t framePending)
{
   switch (MAC_cntxt.txModState)
   {
      case MAC_TX_MOD_STATE_CSMA_BACK_OFF:
           {
#ifdef MAC_STATS_ENA
               MAC_cntxt.immAckInCsmaBackOffCnt ++;
#endif
               PLTFRM_stopTimerA0();
               MAC_updateAckBuff(rcvdPktInfo_p, framePending);
               // Send ACK now without doing CSMA/CA
               MAC_cntxt.backOffInterrupted = 1;
               MAC_cntxt.ackRxPending = 0;
               MAC_sendAckNow();
           }
           break;

      case MAC_TX_MOD_STATE_WAIT_ACK:
           {
#ifdef MAC_STATS_ENA
               MAC_cntxt.immAckInWaitAckCnt ++;
#endif
               // This node is expecting an ack but got a data/cmd packet
               // instead !!
               PLTFRM_stopTimerA0();
               MAC_updateAckBuff(rcvdPktInfo_p, framePending);
               // Send ACK now without doing CSMA/CA
               MAC_cntxt.ackRxPending = 1;
               MAC_sendAckNow();

           }
           break;

      case MAC_TX_MOD_STATE_IDLE:
           {
#ifdef MAC_STATS_ENA
               MAC_cntxt.immAckWhenIdle ++;
#endif
               MAC_updateAckBuff(rcvdPktInfo_p, framePending);
               // Send ACK now without doing CSMA/CA
               MAC_cntxt.ackRxPending = 0;
               MAC_cntxt.backOffInterrupted = 0;
               MAC_sendAckNow();
           }
           break;

      default:
           break;
   }
}


#ifdef DEV_TYPE_RFD
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t MAC_coordPollProc(void)
{
   UINT8_t rc = 0;

   if (MAC_cntxt.pollReqSent
       || MAC_cntxt.pollTimerStarted)
   {
       if (MAC_cntxt.pollTimerStarted)
       {
           PLTFRM_stopTimerA1();
           MAC_cntxt.pollTimerStarted = 0;
       }
       else
       {
           // Looks like the ACK (with pending bit set to 1) got lost !!
           // Act as if ACK was received ...
           MAC_cntxt.pollReqSent = 0;
       }

       SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_COORD_POLL_PENDING;

       rc = 1;
   }

   return rc;
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
UINT8_t MAC_checkDupPkt(MCPS_dataIndParams_s *rcvdPktInfo_p)
{
    UINT8_t rc = 0;

#ifdef DEV_TYPE_RFD
    if (MAC_cntxt.rcvdPktInfo.dsn == MAC_cntxt.lastRcvdDSN)
    {
#ifdef MAC_STATS_ENA
       MAC_cntxt.dupPktRcvdCnt ++;
#endif
       rc = 1;
    }
    else
        MAC_cntxt.lastRcvdDSN = MAC_cntxt.rcvdPktInfo.dsn;
#else
#warning TODO
#endif

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
UINT8_t MAC_processRcvdPdu(const UINT8_t bdHndl,
                           const SINT16_t rssi,
                           const SINT16_t lqi)
{
   UINT8_t addrMode, rc = 1,
           macPduLen = SYS_GET_BD_LEN(bdHndl),
           dstShortAddrHdrOff = 0x0, srcShortAddrHdrOff = 0x0;
   UINT8_t frameType, ackReqFlag = 0, framePending = 0;
   UINT16_t hdrOff;
   MAC_addr_s destAddr;
   UINT8_t *macPdu_p = SYS_GET_BD_BUFF(bdHndl);

#ifdef MAC_STATS_ENA
   MAC_cntxt.pduRcvdCnt ++;
#endif

   memset(&MAC_cntxt.rcvdPktInfo, 0, sizeof(MCPS_dataIndParams_s));

   // BD buffer has a two bytes prefix.
   macPdu_p += (SYS_TRSCN_Q_HNDL_FIELD_LEN + SYS_TRSCN_TYPE_FIELD_LEN);
   macPduLen -= (SYS_TRSCN_Q_HNDL_FIELD_LEN + SYS_TRSCN_TYPE_FIELD_LEN);


   frameType = (*macPdu_p) & MAC_FC_FRAME_TYPE_BITS_SHIFT_MSK;
#ifndef LPWMN_COORD
   if (MAC_cntxt.scanInProgress && frameType != MAC_FRAME_TYPE_BEACON)
   {
       // Only accepting beacons at this point
       return 0;
   }
#endif

   // This function will return 0 if it drops the packet. The caller will free
   // the packet.

   /*
    * If the MAC sub-layer is not in promiscuous mode (i.e., macPromiscuousMode
    * is set to FALSE), it shall accept only frames that satisfy all of the
    * following third-level filtering requirements:
    *
    * > The Frame Type field shall not contain a reserved frame type.
    */

   /*
    * > The Frame Version field shall not contain a reserved value.
    */
   {
      UINT8_t frameVer = macPdu_p[1];
      frameVer >>= MAC_FC_FRAME_VER_BITS_SHIFT;
      frameVer &= MAC_FC_FRAME_VER_BITS_MSK;
      if (frameVer > MAC_FRAME_VER_802_15_4)
          return 0;
   }

   ackReqFlag = (macPdu_p[0] & MAC_FC_ACK_REQ_BIT_SHIFT_MSK) ? 0x1 : 0x0;

   hdrOff = MAC_PDU_HDR_FC_FIELD_LEN;

#if (MAC_PDU_HDR_SEQ_NR_FIELD_LEN > 1)
   MAC_cntxt.rcvdPktInfo.dsn = UTIL_letoh32(macPdu_p + hdrOff);
#else
   MAC_cntxt.rcvdPktInfo.dsn = macPdu_p[hdrOff];
#endif
   hdrOff += MAC_PDU_HDR_SEQ_NR_FIELD_LEN; // Beyond the DSN field

   addrMode = macPdu_p[1];
   addrMode = (addrMode >> MAC_FC_DAM_BITS_SHIFT) & MAC_FC_DAM_BITS_MSK;
   if (addrMode >= MAC_ADDRESS_MODE_SHORT_ADDR)
   {
       UINT16_t destLPWMNId = macPdu_p[hdrOff + 1];
       destLPWMNId = (destLPWMNId << 8) | macPdu_p[hdrOff];

       /*
        * > If a destination LPWMN identifier is included in the frame, it shall match
        *   macLPWMNId or shall be the broadcast LPWMN identifier.
        */
       if (destLPWMNId != MAC_BROADCAST_LPWMN_ID
           && destLPWMNId != MAC_PIB.macLPWMNId)
       {
#ifdef MAC_STATS_ENA
           MAC_cntxt.destLPWMNIdMisMatchCnt ++;
#endif
           return 0;
       }

       if (destLPWMNId == MAC_BROADCAST_LPWMN_ID)
           MAC_cntxt.rcvdPktInfo.destLPWMNBc = 0x1;
       else
           MAC_cntxt.rcvdPktInfo.destLPWMNBc = 0x0;

       MAC_cntxt.rcvdPktInfo.destLPWMNIdPresent = 0x1;
       MAC_cntxt.rcvdPktInfo.destLPWMNId = destLPWMNId;

       hdrOff += MAC_LPWMN_ID_LEN;

       /*
        * > If a short destination address is included in the frame, it shall match
        *   either macShortAddress or the broadcast address. Otherwise, if an extended
        *   destination addr is included in the frame, it shall match macExtendedAddress.
        */
       if (addrMode == MAC_ADDRESS_MODE_SHORT_ADDR)
       {
           destAddr.shortAddr = macPdu_p[hdrOff + 1];
           destAddr.shortAddr = (destAddr.shortAddr << 8) | macPdu_p[hdrOff];
           if (destAddr.shortAddr != MAC_BROADCAST_SHORT_ADDR
               && destAddr.shortAddr != MAC_PIB.macShortAddress)
           {
#ifdef MAC_STATS_ENA
               MAC_cntxt.destShortAddrMisMatchCnt ++;
#endif
               return 0;
           }

           dstShortAddrHdrOff = hdrOff;

           hdrOff += MAC_SHORT_ADDR_LEN;
       }
       else
       {
           UINT8_t idx;

           // extended address mode
           for (idx=0; idx<MAC_EXT_ADDR_LEN; idx++)
           {
                if (macPdu_p[hdrOff + idx] != MAC_PIB.macExtendedAddress[idx])
                    break;
           }

           if (idx < MAC_EXT_ADDR_LEN)
           {
#ifdef MAC_STATS_ENA
               MAC_cntxt.destExtAddrMisMatchCnt ++;
#endif
               return 0;
           }
           hdrOff += MAC_EXT_ADDR_LEN;
       }
   }

   MAC_cntxt.rcvdPktInfo.destAddrMode = addrMode;

   MAC_cntxt.rcvdPktInfo.srcAddrMode = MAC_ADDRESS_MODE_NO_ADDR;
   MAC_cntxt.rcvdPktInfo.srcAddr.shortAddr = MAC_BROADCAST_SHORT_ADDR;
   addrMode = macPdu_p[1];
   addrMode = (addrMode >> MAC_FC_SAM_BITS_SHIFT) & MAC_FC_SAM_BITS_MSK;
   if (addrMode >= MAC_ADDRESS_MODE_SHORT_ADDR)
   {
       /*
        * The LPWMN ID Compression field specifies whether the MAC frame is to
        * be sent containing only one of the LPWMN identifier fields when both
        * source and destination addresses are present.
        *
        * If this field is set to one and both the source and destination
        * addresses are present, the frame shall contain only the Destination
        * LPWMN Identifier field, and the Source LPWMN Identifier field shall be
        * assumed equal to that of the destination.
        */
       if (!(macPdu_p[0] & MAC_FC_LPWMN_ID_COMP_BIT_SHIFT_MSK))
       {
           UINT16_t _lpwmnId;
           /*
            * If this field is set to zero, then the LPWMN Identifier field shall
            * be present if and only if the corresponding address is present.
            */

           _lpwmnId = macPdu_p[hdrOff + 1];
           _lpwmnId = (_lpwmnId << 8) | macPdu_p[hdrOff];
           MAC_cntxt.rcvdPktInfo.srcLPWMNId = _lpwmnId;
           MAC_cntxt.rcvdPktInfo.srcLPWMNIdPresent = 1;
           hdrOff += MAC_LPWMN_ID_LEN;
       }

       if (addrMode == MAC_ADDRESS_MODE_SHORT_ADDR)
       {
           UINT16_t shortAddr = macPdu_p[hdrOff + 1];
           shortAddr = (shortAddr << 8) | macPdu_p[hdrOff];
#ifdef DEV_TYPE_RFD
           // Accept messages from parent only
           if (MAC_PIB.macCoordShortAddress != MAC_BROADCAST_SHORT_ADDR)
           {
               if (shortAddr != MAC_PIB.macCoordShortAddress)
                   return 0;
           }
           MAC_cntxt.rcvdPktInfo.srcAddr.shortAddr = shortAddr;
#endif
           srcShortAddrHdrOff = hdrOff;
           hdrOff += MAC_SHORT_ADDR_LEN;
       }
       else
       {
           memcpy(MAC_cntxt.rcvdPktInfo.srcAddr.extAddr,
                  macPdu_p + hdrOff,
                  MAC_EXT_ADDR_LEN);
           hdrOff += MAC_EXT_ADDR_LEN;
       }
   }
   MAC_cntxt.rcvdPktInfo.srcAddrMode = addrMode;

#ifdef FLASH_LED_ON_PKT_RX_TX
   PLTFRM_ON_LED_1();
#endif

#ifndef DEV_TYPE_RFD
   if (ackReqFlag
       && (MAC_cntxt.pendIndTrxDestAddr == MAC_BROADCAST_SHORT_ADDR))
   {
#ifndef DEV_TYPE_RFD
       if ((MAC_cntxt.rcvdPktInfo.srcAddrMode == MAC_ADDRESS_MODE_SHORT_ADDR)
#ifdef LPWMN_COORD
           && (NM_lookUpLocalRFD(MAC_cntxt.rcvdPktInfo.srcAddr.shortAddr))
#else
           && (MAC_lookUpAssocRFDList(MAC_cntxt.rcvdPktInfo.srcAddr.shortAddr))
#endif
       )
       {
           if (MAC_checkFrameIsPending(MAC_cntxt.rcvdPktInfo.srcAddr.shortAddr))
               framePending = 1;
       }
#endif
   }
#endif

   // Drop duplicate packets
   if (frameType == MAC_FRAME_TYPE_CMD
       || frameType == MAC_FRAME_TYPE_DATA)
   {
       if (MAC_checkDupPkt(&MAC_cntxt.rcvdPktInfo))
       {
           if (ackReqFlag)
           {
               // Schedule ack transmission
               MAC_schedAckTrx(&MAC_cntxt.rcvdPktInfo, framePending);
           }
           return 0;
       }

#ifdef DEV_TYPE_RFD

       // An RFD can receive a DATA packet only during the polling process
       // An RFD can receive a CMD packet when joining or during the polling
       // process

       if (MAC_cntxt.pollReqSent == 0x0 && MAC_cntxt.pollTimerStarted == 0x0)
       {
           // RFD is not polling the parent at this time.
           // Only accept command frames if this node has not joined the nwk
           if (frameType != MAC_FRAME_TYPE_CMD
               || MAC_PIB.macShortAddress != MAC_BROADCAST_SHORT_ADDR)
           {
#ifdef MAC_STATS_ENA
               MAC_cntxt.rfdDropUnExpFrameCnt ++;
#endif
               return 0;
           }
       }

       if (MAC_cntxt.pollReqSent)
       {
           // This RFD has sent out a data request command and is waiting
           // for an ack. Instead it has received a cmd or data frame.
           // This means the parent received the data request command.
           // No point waiting for the ACK now.

           // Note that pollReqSent is set to 1 right before the CSMA procedure
           // is started. "txModState" is set to MAC_TX_MOD_STATE_WAIT_ACK only
           // after the "data-request cmd" is successfully sent out.

           MAC_ackRcvdHndlr(1);
       }
#endif
   }



   // macPdu_p is pointing at the frame control field
   switch (frameType)
   {
      case MAC_FRAME_TYPE_CMD:
           {
              MAC_cmdFrameHandler(macPdu_p + hdrOff,
                                  macPduLen - hdrOff,
                                  &framePending);
              if (ackReqFlag)
              {
                  // Schedule ack transmission
                  MAC_schedAckTrx(&MAC_cntxt.rcvdPktInfo, framePending);
              }

              rc = 0;  // Caller will free memory
           }
           break;

      case MAC_FRAME_TYPE_ACK:
           {
              // Ack packet has source and destination short address and
              // at least one non broadcast lpwmn id. There is no payload.
              if (dstShortAddrHdrOff > 0x0 && srcShortAddrHdrOff > 0x0
                  && MAC_cntxt.rcvdPktInfo.destLPWMNIdPresent == 0x1
                  && MAC_cntxt.rcvdPktInfo.destLPWMNId == MAC_PIB.macLPWMNId)
              {
                  if (MAC_cntxt.txModState == MAC_TX_MOD_STATE_WAIT_ACK)
                  {
                      /*
                       * ACK frame MAC PDU format
                       * FCF (2 bytes) | DSN (1 or 4 bytes) | Remote-RSSI | Remote-LQI | RSSI | CORRELATION
                       */

                      if ((MAC_cntxt.rcvdPktInfo.dsn == MAC_cntxt.expAckDSN)
                          && (MAC_cntxt.expAckSrcShortAddr == MAC_cntxt.rcvdPktInfo.srcAddr.shortAddr))
                      {
                          // ACK has expected DSN
#ifdef DEV_TYPE_RFD
                          MAC_cntxt.txFlrCnt = 0x0;
#endif

                          // Free the packet since we have received an ACK for it.
                          MAC_freeCurrTxPkt();

                          MAC_cntxt.expAckSrcShortAddr = MAC_BROADCAST_SHORT_ADDR;

                          MAC_ackRcvdHndlr((macPdu_p[1] & MAC_FC_PENDING_BIT_SHIFT_MSK) ? 0x1 : 0x0);
                      }
                      else
                      {
#ifdef MAC_STATS_ENA
                          MAC_cntxt.badAckDSNCnt ++;
#endif
                      }
                  }
                  else
                  {
#ifdef MAC_STATS_ENA
                      MAC_cntxt.unExpAckCnt ++;
#endif
                  }
              }
              else
              {
#ifdef MAC_STATS_ENA
                  MAC_cntxt.ackSniffedCnt ++;
#endif
              }

              rc = 0;
           }
           break;

#ifndef LPWMN_COORD
      case MAC_FRAME_TYPE_BEACON:
           {
#ifdef MAC_STATS_ENA
              MAC_cntxt.bcnRcvdCnt ++;
#endif
              rc = 0;

              if (MAC_cntxt.rcvdPktInfo.srcAddrMode < MAC_ADDRESS_MODE_SHORT_ADDR)
                  break;

              {
                  UINT8_t sfOrder = macPdu_p[hdrOff];
                  sfOrder = (sfOrder >> MAC_SF_SPEC_SF_ORDER_BIT_SHIFT);
                  sfOrder &= MAC_SF_SPEC_SF_ORDER_BIT_MSK;

                  // Check the super frame specification field
                  if (((macPdu_p[hdrOff] & MAC_SF_SPEC_BCN_ORDER_BIT_MSK) != 15)
                      || (sfOrder != 15))
                  {
                      // Can associate with non beacon enabled LPWMNs only
                      break;
                  }

                  // Check if this coordinator is accepting association
                  if ((macPdu_p[hdrOff + 1] & MAC_SF_SPEC_ASSOC_PERMIT_BIT_SHIFT_MSK) == 0x0)
                  {
                      // Drop this beacon since coordinator is not accepting association
                      break;
                  }
              }

              /*
               * > If the frame type indicates that the frame is a beacon frame,
               *   the source LPWMN identifier shall match macLPWMNId unless macLPWMNId
               *   is equal to the broadcast LPWMN identifier, in which case the
               *   beacon frame shall be accepted regardless of the source LPWMN id.
               */
              if (MAC_cntxt.scanInProgress)
              {
                  UINT8_t dupBcn = 0;

                  /*
                   * The device shall reject all non beacon frames and record the
                   * information contained in all unique beacons in a LPWMN descriptor
                   * structure, as described in Table 17, including the channel
                   * information and, if required, the preamble code.
                   */
                   if (MAC_PIB.macAutoRequest)
                   {
                       SINT8_t idx = MAC_cntxt.uniqueBcnRcvdCnt;

                       /*
                        * If a beacon frame is received when macAutoRequest is set
                        * to TRUE, the list of LPWMN descriptor structures shall be
                        * stored by the MAC sublayer until the scan is complete; at
                        * this time, the list shall be sent to the next higher layer
                        * in the LPWMN-Descriptor-List parameter of the MLME-SCAN.confirm
                        * primitive. A device shall be able to store at least one LPWMN
                        * descriptor. A beacon frame shall be assumed to be unique if
                        * it contains both a LPWMN identifier and a source address that
                        * has not been seen before during the scan of the current channel.
                        */

                       while (idx > 0)
                       {
                           MAC_lpwmnDescriptor_s *entry_p = &(MAC_cntxt.lpwmnDescriptorList[idx-1]);

                           if (entry_p->channelNumber == PHY_PIB.phyCurrentChannel)
                           {
                               if (entry_p->coordLPWMNId == MAC_cntxt.rcvdPktInfo.srcLPWMNId)
                               {
                                   if (MAC_cntxt.rcvdPktInfo.srcAddrMode == MAC_ADDRESS_MODE_SHORT_ADDR)
                                   {
                                       if (MAC_cntxt.rcvdPktInfo.srcAddr.shortAddr == entry_p->coordAddress.shortAddr)
                                       {
                                           dupBcn = 1;
                                           break;
                                       }
                                       else
                                       {
                                           if (memcmp((void *)MAC_cntxt.rcvdPktInfo.srcAddr.extAddr,
                                                      (void *)entry_p->coordAddress.extAddr,
                                                      MAC_EXT_ADDR_LEN) == 0)
                                           {
                                               dupBcn = 1;
                                               break;
                                           }
                                       }
                                   }
                               }
                           }
                           else
                           {
                               // beacons are arranged by channel number
                               break;
                           }

                           idx --;
                       }

                       if (dupBcn == 0)
                       {
                           MAC_lpwmnDescriptor_s *entry_p = &(MAC_cntxt.lpwmnDescriptorList[MAC_cntxt.uniqueBcnRcvdCnt]);

                           // Insert this beacon in the LPWMN descriptor list
                           entry_p->channelNumber = PHY_PIB.phyCurrentChannel;
                           entry_p->coordLPWMNId = MAC_cntxt.rcvdPktInfo.srcLPWMNId;
                           entry_p->coordAddrMode = MAC_cntxt.rcvdPktInfo.srcAddrMode;
                           entry_p->rssi = rssi;

                           if (entry_p->coordAddrMode == MAC_ADDRESS_MODE_SHORT_ADDR)
                               entry_p->coordAddress.shortAddr = MAC_cntxt.rcvdPktInfo.srcAddr.shortAddr;
                           else
                               memcpy(entry_p->coordAddress.extAddr,
                                      MAC_cntxt.rcvdPktInfo.srcAddr.extAddr,
                                      MAC_EXT_ADDR_LEN);
                           MAC_cntxt.uniqueBcnRcvdCnt ++;
                           if (MAC_cntxt.uniqueBcnRcvdCnt >= MAC_LPWMN_DESCRIPTOR_LIST_MAX_SIZE)
                           {
                               // LPWMN descriptor list is filled up. Terminate scan procedure

                               MAC_cntxt.scanCnfrmParams.lpwmnDescriptorList_p = MAC_cntxt.lpwmnDescriptorList;
                               MAC_cntxt.scanCnfrmParams.resultListSize = MAC_cntxt.uniqueBcnRcvdCnt;
                               MAC_cntxt.scanCnfrmParams.unScannedChanBitMsk = MAC_cntxt.unScannedChanBitMsk;
                               MAC_cntxt.scanCnfrmParams.status = MAC_STS_SUCCESS;
                               MAC_cleanUpScanProc( );
                               SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_SCAN_CONFIRM;
                           }
                       }
                   }
                   else
                   {
                       // TODO
                   }
              }
           }
           break;
#endif

      case MAC_FRAME_TYPE_DATA:
           {
               // Forward to upper layer ...
#ifdef MAC_STATS_ENA
               MAC_cntxt.dataFrameRcvdCnt ++;
#endif

                // MAC data packet destination and source address must be short
               if (dstShortAddrHdrOff == 0x0 || srcShortAddrHdrOff == 0x0)
                   return 0;

#if !defined(DEV_TYPE_RFD)
               // If this packet is not for this node and this node has a route
               // to the packet's destination, reuse the MAC header
               if (ADP_pktFwdCheck(macPdu_p + hdrOff,
                                   macPdu_p + dstShortAddrHdrOff,
                                   macPdu_p + srcShortAddrHdrOff,
                                   macPduLen - hdrOff,
                                   bdHndl))
               {
                   return 1;
               }
#endif

               if (SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_MAC_DATA_INDICATION)
               {
                   // APP has not consumed the previous data indication !!
                   // drop this packet.
                   return 0;
               }

               MAC_cntxt.rcvdPktInfo.rssi = rssi;
               MAC_cntxt.rcvdPktInfo.lqi = lqi;
               MAC_cntxt.rcvdPktInfo.bdHndl = bdHndl;

               // PLTFRM_LOG("<%s> d-pkt s:0x%x/l:%d/r:%d/c:%d>\n\r",
               //           __FUNCTION__, MAC_cntxt.rcvdPktInfo.srcAddr.shortAddr,
               //           macPduLen, rssi, correlation);

               SYS_ADJ_BD_HEAD(bdHndl, hdrOff);

               // BD buffer still has a two bytes prefix for queue handle and
               // transaction type

               {
                  UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);
                  *buff_p = SYS_MAX_BUFF_DESC_CNT;  // init handle
               }

               /*
                * If the valid frame is a data frame, the MAC sub-layer shall pass
                * the frame to the next higher layer. This is achieved by issuing
                * the MCPS-DATA.indication primitive containing the frame info.
                */
               SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_DATA_INDICATION;

               if (ackReqFlag)
               {
                   // Schedule ack transmission
                   MAC_schedAckTrx(&MAC_cntxt.rcvdPktInfo, framePending);
               }
           }
           break;

      default:
           {
#ifdef MAC_STATS_ENA
               MAC_cntxt.unknownFrameRcvdCnt ++;
#endif
               rc = 0;
           }
           break;
   }

#ifdef FLASH_LED_ON_PKT_RX_TX
   PLTFRM_OFF_LED_1();
#endif

   return rc;
}


#ifdef DEV_TYPE_RFD
/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
void MLME_pollRequest(void)
{
   MAC_sts_t sts = MAC_STS_SUCCESS;
   MAC_pduInfo_s pduInfo;

   /*
    * The MLME-POLL.request primitive is generated by the next higher layer
    * and issued to its MLME when data are to be requested from a coordinator.
    * 7.1.16.1.3 Effect on receipt
    * On receipt of the MLME-POLL.request primitive, the MLME generates and
    * sends a data request command (see 7.3.4).
    */

   if (MAC_cntxt.pollReqTxAttempt || MAC_cntxt.pollReqSent || MAC_cntxt.pollTimerStarted)
   {
       SYS_fatal(SYS_FATAL_ERR_1);
   }

   pduInfo.frameType = MAC_FRAME_TYPE_CMD;
   pduInfo.sam = MAC_ADDRESS_MODE_SHORT_ADDR;
   pduInfo.dam = MAC_ADDRESS_MODE_SHORT_ADDR;
   pduInfo.framePending = 0x0;
   pduInfo.lpwmnIdComp = MAC_DATA_PKT_LPWMN_ID_COMP_FLAG;
   pduInfo.ackRequired = 0x1;

   /*
    * The Sequence Number field shall contain the current value of macDSN.
    * macDSN is the sequence number added to the transmitted data or MAC
    * command frame.
    */
   pduInfo.seqNr = MAC_getNextDSN();

#ifdef MAC_SEC_ENA
#error TODO !!
#else
   pduInfo.secEna = 0x0;
#endif

   pduInfo.bdHndl = SYS_allocMem(MAC_DATA_REQ_CMD_PDU_PYLD_LEN);
   if (SYS_BD_HNDL_IS_VALID(pduInfo.bdHndl) == 0x0)
   {
       sts = MAC_STS_OOM;
   }
   else
   {
      UINT8_t *buff_p = SYS_GET_BD_BUFF(pduInfo.bdHndl);

      *(buff_p) = MAC_CMD_DATA_REQUEST;

      sts = MAC_buildPDU(&pduInfo, NULL, MAC_PIB.macCoordShortAddress, 0);
      if (sts == MAC_STS_SUCCESS)
      {
          MAC_cntxt.ackRequired = pduInfo.ackRequired;
          MAC_cntxt.txAttempCnt = 1;
          MAC_cntxt.macPduLen = SYS_GET_BD_LEN(pduInfo.bdHndl);
          MAC_cntxt.expAckDSN =  pduInfo.seqNr;
          MAC_cntxt.expAckSrcShortAddr = MAC_PIB.macCoordShortAddress;
          MAC_cntxt.cnfrmReqd = 0x1;

          if (SYS_BD_HNDL_IS_VALID(MAC_cntxt.currTxPduHndl))
          {
              SYS_fatal(SYS_FATAL_ERR_1);
          }

          MAC_cntxt.currTxPduHndl = pduInfo.bdHndl;

#ifdef MAC_STATS_ENA
          MAC_cntxt.dataReqCmdTxCnt ++;
#endif

          MAC_cntxt.pollReqTxAttempt = 1;

          MAC_csmaInit();
          MAC_doCsmaProc();
      }
   }

   if (sts != MAC_STS_SUCCESS)
   {
       MAC_cntxt.mlmePollCnfrmParams.status = sts;
       SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_MLME_POLL_CONFIRM;
   }
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
void MCPS_dataReq(const MCPS_dataReqParams_s * const params_p)
{
   MAC_sts_t sts = MAC_STS_SUCCESS;
   MAC_pduInfo_s pduInfo;

   if (params_p->srcAddrMode == MAC_ADDRESS_MODE_NO_ADDR
       && params_p->destAddrMode == MAC_ADDRESS_MODE_NO_ADDR)
   {
       /*
        * If both the SrcAddrMode and the DstAddrMode parameters
        * are set to NO_ADDRESS in the MCPS-DATA.request primitive,
        * the status shall be set to INVALID_ADDRESS.
        */
       sts = MAC_STS_INVALID_ADDRESS;
       SYS_freeMem(params_p->msduBDHndl);
       goto _end;
   }

   // TODO - verify all the params are valid

   pduInfo.frameType = MAC_FRAME_TYPE_DATA;
   pduInfo.sam = params_p->srcAddrMode;
   pduInfo.dam = params_p->destAddrMode;

#ifdef MAC_SEC_ENA
   pduInfo.secEna = params_p->securityLevel != MAC_SECURITY_LEVEL_NONE ? 0x1 : 0x0;
#else
   pduInfo.secEna = 0x0;
#endif
   pduInfo.framePending = 0x0;
   pduInfo.lpwmnIdComp = MAC_DATA_PKT_LPWMN_ID_COMP_FLAG;
   pduInfo.ackRequired = params_p->ackTx ? 0x1 : 0x0;

   pduInfo.bdHndl = params_p->msduBDHndl;

   /*
    * The Sequence Number field shall contain the current value of macDSN.
    * macDSN is the sequence number added to the transmitted data or MAC
    * command frame.
    */
   pduInfo.seqNr = MAC_getNextDSN();

   sts = MAC_buildPDU(&pduInfo, NULL, params_p->destShortAddr, params_p->indirectTx);
   if (sts != MAC_STS_SUCCESS)
   {
       // The MSDU has been freed at this point
       goto _end;
   }

   if (params_p->indirectTx == 0x0)
   {
       /*
        * If the TxOptions parameter specifies that an indirect
        * transmission is not required, the MAC sub-layer will
        * transmit the MSDU using CSMA-CA either in the CAP for
        * a beacon-enabled LPWMN or immediately for a non beacon-
        * enabled LPWMN, or at the next time slot to the destination
        * address if in TSCH mode.
        */

       MAC_cntxt.ackRequired = pduInfo.ackRequired;
       MAC_cntxt.txAttempCnt = 1;
       MAC_cntxt.macPduLen = SYS_GET_BD_LEN(pduInfo.bdHndl);
       MAC_cntxt.expAckDSN =  pduInfo.seqNr;
       MAC_cntxt.expAckSrcShortAddr = params_p->destShortAddr;
       MAC_cntxt.cnfrmReqd = 0x1;

       if (SYS_BD_HNDL_IS_VALID(MAC_cntxt.currTxPduHndl))
       {
           SYS_fatal(SYS_FATAL_ERR_1);
       }

       MAC_cntxt.currTxPduHndl = pduInfo.bdHndl;


#ifdef MAC_STATS_ENA
       MAC_cntxt.dataReqCnt ++;
#endif

       MAC_csmaInit();
       MAC_doCsmaProc();
   }
   else
   {
#ifdef DEV_TYPE_RFD
       sts = MAC_STS_INVALID_PARAMETER;
#else
       if (params_p->destAddrMode == MAC_ADDRESS_MODE_SHORT_ADDR)
       {
           /*
            * If the TxOptions parameter specifies that an indirect transmission is
            * required and this primitive is rxd by the MAC sub layer of a coordinator,
            * the data frame is sent using indirect transmission, as described in 7.5.6.
            */
           if (!(MAC_addFrameToPendQ(params_p->destShortAddr, pduInfo.bdHndl)))
               sts = MAC_STS_OOM;
       }
       else
       {
           sts = MAC_STS_INVALID_PARAMETER;
       }

       if (sts != MAC_STS_SUCCESS)
           SYS_freeMem(pduInfo.bdHndl);
       else
           sts = MAC_STS_TX_PENDING;
#endif
   }

_end:
   if (sts != MAC_STS_SUCCESS)
   {
       MAC_cntxt.datacnfrmParams.status = sts;
       MAC_cntxt.datacnfrmParams.destShortAddr = params_p->destShortAddr;

       SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_DATA_CONFIRM;
   }

   return;
}


/*
 *******************************************************************
 *
 *
 *
 *
 *******************************************************************
 */
MAC_sts_t MAC_buildSendAssocnReqCmd(MLME_assocRequestParams_s *params_p)
{
   MAC_pduInfo_s pduInfo;
   UINT8_t *hdr_p;
   UINT8_t offset = 0x0, macHdrLen;

   /*
    * 5.3.1 Association request command
    * The association request command allows a device to request association with
    * a LPWMN through the LPWMN coordinator or a coordinator.
    * This command shall only be sent by an unassociated device that wishes to
    * associate with a LPWMN. A device shall only associate with a LPWMN through the
    * LPWMN coordinator or a coordinator allowing association, as determined through
    * the scan procedure.
    * All devices shall be capable of transmitting this command, although an RFD
    * is not required to be capable of receiving it.
    *
    * The association request command shall be formatted as illustrated in Fig
    * 49.
    *
    * -----------------------------------------------------------
    * Octets:       Variable           1                1
    * -----------------------------------------------------------
    *                 MHR        Cmd Frame Id     Capability Info
    * -----------------------------------------------------------
    */

   pduInfo.frameVersion = MAC_FRAME_VER_802_15_4;

   pduInfo.frameType = MAC_FRAME_TYPE_CMD;

   /*
    * The Source Addressing Mode field shall be set to indicate extended addressing.
    */
   pduInfo.sam = MAC_ADDRESS_MODE_EXTENDED_ADDR;

   /*
    * The Destination Addressing Mode field shall be set to the same mode as
    * indicated in the beacon frame to which the association request command
    * refers.
    */
   pduInfo.dam = params_p->coordAddrMode;

   pduInfo.secEna = 0x0;

   /*
    * The Frame Pending field shall be set to zero and ignored upon reception,
    * and the AR field shall be set to 1.
    */
   pduInfo.framePending = 0x0;

   // Issue #26 (rkris, Nov/16/2013)
   // It is possible that this device does not get ack for association request but
   // gets association response from coordinator. In this case, the MAC layer will
   // try to retransmit the association request event though this node has already
   // associated. To avoid such complications, do not expect ack for the association
   // request message. Instead time out waiting for association response.
   pduInfo.ackRequired = 0x0;

   pduInfo.lpwmnIdComp = 0x0;

   macHdrLen = MAC_calcHdrLen(&pduInfo);
   pduInfo.bdHndl = SYS_allocMem(macHdrLen + MAC_ASSOC_REQ_CMD_PDU_PYLD_LEN);
   if (SYS_BD_HNDL_IS_VALID(pduInfo.bdHndl) == 0x0)
       return MAC_STS_OOM;

   hdr_p = SYS_GET_BD_BUFF(pduInfo.bdHndl);

   offset = MAC_buildFrameCntrlHdr(hdr_p, &pduInfo);

   /*
    * The Sequence Number field shall contain the current value of macDSN.
    * macDSN is the sequence number added to the transmitted data or MAC
    * command frame.
    */
   pduInfo.seqNr = MAC_getNextDSN();
#if (MAC_PDU_HDR_SEQ_NR_FIELD_LEN > 1)
   UTIL_htole32(hdr_p + offset, pduInfo.seqNr);
#else
   hdr_p[offset] = pduInfo.seqNr;
#endif
   offset += MAC_PDU_HDR_SEQ_NR_FIELD_LEN;
   /*
    * The Destination LPWMN Identifier field shall contain the identifier of the
    * LPWMN to which to associate.
    */
   UTIL_htole16(hdr_p + offset, params_p->coordLPWMNId);
   offset += MAC_LPWMN_ID_LEN;

   /*
    * The Destination Address field shall contain the address from the beacon
    * frame that was transmitted by the coordinator to which the association
    * request command is being sent.
    */
   if (params_p->coordAddrMode == MAC_ADDRESS_MODE_SHORT_ADDR)
   {
       MAC_PIB.macCoordShortAddress = params_p->coordAddress.shortAddr;
       UTIL_htole16(hdr_p + offset, params_p->coordAddress.shortAddr);
       offset += MAC_SHORT_ADDR_LEN;
   }
   else
   {
       memcpy(hdr_p + offset,
              params_p->coordAddress.extAddr,
              MAC_EXT_ADDR_LEN);
       offset += MAC_EXT_ADDR_LEN;
   }

   /*
    * The Source LPWMN Identifier field shall contain the broadcast LPWMN id.
    */
   UTIL_htole16(hdr_p + offset, MAC_BROADCAST_LPWMN_ID_LSB);
   offset += MAC_LPWMN_ID_LEN;

   /*
    * The Source Address field shall contain the value of macExtendedAddress.
    */
   {
       memcpy(hdr_p + offset,
              MAC_PIB.macExtendedAddress,
              MAC_EXT_ADDR_LEN);
       offset += MAC_EXT_ADDR_LEN;
   }

   hdr_p[offset] = MAC_CMD_ASSOCIATION_REQ;
   hdr_p[offset + 1] = params_p->CapabilityInformation;;
   offset += MAC_ASSOC_REQ_CMD_PDU_PYLD_LEN;

   MAC_cntxt.ackRequired = pduInfo.ackRequired;
   MAC_cntxt.txAttempCnt = 1;
   MAC_cntxt.macPduLen = macHdrLen + MAC_ASSOC_REQ_CMD_PDU_PYLD_LEN;
   MAC_cntxt.cnfrmReqd = 0x1;

   if (SYS_BD_HNDL_IS_VALID(MAC_cntxt.currTxPduHndl))
       SYS_fatal(SYS_FATAL_ERR_1);

   MAC_cntxt.currTxPduHndl = pduInfo.bdHndl;

   MAC_csmaInit();
   MAC_doCsmaProc();

   return MAC_STS_SUCCESS;
}


/*
 * 8.4 Packet Handling in Receive Mode
 * ----------------------------------------------------------------------------
 * > In receive mode, the radio will search for a valid sync word (if
 *   SYNC_CFG1.SYNC_MODE != 0) and when found, the demodulator has obtained
 *   both bit and byte synchronization and will receive the first payload
 *   byte. For more details on byte synchronization/sync word detection,
 *   please see Section 6.7
 * > If whitening is enabled (PKT_CFG1.WHITE_DATA = 1), the data will be
 *   de-whitened at this stage.
 * > When variable packet length mode is enabled (PKT_CFG0.LENGTH_CONFIG = 1),
 *   the first byte is the length byte. The packet handler stores this value as
 *   the packet length and receives the number of bytes indicated by the length
 *   byte.
 * > Next, the packet handler optionally checks the address (if PKT_CFG1.
 *   ADDR_CHECK_CFG != 0) and only continues the reception if the address matches.
 * > If automatic CRC check is enabled (PKT_CFG1.CRC_CFG != 0), the packet handler
 *   computes CRC and matches it with the appended CRC checksum.
 * > At the end of the payload, the packet handler will optionally write two
 *   extra packet status bytes (see Table 23 and Table 24) that contain CRC status,
 *   LQI, and RSSI value if PKT_CFG1.APPEND_STATUS = 1.
 *
 *
 * CRC_AUTOFLUSH bit in FIFO_CFG register
 * --------------------------------------
 * Automatically flushes the last packet received in the RX FIFO if a CRC error
 * occurred. If this bit has been turned off and should be turned on again, an SFRX
 * strobe must first be issued
 *
 *
 * TERM_ON_BAD_PACKET_EN bit in RFEND_CFG0 register
 * ------------------------------------------------
 * 0x0 - Terminate on bad packet disabled. When a bad packet is received (address,
 *       length, or CRC error) the radio stays in RX regardless of the RFEND_CFG1.
 *       RXOFF_MODE
 *
 * 0x1 - Terminate on bad packet enabled. RFEND_CFG1.RXOFF_MODE is ignored and the
 *       radio enters IDLE mode (or SLEEP mode if eWOR is used) when a bad packet
 *       has been received
 * ----------------------------------------------------------------------------
 */


UINT16_t __dbgCnt1 = 0;

#define CC1200_RSSI_MON

#ifdef CC1200_RSSI_MON
#define CC1200_RX_RSSI_LOG_ARRAY_SZ  256
UINT16_t CC1200_rxRSSILogIdx = 0;
SINT8_t CC1200_rxRSSILogArray[CC1200_RX_RSSI_LOG_ARRAY_SZ];
SINT8_t CC1200_maxRxRSSI = -128, CC1200_minRxRSSI = 127;
UINT16_t CC1200_rssiSampleCnt = 0;

UINT8_t CC1200_freqOffEst1 = 0;
UINT8_t CC1200_freqOffEst0 = 0;
UINT8_t CC1200_freqOff1 = 0;
UINT8_t CC1200_freqOff0 = 0;
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
void MAC_processRxFifo(void)
{
   UINT8_t rxFifoByteCnt, pktProcCnt = 0;
   PLTFRM_sts_t sts;

   // FIFO should have at least one good packet.
   // CC1200 is configured to go back to RX mode on receiving a good/bad packet.

   // Two status bytes (see Table 23 and Table 24) with RSSI value, LQI, and
   // CRC status can be appended in the RX FIFO by setting PKT_CFG1.APPEND_STATUS
   // = 1.

   // Format of each frame stored in the RX FIFO is -
   //
   // > Length Field (1 byte) (value N)
   // > Payload (N bytes)
   // > RSSI (1 byte)
   // > CRC-OK (1 bit) | CORRELATION (7 bits)


   do
   {
      sts = CC1200_getRXFIFOByteCnt(&rxFifoByteCnt);
      if (sts != PLTFRM_STS_SUCCESS)
          SYS_fatal(SYS_FATAL_ERR_1);

      if (rxFifoByteCnt > (CC1200_FRAME_LENGTH_FIELD_LEN
                           + CC1200_APPENDED_RSSI_INFO_LEN
                           + CC1200_APPENDED_CRC_LQI_INFO_LEN))
      {
          UINT8_t pyldLen;

#ifdef CC1200_RSSI_MON
          sts = CC1200_readExtAddrSpaceReg(CC1200_FREQOFF_EST0_EXT_REG_ADDR, &CC1200_freqOffEst0);
          if (sts != PLTFRM_STS_SUCCESS)
              SYS_fatal(SYS_FATAL_ERR_1);
          sts = CC1200_readExtAddrSpaceReg(CC1200_FREQOFF_EST1_EXT_REG_ADDR, &CC1200_freqOffEst1);
          if (sts != PLTFRM_STS_SUCCESS)
              SYS_fatal(SYS_FATAL_ERR_1);
#endif

          sts = CC1200_readRxFIFO(CC1200_FRAME_LENGTH_FIELD_LEN, &pyldLen);
          if (sts == PLTFRM_STS_SUCCESS)
          {
              rxFifoByteCnt -= CC1200_FRAME_LENGTH_FIELD_LEN;

              if (rxFifoByteCnt >= (pyldLen
                                    + CC1200_APPENDED_RSSI_INFO_LEN
                                    + CC1200_APPENDED_CRC_LQI_INFO_LEN))
              {
                  UINT8_t bdHndl;

                  rxFifoByteCnt -= (pyldLen
                                    + CC1200_APPENDED_RSSI_INFO_LEN
                                    + CC1200_APPENDED_CRC_LQI_INFO_LEN);

                  pktProcCnt ++;

                  /*
                   * The packet length is defined as the payload data and the optional address
                   * byte, excluding the optional length byte, the optional CRC, and the optional
                   * append status.
                   */

                  bdHndl = SYS_allocMem(SYS_TRSCN_Q_HNDL_FIELD_LEN
                                        + SYS_TRSCN_TYPE_FIELD_LEN
                                        + pyldLen);
                  if (SYS_BD_HNDL_IS_VALID(bdHndl))
                  {
                      const UINT16_t off = SYS_TRSCN_Q_HNDL_FIELD_LEN + SYS_TRSCN_TYPE_FIELD_LEN;
                      sts = CC1200_readRxFIFO(pyldLen, SYS_GET_BD_BUFF(bdHndl) + off);
                      if (sts == PLTFRM_STS_SUCCESS)
                      {
                          UINT8_t buff[2];
                          SINT16_t rssi, lqiVal;

                          /*
                           * If the PKT_CFG1.APPEND_STATUS bit is set, two status bytes containing
                           * information on CRC, LQI and RSSI will be written to the RXFIFO.
                           * RSSI byte - 8 bits
                           * CRC_OK_LQI byte - 1 bit CRC OK : 7 bits LQI
                           */
                          sts = CC1200_readRxFIFO(CC1200_APPENDED_RSSI_INFO_LEN
                                                  + CC1200_APPENDED_CRC_LQI_INFO_LEN,
                                                  (UINT8_t *)buff);
                          if (sts == PLTFRM_STS_SUCCESS)
                          {
                              SINT8_t tmpS8 = buff[0];
                              rssi = tmpS8;
                              rssi -= (CC1200_RSSI_OFFSET_VALUE);

                              lqiVal = buff[1];
                              lqiVal &= ~0x80;   // MSBit is CRC ok/not-ok bit

#ifdef CC1200_RSSI_MON
                              if (rssi > CC1200_maxRxRSSI)
                                  CC1200_maxRxRSSI = rssi;
                              else
                              {
                                  if (rssi < CC1200_minRxRSSI)
                                      CC1200_minRxRSSI = rssi;
                              }
                              CC1200_rxRSSILogArray[CC1200_rxRSSILogIdx] = rssi;
                              CC1200_rxRSSILogIdx = (CC1200_rxRSSILogIdx + 1) % CC1200_RX_RSSI_LOG_ARRAY_SZ;
                              CC1200_rssiSampleCnt ++;
#endif

                              if (MAC_processRcvdPdu(bdHndl, rssi, lqiVal) == 0x0)
                              {
                                  SYS_freeMem(bdHndl);
                              }
                          }
                      }
                  }
                  else
                  {
                      UINT8_t dummyByte, idx;

                      // Out of buffers !! Flush this packet.

                      pyldLen += (CC1200_APPENDED_RSSI_INFO_LEN
                                  + CC1200_APPENDED_CRC_LQI_INFO_LEN);

                      for (idx=0; idx < pyldLen; idx++)
                      {
                          sts = CC1200_readRxFIFO(1, &dummyByte);
                          if (sts != PLTFRM_STS_SUCCESS)
                              SYS_fatal(SYS_FATAL_ERR_1);
                      }
                  }
              }
              else
              {
                  if (pktProcCnt < 1)
                      sts = PLTFRM_STS_RADIO_RX_FIFO_ERR;
                  break;
              }
          }
          else
          {
              SYS_fatal(SYS_FATAL_ERR_1);
          }
      }
      else
      {
           if (pktProcCnt < 1)
           {
               sts = PLTFRM_STS_RADIO_RX_FIFO_ERR;
#ifdef MAC_STATS_ENA
               MAC_cntxt.MAC_emptyFifoCnt ++;
#endif
           }
           break;
      }
   } while (1);


#if 0
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_1);
#endif

   // <Note>
   // The "PKT_CRC_OK" signal gets de-asserted when RX mode is entered and
   // PKT_CFG1.CRC_CFG != 0.

   return;
}

#ifndef DEV_TYPE_RFD
/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
void MAC_buildSendBeaconPdu(void)
{
   MAC_pduInfo_s pduInfo;
   UINT8_t byte;
   UINT8_t *buff_p;

   pduInfo.bdHndl = SYS_allocMem(MAC_BCN_PYLD_LEN);
   if (!(SYS_BD_HNDL_IS_VALID(pduInfo.bdHndl)))
       return;

   buff_p = SYS_GET_BD_BUFF(pduInfo.bdHndl);

   /*
    * A device shall be permitted to transmit beacon frames only if macShortAddress
    * is not equal to 0xffff.
    */

   /*
    * 5.2.2.1.1 Beacon frame MHR fields
    * The Frame Type field shall contain the value that indicates a beacon frame,
    * as shown in Table 2, and the Source Addressing Mode field shall be set to
    * indicate the beacon source addressing mode, as defined in 5.1.2.4. The
    * Security Enabled field shall be set to one if security is enabled and the
    * Frame Version field is not zero. If a broadcast data or command frame is
    * pending, the Frame Pending field shall be set to one. All other fields in
    * the Frame Control field shall be set to zero and ignored on reception.
    */

   pduInfo.frameType = MAC_FRAME_TYPE_BEACON;

   /*
    * The addressing fields shall comprise only the source address fields. The
    * Source LPWMN Identifier and Source Address fields shall contain the LPWMN id
    * and address, respectively, of the device transmitting the beacon.
    */

   /*
    * Source LPWMN Identifier field of the beacon frame will convey the value of
    * macLPWMNId.
    * The address used in the Source Address field of the beacon frame shall
    * contain the value of macExtendedAddress if macShortAddress is equal to 0xfffe
    * or macShortAddress otherwise.
    */

   if (MAC_PIB.macShortAddress == 0xfffe)
       pduInfo.sam = MAC_ADDRESS_MODE_EXTENDED_ADDR;
   else
       pduInfo.sam = MAC_ADDRESS_MODE_SHORT_ADDR;

   pduInfo.dam = MAC_ADDRESS_MODE_NO_ADDR;

   /*
    * The Security Enabled field shall be set to one if security is enabled.
    */
   pduInfo.secEna = 0x0;

   /*
    * If a broadcast data or command frame is pending, the Frame Pending field
    * shall be set to one.
    */
   pduInfo.framePending = 0x0;

   /*
    * All other fields in the Frame Control field shall be set to zero and
    * ignored on reception.
    */
   pduInfo.lpwmnIdComp = 0x0;
   pduInfo.ackRequired = 0x0;

   /*
    * The Auxiliary Security Header field, if present, shall contain the info
    * required for security processing of the beacon frame, as specified in 5.2.1.7.
    */

   /*
    * The Sequence Number field shall contain the current value of macBSN.
    */
   pduInfo.seqNr = MAC_PIB.macBSN;
   MAC_PIB.macBSN = (MAC_PIB.macBSN + 1) & 0xff;

   /*
    * If macBeaconOrder = 15, the coordinator shall not transmit beacon frames
    * except when requested to do so, such as on receipt of a beacon request cmd.
    * The value of macSuperframeOrder shall be ignored if macBeaconOrder =15.
    */
   byte = MAC_PIB.macSuperframeOrder;
   byte = (byte << MAC_SF_SPEC_SF_ORDER_BIT_SHIFT) & MAC_SF_SPEC_SF_ORDER_BIT_SHIFT_MSK;
   byte |= MAC_PIB.macBeaconOrder;

   buff_p[0] = byte;

   /*
    * Super-frame Specification field
    */
   byte = 0x0;
   /*
    * The Association Permit field shall be set to one if macAssociationPermit
    * is set to TRUE (i.e., the coordinator is accepting association to the LPWMN).
    * The association permit bit shall be set to zero if the coordinator is
    * currently not accepting association requests on its network.
    */
   if (MAC_PIB.macAssociationPermit)
       byte |= MAC_SF_SPEC_ASSOC_PERMIT_BIT_SHIFT_MSK;

#ifdef LPWMN_COORD
   /*
    * The LPWMN Coordinator field shall be set to one if the beacon frame is being
    * transmitted by the LPWMN coordinator. Otherwise, the LPWMN Coordinator field
    * shall be set to zero.
    */
   byte |= MAC_SF_SPEC_IS_COORD_BIT_SHIFT_MSK;
#endif

   buff_p[1] = byte;

   // GTS Specification field
   buff_p[2] = 0x0;

   // Pending address field
   buff_p[3] = 0x0;

   if (MAC_buildPDU(&pduInfo, NULL, MAC_BROADCAST_SHORT_ADDR, 0) == MAC_STS_SUCCESS)
   {
       MAC_cntxt.txAttempCnt = 1;
       MAC_cntxt.cnfrmReqd = 0x0;
       MAC_cntxt.ackRequired = 0x0;
       MAC_cntxt.macPduLen = SYS_GET_BD_LEN(pduInfo.bdHndl);

       if (SYS_BD_HNDL_IS_VALID(MAC_cntxt.currTxPduHndl))
       {
           SYS_fatal(SYS_FATAL_ERR_1);
       }

       MAC_cntxt.currTxPduHndl = pduInfo.bdHndl;

       MAC_csmaInit();
       MAC_doCsmaProc();
   }

   return;
}
#endif


#ifndef LPWMN_COORD
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void MAC_cleanUpScanProc(void)
{
   MAC_cntxt.scanInProgress = 0x0;

#ifdef MAC_STATS_ENA
   MAC_cntxt.scanCleanUpCnt ++;
#endif

   // Turn the radio off (force it to IDLE mode)
   RADIO_hardStop();

   return;
}


/*
 *******************************************************************
 *
 *
 *
 *
 *******************************************************************
 */
MAC_sts_t MAC_sendBcnRequest(void)
{
   MAC_sts_t sts;
   MAC_pduInfo_s pduInfo;
   MAC_addr_s destAddr;

   /*
    * 5.3.7 Beacon request command
    * The beacon request command is used by a device to locate all
    * coordinators within its radio communications range during an
    * active scan.
    *
    * This command is optional for an RFD.
    *
    * The beacon request command shall be formatted as illustrated
    * in Figure 56.
    *
    * -----------------------------------------------------------
    * Octets       7                              1
    * -----------------------------------------------------------
    *             MHR                        Cmd Frame Id
    * -----------------------------------------------------------
    *
    * The Destination Addressing Mode field shall be set to indicate
    * short addressing, and the Source Addressing Mode field shall be
    * set to indicate that the source addressing information is not present.
    * The Frame Pending field shall be set to zero and ignored upon reception.
    * The AR field and Security Enabled field shall also be set to zero.
    * The Destination LPWMN Identifier field shall contain the broadcast LPWMN id.
    * The Destination Address field shall contain the broadcast short address.
    */

   pduInfo.frameType = MAC_FRAME_TYPE_CMD;
   pduInfo.sam = MAC_ADDRESS_MODE_NO_ADDR;
   pduInfo.dam = MAC_ADDRESS_MODE_SHORT_ADDR;
   pduInfo.secEna = 0x0;
   pduInfo.framePending = 0x0;
   pduInfo.lpwmnIdComp = 0x0;
   pduInfo.ackRequired = 0x0;

   /*
    * The Sequence Number field shall contain the current value of macDSN.
    * macDSN is the sequence number added to the transmitted data or MAC
    * command frame.
    */
   pduInfo.seqNr = MAC_getNextDSN();

   destAddr.shortAddr = MAC_BROADCAST_SHORT_ADDR;

   pduInfo.bdHndl = SYS_allocMem(MAC_BEACON_REQ_CMD_PDU_PYLD_LEN);
   if (SYS_BD_HNDL_IS_VALID(pduInfo.bdHndl) == 0x0)
   {
       return MAC_STS_OOM;
   }
   else
   {
      UINT8_t *buff_p = SYS_GET_BD_BUFF(pduInfo.bdHndl);

      *(buff_p) = MAC_CMD_BEACON_REQUEST;

      sts = MAC_buildPDU(&pduInfo,
                         &destAddr,
                         MAC_BROADCAST_SHORT_ADDR, 0);
      if (sts != MAC_STS_SUCCESS)
      {
          goto _end;
      }

      MAC_cntxt.ackRequired = 0x0;
      MAC_cntxt.cnfrmReqd = 0x0;
      MAC_cntxt.txAttempCnt = 1;
      MAC_cntxt.macPduLen = SYS_GET_BD_LEN(pduInfo.bdHndl);

      if (SYS_BD_HNDL_IS_VALID(MAC_cntxt.currTxPduHndl))
      {
          SYS_fatal(SYS_FATAL_ERR_1);
      }

      MAC_cntxt.currTxPduHndl = pduInfo.bdHndl;

#ifdef MAC_STATS_ENA
      MAC_cntxt.bcnReqCmdTxCnt ++;
#endif

      MAC_csmaInit();
      MAC_doCsmaProc();
   }

_end:
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
void MAC_doActiveScan(void)
{
   MAC_sts_t sts = MAC_STS_SUCCESS;
   UINT8_t channBit = 0x0, sendCnfrm = 0x0;

   PLTFRM_LOG("<%s> cbm:0x%x/ubc:%d/lsc:0x%x\r\n",
              __FUNCTION__, MAC_cntxt.scanChannelsMsk,
              MAC_cntxt.uniqueBcnRcvdCnt, MAC_cntxt.scanChan);

   if (MAC_cntxt.scanChan != 0x1f)
       channBit = (MAC_cntxt.scanChan - PHY_865_867_MHZ_BAND_CHAN_1) + 1;
   /*
    * An active scan over a specified set of channels is requested using
    * the MLME-SCAN.request primitive with the ScanType parameter set to
    * indicate an active scan.
    */

    // Get the first requested channel
    while ((channBit <= PHY_865_867_MHZ_BAND_CHAN_9)
           && (MAC_cntxt.scanChannelsMsk & (1 << channBit)) == 0x0)
    {
        channBit ++;
    }

    if (channBit > PHY_865_867_MHZ_BAND_CHAN_9)
    {
        // No more channels left to scan

        /*
         * If the MLME was able to send a beacon request command on at least one
         * of the channels but no beacons were found, the MLME-SCAN.confirm
         * primitive will contain a null set of LPWMN descriptor values, regardless
         * of the value of macAutoRequest, and a status of NO_BEACON.
         */
        if (MAC_cntxt.uniqueBcnRcvdCnt > 0)
        {
            sts = MAC_STS_SUCCESS;
            MAC_cntxt.scanCnfrmParams.lpwmnDescriptorList_p = MAC_cntxt.lpwmnDescriptorList;
            MAC_cntxt.scanCnfrmParams.resultListSize = MAC_cntxt.uniqueBcnRcvdCnt;
            MAC_cntxt.scanCnfrmParams.unScannedChanBitMsk = MAC_cntxt.unScannedChanBitMsk;
        }
        else
        {
            sts = MAC_STS_NO_BEACON;
        }

        sendCnfrm = 1;
    }
    else
    {
        CC1200_state_t cc1200State;

        MAC_cntxt.scanChan = channBit + PHY_865_MHZ_BAND_CHAN_1;

        /*
         * For each channel, the device shall first  switch to the channel, by
         * setting phyCurrentChannel and phyCurrentPage accordingly.
         */
        if (RADIO_setChan(MAC_cntxt.scanChan) == PLTFRM_STS_SUCCESS)
        {
            PHY_PIB.phyCurrentChannel = MAC_cntxt.scanChan;
        }
        else
        {
            sts = MAC_STS_RADIO_HW_ERR;
            sendCnfrm = 1;
            goto _end;
        }

        CC1200_getCurrState(&cc1200State);
        if (cc1200State != CC1200_STATE_IDLE)
        {
            if (RADIO_hardStop( ) != PLTFRM_STS_SUCCESS)
                SYS_fatal(SYS_FATAL_ERR_1);
        }

        // Flush the RX FIFO in case we have a beacon lying around in the RX FIFO
        // Flush the RX FIFO. Only issue SFRX in IDLE or RX_FIFO_ERR states.
        RADIO_flushRx();

        /*
         * During an active or passive scan, the MAC sub-layer shall discard
         * all frames received over the PHY data service that are not beacon
         * frames.
         */

        MAC_cntxt.scanInProgress = 0x1;

        /*
         * For an active scan, the device shall send a beacon request command,
         * as described in 5.3.7.
         */

        sts = MAC_sendBcnRequest();
        if (sts != MAC_STS_SUCCESS)
            sendCnfrm = 1;
    }

_end:

    if (sendCnfrm)
    {
        MAC_cntxt.scanCnfrmParams.status = sts;
        if (sts != MAC_STS_SUCCESS)
        {
            MAC_cntxt.scanCnfrmParams.resultListSize = 0x0;
            MAC_cntxt.scanCnfrmParams.lpwmnDescriptorList_p = NULL;
        }
        MAC_cleanUpScanProc( );
        SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_SCAN_CONFIRM;
    }

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
void MAC_evtHndlr(const UINT16_t globalEvtId)
{
    switch (globalEvtId)
    {
#ifndef DEV_TYPE_RFD
       case SYS_GLOBAL_EVT_MAC_BCN_REQUEST:
            {
               MAC_buildSendBeaconPdu();
            }
            break;
#endif

       case SYS_GLOBAL_EVT_MAC_RETRY_CCA:
            {
               MAC_doCsmaProc();
            }
            break;

       case SYS_GLOBAL_EVT_MAC_FRAME_RX_DONE:
            {
               MAC_processRxFifo();
            }
            break;

       case SYS_GLOBAL_EVT_MAC_FRAME_TX_DONE:
            {
               if (MAC_cntxt.txModState == MAC_TX_MOD_STATE_WAIT_TX_OVER)
                   MAC_txOverEvtHndlr(1);
               else
               {
#ifdef MAC_STATS_ENA
                   MAC_cntxt.unExpFrameTxDoneEvtCnt ++;
#endif
               }
            }
            break;

#if 0
       case SYS_GLOBAL_EVT_MAC_DATA_REQ_CMD_RCVD:
            {
               if (MAC_cntxt.commStsIndParams.dstAddrMode >= MAC_ADDRESS_TYPE_SHORT)
                   MAC_processDataRequestCmd();
            }
            break;
#endif
    }
    return;
}


#ifndef LPWMN_COORD
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void MAC_handleCsmaFlrDuringScan(void)
{
   /*
    * If, during an active scan, the MLME is unable to transmit a beacon request
    * command on a channel specified by the ScanChannels parameter due to a channel
    * access failure, the channel will appear in the list of unscanned channels
    * returned by the MLME-SCAN.confirm primitive.
    *
    * If the MLME is unable to transmit an orphan notification command on a channel
    * specified by the ScanChannels parameter due to a channel access failure, the
    * channel will appear in the list of unscanned channels returned by the MLME-
    * SCAN.confirm primitive.
    */
   MAC_cntxt.unScannedChanBitMsk |=
                  (1 << (MAC_cntxt.scanChan - PHY_2450_MHZ_BAND_PAGE_0_CHAN_11));

#ifdef MAC_STATS_ENA
   MAC_cntxt.csmaFlrDuringScanCnt ++;
#endif

   if (MAC_cntxt.scanType == MAC_SCAN_TYPE_ACTIVE)
   {
       MAC_doActiveScan();
   }
   else
   {

   }

   return;
}
#endif


#ifndef LPWMN_COORD
/*
 *******************************************************************
 *
 *
 *
 *
 *******************************************************************
 */
void MLME_assocRequest(MLME_assocRequestParams_s *params_p)
{
   MAC_sts_t sts;

   MAC_PIB.macCoordShortAddress = MAC_BROADCAST_SHORT_ADDR;


   /*
    * On receipt of the MLME-ASSOCIATE.request primitive, the MLME of
    * an unassociated device first updates the appropriate PHY and MAC
    * PIB attributes, as described in 5.1.3.1, and then generates an
    * association request command, as defined in 5.3.1.
    */

   /*
    * 5.1.3.1
    * Following the selection of a LPWMN with which to associate, the next
    * higher layers shall request through the MLME-ASSOCIATE.request primitive,
    * as described in 6.2.2.1, that the MLME configures the following PHY and
    * MAC PIB attributes to the values necessary for association:
    *
    * > "phyCurrentChannel" shall be set equal to the ChannelNumber parameter
    *   of the MLME-ASSOCIATE.request primitive.
    */
   if (RADIO_setChan(params_p->channelNumber))
   {
       PHY_PIB.phyCurrentChannel = params_p->channelNumber;

       PLTFRM_LOG("<%s> Radio channel set to <%d>\r\n",
                  __FUNCTION__, params_p->channelNumber);
   }

   /*
    * > "phyCurrentPage" shall be set equal to the ChannelPage parameter of the
    *   MLME-ASSOCIATE.request primitive.
    *   This implementation supports page 0 only.
    */

   /*
    * > "macLPWMNId" shall be set equal to the CoordLPWMNId parameter of the
    *   MLME-ASSOCIATE.request primitive.
    */
   MAC_PIB.macLPWMNId = params_p->coordLPWMNId;

   /*
    * > "macCoordExtendedAddress or macCoordShortAddress, depending on which is
    *   known from the beacon frame from the coordinator through which it wishes
    *   to associate, shall be set equal to the CoordAddress parameter of the
    *   MLME-ASSOCIATE.request primitive.
    */
   if (params_p->coordAddrMode == MAC_ADDRESS_MODE_SHORT_ADDR)
   {
       MAC_PIB.macCoordShortAddress = params_p->coordAddress.shortAddr;
   }
   else
   {
       /*
        * A value of 0xfffe indicates that the coordinator is only using its
        * extended address.
        */
       MAC_PIB.macCoordShortAddress = 0xfffe;
       memcpy(MAC_PIB.macCoordExtendedAddress,
              params_p->coordAddress.extAddr,
              MAC_EXT_ADDR_LEN);
   }

   MAC_cntxt.assocReqInProgress = 1;

   /*
    * The MAC sub-layer of an un-associated device shall initiate the association
    * procedure by sending an association request command, as described in 5.3.1,
    * to the coordinator of an existing LPWMN; if the association request command
    * cannot be sent due to a channel access failure, the MAC sub-layer shall notify
    * the next higher layer.
    */
   sts = MAC_buildSendAssocnReqCmd(params_p);
   if (sts != MAC_STS_SUCCESS)
       MAC_sendAssocCnfrm(sts, MAC_BROADCAST_SHORT_ADDR);
#ifdef DEV_TYPE_RFD
   else
   {
       MAC_cntxt.lastRcvdDSN = MAC_MAX_DSN;
   }
#endif

   return;
}
#endif


#if !defined(LPWMN_COORD) && !defined(DEV_TYPE_RFD)
/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t MAC_addToAssocRFDList(const MAC_shortAddr_t shortAddr)
{
   if (MAC_cntxt.assocRFDCnt < MAC_MAX_ASSOC_RFD_CNT)
   {
       UINT16_t idx;
       for (idx=0; idx<MAC_MAX_ASSOC_RFD_CNT; idx++)
       {
           if (MAC_cntxt.assocRFDList[idx] == MAC_BROADCAST_SHORT_ADDR)
           {
               MAC_cntxt.assocRFDList[idx] = shortAddr;
               MAC_cntxt.assocRFDCnt ++;
               return 1;
           }
       }
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
UINT8_t MAC_lookUpAssocRFDList(const MAC_shortAddr_t shortAddr)
{
   UINT16_t idx;

   if (MAC_cntxt.assocRFDCnt == 0x0)
       return 0;

   for (idx=0; idx<MAC_MAX_ASSOC_RFD_CNT; idx++)
        if (MAC_cntxt.assocRFDList[idx] == shortAddr)
            break;

   return idx < MAC_MAX_ASSOC_RFD_CNT ? 0x1 : 0x0;
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
MAC_sts_t MAC_init(void)
{
   memset(&MAC_cntxt, 0, sizeof(MAC_cntxt_s));
   MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE;
   MAC_PIB_init();

#if !defined(LPWMN_COORD) && !defined(DEV_TYPE_RFD)
    memset(MAC_cntxt.assocRFDList, 0xff,
           MAC_MAX_ASSOC_RFD_CNT*MAC_SHORT_ADDR_LEN);
#endif

   MAC_cntxt.currTxPduHndl = SYS_INV_BD_HDNL;

   MAC_cntxt.expAckSrcShortAddr = MAC_BROADCAST_SHORT_ADDR;

   // Build the static ACK buffer
   MAC_buildFCInAckBuff();
#ifdef LPWMN_COORD
   MAC_updateSrcInAckBuff();
#endif

   return MAC_STS_SUCCESS;
}
#endif



