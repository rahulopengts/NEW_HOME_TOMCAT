/*
 * File Name : mac.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : 8/2/2013
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

#ifndef __MAC_H__
#define __MAC_H__

#include <typedefs.h>
#include <mac_defs.h>
#include <system.h>

#define MAC_RCVD_PDU_INFO_LIST_TTE_SECS  16   //  seconds
#define MAC_ASSOC_WHITE_LIST_MAX_ENTRY_CNT  8
#define MAC_ASSOC_BLACK_LIST_MAX_ENTRY_CNT  8

#define MAC_NEIGHBOR_TBL_ENTRY_CNT  8

typedef UINT16_t MAC_statsCntr_t;

#define MAC_TRACE_PT_1   1
#define MAC_TRACE_PT_2   2
#define MAC_TRACE_PT_3   3
#define MAC_TRACE_PT_4   4
#define MAC_TRACE_PT_5   5
#define MAC_TRACE_PT_6   6
#define MAC_TRACE_PT_7   7
#define MAC_TRACE_PT_8   8
#define MAC_TRACE_PT_9   9
#define MAC_TRACE_PT_10  10
#define MAC_TRACE_PT_11  11
#define MAC_TRACE_PT_12  12
#define MAC_TRACE_PT_13  13
#define MAC_TRACE_PT_14  14
#define MAC_TRACE_PT_15  15
#define MAC_TRACE_PT_16  16
#define MAC_TRACE_PT_17  17
#define MAC_TRACE_PT_18  18
#define MAC_TRACE_PT_19  19
#define MAC_TRACE_PT_20  20
#define MAC_TRACE_PT_21  21
#define MAC_TRACE_PT_22  22
#define MAC_TRACE_PT_23  23
#define MAC_TRACE_PT_24  24
#define MAC_TRACE_PT_25  25
#define MAC_TRACE_PT_26  26
#define MAC_TRACE_PT_27  27
#define MAC_TRACE_PT_28  28
#define MAC_TRACE_PT_29  29
#define MAC_TRACE_PT_30  30
#define MAC_TRACE_PT_31  31
#define MAC_TRACE_PT_32  32
#define MAC_TRACE_PT_33  33
#define MAC_TRACE_PT_34  34
#define MAC_TRACE_PT_35  35
#define MAC_TRACE_PT_36  36
#define MAC_TRACE_PT_37  37
#define MAC_TRACE_PT_38  38
#define MAC_TRACE_PT_39  39
#define MAC_TRACE_PT_40  40
#define MAC_TRACE_PT_41  41

#define MAC_TRACE_PT_50  50
#define MAC_TRACE_PT_51  51
#define MAC_TRACE_PT_52  52
#define MAC_TRACE_PT_53  53
#define MAC_TRACE_PT_54  54
#define MAC_TRACE_PT_55  55
#define MAC_TRACE_PT_56  56

#define MAC_TRACE_PT_60  60
#define MAC_TRACE_PT_61  61
#define MAC_TRACE_PT_62  62
#define MAC_TRACE_PT_63  63

#define MAC_TRACE_PT_100  100
#define MAC_TRACE_PT_101  101
#define MAC_TRACE_PT_102  102   // last
#define MAC_TRACE_PT_103  103
#define MAC_TRACE_PT_104  104
#define MAC_TRACE_PT_105  105

#define MAC_TRACE_PT_110 110
#define MAC_TRACE_PT_111 111
#define MAC_TRACE_PT_112 112
#define MAC_TRACE_PT_113 113
#define MAC_TRACE_PT_114 114
#define MAC_TRACE_PT_115 115
#define MAC_TRACE_PT_116 116
#define MAC_TRACE_PT_117 117


// rkris@wisense.in / june/6/15
#define MAC_PENDING_ACTION_BIT_TX_ON_CCA_CTRL  0x1


typedef enum
{
   MAC_STS_SUCCESS =               0x0,
   MAC_STS_TRANSACTION_OVERFLOW =  0x1,
   MAC_STS_TRANSCATION_EXPIRED =   0x2,
   MAC_STS_CHANNEL_ACCESS_FLR =    0x3,
   MAC_STS_INVALID_ADDRESS =       0x4,
   MAC_STS_INVALID_GTS =           0x5,
   MAC_STS_NO_ACK =                0x6,
   MAC_STS_COUNTER_ERROR =         0x7,
   MAC_STS_UNAVAILABLE_KEY =       0x8,
   MAC_STS_INVALID_PARAMETER =     0x9,
   MAC_STS_FRAME_TOO_LONG =        0xa,
   MAC_STS_NO_BEACON =             0xb,
   MAC_STS_SCAN_IN_PROGRESS =      0xc,
   MAC_STS_NO_DATA =               0xd,
   MAC_STS_OOM =                   0xe,
   MAC_STS_TX_PENDING =            0xf,
   MAC_STS_RADIO_HW_ERR =          0xc0,
   MAC_STS_BUSY =                  0xc1,
   MAC_STS_POLL_IN_PROGRESS =      0xc2,
   MAC_STS_LIST_FULL =             0xc3,
   MAC_STS_LIST_EMPTY =            0xc4,
   MAC_STS_ENTRY_NOT_FOUND_IN_LIST =  0xc5,
   MAC_STS_FRAME_PENDING           = 0xc6,
   MAC_STS_NO_FRAME_PENDING        = 0xc7,
} MAC_sts_t;


#if (MAC_PDU_HDR_SEQ_NR_FIELD_LEN == 4)
typedef UINT32_t MAC_dsn_t;
#elif (MAC_PDU_HDR_SEQ_NR_FIELD_LEN == 2)
typedef UINT16_t MAC_dsn_t;
#else
typedef UINT8_t MAC_dsn_t;
#endif

#define MAC_RCVD_PKT_INFO_LIST_MAX_SZ  32

typedef UINT16_t MAC_shortAddr_t;
typedef UINT16_t MAC_nwkId_t;

#define MAC_SHORT_ADDR_LEN  (sizeof(MAC_shortAddr_t))
#define MAC_LPWMN_ID_LEN  (sizeof(MAC_nwkId_t))

typedef struct
{
  UINT8_t _byte[MAC_EXT_ADDR_LEN];
} MAC_extAddr_t;

typedef union
{
#ifdef IEEE_802_15_4e  
  UINT8_t simple;  
#endif  
  MAC_shortAddr_t shortAddr;
  UINT8_t extAddr[MAC_EXT_ADDR_LEN];
} MAC_addr_s;


/*
 * Table 17 — Elements of LPWMNDescriptor.
 */
typedef struct
{
    /*
     * The coordinator addressing mode corresponding to the received beacon frame.
     * Allowed values (MAC_ADDRESS_TYPE_SHORT / MAC_ADDRESS_TYPE_EXTENDED)
     */
    UINT8_t coordAddrMode:2;   // If this is MAC_ADDRESS_TYPE_UNKNOWN, this entry
                               // is free.
    
    /*
     * Copy of the "LPWMN coordinator" bit from the recevied beacon's superframe
     * specification field.
     * The LPWMN Coordinator field shall be set to one if the beacon frame is being
     * transmitted by the LPWMN coordinator. Otherwise, the LPWMN Coordinator field
     * shall be set to zero.
     */
    UINT8_t isLPWMNCoord:1;
    
    /*
     * The current channel number occupied by the network.
     */
    UINT8_t channelNumber:5;   
   
    /*
     * The LQI at which the network beacon was received. Lower values
     * represent lower LQI, as defined in 8.2.6.
     */
#if 0    
    UINT8_t linkQuality;
#else
    SINT16_t rssi;
#endif
    
    /*
     * The LPWMN identifier of the coordinator as specified in the received beacon
     * frame.
     */
    MAC_nwkId_t coordLPWMNId;
    
    /*
     * The address of the coordinator as specified in the received beacon frame. 
     * Address format is as specified by the CoordAddrMode parameter.
     */
    UINT16_t coordShortAddress;
       
    // UINT16_t superFrameSpec;
} MAC_lpwmnDescriptor_s;

/*
 * 6.2.2.4 MLME-ASSOCIATE.confirm
 */
typedef struct
{
   UINT8_t status;
   MAC_shortAddr_t assocShortAddr;
   
#ifdef MAC_SEC_ENA
   SecurityLevel;
   KeyIdMode;
   KeySource;
   KeyIndex;
#endif
} MLME_assocCnfrmParams_s;

/*
 * 6.2.2.3 MLME-ASSOCIATE.response
 */
typedef struct
{   
   UINT8_t status;  
   UINT8_t deviceAddress[MAC_EXT_ADDR_LEN];
   MAC_shortAddr_t assocShortAddress;

#ifdef MAC_SEC_ENA
   SecurityLevel;
   KeyIdMode;
   KeySource;
   KeyIndex;
#endif
} MLME_assocResponseParams_s;

/*
 * Table 10 — MLME-ASSOCIATE.indication parameters
 */
typedef struct
{
   /*
    * The address of the device requesting association.
    */
   UINT8_t deviceAddress[MAC_EXT_ADDR_LEN];
   
   /*
    * The operational capabilities of the device requesting association.
    */
   UINT8_t CapabilitiyInformation;
   
#ifdef MAC_SEC_ENA
   SecurityLevel;
   KeyIdMode;
   KeySource;
   KeyIndex;
#endif   
} MLME_assocIndParams_s;


typedef struct
{
  /*
   * The channel number on which to attempt association.
   */
  UINT8_t channelNumber;
  
  /*
   * The identifier of the LPWMN with which to associate.
   */
  MAC_nwkId_t coordLPWMNId;
  
  /*
   * The address of the coordinator with which to associate.
   */
  UINT16_t coordShortAddress;
  
  /*
   * Specifies the operational capabilities of the associating device.
   */
  UINT8_t CapabilityInformation;
  
} MLME_assocRequestParams_s;


typedef struct
{
  UINT8_t status;
#ifdef DEV_TYPE_RFD
  UINT8_t framePending;
#endif
  MAC_shortAddr_t destShortAddr;
} MCPS_dataConfirmParams_s;

typedef struct
{
   UINT8_t sduPyldBdHndl;
   UINT8_t sduPyldLen;
} MAC_sduPyldInfo_s;

typedef struct
{
  UINT8_t srcAddrMode:2; 
  UINT8_t destAddrMode:2;
  UINT8_t ackTx:1;
  UINT8_t indirectTx:1;
  UINT8_t destLPWMNIdBc:1;
 
  // This implementation supports only 1 data request at a time.
  UINT8_t msduBDHndl;

#if 1
  // SDU passed to MAC has two parts. One is the ADP header and the other is the
  // ADP payload. The ADP header is in the buffer associated with "msduHdrBDHndl".
  // The ADP payload information is in sduPyldInfo. This structure supports ADP
  // level fragmentation. The "sduPyldLen" and "sduPyldOff" fields specify the
  // length and offset of the fragment passed to MAC for transmission.
  MAC_sduPyldInfo_s sduPyldInfo;
#endif

#ifdef MAC_SEC_ENA  
  keyIdMode;
  keySource;
  securityLevel;
  keyIndex;
#endif  
  
#if 1
  // Destination address can be short only
  MAC_shortAddr_t destShortAddr;
#else  
  MAC_addr_s destAddr;
#endif  
} MCPS_dataReqParams_s;

typedef struct
{
  UINT8_t srcAddrMode:2;
  UINT8_t destAddrMode:2;
  UINT8_t destLPWMNBc:1;
  UINT8_t srcLPWMNIdPresent:1;
  UINT8_t destLPWMNIdPresent:1;
  UINT8_t rejoinRequired:1;

  
  UINT8_t correlation;
  UINT8_t lqi;
  SINT8_t rssi;
  
  MAC_dsn_t dsn;

  MAC_nwkId_t srcLPWMNId;
  MAC_nwkId_t destLPWMNId;

  MAC_addr_s srcAddr;
  
#if 1   
  // Data packets can use short addresses
  MAC_shortAddr_t destShortAddr;
#else  
  MAC_addr_s destAddr;
#endif  

  UINT8_t bdHndl;
  
#ifdef MAC_SECURITY_ENA  
  UINT8_t securityLevel:3;
  UINT8_t keyIdMode:2;
  UINT8_t keyIndex;
#endif  
} MCPS_dataIndParams_s;

/*
 * Table 31 — MLME-SCAN.confirm parameters
 */
typedef struct
{
   UINT8_t status;
   
   /*
    * The number of elements returned in the appropriate result lists. This value 
    * is zero for the result of an orphan scan.
    */
   UINT8_t resultListSize;
   
   /*
    * A list of the channels given in the request which were not scanned.
    * If, during an active scan, the MLME is unable to transmit a beacon request 
    * command on a channel specified by the ScanChannels parameter due to a channel 
    * access failure, the channel will appear in the list of unscanned channels 
    * returned by the MLME-SCAN.confirm primitive.
    */
   UINT16_t unScannedChanBitMsk;
   
   /*
    * The list of LPWMN descriptors, one for each beacon found during an active or
    * passive scan if macAutoRequest is set to TRUE. This parameter is null for ED
    * and orphan scans or when macAutoRequest is set to FALSE during an active or
    * passive scan.
    */
   MAC_lpwmnDescriptor_s *lpwmnDescriptorList_p;
  
} MLME_scanCnfrmParams_s;


/*
 * Table 30 — MLME-SCAN.request parameters
 */
typedef struct
{
   // UINT8_t scanType;
   
   /*
    * The channel numbers to be scanned.
    * 1 bit for each of the 16 channels in the 2450 MHz band.
    * Least significant for channel 11.
    */
   UINT16_t scanChannelsMsk;
   
   /*
    * value used to calculate the length of time to spend scanning each channel 
    * for ED, active, and passive scans. This parameter is ignored for orphan 
    * scans. The time spent scanning each channel is [aBaseSuperframeDuration × 
    * (2n+ 1)], where 'n' is the value of the ScanDuration parameter.
    */
   UINT8_t scanDurationSecs;
   
   /*
    * The channel page on which to perform the scan.
    * Not implemented (2450 MhZ band has only one channel (channel 0).
    * UINT8_t ChannelPage;
    */
   
} MLME_scanRequestParams_s;
      
typedef void (*MCPS_dataIndCbFunc_t)(MCPS_dataIndParams_s *params_p);

typedef struct
{
  UINT8_t frameType:3;
  UINT8_t dam:2;
  UINT8_t sam:2;
  UINT8_t secEna:1;
  UINT8_t framePending:1;  
  UINT8_t ackRequired:1;
  UINT8_t lpwmnIdComp:1;
  UINT8_t frameVersion:2;
  UINT8_t rsvd:3;  
 
  MAC_dsn_t seqNr; 
  UINT8_t bdHndl;
  
  // UINT8_t *hdr_p;
  // UINT8_t *pyld_p;
} MAC_pduInfo_s;


typedef struct
{
  UINT8_t adpPduLen:7;
  UINT8_t ackReq:1;
  MAC_shortAddr_t destShortAddr;
  UINT8_t *adpPdu_p;   
} MAC_sigPktTxParams_s;


typedef struct
{
  UINT8_t numBackOffs:4;
  UINT8_t backOffExp:4; 
} MAC_csmaCntxt_s;

typedef enum
{
    MAC_TX_MOD_STATE_UNKNOWN,
    MAC_TX_MOD_STATE_IDLE,
    MAC_TX_MOD_STATE_CSMA_BACK_OFF,
    MAC_TX_MOD_STATE_RETRY_CSMA,
    MAC_TX_MOD_STATE_WAIT_TX_OVER,
    MAC_TX_MOD_STATE_WAIT_ACK,
    MAC_TX_MOD_STATE_IFS,
    MAC_TX_MOD_STATE_CONFIRM,
    MAC_TX_MOD_STATE_CHANN_SCAN
} MAC_txModState_t;

typedef enum
{
    MAC_STATE_UNINITIALIZED,
    MAC_STATE_INITIALIZED,
    MAC_STATE_ACTIVE_SCAN,
    MAC_STATE_ACTIVE,
    MAC_STATE_ERROR
} MAC_state_t;

typedef enum
{
   MAC_EVT_TIMER_EXPIRY,
   MAC_EVT_FRAME_RECEIVED,
   MAC_EVT_FRAME_TRANSMITTED
} MAC_eventId_t;

typedef struct 
{
   UINT16_t frameOff:14;
   UINT16_t addrType:1;  // 0 - short, 1 - extended
   UINT16_t mcpsMlmeFlag:1;  // 0 (MCPS-DATA.request), 1 (MLME-ASSOCIATE.response)
   UINT8_t frameLen;
} MAC_pendingFramesTblEntry_s;      

/*
 * Table 18 — MLME-COMM-STATUS.indication parameters
 */
typedef struct
{
#if 0
   /*
    * The LPWMN identifier of the device from which the frame was received or to
    * which the frame was being sent.
    */
   MAC_nwkId_t LPWMNId;
     
   UINT8_t SrcAddrMode;
   MAC_addr_s srcAddr;
#endif
   
   /*
    * The destination addressing mode for this primitive.
    */
   UINT8_t dstAddrMode;
   
   /*
    * The individual device address of the device for which the frame was intended.
    */
   MAC_addr_s dstAddr;
   
   /*
    * The communications status.
    */
   UINT8_t status;

#ifdef MAC_SEC_ENA
   SecurityLevel;
   KeyIdMode;
   KeySource;
   KeyIndex;
#endif
} MLME_commStsIndParams_s;

typedef struct
{
   UINT8_t addrMode;
   MAC_addr_s addr;
} MAC_rcvdDataReqCmdInfo_s;


typedef struct
{
   MAC_sts_t status;
} MLME_pollCnfrmParams_s;


/*
 * <DSN - data sequence number>
 * Each time a data or a MAC command frame is generated, the MAC sub-layer
 * shall copy the value of macDSN into the Sequence Number field of the MHR
 * of the outgoing frame and then increment it by one. Each device shall
 * generate exactly one DSN regardless of the number of unique devices with
 * which it wishes to communicate.
 *
 * On the receiver, the DSN can be used to detect duplicate packets.
 */
typedef struct
{
   MAC_dsn_t dataSeqNr;  // Received DSN
   UINT8_t timeToExpiry;  // In seconds
#ifndef DEV_TYPE_RFD
   MAC_shortAddr_t senderAddr;   // Sender's short address
#endif
} MAC_rcvdDSNInfo_s;
  

#if defined(DEV_TYPE_FFD) || defined(LPWMN_COORD)
typedef struct
{
   MAC_dsn_t lastRcvdDSN;
   UINT16_t srcAddr;
   UINT8_t tteSecs;  // Time to expiry in seconds
} MAC_rcvdPktInfo_s;
#endif

typedef struct
{
   UINT8_t whiteListBitMsk;
#if MAC_ASSOC_WHITE_LIST_MAX_ENTRY_CNT > 8
#error MAC_ASSOC_WHITE_LIST_MAX_ENTRY_CNT can be at most 8
#endif
   UINT8_t extAddrArray[MAC_EXT_ADDR_LEN * MAC_ASSOC_WHITE_LIST_MAX_ENTRY_CNT];
} MAC_assocWhiteList_s;

typedef struct
{
   UINT8_t blackListBitMsk;
#if MAC_ASSOC_BLACK_LIST_MAX_ENTRY_CNT > 8
#error MAC_ASSOC_BLACK_LIST_MAX_ENTRY_CNT can be at most 8
#endif
   UINT8_t extAddrArray[MAC_EXT_ADDR_LEN * MAC_ASSOC_BLACK_LIST_MAX_ENTRY_CNT];
} MAC_assocBlackList_s;


typedef struct
{
  MAC_statsCntr_t dataReqCnt;
  MAC_statsCntr_t csmaAttemptTxOnCcaCnt;
  MAC_statsCntr_t txStrobeCnt;
  MAC_statsCntr_t rcvdFrameCnt;
  MAC_statsCntr_t dataFrameRcvdCnt;
  MAC_statsCntr_t validAckRxCnt;
  MAC_statsCntr_t ccaOkCnt;
  MAC_statsCntr_t unExpFrameTxDoneEvtCnt;
#ifndef RADIO_CC1200
  // MAC_statsCntr_t goodFcsCnt;
  // MAC_statsCntr_t badFcsCnt;
#endif
  MAC_statsCntr_t frameTxDoneEvtCnt;
  MAC_statsCntr_t unknownFrameRcvdCnt;
  MAC_statsCntr_t badAckDSNCnt;
  MAC_statsCntr_t unExpAckCnt;
  MAC_statsCntr_t destShortAddrMisMatchCnt;
  MAC_statsCntr_t destLPWMNIdMisMatchCnt;
  MAC_statsCntr_t destExtAddrMisMatchCnt;
  MAC_statsCntr_t scanTmoCnt;
  MAC_statsCntr_t rxMemAllocFlrCnt;
  MAC_statsCntr_t rxErrRecoveryCnt;
  MAC_statsCntr_t rxOvflCnt;
  MAC_statsCntr_t supuriousRxOvflCnt;
  MAC_statsCntr_t MAC_emptyFifoCnt;
  MAC_statsCntr_t fwdPktTxReqCnt;
  MAC_statsCntr_t MAC_txOverTmoCnt;
  MAC_statsCntr_t scanCleanUpCnt;
  MAC_statsCntr_t scanReqCnt;
  MAC_statsCntr_t csmaFlrDuringScanCnt;
  MAC_statsCntr_t scanTimerStartedCnt;
  MAC_statsCntr_t bcnReqCmdTxCnt;
  MAC_statsCntr_t ackSniffedCnt;
  MAC_statsCntr_t pduRcvdCnt;
  MAC_statsCntr_t dupPktRcvdCnt;
  MAC_statsCntr_t cmdFrameRcvdCnt;
  MAC_statsCntr_t assocRespRxCnt;
  MAC_statsCntr_t bcnTxAttemptCnt;
  MAC_statsCntr_t channAccessFlrPoll;
  MAC_statsCntr_t immAckRetryCSMA;
  MAC_statsCntr_t immAckCnfrm;
  MAC_statsCntr_t unknownSrcAddrCnt;
  MAC_statsCntr_t pktRcvdWaitTxOverCnt;
  MAC_statsCntr_t abortTxCnt;
  MAC_statsCntr_t ackTxStrobeCnt;
  MAC_statsCntr_t ackRcvdCnt;
  MAC_statsCntr_t ackSchedAttemptCnt;
  MAC_statsCntr_t badRxFIFOPyldLenCnt;
  MAC_statsCntr_t pduDropCnt_1, pduDropCnt_2, pduDropCnt_3, pduDropCnt_4,
                  pduDropCnt_5, pduDropCnt_6, pduDropCnt_7, pduDropCnt_8,
                  pduDropCnt_9, pduDropCnt_10, pduDropCnt_11, pduDropCnt_12,
                  pduDropCnt_13, pduDropCnt_14;
  MAC_statsCntr_t pyldLenZeroCnt;
  MAC_statsCntr_t rejoinReqSent;
  MAC_statsCntr_t MAC_noPktInRxFIFOCnt;
  MAC_statsCntr_t reassocCheckCnt, reassocReqCnt;
#ifndef DEV_TYPE_RFD
  MAC_statsCntr_t pendingFrameTxCnt;
#endif

  MAC_statsCntr_t rxFifoOOMCnt;

#ifdef DEV_TYPE_RFD
  MAC_statsCntr_t dataReqCmdTxCnt;
  MAC_statsCntr_t rfdDropUnExpFrameCnt;
#endif

#if defined(RADIO_CC1200) || defined(RADIO_CC1101) || defined(RADIO_CC2500)
  MAC_statsCntr_t immAckInCsmaBackOffCnt;
  MAC_statsCntr_t immAckInWaitAckCnt;
  MAC_statsCntr_t immAckWhenIdle;
  MAC_statsCntr_t rxFifoOvflErrCnt;
  MAC_statsCntr_t txFifoOvflErrCnt;
  MAC_statsCntr_t rxFifoUnflErrCnt;
  MAC_statsCntr_t txFifoUnflErrCnt;
  MAC_statsCntr_t ccaDoneTmoCnt;
  // UINT16_t channCCAFlrCnt[PHY_865_867_MHZ_BAND_CHANNEL_CNT];
#endif

  UINT32_t lastBackOffDelay;

} MAC_stats_s;


typedef struct
{
   SINT8_t rssi;
   UINT8_t lqi;
   UINT16_t timeSinceLastRx;
   UINT16_t rxPktCnt;
   UINT16_t shortAddr;
} MAC_neighborTblEntry_s;


typedef struct
{
  MAC_txModState_t txModState;
  
  UINT8_t ackRequired:1;
  UINT8_t cnfrmReqd:1;
  UINT8_t txAttempCnt:5;
#ifdef LPWMN_COORD
  UINT8_t acceptAssocReq:1;
#elif defined(DEV_TYPE_RFD)
  UINT8_t postWakeUpPktRxFlag:1;
#else
  UINT8_t rsvd_1:1;
#endif

  UINT8_t macPduLen;
  MAC_dsn_t expAckDSN;

#ifndef LPWMN_COORD  
  UINT8_t bcnRcvdCnt;
  UINT8_t scanDurationSecs;
  UINT8_t scanType:2;
  UINT8_t scanInProgress:1;
  UINT8_t uniqueBcnRcvdCnt:5;
#endif
  
  UINT8_t scanChan:5;
  UINT8_t assocReqInProgress:1;  // Used on node which is trying to associate

#ifdef DEV_TYPE_RFD
  UINT8_t pollReqSent:1;
#else
  UINT8_t pendingFrameTxInProgress:1;
#endif
  UINT8_t macRxFrameBuffBusy:1;

#ifdef DEV_TYPE_RFD
  UINT8_t pollTimerStarted:1;
  UINT8_t pollReqTxAttempt:1;
  UINT8_t rsvd_1:6;
#endif

#if defined(DEV_TYPE_RFD) || defined(DEV_TYPE_FFD)
  UINT16_t preConfLPWMNId;
#endif

#if defined(RADIO_CC1200) || defined(RADIO_CC1101) || defined(RADIO_CC2500)
  UINT8_t currTxPduHndl;
  UINT8_t immAckTxInProgress:1;
  UINT8_t backOffInterrupted:1;
  UINT8_t ackRxPending:1;
  UINT8_t csmaRetryInterrupted:1;
  UINT8_t confirmInterrupted:1;
  UINT8_t acceptBcnReqs:1;  // Only applicable to Coord and FFDs
  UINT8_t txOnCCAEna:1;
  UINT8_t rsvd_2:1;
  MAC_shortAddr_t expAckSrcShortAddr;
  // SYS_trscnQ_s pendingAckTxTrscnQ;
  UINT8_t ackTxBuff[MAC_SIM_802_15_4_ACK_PDU_LEN];
#endif

  UINT8_t pendingAction;  // rkris@wisense.in / june/6/15

#ifndef LPWMN_COORD  
  MAC_nwkId_t prevLPWMNId;
  UINT16_t scanChannelsMsk;
  UINT16_t unScannedChanBitMsk;
#endif

  UINT16_t macAckTxCnt;

#if defined(DEV_TYPE_FFD) || defined(LPWMN_COORD)
  UINT16_t bcnReqRcvdCnt;
  UINT16_t timeSinceLastBncReq;
  UINT16_t bcnReqSrcExtAddr;   // least significant 2 bytes
  UINT16_t assocReqRcvdCnt;
  UINT16_t assocRespTxCnt;
#endif
  
  MAC_csmaCntxt_s csmaCntxt;
  
  MCPS_dataIndParams_s rcvdPktInfo;
  
#ifndef LPWMN_COORD  
  MLME_assocCnfrmParams_s assocCnfrmParams;
  MLME_scanCnfrmParams_s scanCnfrmParams;
#endif  
  
  MCPS_dataConfirmParams_s datacnfrmParams;

#ifdef DEV_TYPE_RFD
  MLME_pollCnfrmParams_s mlmePollCnfrmParams;
  UINT8_t txFlrCnt;
#endif

  // MAC_rcvdDataReqCmdInfo_s rcvdDataReqCmdInfo;
  // MLME_commStsIndParams_s commStsIndParams;
  
#ifndef LPWMN_COORD    
  MAC_lpwmnDescriptor_s lpwmnDescriptorList[MAC_LPWMN_DESCRIPTOR_LIST_MAX_SIZE];
#endif

#ifdef DEV_TYPE_FFD
  UINT8_t assocRFDCnt;
  MAC_shortAddr_t assocRFDList[MAC_MAX_ASSOC_RFD_CNT];
  MAC_extAddr_t assocRFDExtAddrList[MAC_MAX_ASSOC_RFD_CNT];
  SYS_trscnQ_s assocRFDPendingFramesQList[MAC_MAX_ASSOC_RFD_CNT];
  UINT16_t assocRFDKATTEList[MAC_MAX_ASSOC_RFD_CNT];
#ifdef FFD_TRACK_ASSOC_RFD_RX_PARAMS
  SINT8_t assocRFDLastRcvdPktRSSIList[MAC_MAX_ASSOC_RFD_CNT];
  SINT8_t assocRFDLastRcvdPktLQIList[MAC_MAX_ASSOC_RFD_CNT];
#endif
#endif

  UINT16_t ackTmoExtraMilliSecs;

#ifndef DEV_TYPE_RFD
  UINT8_t timeSinceRFDDataReq;
  MAC_shortAddr_t pendIndTrxDestAddr;
#endif

#ifdef DEV_TYPE_RFD
  MAC_dsn_t lastRcvdDSN;
#else
  MAC_rcvdDSNInfo_s rcvdDSNInfoList[MAC_RCVD_DSN_INFO_LIST_SZ];
#endif

#ifndef DEV_TYPE_RFD
  // MAC_pendingFramesTblEntry_s pendingFramesIdxTbl[MAC_PENDING_FRAMES_IDX_TBL_SZ];
  // UINT8_t pendingFramesBuff[MAC_PENDING_FRAMES_BUFF_LEN];
#endif

#ifdef LPWMN_COORD
#ifndef LPWMN_COORD_ASSOC_WHITE_LIST_DIS
  MAC_assocWhiteList_s assocWhiteList;
#endif
#ifndef LPWMN_COORD_ASSOC_BLACK_LIST_DIS
  MAC_assocBlackList_s assocBlackList;
#endif
#endif

#if defined(DEV_TYPE_FFD) || defined(LPWMN_COORD)
  MAC_rcvdPktInfo_s rcvdPktInfoList[MAC_RCVD_PKT_INFO_LIST_MAX_SZ];
#endif

  UINT16_t ackPktTrxTimeMilliSecs;

  // May/4/15 - rkris@wisense.in
  // These can be retrieved remotely
  UINT32_t csmaFlrCnt;
  UINT32_t ccaFlrCnt;
  UINT32_t ackTmoCnt;

  // Feb/21/16 - rkris@wisense.in
  UINT16_t badUCSAVerifCnt;
  UINT16_t goodUCSAVerifCnt;

#ifdef MAC_NEIGHBOR_TBL_ENA
  MAC_neighborTblEntry_s neighborTbl[MAC_NEIGHBOR_TBL_ENTRY_CNT];
#endif

#ifdef MAC_STATS_ENA
  MAC_stats_s stats;
#endif

} MAC_cntxt_s;

extern MAC_cntxt_s MAC_cntxt;
       
#ifdef CC1101_WOR_ENA
/*
 * In WOR mode, do not try to transmit if CC1101 has reported that
 * it has received a SYNC word.
 */
#define MAC_TX_MODULE_IS_IDLE() \
	    ((MAC_cntxt.txModState == MAC_TX_MOD_STATE_IDLE) && \
         (CC1101_cntxt.syncRcvdFlag == 0))
#else
#define MAC_TX_MODULE_IS_IDLE() \
	    (MAC_cntxt.txModState == MAC_TX_MOD_STATE_IDLE)
#endif

#define MAC_TX_MODULE_CAN_RETRY_CSMA() (MAC_cntxt.txModState == MAC_TX_MOD_STATE_RETRY_CSMA)
#define MAC_TX_MODULE_CAN_CONFIRM() (MAC_cntxt.txModState == MAC_TX_MOD_STATE_CONFIRM)

#define MAC_resetTxModState( )  \
 do \
 {  \
     MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE; \
 } while (0)
   

#define MAC_allowNwkJoins( )  \
do \
{  \
  MAC_cntxt.acceptAssocReq = 1; \
} while(0)

#define MAC_disAllowNwkJoins( )  \
do \
{  \
  MAC_cntxt.acceptAssocReq = 0; \
} while(0)

#define MAC_cfgDropBcnReqs( ) \
do \
{  \
  MAC_cntxt.acceptBcnReqs = 0; \
} while(0)

#define MAC_cfgAcceptBcnReqs( ) \
do \
{  \
  MAC_cntxt.acceptBcnReqs = 1; \
} while(0)

#define MAC_nwkJoinsAllowed( )  (MAC_cntxt.acceptAssocReq != 0x0)


extern void MLME_scanRequest(MLME_scanRequestParams_s *params_p);

extern void MLME_assocRequest(MLME_assocRequestParams_s *params_p);
  
extern MAC_sts_t MAC_init(void);

extern MAC_sts_t MAC_sendBcnRequest(void);

extern MAC_sts_t MAC_sendAssocnReqCmd(UINT8_t capInfo,
                                      UINT8_t destAddrMode,
                                      MAC_nwkId_t destLPWMNId,
                                      MAC_addr_s *destAddr_p);

extern void MAC_evtHndlr(const UINT16_t globalEvtId);

extern UINT8_t MAC_txPduHdrBuff[ ];

extern void MAC_doActiveScan(void);

extern void MAC_cleanUpScanProc(void);

extern void MCPS_dataReq(const MCPS_dataReqParams_s * const params_p);

extern void MAC_sigPktTxReq(MAC_sigPktTxParams_s *params_p);

extern void MAC_rxOvflProc(void);

extern void MAC_sendAdpFwdPkt(UINT8_t bdHndl, UINT8_t pktIsBroadCast);

extern void MAC_checkPendingAckTrxQ(void);

extern MAC_shortAddr_t MAC_getNextHop(const UINT8_t bdHndl);

extern void MLME_assocResponse(void);

extern MAC_sts_t MAC_buildSendAssocnRespCmd(const UINT8_t * const extAddr_p,
                                            const UINT8_t status,
                                            const MAC_shortAddr_t allocShortAddr);

extern void MAC_buildSigPktMacHdr(const MAC_shortAddr_t destAddr, const UINT16_t sduLen);

extern void MAC_startSigPktTx(void);

extern UINT8_t MAC_lookUpAssocRFDList(const MAC_shortAddr_t shortAddr);

extern UINT8_t MAC_addToAssocRFDList(const MAC_shortAddr_t shortAddr, UINT8_t *extAddr_p);

extern MAC_sts_t MLME_pollRequest(void);

extern UINT8_t MAC_calcHdrLen(const MAC_pduInfo_s * const pduInfo_p);

extern UINT8_t MAC_coordPollProc(void);

extern void MAC_sendPendFrame(void);

#ifdef LPWMN_COORD
#ifndef LPWMN_COORD_ASSOC_WHITE_LIST_DIS
extern MAC_sts_t MAC_addNodeToAssocWhiteList(UINT8_t *extAddr_p);
extern MAC_sts_t MAC_delNodeFromAssocWhiteList(UINT8_t *extAddr_p);
extern void MAC_clearAssocWhiteList(void);
extern UINT8_t MAC_lookUpAssocWhiteList(UINT8_t *extAddr_p);
#endif

#ifndef LPWMN_COORD_ASSOC_BACK_LIST_DIS
extern MAC_sts_t MAC_addNodeToAssocBlackList(UINT8_t *extAddr_p);
extern MAC_sts_t MAC_delNodeFromAssocBlackList(UINT8_t *extAddr_p);
extern void MAC_clearAssocBlackList(void);
extern UINT8_t MAC_lookUpAssocBlackList(UINT8_t *extAddr_p);
#endif
#endif

#ifdef DEV_TYPE_FFD
extern UINT8_t MAC_lookUpExtAddrInAssocRFDList(const UINT8_t * const extAddr_p);
extern void MAC_purgeRFDPendFrames(UINT8_t assocRFDListIdx);
#endif

#ifdef DEV_TYPE_RFD
extern void MAC_wakeUpProc(void);
#endif

#if defined(DEV_TYPE_FFD) || defined(LPWMN_COORD)
void MAC_checkRcvdPktInfoList(void);
#endif

extern void MAC_buildSendNwkRestartCmd(void);

// rkris@wisense.in / jan/21/15
extern MAC_sts_t MAC_txRawPkt(UINT8_t bdHndl);

extern UINT8_t MAC_addFrameToPendQ(MAC_shortAddr_t shortAddr, UINT8_t pdubdHndl);

#ifdef RFD_KA_ENA
extern void MAC_checkRFDKATmo(void);
#endif

extern void MAC_updateCfgInfo(UINT8_t tlvId, UINT8_t tlvValLen, UINT8_t *val_p);
extern void MAC_readCfgInfo(void);
extern void MAC_getNeighborTblEntry(MAC_neighborTblEntry_s *entry_p);
extern void MAC_updNeighborTblPktTS(UINT8_t interval);
extern void MAC_checkPendAction(void);

// Feb/21/16 - rkris@wisense.in
extern UINT8_t MAC_verifyUCSA(UINT16_t shortAddr);

#endif

