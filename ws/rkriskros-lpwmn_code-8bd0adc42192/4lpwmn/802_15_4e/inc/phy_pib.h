/*
 * File Name : phy_pib.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : 8/14/2013
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

#ifndef __PHY_PIB_H__
#define __PHY_PIB_H__

#include <typedefs.h>


// MAC config params stored in NVM
#define PHY_TLV_TYPE_FIELD_SZ  1
#define PHY_TLV_LEN_FIELD_SZ  1
#define PHY_TLV_HDR_SZ (PHY_TLV_TYPE_FIELD_SZ + PHY_TLV_LEN_FIELD_SZ)

#define PHY_TLV_TYPE_CFG_DATA   0xe0
#define PHY_TLV_TYPE_RF_CHANN_ID   0xe1

#define PHY_RF_CHANN_ID_TLV_LEN  1



typedef struct
{ 
  /*
   * phyCurrentChannel (Type : Integer) (Range : As defined in 8.1.2)
   * The RF channel to use for all following transmissions and receptions, 
   * 8.1.2.
   */ 
  UINT8_t phyCurrentChannel;

#ifdef RFD_NO_RESCAN_ON_REASSOC
  UINT8_t workingChannel;
#endif

  /*
   * The transmit power of the device in dBm.
   */
  SINT16_t phyTXPower;   // integer part
  SINT16_t phyTXPowerDecPart;  // decimal part

  // Implementation specific
  UINT8_t changeReqBitMsk;

#if 0
  // This implementation support page 0 only  
  /*
   * phyCurrentPage (Type : Integer) (Range : Any valid channel page)
   * This is the current PHY channel page. This is used in conjunction
   * with phyCurrentChannel to  uniquely identify the channel currently 
   * being used.
   */ 
  UINT8_t phyCurrentPage;
#endif
  
} PHY_PIB_s;


extern PHY_PIB_s PHY_PIB;

extern void PHY_PIB_init(void);

extern void PHY_updateCfgInfo(UINT8_t tlvId, UINT8_t tlvValLen, UINT8_t *val_p);

extern void PHY_readCfgInfo(void);

#endif
