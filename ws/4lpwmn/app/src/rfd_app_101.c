/*
 * File Name: rfd_app_101.c
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
#ifdef RFD_APP_101
#ifdef PLTFRM_LM75B_TEMP_SENSOR_ENA

#include <typedefs.h>
#include <system.h>
#include <pltfrm.h>
#include <util.h>
#include <mac_pib.h>
#include <adp.h>
#include <rfd_app.h>
#include <dis.h>
#include <lm75b.h>


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
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + LM75B_SENSOR_OUTPUT_VALUE_SZ;
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


       {
          SINT16_t temp16;
          UINT8_t *tlvHdr_p = buff_p;

          buff_p += UTIL_TLV_HDR_SZ;

          // Call the LM75B driver's function to get the last temperature value
          // sampled by the LM75B sensor. The driver code is in the file "pltfrm/
          // src/lm75b.c".
          if (LM75B_getTempVal(PLTFRM_LM75B_1_DEV_ID, &temp16) != PLTFRM_STS_SUCCESS)
          {
              SYS_fatal(SYS_FATAL_ERR_81);
          }
          else
          {
              if (LM75B_shutDown(PLTFRM_LM75B_1_DEV_ID) != PLTFRM_STS_SUCCESS)
              {
                  SYS_fatal(SYS_FATAL_ERR_82);
              }
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
void RFD_APP_LM75B_intTmoHndlr(void)
{
   // No need to do anything here. The purpose of the timer interrupt
   // is to wake up the micro. After waking up, RFD_APP_wakeUpProc( )
   // will be called.
   return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t RFD_APP_startLM75BConv(void)
{
   PLTFRM_sts_t sts;
   SINT16_t tempVal;
   UINT16_t tmoVal;

   sts = LM75B_startSingleConv(PLTFRM_LM75B_1_DEV_ID,
                               &tempVal,
                               &tmoVal);
   if (sts != PLTFRM_STS_OPN_IN_PROGRESS)
   {
       sts = PLTFRM_STS_DEV_NOT_READY;
   }
   else
   {
       sts = PLTFRM_startTimerA1(0, tmoVal, RFD_APP_LM75B_intTmoHndlr);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ENTER_LPM_REQUEST;

           /*
            * Disable periodic wake up since we have started a timer above which
            * will wake up the micro after "tmoVal" seconds. Periodic wake up
            * can be disabled by calling the "SYS_SET_WAKEUP_INTERVAL( )" macro
            * with argument "0".
            */
           SYS_SET_WAKEUP_INTERVAL(0);
       }
       else
           SYS_fatal(SYS_FATAL_ERR_441);
   }

   return sts;
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
   if (!MAC_nodeJoined())
       return;

   switch (RFD_APP_state)
   {
       case RFD_APP_STATE_CONVERSION_DONE:
            {
                if (RFD_APP_startLM75BConv()  == PLTFRM_STS_SUCCESS)
                    RFD_APP_state = RFD_APP_STATE_WAIT_CONVERSION;
                else
                {
                    SYS_fatal(SYS_FATAL_ERR_89);
                }
            }
            break;

       case RFD_APP_STATE_WAIT_CONVERSION:
            {
                RFD_APP_buildSendSnsrUpdate();
                RFD_APP_state = RFD_APP_STATE_CONVERSION_DONE;
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
