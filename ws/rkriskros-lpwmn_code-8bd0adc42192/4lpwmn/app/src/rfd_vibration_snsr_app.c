/*
 * File Name: rfd_vibration_sensor_app.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Sep/20/2015
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
#ifdef RFD_VIBRATION_SNSR_APP_ENA
#ifdef PLTFRM_ADXL345_ENA

#include <typedefs.h>
#include <system.h>
#include <pltfrm.h>
#include <util.h>
#include <mac_pib.h>
#include <adp.h>
#include <rfd_app.h>
#include <dis.h>
#include <adxl345.h>

#define APP_ACCEL_SAMPLE_CNT  128
SINT16_t APP_accelSampleArray[APP_ACCEL_SAMPLE_CNT];
SINT16_t APP_minAccelVal = 1000;
SINT16_t APP_maxAccelVal = -1000;


#define VIBRATION_STATE_NO   0
#define VIBRATION_STATE_YES  1
#define VIBRATION_STATE_UNKNOWN  2

#define VIBRATION_STATE_SZ  1

#define RFD_VIBRATION_SNSR_APP_MAX_UPDATE_INTERVAL  \
        (RFD_APP_snsrRepInterval * 3)

UINT16_t RFD_APP_dataRxReqFlrCnt = 0;
UINT16_t RFD_APP_dataTxReqCnt = 0;
UINT16_t RFD_APP_memAllocFlrCnt = 0;


UINT8_t APP_prevVibState = VIBRATION_STATE_UNKNOWN;

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
             + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + VIBRATION_STATE_SZ
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
          *(buff_p ++) = PLTFRM_VIBRATION_SNSR_1_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    VIBRATION_STATE_SZ);
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
       APP_prevVibState = VIBRATION_STATE_UNKNOWN;
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
	APP_prevVibState = VIBRATION_STATE_UNKNOWN;
}



/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void APP_checkVibration(UINT8_t devId, UINT8_t *vibState_p)
{
   PLTFRM_sts_t sts;
   UINT16_t idx;
   SINT32_t totAccelVal = 0, avgAccelVal;

   *vibState_p = VIBRATION_STATE_NO;

   sts = ADXL345_wakeUp(devId);
   if (sts != PLTFRM_STS_SUCCESS)
	   SYS_fatal(SYS_FATAL_ERR_3032);

   sts = ADXL345_startMeas(devId);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_3041);

   for (idx=0; idx<100; idx++)    // Drop initial samples ...
        PLTFRM_delay1MilliSec();

   APP_minAccelVal = 32000;
   APP_maxAccelVal = -32000;

   for (idx=0; idx<APP_ACCEL_SAMPLE_CNT; idx++)
   {
       SINT16_t accelVal;

       sts = ADXL345_readAccelData(devId, NULL, NULL, &accelVal);
       if (sts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_3033);

       APP_accelSampleArray[idx] = accelVal;

       // Put micro to sleep here ..

       PLTFRM_delay625MicroSecs();   // 1600 HZ, B/W : 800 HZ
   }

   sts = ADXL345_stopMeas(devId);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_3042);

   sts = ADXL345_putToSleep(devId);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_3034);

   // Process the sampled data here ...

   for (idx=0; idx<APP_ACCEL_SAMPLE_CNT; idx++)
   {
       SINT16_t accelVal = APP_accelSampleArray[idx];

       accelVal *= 100;

       totAccelVal += accelVal;

       if (accelVal > APP_maxAccelVal)
    	   APP_maxAccelVal = accelVal;

       if (accelVal < APP_minAccelVal)
    	   APP_minAccelVal = accelVal;
   }

   avgAccelVal = totAccelVal;

#if (APP_ACCEL_SAMPLE_CNT == 128)
   avgAccelVal >>= 7;
#elif (APP_ACCEL_SAMPLE_CNT == 64)
   avgAccelVal >>= 6;
#elif (APP_ACCEL_SAMPLE_CNT == 32)
   avgAccelVal >>= 5;
#else
#error !!
#endif

   if (avgAccelVal < 200)
   {
       SINT32_t totDiffSquared = 0, avgDiffSquared;

       avgAccelVal /= 100;

       for (idx=0; idx<APP_ACCEL_SAMPLE_CNT; idx++)
       {
           SINT32_t diff = APP_accelSampleArray[idx];

           diff -= avgAccelVal;
           diff *= diff;

           totDiffSquared += diff;
       }

       avgDiffSquared = totDiffSquared;

#if (APP_ACCEL_SAMPLE_CNT == 128)
       avgDiffSquared >>= 7;
#elif (APP_ACCEL_SAMPLE_CNT == 64)
       avgDiffSquared >>= 6;
#elif (APP_ACCEL_SAMPLE_CNT == 32)
       avgDiffSquared >>= 5;
#else
#error !!
#endif

       if (avgDiffSquared > 100)
           *vibState_p = VIBRATION_STATE_YES;
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
void RFD_APP_wakeUpProc( )
{
   UINT8_t currVibState;

   if (!MAC_nodeJoined())
	   return;

   APP_checkVibration(PLTFRM_ADXL345_1_DEV_ID, &currVibState);

   APP_timeSinceLastUpdate += SYS_wakeUpIntervalSecs;

   if (APP_prevVibState != currVibState
       || APP_timeSinceLastUpdate >= RFD_VIBRATION_SNSR_APP_MAX_UPDATE_INTERVAL)
   {
       RFD_APP_buildSendSnsrUpdate(currVibState);

       APP_prevVibState = currVibState;

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
   UINT8_t devId = PLTFRM_ADXL345_1_DEV_ID;
   PLTFRM_sts_t sts;

   sts = ADXL345_setRange(devId, ADXL345_RANGE_16G);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_3035);

   sts = ADXL345_setResolution(devId, ADXL345_RESOLUTION_10_BITS_FIXED);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_3036);

   sts = ADXL345_setDataLayoutMode(devId, ADXL345_ACCEL_DATA_RIGHT_JUSTIFIED);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_3037);

   sts = ADXL345_cfgFIFO(devId, ADXL345_FIFO_MODE_BYPASS);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_3038);

   sts = ADXL345_setOutputRate(devId, ADXL345_OUTPUT_DATA_RATE_1600_HZ);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_3039);

   // Configure sleep mode sampling rate to the minimum (1 HZ)
   // Disable activity detection on each axis
   // Disable inactivity detection on each axis

   sts = ADXL345_putToSleep(devId);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_3040);
}

#endif
#endif
#endif
