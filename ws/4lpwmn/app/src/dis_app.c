/*
 * File Name: dis_app.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Oct/27/2013
 *
 *
 * Copyright (c) <2013>, <ram krishnan>
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

#include <stdlib.h>
#include <string.h>
#include <typedefs.h>
#include <pltfrm.h>
#include <adp.h>
#include <system.h>
#include <util.h>
#include <lpwmn_app.h>
#include <dis.h>
#include <lm75b.h>
#include <sfh_7773.h>

#if 0
#if defined(DEV_TYPE_FFD) || defined(DEV_TYPE_RFD)

#ifndef DEV_TYPE_RFD
void APP_procGPIOEvt(void)
{

}
#endif

#ifdef DIS_ENA

#define __FIRST_ITER   0x1
#define __SECOND_ITER  0x2

DIS_sts_t APP_checkSnsrDataReqBitMsk(void);

UINT16_t FFD_APP_rcvdMsgSrcAddr;
UINT8_t FFD_APP_rcvdMsgBdHndl;
DIS_sts_t APP_disRespSts;

#ifdef PLTFRM_SFH_7773_SENSOR_ENA
SINT32_t APP_SFH_7773_luxVal;
#endif

#ifdef PLTFRM_LM7B_TEMP_SENSOR_ENA
SINT16_t APP_LM75B_tempVal;
#endif

UINT8_t APP_snsrDataReqBitMsk;

#ifdef PLTFRM_ON_CHIP_VCC_SENSE_ENA
UINT8_t APP_battVSnsrOpnModeBitMsk;
#endif

#ifdef PLTFRM_LM7B_TEMP_SENSOR_ENA
UINT8_t APP_lm75BOpnModeBitMsk;
#endif

#ifdef PLTFRM_ON_CHIP_TEMP_SENSOR_ENA
UINT8_t APP_onChipTempSnsrOpnModeBitMsk;
#endif

#ifdef PLTFRM_SFH_7773_SENSOR_ENA
UINT8_t APP_sfh7773OpnModeBitMsk;
#endif
/*
 * Assign id to each sensor on the node.
 */
UINT8_t __BATTERY_VOLTAGE_DIS_SENSOR_ID;
UINT8_t __MSP430_ON_CHIP_TEMP_DIS_SENSOR_ID;
UINT8_t __SFH_7773_ALS_DIS_SENSOR_ID;
UINT8_t __LM75B_TEMP_DIS_SENSOR_ID;


const UINT8_t __partNrA[ ] = "MSP430";
const UINT8_t __partNrB[ ] = "LM75B";
const UINT8_t __partNrC[ ] = "SFH 7773";

const UINT8_t __mfrA[ ] = "TI";
const UINT8_t __mfrB[ ] = "NXP";
const UINT8_t __mfrC[ ] = "OSRAM OPTO";

UINT8_t __nodeSensorCnt;
const DIS_nodePowerSrc_t  __nodePwrSrc = DIS_NODE_POWER_SRC_BATTERY;
const UINT16_t __battCapMAH = 200;


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void APP_sendToAdp(UINT8_t bdHndl)
{
   ADP_cntxt.txParams.bdHndl = bdHndl;
   ADP_cntxt.txParams.localOrig = 0x1;
   ADP_cntxt.txParams.destShortAddr = FFD_APP_rcvdMsgSrcAddr;
   ADP_cntxt.txParams.nextHopAddr = MAC_BROADCAST_SHORT_ADDR;

   ADP_appPyldTxReq();
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
UINT8_t APP_buildStsTlvMsg(UINT8_t msgType, DIS_sts_t sts)
{
   UINT8_t respBdHndl;

   respBdHndl = SYS_allocMem(DIS_MSG_TYPE_SZ + DIS_STS_TLV_SZ);
   if (SYS_BD_HNDL_IS_VALID(respBdHndl))
   {
       UINT8_t *respBuff_p = SYS_GET_BD_BUFF(respBdHndl);
       *(respBuff_p ++) = msgType;
       respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                     DIS_TLV_TYPE_STATUS,
                                     DIS_STS_FIELD_SZ);
       *(respBuff_p) = sts;
   }

   return respBdHndl;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void APP_procGetSnsrDetailsCmd(UINT8_t *buff_p, UINT8_t buffLen)
{
   UINT16_t rc, respMsgLen, iter;
   UINT8_t tlvLen1, *buff1_p, *respBuff_p, *respMsg_p,
           respBdHndl = SYS_INV_BD_HDNL;
   DIS_sts_t sts = DIS_STS_SUCCESS;

   rc = UTIL_getTlv(buff_p, buffLen, DIS_TLV_TYPE_SENSOR_ID, &tlvLen1, &buff1_p);
   if (rc == 0)
       sts = DIS_STS_INV_MSG;

   if ((sts == DIS_STS_SUCCESS) && (tlvLen1 == DIS_SENSOR_ID_FIELD_SZ))
   {
       UINT8_t sensorId = *buff1_p;

       if (sensorId < __nodeSensorCnt)
       {
           respMsgLen = DIS_MSG_TYPE_SZ
                        + UTIL_TLV_HDR_SZ          // To accommodate the DIS_TLV_TYPE_SENSOR_DETAILS TLV header
                        + DIS_SENSOR_ID_TLV_SZ
                        + DIS_SENSOR_TYPE_TLV_SZ
                        + DIS_DATA_UNIT_TLV_SZ
                        + DIS_SENSOR_OPN_MODE_TLV_SZ;

           for (iter=__FIRST_ITER; iter<=__SECOND_ITER; iter++)
           {
              if (iter == __SECOND_ITER)
              {
                  // Allocate buffer
                  respBdHndl = SYS_allocMem(respMsgLen);
                  if (!(SYS_BD_HNDL_IS_VALID(respBdHndl)))
                  {
                      return;
                  }
                  else
                  {
                      respMsg_p = SYS_GET_BD_BUFF(respBdHndl);
                      respBuff_p = respMsg_p;
                      *(respBuff_p ++) = DIS_MSG_TYPE_SENSOR_DETAILS;
                      respBuff_p = UTIL_buildTlvHdr(respBuff_p, DIS_TLV_TYPE_SENSOR_DETAILS,
                                                    respMsgLen - DIS_MSG_TYPE_SZ - UTIL_TLV_HDR_SZ);
                  }
              }

              if (iter == __SECOND_ITER)
              {
                  respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                DIS_TLV_TYPE_SENSOR_ID,
                                                DIS_SENSOR_ID_FIELD_SZ);
                  *(respBuff_p ++) = __BATTERY_VOLTAGE_DIS_SENSOR_ID;
              }

#ifdef PLTFRM_ON_CHIP_VCC_SENSE_ENA
              if (sensorId == __BATTERY_VOLTAGE_DIS_SENSOR_ID)
              {
                  if (iter == __FIRST_ITER)
                  {
                      respMsgLen += (UTIL_TLV_HDR_SZ + sizeof(__partNrA));
                      respMsgLen += (UTIL_TLV_HDR_SZ + sizeof(__mfrA));
                      respMsgLen += (UTIL_TLV_HDR_SZ + DIS_TIME_IN_MICROSECS_TLV_SZ);  // Active time
                  }
                  else
                  {
                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_SENSOR_TYPE,
                                                    DIS_SENSOR_TYPE_FIELD_SZ);
                      *(respBuff_p ++) = DIS_SENSOR_TYPE_BATTERY_VOLTAGE;

                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_DATA_UNIT,
                                                    DIS_DATA_UNIT_FIELD_SZ);
                      *(respBuff_p ++) = DIS_DATA_UNIT_VOLT;

                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_SENSOR_OPN_MODE,
                                                    DIS_SENSOR_OPN_MODE_BIT_MSK_SZ);
                      *(respBuff_p ++) = APP_battVSnsrOpnModeBitMsk;

                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_SENSOR_ACTIVE_TIME,
                                                    DIS_TIME_IN_MICROSECS_TLV_SZ);
                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_TIME_IN_MICROSECS,
                                                    DIS_TIME_IN_MICROSECS_FIELD_SZ);
                      respBuff_p = UTIL_htons(respBuff_p,
                                              PLTFRM_ON_CHIP_VCC_SENSOR_ACTIVE_TIME_MICROSECS);

                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_SENSOR_PART_NR,
                                                    sizeof(__partNrA));
                      memcpy(respBuff_p, __partNrA, sizeof(__partNrA));
                      respBuff_p += sizeof(__partNrA);

                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_SENSOR_MFR,
                                                    sizeof(__mfrA));
                      memcpy(respBuff_p, __mfrA, sizeof(__mfrA));
                      respBuff_p += sizeof(__mfrA);
                  }
 
                  continue;
              }
#endif

#ifdef PLTFRM_ON_CHIP_TEMP_SENSOR_ENA
              if (sensorId == __MSP430_ON_CHIP_TEMP_DIS_SENSOR_ID)
              {
                  if (iter == __FIRST_ITER)
                  {
                      respMsgLen += (UTIL_TLV_HDR_SZ + sizeof(__partNrA));
                      respMsgLen += (UTIL_TLV_HDR_SZ + sizeof(__mfrA));
                      respMsgLen += (UTIL_TLV_HDR_SZ + DIS_TIME_IN_MICROSECS_TLV_SZ);  // Active time
                  }
                  else
                  {
                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_SENSOR_TYPE,
                                                    DIS_SENSOR_TYPE_FIELD_SZ);
                      *(respBuff_p ++) = DIS_SENSOR_TYPE_TEMPERATURE;

                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_DATA_UNIT,
                                                    DIS_DATA_UNIT_FIELD_SZ);
                      *(respBuff_p ++) = DIS_DATA_UNIT_CELCIUS;

                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_SENSOR_OPN_MODE,
                                                    DIS_SENSOR_OPN_MODE_BIT_MSK_SZ);
                      *(respBuff_p ++) = APP_onChipTempSnsrOpnModeBitMsk;

                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_SENSOR_ACTIVE_TIME,
                                                    DIS_TIME_IN_MICROSECS_TLV_SZ);
                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_TIME_IN_MICROSECS,
                                                    DIS_TIME_IN_MICROSECS_FIELD_SZ);
                      respBuff_p = UTIL_htons(respBuff_p,
                                              PLTFRM_ON_CHIP_TEMP_SENSOR_ACTIVE_TIME_MICROSECS);

                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_SENSOR_PART_NR,
                                                    sizeof(__partNrA));
                      memcpy(respBuff_p, __partNrA, sizeof(__partNrA));
                      respBuff_p += sizeof(__partNrA);

                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_SENSOR_MFR,
                                                    sizeof(__mfrA));
                      memcpy(respBuff_p, __mfrA, sizeof(__mfrA));
                      respBuff_p += sizeof(__mfrA);
                  }

                  continue;
              }
#endif

#ifdef PLTFRM_LM7B_TEMP_SENSOR_ENA
              if (sensorId == __LM75B_TEMP_DIS_SENSOR_ID)
              {
                  if (iter == __FIRST_ITER)
                  {
                      respMsgLen += (UTIL_TLV_HDR_SZ + sizeof(__partNrB));
                      respMsgLen += (UTIL_TLV_HDR_SZ + sizeof(__mfrB));
                      respMsgLen += (UTIL_TLV_HDR_SZ + DIS_TIME_IN_MILLISECS_TLV_SZ);  // Active time
                      respMsgLen += (UTIL_TLV_HDR_SZ + 1);  // Sensor minimum output (signed byte)
                      respMsgLen += (UTIL_TLV_HDR_SZ + 1);  // Sensor maximum output (signed byte)
                  }
                  else
                  {
                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_SENSOR_TYPE,
                                                    DIS_SENSOR_TYPE_FIELD_SZ);
                      *(respBuff_p ++) = DIS_SENSOR_TYPE_TEMPERATURE;

                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_DATA_UNIT,
                                                    DIS_DATA_UNIT_FIELD_SZ);
                      *(respBuff_p ++) = DIS_DATA_UNIT_CELCIUS;

                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_SENSOR_OPN_MODE,
                                                    DIS_SENSOR_OPN_MODE_BIT_MSK_SZ);
                      *(respBuff_p ++) = APP_lm75BOpnModeBitMsk;

                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_SENSOR_ACTIVE_TIME,
                                                    DIS_TIME_IN_MILLISECS_TLV_SZ);
                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_TIME_IN_MILLISECS,
                                                    DIS_TIME_IN_MILLISECS_FIELD_SZ);
                      respBuff_p = UTIL_htons(respBuff_p, LM75B_TEMP_SENSING_TIME);

                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_SENSOR_PART_NR,
                                                    sizeof(__partNrB));
                      memcpy(respBuff_p, __partNrB, sizeof(__partNrB));
                      respBuff_p += sizeof(__partNrB);

                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_SENSOR_MFR,
                                                    sizeof(__mfrB));
                      memcpy(respBuff_p, __mfrB, sizeof(__mfrB));
                      respBuff_p += sizeof(__mfrB);

                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_SENSOR_MIN_OUTPUT_VAL, 1);
                      *(respBuff_p ++) = LM75B_MIN_OUTPUT_VAL;

                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_SENSOR_MAX_OUTPUT_VAL, 1);
                      *(respBuff_p ++) = LM75B_MAX_OUTPUT_VAL;
                  }

                  continue;
              }
#endif

#ifdef PLTFRM_SFH_7773_SENSOR_ENA
              if (sensorId == __SFH_7773_ALS_DIS_SENSOR_ID)
              {
                  if (iter == __FIRST_ITER)
                  {
                      respMsgLen += (UTIL_TLV_HDR_SZ + sizeof(__partNrC));
                      respMsgLen += (UTIL_TLV_HDR_SZ + sizeof(__mfrC));
                  }
                  else
                  {
                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_SENSOR_TYPE,
                                                    DIS_SENSOR_TYPE_FIELD_SZ);
                      *(respBuff_p ++) = DIS_SENSOR_TYPE_AMBIENT_LIGHT;

                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_DATA_UNIT,
                                                    DIS_DATA_UNIT_FIELD_SZ);
                      *(respBuff_p ++) = DIS_DATA_UNIT_LUX;

                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_SENSOR_PART_NR,
                                                    sizeof(__partNrC));
                      memcpy(respBuff_p, __partNrC, sizeof(__partNrC));
                      respBuff_p += sizeof(__partNrC);

                      respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                    DIS_TLV_TYPE_SENSOR_MFR,
                                                    sizeof(__mfrC));
                      memcpy(respBuff_p, __mfrC, sizeof(__mfrC));
                      respBuff_p += sizeof(__mfrC);
                  }

                  continue;
              }
#endif
           }
       }
       else
       {
           if (sts == DIS_STS_SUCCESS)
               sts = DIS_STS_INV_SENSOR_ID;
       }
   }
   else
   {
       sts = DIS_STS_INV_TLV_SZ;
   }

   if (sts != DIS_STS_SUCCESS)
   {
       if (SYS_BD_HNDL_IS_VALID(respBdHndl))
           SYS_fatal(SYS_FATAL_ERR_1);
       respBdHndl = APP_buildStsTlvMsg(DIS_MSG_TYPE_SENSOR_DETAILS, sts);
   }

   if (SYS_BD_HNDL_IS_VALID(respBdHndl))
   {
       APP_sendToAdp(respBdHndl);
   }

   return;
}

#ifdef PLTFRM_LM7B_TEMP_SENSOR_ENA
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void APP_LM75B_onvTmoCbFunc(void)
{
   PLTFRM_sts_t sts;

   sts = LM75B_getTempVal(PLTFRM_LM75B_1_DEV_ID, &APP_LM75B_tempVal);
   if (sts != PLTFRM_STS_SUCCESS)
       APP_LM75B_tempVal  = -255;

   LM75B_cfg(PLTFRM_LM75B_1_DEV_ID, LM75B_OPN_MODE_SHUT_DOWN);

   APP_snsrDataReqBitMsk &= ~(1 << __LM75B_TEMP_DIS_SENSOR_ID);

   (void)APP_checkSnsrDataReqBitMsk();
}
#endif


#ifdef PLTFRM_SFH_7773_SENSOR_ENA
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void APP_SFH_7773_convTmoCbFunc(void)
{
   PLTFRM_sts_t pltfrmSts;
   UINT16_t tmoValMsecs;

   pltfrmSts = SFH_7773_getALSOutput(PLTFRM_SFH_7773_1_DEV_ID, &APP_SFH_7773_luxVal);
   if (pltfrmSts != PLTFRM_STS_SUCCESS)
       APP_SFH_7773_luxVal = -1;

   // <TODO> Wait till sensor goes back to stand by mode
   SFH_7773_cfgALSOpnMode(PLTFRM_SFH_7773_1_DEV_ID,
                          SFH_7773_OPN_MODE_STAND_BY,
                          &tmoValMsecs);

   APP_snsrDataReqBitMsk &= ~(__SFH_7773_ALS_DIS_SENSOR_ID);

   (void)APP_checkSnsrDataReqBitMsk();
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t APP_SFH_7773_startConv(void)
{
   PLTFRM_sts_t pltfrmSts;
   UINT16_t tmoValMsecs;

   pltfrmSts = SFH_7773_startALSConv(PLTFRM_SFH_7773_1_DEV_ID,
                                     &APP_SFH_7773_luxVal,
                                     &tmoValMsecs);
   if (pltfrmSts == PLTFRM_STS_OPN_IN_PROGRESS)
   {
       pltfrmSts = msp430g2553_startTimerA1(0, tmoValMsecs, APP_SFH_7773_convTmoCbFunc);
       if (pltfrmSts != PLTFRM_STS_SUCCESS)
           SYS_fatal(SYS_FATAL_ERR_1);
   }

   return pltfrmSts;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void APP_SFH_7773_opnModeChangeTmoCbFunc(void)
{
   PLTFRM_sts_t pltfrmSts = APP_SFH_7773_startConv();
   if (pltfrmSts != PLTFRM_STS_OPN_IN_PROGRESS)
   {
       APP_snsrDataReqBitMsk &= ~(__SFH_7773_ALS_DIS_SENSOR_ID);
       (void)APP_checkSnsrDataReqBitMsk();
   }
   return;
}
#endif

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
DIS_sts_t APP_checkSnsrDataReqBitMsk(void)
{
   DIS_sts_t sts = DIS_STS_SUCCESS;

   if (APP_snsrDataReqBitMsk == 0x0)
   {
       SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_APP_DATA_REQUEST;
       return sts;
   }

#ifdef PLTFRM_LM7B_TEMP_SENSOR_ENA
   if (APP_snsrDataReqBitMsk & (1 << __LM75B_TEMP_DIS_SENSOR_ID))
   {
       UINT16_t tmoValMsecs;
       PLTFRM_sts_t pltfrmSts;

       APP_LM75B_tempVal = -1;

       pltfrmSts = LM75B_startSingleConv(PLTFRM_LM75B_1_DEV_ID,
                                         &APP_LM75B_tempVal,
                                         &tmoValMsecs);
       if (pltfrmSts == PLTFRM_STS_OPN_IN_PROGRESS)
       {
           pltfrmSts = msp430g2553_startTimerA1(0, tmoValMsecs, APP_LM75B_onvTmoCbFunc);
           if (pltfrmSts != PLTFRM_STS_SUCCESS)
               SYS_fatal(SYS_FATAL_ERR_1);
           else
           {
               sts = DIS_STS_REQ_IN_PROGRESS;
           }
       }
       else
       {
           APP_snsrDataReqBitMsk &= ~(1 << __LM75B_TEMP_DIS_SENSOR_ID);
           if (pltfrmSts != PLTFRM_STS_SUCCESS)
               sts = DIS_STS_HW_ERROR;
       }
   }
#endif

   if (sts == DIS_STS_REQ_IN_PROGRESS)
       return sts;

#ifdef PLTFRM_SFH_7773_SENSOR_ENA
   if (APP_snsrDataReqBitMsk & (1 << __SFH_7773_ALS_DIS_SENSOR_ID))
   {
       SFH_7773_mode_t opnMode = SFH_7773_getOpnMode(PLTFRM_SFH_7773_1_DEV_ID);

       APP_SFH_7773_luxVal = -1;

       switch (opnMode)
       {
           case SFH_7773_OPN_MODE_STAND_BY:
                {
                   UINT16_t tmoValMsecs;
                   PLTFRM_sts_t pltfrmSts;

                   pltfrmSts = SFH_7773_cfgALSOpnMode(PLTFRM_SFH_7773_1_DEV_ID,
                                                      SFH_7773_OPN_MODE_TRIGGERED,
                                                      &tmoValMsecs);
                   if (pltfrmSts == PLTFRM_STS_OPN_IN_PROGRESS)
                   {
                       pltfrmSts = msp430g2553_startTimerA1(0, tmoValMsecs,
                                                            APP_SFH_7773_opnModeChangeTmoCbFunc);
                       if (pltfrmSts != PLTFRM_STS_SUCCESS)
                           SYS_fatal(SYS_FATAL_ERR_1);
                       else
                       {
                           sts = DIS_STS_REQ_IN_PROGRESS;
                           break;
                       }
                   }
                   else
                   {
                       sts = DIS_STS_HW_ERROR;
                       APP_snsrDataReqBitMsk &= ~(__SFH_7773_ALS_DIS_SENSOR_ID);
                   }
                }
                break;

           case SFH_7773_OPN_MODE_FREE_RUNNING:
                {
                   PLTFRM_sts_t pltfrmSts;

                   pltfrmSts = SFH_7773_getALSOutput(PLTFRM_SFH_7773_1_DEV_ID, &APP_SFH_7773_luxVal);
                   if (pltfrmSts != PLTFRM_STS_SUCCESS)
                       sts = DIS_STS_HW_ERROR;
                   APP_snsrDataReqBitMsk &= ~(__SFH_7773_ALS_DIS_SENSOR_ID);
                }
                break;

           case SFH_7773_OPN_MODE_TRIGGERED:
                {
                   PLTFRM_sts_t pltfrmSts;

                   pltfrmSts = APP_SFH_7773_startConv();
                   if (pltfrmSts == PLTFRM_STS_OPN_IN_PROGRESS)
                       sts = DIS_STS_REQ_IN_PROGRESS;
                   else
                   {
                       sts = DIS_STS_HW_ERROR;
                       APP_snsrDataReqBitMsk &= ~(__SFH_7773_ALS_DIS_SENSOR_ID);
                   }
                }
                break;

           default:
                {
                   sts = DIS_STS_HW_ERROR;
                   APP_snsrDataReqBitMsk &= ~(__SFH_7773_ALS_DIS_SENSOR_ID);
                }
                break;
       }
   }
#endif

   return sts;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
DIS_sts_t APP_procGetSnsrDataCmd(UINT8_t *buff_p, UINT8_t buffLen)
{
   UINT16_t rc, idx = 0;
   UINT8_t tlvLen1, *buff1_p;
   DIS_sts_t sts = DIS_STS_SUCCESS;

   if (__nodeSensorCnt < 1)
       return DIS_STS_INV_SENSOR_ID;

   APP_snsrDataReqBitMsk = 0;

   rc = UTIL_getTlv(buff_p, buffLen, DIS_TLV_TYPE_SENSOR_ID_LIST, &tlvLen1, &buff1_p);
   if (rc == 0)
       sts = DIS_STS_INV_MSG;
   else
   {
       while (tlvLen1 >= DIS_SENSOR_ID_TLV_SZ)
       {
           UINT8_t tlvLen2, sensorId;
           UINT8_t *buff2_p;

           rc = UTIL_getTlv(buff1_p, tlvLen1, DIS_TLV_TYPE_SENSOR_ID, &tlvLen2, &buff2_p);
           if (rc == 0)
           {
               sts = DIS_STS_INV_MSG;
               break;
           }

           if (tlvLen2 != DIS_SENSOR_ID_FIELD_SZ)
           {
               sts = DIS_STS_INV_TLV_SZ;
               break;
           }

           sensorId = *buff2_p;

           if (sensorId == DIS_SENSOR_ID_ALL && idx > 0)
           {
               sts = DIS_STS_INV_MSG;
               break;
           }
#ifdef PLTFRM_ON_CHIP_VCC_SENSE_ENA
           if (sensorId == DIS_SENSOR_ID_ALL || sensorId == __BATTERY_VOLTAGE_DIS_SENSOR_ID)
               APP_snsrDataReqBitMsk |= (1 << __BATTERY_VOLTAGE_DIS_SENSOR_ID);
#endif

#ifdef PLTFRM_ON_CHIP_TEMP_SENSOR_ENA
           if (sensorId == DIS_SENSOR_ID_ALL || sensorId == __MSP430_ON_CHIP_TEMP_DIS_SENSOR_ID)
               APP_snsrDataReqBitMsk |= (1 << __MSP430_ON_CHIP_TEMP_DIS_SENSOR_ID);
#endif

#ifdef PLTFRM_LM7B_TEMP_SENSOR_ENA
           if (sensorId == DIS_SENSOR_ID_ALL || sensorId == __LM75B_TEMP_DIS_SENSOR_ID)
               APP_snsrDataReqBitMsk |= (1 << __LM75B_TEMP_DIS_SENSOR_ID);
#endif

#ifdef PLTFRM_SFH_7773_SENSOR_ENA
           if (sensorId == DIS_SENSOR_ID_ALL || sensorId == __SFH_7773_ALS_DIS_SENSOR_ID)
               APP_snsrDataReqBitMsk |= (1 << __SFH_7773_ALS_DIS_SENSOR_ID);
#endif

           if (sensorId == DIS_SENSOR_ID_ALL)
               break;

           buff1_p += DIS_SENSOR_ID_TLV_SZ;
           tlvLen1 -= DIS_SENSOR_ID_TLV_SZ;

           idx ++;
       }  // while
   }

   if (sts == DIS_STS_SUCCESS && APP_snsrDataReqBitMsk == 0x0)
       sts = DIS_STS_INV_SENSOR_ID;

   if (sts == DIS_STS_SUCCESS)
   {
       // Start getting data from sensors which cannot provide data instantaneously.
       // For example, the LM75B conversion time is 10 milliseconds.

       // These two sensors will be polled when building the response
#ifdef PLTFRM_ON_CHIP_VCC_SENSE_ENA
       APP_snsrDataReqBitMsk = ~(1 << __BATTERY_VOLTAGE_DIS_SENSOR_ID);
#endif

#ifdef PLTFRM_ON_CHIP_TEMP_SENSOR_ENA
       APP_snsrDataReqBitMsk = ~(1 << __MSP430_ON_CHIP_TEMP_DIS_SENSOR_ID);
#endif

       sts = APP_checkSnsrDataReqBitMsk();
   }
   else
   {
       APP_snsrDataReqBitMsk = 0x0;
   }

   return sts;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void APP_buildSendSnsrDataResp(UINT8_t *buff_p, UINT8_t buffLen)
{
   UINT16_t rc, iter, respMsgLen;
   UINT8_t cTlvLen1, *cBuff1_p, *respBuff_p, *respMsg_p,
           respBdHndl = SYS_INV_BD_HDNL;

   if (APP_disRespSts != DIS_STS_SUCCESS)
   {
       respBdHndl = APP_buildStsTlvMsg(DIS_MSG_TYPE_SENSOR_DETAILS,
                                       APP_disRespSts);
       goto _end;
   }

   rc = UTIL_getTlv(buff_p, buffLen, DIS_TLV_TYPE_SENSOR_ID_LIST, &cTlvLen1, &cBuff1_p);
   if (rc == 0)
       SYS_fatal(SYS_FATAL_ERR_1);  // We have already verified that the message is ok
   else
   {
       respMsgLen = DIS_MSG_TYPE_SZ
                    + UTIL_TLV_HDR_SZ;  // To accommodate the DIS_TLV_TYPE_SENSOR_OUTPUT_LIST TLV header

       // Go through the TLVs twice. First time to calculate the response length and
       // second time to get the sensor values and build the response.

       for (iter=__FIRST_ITER; iter<=__SECOND_ITER; iter++)
       {
          UINT8_t tlvLen1 = cTlvLen1, idx = 0;
          UINT8_t *buff1_p = cBuff1_p;

          if (iter == __SECOND_ITER)
          {
              if (respMsgLen > (DIS_MSG_TYPE_SZ + UTIL_TLV_HDR_SZ))
              {
                  // Allocate buffer
                  respBdHndl = SYS_allocMem(respMsgLen);
                  if (!(SYS_BD_HNDL_IS_VALID(respBdHndl)))
                  {
                      return;
                  }
                  else
                  {
                      respMsg_p = SYS_GET_BD_BUFF(respBdHndl);
                      respBuff_p = respMsg_p;
                      *(respBuff_p ++) = DIS_MSG_TYPE_SENSOR_OUTPUT;
                      respBuff_p = UTIL_buildTlvHdr(respBuff_p, DIS_TLV_TYPE_SENSOR_OUTPUT_LIST,
                                                    respMsgLen - DIS_MSG_TYPE_SZ - UTIL_TLV_HDR_SZ);
                  }
              }
              else
              {
                  SYS_fatal(SYS_FATAL_ERR_1);  // We have already verified that the message is ok
                  break;
              }
          }

          while (tlvLen1 >= DIS_SENSOR_ID_TLV_SZ)
          {
             UINT8_t tlvLen2, sensorId;
             UINT8_t *buff2_p;

             rc = UTIL_getTlv(buff1_p, tlvLen1, DIS_TLV_TYPE_SENSOR_ID, &tlvLen2, &buff2_p);
             if (rc == 0 || tlvLen2 != DIS_SENSOR_ID_FIELD_SZ)
             {
                 SYS_fatal(SYS_FATAL_ERR_1);  // We have already verified that the message is ok
             }

             sensorId = *buff2_p;

#ifdef PLTFRM_ON_CHIP_VCC_SENSE_ENA
             if (sensorId == DIS_SENSOR_ID_ALL || sensorId == __BATTERY_VOLTAGE_DIS_SENSOR_ID)
             {
                 if (iter == __FIRST_ITER)
                 {
                     respMsgLen += (UTIL_TLV_HDR_SZ  // DIS_TLV_TYPE_SENSOR_OUTPUT
                                    + UTIL_TLV_HDR_SZ + DIS_SENSOR_ID_FIELD_SZ
                                    + UTIL_TLV_HDR_SZ + PLTFRM_ON_CHIP_VCC_SENSOR_OUTPUT_LEN
                                    + UTIL_TLV_HDR_SZ + DIS_DATA_SCALE_FACTOR_FIELD_SZ);
                 }
                 else
                 {
                     UINT16_t battVoltage;
                     UINT8_t *tlvHdr_p = respBuff_p;

                     respBuff_p += UTIL_TLV_HDR_SZ;

                     battVoltage = pltfrm_vccSense();

                     respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                   DIS_TLV_TYPE_SENSOR_ID,
                                                   DIS_SENSOR_ID_FIELD_SZ);
                     *(respBuff_p ++) = sensorId;

                     respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                   DIS_TLV_TYPE_VALUE,
                                                   PLTFRM_ON_CHIP_VCC_SENSOR_OUTPUT_LEN);
                     respBuff_p = UTIL_htons(respBuff_p, battVoltage);

                     respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                   DIS_TLV_TYPE_DATA_SCALE_FACTOR,
                                                   DIS_DATA_SCALE_FACTOR_FIELD_SZ);
                     *(respBuff_p ++) = DIS_DATA_SCALE_MILLI;

                     UTIL_buildTlvHdr(tlvHdr_p,
                                      DIS_TLV_TYPE_SENSOR_OUTPUT,
                                      respBuff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
                 }
             }
#endif

#ifdef PLTFRM_ON_CHIP_TEMP_SENSOR_ENA
             if (sensorId == DIS_SENSOR_ID_ALL || sensorId == __MSP430_ON_CHIP_TEMP_DIS_SENSOR_ID)
             {
                 if (iter == __FIRST_ITER)
                 {
                     respMsgLen += (UTIL_TLV_HDR_SZ  // DIS_TLV_TYPE_SENSOR_OUTPUT
                                    + UTIL_TLV_HDR_SZ + DIS_SENSOR_ID_FIELD_SZ
                                 + UTIL_TLV_HDR_SZ + PLTFRM_ON_CHIP_TEMP_SENSOR_OUTPUT_LEN
                                 + UTIL_TLV_HDR_SZ + DIS_DATA_SCALE_FACTOR_FIELD_SZ);
                 }
                 else
                 {
                     SINT16_t temp;
                     UINT8_t *tlvHdr_p = respBuff_p;

                     respBuff_p += UTIL_TLV_HDR_SZ;

                     PLTFRM_onChipTempSensorInit();
                     temp = PLTFRM_readOnChipTempSensor();

                     respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                   DIS_TLV_TYPE_SENSOR_ID,
                                                   DIS_SENSOR_ID_FIELD_SZ);
                     *(respBuff_p ++) = sensorId;

                     respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                   DIS_TLV_TYPE_VALUE,
                                                   PLTFRM_ON_CHIP_TEMP_SENSOR_OUTPUT_LEN);
                     respBuff_p = UTIL_htons(respBuff_p, (UINT16_t)temp);

                     respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                   DIS_TLV_TYPE_DATA_SCALE_FACTOR,
                                                   DIS_DATA_SCALE_FACTOR_FIELD_SZ);
                     *(respBuff_p ++) = DIS_DATA_SCALE_CENTI;

                     UTIL_buildTlvHdr(tlvHdr_p,
                                      DIS_TLV_TYPE_SENSOR_OUTPUT,
                                      respBuff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
                 }
             }
#endif

#ifdef PLTFRM_LM7B_TEMP_SENSOR_ENA
             if (sensorId == DIS_SENSOR_ID_ALL || sensorId == __LM75B_TEMP_DIS_SENSOR_ID)
             {
                 if (iter == __FIRST_ITER)
                 {
                     respMsgLen += (UTIL_TLV_HDR_SZ  // DIS_TLV_TYPE_SENSOR_OUTPUT
                                    + UTIL_TLV_HDR_SZ + DIS_SENSOR_ID_FIELD_SZ
                                    + UTIL_TLV_HDR_SZ + LM75B_SENSOR_OUTPUT_VALUE_SZ);
                 }
                 else
                 {
                     UINT8_t *tlvHdr_p = respBuff_p;

                     respBuff_p += UTIL_TLV_HDR_SZ;

                     respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                   DIS_TLV_TYPE_SENSOR_ID,
                                                   DIS_SENSOR_ID_FIELD_SZ);
                     *(respBuff_p ++) = sensorId;

                     respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                   DIS_TLV_TYPE_VALUE,
                                                   LM75B_SENSOR_OUTPUT_VALUE_SZ);
                     respBuff_p = UTIL_htons(respBuff_p, (UINT16_t)APP_LM75B_tempVal);

                     UTIL_buildTlvHdr(tlvHdr_p,
                                      DIS_TLV_TYPE_SENSOR_OUTPUT,
                                      respBuff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
                 }
             }
#endif

#ifdef PLTFRM_SFH_7773_SENSOR_ENA
             if (sensorId == DIS_SENSOR_ID_ALL || sensorId == __SFH_7773_ALS_DIS_SENSOR_ID)
             {
                 if (iter == __FIRST_ITER)
                 {
                     respMsgLen += (UTIL_TLV_HDR_SZ  // DIS_TLV_TYPE_SENSOR_OUTPUT
                                    + UTIL_TLV_HDR_SZ + DIS_SENSOR_ID_FIELD_SZ
                                    + UTIL_TLV_HDR_SZ + SFH_7773_SENSOR_OUTPUT_VALUE_SZ);
                 }
                 else
                 {
                     UINT8_t *tlvHdr_p = respBuff_p;

                     respBuff_p += UTIL_TLV_HDR_SZ;

                     respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                   DIS_TLV_TYPE_SENSOR_ID,
                                                   DIS_SENSOR_ID_FIELD_SZ);
                     *(respBuff_p ++) = sensorId;

                     respBuff_p = UTIL_buildTlvHdr(respBuff_p,
                                                   DIS_TLV_TYPE_VALUE,
                                                   SFH_7773_SENSOR_OUTPUT_VALUE_SZ);
                     respBuff_p = UTIL_htonl(respBuff_p, (UINT32_t)APP_SFH_7773_luxVal);

                     UTIL_buildTlvHdr(tlvHdr_p,
                                      DIS_TLV_TYPE_SENSOR_OUTPUT,
                                      respBuff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
                 }
             }
#endif

             if (sensorId == DIS_SENSOR_ID_ALL)
                 break;

             buff1_p += DIS_SENSOR_ID_TLV_SZ;
             tlvLen1 -= DIS_SENSOR_ID_TLV_SZ;

             idx ++;
         }  // while
       }  // for
   }

_end:
   if (SYS_BD_HNDL_IS_VALID(respBdHndl))
   {
       APP_sendToAdp(respBdHndl);
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
void APP_buildSendDevCapInfo(void)
{
   UINT8_t respBdHndl, respMsgLen, incBattCapTlv = 0;

   respMsgLen = DIS_MSG_TYPE_SZ
                + UTIL_TLV_HDR_SZ
                + DIS_SENSOR_CNT_TLV_SZ
                + DIS_PWR_SRC_TLV_SZ;

   if (__nodePwrSrc != DIS_NODE_POWER_SRC_MAINS
       && __battCapMAH > 0)
   {
       incBattCapTlv = 1;
       respMsgLen += DIS_BATT_CAP_MAH_TLV_SZ;
   }

   respBdHndl = SYS_allocMem(respMsgLen);
   if (!(SYS_BD_HNDL_IS_VALID(respBdHndl)))
   {
       return;
   }
   else
   {
       UINT8_t *respMsg_p = SYS_GET_BD_BUFF(respBdHndl),
               *tlvHdr_p;

       *(respMsg_p ++) = DIS_MSG_TYPE_NODE_CAP_INFO;

       tlvHdr_p = respMsg_p;
       respMsg_p += UTIL_TLV_HDR_SZ;

       respMsg_p = UTIL_buildTlvHdr(respMsg_p,
                                    DIS_TLV_TYPE_SENSOR_COUNT,
                                    DIS_SENSOR_CNT_FIELD_SZ);
       *(respMsg_p ++) = __nodeSensorCnt;

       respMsg_p = UTIL_buildTlvHdr(respMsg_p,
                                    DIS_TLV_TYPE_NODE_PWR_SRC,
                                    DIS_NODE_PWR_SRC_FIELD_SZ);
       *(respMsg_p ++) = __nodePwrSrc;

       if (incBattCapTlv)
       {
           respMsg_p = UTIL_buildTlvHdr(respMsg_p,
                                        DIS_TLV_TYPE_BATT_CAP_MAH,
                                        DIS_BATT_CAP_MAH_FIELD_SZ);
           respMsg_p = UTIL_htons(respMsg_p, __battCapMAH);
       }

       UTIL_buildTlvHdr(tlvHdr_p,
                        DIS_TLV_TYPE_NODE_CAP_INFO,
                        respMsg_p - tlvHdr_p - UTIL_TLV_HDR_SZ);

       APP_sendToAdp(respBdHndl);
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
void APP_buildSendPendDataReqs(void)
{
   UINT8_t *buff_p;
   UINT16_t buffLen;

   if (!SYS_BD_HNDL_IS_VALID(FFD_APP_rcvdMsgBdHndl))
       SYS_fatal(SYS_FATAL_ERR_1);

   buff_p = SYS_GET_BD_BUFF(FFD_APP_rcvdMsgBdHndl);
   buffLen = SYS_GET_BD_LEN(FFD_APP_rcvdMsgBdHndl);

   if (buffLen >= DIS_MSG_TYPE_SZ)
   {
       switch (*buff_p)
       {
           case DIS_MSG_TYPE_GET_NODE_CAP_INFO:
                {
                    APP_buildSendDevCapInfo();
                }
                break;

           case DIS_MSG_TYPE_GET_SENSOR_OUTPUT:
                {
                    APP_buildSendSnsrDataResp(buff_p, buffLen);
                }
                break;

           case DIS_MSG_TYPE_GET_SENSOR_DETAILS:
                {
                    APP_procGetSnsrDetailsCmd(buff_p + DIS_MSG_TYPE_SZ,
                                              buffLen - DIS_MSG_TYPE_SZ);
                }
                break;

           default:
                {

                }
                break;
       }
   }

   SYS_freeMem(FFD_APP_rcvdMsgBdHndl);
   FFD_APP_rcvdMsgBdHndl = SYS_INV_BD_HDNL;
   FFD_APP_rcvdMsgSrcAddr = MAC_BROADCAST_SHORT_ADDR;

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
void APP_processDataInd(ADP_dataIndParams_s *indParams_p)
{
   UINT8_t *buff_p = SYS_GET_BD_BUFF(indParams_p->bdHndl);
   UINT16_t buffLen = SYS_GET_BD_LEN(indParams_p->bdHndl);

   if (buffLen >= DIS_MSG_TYPE_SZ)
   {
       switch (*buff_p)
       {
           case DIS_MSG_TYPE_GET_SENSOR_OUTPUT:
                {
                    APP_disRespSts = APP_procGetSnsrDataCmd(buff_p + DIS_MSG_TYPE_SZ,
                                                            buffLen - DIS_MSG_TYPE_SZ);
                    if (APP_disRespSts != DIS_STS_REQ_IN_PROGRESS)
                    {
                        SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_APP_DATA_REQUEST;
                    }
                }
                break;

           case DIS_MSG_TYPE_GET_SENSOR_DETAILS:
           case DIS_MSG_TYPE_GET_NODE_CAP_INFO:
                {
                    SYS_globalEvtMsk0 |= SYS_GLOBAL_EVT_APP_DATA_REQUEST;
                }
                break;

           case DIS_MSG_TYPE_CFG_SENSOR:
                {
                }
                break;

           default:
                {
                    SYS_freeMem(indParams_p->bdHndl);
                    indParams_p->bdHndl = SYS_INV_BD_HDNL;
                    indParams_p->srcAddr = MAC_BROADCAST_SHORT_ADDR;
                }
                break;
       }

       FFD_APP_rcvdMsgBdHndl = indParams_p->bdHndl;
       FFD_APP_rcvdMsgSrcAddr = indParams_p->srcAddr;
   }
   else
   {
       SYS_freeMem(indParams_p->bdHndl);
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
void APP_evtHndlr(UINT16_t globalEvtId, void *params_p)
{          
   switch (globalEvtId)
   {  
      case SYS_GLOBAL_EVT_ADP_DATA_INDICATION:
           {
              ADP_dataIndParams_s *indParams_p = (ADP_dataIndParams_s *)params_p;

              if (!(SYS_BD_HNDL_IS_VALID(FFD_APP_rcvdMsgBdHndl)))
                  APP_processDataInd(indParams_p);
              else
                  SYS_freeMem(indParams_p->bdHndl);
           }
           break;
          
      case SYS_GLOBAL_EVT_ADP_DATA_CONFIRM:
           {
              ADP_dataCnfrmParams_s *dcParams_p = (ADP_dataCnfrmParams_s *)params_p;
              if (dcParams_p->sts == ADP_STS_SUCCESS)
              {
              }
              else
              {
              }
           }
           break;

      default:
          break;
   }
}
#else
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
      case SYS_GLOBAL_EVT_ADP_DATA_INDICATION:
           {
              ADP_dataIndParams_s *indParams_p = (ADP_dataIndParams_s *)params_p;
              SYS_freeMem(indParams_p->bdHndl);
              indParams_p->bdHndl = SYS_INV_BD_HDNL;
           }
           break;

      case SYS_GLOBAL_EVT_ADP_DATA_CONFIRM:
           {
              ADP_dataCnfrmParams_s *dcParams_p = (ADP_dataCnfrmParams_s *)params_p;
              if (dcParams_p->sts == ADP_STS_SUCCESS)
              {
              }
              else
              {
              }
           }
           break;

      default:
          break;
   }
}
#endif
              
/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void APP_init(void)
{
#ifdef DIS_ENA
   FFD_APP_rcvdMsgSrcAddr = MAC_BROADCAST_SHORT_ADDR;
   FFD_APP_rcvdMsgBdHndl = SYS_INV_BD_HDNL;

   APP_disRespSts = DIS_STS_SUCCESS;

#ifdef PLTFRM_LM7B_TEMP_SENSOR_ENA
   APP_LM75B_tempVal = -255;
#endif

   APP_snsrDataReqBitMsk = 0x0;

   __nodeSensorCnt = 0;

#ifdef PLTFRM_ON_CHIP_VCC_SENSE_ENA
   __BATTERY_VOLTAGE_DIS_SENSOR_ID = __nodeSensorCnt ++;
   APP_battVSnsrOpnModeBitMsk = (1 << DIS_SENSOR_OPN_MODE_PULL);
#endif

#ifdef PLTFRM_ON_CHIP_TEMP_SENSOR_ENA
   __MSP430_ON_CHIP_TEMP_DIS_SENSOR_ID = __nodeSensorCnt ++;
   APP_onChipTempSnsrOpnModeBitMsk = (1 << DIS_SENSOR_OPN_MODE_PULL);
#endif

#ifdef PLTFRM_LM7B_TEMP_SENSOR_ENA
  __SFH_7773_ALS_DIS_SENSOR_ID = __nodeSensorCnt ++;
  APP_lm75BOpnModeBitMsk = (1 << DIS_SENSOR_OPN_MODE_PULL);
#endif

#ifdef PLTFRM_SFH_7773_SENSOR_ENA
   if (SFH_7773_getOpnMode(PLTFRM_SFH_7773_1_DEV_ID) != SFH_7773_OPN_MODE_UNKNOWN)
   {
       __LM75B_TEMP_DIS_SENSOR_ID = __nodeSensorCnt ++;
       APP_sfh7773OpnModeBitMsk = (1 << DIS_SENSOR_OPN_MODE_PULL);
   }
#endif

#endif

   return;
}
#endif
#endif
