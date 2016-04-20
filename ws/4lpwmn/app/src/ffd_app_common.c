/*
 * File Name: ffd_app_1.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Nov/28/2014
 *
 *
 * Copyright (c) <2014>, <ram krishnan>
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

#ifdef DEV_TYPE_FFD

#include <string.h>
#include <typedefs.h>
#include <system.h>
#include <pltfrm.h>
#include <util.h>
#include <mac_pib.h>
#include <adp.h>
#include <dis.h>
#include <ffd_app_common.h>
#include <aodvl.h>
#include <node_mgr.h>
#include <nvm.h>
#include <fu.h>

#ifdef PLTFRM_LM75B_TEMP_SENSOR_ENA
#include <lm75b.h>
#endif

#ifdef PLTFRM_TSL45315_ENA
#include <tsl45315.h>
#endif

UINT8_t FFD_APP_periodicPushStopped = 0;

#ifndef NODE_HOST_IF_ENA
UINT32_t FFD_APP_commIntervalInSecs = 5;


UINT8_t __attrValBuff[NM_MAX_ATTR_VAL_LEN];

UINT16_t FFD_APP_noOpReqRcvdCnt = 0;

UINT8_t APP_nodeJoinDoneIndCnt = 0;

UINT32_t FFD_APP_dataTxReqCnt = 0;
UINT32_t FFD_APP_dataRxReqFlrCnt = 0;
UINT32_t FFD_APP_cnfrmOkCnt = 0;
UINT32_t FFD_APP_cnfrmFlrCnt = 0;
UINT16_t FFD_APP_memAllocFlrCnt = 0;
UINT16_t FFD_APP_dataIndCnt = 0;
UINT16_t FFD_APP_dataIndDropCnt = 0;

UINT8_t APP_nodeJoined = 0;
UINT32_t APP_txInterval = 0;

UINT8_t APP_pendingTxMsgHndl = SYS_INV_BD_HDNL;
UINT16_t APP_pendingTxMsgDestAddr = MAC_BROADCAST_SHORT_ADDR;
UINT8_t FFD_appRespTxFlag = 0, FFD_snsrDataTxPendFlag = 0;


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void APP_nodeJoinDoneInd(void)
{
   APP_nodeJoinDoneIndCnt ++;
   APP_nodeJoined = 1;
#ifdef FFD_APP_SEND_DATA_BACK_TO_BACK
   SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_APP_DATA_REQ;
   FFD_snsrDataTxPendFlag = 1;
#else
   APP_txInterval = FFD_APP_commIntervalInSecs;
#endif
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void FFD_APP_stopPeriodicPush(void)
{
   FFD_APP_periodicPushStopped = 1;
}


#ifdef __TEST_DUMMY_ASSOC__
UINT8_t APP_sendDummyAssocReq = 0;
UINT8_t APP_dummyAssocReqExtAddr[ ] = {0x11, 0x13, 0x15, 0x24, 0x26, 0x28, 0x30, 0x32};
#endif

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void FFD_APP_periodicTmrExpCbFunc(void)
{
   if (APP_nodeJoined == 0
       || FFD_APP_periodicPushStopped)
       return;

#ifdef FFD_APP_SEND_DATA_BACK_TO_BACK
   return;
#endif

#ifdef __TEST_DUMMY_ASSOC__
   if (APP_sendDummyAssocReq)
   {
       APP_sendDummyAssocReq = 0;
       ADP_macAssocIndHndlr(APP_dummyAssocReqExtAddr, 0x0);
   }
#endif

   if (APP_txInterval > 0)
   {
       APP_txInterval --;
       if (APP_txInterval > 0)
           return;
   }
   else
       return;

   /*
    * Bug fix - Dec/17/2014
    * Cannot call ADP API directly since ADP will call MCPS_dataReq(). Consider the
    * scenario in which FFD has forwarded a mesh packet to the coordinator and is
    * waiting for ACK back from the coord or next hop to coord. Meanwhile the periodic
    * APP timer expires on the FFD in response to which FFD attempts to send a APP data
    * packet to the coordinator. At this point the MAC "tx" is MAC_TX_MOD_STATE_WAIT_ACK"
    * so MCPS_dataReq() will assert if it is called now.
    * The fix is to set a system flag.
    */

   /*
    * Bug fix - Aug/13/2015
    * If sensor data cannot be collected in one shot (if sensor requires time to convert),
    * then MAC might end up forwarding a packet mean while app layer collects sensor
    * data and then requests MAC to transmit even though MAC has not finished with the
    * packet being forwarded. Scenario - RFD sending to FFD and FFD itself sends sensor data.
    */
   FFD_APP_collectSensorData();

   return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void FFD_APP_procPendDataReq(void)
{
   if (FFD_snsrDataTxPendFlag)
   {
       FFD_snsrDataTxPendFlag = 0;
       FFD_APP_buildSendSnsrUpdate();
   }
   else
   {
       if (FFD_appRespTxFlag)
       {
           FFD_appRespTxFlag = 0;
           if (!(SYS_BD_HNDL_IS_VALID(APP_pendingTxMsgHndl))
               || APP_pendingTxMsgDestAddr == MAC_BROADCAST_SHORT_ADDR)
           {
               SYS_fatal(SYS_FATAL_ERR_481);
           }
           else
           {
               ADP_cntxt.txParams.destShortAddr = APP_pendingTxMsgDestAddr;
               ADP_cntxt.txParams.bdHndl = APP_pendingTxMsgHndl;
               ADP_cntxt.txParams.srcModId = ADP_TX_REQ_SRC_APP;

               APP_pendingTxMsgHndl = SYS_INV_BD_HDNL;
               APP_pendingTxMsgDestAddr = MAC_BROADCAST_SHORT_ADDR;

               if (ADP_appPyldTxReq() != ADP_STS_SUCCESS)
               {
                   FFD_APP_dataRxReqFlrCnt ++;
               }
           }
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
void FFD_APP_COMMON_readCfgInfo(void)
{
   UINT8_t tempBd = SYS_allocMem(NVM_APP_CFG_DATA_LEN);

   if (SYS_BD_HNDL_IS_VALID(tempBd))
   {
       PLTFRM_sts_t sts;
       UINT8_t *buff_p = SYS_GET_BD_BUFF(tempBd);

       sts = NVM_readMem(PLTFRM_NVM_DEV_ID,
                         NVM_APP_CFG_DATA_OFFSET,
                         NVM_APP_CFG_DATA_LEN,
                         buff_p);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           UINT8_t tlvLen1, rc, *buff1_p;

           rc = UTIL_getTlv(buff_p, NVM_APP_CFG_DATA_LEN, APP_TLV_TYPE_CFG_DATA, &tlvLen1, &buff1_p);
           if (rc && (tlvLen1 > 0) && (tlvLen1 <= (NVM_APP_CFG_DATA_LEN - APP_TLV_HDR_SZ)))
           {
               UINT8_t tlvLen2, *buff2_p;

               rc = UTIL_getTlv(buff1_p, tlvLen1, APP_TLV_TYPE_DATA_PUSH_INTERVAL, &tlvLen2, &buff2_p);
               if (rc)
               {
                   if (tlvLen2 == APP_DATA_PUSH_INTERVAL_VALUE_FIELD_SZ)
                   {
                	   FFD_APP_commIntervalInSecs = UTIL_ntohl(buff2_p);
                   }
               }
           }
           else
           {
               UINT8_t *buff1_p = buff_p;

               // Initialize the EEPROM ...
               *(buff1_p ++) = APP_TLV_TYPE_CFG_DATA;
               *(buff1_p ++) = APP_TLV_HDR_SZ + APP_DATA_PUSH_INTERVAL_VALUE_FIELD_SZ;
               *(buff1_p ++) = APP_TLV_TYPE_DATA_PUSH_INTERVAL;
               *(buff1_p ++) = APP_DATA_PUSH_INTERVAL_VALUE_FIELD_SZ;
               buff1_p = UTIL_htonl(buff1_p, FFD_APP_commIntervalInSecs);

               sts = NVM_writeMem(PLTFRM_NVM_DEV_ID,
                                  NVM_APP_CFG_DATA_OFFSET,
                                  buff1_p - buff_p,
                                  buff_p);
               if (sts != PLTFRM_STS_SUCCESS)
                   SYS_fatal(SYS_FATAL_ERR_900);
           }
       }

       SYS_freeMem(tempBd);
   }
   else
   {
      SYS_fatal(SYS_FATAL_ERR_901);
   }
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void FFD_APP_COMMON_updateCfgInfo(UINT8_t tlvId,
                                  UINT8_t tlvValLen,
                                  UINT8_t *val_p)
{
   UINT8_t tempBd = SYS_allocMem(NVM_APP_CFG_DATA_LEN);

   if (SYS_BD_HNDL_IS_VALID(tempBd))
   {
       PLTFRM_sts_t sts;
       UINT8_t *buff_p = SYS_GET_BD_BUFF(tempBd);

       sts = NVM_readMem(PLTFRM_NVM_DEV_ID,
                         NVM_APP_CFG_DATA_OFFSET,
                         NVM_APP_CFG_DATA_LEN,
                         buff_p);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           UINT8_t tlvLen1, rc, *buff1_p;

           rc = UTIL_getTlv(buff_p, NVM_APP_CFG_DATA_LEN, APP_TLV_TYPE_CFG_DATA, &tlvLen1, &buff1_p);
           if (rc)
           {
               UINT8_t tlvLen2, *buff2_p;

               rc = UTIL_getTlv(buff1_p, tlvLen1, APP_TLV_TYPE_DATA_PUSH_INTERVAL, &tlvLen2, &buff2_p);
               if (rc)
               {
                   if (tlvLen2 == tlvValLen)
                   {
                       switch (tlvValLen)
                       {
                          case 4:
                              UTIL_htonl(buff2_p, *((UINT32_t *)val_p));
                              break;

                          case 2:
                              UTIL_htons(buff2_p, *((UINT16_t *)val_p));
                              break;

                          default:
                              memcpy(buff2_p, val_p, tlvValLen);
                              break;
                       }

                       sts = NVM_writeMem(PLTFRM_NVM_DEV_ID,
                                          NVM_APP_CFG_DATA_OFFSET + (buff2_p - buff_p),
                                          tlvValLen, buff2_p);
                       if (sts != PLTFRM_STS_SUCCESS)
                           SYS_fatal(SYS_FATAL_ERR_898);
                   }
               }
           }
       }

       SYS_freeMem(tempBd);
   }
   else
   {
      SYS_fatal(SYS_FATAL_ERR_899);
   }
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void FFD_APP_rcvdMsgHndlr(UINT8_t bdHndl, UINT16_t srcAddr)
{
   UINT8_t msgLen = SYS_GET_BD_LEN(bdHndl);
   UINT8_t *msg_p;
   UINT16_t offset = 0;

   if (msgLen >= DIS_MSG_TYPE_SZ)
   {
       msg_p = SYS_GET_BD_BUFF(bdHndl);
       msgLen --;

       switch (msg_p[offset])
       {
#ifdef RFD_KA_ENA
          case DIS_MSG_TYPE_RFD_KEEP_ALIVE_MSG:
               {
                  // rkris@wisense.in / june/11/15
                  MAC_procRFDKAMsg(srcAddr);
               }
               break;
#endif

#ifdef UC_FU_ENA
          case DIS_MSG_TYPE_UC_FU_INIT_1:
          case DIS_MSG_TYPE_UC_FU_INIT_2:
          case DIS_MSG_TYPE_UC_FU_IMAGE_PAGE:
          case DIS_MSG_TYPE_SWITCH_TO_SBY_IMAGE:
               {
            	  UC_FU_procRcvdMsg(msg_p, msgLen + 1);
               }
               break;
#endif
          case DIS_MSG_TYPE_NO_OP:
               {
                  FFD_APP_noOpReqRcvdCnt ++;
               }
               break;

          case DIS_MSG_TYPE_DELAYED_REBOOT_NODE_REQ:
               {
                  // rkris@wisense.in / april/20/15

                  if (msgLen == DIS_REBOOT_DELAY_TLV_SZ)
                  {
                      offset ++;
                      if (msg_p[offset] == DIS_TLV_TYPE_REBOOT_DELAY)
                      {
                          offset ++;
                          if (msg_p[offset] == (DIS_REBOOT_DELAY_TLV_SZ - DIS_TLV_HDR_SZ))
                          {
                              offset ++;
                              SYS_rebootDelaySecs = UTIL_ntohs(msg_p + offset);
                          }
                      }
                  }
               }
               break;

          case DIS_MSG_TYPE_REBOOT_NODE_REQ:
               {
                  // rkris@wisense.in / april/20/15
                  PLTFRM_saveSysFEId(SYS_FATAL_ERR_REBOOT_ON_REQ);
                  PLTFRM_mcuReset();
               }
               break;

          case DIS_MSG_TYPE_CFG_NODE_DROP_BCN_REQS:
        	   MAC_cfgDropBcnReqs();
               break;

          case DIS_MSG_TYPE_CFG_NODE_ACCEPT_BCN_REQS:
        	   MAC_cfgAcceptBcnReqs();
               break;

          case DIS_MSG_TYPE_GET_GPIO_PORT_OP_STATE:
               {
                   if (!(SYS_BD_HNDL_IS_VALID(APP_pendingTxMsgHndl)))
                   {
                       UINT8_t pktLen = ADP_ELEMENT_TYPE_LEN
                                        + DIS_MSG_TYPE_SZ
                                        + DIS_TLV_HDR_SZ;  // port list TLV
#ifdef __MSP430G2955__
                       pktLen += (DIS_TLV_HDR_SZ
                                  + DIS_GPIO_PORT_ID_TLV_SZ
                                  + DIS_GPIO_PORT_OP_STATE_TLV_SZ)*(PLTFRM_GPIO_PORT_COUNT);
#else
#error micro not supported !!
#endif
                       APP_pendingTxMsgHndl = SYS_allocMem(pktLen);
                       if (SYS_BD_HNDL_IS_VALID(APP_pendingTxMsgHndl))
                       {
                           UINT8_t portId, *buff_p = SYS_GET_BD_BUFF(APP_pendingTxMsgHndl), *tlv_p;

                           buff_p += ADP_ELEMENT_TYPE_LEN;
                           *buff_p = DIS_MSG_TYPE_GET_GPIO_PORT_OP_STATE;
                           buff_p += DIS_MSG_TYPE_SZ;
                           tlv_p = buff_p;
                           buff_p += DIS_TLV_HDR_SZ;  // List

                           for (portId=PLTFRM_GPIO_PORT_1; portId<=PLTFRM_GPIO_PORT_4; portId++)
                           {
                                buff_p = UTIL_buildTlvHdr(buff_p, DIS_TLV_TYPE_GPIO_PORT_INFO,
                                                          DIS_GPIO_PORT_ID_TLV_SZ + DIS_GPIO_PORT_OP_STATE_TLV_SZ);
                                buff_p = UTIL_buildTlvHdr(buff_p, DIS_TLV_TYPE_GPIO_PORT_ID, 1);
                                *(buff_p ++) = portId;
                                buff_p = UTIL_buildTlvHdr(buff_p, DIS_TLV_TYPE_GPIO_PORT_OP_STATE, 1);
                                PLTFRM_getGPIOPortOp(portId, buff_p);
                                buff_p ++;
                           }

                           buff_p = UTIL_buildTlvHdr(tlv_p,
                                                     DIS_TLV_TYPE_GPIO_PORT_INFO_LIST,
                                                     buff_p - tlv_p - DIS_TLV_HDR_SZ);

                           APP_pendingTxMsgDestAddr = srcAddr;

                           FFD_appRespTxFlag = 1;

                           // Can't send packet to ADP/MAC right now since MAC will be busy acking
                           // this received message.

                           SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_APP_DATA_REQ;
                       }
                   }
               }
               break;

          case DIS_MSG_TYPE_CTRL_DIGITAL_IO:
               {
                  offset ++;
                  if (msgLen == DIS_GPIO_CTRL_TLV_SZ)
                  {
                      if (msg_p[offset++] == DIS_TLV_TYPE_GPIO_CTRL)
                      {
                          UINT8_t buffLen = msg_p[offset ++];
                          UINT8_t *buff_p = msg_p + offset;

                          if (buffLen == (DIS_GPIO_CTRL_TLV_SZ - DIS_TLV_HDR_SZ))
                          {
                              UINT8_t portId, pinNr, opVal, rc, tlvLen1;
                              UINT8_t *buff1_p;

                              rc = UTIL_getTlv(buff_p, buffLen, DIS_TLV_TYPE_GPIO_PORT_ID, &tlvLen1, &buff1_p);
                              if (rc)
                              {
                                  if (tlvLen1 == DIS_GPIO_PORT_ID_FIELD_SZ)
                                  {
                                      portId = *buff1_p;
                                      if (portId == 0x0)
                                          break;
                                      else
                                      {
                                          portId --;
                                          if (portId > PLTFRM_GPIO_PORT_4)
                                              break;
                                      }
                                  }
                                  else
                                      break;
                              }
                              else
                                 break;


                              rc = UTIL_getTlv(buff_p, buffLen, DIS_TLV_TYPE_GPIO_PIN_ID, &tlvLen1, &buff1_p);
                              if (rc)
                              {
                                  if (tlvLen1 == DIS_GPIO_PIN_ID_FIELD_SZ)
                                  {
                                      pinNr = *buff1_p;
                                      if (pinNr > PLTFRM_GPIO_PORT_PIN_7)
                                          break;
                                  }
                                  else
                                      break;
                              }
                              else
                                  break;

                              rc= UTIL_getTlv(buff_p, buffLen, DIS_TLV_TYPE_GPIO_VAL, &tlvLen1, &buff1_p);
                              if (rc)
                              {
                                  if (tlvLen1 == DIS_GPIO_VAL_FIELD_SZ)
                                  {
                                      opVal = *buff1_p;
                                      if (opVal > 1)
                                          break;
                                  }
                                  else
                                      break;
                              }
                              else
                                  break;

                              if (PLTFRM_getGPIOPinDevId(portId, pinNr) == PLTFRM_GPIO_REMOTE_CTRL_DEV_ID)
                              {
                                  PLTFRM_sts_t sts = PLTFRM_setGPIOPinOutput(portId, pinNr, opVal);
                                  if (sts != PLTFRM_STS_SUCCESS)
                                  {
                                      SYS_fatal(SYS_FATAL_ERR_545);
                                  }
                              }
                          }
                      }
                  }
              }
              break;

          case DIS_MSG_TYPE_SET_ATTR_VAL:
               {
                  if (msgLen >= DIS_TLV_HDR_SZ + DIS_ATTR_ID_TLV_SZ + DIS_TLV_HDR_SZ)
                  {
                       offset ++;
                       if (msg_p[offset] == DIS_TLV_TYPE_ATTR_INFO)
                       {
                           offset ++;
                           if (msg_p[offset] >= DIS_ATTR_ID_TLV_SZ + DIS_TLV_HDR_SZ)
                           {
                               UINT8_t tlvLen1;
                               UINT8_t *tlvBuff_p;

                               offset ++;
                               if (UTIL_getTlv(msg_p + offset, msgLen - DIS_TLV_HDR_SZ,
                                               DIS_TLV_TYPE_ATTR_ID, &tlvLen1, &tlvBuff_p))
                               {
                                   UINT16_t attrId;

                                   if (tlvLen1 == DIS_ATTR_ID_FIELD_SZ)
                                   {
                                       UINT8_t attrValLen;

                                       attrId = UTIL_ntohs(tlvBuff_p);
                                       attrValLen = NM_getAttrValSz(attrId);
                                       if (attrValLen > 0)
                                       {
                                           if (UTIL_getTlv(msg_p + offset, msgLen - DIS_TLV_HDR_SZ,
                                                           DIS_TLV_TYPE_ATTR_VAL, &tlvLen1, &tlvBuff_p))
                                           {
                                               if ((tlvLen1 == attrValLen)
                                                   && (msgLen >= DIS_TLV_HDR_SZ
                                                                 + DIS_ATTR_ID_TLV_SZ
                                                                 + DIS_TLV_HDR_SZ
                                                                 + attrValLen))
                                               {
                                                   NM_setAttrVal(attrId, tlvBuff_p);
                                               }
                                           }
                                       }
                                   }
                               }
                           }
                       }
                  }
               }
               break;

          case DIS_MSG_TYPE_GET_LIST_ATTR_VAL:
               {
                   if (msgLen >= (DIS_LIST_ATTR_ID_TLV_SZ + DIS_LIST_ATTR_IDX_TLV_SZ + DIS_TLV_HDR_SZ))
                   {
                       if (msg_p[++offset] == DIS_TLV_TYPE_LIST_ATTR_INFO
                           && msg_p[++offset] >= (DIS_LIST_ATTR_ID_TLV_SZ + DIS_LIST_ATTR_IDX_TLV_SZ))
                       {
                           UINT8_t tlvLen1;
                           UINT8_t *tlvBuff_p;

                           offset ++;
                           if (UTIL_getTlv(msg_p + offset, msgLen - DIS_TLV_HDR_SZ,
                                           DIS_TLV_TYPE_LIST_ATTR_ID, &tlvLen1, &tlvBuff_p))
                           {
                               if (tlvLen1 == DIS_LIST_ATTR_ID_FIELD_SZ)
                               {
                                   UINT16_t listAttrId = UTIL_ntohs(tlvBuff_p);

                                   if (UTIL_getTlv(msg_p + offset, msgLen - DIS_TLV_HDR_SZ,
                                                   DIS_TLV_TYPE_LIST_ATTR_IDX, &tlvLen1, &tlvBuff_p))
                                   {
                                       UINT16_t listAttrIdx = UTIL_ntohs(tlvBuff_p), listAttrValLen;

                                       listAttrValLen = NM_getAttrValSz(listAttrId);
                                       if (listAttrValLen > 0)
                                 	       NM_getListAttrVal(listAttrId, listAttrIdx, __attrValBuff);
                                       else
                                       {
                                           listAttrValLen = 1;
                                           __attrValBuff[0] = 0;
                                       }

                                      if (!(SYS_BD_HNDL_IS_VALID(APP_pendingTxMsgHndl)))
                                      {
                                          const UINT8_t pktLen = ADP_ELEMENT_TYPE_LEN
                                                                 + DIS_MSG_TYPE_SZ
                                                                 + DIS_TLV_HDR_SZ   // DIS_TLV_TYPE_LIST_ATTR_INFO
                                                                 + DIS_LIST_ATTR_ID_TLV_SZ
                                                                 + DIS_LIST_ATTR_IDX_TLV_SZ
                                                                 + DIS_TLV_HDR_SZ + listAttrValLen;

                                          APP_pendingTxMsgHndl = SYS_allocMem(pktLen);
                                          if (SYS_BD_HNDL_IS_VALID(APP_pendingTxMsgHndl))
                                          {
                                              UINT8_t *buff_p = SYS_GET_BD_BUFF(APP_pendingTxMsgHndl);

                                              buff_p += ADP_ELEMENT_TYPE_LEN;
                                              *buff_p = DIS_MSG_TYPE_GET_LIST_ATTR_VAL;
                                              buff_p += DIS_MSG_TYPE_SZ;
                                              buff_p = UTIL_buildTlvHdr(buff_p,
                                                                        DIS_TLV_TYPE_LIST_ATTR_INFO,
                                                                        DIS_LIST_ATTR_ID_TLV_SZ
                                                                        + DIS_LIST_ATTR_IDX_TLV_SZ
                                                                        + DIS_TLV_HDR_SZ + listAttrValLen);
                                              buff_p = UTIL_buildTlvHdr(buff_p, DIS_TLV_TYPE_LIST_ATTR_ID, DIS_LIST_ATTR_ID_FIELD_SZ);
                                              buff_p = UTIL_htons(buff_p, listAttrId);
                                              buff_p = UTIL_buildTlvHdr(buff_p, DIS_TLV_TYPE_LIST_ATTR_IDX, DIS_LIST_ATTR_IDX_FIELD_SZ);
                                              buff_p = UTIL_htons(buff_p, listAttrIdx);
                                              buff_p = UTIL_buildTlvHdr(buff_p, DIS_TLV_TYPE_ATTR_VAL, listAttrValLen);
                                              memcpy(buff_p, __attrValBuff, listAttrValLen);
                                              buff_p += listAttrValLen;

                                              APP_pendingTxMsgDestAddr = srcAddr;

                                              // Can't send packet to ADP/MAC right now since MAC will be busy acking
                                              // this received message.
                                              FFD_appRespTxFlag = 1;

                                              SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_APP_DATA_REQ;
                                          }
                                       }
                                   }
                               }
                           }
                       }
                   }
               }
               break;

          case DIS_MSG_TYPE_GET_ATTR_VAL:
               {
                   if (msgLen >= DIS_ATTR_ID_TLV_SZ)
                   {
                       offset ++;
                       if (msg_p[offset] == DIS_TLV_TYPE_ATTR_ID)
                       {
                           offset ++;
                           if (msg_p[offset] == DIS_ATTR_ID_FIELD_SZ)
                           {
                               UINT16_t attrId, attrValLen;

                               offset ++;
                               attrId = UTIL_ntohs(msg_p + offset);

                               attrValLen = NM_getAttrValSz(attrId);
                               if (attrValLen > 0)
                             	   NM_getAttrVal(attrId, __attrValBuff);
                               else
                               {
                                   attrValLen = 1;
                                   __attrValBuff[0] = 0;
                               }

                               if (!(SYS_BD_HNDL_IS_VALID(APP_pendingTxMsgHndl)))
                               {
                                   const UINT8_t pktLen = ADP_ELEMENT_TYPE_LEN
                                                          + DIS_MSG_TYPE_SZ
                                                          + DIS_TLV_HDR_SZ
                                                          + DIS_ATTR_ID_TLV_SZ
                                                          + DIS_TLV_HDR_SZ
                                                          + attrValLen;

                                   APP_pendingTxMsgHndl = SYS_allocMem(pktLen);
                                   if (SYS_BD_HNDL_IS_VALID(APP_pendingTxMsgHndl))
                                   {
                                       UINT8_t *buff_p = SYS_GET_BD_BUFF(APP_pendingTxMsgHndl);

                                       buff_p += ADP_ELEMENT_TYPE_LEN;
                                       *buff_p = DIS_MSG_TYPE_GET_ATTR_VAL;
                                       buff_p += DIS_MSG_TYPE_SZ;
                                       buff_p = UTIL_buildTlvHdr(buff_p,
                                                                 DIS_TLV_TYPE_ATTR_INFO,
                                                                 DIS_ATTR_ID_TLV_SZ + DIS_TLV_HDR_SZ + attrValLen);
                                       buff_p = UTIL_buildTlvHdr(buff_p, DIS_TLV_TYPE_ATTR_ID, DIS_ATTR_ID_FIELD_SZ);
                                       buff_p = UTIL_htons(buff_p, attrId);
                                       buff_p = UTIL_buildTlvHdr(buff_p, DIS_TLV_TYPE_ATTR_VAL, attrValLen);
                                       memcpy(buff_p, __attrValBuff, attrValLen);
                                       buff_p += attrValLen;

                                       APP_pendingTxMsgDestAddr = srcAddr;

                                       // Can't send packet to ADP/MAC right now since MAC will be busy acking
                                       // this received message.
                                       FFD_appRespTxFlag = 1;

                                       SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_APP_DATA_REQ;
                                   }
                               }
                           }
                       }
                   }
               }
               break;

#ifdef MAC_NEIGHBOR_TBL_ENA
          case DIS_MSG_TYPE_GET_NEIGHBOR_TBL_ENTRY:
               {
                   if (msgLen >= DIS_SHORT_ADDR_TLV_SZ)
                   {
                       offset ++;
                       if (msg_p[offset] == DIS_TLV_TYPE_SHORT_ADDR)
                       {
                           offset ++;
                           if (msg_p[offset] == MAC_SHORT_ADDR_LEN)
                           {
                               UINT16_t shortAddr;

                               offset ++;
                               shortAddr = UTIL_ntohs(msg_p + offset);

                               if (!(SYS_BD_HNDL_IS_VALID(APP_pendingTxMsgHndl)))
                               {
                                   const UINT8_t tlvLen = DIS_TLV_HDR_SZ   // Neighbor table entry TLV
                                                          + DIS_SHORT_ADDR_TLV_SZ
                                                          + DIS_LQI_TLV_SZ
                                                          + DIS_RSSI_TLV_SZ
                                                          + DIS_SECONDS_SINCE_PKT_RX_TLV_SZ
                                                          + DIS_RCVD_PKT_CNT_TLV_SZ;
                                   const UINT8_t pktLen = ADP_ELEMENT_TYPE_LEN
                                                          + DIS_MSG_TYPE_SZ + tlvLen;

                                   APP_pendingTxMsgHndl = SYS_allocMem(pktLen);
                                   if (SYS_BD_HNDL_IS_VALID(APP_pendingTxMsgHndl))
                                   {
                                       UINT8_t *buff_p = SYS_GET_BD_BUFF(APP_pendingTxMsgHndl);
                                       MAC_neighborTblEntry_s nTEntry;

                                       memset(&nTEntry, 0, sizeof(nTEntry));
                                       nTEntry.shortAddr = shortAddr;

                                       MAC_getNeighborTblEntry(&nTEntry);

                                       buff_p += ADP_ELEMENT_TYPE_LEN;
                                       *buff_p = DIS_MSG_TYPE_GET_NEIGHBOR_TBL_ENTRY;
                                       buff_p += DIS_MSG_TYPE_SZ;
                                       buff_p = UTIL_buildTlvHdr(buff_p,
                                                                 DIS_TLV_TYPE_MAC_NEIGHBOR_TBL_ENTRY,
                                                                 tlvLen - DIS_TLV_HDR_SZ);
                                       buff_p = UTIL_buildTlvHdr(buff_p, DIS_TLV_TYPE_SHORT_ADDR, MAC_SHORT_ADDR_LEN);
                                       buff_p = UTIL_htons(buff_p, shortAddr);

                                       buff_p = UTIL_buildTlvHdr(buff_p, DIS_TLV_TYPE_LQI, DIS_LQI_TLV_FIELD_SZ);
                                       *(buff_p ++) = nTEntry.lqi;

                                       buff_p = UTIL_buildTlvHdr(buff_p, DIS_TLV_TYPE_RSSI, DIS_RSSI_TLV_FIELD_SZ);
                                       *(buff_p ++) = nTEntry.rssi;

                                       buff_p = UTIL_buildTlvHdr(buff_p, DIS_TLV_TYPE_RCVD_PKT_CNT,
                                                                 DIS_RCVD_PKT_CNT_TLV_FIELD_SZ);
                                       buff_p = UTIL_htons(buff_p, nTEntry.rxPktCnt);

                                       buff_p = UTIL_buildTlvHdr(buff_p, DIS_TLV_TYPE_SECONDS_SINCE_PKT_RX,
                                                                 DIS_SECONDS_SINCE_PKT_RX_TLV_FIELD_SZ);
                                       buff_p = UTIL_htons(buff_p, nTEntry.timeSinceLastRx);

                                       APP_pendingTxMsgDestAddr = srcAddr;

                                       // Can't send packet to ADP/MAC right now since MAC will be busy acking
                                       // this received message.
                                       FFD_appRespTxFlag = 1;

                                       SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_APP_DATA_REQ;
                                   }
                               }
                           }
                       }
                   }
               }
               break;
#endif

          case DIS_MSG_TYPE_CFG_NODE_DATA_PUSH_INTERVAL:
               {
                   if (msgLen >= (DIS_TLV_HDR_SZ + LPWMN_GW_MSG_NODE_DATA_PUSH_INTERVAL_FIELD_LEN))
                   {
                       offset ++;
                       if (msg_p[offset] == DIS_TLV_TYPE_PUSH_INTERVAL)
                       {
                           offset ++;
                           if (msg_p[offset] == LPWMN_GW_MSG_NODE_DATA_PUSH_INTERVAL_FIELD_LEN)
                           {
                               UINT16_t pushIntervalSecs;

                               offset ++;
                               pushIntervalSecs = UTIL_ntohs(msg_p + offset);
                               if (pushIntervalSecs > 0)
                               {
                                   FFD_APP_commIntervalInSecs = pushIntervalSecs;
                                   APP_txInterval = FFD_APP_commIntervalInSecs;

                                   FFD_APP_COMMON_updateCfgInfo(APP_TLV_TYPE_DATA_PUSH_INTERVAL,
                                                                sizeof(FFD_APP_commIntervalInSecs),
                                                                (UINT8_t *)&FFD_APP_commIntervalInSecs);
                               }
                           }
                       }
                   }
               }
               break;

          case DIS_MSG_TYPE_GET_RT_NEXT_HOP:
               {
                   if (msgLen >= DIS_SHORT_ADDR_TLV_SZ)
                   {
                       offset ++;
                       if (msg_p[offset] == DIS_TLV_TYPE_SHORT_ADDR)
                       {
                           UINT16_t rtToSA;
                           const UINT8_t pktLen = ADP_ELEMENT_TYPE_LEN
                                                  + DIS_MSG_TYPE_SZ
                                                  + DIS_SHORT_ADDR_TLV_SZ;

                           offset += UTIL_TLV_HDR_SZ;
                           rtToSA = UTIL_ntohs(msg_p + offset);

                           if (!(SYS_BD_HNDL_IS_VALID(APP_pendingTxMsgHndl)))
                           {
                               APP_pendingTxMsgHndl = SYS_allocMem(pktLen);

                               if (SYS_BD_HNDL_IS_VALID(APP_pendingTxMsgHndl))
                               {
                                   UINT8_t *buff_p = SYS_GET_BD_BUFF(APP_pendingTxMsgHndl);
                                   UINT16_t nextHop;

                                   buff_p += ADP_ELEMENT_TYPE_LEN;

                                   *buff_p = DIS_MSG_TYPE_GET_RT_NEXT_HOP;
                                   buff_p += DIS_MSG_TYPE_SZ;
                                   buff_p = UTIL_buildTlvHdr(buff_p,
                                                             DIS_TLV_TYPE_SHORT_ADDR,
                                                             MAC_SHORT_ADDR_LEN);

                                   if (MAC_lookUpAssocRFDList(rtToSA))
                                	   nextHop = rtToSA;
                                   else
                                       nextHop = AODVL_getRoute(rtToSA);
                                   UTIL_htons(buff_p, nextHop);

                                   APP_pendingTxMsgDestAddr = srcAddr;

                                   // Can't send packet to ADP/MAC right now since MAC will be busy acking
                                   // this received message.
                                   FFD_appRespTxFlag = 1;

                                   SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_APP_DATA_REQ;
                               }
                           }
                       }
                   }
               }
               break;

          case DIS_MSG_TYPE_GET_NODE_SENSOR_COUNT:
               {
                   const UINT8_t pktLen = ADP_ELEMENT_TYPE_LEN
                                          + DIS_MSG_TYPE_SZ
                                          + DIS_SENSOR_COUNT_TLV_SZ;

                   if (!(SYS_BD_HNDL_IS_VALID(APP_pendingTxMsgHndl)))
                   {
                       APP_pendingTxMsgHndl = SYS_allocMem(pktLen);

                       if (SYS_BD_HNDL_IS_VALID(APP_pendingTxMsgHndl))
                       {
                           UINT8_t *buff_p = SYS_GET_BD_BUFF(APP_pendingTxMsgHndl);
                           buff_p += ADP_ELEMENT_TYPE_LEN;

                           *buff_p = DIS_MSG_TYPE_NODE_SENSOR_COUNT;
                           buff_p += DIS_MSG_TYPE_SZ;
                           buff_p = UTIL_buildTlvHdr(buff_p,
                                                     DIS_TLV_TYPE_SENSOR_COUNT,
                                                     DIS_SENSOR_COUNT_FIELD_SZ);
                           *(buff_p ++) = PLTFRM_getNodeSnsrCnt();

                           APP_pendingTxMsgDestAddr = srcAddr;

                           // Can't send packet to ADP/MAC right now since MAC will be busy acking
                           // this received message.
                           FFD_appRespTxFlag = 1;

                           SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_APP_DATA_REQ;
                       }
                   }
                   else
                   {
                       SYS_fatal(SYS_FATAL_ERR_480);
                   }
               }
               break;

          default:
               {
               }
               break;
       }

       return;
   }
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void APP_evtHndlr(UINT16_t globalEvtId, void *params_p)
{
   switch (globalEvtId)
   {
      case SYS_GLOBAL_EVT_ADP_DATA_CONFIRM:
           {
              ADP_dataCnfrmParams_s *dcParams_p = (ADP_dataCnfrmParams_s *)params_p;

              /*
               * This app has received a confirmation for the data transmission request.
               * The node can now be put in to deep sleep for "RFD_APP_snsrRepInterval"
               * seconds. This app requests the SYS module to do so by setting the
               * "SYS_GLOBAL_EVT_ENTER_LPM_REQUEST" bit in "SYS_globalEvtMsk1". When
               * control returns to the system loop (in system.c), the SYS module will
               * put the radio into low power mode (LPM2), start the periodic wake up
               * timer (timeout - RFD_APP_snsrRepInterval seconds) and then put the MSP430
               * into low power mode (LPM3).
               */

              if (dcParams_p->sts == ADP_STS_SUCCESS)
              {
                  /*
                   * The message was successfully sent to the next hop node in the
                   * path to the specified destination. In this app, the destination
                   * node is the LPWMN coordinator. If this node is just one hop
                   * away from the LPWMN coordinator, "SUCCESS" implies the coordinator
                   * has acknowledged this message. If this node is more than 1 hop away
                   * from the coordinator, "SUCCESS" implies that the next hop in the
                   * path to the coordinator has acknowledged this message.
                   */
                  FFD_APP_cnfrmOkCnt ++;
              }
              else
              {
                  /*
                   * Possible reasons for failure include -
                   * > No acknowledgment from next hop node in the path to the intended
                   *   destination.
                   * > CSMA/CA failure (medium remained busy during all attempts)
                   */
                  FFD_APP_cnfrmFlrCnt ++;
              }
#ifdef FFD_APP_SEND_DATA_BACK_TO_BACK
              SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_APP_DATA_REQ;
              if (FFD_appRespTxFlag == 0)
                  FFD_snsrDataTxPendFlag = 1;
#ifdef CC1101_TIMING_DBG_ENA
              // P1OUT |= (BIT6); P1OUT |= (BIT7);
#endif
#else
              // Bug fix /  rkris@wisense.in / feb/25/2016
              // Any message sent to FFD app will reset APP_txInterval to FFD_APP_commIntervalInSecs
              // which is not the expected behaviour.
              if (APP_txInterval == 0)
            	  APP_txInterval = FFD_APP_commIntervalInSecs;
              if (FFD_appRespTxFlag == 1)
              {
                  SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_APP_DATA_REQ;
              }
#endif
           }
           break;

      case SYS_GLOBAL_EVT_ADP_DATA_INDICATION:
           {
              ADP_dataIndParams_s *dcParams_p = (ADP_dataIndParams_s *)params_p;

              FFD_APP_dataIndCnt ++;

              if (SYS_BD_HNDL_IS_VALID(APP_pendingTxMsgHndl))
              {
                  FFD_APP_dataIndDropCnt ++;
              }
              else
              {
                  if (SYS_BD_HNDL_IS_VALID(dcParams_p->bdHndl))
                  {
                      FFD_APP_rcvdMsgHndlr(dcParams_p->bdHndl, dcParams_p->srcAddr);
                      SYS_freeMem(dcParams_p->bdHndl);
                  }
                  else
                  {
                      SYS_fatal(SYS_FATAL_ERR_79);
                  }
              }
           }
           break;

      default:
           break;
   }
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void APP_COMMON_procGPIOEvt(void)
{
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void FFD_APP_commonInit(void)
{
   FFD_APP_COMMON_readCfgInfo();

   APP_txInterval = FFD_APP_commIntervalInSecs;

#ifdef FFD_APP_PKT_GEN
   FFD_appInit();
#endif

#ifdef FFD_APP_1
#ifdef PLTFRM_LM75B_TEMP_SENSOR_ENA
   if (LM75B_cfg(PLTFRM_LM75B_1_DEV_ID, LM75B_OPN_MODE_NORMAL) != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_6004);
#endif

#ifdef PLTFRM_TSL45315_ENA
   if (TSL45315_cfgALSOpnMode(PLTFRM_TSL45315_1_DEV_ID, TSL45315_OPN_MODE_NORMAL, NULL) != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_6006);
#endif
#endif
}
#endif
#endif
