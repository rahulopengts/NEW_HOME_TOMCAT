/*
 * File Name : phy_defs.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : 8/5/2013
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

#ifndef __PHY_DEFS_H__
#define __PHY_DEFS_H__

#ifdef RADIO_CC1200
#include <cc1200.h>
#endif

/*
 * Single page for 2450 MHz band
 */
#define PHY_2450_MHZ_BAND_PAGE_0    0x0

#define PHY_2450_MHZ_BAND_CHANNEL_MSK  0xffff   // 16 channels

/*
 * In channel page zero, 16 channels are available in the 2450 MHz band.
 *
 * Fc = 2405 + 5*(k - 11) in megahertz, for k = 11, 12, ....., 26
 *
 * 8.1.2 Channel assignments
 * Channel assignments are defined through a combination of channel numbers and 
 * channel pages.
 */
#define PHY_2450_MHZ_BAND_PAGE_0_CHAN_11   11       // 2405
#define PHY_2450_MHZ_BAND_PAGE_0_CHAN_12   12       // 2410 
#define PHY_2450_MHZ_BAND_PAGE_0_CHAN_13   13       // 2415
#define PHY_2450_MHZ_BAND_PAGE_0_CHAN_14   14       // 2420
#define PHY_2450_MHZ_BAND_PAGE_0_CHAN_15   15       // 2425
#define PHY_2450_MHZ_BAND_PAGE_0_CHAN_16   16       // 2430
#define PHY_2450_MHZ_BAND_PAGE_0_CHAN_17   17       // 2435
#define PHY_2450_MHZ_BAND_PAGE_0_CHAN_18   18       // 2440 
#define PHY_2450_MHZ_BAND_PAGE_0_CHAN_19   19       // 2445
#define PHY_2450_MHZ_BAND_PAGE_0_CHAN_20   20       // 2450
#define PHY_2450_MHZ_BAND_PAGE_0_CHAN_21   21       // 2455
#define PHY_2450_MHZ_BAND_PAGE_0_CHAN_22   22       // 2460
#define PHY_2450_MHZ_BAND_PAGE_0_CHAN_23   23       // 2465
#define PHY_2450_MHZ_BAND_PAGE_0_CHAN_24   24       // 2470
#define PHY_2450_MHZ_BAND_PAGE_0_CHAN_25   25       // 2475
#define PHY_2450_MHZ_BAND_PAGE_0_CHAN_26   26       // 2480
   
#define PHY_2450_MHZ_BAND_CHANNEL_CNT  16


#define PHY_2400_MHZ_BAND_CHAN_1    0x0
#define PHY_2400_MHZ_BAND_CHAN_2    0x1
#define PHY_2400_MHZ_BAND_CHAN_3    0x2
#define PHY_2400_MHZ_BAND_CHAN_4    0x3
#define PHY_2400_MHZ_BAND_CHAN_5    0x4
#define PHY_2400_MHZ_BAND_CHAN_6    0x5

#define PHY_2400_MHZ_BAND_CHAN_INV  0x10

#define PHY_2400_MHZ_BAND_CHANNEL_MSK  0x3f


#define PHY_865_867_MHZ_BAND_CHAN_1   0x0
#define PHY_865_867_MHZ_BAND_CHAN_2   0x1
#define PHY_865_867_MHZ_BAND_CHAN_3   0x2
#define PHY_865_867_MHZ_BAND_CHAN_4   0x3
#define PHY_865_867_MHZ_BAND_CHAN_5   0x4
#define PHY_865_867_MHZ_BAND_CHAN_6   0x5
#define PHY_865_867_MHZ_BAND_CHAN_7   0x6
#define PHY_865_867_MHZ_BAND_CHAN_8   0x7
#define PHY_865_867_MHZ_BAND_CHAN_9   0x8
#define PHY_865_867_MHZ_BAND_CHAN_INV  0x10

#define PHY_865_867_MHZ_BAND_CHANNEL_CNT  9

#define PHY_865_867_MHZ_BAND_CHANNEL_MSK  0x1ff // 9 channels ( 1 - 9)
   


#define PHY_868_MHZ_BAND_CHAN_1  0x0
#define PHY_868_MHZ_BAND_CHANNEL_CNT  1
#define PHY_868_MHZ_BAND_CHANNEL_MSK  0x1

/*
 * <802.15.4-2011>
 * Table 70 : PHY constants
 */
#define PHY_aMaxPHYPacketSize    127
#define PHY_aTurnaroundTime      12        // In symbols

// <802.15.4-2011>
// 10.3.3
#define PHY_O_QPSK_868_MHZ_SYMBOL_LENGTH_USECS   40   // 25 ksymbol/sec
#define PHY_O_QPSK_780_MHZ_SYMBOL_LENGTH_USECS   16   // 62.5 ksymbol/sec
#define PHY_O_QPSK_915_MHZ_SYMBOL_LENGTH_USECS   16   // 62.5 ksymbol/sec
#define PHY_2450_MHZ_BAND_SYMBOL_LENGTH_USECS  16   // 62.5 ksymbol/sec

#define PHY_2450_MHZ_BAND_SYMBOL_LENGTH_USECS_SHIFT  4


#ifdef RADIO_CC2520
#define PHY_SYMBOL_LENGTH_USECS  PHY_2450_MHZ_BAND_SYMBOL_LENGTH_USECS
#define PHY_SYMBOL_LENGTH_USECS_SHIFT  PHY_2450_MHZ_BAND_SYMBOL_LENGTH_USECS_SHIFT
  

/* 
 * The number of symbols per octet for the current PHY (0-QPSK @ 2.4 GHz).
 * This is actually a PHY PIB.
 */
#define PHY_SYMBOLS_PER_OCTET  2

/*
 * The duration of the synchronization hdr (SHR) in symbols for the current PHY
 * (O-QPSK @ 2.4 GHz).
 */
#define PHY_SRH_DURATION  (5 * PHY_SYMBOLS_PER_OCTET)
   
/*
 * 10.1.3 Frame Length field
 * The Frame Length field specifies the total number of octets contained in the 
 * PSDU (i.e., PHY payload). It is a value between 0 and aMaxPHYPacketSize, as 
 * described in 9.2. Table 72 summarizes the type of payload versus the frame 
 * length value.
 */   
#define PHY_PDU_PYLD_LEN_FIELD_LEN   1

#elif defined(RADIO_CC2500)
#define PHY_SYMBOL_LEN_USECS  100  // <TODO>
#elif defined(RADIO_CC1101)
#ifdef RADIO_BAUD_RATE_10000
#define PHY_SYMBOL_LEN_USECS  100  // <TODO>
#elif defined(RADIO_BAUD_RATE_1200)
#define PHY_SYMBOL_LEN_USECS  834  // <TODO>
#elif defined(RADIO_BAUD_RATE_38383)
#define PHY_SYMBOL_LEN_USECS  26  // 26.0532 microsecs
#else
#error radio baud rate not specified !!
#endif
#endif


#define PHY_TX_POWER_CHANGE_REQ_BM  (1 << 0)
#define PHY_CHANN_CHANGE_REQ_BM     (1 << 1)

#endif

