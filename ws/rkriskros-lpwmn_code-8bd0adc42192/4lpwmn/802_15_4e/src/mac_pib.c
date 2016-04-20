/*
 * File Name : mac_pib.c
 * Author : ram krishnan (rkris@wisense.in)
 * Created : 8/2/2013
 *
 *
 * Copyright (c) <2013>, <ram krishnan>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
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
#include <mac_pib.h>


MAC_PIB_s MAC_PIB;

void MAC_PIB_init(  )
{
   memset(&MAC_PIB, 0, sizeof(MAC_PIB_s));
   
   /*
    * <802.15.4-2011>
    * If this  value is 0xffff, the device is not associated.
    */
#ifndef LPWMN_COORD
   /*
    * <802.15.4-2006>
    * A value of 0xffff indicates that the device does not have a short
    * address.
    */
   MAC_PIB.macShortAddress = MAC_BROADCAST_SHORT_ADDR;

#ifdef DEV_TYPE_FFD
   MAC_PIB.prevAllocShortAddr = MAC_BROADCAST_SHORT_ADDR;
#endif
#else
   MAC_PIB.macShortAddress = LPWMN_COORD_SHORT_ADDR;
#endif   
   
   // rkris@wisense.in - aug/9/15 - LPWMN Id is picked up from NVM
   MAC_PIB.macLPWMNId = MAC_BROADCAST_LPWMN_ID;

   // <802.15.4e>
   // MAC_PIB.macTSCHenabled = 0x0;
   
   // <802.15.4>
   MAC_PIB.macMinBE = 0x3;

   // <802.15.4>
   MAC_PIB.macMaxBE = 0x5;

#ifdef RADIO_BAUD_RATE_1200
   // <802.15.4>
   MAC_PIB.macMaxCSMABackoffs = 0x8;  // To account for long packets (.5 seconds)
#elif defined RADIO_BAUD_RATE_38383
   MAC_PIB.macMaxCSMABackoffs = 0x4;
#elif defined RADIO_BAUD_RATE_10000
   MAC_PIB.macMaxCSMABackoffs = 0x4;
#elif defined RADIO_BAUD_RATE_250000
   MAC_PIB.macMaxCSMABackoffs = 0x4;
#else
#error specified baud rate not supported !!
#endif
   // <802.15.4>
   MAC_PIB.macMaxFrameRetries = 0x3;

   // <802.15.4>
#ifdef DEV_TYPE_RFD
   MAC_PIB.macRxOnWhenIdle = 0x0;
#else
   MAC_PIB.macRxOnWhenIdle = 0x1;
#endif
   
   MAC_PIB.macAssociationPermit = 0x1;
   
   // TODO - set to a random value (0x00 - 0xff)
   MAC_PIB.macBSN = 0x13;
   
   // TODO - set to a random value (0x00 - 0xff)
   MAC_PIB.macDSN = 0xa5;
   
#if 0   
   MAC_PIB.macSIFSPeriod = MAC_MIN_SIFS_PERIOD_USECS;
   
   MAC_PIB.macLIFSPeriod = MAC_MIN_LIFS_PERIOD_USECS;
#endif
   
  /*
   * LPWMNs that do not wish to use the superframe structure (referred to as non
   * beacon-enabled LPWMNs) shall set both macBeaconOrder and macSuperframeOrder to
   * 15. In this case, a coordinator shall not transmit beacons, except upon 
   * receipt of a beacon request command; all transmissions, with the exception of
   * ack frames and any data frame that quickly follows the ack of a data request
   * command, as described in 5.1.6.3, shall use an unslotted CSMA-CA mechanism 
   * to access the channel. In addition, GTSs shall not be permitted.
   */   
   MAC_PIB.macBeaconOrder = 0xf;
   MAC_PIB.macSuperframeOrder = 0xf;
   
   MAC_PIB.macAutoRequest = 0x1;
   
   MAC_PIB.macCoordShortAddress = MAC_BROADCAST_SHORT_ADDR;
   
   MAC_PIB.macResponseWaitTime = 32;
  
#if 0   
   // See mac_pib.h
   MAC_PIB.macAckWaitDurationMicroSecs = MAC_aUnitBackoffPeriod 
                                + PHY_aTurnaroundTime
                                + PHY_SRH_DURATION
                                + (PHY_PDU_PYLD_LEN_FIELD_LEN * PHY_SYMBOLS_PER_OCTET)
                                + (MAC_ACK_PDU_LEN * PHY_SYMBOLS_PER_OCTET);
   MAC_PIB.macAckWaitDurationMicroSecs *= PHY_SYMBOL_LENGTH_USECS;                              
#endif
   
   return;
}


