/*
 * File Name: aodvl.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Sep/5/2013
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

#if !defined(DEV_TYPE_RFD) && !defined(DEV_TYPE_SNIFFER)

#include <string.h>
#include <typedefs.h>
#include <system.h>
#include <pltfrm.h>
#include <radio.h>
#include <mac.h>
#include <mac_pib.h>
#include <adp.h>
#include <aodvl.h>
#include <node_mgr.h>
#include <util.h>
#include <gw.h>

AODVL_cntxt_s AODVL_cntxt;

AODVL_routingTblEntry_s *AODVL_lookUpRtTbl(UINT16_t destAddr);


#ifdef LPWMN_COORD
/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
void AODVL_sendPathDiscCnrm(ADP_sts_t sts, UINT8_t bdHndl)
{
   if (AODVL_cntxt.pathDiscCnfrmCbFunc_p != NULL)
       (*AODVL_cntxt.pathDiscCnfrmCbFunc_p)(sts, bdHndl);
   else
   {
       if (bdHndl < SYS_MAX_BUFF_DESC_CNT)
           SYS_freeMem(bdHndl);
   }
   AODVL_cntxt.pathDiscInProgress = 0x0;
   AODVL_cntxt.pathDiscTgt = MAC_BROADCAST_SHORT_ADDR;
   AODVL_cntxt.pathDiscCnfrmCbFunc_p = NULL;
   return;
}
#endif

/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
void AODVL_periodicTmrExpCbFunc(void)
{
   UINT8_t idx, activeTmrCnt = 0;
   AODVL_routeDiscTblEntry_s *entry_p = &(AODVL_cntxt.rtDiscTbl[0]);
   
#ifdef LPWMN_COORD
   if (AODVL_cntxt.pathDiscInProgress)
   {
       if (AODVL_cntxt.pathDiscTmrTTE > 0)
       {
           AODVL_cntxt.pathDiscTmrTTE --;
           if (AODVL_cntxt.pathDiscTmrTTE == 0)
           {
               GW_sendPathDiscEvt(LPWMN_GW_EVT_TYPE_PATH_DISC_OVER,
                                  ADP_STS_PATH_DISC_TIMED_OUT,
                                  SYS_MAX_BUFF_DESC_CNT);

               AODVL_sendPathDiscCnrm(ADP_STS_PATH_DISC_TIMED_OUT, 
                                      SYS_MAX_BUFF_DESC_CNT);
           }
           else
               activeTmrCnt ++;
       }
       else
       {
           SYS_fatal(SYS_FATAL_ERR_67);
       }
   }
#endif

   for (idx=0; idx<AODVL_RT_DISC_TBL_SZ; idx++)
   {
      if (entry_p->destAddr != MAC_BROADCAST_SHORT_ADDR
          && entry_p->timeToExpiry > 0)  // rkris@wisense.in / feb/24/16
      {
          // "timeToExpiry" can be 0 when a route disc request is received but periodic
    	  // timer expires before RREQ message gets sent out.
          
          entry_p->timeToExpiry --;
          if (entry_p->timeToExpiry == 0)
          {
              if (entry_p->origAddr != MAC_PIB.macShortAddress)
              { 
                  // Delete this route request table entry
                  entry_p->destAddr = MAC_BROADCAST_SHORT_ADDR;
              }
              else
              {
                  if (entry_p->_union_0.rtRepRcvdCnt > 0)
                  {
                      // Route discovery has passed ....
                      ADP_procRtDiscOpnRslt(entry_p->destAddr,
                                            entry_p->downStreamHop);
#ifdef GW_ENA
                      GW_sendLocalRtDiscOverEvt(entry_p->destAddr,
                                                entry_p->downStreamHop);
#endif

#ifdef DEV_TYPE_FFD
                      // rkris@wisense.in / June/9/15
                      // Note this FFD's cost to the LPWMN coordinator
                      if (entry_p->destAddr == LPWMN_COORD_SHORT_ADDR)
                      {
                          AODVL_cntxt.coordRtCost = entry_p->residualRtCost;
                          AODVL_cntxt.coordWLC = entry_p->residualWLC;
                      }
#endif
                      
                      // Delete this route request table entry
                      entry_p->destAddr = MAC_BROADCAST_SHORT_ADDR;                      
                  }
                  else
                  {
                      // Route discovery has failed !!
                      if (entry_p->rtDiscTryCnt >= AODVL_MAX_RT_DISC_TRY_CNT)
                      {
                          // Give up !!
                          ADP_procRtDiscOpnRslt(entry_p->destAddr,
                                                MAC_BROADCAST_SHORT_ADDR);
#ifdef GW_ENA
                          GW_sendLocalRtDiscOverEvt(entry_p->destAddr,
                        		                    MAC_BROADCAST_SHORT_ADDR);
#endif
                          // Delete this route request table entry
                          entry_p->destAddr = MAC_BROADCAST_SHORT_ADDR;
                      }
                      else
                      {
                          entry_p->rtReqId = AODVL_cntxt.nextRtReqId;
                          AODVL_cntxt.nextRtReqId ++;
                      
                          entry_p->timeToExpiry = AODVL_cntxt.netTraversalTime;
                          entry_p->rtReqPending = 0x1;
                          SYS_globalEvtMsk0 |= AODVL_CTRL_PKT_TX_REQUEST;
#ifdef GW_ENA
                          GW_sendLocalRtDiscStartedEvt(entry_p->destAddr, entry_p->rtDiscTryCnt);
#endif
                      }
                  }
              }
          }
          else
              activeTmrCnt ++;
      }
      
      entry_p ++;
   }
   
#if 0
   // rkris, Nov/16/2013, 1 Hz timer is always running
   if (activeTmrCnt == 0)
   {
       // Stop the periodic timer
       PLTFRM_stop1HzPeriodicTimer();
       AODVL_cntxt.timerActive = 0x0;
   }
#endif
}


/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
UINT8_t AODVL_lookUpNodeInRtDiscTbl(UINT16_t nodeAddr)
{
   UINT8_t idx;
   AODVL_routeDiscTblEntry_s *entry_p = &(AODVL_cntxt.rtDiscTbl[0]);

   for (idx=0; idx<AODVL_RT_DISC_TBL_SZ; idx++)
   {
        if (entry_p->destAddr == nodeAddr)
        {
            break;
        }
        else
            entry_p ++;
   }

   if (idx >= AODVL_RT_DISC_TBL_SZ)
       entry_p = NULL;

   return entry_p != NULL ? 0x1 : 0x0;
}


/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
AODVL_routeDiscTblEntry_s *AODVL_lookUpRtDiscTbl(UINT16_t origAddr, 
                                                 UINT8_t reqId)
{
   UINT8_t idx;
   AODVL_routeDiscTblEntry_s *entry_p = &(AODVL_cntxt.rtDiscTbl[0]);
   
   for (idx=0; idx<AODVL_RT_DISC_TBL_SZ; idx++)
   {
        if (entry_p->origAddr == origAddr
            && entry_p->rtReqId == reqId)
        {
            break;
        }
        else
            entry_p ++;
   }
  
   if (idx >= AODVL_RT_DISC_TBL_SZ)
       entry_p = NULL;   
   
   return entry_p;
}


/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
AODVL_routeDiscTblEntry_s *AODVL_allocRtDiscTblEntry(void)
{
   UINT8_t idx;
   AODVL_routeDiscTblEntry_s *entry_p = &(AODVL_cntxt.rtDiscTbl[0]);
   
   for (idx=0; idx<AODVL_RT_DISC_TBL_SZ; idx++)
   {
       if (entry_p->destAddr == MAC_BROADCAST_SHORT_ADDR)
           break;
       else
           entry_p ++;
   }
     
   if (idx >= AODVL_RT_DISC_TBL_SZ)
   {
       // Route discovery table is full !!
       entry_p = NULL;

       AODVL_cntxt.rtDiscTblFullCnt ++;
   }   
   else
   {     
       memset(entry_p, 0, sizeof(AODVL_routeDiscTblEntry_s));
   }
   
   return entry_p;
}


/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
AODVL_routingTblEntry_s *AODVL_allocRtTblEntry(void)
{
   UINT8_t idx;
   AODVL_routingTblEntry_s *entry_p = AODVL_cntxt.routingTbl;
   
   for (idx=0; idx<AODVL_ROUTING_TBL_SZ; idx++)
   {
       if (entry_p->destAddr == MAC_BROADCAST_SHORT_ADDR)
           break;
       entry_p ++;
   }

#ifdef AODVL_RT_TBL_IN_MRU_ORDER
   if (idx >= AODVL_ROUTING_TBL_SZ)
   {
       UINT8_t prevIdx;

       idx = AODVL_cntxt.rtTblHeadIdx;

       // Delete the least recently used routing table entry

       do
       {
          prevIdx = idx;
          idx = AODVL_cntxt.routingTbl[idx].nextIdx;
          if (idx == AODVL_RT_TBL_INV_IDX
              || AODVL_cntxt.routingTbl[idx].nextIdx == AODVL_RT_TBL_INV_IDX)
          {
              break;
          }
       } while (1);

       entry_p = &(AODVL_cntxt.routingTbl[prevIdx]);
       idx = entry_p->nextIdx;
       if (idx != AODVL_RT_TBL_INV_IDX)
       {
           entry_p->nextIdx = AODVL_RT_TBL_INV_IDX;
           entry_p = &(AODVL_cntxt.routingTbl[idx]);
       }
   }
   
   // Insert at the head of the MRU queue
   if (idx != AODVL_cntxt.rtTblHeadIdx)
   {
      entry_p->nextIdx = AODVL_cntxt.rtTblHeadIdx;
      AODVL_cntxt.rtTblHeadIdx = idx;
      entry_p->destAddr = MAC_BROADCAST_SHORT_ADDR;
   }

#else
   if (idx >= AODVL_ROUTING_TBL_SZ)
       entry_p = NULL;
#endif

   return entry_p;
}


/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
UINT8_t AODVL_calcTotalRtCost(UINT8_t rcvdRtCost, 
                              UINT16_t senderAddr,
                              SINT8_t measLQI)
{
   UINT16_t totRtCost = rcvdRtCost;
   totRtCost ++;  // Hop count
   if (totRtCost > AODVL_MAX_RT_COST_VAL)   
       totRtCost = AODVL_MAX_RT_COST_VAL;
   return totRtCost;
}


/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
UINT8_t AODVL_calcTotalWLC(UINT8_t rcvdWLC,
                           UINT16_t senderAddr,
                           SINT8_t measLQI)
{
   UINT8_t totWLC = rcvdWLC;
   if (measLQI < AODVL_WEAK_LINK_THRESHOLD_LQI)
       totWLC ++;
   if (totWLC > AODVL_MAX_WLC)
       totWLC = AODVL_MAX_WLC;
   return totWLC;
}


/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
void AODVL_pktTxCnfrmHndlr(void)
{
  
}

#ifdef LPWMN_COORD
#ifndef AODVL_PATH_DISC_DISABLED
/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
ADP_sts_t AODVL_buildSendPathReqMsg(void)
{
   AODVL_routingTblEntry_s *rtTblEntry_p;
   ADP_sts_t sts = ADP_STS_SUCCESS;

   rtTblEntry_p = AODVL_lookUpRtTbl(AODVL_cntxt.pathDiscTgt);
   if (rtTblEntry_p != NULL)
   {
       UINT8_t bdHndl = SYS_allocMem(ADP_ELEMENT_TYPE_LEN + AODVL_PATH_REQ_MSG_MIN_LEN);
       if (SYS_BD_HNDL_IS_VALID(bdHndl))
       {
           UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);

          /*
           * Byte 0
           *   > Bits 0 - 2 -> Message Type
           *   > Bit 3 - 7 -> reserved
           * Bytes 1-2: PREQ originator
           * Bytes 3-4: PREQ destination
           * Byte 5: Fwd hop count (N1)
           * Bytes 6..: Forward hops (not including originator and destination)
           * Byte 6 + (N1*2): Status byte
           * Byte 6 + (N1*2) + 1: Reverse hop count (N2)
           * Bytes 6 + (N1*2) + 1 + 1 ..... : Reverse hops (not including originator and destination)
           */

          *(buff_p ++) = ADP_ELEMENT_TYPE_AODVL;
          *(buff_p ++) = AODVL_MSG_TYPE_PATH_REQ;
          buff_p = UTIL_htons(buff_p, MAC_PIB.macShortAddress);
          buff_p = UTIL_htons(buff_p, rtTblEntry_p->destAddr);
          *(buff_p ++) = 0;   // hop count

          ADP_sendSigPkt(bdHndl, rtTblEntry_p->nextHop, 1);
      }
      else
          sts = ADP_STS_OOM;
   }
   else
      sts = ADP_STS_NO_ROUTE;

   return sts;
}
#endif
#endif


/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
void AODVL_buildSendRouteMsg(AODVL_routeDiscTblEntry_s *entry_p,
                             UINT8_t msgType)
{
   UINT8_t bdHndl;
    
   bdHndl = SYS_allocMem(ADP_ELEMENT_TYPE_LEN + AODVL_RT_REQ_MSG_LEN);
   if (SYS_BD_HNDL_IS_VALID(bdHndl))
   {
       UINT8_t byte;
       UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);

       *(buff_p ++) = ADP_ELEMENT_TYPE_AODVL;
  
       /*
        * Byte 0 
        *   > Bits 0 - 2 -> Message Type
        *   > Bit 3 -> Local Route Repair
        *   > Bits 4 - 7  -> cost Type
        * Bytes 1 : Route cost  
        * Byte  2 : Weak link count (bits 0 to 3) 
        * Byte  3 : Route request id
        * Bytes 4 - 5 : Destination short address
        * Bytes 6 - 7 : Originator short address
        */

       byte = AODVL_DFLT_RT_COST_TYPE;
       byte = (byte << AODVL_RT_REQ_MSG_COST_TYPE_SHIFT);
       byte &= AODVL_RT_REQ_MSG_COST_TYPE_SHIFT_BM;
       byte |= msgType;    
       byte |= (entry_p->rtRepairFlag ? AODVL_RT_REQ_MSG_RT_REPAIR_FLAG_SHIFT_BM : 0x0);
       *(buff_p ++) = byte;
   
       if (msgType == AODVL_MSG_TYPE_RT_REQ)
       {
           *(buff_p ++) = entry_p->_union_0.fwdRtCost;
           *(buff_p ++) = entry_p->fwdWLC;
       }
       else
       {
           *(buff_p ++) = entry_p->residualRtCost;
           *(buff_p ++) = entry_p->residualWLC; 
       }

       *(buff_p ++) = entry_p->rtReqId;
       buff_p = UTIL_htons(buff_p, entry_p->destAddr);
       buff_p = UTIL_htons(buff_p, entry_p->origAddr);
   
       ADP_sendSigPkt(bdHndl, 
                      msgType == AODVL_MSG_TYPE_RT_REQ ? \
                                 MAC_BROADCAST_SHORT_ADDR : entry_p->upStreamHop, 
                      msgType == AODVL_MSG_TYPE_RT_REQ ? 0x0 : 0x1);
   }  
   else
   {
       // Will retry again anyway ...
   }

   return;
}


/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
void AODVL_buildSendRtErrMsg(UINT16_t rtErrAddr, UINT16_t pktOrigAddr)
{
    UINT16_t nextHop;
    UINT8_t bdHndl, pktLen = ADP_ELEMENT_TYPE_LEN + AODVL_RT_ERR_MSG_LEN;

    /*
     * Byte 0
     *   > Bits 0 - 2 -> Message Type
     *   > Bit 3 - 7 -> reserved
     * Bytes 1 - 2 : Address to which this node failed to obtain a route.
     */
    nextHop = ADP_getRoute(pktOrigAddr);
    if (nextHop == MAC_BROADCAST_SHORT_ADDR)
        return;

    if (nextHop != pktOrigAddr)
    {
        // Mesh header
        pktLen += (ADP_ELEMENT_TYPE_LEN + ADP_MESH_HDR_LEN);
    }

    bdHndl = SYS_allocMem(pktLen);
    if (SYS_BD_HNDL_IS_VALID(bdHndl))
    {
        UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl);
        if (nextHop != pktOrigAddr)
            buff_p = ADP_prefixMeshHdr(buff_p, pktOrigAddr);
        *buff_p = ADP_ELEMENT_TYPE_AODVL;
        buff_p += ADP_ELEMENT_TYPE_LEN;
        *buff_p = AODVL_MSG_TYPE_RT_ERR;
        buff_p += AODVL_MSG_FLAGS_FIELD_LEN;
        buff_p = UTIL_htons(buff_p, rtErrAddr);

        ADP_sendSigPkt(bdHndl, nextHop, 0x1);
    }

    return;
}


/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
void AODVL_pktTxProc(void)
{
   UINT8_t idx, rtOpnStarted = 0, rtOpnPending = 0;
   AODVL_routeDiscTblEntry_s *entry_p = &(AODVL_cntxt.rtDiscTbl[0]);

#ifdef LPWMN_COORD
#ifndef AODVL_PATH_DISC_DISABLED
   if (AODVL_cntxt.pathDiscTgt != MAC_BROADCAST_SHORT_ADDR
       && AODVL_cntxt.pathDiscInProgress == 0x0)
   {
       ADP_sts_t sts;

       // Path discovery requested 
       if ((sts = AODVL_buildSendPathReqMsg()) == ADP_STS_SUCCESS)
       {
           AODVL_cntxt.pathDiscTmrTTE = AOVL_PATH_RESP_TMO_SECS;
           AODVL_cntxt.pathDiscInProgress = 0x1;
           rtOpnStarted = 1;

           GW_sendPathDiscEvt(LPWMN_GW_EVT_TYPE_PATH_DISC_STARTED,
                              sts, SYS_MAX_BUFF_DESC_CNT);
       }
       else
       {
           GW_sendPathDiscEvt(LPWMN_GW_EVT_TYPE_PATH_DISC_OVER,
                              sts, SYS_MAX_BUFF_DESC_CNT);
           AODVL_cntxt.pathDiscTgt = MAC_BROADCAST_SHORT_ADDR;
       }
   }   
#endif
#endif
   
   for (idx=0; idx<AODVL_RT_DISC_TBL_SZ; idx++)
   {
       if (entry_p->rtReqPending)
       {
           if (rtOpnStarted)
           {
               rtOpnPending = 1;
               break;
           }
           
           entry_p->rtReqPending = 0x0;
           AODVL_buildSendRouteMsg(entry_p, AODVL_MSG_TYPE_RT_REQ);
           if (entry_p->origAddr == MAC_PIB.macShortAddress)
           {
               // Locally initiated route discovery.
               // After broadcasting a RREQ, a node waits for a RREP. If a route
               // is not discovered within NET_TRAVERSAL_TIME milliseconds, the 
               // node MAY try again the discovery process a maximum of RREQ_RETRIES 
               // times.
               entry_p->timeToExpiry = AODVL_cntxt.netTraversalTime;
               entry_p->rtDiscTryCnt ++;
           } 
           rtOpnStarted = 1;
       }
       
       if (entry_p->rtRepPending)
       {
           if (rtOpnStarted)
           {
               rtOpnPending = 1;
               break;
           }
           
           entry_p->rtRepPending = 0x0;
           AODVL_buildSendRouteMsg(entry_p, AODVL_MSG_TYPE_RT_REP);
           rtOpnStarted = 1;
       }
     
       entry_p ++;
   }
   
   if (rtOpnPending)
       SYS_globalEvtMsk0 |= AODVL_CTRL_PKT_TX_REQUEST; 
   else
       SYS_globalEvtMsk0 &= ~AODVL_CTRL_PKT_TX_REQUEST; 
   
   return;

}


/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
UINT8_t AODVL_isNewFwdRtBetter(AODVL_routeDiscTblEntry_s *entry_p, 
                               UINT8_t newRtCost, UINT8_t newWLC)
{
    UINT8_t rc = 1;
        
    /*
     * If the CT field of the RREQ is 0 (i.e. hop count while avoiding weak
     * links), the route cost becomes a tuple of (WL, RC) and is ordered
     * lexicographically. That is, the route cost (WL,RC) is said to be
     * better (or smaller) than or equal to (WL',RC') if the following
     * condition holds.
     *
     * (WL,RC) <= (WL',RC') if and only if WL < WL', or WL == WL' and RC <= RC' 
     */
    
    if ((newWLC > entry_p->fwdWLC)
        || (newWLC == entry_p->fwdWLC) && (newRtCost >= entry_p->_union_0.fwdRtCost))
    {
        rc = 0;
    }
    
    return rc;
}


/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
UINT8_t AODVL_isNewRevRtBetter(AODVL_routeDiscTblEntry_s *entry_p, 
                               UINT8_t newRtCost, UINT8_t newWLC)
{
    UINT8_t rc = 1;
        
    /*
     * If the CT field of the RREQ is 0 (i.e. hop count while avoiding weak
     * links), the route cost becomes a tuple of (WL, RC) and is ordered
     * lexicographically. That is, the route cost (WL,RC) is said to be
     * better (or smaller) than or equal to (WL',RC') if the following
     * condition holds.
     *
     * (WL,RC) <= (WL',RC') if and only if WL < WL', or WL == WL' and RC <= RC' 
     */
    
    if ((newWLC > entry_p->residualWLC)
        || (newWLC == entry_p->residualWLC) && (newRtCost >= entry_p->residualRtCost))
    {
        rc = 0;
    }
    
    return rc;
}


#ifndef AODVL_PATH_DISC_DISABLED
/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
void AODVL_sendPathRep(UINT16_t origAddr, UINT8_t sts, 
                       UINT8_t *msgBuff_p, UINT8_t msgLen)
{
   UINT8_t pktLen, bdHndl;
   UINT16_t nextHop = AODVL_getRoute(origAddr);

   // msgBuff_p is pointing to received PATH-REQ msg.
   // msgLen is the length of the PATH-REQ msg.

   // Check if this node has route back to path request originator
   if (nextHop == MAC_BROADCAST_SHORT_ADDR)
       return;  

   msgBuff_p[0] = AODVL_MSG_TYPE_PATH_REP;
                         
   pktLen = ADP_ELEMENT_TYPE_LEN 
            + msgLen
            + (sts != ADP_STS_SUCCESS ? MAC_SHORT_ADDR_LEN : 0x0)
            + AODVL_PATH_REP_MSG_STS_FIELD_LEN
            + AODVL_PATH_REP_MSG_HOP_CNT_FIELD_LEN;

   bdHndl = SYS_allocMem(pktLen);
   if (SYS_BD_HNDL_IS_VALID(bdHndl))
   {
       UINT8_t *bdBuff_p = SYS_GET_BD_BUFF(bdHndl);

       *(bdBuff_p ++) = ADP_ELEMENT_TYPE_AODVL;
       memcpy(bdBuff_p, msgBuff_p, msgLen);
       bdBuff_p += msgLen;
   
       if (sts != ADP_STS_SUCCESS)
           bdBuff_p = UTIL_htons(bdBuff_p, MAC_PIB.macShortAddress); 
     
       *(bdBuff_p ++) = sts;
       *(bdBuff_p ++) = 0x0;
       
       ADP_sendSigPkt(bdHndl, nextHop, 0x1);
   }
}
                       

/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
void AODLV_fwdPathReqMsg(UINT8_t rcvdPktBdHndl,
                         const UINT8_t rcvdPathReqLen,
                         const UINT16_t nextHop)
{
   UINT8_t newPktBdHndl, pktLen = ADP_ELEMENT_TYPE_LEN + rcvdPathReqLen + MAC_SHORT_ADDR_LEN;  
   const UINT8_t * const rcvdPathReq_p = SYS_GET_BD_BUFF(rcvdPktBdHndl);
    
   newPktBdHndl = SYS_allocMem(pktLen);
   if (SYS_BD_HNDL_IS_VALID(newPktBdHndl))
   {
       UINT8_t *newBuff_p = SYS_GET_BD_BUFF(newPktBdHndl);
   
       /*
        * Byte 0 
        *   > Bits 0 - 2 -> Message Type
        *   > Bit 3 - 7 -> reserved
        * Bytes 1-2: PREQ originator   
        * Bytes 3-4: PREQ destination     
        * Byte 5: Fwd hop count (N1)
        * Bytes 6..: Forward hops (not including originator and destination)
        * Byte 6 + (N1*2): Status byte 
        * Byte 6 + (N1*2) + 1: Reverse hop count (N2)
        * Bytes 6 + (N1*2) + 1 + 1 ..... : Reverse hops (not including originator and destination)
        */

       *(newBuff_p ++) = ADP_ELEMENT_TYPE_AODVL;
       memcpy(newBuff_p, rcvdPathReq_p, rcvdPathReqLen);
       newBuff_p += rcvdPathReqLen;
       UTIL_htons(newBuff_p, MAC_PIB.macShortAddress); 
                       
       SYS_freeMem(rcvdPktBdHndl);  // free the received path request bd
       
       ADP_sendSigPkt(newPktBdHndl, nextHop, 0x1);
   }
   else
   {
       // Forget it !!
   }

   return;
}


/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
void AODVL_processPathReqMsg(UINT8_t bdHndl)
{
   const UINT8_t msgLen = SYS_GET_BD_LEN(bdHndl);
   UINT8_t sts;
 
   /*
    * Byte 0 
    *   > Bits 0 - 2 -> Message Type
    *   > Bit 3 - 7 -> reserved
    * Bytes 1-2: PREQ originator   
    * Bytes 3-4: PREQ destination     
    * Byte 5: Fwd hop count (N1)
    * Bytes 6..: Forward hops (not including originator and destination)
    * Byte 6 + (N1*2): Status byte 
    * Byte 6 + (N1*2) + 1: Reverse hop count (N2)
    * Bytes 6 + (N1*2) + 1 + 1 ..... : Reverse hops (not including originator and destination)
    */   
    
   if (msgLen < AODVL_PATH_REQ_MSG_MIN_LEN)
   {
       SYS_freeMem(bdHndl);
       return;
   }
   else
   { 
       UINT8_t *buff_p = SYS_GET_BD_BUFF(bdHndl), off = AODVL_MSG_FLAGS_FIELD_LEN;
       UINT16_t origAddr = UTIL_ntohs(buff_p + off), destAddr;

       off += MAC_SHORT_ADDR_LEN;
       destAddr = UTIL_ntohs(buff_p + off);
       off += MAC_SHORT_ADDR_LEN;
       
       // Feb/21/16 - rkris@wisense.in
       if ((MAC_verifyUCSA(origAddr) == 0)
    	   || (MAC_verifyUCSA(destAddr) == 0))
       {
    	   SYS_freeMem(bdHndl);
           return;
       }
       
#ifdef LPWMN_COORD

	   // Feb/4/16
       // Node is not registered !! - Looks like coord rebooted and some old node sent
	   // a path request.
       if (NM_lookUpShortAddr(origAddr) == NULL)
       {
    	   SYS_freeMem(bdHndl);
    	   return;
       }

       if (origAddr == MAC_PIB.macShortAddress)
       {
           if ((AODVL_cntxt.pathDiscInProgress != 0x0)
               && (AODVL_cntxt.pathDiscTgt == destAddr))
           {
               // Looks like there is a loop in the network !!
               // Called function should free the BD
               GW_sendPathDiscEvt(LPWMN_GW_EVT_TYPE_PATH_DISC_OVER,
                                  ADP_STS_ROUTING_LOOP,
                                  bdHndl);

               AODVL_sendPathDiscCnrm(ADP_STS_ROUTING_LOOP, bdHndl);
           }
           return;
       }   
       else
#endif
       {
           if (destAddr == MAC_PIB.macShortAddress)
           {
               UINT16_t nextHop = AODVL_getRoute(origAddr);
               
               // PREQ has reached the intended destination. Send PREP back.
               
               if (nextHop != MAC_BROADCAST_SHORT_ADDR)
               {   
                   // Route exists ... 
                   AODVL_sendPathRep(origAddr, ADP_STS_SUCCESS, buff_p, msgLen);
                   SYS_freeMem(bdHndl);
               }
               else
               {
                   // Drop the path request message silently !!
                   SYS_freeMem(bdHndl);
                   return;
               }
           }
           else
           {
               // Check if there is a route to the intended destination
               UINT16_t nextHop = AODVL_getRoute(destAddr);
               if (nextHop == MAC_BROADCAST_SHORT_ADDR)
               {
                   // No route to destination ... send PREP back to originator
                   sts = ADP_STS_NO_ROUTE;                 
                   
                   // Update forward hop count
                   buff_p[off] ++;
                       
                   AODVL_sendPathRep(origAddr, sts, buff_p, msgLen);    
                   SYS_freeMem(bdHndl);
               }
               else
               {  
                   if (buff_p[off] >= (AODVL_ROUTE_MAX_HOP_COUNT - 1))
                   {
                       sts = ADP_STS_MAX_HOPS_EXCEEDED;
                       
                       // Update forward hop count
                       buff_p[off] ++;
                       
                       AODVL_sendPathRep(origAddr, sts, buff_p, msgLen);  
                       SYS_freeMem(bdHndl);
                   }
                   else
                   {
                       // Forward this path request message 
                       
                       // Update the forward hop count
                       buff_p[off] ++;

                       AODLV_fwdPathReqMsg(bdHndl, msgLen, nextHop);
                   }
               }
           }
       }   
   } 
   
   return;
}


/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
void AODVL_processPathRepMsg(UINT8_t rcvdPathRepBdHndl)
{
   const UINT8_t rcvdPathRepLen = SYS_GET_BD_LEN(rcvdPathRepBdHndl);
 
   if (rcvdPathRepLen >= AODVL_PATH_REP_MSG_MIN_LEN)
   { 
       UINT8_t * const rcvdPathRep_p = SYS_GET_BD_BUFF(rcvdPathRepBdHndl);
       UINT8_t off = AODVL_MSG_FLAGS_FIELD_LEN;
       const UINT16_t pathRepOrig = UTIL_ntohs(rcvdPathRep_p + off + MAC_SHORT_ADDR_LEN);
       UINT16_t pathRepDest, nextHop;
       UINT8_t fwHopCnt;
       
       if (MAC_verifyUCSA(pathRepOrig) == 0)
       {
           SYS_freeMem(rcvdPathRepBdHndl);
           return;
       }

       if (pathRepOrig == MAC_PIB.macShortAddress)
       {
           // Looks like there is a loop in the path from PREQ destination to
           // path req originator. Drop this packet !!.
           SYS_freeMem(rcvdPathRepBdHndl);
           return;         
       }

#ifdef LPWMN_COORD
	   // Feb/4/16
       // Node is not registered !! - Looks like coord rebooted and some old node sent
	   // a path reply.
       if (NM_lookUpShortAddr(pathRepOrig) == NULL)
       {
    	   SYS_freeMem(rcvdPathRepBdHndl);
    	   return;
       }
#endif

       pathRepDest = UTIL_ntohs(rcvdPathRep_p + off);
       if (MAC_verifyUCSA(pathRepDest) == 0)
       {
           SYS_freeMem(rcvdPathRepBdHndl);
           return;
       }
       
#ifdef LPWMN_COORD
       if (pathRepDest == MAC_PIB.macShortAddress)
       {
           if (AODVL_cntxt.pathDiscInProgress != 0x0
              && (AODVL_cntxt.pathDiscTgt != MAC_BROADCAST_SHORT_ADDR)
              && (AODVL_cntxt.pathDiscTgt == pathRepOrig))
           { 
              SYS_ADJ_BD_HEAD(rcvdPathRepBdHndl,
                              AODVL_MSG_FLAGS_FIELD_LEN);

              GW_sendPathDiscEvt(LPWMN_GW_EVT_TYPE_PATH_DISC_OVER,
                                 ADP_STS_SUCCESS,
                                 rcvdPathRepBdHndl);

              // Path reply has reached the path request originator ....
              // Callee will free rcvdPathRepBdHndl
              AODVL_sendPathDiscCnrm(ADP_STS_SUCCESS, rcvdPathRepBdHndl);
           }
           else
           {
              SYS_freeMem(rcvdPathRepBdHndl);
           }

           return;
       }
#endif
       
       // Check if this node has route back to path request originator
       nextHop =  AODVL_getRoute(pathRepDest);       
       if (nextHop == MAC_BROADCAST_SHORT_ADDR)
       {
           SYS_freeMem(rcvdPathRepBdHndl);
           return;
       }      
       
       off += (MAC_SHORT_ADDR_LEN*2);
       fwHopCnt = rcvdPathRep_p[off];
       if (fwHopCnt >= (AODVL_ROUTE_MAX_HOP_COUNT - 1))
       {
           // Invalid value !!
           SYS_freeMem(rcvdPathRepBdHndl);
           return;             
       }
       
       // Get the reverse path hop count
       off +=  AODVL_PATH_REP_MSG_HOP_CNT_FIELD_LEN
               + (fwHopCnt * MAC_SHORT_ADDR_LEN) 
               + AODVL_PATH_REP_MSG_STS_FIELD_LEN;
       
       if (rcvdPathRep_p[off] >= (AODVL_ROUTE_MAX_HOP_COUNT - 1))
       {
           // Reached max allowed hops. Drop this packet
           SYS_freeMem(rcvdPathRepBdHndl);
           return;
       }
      
       // Update reverse path hop count 
       rcvdPathRep_p[off] ++;
       
       {
          UINT16_t newBdLen = ADP_ELEMENT_TYPE_LEN
                              + rcvdPathRepLen
                              + MAC_SHORT_ADDR_LEN;

          UINT8_t newPktBdHndl = SYS_allocMem(newBdLen);

          if (SYS_BD_HNDL_IS_VALID(newPktBdHndl))
          {
              UINT8_t *newBuff_p;

              newBuff_p = SYS_GET_BD_BUFF(newPktBdHndl);
              *(newBuff_p ++) = ADP_ELEMENT_TYPE_AODVL;
              memcpy(newBuff_p, rcvdPathRep_p, rcvdPathRepLen);
              newBuff_p += rcvdPathRepLen;
              UTIL_htons(newBuff_p, MAC_PIB.macShortAddress);
              ADP_sendSigPkt(newPktBdHndl, nextHop, 0x1);
          }
       }
   }
   
   SYS_freeMem(rcvdPathRepBdHndl);

   return;
}
#endif

UINT8_t __nonCordRtRepRcvdCnt = 0;

/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
void AODVL_processRtRepMsg(AODVL_rcvdMsgInfo_s *info_p)
{
   UINT8_t msgLen = SYS_GET_BD_LEN(info_p->bdHndl);
    
   if (msgLen < AODVL_RT_REP_MSG_LEN)
       return;
   else
   {  
       UINT8_t *buff_p = SYS_GET_BD_BUFF(info_p->bdHndl);
       UINT16_t origAddr;  
       
       // Get the originator of the RREQ (destination of RREP)
       origAddr = UTIL_ntohs(buff_p + AODVL_RT_REP_MSG_OADDR_FIELD_OFF);
       
       // Feb/21/16 - rkris@wisense.in
       if (MAC_verifyUCSA(origAddr) == 0)
           return;

       /*
        * The node then checks whether it has a route entry for the destination
        * of the RREP (i.e. the originator of the corresponding RREQ). If it
        * does not have the route entry, it just discards the RREP.  Otherwise,
        * it also checks for the existence of the corresponding RREQ entry
        * (which has the same RREQ ID and originator address pair as that of
        * the RREP) in the route request table. If there is no such entry, then
        * it just discards the RREP.
        */
       
       if (origAddr != MAC_PIB.macShortAddress
           && (AODVL_lookUpRtTbl(origAddr) == NULL))
       {
           return;
       }
       else
       {
           UINT8_t reqId = buff_p[AODVL_RT_REQ_MSG_REQ_ID_FIELD_OFF];
           AODVL_routeDiscTblEntry_s *rtDiscTblEntry_p; 
           
           rtDiscTblEntry_p = AODVL_lookUpRtDiscTbl(origAddr, reqId);
           if (rtDiscTblEntry_p == NULL)
               return;
           else
           {
               UINT8_t totRtCost, totWLC;
               UINT16_t destAddr;
               AODVL_routingTblEntry_s *fwdRtTblEntry_p; 
               
               /* 
                * If there is such an entry and the entry has worse reverse route 
                * cost (i.e. higher value) than the route cost of the RREP, the 
                * node updates the entry with the information of the RREP and 
                * forwards it to the previous hop node toward the destination of 
                * the RREP.  If the entry has better reverse route cost (i.e. lower 
                * value) than that of the RREP, the node just discards the RREP.
                */
               
               if (info_p->senderAddr != 0x1)
               {
            	   __nonCordRtRepRcvdCnt ++;
               }

               totRtCost = AODVL_calcTotalRtCost(buff_p[AODVL_RT_REP_MSG_RT_COST_FIELD_OFF], 
                                                 info_p->senderAddr, 
                                                 info_p->measLQI);
               
               if (totRtCost > AODVL_ROUTE_MAX_HOP_COUNT)
               {
                   // Exceeded max number of hops allowed in a route. Drop this RREQ
                   return;
               }
               
               totWLC = AODVL_calcTotalWLC((buff_p[AODVL_RT_REP_MSG_WLC_FIELD_OFF]) & AODVL_RT_REP_MSG_WLC_BM,
                                           info_p->senderAddr, 
                                           info_p->measLQI);     
               
               if (AODVL_isNewRevRtBetter(rtDiscTblEntry_p, totRtCost, totWLC) == 0x0)
               {
                   // Exisiting reverse route is better !!. Drop this RREP
                   return;
               }       
               else
               {
                   rtDiscTblEntry_p->downStreamHop = info_p->senderAddr;            
                   rtDiscTblEntry_p->residualRtCost = totRtCost;
                   rtDiscTblEntry_p->residualWLC = totWLC;
               
                   destAddr = UTIL_ntohs(buff_p + AODVL_RT_REP_MSG_DADDR_FIELD_OFF);

                   // Feb/21/16 - rkris@wisense.in
                   if (MAC_verifyUCSA(destAddr) == 0)
                       return;
             
                   fwdRtTblEntry_p = AODVL_lookUpRtTbl(destAddr);
                   if (fwdRtTblEntry_p == NULL)
                   {
                       fwdRtTblEntry_p = AODVL_allocRtTblEntry( );
                       if (fwdRtTblEntry_p == NULL)
                       {
                           // No space in routing table !! Drop this RREQ
                           return;
                       }
                       fwdRtTblEntry_p->destAddr = destAddr;                   
                   }  
               
                   fwdRtTblEntry_p->nextHop = info_p->senderAddr;      
                   
                   if (origAddr == MAC_PIB.macShortAddress)
                   {
                       // RREP has reached the node which initiated the route discovery
                       // process
                     
                       if (rtDiscTblEntry_p->_union_0.rtRepRcvdCnt == 0x0)
                       { 
                           // First RREP received ... start timer to wait for
                           // better RREPs (high hop count but low weak link count) 
                         
                           rtDiscTblEntry_p->timeToExpiry = AODVL_RT_DISC_HOLD_TIME;  
                       }
                       rtDiscTblEntry_p->_union_0.rtRepRcvdCnt ++;
                   }  
                   else
                   {
                       // Forward the RREP
                       rtDiscTblEntry_p->rtRepPending = 0x1;
                       SYS_globalEvtMsk0 |= AODVL_CTRL_PKT_TX_REQUEST;                         
                   }
               }
           }
       }
   }
   
   return;
}


/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
void AODVL_processRtReqMsg(AODVL_rcvdMsgInfo_s *info_p)
{
   UINT8_t msgLen = SYS_GET_BD_LEN(info_p->bdHndl);

   if (msgLen < AODVL_RT_REQ_MSG_LEN)
       return;
   else
   {
       UINT8_t *buff_p = SYS_GET_BD_BUFF(info_p->bdHndl);
       AODVL_routeDiscTblEntry_s *entry_p;     
       UINT8_t totRtCost, totWLC, reqId, rtCostType;
       UINT16_t destAddr, origAddr, destIsAssocRFD;
       AODVL_routingTblEntry_s *rtTblEntry_p;
       
       origAddr = UTIL_ntohs(buff_p + AODVL_RT_REQ_MSG_OADDR_FIELD_OFF);  

       // Feb/21/16 - rkris@wisense.in
       if (MAC_verifyUCSA(origAddr) == 0)
           return;

       if (origAddr == MAC_PIB.macShortAddress)
       {
           // This RREQ was originated by this node itself !! Drop it. 
           return;
       }
       
#ifdef LPWMN_COORD
	   // Feb/4/16
       // Node is not registered !! - Looks like coord rebooted and some old node sent
	   // a route request.
       if (NM_lookUpShortAddr(origAddr) == NULL)
       {
    	   GW_sendRtReqFromInvSrc(origAddr);
    	   return;
       }
#endif

       rtCostType = buff_p[AODVL_RT_REQ_MSG_FLAGS_FIELD_OFF];
       rtCostType = (rtCostType >> AODVL_RT_REQ_MSG_COST_TYPE_SHIFT) & AODVL_RT_REQ_MSG_COST_TYPE_BM;
       if (rtCostType != AODVL_DFLT_RT_COST_TYPE)
       {
           // Route cost type not supported !!. Drop this RREQ.
           return;
       }
       
       /*
        * When the destination receives a RREQ, it tries to find the entry of the same 
        * originator address and RREQ ID pair in the route request table. If the entry 
        * is found, the destination compares the route cost of the RREQ with the forward 
        * route cost of the entry. If the cost of the RREQ is better than(i.e. less than) 
        * that of the entry, the destination updates the reverse route to the originator 
        * in the routing table and generates a RREP in reply. If the cost of the RREQ is 
        * not less than that of the entry, the destination just discards the RREQ.
        */       
       
       destAddr = UTIL_ntohs(buff_p + AODVL_RT_REQ_MSG_DADDR_FIELD_OFF);

       // Feb/21/16 - rkris@wisense.in
       if (MAC_verifyUCSA(destAddr) == 0)
           return;

#ifdef LPWMN_COORD
       destIsAssocRFD = NM_lookUpLocalRFD(destAddr) ? 0x1 : 0x0;
#else
       destIsAssocRFD = MAC_lookUpAssocRFDList(destAddr) ? 0x1 : 0x0;
#endif

       reqId = buff_p[AODVL_RT_REQ_MSG_REQ_ID_FIELD_OFF];
       
       entry_p = AODVL_lookUpRtDiscTbl(origAddr, reqId);
       if (entry_p == NULL)
       {   
           // This is a new RREQ message
           if ((entry_p = AODVL_allocRtDiscTblEntry( )) == NULL)
           {
               // No free entry in the table !!. Drop this RREQ.
               return; 
           }           

           entry_p->destAddr = destAddr;
           entry_p->origAddr = origAddr;
           entry_p->rtReqId = reqId;
           entry_p->rtRepairFlag = buff_p[AODVL_RT_REQ_MSG_FLAGS_FIELD_OFF] \
                                   & AODVL_RT_REQ_MSG_RT_REPAIR_FLAG_SHIFT_BM;
           entry_p->_union_0.fwdRtCost = AODVL_MAX_RT_COST_VAL;
           entry_p->fwdWLC = AODVL_MAX_WLC;
           
           // Check if the destination is this FFD or an RFD associated with this FFD.
           if (destAddr != MAC_PIB.macShortAddress
               && destIsAssocRFD == 0x0)
           {
               entry_p->residualWLC = AODVL_MAX_WLC;
               entry_p->residualRtCost = AODVL_MAX_RT_COST_VAL;
           }
           
           entry_p->timeToExpiry = AODVL_cntxt.netTraversalTime - 2;
       }

       totRtCost = AODVL_calcTotalRtCost(buff_p[AODVL_RT_REQ_MSG_RT_COST_FIELD_OFF], 
                                         info_p->senderAddr, 
                                         info_p->measLQI);
       
       if (totRtCost > AODVL_ROUTE_MAX_HOP_COUNT)
       {
           // Exceeded max number of hops allowed in a route. Drop this RREQ
           return;
       }
       
       totWLC = AODVL_calcTotalWLC((buff_p[AODVL_RT_REQ_MSG_WLC_FIELD_OFF]) & AODVL_RT_REQ_MSG_WLC_BM,
                                   info_p->senderAddr, 
                                   info_p->measLQI);   
       

       if (AODVL_isNewFwdRtBetter(entry_p, totRtCost, totWLC))
       {
           entry_p->upStreamHop = info_p->senderAddr;            
           entry_p->_union_0.fwdRtCost = totRtCost;
           entry_p->fwdWLC = totWLC;
       }
       else
       {
           // Existing forward route is better !!. Drop this RREQ
           return;
       }
       
       // Create/update route to originator
       if ((rtTblEntry_p = AODVL_lookUpRtTbl(origAddr)) == NULL)
       {
           rtTblEntry_p = AODVL_allocRtTblEntry( );
           if (rtTblEntry_p == NULL)
           {
               // No space in routing table !! Drop this RREQ
               return;
           }
           
           rtTblEntry_p->destAddr = origAddr;
       }
       
       // Update next hop to originator
       rtTblEntry_p->nextHop = info_p->senderAddr;
       
       if (destAddr == MAC_PIB.macShortAddress
           || destIsAssocRFD)

       {
           // This node is the destination of the received RREQ message         
           // Generate route reply
           entry_p->rtRepPending = 0x1;
           SYS_globalEvtMsk0 |= AODVL_CTRL_PKT_TX_REQUEST;                  
       }
       else
       {
           // Forward the RREQ ...        
           entry_p->rtReqPending = 0x1;
           SYS_globalEvtMsk0 |= AODVL_CTRL_PKT_TX_REQUEST;         
       }
   }
   
   return;
}


/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
void AODVL_processRtErrMsg(AODVL_rcvdMsgInfo_s *info_p)
{
   UINT8_t msgLen = SYS_GET_BD_LEN(info_p->bdHndl);

   if (msgLen < AODVL_RT_ERR_MSG_LEN)
       return;
   else
   {
       UINT8_t *buff_p = SYS_GET_BD_BUFF(info_p->bdHndl);
       UINT16_t rtErrAddr = UTIL_ntohs(buff_p + AODVL_MSG_FLAGS_FIELD_LEN);

       // Feb/21/16 - rkris@wisense.in
       if (MAC_verifyUCSA(rtErrAddr))
       {
           AODVL_clearRoutes(rtErrAddr);
       }
   }

   return;
}


/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
void AODVL_rcvdMsgHndlr(AODVL_rcvdMsgInfo_s *info_p)
{
   UINT8_t msgType;
   UINT8_t *buff_p = SYS_GET_BD_BUFF(info_p->bdHndl);
      
   msgType = buff_p[0] & AODVL_RT_REQ_MSG_TYPE_BM;
   
   switch (msgType)
   {
      case AODVL_MSG_TYPE_RT_ERR:
           AODVL_processRtErrMsg(info_p);
           SYS_freeMem(info_p->bdHndl);
           break;

      case AODVL_MSG_TYPE_RT_REQ:
           AODVL_processRtReqMsg(info_p);
           SYS_freeMem(info_p->bdHndl);
           break;
           
      case AODVL_MSG_TYPE_RT_REP:
           AODVL_processRtRepMsg(info_p);
           SYS_freeMem(info_p->bdHndl);
           break;         
           
      case AODVL_MSG_TYPE_PATH_REQ:     
      case AODVL_MSG_TYPE_PATH_REP: 
           {
              UINT8_t *buff_p;
              
              SYS_PULL_BD_HEAD(info_p->bdHndl,  
                               SYS_TRSCN_Q_HNDL_FIELD_LEN + SYS_TRSCN_TYPE_FIELD_LEN);
              
              buff_p = SYS_GET_BD_BUFF(info_p->bdHndl);
              
              buff_p[0] = SYS_MAX_BUFF_DESC_CNT;
              buff_p[1] = msgType == AODVL_MSG_TYPE_PATH_REQ \
                          ? ADP_TRSCN_TYPE_AODVL_PATH_REQ : ADP_TRSCN_TYPE_AODVL_PATH_REP;
                          
              SYS_addToTrscnsQ(&(ADP_cntxt.trscnQ), info_p->bdHndl);     
              SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ADP_TRSCN_PENDING;
           }
           break;
           
      default:
          break;
   }
}


#ifdef LPWMN_COORD
/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
UINT8_t AODVL_pathDiscRequest(AODVL_pathDiscParams_s *params_p)
{
   UINT8_t rc = 1;
   
   if (AODVL_cntxt.pathDiscTgt == MAC_BROADCAST_SHORT_ADDR
       && AODVL_cntxt.pathDiscInProgress == 0x0)
   {
       UINT8_t idx;
       
       for (idx=0; idx<AODVL_ROUTING_TBL_SZ; idx++)
       {
           if (AODVL_cntxt.routingTbl[idx].destAddr == params_p->destAddr)
               break;
       }
       
       if (idx < AODVL_ROUTING_TBL_SZ)
       {
           AODVL_cntxt.pathDiscCnfrmCbFunc_p = params_p->cbFunc_p;
           AODVL_cntxt.pathDiscTgt = params_p->destAddr;
           SYS_globalEvtMsk0 |= AODVL_CTRL_PKT_TX_REQUEST; 
       }

       else
       {
           rc = 0;
           AODVL_cntxt.pathDiscTgt = params_p->destAddr;
           GW_sendPathDiscEvt(LPWMN_GW_EVT_TYPE_PATH_DISC_OVER,
                              ADP_STS_NO_ROUTE,
                              SYS_MAX_BUFF_DESC_CNT);
           AODVL_cntxt.pathDiscTgt = MAC_BROADCAST_SHORT_ADDR;
#ifdef AODVL_PATH_DISC_CAN_TRIGGER_RT_DISC
           AODVL_rtDiscRequest(params_p->destAddr);
#endif
       }

   }
   else
       rc = 0;
   
   return rc;
}
#endif


/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
UINT8_t AODVL_rtDiscRequest(UINT16_t destAddr)
{
   AODVL_routeDiscTblEntry_s *entry_p = &(AODVL_cntxt.rtDiscTbl[0]);
   UINT16_t idx;

   // Locally initiated route discovery. Check if discovery
   // to specified address is already underway

   for (idx=0; idx<AODVL_RT_DISC_TBL_SZ; idx++)
   {
        if (entry_p->destAddr == destAddr)
            break;
        else
            entry_p ++;
   }
   
   if (idx < AODVL_RT_DISC_TBL_SZ)
       return 1;

   if ((entry_p = AODVL_allocRtDiscTblEntry( )) == NULL)
   {
       // No free entry in the table !!
       return 0;
   }
   
   entry_p->destAddr = destAddr;
   entry_p->origAddr = MAC_PIB.macShortAddress;
   entry_p->downStreamHop = MAC_BROADCAST_SHORT_ADDR;
   entry_p->upStreamHop = MAC_BROADCAST_SHORT_ADDR;
   entry_p->rtRepairFlag = 0x0;
   
   entry_p->residualWLC = AODVL_MAX_WLC;
   entry_p->residualRtCost = AODVL_MAX_RT_COST_VAL;
   
   entry_p->rtReqId = AODVL_cntxt.nextRtReqId;
   AODVL_cntxt.nextRtReqId ++;
   
   entry_p->rtReqPending = 0x1;
   entry_p->timeToExpiry = 0;  // Will be set on RREQ transmission  
   
#ifdef GW_ENA
   // Send out event to host
   GW_sendLocalRtDiscStartedEvt(entry_p->destAddr, 0);
#endif

   SYS_globalEvtMsk0 |= AODVL_CTRL_PKT_TX_REQUEST;
   
   return 1;
}


#ifdef AODVL_RT_TBL_IN_MRU_ORDER
/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
void AODVL_clearRoutes(UINT16_t shortAddr)
{
   UINT8_t idx = AODVL_cntxt.rtTblHeadIdx, prevIdx = AODVL_RT_TBL_INV_IDX;

   while (idx != AODVL_RT_TBL_INV_IDX)
   {
       AODVL_routingTblEntry_s *entry_p = &(AODVL_cntxt.routingTbl[idx]);

       if (entry_p->destAddr == shortAddr
           || entry_p->nextHop == shortAddr)
       {
           if (prevIdx == AODVL_RT_TBL_INV_IDX)
               AODVL_cntxt.rtTblHeadIdx = entry_p->nextIdx;
           else
               AODVL_cntxt.routingTbl[prevIdx].nextIdx =  entry_p->nextIdx;
           idx = entry_p->nextIdx;
           entry_p->nextIdx= AODVL_RT_TBL_INV_IDX;
           entry_p->destAddr = MAC_BROADCAST_SHORT_ADDR;
       }
       else
       {
           prevIdx = idx;
           idx = entry_p->nextIdx;
       }
   }

#ifdef DEV_TYPE_FFD
   // rkris@wisense.in - june/8/15
   if (shortAddr == LPWMN_COORD_SHORT_ADDR)
   {
       AODVL_cntxt.coordRtCost = AODVL_MAX_RT_COST_VAL;
       AODVL_cntxt.coordWLC = AODVL_MAX_WLC;
   }
#endif

   return;
}
#else
/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
void AODVL_clearRoutes(UINT16_t shortAddr)
{
   UINT8_t idx;
   
   for (idx=0; idx<AODVL_ROUTING_TBL_SZ; idx++)
   {

       if (AODVL_cntxt.routingTbl[idx].destAddr == shortAddr
           || AODVL_cntxt.routingTbl[idx].nextHop == shortAddr)
       {
           AODVL_cntxt.routingTbl[idx].nextHop = MAC_BROADCAST_SHORT_ADDR;
           AODVL_cntxt.routingTbl[idx].destAddr = MAC_BROADCAST_SHORT_ADDR;
       }
   }
   
#ifdef DEV_TYPE_FFD
   // rkris@wisense.in - june/8/15
   if (shortAddr == LPWMN_COORD_SHORT_ADDR)
   {
       AODVL_cntxt.coordRtCost = AODVL_MAX_RT_COST_VAL;
       AODVL_cntxt.coordWLC = AODVL_MAX_WLC;
   }
#endif

   return;
}
#endif

#ifdef AODVL_RT_TBL_IN_MRU_ORDER
/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
AODVL_routingTblEntry_s *AODVL_lookUpRtTbl(UINT16_t destAddr)
{
   UINT8_t idx = AODVL_cntxt.rtTblHeadIdx, prevIdx = AODVL_RT_TBL_INV_IDX;

   while (idx != AODVL_RT_TBL_INV_IDX)
   {
        AODVL_routingTblEntry_s *entry_p = &(AODVL_cntxt.routingTbl[idx]);
        if (entry_p->destAddr == destAddr)
        {
            if (prevIdx != AODVL_RT_TBL_INV_IDX)
            {
                AODVL_cntxt.routingTbl[prevIdx].nextIdx =  entry_p->nextIdx;
                entry_p->nextIdx = AODVL_cntxt.rtTblHeadIdx;
                AODVL_cntxt.rtTblHeadIdx = idx;
            }
            return entry_p;
        }
        else
        {
            prevIdx = idx;
            idx = entry_p->nextIdx;
        }
   }
   return NULL;
}


/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
UINT8_t AODVL_checkRt(UINT16_t destAddr)
{
   UINT8_t idx = AODVL_cntxt.rtTblHeadIdx;

   while (idx != AODVL_RT_TBL_INV_IDX)
   {
        AODVL_routingTblEntry_s *entry_p = &(AODVL_cntxt.routingTbl[idx]);
        if (entry_p->destAddr == destAddr)
            return 1;
        else
            idx = entry_p->nextIdx;
   }

   return 0;
}

#else
/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
AODVL_routingTblEntry_s *AODVL_lookUpRtTbl(UINT16_t destAddr)
{
   UINT8_t idx;
   AODVL_routingTblEntry_s *entry_p = AODVL_cntxt.routingTbl;

   for (idx=0; idx<AODVL_ROUTING_TBL_SZ; idx++)
   {
       if (entry_p->destAddr == destAddr)
           break;
       entry_p ++;
   }

   if (idx >= AODVL_ROUTING_TBL_SZ)
       entry_p = NULL;

   return entry_p;
}
#endif


/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
UINT16_t AODVL_getRoute(UINT16_t destAddr)
{
   AODVL_routingTblEntry_s *rtTblEntry_p;

   rtTblEntry_p = AODVL_lookUpRtTbl(destAddr);
   return  rtTblEntry_p == NULL ? MAC_BROADCAST_SHORT_ADDR : rtTblEntry_p->nextHop;
}

/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
void AODVL_purgeRtTbls(UINT16_t shortAddr)
{
   UINT8_t idx;
   
#ifdef GW_ENA
   GW_sendRtPurgeEvent(shortAddr);
#endif

#ifdef AODVL_RT_TBL_IN_MRU_ORDER
   AODVL_clearRoutes(shortAddr);
#else
   for (idx=0; idx<AODVL_ROUTING_TBL_SZ; idx++)
   {
       if (AODVL_cntxt.routingTbl[idx].destAddr != MAC_BROADCAST_SHORT_ADDR)
       {
           if (AODVL_cntxt.routingTbl[idx].destAddr == shortAddr
               || AODVL_cntxt.routingTbl[idx].nextHop == shortAddr)
           {
               // Delete this entry .... 
               AODVL_cntxt.routingTbl[idx].destAddr = MAC_BROADCAST_SHORT_ADDR;
           } 
       }
   }
#endif
   
   for (idx=0; idx<AODVL_RT_DISC_TBL_SZ; idx++)
   {
        if (AODVL_cntxt.rtDiscTbl[idx].destAddr != MAC_BROADCAST_SHORT_ADDR)
        {
            if (AODVL_cntxt.rtDiscTbl[idx].destAddr == shortAddr
                || AODVL_cntxt.rtDiscTbl[idx].origAddr == shortAddr)
            {
                // Delete this entry .... 

                // Send confirmation ....

                AODVL_cntxt.rtDiscTbl[idx].destAddr = MAC_BROADCAST_SHORT_ADDR;
            }
        }
   }

#ifdef DEV_TYPE_FFD
   // rkris@wisense.in - june/8/15
   if (shortAddr == LPWMN_COORD_SHORT_ADDR)
   {
       AODVL_cntxt.coordRtCost = AODVL_MAX_RT_COST_VAL;
       AODVL_cntxt.coordWLC = AODVL_MAX_WLC;
   }
#endif

   return;
}


/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
UINT16_t AODVL_getValidRtTblEntryCnt(void)
{
   UINT16_t idx, validCnt = 0;

   for (idx=0; idx<AODVL_ROUTING_TBL_SZ; idx++)
   {
        if (AODVL_cntxt.routingTbl[idx].destAddr != MAC_BROADCAST_SHORT_ADDR)
        	validCnt ++;
   }

   return validCnt;
}


/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
UINT16_t AODVL_getValidRtDiscTblEntryCnt(void)
{
   UINT16_t idx, validCnt = 0;

   for (idx=0; idx<AODVL_RT_DISC_TBL_SZ; idx++)
   {
        if (AODVL_cntxt.rtDiscTbl[idx].destAddr != MAC_BROADCAST_SHORT_ADDR)
        	validCnt ++;
   }

   return validCnt;
}


/*
 ***************************************************************************
 *
 *
 *
 ***************************************************************************
 */
void AODVL_init(void)
{
   UINT8_t idx;
   
   memset(&AODVL_cntxt, 0, sizeof(AODVL_cntxt));
  
   for (idx=0; idx<AODVL_ROUTING_TBL_SZ; idx++)
   {
        AODVL_cntxt.routingTbl[idx].destAddr = MAC_BROADCAST_SHORT_ADDR;
        AODVL_cntxt.routingTbl[idx].nextIdx = AODVL_RT_TBL_INV_IDX;
   }
   
   for (idx=0; idx<AODVL_RT_DISC_TBL_SZ; idx++)
        AODVL_cntxt.rtDiscTbl[idx].destAddr = MAC_BROADCAST_SHORT_ADDR;
   
   AODVL_cntxt.netTraversalTime = 10;  // in seconds
   
   AODVL_cntxt.timerActive = 0x0; 
   
#ifdef LPWMN_COORD
   AODVL_cntxt.pathDiscTgt = MAC_BROADCAST_SHORT_ADDR;
#endif

#ifdef DEV_TYPE_FFD
   // rkris@wisense.in - june/8/15
   AODVL_cntxt.coordRtCost = AODVL_MAX_RT_COST_VAL;
   AODVL_cntxt.coordWLC = AODVL_MAX_WLC;
#endif

   return;
}

#endif
