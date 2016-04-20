/*
 * File Name: mds_app.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Sep/14/2015
 *
 *
 * Copyright (c) <2015>, <ram krishnan>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
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
#ifdef RFD_APP_MDS
#ifdef PLTFRM_MAINS_DETECT_SNSR_ENA

#include <typedefs.h>
#include <system.h>
#include <pltfrm.h>
#include <util.h>
#include <mac_pib.h>
#include <adp.h>
#include <rfd_app.h>
#include <dis.h>
#include <mds.h>


#define MDS_RFD_APP_MAX_UPDATE_INTERVAL  60

UINT16_t RFD_APP_dataRxReqFlrCnt = 0;
UINT16_t RFD_APP_dataTxReqCnt = 0;
UINT16_t RFD_APP_memAllocFlrCnt = 0;


UINT8_t APP_mainsPrevState = MDS_STATE_UNKNOWN;

UINT32_t APP_timeSinceLastUpdate = 0;

/*
 ********************************************************************
 * Function:
 * RFD_APP_buildSendSnsrUpdate
 *
 ********************************************************************
 */
void RFD_APP_buildSendSnsrUpdate(UINT8_t mainsState)
{
   UINT16_t battVoltage;
   UINT8_t pktLen = ADP_ELEMENT_TYPE_LEN
                    + DIS_MSG_TYPE_SZ
                    + UTIL_TLV_HDR_SZ;   // DIS_TLV_TYPE_SENSOR_OUTPUT_LIST
   UINT8_t flrFlag = 0;

#ifdef PLTFRM_ON_CHIP_VCC_SENSE_ENA
   pktLen += UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
              + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + PLTFRM_ON_CHIP_VCC_SENSOR_OUTPUT_LEN
              + DIS_DATA_SCALE_FACTOR_TLV_SZ;
#endif

   pktLen += UTIL_TLV_HDR_SZ
             + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + MDS_OUTPUT_VALUE_SZ
             + DIS_DATA_SCALE_FACTOR_TLV_SZ;

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

       {
          UINT8_t *tlvHdr_p = buff_p;

          buff_p += UTIL_TLV_HDR_SZ;

          battVoltage = pltfrm_vccSense(1);

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_MDS_1_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    MDS_OUTPUT_VALUE_SZ);
          *(buff_p ++) = mainsState;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_DATA_SCALE_FACTOR,
                                    DIS_DATA_SCALE_FACTOR_FIELD_SZ);
          *(buff_p ++) = DIS_DATA_SCALE_NONE;

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }

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
       SYS_SET_WAKEUP_INTERVAL(RFD_APP_snsrRepInterval);
       SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ENTER_LPM_REQUEST;
       APP_mainsPrevState = MDS_STATE_UNKNOWN;
   }

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
void MDS_APP_cnfrmFlrInd(void)
{
   APP_mainsPrevState = MDS_STATE_UNKNOWN;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_wakeUpProc( )
{
   UINT8_t mainsCurrState;

   if (!MAC_nodeJoined())
	   return;

   if (MDS_getCurrState(PLTFRM_MDS_1_DEV_ID, &mainsCurrState) != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_3031);

   APP_timeSinceLastUpdate += SYS_wakeUpIntervalSecs;

   if (APP_mainsPrevState != mainsCurrState
       || APP_timeSinceLastUpdate >= MDS_RFD_APP_MAX_UPDATE_INTERVAL)
   {
       RFD_APP_buildSendSnsrUpdate(mainsCurrState);

       APP_mainsPrevState = mainsCurrState;

       APP_timeSinceLastUpdate = 0;
   }
   else
   {
       SYS_SET_WAKEUP_INTERVAL(RFD_APP_snsrRepInterval);
       SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ENTER_LPM_REQUEST;
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
   // Nothing to be done
}

#endif
#endif
#endif
