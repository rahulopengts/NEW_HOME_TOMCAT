/*
 * File Name: s_mac_cc2500.c
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


#if defined(RADIO_CC2500)
#ifndef CC2500_WOR_ENA

#include <string.h>
#include <pltfrm.h>
#include <mac.h>
#include <mac_pib.h>
#include <node_mgr.h>
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
#if defined(DEV_TYPE_SNIFFER) || defined(CUSTOMER_ANDY_SPECIFIC_COORD_BUILD)
#include <gw.h>
#endif
#ifdef SMAC_TIMER_SRC_DBG_ENA
UINT16_t __timerA0Src = 0;
UINT16_t __timerA0Du = 0;
#endif

MAC_cntxt_s MAC_cntxt;

#if defined(DEV_TYPE_SNIFFER) || defined(CUSTOMER_ANDY_SPECIFIC_COORD_BUILD)
UINT16_t MAC_rcvdPktSeqNr = 0;
#endif

#ifdef MAC_TRACE_ENA
UTIL_traceCntxt_s MAC_traceCntxt;
#endif
   

UINT16_t MAC_ackPktTrxTimeMilliSecs = 0;

void MAC_doCsmaProc(void);
void MAC_csmaInit(void);
void MAC_updateSrcInAckBuff(void);
void MAC_handleCsmaFlrDuringScan(void);
void MAC_assocReqAckHndlr(void);
SINT16_t CC2500_calcPktRSSI(UINT8_t rssiRaw);

extern UINT8_t MAC_checkResetRFDKATmo(const MAC_shortAddr_t shortAddr);

SINT8_t MAC_getAssocRFDListIdx(const MAC_shortAddr_t shortAddr);

extern UINT8_t ADP_pktFwdCheck(UINT8_t *adpPdu_p,
                               UINT8_t *dstShort_p,  // Points to dest short in the MAC header
                               UINT8_t *srcShort_p,
                               UINT8_t adpPduLen,
                               UINT8_t bdHndl);

extern void ADP_macAssocIndHndlr(UINT8_t *srcExtAddr_p, UINT8_t capInfo, UINT16_t lastSysFEId);

extern UINT8_t NM_lookUpLocalRFD(UINT16_t shortAddr);
extern SYS_trscnQ_s *NM_checkPendingFrame(MAC_shortAddr_t shortAddr);


#ifndef DEV_TYPE_SNIFFER

#ifdef DEV_TYPE_RFD
#ifdef RFD_COORD_POLL_ENA
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
   MAC_cntxt.mlmePollCnfrmParams.status = sts;
   SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_MLME_POLL_REQ_CONFIRM;
}
// RFD_COORD_POLL_ENA
#endif
// DEV_TYPE_RFD
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
MAC_dsn_t MAC_getNextDSN(void)
{
   MAC_dsn_t nextDSN = MAC_PIB.macDSN;

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

   // 0 is not a valid DSN

   if (MAC_PIB.macDSN == 0x0)
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
   MAC_cntxt.stats.scanTmoCnt ++;
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
       MAC_cntxt.txModState = MAC_TX_MOD_STATE_CONFIRM;

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
void MAC_freeCurrTxPkt(UINT16_t sysFatalId)
{
   if (!(SYS_BD_HNDL_IS_VALID(MAC_cntxt.currTxPduHndl)))
       SYS_fatal(sysFatalId);

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
   // no radio activity. CC2500 current consumption is 1.7 mA (Only voltage
   // regulator to digital part and crystal oscillator running). 

   if (sts != MAC_STS_SUCCESS)
   {
       if (CC2500_forceIdle( ) != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_16);

       // Reset related PIB attributes
       MAC_PIB.macLPWMNId = MAC_BROADCAST_LPWMN_ID;
       MAC_PIB.macCoordShortAddress = MAC_BROADCAST_SHORT_ADDR;
   }

   MAC_cntxt.assocReqInProgress = 0;

   MAC_cntxt.assocCnfrmParams.status = sts;
   MAC_cntxt.assocCnfrmParams.assocShortAddr = allocShortAddr;

   SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_MAC_ASSOC_CONFIRM;

   MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE;  // rkris@wisense.in - march/2/15

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
 *****************************************************f***************
 */
UINT8_t MAC_checkReassocCondition(void)
{
	MAC_cntxt.txFlrCnt ++;

#ifdef MAC_STATS_ENA
   MAC_cntxt.stats.reassocCheckCnt ++;
#endif

   if (MAC_cntxt.txFlrCnt > MAC_RFD_REASSOC_THRESHOLD)
   {
       MAC_cntxt.txFlrCnt = 0x0;

#ifdef MAC_STATS_ENA
       MAC_cntxt.stats.reassocReqCnt ++;
#endif

       MAC_PIB.macShortAddress = MAC_BROADCAST_SHORT_ADDR;
       // rkris@wisense.in - jan/28/15 (macCoordShortAddress was not being
       // reset to MAC_BROADCAST_SHORT_ADDR)
       MAC_PIB.macCoordShortAddress = MAC_BROADCAST_SHORT_ADDR;
       MAC_PIB.macLPWMNId = MAC_BROADCAST_LPWMN_ID;

       // TODO - reset the cc2500
       // RADIO_restart();

       // rkris@wisense.in - 2/march/15
       MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE;

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
   if (MAC_cntxt.pendIndTrxDestAddr == MAC_BROADCAST_SHORT_ADDR)
       SYS_fatal(SYS_FATAL_ERR_483);

   MAC_cntxt.pendingFrameTxInProgress = 0;
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

   if (SYS_globalEvtMsk2 & SYS_GLOBAL_EVT_NODE_RESET_PEND)
       MAC_cntxt.txAttempCnt = MAC_PIB.macMaxFrameRetries + 1;

#ifdef MAC_TRACE_ENA
   UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_35);
#endif

   MAC_cntxt.ackTmoCnt ++;

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
#ifndef DEV_TYPE_RFD
           UINT16_t nextHop = MAC_cntxt.expAckSrcShortAddr;
#endif

#ifdef MAC_TRACE_ENA
           UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_36);
#endif
           /*
            * If an acknowledgment is still not received after macMaxFrameRetries
            * retransmissions, the MAC sub-layer shall assume the transmission has
            * failed and notify the next higher layer of the failure.
            */

           MAC_cntxt.expAckSrcShortAddr = MAC_BROADCAST_SHORT_ADDR;

           // Free the BD (containing the unacked packet)
           MAC_freeCurrTxPkt(SYS_FATAL_ERR_600);

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
#ifdef RFD_COORD_POLL_ENA
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
                   MAC_cntxt.pollReqSent = 0;

                   // Do not dis-associate now. When this RFD wakes up next time, it will
                   // send some data anyway.

                   return;
               }
#endif
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

#ifndef DEV_TYPE_RFD
               AODVL_purgeRtTbls(nextHop);
#endif

#ifdef DEV_TYPE_RFD
               MAC_checkReassocCondition();
#endif
               MAC_sendDataCnfrm();
           }
       }
       else
       {
#ifdef MAC_TRACE_ENA
           UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_37);
#endif
           MAC_cntxt.txAttempCnt ++;

           MAC_csmaInit();
           MAC_doCsmaProc();
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
 *
 ********************************************************************
 */
void MAC_txOverEvtHndlr(const UINT8_t intRcvd)
{
   PLTFRM_sts_t sts;

#ifdef MAC_STATS_ENA
   if (intRcvd)
   {
       MAC_cntxt.stats.frameTxDoneEvtCnt ++;
   }
#endif

   /*
    * When a packet is successfully transmitted, the radio controller goes to
    * the state indicated by the MCSM1.TXOFF_MODE setting.
    * In this implementation, the radio is configured to go to IDLE after TX.
    */

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

       MAC_freeCurrTxPkt(SYS_FATAL_ERR_601);

       scanDuration = MAC_SCAN_DURATION_MILLI_SECS;
       if (PLTFRM_startTimerA0(scanDuration, 0, MAC_scanDurationTmoHndlr) != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_17);

#ifdef MAC_STATS_ENA
       MAC_cntxt.stats.scanTimerStartedCnt ++;
#endif

       MAC_cntxt.txModState = MAC_TX_MOD_STATE_CHANN_SCAN;

       return;
   }
#endif

   // PLTFRM_LOG("<%s> pkt (len - %d) trx over\n\r", __FUNCTION__, (SINT16_t)MAC_cntxt.macPduLen);

   if ((MAC_cntxt.immAckTxInProgress == 0x0) && (MAC_cntxt.ackRequired))
   {
       UINT16_t ackTmoMilliSecs;

#ifdef CC2500_TIMING_DBG_ENA
       // P1OUT &= ~BIT6;
       // P1OUT |= BIT7;
#endif

       ackTmoMilliSecs = CC2500_RX_TO_TX_ACK_TURN_AROUND_DELAY_MILLI_SECS
                         + MAC_ackPktTrxTimeMilliSecs
                         + MAC_cntxt.ackTmoExtraMilliSecs;

       /*
        * A frame transmitted with the AR field set to request an acknowledgment,
        * as defined in 5.2.1.1.4, shall be acknowledged by the recipient. If the
        * intended recipient correctly receives the frame, it shall generate and
        * send an acknowledgment frame containing the same DSN from the data or
        * MAC command frame that is being acknowledged.
        *
        * The CC2500 does not have automatic ack transmission facility. As a
        * result, we need to wait for a worst case duration of say 'x' seconds
        * for the corresponding ack.
        *
        * This node will not transmit any non-ack packet till it receives an
        * ack for the last transmitted non-ack pkt. Note that the last transmitted
        * packet will need to be resent to the TX FIFO in case it has to be retrxd.
        * The corresponding buffer descriptor (BD) is stored in the variable 
        * CC2500_cntxt.currTxPduHndl.
        */

#ifdef MAC_TRACE_ENA
       UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_34);
#endif

#ifdef SMAC_TIMER_SRC_DBG_ENA
       __timerA0Src = 1;
       __timerA0Du = 1500;
#endif

       sts = PLTFRM_startTimerA0(ackTmoMilliSecs,
                                 0,
                                 MAC_ackTmoHndlr);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_18);

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

#ifdef MAC_TRACE_ENA
       UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_110);
#endif

       if (MAC_cntxt.immAckTxInProgress == 0x0)
           MAC_freeCurrTxPkt(SYS_FATAL_ERR_602);

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
#ifdef MAC_TRACE_ENA
               UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_111);
#endif
               MAC_cntxt.datacnfrmParams.status = MAC_STS_SUCCESS;
               MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE;
               MAC_sendDataCnfrm();
           }
           else
           {
               if (MAC_cntxt.backOffInterrupted)
               {
#ifdef MAC_TRACE_ENA
                   UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_112);
#endif
                   // Restart CSMA-CA
                   MAC_cntxt.csmaCntxt.backOffExp = MAC_PIB.macMinBE;
                   MAC_cntxt.txModState = MAC_TX_MOD_STATE_RETRY_CSMA;  // MAC_TX_MOD_STATE_CSMA_BACK_OFF;
                   SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_RETRY_CCA;
                   MAC_cntxt.backOffInterrupted = 0;
               }
               else
               {
                   if (MAC_cntxt.ackRxPending)
                   {
#ifdef MAC_TRACE_ENA
                       UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_113);
#endif
                       MAC_cntxt.txModState = MAC_TX_MOD_STATE_WAIT_ACK;
                       MAC_cntxt.ackRxPending = 0;
#ifdef SMAC_TIMER_SRC_DBG_ENA
                       __timerA0Src = 2;
                       __timerA0Du = 5000;
#endif
                       sts = PLTFRM_startTimerA0(5,   // HACK (5 millisecs)
                                                 0,   // Hack
                                                 MAC_ackTmoHndlr);
                       if (sts != PLTFRM_STS_SUCCESS)
                           SYS_fatal(SYS_FATAL_ERR_19);
                   }
                   else
                   {
#ifdef MAC_TRACE_ENA
                       UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_114);
#endif
                       // Bug fix - rkris@wisense.in - dec/23/14
                       if (MAC_cntxt.csmaRetryInterrupted)
                       {
                           // MAC TX module was about to retry CSMA before this ACK was sent
                           MAC_cntxt.csmaRetryInterrupted = 0;
                           MAC_cntxt.txModState = MAC_TX_MOD_STATE_RETRY_CSMA;
                       }
                       else
                       {
                           if (MAC_cntxt.confirmInterrupted)
                           {
                               MAC_cntxt.confirmInterrupted = 0;
                               MAC_cntxt.txModState = MAC_TX_MOD_STATE_CONFIRM;
                           }
                           else
                           {
                               // MAC Tx module was IDLE before this ACK was sent
                               MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE;
                           }
                       }
                   }
               }
           }
       }

       MAC_cntxt.immAckTxInProgress = 0x0;
   }

   return;
}
  

CC2500_state_t CC2500_dbgState1;




/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void MAC_txCCAFlr(void)
{
   // Packet did not get transmitted !!

   if (CC2500_getCurrState(&CC2500_dbgState1) != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_20);

#ifdef MAC_STATS_ENA
   // MAC_cntxt.stats.channCCAFlrCnt[MAC_cntxt.scanChan] ++;
#endif

   MAC_cntxt.ccaFlrCnt ++;

   // Flush the TX FIFO <TODO>

   MAC_cntxt.csmaCntxt.numBackOffs ++;

   if (MAC_cntxt.csmaCntxt.numBackOffs > MAC_PIB.macMaxCSMABackoffs)
   {
       // All attempts exhausted !!
       MAC_cntxt.csmaFlrCnt ++;


       // Do not flush the TX FIFO now ...
       // We could be receiving a packet right now and flushing the TX FIFO requires
       // the C1101 to be put into the IDLE state.

       MAC_freeCurrTxPkt(SYS_FATAL_ERR_603);

#ifndef DEV_TYPE_RFD
       // Bug fixed on dec/5/14
       if (MAC_cntxt.pendingFrameTxInProgress)
           MAC_stopPendingFrameTrx();
#endif

       /*
        * If the transmission uses CSMA-CA and the CSMA-CA algorithm failed due to
        * adverse conditions on the channel, and the TxOptions parameter specifies
        * that a direct transmission is required, the MAC sublayer will discard the
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
                * access failure, the MAC sublayer shall notify the next higher layer.
                */
               MAC_sendAssocCnfrm(MAC_STS_CHANNEL_ACCESS_FLR,
                                  MAC_BROADCAST_SHORT_ADDR);
           }
           else
#endif
           {
#ifdef DEV_TYPE_RFD
               if (MAC_cntxt.pollReqSent)
               {
                   // Failed to send "data-request cmd" to parent !!.
#ifdef MAC_STATS_ENA
                   MAC_cntxt.stats.channAccessFlrPoll ++;
#endif
                   MAC_sendPollCnfrm(MAC_STS_CHANNEL_ACCESS_FLR);
                   MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE;
                   MAC_cntxt.pollReqSent = 0;
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

       // Bug fix - rkris@wisense.in, Dec/23/14
       MAC_cntxt.txModState = MAC_TX_MOD_STATE_RETRY_CSMA;

       SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_RETRY_CCA;
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


/*
 * The GDO pins can be used to give an interrupt when a complete packet has
 * been received with OK CRC by setting IOCFGx.GDOx_CFG = 0x7. De-asserts
 * when the first byte is read from the RX FIFO.
 */


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void MAC_txOverCbFunc(void)
{
  CC2500_state_t cc2500State;

#ifdef MAC_TRACE_ENA
  UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_29);
#endif
  
#ifdef CC2500_TIMING_DBG_ENA
  // P1OUT |= BIT6;
  // P1OUT &= ~BIT7;
#endif

  // The packet should have been transmitted by now and the Radio
  // should be back in the IDLE state.

  if (CC2500_getCurrState(&cc2500State) != PLTFRM_STS_SUCCESS)
      SYS_fatal(SYS_FATAL_ERR_21);

#ifdef CUSTOMER_ANDY_SPECIFIC_COORD_BUILD
  if (cc2500State == CC2500_STATE_RX)
#else
  /*
   * rkris@wisense.in - jan/28/15
   * Somtimes the CC2500 state has been RX instead of IDLE !!
   * although radio is configured to go to IDLE after TX.
   */
  if (cc2500State == CC2500_STATE_IDLE
      || cc2500State == CC2500_STATE_RX)
#endif
  {
      UINT8_t byteCnt;
      PLTFRM_sts_t sts;

#ifdef CC2500_TIMING_DBG_ENA
      // P1OUT &= ~BIT6;
#endif

      // Now check if the TX FIFO is empty ...
      sts = CC2500_getTXFIFOByteCnt(&byteCnt);
      if (sts != PLTFRM_STS_SUCCESS || byteCnt > 0)
          SYS_fatal(SYS_FATAL_ERR_22);
     
#ifndef CUSTOMER_ANDY_SPECIFIC_COORD_BUILD
      /*   
       * In this implementation, the radio goes to the IDLE state after transmission.
       * Put the radio back into RX asap.
       * We have configured the CC2500 to auto calibrate when going from IDLE to RX/TX.
       * This time is 1953/fxosc + FS calibration Time
       * Here fxosc is 26 MHz and "FS calibration Time" is 712 microsecs when TEST0 is
       * set to 9 and FSCAL3.CHP_CURR_CAL_EN is set to 2.
       * The auto-cal duration comes out to around 787 microsecs.
       */ 
      sts = CC2500_startRx();
      if (sts != PLTFRM_STS_SUCCESS)
          SYS_fatal(SYS_FATAL_ERR_23);

#ifdef MAC_TRACE_ENA
      UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_30);
#endif
#endif

      // Packet transmitted successfully ...
      MAC_txOverEvtHndlr(1);
  }
  else
  {
      SYS_fatal(SYS_FATAL_ERR_24);
  }

  return;
}
           

#ifdef MAC_RSSI_LOG_ENA
UINT8_t MAC_rssiLogIdx = 0;
SINT16_t MAC_rssiLog[256];
UINT16_t MAC_rssiSmplCnt = 0;
UINT16_t MAC_rssiOkSmplCnt = 0;
SINT16_t maxRSSI = -32767;
SINT16_t minRSSI = 32767;
#endif

UINT16_t __idx;


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
  CC2500_state_t cc2500State;
  UINT8_t txFIFOByteCnt;

#ifdef CC2500_TIMING_DBG_ENA
   // P1OUT |= BIT6;
   // P1OUT &= ~BIT7=-0=
#endif

#ifdef MAC_TRACE_ENA
  UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_1);
#endif

#ifdef MAC_STATS_ENA
  MAC_cntxt.stats.csmaAttemptTxOnCcaCnt ++;
#endif

  if (SYS_GET_BD_LEN(MAC_cntxt.currTxPduHndl) == 0)
  {
      SYS_fatal(SYS_FATAL_ERR_461);
  }

  if (CC2500_cntxt.inLPM)
  {
      if (CC2500_exitLPM() != PLTFRM_STS_SUCCESS)
          SYS_fatal(SYS_FATAL_ERR_25);

      // Radio is in IDLE state here ...
  }

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
    * When an STX or SFSTXON command strobe is given while CC2500 is in
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
   // CC1200_checkRadioErrors();

   if (CC2500_getCurrState(&cc2500State) != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_26);

   if (CC2500_getTXFIFOByteCnt(&txFIFOByteCnt) != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_27);

   if (txFIFOByteCnt > 0)
   {
       if (cc2500State != CC2500_STATE_IDLE)
       {
           if (CC2500_forceIdle() !=  PLTFRM_STS_SUCCESS)
               SYS_fatal(SYS_FATAL_ERR_28);

           cc2500State = CC2500_STATE_IDLE;
       }

       if (CC2500_flushTxFIFO( ) != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_29);
   }

   // Send the packet to the FIFO before triggering transmission
   CC2500_transferPktToTxFifo(MAC_cntxt.currTxPduHndl);

   if (cc2500State == CC2500_STATE_IDLE)
   {
       sts = CC2500_startRx();
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_30);

       /*
        * It is important to note that the radio must stay in RX for a little while
        * before the RSSI is valid (if STX is strobed before the RSSI is valid, the
        * packet will not be sent).
        */
       sts = CC2500_waitTillRSSIValid();
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_31);
   }

   /*
    *  
    * When the STX or SFSTXON command strobe is given while CC2500 is in
    * the RX state, the TX or FSTXON state is only entered if the clear 
    * channel requirements are fulfilled. Otherwise, the chip will remain 
    * in RX. If the channel then becomes available, the radio will not enter 
    * TX or FSTXON state before a new strobe command is sent on the SPI intf. 
    * This feature is called TX-if-CCA. 
    *
    * MCSM1.CCA_MODE selects the mode to use when determining CCA.
    *  
    * In this implementation, the CCA requirement is that the RSSI should be 
    * below threshold and not currently receiving a packet.
    *
    * The RSSI value is an estimate of the signal power level in the chosen 
    * channel. This value is based on the current gain setting in the RX chain 
    * and the measured signal level in the channel.
    *
    * In RX mode, the RSSI value can be read continuously from the RSSI status 
    * reg until the demodulator detects a sync word (when sync word detection 
    * is enabled). At that point the RSSI readout value is frozen until the 
    * next time the chip enters the RX state.
    */

   CC2500_cntxt.txStartedFlag = 0;

#ifdef CC2500_TIMING_DBG_ENA
   // P1OUT |= BIT6;
#endif


#ifdef MAC_TRACE_ENA
   UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_3);
#endif

#ifdef MAC_RSSI_LOG_ENA
   maxRSSI = -32767;
   minRSSI = 32767;
   MAC_rssiLogIdx = 0;
   memset(MAC_rssiLog, 0, sizeof(MAC_rssiLog));

   for (;;)
   {
      UINT8_t rssiU8;
      SINT16_t rssiS16;

      if (CC2500_readReg(CC2500_RSSI_REG_ADDR, &rssiU8) != PLTFRM_STS_SUCCESS)
          SYS_fatal(SYS_FATAL_ERR_843);
      rssiS16 = CC2500_calcPktRSSI(rssiU8);
      PLTFRM_delay1MilliSec();
      MAC_rssiLog[MAC_rssiLogIdx ++] = rssiS16;

      if (rssiS16 > maxRSSI)
    	  maxRSSI = rssiS16;
      if (rssiS16 < minRSSI)
    	  minRSSI = rssiS16;

      if (MAC_rssiLogIdx == 0x0)
    	  break;
   }
#endif

   // rkris@wisense.in - dec/24/14 - bug fix
   // Check if a packet has been received rght before sending the STX probe.
   // Since the CC2500 would usually be in RX state (at least on the coord and FFDs)
   // till this point, it is possible that the CC2500 has just completely received
   // a packet and as a result the STX probe will succeed.
   if (SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_MAC_FRAME_RX_DONE)
   {
#ifdef MAC_STATS_ENA
       MAC_cntxt.stats.abortTxCnt ++;
#endif

#ifdef MAC_TRACE_ENA
       UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_2);
#endif
       goto _skipTx;
   }

#ifdef CC2500_TIMING_DBG_ENA
   P1OUT &= ~BIT7;
#endif

   sts = CC2500_sendCmdStrobe(CC2500_CMD_STROBE_STX, NULL);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_33);
   }

#ifdef MAC_STATS_ENA
   MAC_cntxt.stats.txStrobeCnt ++;
#endif

   /*
    * Wait for a PA_PD signal (IOCFGn=0x1B) to be asserted. This should happen
    * about 782/fRef us after the STX strobe has been issued (fRef is the
    * reference frequency for the synthesizer in MHz). We are using a 26 MHz
    * crystal which gives a delta of 30.1 microsecs.
    * PA_PD goes high after 782/fRef and then goes low at the end of the packet.
    */

   // PLTFRM_delay100MicroSecs( );

#ifdef MAC_RSSI_LOG_ENA
   MAC_rssiSmplCnt = 0;
   MAC_rssiOkSmplCnt = 0;
#endif

   {
      // Increased from 50 to 70 to support preamble length of 24
      for (__idx=0; __idx<70; __idx++)
      {
#ifdef MAC_RSSI_LOG_ENA
           MAC_rssiSmplCnt ++;
#endif
           if (CC2500_cntxt.txStartedFlag)
           {
#ifdef MAC_RSSI_LOG_ENA
               MAC_rssiOkSmplCnt = MAC_rssiSmplCnt;
#endif
               break;
           }

           PLTFRM_delay10MicroSecs( );
      }
   }


_skipTx:
   if (CC2500_cntxt.txStartedFlag)
   {
       UINT16_t tmoMilliSecs;

#ifdef FLASH_LED_ON_PKT_RX_TX
       PLTFRM_ON_LED_1();
#endif

#ifdef MAC_TRACE_ENA
       UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_4);
#endif

#ifdef MAC_STATS_ENA
       MAC_cntxt.stats.ccaOkCnt ++;
#endif

       // CC2500 is configured to go to IDLE mode after packet transmission.
       
       CC2500_cntxt.txStartedFlag = 0;

       if (SYS_GET_BD_LEN(MAC_cntxt.currTxPduHndl) == 0)
       {
           SYS_fatal(SYS_FATAL_ERR_460);
       }

       tmoMilliSecs = CC2500_calcPktTrxTime(SYS_GET_BD_LEN((MAC_cntxt.currTxPduHndl)));

       // TX->IDLE without calibration is .25/baud_Rate
       tmoMilliSecs += (CC2500_txToIdleTransitionTimeMilli);

#ifdef RADIO_BAUD_RATE_1200
       tmoMilliSecs += 5;   // Taking 5 milli-secs extra !! Why ?
#endif
#ifdef SMAC_TIMER_SRC_DBG_ENA
       __timerA0Src = 3;
       __timerA0Du = tmoMilliSecs;
#endif
       if (PLTFRM_startTimerA0(tmoMilliSecs, 0, MAC_txOverCbFunc) != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_34);
       MAC_cntxt.txModState = MAC_TX_MOD_STATE_WAIT_TX_OVER;

#ifdef FLASH_LED_ON_PKT_RX_TX
       PLTFRM_OFF_LED_1();
#endif

#ifdef MAC_TRACE_ENA
       UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_5);
#endif
   }
   else
   {
#ifdef CC2500_TIMING_DBG_ENA
       // P1OUT &= ~BIT6;
#endif
#ifdef MAC_TRACE_ENA
       UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_6);
#endif

       MAC_txCCAFlr( );
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
   UINT8_t *macHdr_p = SYS_GET_BD_BUFF(bdHndl);
   MAC_dsn_t dsn = MAC_getNextDSN();

#ifdef MAC_TRACE_ENA
   UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_38);
#endif
   /*
    * The Sequence Number field shall contain the current value of macDSN.
    * macDSN is the sequence number added to the transmitted data or MAC
    * command frame.
    */

#if (MAC_PDU_HDR_SEQ_NR_FIELD_LEN == 4)
   UTIL_htole32(macHdr_p + MAC_PDU_HDR_DSN_FIELD_OFF, dsn);
#elif (MAC_PDU_HDR_SEQ_NR_FIELD_LEN == 2)
   UTIL_htole16(macHdr_p + MAC_PDU_HDR_DSN_FIELD_OFF, dsn);
#else
   macHdr_p[MAC_PDU_HDR_DSN_FIELD_OFF] = dsn;
#endif

   MAC_cntxt.ackRequired = 0x1;
   MAC_cntxt.txAttempCnt = 1;
   MAC_cntxt.macPduLen = SYS_GET_BD_LEN(bdHndl);
   MAC_cntxt.cnfrmReqd = 0x1;
   MAC_cntxt.expAckDSN =  dsn;
   MAC_cntxt.expAckSrcShortAddr = UTIL_letoh16(macHdr_p + MAC_PDU_HDR_DEST_ADDR_OFF);

#ifdef MAC_STATS_ENA
   MAC_cntxt.stats.fwdPktTxReqCnt ++;
#endif
   
   MAC_cntxt.currTxPduHndl = bdHndl;

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
 *
 ********************************************************************
 */
UINT16_t MAC_getCsmaBackOff(const UINT16_t backOffLimit)
{
   UINT16_t randVal;

#ifdef MAC_CSMA_SW_RAND_GEN
   randVal = pltfrm_randU16();
#else
#error random generator not available !!
#endif

   randVal = randVal % (backOffLimit + 1);

   return randVal;
}

#ifdef MAC_DELAY_LOG_ENA
UINT8_t MAC_delayLogIdx = 0;
UINT32_t MAC_delayLog[8];
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
#ifdef MAC_CSMA_CA_DISABLED
      if (0)
#else
      if (MAC_cntxt.csmaCntxt.backOffExp > 0x0)
#endif
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
           * to 255*20*(1/baud) microseconds.
           * - At 38383 baud, range is 0 to 133 millisecs
           * - At 10000 baud, range is 0 to 510 millisecs
           * - At 1200 baud, range is 0 to 4.253 seconds.
           */
          backOffDelay *= PHY_SYMBOL_LEN_USECS;
#else
          backOffDelay = 0;
#endif

#ifdef MAC_DELAY_LOG_ENA
          MAC_delayLog[MAC_delayLogIdx ++] = backOffDelay;
          MAC_delayLogIdx &= 0x7;
#endif

          MAC_cntxt.txModState = MAC_TX_MOD_STATE_CSMA_BACK_OFF;

          if (backOffDelay > (UINT32_t)2000000)   // TimerA0 limit is 2 seconds
              backOffDelay = (UINT32_t)2000000;

#ifdef MAC_STATS_ENA
          MAC_cntxt.stats.lastBackOffDelay = backOffDelay;
#endif

          if (backOffDelay > 0)
          {
#ifdef SMAC_TIMER_SRC_DBG_ENA
              __timerA0Src = 4;
              __timerA0Du = backOffDelay;
#endif
              if (PLTFRM_startTimerA0(backOffDelay/1000,  // milli-secs
                                      backOffDelay % 1000,  // micro-secs
                                      MAC_csmaAttemptTxOnCca) != PLTFRM_STS_SUCCESS)
              {
                  SYS_fatal(SYS_FATAL_ERR_35);
              }

#ifdef MAC_TRACE_ENA
              UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_9);
#endif
          }
          else
          {
#ifdef MAC_TRACE_ENA
              UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_10);
#endif
              MAC_csmaAttemptTxOnCca(  );
          }
      }
      else
      {
#ifdef MAC_TRACE_ENA
          UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_11);
#endif
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
#if (MAC_PDU_HDR_SEQ_NR_FIELD_LEN == 4)
   UTIL_htole32(hdr_p + offset, pduInfo_p->seqNr);
#elif (MAC_PDU_HDR_SEQ_NR_FIELD_LEN == 2)
   UTIL_htole16(hdr_p + offset, pduInfo_p->seqNr);
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
   if (pduInfo_p->dam != MAC_ADDRESS_MODE_NO_ADDR)
   {
       UTIL_htole16(hdr_p + offset, MAC_PIB.macLPWMNId);
       offset += MAC_LPWMN_ID_LEN;

       /*
        * The Destination Address field, when present, with a length
        * specified in the Destination Addressing Mode field of the
        * Frame Control field as described in 5.2.1.1.6, specifies the
        * address of the intended recipient of the frame. A value of
        * 0xffff in this field shall represent the broadcast short
        * address, which shall be accepted as a valid address by all
        * devices currently listening to the channel.
        */
       if (pduInfo_p->dam == MAC_ADDRESS_MODE_SHORT_ADDR)
       {
           UINT16_t da = destAddr_p != NULL ? destAddr_p->shortAddr : destShortAddr;
           UTIL_htole16(hdr_p + offset, da);
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
           UTIL_htole16(hdr_p + offset, MAC_PIB.macLPWMNId);
           offset += MAC_LPWMN_ID_LEN;
       }

       if (pduInfo_p->sam == MAC_ADDRESS_MODE_SHORT_ADDR)
       {
           UTIL_htole16(hdr_p + offset, MAC_PIB.macShortAddress);
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
      if (pduLen > CC2500_MAX_MAC_PDU_LEN)
      {
          SYS_freeMem(pduInfo_p->bdHndl);
          sts = MAC_STS_FRAME_TOO_LONG;
      }
   }

   return sts;
}


#if defined(DEV_TYPE_FFD) || defined(LPWMN_COORD)
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
   SYS_trscnQ_s *trscnQ_p = NULL;

#ifdef LPWMN_COORD
   trscnQ_p = NM_checkPendingFrame(shortAddr);
#else
   {
      UINT16_t idx;
      for (idx=0; idx<MAC_MAX_ASSOC_RFD_CNT; idx++)
      {
         if (MAC_cntxt.assocRFDList[idx] == shortAddr)
         {
             trscnQ_p = &(MAC_cntxt.assocRFDPendingFramesQList[idx]);
             if (SYS_TRSCN_Q_IS_EMPTY(trscnQ_p))
                 trscnQ_p = NULL;
             break;
         }
      }
   }
#endif

   return trscnQ_p;
}

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
       return;

   // Make sure that this RFD has not disappeared ...
   trscnQ_p = MAC_checkFrameIsPending(MAC_cntxt.pendIndTrxDestAddr);

   if (trscnQ_p == NULL)
       return;

   if (SYS_TRSCN_Q_IS_EMPTY(trscnQ_p))
       return;

   bdHndl = SYS_GET_TRSCN_Q_HEAD(trscnQ_p);
   SYS_delFromTrscnsQ(trscnQ_p, bdHndl);

   // In this implementation, if this frame is not successfully transmitted, it
   // is discarded (instead of being put back at the head of the RFD's pending frame
   // queue). We are depending on multiple retransmits attempts to get the packet
   // accross.

   SYS_ADJ_BD_HEAD(bdHndl, SYS_TRSCN_Q_HNDL_FIELD_LEN);

   MAC_cntxt.ackRequired = 1;   // Unicast packets always require an ACK
   MAC_cntxt.txAttempCnt = 1;
   MAC_cntxt.macPduLen = SYS_GET_BD_LEN(bdHndl);


#if 0
   {
      UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);
#if (MAC_PDU_HDR_SEQ_NR_FIELD_LEN == 4)
      MAC_cntxt.expAckDSN = UTIL_letoh32(buff_p + MAC_PDU_HDR_FC_FIELD_LEN);
#elif (MAC_PDU_HDR_SEQ_NR_FIELD_LEN == 2)
      MAC_cntxt.expAckDSN = UTIL_letoh16(buff_p + MAC_PDU_HDR_FC_FIELD_LEN);
#else
      MAC_cntxt.expAckDSN = buff_p[MAC_PDU_HDR_FC_FIELD_LEN];
#endif
   }
#else
   {
      UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);

      // Bug fix - march/2/15 - allocate new DSN

      MAC_cntxt.expAckDSN =  MAC_getNextDSN();
#if (MAC_PDU_HDR_SEQ_NR_FIELD_LEN == 4)
      UTIL_htole32(buff_p + MAC_PDU_HDR_FC_FIELD_LEN, MAC_cntxt.expAckDSN);
#elif (MAC_PDU_HDR_SEQ_NR_FIELD_LEN == 2)
      UTIL_htole16(buff_p + MAC_PDU_HDR_FC_FIELD_LEN, MAC_cntxt.expAckDSN);
#else
      buff_p[MAC_PDU_HDR_FC_FIELD_LEN] = MAC_cntxt.expAckDSN;
#endif
   }
#endif

   MAC_cntxt.expAckSrcShortAddr = MAC_cntxt.pendIndTrxDestAddr;
   MAC_cntxt.cnfrmReqd = 0x0;  // No confirmation required !!

   if (SYS_BD_HNDL_IS_VALID(MAC_cntxt.currTxPduHndl))
   {
       SYS_fatal(SYS_FATAL_ERR_482);
   }

   MAC_cntxt.currTxPduHndl = bdHndl;

#ifdef MAC_STATS_ENA
   MAC_cntxt.stats.pendingFrameTxCnt ++;
#endif

   MAC_cntxt.pendingFrameTxInProgress = 1;

   MAC_csmaInit();
   MAC_doCsmaProc();

   return;
}


#ifdef DEV_TYPE_FFD
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void MAC_purgeRFDPendFrames(UINT8_t assocRFDTblIdx)
{
   SYS_trscnQ_s *trscnQ_p; 

   if (assocRFDTblIdx >= MAC_MAX_ASSOC_RFD_CNT)
       SYS_fatal(SYS_FATAL_ERR_36);

   trscnQ_p = &(MAC_cntxt.assocRFDPendingFramesQList[assocRFDTblIdx]);
   SYS_purgeTrscnsQ(trscnQ_p);

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
UINT8_t MAC_addFrameToPendQ(MAC_shortAddr_t shortAddr, UINT8_t pdubdHndl)
{
   UINT8_t rc = 1;
   SYS_trscnQ_s *trscnQ_p = NULL;

#ifdef LPWMN_COORD
   trscnQ_p = NM_getRFDPendTrscnQ(shortAddr);
#else
   {
      UINT16_t idx;

      for (idx=0; idx<MAC_MAX_ASSOC_RFD_CNT; idx++)
      {
          if (MAC_cntxt.assocRFDList[idx] == shortAddr)
          {
              trscnQ_p = &(MAC_cntxt.assocRFDPendingFramesQList[idx]);
              break;
          }
      }
   }
#endif

   if (trscnQ_p == NULL)
   {
       rc = 0;
   }
   else
   {
       SYS_addToTrscnsQ(trscnQ_p, pdubdHndl);
   }

   return rc;
}
// DEV_TYPE_FFD || LPWMN_COORD
#endif


#ifndef DEV_TYPE_RFD
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t MAC_dataReqCmdHndlr(MAC_shortAddr_t rfdAddr)
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
#ifdef DEV_TYPE_FFD
        case MAC_CMD_NWK_RESTART:
             {
                SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_NWK_RESTART_CMD_RCVD;
                SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_NODE_RESET_PEND;
             }
             break;
#endif

#if !defined(DEV_TYPE_RFD)
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

#ifdef MAC_STATS_ENA
                MAC_cntxt.stats.bcnReqRcvdCnt ++;
#endif

                // Accept beacon requests only when associated.
                if (MAC_PIB.macShortAddress != MAC_BROADCAST_SHORT_ADDR
                    && MAC_cntxt.acceptBcnReqs    // May/23/15, rkris@wisense.in
#ifdef DEV_TYPE_FFD
                    && MAC_cntxt.assocRFDCnt < MAC_MAX_ASSOC_RFD_CNT
#endif
                   )
                {
                   // Check if the source node is already associated with this node.

                   SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_BCN_REQUEST;
                }
             }
             break;
#endif

#ifndef LPWMN_COORD
        case MAC_CMD_ASSOCIATION_RESP:
             {
                UINT8_t assocSts;

#ifdef MAC_STATS_ENA
                MAC_cntxt.stats.assocRespRxCnt ++;
#endif

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
#ifdef DEV_TYPE_FFD
                // Drop this command if this FFD is not yet associated
                if (MAC_PIB.macShortAddress == MAC_BROADCAST_SHORT_ADDR)
                {
                    // Drop this frame
                    break;
                }
#endif

#ifdef LPWMN_COORD
                if (!MAC_nwkJoinsAllowed())
                {
                    // Network joins not allowed. Drop this frame.
                    break;
                }
#ifndef LPWMN_COORD_ASSOC_WHITE_LIST_DIS
                if (!MAC_lookUpAssocWhiteList(MAC_cntxt.rcvdPktInfo.srcAddr.extAddr))
                {
                    // White list is not empty and this node is not in the list.
                    break;
                }
#endif
#endif

                /*
                 * The MLME-ASSOCIATE.indication primitive is used to indicate the
                 * reception of an association request command.
                 * When the next higher layer of a coordinator receives the MLME-
                 * ASSOCIATE.indication primitive, the coordinator determines whether
                 * to accept or reject the un associated device using an algorithm
                 * outside the scope of this standard.
                 */
#ifdef MAC_STATS_ENA
                MAC_cntxt.stats.assocReqRcvdCnt ++;
#endif

                ADP_macAssocIndHndlr(MAC_cntxt.rcvdPktInfo.srcAddr.extAddr,
                                     cmd_p[1], UTIL_ntohs(&cmd_p[2]));  // Get the last saved fatal error Id
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
                    *dataPendFlag_p = MAC_dataReqCmdHndlr(MAC_cntxt.rcvdPktInfo.srcAddr.shortAddr);
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
       SYS_fatal(SYS_FATAL_ERR_38);

   return;
}
// !LPWMN_COORD
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
#ifdef DEV_TYPE_RFD
   MAC_cntxt.datacnfrmParams.framePending = 0;
#endif

#ifdef CC2500_TIMING_DBG_ENA
   // P1OUT &= ~BIT6;
   // P1OUT |= BIT7;
#endif

   // Stop the ack-timeout timer
   PLTFRM_stopTimerA0();

#ifdef MAC_STATS_ENA
   MAC_cntxt.stats.validAckRxCnt ++;
#endif

#ifdef DEV_TYPE_RFD
#ifdef RFD_COORD_POLL_ENA
   if (MAC_cntxt.pollReqSent)
   {
       MAC_cntxt.pollReqSent = 0;
       MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE;
       MAC_sendPollCnfrm(framePending ? MAC_STS_FRAME_PENDING : MAC_STS_NO_FRAME_PENDING);
       return;
   }
   else
   {
       MAC_cntxt.datacnfrmParams.framePending = framePending ? 1 : 0;
   }
#endif
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
#if (MAC_PDU_HDR_SEQ_NR_FIELD_LEN == 4)
   UTIL_htole32(buff_p + offset, pduInfo.seqNr);
#elif (MAC_PDU_HDR_SEQ_NR_FIELD_LEN == 2)
   UTIL_htole16(buff_p + offset, pduInfo.seqNr);
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
       SYS_fatal(SYS_FATAL_ERR_39);
   }

   MAC_cntxt.currTxPduHndl = pduInfo.bdHndl;

   CC2500_insertTurnAroundDelay();

   MAC_csmaInit();
   MAC_doCsmaProc();

   return sts;
}
// !DEV_TYPE_RFD
#endif


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
   UINT8_t *buff_p = MAC_cntxt.ackTxBuff
                     + MAC_PDU_HDR_FC_FIELD_MAX_LEN
                     + MAC_PDU_HDR_SEQ_NR_FIELD_LEN;

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

#if (MAC_PDU_HDR_SEQ_NR_FIELD_LEN == 4)
   UTIL_htole32(buff_p, rcvdPktInfo_p->dsn);
#elif (MAC_PDU_HDR_SEQ_NR_FIELD_LEN == 2)
   UTIL_htole16(buff_p, rcvdPktInfo_p->dsn);
#else
   *(buff_p) = rcvdPktInfo_p->dsn;
#endif

   buff_p += (MAC_PDU_HDR_SEQ_NR_FIELD_LEN + MAC_LPWMN_ID_LEN);
   buff_p = UTIL_htole16(buff_p, rcvdPktInfo_p->srcAddr.shortAddr);
   buff_p += MAC_SHORT_ADDR_LEN;
   *(buff_p ++) = rcvdPktInfo_p->rssi;
   *(buff_p) = rcvdPktInfo_p->lqi;

   return;
}

CC2500_state_t __sanState_2, __sanState_1, __sanState_3,
               __sanState_4, __sanState_5;

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

#ifdef CC2500_TIMING_DBG_ENA
   // P1OUT &= ~BIT6;
#endif

   {
       sts = CC2500_getCurrState(&__sanState_1);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_21);
   }

   // Go to IDLE mode and then send STX strobe.
   sts = CC2500_forceIdle();
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_40);

   CC2500_insertTurnAroundDelay();

   {
       sts = CC2500_getCurrState(&__sanState_2);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_21);
   }
   /*
    * Here we are sending an ACK packet after receiving an packet which needs to be
    * acknowledged.
    */

   // *** Do not touch any of these variables below ****
   // -  MAC_cntxt.ackRequired = 0;
   // -  MAC_cntxt.txAttempCnt = 1;
   // -  MAC_cntxt.macPduLen = MAC_SIM_802_15_4_ACK_PDU_LEN;
   // -  MAC_cntxt.expAckDSN =  0;
   // -  MAC_cntxt.cnfrmReqd = 0;

   // Flush the TX FIFO. This can be done at this point since the radio is in IDLE 
   // mode right now.

#ifdef CC2500_TIMING_DBG_ENA
   {
      // P1OUT |= BIT7;
      // PLTFRM_delay10MicroSecs();
      // P1OUT &= ~BIT7;
   }
#endif

   sts = CC2500_sendCmdStrobe(CC2500_CMD_STROBE_SFTX, NULL);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_41);

   {
       sts = CC2500_getCurrState(&__sanState_3);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_21);
   }

#ifdef CC2500_TIMING_DBG_ENA
   // P1OUT |= (BIT7);
#endif
   sts = CC2500_writeBuffToTxFIFO(MAC_cntxt.ackTxBuff,
                                  sizeof(MAC_cntxt.ackTxBuff));
#ifdef CC2500_TIMING_DBG_ENA
   // P1OUT &= ~(BIT7);
#endif
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_42);


   // STX strobe
   // In IDLE state: Enable TX. Perform calibration first if MCSM0.FS_AUTOCAL = 1.
   
   CC2500_cntxt.txStartedFlag = 0;

#ifdef MAC_TRACE_ENA
   UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_31);
#endif

#ifdef CC2500_TIMING_DBG_ENA
   // P1OUT &= ~(BIT6);
#endif

   sts = CC2500_sendCmdStrobe(CC2500_CMD_STROBE_STX, NULL);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_43);
   else
   {
#ifdef MAC_STATS_ENA
       MAC_cntxt.stats.ackTxStrobeCnt ++;
#endif

       {
           sts = CC2500_getCurrState(&__sanState_4);
           if (sts != PLTFRM_STS_SUCCESS)
               SYS_fatal(SYS_FATAL_ERR_21);
       }
       /*
        * Wait for a PA_PD signal (IOCFGn=0x1B) to be asserted. This should happen
        * about 1953/fRef + FS_calibration_time after the STX strobe has been issued 
        * (fRef is the reference frequency for the synthesizer in MHz). We are using 
        * a 26 MHz crystal which gives a delta of 799 microsecs.
        */

#ifdef FLASH_LED_ON_PKT_RX_TX
       PLTFRM_ON_LED_1();
#endif

       PLTFRM_delay1MilliSec( );

#ifdef FLASH_LED_ON_PKT_RX_TX
       PLTFRM_OFF_LED_1();
#endif

       {
           sts = CC2500_getCurrState(&__sanState_5);
           if (sts != PLTFRM_STS_SUCCESS)
               SYS_fatal(SYS_FATAL_ERR_21);
       }

       if (CC2500_cntxt.txStartedFlag)
       {
           UINT16_t tmoMilliSecs;
       
           // CC2500 is configured to go to IDLE mode after packet transmission.

           CC2500_cntxt.txStartedFlag = 0;

           tmoMilliSecs = MAC_ackPktTrxTimeMilliSecs;
           // tmoMilliSecs = CC2500_calcPktTrxTime(sizeof(MAC_cntxt.ackTxBuff));
           // TX->IDLE without calibration is .25/baud_Rate (1 microsec at 250 Kbaud)
           tmoMilliSecs += CC2500_txToIdleTransitionTimeMilli;
#ifdef RADIO_BAUD_RATE_1200
           tmoMilliSecs += 5;   // Taking 5 milli-secs extra !! Why ?
#endif
#ifdef SMAC_TIMER_SRC_DBG_ENA
           __timerA0Src = 6;
           __timerA0Du = tmoMilliSecs;
#endif
           if (PLTFRM_startTimerA0(tmoMilliSecs, 0, MAC_txOverCbFunc) != PLTFRM_STS_SUCCESS)
               SYS_fatal(SYS_FATAL_ERR_44);
           MAC_cntxt.txModState = MAC_TX_MOD_STATE_WAIT_TX_OVER;

#ifdef MAC_STATS_ENA
           MAC_cntxt.stats.ackTrxStartedCnt ++;
#endif

           MAC_cntxt.immAckTxInProgress = 0x1;
       }
       else
       {
           // Transmission failed !! Should never happen since we were in IDLE state.
           SYS_fatal(SYS_FATAL_ERR_45);
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
   if (MAC_cntxt.txModState != MAC_TX_MOD_STATE_WAIT_TX_OVER)
   {
       MAC_cntxt.backOffInterrupted = 0;
       MAC_cntxt.ackRxPending = 0;
       MAC_cntxt.csmaRetryInterrupted = 0;
       MAC_cntxt.confirmInterrupted = 0;
   }

#ifdef MAC_TRACE_ENA
   UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_20);
#endif

   switch (MAC_cntxt.txModState)
   {
      case MAC_TX_MOD_STATE_CSMA_BACK_OFF:
           {
#ifdef MAC_STATS_ENA
              MAC_cntxt.stats.immAckInCsmaBackOffCnt ++;
#endif
#ifdef MAC_TRACE_ENA
              UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_21);
#endif
              PLTFRM_stopTimerA0();
              // Send ACK now without doing CSMA/CA
              MAC_cntxt.backOffInterrupted = 1;
           }
           break;

      case MAC_TX_MOD_STATE_WAIT_ACK:
           {
#ifdef MAC_STATS_ENA
              MAC_cntxt.stats.immAckInWaitAckCnt ++;
#endif
#ifdef MAC_TRACE_ENA
              UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_22);
#endif
              // This node is expecting an ack but got a data/cmd packet
              // instead !!
              PLTFRM_stopTimerA0();
              MAC_cntxt.ackRxPending = 1;
           }
           break;

      case MAC_TX_MOD_STATE_IDLE:
           {
#ifdef MAC_STATS_ENA
              MAC_cntxt.stats.immAckWhenIdle ++;
#endif
#ifdef MAC_TRACE_ENA
              UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_23);
#endif
           }
           break;

      case MAC_TX_MOD_STATE_CONFIRM:
           {
#ifdef MAC_STATS_ENA
              MAC_cntxt.stats.immAckCnfrm ++;
#endif
#ifdef MAC_TRACE_ENA
              UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_24);
#endif
              MAC_cntxt.confirmInterrupted = 1;
           }
           break;

      case MAC_TX_MOD_STATE_RETRY_CSMA:
           {
#ifdef MAC_STATS_ENA
              MAC_cntxt.stats.immAckRetryCSMA ++;
#endif
#ifdef MAC_TRACE_ENA
              UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_25);
#endif
              MAC_cntxt.csmaRetryInterrupted = 1;
           }
           break;

      case MAC_TX_MOD_STATE_WAIT_TX_OVER:
           {
              // If a packet was received just before the CC2500 was strobed, we will
              // hit this case. Can't send ACK right now since CC2500 is transmitting
              // a packet. Just do not send ACK which is ok since the probability of
              // this happening are pretty slim.
#ifdef MAC_STATS_ENA
              MAC_cntxt.stats.pktRcvdWaitTxOverCnt ++;
#endif
#ifdef MAC_TRACE_ENA
              UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_26);
#endif
           }
           break;

      default:
           {
              SYS_fatal(SYS_FATAL_ERR_46);
           }
           break;
   }

   
   if (MAC_cntxt.txModState != MAC_TX_MOD_STATE_WAIT_TX_OVER)
   {
#ifdef MAC_TRACE_ENA
       UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_27);
#endif
       // Send ACK now without doing CSMA/CA
       MAC_updateAckBuff(rcvdPktInfo_p, framePending);
       MAC_sendAckNow();
   }

   return;
}


#if defined(DEV_TYPE_FFD) || defined(LPWMN_COORD)
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void MAC_initRcvdPktInfoList(void)
{
   UINT16_t idx;
   MAC_rcvdPktInfo_s *entry_p = MAC_cntxt.rcvdPktInfoList;

   for (idx=0; idx<MAC_RCVD_PKT_INFO_LIST_MAX_SZ; idx++)
   {
        entry_p->srcAddr = MAC_BROADCAST_SHORT_ADDR;
        entry_p->tteSecs = 0x0;
        entry_p->lastRcvdDSN = MAC_INVALID_DSN;
        entry_p ++;     
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
void MAC_checkRcvdPktInfoList(void)
{
   UINT16_t idx;
   MAC_rcvdPktInfo_s *entry_p = MAC_cntxt.rcvdPktInfoList;

   // Called every second from SYS_periodicTmrExpCbFunc()

   // TODO - Needs optimization ...
   for (idx=0; idx<MAC_RCVD_PKT_INFO_LIST_MAX_SZ; idx++)
   {
        if (entry_p->srcAddr != MAC_BROADCAST_SHORT_ADDR)
        {
            if (entry_p->tteSecs == 0x0)
                SYS_fatal(SYS_FATAL_ERR_47);
            else
            { 
                entry_p->tteSecs --;
                if (entry_p->tteSecs == 0x0)
                {
                    entry_p->srcAddr = MAC_BROADCAST_SHORT_ADDR;
                    entry_p->tteSecs = 0x0;
                    entry_p->lastRcvdDSN = MAC_INVALID_DSN;
                }
            }
        }

        entry_p ++;
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
UINT8_t MAC_checkDupPkt(MCPS_dataIndParams_s *rcvdPktInfo_p)
{
    UINT8_t rc = 0;

#ifdef DEV_TYPE_RFD
    if (rcvdPktInfo_p->dsn == MAC_cntxt.lastRcvdDSN)
    {
#ifdef MAC_STATS_ENA
        MAC_cntxt.stats.dupPktRcvdCnt ++;
#endif
        rc = 1;
    }
    else
    {
        MAC_cntxt.lastRcvdDSN = rcvdPktInfo_p->dsn;
    }
#else
    {
        UINT16_t idx, freeIdx = MAC_RCVD_PKT_INFO_LIST_MAX_SZ, 
                 minTTEIdx = MAC_RCVD_PKT_INFO_LIST_MAX_SZ, 
                 minTTE = MAC_RCVD_PDU_INFO_LIST_TTE_SECS + 1;
        MAC_rcvdPktInfo_s *entry_p = MAC_cntxt.rcvdPktInfoList;

        // TODO - Needs optimization ...
        for (idx=0; idx<MAC_RCVD_PKT_INFO_LIST_MAX_SZ; idx++)
        {
             if (freeIdx == MAC_RCVD_PKT_INFO_LIST_MAX_SZ)
             {
                 if (entry_p->srcAddr == MAC_BROADCAST_SHORT_ADDR)
                     freeIdx = idx;
                 else
                 {
                     // Keep track of oldest entry
                     if (entry_p->tteSecs < minTTE)
                     {
                         minTTEIdx = idx; 
                         minTTE = entry_p->tteSecs;
                     }
                 }
             }     

             if (entry_p->srcAddr == rcvdPktInfo_p->srcAddr.shortAddr)
             {
                 if (entry_p->lastRcvdDSN == rcvdPktInfo_p->dsn)
                 {
#ifdef MAC_STATS_ENA
                     MAC_cntxt.stats.dupPktRcvdCnt ++;
#endif
                     rc = 1;
                 }
                 else
                 {

                     entry_p->lastRcvdDSN = rcvdPktInfo_p->dsn;
                     entry_p->tteSecs = MAC_RCVD_PDU_INFO_LIST_TTE_SECS;
                 }

                 break;
            }        
            entry_p ++;
        }

        if (idx >= MAC_RCVD_PKT_INFO_LIST_MAX_SZ)
        {
            UINT16_t newIdx;

            // New short address.
            if (freeIdx < MAC_RCVD_PKT_INFO_LIST_MAX_SZ)
                newIdx = freeIdx;
            else
                newIdx = minTTEIdx;

            if (newIdx >= MAC_RCVD_PKT_INFO_LIST_MAX_SZ)
                SYS_fatal(SYS_FATAL_ERR_48);

            entry_p = MAC_cntxt.rcvdPktInfoList + newIdx;

            entry_p->srcAddr = rcvdPktInfo_p->srcAddr.shortAddr;
            entry_p->tteSecs = MAC_RCVD_PDU_INFO_LIST_TTE_SECS;
            entry_p->lastRcvdDSN = rcvdPktInfo_p->dsn;
        }
    }
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
void MAC_wakeUpProc(void)
{
    // Duplicate packet detection.
#ifdef DEV_TYPE_RFD
    // On wake up, skip duplicate packet detection on the first received
    // packet (from parent).
    MAC_cntxt.lastRcvdDSN = MAC_INVALID_DSN;
#endif
}

static MCPS_dataIndParams_s MAC_dataIndInfo;

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
   MAC_cntxt.stats.pduRcvdCnt ++;
#endif

#ifdef MAC_TRACE_ENA
   UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_12);
#endif

#ifdef FLASH_LED_ON_PKT_RX_TX
   PLTFRM_ON_LED_2();
#endif

   // BD buffer has a two bytes prefix.
   macPdu_p += (SYS_TRSCN_Q_HNDL_FIELD_LEN + SYS_TRSCN_TYPE_FIELD_LEN);
   macPduLen -= (SYS_TRSCN_Q_HNDL_FIELD_LEN + SYS_TRSCN_TYPE_FIELD_LEN);

   frameType = (*macPdu_p) & MAC_FC_FRAME_TYPE_BITS_SHIFT_MSK;

   // opti
   memset(&MAC_dataIndInfo, 0, sizeof(MCPS_dataIndParams_s));

   MAC_dataIndInfo.rssi = (SINT8_t)rssi;
   MAC_dataIndInfo.lqi = lqi;

#ifndef LPWMN_COORD
   if (MAC_cntxt.scanInProgress && frameType != MAC_FRAME_TYPE_BEACON)
   {
       // Only accepting beacons at this point
#ifdef FLASH_LED_ON_PKT_RX_TX
       PLTFRM_OFF_LED_2();
#endif
#ifdef MAC_STATS_ENA
       MAC_cntxt.stats.pduDropCnt_1 ++;
#endif

#ifdef MAC_TRACE_ENA
       UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_50);
#endif
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
      {
#ifdef FLASH_LED_ON_PKT_RX_TX
          PLTFRM_OFF_LED_2();
#endif
#ifdef MAC_STATS_ENA
          MAC_cntxt.stats.pduDropCnt_2 ++;
#endif

#ifdef MAC_TRACE_ENA
          UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_51);
#endif
          return 0;
      }
   }

   ackReqFlag = (macPdu_p[0] & MAC_FC_ACK_REQ_BIT_SHIFT_MSK) ? 0x1 : 0x0;

   hdrOff = MAC_PDU_HDR_FC_FIELD_LEN;

#if (MAC_PDU_HDR_SEQ_NR_FIELD_LEN == 4)
   MAC_dataIndInfo.dsn = UTIL_letoh32(macPdu_p + hdrOff);
#elif (MAC_PDU_HDR_SEQ_NR_FIELD_LEN == 2)
   MAC_dataIndInfo.dsn = UTIL_letoh16(macPdu_p + hdrOff);
#else
   MAC_dataIndInfo.dsn = macPdu_p[hdrOff];
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
           MAC_cntxt.stats.destLPWMNIdMisMatchCnt ++;
#endif
#ifdef FLASH_LED_ON_PKT_RX_TX
           PLTFRM_OFF_LED_2();
#endif
#ifdef MAC_STATS_ENA
           MAC_cntxt.stats.pduDropCnt_3 ++;
#endif

#ifdef MAC_TRACE_ENA
           UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_52);
#endif
           return 0;
       }

       if (destLPWMNId == MAC_BROADCAST_LPWMN_ID)
           MAC_dataIndInfo.destLPWMNBc = 0x1;
       else
           MAC_dataIndInfo.destLPWMNBc = 0x0;

       MAC_dataIndInfo.destLPWMNIdPresent = 0x1;
       MAC_dataIndInfo.destLPWMNId = destLPWMNId;

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
#ifdef MAC_TRACE_ENA
               UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_39);
#endif
#ifdef MAC_STATS_ENA
               MAC_cntxt.stats.destShortAddrMisMatchCnt ++;
#endif
#ifdef FLASH_LED_ON_PKT_RX_TX
               PLTFRM_OFF_LED_2();
#endif
#ifdef MAC_STATS_ENA
               MAC_cntxt.stats.pduDropCnt_4 ++;
#endif

#ifdef MAC_TRACE_ENA
               UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_53);
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
               MAC_cntxt.stats.destExtAddrMisMatchCnt ++;
#endif
#ifdef FLASH_LED_ON_PKT_RX_TX
               PLTFRM_OFF_LED_2();
#endif
#ifdef MAC_STATS_ENA
               MAC_cntxt.stats.pduDropCnt_5 ++;
#endif

#ifdef MAC_TRACE_ENA
               UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_54);
#endif
               return 0;
           }
           hdrOff += MAC_EXT_ADDR_LEN;
       }
   }
   MAC_dataIndInfo.destAddrMode = addrMode;

   MAC_dataIndInfo.srcAddrMode = MAC_ADDRESS_MODE_NO_ADDR;
   MAC_dataIndInfo.srcAddr.shortAddr = MAC_BROADCAST_SHORT_ADDR;
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
           MAC_dataIndInfo.srcLPWMNId = _lpwmnId;
           MAC_dataIndInfo.srcLPWMNIdPresent = 1;
           hdrOff += MAC_LPWMN_ID_LEN;
       }

       if (addrMode == MAC_ADDRESS_MODE_SHORT_ADDR)
       {
           UINT16_t shortAddr = macPdu_p[hdrOff + 1];
           shortAddr = (shortAddr << 8) | macPdu_p[hdrOff];

           // rkris@wisense.in / april/22/15 - source cannot be broadcast
           if (shortAddr == MAC_BROADCAST_SHORT_ADDR)
           {
#ifdef FLASH_LED_ON_PKT_RX_TX
               PLTFRM_OFF_LED_2();
#endif
#ifdef MAC_STATS_ENA
               MAC_cntxt.stats.pduDropCnt_12 ++;
#endif
               return 0;
           }

#ifdef DEV_TYPE_RFD
           // Accept messages from parent only
           if (MAC_PIB.macCoordShortAddress != MAC_BROADCAST_SHORT_ADDR)
           {
               if (shortAddr != MAC_PIB.macCoordShortAddress)
               {
#ifdef FLASH_LED_ON_PKT_RX_TX
                   PLTFRM_OFF_LED_2();
#endif
#ifdef MAC_STATS_ENA
                   MAC_cntxt.stats.pduDropCnt_6 ++;
#endif

#ifdef MAC_TRACE_ENA
                   UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_55);
#endif
                   return 0;
               }
           }
#endif
           MAC_dataIndInfo.srcAddr.shortAddr = shortAddr;

           srcShortAddrHdrOff = hdrOff;
           hdrOff += MAC_SHORT_ADDR_LEN;
       }
       else
       {
           memcpy(MAC_dataIndInfo.srcAddr.extAddr,
                  macPdu_p + hdrOff,
                  MAC_EXT_ADDR_LEN);
           hdrOff += MAC_EXT_ADDR_LEN;
       }
   }
   MAC_dataIndInfo.srcAddrMode = addrMode;


#ifndef DEV_TYPE_RFD
   if (MAC_dataIndInfo.srcAddrMode == MAC_ADDRESS_MODE_SHORT_ADDR)
   {
       UINT8_t isRFD;

       // Check if the source node is a child (RFD) of this node.
#ifdef LPWMN_COORD
       isRFD = NM_checkResetKATMO(MAC_dataIndInfo.srcAddr.shortAddr);
#else
       isRFD = MAC_checkResetRFDKATmo(MAC_dataIndInfo.srcAddr.shortAddr);
#endif

       /*
        * If a non-RFD receives a frame from an RFD with the ack-request flag set,
        * this non RFD will set the "frame pending" bit in the ack frame only if
        * there is no other indirect frame pending transmission.
        */
       if (isRFD && ackReqFlag
           && (MAC_cntxt.pendIndTrxDestAddr == MAC_BROADCAST_SHORT_ADDR))
       {
           // Check if there is frame pending transmission to this RFD.
           if (MAC_checkFrameIsPending(MAC_dataIndInfo.srcAddr.shortAddr))
               framePending = 1;
       }
   }
#endif


   if (frameType == MAC_FRAME_TYPE_CMD
       || frameType == MAC_FRAME_TYPE_DATA
       && (MAC_dataIndInfo.srcAddrMode == MAC_ADDRESS_MODE_SHORT_ADDR))
   {
       // Drop duplicate packets
       if (MAC_checkDupPkt(&MAC_dataIndInfo))
       {
#ifdef FLASH_LED_ON_PKT_RX_TX
           PLTFRM_OFF_LED_2();
#endif

#ifdef MAC_TRACE_ENA
           UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_13);
#endif
           // Send ack if required.
           if (ackReqFlag)
           {
               // rkris@wisense.in / March/1/15 - if this node is the
               // coordinator, check if sender is a registered node. When
               // coord is restarted quickly, neighbouring nodes should not
               // get acks for data packets. Instead they should rejoin the
               // network if they are RFDs. FFDs will anyway get the global
               // rejoin request broadcast message.
               // This problem is only valid for coordinators since they have
               // a fixed short address (0x1). FFDs on the other hand get a
               // new short address on restarting.
#ifdef LPWMN_COORD
               if (NM_lookUpShortAddr(MAC_dataIndInfo.srcAddr.shortAddr) == NULL)
               {
#ifdef MAC_STATS_ENA
                   MAC_cntxt.stats.pduDropCnt_11 ++;
#endif
                   return 0;  // Caller "will" free bdHndl.
               }
#endif

               // Schedule ack transmission
               MAC_schedAckTrx(&MAC_dataIndInfo, framePending);
           }
#ifdef MAC_STATS_ENA
           MAC_cntxt.stats.pduDropCnt_7 ++;
#endif
           return 0;  // Caller "will" free bdHndl.
       }

#ifdef DEV_TYPE_RFD
       if (MAC_cntxt.pollReqSent)
       {
           // This RFD has sent out a data request command and is waiting
           // for an ack. Instead it has received a cmd or data frame.
           // This means the parent received the data request command.
           // No point waiting for the ACK now.

           // Note that pollReqSent is set to 1 right before the CSMA procedure
           // is started. "txModState" is set to MAC_TX_MOD_STATE_WAIT_ACK only
           // after the "data-request cmd" is successfully sent out.

           MAC_freeCurrTxPkt(SYS_FATAL_ERR_605); // Bug fix - rkris, feb/21/15
           MAC_cntxt.expAckSrcShortAddr = MAC_BROADCAST_SHORT_ADDR; // Bug fix - rkris, feb/21/15
           MAC_ackRcvdHndlr(1);
       }
#endif
   }

#ifdef FFD_TRACK_ASSOC_RFD_RX_PARAMS
#ifdef DEV_TYPE_FFD
   {
       if (MAC_dataIndInfo.srcAddrMode == MAC_ADDRESS_MODE_SHORT_ADDR)
       {
           SINT8_t assocRFDListIdx = MAC_getAssocRFDListIdx(MAC_dataIndInfo.srcAddr.shortAddr);
           if (assocRFDListIdx > 0)
           {
               MAC_cntxt.assocRFDLastRcvdPktRSSIList[assocRFDListIdx] = (SINT8_t)rssi;
               MAC_cntxt.assocRFDLastRcvdPktLQIList[assocRFDListIdx] = (UINT8_t)lqi;
           }
       }
   }
#endif
#endif

   // macPdu_p is pointing at the frame control field
   switch (frameType)
   {
      case MAC_FRAME_TYPE_CMD:
           {
#ifdef MAC_STATS_ENA
              MAC_cntxt.stats.cmdFrameRcvdCnt ++;
#endif
              if (SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_MAC_DATA_INDICATION)
              {
#ifdef MAC_TRACE_ENA
                  UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_28);
#endif
                  // MAC_cntxt.rcvdPktInfo cannot be touched !!
                  rc = 0;
                  break;
              }

              memcpy(&(MAC_cntxt.rcvdPktInfo),
                     &(MAC_dataIndInfo),
                     sizeof(MCPS_dataIndParams_s));
#ifdef MAC_TRACE_ENA
              UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_14);
#endif
              MAC_cmdFrameHandler(macPdu_p + hdrOff,
                                  macPduLen - hdrOff,
                                  &framePending);

              if (ackReqFlag)
              {
                  // Schedule ack transmission
                  MAC_schedAckTrx(&(MAC_cntxt.rcvdPktInfo), framePending);
              }

              rc = 0;  // Caller will free memory
           }
           break;

      case MAC_FRAME_TYPE_ACK:
           {
#ifdef MAC_STATS_ENA
              MAC_cntxt.stats.ackRcvdCnt ++;
#endif
#ifdef MAC_TRACE_ENA
              UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_15);
#endif
              // Ack packet has source and destination short address and
              // at least one non broadcast lpwmn id. There is no payload.
              if (dstShortAddrHdrOff > 0x0 && srcShortAddrHdrOff > 0x0
                  && MAC_dataIndInfo.destLPWMNIdPresent == 0x1
                  && MAC_dataIndInfo.destLPWMNId == MAC_PIB.macLPWMNId)
              {
                  if (MAC_cntxt.txModState == MAC_TX_MOD_STATE_WAIT_ACK)
                  {
                      /*
                       * ACK frame MAC PDU format
                       * FCF (2 bytes) | DSN (1 or 4 bytes) | Remote-RSSI | Remote-LQI | RSSI | CORRELATION
                       */

                      if ((MAC_dataIndInfo.dsn == MAC_cntxt.expAckDSN)
                          && (MAC_cntxt.expAckSrcShortAddr == MAC_dataIndInfo.srcAddr.shortAddr))
                      {
                          // ACK has expected DSN
#ifdef DEV_TYPE_RFD
                          MAC_cntxt.txFlrCnt = 0x0;
#endif
#ifdef CC2500_TIMING_DBG_ENA
                          // P1OUT |= BIT7;
                          // PLTFRM_delay10MicroSecs();
                          // P1OUT &= ~BIT7;
#endif
                          // Free the packet since we have received an ACK for it.
                          MAC_freeCurrTxPkt(SYS_FATAL_ERR_604);

                          MAC_cntxt.expAckSrcShortAddr = MAC_BROADCAST_SHORT_ADDR;

#ifdef MAC_TRACE_ENA
                          UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_16);
#endif

                          MAC_ackRcvdHndlr((macPdu_p[0] & MAC_FC_PENDING_BIT_SHIFT_MSK) ? 0x1 : 0x0);
                      }
                      else
                      {
#ifdef MAC_STATS_ENA
                          MAC_cntxt.stats.badAckDSNCnt ++;
#endif
                      }
                  }
                  else
                  {
#ifdef MAC_STATS_ENA
                      MAC_cntxt.stats.unExpAckCnt ++;
#endif
                  }
              }
              else
              {
#ifdef MAC_STATS_ENA
                  MAC_cntxt.stats.ackSniffedCnt ++;
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

              if (MAC_dataIndInfo.srcAddrMode < MAC_ADDRESS_MODE_SHORT_ADDR)
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
                               if (entry_p->coordLPWMNId == MAC_dataIndInfo.srcLPWMNId)
                               {
                                   if (MAC_dataIndInfo.srcAddrMode == MAC_ADDRESS_MODE_SHORT_ADDR)
                                   {
                                       if (MAC_dataIndInfo.srcAddr.shortAddr == entry_p->coordAddress.shortAddr)
                                       {
                                           dupBcn = 1;
                                           break;
                                       }
                                       else
                                       {
                                           if (memcmp((void *)MAC_dataIndInfo.srcAddr.extAddr,
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
                           entry_p->coordLPWMNId = MAC_dataIndInfo.srcLPWMNId;
                           entry_p->coordAddrMode = MAC_dataIndInfo.srcAddrMode;
                           entry_p->rssi = rssi;

                           if (entry_p->coordAddrMode == MAC_ADDRESS_MODE_SHORT_ADDR)
                               entry_p->coordAddress.shortAddr = MAC_dataIndInfo.srcAddr.shortAddr;
                           else
                               memcpy(entry_p->coordAddress.extAddr,
                                      MAC_dataIndInfo.srcAddr.extAddr,
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
#ifdef CC2500_TIMING_DBG_ENA
               // P1OUT |= BIT6;
               // P1OUT &= ~BIT7;
#endif
               // rkris@wisense.in - april/22/15
               // Do not accept any data packets until this node gets a short address
               if (!(MAC_nodeJoined()))
               {
#ifdef MAC_STATS_ENA
                   MAC_cntxt.stats.pduDropCnt_13 ++;
#endif
#ifdef FLASH_LED_ON_PKT_RX_TX
                   PLTFRM_OFF_LED_2();
#endif
                   return 0; // Caller will free bdHndl
               }

               // Forward to upper layer ...
#ifdef MAC_STATS_ENA
               MAC_cntxt.stats.dataFrameRcvdCnt ++;
#endif
#ifdef MAC_TRACE_ENA
               UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_17);
#endif
#ifdef LPWMN_COORD
               // Drop this data frame if the sender is not a registered node.
               if (MAC_dataIndInfo.srcAddrMode == MAC_ADDRESS_MODE_SHORT_ADDR)
               {
                   if (NM_lookUpShortAddr(MAC_dataIndInfo.srcAddr.shortAddr) == NULL)
                   {
#ifdef MAC_STATS_ENA
                       MAC_cntxt.stats.unknownSrcAddrCnt ++;
#endif
#ifdef FLASH_LED_ON_PKT_RX_TX
                       PLTFRM_OFF_LED_2();
#endif
#ifdef MAC_STATS_ENA
                       MAC_cntxt.stats.pduDropCnt_8 ++;
#endif
                       return 0; // Caller will free bdHndl
                   }
               }
#endif
               // MAC data packet destination and source address must be short
               if (dstShortAddrHdrOff == 0x0 || srcShortAddrHdrOff == 0x0)
               {
#ifdef FLASH_LED_ON_PKT_RX_TX
                   PLTFRM_OFF_LED_2();
#endif
#ifdef MAC_STATS_ENA
                   MAC_cntxt.stats.pduDropCnt_9 ++;
#endif
                   return 0;  // Caller will free bdHndl
               }

#if !defined(DEV_TYPE_RFD)
               // If this packet is not for this node and this node has a route
               // to the packet's destination, reuse the MAC header
               if (ADP_pktFwdCheck(macPdu_p + hdrOff,
                                   macPdu_p + dstShortAddrHdrOff,
                                   macPdu_p + srcShortAddrHdrOff,
                                   macPduLen - hdrOff,
                                   bdHndl))
               {
#ifdef FLASH_LED_ON_PKT_RX_TX
                   PLTFRM_OFF_LED_2();
#endif

#ifdef MAC_TRACE_ENA
                  UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_56);
#endif
                   // Bug fix - Dec/16/14 (ack was not getting sent earlier)
                   if (ackReqFlag)
                   {
                       // Schedule ack transmission
                       MAC_schedAckTrx(&MAC_dataIndInfo, framePending);
                   }

                   return 1; // Caller will "not" free bdHndl.
               }
#endif

               if (SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_MAC_DATA_INDICATION)
               {
                   // APP has not consumed the previous data indication !!
                   // drop this packet.
#ifdef FLASH_LED_ON_PKT_RX_TX
                   PLTFRM_OFF_LED_2();
#endif
#ifdef MAC_STATS_ENA
                   MAC_cntxt.stats.pduDropCnt_10 ++;
#endif
#ifdef MAC_TRACE_ENA
                   UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_18);
#endif
                   return 0;  // Caller will free bdHndl.
               }

               MAC_dataIndInfo.rssi = rssi;
               MAC_dataIndInfo.lqi = lqi;
               MAC_dataIndInfo.bdHndl = bdHndl;

               // PLTFRM_LOG("<%s> d-pkt s:0x%x/l:%d/r:%d/c:%d>\n\r",
               //           __FUNCTION__, MAC_dataIndInfo.srcAddr.shortAddr,
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
               memcpy(&(MAC_cntxt.rcvdPktInfo),
                      &(MAC_dataIndInfo),
                      sizeof(MCPS_dataIndParams_s));

               SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_DATA_INDICATION;
#ifdef MAC_TRACE_ENA
               UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_19);
#endif
               if (ackReqFlag)
               {
                   // Schedule ack transmission
#ifdef MAC_STATS_ENA
                   MAC_cntxt.stats.ackSchedAttemptCnt ++;
#endif
                   MAC_schedAckTrx(&MAC_dataIndInfo, framePending);
               }
           }
           break;

      default:
           {
#ifdef MAC_STATS_ENA
               MAC_cntxt.stats.unknownFrameRcvdCnt ++;
#endif
               rc = 0;
           }
           break;
   }

#ifdef FLASH_LED_ON_PKT_RX_TX
   PLTFRM_OFF_LED_2();
#endif

   return rc;
}



#ifdef DEV_TYPE_RFD
#ifdef RFD_COORD_POLL_ENA
/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
MAC_sts_t MLME_pollRequest(void)
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


   if (MAC_cntxt.pollReqSent)
   {
       SYS_fatal(SYS_FATAL_ERR_49);
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
              SYS_fatal(SYS_FATAL_ERR_50);
          }

          MAC_cntxt.currTxPduHndl = pduInfo.bdHndl;

#ifdef MAC_STATS_ENA
          MAC_cntxt.stats.dataReqCmdTxCnt ++;
#endif

          // MAC_cntxt.pollReqTxAttempt = 1;

          MAC_csmaInit();
          MAC_doCsmaProc();
      }
   }
   
   if (sts == MAC_STS_SUCCESS)
       MAC_cntxt.pollReqSent = 1;

#if 0
   if (sts != MAC_STS_SUCCESS)
   {
       MAC_cntxt.mlmePollCnfrmParams.status = sts;
       SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_MLME_POLL_REQ_CONFIRM;
   }
#endif

   return sts;
}
// RFD_COORD_POLL_ENA
#endif
// DEV_TYPE_RFD
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
MAC_sts_t MAC_txRawPkt(UINT8_t bdHndl)
{
   MAC_sts_t sts = MAC_STS_SUCCESS;

   MAC_cntxt.ackRequired = 0;
   MAC_cntxt.txAttempCnt = 1;
   MAC_cntxt.macPduLen = SYS_GET_BD_LEN(bdHndl);
   MAC_cntxt.cnfrmReqd = 0x1;

   if (SYS_BD_HNDL_IS_VALID(MAC_cntxt.currTxPduHndl))
   {
       SYS_fatal(SYS_FATAL_ERR_51);
   }

   MAC_cntxt.currTxPduHndl = bdHndl;

#ifdef MAC_STATS_ENA
   MAC_cntxt.stats.dataReqCnt ++;
#endif

#ifdef MAC_TRACE_ENA
   UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_8);
#endif

   MAC_csmaInit();
   MAC_doCsmaProc();

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
void MCPS_dataReq(const MCPS_dataReqParams_s * const params_p)
{
   MAC_sts_t sts = MAC_STS_SUCCESS;
   MAC_pduInfo_s pduInfo;

#ifdef CC2500_TIMING_DBG_ENA
   // P1OUT |= BIT7;
#endif

#ifdef MAC_STATS_ENA
#ifdef RESET_MAC_STATS_BEFORE_DATA_REQ
   memset(&(MAC_cntxt.stats), 0, sizeof(MAC_stats_s));
#endif
#endif

#ifdef CC2500_TIMING_DBG_ENA
   // P1OUT &= ~BIT6;
   // P1OUT &= ~BIT7;
#endif

#ifdef MAC_TRACE_ENA
   UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_7);
#endif

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

#ifdef CC2500_TIMING_DBG_ENA
   // P1OUT |= BIT6;
#endif

   sts = MAC_buildPDU(&pduInfo, NULL, params_p->destShortAddr, params_p->indirectTx);
   if (sts != MAC_STS_SUCCESS)
   {
       // The MSDU has been freed at this point
       goto _end;
   }

#ifdef CC2500_TIMING_DBG_ENA
   // P1OUT &= ~BIT6;
#endif

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
           SYS_fatal(SYS_FATAL_ERR_51);
       }

       MAC_cntxt.currTxPduHndl = pduInfo.bdHndl;

#ifdef MAC_STATS_ENA
       MAC_cntxt.stats.dataReqCnt ++;
#endif

#ifdef MAC_TRACE_ENA
       UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_8);
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

#ifndef DEV_TYPE_SNIFFER


/*
 *******************************************************************
 *
 *
 *
 *
 *******************************************************************
 */
void MAC_buildSendNwkRestartCmd(void)
{
   MAC_pduInfo_s pduInfo;
   UINT8_t *hdr_p;
   UINT8_t offset = 0x0, macHdrLen;
   PLTFRM_sts_t sts;
   
   pduInfo.frameVersion = MAC_FRAME_VER_802_15_4;
   pduInfo.frameType = MAC_FRAME_TYPE_CMD;

   /*
    * The Source Addressing Mode field shall be set to indicate extended addressing.
    */
   pduInfo.sam = MAC_ADDRESS_MODE_SHORT_ADDR;
   pduInfo.dam = MAC_ADDRESS_MODE_SHORT_ADDR;

   pduInfo.secEna = 0x0;
   
   /*
    * The Frame Pending field shall be set to zero and ignored upon reception,
    * and the AR field shall be set to 1.
    */
   pduInfo.framePending = 0x0;
   
   pduInfo.ackRequired = 0x0;
   pduInfo.lpwmnIdComp = 0x1;
   
   macHdrLen = MAC_calcHdrLen(&pduInfo);
   pduInfo.bdHndl = SYS_allocMem(macHdrLen + MAC_NWK_RESTART_CMD_PDU_PYLD_LEN);
   if (SYS_BD_HNDL_IS_VALID(pduInfo.bdHndl) == 0x0)
   {
       SYS_fatal(SYS_FATAL_ERR_505);
   } 

   hdr_p = SYS_GET_BD_BUFF(pduInfo.bdHndl);

   offset = MAC_buildFrameCntrlHdr(hdr_p, &pduInfo);

   /*
    * The Sequence Number field shall contain the current value of macDSN.
    * macDSN is the sequence number added to the transmitted data or MAC
    * command frame.
    */
   pduInfo.seqNr = MAC_getNextDSN();
#if (MAC_PDU_HDR_SEQ_NR_FIELD_LEN == 4)
   UTIL_htole32(hdr_p + offset, pduInfo.seqNr);
#elif (MAC_PDU_HDR_SEQ_NR_FIELD_LEN == 2)
   UTIL_htole16(hdr_p + offset, pduInfo.seqNr);
#else
   hdr_p[offset] = pduInfo.seqNr;
#endif

   offset += MAC_PDU_HDR_SEQ_NR_FIELD_LEN;
   /*
    * The Destination LPWMN Identifier field shall contain the identifier of the
    * LPWMN to which to associate.
    */
   UTIL_htole16(hdr_p + offset, MAC_PIB.macLPWMNId);
   offset += MAC_LPWMN_ID_LEN;
       
   UTIL_htole16(hdr_p + offset, MAC_BROADCAST_SHORT_ADDR);
   offset += MAC_SHORT_ADDR_LEN;
   
   UTIL_htole16(hdr_p + offset, LPWMN_COORD_SHORT_ADDR);
   offset += MAC_SHORT_ADDR_LEN;
   
   hdr_p[offset ++] = MAC_CMD_NWK_RESTART;
   
   MAC_cntxt.ackRequired = 0;
   MAC_cntxt.txAttempCnt = 1;
   MAC_cntxt.macPduLen = macHdrLen + MAC_NWK_RESTART_CMD_PDU_PYLD_LEN;
   MAC_cntxt.cnfrmReqd = 0x0;

   sts = CC2500_forceIdle();
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_506);
  
   // Flush the transmit FIFO 
   sts = CC2500_sendCmdStrobe(CC2500_CMD_STROBE_SFTX, NULL);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_507);

   CC2500_transferPktToTxFifo(pduInfo.bdHndl);
   
   CC2500_cntxt.txStartedFlag = 0;
   
   sts = CC2500_sendCmdStrobe(CC2500_CMD_STROBE_STX, NULL);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_508);

   PLTFRM_delay1MilliSec( );
   
   if (CC2500_cntxt.txStartedFlag)
   {
       UINT16_t tmoMilliSecs, sec;
       UINT8_t MAC_nwkRestartCmdTxDone = 0;
       
       // CC2500 is configured to go to IDLE mode after packet transmission.

       CC2500_cntxt.txStartedFlag = 0;
       
       tmoMilliSecs = CC2500_calcPktTrxTime(SYS_GET_BD_LEN((pduInfo.bdHndl)));

       // TX->IDLE without calibration is .25/baud_Rate
       tmoMilliSecs += (CC2500_txToIdleTransitionTimeMilli);
#ifdef SMAC_TIMER_SRC_DBG_ENA
       __timerA0Src = 7;
       __timerA0Du = tmoMilliSecs;
#endif
       // The call back will not be called since we are not returning control
       // to system loop.
       if (PLTFRM_startTimerA0(tmoMilliSecs, 0, NULL) != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_509);

       MAC_nwkRestartCmdTxDone = 0;

       for (sec=0; sec<tmoMilliSecs + 10; sec++)
       {
            if (SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_TIMER_A0_EXPIRY)
            {
                SYS_globalEvtMsk0 &= ~(SYS_GLOBAL_EVT_TIMER_A0_EXPIRY);
                MAC_nwkRestartCmdTxDone = 1;
                break;
            }

            PLTFRM_delay1MilliSec( );
       }

       if (MAC_nwkRestartCmdTxDone == 0)
           SYS_fatal(SYS_FATAL_ERR_510);
   }
   else
   {
       // Transmission failed !! Should never happen since we were in IDLE state.
       SYS_fatal(SYS_FATAL_ERR_511);
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
#if (MAC_PDU_HDR_SEQ_NR_FIELD_LEN == 4)
   UTIL_htole32(hdr_p + offset, pduInfo.seqNr);
#elif (MAC_PDU_HDR_SEQ_NR_FIELD_LEN == 2)
   UTIL_htole16(hdr_p + offset, pduInfo.seqNr);
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

   hdr_p[offset ++] = MAC_CMD_ASSOCIATION_REQ;
   hdr_p[offset ++] = params_p->CapabilityInformation;
   UTIL_htons(hdr_p + offset, PLTFRM_savedSysFEId);
   offset += SYSTEM_FATAL_ERROR_ID_LEN;

   MAC_cntxt.ackRequired = pduInfo.ackRequired;
   MAC_cntxt.txAttempCnt = 1;
   MAC_cntxt.macPduLen = macHdrLen + MAC_ASSOC_REQ_CMD_PDU_PYLD_LEN;
   MAC_cntxt.cnfrmReqd = 0x1;

   if (SYS_BD_HNDL_IS_VALID(MAC_cntxt.currTxPduHndl))
       SYS_fatal(SYS_FATAL_ERR_52);

   MAC_cntxt.currTxPduHndl = pduInfo.bdHndl;

   MAC_csmaInit();
   MAC_doCsmaProc();

   return MAC_STS_SUCCESS;
}
// !LPWMN_COORD
#endif

// !DEV_TYPE_SNIFFER
#endif



// #define CC2500_RX_FIFO_DBG_ENA

#ifdef CC2500_RX_FIFO_DBG_ENA
typedef struct
{
   UINT16_t rxIntCnt;
   UINT16_t rxFIFOByteCnt;
}CC2500_rxFIFODbgArrEntry_s;

UINT16_t CC2500_rxFIFODbgArrIdx = 0;

#define CC2500_RX_FIFO_DBG_ARRAY_LEN  16

CC2500_rxFIFODbgArrEntry_s CC2500_rxFIFODbgArr[CC2500_RX_FIFO_DBG_ARRAY_LEN];
#endif


// UINT8_t MAC_pktRxBuff[CC2500_RX_FIFO_LEN];

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
void MAC_processRxFifo(void)
{
   UINT8_t rxFifoByteCnt, pktProcCnt = 0;
   PLTFRM_sts_t sts;

#ifdef CC2500_TIMING_DBG_ENA
   // P1OUT |= BIT6;
#endif
   /*
    * In receive mode, the packet handling support will de-construct the data 
    * packet by implementing the following (if enabled):
    *
    * - Preamble detection
    * - Sync word detection
    * - CRC computation and CRC check [enabled in this implementation]
    * - One byte address check [disabled in this implementation]
    * - Packet length check 
    *   (length byte checked against a programmable maximum length) [enabled in this implementation]
    * - De-whitening
    * - De-interleaving and decoding
    *
    * Note that when PKTCTRL1.APPEND_STATUS is enabled, the maximum allowed 
    * packet length is reduced by two bytes in order to make room in the RX 
    * FIFO for the two status bytes appended at the end of the packet.
    * The status bytes contain RSSI (first byte) and LQI values, as well as 
    * CRC OK.
    */

   /*
    * 15.3.2 Maximum Length Filtering
    * In variable packet length mode, PKTCTRL0.LENGTH_CONFIG=1, the PKTLEN.PACKET_LENGTH 
    * register value is used to set the maximum allowed packet length. If the received 
    * length byte has a larger value than this, the packet is discarded and receive mode 
    * restarted (regardless of the MCSM1.RXOFF_MODE setting).
    */

   /* 
    * If received packet fails CRC check and PKTCTRL1.CRC_AUTOFLUSH is set to 
    * 1, The CRC auto flush function will flush the entire RX FIFO and the CC110
    * will move to the state configured by the MCSM1.RXOFF_MODE setting.
    */

   // FIFO should have at least one good packet.

   /*
    * Note that the pkt-received-interrupt will occur only when a packet with good CRC 
    * is received.
    * CC2500 has been configured to go to RX mode on receiving a good or bad
    * packet.
    */

   // Two status bytes (see Table 23 and Table 24) with RSSI value, LQI, and
   // CRC status can be appended in the RX FIFO by setting PKT_CFG1.APPEND_STATUS
   // = 1.

   // Format of each frame stored in the RX FIFO is -
   //
   // > Length Field (1 byte) (value N)
   // > Payload (N bytes)
   // > RSSI (1 byte)
   // > CRC-OK (1 bit) | CORRELATION (7 bits)

   /*
    * When variable packet length mode is enabled, the first byte is the length byte.
    * The packet handler stores this value as the packet length and receives the number
    * of bytes indicated by the length byte.
    * If automatic CRC check is enabled, the packet handler computes CRC and matches it
    * with the appended CRC checksum.
    * At the end of the payload, the packet handler will optionally write two extra packet
    * status bytes (see Table 27 and Table 28) that contain CRC status, link quality ind,
    * and RSSI value.
    */
#ifdef CC2500_TIMING_DBG_ENA
   // P1OUT |= BIT7;
#endif
   sts = CC2500_getRXFIFOByteCnt(&rxFifoByteCnt);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_53);

   sts = CC2500_readRxFIFO(rxFifoByteCnt, MAC_pktRxBuff);
#ifdef CC2500_TIMING_DBG_ENA
   // P1OUT &= ~BIT7;
#endif
   if (sts == PLTFRM_STS_SUCCESS)
   {
       UINT16_t rxBuffOff = 0;

       while (rxFifoByteCnt > CC2500_FRAME_LENGTH_FIELD_LEN)
       {
           UINT8_t pyldLen = *(MAC_pktRxBuff + rxBuffOff);
           rxFifoByteCnt -= CC2500_FRAME_LENGTH_FIELD_LEN;
           rxBuffOff += CC2500_FRAME_LENGTH_FIELD_LEN;

#ifdef MAC_TRACE_ENA
           UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_41);
#endif

           if (pyldLen > RADIO_MAX_MAC_PDU_LEN)
           {
               // rkris@wisense.in - Jan/26/15
               // This condition was seen in a test setup with 10 RFDs sending
               // traffic to coordinator once every second. The "MAC_pktRxBuff"
               // contained more than 1 packet. The first packet was read out
               // ok but the length field for the second packet was missing !!.
               // The first packet was a 12 byte ack from the coord. The second
               // packet was a data packet from another RFD to the coord. Both
               // packets were completely received (except for the missing length
               // byte for the second packet).
#ifdef MAC_TRACE_ENA
               UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_40);
#endif

#ifdef MAC_STATS_ENA
               MAC_cntxt.badRxFIFOPyldLenCnt ++;
#endif
               // The FIFO may contain an incomplete packet. Flush it.

               // Change radio state to IDLE
               if (CC2500_forceIdle() != PLTFRM_STS_SUCCESS)
                   SYS_fatal(SYS_FATAL_ERR_541);

               // Flush command can only be issued in IDLE or RXFIFO_OVERFLOW state.
               if (CC2500_flushRxFIFO() != PLTFRM_STS_SUCCESS)
                   SYS_fatal(SYS_FATAL_ERR_542);

               // Put radio back into RX mode
               sts = CC2500_startRx();
               if (sts != PLTFRM_STS_SUCCESS)
                   SYS_fatal(SYS_FATAL_ERR_543);

               return;
           }

           if (rxFifoByteCnt >= (pyldLen
                                 + CC2500_APPENDED_RSSI_INFO_LEN
                                 + CC2500_APPENDED_CRC_LQI_INFO_LEN))
           {
               UINT8_t bdHndl;
               pktProcCnt ++;

#ifdef MAC_TRACE_ENA
               UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_62);
#endif

               bdHndl = SYS_allocMem(SYS_TRSCN_Q_HNDL_FIELD_LEN
                                     + SYS_TRSCN_TYPE_FIELD_LEN
                                     + pyldLen);
               if (SYS_BD_HNDL_IS_VALID(bdHndl))
               {
                   const UINT16_t off = SYS_TRSCN_Q_HNDL_FIELD_LEN + SYS_TRSCN_TYPE_FIELD_LEN;
                   SINT16_t rssi, lqiVal;

                   memcpy(SYS_GET_BD_BUFF(bdHndl) + off, MAC_pktRxBuff + rxBuffOff, pyldLen);

                   rxBuffOff += pyldLen;

#ifdef MAC_TRACE_ENA
                   UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_63);
#endif

                   /*
                    * If the PKTCTRL1.APPEND_STATUS bit is set, two status bytes containing
                    * information on CRC, LQI and RSSI will be written to the RXFIFO.
                    * RSSI byte - 8 bits
                    * CRC_OK_LQI byte - 1 bit CRC OK : 7 bits LQI
                    *
                    * The RSSI value is an estimate of the signal power level in the chosen
                    * channel. This value is based on the current gain setting in the RX chain
                    * and the measured signal level in the channel.
                    */

                   rssi = CC2500_calcPktRSSI(*(MAC_pktRxBuff + rxBuffOff));
                   rxBuffOff += CC2500_APPENDED_RSSI_INFO_LEN;
                   lqiVal = *(MAC_pktRxBuff + rxBuffOff);
                   lqiVal &= ~0x80;   // MSBit is CRC ok/not-ok bit
                   rxBuffOff += CC2500_APPENDED_CRC_LQI_INFO_LEN;
                   rxFifoByteCnt -= (pyldLen + CC2500_APPENDED_RSSI_INFO_LEN + CC2500_APPENDED_CRC_LQI_INFO_LEN);

#ifdef DEV_TYPE_SNIFFER
                   {
                      MCPS_dataIndParams_s dataIndParams;

                      SYS_ADJ_BD_HEAD(bdHndl,
                                      SYS_TRSCN_Q_HNDL_FIELD_LEN + SYS_TRSCN_TYPE_FIELD_LEN);

                      dataIndParams.rssi = rssi;
                      dataIndParams.lqi = lqiVal;
                      dataIndParams.bdHndl = bdHndl;

                      GW_relayMsgFromRemoteNode(MAC_rcvdPktSeqNr ++,
                                                &dataIndParams);
                   }
#else
#ifdef CC2500_TIMING_DBG_ENA
                   // P1OUT &= ~BIT6;
#endif
                   if (MAC_processRcvdPdu(bdHndl, rssi, lqiVal) == 0x0)
                   {
                       SYS_freeMem(bdHndl);
                   }
#endif
              }
              else
              {
#ifdef MAC_TRACE_ENA
                 UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_60);
#endif
                 // Packet buffer allocation failed !!
                 break;
              }
          }
          else
          {
#ifdef MAC_TRACE_ENA
              UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_61);
#endif
              // Partial packet (not the first one) ...
              break;
          }
       } // while loop
   }
   else
   {
       SYS_fatal(SYS_FATAL_ERR_55);
   }

#ifdef MAC_STATS_ENA
   if (pktProcCnt == 0)
       MAC_cntxt.MAC_emptyFifoCnt ++;
#endif

   return;
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
void MAC_processRxFifo(void)
{
   UINT8_t rxFifoByteCnt, pktProcCnt = 0;
   PLTFRM_sts_t sts;

#ifdef MAC_TRACE_ENA
   UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_101);
#endif

#ifdef CC2500_TIMING_DBG_ENA
   // P1OUT |= BIT6;
   // P1OUT &= ~BIT7;
#endif
   /*
    * In receive mode, the packet handling support will de-construct the data
    * packet by implementing the following (if enabled):
    *
    * - Preamble detection
    * - Sync word detection
    * - CRC computation and CRC check [enabled in this implementation]
    * - One byte address check [disabled in this implementation]
    * - Packet length check
    *   (length byte checked against a programmable maximum length) [enabled in this implementation]
    * - De-whitening
    * - De-interleaving and decoding
    *
    * Note that when PKTCTRL1.APPEND_STATUS is enabled, the maximum allowed
    * packet length is reduced by two bytes in order to make room in the RX
    * FIFO for the two status bytes appended at the end of the packet.
    * The status bytes contain RSSI (first byte) and LQI values, as well as
    * CRC OK.
    */

   /*
    * 15.3.2 Maximum Length Filtering
    * In variable packet length mode, PKTCTRL0.LENGTH_CONFIG=1, the PKTLEN.PACKET_LENGTH
    * register value is used to set the maximum allowed packet length. If the received
    * length byte has a larger value than this, the packet is discarded and receive mode
    * restarted (regardless of the MCSM1.RXOFF_MODE setting).
    */

   /*
    * If received packet fails CRC check and PKTCTRL1.CRC_AUTOFLUSH is set to
    * 1, The CRC auto flush function will flush the entire RX FIFO and the CC110
    * will move to the state configured by the MCSM1.RXOFF_MODE setting.
    */

   // FIFO should have at least one good packet.

   /*
    * Note that the pkt-received-interrupt will occur only when a packet with good CRC
    * is received.
    * CC2500 has been configured to go to RX mode on receiving a good or bad
    * packet.
    */

   // Two status bytes (see Table 23 and Table 24) with RSSI value, LQI, and
   // CRC status can be appended in the RX FIFO by setting PKT_CFG1.APPEND_STATUS
   // = 1.

   // Format of each frame stored in the RX FIFO is -
   //
   // > Length Field (1 byte) (value N)
   // > Payload (N bytes)
   // > RSSI (1 byte)
   // > CRC-OK (1 bit) | CORRELATION (7 bits)

   /*
    * When variable packet length mode is enabled, the first byte is the length byte.
    * The packet handler stores this value as the packet length and receives the number
    * of bytes indicated by the length byte.
    * If automatic CRC check is enabled, the packet handler computes CRC and matches it
    * with the appended CRC checksum.
    * At the end of the payload, the packet handler will optionally write two extra packet
    * status bytes (see Table 27 and Table 28) that contain CRC status, link quality ind,
    * and RSSI value.
    */

   do
   {
#ifndef CUSTOMER_ANDY_SPECIFIC_COORD_BUILD
      if (CC2500_cntxt.crcOkPktInRxFIFOCnt == 0)
      {
#ifdef MAC_STATS_ENA
          MAC_cntxt.stats.MAC_noPktInRxFIFOCnt ++;
#endif
#ifdef MAC_TRACE_ENA
          UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_100);
#endif
          break;
      }
#endif

      sts = CC2500_getRXFIFOByteCnt(&rxFifoByteCnt);
      if (sts != PLTFRM_STS_SUCCESS)
          SYS_fatal(SYS_FATAL_ERR_53);

#ifndef CUSTOMER_ANDY_SPECIFIC_COORD_BUILD
      if (rxFifoByteCnt == 0x0)
      {
          __bic_SR_register(GIE);
          CC2500_cntxt.crcOkPktInRxFIFOCnt = 0;
          __bis_SR_register(GIE);
      }
#endif

#ifdef MAC_TRACE_ENA
      UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_41);
#endif

#ifdef CC2500_RX_FIFO_DBG_ENA
      CC2500_rxFIFODbgArr[CC2500_rxFIFODbgArrIdx].rxIntCnt = CC2500_cntxt.crcOkPktRcvdIntCnt;
      CC2500_rxFIFODbgArr[CC2500_rxFIFODbgArrIdx].rxFIFOByteCnt = rxFifoByteCnt;
      CC2500_rxFIFODbgArrIdx ++;
      if (CC2500_rxFIFODbgArrIdx >= CC2500_RX_FIFO_DBG_ARRAY_LEN)
          CC2500_rxFIFODbgArrIdx = 0;
#endif

      if (rxFifoByteCnt > (CC2500_FRAME_LENGTH_FIELD_LEN
#ifdef CUSTOMER_ANDY_SPECIFIC_COORD_BUILD
                           + CC2500_FRAME_ADDRESS_FIELD_LEN
#endif
                           + CC2500_APPENDED_RSSI_INFO_LEN
                           + CC2500_APPENDED_CRC_LQI_INFO_LEN))
      {
          UINT8_t pyldLen;

#ifdef MAC_TRACE_ENA
          UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_60);
#endif

          sts = CC2500_readRxFIFO(CC2500_FRAME_LENGTH_FIELD_LEN, &pyldLen);
          if (sts == PLTFRM_STS_SUCCESS)
          {
              if (pyldLen == 0x0)
              {
#ifdef MAC_STATS_ENA
                  MAC_cntxt.stats.pyldLenZeroCnt ++;
#endif
                  CC2500_checkRadioErrors();

                  return;
              }

              rxFifoByteCnt -= CC2500_FRAME_LENGTH_FIELD_LEN;

              if (rxFifoByteCnt >= (pyldLen
                                    + CC2500_APPENDED_RSSI_INFO_LEN
                                    + CC2500_APPENDED_CRC_LQI_INFO_LEN))
              {
                  UINT8_t bdHndl;

                  rxFifoByteCnt -= (pyldLen
                                    + CC2500_APPENDED_RSSI_INFO_LEN
                                    + CC2500_APPENDED_CRC_LQI_INFO_LEN);

                  pktProcCnt ++;

#ifdef CUSTOMER_ANDY_SPECIFIC_COORD_BUILD
                  {
                     UINT8_t localAddr;
                     pyldLen -= CC2500_FRAME_ADDRESS_FIELD_LEN;
                     CC2500_readRxFIFO(CC2500_FRAME_ADDRESS_FIELD_LEN, &localAddr);
                  }
#endif

#ifdef MAC_TRACE_ENA
                  UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_61);
#endif

#ifdef MAC_STATS_ENA
                  MAC_cntxt.stats.pduRcvdCnt ++;
#endif

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
                      sts = CC2500_readRxFIFO(pyldLen, SYS_GET_BD_BUFF(bdHndl) + off);
                      if (sts == PLTFRM_STS_SUCCESS)
                      {
                          UINT8_t buff[2];
                          SINT16_t rssi, lqiVal;

                          /*
                           * If the PKTCTRL1.APPEND_STATUS bit is set, two status bytes containing
                           * information on CRC, LQI and RSSI will be written to the RXFIFO.
                           * RSSI byte - 8 bits
                           * CRC_OK_LQI byte - 1 bit CRC OK : 7 bits LQI
                           *
                           * The RSSI value is an estimate of the signal power level in the chosen
                           * channel. This value is based on the current gain setting in the RX chain
                           * and the measured signal level in the channel.
                           */
                          sts = CC2500_readRxFIFO(CC2500_APPENDED_RSSI_INFO_LEN
                                                  + CC2500_APPENDED_CRC_LQI_INFO_LEN,
                                                  (UINT8_t *)buff);
                          if (sts == PLTFRM_STS_SUCCESS)
                          {
                              rssi = CC2500_calcPktRSSI(buff[0]);

                              lqiVal = buff[1];
                              lqiVal &= ~0x80;   // MSBit is CRC ok/not-ok bit



#if defined(DEV_TYPE_SNIFFER) || defined(CUSTOMER_ANDY_SPECIFIC_COORD_BUILD)
                              {
                                  MCPS_dataIndParams_s dataIndParams;

                                  SYS_ADJ_BD_HEAD(bdHndl,
                                                  SYS_TRSCN_Q_HNDL_FIELD_LEN + SYS_TRSCN_TYPE_FIELD_LEN);

                                  dataIndParams.rssi = rssi;
                                  dataIndParams.lqi = lqiVal;
                                  dataIndParams.bdHndl = bdHndl;

                                  GW_relayMsgFromRemoteNode(MAC_rcvdPktSeqNr ++,
                                                            &dataIndParams);
                              }
#else
#ifdef CC2500_TIMING_DBG_ENA
                              // P1OUT &= ~BIT6;
                              // P1OUT |= BIT7;
#endif
                              if (MAC_processRcvdPdu(bdHndl, rssi, lqiVal) == 0x0)
                              {
                                  SYS_freeMem(bdHndl);
                              }
#endif
                          }
                      }
                  }
                  else
                  {
#ifdef MAC_TRACE_ENA
                      UTIL_AddTrace(&MAC_traceCntxt, MAC_TRACE_PT_60);
#endif

#ifdef MAC_STATS_ENA
                      MAC_cntxt.stats.rxFifoOOMCnt ++;
#endif
                      // Out of buffers !! Flush this packet.

                      pyldLen += (CC2500_APPENDED_RSSI_INFO_LEN
                                  + CC2500_APPENDED_CRC_LQI_INFO_LEN);

                      CC2500_readRxFIFO(pyldLen, NULL);

                      // for (idx=0; idx < pyldLen; idx++)
                      // {
                      //    sts = CC2500_readRxFIFO(1, &dummyByte);
                      //    if (sts != PLTFRM_STS_SUCCESS)
                      //        SYS_fatal(SYS_FATAL_ERR_54);
                      // }
                  }
#ifndef CUSTOMER_ANDY_SPECIFIC_COORD_BUILD
                  __bic_SR_register(GIE);
                  if (CC2500_cntxt.crcOkPktInRxFIFOCnt > 0)
                      CC2500_cntxt.crcOkPktInRxFIFOCnt --;
                  __bis_SR_register(GIE);
#endif
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
              SYS_fatal(SYS_FATAL_ERR_55);
          }
      }
      else
      {
           if (pktProcCnt < 1)
           {
               sts = PLTFRM_STS_RADIO_RX_FIFO_ERR;
#ifdef MAC_STATS_ENA
               MAC_cntxt.stats.MAC_emptyFifoCnt ++;
#endif
           }
           break;
      }
   } while (1);


#if 0
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_56);
#endif

   // <Note>
   // The "PKT_CRC_OK" signal gets de-asserted when RX mode is entered and
   // PKT_CFG1.CRC_CFG != 0.

   return;
}
#endif


#ifndef DEV_TYPE_SNIFFER
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

   /*
    * rkris@wisense.in - june/8/2015 - Non standard beacon.
    * Include routing cost to coordinator
    */
#ifdef DEV_TYPE_FFD
   buff_p[2] = AODVL_cntxt.coordRtCost;
   buff_p[3] = AODVL_cntxt.coordWLC;
#else
   buff_p[2] = 0;
   buff_p[3] = 0;
#endif

   if (MAC_buildPDU(&pduInfo, NULL, MAC_BROADCAST_SHORT_ADDR, 0) == MAC_STS_SUCCESS)
   {
       MAC_cntxt.txAttempCnt = 1;
       MAC_cntxt.cnfrmReqd = 0x0;
       MAC_cntxt.ackRequired = 0x0;
       MAC_cntxt.macPduLen = SYS_GET_BD_LEN(pduInfo.bdHndl);

       if (SYS_BD_HNDL_IS_VALID(MAC_cntxt.currTxPduHndl))
       {
           SYS_fatal(SYS_FATAL_ERR_57);
       }

       MAC_cntxt.currTxPduHndl = pduInfo.bdHndl;

#ifdef MAC_STATS_ENA
       MAC_cntxt.stats.bcnTxAttemptCnt ++;
#endif

       MAC_csmaInit();
       MAC_doCsmaProc();
   }

   return;
}
// !DEV_TYPE_RFD
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
   MAC_cntxt.stats.scanCleanUpCnt ++;
#endif

   MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE;

   // Turn the radio off (force it to IDLE mode)
   CC2500_forceIdle();

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

      *(buff_p ++) = MAC_CMD_BEACON_REQUEST;
      memcpy(buff_p, MAC_PIB.macExtendedAddress, MAC_EXT_ADDR_LEN);

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
          SYS_fatal(SYS_FATAL_ERR_58);
      }

      MAC_cntxt.currTxPduHndl = pduInfo.bdHndl;

#ifdef MAC_STATS_ENA
      MAC_cntxt.stats.bcnReqCmdTxCnt ++;
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

   if (MAC_cntxt.scanChan != PHY_2400_MHZ_BAND_CHAN_INV)
       channBit = (MAC_cntxt.scanChan - PHY_2400_MHZ_BAND_CHAN_1) + 1;
   /*
    * An active scan over a specified set of channels is requested using
    * the MLME-SCAN.request primitive with the ScanType parameter set to
    * indicate an active scan.
    */

    // Get the first requested channel
    while ((channBit < PHY_2400_MHZ_BAND_CHANNEL_CNT)
           && (MAC_cntxt.scanChannelsMsk & (1 << channBit)) == 0x0)
    {
        channBit ++;
    }

    if (channBit >= PHY_2400_MHZ_BAND_CHANNEL_CNT)
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
        CC2500_state_t cc2500State;

        MAC_cntxt.scanChan = channBit + PHY_2400_MHZ_BAND_CHAN_1;

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

        CC2500_getCurrState(&cc2500State);
        if (cc2500State != CC2500_STATE_IDLE)
        {
            if (CC2500_forceIdle()  != PLTFRM_STS_SUCCESS)
                SYS_fatal(SYS_FATAL_ERR_59);
        }

        // Flush the RX FIFO in case we have a beacon lying around in the RX FIFO
        // Flush the RX FIFO. Only issue SFRX in IDLE or RX_FIFO_ERR states.
        if (CC2500_sendCmdStrobe(CC2500_CMD_STROBE_SFRX, NULL) != PLTFRM_STS_SUCCESS)
            SYS_fatal(SYS_FATAL_ERR_60);

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
// !LPWMN_COORD
#endif
// !DEV_TYPE_SNIFFER
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
#ifndef DEV_TYPE_SNIFFER
        case SYS_GLOBAL_EVT_MAC_RETRY_CCA:
            {
               MAC_doCsmaProc();
            }
            break;

#ifndef DEV_TYPE_RFD
       case SYS_GLOBAL_EVT_MAC_BCN_REQUEST:
            {
               MAC_buildSendBeaconPdu();
            }
            break;
#endif
#endif

       case SYS_GLOBAL_EVT_MAC_FRAME_RX_DONE:
            {
               MAC_processRxFifo();
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


#ifndef DEV_TYPE_SNIFFER
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
                  (1 << (MAC_cntxt.scanChan - PHY_2400_MHZ_BAND_CHAN_1));

#ifdef MAC_STATS_ENA
   MAC_cntxt.stats.csmaFlrDuringScanCnt ++;
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
// !LPWMN_COORD
#endif


#ifndef LPWMN_COORD
#ifdef DEV_TYPE_FFD
#ifdef STATIC_ASSOC_ENA

#warning Enable For debugging only !!!

void MLME_staticAssoc(void)
{
   PLTFRM_sts_t sts;

   if (RADIO_setChan(0x1))
   {
       PHY_PIB.phyCurrentChannel = 0x1;
   }

   MAC_PIB.macLPWMNId = 0x5432;
   MAC_PIB.macCoordShortAddress = 0x1;
   MAC_PIB.macShortAddress = 0x2;

   sts = CC2500_startRx();
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_61);

   return;
}
// !STATIC_ASSOC_ENA
#endif
// !DEV_TYPE_FFD
#endif


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
   // Bug fix, rkris@wisense.in / april/18/2015
   if (RADIO_setChan(params_p->channelNumber) == PLTFRM_STS_SUCCESS)
   {
       PHY_PIB.phyCurrentChannel = params_p->channelNumber;

       PLTFRM_LOG("<%s> Radio channel set to <%d>\r\n",
                  __FUNCTION__, params_p->channelNumber);
   }
   else
   {
       SYS_fatal(SYS_FATAL_ERR_825);
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
       MAC_cntxt.lastRcvdDSN = MAC_INVALID_DSN;
   }
#endif

   return;
}
// !LPWMN_COORD
#endif


#ifdef DEV_TYPE_FFD



/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
SINT8_t MAC_getAssocRFDListIdx(const MAC_shortAddr_t shortAddr)
{
   SINT8_t idx;

   if (MAC_cntxt.assocRFDCnt == 0x0)
       return -1;  // Bug fix -  rkris@wisense.in - april/21/15

   for (idx=0; idx<MAC_MAX_ASSOC_RFD_CNT; idx++)
        if (MAC_cntxt.assocRFDList[idx] == shortAddr)
            break;

   return idx < MAC_MAX_ASSOC_RFD_CNT ? idx : -1;
}


#ifdef RFD_KA_ENA

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void MAC_checkRFDKATmo(void)
{
   if (MAC_cntxt.assocRFDCnt > 0)
   {
       UINT16_t idx;

       for (idx=0; idx<MAC_MAX_ASSOC_RFD_CNT; idx++)
       {
            if (MAC_cntxt.assocRFDList[idx] == MAC_BROADCAST_SHORT_ADDR)
                continue;

            if (MAC_cntxt.assocRFDKATTEList[idx] == 0x0)
                SYS_fatal(SYS_FATAL_ERR_873);

            MAC_cntxt.assocRFDKATTEList[idx] --;
            if (MAC_cntxt.assocRFDKATTEList[idx] == 0x0)
            {
                {
                   SYS_trscnQ_s *trscnQ_p = &(MAC_cntxt.assocRFDPendingFramesQList[idx]);
                   SYS_purgeTrscnsQ(trscnQ_p);
                }
                MAC_cntxt.assocRFDList[idx] = MAC_BROADCAST_SHORT_ADDR;
                MAC_cntxt.assocRFDKATTEList[idx] = 0;
                memset(&(MAC_cntxt.assocRFDExtAddrList[idx]), 0x0, MAC_EXT_ADDR_LEN);
                MAC_cntxt.assocRFDCnt --;
                if (MAC_cntxt.assocRFDCnt == 0)
                    break;
            }
       }
   }

   return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void MAC_procRFDKAMsg(UINT16_t srcShortAddr)
{
	SINT8_t idx = MAC_getAssocRFDListIdx(srcShortAddr);

	if (idx < 0)
        return;

	MAC_cntxt.assocRFDKATTEList[idx] = NM_cntxt.rfdKeepAliveTmoSecs;

	return;
}
#endif

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
UINT8_t MAC_lookUpAssocRFDList(const MAC_shortAddr_t shortAddr)
{
   // Bug fix - rkris@wisense.in - april/21/15
   return MAC_getAssocRFDListIdx(shortAddr) >= 0 ? 0x1 : 0x0;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
UINT8_t MAC_checkResetRFDKATmo(const MAC_shortAddr_t shortAddr)
{
   SINT8_t idx = MAC_getAssocRFDListIdx(shortAddr);
   if (idx >= 0)
       MAC_cntxt.assocRFDList[idx] = NM_cntxt.rfdKeepAliveTmoSecs;
   return idx >= 0 ? 0x1 : 0x0;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t MAC_lookUpExtAddrInAssocRFDList(const UINT8_t * const extAddr_p)
{
   UINT16_t idx;

   if (MAC_cntxt.assocRFDCnt == 0x0)
       return MAC_MAX_ASSOC_RFD_CNT;

   for (idx=0; idx<MAC_MAX_ASSOC_RFD_CNT; idx++)
   {
       if (memcmp(&(MAC_cntxt.assocRFDExtAddrList[idx]),
                  extAddr_p, MAC_EXT_ADDR_LEN) == 0)
       {
           break;
       }
   }

   return idx;
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
UINT8_t MAC_addToAssocRFDList(const MAC_shortAddr_t shortAddr,
                              UINT8_t *extAddr_p)
{

   // rkris@wisense.in - Jan/29/15 - Bug fix
   // check if this node is already in this list or not.

   // TODO - Get LPWMN Coord to send a message to FFD if one of it's children
   // re-associates through another FFD.

   if (MAC_cntxt.assocRFDCnt > 0)
   {
       UINT16_t idx = MAC_lookUpExtAddrInAssocRFDList(extAddr_p);
       if (idx < MAC_MAX_ASSOC_RFD_CNT)
       {
           MAC_cntxt.assocRFDList[idx] = shortAddr;
           return 1;
       }
   }

   if (MAC_cntxt.assocRFDCnt < MAC_MAX_ASSOC_RFD_CNT)
   {
       UINT16_t idx;

       for (idx=0; idx<MAC_MAX_ASSOC_RFD_CNT; idx++)
       {
           if (MAC_cntxt.assocRFDList[idx] == MAC_BROADCAST_SHORT_ADDR)
           {
               MAC_cntxt.assocRFDList[idx] = shortAddr;
#ifdef RFD_KA_ENA
               MAC_cntxt.assocRFDKATTEList[idx] = NM_cntxt.rfdKeepAliveTmoSecs;
#endif
               memcpy(&(MAC_cntxt.assocRFDExtAddrList[idx]), extAddr_p, MAC_EXT_ADDR_LEN);
               MAC_cntxt.assocRFDCnt ++;
               return 1;
           }
       }
   }

   return 0;
}

// DEV_TYPE_FFD
#endif



#ifdef LPWMN_COORD
#ifndef LPWMN_COORD_ASSOC_WHITE_LIST_DIS

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void MAC_clearAssocWhiteList(void)
{
   if (MAC_cntxt.assocWhiteList.whiteListBitMsk != 0x0)
   {
       MAC_cntxt.assocWhiteList.whiteListBitMsk = 0x0;
       memset(MAC_cntxt.assocWhiteList.extAddrArray,
              0x0, sizeof(MAC_cntxt.assocWhiteList.extAddrArray));
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
MAC_sts_t MAC_delNodeFromAssocWhiteList(UINT8_t *extAddr_p)
{
   MAC_sts_t sts = MAC_STS_SUCCESS;

   if (MAC_cntxt.assocWhiteList.whiteListBitMsk != 0x0)
   {
       UINT16_t idx;

       for (idx=0; idx<MAC_ASSOC_WHITE_LIST_MAX_ENTRY_CNT; idx++)
       {
          if ((MAC_cntxt.assocWhiteList.whiteListBitMsk & (1 << idx)))
          {
              UINT8_t *a_p = MAC_cntxt.assocWhiteList.extAddrArray + (idx*MAC_EXT_ADDR_LEN);
              if (memcmp(a_p, extAddr_p, MAC_EXT_ADDR_LEN) == 0x0)
              {
                  MAC_cntxt.assocWhiteList.whiteListBitMsk &= ~(1 << idx);
                  memset(a_p, 0x0, MAC_EXT_ADDR_LEN);
                  break;
              }
          }
       }

       if (idx >= MAC_ASSOC_WHITE_LIST_MAX_ENTRY_CNT)
           sts = MAC_STS_ENTRY_NOT_FOUND_IN_LIST;
   }
   else
       sts = MAC_STS_LIST_EMPTY;

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
UINT8_t MAC_lookUpAssocWhiteList(UINT8_t *extAddr_p)
{
   UINT8_t rc = 0;

   if (MAC_cntxt.assocWhiteList.whiteListBitMsk != 0x0)
   {
       UINT16_t idx;

       for (idx=0; idx<MAC_ASSOC_WHITE_LIST_MAX_ENTRY_CNT; idx++)
       {
          if ((MAC_cntxt.assocWhiteList.whiteListBitMsk & (1 << idx)))
          {
              UINT8_t *a_p = MAC_cntxt.assocWhiteList.extAddrArray + (idx*MAC_EXT_ADDR_LEN);
              if (memcmp(a_p, extAddr_p, MAC_EXT_ADDR_LEN) == 0x0)
              {
                  rc = 1;
                  break;
              }
          }
       }
   }
   else
      rc = 1;

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
MAC_sts_t MAC_addNodeToAssocWhiteList(UINT8_t *extAddr_p)
{
   MAC_sts_t sts = MAC_STS_SUCCESS;

   if (MAC_cntxt.assocWhiteList.whiteListBitMsk != 0xff)
   {
       UINT16_t idx;

       for (idx=0; idx<MAC_ASSOC_WHITE_LIST_MAX_ENTRY_CNT; idx++)
       {
          if ((MAC_cntxt.assocWhiteList.whiteListBitMsk & (1 << idx)) == 0x0)
          {
              MAC_cntxt.assocWhiteList.whiteListBitMsk |= (1 << idx);

              memcpy(MAC_cntxt.assocWhiteList.extAddrArray + (idx*MAC_EXT_ADDR_LEN),
                     extAddr_p, MAC_EXT_ADDR_LEN);
              break;
          }
       }
   }
   else
       sts = MAC_STS_LIST_FULL;

   return sts;
}
//
#endif
// LPWMN_COORD
#endif
// !DEV_TYPE_SNIFFER
#endif

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void MAC_checkPendAction(void)
{
   if (MAC_cntxt.pendingAction)
   {
	   if (MAC_cntxt.pendingAction & MAC_PENDING_ACTION_BIT_TX_ON_CCA_CTRL)
	   {
	       MAC_cntxt.pendingAction &= ~(MAC_PENDING_ACTION_BIT_TX_ON_CCA_CTRL);

	       if (MAC_cntxt.txOnCCAEna)
	           RADIO_enableTxOnCCA();
	       else
	           RADIO_disableTxOnCCA();
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
MAC_sts_t MAC_init(void)
{
   memset(&MAC_cntxt, 0, sizeof(MAC_cntxt_s));
   MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE;

   MAC_cntxt.txOnCCAEna = 1;

#ifdef MAC_TRACE_ENA
   UTIL_initTraceCntxt(&MAC_traceCntxt);
#endif

#ifdef LPWMN_COORD
   MAC_cntxt.acceptAssocReq = 1;
#endif

#ifndef DEV_TYPE_RFD
   MAC_cntxt.acceptBcnReqs = 1;
#endif

   MAC_PIB_init();

   MAC_cntxt.ackTmoExtraMilliSecs = 200;

#ifdef DEV_TYPE_FFD
   {
      UINT16_t idx;
      memset(MAC_cntxt.assocRFDList, 0xff,
             MAC_MAX_ASSOC_RFD_CNT*MAC_SHORT_ADDR_LEN);
      for (idx=0; idx<MAC_MAX_ASSOC_RFD_CNT; idx++)
      {
         SYS_initTrscnsQ(&(MAC_cntxt.assocRFDPendingFramesQList[idx]));
      }
   }
#endif

#ifndef DEV_TYPE_SNIFFER
   MAC_cntxt.currTxPduHndl = SYS_INV_BD_HDNL;

   MAC_cntxt.expAckSrcShortAddr = MAC_BROADCAST_SHORT_ADDR;

#ifdef LPWMN_COORD
#ifndef LPWMN_COORD_ASSOC_WHITE_LIST_DIS
   MAC_cntxt.assocWhiteList.whiteListBitMsk = 0x0;
#endif
#endif

   // Build the static ACK buffer
   MAC_buildFCInAckBuff();
#ifdef LPWMN_COORD
   MAC_updateSrcInAckBuff();
#endif

#ifdef DEV_TYPE_RFD
   MAC_cntxt.lastRcvdDSN = MAC_INVALID_DSN;
#else
   MAC_cntxt.pendIndTrxDestAddr = MAC_BROADCAST_SHORT_ADDR;
    
   MAC_initRcvdPktInfoList();
#endif

   MAC_ackPktTrxTimeMilliSecs = CC2500_calcPktTrxTime(sizeof(MAC_cntxt.ackTxBuff));

#endif

   return MAC_STS_SUCCESS;
}

#endif
#endif
