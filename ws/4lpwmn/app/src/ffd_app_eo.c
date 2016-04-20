/*
 * File Name: ffd_app_eo.c
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
#ifdef FFD_APP_EO

#include <typedefs.h>
#include <system.h>
#include <pltfrm.h>
#include <util.h>
#include <mac_pib.h>
#include <adp.h>
#include <dis.h>
#include <he055t01.h>

#define FFD_APP_COMM_INTERVAL_IN_SECS  3

UINT8_t APP_nodeJoinDoneIndCnt = 0;

UINT16_t FFD_APP_dataTxReqCnt = 0;
UINT16_t FFD_APP_dataRxReqFlrCnt = 0;
UINT16_t FFD_APP_cnfrmOkCnt = 0;
UINT16_t FFD_APP_cnfrmFlrCnt = 0;
UINT16_t FFD_APP_memAllocFlrCnt = 0;
UINT16_t FFD_APP_dataIndCnt = 0;
UINT16_t FFD_APP_dataIndDropCnt = 0;

UINT8_t APP_nodeJoined = 0;
UINT16_t APP_txInterval = FFD_APP_COMM_INTERVAL_IN_SECS;

UINT8_t APP_pendingTxMsgHndl = SYS_INV_BD_HDNL;
UINT16_t APP_pendingTxMsgDestAddr = MAC_BROADCAST_SHORT_ADDR;
UINT8_t FFD_appRespTxFlag = 0, FFD_snsrDataTxPendFlag = 0;

/*
 ********************************************************************
 * Function:
 * FFD_APP_buildSendSnsrUpdate
 *
 * Description:
 * This function samples the three sensors on this node. It builds a message
 * with the sensor data and then requests the ADP (adaptation) layer to
 * send the packet out over the mesh network to the network coordinator (short
 * address 0x0001).
 *
 ********************************************************************
 */
void FFD_APP_buildSendSnsrUpdate(void)
{
   UINT8_t pktLen = ADP_ELEMENT_TYPE_LEN
                    + DIS_MSG_TYPE_SZ
                    + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT_LIST
                    + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
#ifdef PLTFRM_ON_CHIP_VCC_SENSE_ENA
                    + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ
                    + PLTFRM_ON_CHIP_VCC_SENSOR_OUTPUT_LEN + DIS_DATA_SCALE_FACTOR_TLV_SZ
#endif

#ifdef PLTFRM_HE055T01_SENSOR_ENA
                    + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ
                    + PLTFRM_HE055T01_SENSOR_OUTPUT_LEN + DIS_DATA_SCALE_FACTOR_TLV_SZ
#endif
                    ;

   if (pktLen == 0x0)
       SYS_fatal(SYS_FATAL_ERR_530);
   /*
    * The SYS_allocMem() function allocates contiguous memory buffer of the requested length.
    * It is implemented in system.c. Since this function can fail, you need to test the return
    * value of this function using the macro "SYS_BD_HNDL_IS_VALID(hndl)". If the function
    * succeeds in allocating the requested amount of memory, it will return a valid handle.
    */
   ADP_cntxt.txParams.bdHndl = SYS_allocMem(pktLen);
   if (SYS_BD_HNDL_IS_VALID(ADP_cntxt.txParams.bdHndl))
   {
       UINT8_t *buff_p, *tlvList_p;

       // Memory buffer was sucessfully allocated.

       buff_p = SYS_GET_BD_BUFF(ADP_cntxt.txParams.bdHndl);
       // buff_p points to the start of the memory buffer allocated by SYS_allocMem()

       buff_p += ADP_ELEMENT_TYPE_LEN;

       *buff_p = DIS_MSG_TYPE_SENSOR_OUTPUT;
       buff_p += DIS_MSG_TYPE_SZ;

       tlvList_p = buff_p;
       buff_p += UTIL_TLV_HDR_SZ;

#ifdef PLTFRM_ON_CHIP_VCC_SENSE_ENA
       {
          UINT16_t battVoltage;
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

#ifdef PLTFRM_HE055T01_SENSOR_ENA
       {
           UINT32_t currentMilliAmps;
           PLTFRM_sts_t sts;
           UINT8_t *tlvHdr_p = buff_p;

           buff_p += UTIL_TLV_HDR_SZ;

           sts = HE055T01_getSnsrOp(PLTFRM_HE055T01_1_DEV_ID, &currentMilliAmps);
           if (sts != PLTFRM_STS_SUCCESS)
           {
               currentMilliAmps = 0x7fffffff;
           }

           buff_p = UTIL_buildTlvHdr(buff_p,
                                     DIS_TLV_TYPE_SENSOR_ID,
                                     DIS_SENSOR_ID_FIELD_SZ);
           *(buff_p ++) = PLTFRM_HE055T01_1_DEV_ID;

           buff_p = UTIL_buildTlvHdr(buff_p,
                                     DIS_TLV_TYPE_VALUE,
                                     PLTFRM_HE055T01_SENSOR_OUTPUT_LEN);
           buff_p = UTIL_htonl(buff_p, currentMilliAmps);

           // Send the scale factor since the output is in milli-amps
           buff_p = UTIL_buildTlvHdr(buff_p,
                                     DIS_TLV_TYPE_DATA_SCALE_FACTOR,
                                     DIS_DATA_SCALE_FACTOR_FIELD_SZ);
           *(buff_p ++) = DIS_DATA_SCALE_MILLI;

           UTIL_buildTlvHdr(tlvHdr_p,
                            DIS_TLV_TYPE_SENSOR_OUTPUT,
                            buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }

#endif

       UTIL_buildTlvHdr(tlvList_p,
                        DIS_TLV_TYPE_SENSOR_OUTPUT_LIST,
                        buff_p - tlvList_p - UTIL_TLV_HDR_SZ);

       FFD_APP_dataTxReqCnt ++;

       ADP_cntxt.txParams.destShortAddr = LPWMN_COORD_MAC_SHORT_ADDR;  // The destination of this message
       ADP_cntxt.txParams.srcModId = ADP_TX_REQ_SRC_APP;

#ifdef CC1101_TIMING_DBG_ENA
       P1OUT |= BIT7;
#endif
       if (ADP_appPyldTxReq() != ADP_STS_SUCCESS)
       {
           FFD_APP_dataRxReqFlrCnt ++;
#ifdef FFD_APP_SEND_DATA_BACK_TO_BACK
           SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_APP_DATA_REQ;
           if (FFD_appRespTxFlag == 0)
               FFD_snsrDataTxPendFlag = 1;
#else
           APP_txInterval = FFD_APP_COMM_INTERVAL_IN_SECS;
           if (FFD_appRespTxFlag == 1)
           {
               SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_APP_DATA_REQ;
           }
#endif
       }
   }
   else
   {
       FFD_APP_memAllocFlrCnt ++;
       APP_txInterval = FFD_APP_commIntervalInSecs;
       if (FFD_appRespTxFlag == 1)
           SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_APP_DATA_REQ;
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
void FFD_APP_periodicTmrExpCbFunc(void)
{
   return;

   if (APP_nodeJoined == 0)
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

   SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_APP_DATA_REQ;

   FFD_snsrDataTxPendFlag = 1;

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
void APP_nodeJoinDoneInd(void)
{
   APP_nodeJoinDoneIndCnt ++;
   APP_nodeJoined = 1;
#ifdef FFD_APP_SEND_DATA_BACK_TO_BACK
   SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_APP_DATA_REQ;
   FFD_snsrDataTxPendFlag = 1;
#else
   APP_txInterval = FFD_APP_COMM_INTERVAL_IN_SECS;
#endif
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
              APP_txInterval = FFD_APP_COMM_INTERVAL_IN_SECS;
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
 ********************************************************************
 */
void APP_procGPIOEvt(void)
{
   return;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void APP_init(void)
{
   // Do nothing !!
}

#endif
#endif
