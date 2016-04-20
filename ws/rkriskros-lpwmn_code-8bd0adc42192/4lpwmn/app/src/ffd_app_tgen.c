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
#ifdef FFD_APP_PKT_GEN

#include <typedefs.h>
#include <system.h>
#include <pltfrm.h>
#include <util.h>
#include <mac_pib.h>
#include <adp.h>
#include <dis.h>
#include <ffd_app_common.h>


UINT8_t APP_bdHndl;
UINT8_t *APP_fixedBdBuff_p;
UINT8_t APP_fixedBdLen;


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
   ADP_cntxt.txParams.bdHndl = SYS_allocMem(APP_fixedBdLen);
   if (SYS_BD_HNDL_IS_VALID(ADP_cntxt.txParams.bdHndl))
   {
       UINT8_t *buff_p = SYS_GET_BD_BUFF(ADP_cntxt.txParams.bdHndl);

       memcpy(buff_p, APP_fixedBdBuff_p, APP_fixedBdLen);

       FFD_APP_dataTxReqCnt ++;

       ADP_cntxt.txParams.destShortAddr = LPWMN_COORD_MAC_SHORT_ADDR;  // The destination of this message
       ADP_cntxt.txParams.srcModId = ADP_TX_REQ_SRC_APP;

       if (ADP_appPyldTxReq() != ADP_STS_SUCCESS)
       {
           FFD_APP_dataRxReqFlrCnt ++;
#ifdef FFD_APP_SEND_DATA_BACK_TO_BACK
           SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_APP_DATA_REQ;
           if (FFD_appRespTxFlag == 0)
               FFD_snsrDataTxPendFlag = 1;
#else
           APP_txInterval = FFD_APP_commIntervalInSecs;
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
void FFD_appInit(void)
{
   APP_fixedBdLen = ADP_ELEMENT_TYPE_LEN
		            + DIS_MSG_TYPE_SZ
                    + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT_LIST

#ifdef PLTFRM_ON_CHIP_VCC_SENSE_ENA
                    + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                    + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + PLTFRM_ON_CHIP_VCC_SENSOR_OUTPUT_LEN + DIS_DATA_SCALE_FACTOR_TLV_SZ
#endif
#ifdef PLTFRM_ON_CHIP_VCC_SENSE_ENA
                    + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                    + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + PLTFRM_ON_CHIP_VCC_SENSOR_OUTPUT_LEN + DIS_DATA_SCALE_FACTOR_TLV_SZ
#endif
#ifdef PLTFRM_ON_CHIP_VCC_SENSE_ENA
                    + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                    + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + PLTFRM_ON_CHIP_VCC_SENSOR_OUTPUT_LEN + DIS_DATA_SCALE_FACTOR_TLV_SZ
#endif
#ifdef PLTFRM_ON_CHIP_VCC_SENSE_ENA
                    + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                    + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + PLTFRM_ON_CHIP_VCC_SENSOR_OUTPUT_LEN + DIS_DATA_SCALE_FACTOR_TLV_SZ - 1;
#endif
#ifdef APP_MAX_THROUGHPUT_TEST
   APP_fixedBdLen = 51;
#endif
	                    ;
   if (APP_fixedBdLen == 0x0)
	  SYS_fatal(SYS_FATAL_ERR_530);

   APP_bdHndl = SYS_allocMem(APP_fixedBdLen);
   if (SYS_BD_HNDL_IS_VALID(APP_bdHndl))
   {
	   UINT8_t *tlvList_p, *buff_p;

	   APP_fixedBdBuff_p = SYS_GET_BD_BUFF(ADP_cntxt.txParams.bdHndl);
       buff_p = APP_fixedBdBuff_p;

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
	                                0);
	      // *(buff_p ++) = DIS_DATA_SCALE_MILLI;

	      UTIL_buildTlvHdr(tlvHdr_p,
	                       DIS_TLV_TYPE_SENSOR_OUTPUT,
	                       buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
	   }
#endif


       UTIL_buildTlvHdr(tlvList_p,
                        DIS_TLV_TYPE_SENSOR_OUTPUT_LIST,
                        buff_p - tlvList_p - UTIL_TLV_HDR_SZ);
   }
   else
       SYS_fatal(SYS_FATAL_ERR_531);

   return;
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


#endif
#endif
