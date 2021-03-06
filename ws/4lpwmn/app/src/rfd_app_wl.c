/*
 * File Name: rfd_app_wl.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Feb/11/2015
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
#ifdef RFD_APP_WL
#if defined(PLTFRM_MP3V5050GP_ENA) && defined(PLTFRM_ON_CHIP_VCC_SENSE_ENA)
#include <typedefs.h>
#include <system.h>
#include <pltfrm.h>
#include <util.h>
#include <mac_pib.h>
#include <adp.h>
#include <rfd_app.h>
#include <dis.h>
#include <mp3v5050gp.h>


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
void RFD_APP_buildSendSnsrUpdate(void)
{
   const UINT8_t pktLen = ADP_ELEMENT_TYPE_LEN
                          + DIS_MSG_TYPE_SZ
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT_LIST
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + MP3V5050GP_SENSOR_OUTPUT_VALUE_SZ + DIS_DATA_SCALE_FACTOR_TLV_SZ
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + PLTFRM_ON_CHIP_VCC_SENSOR_OUTPUT_LEN + DIS_DATA_SCALE_FACTOR_TLV_SZ;
   UINT8_t flrFlag = 0;
   UINT8_t *tlvHdr_p;

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
       UINT32_t suppVoltage, snsrOp32;

       // Memory buffer was successfully allocated.

       buff_p = SYS_GET_BD_BUFF(ADP_cntxt.txParams.bdHndl);
       // buff_p points to the start of the memory buffer allocated by SYS_allocMem()

       buff_p += ADP_ELEMENT_TYPE_LEN;

       *buff_p = DIS_MSG_TYPE_SENSOR_OUTPUT;
       buff_p += DIS_MSG_TYPE_SZ;

       tlvList_p = buff_p;
       buff_p += UTIL_TLV_HDR_SZ;

       tlvHdr_p = buff_p;

       buff_p += UTIL_TLV_HDR_SZ;

       buff_p = UTIL_buildTlvHdr(buff_p,
                                 DIS_TLV_TYPE_SENSOR_ID,
                                 DIS_SENSOR_ID_FIELD_SZ);
       *(buff_p ++) = PLTFRM_ON_CHIP_VCC_SENSOR_DEV_ID;

       suppVoltage = pltfrm_vccSense(1);

       if (MP3V5050GP_getSensorOutput(PLTFRM_MP3V5050GP_1_DEV_ID,
                                      &snsrOp32) != PLTFRM_STS_SUCCESS)
       {
           SYS_fatal(SYS_FATAL_ERR_548);
       }

       buff_p = UTIL_buildTlvHdr(buff_p,
                                 DIS_TLV_TYPE_VALUE,
                                 PLTFRM_ON_CHIP_VCC_SENSOR_OUTPUT_LEN);
       buff_p = UTIL_htons(buff_p, suppVoltage);

       // Send the scale factor since the output is in milli-volts
       buff_p = UTIL_buildTlvHdr(buff_p,
                                 DIS_TLV_TYPE_DATA_SCALE_FACTOR,
                                 DIS_DATA_SCALE_FACTOR_FIELD_SZ);
       *(buff_p ++) = DIS_DATA_SCALE_MILLI;

       UTIL_buildTlvHdr(tlvHdr_p,
                        DIS_TLV_TYPE_SENSOR_OUTPUT,
                        buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);

       tlvHdr_p = buff_p;

       buff_p += UTIL_TLV_HDR_SZ;

       // Store the sampled value in the message buffer
       buff_p = UTIL_buildTlvHdr(buff_p,
                                 DIS_TLV_TYPE_SENSOR_ID,
                                 DIS_SENSOR_ID_FIELD_SZ);
       *(buff_p ++) = PLTFRM_MP3V5050GP_1_DEV_ID;

       buff_p = UTIL_buildTlvHdr(buff_p,
                                 DIS_TLV_TYPE_VALUE,
                                 MP3V5050GP_SENSOR_OUTPUT_VALUE_SZ);
       buff_p = UTIL_htonl(buff_p, snsrOp32);

       // Send the scale factor since the output is in milli-volts
       buff_p = UTIL_buildTlvHdr(buff_p,
                                 DIS_TLV_TYPE_DATA_SCALE_FACTOR,
                                 DIS_DATA_SCALE_FACTOR_FIELD_SZ);
       *(buff_p ++) = DIS_DATA_SCALE_MILLI;

       UTIL_buildTlvHdr(tlvHdr_p,
                        DIS_TLV_TYPE_SENSOR_OUTPUT,
                        buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);

       UTIL_buildTlvHdr(tlvList_p,
                        DIS_TLV_TYPE_SENSOR_OUTPUT_LIST,
                        buff_p - tlvList_p - UTIL_TLV_HDR_SZ);

       ADP_cntxt.txParams.destShortAddr = LPWMN_COORD_MAC_SHORT_ADDR;  // The destination of this message
       ADP_cntxt.txParams.srcModId = ADP_TX_REQ_SRC_APP;

       /*
        * Call the ADP layer to send the packet over the mesh network to the coordinator.
        */
       if (ADP_appPyldTxReq() != ADP_STS_SUCCESS)
       {
           flrFlag = 1;
       }
   }
   else
   {
       flrFlag = 1;
   }

   if (flrFlag)
   {
       SYS_SET_WAKEUP_INTERVAL(RFD_APP_snsrRepInterval);
       SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ENTER_LPM_REQUEST;
   }

   return;
}


/*
 ********************************************************************
 * Function - RFD_APP_wakeUpProc
 * Arguements - None
 * Description - This function is called whenever the RFD wakes up
 * from deep sleep mode. Note that the RFD went into deep sleep because
 * of a prior request from this APP. This APP sets the sleep duration (in
 * seconds) using the macro "SYS_SET_WAKEUP_INTERVAL(interval_in_secs)"
 * before requesting the "SYSTEM" module to put the RFD into deep sleep.
 *
 ********************************************************************
 */
void RFD_APP_wakeUpProc( )
{
   // rkris@wisense.in / may/3/15
   if (!MAC_nodeJoined())
       return;

   RFD_APP_buildSendSnsrUpdate();
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_init(void)
{

}


#endif
#endif
#endif
