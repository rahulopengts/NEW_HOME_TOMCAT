/*
 * File Name : mac_pib.h
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

#ifndef __MAC_PIB_H__
#define __MAC_PIB_H__

#include <typedefs.h>
#include <mac.h>

#define MAC_SLOT_FRAME_TABLE_SIZE  1

#define MAC_LINK_TABLE_SIZE  1

#define MAC_HOPPING_SEQUENCE_TABLE_SIZE  1

#define MAC_nodeHasJoined()  (MAC_PIB.macShortAddress != MAC_BROADCAST_SHORT_ADDR)

/*
 * Table 52c : TSCH-MAC PIB attributes for macSlotframeTable
 */
typedef struct
{
  // Identifier of the slotframe
  // Integer  (0x00 - 0xff)
  UINT8_t macSlotFrameHandle;

  // Number of timeslots in the slotframe
  // Integer (0x0000 - 0xffff)
  UINT16_t macSlotFrameSize;  // If 0, entry if free
} MAC_slotFrameTableEntry_s;


/*
 * Table 52d : TSCH-MAC PIB attributes for macLinkTable
 */

// Flags indicating how the link is used.
#define MAC_LINK_OPTION_TRANSMIT      0x1
#define MAC_LINK_OPTION_RECEIVE       0x2
#define MAC_LINK_OPTION_SHARED        0x3
#define MAC_LINK_OPTION_TIME_KEEPING  0x4


/*
 * macMaxFrameTotalWaitTime =
 *  (
 *    (Sum{k in (0 to m-1)}[(2^(macMinBE + k)) - 1]
 *    + ((((2^macMaxBE) - 1) x (macMaxCSMABackoffs - m))
 *  ) x aUnitBackoffPeriod
 *  + phyMaxFrameDuration
 *
 * rkis - macMaxFrameTotalWaitTime is calculated assuming that the coordinator's transmission
 * succeeds only in the last allowed attempt and in each previous attempt the back off time is
 * the worst case time ((2^be) - 1) x aUnitBackoffPeriod symbols. That is rand(0, N-1) returns
 * N-1.
 * In addition, the assumption is that all nodes in the network use the same values for
 * macMaxCSMABamckoffs, macMinBE and macMaxBE.
 *
 * m = min(macMaxBE-macMinBE, macMaxCSMABackoffs)
 * where macMaxCSMABackoffs default value is 4
 * and macMaxBE default value is 5
 * and macMinBE default value is 3
 * Therefore m = min(5-3, 4) => m(2, 4) => 2
 *
 * For PHYs other than CSS and UWB, the attribute phyMaxFrameDuration is given by:
 * phyMaxFrameDuration = phySHRDuration + ceiling([aMaxPHYPacketSize + 1] x phySymbolsPerOctet)
 * where ceiling() is a function that returns the smallest integer value greater
 * than or equal to its argument value.
 *
 * aMaxPHYPacketSize is 127  (Table 70 — PHY constants)
 * phySymbolsPerOctet is 2 for O-QPSK PHY
 * The PHY PIB attribute phySHRDuration is given by:
 *    phySHRDuration = Npreamblesymbols + Nsfd
 *
 * For the O-QPSK PHY, Npreamblesymbols is 8 symbols and Nsfd is 2 symbols
 *
 * phyMaxFrameDuration = 8 + 2 + ceiling(128 x 2) => 10 + 256 => 266
 *
 * aUnitBackoffPeriod is 20 (Table 51 — MAC sublayer constants)
 *
 * macMaxFrameTotalWaitTime =
 * => (
 *       ((2^3) - 1) + ((2^4) - 1)
 *       + ((2^5) - 1) x (4 - 2)
 *    ) x 20
 *    + 266
 *
 * => (7 + 15 + (31) x 2) x 20 + 266  => (84 x 20) + 266 => 1946 symbols => 1946 x 16 microsec ~ 32 millisecs
 */
#define MAC_MAX_FRAME_TOTAL_WAIT_TIME   1946   // Symbols


typedef struct
{
  // Integer, 0x00 - 0xff, Identifier of the slotframe
  UINT8_t macSlotFrameHandle;

  // Bitmap, 0x0 - 0xf
  UINT8_t macLinkOptions;

  // Enumeration, NORMAL/ADVERTISING, type of link
  UINT8_t macLinkType;

  // Integer, 0x0000 - 0xffff, Identifier of link 
  UINT16_t macLinkHandle;

  // IEEE address, 16 bits, address of the node connected to this link
  UINT16_t macNodeAddress;
  
  // Integer , 0x0000 - 0xffff, Timeslot for this link 
  UINT16_t macTimeSlot;
 
  // Integer , 0x0000 - 0xffff, Channel offset for this link 
  UINT16_t macChannelOffset;
} MAC_linkTableEntry_s;


/*
 * Table 52f - MAC PIB attributes for Hopping Sequence
 */
typedef struct
{
  // Integer, 0x0 - 0xf
  // Each hopping sequence has a unique ID
  UINT8_t macHoppingSequenceID;

  // Integer, 0x0 - 0x1ff
  // Number of channels supported by the PHY on this channel 
  // page
  UINT16_t macNumberOfChannels; 
  
  // Integer, 0x0000 - 0xffff
  // The number of channels in the Hopping Sequence. Does not 
  // necessarily equal numberOfChannels.
  UINT16_t macHoppingSequenceLength;

  // Integer, 0x0000 - 0xffff
  // Index of the current position in the hopping sequence 
  // list.
  UINT16_t macCurrentHop;
} MAC_hoppingSequenceTableEntry_s;


#if 0
typedef struct
{
  UINT64_t u64Val;
} ASN_s;
#endif

typedef struct
{
#ifdef IEEE_802_15_4e
  /*
   * The 8-bit address that the device uses to communicate in the LPWMN.
   * If the device is the LPWMN coordinator, this value shall be chosen
   * before a LPWMN is started. Otherwise, the address is allocated by a
   * coordinator during association. A value of 0xfe indicates that the 
   * device has associated but has not been allocated an address. A val
   * of 0xff indicates that the device does not have a simple address.
   */ 
  UINT8_t macSimpleAddress;  // 802.15.4e
#endif

  /*
   * The sequence number added to the transmitted data or MAC command
   * frame. Default value is a random value from within the range
   */
#if (MAC_PDU_HDR_SEQ_NR_FIELD_LEN == 4)
  UINT32_t macDSN;
#elif (MAC_PDU_HDR_SEQ_NR_FIELD_LEN == 2)
  UINT16_t macDSN;
#else
  UINT8_t macDSN;
#endif
  
  /*
   * The sequence number added to the transmitted beacon frame.
   * Range is 0 to 0xff. Initial value should be a random value
   * from within the range.
   */
  UINT8_t macBSN;
  
  /*
   * The maximum value of the backoff exponent, BE, in the CSMA-CA algorithm,
   * as defined in 5.1.1.4.
   * Range is 3 to 8. Default value is 5.
   */ 
  UINT8_t macMaxBE:4;

  /*
   * The minimum value of the backoff exponent (BE) in the CSMA-CA algorithm,
   * as described in 5.1.1.4.
   * Range is 0 to macMaxBE. Default value is 3.
   */ 
  UINT8_t macMinBE:4;
  
  /*
   * The maximum number of backoffs the CSMA-CA algorithm will attempt
   * before declaring a channel access failure.
   * Range is 0 to 5. Default value is 4.
   */
  UINT8_t macMaxCSMABackoffs;
  
  /*
   * The maximum number of retries allowed after a transmission failure.
   * Range is 0 to 7. Default value is 3.
   */
  UINT8_t macMaxFrameRetries;
  
  /*
   * Indication of whether the MAC sublayer is to enable its receiver during 
   * idle periods. For a beaconenabled LPWMN, this attribute is relevant only
   * during the CAP of the incoming superframe. For a nonbeacon-enabled LPWMN,
   * this attribute is relevant at all times. 
   * Allowed values :  TRUE/FALSE. Default is FALSE.
   */
  UINT8_t macRxOnWhenIdle:1;
  
  /*
   * Indication of whether a coordinator is currently allowing association. A
   * value of TRUE indicates that association is permitted.
   * Allowed values :  TRUE/FALSE. Default is FALSE.
   */
  UINT8_t macAssociationPermit:1;
  
  /*
   * Indication of whether a device automatically sends a data request command if
   * its address is listed in the beacon frame. A value of TRUE indicates that the
   * data request command is automatically sent. This attribute also affects the
   * generation of the MLME-BEACON-NOTIFY.indication. Default is TRUE.
   */
  UINT8_t macAutoRequest:1;
  
  UINT8_t rsvd:5;
  
  /*
   * LPWMNs that do not wish to use the superframe structure (referred to as non
   * beacon-enabled LPWMNs) shall set both macBeaconOrder and macSuperframeOrder to
   * 15. In this case, a coordinator shall not transmit beacons, except upon 
   * receipt of a beacon request command; all transmissions, with the exception of
   * ack frames and any data frame that quickly follows the ack of a data request
   * command, as described in 5.1.6.3, shall use an unslotted CSMA-CA mechanism 
   * to access the channel. In addition, GTSs shall not be permitted.
   */
   
  /*
   * Indicates the frequency with which the beacon is transmitted, as defined in
   * 5.1.1.1. Range is 0 to 15. Default value is 15.
   */
  UINT8_t macBeaconOrder:4;
  
  /*
   * The length of the active portion of the outgoing superframe, including the
   * beacon frame, as defined in 5.1.1.1. Range is 0 to 15. Default value is 15.
   */
  UINT8_t macSuperframeOrder:4;
  
  /*
   * The maximum time, in multiples of aBaseSuperframeDuration, a device shall wait 
   * for a response command frame to be available following a request command frame.
   * Range is 2 to 64, default value is 32.
   */
  UINT8_t macResponseWaitTime;
  
  /*
   * The address that the device uses to communicate in the LPWMN. If the
   * device is the LPWMN coordinator, this value shall be chosen before a
   * LPWMN is started.  Otherwise, the short address is allocated by a
   * coordinator during association.  A value of 0xfffe indicates that 
   * the device has associated but has not been allocated an address. A 
   * value of 0xffff indicates that the device does not have a short 
   * address. Default value is 0xffff.
   */ 
  MAC_shortAddr_t macShortAddress;
  
#ifdef DEV_TYPE_FFD
  MAC_shortAddr_t prevAllocShortAddr;
#endif

  /*
   * The identifier of the LPWMN on which the device is operating. If this
   * value is 0xffff, the device is not associated.
   */
  MAC_nwkId_t macLPWMNId;

  /*
   * The extended address assigned to the device.
   */ 
  UINT8_t macExtendedAddress[MAC_EXT_ADDR_LEN];

  /*
   * The short address assigned to the coordinator through which the device is
   * associated. A value of 0xfffe indicates that the coordinator is only using 
   * its extended address. A value of 0xffff indicates that this value is unknown.
   */
  MAC_shortAddr_t macCoordShortAddress;
     
  /*
   * The address of the coordinator through which the device is associated.
   */
  UINT8_t macCoordExtendedAddress[MAC_EXT_ADDR_LEN];
  
#ifdef IEEE_802_15_4e  
  /*
   * Boolean (TRUE/FALSE)
   * If TRUE, the device is using functionality specific to TSCH.
   */
  UINT8_t macTSCHenabled;
#endif  
  
#if 0
  // These attributes are fixed for given PHY
  /*
   * The minimum time forming a SIFS period.
   * 
   * Section 8.1.3
   * For all PHYs other than the UWB PHY, the minimum SIFS period is 12 symbols.
   */
  UINT8_t macSIFSPeriod;
 
  /*
   * The minimum time forming a LIFS period.
   * 
   * Section 8.1.3
   * For all PHYs other than the UWB PHY, the minimum LIFS period is 40 symbols.
   */  
  UINT8_t macLIFSPeriod;

  /*
   * The maximum number of symbols to wait for an acknowledgment frame to
   * arrive following a transmitted data frame. This value is dependent on
   * the supported PHY, which determines both the selected channel and channel 
   * page. The calculated value is the time to commence transmitting the ACK 
   * plus the length of the ACK frame. The commencement time is described in 
   * 5.1.6.4.2.
   *
   * The read-only attribute macAckWaitDuration is dependent on a combination of 
   * constants and PHY PIB attributes. The PHY PIB attributes are listed in Table 
   * 71. The formula for relating the constants and attributes is:
   * 
   * macAckWaitDuration = aUnitBackoffPeriod   (20 * 16 us) 
   *                      + aTurnaroundTime   (12 * 16 us)
   *                      + phySHRDuration    (4+1)*2*16 => 160 us
   *                      + ceiling (6 x phySymbolsPerOctet)   // 6*2*16 => 192 us
   * macAckWaitDuration = 864 microseconds
   *
   * where the number 6 comes from the number of PHY header octets plus the number 
   * of PSDU octets in an ack frame and ceiling() is a function that returns the 
   * smallest integer value greater than or equal to its argument value.
   */
  UINT16_t macAckWaitDurationMicroSecs;  // Storing in microsecs to avoid run time
                                         // computation
#endif
  
#ifdef IEEE_802_15_4e  
  MAC_slotFrameTableEntry_s  macSlotFrameTable[MAC_SLOT_FRAME_TABLE_SIZE];

  MAC_linkTableEntry_s  macLinkTable[MAC_LINK_TABLE_SIZE];  

  MAC_hoppingSequenceTableEntry_s  macHoppingSequenceTable[MAC_HOPPING_SEQUENCE_TABLE_SIZE];

  /*
   * --------------------------------------------------------------------
   * Table 52b : TSCH-specific MAC PIB attributes
   * --------------------------------------------------------------------
   * Attribute : macASN  
   * Type : Integer
   * Range : 0x0 - 0xffffffffff (40 bits)
   * Description : The Absolute Slot Number, i.e., the number of slots 
   *               that has elapsed since the start of the network.
   *               Used for nonce construction when security is enabled.
   * Default : 0x0
   * --------------------------------------------------------------------
   */
  ASN_s  macASN;
#endif
} MAC_PIB_s;

extern MAC_PIB_s MAC_PIB;

#define MAC_nodeJoined() \
	    (MAC_PIB.macShortAddress != MAC_BROADCAST_SHORT_ADDR)
  
extern void MAC_PIB_init(void);

#endif
