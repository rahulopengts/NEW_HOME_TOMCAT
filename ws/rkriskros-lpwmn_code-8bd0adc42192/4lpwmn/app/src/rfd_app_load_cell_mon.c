/*
 * File Name: rfd_app_load_cell_mon.c
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
#ifdef RFD_APP_LOAD_CELL_MON

#include <typedefs.h>
#include <system.h>
#include <pltfrm.h>
#include <uart.h>
#include <util.h>
#include <mac_pib.h>
#include <adp.h>
#include <rfd_app.h>
#include <dis.h>
#include <ad7797.h>


#if 0
#define LOAD_CELL_CAL_WEIGHT_GRAMS  1000
#define LC_CAL_SAMPLE_CNT_SHIFT  3
#define LC_CAL_SAMPLE_CNT (1 << LC_CAL_SAMPLE_CNT_SHIFT)

SINT32_t LC_totalCalAdcVal = 0, LC_avgCalAdcVal = 0;
UINT8_t LC_calSampleCnt, LC_calDone = 0;
#endif

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
void RFD_APP_buildSendSnsrUpdate(SINT32_t loadSnsrOp)
{
   const UINT8_t pktLen = ADP_ELEMENT_TYPE_LEN
                          + DIS_MSG_TYPE_SZ
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT_LIST
#ifdef PLTFRM_ON_CHIP_VCC_SENSE_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + PLTFRM_ON_CHIP_VCC_SENSOR_OUTPUT_LEN
                          + DIS_DATA_SCALE_FACTOR_TLV_SZ
#endif
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + AD7797_SENSOR_OUTPUT_VALUE_SZ;
   UINT8_t flrFlag = 0;

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

       {
          UINT8_t *tlvHdr_p = buff_p;

          buff_p += UTIL_TLV_HDR_SZ;

          // Store the sampled value in the message buffer
          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_AD7797_1_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    AD7797_ADC_OUTPUT_VAL_SZ);
          buff_p = UTIL_htonl(buff_p, loadSnsrOp);

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }

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
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_AD7797_intTmoHndlr(void)
{

}

#if 0
/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_ADD7797Cal(void)
{
   SINT32_t ad7797OffSetRegVal;

   if (AD7797_startSystemOffsetCal(PLTFRM_AD7797_1_DEV_ID, &ad7797OffSetRegVal) != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_1);
   }
}
#endif


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
#if 1
void RFD_APP_wakeUpProc( )
{
   // rkris@wisense.in / may/3/15
   if (!MAC_nodeJoined())
       return;

   switch (RFD_APP_state)
   {
       case RFD_APP_STATE_WAIT_CONVERSION:
            {
                PLTFRM_sts_t sts;
                SINT32_t adcVal;

                sts = AD7797_getADCVal(PLTFRM_AD7797_1_DEV_ID, &adcVal);
                if (sts != PLTFRM_STS_SUCCESS)
                {
                    SYS_fatal(SYS_FATAL_ERR_614);
                }
                else
                {
                	RFD_APP_buildSendSnsrUpdate(adcVal);
                	RFD_APP_state = RFD_APP_STATE_CONVERSION_DONE;
                }
            }
            break;

       case RFD_APP_STATE_CONVERSION_DONE:
            {
                UINT16_t tmoVal;
                PLTFRM_sts_t sts;

                sts = AD7797_startSingleConv(PLTFRM_AD7797_1_DEV_ID, &tmoVal);
                if (sts != PLTFRM_STS_OPN_IN_PROGRESS)
                {
                    SYS_fatal(SYS_FATAL_ERR_700);
                }
                else
                {
                    if (PLTFRM_startTimerA1(0, tmoVal, RFD_APP_AD7797_intTmoHndlr) != PLTFRM_STS_SUCCESS)
                    {
                        SYS_fatal(SYS_FATAL_ERR_701);
                    }
                    else
                    {
                        SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ENTER_LPM_REQUEST;
                        RFD_APP_state = RFD_APP_STATE_WAIT_CONVERSION;
                        SYS_SET_WAKEUP_INTERVAL(0);
                    }
                }
            }
            break;

       default:
            {
                SYS_fatal(SYS_FATAL_ERR_90);
            }
            break;
   }
}
#else
void RFD_APP_wakeUpProc( )
{
   switch (RFD_APP_state)
   {
       case RFD_APP_STATE_WAIT_CONVERSION:
            {
                SINT32_t adcVal;
                PLTFRM_sts_t sts;

                sts = AD7797_getADCVal(PLTFRM_AD7797_1_DEV_ID, &adcVal);
                if (sts != PLTFRM_STS_SUCCESS)
                {
                    SYS_fatal(SYS_FATAL_ERR_1);
                }
                else
                {
                    // W = (Nadc x Wfsi) / (128 x (2^23) x S)
                    // W = (Nadc x Wfsi) / ((2^30) x S)

                    RFD_APP_state = RFD_APP_STATE_CONVERSION_DONE;

                    if (LC_calDone == 0x0)
                    {
                        if (adcVal > 50000)
                        {
                            LC_calSampleCnt ++;

                            // Drop first two sample
                            if (LC_calSampleCnt > 2)
                                LC_totalCalAdcVal += adcVal;

                            if (LC_calSampleCnt >= LC_CAL_SAMPLE_CNT + 2)
                            {
                                LC_avgCalAdcVal = LC_totalCalAdcVal;
                                LC_avgCalAdcVal >>= LC_CAL_SAMPLE_CNT_SHIFT;

                                // Cal weight is 800 grams.
                                // W(x) = Nadc(x) x LOAD_CELL_CAL_WEIGHT_GRAMS / Ncal

                                LC_calDone = 1;
                            }
                        }
                        else
                        {
                            LC_totalCalAdcVal = 0;
                            LC_calSampleCnt = 0;
                        }

                        SYS_SET_WAKEUP_INTERVAL(RFD_APP_DFLT_WAKE_UP_INTERVAL_SECS);
                        SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ENTER_LPM_REQUEST;
                   }
                   else
                   {
                       UINT32_t currLoad;

                       if (adcVal < 0)
                           adcVal = 0;
                       currLoad = adcVal;
                       currLoad *= LOAD_CELL_CAL_WEIGHT_GRAMS;
                       currLoad /= LC_avgCalAdcVal;
                       RFD_APP_buildSendSnsrUpdate(currLoad);
                   }
               }
            }
            break;

       case RFD_APP_STATE_CONVERSION_DONE:
            {
               UINT16_t tmoVal;
               PLTFRM_sts_t sts;

               sts = AD7797_startSingleConv(PLTFRM_AD7797_1_DEV_ID, &tmoVal);
               if (sts != PLTFRM_STS_OPN_IN_PROGRESS)
               {
                   SYS_fatal(SYS_FATAL_ERR_1);
               }
               else
               {
                   if (PLTFRM_startTimerA1(0, tmoVal, RFD_APP_AD7797_intTmoHndlr) != PLTFRM_STS_SUCCESS)
                   {
                       SYS_fatal(SYS_FATAL_ERR_1);
                   }
                   else
                   {
                       SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ENTER_LPM_REQUEST;
                       RFD_APP_state = RFD_APP_STATE_WAIT_CONVERSION;
                       SYS_SET_WAKEUP_INTERVAL(0);
                   }
               }
            }
            break;

       default:
            {
                SYS_fatal(SYS_FATAL_ERR_90);
            }
            break;
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
void RFD_APP_init(void)
{
#ifdef RFD_APP_LOAD_CELL_MON
#ifndef AD7797_DIRECT_INTERFACE
#ifdef PLTFRM_UART_ENA
    UART_start(PLTFRM_UART_HW_1_DEV_ID);
#else
#error UART not enabled !!
#endif
#endif
#endif
}

#endif
#endif

