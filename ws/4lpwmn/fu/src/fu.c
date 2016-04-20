/*
 * File Name : fu.c
 * Author : ram krishnan (rkris@wisense.in)
 * Created : Aug/2015
 *
 *
 * Copyright (c) <2015>, <ram krishnan>
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

#if defined(UC_FU_ENA)

#if defined(PLTFRM_M24M01_ENA)

#include <string.h>
#include <dis.h>
#include <pltfrm.h>
#include <system.h>
#include <fu.h>
#include <adp.h>

#define UC_FU_STATS_ENA

#ifdef UC_FU_STATS_ENA
#pragma LOCATION(UC_FU_respTxCnt, 0x1100);
UINT16_t UC_FU_respTxCnt = 0;
#pragma LOCATION(UC_FU_respAllocFlrCnt, 0x1102);
UINT16_t UC_FU_respAllocFlrCnt = 0;
#pragma LOCATION(UC_FU_wrongPageRcvdCnt, 0x1104);
UINT16_t UC_FU_wrongPageRcvdCnt = 0;
#pragma LOCATION(UC_FU_correctPageRcvdCnt, 0x1106);
UINT16_t UC_FU_correctPageRcvdCnt = 0;
#endif


#pragma LOCATION(UC_FU_cntxt, 0x1108);
UC_FU_cntxt_s  UC_FU_cntxt;


/*
 * MSP430G2955
 * Flash Memory: 56 KB
 * Int Vector:  0xFFC0 to 0xFFFF -> 64 bytes
 * Code memory: 0x2100 to 0xFFC0 -> 57024 bytes
 */


/*
 * The onboard M24M01 EEPROM has 128 kilo-bytes of memory. This is enough to hold
 * two firmware images. Once a node has downloaded a new image from the gateway,
 * it will write the image to the MSP430's onboard flash memory and then reset
 * itself. The AT24MAC602 will hold the address (in the M24M01) of the
 * image corresponding to the firmware currently running on the MSP430. If the new
 * image has a problem (like repeated FU_fatal(), the node can go back to the old
 * image. The AT24MAC602 will also have a field (per image) which will indicate if
 * the image is good or bad.
 */


#pragma RETAIN(FU_markActiveImage)
#pragma CODE_SECTION(FU_markActiveImage, ".flashUpdate")

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void FU_markActiveImage(void)
{
   UINT8_t buff[FW_STORE_BR_FLAGS_LEN];

   __readImageStoreFlags(buff);

   if (UC_FU_cntxt.upgradeableImage == FW_STORE_IMAGE_1)
   {
       buff[1] &= ~FW_STORE_BR_ACTIVE_IMAGE_BIT;
       buff[0] |= (FW_STORE_BR_VALID_IMAGE_BIT
                   | FW_STORE_BR_ACTIVE_IMAGE_BIT);
       buff[0] &= ~FW_STORE_BR_UPDATE_IN_PROGRESS_BIT;
   }
   else
   {
       buff[0] &= ~FW_STORE_BR_ACTIVE_IMAGE_BIT;
       buff[1] |= (FW_STORE_BR_VALID_IMAGE_BIT
                   | FW_STORE_BR_ACTIVE_IMAGE_BIT);
       buff[1] &= ~FW_STORE_BR_UPDATE_IN_PROGRESS_BIT;
   }

   __writeImageStoreFlags(buff);

   {
       UINT8_t vBuff[FW_STORE_BR_FLAGS_LEN];

       __readImageStoreFlags(vBuff);

       if (buff[0] != vBuff[0]
           || buff[1] != vBuff[1])
       {
           __FU_fatal();
       }
   }

   return;
}


#pragma LOCATION(FU_checkStartReflash, 0xeb60);
#pragma RETAIN(FU_checkStartReflash)


/*
 ********************************************************************
 *  This function and all functions called by this function should lie
 *  in the .flashUpdate section.
 ********************************************************************
 */
void FU_checkStartReflash(void)
{
   if (UC_FU_cntxt.state == UC_FU_STATE_RECEIVED_IMAGE
       || UC_FU_cntxt.state == UC_FU_STATE_READY_TO_SWITCH_TO_SBY_IMAGE)
   {
       if (UC_FU_cntxt.timeToReflash < 1)
           __FU_fatal();
       else
       {
           UC_FU_cntxt.timeToReflash --;
           if (UC_FU_cntxt.timeToReflash == 0)
           {
               UINT32_t storeOff = FW_STORE_IMAGE_1_START_ADDR;

               if (UC_FU_cntxt.upgradeableImage == FW_STORE_IMAGE_2)
               {
                   UINT8_t idx;

                   storeOff = M24M01_MEM_SIZE_KB;

                   // storeOff <<= 9;  // 128 KB / 2

                   for (idx=0; idx<9; idx++)
                        storeOff += storeOff;

               }

               // This function will clear and write the new image to the flash.
               // Beyond this point, only calls functions in the "flashUpdate"
               // segment.
               PLTFRM_reflash(storeOff, UC_FU_cntxt.segList, UC_FU_cntxt.segCnt);

               // Update the boot record on the eeprom
               FU_markActiveImage();

               // Reset
               PLTFRM_mcuReset();
           }
       }
   }
}


/*
 ***********************************************************************************************
 ***********************************************************************************************
 ***********************************************************************************************
 ***********************************************************************************************
 ***********************************************************************************************
 ***********************************************************************************************
 ***********************************************************************************************
 */


#if 0
/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void FU_reflashTmoCbFn(void)
{
	UC_FU_cntxt.timeToReflash = 1;
	FU_checkStartReflash();
}
#endif

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void FU_readPageFromStore(UINT32_t storeOff, UINT8_t *buff_p, UINT16_t len)
{
   PLTFRM_sts_t sts;

   sts = M24M01_read(PLTFRM_M24M01_1_DEV_ID,
                     storeOff,
                     len, buff_p);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_3010);

   return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void FU_savePageToStore(UINT32_t storeAddr, UINT8_t *buff_p, UINT16_t len)
{
   PLTFRM_sts_t sts;

   sts = M24M01_write(PLTFRM_M24M01_1_DEV_ID,
                      storeAddr,  // UC_FU_cntxt.storeOffset,
                      len, buff_p);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_3006);

   // UC_FU_cntxt.storeOffset += len;

   return;
}




/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void FU_resetStorageBootRecord(void)
{
   PLTFRM_sts_t sts;
   UINT8_t buff[FW_STORE_BR_SIGNATURE_LEN
                + FW_STORE_BR_FLAGS_LEN];

   buff[0] = 0xa5;
   buff[1] = 0x3c;
   buff[2] = 0x0;  // Image_1 flags
   buff[3] = 0x0;  // Image_2 flags
   sts = M24M01_write(PLTFRM_M24M01_1_DEV_ID,
                      FW_STORE_BR_SIGNATURE_OFF,
                      sizeof(buff),
                      buff);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_3002);

   {
       UINT8_t vBuff[FW_STORE_BR_IMAGE_INFO_SIGNATURE_LEN
                     + FW_STORE_BR_IMAGE_INFO_SEG_CNT_FIELD_LEN];

       UTIL_htons(vBuff, FW_STORE_BR_IMAGE_1_INFO_SIGNATURE);
       UTIL_htons(vBuff + FW_STORE_BR_IMAGE_INFO_SIGNATURE_LEN, 0x0);

       sts = M24M01_write(PLTFRM_M24M01_1_DEV_ID,
                          FW_STORE_BR_IMAGE_1_OFF,
                          sizeof(vBuff),
                          vBuff);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_3018);

       UTIL_htons(vBuff, FW_STORE_BR_IMAGE_2_INFO_SIGNATURE);
       sts = M24M01_write(PLTFRM_M24M01_1_DEV_ID,
                          FW_STORE_BR_IMAGE_2_OFF,
                          sizeof(vBuff),
                          vBuff);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_3019);
   }

   return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void FU_readImageStoreFlags(UINT8_t *buff_p)
{
   PLTFRM_sts_t sts;

   sts = M24M01_read(PLTFRM_M24M01_1_DEV_ID,
                     FW_STORE_BR_FLAGS_OFF,
                     FW_STORE_BR_FLAGS_LEN,
                     buff_p);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_3015);

   return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void FU_writeImageStoreFlags(UINT8_t *buff_p)
{
   PLTFRM_sts_t sts;

   sts = M24M01_write(PLTFRM_M24M01_1_DEV_ID,
                      FW_STORE_BR_FLAGS_OFF,
                      FW_STORE_BR_FLAGS_LEN,
                      buff_p);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_3016);

   return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
UINT8_t FU_loadImageInfoFromStore(UINT8_t imageId)
{
    UINT16_t expSign, readSign;
    UINT32_t storeOff;
    UINT8_t buff[6], segCnt, idx;
    UC_FU_segInfo_s *segInfo_p = UC_FU_cntxt.segList;

    if (imageId == FW_STORE_IMAGE_1)
    {
        expSign = FW_STORE_BR_IMAGE_1_INFO_SIGNATURE;
        storeOff = FW_STORE_BR_IMAGE_1_OFF;
    }
    else
    {
        expSign = FW_STORE_BR_IMAGE_2_INFO_SIGNATURE;
        storeOff = FW_STORE_BR_IMAGE_2_OFF;
    }

    if (M24M01_read(PLTFRM_M24M01_1_DEV_ID, storeOff,
                    FW_STORE_BR_IMAGE_INFO_SIGNATURE_LEN
                    + FW_STORE_BR_IMAGE_INFO_SEG_CNT_FIELD_LEN,
                    buff) != PLTFRM_STS_SUCCESS)
        SYS_fatal(SYS_FATAL_ERR_3017);

    readSign = UTIL_ntohs(buff);
    if (readSign != expSign)
        return 0;

    segCnt = UTIL_ntohs(buff + FW_STORE_BR_IMAGE_INFO_SIGNATURE_LEN);
    if (segCnt == 0 || segCnt > FU_MAX_FW_SEG_CNT)
        return 0;

    storeOff += (FW_STORE_BR_IMAGE_INFO_SIGNATURE_LEN
                + FW_STORE_BR_IMAGE_INFO_SEG_CNT_FIELD_LEN);

    UC_FU_cntxt.segCnt = segCnt;

    for (idx=0; idx<segCnt; idx++)
    {
        UINT8_t off = 0, infoLen = FW_STORE_BR_SEG_SA_IN_EXEC_FIELD_SZ
                                   + FW_STORE_BR_SEG_LENGTH_FIELD_SZ
                                   + FW_STORE_BR_SEG_CRC_FIELD_SZ;

        if (M24M01_read(PLTFRM_M24M01_1_DEV_ID, storeOff,
                        infoLen,
                        buff) != PLTFRM_STS_SUCCESS)
            SYS_fatal(SYS_FATAL_ERR_3017);

        segInfo_p->segAddr = UTIL_ntohs(buff);
        off += FW_STORE_BR_SEG_SA_IN_EXEC_FIELD_SZ;
        segInfo_p->segLen =  UTIL_ntohs(buff + off);
        off += FW_STORE_BR_SEG_LENGTH_FIELD_SZ;
        segInfo_p->segCrc =  UTIL_ntohs(buff + off);

        if (UC_FU_cntxt.segList[idx].segLen == 0)
            return 0;

        segInfo_p->segRcvd = 1;

        // pageCnt, lastPageSz are not required when reflashing

        storeOff += infoLen;
        segInfo_p ++;
    }

    return 1;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void FU_getImageRcvdTS(UINT8_t imageId, UINT8_t *buff_p)
{
    if (M24M01_read(PLTFRM_M24M01_1_DEV_ID,
                     imageId == FW_STORE_IMAGE_1 ? \
                     FW_STORE_IMAGE_1_RCVD_TIME_STAMP_OFF : FW_STORE_IMAGE_2_RCVD_TIME_STAMP_OFF,
                     FW_STORE_IMAGE_RCVD_TIME_STAMP_LEN,
                     buff_p) != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_3022);

   return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void FU_getImageInfo(UINT8_t imageId, FU_imageInfo_s *imageInfo_p)
{
    imageInfo_p->len = 0;
    imageInfo_p->segCnt = 0;
    imageInfo_p->type = 0;

    if (FU_loadImageInfoFromStore(imageId))
    {
        UINT8_t idx;

        imageInfo_p->segCnt = UC_FU_cntxt.segCnt;

        for (idx=0; idx<UC_FU_cntxt.segCnt; idx++)
             imageInfo_p->len += UC_FU_cntxt.segList[idx].segLen;
    }

    return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
 void FU_updateFWStoreBootRecord(void)
 {
    UINT16_t len;
    UINT8_t bdHndl;

    len = FW_STORE_BR_IMAGE_INFO_SIGNATURE_LEN
          + FW_STORE_BR_IMAGE_INFO_SEG_CNT_FIELD_LEN
          + UC_FU_cntxt.segCnt * (FW_STORE_BR_IMAGE_INFO_SEG_ADDR
                                  + FW_STORE_BR_IMAGE_INFO_SEG_LEN
                                  + FW_STORE_BR_IMAGE_INFO_SEG_CRC);

    bdHndl = SYS_allocMem(len);
    if (SYS_BD_HNDL_IS_VALID(bdHndl))
    {
        UINT16_t sign;
        UINT8_t idx, brOff, imageFlags[FW_STORE_MAX_IMAGE_CNT];
        UINT8_t *start_p, *buff_p;

        buff_p = SYS_GET_BD_BUFF(bdHndl);
        start_p = buff_p;

        if (UC_FU_cntxt.upgradeableImage == FW_STORE_IMAGE_1)
        {
            sign = FW_STORE_BR_IMAGE_1_INFO_SIGNATURE;
            brOff = FW_STORE_BR_IMAGE_1_OFF;
        }
        else
        {
            sign = FW_STORE_BR_IMAGE_2_INFO_SIGNATURE;
            brOff = FW_STORE_BR_IMAGE_2_OFF;
        }

        buff_p = UTIL_htons(buff_p, sign);
        buff_p = UTIL_htons(buff_p, UC_FU_cntxt.segCnt);

        for (idx=0; idx<UC_FU_cntxt.segCnt; idx++)
        {
             buff_p = UTIL_htons(buff_p, UC_FU_cntxt.segList[idx].segAddr);
             buff_p = UTIL_htons(buff_p, UC_FU_cntxt.segList[idx].segLen);
             buff_p = UTIL_htons(buff_p, UC_FU_cntxt.segList[idx].segCrc);
        }

        // Write the info
        if (M24M01_write(PLTFRM_M24M01_1_DEV_ID,
                         brOff, buff_p - start_p, start_p) != PLTFRM_STS_SUCCESS)
            SYS_fatal(SYS_FATAL_ERR_3004);

        SYS_freeMem(bdHndl);

        FU_readImageStoreFlags(imageFlags);

        buff_p = UC_FU_cntxt.upgradeableImage == FW_STORE_IMAGE_1 ? &(imageFlags[0]) : &(imageFlags[1]);

        (*buff_p) &= ~(FW_STORE_BR_VALID_IMAGE_BIT
                       | FW_STORE_BR_ACTIVE_IMAGE_BIT);
        (*buff_p) |= FW_STORE_BR_UPDATE_IN_PROGRESS_BIT;

        FU_writeImageStoreFlags(imageFlags);

        if (M24M01_write(PLTFRM_M24M01_1_DEV_ID,
                         UC_FU_cntxt.upgradeableImage == FW_STORE_IMAGE_1 ? \
                         FW_STORE_IMAGE_1_RCVD_TIME_STAMP_OFF : FW_STORE_IMAGE_2_RCVD_TIME_STAMP_OFF,
                         FW_STORE_IMAGE_RCVD_TIME_STAMP_LEN,
                         UC_FU_cntxt.imageRcvdTSBuff) != PLTFRM_STS_SUCCESS)
            SYS_fatal(SYS_FATAL_ERR_3021);
    }
    else
        SYS_fatal(SYS_FATAL_ERR_3005);

    return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void UC_FU_sendResp(void)
{
   UINT8_t pktLen = ADP_ELEMENT_TYPE_LEN
                    + DIS_MSG_TYPE_SZ
                    + UTIL_TLV_HDR_SZ
                    + DIS_UC_FU_STATE_TLV_SZ
                    + DIS_UC_FU_RET_CODE_TLV_SZ;

   if (UC_FU_cntxt.state != UC_FU_STATE_IDLE)
       pktLen += (DIS_UC_FU_SEG_IDX_TLV_SZ
                  + DIS_UC_FU_PAGE_IDX_TLV_SZ
                  + DIS_UC_FU_PAGE_CRC_TLV_SZ);

   ADP_cntxt.txParams.bdHndl = SYS_allocMem(pktLen);
   if (SYS_BD_HNDL_IS_VALID(ADP_cntxt.txParams.bdHndl))
   {
       UINT8_t *buff_p, *tlvList_p;

       buff_p = SYS_GET_BD_BUFF(ADP_cntxt.txParams.bdHndl);
       buff_p += ADP_ELEMENT_TYPE_LEN;

       *buff_p = DIS_MSG_TYPE_NODE_UC_FU_MODULE_INFO;
       buff_p += DIS_MSG_TYPE_SZ;

       tlvList_p = buff_p;
       buff_p += UTIL_TLV_HDR_SZ;

       buff_p = UTIL_buildTlvHdr(buff_p,
                                 DIS_TLV_TYPE_UC_FU_STATE,
                                 DIS_UC_FU_STATE_TLV_SZ - UTIL_TLV_HDR_SZ);
       *(buff_p ++) = UC_FU_cntxt.state;

       buff_p = UTIL_buildTlvHdr(buff_p,
                                 DIS_TLV_TYPE_UC_FU_RET_CODE,
                                 DIS_UC_FU_RET_CODE_TLV_SZ - UTIL_TLV_HDR_SZ);
       *(buff_p ++) = UC_FU_cntxt.respRC;

       if (UC_FU_cntxt.state != UC_FU_STATE_IDLE)
       {
           buff_p = UTIL_buildTlvHdr(buff_p,
                                     DIS_TLV_TYPE_UC_FU_SEG_IDX,
                                     DIS_UC_FU_SEG_IDX_TLV_SZ - UTIL_TLV_HDR_SZ);
           *(buff_p ++) = UC_FU_cntxt.currSegIdx;

           buff_p = UTIL_buildTlvHdr(buff_p,
                                     DIS_TLV_TYPE_UC_FU_PAGE_IDX,
                                     DIS_UC_FU_PAGE_IDX_TLV_SZ - UTIL_TLV_HDR_SZ);
           buff_p = UTIL_htons(buff_p, UC_FU_cntxt.nextExpPageIdx);

           buff_p = UTIL_buildTlvHdr(buff_p,
                                     DIS_TLV_TYPE_UC_FU_PAGE_CRC,
                                     DIS_UC_FU_PAGE_CRC_TLV_SZ - UTIL_TLV_HDR_SZ);
           buff_p = UTIL_htons(buff_p, UC_FU_cntxt.lastPageCRC);
       }

       UTIL_buildTlvHdr(tlvList_p,
                        DIS_TLV_TYPE_NODE_UC_FU_MODULE_INFO,
                        buff_p - tlvList_p - UTIL_TLV_HDR_SZ);

       ADP_cntxt.txParams.destShortAddr = LPWMN_COORD_MAC_SHORT_ADDR;
       ADP_cntxt.txParams.srcModId = ADP_TX_REQ_SRC_UC_FU;

#ifdef UC_FU_STATS_ENA
       UC_FU_respTxCnt ++;
#endif
       ADP_appPyldTxReq();
   }
   else
   {
#ifdef UC_FU_STATS_ENA
       UC_FU_respAllocFlrCnt ++;
#endif
   }

   return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
UINT8_t FU_verifySegCRC(void)
{
   UINT32_t ckSum = 0, storeOff = FW_STORE_IMAGE_1_START_ADDR;
   const UINT16_t segIdx = UC_FU_cntxt.currSegIdx;
   UINT16_t idx, segLen = UC_FU_cntxt.segList[segIdx].segLen;

   // This takes a while, so switch off the radio
   RADIO_hardStop();

   if (UC_FU_cntxt.upgradeableImage == FW_STORE_IMAGE_2)
   {
       storeOff = M24M01_MEM_SIZE_KB;
       storeOff <<= 9;   // 128 KB / 2
   }

   for (idx=0; idx<segIdx; idx++)
   {
        storeOff += UC_FU_cntxt.segList[idx].segLen;
   }

   while (segLen > 1)
   {
      UINT8_t buff[2];
      UINT16_t tmp;

      if ((segLen & 0xf) == 0
          || (((segLen + 1) & 0xf) == 0))
      {
          PLTFRM_TOGGLE_LED_1();
          PLTFRM_TOGGLE_LED_2();
      }

      if (M24M01_read(PLTFRM_M24M01_1_DEV_ID, storeOff, 2, buff) != PLTFRM_STS_SUCCESS)
          SYS_fatal(SYS_FATAL_ERR_3007);

      tmp = buff[0];
      tmp = (tmp << 8) | buff[1];
      ckSum = ckSum + tmp;
      storeOff += 2;
      segLen -= 2;
   }


   if (segLen > 0)
   {
      UINT8_t buff;

      if (M24M01_read(PLTFRM_M24M01_1_DEV_ID, storeOff, 1, &buff) != PLTFRM_STS_SUCCESS)
          SYS_fatal(SYS_FATAL_ERR_3008);
      ckSum += buff;
   }

   while (ckSum >> 16)
   {
      ckSum = (ckSum & 0xffff) + (ckSum >> 16);
   }

   ckSum = ~ckSum;
   ckSum = ckSum & 0xffff;


   RADIO_flushRx( );
   RADIO_flushTx( );

   return (ckSum == UC_FU_cntxt.segList[segIdx].segCrc);
}




/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void FU_getImageStoreFlags(UINT8_t *buff_p)
{
   UINT8_t storeFlags[FW_STORE_MAX_IMAGE_CNT];

   memset(buff_p, 0, FU_IMAGE_STORE_IMAGE_FLAGS_ATTR_VAL_LEN);

   {
      UINT8_t idx;

      FU_readImageStoreFlags(storeFlags);

      for (idx=0; idx<FW_STORE_MAX_IMAGE_CNT; idx++)
      {
           buff_p[0] |= (1 << idx);   // Storage available

           if (storeFlags[idx] & FW_STORE_BR_VALID_IMAGE_BIT)
               buff_p[1] |= (1 << idx);

           if (storeFlags[idx] & FW_STORE_BR_ACTIVE_IMAGE_BIT)
               buff_p[2] |= (1 << idx);

           if (storeFlags[idx] & FW_STORE_BR_UPDATE_IN_PROGRESS_BIT)
               buff_p[3] |= (1 << idx);
      }
   }

   return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
UINT8_t FU_findUpgradeableImageLocn(UINT8_t *buff_p)
{
    UINT8_t flag = 0;

    if ((buff_p[0] & FW_STORE_BR_VALID_IMAGE_BIT)
        && (buff_p[0] & FW_STORE_BR_ACTIVE_IMAGE_BIT))
        flag |= 0x1;

    if ((buff_p[1] & FW_STORE_BR_VALID_IMAGE_BIT)
        && (buff_p[1] & FW_STORE_BR_ACTIVE_IMAGE_BIT))
        flag |= 0x2;

    UC_FU_cntxt.upgradeableImage = FW_STORE_IMAGE_NONE;

    if (flag == 0x3)
    {
        // Boot record is corrupted !!
        FU_resetStorageBootRecord();
        return 0;
    }
    else
    {
        switch (flag)
        {
           case 1:
                UC_FU_cntxt.upgradeableImage = FW_STORE_IMAGE_2;
                break;

           case 2:
                UC_FU_cntxt.upgradeableImage = FW_STORE_IMAGE_1;
                break;

           default:
                UC_FU_cntxt.upgradeableImage = FW_STORE_IMAGE_1;
                break;
        }
    }

    return 1;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void FU_init(void)
{
   memset(&UC_FU_cntxt, 0, sizeof(UC_FU_cntxt_s));

   UC_FU_cntxt.state = UC_FU_STATE_IDLE;

   // Read the EEPROM boot record.
   {
      PLTFRM_sts_t sts;

      UINT8_t buff[FW_STORE_MAX_IMAGE_CNT];

      sts = M24M01_read(PLTFRM_M24M01_1_DEV_ID,
                        FW_STORE_BR_SIGNATURE_OFF,
                        FW_STORE_BR_SIGNATURE_LEN,
                        buff);
      if (sts != PLTFRM_STS_SUCCESS)
          SYS_fatal(SYS_FATAL_ERR_3003);

      if (UTIL_ntohs((UINT8_t *)&buff) != FW_STORE_BR_SIGNATURE)
      {
          FU_resetStorageBootRecord();
          return;
      }

      FU_readImageStoreFlags(buff);

      FU_findUpgradeableImageLocn(buff);
   }

   return;
}

#endif


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void UC_FU_procRcvdMsg(UINT8_t *msg_p, UINT8_t msgLen)
{
   UINT8_t msgType = *msg_p;

   switch (msgType)
   {
#ifdef PLTFRM_M24M01_ENA

      case DIS_MSG_TYPE_SWITCH_TO_SBY_IMAGE:
           {
               UINT8_t validImgCnt = 0, activeImageCnt = 0,
                       brCorrupted = 0, activeImageId = FW_STORE_IMAGE_NONE,
                       imageFlags[FW_STORE_BR_FLAGS_LEN];

               msg_p ++;
               msgLen --;

               UC_FU_cntxt.respRC = UC_FU_RC_SUCCESS;

               if (UC_FU_cntxt.state == UC_FU_STATE_RECEIVING_IMAGE
                   || UC_FU_cntxt.state == UC_FU_STATE_READY_TO_SWITCH_TO_SBY_IMAGE)
               {
                   UC_FU_cntxt.respRC = UC_FU_RC_INV_REQUEST;
                   SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_UC_FU_COMM;
                   break;
               }

               FU_readImageStoreFlags(imageFlags);

               if (imageFlags[0] & FW_STORE_BR_VALID_IMAGE_BIT)
               {
                   validImgCnt ++;

                   if (imageFlags[0] & FW_STORE_BR_ACTIVE_IMAGE_BIT)
                   {
                       activeImageId = FW_STORE_IMAGE_1;
                       activeImageCnt ++;
                   }
               }

               if (imageFlags[1] & FW_STORE_BR_VALID_IMAGE_BIT)
               {
                   validImgCnt ++;

                   if (imageFlags[1] & FW_STORE_BR_ACTIVE_IMAGE_BIT)
                   {
                       activeImageId = FW_STORE_IMAGE_2;
                       activeImageCnt ++;
                   }
               }

               if (activeImageCnt > 1)
               {
                   // Can have only one active image at any point !!
                   brCorrupted = 1;
               }
               else
               {
                  if (validImgCnt < 2)
                  {
                      // Only one valid image in store !!
                      UC_FU_cntxt.respRC = UC_FU_RC_NO_STANDBY_IMAGE;
                  }
                  else
                  {
                      if (activeImageCnt == 0)
                      {
                          // Can't have > 0 valid images but no active images !!
                          brCorrupted = 1;
                      }
                      else
                      {
                          // 2 valid images and 1 active image.
                          if (FU_loadImageInfoFromStore(activeImageId == FW_STORE_IMAGE_1 ? FW_STORE_IMAGE_2 : FW_STORE_IMAGE_1))
                          {
                              UINT8_t segIdx;

                              for (segIdx=0; segIdx<UC_FU_cntxt.segCnt; segIdx++)
                              {
                                   UC_FU_cntxt.currSegIdx = segIdx;
                                   if (!(FU_verifySegCRC()))
                                   {
                                       brCorrupted = 1;
                                       break;
                                   }
                              }

                              if (brCorrupted == 0)
                              {
                            	  UC_FU_cntxt.state = UC_FU_STATE_READY_TO_SWITCH_TO_SBY_IMAGE;

#if 0
#ifdef DEV_TYPE_FFD
                                  UC_FU_cntxt.timeToReflash = UC_FU_REFLASH_WAIT_SECS;
#elif defined(DEV_TYPE_RFD)
                                  SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_IMAGE_REFLASH_PENDING;

                                  if (PLTFRM_startTimerA1(UC_FU_REFLASH_WAIT_SECS, 0, FU_reflashTmoCbFn) != PLTFRM_STS_SUCCESS)
                                      SYS_fatal(SYS_FATAL_ERR_3023);
#endif
#else
                                  UC_FU_cntxt.timeToReflash = 1;
                                  FU_checkStartReflash();
                                  break;
#endif

                              }
                          }
                          else
                              brCorrupted = 1;
                      }
                  }
               }

               if (brCorrupted)
               {
                   FU_resetStorageBootRecord();
                   UC_FU_cntxt.respRC = UC_FU_RC_STORE_IMAGE_INFO_CORRUPT;
               }

               SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_UC_FU_COMM;
           }
           break;

      case DIS_MSG_TYPE_UC_FU_IMAGE_PAGE:
           {
              msg_p ++;
              msgLen --;

              if (UC_FU_cntxt.state != UC_FU_STATE_RECEIVING_IMAGE
                  && UC_FU_cntxt.state != UC_FU_STATE_INIT_RESP_SENT)
              {
                  SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_UC_FU_COMM;
                  break;
              }

              // First two bytes have the page index and segment index
              // Rest of the bytes contain "page sz" number of bytes of
              // page data (except for the last page in a segment).

              if (msgLen >= UC_FU_PAGE_MSG_SEG_INFO_FIELD_SZ)
              {
                  UINT16_t pageIdx;
                  UINT8_t segIdx;

                  pageIdx = UTIL_ntohs(msg_p);
                  segIdx = (pageIdx >> UC_FU_PAGE_MSG_SEG_IDX_FIELD_SHIFT) & UC_FU_PAGE_MSG_SEG_IDX_FIELD_BM;
                  pageIdx &= ~UC_FU_PAGE_MSG_SEG_IDX_FIELD_BM_SHIFTED;

                  if (segIdx == UC_FU_cntxt.currSegIdx)
                  {
                      UC_FU_segInfo_s *seg_p = &(UC_FU_cntxt.segList[UC_FU_cntxt.currSegIdx]);

                      if (pageIdx == UC_FU_cntxt.nextExpPageIdx
                          || ((UC_FU_cntxt.nextExpPageIdx > 0) && (pageIdx == UC_FU_cntxt.nextExpPageIdx - 1)))
                      {
                          UINT8_t thisPageLen;

#ifdef UC_FU_STATS_ENA
                          UC_FU_correctPageRcvdCnt ++;
#endif

                          msg_p += UC_FU_PAGE_MSG_SEG_INFO_FIELD_SZ;
                          msgLen -= UC_FU_PAGE_MSG_SEG_INFO_FIELD_SZ;

                          if (pageIdx == (seg_p->pageCnt - 1))
                              thisPageLen = seg_p->lastPageSz;
                          else
                              thisPageLen = UC_FU_cntxt.pageSz;

                          if (msgLen == thisPageLen)
                          {
                              UINT32_t currStoreOff = pageIdx;
                              currStoreOff *= UC_FU_cntxt.pageSz;

                              currStoreOff += UC_FU_cntxt.imageBase;

                              {
                                 UINT8_t idx;
                                 for (idx=0; idx<UC_FU_cntxt.currSegIdx; idx++)
                                     currStoreOff += UC_FU_cntxt.segList[idx].segLen;
                              }

                              FU_savePageToStore(currStoreOff, msg_p, msgLen);
                              memset(msg_p, 0x0, msgLen);
                              FU_readPageFromStore(currStoreOff, msg_p, msgLen);

                              UC_FU_cntxt.lastPageCRC = PLTFRM_calcCkSum16(msg_p, msgLen);

                              UC_FU_cntxt.nextExpPageIdx = pageIdx + 1;

                              if (UC_FU_cntxt.state == UC_FU_STATE_INIT_RESP_SENT)
                                  UC_FU_cntxt.state = UC_FU_STATE_RECEIVING_IMAGE;

                              UC_FU_cntxt.respRC = UC_FU_RC_SUCCESS;

                              if (pageIdx == (seg_p->pageCnt - 1))
                              {
                                  // Last page of this segment received ...

                                  if (!(FU_verifySegCRC()))
                                  {
                                      UC_FU_cntxt.nextExpPageIdx = 0;
                                      UC_FU_cntxt.respRC = UC_FU_RC_SEG_CRC_MISMATCH;
                                  }
                                  else
                                  {
                                      seg_p->segRcvd = 1;

                                      if (UC_FU_cntxt.currSegIdx >= (UC_FU_cntxt.segCnt - 1))
                                      {
#if 1
                                          // Complete image received ...
                                          UC_FU_cntxt.state = UC_FU_STATE_RECEIVED_IMAGE;
                                          UC_FU_cntxt.timeToReflash = UC_FU_REFLASH_WAIT_SECS;
#else
                                          UC_FU_cntxt.timeToReflash = 1;
                                          FU_checkStartReflash();
                                          break;
#endif
                                      }
                                      else
                                      {
                                          UC_FU_cntxt.currSegIdx ++;
                                          UC_FU_cntxt.nextExpPageIdx = 0;
                                      }
                                  }
                              }

                              SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_UC_FU_COMM;
                          }
                      }
                      else
                      {
#ifdef UC_FU_STATS_ENA
                          UC_FU_wrongPageRcvdCnt ++;
#endif
                          // Send response to source indicating the next expected seg idx + page index
                          SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_UC_FU_COMM;
                          UC_FU_cntxt.respRC = UC_FU_RC_WRONG_PAGE_RCVD;
                      }
                  }
                  else
                  {
                      // Send response to source indicating the next expected seg idx + page index
                      SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_UC_FU_COMM;
                      UC_FU_cntxt.respRC = UC_FU_RC_WRONG_PAGE_RCVD;
                  }
              }
           }
           break;
#endif

      case DIS_MSG_TYPE_UC_FU_INIT_2:
      case DIS_MSG_TYPE_UC_FU_INIT_1:
           {
              UINT8_t tlvLen1, rc = UC_FU_RC_SUCCESS;
              UINT8_t *buff1_p, segCnt = 0;

              if (msgType == DIS_MSG_TYPE_UC_FU_INIT_2)
              {
                 if (UC_FU_cntxt.state != UC_FU_STATE_WAIT_INIT_2)
                     break;
              }

#ifndef PLTFRM_M24M01_ENA
              rc = UC_FU_RC_NO_STORAGE;
#else
              msg_p ++;
              msgLen --;

              do
              {
#if 0
                 if (UC_FU_cntxt.state != UC_FU_STATE_IDLE)
                 {
                     rc = UC_FU_RC_INV_REQUEST;
                     break;
                 }
#endif

                 if (msgType == DIS_MSG_TYPE_UC_FU_INIT_1)
                 {
                     UC_FU_cntxt.segCnt = 0;
                     UC_FU_cntxt.currSegIdx = 0;
                     UC_FU_cntxt.nextExpPageIdx = 0;
                 }

                 if (UTIL_getTlv(msg_p, msgLen, DIS_TLV_TYPE_UC_FU_SESSION_PARAMS, &tlvLen1, &buff1_p))
                 {
                     UINT8_t tlvLen2, *buff2_p;

                     if (UTIL_getTlv(buff1_p, tlvLen1, DIS_TLV_TYPE_FU_PAGE_SZ, &tlvLen2, &buff2_p))
                     {
                         if (tlvLen2 == DIS_FU_PAGE_SZ_FIELD_LEN)
                             UC_FU_cntxt.pageSz = *(buff2_p);
                         else
                         {
                             rc = UC_FU_RC_MALFORMED_INIT_REQ;
                             break;
                         }
                     }
                     else
                     {
                         if (msgType == DIS_MSG_TYPE_UC_FU_INIT_1)
                         {
                             rc = UC_FU_RC_MALFORMED_INIT_REQ;
                             break;
                         }
                     }

                     if (UTIL_getTlv(buff1_p, tlvLen1, DIS_TLV_TYPE_TIME_STAMP, &tlvLen2, &buff2_p))
                     {
                         if (tlvLen2 == FW_STORE_IMAGE_RCVD_TIME_STAMP_LEN)
                             memcpy(UC_FU_cntxt.imageRcvdTSBuff, buff2_p, FW_STORE_IMAGE_RCVD_TIME_STAMP_LEN);
                         else
                         {
                             rc = UC_FU_RC_MALFORMED_INIT_REQ;
                             break;
                         }
                     }
                     else
                     {
                         if (msgType == DIS_MSG_TYPE_UC_FU_INIT_1)
                         {
                             rc = UC_FU_RC_MALFORMED_INIT_REQ;
                             break;
                         }
                     }

                     if (UTIL_getTlv(buff1_p, tlvLen1, DIS_TLV_TYPE_FW_SEG_LIST, &tlvLen2, &buff2_p))
                     {
                         UINT8_t tlvLen3, *buff3_p;

                         while (UTIL_getTlv(buff2_p, tlvLen2, DIS_TLV_TYPE_FW_SEG_INFO, &tlvLen3, &buff3_p))
                         {
                             UINT8_t tlvLen4, *buff4_p;
                             UC_FU_segInfo_s *seg_p = &(UC_FU_cntxt.segList[segCnt]);

                             if (UTIL_getTlv(buff3_p, tlvLen3, DIS_TLV_TYPE_FW_SEG_START, &tlvLen4, &buff4_p))
                                 seg_p->segAddr = UTIL_ntohs(buff4_p);
                             else
                             {
                                 if (msgType == DIS_MSG_TYPE_UC_FU_INIT_1)
                                 {
                                     rc = UC_FU_RC_MALFORMED_INIT_REQ;
                                     break;
                                 }
                             }

                             if (UTIL_getTlv(buff3_p, tlvLen3, DIS_TLV_TYPE_FW_SEG_LEN, &tlvLen4, &buff4_p))
                                 seg_p->segLen = UTIL_ntohs(buff4_p);
                             else
                             {
                                 if (msgType == DIS_MSG_TYPE_UC_FU_INIT_2)
                                 {
                                     rc = UC_FU_RC_MALFORMED_INIT_REQ;
                                     break;
                                 }
                             }

                             if (UTIL_getTlv(buff3_p, tlvLen3, DIS_TLV_TYPE_FW_SEG_CRC, &tlvLen4, &buff4_p))
                                 seg_p->segCrc = UTIL_ntohs(buff4_p);
                             else
                             {
                                 if (msgType == DIS_MSG_TYPE_UC_FU_INIT_2)
                                 {
                                     rc = UC_FU_RC_MALFORMED_INIT_REQ;
                                     break;
                                 }
                             }

                             if (msgType == DIS_MSG_TYPE_UC_FU_INIT_2)
                             {
                                 seg_p->segRcvd = 0;
                                 seg_p->pageCnt = seg_p->segLen;
                                 seg_p->pageCnt /= UC_FU_cntxt.pageSz;
                                 seg_p->lastPageSz = seg_p->segLen % UC_FU_cntxt.pageSz;
                                 if (seg_p->lastPageSz)
                                     seg_p->pageCnt ++;
                                 else
                                     seg_p->lastPageSz = UC_FU_cntxt.pageSz;
                             }

                             segCnt ++;
                             tlvLen2 = ((buff2_p + tlvLen2) - (buff3_p + tlvLen3));
                             buff2_p = (buff3_p + tlvLen3);
                         }

                         if (segCnt == 0
                             || (msgType == DIS_MSG_TYPE_UC_FU_INIT_2 && segCnt != UC_FU_cntxt.segCnt))
                         {
                             rc = UC_FU_RC_MALFORMED_INIT_REQ;
                             break;
                         }
                         else
                             UC_FU_cntxt.segCnt = segCnt;
                     }
                     else
                     {
                         rc = UC_FU_RC_MALFORMED_INIT_REQ;
                         break;
                     }
                 }
                 else
                 {
                     rc = UC_FU_RC_MALFORMED_INIT_REQ;
                     break;
                 }
              } while (0);

              if (rc == UC_FU_RC_SUCCESS)
              {
                  if (msgType == DIS_MSG_TYPE_UC_FU_INIT_2)
                  {
                      UC_FU_cntxt.state = UC_FU_STATE_INIT_RESP_SENT;

                      if (UC_FU_cntxt.upgradeableImage == FW_STORE_IMAGE_1)
                          UC_FU_cntxt.imageBase = FW_STORE_IMAGE_1_START_ADDR;
                      else
                      {
                          UC_FU_cntxt.imageBase = M24M01_MEM_SIZE_KB;
                          UC_FU_cntxt.imageBase <<= 9;   // 128 KB / 2
                      }
                      FU_updateFWStoreBootRecord();
                      UC_FU_wrongPageRcvdCnt = 0;
                      UC_FU_correctPageRcvdCnt = 0;
#ifdef DEV_TYPE_FFD
                      FFD_APP_stopPeriodicPush();
#endif
                  }
                  else
                      UC_FU_cntxt.state = UC_FU_STATE_WAIT_INIT_2;
              }
#endif

              UC_FU_cntxt.respRC = rc;
              SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_UC_FU_COMM;
           }
           break;

      default:
           break;
   }

   return;
}


#endif
