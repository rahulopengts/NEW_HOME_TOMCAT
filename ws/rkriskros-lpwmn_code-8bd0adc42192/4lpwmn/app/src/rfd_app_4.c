/*
 * File Name: rfd_app_4.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Jan/9/2014
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

#ifdef DEV_TYPE_RFD

#ifdef RFD_APP_4

#if defined(PLTFRM_EKMC160111X_ENA) \
    && defined(PLTFRM_ON_CHIP_VCC_SENSE_ENA)

#include <typedefs.h>
#include <system.h>
#include <pltfrm.h>
#include <util.h>
#include <mac_pib.h>
#include <adp.h>
#include <rfd_app.h>
#include <ekmc160111x.h>
#include <dis.h>


#ifdef PLTFRM_LM75B_TEMP_SENSOR_ENA
#include <lm75b.h>
#endif

#ifdef PLTFRM_TSL45315_ENA
#include <tsl45315.h>
#endif

#define MOTION_SENSED_CNT_FIELD_SZ  4




#define RFD_APP_DATA_SEQ_NR_SZ   2
#define RFD_APP_DATA_SNSR_BIT_MSK_SZ   1

UINT16_t RFD_APP_pktSeqNr = 0;

// Stats
UINT16_t RFD_APP_dataTxReqCnt = 0;
UINT16_t RFD_APP_dataRxReqFlrCnt = 0;
UINT16_t RFD_APP_memAllocFlrCnt = 0;

#ifdef EKM160111X_ENA_ON_PWR_UP
UINT8_t RFD_APP_cktStbWait = 0;
#endif

UINT32_t RFD_APP_motionSenseCnt = 0;

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_buildSendSnsrUpdate(UINT32_t motionDetected)
{
   UINT8_t flrFlag = 0;
   UINT8_t pktLen =  ADP_ELEMENT_TYPE_LEN
                      + DIS_MSG_TYPE_SZ
                      + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT_LIST
#ifdef PLTFRM_ON_CHIP_VCC_SENSE_ENA
                      + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                      + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + PLTFRM_ON_CHIP_VCC_SENSOR_OUTPUT_LEN
                      + DIS_DATA_SCALE_FACTOR_TLV_SZ
#endif
#ifdef PLTFRM_EKMC160111X_ENA
                      + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                      + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + MOTION_SENSED_CNT_FIELD_SZ
#endif

#ifdef PLTFRM_LM75B_TEMP_SENSOR_ENA
                      + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                      + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + LM75B_SENSOR_OUTPUT_VALUE_SZ
#endif
#ifdef PLTFRM_TSL45315_ENA
                      + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                      + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + TSL45315_SENSOR_OUTPUT_VALUE_SZ
#endif
                     ;

   ADP_cntxt.txParams.bdHndl = SYS_allocMem(pktLen);
   if (SYS_BD_HNDL_IS_VALID(ADP_cntxt.txParams.bdHndl))
   {
       UINT8_t *buff_p, *tlvList_p;

       // Memory buffer was successfully allocated.

       buff_p = SYS_GET_BD_BUFF(ADP_cntxt.txParams.bdHndl);
       // buff_p points to the start of the memory buffer allocated by SYS_allocMem()

       buff_p += ADP_ELEMENT_TYPE_LEN;

       *buff_p = DIS_MSG_TYPE_SENSOR_OUTPUT;
       buff_p += DIS_MSG_TYPE_SZ;

       tlvList_p = buff_p;
       buff_p += UTIL_TLV_HDR_SZ;

#ifdef PLTFRM_ON_CHIP_VCC_SENSE_ENA
       {
          UINT8_t *tlvHdr_p = buff_p;
          UINT16_t battVoltage;

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

#ifdef PLTFRM_LM75B_TEMP_SENSOR_ENA
       {
          SINT16_t temp16;
          UINT8_t *tlvHdr_p = buff_p;

          buff_p += UTIL_TLV_HDR_SZ;

          // Call the LM75B driver's function to get the last temperature value
          // sampled by the LM75B sensor. The driver code is in the file "pltfrm/
          // src/lm75b.c".
          if (LM75B_getTempVal(PLTFRM_LM75B_1_DEV_ID, &temp16) != PLTFRM_STS_SUCCESS)
          {
              SYS_fatal(SYS_FATAL_ERR_6005);
          }

          // Store the sampled value in the message buffer
          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_LM75B_1_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    LM75B_SENSOR_OUTPUT_VALUE_SZ);
          buff_p = UTIL_htons(buff_p, temp16);

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }
#endif

#ifdef PLTFRM_TSL45315_ENA
       {
          UINT8_t *tlvHdr_p = buff_p;
          SINT32_t lux32;

          buff_p += UTIL_TLV_HDR_SZ;

          /*
           * Call the TSL45315 driver's function to get the last light intensity
           * value sampled by the TSL45315 sensor. The driver code is in the file
           * "pltfrm/src/tsl45315.c".
           */
          if (TSL45315_getALSOutput(PLTFRM_TSL45315_1_DEV_ID, &lux32) != PLTFRM_STS_SUCCESS)
          {
              SYS_fatal(SYS_FATAL_ERR_83);
          }

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_TSL45315_1_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    TSL45315_SENSOR_OUTPUT_VALUE_SZ);
          buff_p = UTIL_htonl(buff_p, lux32);

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }
#endif

#ifdef PLTFRM_EKMC160111X_ENA
       {
          UINT8_t *tlvHdr_p = buff_p;

          buff_p += UTIL_TLV_HDR_SZ;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_EKMC160111X_1_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    MOTION_SENSED_CNT_FIELD_SZ);
          buff_p = UTIL_htonl(buff_p, motionDetected);

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }
#endif

       UTIL_buildTlvHdr(tlvList_p,
                        DIS_TLV_TYPE_SENSOR_OUTPUT_LIST,
                        buff_p - tlvList_p - UTIL_TLV_HDR_SZ);

       RFD_APP_dataTxReqCnt ++;

       ADP_cntxt.txParams.destShortAddr = LPWMN_COORD_MAC_SHORT_ADDR;  // The destination of this message
       ADP_cntxt.txParams.srcModId = ADP_TX_REQ_SRC_APP;

       /*
        * Call the ADP layer to send the packet over the mesh network to the coordinator.
        */
       if (ADP_appPyldTxReq() != ADP_STS_SUCCESS)
       {
           flrFlag = 1;
           RFD_APP_dataRxReqFlrCnt ++;
       }
       else
       {
    	   RFD_APP_lastTxMsgType = DIS_MSG_TYPE_SENSOR_OUTPUT;
       }
   }
   else
   {
       flrFlag = 1;
       RFD_APP_memAllocFlrCnt ++;
   }

   if (flrFlag)
   {
       // If ADP_appPyldTxReq() fails and this message was trigerred by
       // the PIR sensor, cancel the debounce so that another transmission
       // can be attempted if the PIR interrupts again.
       if (motionDetected > 0)
           EKMC160111X_debounceIntervalSecs = 0;

       SYS_SET_WAKEUP_INTERVAL(RFD_APP_snsrRepInterval);
       SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ENTER_LPM_REQUEST;
   }
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_wakeUpProc( )
{
   UINT8_t evtPendFlag = 0;

#ifdef EKM160111X_ENA_ON_PWR_UP
   if (RFD_APP_cktStbWait)
   {
	   RFD_APP_cktStbWait = 0;
	   EKMC160111X_unMaskDevEvent(PLTFRM_EKMC160111X_1_DEV_ID);
	   SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_JOIN_NWK_REQ;
	   return;
   }
#endif

   // rkris@wisense.in / may/3/15
   if (!MAC_nodeJoined())
       return;

   if (EKMC160111X_debounceIntervalSecs > 0)
   {
       SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ENTER_LPM_REQUEST;
       return;
   }

   if (EKMC160111X_debounceIntervalSecs == -1)
   {
       // RFD woken up when debounce interval has elapsed. No need
       // to send another message.

       EKMC160111X_unMaskDevEvent(PLTFRM_EKMC160111X_1_DEV_ID);

       EKMC160111X_debounceIntervalSecs = 0;
   }

   if (EKMC160111X_checkPendingEvt(PLTFRM_EKMC160111X_1_DEV_ID, &evtPendFlag) == PLTFRM_STS_SUCCESS)
   {
	   if (evtPendFlag)
	   {
		   RFD_APP_motionSenseCnt ++;
           EKMC160111X_debounceIntervalSecs = EKMC160111X_DFLT_DEBOUNCE_TIME_SECS;
       }

	   RFD_APP_buildSendSnsrUpdate(evtPendFlag ? RFD_APP_motionSenseCnt : 0);
   }
   else
       SYS_fatal(SYS_FATAL_ERR_6007);

}

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_wakeUpTimerExpEvt(void)
{
   return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void APP_procGPIOEvt()
{
   return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_snsrCtrlUpdate(UINT8_t snsrCntrlUpdate)
{
   if (snsrCntrlUpdate == 0)
   {
       // Power down the sensor
       if (EKMC160111X_powerDevDown(PLTFRM_EKMC160111X_1_DEV_ID) != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_6008);

       RFD_APP_updatePushInterval(PIR_SENSOR_PWR_DOWN_PUSH_INTERVAL);
   }
   else
   {
       //  Power up the sensor
       if (EKMC160111X_powerDevUp(PLTFRM_EKMC160111X_1_DEV_ID) != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_6009);

       RFD_APP_updatePushInterval(PIR_SENSOR_PWR_UP_PUSH_INTERVAL);
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
void RFD_APP_init(void)
{
#ifdef PLTFRM_LM75B_TEMP_SENSOR_ENA
   if (LM75B_cfg(PLTFRM_LM75B_1_DEV_ID, LM75B_OPN_MODE_NORMAL) != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_6004);
#endif

#ifdef PLTFRM_TSL45315_ENA
   if (TSL45315_cfgALSOpnMode(PLTFRM_TSL45315_1_DEV_ID, TSL45315_OPN_MODE_NORMAL, NULL) != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_6006);
#endif

   // Micro will wake up only when interrupted by the motion sensor


#ifdef EKM160111X_ENA_ON_PWR_UP
   RFD_APP_cktStbWait = 1;
   EKMC160111X_powerDevUp(PLTFRM_EKMC160111X_1_DEV_ID);
   SYS_SET_WAKEUP_INTERVAL(EKMC160111X_CKT_STABILITY_TIME_SECS + 5);
   SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ENTER_LPM_REQUEST;
#endif

   return;
}

#endif
#endif
#endif
