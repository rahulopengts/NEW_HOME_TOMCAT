/*
 * File Name : mlme.h
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

#ifndef __MLME_H__
#define __MLME_H__

#include <typedefs.h>


/*
 * Table 44d : MLME-SET-SLOTFRAME.request parameters
 */

#define MLME_SET_SLOT_FRAME_OPN_ADD     0x0
#define MLME_SET_SLOT_FRAME_OPN_DELETE  0x1
#define MLME_SET_SLOT_FRAME_OPN_MODIFY  0x2

typedef struct
{
  /*
   * When a slotframe is created, it is associated with a slotframe handle 
   * for identification.
   */ 
  UINT8_t slotFrameHndl;  // Unique identifier of the slotframe (0x00 - 0xff)

  UINT8_t operation;  // Operation to perform (Add/Modifiy/Delete)

  /*
   * The number of timeslots in a given slotframe (slotframe size) determines 
   * how often each timeslot repeats, thus setting a communication schedule 
   * for nodes that use the timeslots.
   */ 
  UINT16_t size;  // Number of timeslots in the new slotframe (0x0 - 0xffff)
} MLME_setSlotFrameReqParams_s;



/*
 * Table 44f : MLME-SET-LINK.request parameters
 */

#define MLME_SET_LINK_OPN_ADD     0x0
#define MLME_SET_LINK_OPN_DELETE  0x1
#define MLME_SET_LINK_OPN_MODIFY  0x2


#define MLME_LINK_OPTION_TRANSMIT      0x1
#define MLME_LINK_OPTION_RECEIVE       0x2
#define MLME_LINK_OPTION_SHARED        0x4
#define MLME_LINK_OPTION_TIME_KEEPING  0x8


typedef struct
{
   UINT8_t opn;
   UINT8_t slotFrameHndl;
   UINT8_t channelOffset;
   UINT8_t linkOptions;
   UINT8_t linkType;
  
   // Address of the neighor device connected by the link.
   // 0xffff indicates the link may be used for frames 
   // destined for the broadcast address 
   UINT16_t nodeAddr;

   UINT16_t linkHndl;
   
   // Timeslot of the link to be added, as described in 5.1.1.5.
   UINT16_t timeSlot;  // offset within a slotframe
} MLME_setLinkReqParams_s;


typedef struct
{
   // Indicates whether to send a beacon (0x0) or enhanced
   // beacon (0x1)
   UINT8_t beaconType;

   UINT8_t channel;  
   
   UINT8_t channelPage;  

} MLME_beaconReqParams_s;




#endif
