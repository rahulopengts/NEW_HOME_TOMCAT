/*
 * File Name : mac_defs.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : 8/5/2013
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

#ifndef __MAC_DEFS_H__
#define __MAC_DEFS_H__

#include <typedefs.h>
#include <phy_defs.h>


// Dec/9/14
#define MAC_INVALID_DSN  0x0   

#define MAC_DEFAULT_LPWMN_ID  0x7654

#define MAC_EXT_ADDR_LEN  8
#define MAC_SIMPLE_ADDR_LEN  1
#define MAC_FCS_LEN  2
#define MAC_PDU_HDR_FC_FIELD_LEN  2 

#define LPWMN_COORD_SHORT_ADDR  0x1
#define MAC_MAX_UNICAST_SHORT_ADDR  0x7fff
#define MAC_MIN_MULTICAST_SHORT_ADDR  0x8000
#define MAC_MAX_MULTICAST_SHORT_ADDR  0xfffe
#define MAC_BROADCAST_SHORT_ADDR  0xffff

#define MAC_MAX_HDR_PLUS_PYLD_LEN  RADIO_MAX_MAC_PDU_LEN
// #define MAC_MAX_HDR_PLUS_PYLD_LEN  (PHY_aMaxPHYPacketSize - MAC_FCS_LEN)

#define MAC_DATA_PKT_LPWMN_ID_COMP_FLAG  0x1

#define LPWMN_COORD_MAC_SHORT_ADDR  0x1

#define MAC_BCN_PYLD_LEN  4

#if defined(RADIO_CC1200) || defined(RADIO_CC1101)
#define MAC_PDU_HDR_SEQ_NR_FIELD_LEN  2
#else
#define MAC_PDU_HDR_SEQ_NR_FIELD_LEN  1
#endif

#if (MAC_PDU_HDR_SEQ_NR_FIELD_LEN == 4) 
#define MAC_MAX_DSN  ((UINT32_t)0xffffffff)
#elif (MAC_PDU_HDR_SEQ_NR_FIELD_LEN == 2) 
#define MAC_MAX_DSN  ((UINT16_t)0xffff)
#else
#define MAC_MAX_DSN  ((UINT8_t)0xff)
#endif

#define MAC_PDU_HDR_FC_FIELD_OFF   0x0
#define MAC_PDU_HDR_DSN_FIELD_OFF  (MAC_PDU_HDR_FC_FIELD_OFF \
                                    + MAC_PDU_HDR_FC_FIELD_LEN)
#define MAC_PDU_HDR_DEST_LPWMN_ID_FIELD_OFF  (MAC_PDU_HDR_DSN_FIELD_OFF \
                                              + MAC_PDU_HDR_SEQ_NR_FIELD_LEN)
#define MAC_PDU_HDR_DEST_ADDR_FIELD_OFF  (MAC_PDU_HDR_DEST_LPWMN_ID_FIELD_OFF \
                                          + MAC_LPWMN_ID_LEN)

#define MAC_ADDRESS_TYPE_UNKNOWN   0x0
#define MAC_ADDRESS_TYPE_SHORT     0x1
#define MAC_ADDRESS_TYPE_EXTENDED  0x2

#define MAC_LPWMN_DESCRIPTOR_LIST_MAX_SIZE  4

#define MAC_SHORT_ADDR_MSK  0xffff

#define MAC_INVALID_SHORT_ADDR  0x0 

#define MAC_RCVD_DSN_INFO_LIST_SZ  4

#define MAC_COORD_POLL_TIMEOUT_SECS  5


// Number of transmit failures before RFD will try to re-associate
// and hopefully select another parent
#define MAC_RFD_REASSOC_THRESHOLD   5

// Max number of RFDs which can associate with an FFD which is not the LPWMN coordinator.
#define MAC_MAX_ASSOC_RFD_CNT   16

#define MAC_MIN_LIFS_PERIOD_USECS (40 * PHY_SYMBOL_LENGTH_USECS)
#define MAC_MIN_SIFS_PERIOD_USECS (12 * PHY_SYMBOL_LENGTH_USECS)   

#define MAC_ACK_WAIT_DURAITION_SYMBOLS  (MAC_aUnitBackoffPeriod  \
                                         + PHY_aTurnaroundTime   \
                                         + PHY_SRH_DURATION      \
                                         + (PHY_PDU_PYLD_LEN_FIELD_LEN * PHY_SYMBOLS_PER_OCTET)  \
                                         + (MAC_ACK_PDU_LEN * PHY_SYMBOLS_PER_OCTET))

#define MAC_ACK_WAIT_DURAITION_MICROSECS  (UINT16_t)((MAC_ACK_WAIT_DURAITION_SYMBOLS)*(PHY_SYMBOL_LENGTH_USECS))
   
#define MAC_IFS_TYPE_NONE   0x0
#define MAC_IFS_TYPE_SHORT  0x1
#define MAC_IFS_TYPE_LONG   0x2


#define MAC_CSMA_STATE_IDLE      0x0
#define MAC_CSMA_STATE_BACK_OFF  0x1
#define MAC_CSMA_STATE_ERROR     0xa


#define MAC_PENDING_FRAMES_IDX_TBL_SZ  3
#ifdef __MSP430F2274__
#define MAC_PENDING_FRAMES_BUFF_LEN  2
#else
#define MAC_PENDING_FRAMES_BUFF_LEN  32
#endif

#define MAC_INDIRECT_TX_TRIGGER_MCPS    0x0
#define MAC_INDIRECT_TX_TRIGGER_MLME    0x1


/*
 * Table 6 — Valid values of the Association Status field
 */
#define MAC_ASSOCIATION_STS_SUCCESS            0x0
#define MAC_ASSOCIATION_STS_LPWMN_AT_CAPACITY    0x1
#define MAC_ASSOCIATION_STS_LPWMN_ACCESS_DENIED  0x2

/*
 * 5.3.1.2 Capability Information field
 */

/*
 * The Device Type field shall be set to one if the device is an FFD. Otherwise, 
 * the Device Type field shall be set to zero to indicate an RFD.
 */
#define MAC_ASSOC_REQ_CAP_INO_DEV_TYPE_BIT_NR       1

/*
 * The Power Source field shall be set to one if the device is receiving power 
 * from the alternating current mains. Otherwise, the Power Source field shall be 
 * set to zero.
 */
#define MAC_ASSOC_REQ_CAP_INO_POWER_SOURCE_BIT_NR   2
   
/*
 * The Receiver On When Idle field shall be set to one if the device does not 
 * disable its receiver to conserve power during idle periods. Otherwise, the 
 * Receiver On When Idle field shall be set to zero.
 */
#define MAC_ASSOC_REQ_CAP_INO_RX_ON_IDLE_BIT_NR     3

/*
 * The Security Capability field shall be set to one if the device is capable of 
 * sending and receiving cryptographically protected MAC frames as specified in 
 * 7.2; it shall be set to zero otherwise.
 */
#define MAC_ASSOC_REQ_CAP_INO_SEC_CAP_BIT_NR        6

/*
 * The Allocate Address field shall be set to one if the device wishes the 
 * coordinator to allocate a short address as a result of the association procedure. 
 * Otherwise, it shall be set to zero.
 */
#define MAC_ASSOC_REQ_CAP_INO_ALLOC_ADDR_BIT_NR     7

 
#define MAC_ASSOC_REQ_CAP_INFO_DEV_TYPE_BIT_SHIFT_MSK      (1 << 1)
#define MAC_ASSOC_REQ_CAP_INFO_POWER_SOURCE_BIT_SHIFT_MSK  (1 << 2)
#define MAC_ASSOC_REQ_CAP_INFO_RX_ON_IDLE_BIT_SHIFT_MSK    (1 << 3)
#define MAC_ASSOC_REQ_CAP_INFO_SEC_CAP_BIT_SHIFT_MSK       (1 << 6)
#define MAC_ASSOC_REQ_CAP_INFO_ALLOC_ADDR_BIT_SHIFT_MSK    (1 << 7)


// Table 30 — MLME-SCAN.request parameters
#define MAC_SCAN_TYPE_ED       0x0
#define MAC_SCAN_TYPE_ACTIVE   0x1
#define MAC_SCAN_TYPE_PASSIVE  0x2
#define MAC_SCAN_TYPE_ORPHAN   0x3

#define MAC_PDU_MAX_LEN  128

#define MAC_ACK_PDU_LEN  (MAC_PDU_HDR_FC_FIELD_LEN \
                          + MAC_PDU_HDR_SEQ_NR_FIELD_LEN \
                          + MAC_FCS_LEN)
   
#define MAC_PDU_HDR_FC_FIELD_MAX_LEN   2            // 1 OR 2
#define MAC_PDU_HDR_AUX_SEC_HDR_MAX_LEN   14


#define MAC_ACK_PDU_RSSI_FIELD_LEN  1
#define MAC_ACK_PDU_LQI_FIELD_LEN   1


#define MAC_SIM_802_15_4_ACK_PDU_LEN  (MAC_PDU_HDR_FC_FIELD_LEN \
                                       + MAC_PDU_HDR_SEQ_NR_FIELD_LEN \
                                       + MAC_LPWMN_ID_LEN \
                                       + MAC_SHORT_ADDR_LEN \
                                       + MAC_SHORT_ADDR_LEN \
                                       + MAC_ACK_PDU_RSSI_FIELD_LEN \
                                       + MAC_ACK_PDU_LQI_FIELD_LEN)


#define MAC_BROADCAST_LPWMN_ID_MSB   0xff
#define MAC_BROADCAST_LPWMN_ID_LSB   0xff   
#define MAC_BROADCAST_LPWMN_ID   0xffff



// Table 30 MLME-SCAN.request parameters
// Standard specified range is 0 to 14 but this implementation
// supports the range 0 to 6. Value 6 corresponds to (60 * 16 * 16 * (2^6 + 1))
// => 998400 microsecs => .998400 seconds
#define MLME_SCAN_REQ_MAX_SCAN_DURATION  6

   
#define MAC_PDU_MAX_HDR_LEN   ( \
        MAC_PDU_HDR_FC_FIELD_MAX_LEN + \
        MAC_PDU_HDR_SEQ_NR_FIELD_LEN + \
        MAC_LPWMN_ID_LEN + \
        MAC_EXT_ADDR_LEN + \
        MAC_LPWMN_ID_LEN + \
        MAC_EXT_ADDR_LEN + \
        MAC_PDU_HDR_AUX_SEC_HDR_MAX_LEN )


#define MAC_CMD_PDU_MAX_LEN  16

#define MAC_PDU_HDR_DEST_ADDR_OFF  (MAC_PDU_HDR_FC_FIELD_MAX_LEN + \
                                    MAC_PDU_HDR_SEQ_NR_FIELD_LEN + \
                                    MAC_LPWMN_ID_LEN)

/*
 * Type of link (normal or advertising).
 * An advertising link may be used to send an enhanced beacon.
 */
#define MAC_LINK_TYPE_NORMAL       0x0
#define MAC_LINK_TYPE_ADVERTISING  0x1

/*
 * Table 3 - Possible values of the Destination Addressing
 * Mode and Source Addressing Mode fields.
 */
#define MAC_ADDRESS_MODE_NO_ADDR        0x0
#define MAC_ADDRESS_MODE_RSVD           0x1
#define MAC_ADDRESS_MODE_SHORT_ADDR     0x2
#define MAC_ADDRESS_MODE_EXTENDED_ADDR  0x3

/*
 * Table 2 - Values of the Frame Type Field
 */
#define MAC_FRAME_TYPE_BEACON        0x0
#define MAC_FRAME_TYPE_DATA          0x1
#define MAC_FRAME_TYPE_ACK           0x2
#define MAC_FRAME_TYPE_CMD           0x3
#define MAC_FRAME_TYPE_LLDN          0x4   // 802.15.4e
#define MAC_FRAME_TYPE_MULTIPURPOSE  0x5   // 802.15.4e

/*
 * Table 5 - MAC command frames
 */
#define MAC_CMD_ASSOCIATION_REQ          0x1
#define MAC_CMD_ASSOCIATION_RESP         0x2
#define MAC_CMD_DISASSOCIATION_NOTIF     0x3
#define MAC_CMD_DATA_REQUEST             0x4
#define MAC_CMD_LPWMN_ID_CONFLICT_NOTIF  0x5
#define MAC_CMD_ORPHAN_NOTIF             0x6
#define MAC_CMD_BEACON_REQUEST           0x7
#define MAC_CMD_COORD_REALIGNMENT        0x8
#define MAC_CMD_GTS_REQUEST              0x9
#define MAC_CMD_NWK_RESTART              0xa

#define MAC_CMD_ID_FIELD_LEN   1

#define MAC_NODE_CAPABILITY_INFO_LEN  0x1

   
// Figure 49 - Association request command format
#define MAC_ASSOC_REQ_CMD_PDU_PYLD_LEN    (MAC_CMD_ID_FIELD_LEN \
                                           + MAC_NODE_CAPABILITY_INFO_LEN \
                                           + SYSTEM_FATAL_ERROR_ID_LEN)
   
// Figure 51 - Association response command format
#define MAC_ASSOC_RESP_CMD_PDU_PYLD_LEN   (MAC_CMD_ID_FIELD_LEN \
                                           + MAC_SHORT_ADDR_LEN + 1)

// Figure 56 - Beacon request command format
// rkris@wisense.in / may/4/15, add source extended address to beacon req payload
#define MAC_BEACON_REQ_CMD_PDU_PYLD_LEN   (MAC_CMD_ID_FIELD_LEN \
                                           + MAC_EXT_ADDR_LEN)

// Figure 53 - Data request command format
#define MAC_DATA_REQ_CMD_PDU_PYLD_LEN  MAC_CMD_ID_FIELD_LEN

// Network restart command (1 byte) 
#define MAC_NWK_RESTART_CMD_PDU_PYLD_LEN  MAC_CMD_ID_FIELD_LEN
   


#define MAC_ASSOC_RESP_STS_LEN  0x1

   
/*
 * Table 6 - Valid values of the association status field.
 */   
#define MAC_ASSOC_STS_SUCCESS            0x0
#define MAC_ASSOC_STS_LPWMN_AT_CAPACITY    0x1
#define MAC_ASSOC_STS_LPWMN_ACCESS_DENIED  0x2   

/*
 * The frames in the MAC sublayer are described as a sequence of 
 * fields in a specific order. All frame formats in this subclause 
 * are depicted in the order in which they are transmitted by the 
 * PHY, from left to right, where the leftmost bit is transmitted 
 * first in time. Bits within each field are numbered from 0 
 * (leftmost and least significant) to k - 1 (rightmost and most 
 * significant), where the length of the field is k bits. Fields 
 * that are longer than a single octet are sent to the PHY in the 
 * order from the octet containing the lowest numbered bits to the 
 * octet containing the highest numbered bits.
 */ 

/*
 * Figure 36 : Format of the Frame Control field
 * LSB
 */
#define MAC_FC_FRAME_TYPE_BITS_SHIFT   0x0
#define MAC_FC_SEC_ENA_BIT_SHIFT       0x3
#define MAC_FC_PENDING_BIT_SHIFT       0x4
#define MAC_FC_ACK_REQ_BIT_SHIFT            0x5
#define MAC_FC_LPWMN_ID_COMP_BIT_SHIFT      0x6
#define MAC_FC_DEV_TYPE_BIT_SHIFT           0x7

#define MAC_FC_FRAME_TYPE_BITS_SHIFT_MSK      0x7
#define MAC_FC_SEC_ENA_BIT_SHIFT_MSK          0x8
#define MAC_FC_PENDING_BIT_SHIFT_MSK         0x10
#define MAC_FC_ACK_REQ_BIT_SHIFT_MSK         0x20
#define MAC_FC_LPWMN_ID_COMP_BIT_SHIFT_MSK   0x40
#define MAC_FC_DEV_TYPE_BIT_SHIFT_MSK        0x80  // 1 - FFD, 0 - RFD (rkris@wisense.in / aug/1

#define MAC_FC_FRAME_TYPE_BITS_MSK     0x7
#define MAC_FC_SEC_ENA_BIT_MSK         0x1
#define MAC_FC_PENDING_BIT_MSK         0x1
#define MAC_FC_ACK_REQ_BIT_MSK         0x1
#define MAC_FC_LPWMN_ID_COMP_BIT_MSK   0x1


/*
 * Table 3a : Possible values of the Frame Version field
 * <802.15.4e>
 */
#define MAC_FRAME_VER_802_15_4_2003   0x0
#define MAC_FRAME_VER_802_15_4        0x1


/*
 * Figure 36 : Format of the Frame Control field
 * MSB
 */
#define MAC_FC_REJOIN_REQUIRED_BIT_SHIFT       0x0
#define MAC_FC_DAM_BITS_SHIFT                  0x2
#define MAC_FC_FRAME_VER_BITS_SHIFT            0x4
#define MAC_FC_SAM_BITS_SHIFT                  0x6

#define MAC_FC_REJOIN_REQUIRED_BIT_SHIFT_MSK       0x01
#define MAC_FC_DAM_BITS_SHIFT_MSK                  0x0c
#define MAC_FC_FRAME_VER_BITS_SHIFT_MSK            0x30
#define MAC_FC_SAM_BITS_SHIFT_MSK                  0xc0

#define MAC_FC_REJOIN_REQUIRED_BIT_MSK       0x1
#define MAC_FC_DAM_BITS_MSK                  0x3
#define MAC_FC_FRAME_VER_BITS_MSK            0x3
#define MAC_FC_SAM_BITS_MSK                  0x3
   

/*
 * Table 58 : Security levels available to the MAC sublayer
 */
#define MAC_SECURITY_LEVEL_NONE          0x0
#define MAC_SECURITY_LEVEL_MIC_32        0x1
#define MAC_SECURITY_LEVEL_MIC_64        0x2
#define MAC_SECURITY_LEVEL_MIC_128       0x3
#define MAC_SECURITY_LEVEL_ENC           0x4
#define MAC_SECURITY_LEVEL_ENC_MIC_32    0x5
#define MAC_SECURITY_LEVEL_ENC_MIC_64    0x6
#define MAC_SECURITY_LEVEL_ENC_MIC_128   0x7

/*
 * Table 59 : Values of the key identifier mode
 */
#define MAC_KEY_ID_MODE_0   0x0
#define MAC_KEY_ID_MODE_1   0x1
#define MAC_KEY_ID_MODE_2   0x2
#define MAC_KEY_ID_MODE_3   0x3
   
   
/*
 * 5.2.2.1.2 Superframe Specification field
 */
#define MAC_SF_SPEC_BCN_ORDER_BIT_SHIFT  0x0  // Bits 0 to 3
#define MAC_SF_SPEC_SF_ORDER_BIT_SHIFT  0x4   // Bits 4 to 7
#define MAC_SF_SPEC_FINAL_CAP_SLOT_BIT_SHIFT  0x0   // Bits 8 to 11
#define MAC_SF_SPEC_BLE_BIT_SHIFT  0x4 // Bit 12
#define MAC_SF_SPEC_IS_COORD_BIT_SHIFT  0x6  // Bit 14    
#define MAC_SF_SPEC_ASSOCIATION_PERMIT_BIT_SHIFT  0x7  // Bit 15
   
#define MAC_SF_SPEC_BCN_ORDER_BIT_MSK  0xf  // Bits 0 to 3
#define MAC_SF_SPEC_SF_ORDER_BIT_MSK  0xf  // Bits 4 to 7
#define MAC_SF_SPEC_FINAL_CAP_SLOT_BIT_MSK  0xf  // Bits 8 to 11
#define MAC_SF_SPEC_BLE_BIT_MSK  0x1  // Bit 12
#define MAC_SF_SPEC_IS_COORD_BIT_MSK  0x1  // Bit 14    
#define MAC_SF_SPEC_ASSOCIATION_PERMIT_BIT_MSK  0x1  // Bit 15   
   
#define MAC_SF_SPEC_BCN_ORDER_BIT_SHIFT_MSK  0xf   
#define MAC_SF_SPEC_SF_ORDER_BIT_SHIFT_MSK  0xf0
#define MAC_SF_SPEC_FINAL_CAP_SLOT_BIT_SHIFT_MSK  0xf   // Bits 8 to 11
#define MAC_SF_SPEC_BLE_BIT_SHIFT_MSK  0x10 // Bit 12
#define MAC_SF_SPEC_IS_COORD_BIT_SHIFT_MSK  0x40  // Bit 14    
#define MAC_SF_SPEC_ASSOC_PERMIT_BIT_SHIFT_MSK  0x80  // Bit 15   


#define MAC_TX_REQ_TYPE_UNKNOWN   0x0
#define MAC_TX_REQ_TYPE_DATA   0x1
#define MAC_TX_REQ_TYPE_ASSOC_REQ   0x2
#define MAC_TX_REQ_TYPE_DISASSOC_REQ   0x3
#define MAC_TX_REQ_TYPE_BCN_PDU  0x4

   
/*
 * <802.15.4-2011>
 * Table 51 : MAC sublayer constants   
 */

/*
 * The minimum number of octets added by the MAC sublayer to the PSDU.
 */ 
#define MAC_aMinMPDUOverhead   9

/*
 * The maximum number of octets that can be transmitted in the MAC
 * Payload field.
 */ 
#define MAC_aMaxMACPayloadSize   ((aMaxPHYPacketSize) - (MAC_aMinMPDUOverhead))

/*
 * The number of symbols forming the basic time period used by the CSMA/CA
 * algorithm.
 */ 
#define MAC_aUnitBackoffPeriod   20
   
 /*
  * The maximum size of an MPDU, in octets, that can be followed by a SIFS period.
  */
#define MAC_aMaxSIFSFrameSize   18
   

/*
 * The number of slots contained in any superframe.
 */
#define MAC_aNumSuperframeSlots  16   

/*
 * The number of symbols forming a superframe slot when the superframe order is 
 * equal to zero, as described in 5.1.1.1.
 */   
#define MAC_aBaseSlotDuration  60   
   
/*
 * The number of symbols forming a superframe when the superframe order is equal 
 * to zero.
 */   
#define MAC_aBaseSuperframeDuration   ((MAC_aBaseSlotDuration) * (MAC_aNumSuperframeSlots))   


// MAC config params stored in NVM
#define MAC_TLV_TYPE_FIELD_SZ  1
#define MAC_TLV_LEN_FIELD_SZ  1
#define MAC_TLV_HDR_SZ (MAC_TLV_TYPE_FIELD_SZ + MAC_TLV_LEN_FIELD_SZ)

#define MAC_TLV_TYPE_CFG_DATA   0xd0
#define MAC_TLV_TYPE_LPWMN_ID  0xd1
#define MAC_TLV_TYPE_LAST_ALLOCATED_SHORT_ADDR  0xd2

#endif
