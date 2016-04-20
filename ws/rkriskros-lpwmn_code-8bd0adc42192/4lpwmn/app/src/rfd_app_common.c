/*
 * File Name: rfd_app_common.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Jan/30/2015
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

#ifdef DEV_TYPE_RFD

#include <typedefs.h>
#include <system.h>
#include <pltfrm.h>
#include <util.h>
#include <mac_pib.h>
#include <adp.h>
#include <rfd_app.h>
#include <dis.h>
#include <node_mgr.h>
#include <nvm.h>
#ifdef UC_FU_ENA
#include <fu.h>
#endif

UINT8_t RFD_APP_lastTxMsgType;

// rkris@wisense.in / mar/6/16
UINT32_t RFD_APP_lastSnsrUpdateSentTS = 0;
UINT32_t RFD_APP_maxSnsrMsgTxInterval = 0;

UINT8_t __attrValBuff[NM_MAX_ATTR_VAL_LEN];

#ifdef PLTFRM_EKMC160111X_ENA
#include <ekmc160111x.h>
#endif

RFD_APP_state_t RFD_APP_state = RFD_APP_STATE_CONVERSION_DONE;
SINT32_t RFD_APP_snsrRepInterval = RFD_APP_DFLT_WAKE_UP_INTERVAL_SECS;
#if (APP_DATA_PUSH_INTERVAL_VALUE_FIELD_SZ != 4)
#error mismatch !!
#endif

UINT8_t APP_pendingTxMsgHndl = SYS_INV_BD_HDNL;
UINT16_t APP_pendingTxMsgDestAddr = MAC_BROADCAST_SHORT_ADDR;

// Stats
UINT32_t RFD_APP_cnfrmOkCnt = 0;
UINT32_t RFD_APP_cnfrmFlrCnt = 0;
UINT16_t RFD_APP_dataIndCnt = 0;
UINT16_t RFD_APP_pollReqCnt = 0;
UINT16_t RFD_APP_framePendingCnt = 0;
UINT16_t RFD_APP_frameNotPendingCnt = 0,
         RFD_APP_msgRcvdCnt = 0,
         RFD_APP_coordDataWaitTmoCnt = 0,
         RFD_APP_pollReqCnfrmHndlrCnt = 0,
         RFD_APP_pendDataReqCnt = 0,
         RFD_APP_dataCnfrmCnt = 0,
         RFD_APP_ackWithFramePendingCnt = 0,
         RFD_APP_unExpDataIndCnt = 0,
         RFD_APP_COMMON_keepAliveTxReqAttemptCnt = 0,
         RFD_APP_COMMON_keepAliveTxReqFlrCnt = 0;

#ifdef RFD_KA_ENA
SINT32_t RFD_APP_sleepTimeElapsed = 0, RFD_APP_timeSinceLastParentAck = 0;
#endif

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_reqSysLPM(void)
{
  /*
   * The node can now be put in to deep sleep for "RFD_APP_snsrRepInterval"
   * seconds. This app requests the SYS module to do so by setting the
   * "SYS_GLOBAL_EVT_ENTER_LPM_REQUEST" bit in "SYS_globalEvtMsk1". When
   * control returns to the system loop (in system.c), the SYS module will
   * put the radio into low power mode (LPM2), start the periodic wake up
   * timer (timeout - RFD_APP_snsrRepInterval seconds) and then put the MSP430
   * into low power mode (LPM3).
   */

   SYS_SET_WAKEUP_INTERVAL(RFD_APP_snsrRepInterval);
   SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ENTER_LPM_REQUEST;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_coordPollStartFlr(void)
{
   RFD_APP_state = RFD_APP_STATE_CONVERSION_DONE;
   RFD_APP_reqSysLPM();
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_COMMON_updateCfgInfo(UINT8_t tlvId,
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
               // rc = UTIL_getTlv(buff1_p, tlvLen1, APP_TLV_TYPE_DATA_PUSH_INTERVAL, &tlvLen2, &buff2_p);
               rc = UTIL_getTlv(buff1_p, tlvLen1, tlvId, &tlvLen2, &buff2_p);
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
                           SYS_fatal(SYS_FATAL_ERR_884);
                   }
               }
           }
       }

       SYS_freeMem(tempBd);
   }
   else
   {
      SYS_fatal(SYS_FATAL_ERR_885);
   }
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_coordDataWaitTmo(void)
{
   // No data from coord. Give up and go back to sleep !!
   RFD_APP_coordDataWaitTmoCnt ++;
   RFD_APP_state = RFD_APP_STATE_CONVERSION_DONE;
   RFD_APP_reqSysLPM();
}



/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_pollReqCnfrmHndlr(void)
{
  RFD_APP_pollReqCnfrmHndlrCnt ++;

  if (RFD_APP_state != RFD_APP_STATE_POLL_REQ_IN_PROGRESS)
  {
      SYS_fatal(SYS_FATAL_ERR_450);
      return;
  }

  switch (MAC_cntxt.mlmePollCnfrmParams.status)
  {
     case MAC_STS_FRAME_PENDING:
          {
              PLTFRM_sts_t sts;

              RFD_APP_framePendingCnt ++;

              // Wait some time for Coord to send data
              sts = PLTFRM_startTimerA1(0, RFD_APP_WAIT_COORD_DATA_TMO_MSECS, RFD_APP_coordDataWaitTmo);
              if (sts == PLTFRM_STS_SUCCESS)
              {
                  RFD_APP_state = RFD_APP_STATE_WAIT_DATA_FROM_COORD;
              }
              else
              {
                  SYS_fatal(SYS_FATAL_ERR_442);
              }
          }
          break;

     default:
     case MAC_STS_NO_ACK:
     case MAC_STS_CHANNEL_ACCESS_FLR:
     case MAC_STS_NO_DATA:
          {
             RFD_APP_frameNotPendingCnt ++;
             // Put the node back to sleep
             RFD_APP_state = RFD_APP_STATE_CONVERSION_DONE;
             if (MAC_nodeJoined())
                 RFD_APP_reqSysLPM();
          }
          break;
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
void RFD_APP_procPendDataReq(void)
{
   if (!(SYS_BD_HNDL_IS_VALID(APP_pendingTxMsgHndl))
       || APP_pendingTxMsgDestAddr == MAC_BROADCAST_SHORT_ADDR)
   {
       SYS_fatal(SYS_FATAL_ERR_420);
   }
   else
   {
       ADP_cntxt.txParams.destShortAddr = APP_pendingTxMsgDestAddr;
       ADP_cntxt.txParams.bdHndl = APP_pendingTxMsgHndl;
       ADP_cntxt.txParams.srcModId = ADP_TX_REQ_SRC_APP;

       RFD_APP_pendDataReqCnt ++;

       APP_pendingTxMsgHndl = SYS_INV_BD_HDNL;
       APP_pendingTxMsgDestAddr = MAC_BROADCAST_SHORT_ADDR;

       if (ADP_appPyldTxReq() != ADP_STS_SUCCESS)
       {
           RFD_APP_reqSysLPM();
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
void APP_nodeJoinDoneInd(void)
{
#ifdef RFD_KA_ENA
   RFD_APP_timeSinceLastParentAck = 0;
   RFD_APP_sleepTimeElapsed = 0;
#endif

#ifdef PLTFRM_EKMC160111X_ENA
   // RFD_APP_wakeUpProc();
#endif


#ifdef VEHICLE_DETECTION_USING_HIGH_PASS_FILTER_ENA
   __HPF_init( );
#endif

#if 1
   {
      SINT32_t wakeUpInterval = RFD_APP_snsrRepInterval;

      // rkris@wisense.in - sep/6/15
      // After joining the network, wake up after "RFD_APP_FAST_RETRY_ON_TX_FLR_INTERVAL_SECS"
      // seconds instead of the configured data push interval.

      if (wakeUpInterval > (RFD_APP_FAST_RETRY_ON_TX_FLR_INTERVAL_SECS*3))
          wakeUpInterval = RFD_APP_FAST_RETRY_ON_TX_FLR_INTERVAL_SECS;

      SYS_SET_WAKEUP_INTERVAL(wakeUpInterval);
      SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ENTER_LPM_REQUEST;
   }
#endif
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_prepareToEnterLPM(void)
{
   // Interrupts are disabled globally at this point.
   // The MSP430 will go into deep sleep mode on returning
   // from this function.
   return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
UINT8_t RFD_APP_continueWithLowSupplyV(void)
{
   return 0;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void APP_COMMON_procGPIOEvt(void)
{
#ifdef RFD_APP_PIR_MS
   RFD_APP_procGPIOEvt();
#endif
   return;
}


/*
 ********************************************************************
 * Function:
 * RFD_APP_updatePushInterval
 *
 * Description:
 * This function samples the three sensors on this node. It builds a message
 * with the sensor data and then requests the ADP (adaptation) layer to
 * send the packet out over the mesh network to the network coordinator (short
 * address 0x0001).
 *
 ********************************************************************
 */
RFD_APP_updatePushInterval(UINT16_t newPushInterval)
{
   if (newPushInterval > 0)
   {
       RFD_APP_snsrRepInterval = newPushInterval;

       RFD_APP_COMMON_updateCfgInfo(APP_TLV_TYPE_DATA_PUSH_INTERVAL,
                                    sizeof(RFD_APP_snsrRepInterval),
                                    (UINT8_t *)&RFD_APP_snsrRepInterval);
   }
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
UINT8_t RFD_APP_rcvdMsgHndlr(UINT8_t bdHndl, UINT16_t srcAddr)
{
   UINT8_t retCode = 0, msgLen = SYS_GET_BD_LEN(bdHndl);
   UINT8_t *msg_p;
   UINT16_t offset = 0;

   RFD_APP_msgRcvdCnt ++;

   if (msgLen >= DIS_MSG_TYPE_SZ)
   {
       msg_p = SYS_GET_BD_BUFF(bdHndl);
       msgLen --;

       switch (msg_p[offset])
       {
#ifdef UC_FU_ENA
          case DIS_MSG_TYPE_SWITCH_TO_SBY_IMAGE:
               {
                  UC_FU_procRcvdMsg(msg_p, msgLen + 1);
               }
               break;
#endif

          case DIS_MSG_TYPE_REBOOT_NODE_REQ:
               {
                  // rkris@wisense.in / april/20/15
                  PLTFRM_saveSysFEId(SYS_FATAL_ERR_REBOOT_ON_REQ);
                  PLTFRM_mcuReset();
               }
               break;

#ifdef GPIO_MGMT_ENA
          case DIS_MSG_TYPE_GET_GPIO_PORT_OP_STATE:
               {
                   if (!(SYS_BD_HNDL_IS_VALID(APP_pendingTxMsgHndl)))
                   {
                       UINT8_t pktLen = ADP_ELEMENT_TYPE_LEN
                                        + DIS_MSG_TYPE_SZ
                                        + DIS_TLV_HDR_SZ;  // port list TLV
#if defined( __MSP430G2955__) || defined(__MSP430F2274__)
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

                           // Can't send packet to ADP/MAC right now since MAC will be busy acking
                           // this received message.

                           SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_APP_DATA_REQ;
                           retCode = 1;  // Response needs to be sent.
                       }
                   }
               }
               break;
#endif

#ifdef PLTFRM_REMOTE_GPIO_CTRL_ENA
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
#endif

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

                                       SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_APP_DATA_REQ;
                                       retCode = 1;  // Response needs to be sent.
                                   }
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

                           SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_APP_DATA_REQ;
                           retCode = 1;  // Response needs to be sent.
                       }
                   }
                   else
                   {
                       SYS_fatal(SYS_FATAL_ERR_463);
                   }
               }
               break;

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
                               RFD_APP_updatePushInterval(pushIntervalSecs);
                           }
                       }
                   }
               }
               break;

          default:
               {
               }
               break;
       }
   }

   return retCode;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_pollCoord(void)
{
   SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_COORD_POLL_PENDING;
   RFD_APP_state = RFD_APP_STATE_POLL_REQ_IN_PROGRESS;
   RFD_APP_pollReqCnt ++;
}


#ifdef RFD_KA_ENA
/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_keepAliveTmrExpEvt(void)
{
   UINT16_t battVoltage;
   UINT8_t pktLen = ADP_ELEMENT_TYPE_LEN
                    + DIS_MSG_TYPE_SZ
#ifdef PLTFRM_ON_CHIP_VCC_SENSE_ENA
                    + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                    + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + PLTFRM_ON_CHIP_VCC_SENSOR_OUTPUT_LEN
                    + DIS_DATA_SCALE_FACTOR_TLV_SZ
#endif
   ;

   ADP_cntxt.txParams.bdHndl = SYS_allocMem(pktLen);
   if (SYS_BD_HNDL_IS_VALID(ADP_cntxt.txParams.bdHndl))
   {
       UINT8_t *buff_p;

       buff_p = SYS_GET_BD_BUFF(ADP_cntxt.txParams.bdHndl);
       // buff_p points to the start of the memory buffer allocated by SYS_allocMem()

       buff_p += ADP_ELEMENT_TYPE_LEN;

       *buff_p = DIS_MSG_TYPE_RFD_KEEP_ALIVE_MSG;
       buff_p += DIS_MSG_TYPE_SZ;

#ifdef PLTFRM_ON_CHIP_VCC_SENSE_ENA
       {
          UINT8_t *tlvHdr_p = buff_p;

          buff_p += UTIL_TLV_HDR_SZ;

          // Call the driver function to sample and return the current voltage
          // at the micro's Vcc pin. The returned value is in milli-volts.
          // The driver code is in the file "pltfrm/src/vcc_sense.c".
          battVoltage = pltfrm_vccSense(1);

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_ON_CHIP_VCC_SENSOR_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    PLTFRM_ON_CHIP_VCC_SENSOR_OUTPUT_LEN);
          buff_p = UTIL_htons(buff_p, battVoltage);

          // Send the scale factor since the output is in milli-volts
          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_DATA_SCALE_FACTOR,
                                    DIS_DATA_SCALE_FACTOR_FIELD_SZ);
          *(buff_p ++) = DIS_DATA_SCALE_MILLI;

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }
#endif
   }
   else
   {
       // RFD should not run out of memory !!
       SYS_fatal(SYS_FATAL_ERR_872);
   }

   RFD_APP_COMMON_keepAliveTxReqAttemptCnt ++;

   ADP_cntxt.txParams.destShortAddr = MAC_PIB.macCoordShortAddress;  // Parent of this FFD
   ADP_cntxt.txParams.srcModId = ADP_TX_REQ_SRC_APP;

   if (ADP_appPyldTxReq() != ADP_STS_SUCCESS)
   {
       RFD_APP_COMMON_keepAliveTxReqFlrCnt ++;
       SYS_SET_WAKEUP_INTERVAL(RFD_APP_snsrRepInterval);
       SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ENTER_LPM_REQUEST;
   }
   else
       RFD_APP_lastTxMsgType = DIS_MSG_TYPE_RFD_KEEP_ALIVE_MSG;

   return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
UINT8_t RFD_APP_checkKAProc(void)
{
   UINT8_t rc = 0;

   // Called from system.c every second ..

   if (NM_cntxt.rfdKeepAliveTmoSecs == 0)
       return rc;

   RFD_APP_timeSinceLastParentAck ++;
   RFD_APP_sleepTimeElapsed ++;

   if (RFD_APP_timeSinceLastParentAck > (NM_cntxt.rfdKeepAliveTmoSecs >> 1))
       rc = 1;

   return rc;
}
#endif


UINT16_t EKMC160111X_cnfrmCnt = 0;

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_evtHndlr(UINT16_t globalEvtId, void *params_p)
{
   switch (globalEvtId)
   {
      case SYS_GLOBAL_EVT_ADP_DATA_CONFIRM:
           {
              ADP_dataCnfrmParams_s *dcParams_p = (ADP_dataCnfrmParams_s *)params_p;

               RFD_APP_dataCnfrmCnt ++;

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

#ifdef PLTFRM_EKMC160111X_ENA
              EKMC160111X_cnfrmCnt ++;
#endif

#ifdef RFD_APP_BAR_CODE_SCANNER
              UART_flushRcvdData();
#endif

#ifdef RFD_KA_ENA
              // rkris@wisense.in / june/10/15
              if (RFD_APP_lastTxMsgType == DIS_MSG_TYPE_RFD_KEEP_ALIVE_MSG)
              {
                  // This is confirmation for a keep alive frame.

                  if (SYS_wakeUpIntervalSecs > 0)
                  {
                      SYS_wakeUpIntervalSecs -= RFD_APP_sleepTimeElapsed;
                      if (SYS_wakeUpIntervalSecs < 0)
                          SYS_wakeUpIntervalSecs = 1;
                  }

                  if (dcParams_p->sts == ADP_STS_SUCCESS)
                      RFD_APP_timeSinceLastParentAck = 0;
                  else
                  {
                      SINT16_t delta = NM_cntxt.rfdKeepAliveTmoSecs;

                      delta >>= 1;
                      if (delta > RFD_KEEP_ALIVE_RETRY_TIME_SECS)
                          delta -= RFD_KEEP_ALIVE_RETRY_TIME_SECS;
                      else
                          delta >>= 1;

                      RFD_APP_timeSinceLastParentAck = delta;
                  }

                  SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ENTER_LPM_REQUEST;
                  return;
              }
#endif


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

#ifdef RFD_COORD_POLL_ENA
                  if (dcParams_p->framePending)
                  {
                      RFD_APP_ackWithFramePendingCnt ++;
                      RFD_APP_pollCoord();
                  }
                  else
                      RFD_APP_reqSysLPM();
#else
                  RFD_APP_reqSysLPM();
#endif

#ifdef RFD_KA_ENA
                  RFD_APP_timeSinceLastParentAck = 0;
#endif
                  RFD_APP_cnfrmOkCnt ++;
              }
              else
              {
                  /*
                   * Possible reasons for failure include -
                   * > No acknowledgment from next hop node in the path to the intended
                   *   destination.
                   * > CSMA/CA failure (medium remained busy during all attempts)
                   */
                  RFD_APP_cnfrmFlrCnt ++;
                  RFD_APP_state = RFD_APP_STATE_CONVERSION_DONE;

#ifdef RFD_APP_MDS
                  MDS_APP_cnfrmFlrInd();
#endif

                  if (MAC_nodeJoined())    // rkris@wisense.in - aug/18/15
                  {
                      SINT32_t wakeUpInterval = RFD_APP_snsrRepInterval;
                      // rkris@wisense.in - sep/6/15
                      // On transmit failure, wake up after RFD_APP_FAST_RETRY_ON_TX_FLR_INTERVAL_SECS to
                      // retry instead of waiting for the configured data push interval. This will force
                      // this RFD to look for another parent.
                      if (wakeUpInterval > (RFD_APP_FAST_RETRY_ON_TX_FLR_INTERVAL_SECS * 3))
                          wakeUpInterval = RFD_APP_FAST_RETRY_ON_TX_FLR_INTERVAL_SECS;

                      SYS_SET_WAKEUP_INTERVAL(wakeUpInterval);
                      SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ENTER_LPM_REQUEST;
                  }
              }
           }
           break;

      case SYS_GLOBAL_EVT_ADP_DATA_INDICATION:
            {
               UINT8_t respPending = 0;
               ADP_dataIndParams_s *dcParams_p = (ADP_dataIndParams_s *)params_p;

               if (SYS_BD_HNDL_IS_VALID(dcParams_p->bdHndl))
               {
                   if (RFD_APP_state != RFD_APP_STATE_WAIT_DATA_FROM_COORD)
                   {
                       RFD_APP_unExpDataIndCnt ++;
                       SYS_freeMem(dcParams_p->bdHndl);
                       break;  // Nothing to be sent in response.
                   }

                   RFD_APP_state = RFD_APP_STATE_CONVERSION_DONE;

                   PLTFRM_stopTimerA1();

                   RFD_APP_dataIndCnt ++;

                   respPending = RFD_APP_rcvdMsgHndlr(dcParams_p->bdHndl, dcParams_p->srcAddr);
                   SYS_freeMem(dcParams_p->bdHndl);
               }
               else
               {
                   SYS_fatal(SYS_FATAL_ERR_462);
               }

               // RFD_APP_state = RFD_APP_STATE_CONVERSION_DONE;

               if (respPending == 0)
                   RFD_APP_reqSysLPM();
            }
            break;

      default:
           break;
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
void RFD_APP_COMMON_readCfgInfo(void)
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
                       RFD_APP_snsrRepInterval = UTIL_ntohl(buff2_p);

#ifdef WISENSE_PIR_SENSOR_NODE
                       // Power on the PIR sensor if push interval is 120 secs. Otherwise
                       // power it off. This is to handle RFD PIR sensor reboot/reset etc.
                       if (RFD_APP_snsrRepInterval <= PIR_SENSOR_PWR_UP_PUSH_INTERVAL)
                    	   RFD_APP_snsrCtrlUpdate(1);
                       else
                    	   RFD_APP_snsrCtrlUpdate(0);
#endif
                   }
               }

#if 0
               rc = UTIL_getTlv(buff1_p, tlvLen1, APP_TLV_TYPE_NV_ATTR_LIST, &tlvLen2, &buff2_p);
               if (rc)
               {
                   while (tlvLen2 > 0)
                   {
                	   UINT8_t tlvLen3, *buff3_p;

                	   rc = UTIL_getTlv(buff2_p, tlvLen2, APP_TLV_TYPE_NV_ATTR, &tlvLen3, &buff3_p);
                	   if (rc)
                	   {
                           if (tlvLen3 > DIS_ATTR_ID_FIELD_SZ)
                           {
                        	   UINT8_t attrValLen;
                               UINT16_t attrId = UTIL_ntohs(buff3_p);

                               switch (attrId)
                               {
#ifdef WISENSE_PIR_SENSOR_NODE
                                  case APP_PIR_SENSOR_ENA_DIS_CTRL_ATTR_ID:
                                	   attrValLen = 1;
                                	   if (tlvLen3 == (attrValLen + DIS_ATTR_ID_FIELD_SZ))
                                	   {
                                           UINT8_t attrVal = *(buff3_p + DIS_ATTR_ID_FIELD_SZ);

                                           if (attrVal <= 1)
                                           {
                                        	   RFD_APP_snsrCtrlUpdate(attrVal);
                                           }
                                           else
                                           {
                                        	   // Corrupt value (can be 0 or 1 only) !!
                                           }
                                	   }
                                	   else
                                	   {
                                		   // Corrupt length field
                                	   }
                                	   break;
#endif
                                  default:
                                	   break;
                               }

                        	   buff2_p = buff3_p + tlvLen3;
                        	   if (tlvLen2 >= (tlvLen3 + APP_TLV_HDR_SZ))
                        	       tlvLen2 -= (tlvLen3 + APP_TLV_HDR_SZ);
                        	   else
                        	   {
                        		   // Corrupt - enclosed TLV is beyond end of parent TLV
                        	   }
                           }
                           else
                           {
                        	   // APP_TLV_TYPE_NV_ATTR value field length is <=  2 !!
                           }
                	   }
                       else
                       {
                    	   // No APP_TLV_TYPE_NV_ATTR found !!
                       }
                   }  // while (tlvLen2 > 0) loop
               }  // APP_TLV_TYPE_NV_ATTR_LIST attribute found case
#endif
           }
           else
           {
               UINT8_t *buff1_p = buff_p;

               // Initialize the EEPROM ...
               *(buff1_p ++) = APP_TLV_TYPE_CFG_DATA;
               *(buff1_p ++) = APP_TLV_HDR_SZ + APP_DATA_PUSH_INTERVAL_VALUE_FIELD_SZ;
               *(buff1_p ++) = APP_TLV_TYPE_DATA_PUSH_INTERVAL;
               *(buff1_p ++) = APP_DATA_PUSH_INTERVAL_VALUE_FIELD_SZ;
               buff1_p = UTIL_htonl(buff1_p, RFD_APP_snsrRepInterval);

               sts = NVM_writeMem(PLTFRM_NVM_DEV_ID,
                                  NVM_APP_CFG_DATA_OFFSET,
                                  buff1_p - buff_p,
                                  buff_p);
               if (sts != PLTFRM_STS_SUCCESS)
                   SYS_fatal(SYS_FATAL_ERR_886);
           }
       }

       SYS_freeMem(tempBd);
   }
   else
   {
      SYS_fatal(SYS_FATAL_ERR_883);
   }
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_COMMON_init(void)
{
   RFD_APP_init();

   RFD_APP_COMMON_readCfgInfo();

   return;
}

#endif
