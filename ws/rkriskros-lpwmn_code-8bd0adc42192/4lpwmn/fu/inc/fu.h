/*
 * File Name : fu.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : 11/5/2013
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

#ifndef __FU_H__
#define __FU_H__

#include <typedefs.h>
#include <m24m01.h>
#include <radio.h>

#define UC_FU_REFLASH_WAIT_SECS  10


#define FW_STORE_MAX_IMAGE_CNT  2

#define FW_STORE_IMAGE_NONE  0x0
#define FW_STORE_IMAGE_1   0x1
#define FW_STORE_IMAGE_2   0x2


#define FW_IMAGE_TYPE_RFD      0x1
#define FW_IMAGE_TYPE_FFD      0x2
#define FW_IMAGE_TYPE_COORD    0x3
#define FW_IMAGE_TYPE_SNIFFER  0x4


// First byte shows availability of storage for up to 8 images.
// Second byte contains "valid" bit for up to 8 images.
// Third byte contains "active" bit for up to 8 images.
// Fourth byte contains "upgrade-in-progress" bit for up to 8 images.
// Right now only 2 images can be stored.
#define FU_IMAGE_STORE_IMAGE_FLAGS_ATTR_VAL_LEN  4

#define FU_IMAGE_STORE_IMAGE_LEN_ATTR_VAL_LEN  2
#define FU_IMAGE_STORE_IMAGE_SEG_CNT_ATTR_VAL_LEN  1
#define FU_IMAGE_STORE_IMAGE_TYPE_ATTR_VAL_LEN  1



// BR is short for BOOT_RECORD


#define FW_STORE_BR_SIGNATURE_OFF  0x0
#define FW_STORE_BR_SIGNATURE_LEN  2
#define FW_STORE_BR_SIGNATURE  0xa53c

#define FW_STORE_BR_FLAGS_OFF  FW_STORE_BR_SIGNATURE_LEN
#define FW_STORE_BR_FLAGS_LEN  2   // One byte per image

#define FW_STORE_IMAGE_RCVD_TIME_STAMP_LEN   4
#define FW_STORE_IMAGE_1_RCVD_TIME_STAMP_OFF  (FW_STORE_BR_FLAGS_OFF \
		                                       + FW_STORE_BR_FLAGS_LEN)
#define FW_STORE_IMAGE_2_RCVD_TIME_STAMP_OFF  (FW_STORE_IMAGE_1_RCVD_TIME_STAMP_OFF \
                                               + FW_STORE_IMAGE_RCVD_TIME_STAMP_LEN)

#define FW_STORE_MAX_SUPPORTED_IMAGE_SZ_KB  62


#define FW_STORE_BR_IMAGE_INFO_LEN   64
#define FW_STORE_BR_IMAGE_1_OFF  16
#define FW_STORE_BR_IMAGE_2_OFF  \
	    (FW_STORE_BR_IMAGE_1_OFF + FW_STORE_BR_IMAGE_INFO_LEN)


#define FW_STORE_BR_IMAGE_1_INFO_SIGNATURE  0xe92d
#define FW_STORE_BR_IMAGE_2_INFO_SIGNATURE  0x784b

#define FW_STORE_BR_IMAGE_INFO_SIGNATURE_OFF  0x0
#define FW_STORE_BR_IMAGE_INFO_SIGNATURE_LEN  0x2
#define FW_STORE_BR_IMAGE_INFO_SEG_CNT_FIELD_OFF   FW_STORE_BR_IMAGE_INFO_SIGNATURE_LEN
#define FW_STORE_BR_IMAGE_INFO_SEG_CNT_FIELD_LEN   0x2

#define FW_STORE_BR_IMAGE_INFO_SEG_LEN  2
#define FW_STORE_BR_IMAGE_INFO_SEG_ADDR  2
#define FW_STORE_BR_IMAGE_INFO_SEG_CRC  2

#define FW_STORE_BR_SEG_LENGTH_FIELD_SZ  2
#define FW_STORE_BR_SEG_CRC_FIELD_SZ  2
#define FW_STORE_BR_SEG_SA_IN_STORE_FIELD_SZ  2
#define FW_STORE_BR_SEG_SA_IN_EXEC_FIELD_SZ  2
#define FW_STORE_BR_SEG_INFO_LEN  (FW_STORE_IMAGE_SEG_LENGTH_FIELD_SZ  \
                                   + FW_STORE_IMAGE_SEG_CRC_FIELD_SZ \
							       + FW_STORE_IMAGE_SEG_SA_IN_STORE_FIELD_SZ \
								   + FW_STORE_IMAGE_SEG_SA_IN_EXEC_FIELD_SZ)


#ifdef UC_FU_ENA
#ifdef PLTFRM_M24M01_ENA

/*
 * 1 Mega-bits -> 1024 * 1024 bits -> 128 * 1024 bytes -> 128 kilo-bytes
 */
#define FW_STORE_TOTAL_SIZE_KB  M24M01_MEM_SIZE_KB
#define FW_STORE_PAGE_SIZE_BYTES  M24M01_PAGE_SZ

// The lower 64 KB will store 1 image and the upper 64 KB will store the
// second image.
#define FW_STORE_IMAGE_1_START_ADDR  FW_STORE_PAGE_SIZE_BYTES
// #define FW_STORE_IMAGE_2_START_ADDR  (UINT32_t)((M24M01_MEM_SIZE_KB / 2) << 10)
#else
#error firmware storage device not specified !!
#endif
#endif

#define UC_FU_PAGE_MSG_SEG_INFO_FIELD_SZ  2
#define UC_FU_PAGE_MSG_SEG_IDX_FIELD_SHIFT  13
#define UC_FU_PAGE_MSG_SEG_IDX_FIELD_BM  0x7
#define UC_FU_PAGE_MSG_SEG_IDX_FIELD_BM_SHIFTED  0xe000   // Upper 3 bits


#define FW_STORE_BR_VALID_IMAGE_BIT  0x01
#define FW_STORE_BR_UPDATE_IN_PROGRESS_BIT  0x02
#define FW_STORE_BR_ACTIVE_IMAGE_BIT  0x04


#ifdef __MSP430G2955__
#define FU_MAX_FW_SEG_CNT  4
#else
#error micro not specified or supported !!
#endif

typedef enum
{
  UC_FU_STATE_IDLE,
  UC_FU_STATE_WAIT_INIT_2,
  UC_FU_STATE_INIT_RESP_SENT,
  UC_FU_STATE_ABORT_RCVD,
  UC_FU_STATE_RECEIVING_IMAGE,
  UC_FU_STATE_RECEIVED_IMAGE,
  UC_FU_STATE_WRITING_SEG,
  UC_FU_STATE_READY_TO_SWITCH_TO_SBY_IMAGE
} UC_FU_state_t;


typedef enum
{
  UC_FU_RC_SUCCESS = 0,
  UC_FU_RC_NO_STORAGE = 1,
  UC_FU_RC_INV_REQUEST = 2,
  UC_FU_RC_INV_PAGE_SZ = 3,
  UC_FU_RC_INV_TOO_MANY_SEGMENTS = 4,
  UC_FU_RC_SEG_TOO_BIG = 5,
  UC_FU_RC_INV_SEG_ADDR = 6,
  UC_FU_RC_EEPROM_ACCESS_ERROR = 7,
  UC_FU_RC_NO_ALTERNATE_IMAGE = 8,
  UC_FU_RC_MALFORMED_INIT_REQ = 9,
  UC_FU_RC_WRONG_PAGE_RCVD = 10,
  UC_FU_RC_SEG_CRC_MISMATCH = 11,
  UC_FU_RC_NO_STANDBY_IMAGE = 12,
  UC_FU_RC_STORE_IMAGE_INFO_CORRUPT = 13
} UC_FU_rc_t;


// Do not use any bit fields

typedef struct
{
   UINT8_t segRcvd;
   UINT8_t segWritten;
   UINT8_t lastPageSz;
   UINT16_t pageCnt;
   UINT16_t segAddr;
   UINT16_t segLen;
   UINT16_t segCrc;
} UC_FU_segInfo_s;

typedef struct
{
   UINT8_t currSegIdx;
   UINT8_t segCnt;
   UINT8_t state;
   UINT8_t pageSz;
   UINT8_t respRC;

   UINT8_t validImgBitMsk;
   UINT8_t upgradeableImage;
   UINT8_t timeToReflash;

   UINT16_t nextExpPageIdx;
   UINT16_t lastPageCRC;
   UINT32_t imageBase;

   UINT8_t imageRcvdTSBuff[FW_STORE_IMAGE_RCVD_TIME_STAMP_LEN];

   UC_FU_segInfo_s segList[FU_MAX_FW_SEG_CNT];
} UC_FU_cntxt_s;


typedef struct
{
   UINT8_t type;
   UINT8_t segCnt;
   UINT16_t len;
} FU_imageInfo_s;


extern void UC_FU_procRcvdMsg(UINT8_t *msg_p, UINT8_t msgLen);
extern void FU_init(void);
extern void UC_FU_sendResp(void);
void FU_checkStartReflash(void);
extern void PLTFRM_reflash(UINT32_t imageStartInStore, UC_FU_segInfo_s *segInfo_p, UINT8_t segCnt);
extern void FU_readImageStoreFlags(UINT8_t *buff_p);
extern void FU_getImageStoreFlags(UINT8_t *buff_p);


extern PLTFRM_sts_t FU_M24M01_read(UINT32_t addr,
		                           UINT16_t len,
                                   UINT8_t *byte_p);

extern PLTFRM_sts_t FU_M24M01_write(UINT32_t addr,
		                            UINT16_t len,
                                    UINT8_t *byte_p);


extern void FU_fatal(void);

extern UINT8_t PLTFRM_checkFlashSegCRC(UC_FU_segInfo_s *seg_p);

extern void FU_delay10MicroSecs(void);
extern void FU_delay1MilliSec(void);

extern PLTFRM_sts_t __FU_M24M01_write(UINT32_t addr,
                                      UINT8_t byte);

extern void FU_getImageInfo(UINT8_t imageId, FU_imageInfo_s *imageInfo_p);

extern void FU_getImageRcvdTS(UINT8_t imageId, UINT8_t *buff_p);

extern void __FU_fatal(void);
extern void __readImageStoreFlags(UINT8_t *buff_p);
extern void __writeImageStoreFlags(UINT8_t *buff_p);

#endif
