/*
 * File Name: adp.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Sep/7/2013
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
   
#include <typedefs.h>
#include <mesh.h>
#include <ipv4.h>
#include <node_mgr.h>
#include <udp.h>
#include <system.h>
#include <util.h>
#include <radio.h>
#include <pltfrm.h>
#include <mac.h>
#include <mac_pib.h>
#include <adp.h>
#include <aodvl.h>
#include <gw.h>
#include <dis.h>


/*
 * Mesh header
 *   [Hops Left][Dest short address (2)][Source short address (2)]
 *  
 * Frag header
 *   [Tag][offset][
 *
 * Compressed IPv4 header + UDP header
 *   [IPv4 header without options][UDP header (8)][UDP payload]
 * 
 * - IPv4 version will be compressed to 0 bits.
 * - IP header field will be compressed to 0 bits
 * - TOS field will be compressed to 0 bits (not used)
 * - Packet length (hdr + data) field will be compressed to 0 bits  
 * - 16 bit fragment id will be compressed to 0 bits (no support for IPv4 fragments)
 * - 3 bit flags field will be compressed to 0 bits (not used)
 * - 13 bit Fragment offset will be compressed to 0 bits (no support for IPv4 fragments)
 * - 8 bit TTL is compressed to 0 bits  
 * - Protocol field will be compressed if next header type is UDP.   
 * - 16 bit Header checksum will be compressed to 0 bits   
 * - IPv4 addresses (fully inline/ fully elided / nwk id / host id)
 * 
 * 4oLPWMN  (IPv4 over low power wireless mesh network) 
 * IPv4 addresses assigned to nodes will have a 16 bit network id and
 * 16 bit host id. The host id will be derieved from the node's short 
 * address. The network mask for the whole network will be 255.255.0.0.
 *
 * 4oLPWMN coordinator address have short address 0x1 (not 0x0). No node will
 * have short address 0x0.
 *
 * DAM / SAM
 *   00 - Completely inline
 *   01 - 16 bit network prefix
 *   10 - 16 bit host id
 *   11 - Completely elided (Derieved from fixed prefix and short address)   
 * 
 * NHC
 *   1 - Protocol field compressed (is UDP)
 *   0 - Protocol field carried inline (!= UDP)
 *
 * <<< UDP header compression >>>
 *
 * C: Checksum:
 *    0: All 16 bits of Checksum are carried in-line.
 *    1: All 16 bits of Checksum are elided.  The Checksum is recovered
 *       by recomputing it on the 6LoWPAN termination point.
 *
 */

// UINT8_t __rxenable1_duringadptx = 0;


ADP_cntxt_s ADP_cntxt;


#if !defined(DEV_TYPE_RFD) && !defined(DEV_TYPE_SNIFFER)
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT16_t ADP_getRoute(UINT16_t destAddr)
{
   UINT8_t rc;

   // First check if the destination is an RFD associated with this
   // node
#ifdef LPWMN_COORD
   rc = NM_lookUpLocalRFD(destAddr);
#else
   rc = MAC_lookUpAssocRFDList(destAddr);
#endif

   if (rc)
       return destAddr;
   else
       return AODVL_getRoute(destAddr);
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
void ADP_clearAppTxParams(void)
{
   ADP_cntxt.txParams.destShortAddr = MAC_BROADCAST_SHORT_ADDR;
   ADP_cntxt.txParams.nextHopAddr = MAC_BROADCAST_SHORT_ADDR;
   ADP_cntxt.txParams.bdHndl = SYS_MAX_BUFF_DESC_CNT;
   ADP_cntxt.txParams.localOrig = 0x1;
   memset(&ADP_cntxt.fragInfo, 0, sizeof(ADP_dataReqFragInfo_s));
}


#ifdef LPWMN_COORD
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t ADP_addrCheck(UINT16_t shortAddr)
{
   return ((NM_lookUpShortAddr(shortAddr) != NULL) ? 0x1 : 0x0);
}
#endif


#ifdef IPv4_SUPPORT_ENA
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT16_t ADP_getDestShortAddr(UINT32_t *destIp_p)
{
   UINT16_t dstShortAddr = MAC_BROADCAST_SHORT_ADDR;
   UINT32_t netMsk = ADP_cntxt.ipv4NetAddr;
   UINT32_t tmpU32 = *destIp_p;
   
   netMsk = (netMsk << IPv4_NET_ADDR_SHIFT) & IPv4_NET_MASK;
   
   tmpU32 &= IPv4_NET_MASK;
   
   if (tmpU32 == netMsk)
   { 
       dstShortAddr = (*destIp_p) & MAC_SHORT_ADDR_MSK;   
   }
   else
   { 
#ifndef LPWMN_COORD
       dstShortAddr = LPWMN_COORD_SHORT_ADDR;   
#endif      
   }
   
   return dstShortAddr;
}
#endif


#if !defined(DEV_TYPE_RFD) && !defined(DEV_TYPE_SNIFFER)
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void ADP_scheduleRtErrTrx(UINT16_t destAddr, UINT16_t pktOrigAddr)
{
    UINT8_t bdHndl;

    bdHndl = SYS_allocMem(SYS_TRSCN_Q_HNDL_FIELD_LEN
                          + ADP_TRSCN_TYPE_FIELD_LEN
                          + MAC_SHORT_ADDR_LEN
                          + MAC_SHORT_ADDR_LEN);
    if (SYS_BD_HNDL_IS_VALID(bdHndl))
    {
        UINT8_t *bdBuff_p = SYS_GET_BD_BUFF(bdHndl);

        *(bdBuff_p) = SYS_MAX_BUFF_DESC_CNT;
        bdBuff_p += SYS_TRSCN_Q_HNDL_FIELD_LEN;

        *(bdBuff_p) = ADP_TRSCN_TYPE_RERR_TRX_REQ;
        bdBuff_p += ADP_TRSCN_TYPE_FIELD_LEN;

        UTIL_htons(bdBuff_p, destAddr);
        bdBuff_p += MAC_SHORT_ADDR_LEN;

        UTIL_htons(bdBuff_p, pktOrigAddr);
        bdBuff_p += MAC_SHORT_ADDR_LEN;

        SYS_addToTrscnsQ(&(ADP_cntxt.trscnQ), bdHndl);
        SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ADP_TRSCN_PENDING;
    }

    return;
}
#endif


#ifndef DEV_TYPE_SNIFFER
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void ADP_sendSigPkt(UINT8_t bdHndl, UINT16_t nextHop, UINT8_t ackReqd)
{
   MCPS_dataReqParams_s macTxReqParams;

   macTxReqParams.destShortAddr = nextHop;
   macTxReqParams.destAddrMode = MAC_ADDRESS_MODE_SHORT_ADDR;
   macTxReqParams.srcAddrMode = MAC_ADDRESS_MODE_SHORT_ADDR;
   macTxReqParams.ackTx = ackReqd ? 0x1 : 0x0;
   macTxReqParams.destLPWMNIdBc = 0x0;
   macTxReqParams.indirectTx = 0x0;
   macTxReqParams.msduBDHndl = bdHndl;

   ADP_cntxt.pendTxReqSrc = ADP_TX_REQ_SRC_AODVL;

   MCPS_dataReq(&macTxReqParams);
}
#endif


#if !defined(DEV_TYPE_RFD) && !defined(DEV_TYPE_SNIFFER)
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void ADP_procRtDiscOpnRslt(UINT16_t destAddr,
                           UINT16_t nextHopAddr)
{
   SYS_trscnQ_s *trscnQ_p;
   UINT8_t bdHndl;

   // Update PIB counter
   if (nextHopAddr == MAC_BROADCAST_SHORT_ADDR)
	   ADP_cntxt.rtDiscFlrCnt ++;
   else
	   ADP_cntxt.rtDiscSuccessCnt ++;

#if defined(DEV_TYPE_FFD) && defined(FFD_RBT_ON_RPT_RT_FLR_TO_COORD)
   {
      // rkris@wisense.in / june/8/15 - recovery
      if (destAddr == LPWMN_COORD_SHORT_ADDR)
      {
          if (nextHopAddr == MAC_BROADCAST_SHORT_ADDR)
          {
              ADP_cntxt.rtDiscToCoordFlrCnt ++;

              if (ADP_cntxt.rtDiscToCoordFlrCnt > ADP_RT_DISC_TO_COORD_FLR_CNT_THRESHOLD)
              {
                  PLTFRM_resetSelf();
              }
          }
          else
              ADP_cntxt.rtDiscToCoordFlrCnt = 0;
      }
   }
#endif

      
   if (ADP_cntxt.txParams.destShortAddr == destAddr)
   {
       if (nextHopAddr == MAC_BROADCAST_SHORT_ADDR) 
       {
           ADP_cntxt.cnfrmSts.sts = ADP_STS_RT_DISC_FAILED;

           if (!(SYS_TRSCN_Q_HNDL_VALID(ADP_cntxt.txParams.bdHndl)))
           {
               SYS_fatal(SYS_FATAL_ERR_65);
           }

           SYS_freeMem(ADP_cntxt.txParams.bdHndl);

#if defined(LPWMN_COORD) && defined(GW_ENA)
           if (ADP_cntxt.txParams.localOrig == 0)
               GW_relayCnfrm(LPWMN_ADP_MODULE, (UINT8_t)ADP_STS_RT_DISC_FAILED);
           else
#endif
           SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_ADP_DATA_CONFIRM;

           ADP_clearAppTxParams();
       }
       else
       {
           ADP_cntxt.txParams.nextHopAddr = nextHopAddr;
            
           SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_APP_DATA_REQ_POST_RT_DISC;
       }
   }

   // Now check the list of to be forwarded packets which are pending 
   // route discovery
   trscnQ_p = &(ADP_cntxt.rtPendFwdPktTrscnQ);
   bdHndl = SYS_GET_TRSCN_Q_HEAD(trscnQ_p);
   while (SYS_TRSCN_Q_IS_NOT_EMPTY(trscnQ_p))
   {
      UINT16_t inQDstAddr;
      UINT8_t *buff_p, trscnRmvd = 0, meshHdrOff;
            
      buff_p = SYS_GET_BD_BUFF(bdHndl);
      meshHdrOff = *(buff_p + SYS_TRSCN_Q_HNDL_FIELD_LEN);
      
      inQDstAddr = UTIL_ntohs(buff_p + meshHdrOff + ADP_MESH_HDR_HOP_COUNT_FIELD_LEN);
    
      if (inQDstAddr == destAddr) 
      {
          SYS_delFromTrscnsQ(trscnQ_p, bdHndl);
          trscnRmvd = 1;

          if (nextHopAddr == MAC_BROADCAST_SHORT_ADDR)
          {
              UINT8_t rc;
              UINT16_t pktOrigAddr = UTIL_ntohs(buff_p + meshHdrOff
                                                + ADP_MESH_HDR_HOP_COUNT_FIELD_LEN
                                                + MAC_SHORT_ADDR_LEN);
              SYS_freeMem(bdHndl);

              if (pktOrigAddr != MAC_PIB.macShortAddress)
              {
              // Do not send route error to locally associated RFD
#ifdef LPWMN_COORD
                  rc = NM_lookUpLocalRFD(pktOrigAddr);
#else
                  rc = MAC_lookUpAssocRFDList(pktOrigAddr);
#endif
                  if (rc == 0)
                  {
                      // Send a RERR message to the originator
                      ADP_scheduleRtErrTrx(destAddr, pktOrigAddr);
                  }
              }
          }
          else
          {
              UINT8_t *macDstShort_p;
              
              // Update the destination short addresses in the MAC header
              // Mesh hdr hop count and source MAC address has already been updated in ADP_pktFwdCheck()
          
              macDstShort_p = buff_p + SYS_TRSCN_Q_HNDL_FIELD_LEN + ADP_TRSCN_TYPE_FIELD_LEN;
              macDstShort_p += MAC_PDU_HDR_DEST_ADDR_OFF;

              // Update destination MAC address to next hop
              macDstShort_p[0] = (UINT8_t)nextHopAddr;
              macDstShort_p[1] = (UINT8_t)((nextHopAddr >> 8) & 0xff);
                                
              // Set the transaction type
              *(buff_p + SYS_TRSCN_Q_HNDL_FIELD_LEN) = ADP_TRSCN_TYPE_MESH_PDU;
                            
              SYS_addToTrscnsQ(&(ADP_cntxt.trscnQ), bdHndl);  
              SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ADP_TRSCN_PENDING;
          }
         
          if (trscnRmvd)
              bdHndl = SYS_GET_TRSCN_Q_HEAD(trscnQ_p); 
      }
      else
      {
          bdHndl = SYS_GET_TRSCN_Q_NEXT(bdHndl);
          if (!(SYS_TRSCN_Q_HNDL_VALID(bdHndl)))
              break;  // Reached end of list
      }
   }

   // Now check the list of assoc messages pending route discovery
   // Message types in this list can be one of :
   //  > ADP_TRSCN_TYPE_ASSOC_REQ (from some node to LPWMN coord)
   //  > ADP_TRSCN_TYPE_ASSOC_RESP_TO_RELAY (from LPWMN coord to some node)
   // In both cases, the first two fields in the BD buffer (after the hndl/type
   // bytes) are the destinaton short address followed by the next hop address.
   
   trscnQ_p = &(ADP_cntxt.assocRtPendTrscnQ);
   bdHndl = SYS_GET_TRSCN_Q_HEAD(trscnQ_p);
   
   while (SYS_TRSCN_Q_IS_NOT_EMPTY(trscnQ_p))
   {
      UINT16_t inQDstAddr;
      UINT8_t *buff_p, trscnRmvd = 0;
            
      buff_p = SYS_GET_BD_BUFF(bdHndl);
      buff_p += (SYS_TRSCN_Q_HNDL_FIELD_LEN + ADP_TRSCN_TYPE_FIELD_LEN);
        
      inQDstAddr = UTIL_ntohs(buff_p);   // *((UINT16_t *)(buff_p));
    
      if (inQDstAddr == destAddr) 
      {
          SYS_delFromTrscnsQ(trscnQ_p, bdHndl);
          trscnRmvd = 1;

          if (nextHopAddr == MAC_BROADCAST_SHORT_ADDR)
          {
              SYS_freeMem(bdHndl);
          }
          else
          {
              buff_p += MAC_SHORT_ADDR_LEN;
              UTIL_htons(buff_p, nextHopAddr);   // *((UINT16_t *)(buff_p)) = nextHopAddr;
              SYS_addToTrscnsQ(&(ADP_cntxt.trscnQ), bdHndl);
              SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ADP_TRSCN_PENDING;
          }
         
          if (trscnRmvd)
              bdHndl = SYS_GET_TRSCN_Q_HEAD(trscnQ_p); 
      }
      else
      {
          bdHndl = SYS_GET_TRSCN_Q_NEXT(bdHndl);
          if (!(SYS_TRSCN_Q_HNDL_VALID(bdHndl)))
              break;  // Reached end of list
      }
   }
   
   return;
}
#endif
      

#ifndef DEV_TYPE_SNIFFER

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
ADP_sts_t ADP_appPyldTxReq(void)
{
   ADP_sts_t sts = ADP_STS_SUCCESS;
   UINT8_t *buff_p;

#ifndef LPWMN_COORD
   // On an LPWMN coord, this request can be from the GW interface or from
   // the app layer on the coord itself. On an RFD or FFD, this request
   // comes from the local APP layer only.
   ADP_cntxt.txParams.localOrig = 0x1;
#endif

#ifdef LPWMN_COORD
   if (ADP_cntxt.txParams.localOrig == 0)
   {
       SYS_PULL_BD_HEAD(ADP_cntxt.txParams.bdHndl, ADP_ELEMENT_TYPE_LEN);
   }
#endif

   buff_p = SYS_GET_BD_BUFF(ADP_cntxt.txParams.bdHndl);
   *buff_p = ADP_ELEMENT_TYPE_NORMAL_DATA;

   if (ADP_cntxt.txParams.destShortAddr == MAC_BROADCAST_SHORT_ADDR)
   {
       // Mesh wide broadcast request ...
	   ADP_cntxt.txParams.nextHopAddr = MAC_BROADCAST_SHORT_ADDR;
       return ADP_appPyldTxProc();
   }
    
#ifdef LPWMN_COORD
   // Check if destination address is a registered node or not
   if (ADP_addrCheck(ADP_cntxt.txParams.destShortAddr) == 0x0)
   {
       SYS_freeMem(ADP_cntxt.txParams.bdHndl);
       ADP_cntxt.txParams.bdHndl = SYS_MAX_BUFF_DESC_CNT;
       return ADP_STS_NODE_NOT_FOUND;
   }
#endif     
     
#ifdef DEV_TYPE_RFD
  /*
   * < From RFC 4944 >
   * Only "Full Function Devices" (FFDs) are expected to participate as routers in
   * a mesh. "Reduced Function Devices" (RFDs) limit themselves to discovering FFDs
   * and using them for all their forwarding, in a manner similar to how IP hosts
   * typically use default routers to forward all their off-link traffic. For an RFD
   * using mesh delivery, the "forwarder" is always the appropriate FFD.
   */

  if (MAC_PIB.macCoordShortAddress == MAC_BROADCAST_SHORT_ADDR)
  {
      SYS_freeMem(ADP_cntxt.txParams.bdHndl);
      ADP_cntxt.txParams.bdHndl = SYS_MAX_BUFF_DESC_CNT;
      return ADP_STS_NODE_NOT_ASSOCIATED;
  }

  ADP_cntxt.txParams.nextHopAddr = MAC_PIB.macCoordShortAddress;

  sts = ADP_appPyldTxProc();

#else
   ADP_cntxt.txParams.nextHopAddr = ADP_getRoute(ADP_cntxt.txParams.destShortAddr);
   if (ADP_cntxt.txParams.nextHopAddr != MAC_BROADCAST_SHORT_ADDR)
   {
       // Route exists. Request MAC to transmit data 
       sts = ADP_appPyldTxProc();

       if (sts != ADP_STS_SUCCESS)
       {
    	   ADP_cntxt.appPyldTxProcFlrCnt ++;
       }
   }
   else
   {
       if (AODVL_rtDiscRequest(ADP_cntxt.txParams.destShortAddr) == 0x0)
       {
           // AODVL is not able to accommodate another route discovery request
           SYS_freeMem(ADP_cntxt.txParams.bdHndl);
           ADP_clearAppTxParams();
           ADP_cntxt.rtDiscErrCnt ++;
           sts = ADP_STS_RT_DISC_OOM;
       }
       else
       {
           // Data request params will be available in ADP_cntxt.txParams since 
           // only higher layer cannot send another data request before MAC
           // confirms the previous one. This means the APP layer (SDU) need
           // not be stored in the route pending list.

    	   ADP_cntxt.rtDiscStartedCnt ++;
       }
   }
#endif

   return sts;
}
 
/*
 ********************************************************************
 *
 * 10.1.3 Frame Length field
 * The Frame Length field specifies the total number of octets contained in
 * the PSDU (i.e., PHY payload). It is a value between 0 and aMaxPHYPacketSize,
 * as described in 9.2.
 * In section "aMaxPHYPacketSize" is defined as 127.
 * The MAC PDU has the following components :-
 * > MAC header
 * > MAC payload
 * > FCS (2 bytes)
 * So the max number of bytes available for MAC header and MAC payload is 125
 * bytes.
 *
 ********************************************************************
 */
UINT8_t ADP_calcADPHdrLen(void)
{
   UINT8_t hdrLen = 0;

   if (ADP_cntxt.txParams.nextHopAddr != ADP_cntxt.txParams.destShortAddr)
   {
       // Prefix mesh header
       // Hop count | destination short address | source short address
       hdrLen += (ADP_ELEMENT_TYPE_LEN + ADP_MESH_HDR_LEN);
   } 
   else
   {
       if (ADP_cntxt.txParams.destShortAddr == MAC_BROADCAST_SHORT_ADDR)
       {
           // Mesh level broadcast request
           hdrLen += (ADP_ELEMENT_TYPE_LEN + ADP_MESH_BC_HDR_LEN);
       }
   }

   return hdrLen;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
ADP_sts_t ADP_sendFrag(MCPS_dataReqParams_s *macTxReqParams_p)
{
    UINT8_t *buff_p;
    UINT8_t adpBDHndl;
    UINT16_t currPyldLen;
    ADP_dataReqParams_s *adpTxP_p = &(ADP_cntxt.txParams);

    if (ADP_cntxt.fragInfo.fragCnt > 1)
    {
        if (ADP_cntxt.fragInfo.sduLenLeft >= ADP_cntxt.fragInfo.fragLen)
            currPyldLen = ADP_cntxt.fragInfo.fragLen;
        else
            currPyldLen = ADP_cntxt.fragInfo.sduLenLeft;
    }
    else
    {
        currPyldLen = ADP_cntxt.fragInfo.sduLenLeft;
    }

    adpBDHndl = SYS_allocMem(ADP_cntxt.fragInfo.adpHdrLen + currPyldLen);
    if (SYS_BD_HNDL_IS_VALID(adpBDHndl) == 0x0)
    {
        // OOM !!
        return ADP_STS_OOM;
    }

    {
        UINT16_t offset = 0;
        UINT8_t *srcBuff_p = SYS_GET_BD_BUFF(ADP_cntxt.txParams.bdHndl);

        if (ADP_cntxt.fragInfo.fragCnt > 1)
        {
            offset = ADP_cntxt.fragInfo.fragLen;
            offset *= ADP_cntxt.fragInfo.currFragIdx;
            srcBuff_p += offset;
        }

        memcpy(SYS_GET_BD_BUFF(adpBDHndl) + ADP_cntxt.fragInfo.adpHdrLen,
               srcBuff_p, currPyldLen);

        ADP_cntxt.fragInfo.sduLenLeft -= currPyldLen;
    }

    buff_p = SYS_GET_BD_BUFF(adpBDHndl);

    if (adpTxP_p->nextHopAddr != adpTxP_p->destShortAddr)
    {
        // Prefix mesh header
        // Hop count | destination short addr | source short addr

        *(buff_p) = ADP_ELEMENT_TYPE_MESH_HDR;
        buff_p += ADP_ELEMENT_TYPE_LEN;

        *(buff_p) = ADP_MAX_HOP_COUNT;
        buff_p += ADP_MESH_HDR_HOP_COUNT_FIELD_LEN;

        UTIL_htons(buff_p, adpTxP_p->destShortAddr);
        buff_p += MAC_SHORT_ADDR_LEN;

        UTIL_htons(buff_p, MAC_PIB.macShortAddress);
        buff_p += MAC_SHORT_ADDR_LEN;
    }
    else
    {
        if (adpTxP_p->destShortAddr == MAC_BROADCAST_SHORT_ADDR)
        {
            // Mesh broadcast
            *(buff_p) = ADP_ELEMENT_TYPE_MESH_BC_HDR;
            buff_p += ADP_ELEMENT_TYPE_LEN;

            *(buff_p) = adpTxP_p->meshBroadcastMaxHopCnt;
            buff_p += ADP_MESH_BC_HDR_HOP_COUNT_FIELD_LEN;

            *(buff_p) = (ADP_cntxt.meshBroadcastSeqNr ++);
            buff_p += ADP_MESH_BC_HDR_SEQ_NR_FIELD_LEN;

            UTIL_htons(buff_p, MAC_PIB.macShortAddress);
            buff_p += MAC_SHORT_ADDR_LEN;
        }
    }

    if (ADP_cntxt.fragInfo.fragCnt > 1)
    {
        UINT8_t byte;

        // Insert frag header
        *(buff_p) = ADP_ELEMENT_TYPE_FRAG_HDR;
        buff_p += ADP_ELEMENT_TYPE_LEN;

        byte = ADP_cntxt.fragInfo.fragTag;
        byte <<= ADP_FRAG_HDR_TAG_FIELD_SHIFT;
        if (ADP_cntxt.fragInfo.sduLenLeft == 0)
            byte |= ADP_FRAG_HDR_LAST_FRAG_IND_BIT;  // Last frag indication
        byte |= ADP_cntxt.fragInfo.currFragIdx;

        *(buff_p) = byte;
        buff_p += ADP_FRAG_HDR_LEN;
    }

    macTxReqParams_p->destShortAddr = ADP_cntxt.txParams.nextHopAddr;
#ifdef FFD_APP_SEND_DATA_BACK_TO_BACK
    macTxReqParams_p->ackTx = 0x0;
#else
    // No ack for broadcast frames
    macTxReqParams_p->ackTx = (adpTxP_p->destShortAddr == MAC_BROADCAST_SHORT_ADDR) ? 0x0 : 0x1;
#endif

#ifdef DEV_TYPE_RFD
    macTxReqParams_p->indirectTx = 0;
#else
    if (adpTxP_p->destShortAddr == MAC_BROADCAST_SHORT_ADDR)
        macTxReqParams_p->indirectTx = 0;
    else
    {
       if (macTxReqParams_p->destShortAddr == adpTxP_p->destShortAddr)
       {
           // Final destination is a neighbor. Check if neighbor is an RFD.
#ifdef LPWMN_COORD
           macTxReqParams_p->indirectTx = NM_lookUpLocalRFD(macTxReqParams_p->destShortAddr) ? 0x1 : 0x0;
#else
           macTxReqParams_p->indirectTx = MAC_lookUpAssocRFDList(macTxReqParams_p->destShortAddr) ? 0x1 : 0x0;
#endif
       }
    }
#endif

    macTxReqParams_p->destLPWMNIdBc = 0x0;
    macTxReqParams_p->msduBDHndl = adpBDHndl;

#ifdef LPWMN_COORD
    if (ADP_cntxt.txParams.localOrig == 0)
    {
        if (macTxReqParams_p->indirectTx)
        {
            // Bug fix - rkris@wisense.in / aug/13/15
            // MAC does not send confirmation for indirect transmissions, so we
            // need to delete the ADP copy. Note that indirect transmissions can't
            // be fragmented transmissions (not supported).
            SYS_freeMem(ADP_cntxt.txParams.bdHndl);
            ADP_cntxt.txParams.bdHndl = SYS_MAX_BUFF_DESC_CNT;
            GW_cntxt.relayFrameBdHndl = SYS_MAX_BUFF_DESC_CNT;
            ADP_cntxt.txParams.destShortAddr = MAC_BROADCAST_SHORT_ADDR;  // Bug fix: sep/6/15
            ADP_cntxt.txParams.nextHopAddr = MAC_BROADCAST_SHORT_ADDR;  // Bug fix: sep/6/15
            ADP_cntxt.pendTxReqSrc = ADP_TX_REQ_SRC_NONE;
        }
        else
            ADP_cntxt.pendTxReqSrc = ADP_TX_REQ_SRC_GW;
    }
    else
#endif
        ADP_cntxt.pendTxReqSrc = adpTxP_p->srcModId;   // ADP_TX_REQ_SRC_APP;

    MCPS_dataReq(macTxReqParams_p);

    return ADP_STS_SUCCESS;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void ADP_fragTxDoneHndlr(MCPS_dataConfirmParams_s *macCnfrmParams_p)
{
   ADP_sts_t sts = ADP_STS_SUCCESS;

   if (macCnfrmParams_p->status == MAC_STS_SUCCESS)
   {
       ADP_cntxt.fragInfo.currFragIdx ++;
       if (ADP_cntxt.fragInfo.currFragIdx >= ADP_cntxt.fragInfo.fragCnt)
       {
           // All frags sent ...
           ADP_cntxt.cnfrmSts.sts = ADP_STS_SUCCESS;
       }
       else
       {
           MCPS_dataReqParams_s macTxReqParams;

           macTxReqParams.indirectTx = 0;  // Bug fix - march/2/15 - rkris@wisense.in
           macTxReqParams.destAddrMode = MAC_ADDRESS_MODE_SHORT_ADDR;
           macTxReqParams.srcAddrMode = MAC_ADDRESS_MODE_SHORT_ADDR;

           sts = ADP_sendFrag(&macTxReqParams);
           if (sts == ADP_STS_SUCCESS)  // Frag built and sent ..
               return;
       }
   }
   else
   {
       sts = ADP_STS_TX_FLR;
   }

#ifdef DEV_TYPE_RFD
   ADP_cntxt.cnfrmSts.framePending = macCnfrmParams_p->framePending;
#endif

   SYS_freeMem(ADP_cntxt.txParams.bdHndl);
   ADP_cntxt.cnfrmSts.sts = sts;
   ADP_clearAppTxParams();

   // Commented out - rkris@wisense.in / aug/18/2015
   // #ifdef DEV_TYPE_RFD
   //   if (MAC_nodeJoined())
   // #endif

   if (ADP_cntxt.pendTxReqSrc == ADP_TX_REQ_SRC_APP)
   {
       SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_ADP_DATA_CONFIRM;
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
ADP_sts_t ADP_appPyldTxProc(void)
{
    ADP_sts_t sts = ADP_STS_SUCCESS;
    UINT16_t pyldLen;
    MCPS_dataReqParams_s macTxReqParams;

    pyldLen = SYS_GET_BD_LEN(ADP_cntxt.txParams.bdHndl);

    memset(&(ADP_cntxt.fragInfo), 0x0, sizeof(ADP_dataReqFragInfo_s));

    ADP_cntxt.fragInfo.sduLenLeft = pyldLen;
    ADP_cntxt.fragInfo.adpHdrLen = ADP_calcADPHdrLen();

    macTxReqParams.indirectTx = 0;  // Bug fix - march/2/15 - rkris@wisense.in
    macTxReqParams.destAddrMode = MAC_ADDRESS_MODE_SHORT_ADDR;
    macTxReqParams.srcAddrMode = MAC_ADDRESS_MODE_SHORT_ADDR;

    {
       MAC_pduInfo_s pduInfo;
       pduInfo.dam = macTxReqParams.destAddrMode;
       pduInfo.sam = macTxReqParams.srcAddrMode;
       pduInfo.lpwmnIdComp = MAC_DATA_PKT_LPWMN_ID_COMP_FLAG;
       ADP_cntxt.fragInfo.macHdrLen = MAC_calcHdrLen(&pduInfo);
    }

#ifdef ADP_FRAG_ENA
    if (ADP_cntxt.fragInfo.macHdrLen
        + ADP_cntxt.fragInfo.adpHdrLen
        + pyldLen > MAC_MAX_HDR_PLUS_PYLD_LEN)
    {
        // Fragmentation required !!

        if (ADP_cntxt.txParams.destShortAddr == MAC_BROADCAST_SHORT_ADDR
            || ADP_cntxt.txParams.destShortAddr != LPWMN_COORD_SHORT_ADDR)
        {
            // Currently multi frag packets can be sent by non-coord nodes to coord.
            // Nodes cannot send multi frag packets to other nodes. Coord cannot
            // send multi frag packets to any node.
        	// Also, mesh level broadcasts cannot have multiple fragments.
            sts = ADP_STS_FRAG_NOT_SUPPORTED;
        }
        else
        {
            ADP_cntxt.fragInfo.adpHdrLen += (ADP_ELEMENT_TYPE_LEN + ADP_FRAG_HDR_LEN);

            ADP_cntxt.fragInfo.fragLen = MAC_MAX_HDR_PLUS_PYLD_LEN
                                         - ADP_cntxt.fragInfo.macHdrLen
                                         - ADP_cntxt.fragInfo.adpHdrLen;   // mesh hdr + frag hdr is present in every fragment

            // Calculate the number of fragments
            do
            {
               ADP_cntxt.fragInfo.fragCnt ++;
               if (pyldLen > ADP_cntxt.fragInfo.fragLen)
                   pyldLen -= ADP_cntxt.fragInfo.fragLen;
               else
                   break;
            } while (1);

            if (ADP_cntxt.fragInfo.fragCnt > ADP_MAX_FRAGS_PER_SDU)
                sts = ADP_STS_TOO_MANY_FRAGMENTS;
            else
            {
                ADP_cntxt.fragInfo.fragTag = ADP_cntxt.fragTag;
                ADP_cntxt.fragTag ++;
            }
        }
    }
    else
    {
        ADP_cntxt.fragInfo.fragCnt = 0x1;
    }
#else
    if (ADP_cntxt.fragInfo.macHdrLen
        + ADP_cntxt.fragInfo.adpHdrLen
        + pyldLen > MAC_MAX_HDR_PLUS_PYLD_LEN)
    {
        sts = ADP_STS_FRAG_NOT_SUPPORTED;
    }
    else
        ADP_cntxt.fragInfo.fragCnt = 0x1;
#endif

    if (sts == ADP_STS_SUCCESS)
        sts = ADP_sendFrag(&macTxReqParams);

    if (sts != ADP_STS_SUCCESS)
    {
    	SYS_freeMem(ADP_cntxt.txParams.bdHndl);
        ADP_cntxt.cnfrmSts.sts = sts;
        ADP_clearAppTxParams();
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
void ADP_retryAppDataTxPostRtDisc(void)
{
   ADP_sts_t sts;
   
   /*
    * All the required parameters are stored in ADP_cntxt.txParams
    */
  
   sts = ADP_appPyldTxProc();  
   if (sts != ADP_STS_SUCCESS)
   {
       ADP_cntxt.cnfrmSts.sts = sts;

#if defined(LPWMN_COORD) && defined(GW_ENA)
       if (ADP_cntxt.txParams.localOrig == 0x0)
           GW_relayCnfrm(LPWMN_ADP_MODULE, (UINT8_t)sts);
       else
#endif
           SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_ADP_DATA_CONFIRM;

       ADP_clearAppTxParams();
   }
   
   return;
}


#ifdef LPWMN_COORD

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void ADP_rfdReassocProc(UINT16_t oldParent, UINT16_t rfdAddr)
{
   // <TODO>
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void ADP_procRelayedAssocReq(UINT8_t *rxBuff_p, UINT16_t relayNode)
{
   NM_nodeListEntry_s *entry_p;
   UINT8_t bdHndl;
   UINT16_t parentNodeIdx = NM_getNodeIdx(relayNode);

   if (parentNodeIdx == NM_MAX_SUPPORTED_NODE_CNT)
   {
       // Parent node is not registered with the LPWMN coordinator !!
       return;
   }

   // GW_sendAssocReqRcvdEvt(rxBuff_p);

   entry_p = NM_lookUpNode(rxBuff_p);
   if (entry_p != NULL && entry_p->reassocTmo > 0)
   {
  	   // Drop back to back assoc requests from the same node (not conneted directly
	   // to coordinator) within 1 minute.
       // interval. This is to take care of case shown below:
	   //
	   // Joining node is 2 or more hops away from Coordinator
	   //
	   // > Node sends first assoc request through parent.
	   //
	   // > Parent fails to forward packet.
	   //
	   // > Node times out (after 10 seconds) waiting for assoc response.
	   //   Node resends assoc request.
	   //
	   // > Parent starts route discovey to coord (10 seconds or more). Assoc
	   //   request is buffered.
	   //
	   // > Node times out again (after 10 seconds) waiting for assoc response.
	   //   Node resends assoc request.
	   //
	   // > Coordinator finally gets the first assoc request and sends back
	   //   assoc response to requesting node with some short address.
	   //
	   // > Then coordinator gets second assoc request and it allocates another
	   //   short address and sends back to requesting node.
	   //
	   // > Requesting node drops the second assoc response.
	   //
	   // > When newly joined node sends data with first short address, the coord
	   //   drops these packets since this short address is not registered !!

	   GW_sendAssocReqDropEvt(rxBuff_p, entry_p->reassocTmo);

       return;
   }

   bdHndl = SYS_allocMem(SYS_TRSCN_Q_HNDL_FIELD_LEN
                          + ADP_TRSCN_TYPE_FIELD_LEN
                          + MAC_SHORT_ADDR_LEN   // short address of relay node
                          + MAC_SHORT_ADDR_LEN   // next hop to relay node
                          + MAC_EXT_ADDR_LEN
                          + MAC_ASSOC_RESP_STS_LEN
                          + MAC_SHORT_ADDR_LEN
                          + MAC_NODE_CAPABILITY_INFO_LEN);
   if (SYS_BD_HNDL_IS_VALID(bdHndl))
   {
       UINT8_t status = MAC_ASSOCIATION_STS_SUCCESS;
       UINT8_t *bdBuff_p = SYS_GET_BD_BUFF(bdHndl);
       
       *(bdBuff_p) = SYS_MAX_BUFF_DESC_CNT;
       bdBuff_p += SYS_TRSCN_Q_HNDL_FIELD_LEN;
       
       *(bdBuff_p) = ADP_TRSCN_TYPE_ASSOC_RESP_TO_RELAY;
       bdBuff_p += ADP_TRSCN_TYPE_FIELD_LEN;
       
       UTIL_htons(bdBuff_p, relayNode);    // *((UINT16_t *)bdBuff_p) = relayNode;
       bdBuff_p += MAC_SHORT_ADDR_LEN;
       
       // Next hop not determined yet       
       UTIL_htons(bdBuff_p, MAC_BROADCAST_SHORT_ADDR);
       bdBuff_p += MAC_SHORT_ADDR_LEN;
       
       memcpy(bdBuff_p, rxBuff_p, MAC_EXT_ADDR_LEN);
       bdBuff_p += MAC_EXT_ADDR_LEN;
        
       // The relayed association request message has the association info in the
       // following format - 
       // <mac extended address> <cap info byte>
  
       if (entry_p == NULL)
       {
           if ((entry_p = NM_allocNodeTblEntry()) != NULL)
           {
               memcpy(entry_p->extAddr, rxBuff_p, MAC_EXT_ADDR_LEN);
               entry_p->nodeCapInfo = *(rxBuff_p + MAC_EXT_ADDR_LEN);
           }
           else
           {
               // Node table entry is full. Reject this association request
               status = MAC_ASSOCIATION_STS_LPWMN_AT_CAPACITY;
           }
       }
       else
       {
           UINT8_t flag = 0;

           // Node is re-associating
           if (AODVL_lookUpNodeInRtDiscTbl(entry_p->shortAddr))
           {
               flag = 1;
           }

           AODVL_purgeRtTbls(entry_p->shortAddr);

           if (flag)
               ADP_procRtDiscOpnRslt(entry_p->shortAddr, MAC_BROADCAST_SHORT_ADDR);

           if ((entry_p->nodeCapInfo & MAC_ASSOC_REQ_CAP_INFO_DEV_TYPE_BIT_SHIFT_MSK) == 0x0)
           {
               // Re-associating node is an RFD. Inform the older parent that
               // this RFD is now associating through some other node.

               // If parentNodeIdx is NM_MAX_SUPPORTED_NODE_CNT, it means LPWMN coordinator
               // is the parent.
               if (entry_p->parentNodeIdx < NM_MAX_SUPPORTED_NODE_CNT)
               {
                   ADP_rfdReassocProc(NM_nodeAddr(entry_p->parentNodeIdx),
                                      entry_p->shortAddr);
               }
           }
           else
           {
               // Reassociating node is an FFD. Delete all associated children RFDs since the
               // RFDs will reassociate.
               NM_deleteRFDs(entry_p - NM_cntxt.NM_nodeList);
           }
       }

       if (entry_p != NULL)
       {
           entry_p->shortAddr = NM_allocShortAddr();
           entry_p->reassocTmo = NM_TIME_TO_NEXT_REASSOC_SECS;  // rkris@wisense.in / sep/23/15
           entry_p->parentNodeIdx = (UINT8_t)parentNodeIdx;
           entry_p->nodeCapInfo = *(rxBuff_p + MAC_EXT_ADDR_LEN);
           entry_p->lastSysFEId = UTIL_ntohs(rxBuff_p + MAC_EXT_ADDR_LEN + MAC_NODE_CAPABILITY_INFO_LEN);
       }

#ifdef GW_ENA
       // Feb/9/14, rkris - inform GW that this node has joined
       GW_sendNodeRegEvt(entry_p);
#endif

       *(bdBuff_p) = status;
       bdBuff_p ++;
       
       UTIL_htons(bdBuff_p,
                  entry_p != NULL ? entry_p->shortAddr : MAC_BROADCAST_SHORT_ADDR);
       bdBuff_p += MAC_SHORT_ADDR_LEN;

       *(bdBuff_p) = (entry_p != NULL ? entry_p->nodeCapInfo : 0);
       bdBuff_p += MAC_NODE_CAPABILITY_INFO_LEN;

       // Now send out a ASSOC-RESP message to the node which relayed
       // this ASSOC-REQ.       
        
       SYS_addToTrscnsQ(&(ADP_cntxt.trscnQ), bdHndl);
       SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ADP_TRSCN_PENDING;
    } 
 
    return;
}

#else

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void ADP_procRelayedAssocResp(UINT8_t *buff_p)
{
    UINT8_t bdHndl;
    
    // Element layout is :
    // <extended addr> <status> <allocated short>

    // Need to send out MAC-ASSOC-RESP when MAC is idle.
    // Add this request to the transaction queue
    bdHndl = SYS_allocMem(SYS_TRSCN_Q_HNDL_FIELD_LEN
                          + ADP_TRSCN_TYPE_FIELD_LEN
                          + MAC_EXT_ADDR_LEN
                          + MAC_ASSOC_RESP_STS_LEN
                          + MAC_SHORT_ADDR_LEN
                          + MAC_NODE_CAPABILITY_INFO_LEN);
    if (SYS_BD_HNDL_IS_VALID(bdHndl))
    {
        UINT8_t *bdBuff_p = SYS_GET_BD_BUFF(bdHndl);
        bdBuff_p += SYS_TRSCN_Q_HNDL_FIELD_LEN;
        *(bdBuff_p) = ADP_TRSCN_TYPE_ASSOC_RESP;
        bdBuff_p  += ADP_TRSCN_TYPE_FIELD_LEN;
        memcpy(bdBuff_p, buff_p, ADP_ASSOC_RESP_ELEMENT_LEN);
        bdBuff_p += ADP_ASSOC_RESP_ELEMENT_LEN;
        
        SYS_addToTrscnsQ(&(ADP_cntxt.trscnQ), bdHndl);
        SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ADP_TRSCN_PENDING;
    } 
}

#endif


#ifdef LPWMN_COORD
#ifdef ADP_FRAG_ENA

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
ADP_reassmCntxt_s *ADP_lookUpReassyCntxt(UINT16_t shortAddr)
{
   UINT16_t idx;
   ADP_reassmCntxt_s *rc_p = &(ADP_cntxt.reAssyFragCntxtList[0]);

   if (ADP_cntxt.reassyCntxtsInUse > 0)
   {
       for (idx = 0; idx < ADP_MAX_REASSY_CNTXTS; idx++)
       {
           if (rc_p->srcShort == shortAddr)
               return rc_p;
           rc_p ++;
       }
   }

   return NULL;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void ADP_cleanUpReassyCntxt(ADP_reassmCntxt_s *rc_p)
{
   SYS_purgeTrscnsQ(&(rc_p->reassyQ));
   rc_p->srcShort = MAC_BROADCAST_SHORT_ADDR;
   rc_p->nextExpFragIdx = 0;
   rc_p->timeToExpiry = 0;
   rc_p->fragTag = 0xff;

   if (ADP_cntxt.reassyCntxtsInUse == 0)
       SYS_fatal(SYS_FATAL_ERR_863);

   ADP_cntxt.reassyCntxtsInUse --;

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
ADP_freeFragCntxt(UINT16_t shortAddr)
{
   ADP_reassmCntxt_s *rc_p;

   rc_p = ADP_lookUpReassyCntxt(shortAddr);
   if (rc_p != NULL)
       ADP_cleanUpReassyCntxt(rc_p);

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
ADP_reassmCntxt_s *ADP_allocReassyCntxt(void)
{
   if (ADP_cntxt.reassyCntxtsInUse < ADP_MAX_REASSY_CNTXTS)
   {
       UINT16_t idx;
       ADP_reassmCntxt_s *rc_p = &(ADP_cntxt.reAssyFragCntxtList[0]);

       for (idx = 0; idx < ADP_MAX_REASSY_CNTXTS; idx++)
       {
           if (rc_p->srcShort == MAC_BROADCAST_SHORT_ADDR)
           {
               SYS_initTrscnsQ(&(rc_p->reassyQ));
               rc_p->nextExpFragIdx = 0;
               rc_p->fragTag = 0xff;
               rc_p->timeToExpiry = ADP_REASSY_PROC_TMO_SECS;
               ADP_cntxt.reassyCntxtsInUse ++;
               return rc_p;
           }
           rc_p ++;
       }

       SYS_fatal(SYS_FATAL_ERR_864);
   }

   return NULL;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void ADP_checkReassyTmo(void)
{
   UINT16_t idx, inUseCnt = 0;
   ADP_reassmCntxt_s *rc_p = &(ADP_cntxt.reAssyFragCntxtList[0]);

   for (idx = 0; idx < ADP_MAX_REASSY_CNTXTS; idx++)
   {
        if (rc_p->srcShort != MAC_BROADCAST_SHORT_ADDR)
        {
            if (rc_p->timeToExpiry == 0)
                SYS_fatal(SYS_FATAL_ERR_862);

            rc_p->timeToExpiry --;
            if (rc_p->timeToExpiry == 0)
            {
                ADP_cleanUpReassyCntxt(rc_p);
            }

            inUseCnt ++;
        }

        rc_p ++;
   }

   if (inUseCnt == 0)
       SYS_fatal(SYS_FATAL_ERR_865);

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
ADP_reassmCntxt_s *ADP_initReassyCntxts(void)
{
   UINT16_t idx;
   ADP_reassmCntxt_s *rc_p = &(ADP_cntxt.reAssyFragCntxtList[0]);

   for (idx = 0; idx < ADP_MAX_REASSY_CNTXTS; idx++)
   {
       SYS_initTrscnsQ(&(rc_p->reassyQ));
       rc_p->nextExpFragIdx = 0;
       rc_p->timeToExpiry = 0;
       rc_p->srcShort = MAC_BROADCAST_SHORT_ADDR;
       rc_p ++;
   }

   ADP_cntxt.reassyCntxtsInUse = 0;

   return NULL;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t ADP_reassmFrags(ADP_reassmCntxt_s *rc_p)
{
   UINT8_t tBd, fragCnt = 0, inQHndl = SYS_GET_TRSCN_Q_HEAD(&(rc_p->reassyQ));
   UINT16_t totalLen = 0;

   if (!(SYS_BD_HNDL_IS_VALID(inQHndl)))
       SYS_fatal(SYS_FATAL_ERR_861);

   while (SYS_BD_HNDL_IS_VALID(inQHndl))
   {
       totalLen += SYS_GET_BD_LEN(inQHndl);
       totalLen -= SYS_TRSCN_Q_HNDL_FIELD_LEN;
       inQHndl = SYS_GET_TRSCN_Q_NEXT(inQHndl);
       fragCnt ++;
       if (fragCnt > ADP_MAX_FRAGS_PER_SDU)
           SYS_fatal(SYS_FATAL_ERR_859);
   }

   tBd = SYS_allocMem(totalLen);
   if (SYS_BD_HNDL_IS_VALID(tBd))
   {
       UINT8_t *dB_p = SYS_GET_BD_BUFF(tBd);
       UINT8_t inQHndl = SYS_GET_TRSCN_Q_HEAD(&(rc_p->reassyQ));

       while (SYS_BD_HNDL_IS_VALID(inQHndl))
       {
          UINT8_t *sB_p = SYS_GET_BD_BUFF(inQHndl), nextHndl;
          UINT16_t fragLen = SYS_GET_BD_LEN(inQHndl);

          if (fragLen < 2)
              SYS_fatal(SYS_FATAL_ERR_857);

          memcpy(dB_p, sB_p + SYS_TRSCN_Q_HNDL_FIELD_LEN, fragLen - SYS_TRSCN_Q_HNDL_FIELD_LEN);
          dB_p += (fragLen - SYS_TRSCN_Q_HNDL_FIELD_LEN);

          nextHndl = *(sB_p);
          SYS_freeMem(inQHndl);
          inQHndl = nextHndl;
       }
   }

   SYS_initTrscnsQ(&(rc_p->reassyQ));

   ADP_cleanUpReassyCntxt(rc_p);

   return tBd;
}
#endif
#endif


UINT8_t __junk = 0;

/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
void ADP_procRcvdPkt(MCPS_dataIndParams_s *indParams_p)
{
    UINT8_t *buff_p = SYS_GET_BD_BUFF(indParams_p->bdHndl);
    UINT16_t msduLength = SYS_GET_BD_LEN(indParams_p->bdHndl);
    UINT8_t pktProcDone = 0, dropPkt = 1;
    UINT16_t pktOriginator;
    
    buff_p += (SYS_TRSCN_Q_HNDL_FIELD_LEN + SYS_TRSCN_TYPE_FIELD_LEN);
    msduLength -= (SYS_TRSCN_Q_HNDL_FIELD_LEN + SYS_TRSCN_TYPE_FIELD_LEN);
   
    // MAC source and destination address should both be short
    if (indParams_p->srcAddrMode != MAC_ADDRESS_MODE_SHORT_ADDR
        || indParams_p->destAddrMode != MAC_ADDRESS_MODE_SHORT_ADDR)
    {
        SYS_freeMem(indParams_p->bdHndl);
        return;
    }
    
    // If the packet has a mesh header, pktOriginator will be overwritten with
    // the source address from the mesh header.
    pktOriginator = indParams_p->srcAddr.shortAddr;
    
    do
    { 
       UINT8_t elementType = *buff_p;
       
       if (msduLength < 1)
       {
           break;
       }
             
       msduLength --;
       buff_p ++;

       switch (elementType)
       {
#ifndef DEV_TYPE_RFD
           case ADP_ELEMENT_TYPE_AODVL:
                { 
                   AODVL_rcvdMsgInfo_s info;
                   
                   SYS_ADJ_BD_HEAD(indParams_p->bdHndl,
                                   buff_p - (SYS_GET_BD_BUFF(indParams_p->bdHndl)));                   
                   
                   info.bdHndl = indParams_p->bdHndl;
                   info.measLQI = indParams_p->rssi;
                   info.senderAddr = indParams_p->srcAddr.shortAddr;
                   
                   // Hand over to the AODVL module
                   AODVL_rcvdMsgHndlr(&info);
                   
                   pktProcDone = 1;
                   dropPkt = 0;
                }
                break;
#endif
           case ADP_ELEMENT_TYPE_MESH_BC_HDR:
                {
                   // ADP_pktFwdCheck( ) (called from MAC) has already verified
                   // that this is a mesh broadcast packet.

                   pktOriginator = UTIL_ntohs(buff_p + ADP_MESH_BC_HDR_SRC_ADDR_OFF);

                   buff_p += ADP_MESH_BC_HDR_LEN;
                   msduLength -= ADP_MESH_BC_HDR_LEN;
                }
                break;

           case ADP_ELEMENT_TYPE_MESH_HDR:
                {                   
                   // ADP_pktFwdCheck( ) (called from MAC) has already handled
                   // packets not intended for this node. This case will be hit
                   // when packet has mesh header and is addressed to this node
                  
                   pktOriginator = UTIL_ntohs(buff_p + ADP_MESH_HDR_SRC_ADDR_OFF);
                   buff_p += ADP_MESH_HDR_LEN;
                   msduLength -= ADP_MESH_HDR_LEN;
                }
                break;
      
#ifdef LPWMN_COORD
           case ADP_ELEMENT_TYPE_ASSOC_REQ:
                {
                   pktProcDone = 1;
 
                   // <ext mac address> <cap info byte>
                   if (msduLength >= ADP_ASSOC_REQ_ELEMENT_LEN)
                   {   
                       ADP_procRelayedAssocReq(buff_p, pktOriginator);      
                   }
                }
                break;
#else
           case ADP_ELEMENT_TYPE_ASSOC_RESP:
                {  
                   pktProcDone = 1;
                   
                   (void)pktOriginator;
                   
                   if (msduLength < ADP_ASSOC_RESP_ELEMENT_LEN)
                       return;         
                   
                   ADP_procRelayedAssocResp(buff_p);
                }
                break;
#endif

#ifdef LPWMN_COORD
#ifdef ADP_FRAG_ENA
           case ADP_ELEMENT_TYPE_FRAG_HDR:
                {
                	UINT8_t fragIdx, lastFrag, fragTag = *(buff_p);
                	ADP_reassmCntxt_s *rc_p;

                	fragTag = (fragTag >> ADP_FRAG_HDR_TAG_FIELD_SHIFT) & ADP_FRAG_HDR_TAG_FIELD_MSK;
                	fragIdx = (*(buff_p)) & ADP_FRAG_HDR_IDX_FIELD_SHIFT_MSK;
                	lastFrag = (*(buff_p)) & ADP_FRAG_HDR_LAST_FRAG_IND_BIT ? 0x1 : 0x0;

                	buff_p += ADP_FRAG_HDR_LEN;

                	if (fragIdx > 0)
                        __junk ++;

                 	rc_p = ADP_lookUpReassyCntxt(pktOriginator);

                	if (((rc_p != NULL) && (fragTag != rc_p->fragTag || fragIdx != rc_p->nextExpFragIdx))
                	    || (rc_p == NULL && fragIdx != 0))
                	{
                	    // Not expecting the received frag !!.
                		// Delete the frame (pending completion) and the newly received frame

                		ADP_cleanUpReassyCntxt(rc_p);
                        dropPkt = 1;
                        pktProcDone = 1;
                	}
                	else
                	{
                	    if (rc_p == NULL)
                	        rc_p = ADP_allocReassyCntxt();

                	    if (rc_p == NULL)  // no free reassembly context !!
                	    {
                	        dropPkt = 1;
                	        pktProcDone = 1;
                	        break;
                	    }

                	    rc_p->srcShort = pktOriginator;
                	    rc_p->fragTag = fragTag;

                	    // Remove all the ADP headers. Leave 1 byte for adding this frame to the reassembly queue.
                        SYS_ADJ_BD_HEAD(indParams_p->bdHndl,
                                        buff_p - (SYS_GET_BD_BUFF(indParams_p->bdHndl)) - SYS_TRSCN_Q_HNDL_FIELD_LEN);

                        SYS_addToTrscnsQ(&(rc_p->reassyQ), indParams_p->bdHndl);

                        if (lastFrag)
                        {
                            UINT8_t rBd = ADP_reassmFrags(rc_p);
                            if (SYS_BD_HNDL_IS_VALID(rBd))
                            {
                                indParams_p->bdHndl = rBd;
                                buff_p = SYS_GET_BD_BUFF(indParams_p->bdHndl);
                                msduLength = SYS_GET_BD_LEN(indParams_p->bdHndl);

                            }
                            else
                            {
                                dropPkt = 1;
                                pktProcDone = 1;
                                // indParams_p->bdHndl is still points to received frag.
                            }
                        }
                        else
                        {
                            rc_p->nextExpFragIdx ++;
                            pktProcDone = 1;
                            dropPkt = 0;
                        }
                    }
                }
                break;
#endif
#endif

           case ADP_ELEMENT_TYPE_NORMAL_DATA:
                {
                   pktProcDone = 1;
#ifdef ADP_STATS_ENA
                   ADP_cntxt.ADP_dataRcvdCnt ++;
#endif

                   SYS_ADJ_BD_HEAD(indParams_p->bdHndl,
                                   buff_p - (SYS_GET_BD_BUFF(indParams_p->bdHndl)));

#if defined(LPWMN_COORD) && defined(GW_ENA)
                   // For now send all packets to the GW module
                   GW_relayMsgFromRemoteNode(pktOriginator, indParams_p);
                   dropPkt = 0;
#else
                   // Drop packet if previous data indication still not handled !!
                   if (!(SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_ADP_DATA_INDICATION))
                   {
                       ADP_cntxt.dataIndParams.bdHndl = indParams_p->bdHndl;
                       ADP_cntxt.dataIndParams.srcAddr = pktOriginator;
                       SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_ADP_DATA_INDICATION;
                       dropPkt = 0;
                   }
#endif
                }
                break;

           default:
                {
                   dropPkt = 1;
                   pktProcDone = 1;
                }
                break;
       }
    } while (pktProcDone == 0x0);
    
    if (dropPkt)
        SYS_freeMem(indParams_p->bdHndl);
    
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
void ADP_checkBcLogTbl(UINT8_t elapsedTimeSecs)
{
   UINT8_t idx;
   ADP_bcLogTblEntry_s *entry_p = &(ADP_cntxt.bcLogTbl[0]);

   for (idx=0; idx<ADP_BC_LOG_TBL_SZ; idx++)
   {
       if (entry_p->srcAddr != MAC_BROADCAST_SHORT_ADDR)
       {
           entry_p->ttl -= elapsedTimeSecs;
           if (entry_p->ttl <= 0)
               entry_p->srcAddr = MAC_BROADCAST_SHORT_ADDR;
       }

       entry_p ++;
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
UINT8_t ADP_checkDupBroadcastPkt(UINT16_t srcAddr, UINT8_t seqNr)
{
   UINT8_t rc = 1, idx, freeIdx = ADP_BC_LOG_TBL_SZ;
   ADP_bcLogTblEntry_s *entry_p = &(ADP_cntxt.bcLogTbl[0]);

   for (idx=0; idx<ADP_BC_LOG_TBL_SZ; idx++)
   {
        if (entry_p->srcAddr == srcAddr
            && entry_p->seqNr == seqNr)
        {
            ADP_cntxt.meshBroadcastDupPktDroppedCnt ++;
            break;
        }

        if (freeIdx == ADP_BC_LOG_TBL_SZ
            && entry_p->srcAddr == MAC_BROADCAST_SHORT_ADDR)
        {
            freeIdx = idx;
        }

        entry_p ++;
   }

   if (idx >= ADP_BC_LOG_TBL_SZ)
   {
       if (freeIdx < ADP_BC_LOG_TBL_SZ)
       {
           entry_p = &(ADP_cntxt.bcLogTbl[freeIdx]);
           entry_p->srcAddr = srcAddr;
           entry_p->seqNr = seqNr;
           entry_p->ttl = ADP_BC_LOG_TBL_ENTRY_MAX_TTL_IN_SECS;
           rc = 0;
        }
   }

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
UINT8_t ADP_pktFwdCheck(UINT8_t *adpPdu_p,
                        UINT8_t *dstShort_p,  // Points to dest short in the MAC header
                        UINT8_t *srcShort_p,
                        UINT8_t adpPduLen,
                        UINT8_t bdHndl)
{    
    UINT8_t rc = 0;
    
    // The mesh header should be the first header in the ADP PDU
  
    if ((adpPduLen >= ADP_ELEMENT_TYPE_LEN + ADP_MESH_HDR_LEN)
        && adpPdu_p[0] == ADP_ELEMENT_TYPE_MESH_HDR)
    {
        adpPdu_p ++;
        
        // adpPdu_p is now pointing to the mesh header
        
        // The hop count has to be at least 1 for this node to process this message
        // even when the mesh destination is this node. Hop count 0 is not allowed.
        if (adpPdu_p[ADP_MESH_HDR_HOP_COUNT_OFF] >= 1)
        {
            UINT16_t srcAddr = UTIL_ntohs(adpPdu_p + ADP_MESH_HDR_SRC_ADDR_OFF);

            // Feb/21/16 - rkris@wisense.in
            if (MAC_verifyUCSA(srcAddr) == 0)
            {
            	// Invalid short address !!
                SYS_freeMem(bdHndl);
                return 1;
            }

            if (srcAddr == MAC_PIB.macShortAddress)
            {
            	// rkris@wisense.in / feb/21/16
            	// Mesh PDU originated locally !! - Looks like there is a loop.
                SYS_freeMem(bdHndl);
                ADP_cntxt.meshLoopDetectCnt ++;
                return 1;
            }
            else
            {                      
                UINT16_t destAddr = UTIL_ntohs(adpPdu_p + ADP_MESH_HDR_DEST_ADDR_OFF);

                if (destAddr != MAC_BROADCAST_SHORT_ADDR && MAC_verifyUCSA(destAddr) == 0)
                {
                	// Invalid short address (neither bc, nor valid unicast address)
                    SYS_freeMem(bdHndl);
                    return 1;
                }

                if (destAddr != MAC_PIB.macShortAddress)
                {  
                    // Packet not intended for this node .... 
                    // Check if hop count field allows this packet to be forwarded
                    if (adpPdu_p[ADP_MESH_HDR_HOP_COUNT_OFF] > 1)
                    {
                        UINT16_t nextHop;
                        
                        // Update hop count
                        adpPdu_p[ADP_MESH_HDR_HOP_COUNT_OFF] --;  // Update the hops left field
                            
                        // Update source MAC address to this hop
                        srcShort_p[0] = (UINT8_t)MAC_PIB.macShortAddress;
                        srcShort_p[1] = (UINT8_t)((MAC_PIB.macShortAddress >> 8) & 0xff);  

#ifdef DEV_TYPE_FFD
                        if (MAC_lookUpAssocRFDList(destAddr))
#else
                        if (NM_lookUpLocalRFD(destAddr))
#endif
                        {
                            dstShort_p[0] = (UINT8_t)destAddr;
                            dstShort_p[1] = (UINT8_t)((destAddr >> 8) & 0xff);

                            // This BD has a 1 byte trscn type which is not required now

                            SYS_ADJ_BD_HEAD(bdHndl, SYS_TRSCN_TYPE_FIELD_LEN);

                            // Destination is an RFD child of this node.
                            if (!(MAC_addFrameToPendQ(destAddr, bdHndl)))
                            {
                                // No space !!
                                SYS_freeMem(bdHndl);
                            }

                            return 1;
                        }
               
                        nextHop = ADP_getRoute(destAddr);
                        if (nextHop != MAC_BROADCAST_SHORT_ADDR)
                        {
                            // Route exists to specified destination ....
                                
                            // Update destination MAC address to next hop
                            dstShort_p[0] = (UINT8_t)nextHop;
                            dstShort_p[1] = (UINT8_t)((nextHop >> 8) & 0xff);
                                
                            // Set the transaction type
                            {
                               UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);
                               *(buff_p + SYS_TRSCN_Q_HNDL_FIELD_LEN) = ADP_TRSCN_TYPE_MESH_PDU;
                            }
                            
                            SYS_addToTrscnsQ(&(ADP_cntxt.trscnQ), bdHndl); 
                            SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ADP_TRSCN_PENDING;
                        }
                        else
                        {
                            if (AODVL_rtDiscRequest(destAddr) == 0x0)
                            {
                                // AODVL is not able to accommodate another route discovery request
                                SYS_freeMem(bdHndl);

                                ADP_cntxt.rtDiscErrCnt ++;

                                // Do not send route error back to packet originator since RERR
                                // implies route could not be discovered.
                            }
                            else
                            {
                                // Store the offset of the mesh header in the transaction type field
                                // The offset points to the hop count field of the mesh header
                                {
                                   UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);
                                   // adpPdu_p is already pointing to the hop count field
                                   *(buff_p + SYS_TRSCN_Q_HNDL_FIELD_LEN) = (adpPdu_p - buff_p);
                                }
                                
                                // Add to pkts-to-fwd route pending list
                                SYS_addToTrscnsQ(&(ADP_cntxt.rtPendFwdPktTrscnQ), bdHndl);

                                ADP_cntxt.rtDiscStartedCnt ++;
                            }
                        }
                        
                        rc = 1;
                    }
                }            
            }
        }
        else
        {
            SYS_freeMem(bdHndl);
            rc = 1;
        }
    }
    else
    {
        if ((adpPduLen >= ADP_ELEMENT_TYPE_LEN + ADP_MESH_BC_HDR_LEN)
            && adpPdu_p[0] == ADP_ELEMENT_TYPE_MESH_BC_HDR)
        {
            UINT16_t srcAddr;

            adpPdu_p ++;

            srcAddr = UTIL_ntohs(adpPdu_p + ADP_MESH_BC_HDR_SRC_ADDR_OFF);

            // Mesh broadcast frame

            if (
                (srcAddr == MAC_PIB.macShortAddress)
#ifdef LPWMN_COORD
                || (NM_lookUpShortAddr(srcAddr) == NULL)
#endif
                || (adpPdu_p[ADP_MESH_BC_HDR_HOP_COUNT_FIELD_OFF] == 0x0)
                || (ADP_checkDupBroadcastPkt(srcAddr, adpPdu_p[ADP_MESH_BC_HDR_SEQ_NR_FIELD_OFF]))
               )
            {
                SYS_freeMem(bdHndl);
                rc = 1;
            }
            else
            {
                ADP_cntxt.meshBroadcastPktRcvdCnt ++;

                if (adpPdu_p[ADP_MESH_BC_HDR_HOP_COUNT_FIELD_OFF] > 0x1)
                {
                    UINT8_t copyBdHndl;
                    UINT16_t bdLen = SYS_GET_BD_LEN(bdHndl);

                    // This frame can be forwarded ...

                    copyBdHndl = SYS_allocMem(bdLen);
                    if (SYS_BD_HNDL_IS_VALID(bdHndl) == 0x0)
                    {
                        SYS_freeMem(bdHndl);
                        rc = 1;
                    }
                    else
                    {
                        UINT8_t *copyBuff_p = SYS_GET_BD_BUFF(copyBdHndl);
                        UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);

                        memcpy(copyBuff_p, buff_p, bdLen);

                        // Update source MAC address to this hop
                        {
                           UINT8_t *b_p = copyBuff_p + (srcShort_p - buff_p);
                           b_p[0] = (UINT8_t)MAC_PIB.macShortAddress;
                           b_p[1] = (UINT8_t)(MAC_PIB.macShortAddress >> 8);
                        }

                        copyBuff_p[(adpPdu_p - buff_p) + ADP_MESH_BC_HDR_HOP_COUNT_FIELD_OFF] --;

                        // Set the transaction type
                        *(copyBuff_p + SYS_TRSCN_Q_HNDL_FIELD_LEN) = ADP_TRSCN_TYPE_MESH_BC_PDU;

                        SYS_addToTrscnsQ(&(ADP_cntxt.trscnQ), copyBdHndl);
                        SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ADP_TRSCN_PENDING;

                        // Send frame to local ADP layer
                        rc = 0;
                    }
                }
                else
                {
                    // Send frame to local ADP layer
                    rc = 0;
                }
            }
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
void ADP_macAssocIndHndlr(UINT8_t *srcExtAddr_p,
                          UINT8_t capInfo,
                          UINT16_t lastSysFEId)
{
    UINT8_t bdHndl = SYS_allocMem(SYS_TRSCN_Q_HNDL_FIELD_LEN
                                  + ADP_TRSCN_TYPE_FIELD_LEN
#ifndef LPWMN_COORD                                    
                                  + MAC_SHORT_ADDR_LEN   // destination
                                  + MAC_SHORT_ADDR_LEN   // next hop
#endif                                    
                                  + MAC_EXT_ADDR_LEN
                                  + MAC_NODE_CAPABILITY_INFO_LEN
                                  + SYSTEM_FATAL_ERROR_ID_LEN);
    if (SYS_BD_HNDL_IS_VALID(bdHndl))
    {
        UINT8_t *bdBuff_p = SYS_GET_BD_BUFF(bdHndl);
        
        bdBuff_p += SYS_TRSCN_Q_HNDL_FIELD_LEN;
        *(bdBuff_p) = ADP_TRSCN_TYPE_ASSOC_REQ;
        bdBuff_p ++;
#ifndef LPWMN_COORD          
        UTIL_htons(bdBuff_p, MAC_BROADCAST_SHORT_ADDR);
        bdBuff_p += MAC_SHORT_ADDR_LEN;
        UTIL_htons(bdBuff_p, MAC_BROADCAST_SHORT_ADDR);
        bdBuff_p += MAC_SHORT_ADDR_LEN;
#endif        
        memcpy(bdBuff_p,
               srcExtAddr_p,
               MAC_EXT_ADDR_LEN);
        bdBuff_p += MAC_EXT_ADDR_LEN;
        *(bdBuff_p) = capInfo;
        bdBuff_p ++;
        bdBuff_p = UTIL_htons(bdBuff_p, lastSysFEId);

        SYS_addToTrscnsQ(&(ADP_cntxt.trscnQ), bdHndl);
        SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ADP_TRSCN_PENDING;      
    } 
  
    return;
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
UINT8_t * ADP_prefixMeshHdr(UINT8_t *buff_p, UINT16_t destAddr)
{
   *(buff_p) = ADP_ELEMENT_TYPE_MESH_HDR;
   buff_p += ADP_ELEMENT_TYPE_LEN;

   *(buff_p) = ADP_MAX_HOP_COUNT;
   buff_p += ADP_MESH_HDR_HOP_COUNT_FIELD_LEN;

   UTIL_htons(buff_p, destAddr);
   buff_p += MAC_SHORT_ADDR_LEN;

   UTIL_htons(buff_p, MAC_PIB.macShortAddress);
   buff_p += MAC_SHORT_ADDR_LEN;

   return buff_p;
}
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
void ADP_relayAssocReqMsg(UINT8_t trscnBdHndl,
                          UINT16_t nextHop,
                          UINT8_t *extAddr_p,
                          UINT8_t devCapInfo,
                          UINT16_t lastSysFEId)
{
   UINT8_t pduLen, bdHndl;
   
   // Send the extended address and the capability information from 
   // the association request    
   
   if (nextHop == MAC_BROADCAST_SHORT_ADDR)
   {
       nextHop = ADP_getRoute(LPWMN_COORD_SHORT_ADDR);
       if (nextHop == MAC_BROADCAST_SHORT_ADDR)
       {
           // Trigger route discovery ...     
           if (AODVL_rtDiscRequest(LPWMN_COORD_SHORT_ADDR) == 0x0)
           {
               // AODVL is not able to accomodate another route discovery request
               SYS_freeMem(trscnBdHndl);

               ADP_cntxt.rtDiscErrCnt ++;
           }
           else
           {
               UINT8_t *buff_p = SYS_GET_BD_BUFF(trscnBdHndl);
               buff_p += (SYS_TRSCN_Q_HNDL_FIELD_LEN
                          + ADP_TRSCN_TYPE_FIELD_LEN);
               
               UTIL_htons(buff_p, LPWMN_COORD_SHORT_ADDR);
             
               // Add to list of assoc requests pending route discovery
               SYS_addToTrscnsQ(&(ADP_cntxt.assocRtPendTrscnQ), trscnBdHndl);

               ADP_cntxt.rtDiscStartedCnt ++;
           } 
           return;
       }
   }
  
   // Route exists. Request MAC to transmit data 
   
   pduLen = ADP_ELEMENT_TYPE_LEN 
            + MAC_EXT_ADDR_LEN 
            + MAC_NODE_CAPABILITY_INFO_LEN
            + SYSTEM_FATAL_ERROR_ID_LEN;
   if (nextHop != LPWMN_COORD_SHORT_ADDR)
       pduLen += (ADP_ELEMENT_TYPE_LEN + ADP_MESH_HDR_LEN);
   
   bdHndl = SYS_allocMem(pduLen);
   if (SYS_BD_HNDL_IS_VALID(bdHndl) == 0x0)
   {
       SYS_freeMem(trscnBdHndl);
       return;
   }
   else
   {
       UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);
       MCPS_dataReqParams_s macTxReqParams;

       // Msg format is -
       // <mac header> <mesh hdr (optional)> <relayed assoc request>
   
       if (nextHop != LPWMN_COORD_SHORT_ADDR)
       {
           // Prefix mesh header
           // Hop count | destination short addr | source short addr

           buff_p = ADP_prefixMeshHdr(buff_p, LPWMN_COORD_SHORT_ADDR);
       } 
    
       *(buff_p) = ADP_ELEMENT_TYPE_ASSOC_REQ;
       buff_p += ADP_ELEMENT_TYPE_LEN; 
    
       memcpy(buff_p, extAddr_p, MAC_EXT_ADDR_LEN);
       buff_p += MAC_EXT_ADDR_LEN;
    
       *(buff_p) = devCapInfo;
       buff_p += MAC_NODE_CAPABILITY_INFO_LEN;    
       
       buff_p = UTIL_htons(buff_p, lastSysFEId);

       SYS_freeMem(trscnBdHndl);
    
       macTxReqParams.destShortAddr = nextHop;
       macTxReqParams.destAddrMode = MAC_ADDRESS_MODE_SHORT_ADDR;
       macTxReqParams.srcAddrMode = MAC_ADDRESS_MODE_SHORT_ADDR;
       macTxReqParams.ackTx = 0x1;
       macTxReqParams.destLPWMNIdBc = 0x0;
       macTxReqParams.indirectTx = 0x0;
       macTxReqParams.msduBDHndl = bdHndl;

       ADP_cntxt.pendTxReqSrc = ADP_TX_REQ_SRC_ASSOC_REQ_RELAY;
       ADP_cntxt.assocReqRelayedCnt ++;
    
       MCPS_dataReq(&macTxReqParams);
   }
   
   return;      
}
#endif

#ifdef LPWMN_COORD

void ADP_sendRFDReAssocMsg(UINT8_t trscnBdHndl,
                           UINT8_t *trscnBuff_p)
{
   // <TODO>
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
void ADP_sendAssocRespToRelay(UINT8_t trscnBdHndl,
                              UINT8_t *trscnBuff_p)
{
   UINT16_t relayShort, nextHop;
   UINT8_t pduLen, bdHndl;

   // BD buffer layout (pointed to by trscnBuff_p)
   // <relay short> <next hop> <ext addr> <sts> <alloc short addr>
   
   // Bug fix / rkris@wisense.in / april/21/15
   relayShort = UTIL_ntohs(trscnBuff_p);

   nextHop = UTIL_ntohs(trscnBuff_p + MAC_SHORT_ADDR_LEN);
   if (nextHop == MAC_BROADCAST_SHORT_ADDR)
   {
      nextHop = ADP_getRoute(relayShort);
      if (nextHop == MAC_BROADCAST_SHORT_ADDR)
      {
          // Trigger route discovery ...        
          if (AODVL_rtDiscRequest(relayShort) == 0x0)
          {
              // AODVL is not able to accomodate another route discovery request
              SYS_freeMem(trscnBdHndl);

              ADP_cntxt.rtDiscErrCnt ++;
          }
          else
          {          
              // Add to list of assoc requests pending route discovery
              SYS_addToTrscnsQ(&(ADP_cntxt.assocRtPendTrscnQ), trscnBdHndl);

              ADP_cntxt.rtDiscStartedCnt ++;
          } 
          return;
      }
   }
   
   // Route exists. Request MAC to transmit data 
   
   trscnBuff_p += (MAC_SHORT_ADDR_LEN * 2);  // Skip the relay addr and next hop
   
   pduLen = ADP_ELEMENT_TYPE_LEN
            + MAC_EXT_ADDR_LEN
            + MAC_ASSOC_RESP_STS_LEN
            + MAC_SHORT_ADDR_LEN
            + MAC_NODE_CAPABILITY_INFO_LEN;
   if (nextHop != relayShort)
       pduLen += (ADP_ELEMENT_TYPE_LEN + ADP_MESH_HDR_LEN);
   
   bdHndl = SYS_allocMem(pduLen);
   if (SYS_BD_HNDL_IS_VALID(bdHndl) == 0x0)
   {
       SYS_freeMem(trscnBdHndl);
       return;
   }
   else
   {
       UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);
       MCPS_dataReqParams_s macTxReqParams;
       
       if (nextHop != relayShort)
       {
           // Prefix mesh header
           // Hop count | destination short addr | source short addr
        
           *(buff_p) = ADP_ELEMENT_TYPE_MESH_HDR;        
           buff_p += ADP_ELEMENT_TYPE_LEN;
        
           *(buff_p) = ADP_MAX_HOP_COUNT;
           buff_p += ADP_MESH_HDR_HOP_COUNT_FIELD_LEN;
       
           UTIL_htons(buff_p, relayShort); 
           buff_p += MAC_SHORT_ADDR_LEN;
               
           UTIL_htons(buff_p, MAC_PIB.macShortAddress);
           buff_p += MAC_SHORT_ADDR_LEN;
       }        
       
       *(buff_p) = ADP_ELEMENT_TYPE_ASSOC_RESP;
       buff_p += ADP_ELEMENT_TYPE_LEN; 
    
       // Element layout is :
       // <extended addr> <status> <allocated short> <node_cap_info byte>
       
       memcpy(buff_p, trscnBuff_p, MAC_EXT_ADDR_LEN);
       buff_p += MAC_EXT_ADDR_LEN;
       trscnBuff_p += MAC_EXT_ADDR_LEN;
    
       *(buff_p) =  *(trscnBuff_p);
       buff_p += MAC_ASSOC_RESP_STS_LEN;
       trscnBuff_p += MAC_ASSOC_RESP_STS_LEN;
       
       {
          UINT16_t allocShort = UTIL_ntohs(trscnBuff_p);  // *((UINT16_t *)trscnBuff_p);
          UTIL_htons(buff_p, allocShort);
          buff_p += MAC_SHORT_ADDR_LEN;
          trscnBuff_p += MAC_SHORT_ADDR_LEN;
       }
       
       *buff_p = *trscnBuff_p;  // Copy node capability byte
       buff_p += MAC_NODE_CAPABILITY_INFO_LEN;
       trscnBuff_p += MAC_NODE_CAPABILITY_INFO_LEN;

       SYS_freeMem(trscnBdHndl);       
   
       macTxReqParams.destShortAddr = nextHop;
       macTxReqParams.destAddrMode = MAC_ADDRESS_MODE_SHORT_ADDR;
       macTxReqParams.srcAddrMode = MAC_ADDRESS_MODE_SHORT_ADDR;
       macTxReqParams.ackTx = 0x1;
       macTxReqParams.destLPWMNIdBc = 0x0;
       macTxReqParams.indirectTx = 0x0;
       macTxReqParams.msduBDHndl = bdHndl;

       ADP_cntxt.pendTxReqSrc = ADP_TX_REQ_SRC_ASSOC_RESP_TO_RELAY;
    
       MCPS_dataReq(&macTxReqParams);   
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
void ADP_procPendingTrscns(void)
{
   SYS_trscnQ_s *trscnQ_p = &(ADP_cntxt.trscnQ);
   UINT8_t bdHndl;
   UINT8_t *buff_p, trscnType;
   
   if (SYS_TRSCN_Q_IS_EMPTY(trscnQ_p))
       SYS_fatal(SYS_FATAL_ERR_66);
  
   bdHndl = SYS_GET_TRSCN_Q_HEAD(trscnQ_p);
   
   buff_p = SYS_GET_TRSCN_Q_BD_BUFF(bdHndl);
   trscnType = *buff_p;
   buff_p ++;
   
   SYS_delFromTrscnsQ(trscnQ_p, bdHndl);  // does not free the BD 

   switch (trscnType)
   {    
#ifndef DEV_TYPE_RFD
#ifndef AODVL_PATH_DISC_DISABLED
      case ADP_TRSCN_TYPE_AODVL_PATH_REQ:
           {
               SYS_ADJ_BD_HEAD(bdHndl, SYS_TRSCN_Q_HNDL_FIELD_LEN + SYS_TRSCN_TYPE_FIELD_LEN);
               AODVL_processPathReqMsg(bdHndl);
           }
           break;

      case ADP_TRSCN_TYPE_AODVL_PATH_REP:
           {
               SYS_ADJ_BD_HEAD(bdHndl, SYS_TRSCN_Q_HNDL_FIELD_LEN + SYS_TRSCN_TYPE_FIELD_LEN);
               AODVL_processPathRepMsg(bdHndl);
           }
           break;
#endif

      case ADP_TRSCN_TYPE_RERR_TRX_REQ:
           {
               UINT16_t destAddr, origAddr;
               SYS_ADJ_BD_HEAD(bdHndl, SYS_TRSCN_Q_HNDL_FIELD_LEN + SYS_TRSCN_TYPE_FIELD_LEN);
               buff_p = SYS_GET_BD_BUFF(bdHndl);
               destAddr = UTIL_ntohs(buff_p);
               origAddr = UTIL_ntohs(buff_p + MAC_SHORT_ADDR_LEN);
               SYS_freeMem(bdHndl);
               AODVL_buildSendRtErrMsg(destAddr,   // Address to which route discovery failed
                                       origAddr);  // Packet originator
           }
           break;
#endif
      case ADP_TRSCN_TYPE_MESH_PDU:
      case ADP_TRSCN_TYPE_MESH_BC_PDU:
           {
               // This packet has the MAC header also. 
               // Strip the two byte prefix (handle and transaction type)
               SYS_ADJ_BD_HEAD(bdHndl, SYS_TRSCN_Q_HNDL_FIELD_LEN + SYS_TRSCN_TYPE_FIELD_LEN);
               ADP_cntxt.pendTxReqSrc = ADP_TX_REQ_SRC_FWD_PATH;
               MAC_sendAdpFwdPkt(bdHndl, trscnType == ADP_TRSCN_TYPE_MESH_BC_PDU ? 0x1 : 0x0);
           }
           break;
     
#ifdef LPWMN_COORD
      case ADP_TRSCN_TYPE_RFD_REASSOC:
           {
               ADP_sendRFDReAssocMsg(bdHndl, buff_p);
           }
           break;

      case ADP_TRSCN_TYPE_ASSOC_RESP_TO_RELAY:
           {
               ADP_sendAssocRespToRelay(bdHndl, buff_p);
           }
           break;
           
      case ADP_TRSCN_TYPE_ASSOC_REQ:
           {
              NM_nodeListEntry_s *entry_p;
              UINT8_t status;

              // <Extended address> <Capability info>
              
              entry_p = NM_lookUpNode(buff_p);
              if (entry_p == NULL)
              {
                 entry_p = NM_allocNodeTblEntry();
                 memcpy(entry_p->extAddr, buff_p, MAC_EXT_ADDR_LEN);      
              }   
   
              if (entry_p != NULL)
              {
                  if (entry_p->shortAddr != MAC_BROADCAST_SHORT_ADDR)
                  {
                      UINT8_t flag = 0;

                      if ((entry_p->nodeCapInfo & MAC_ASSOC_REQ_CAP_INFO_DEV_TYPE_BIT_SHIFT_MSK) == 0x0)
                      {
                          if (entry_p->parentNodeIdx == NM_MAX_SUPPORTED_NODE_CNT)
                          {
                              // Reassociating node is an RFD and it's parent is this COORD. Delete any
                              // PDUs pending indirect transmission to this RFD.
                              NM_purgeRFDPendTrscnQ(entry_p);
                          }
                      }
                      else
                      {
                          // Reassociating node is an FFD. Delete all associated RFDs since the RFDs
                          // will reassociate.

                          NM_deleteRFDs(entry_p - NM_cntxt.NM_nodeList);
                      }

                      // Check if route discovery is pending for this node (could be FFD or RFD)
                      if (AODVL_lookUpNodeInRtDiscTbl(entry_p->shortAddr))
                          flag = 1;

                      AODVL_purgeRtTbls(entry_p->shortAddr);

                      if (flag)
                          ADP_procRtDiscOpnRslt(entry_p->shortAddr, MAC_BROADCAST_SHORT_ADDR);

#ifdef ADP_FRAG_ENA
                      ADP_freeFragCntxt(entry_p->shortAddr);
#endif
                  }

                  entry_p->shortAddr = NM_allocShortAddr();
                  entry_p->reassocTmo = 0;  // rkris@wisense.in / sep/23/15 (directly connected so 0)
                  entry_p->nodeCapInfo = *(buff_p + MAC_EXT_ADDR_LEN);
                  entry_p->parentNodeIdx = NM_MAX_SUPPORTED_NODE_CNT;
                  entry_p->lastSysFEId = UTIL_ntohs(buff_p + MAC_EXT_ADDR_LEN + MAC_NODE_CAPABILITY_INFO_LEN);
                  status = MAC_ASSOCIATION_STS_SUCCESS;
              }
              else
              {
                  // Node table entry is full. Reject this association request
                  status = MAC_ASSOCIATION_STS_LPWMN_AT_CAPACITY;
              }   
              
              // Now build and send MAC-ASSOC-RESP message to directly
              // attached node (Cnfrm not required).
              MAC_buildSendAssocnRespCmd(buff_p,
                                         status,
                                         status == MAC_ASSOCIATION_STS_SUCCESS ? \
                                                   entry_p->shortAddr : MAC_BROADCAST_SHORT_ADDR);
              SYS_freeMem(bdHndl);
#ifdef GW_ENA
              // Send event to host
              if (status == MAC_ASSOCIATION_STS_SUCCESS)
                  GW_sendNodeRegEvt(entry_p);
#endif
           }
           break;
#else
#ifndef DEV_TYPE_RFD
      case ADP_TRSCN_TYPE_ASSOC_REQ:
           {
              UINT16_t nextHop; 
              UINT8_t nodeCap;
              
              // Format : <dest-short> <next-hop> <ext addr> <cap info>
              
              buff_p += MAC_SHORT_ADDR_LEN;  // Skip the destination short address
              nextHop = UTIL_ntohs(buff_p);  // *((UINT16_t *)(buff_p));
              buff_p += MAC_SHORT_ADDR_LEN;  // Skip the next hop address
              
              nodeCap = *(buff_p + MAC_EXT_ADDR_LEN);
              if ((nodeCap & MAC_ASSOC_REQ_CAP_INFO_DEV_TYPE_BIT_SHIFT_MSK) == 0x0)
              {
                  // This node is an RFD. Check if this node is re-associating.
                  UINT8_t assocRFDListIdx = MAC_lookUpExtAddrInAssocRFDList(buff_p);
                  if (assocRFDListIdx < MAC_MAX_ASSOC_RFD_CNT) 
                  {
                      // This RFD is re-associating. If there are any PDUs pending 
                      // indirect transmission to this node delete them.
                      MAC_purgeRFDPendFrames(assocRFDListIdx);
                  }
              }

              // Callee frees transaction bd handle
              ADP_relayAssocReqMsg(bdHndl, nextHop, buff_p,
                                   buff_p[MAC_EXT_ADDR_LEN],
                                   UTIL_ntohs(buff_p + MAC_EXT_ADDR_LEN + MAC_NODE_CAPABILITY_INFO_LEN));
           }
           break;

      case ADP_TRSCN_TYPE_ASSOC_RESP:    
           {
              UINT16_t allocShortAddr = UTIL_ntohs(buff_p + MAC_EXT_ADDR_LEN + MAC_ASSOC_RESP_STS_LEN);
              UINT8_t nodeCap = buff_p[MAC_EXT_ADDR_LEN + MAC_ASSOC_RESP_STS_LEN + MAC_SHORT_ADDR_LEN];


              ADP_cntxt.relayedAssocRespRcvdCnt ++;

              // This node has received an ASSOC-RESP from the network coordinator
              // Send it to the associating node
             
              // Relayed ASSOC-RESP message layout is -
              // <extended addr> <status> <allocated short address> <node_cap_info byte>

              // If joining node is an RFD, add it to the list of RFDs associated through this FFD
              if ((nodeCap & MAC_ASSOC_REQ_CAP_INFO_DEV_TYPE_BIT_SHIFT_MSK)
                  || (MAC_addToAssocRFDList(allocShortAddr, buff_p)))
              {

                  // Now build and send MAC-ASSOC-RESP message
                  // Cnfrm not required.
                  MAC_buildSendAssocnRespCmd(buff_p,
                                             buff_p[MAC_EXT_ADDR_LEN],
                                             allocShortAddr);
              }
              SYS_freeMem(bdHndl);
           }
           break;        
#endif
#endif
           
      default:
           SYS_fatal(SYS_FATAL_ERR_3101);
           break;
   }
  
   if (SYS_TRSCN_Q_IS_EMPTY(trscnQ_p))
       SYS_globalEvtMsk1 &= ~SYS_GLOBAL_EVT_ADP_TRSCN_PENDING;
   
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
void ADP_evtHndlr(UINT16_t globalEvtId, void *params_p)
{
    switch (globalEvtId)
    {
       case SYS_GLOBAL_EVT_MAC_DATA_CONFIRM:
            {
               MCPS_dataConfirmParams_s *cnfrmParams_p;
               
               cnfrmParams_p = (MCPS_dataConfirmParams_s *)params_p;
#ifdef GW_ENA
               GW_sendMacTxCnfrmEvt(cnfrmParams_p->destShortAddr,
            		                (UINT8_t)cnfrmParams_p->status,
									(UINT8_t)ADP_cntxt.pendTxReqSrc);
#endif

               switch (ADP_cntxt.pendTxReqSrc)
               {
#if defined(LPWMN_COORD) && defined(GW_ENA)
                  case ADP_TX_REQ_SRC_GW:
                       {
                          GW_relayCnfrm(LPWMN_MAC_MODULE, (UINT8_t)cnfrmParams_p->status);

                          // On the gateway, free the packet copy created in ADP_sendFrag()
                          SYS_freeMem(ADP_cntxt.txParams.bdHndl);  // Bug fix (6/Aug/15)

                          ADP_clearAppTxParams();  // Bug fix (15/Nov/14)
                       }
                       break;
#endif

#if defined(DEV_TYPE_FFD) || defined(DEV_TYPE_RFD)
                  case ADP_TX_REQ_SRC_UC_FU:
                       {
                    	   ADP_fragTxDoneHndlr(cnfrmParams_p);
                       }
                       break;
#endif

                  case ADP_TX_REQ_SRC_APP:
                       {
#ifdef LPWMN_COORD
                          if (cnfrmParams_p->status == MAC_STS_SUCCESS)
                              ADP_cntxt.cnfrmSts.sts = ADP_STS_SUCCESS;
                          else
                              ADP_cntxt.cnfrmSts.sts = ADP_STS_TX_FLR;
#ifdef DEV_TYPE_RFD
                          ADP_cntxt.cnfrmSts.framePending = cnfrmParams_p->framePending;
#endif
                          
                          ADP_clearAppTxParams();
               
                          SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_ADP_DATA_CONFIRM;
#else
                          ADP_fragTxDoneHndlr(cnfrmParams_p);
#endif
                       }
                       break;

#ifndef DEV_TYPE_RFD
                  case ADP_TX_REQ_SRC_AODVL:
                  case ADP_TX_REQ_SRC_FWD_PATH:
                  case ADP_TX_REQ_SRC_ASSOC_REQ_RELAY:
                  case ADP_TX_REQ_SRC_ASSOC_RESP_TO_RELAY:
                       {
                          // MAC data confirm for forawrded packet ... 
                          if (cnfrmParams_p->status != MAC_STS_SUCCESS)
                          {
                              // Remove all routes through this next hop 
                              AODVL_clearRoutes(cnfrmParams_p->destShortAddr);
                              
                              cnfrmParams_p->destShortAddr = MAC_BROADCAST_SHORT_ADDR;
                          }
                       }
                       break;                              
#endif

                  default:
                       break;
               }                       
            }
            break;
        
       case SYS_GLOBAL_EVT_MAC_DATA_INDICATION:     
            {  
               ADP_procRcvdPkt((MCPS_dataIndParams_s *)params_p);
            }
            break;
            
            
       case SYS_GLOBAL_EVT_ADP_TRSCN_PENDING:
            {
               // ADP receives this event only when MAC is free ....
               ADP_procPendingTrscns();
            }
            break;
            
       default:
            {
            }
            break;         
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
void ADP_init(void)
{
	memset(&ADP_cntxt, 0, sizeof(ADP_cntxt));

#ifdef IPv4_SUPPORT_ENA
    ADP_cntxt.ipv4NetAddr = 0xc0a8;
#endif
    
    ADP_cntxt.pendTxReqSrc = ADP_TX_REQ_SRC_NONE;
    
#ifndef DEV_TYPE_RFD
    memset(ADP_cntxt.fwdPktQ, 0xff, sizeof(ADP_cntxt.fwdPktQ));
    ADP_cntxt.fwdPktQHead = ADP_FWD_PKT_Q_LEN;
#endif

    ADP_cntxt.txParams.destShortAddr = MAC_BROADCAST_SHORT_ADDR;
    ADP_cntxt.txParams.nextHopAddr = MAC_BROADCAST_SHORT_ADDR;
    
    SYS_initTrscnsQ(&(ADP_cntxt.trscnQ));

#ifndef DEV_TYPE_RFD
    SYS_initTrscnsQ(&(ADP_cntxt.rtPendFwdPktTrscnQ));
    SYS_initTrscnsQ(&(ADP_cntxt.assocRtPendTrscnQ));
#endif

#ifdef LPWMN_COORD
#ifdef ADP_FRAG_ENA
    ADP_initReassyCntxts();
#endif
#endif

#if defined(DEV_TYPE_FFD) && defined(FFD_RBT_ON_RPT_RT_FLR_TO_COORD)
    ADP_cntxt.rtDiscToCoordFlrCnt = 0;
#endif

    return;
}
