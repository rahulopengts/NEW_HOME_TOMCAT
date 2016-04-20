/*
 * File Name: rfd_app_vd.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Nov/23/2015
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
#if defined(RFD_APP_VD) && defined(PLTFRM_MAG3110_SENSOR_ENA)

#include <typedefs.h>
#include <string.h>
#include <system.h>
#include <pltfrm.h>
#include <util.h>
#include <mac_pib.h>
#include <adp.h>
#include <rfd_app.h>
#include <dis.h>
#include <mag3110.h>


typedef struct
{
   UINT8_t settled;
   UINT32_t sampleCnt;
   SINT32_t yc, yp, prevMFS, currMFS;
} HPF_cntxt_s;


UINT16_t RFD_APP_dataRxReqFlrCnt = 0;
UINT16_t RFD_APP_dataTxReqCnt = 0;
UINT16_t RFD_APP_memAllocFlrCnt = 0;

HPF_cntxt_s HPF_X_cntxt;
HPF_cntxt_s HPF_Y_cntxt;
HPF_cntxt_s HPF_Z_cntxt;

UINT8_t VD_HPF_alpha =  7;
UINT16_t VD_HPF_detectionTh = 30;
UINT16_t VD_HPF_settlingTh = 10;

SINT16_t __XmagFS = 0, __YmagFS = 0, __ZmagFS = 0;

/*
 ********************************************************************
 * Function:
 * RFD_APP_buildSendSnsrUpdate
 *
 * Description:
 * This function samples the three sensors on this node. It builds a message
 * with the sensor data and then requests the ADP (adaptation) layer to
 * send the packet out over the mesh network to the network coordinator (short
 * address 0x0001).
 *
 ********************************************************************
 */
void RFD_APP_buildSendSnsrUpdate(UINT8_t vdFlag)
{
   UINT16_t battVoltage;
   UINT8_t flrFlag = 0;
   const UINT8_t pktLen = ADP_ELEMENT_TYPE_LEN
                          + DIS_MSG_TYPE_SZ
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT_LIST
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
#ifdef PLTFRM_ON_CHIP_VCC_SENSE_ENA
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + PLTFRM_ON_CHIP_VCC_SENSOR_OUTPUT_LEN
                          + DIS_DATA_SCALE_FACTOR_TLV_SZ
#endif
                          + UTIL_TLV_HDR_SZ
                          + DIS_SENSOR_ID_TLV_SZ
						  + UTIL_TLV_HDR_SZ + MAG3110_SENSOR_OUTPUT_LEN

                          + UTIL_TLV_HDR_SZ
						  + DIS_SENSOR_ID_TLV_SZ
						  + UTIL_TLV_HDR_SZ + 1;

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
          SINT16_t magFieldStrength;
          PLTFRM_sts_t sts;

          buff_p += UTIL_TLV_HDR_SZ;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_MAG3110_1_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
									MAG3110_SENSOR_OUTPUT_LEN);


          buff_p = UTIL_htons(buff_p, __XmagFS);
          buff_p = UTIL_htons(buff_p, __YmagFS);
          buff_p = UTIL_htons(buff_p, __ZmagFS);

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }

       {
           UINT8_t *tlvHdr_p = buff_p;

           buff_p += UTIL_TLV_HDR_SZ;

           buff_p = UTIL_buildTlvHdr(buff_p,
                                     DIS_TLV_TYPE_SENSOR_ID,
                                     DIS_SENSOR_ID_FIELD_SZ);
           *(buff_p ++) = PLTFRM_WS_VEH_DET_1_DEV_ID;

           buff_p = UTIL_buildTlvHdr(buff_p,
                                     DIS_TLV_TYPE_VALUE,
 									 1);
           *(buff_p ++) = vdFlag;

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
   }
   else
   {
       flrFlag = 1;
       RFD_APP_memAllocFlrCnt ++;
   }

   if (flrFlag)
   {
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
void RFD_APP_procGPIOEvt(void)
{
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_VD_intTmoHndlr(void)
{
   // No need to do anything here. The purpose of the timer interrupt
   // is to wake up the micro. After waking up, RFD_APP_wakeUpProc( )
   // will be called.
   return;
}


#ifdef VEHICLE_DETECTION_USING_HIGH_PASS_FILTER_ENA
/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
UINT8_t __HPF(HPF_cntxt_s *cntxt_p, SINT32_t currMFS)
{
   UINT8_t rc =  0;

   // y[0] := x[0]
   // y[i] := K * y[i-1] + K * (x[i] - x[i-1])

   cntxt_p->currMFS = currMFS;

   if (cntxt_p->sampleCnt == 0)
   {
       cntxt_p->yc = currMFS;
       cntxt_p->settled = 0;
   }
   else
   {
       SINT16_t absYc;

       cntxt_p->yc = VD_HPF_alpha;
       cntxt_p->yc *= (cntxt_p->yp + cntxt_p->currMFS - cntxt_p->prevMFS);
       cntxt_p->yc /= 10;

       absYc = abs(cntxt_p->yc);

       if (cntxt_p->settled)
       {
           if (absYc > VD_HPF_detectionTh)
               rc = 1;
       }
       else
       {
           if (absYc < VD_HPF_settlingTh)
               cntxt_p->settled = 1;
       }
   }

   cntxt_p->prevMFS = currMFS;
   cntxt_p->yp = cntxt_p->yc;
   cntxt_p->sampleCnt ++;

   return rc;
}

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void __HPF_init(void)
{
   memset(&HPF_X_cntxt, 0, sizeof(HPF_cntxt_s));
   memset(&HPF_Y_cntxt, 0, sizeof(HPF_cntxt_s));
   memset(&HPF_Z_cntxt, 0, sizeof(HPF_cntxt_s));
}
#endif

UINT16_t __hpfOutThExceededCnt = 0;

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_wakeUpProc(void)
{
   UINT16_t tmoMilliSec;
   PLTFRM_sts_t sts;

   if (!MAC_nodeJoined())
       return;

   switch (RFD_APP_state)
   {
       case RFD_APP_STATE_WAIT_CONVERSION:
            {
#ifdef VEHICLE_DETECTION_USING_HIGH_PASS_FILTER_ENA
               UINT8_t hpfOut = 0;

               sts = MAG3110_getMeasData(PLTFRM_MAG3110_1_DEV_ID, MAG3110_X_AXIS_ID, &__XmagFS);
               if (sts != PLTFRM_STS_SUCCESS)
                   SYS_fatal(SYS_FATAL_ERR_6050);
               hpfOut = __HPF(&HPF_X_cntxt, __XmagFS);

               sts = MAG3110_getMeasData(PLTFRM_MAG3110_1_DEV_ID, MAG3110_Y_AXIS_ID, &__YmagFS);
               if (sts != PLTFRM_STS_SUCCESS)
                   SYS_fatal(SYS_FATAL_ERR_6051);
               hpfOut += __HPF(&HPF_Y_cntxt, __YmagFS);

               sts = MAG3110_getMeasData(PLTFRM_MAG3110_1_DEV_ID, MAG3110_Z_AXIS_ID, &__ZmagFS);
               if (sts != PLTFRM_STS_SUCCESS)
                   SYS_fatal(SYS_FATAL_ERR_6052);
               hpfOut += __HPF(&HPF_Z_cntxt, __ZmagFS);

               if (hpfOut >= 2)
            	   __hpfOutThExceededCnt ++;

               if ((hpfOut >= 2)
                   || (PLTFRM_secsElapsed(RFD_APP_lastSnsrUpdateSentTS)) >= RFD_APP_maxSnsrMsgTxInterval)
#endif
               {
                   RFD_APP_buildSendSnsrUpdate(hpfOut >= 2 ? 1 : 0);
                   RFD_APP_lastSnsrUpdateSentTS = PLTFRM_currTimeStamp();
               }
               else
               {
            	   SYS_SET_WAKEUP_INTERVAL(RFD_APP_snsrRepInterval);
            	   SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ENTER_LPM_REQUEST;
               }

               RFD_APP_state = RFD_APP_STATE_CONVERSION_DONE;
            }
            break;

       case RFD_APP_STATE_CONVERSION_DONE:
            {
               sts = MAG3110_triggerMeas(PLTFRM_MAG3110_1_DEV_ID, &tmoMilliSec);
               if (sts != PLTFRM_STS_OPN_IN_PROGRESS)
            	   SYS_fatal(SYS_FATAL_ERR_6010);
               else
               {
                   if (tmoMilliSec > 1000)   // Not sampling one shot sampling interval greater than 1 second
                       SYS_fatal(SYS_FATAL_ERR_6001);

                   sts = PLTFRM_startTimerA1(0, tmoMilliSec, RFD_APP_VD_intTmoHndlr);
                   if (sts == PLTFRM_STS_SUCCESS)
                   {
                       SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ENTER_LPM_REQUEST;
                       RFD_APP_state = RFD_APP_STATE_WAIT_CONVERSION;

                       /*
                        * Disable periodic wake up since we have started a timer above which
                        * will wake up the micro after "tmoVal" seconds. Periodic wake up
                        * can be disabled by calling the "SYS_SET_WAKEUP_INTERVAL( )" macro
                        * with argument "0".
                        */
                       SYS_SET_WAKEUP_INTERVAL(0);
                   }
                   else
                       SYS_fatal(SYS_FATAL_ERR_6002);
               }
            }
            break;

       default:
            {
               SYS_fatal(SYS_FATAL_ERR_6000);
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
void RFD_APP_init(void)
{
   // Nothing to be done

   RFD_APP_maxSnsrMsgTxInterval = 10;  // Seconds

#ifdef VEHICLE_DETECTION_USING_HIGH_PASS_FILTER_ENA
   __HPF_init( );
#endif

}

#endif
#endif
