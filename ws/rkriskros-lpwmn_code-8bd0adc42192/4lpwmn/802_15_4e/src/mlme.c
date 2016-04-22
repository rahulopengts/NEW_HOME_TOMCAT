/*
 * File Name: mlme.c
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

#include <stdio.h>
#include <string.h>
#include <typedefs.h>
#include <pltfrm.h>
#include <mlme.h>
#include <system.h>
#include <mac_defs.h>
#include <mac.h>
#include <mac_pib.h>
#include <phy_defs.h>

#ifndef DEV_TYPE_SNIFFER


#ifdef IEEE_802_15_4e
/*
 *******************************************************************
 *
 *
 *
 *
 *******************************************************************
 */
MAC_sts_t MLME_keepAliveReqHndlr(MLME_keepAliveReqParams_s *params_p)
{
   MAC_sts_t macSts = MAC_STS_SUCCESS;


   return macSts;
}


/*
 *******************************************************************
 *
 *
 *
 *
 *
 *******************************************************************
 */
MAC_sts_t MLME_tschModeReqHndlr(UINT8_t mode)
{
   MAC_sts_t macSts = MAC_STS_SUCCESS;

   if (mode != MLME_TSCH_MODE_ON
       && mode != MLME_TSCH_MODE_OFF)
   {
       return MAC_STS_INVALID_PARAMS;
   }

   if (mode == MLME_TSCH_MODE_ON)
   {
       if (MAC_PIB.macTSCHenabled)
           return MAC_STS_INVALID_PARAMS;

   /*
    * The MLME-TSCH-MODE.request may be generated by a higher layer
    * after the device has received advertisements from the network 
    * and is synchronized to a network, i.e., in response to an MLME 
    * BEACON NOTIFY.indication.
        *
    * Upon receipt of the request with TSCHMode set to ON, the MAC 
    * shall start operating its TSCH state machine using slotframes 
    * and links already contained in its macSlotframeTable and 
    * macLinkTable MAC PIB attributes. To successfully complete this 
    * request the device shall already be synchronized to a network.
        */ 
   }
   else
   {
       if (MAC_PIB.macTSCHenabled != 0x0)
           return MAC_STS_INVALID_PARAMS;

       /*
    * The MAC shall stop using slotframes and links upon receipt of 
    * the request with TSCHMode set to off.
    */
   }

   return macSts;
}


/*
 *******************************************************************
 *
 *
 *
 *
 *
 *******************************************************************
 */
MAC_sts_t MLME_setSlotFrameReqHndlr(MLME_setSlotFrameReqParams_s *params_p)
{
   MAC_sts_t macSts = MAC_STS_SUCCESS;

   /*
    * The MLME-SET-SLOTFRAME.request primitive is used to add,  
    * delete, or modify a slotframe at the MAC sublayer. The 
    * slotframeHandle is supplied by a higher layer.
    */ 

   /*
    * On receipt of an MLME-SET-SLOTFRAME.request, the MLME 
    * shall verify the parameters passed with the primitive.
    */

   switch (params_p->opn)
   {
       case MLME_SET_SLOT_FRAME_OPN_ADD:
            {
               macSts = MAC_addSlotFrameTblEntry(slotFrameHndl, params_p->size);
            }
            break;

       case MLME_SET_SLOT_FRAME_OPN_DELETE:
            {
               macSts = MAC_delSlotFrameTblEntry(slotFrameHndl);
            }
            break;

       case MLME_SET_SLOT_FRAME_OPN_MODIFY:
            {
            }
            break;

       default:
            {
                macSts = MAC_STS_INVALID_PARAMS;
            }
            break;
   }

   return macSts;
}


/*
 *******************************************************************
 *
 *
 *
 *
 *
 *******************************************************************
 */
MAC_sts_t MLME_setLinkReqHndlr(MLME_setLinkReqParams_s *params_p)
{
   MAC_sts_t macSts = MAC_STS_SUCCESS;

   /*
    * The MLME-SET-LINK.request primitive requests to add a new link, 
    * or delete or modify an existing link at the MAC sublayer. The 
    * slotframeHandle and linkHandle are supplied by a higher layer.
    */

   switch (params_p->opn)
   {
       case MLME_SET_LINK_OPN_ADD:
            {
            }
            break;

       case MLME_SET_LINK_OPN_DELETE:
            {
            }
            break;

       case MLME_SET_LINK_OPN_MODIFY:
            {
            }
            break;

       default:         
            {
                macSts = MAC_STS_INVALID_PARAMS;
            }
            break;
   }

   return macSts;
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
void MLME_scanRequest(MLME_scanRequestParams_s *params_p)
{  
   UINT8_t sendCnfrm = 0, scanType = MAC_SCAN_TYPE_ACTIVE;
   MAC_sts_t sts = MAC_STS_SUCCESS;
   UINT16_t channMsk;

#ifdef DEV_TYPE_RFD
   // PLTFRM_intDisable();
   // CC1101_init(0);
   // PLTFRM_intEnable();
#endif

#ifdef RADIO_FREQ_BAND_865_867_MHZ
   channMsk = PHY_865_867_MHZ_BAND_CHANNEL_MSK;
#elif defined(RADIO_FREQ_BAND_2400_MHZ)
   channMsk = PHY_2450_MHZ_BAND_CHANNEL_MSK;
#else
#error radio frequency band not specified !!
#endif
   
#ifdef MAC_STATS_ENA
   MAC_cntxt.stats.scanReqCnt ++;
#endif       
   /*
    * The MLME-SCAN.request primitive is used to initiate a channel scan over a 
    * given list of channels. When the MLME receives this primitive, it begins 
    * the appropriate scan procedure, as defined in 5.1.2.
    */
   
   if (MAC_cntxt.scanInProgress)
   {
       sts = MAC_STS_SCAN_IN_PROGRESS; 
       sendCnfrm = 1;
       goto _end;          
   }
   
   if ((params_p->scanChannelsMsk == 0x0)
       || (params_p->scanChannelsMsk & (~channMsk))
       || (params_p->scanDurationSecs > MLME_SCAN_REQ_MAX_SCAN_DURATION))
   {
       sts = MAC_STS_INVALID_PARAMETER;
       sendCnfrm = 1;
       goto _end;     
   }  
      
   switch (scanType)
   {   
       case MAC_SCAN_TYPE_ACTIVE: 
            {
               /*
                * An active scan allows a device to locate any coordinator 
                * transmitting beacon frames within its radio communications 
                * range. An active scan uses the beacon request command to 
                * extract the beacon from a coordinator.
                *
                * Before commencing an active scan, the MAC sublayer shall store 
                * the value of macLPWMNId and then set it to 0xffff for the duration
                * of the scan. This enables the receive filter to accept all beacons 
                * rather than just the beacons from its current LPWMN, as described
                * in 5.1.6.2. On completion of the scan, the MAC sublayer shall 
                * restore the value of macLPWMNId to the value stored before the scan
                * began.
                */
               // MAC_cntxt.prevLPWMNId = MAC_PIB.macLPWMNId;
               MAC_cntxt.scanChannelsMsk = params_p->scanChannelsMsk;     
               MAC_cntxt.scanChan = PHY_865_867_MHZ_BAND_CHAN_INV;
               MAC_cntxt.scanType = MAC_SCAN_TYPE_ACTIVE;
               MAC_cntxt.unScannedChanBitMsk = 0x0;
               MAC_cntxt.scanInProgress = 0x0;
               MAC_cntxt.scanDurationSecs = params_p->scanDurationSecs;
               MAC_cntxt.uniqueBcnRcvdCnt = 0;
               memset(MAC_cntxt.lpwmnDescriptorList, 0, sizeof(MAC_cntxt.lpwmnDescriptorList));
               MAC_doActiveScan();
            }
            break;
            
       case MAC_SCAN_TYPE_PASSIVE:    
       case MAC_SCAN_TYPE_ORPHAN:  
       case MAC_SCAN_TYPE_ED:
       default:
            {
           
            }
            break;            
   }
   
_end:
   if (sendCnfrm)
   {        
       MAC_cntxt.scanCnfrmParams.status = sts;
       MAC_cntxt.scanCnfrmParams.resultListSize = 0x0;
       MAC_cntxt.scanCnfrmParams.lpwmnDescriptorList_p = NULL;
 
       MAC_cleanUpScanProc( );     
       
       SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_MAC_SCAN_CONFIRM;
   }   
   return;
}
#endif
#endif