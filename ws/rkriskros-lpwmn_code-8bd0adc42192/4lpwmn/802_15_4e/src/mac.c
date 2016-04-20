/*
 * File Name: mac.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: 8/2/2013
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

#include <string.h>
#include <pltfrm.h>
#include <mac.h>
#include <mac_pib.h>
#include <mlme.h>
#include <radio.h>
#include <phy_defs.h>
#include <phy_pib.h>
#include <system.h>
#if !defined(DEV_TYPE_RFD) && !defined(DEV_TYPE_FFD_NO_MESH) && !defined(LPWMN_COORD_NO_MESH)
#include <aodvl.h>
#endif
#include <util.h>
#include <pltfrm_log.h>

// Remove -------------------------
#include <node_mgr.h>



#define CSMA_TEST 


MAC_cntxt_s MAC_cntxt;

void MAC_doCsmaProc(UINT8_t ifsType);
void MAC_csmaInit(void);
void MAC_handleCsmaFlrDuringScan(void);
void MAC_doActiveScan(void);
void MAC_sendAssocCnfrm(UINT8_t sts, UINT16_t allocShortAddr);
void MAC_buildSendBeaconPdu(void);


extern void ADP_macAssocIndHndlr(UINT8_t *srcExtAddr_p, UINT8_t capInfo);


extern UINT8_t ADP_pktFwdCheck(UINT8_t *adpPdu_p,
                               UINT8_t *dstShort_p,  // Points to dest short in the MAC header
                               UINT8_t *srcShort_p,
                               UINT8_t adpPduLen,
                               UINT8_t bdHndl);

/*
 * 5.1.1.5.2  Absolute Slot Number (ASN)
 *
 * The total number of timeslots that has elapsed since the start 
 * of the network or an arbitrary start time determined by the LPWMN
 * coordinator is called the Absolute Slot Number (ASN). 
 * It increments globally in the network every macTsTimeslotLength 
 * microseconds (refer to Table 52e). It may be beaconed by devices 
 * already in a TSCH LPWMN, allowing new devices to synchronize. It is
 * used globally by devices in a TSCH LPWMN as the frame counter (thus
 * allowing for time-dependent security) and is used to compute the 
 * channel for any given pairwise communication as described in 
 * 5.1.1.5.3.
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
void MAC_sendDataCnfrm(void)
{   
   if (MAC_cntxt.cnfrmReqd)
   {
       MAC_cntxt.txModState = MAC_TX_MODE_STATE_CONFIRM;
      
       // MAC_cntxt.datacnfrmParams.msduHandle = MAC_cntxt.dataReqMsduHandle;
#ifdef DEV_TYPE_RFD
       if (MAC_cntxt.pollReqSent)
       {
           SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_MLME_POLL_REQ_CONFIRM;
           MAC_cntxt.pollReqSent = 0;
       }
       else
#endif
           SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_DATA_CONFIRM;
   }
   else
   {
       MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE;
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
#if 0
void MAC_updateASN(UINT16_t incVal)
{
   MAC_PIB.macASN.u64Val += incVal;
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
MAC_sts_t MAC_buildEnhancedBcnMsg(   )
{
   /*
    * TSCH uses EBs containing the TSCH Synchronization payload IE, 
    * TSCH-Slotframe and Link payload IE, TSCH Timeslot payload IE, 
    * and Channel Hopping payload IE to advertise the presence of a 
    * TSCH LPWMN and allow new devices to synchronize to it.
    */ 

   /*
    * 5.1.4.2
    * Note that a device sending Enhanced Beacons to advertise a TSCH 
    * LPWMN should set the Timekeeping bit in the Link Option field (as
    * described in 5.2.4.14) for the joining devices' receive link so 
    * that joining devices can maintain time synchronization until 
    * additional time source neighbors are configured by a higher layer.
    */ 

    /*
     * 6.2.19.3
     * ADVERTISE links may be used to send Enhanced beacons as the result 
     * of the MAC receiving a MLME-BEACON.request.
     */

    /*
     * 5.1.2.6
     * Once synchronized and configured by a higher layer to do so, all 
     * FFDs that are already part of the network may send enhanced beacons 
     * announcing the presence of the network. The advertising rate and 
     * content is configured by a higher layer as appropriate to the 
     * density of devices, the desired rate of network formation, and the 
     * energy devoted to network formation.
     */ 


    /*
     * Enhanced beacon frame format
     * ------------------------------------------------------------
     *       Field                         Length
     * ------------------------------------------------------------
     * - Frame Control                   1/2
     * - Sequence number                 0/1
     * - Addressing fields               Variable
     * - Aux security header             0/1/5/6/10/14
     * - Header IEs                      Variable
     * - Payload IEs                     Variable
     * - Beacon payload                  Variable
     * - FCS                             2 
     * ------------------------------------------------------------
     */
    
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
 *
 ********************************************************************
 */
UINT8_t MAC_buildFrameCntrlHdr(UINT8_t *hdr_p,
                               MAC_pduInfo_s *frameInfo_p)
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
MAC_sts_t MAC_buildPDU(MAC_pduInfo_s *pduInfo_p,
                       MAC_addr_s  *destAddr_p,
                       UINT16_t destShortAddr)
{
   UINT8_t *hdr_p = NULL;
   UINT8_t macHdrLen;
   UINT16_t offset = 0x0;
   MAC_sts_t sts = MAC_STS_SUCCESS;
   
   macHdrLen = MAC_calcHdrLen(pduInfo_p);
   
   {
      UINT8_t sduBDHndl = pduInfo_p->bdHndl;
      UINT8_t sduLen = SYS_GET_BD_LEN(sduBDHndl);
      UINT8_t pduBDHndl = SYS_allocMem(macHdrLen + sduLen);
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
   pduInfo_p->frameVersion = MAC_FRAME_VER_802_15_4;
   
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
   hdr_p[offset] = pduInfo_p->seqNr;
   offset ++;
   
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
      
   if ((SYS_GET_BD_LEN(pduInfo_p->bdHndl) + MAC_FCS_LEN) > RADIO_MAX_MAC_PDU_LEN)
   {
       sts = MAC_STS_FRAME_TOO_LONG;
   }
   
   return sts;
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
   
   PLTFRM_startTimerA0(tmoVal, 0, MAC_assocRespTmoHndlr);
  
   return;
}
#endif
                  
/*
 * The execution of an instruction or external events (e.g. reception of a frame) 
 * may result in one or more exceptions. The exceptions provide a very flexible 
 * mechanism for automating tasks. They can for instance be used to trigger execution 
 * of other instructions or they can be routed out to GPIO pins and used as interrupt 
 * signals to the microcontroller. The exception controller is responsible for 
 * handling of the exceptions.
 *
 * Exception flags are stored in status registers and can be read over the SPI or 
 * observed on GPIO. To clear an exception flag, the user must write �0� to the 
 * correct bit in the status register. If the user tries to clear an exception flag 
 * in the exact same clock period as the same exception occurs, the flag will not 
 * be cleared.
 */

#if 0
void MAC_delPendingFrame(UINT8_t tblIdx)
{
   if (tblIdx >= MAC_cntxt.pendingFrameListHead)
   {
       // List is corrupted !!
       SYS_fatal(SYS_FATAL_ERR_1);
   } 
   
   // Compact the pending frame buffer 
   if (tblIdx != (MAC_cntxt.pendingFrameListHead - 1))
   {  
       MAC_pendingFramesTblEntry_s *tblEntry_p;
       UINT8_t *srcBuff_p, *dstBuff_p, idx, byteIdx;
       tblEntry_p = &(MAC_cntxt.pendingFramesIdxTbl[tblIdx]);
       dstBuff_p = MAC_cntxt.pendingFramesBuff + tblEntry_p->frameOff;
       srcBuff_p = dstBuff_p + tblEntry_p->frameLen;       
       for (idx=tblIdx; idx < MAC_cntxt.pendingFrameListHead; idx++)
       {        
           for (byteIdx=0; byteIdx<tblEntry_p->frameLen; byteIdx++)
           {
               *dstBuff_p = *srcBuff_p;
               dstBuff_p ++;
               srcBuff_p ++;
           }
           
           *(tblEntry_p) = *(tblEntry_p + 1);
           
           tblEntry_p ++;
       }
   }
   
   MAC_cntxt.pendingFrameListHead --;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void MAC_indirectTxOverProc(MAC_sts_t sts)
{ 
   UINT8_t tblIdx = MAC_cntxt.txInProgressPendingFrameTblIdx;
   MAC_pendingFramesTblEntry_s *tblEntry_p = &(MAC_cntxt.pendingFramesIdxTbl[tblIdx]);
   
   /*
    * The coordinator shall begin handling a transaction on 
    *  > receipt of an indirect transmission request either via the MCPS-DATA.request 
    *    primitive 
    *  or 
    *  > via a request from the MLME to send a MAC command instigated by a primitive 
    *    from the next higher layer, such as the MLME-ASSOCIATE.response primitive, 
    *    as described in 6.2.2.3. 
    *
    * On completion of the transaction, the MAC sublayer shall indicate a status 
    * value to the next higher layer. 
    *
    * > If a request primitive instigated the indirect transmission, the corresponding 
    *   confirm primitive shall be used to convey the appropriate status value. 
    * > Conversely, if a response primitive instigated the indirect transmission, the 
    *   MLME-COMM-STATUS.indication primitive shall be used to convey the appropriate
    *   status value. The MLME-COMM-STATUS.indication primitive can be related to its 
    *   corresponding response primitive by examining the Destination Address field.
    */
  
   if (sts == MAC_STS_SUCCESS)
   {
       if (tblEntry_p->mcpsMlmeFlag == MAC_INDIRECT_TX_TRIGGER_MLME)
       {
           // remove the frame stored in the pending frame buffer
           MAC_delPendingFrame(tblIdx);
           
           // If there are no pending frames, configure cc2520 to send subsequent
           // acks with the pending bit cleared.
           if (MAC_cntxt.pendingFrameListHead >= MAC_PENDING_FRAMES_IDX_TBL_SZ)
               RADIO_clearFramePendingEveryAck();
           
           MAC_cntxt.commStsIndParams.status = sts;
           // SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_COMM_STS_INDICATION; 
       }
       else
       {
         
       }
   }
   else
   {
     
     
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
 *
 ********************************************************************
 */
void MAC_ifsTmoExpHndlr(void)
{
   MAC_sendDataCnfrm();  
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void MAC_clearTxModState(void)
{
#ifdef MAC_IFS_DELAY_DISABLED     
   MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE; 
#else       
    // Start IFS timer (fixed at 640 microseconds)
   PLTFRM_startTimerA0(0, 
                       MAC_MIN_LIFS_PERIOD_USECS,
                       MAC_ifsTmoExpHndlr);
       
   MAC_cntxt.txModState = MAC_TX_MOD_STATE_IFS;
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
void MAC_startIFSTimer(  )
{
   PLTFRM_startTimerA0(0, 
                       MAC_MIN_LIFS_PERIOD_USECS,
                       MAC_ifsTmoExpHndlr);
       
   MAC_cntxt.txModState = MAC_TX_MOD_STATE_IFS;  
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
void MAC_frameWaitTmoHndlr(void)
{
   // MAC_cntxt.coordPollInProgress = 0;

   // Send MLME-POLL.confirm

   /*
    * If a frame is not received within macMaxFrameTotalWaitTime even
    * though the acknowledgment to the data request command has its Frame
    * Pending field set to one, the MLME will issue the MLME-POLL.confirm
    * primitive with a status of NO_DATA.
    */



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
void MAC_ackRcvdHndlr(UINT8_t framePending)
{  
   /*
    * This function is called when a valid ack has been received.
    */
  
   // Stop the ack-tmo timer
   PLTFRM_stopTimerA0();
    
#ifdef MAC_STATS_ENA
   MAC_cntxt.validAckRxCnt ++;
#endif
  
   /*
    * Configure cc2520 to drop all ACK frames henceforth since we have
    * received the expected ACK.
    */
   cc2520_cfgDisAckFramesRx();

#ifndef LPWMN_COORD
#ifdef DEV_TYPE_RFD
#if 0
   if (MAC_cntxt.coordPollInProgress)
   {
       if (framePending)
       {
           UINT16_t timeOut;

           /*
            * On receipt of the acknowledgment frame with the Frame Pending
            * field set to one, a device shall enable its receiver for at
            * most macMaxFrameTotalWaitTime to receive the corresponding
            * data frame from the coordinator.
            */
           timeOut = MAC_MAX_FRAME_TOTAL_WAIT_TIME;
           timeOut <<= PHY_SYMBOL_LENGTH_USECS_SHIFT;
           PLTFRM_startTimerA0(0, timeOut, MAC_frameWaitTmoHndlr);
       }
   }
#endif
#endif

   if (MAC_cntxt.assocReqInProgress)
   { 
       MAC_cntxt.assocReqInProgress = 0x0;
       MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE;
       MAC_assocReqAckHndlr();
   }
   else
#endif     
   {     
       MAC_cntxt.datacnfrmParams.status = MAC_STS_SUCCESS;
       MAC_startIFSTimer();           
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
void MAC_doCsmaOnIfsExp(void)
{
    MAC_csmaInit();
    MAC_doCsmaProc(MAC_cntxt.macPduLen >= MAC_aMaxSIFSFrameSize ?
                   MAC_IFS_TYPE_LONG : MAC_IFS_TYPE_SHORT);
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
UINT8_t MAC_checkReassocCondition(void)
{
   MAC_cntxt.txFlrCnt ++;

   if (MAC_cntxt.txFlrCnt > MAC_RFD_REASSOC_THRESHOLD)
   {
       MAC_cntxt.txFlrCnt = 0x0;

       MAC_PIB.macShortAddress = MAC_BROADCAST_SHORT_ADDR;
       MAC_PIB.macLPWMNId = MAC_BROADCAST_LPWMN_ID;

       MAC_cntxt.pollReqSent = 0;
       SYS_globalEvtMsk2 &= ~SYS_GLOBAL_EVT_COORD_POLL_PENDING;
       SYS_globalEvtMsk2 &= ~SYS_GLOBAL_EVT_COORD_POLL_IN_PROGRESS;

       RADIO_restart();

       RADIO_cfgShortAddrFilter(MAC_PIB.macShortAddress);
       RADIO_cfgLPWMNIdFilter(MAC_PIB.macLPWMNId);
       RADIO_cfgExtAddrFilter(MAC_PIB.macExtendedAddress);

       SYS_globalEvtMsk1  |= SYS_GLOBAL_EVT_RFD_REASSOC_REQ;

       return 1;
   }

   return 0;
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
  
   /*
    * Configure cc2520 to drop all ACK frames henceforth since we have
    * timed out waiting for ACK.
    */
   cc2520_cfgDisAckFramesRx();
   
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
           
#ifndef LPWMN_COORD         
           if (MAC_cntxt.assocReqInProgress)
           {
               MAC_sendAssocCnfrm(MAC_STS_NO_ACK,
                                  MAC_BROADCAST_SHORT_ADDR);
               
               MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE;
           }
           else
#endif             
           {
               /*
                * If the TxOptions parameter specifies that a direct transmission is 
                * required and the MAC sublayer does not receive an acknowledgment 
                * from the recipient after macMaxFrameRetries retransmissions, as 
                * described in 5.1.6.4, it will discard the MSDU and issue the MCPS-
                * DATA.confirm primitive with a status of NO_ACK.
                */        
             
               MAC_cntxt.datacnfrmParams.status = MAC_STS_NO_ACK;
               
#if !defined(DEV_TYPE_RFD) && !defined(DEV_TYPE_FFD_NO_MESH) && !defined(LPWMN_COORD_NO_MESH)
               AODVL_purgeRtTbls(MAC_cntxt.datacnfrmParams.destShortAddr);
#endif

#ifdef DEV_TYPE_RFD
               if (MAC_checkReassocCondition() == 0x0)
                   MAC_sendDataCnfrm();
#else
               MAC_startIFSTimer();   
#endif
           }               
       }
       else
       { 
           MAC_cntxt.txAttempCnt ++;
           
           // Let us assume that the ACK did get sent but was not received by this
           // node. Suppose data/cmd packat transmission ended at instant T0. Also 
           // suppose that the corresponding ACK's transmission finished at instant T1.
           // In the worst case , T1 = T0 + macAckWaitDuration. Note that the 
           // intended recepient of the ACK will accept the ACK in this worst case.
           // 
           // Now MAC can retransmit only after "T1 + xIFS" where xIFS is LIFS if 
           // packet size > MAC_aMaxSIFSFrameSize or SIFS otherwise.
    
           
#ifdef MAC_IFS_DELAY_DISABLED
           MAC_csmaInit();
           MAC_doCsmaProc(MAC_cntxt.macPduLen >= MAC_aMaxSIFSFrameSize ?
                          MAC_IFS_TYPE_LONG : MAC_IFS_TYPE_SHORT);
    
#else           
           PLTFRM_startTimerA0(0, 
                               MAC_MIN_LIFS_PERIOD_USECS,
                               MAC_doCsmaOnIfsExp);
       
           MAC_cntxt.txModState = MAC_TX_MOD_STATE_IFS;           
#endif
           
       }            
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
 *
 ********************************************************************
 */
void MAC_scanDurationTmoHndlr(void)
{
#ifdef MAC_STATS_ENA
   MAC_cntxt.scanTmoCnt ++;
#endif  
   
   MAC_doActiveScan(  ); 
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
void MAC_txOverEvtHndlr(UINT8_t intRcvd)
{
   if (intRcvd)
   {
       UINT8_t regVal;

#ifdef FLASH_LED_ON_PKT_RX_TX
       PLTFRM_ON_LED_2();
#endif

       cc2520_memRead(CC2520_EXCFLAG0, 0x1, &regVal); 
       if ((regVal & CC2520_EXCFLAG0_TX_FRM_DONE_BIT) == 0x0)
           return;
#ifdef MAC_STATS_ENA
       MAC_cntxt.frameTxDoneEvtCnt ++;
#endif   
   }
   
   PLTFRM_stopTimerA0(); 
   
   // Radio is configured to listen after a transmission
   RADIO_setCurrState(RADIO_STATE_RX_ON);
   
   // Clear the TX_FRM_DONE exception in the cc2520's EXCFLAG0 register
   // cc2520_bitClear() does not work on this register.EXCFLAG0 is write zero 
   // only 
   {
      UINT8_t regVal = 0x0;
      cc2520_memWrite(CC2520_EXCFLAG0, 0x1, &regVal); 
   }
   
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

#ifndef RFD_CONT_TX_TEST
       scanDuration = MAC_aBaseSuperframeDuration; 
       scanDuration <<= MAC_cntxt.scanDuration;
       scanDuration += MAC_aBaseSuperframeDuration;  // in symbols
       scanDuration <<= PHY_SYMBOL_LENGTH_USECS_SHIFT; // in microseconds
           
       // Convert to millisecs
       // X/1000 = (X/1024)*(1.024) = (X/1024) + ((X/1024)*(0.024))
       // < (X/1024) + ((X/1024)*(0.0625)) = ((X >> 10) + ((X >> 10) >> 4))
       scanDuration >>= 10;  
       scanDuration += (scanDuration >> 4);
       scanDuration ++; 
       PLTFRM_startTimerA0(scanDuration, 0, MAC_scanDurationTmoHndlr);
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

   if (MAC_cntxt.ackRequired)
   {        
       /*
        * A frame transmitted with the AR field set to request an acknowledgment, 
        * as defined in 5.2.1.1.4, shall be acknowledged by the recipient. If the 
        * intended recipient correctly receives the frame, it shall generate and
        * send an acknowledgment frame containing the same DSN from the data or 
        * MAC command frame that is being acknowledged.
        * The transmission of an acknowledgment frame in a nonbeacon-enabled LPWMN
        * shall commence macSIFSPeriod after the reception of the last symbol of 
        * the data or MAC command frame.
        *
        * "macSIFSPeriod" is the minimum time forming a SIFS period. It is PHY
        * dependant (as defined in 8.1.3). For PHYs other than UWB PHY, the value
        * of "macSIFSPeriod" is 12 symbols (12 * 16 => 192 microsecs).
        *
        * Note that cc2520 automatically sends out an ack frame 12 symbols
        * (192 microseconds) after the last symbol of the received frame.
        *
        * Acknowledgement frame length is 11 bytes (including preamble, SFD and
        * length field). 
        * At 250 kbps, ack frame will take (11*8)/(250*1000) -> 352 microseconds
        */
     
       /*
        * A device that sends a data or MAC command frame with its AR field set 
        * to acknowledgment requested shall wait for at most macAckWaitDuration 
        * for the corresponding acknowledgment frame to be received.
        * 
        * For the O-QPSK PHY @ 2.4 GHz, this time is a constant (864 microsecs)
        */     
       PLTFRM_startTimerA0(5,   // HACK
                           MAC_ACK_WAIT_DURAITION_MICROSECS,
                           MAC_ackTmoHndlr);
     
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
       // Bug, fixed on feb/7/14, rkris@wisense.in
#if defined(DEV_TYPE_RFD) || defined(DEV_TYPE_FFD)
       if (MAC_cntxt.assocReqInProgress)
       {
           MAC_cntxt.assocReqInProgress = 0x0;
           MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE;
           MAC_assocReqAckHndlr();
       }
       else
#endif
       {
           MAC_cntxt.datacnfrmParams.status = MAC_STS_SUCCESS;
    
#ifdef MAC_IFS_DELAY_DISABLED   
           MAC_sendDataCnfrm();
#else    
           MAC_startIFSTimer();
#endif
       }
   }   

#ifdef FLASH_LED_ON_PKT_RX_TX
   if (intRcvd)
   {
       PLTFRM_OFF_LED_2();
   }
#endif

   return;
}


#if 0
#ifndef DEV_TYPE_RFD
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void MAC_processDataRequestCmd(void)
{
   UINT8_t tblIdx;
   UINT8_t *buff_p = MAC_cntxt.pendingFramesBuff;
   UINT8_t rcvdAddrMode = 
           MAC_cntxt.commStsIndParams.dstAddrMode == MAC_ADDRESS_TYPE_SHORT ? 0x0 : 0x1;
   MAC_pendingFramesTblEntry_s *tblEntry_p;
   UINT8_t *entrySrcAddr_p = NULL;
   
   for (tblIdx = 0; tblIdx < MAC_cntxt.pendingFrameListHead; tblIdx ++)
   {
        tblEntry_p = &(MAC_cntxt.pendingFramesIdxTbl[tblIdx]);
        
        if (rcvdAddrMode == tblEntry_p->addrType)
        {
            UINT16_t entrySrcAddr;
            
            entrySrcAddr_p = MAC_cntxt.pendingFramesBuff;
            entrySrcAddr_p += tblEntry_p->frameOff + MAC_PDU_HDR_DEST_ADDR_OFF;
            
            if (rcvdAddrMode == 0x0)
            {
                // match short addresses
                entrySrcAddr = *(entrySrcAddr_p + 1);
                entrySrcAddr = ((entrySrcAddr << 8) | *(entrySrcAddr_p));
            
                if (MAC_cntxt.commStsIndParams.dstAddr.shortAddr == entrySrcAddr)
                {
                    break;
                }
            }
            else
            {
                // match extended address
                if (memcmp(entrySrcAddr_p, 
                           MAC_cntxt.commStsIndParams.dstAddr.extAddr, 
                           MAC_EXT_ADDR_LEN) == 0)
                {
                    break;
                }
            }
        }
   }
   
   if (tblIdx >= MAC_cntxt.pendingFrameListHead)
   {
       // <TODO>
     
       /* If there is no data frame pending for the requesting device, the 
        * coordinator shall send a data frame without requesting acknowledgment 
        * to the device containing a zero length payload, indicating that no data 
        * are present.
        * If the requesting device does not receive a data frame from the coordinator 
        * within macMaxFrameTotalWaitTime or if the requesting device receives a data 
        * frame from the coordinator with a zero length payload, it shall conclude 
        * that there are no data pending at the coordinator.
        */
     
       // For now, do not send this zero payload frame. Anyway the remote
       // node will timeout after "macMaxFrameTotalWaitTime".
     
       // MAC is now ready to accept another data-request command over the air
       MAC_cntxt.commStsIndParams.dstAddrMode = MAC_ADDRESS_TYPE_UNKNOWN;
   }
   else
   {
       /*
        * The data frame following the acknowledgment of the data request command 
        * shall be transmitted using one of the following mechanisms:
        *   1) From spec
        *   2) Using CSMA-CA, otherwise.
        */
     
       // This implementation always uses CSMA-CA to transmit the pending frame
       // to the requestor.

       // Save the table index. This will be used to remove the entry when
       // the transmission succeeds.
       MAC_cntxt.txInProgressPendingFrameTblIdx = tblIdx;
       MAC_cntxt.ackRequired = 1;
       MAC_cntxt.txAttempCnt = 1;
       MAC_cntxt.macPduLen = tblEntry_p->frameLen + MAC_FCS_LEN;
       MAC_cntxt.expAckDSN =  MAC_getNextDSN();
       MAC_cntxt.cnfrmReqd = 0x1;

       {     
          UINT8_t *dsn_p = MAC_cntxt.pendingFramesBuff;
          dsn_p += (tblEntry_p->frameOff + MAC_PDU_HDR_DSN_FIELD_OFF);
          *dsn_p = MAC_cntxt.expAckDSN;
       }

       MAC_cntxt.datacnfrmParams.status = MAC_STS_TX_PENDING;
       MAC_cntxt.datacnfrmParams.destShortAddr = MAC_cntxt.commStsIndParams.dstAddr.shortAddr;
   
       RADIO_transferPktBuffToTxFifo(MAC_cntxt.pendingFramesBuff + tblEntry_p->frameOff,
                                     tblEntry_p->frameLen);

       MAC_csmaInit();
       MAC_doCsmaProc(MAC_IFS_TYPE_NONE);      
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
UINT8_t MAC_addFrameToPendingList(MAC_pduInfo_s *pduInfo_p,  
                                  UINT8_t remoteAddrLen,
                                  UINT8_t mcpsMlmeFlag)
{
   UINT8_t tblIdx = MAC_cntxt.pendingFrameListHead, 
           frameLen = pduInfo_p->hdrLen + pduInfo_p->pyldLen;
   UINT16_t frameBuffOffset = 0x0;
   UINT8_t *buff_p = MAC_cntxt.pendingFramesBuff;
   MAC_pendingFramesTblEntry_s *tblEntry_p;
   
   // <TODO>
   // Allow only one pending packet per node
   
   /*
    * <5.1.5 Transaction handling>
    * The information contained in the indirect transmission request forms a 
    * transaction, and the coordinator shall be capable of storing at least one 
    * transaction. On receipt of an indirect transmission request, if there is  
    * no capacity to store another transaction, the MAC sublayer shall indicate 
    * to the next higher layer a status of TRANSACTION_OVERFLOW in the appropriate 
    * corresponding primitive.
    *
    * If the coordinator is capable of storing more than one transaction, it shall 
    * ensure that all the transactions for the same device are sent in the order 
    * in which they arrived at the MAC sublayer.
    */
   
   /*
    * Check if the index table is full or not.
    */
   if (tblIdx >= MAC_PENDING_FRAMES_IDX_TBL_SZ)
       return 0;
   
   if (tblIdx > 0)
   {
       // Calculate the offset (in the frames buffer) where the new frame
       // will be copied to.
       tblEntry_p = &(MAC_cntxt.pendingFramesIdxTbl[tblIdx - 1]);
       
       frameBuffOffset = tblEntry_p->frameOff;
       frameBuffOffset += tblEntry_p->frameLen;       
       
       /*
        * Check if there is enough space to accomodate this new frame.
        */      
       if (frameLen > (MAC_PENDING_FRAMES_BUFF_LEN - frameBuffOffset))
           return 0;       
   }       
   
   // Copy the new frame into the pending frames buffer
   memcpy(buff_p + frameBuffOffset,
          MAC_cntxt.MAC_txPduHdrBuff, // pduInfo_p->hdr_p,
          pduInfo_p->hdrLen);
          
   memcpy(buff_p + frameBuffOffset + pduInfo_p->hdrLen,
          pduInfo_p->pyld_p,
          pduInfo_p->pyldLen);
   
   // point to new entry in the pending frames index table
   tblEntry_p = &(MAC_cntxt.pendingFramesIdxTbl[tblIdx]);  
   
   tblEntry_p->addrType = (remoteAddrLen == MAC_SHORT_ADDR_LEN ? 0 : 1);
   tblEntry_p->frameOff = frameBuffOffset;
   tblEntry_p->frameLen = frameLen;
   tblEntry_p->mcpsMlmeFlag = mcpsMlmeFlag;

   // <TODO>
   // Configure cc2520 to set the ACK pending bit in the ack frame transmitted
   // in response to a data-request command received from the node for which
   // this node has a pending frame.
   // Refer to page of the cc2520 spec.
   // For now, configure cc2520 to set this bit irrespective of which node
   // has sent a data-request command frame.   
   if (MAC_cntxt.pendingFrameListHead == 0x0)
       RADIO_setFramePendingEveryAck();
   
   // Increase the number of entries in the pending frames index table
   MAC_cntxt.pendingFrameListHead ++;
   
   return 1;
}
#endif
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
void MAC_sendAssocCnfrm(UINT8_t sts, UINT16_t allocShortAddr)
{
   // Turn the radio off
  
   if (sts != MAC_STS_SUCCESS)
   {
       RADIO_hardRxStop();   
       RADIO_setCurrState(RADIO_STATE_IDLE);
     
       // Reset related PIB attributes
       MAC_PIB.macLPWMNId = MAC_BROADCAST_LPWMN_ID;
       MAC_PIB.macCoordShortAddress = MAC_BROADCAST_SHORT_ADDR;
   }
   
   MAC_cntxt.assocReqInProgress = 0;
   
   MAC_cntxt.assocCnfrmParams.status = sts;
   MAC_cntxt.assocCnfrmParams.assocShortAddr = allocShortAddr;  

   SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_MAC_ASSOC_CONFIRM;
   
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
void MAC_txOverTmoCbFunc( )
{
#ifdef MAC_STATS_ENA  
   MAC_cntxt.MAC_txOverTmoCnt ++;
#endif

   MAC_txOverEvtHndlr(0);
}


UINT8_t MAC_dbgFsmStat1 = 0, MAC_dbgFsmStat0 = 0;

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
   UINT8_t fsmStat1, fsmStat0;
   
   /*
    * <STXONCCA command strobe>
    *
    * If CCA indicates a clear channel:
    *   Enable calibration, then TX.
    *   [ 
    *     Aborts ongoing transmission/reception and forces a TX calibration 
    *     followed by transmission.
    *   ]
    * else  
    *   do nothing
    *
    * Also sample the value of the CCA status signal, and store in status 
    * register.
    *
    *
    * 19.7 Clear Channel Assessment
    * The clear channel assessment (CCA) status signal indicates whether the 
    * channel is available for transmission or not. The CCA function is used to 
    * implement the CSMA-CA functionality specified in the IEEE 802.15.4 spec [2]. 
    * The CCA signal is valid when the receiver has been enabled for at least 8
    * symbol periods. The RSSI_VALID status signal can be used to verify this.
    *
    * 20.5 RSSI
    * CC2520 has a built-in RSSI (Received Signal Strength Indication) which 
    * calculates an 8 bit signed digital value that can be read from a register 
    * or automatically appended to received frames. The RSSI value is the result 
    * of averaging the received power over 8 symbol periods (128 �s) as specified 
    * by IEEE 802.15.4 [2]. The RSSI value is a 2�s complement signed number on 
    * a logarithmic scale with 1dB steps. 
    * The status bit RSSI_VALID should be checked before reading the RSSI value 
    * register. RSSI_VALID indicates that the RSSI value in the register is in 
    * fact valid, which means that the receiver has been enabled for at least 8 
    * symbol periods.
    *
    * Transmission of preamble begins 192 us after the STXON or STXONCCA command 
    * strobe. This is referred to as "TX turnaround time" in [2].
    *
    * CCA mode setting:
    *  > 11 : CCA = 1 when RSSI < CCA_THR-CCA_HYST and not receiving a frame, 
    *         CCA = 0 when RSSI >= CCA_THR or receiving a frame
    */

#ifdef MAC_DEBUG   
   // cc2520_memRead(CC2520_CHIPID, 0x1, &MAC_dbgPreChipId);   
   // cc2520_memRead(CC2520_FSMSTAT1, 0x1, &MAC_dbgPreFSMSTAT1);
   // cc2520_memRead(CC2520_TXFIFOCNT, 0x1, &MAC_dbgPreTXFIFOCNT);
   // c2520_memRead(CC2520_FRMCTRL1, 0x1, &MAC_dbgPreFRMCTRL1);  
   // cc2520_memRead(CC2520_RXENABLE0, 0x1, &MAC_dbgPreRXENABLE0);
   // cc2520_memRead(CC2520_RXENABLE1, 0x1, &MAC_dbgPreRXENABLE1);  
#endif
   
   cc2520_memRead(CC2520_FSMSTAT0, 0x1, &fsmStat0);
   fsmStat0 &= CC2520_FSMSTAT0_FSM_FFCTRL_STATE_BIT_MSK;
   
   
#ifdef WISENSE_HW_REV_3   
   
   // Rev-3 Hack !!!
   // With rev-3 hardware, AUTO-ACK functionality gets cleared for unknown reasons !!
   // RADIO_enableAutoAck( );
      
   if (RADIO_getCurrState() == RADIO_STATE_IDLE
       || fsmStat0 != CC2520_RADIO_STATE_SFD_WAIT)
   {
       // With Rev-3 hardware, RADIO goes to idle state for unknown reasons !!!
       RADIO_hardRxStart();
       RADIO_setCurrState(RADIO_STATE_RX_ON);    
   }

   do
   {
       cc2520_memRead(CC2520_FSMSTAT0, 0x1, &fsmStat0);
   } while (fsmStat0 != 0x6);
#else
   if (RADIO_getCurrState() == RADIO_STATE_IDLE)
   {
       RADIO_hardRxStart();
       RADIO_setCurrState(RADIO_STATE_RX_ON);    
   }   
#endif
   
   cc2520_memRead(CC2520_FSMSTAT0, 0x1, &MAC_dbgFsmStat0);

   // Check if transmit power change is pending ...
   if (PHY_PIB.changeReqBitMsk & PHY_TX_POWER_CHANGE_REQ_BM)
   {
       RADIO_setTxPower(PHY_PIB.phyTXPower, 0);
       PHY_PIB.changeReqBitMsk &= ~(PHY_TX_POWER_CHANGE_REQ_BM);
   }

   /*
    * When transmitting with STXONCCA, the receiver would be on before the
    * transmission.
    */
   RADIO_startTxOnCCA( );
   
   // Check the SAMPLED_CCA bit in FSMSTAT1 register. 
   // The "SAMPLED_CCA" bit contains a sampled value of the CCA. The value is 
   // updated whenever a SSAMPLECCA or STXONCCA strobe is issued.
   // <TODO> STXONCCA to SAMPLED_CCA bit update delay ?
   
   
#ifdef TXONCCADBG   
   loopCnt = 0;
   while (1)
   {
      // Read the FSMSTAT1 register
      cc2520_memRead(CC2520_FSMSTAT1, 0x1, &fsmStat1);
      if (fsmStat1 & CC2520_FSMSTAT1_SAMPLED_CCA_BIT)
          break;
      loopCnt ++;
      PLTFRM_delay10MicroSecs();
   }
#else
   cc2520_memRead(CC2520_FSMSTAT1, 0x1, &fsmStat1);
#endif   
   


   if (fsmStat1 & CC2520_FSMSTAT1_SAMPLED_CCA_BIT)
   {
       RADIO_setCurrState(RADIO_STATE_TX_ON); 
     
#ifdef MAC_STATS_ENA     
       MAC_cntxt.ccaOkCnt ++;
#endif
       cc2520_memRead(CC2520_FSMSTAT0, 0x1, &MAC_dbgFsmStat1);

       PLTFRM_startTimerA0(10, 0, MAC_txOverTmoCbFunc);
     
       // CCA passed. Transmission underway    
       MAC_cntxt.txModState = MAC_TX_MOD_STATE_WAIT_TX_OVER;
       
#ifndef LPWMN_COORD       
       if (MAC_cntxt.scanInProgress == 0x0)
       {
#endif         
           if (MAC_cntxt.ackRequired)
           {
              // The cc2520 has been configured to drop ACK frames by default. Since
              // we are expecting an ACK frame at this point, disable ACK frame
              // filtering.
              cc2520_cfgEnaAckFramesRx( );
           }
#ifndef LPWMN_COORD           
       }
#endif        
   }
   else
   {
       // CCA failed 
     
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
                   MAC_cntxt.datacnfrmParams.status = MAC_STS_CHANNEL_ACCESS_FLR;
                   MAC_sendDataCnfrm();
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
UINT16_t MAC_getCsmaBackOff(UINT16_t backOffLimit)
{
   UINT16_t randVal = pltfrm_randU16();

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
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
void MAC_doCsmaProc(UINT8_t ifsType)
{   
   /*
    * Nonbeacon-enabled LPWMNs use an unslotted CSMA-CA channel access mechanism,
    * as described in 5.1.1.4. Each time a device wishes to transmit data frames 
    * or MAC commands, it waits for a random period. If the channel is found to 
    * be idle, following the random backoff, the device transmits its data. If 
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
           * One backoff period shall be equal to aUnitBackoffPeriod (in symbols)
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
  
          if (ifsType != MAC_IFS_TYPE_NONE)
          {
              /*
               * The MAC sublayer needs a finite amount of time to process data 
               * received by the PHY. To allow for this, two successive frames 
               * transmitted from a device shall be separated by at least an IFS 
               * period; if the first transmission requires an acknowledgment, the 
               * separation between the acknowledgment frame and the second 
               * transmission shall be at least an IFS period. The length of the IFS 
               * period is dependent on the size of the frame that has just been 
               * transmitted. Frames (i.e., MPDUs) of up to aMaxSIFSFrameSize shall be
               * followed by a short interfame space (SIFS) period of a duration of   
               * at least macSIFSPeriod. Frames (i.e., MPDUs) with lengths greater 
               * than aMaxSIFSFrameSize shall be followed by a long interframe spacing
               * (LIFS) period of a duration of at least macLIFSPeriod.
               */           
          }
       
          MAC_cntxt.txModState = MAC_TX_MOD_STATE_CSMA_BACK_OFF;
       
          if (backOffDelay > 0)
          {  
              PLTFRM_startTimerA0(backOffDelay/1000, 
                                  backOffDelay % 1000,
                                  MAC_csmaAttemptTxOnCca);
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
 ********************************************************************
 */
UINT8_t MAC_getNextDSN(void)
{
   UINT8_t nextDSN = MAC_PIB.macDSN;
   
   /*
    * <802.15.4-2011>
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
   MAC_PIB.macDSN = (MAC_PIB.macDSN + 1) & 0xff;    
   
   return nextDSN;
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
void MAC_buildSigPktMacHdr(UINT16_t destAddr, UINT16_t sduLen)
{
   const UINT8_t macHdrLen = MAC_PDU_HDR_FC_FIELD_LEN + MAC_PDU_HDR_SEQ_NR_FIELD_LEN
                       + MAC_LPWMN_ID_LEN + (MAC_SHORT_ADDR_LEN*2);
   UINT8_t macPduLen =  macHdrLen + sduLen + MAC_FCS_LEN;
   UINT8_t dsn = MAC_getNextDSN();
   
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
   MAC_cntxt.expAckDSN =  dsn;
   MAC_cntxt.cnfrmReqd = 0x0;  // Need confirm for RREP and PREQ/PREP messages
   MAC_cntxt.macPduLen = macPduLen;
   
   MAC_cntxt.datacnfrmParams.status = MAC_STS_TX_PENDING;
   MAC_cntxt.datacnfrmParams.destShortAddr = destAddr;
     
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
   MAC_csmaInit();
   MAC_doCsmaProc(MAC_IFS_TYPE_NONE);   
     
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
UINT16_t MAC_getNextHop(const UINT8_t bdHndl)
{
   UINT8_t *macHdr_p = SYS_GET_BD_BUFF(bdHndl);
   UINT16_t macDstAddr;
   
   macDstAddr = macHdr_p[MAC_PDU_HDR_DEST_ADDR_FIELD_OFF + 1];
   macDstAddr = (macDstAddr << 8) | macHdr_p[MAC_PDU_HDR_DEST_ADDR_FIELD_OFF];
   
   return macDstAddr;
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
   /*
    * The Sequence Number field shall contain the current value of macDSN.
    * macDSN is the sequence number added to the transmitted data or MAC 
    * command frame. 
    */   
      
   macHdr_p[MAC_PDU_HDR_DSN_FIELD_OFF] = MAC_getNextDSN();

   MAC_cntxt.ackRequired = 0x1;
   MAC_cntxt.txAttempCnt = 1;
   MAC_cntxt.macPduLen = SYS_GET_BD_LEN(bdHndl) + MAC_FCS_LEN;
   MAC_cntxt.expAckDSN =  macHdr_p[MAC_PDU_HDR_DSN_FIELD_OFF];
   MAC_cntxt.cnfrmReqd = 0x1;
   
   MAC_cntxt.datacnfrmParams.status = MAC_STS_TX_PENDING;
   MAC_cntxt.datacnfrmParams.destShortAddr = MAC_getNextHop(bdHndl);
   
   RADIO_transferPktToTxFifo(bdHndl);
   SYS_freeMem(bdHndl);
   
#ifdef MAC_STATS_ENA           
   MAC_cntxt.fwdPktTxReqCnt ++;
#endif    
           
   MAC_csmaInit();
   MAC_doCsmaProc(MAC_IFS_TYPE_NONE);        
   
   return;
}


/*
 * <INDIRECT TRANSMISSION REQUEST>
 *
 *
 * The coordinator shall begin handling a transaction on receipt of an indirect
 * transmission request either via the MCPS-DATA.request primitive or ....
 *
 * On completion of the transaction, the MAC sub layer shall indicate a status
 * value to the next higher layer. If a request primitive instigated the indirect
 * transmission, the corresponding confirm primitive shall be used to convey the
 * appropriate status value.
 *
 * If the coordinator is capable of storing more than one transaction, it shall
 * ensure that all the transactions for the same device are sent in the order in
 * which they arrived at the MAC sub layer. For each transaction sent, if another
 * exists for the same device, the MAC sub layer shall set its Frame Pending field
 * to one, indicating the additional pending data.
 * Each transaction shall persist in the coord for at most macTransactionPersistenceTime.
 * If the transaction is not successfully extracted by the appropriate device within
 * this time, the transaction information shall be discarded and the MAC sub layer
 * shall indicate to the next higher layer a status of TRANSACTION_EXPIRED in the
 * appropriate corresponding primitive. In order to be successfully extracted, an
 * acknowledgment shall be received if one was requested. If the transaction was
 * successful, the transaction information shall be discarded, and the MAC sub layer
 * shall indicate to the next higher layer a status of SUCCESS in the appropriate
 * corresponding primitive.
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
void MCPS_dataReq(const MCPS_dataReqParams_s * const params_p)
{
   MAC_sts_t sts = MAC_STS_SUCCESS;
   MAC_pduInfo_s pduInfo;
   
   MAC_cntxt.datacnfrmParams.status = MAC_STS_TX_PENDING;
   MAC_cntxt.datacnfrmParams.destShortAddr = params_p->destShortAddr;
      
   if (params_p->srcAddrMode == MAC_ADDRESS_MODE_NO_ADDR
       && params_p->destAddrMode == MAC_ADDRESS_MODE_NO_ADDR)
   {
       /*
        * If both the SrcAddrMode and the DstAddrMode parameters 
        * are set to NO_ADDRESS in the MCPS-DATA.request primitive,
        * the status shall be set to INVALID_ADDRESS.
        */
       sts = MAC_STS_INVALID_ADDRESS;
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
   
#if 1
   pduInfo.bdHndl = params_p->msduBDHndl;
#else   
   pduInfo.pyld_p = params_p->msdu_p;
   pduInfo.pyldLen = params_p->msduLength;
#endif   
   /*
    * The Sequence Number field shall contain the current value of macDSN.
    * macDSN is the sequence number added to the transmitted data or MAC 
    * command frame. 
    */   
   pduInfo.seqNr = MAC_getNextDSN();

   sts = MAC_buildPDU(&pduInfo, NULL, params_p->destShortAddr);                      
   if (sts != MAC_STS_SUCCESS)
   {
       goto _end;
   }
   
#if 0   
   MAC_cntxt.dataReqMsduHandle = params_p->msduHandle;
#endif   
   MAC_cntxt.ackRequired = pduInfo.ackRequired;
   MAC_cntxt.txAttempCnt = 1;
   MAC_cntxt.macPduLen = SYS_GET_BD_LEN(pduInfo.bdHndl) + MAC_FCS_LEN;
   MAC_cntxt.expAckDSN =  pduInfo.seqNr;
   MAC_cntxt.cnfrmReqd = 0x1;
   
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

#ifdef IEEE_802_15_4e     
       if (MAC_PIB.macTSCHenabled)
       {
           // Send packet at the next timeslot 
       }
       else
#endif         
       {           
#ifdef MAC_STATS_ENA           
           MAC_cntxt.dataReqCnt ++;
#endif 

           RADIO_transferPktToTxFifo(pduInfo.bdHndl);
           SYS_freeMem(pduInfo.bdHndl);
           
           MAC_csmaInit();
           MAC_doCsmaProc(MAC_IFS_TYPE_NONE);
       }
   }
   else
   {
#ifdef DEV_TYPE_RFD
      sts = MAC_STS_INVALID_PARAMETER;
#else
      /*
       * If the TxOptions parameter specifies that an indirect transmission is
       * required and this primitive is rxd by the MAC sub layer of a coordinator,
       * the data frame is sent using indirect transmission, as described in 5.1.5
       * and 5.1.6.3.
       */
#if 0
      if (MAC_addFrameToPendingList(&pduInfo,  
                                    MAC_SHORT_ADDR_LEN,
                                    MAC_INDIRECT_TX_TRIGGER_MCPS))
      {
         // Added this packet to pending indirect tx buffer !!
         MAC_cntxt.datacnfrmParams.status = MAC_STS_SUCCESS;
       
         SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_DATA_CONFIRM;       
      }
      else
      {
         // Failed to add this packet to pending indirect tx buffer !!
         SYS_freeMem(pduInfo.bdHndl);
      }
#endif
#endif
   }

_end:
   if (sts != MAC_STS_SUCCESS)
   {          
       MAC_cntxt.datacnfrmParams.status = sts;
       
       SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_DATA_CONFIRM;       
   }

   return; 
}

#ifdef DEV_TYPE_RFD
/*
 *******************************************************************
 *
 *
 *
 *
 *******************************************************************
 */
MAC_sts_t MAC_buildSendDataReqCmd(void)
{
   MAC_pduInfo_s pduInfo;
   UINT8_t *buff_p;
   UINT8_t offset = 0x0, macHdrLen;
   MAC_sts_t sts = MAC_STS_SUCCESS;

   /*
    * The data request command is sent by a device to request data from the PAN
    * coordinator or a coordinator.
    *
    *  This command shall be sent when instructed to do so by the next higher layer
    *  on reception of the MLME-POLL.request primitive.
    *
    *  All devices shall be capable of transmitting this command, although an RFD is
    *  not required to be capable of receiving it.
    *
    * The data request command shall be formatted as illustrated in Figure 53-
    * | MHR Fields (Variable) | Command Frame Id (1 byte) |
    */

   pduInfo.frameVersion = MAC_FRAME_VER_802_15_4;
   pduInfo.frameType = MAC_FRAME_TYPE_CMD;
   pduInfo.secEna = 0x0;

   pduInfo.dam = MAC_ADDRESS_MODE_SHORT_ADDR;
   pduInfo.sam = MAC_ADDRESS_MODE_SHORT_ADDR;

   pduInfo.lpwmnIdComp = 0x1;

   macHdrLen = MAC_calcHdrLen(&pduInfo);
   pduInfo.bdHndl = SYS_allocMem(macHdrLen + MAC_DATA_REQ_CMD_PDU_PYLD_LEN);
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
   buff_p[offset] = pduInfo.seqNr;
   offset ++;

   // LPWMN Id
   buff_p = UTIL_htole16(buff_p + offset, MAC_PIB.macLPWMNId);

   // Destination short address (parent node's address)
   buff_p = UTIL_htole16(buff_p, MAC_PIB.macCoordShortAddress);

   // Source short address
   buff_p = UTIL_htole16(buff_p, MAC_PIB.macShortAddress);

   buff_p[offset] = MAC_CMD_DATA_REQUEST;
   offset ++;

   /*
    * The Frame Pending field shall be set to zero and ignored upon reception, and
    * the AR field shall be set to one.
    */

   MAC_cntxt.ackRequired = 0x1;
   MAC_cntxt.txAttempCnt = 1;
   MAC_cntxt.expAckDSN =  pduInfo.seqNr;
   MAC_cntxt.macPduLen = macHdrLen + MAC_DATA_REQ_CMD_PDU_PYLD_LEN;
   MAC_cntxt.cnfrmReqd = 0x1; // Confirmation required

   RADIO_transferPktToTxFifo(pduInfo.bdHndl);
   SYS_freeMem(pduInfo.bdHndl);

   MAC_csmaInit();
   MAC_doCsmaProc(MAC_IFS_TYPE_NONE);

   return sts;
}
#endif

/*
 *******************************************************************
 *
 *
 *
 *
 *******************************************************************
 */
MAC_sts_t MAC_buildSendAssocnRespCmd(const UINT8_t *extAddr_p,
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
   pduInfo.sam = MAC_ADDRESS_MODE_EXTENDED_ADDR;
   
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
   buff_p[offset] = pduInfo.seqNr;
   offset ++;   
   
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
    * The Source Address field shall contain the value of macExtendedAddress.   
    */
   memcpy(buff_p + offset, MAC_PIB.macExtendedAddress, MAC_EXT_ADDR_LEN);
   offset += MAC_EXT_ADDR_LEN;  

   
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
   MAC_cntxt.macPduLen = macHdrLen + MAC_ASSOC_RESP_CMD_PDU_PYLD_LEN;
   MAC_cntxt.cnfrmReqd = 0x0; // No confirmation required 
     
   RADIO_transferPktToTxFifo(pduInfo.bdHndl);
   SYS_freeMem(pduInfo.bdHndl);
 
   MAC_csmaInit();
   MAC_doCsmaProc(MAC_IFS_TYPE_NONE); 
                         
   return sts;   
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
   hdr_p[offset] = pduInfo.seqNr;
   offset ++;
   
   /*
    * The Destination LPWMN Identifier field shall contain the identifier of the
    * LPWMN to which to associate.
    */      
   hdr_p[offset] = params_p->coordLPWMNId;
   hdr_p[offset + 1] = (params_p->coordLPWMNId >> 8) & 0xff;
   offset += MAC_LPWMN_ID_LEN;  
   
   /*
    * The Destination Address field shall contain the address from the beacon 
    * frame that was transmitted by the coordinator to which the association 
    * request command is being sent.
    */
   if (params_p->coordAddrMode == MAC_ADDRESS_MODE_SHORT_ADDR)
   {
       MAC_PIB.macCoordShortAddress = params_p->coordAddress.shortAddr;
       hdr_p[offset] = (params_p->coordAddress.shortAddr) & 0xff;             
       hdr_p[offset + 1] = ((params_p->coordAddress.shortAddr) >> 8) & 0xff;
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
   hdr_p[offset] = MAC_BROADCAST_LPWMN_ID_LSB;
   hdr_p[offset + 1] = MAC_BROADCAST_LPWMN_ID_MSB;
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
   MAC_cntxt.expAckDSN =  pduInfo.seqNr;   
   MAC_cntxt.macPduLen = macHdrLen + MAC_ASSOC_REQ_CMD_PDU_PYLD_LEN;
   MAC_cntxt.cnfrmReqd = 0x1;
   
   RADIO_transferPktToTxFifo(pduInfo.bdHndl);
   SYS_freeMem(pduInfo.bdHndl);
 
   MAC_csmaInit();
   MAC_doCsmaProc(MAC_IFS_TYPE_NONE); 
   
   return MAC_STS_SUCCESS;
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
   pduInfo.seqNr = MAC_PIB.macDSN;
   MAC_PIB.macDSN = (MAC_PIB.macDSN + 1) & 0xff;

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
                         MAC_BROADCAST_SHORT_ADDR);
      if (sts != MAC_STS_SUCCESS)
      {
          goto _end;
      }
      
      MAC_cntxt.ackRequired = 0x0;
      MAC_cntxt.cnfrmReqd = 0x0;
      MAC_cntxt.txAttempCnt = 1;
      MAC_cntxt.macPduLen = SYS_GET_BD_LEN(pduInfo.bdHndl);  
   
      RADIO_transferPktToTxFifo(pduInfo.bdHndl);
      SYS_freeMem(pduInfo.bdHndl);
      
#ifdef MAC_STATS_ENA
      MAC_cntxt.bcnReqCmdTxCnt ++;
#endif
      
      MAC_csmaInit();
      MAC_doCsmaProc(MAC_IFS_TYPE_NONE); 
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
     
   if (MAC_buildPDU(&pduInfo, NULL, MAC_BROADCAST_SHORT_ADDR) == MAC_STS_SUCCESS)                    
   {
       MAC_cntxt.txAttempCnt = 1;
       MAC_cntxt.cnfrmReqd = 0x0;
       MAC_cntxt.ackRequired = 0x0;
       MAC_cntxt.macPduLen = SYS_GET_BD_LEN(pduInfo.bdHndl);
       
       RADIO_transferPktToTxFifo(pduInfo.bdHndl);
       SYS_freeMem(pduInfo.bdHndl);

       MAC_csmaInit();
       MAC_doCsmaProc(MAC_IFS_TYPE_NONE);
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
void MAC_cmdFrameHandler(UINT8_t *cmd_p, 
                         UINT8_t cmdLen)                         
{
    if (cmdLen < MAC_CMD_ID_FIELD_LEN)
        return;
    
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
                PLTFRM_stopTimerA0( );                
                
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
                    
                    // Configure radio to filter subsequent frames on the basis
                    // of assigned short address and lpwmn id.
                    RADIO_cfgShortAddrFilter(MAC_PIB.macShortAddress);   
                    
                    MAC_cntxt.assocCnfrmParams.assocShortAddr = MAC_PIB.macShortAddress;
                }
                
                MAC_cntxt.assocCnfrmParams.status = assocSts;
                
                SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_MAC_ASSOC_CONFIRM;
             }
             break;
#endif


#if !defined(DEV_TYPE_RFD) && !defined(DEV_TYPE_FFD_NO_MESH)
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
                 * to accept or reject the unassociated device using an algorithm 
                 * outside the scope of this standard.
                 */
#ifdef MAC_STATS_ENA
                MAC_cntxt.assocReqRcvdCnt ++;
#endif          
                
                ADP_macAssocIndHndlr(MAC_cntxt.rcvdPktInfo.srcAddr.extAddr,
                                     cmd_p[1]);
             }
             break;   
#endif

#if 0
#if !defined(DEV_TYPE_RFD)
        case MAC_CMD_DATA_REQUEST:     
             {
                /*
                 * The data request command is sent by a device to request data 
                 * from the LPWMN coordinator or a coordinator.
                 */ 
               
                /*
                 * This implementation can process only one data-request command
                 * at a time.
                 */
                if (MAC_cntxt.commStsIndParams.dstAddrMode != MAC_ADDRESS_TYPE_UNKNOWN)
                {
                     MAC_cntxt.commStsIndParams.dstAddrMode = MAC_cntxt.rcvdPktInfo.srcAddrMode;
                     MAC_cntxt.commStsIndParams.dstAddr = MAC_cntxt.rcvdPktInfo.srcAddr;
                     SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_DATA_REQ_CMD_RCVD;
                }
             }
             break;
#endif
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


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */                     
UINT8_t MAC_processRcvdPdu(UINT8_t bdHndl, SINT16_t rssi, SINT16_t correlation)
{
  UINT8_t addrMode, rc = 1,
          macPduLen = SYS_GET_BD_LEN(bdHndl),
          dstShortAddrHdrOff = 0x0, srcShortAddrHdrOff = 0x0;
  UINT8_t frameType;
  UINT16_t hdrOff;
  MAC_addr_s destAddr;
  UINT8_t *macPdu_p = SYS_GET_BD_BUFF(bdHndl);
  

  // BD buffer has a two bytes prefix.
  macPdu_p += (SYS_TRSCN_Q_HNDL_FIELD_LEN + SYS_TRSCN_TYPE_FIELD_LEN);
  macPduLen -= (SYS_TRSCN_Q_HNDL_FIELD_LEN + SYS_TRSCN_TYPE_FIELD_LEN);
  
    
  // This function will return 0 if it drops the packet. The caller will free
  // the packet.
  
  // process rssi and correlation values
  // <TODO>

  
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
  
   hdrOff = MAC_PDU_HDR_FC_FIELD_LEN + MAC_PDU_HDR_SEQ_NR_FIELD_LEN; // Beyond the DSN field
  
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
       
       // MAC_cntxt.rcvdPktInfo.destLPWMNId = destLPWMNId;
       
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
           UINT16_t lpwmnId;
           /*
            * If this field is set to zero, then the LPWMN Identifier field shall
            * be present if and only if the corresponding address is present.
            */
           
           lpwmnId = macPdu_p[hdrOff + 1];
           lpwmnId = (lpwmnId << 8) | macPdu_p[hdrOff];
           MAC_cntxt.rcvdPktInfo.srcLPWMNId = lpwmnId;
           hdrOff += MAC_LPWMN_ID_LEN; 
       }
       
       if (addrMode == MAC_ADDRESS_MODE_SHORT_ADDR)
       {
           UINT16_t shortAddr = macPdu_p[hdrOff + 1];
           shortAddr = (shortAddr << 8) | macPdu_p[hdrOff];
           MAC_cntxt.rcvdPktInfo.srcAddr.shortAddr = shortAddr;
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
   
   frameType = (*macPdu_p) & MAC_FC_FRAME_TYPE_BITS_SHIFT_MSK;
   
#ifdef FLASH_LED_ON_PKT_RX_TX
   PLTFRM_ON_LED_1();
#endif

   // macPdu_p is pointing at the frame control field
   switch (frameType)
   {
      case MAC_FRAME_TYPE_CMD:
           {
              MAC_cmdFrameHandler(macPdu_p + hdrOff, 
                                  macPduLen - hdrOff);
              rc = 0;  // Caller will free memory
           }
           break;
              
      case MAC_FRAME_TYPE_ACK:
           {
              if (MAC_cntxt.txModState == MAC_TX_MOD_STATE_WAIT_ACK)
              {
                  /*
                   * ACK frame MAC PDU format
                   * FCF (2 bytes) | DSN (1 byte) | RSSI | CORRELATION
                   */
                  if (macPdu_p[2] == MAC_cntxt.expAckDSN)
                  {              
                      // ACK has expected DSN
#ifdef DEV_TYPE_RFD
                      MAC_cntxt.txFlrCnt = 0x0;
#endif
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
                  
                  // Check the superframe specification field
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
                           entry_p->rssi = macPdu_p[macPduLen];
                           if (entry_p->rssi >= 128)
                           {
                               // negative number
                               entry_p->rssi = -(255 - entry_p->rssi + 1);
                           }
                           
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

#if !defined(DEV_TYPE_RFD) && !defined(DEV_TYPE_FFD_NO_MESH) && !defined(LPWMN_COORD_NO_MESH)
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
               MAC_cntxt.rcvdPktInfo.correlation = correlation;
               MAC_cntxt.rcvdPktInfo.bdHndl = bdHndl;

               PLTFRM_LOG("<%s> d-pkt s:0x%x/l:%d/r:%d/c:%d>\n\r",
                          __FUNCTION__, MAC_cntxt.rcvdPktInfo.srcAddr.shortAddr,
                          macPduLen, rssi, correlation);

               SYS_ADJ_BD_HEAD(bdHndl, hdrOff);
               
               // BD buffer still has a two bytes prefix for queue handle and 
               // transaction type
               
               {
                  UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);
                  *buff_p = SYS_MAX_BUFF_DESC_CNT;  // init handle
               }
               
               /*
                * If the valid frame is a data frame, the MAC sublayer shall pass 
                * the frame to the next higher layer. This is achieved by issuing 
                * the MCPS-DATA.indication primitive containing the frame info.
                */
               SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_DATA_INDICATION;
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


/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */                      

void MAC_rxErrRecovery( )
{
#ifdef MAC_STATS_ENA
   MAC_cntxt.rxErrRecoveryCnt ++;
#endif  
   RADIO_flushRx();
   RADIO_hardRxStart(); 
   RADIO_setCurrState(RADIO_STATE_RX_ON);
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
void MAC_processRxFifo(void)
{
   UINT8_t pktLen = 0, pduCnt = 0;
      
   // This function assumes that it is called right after a RX_FRM_DONE
   // exception occurs which implies that there is no incomplete frame
   // in the RX FIFO
   
   /*
    * RX_FRM_DONE exception is raised when a complete frame has been received. 
    * I.E the number of bytes set by the length field is received.
    * Order of exceptions when a packet is received
    *   > SFD
    *   > RX_FRM_ACCEPTED
    *   > SRC_MATCH_DONE
    *   > RX_FRM_DONE
    *
    * When frame filtering is enabled and the filtering algorithm accepts a 
    * received frame, an RX_FRM_ACCEPTED exception will be generated. It will not 
    * be generated if frame filtering is disabled or RX_OVERFLOW or RX_FRM_ABORTED 
    * is generated before the filtering result is known.
    *
    * If a frame is rejected, CC2520 will only start searching for a new frame 
    * after the rejected frame has been completely received (as defined by the 
    * length field) to avoid detecting false SFDs within the frame. Note that
    * rejected frames can generate RX overflow if it occurs before the frame is 
    * rejected.
    */
              
   /*
    * Get the number of bytes in the RX FIFO
    * The RX FIFO can hold one or more received frames, provided that the total 
    * number of bytes is 128 or less.
    */
   
   
   while (1)
   {
      if (pktLen == 0x0)
      {
          UINT8_t rxFifoLen;
          /*
           * When AUTOCRC=1 and APPEND_DATA_MODE=0, the format
           * of each frame stored in the RX FIFO is :
           *    > Length Field (1 byte) (value N)
           *    > MAC PDU excluding FCS (N-2 bytes)
           *    > RSSI (1 byte)
           *    > CRC-OK (1 bit) | CORRELATION (7 bits)
           */
        
          cc2520_memRead(CC2520_RXFIFOCNT, 0x1, &rxFifoLen);
          if (rxFifoLen > (CC2520_RX_FIFO_SIG_INFO_LEN + 1))
          {
              UINT8_t bdHndl;
              pduCnt ++;
              CC2520_readRxFifo(1, &pktLen);
              if (pktLen > CC2520_RX_FIFO_SIG_INFO_LEN)
              {
                  bdHndl = SYS_allocMem(SYS_TRSCN_Q_HNDL_FIELD_LEN 
                                        + SYS_TRSCN_TYPE_FIELD_LEN
                                        + pktLen);              
                  if (SYS_BD_HNDL_IS_VALID(bdHndl))
                  {
                      UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);
                      const UINT16_t off = SYS_TRSCN_Q_HNDL_FIELD_LEN + SYS_TRSCN_TYPE_FIELD_LEN;
                      CC2520_readRxFifo(pktLen, buff_p + off);
                      if (buff_p[off + pktLen - 1] & 0x80)
                      {
                          SINT16_t rssi;
                          SINT16_t correlation;
#ifdef MAC_STATS_ENA
                          MAC_cntxt.goodFcsCnt ++;
#endif                    
                          
                          // CRC is ok, process this frame
                          buff_p[off + pktLen - 1] &= (~(0x80));  // clear the CRC-OK bit

                          /*
                           * CC2520 has a built-in RSSI (Received Signal Strength Indication) which
                           * calculates an 8 bit signed digital value that can be read from a register
                           * or automatically appended to received frames. The RSSI value is the result
                           * of averaging the received power over 8 symbol periods (128 micro seconds)
                           * as specified by IEEE 802.15.4.
                           *
                           * The RSSI value is a 2�s complement signed number on a logarithmic scale with
                           * 1dB steps.
                           *
                           * To find the actual signal power P at the RF pins with reasonable accuracy,
                           * an offset has to be added to the RSSI value.
                           *      P = RSSI - OFFSET [dBm]
                           *
                           * The offset is an empirical value which is found during characterization and
                           * is approximately 76 dBm for the CC2520 reference design. E.g. reading a RSSI
                           * value of -10 from the RSSI register means that the RF input power is
                           * approximately -86 dBm.
                           *
                           * Note that this offset seems to depend on the board design / antenna etc.
                           * This figure of 76 dBm does not apply to all boards.
                           */
                          {
                              SINT8_t tmpS8 = buff_p[off + pktLen - 2];
                              rssi = tmpS8;
                              rssi -= (CC2520_RSSI_OFFSET_VAL - 10);
                          }


                          /*
                           * CC2520 therefore also provides an average correlation value for each incoming
                           * frame, based on the 8 first symbols following the SFD. This unsigned 7-bit
                           * value can be looked upon as a measurement of the �chip error rate,� although
                           * CC2520 does not do chip decision.
                           * As described in section 20.3.4, the average correlation value for the 8 first
                           * symbols is appended to each received frame together with the RSSI and CRC OK/
                           * not OK when MDMCTRL0.AUTOCRC is set. A correlation value of ~110 indicates a
                           * maximum quality frame while a value of ~50 is typically the lowest quality
                           * frames detectable by CC2520.
                           */
                          
                          {
                        	  UINT8_t tempU8 = buff_p[off + pktLen - 1];
                              tempU8 &= ~0x80;
                              correlation = tempU8;
                          }

                          SYS_ADJ_BD_TAIL(bdHndl, CC2520_RX_FIFO_SIG_INFO_LEN);
                          
                          if (MAC_processRcvdPdu(bdHndl, rssi, correlation) == 0x0)
                          {
                              SYS_freeMem(bdHndl); 
                          }
                      }
                      else
                      {
#ifdef MAC_STATS_ENA
                          MAC_cntxt.badFcsCnt ++;
#endif  
                          SYS_freeMem(bdHndl);
                      }
                      pktLen = 0x0;
                  }
                  else
                  {
#ifdef MAC_STATS_ENA
                     MAC_cntxt.rxMemAllocFlrCnt ++;
#endif
                     MAC_rxErrRecovery();
                     break;                    
                  }
              }
              else
              {
                  // Something wrong !!
                  MAC_rxErrRecovery();
                  break;                
              }
          }
          else
          {
              // Something wrong !!
#ifdef MAC_STATS_ENA            
              MAC_cntxt.MAC_emptyFifoCnt ++;
#endif              
              if (pduCnt < 1)
                  MAC_rxErrRecovery();
              break;
          }
      }
   }
        
   /*
    * The SFLUSHRX command strobe resets the RX FIFO, removing all received 
    * frames, and clearing all counters, status signals and sticky error 
    * conditions.
    */
   // cc2520_insStrobe(CC2520_INS_SFLUSHRX);   
   
   {
      UINT8_t regVal = 0x0;
      // Clear all exceptions
      cc2520_memWrite(CC2520_EXCFLAG0, 0x1, &regVal); 
      cc2520_memWrite(CC2520_EXCFLAG1, 0x1, &regVal); 
   }
   
#if 0   
   cc2520_memRead(CC2520_EXCFLAG0, 0x1, &_excflag0); 
   cc2520_memRead(CC2520_EXCFLAG1, 0x1, &_excflag1);
   cc2520_memRead(CC2520_EXCFLAG2, 0x1, &_excflag2);
   cc2520_memRead(CC2520_RXFIFOCNT, 0x1, &_rxfifocnt);
   cc2520_memRead(CC2520_FSMSTAT0, 0x1, &_fsmstat0);
   cc2520_memRead(CC2520_FSMSTAT1, 0x1, &_fsmstat1);
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
void MAC_rcvdFrameHndlr(void)
{
#ifdef MAC_STATS_ENA
   MAC_cntxt.rcvdFrameCnt ++;
#endif  
   
   MAC_processRxFifo();
   
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
void MAC_evtHndlr(UINT16_t globalEvtId)
{
    switch (globalEvtId)
    {
     
       case SYS_GLOBAL_EVT_MAC_BCN_REQUEST:
            {
               MAC_buildSendBeaconPdu(); 
            }
            break;
                        
       case SYS_GLOBAL_EVT_MAC_RETRY_CCA:
            {
               MAC_doCsmaProc(MAC_IFS_TYPE_NONE);
            }
            break;
            
       case SYS_GLOBAL_EVT_MAC_FRAME_RX_DONE:
            {
               MAC_rcvdFrameHndlr( );
            }
            break;
                  
       case SYS_GLOBAL_EVT_MAC_FRAME_TX_DONE: 
            {
               if (MAC_cntxt.txModState == MAC_TX_MOD_STATE_WAIT_TX_OVER)
                   MAC_txOverEvtHndlr(1); 
               else
               {
                   UINT8_t regVal = 0x0;       
                   // Clear the TX_FRM_DONE exception in the cc2520's EXCFLAG0 register
                   cc2520_memWrite(CC2520_EXCFLAG0, 0x1, &regVal);      
#ifdef MAC_STATS_ENA                  
                   MAC_cntxt.unExpFrameTxDoneEvtCnt ++;
#endif
               }    
            }
            break;
#if 0
#ifndef DEV_TYPE_RFD
       case SYS_GLOBAL_EVT_MAC_DATA_REQ_CMD_RCVD:
            {
               if (MAC_cntxt.commStsIndParams.dstAddrMode >= MAC_ADDRESS_TYPE_SHORT)
                   MAC_processDataRequestCmd();
            }
            break;
#endif
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
void MAC_cleanUpScanProc(void)
{
   MAC_cntxt.scanInProgress = 0x0;
   
#ifdef MAC_STATS_ENA
   MAC_cntxt.scanCleanUpCnt ++;
#endif   
   
   // Turn the radio off
   RADIO_hardRxStop();   
   RADIO_setCurrState(RADIO_STATE_IDLE);
   
   // Restore default filtering (Allow CMD and DATA frames)
   {
      UINT8_t regVal = CC2520_FRMFILT1_ACCEPT_FT_3_MAC_CMD_BIT_MSK
                       | CC2520_FRMFILT1_ACCEPT_FT_1_DATA_BIT_MSK;
      cc2520_memWrite(CC2520_FRMFILT1, 0x1, &regVal);
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
void MAC_doActiveScan(void)
{
   MAC_sts_t sts = MAC_STS_SUCCESS;
   UINT8_t channBit = 0x0, sendCnfrm = 0x0;

   PLTFRM_LOG("<%s> cbm:0x%x/ubc:%d/lsc:0x%x\r\n",
              __FUNCTION__, MAC_cntxt.scanChannelsMsk,
              MAC_cntxt.uniqueBcnRcvdCnt, MAC_cntxt.scanChan);
        
   if (MAC_cntxt.scanChan != 0x1f)
       channBit = (MAC_cntxt.scanChan - PHY_2450_MHZ_BAND_PAGE_0_CHAN_11) + 1;
   /*
    * An active scan over a specified set of channels is requested using 
    * the MLME-SCAN.request primitive with the ScanType parameter set to 
    * indicate an active scan. 
    */
  
    // Get the first requested channel
    while ((channBit < PHY_2450_MHZ_BAND_CHANNEL_CNT)
           && (MAC_cntxt.scanChannelsMsk & (1 << channBit)) == 0x0)
    {
        channBit ++;
    }
     
    if (channBit >= PHY_2450_MHZ_BAND_CHANNEL_CNT)
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
        MAC_cntxt.scanChan = channBit + PHY_2450_MHZ_BAND_PAGE_0_CHAN_11;         
         
        /*
         * For each channel, the device shall first  switch to the channel, by 
         * setting phyCurrentChannel and phyCurrentPage accordingly.
         */
        if (RADIO_setChan(MAC_cntxt.scanChan))
        {
            PHY_PIB.phyCurrentChannel = MAC_cntxt.scanChan;
        }
        else
        {
            sts = MAC_STS_RADIO_HW_ERR;
            sendCnfrm = 1;
            goto _end;
        }
        
        // Flush the RX FIFO in case we have a beacon lying around in the RX FIFO
        RADIO_flushRx();
        
        /*
         * During an active or passive scan, the MAC sublayer shall discard 
         * all frames received over the PHY data service that are not beacon 
         * frames.
         */ 
        
        // Configure cc2520 to accept beacon frames only
        if (MAC_cntxt.scanInProgress == 0x0)
        {
            /*
             * <cc2520 spec, 20.3.2 Frame Filtering>
             * Beacon frames (0) are only accepted when:
             *   > FRMFILT1.ACCEPT_FT0_BEACON = 1
             *   > Length byte >= 9
             *   > The destination address mode is 0 (no destination address)
             *   > The source address mode is 2 or 3 (i.e. a source address is included)
             *   > The source LPWMN ID matches LOCAL_LPWMNID, or LOCAL_LPWMNID equals 0xFFFF
             */
            UINT8_t regVal = CC2520_FRMFILT1_ACCEPT_FT_0_BEACON_BIT_MSK;
            cc2520_memWrite(CC2520_FRMFILT1, 0x1, &regVal);
        }
        
        if (RADIO_getCurrState() == RADIO_STATE_IDLE)
        {
            RADIO_hardRxStart();
            RADIO_setCurrState(RADIO_STATE_RX_ON);
        }
        
        MAC_cntxt.scanInProgress = 0x1;
        
        /*
         * For an active scan, the device shall send a beacon request command, 
         * as described in 5.3.7.
         */
        
        sts = MAC_sendBcnRequest();   // TODO - tx fifo needs to be setup only once  
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
void MAC_rxOvflProc( )
{
   UINT8_t fsmStat;

   cc2520_memRead(CC2520_FSMSTAT0, 0x1, &fsmStat);
   if (fsmStat == CC2520_RADIO_STATE_RX_OVERFLOW)
   {    
#ifdef MAC_STATS_ENA
       MAC_cntxt.rxOvflCnt ++;
#endif       
       /*
        * An attempt has been made by RF_core to write to RX FIFO while the 
        * RX FIFO is full. The byte that was attempted written to the RX FIFO 
        * is lost. Reception of data is aborted and the FSM enters the 
        * rx_overflow state. Recommended action is to issue a SFLUSHRX command 
        * strobe to empty the RX FIFO and restart RX.
        *
        * Note that rejected frames can generate RX overflow if it occurs          * before the frame is rejected.
        */
               
       cc2520_insStrobe(CC2520_INS_SFLUSHRX);
       cc2520_hardRxStart();
   }       
   else
   {
#ifdef MAC_STATS_ENA
       MAC_cntxt.supuriousRxOvflCnt ++;
#endif      
   }
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
   if (params_p->coordAddrMode == MAC_ADDRESS_TYPE_SHORT)
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
    * The MAC sublayer of an unassociated device shall initiate the association 
    * procedure by sending an association request command, as described in 5.3.1, 
    * to the coordinator of an existing LPWMN; if the association request command
    * cannot be sent due to a channel access failure, the MAC sublayer shall notify 
    * the next higher layer.
    */
   sts = MAC_buildSendAssocnReqCmd(params_p);
   if (sts != MAC_STS_SUCCESS)
       MAC_sendAssocCnfrm(sts, MAC_BROADCAST_SHORT_ADDR);
   
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
UINT8_t MAC_addToAssocRFDList(UINT16_t shortAddr)
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
UINT8_t MAC_lookUpAssocRFDList(UINT16_t shortAddr)
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




#ifdef DEV_TYPE_RFD

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
MAC_sts_t MLME_pollRequest(void)
{
    MAC_sts_t sts;
    // This API does not have any parameters.

    // if (MAC_cntxt.coordPollInProgress)
    //    return MAC_STS_POLL_IN_PROGRESS;

    /*
     * On receipt of the MLME-POLL.request primitive, the MLME requests
     * data from the coordinator, as described in 5.1.6.3.
     *
     * If the data request command originated from an MLME-POLL.request
     * primitive, the MLME shall perform the security process on the data
     * request command based on the SecurityLevel, KeyIdMode, KeySource,
     * and KeyIndex parameters of the MLME-POLL.request primitive, according
     * to 7.2.1.
     *
     * <Action on the coordinator>
     * If the acknowledgment frame is being sent in response to a received
     * data request command, the device sending the acknowledgment frame shall
     * determine whether it has data pending for the recipient. If the device
     * can determine this before sending the acknowledgment frame, as described
     * in 5.1.6.4.2, it shall set the Frame Pending field according to whether
     * there is pending data. Otherwise, the Frame Pending field shall be set to
     * one.
     *
     * <Action on the RFD>
     *
     * 5.1.6.3 Extracting pending data from a coordinator
     *
     * On receipt of the acknowledgment frame with the Frame Pending field set
     * to one, a device shall enable its receiver for at most macMaxFrameTotalWaitTime
     * to receive the corresponding data frame from the coordinator. If there is an
     * actual data frame pending within the coordinator for the requesting device, the
     * coordinator shall send the frame to the device using one of the mechanisms
     * described in this sub-clause. If there is no data frame pending for the
     * requesting device, the coordinator shall send a data frame without requesting
     * acknowledgment to the device containing a zero length payload, indicating that
     * no data are present, using one of the mechanisms described in this sub-clause.
     *
     * In this implementation, the coordinator will send a pending data frame using
     * CSMA/CA.
     *
     * On receipt of the acknowledgment frame with the Frame Pending field set
     * to zero, the device shall conclude that there are no data pending at the
     * coordinator.
     *
     * If the requesting device does not receive a data frame from the coordinator
     * within macMaxFrameTotalWaitTime or if the requesting device receives a data frame
     * from the coordinator with a zero length payload, it shall conclude that there are
     * no data pending at the coordinator. If the requesting device does receive a data
     * frame from the coordinator, it shall send an acknowledgment frame, if requested,
     * thus confirming receipt. If the Frame Pending field of the data frame received
     * from the coordinator is set to one, the device still has more data pending with
     * the coordinator. In this case it may extract the data by sending a new data request
     * command to the coordinator.
     */

    sts = MAC_buildSendDataReqCmd();
    if (sts == MAC_STS_SUCCESS)
    {
        MAC_cntxt.pollReqSent = 0x1;
        // MAC_cntxt.coordPollInProgress = 0x1;
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
 ********************************************************************
 */
MAC_sts_t MAC_init(void)
{
   memset(&MAC_cntxt, 0, sizeof(MAC_cntxt_s));
   MAC_cntxt.txModState = MAC_TX_MOD_STATE_IDLE;
   MAC_PIB_init(); 

#ifndef LPWMN_COORD
   // On a lpwmn coordinator, the short address and lpwmn id
   // are setup in the NM_start() function in node_mgr.c.
   RADIO_cfgShortAddrFilter(MAC_PIB.macShortAddress);
   RADIO_cfgLPWMNIdFilter(MAC_PIB.macLPWMNId);
#endif
   
#if !defined(LPWMN_COORD) && !defined(DEV_TYPE_RFD)
    memset(MAC_cntxt.assocRFDList, 0xff,
           MAC_MAX_ASSOC_RFD_CNT*MAC_SHORT_ADDR_LEN);
#endif


   return MAC_STS_SUCCESS;
}


