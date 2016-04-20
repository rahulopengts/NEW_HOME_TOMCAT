/*
 * File Name: system.h
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Aug/21/2013
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

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <typedefs.h>
#include <sys_fatalErrs.h>
#include <mac_defs.h>
#include <ipv4.h>
#include <udp.h>

#define SYS_TRAFF_UPDATE_INTERVAL  5

#define SYS_COORD_UP_TIME_FIELD_LEN  4

#define SYSTEM_FATAL_ERROR_ID_LEN  0x2

#define SYS_BACK_TO_BACK_MEM_ALLOC_FLR_CNT_THRESHOLD  96

#define LPWMN_PHY_MODULE      0x1
#define LPWMN_MAC_MODULE      0x2
#define LPWMN_ADP_MODULE      0x3
#define LPWMN_APP_MODULE      0x4
#define LPWMN_PLTFRM_MODULE   0x5
#define LPWMN_SYSTEM_MODULE   0x6

extern SINT32_t SYS_wakeUpIntervalSecs;

#define SYS_SET_WAKEUP_INTERVAL(timeInSecs)  \
do \
{  \
    SYS_wakeUpIntervalSecs = (timeInSecs);  \
} while (0)


#ifdef __MSP430F2274__
#define SYS_BUFF_LEN  128
#define SYS_MAX_BUFF_DESC_CNT  8
#elif defined(__MSP430G2955__)
#define SYS_BUFF_LEN  1024
#define SYS_MAX_BUFF_DESC_CNT  32
#elif defined(__MSP430G2553__)
#define SYS_BUFF_LEN  128
#define SYS_MAX_BUFF_DESC_CNT  8
#else
#error micro not specified !!
#endif

#define SYS_APP_UDP_PORT_NR  0xf0b5

extern void SYS_fatal(UINT16_t sysFatalId);
   
   
// MAC / Upper layer interface

#define SYS_GLOBAL_EVT_TIMER_A0_EXPIRY        (UINT16_t)(1 << 0)      // Timer A0 (used by MAC)
#define SYS_GLOBAL_EVT_TIMER_A1_EXPIRY        (UINT16_t)(1 << 1)      // Timer A1 (used by APP)
#define SYS_GLOBAL_EVT_TIMER_1HZ_PERIODIC_EXPIRY    (UINT16_t)(1 << 2)   // Used by routing protocol
#define SYS_GLOBAL_EVT_APP_DATA_REQUEST         (UINT16_t)(1 << 3)    // APP -> ADP
#define SYS_GLOBAL_EVT_MAC_DATA_CONFIRM         (UINT16_t)(1 << 4)    // MAC -> APP
#define SYS_GLOBAL_EVT_MAC_DATA_INDICATION      (UINT16_t)(1 << 5)    // MAC -> APP
#define SYS_GLOBAL_EVT_MAC_FRAME_TX_DONE        (UINT16_t)(1 << 6)    // cc2520 -> MAC
#define SYS_GLOBAL_EVT_MAC_RETRY_CCA            (UINT16_t)(1 << 7)    // MAC -> MAC
#define SYS_GLOBAL_EVT_MAC_FRAME_RX_DONE        (UINT16_t)(1 << 8)    // cc2520 -> MAC
#define SYS_GLOBAL_EVT_MAC_BCN_REQUEST          (UINT16_t)(1 << 9)    // MAC -> MAC

#define SYS_GLOBAL_EVT_MAC_SCAN_CONFIRM         (UINT16_t)(1 << 10)   // MAC -> APP
#define SYS_GLOBAL_EVT_MAC_DATA_REQ_CMD_RCVD    (UINT16_t)(1 << 11)   // cc2520 -> MAC   
#define SYS_GLOBAL_EVT_RADIO_RX_FIFO_OVFL       (UINT16_t)(1 << 12)   // MAC -> APP
#define AODVL_CTRL_PKT_TX_REQUEST               (UINT16_t)(1 << 13)   // Mesh routing -> MAC
#define SYS_GLOBAL_EVT_ADP_DATA_CONFIRM         (UINT16_t)(1 << 14)   // ADP -> APP
#define SYS_GLOBAL_EVT_ADP_DATA_INDICATION      (UINT16_t)(1 << 15)    // ADP -> APP


#define SYS_GLOBAL_EVT_ADP_FWD_PKT_TX_REQ          (UINT16_t)(1 << 0)    // ADP -> MAC
#define SYS_GLOBAL_EVT_ADP_DATA_REQUEST            (UINT16_t)(1 << 1)    // ADP -> MAC
#define SYS_GLOBAL_EVT_MAC_ASSOC_CONFIRM           (UINT16_t)(1 << 2)    // MAC -> APP
#define SYS_GLOBAL_EVT_MAC_ASSOC_INDICATION        (UINT16_t)(1 << 3)    // MAC -> ADP 
#define SYS_GLOBAL_EVT_APP_DATA_REQ_POST_RT_DISC   (UINT16_t)(1 << 4)    // ADP -> ADP
#define SYS_GLOBAL_EVT_ADP_TRSCN_PENDING           (UINT16_t)(1 << 6)    // * -> ADP
#define SYS_GLOBAL_EVT_ENTER_LPM_REQUEST           (UINT16_t)(1 << 7)    // APP -> SYS
#define SYS_GLOBAL_EVT_UART_BLOCK_TX_DONE          (UINT16_t)(1 << 8)    // UART -> SYS
#define SYS_GLOBAL_EVT_RFD_REASSOC_REQ             (UINT16_t)(1 << 9)    // MAC -> NWM
#define SYS_GLOBAL_EVT_PENDING_FRAME_TRX_REQ       (UINT16_t)(1 << 10)    // MAC -> MAC
#define SYS_GLOBAL_EVT_UART_FRAME_PYLD_RCVD        (UINT16_t)(1 << 11)   // UART -> SYS
#define SYS_GLOBAL_EVT_UART_FRAME_HDR_RCVD         (UINT16_t)(1 << 12)   // UART -> SYS
#define SYS_GLOBAL_EVT_GW_RELAY_REQUEST            (UINT16_t)(1 << 13)   // GW -> MAC
#define SYS_GLOBAL_EVT_WAKEUP_TIMER_EXPIRY_EVT     (UINT16_t)(1 << 14)   // HW -> APP
#define SYS_GLOBAL_EVT_GPIO_INT_EVT                (UINT16_t)(1 << 15)   // HW -> APP

#define SYS_GLOBAL_EVT_COORD_POLL_PENDING          (UINT16_t)(1 << 0)    // MAC -> NWM
#define SYS_GLOBAL_EVT_COORD_POLL_IN_PROGRESS      (UINT16_t)(1 << 1)    //
#define SYS_GLOBAL_EVT_MLME_POLL_REQ_CONFIRM       (UINT16_t)(1 << 2)    // MAC -> APP
#define SYS_GLOBAL_EVT_PENDING_APP_DATA_REQ        (UINT16_t)(1 << 3)    // APP -> ADP
#define SYS_GLOBAL_EVT_NWK_RESTART_CMD_RCVD        (UINT16_t)(1 << 4)    // MAC -> SYS
#define SYS_GLOBAL_EVT_NODE_RESET_PEND             (UINT16_t)(1 << 5)    // MAC -> SYS
#define SYS_GLOBAL_EVT_NODE_SCAN_REQ               (UINT16_t)(1 << 6)    // NM -> MAC   (march/1/15, rkris@wisense.in)
#define SYS_GLOBAL_EVT_NHI_PING_REQ                (UINT16_t)(1 << 7)
#define SYS_GLOBAL_EVT_JOIN_NWK_REQ                (UINT16_t)(1 << 8)    // SYS -> NM
#define SYS_GLOBAL_EVT_PENDING_UC_FU_COMM          (UINT16_t)(1 << 9)
#define SYS_GLOBAL_EVT_IMAGE_REFLASH_PENDING       (UINT16_t)(1 << 10)    // FU -> SYS
#define SYS_GLOBAL_EVT_ARP_REQ_RCVD                (UINT16_t)(1 << 11)

// #define SYS_GLOBAL_EVT_SYNC_RT_BLOCK_TX_DONE


#define SYS_COORD_POLL_IN_PROGRESS( )  (SYS_globalEvtMsk2 & (SYS_GLOBAL_EVT_COORD_POLL_PENDING | SYS_GLOBAL_EVT_MLME_POLL_CONFIRM))




typedef struct
{
   UINT16_t offset;
   UINT16_t len;
} SYS_buffDesc_s;


#define SYS_INV_BD_HDNL SYS_MAX_BUFF_DESC_CNT
#define SYS_BD_HNDL_IS_VALID(hndl)  ((hndl) < SYS_MAX_BUFF_DESC_CNT)
#define SYS_GET_BD_BUFF(hndl) (SYS_buff + SYS_buffDescList[(hndl)].offset)
#define SYS_GET_BD_LEN(hndl) SYS_buffDescList[(hndl)].len
#define SYS_ADJ_BD_TAIL(hndl, by)  \
  do \
  {  \
     if (SYS_buffDescList[(hndl)].len > (by))  \
         SYS_buffDescList[(hndl)].len -= (by); \
     else                                      \
         SYS_fatal(SYS_FATAL_ERR_300);                          \
  } while(0)
    
#define SYS_ADJ_BD_HEAD(hndl, by)  \
  do \
  {  \
     if (SYS_buffDescList[(hndl)].len > by)  \
     {                                       \
         SYS_buffDescList[(hndl)].len -= (by); \
         SYS_buffDescList[(hndl)].offset += (by); \
     }                                       \
     else                                    \
         SYS_fatal(SYS_FATAL_ERR_301);                        \
  } while(0)    
   

// This macro assumes there are "by" unused bytes before the
// specified BD's associated buffer    
#define SYS_PULL_BD_HEAD(hndl, by) \
  do \
  {  \
     SYS_buffDescList[(hndl)].len += (by); \
     SYS_buffDescList[(hndl)].offset -= (by); \
  } while(0)    
     
   
#define SYS_TRSCN_Q_HNDL_FIELD_LEN  0x1    
    
#define SYS_TRSCN_TYPE_FIELD_LEN  0x1    

typedef struct
{
  UINT8_t qHead, qTail;
} SYS_trscnQ_s;

#define SYS_TRSCN_Q_IS_EMPTY(q_p)  ((q_p)->qHead == SYS_MAX_BUFF_DESC_CNT)
#define SYS_TRSCN_Q_IS_NOT_EMPTY(q_p)  ((q_p)->qHead < SYS_MAX_BUFF_DESC_CNT)
#define SYS_GET_TRSCN_Q_HEAD(q_p) ((q_p)->qHead)
#define SYS_GET_TRSCN_Q_TAIL(q_p) ((q_p)->qTail)
#define SYS_GET_TRSCN_Q_BD_BUFF(hndl)  ((SYS_GET_BD_BUFF(hndl)) + SYS_TRSCN_Q_HNDL_FIELD_LEN)
#define SYS_GET_TRSCN_Q_NEXT(hndl) (*(SYS_GET_BD_BUFF(hndl)))
#define SYS_TRSCN_Q_HNDL_VALID(hndl)  ((hndl) < SYS_MAX_BUFF_DESC_CNT)


extern void SYS_initTrscnsQ(SYS_trscnQ_s *queue_p);
extern void SYS_addToTrscnsQ(SYS_trscnQ_s *queue_p, UINT8_t bdHndl);
extern void SYS_delFromTrscnsQ(SYS_trscnQ_s *queue_p, UINT8_t bdHndl);
extern void SYS_purgeTrscnsQ(SYS_trscnQ_s *queue_p);

extern SYS_buffDesc_s SYS_buffDescList[];
extern UINT8_t SYS_buff[];

extern volatile UINT8_t SYS_rxBuffBusy;

extern volatile UINT16_t SYS_globalEvtMsk0;
extern volatile UINT16_t SYS_globalEvtMsk1;
extern volatile UINT16_t SYS_globalEvtMsk2;

extern UINT16_t SYS_rebootDelaySecs;

extern void SYS_loop(void);

extern UINT8_t SYS_allocMem(UINT16_t len);
extern void SYS_freeMem(UINT8_t bdHndl);

extern UINT32_t SYS_upTimeInSecs;

extern UINT16_t SYS_findFreeDescCnt(void);

extern UINT32_t  SYS_memAllocDoneCnt;
extern UINT32_t  SYS_memFreeCnt;
extern UINT16_t  SYS_buffAllocFlrCnt;
extern UINT16_t  SYS_bdAllocFlrCnt;
extern UINT8_t SYS_contBuffAllocFlrCnt;

#endif
