/*
 * File Name: rfd_app_10.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Jan/26/2014
 *
 *
 * Copyright (c) <2014>, <ram krishnan>
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
#ifdef RFD_APP_10

#include <typedefs.h>
#include <system.h>
#include <pltfrm.h>
#include <util.h>
#include <mac_pib.h>
#include <adp.h>
#include <rfd_app.h>
#include <dis.h>

#define RFD_APP_MP3V5010_DFLT_REPORT_INTERVAL  5


#ifdef PLTFRM_ACS712_ENA
#include <acs712.h>
#endif

#ifdef PLTFRM_LM75B_TEMP_SENSOR_ENA
#include <lm75b.h>
#endif

#ifdef PLTFRM_TSL45315_ENA
#include <tsl45315.h>
#endif

#ifdef PLTFRM_MPL3115A2_SENSOR_ENA
#include <mpl3115a2.h>
#endif

#ifdef PLTFRM_MPL115A2_SENSOR_ENA
#include <mpl115a2.h>
#endif

#ifdef PLTFRM_CC2D33S_SENSOR_ENA
#include <cc2d33s.h>
#endif

#ifdef PLTFRM_MMA7660FC_SENSOR_ENA
#include <mma7660fc.h>
#endif

#ifdef NTC_THERMISTOR_ENA
#include <ntc_thermistor.h>
#endif

#ifdef PLTFRM_CHIRP_PWLA_ENA
#include <chirp_pwla.h>
#endif

#ifdef PLTFRM_MP3V5050GP_ENA
#include <mp3v5050gp.h>
#endif

#ifdef PLTFRM_MP3V5004GP_ENA
#include <mp3v5004gp.h>
#endif

#ifdef PLTFRM_MPXV5010G_ENA
#include <mpxv5010g.h>
#endif

#ifdef PLTFRM_MP3V5010_ENA
#include <mp3v5010.h>
#endif

#ifdef FC_28_MOISTURE_SENSOR_ENA
#include <fc28_moisture_sensor.h>
#endif

#ifdef PLTFRM_LDR_ENA
#include <ldr.h>
#endif

#ifdef PLTFRM_INA219_ENA
#include <ina219.h>
#endif

#ifdef PLTFRM_LLS_ENA
#include <lls.h>
#endif

#ifdef WSMS100_ENA
#include <wsms100.h>
#endif

#ifdef PLTFRM_DHT11_ENA
#include <dht11.h>
#endif

#ifdef PLTFRM_MQ5_ENA
#include <mq5.h>
#endif

#ifdef PLTFRM_BATTV_SENSE_ENA
#include <battv.h>
#endif

UINT16_t RFD_APP_dataRxReqFlrCnt = 0;
UINT16_t RFD_APP_dataTxReqCnt = 0;
UINT16_t RFD_APP_memAllocFlrCnt = 0;

UINT8_t RFD_APP_MP3V5010_reportSkipCnt = 0;
UINT32_t RFD_APP_MP3V5010_prevSnsrOp32 = 0;

#define RFD_APP_MP3V5010_MAX_REPORT_SKIP_CNT  35   // three minutes

UINT8_t snsrRepFlag = 0;

#define SHUNT_RES_INV  1
#define OPAMP_GAIN 10


#ifdef PLTFRM_ACS712_ENA
/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
UINT8_t *RFD_APP_addACS712Info(UINT8_t *buff_p, UINT8_t devId)
{
   UINT8_t *tlvHdr_p = buff_p;
   PLTFRM_sts_t sts;
   UINT32_t currVal;

   buff_p += UTIL_TLV_HDR_SZ;

   sts = ACS712_getSensorOutput(devId, &currVal);
   if (sts != PLTFRM_STS_SUCCESS)
       currVal = 0;

   buff_p = UTIL_buildTlvHdr(buff_p,
                             DIS_TLV_TYPE_SENSOR_ID,
                             DIS_SENSOR_ID_FIELD_SZ);
   *(buff_p ++) = devId;

   buff_p = UTIL_buildTlvHdr(buff_p,
                             DIS_TLV_TYPE_VALUE,
                             ACS712_SENSOR_OUTPUT_VALUE_SZ);
   buff_p = UTIL_htonl(buff_p, currVal);

   UTIL_buildTlvHdr(tlvHdr_p,
                    DIS_TLV_TYPE_SENSOR_OUTPUT,
                    buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);

   return buff_p;
}
#endif


#ifdef PLTFRM_INA219_ENA
/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
UINT8_t *RFD_APP_addIN219Info(UINT8_t *buff_p, UINT8_t bvDevId, UINT8_t svDevId)
{
   UINT8_t *tlvHdr_p = buff_p;
   PLTFRM_sts_t sts;
   SINT16_t busV, shuntV;

   buff_p += UTIL_TLV_HDR_SZ;

   sts = INA219_readBusVoltage(bvDevId, &busV);
   if (sts != PLTFRM_STS_SUCCESS)
       busV = 0;

   sts = INA219_readShuntVoltage(svDevId, &shuntV);
   if (sts != PLTFRM_STS_SUCCESS)
       shuntV = 0;

   buff_p = UTIL_buildTlvHdr(buff_p,
                             DIS_TLV_TYPE_SENSOR_ID,
                             DIS_SENSOR_ID_FIELD_SZ);
   *(buff_p ++) = bvDevId;

   buff_p = UTIL_buildTlvHdr(buff_p,
                             DIS_TLV_TYPE_VALUE,
                             IN219_BUS_VOLTAGE_OUTPUT_VAL_SZ);
   buff_p = UTIL_htons(buff_p, busV);

   UTIL_buildTlvHdr(tlvHdr_p,
                    DIS_TLV_TYPE_SENSOR_OUTPUT,
                    buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);

   tlvHdr_p = buff_p;
   buff_p += UTIL_TLV_HDR_SZ;

   buff_p = UTIL_buildTlvHdr(buff_p,
                             DIS_TLV_TYPE_SENSOR_ID,
                             DIS_SENSOR_ID_FIELD_SZ);
   *(buff_p ++) = svDevId;

   buff_p = UTIL_buildTlvHdr(buff_p,
                             DIS_TLV_TYPE_VALUE,
                             IN219_SHUNT_VOLTAGE_OUTPUT_VAL_SZ);
   buff_p = UTIL_htons(buff_p, shuntV);

   UTIL_buildTlvHdr(tlvHdr_p,
                    DIS_TLV_TYPE_SENSOR_OUTPUT,
                    buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);

   return buff_p;
}
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
void RFD_APP_buildSendSnsrUpdate(void)
{
   UINT16_t battVoltage;
   UINT8_t pktLen =       ADP_ELEMENT_TYPE_LEN
                          + DIS_MSG_TYPE_SZ
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT_LIST
#ifdef PLTFRM_ON_CHIP_VCC_SENSE_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + PLTFRM_ON_CHIP_VCC_SENSOR_OUTPUT_LEN
                          + DIS_DATA_SCALE_FACTOR_TLV_SZ
#endif

#ifdef PLTFRM_EXT_VOLT_MON
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + PLTFRM_EXT_VOLT_MON_OUTPUT_LEN
#if 0
                          + DIS_DATA_SCALE_FACTOR_TLV_SZ
#endif
#endif

#ifdef PLTFRM_EXT_CURR_MON
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + PLTFRM_EXT_VOLT_MON_OUTPUT_LEN
#if 0
                          + DIS_DATA_SCALE_FACTOR_TLV_SZ
#endif
#endif

#ifdef PLTFRM_ON_CHIP_TEMP_SENSOR_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + PLTFRM_ON_CHIP_TEMP_SENSOR_OUTPUT_LEN + + DIS_DATA_SCALE_FACTOR_TLV_SZ
#endif

#ifdef NTC_THERMISTOR_ENA
                          + ((UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                              + DIS_SENSOR_ID_TLV_SZ
							  + UTIL_TLV_HDR_SZ
							  + NTC_THERMISTOR_SENSOR_OUTPUT_VALUE_SZ) * PLTFRM_NTC_THERMISTOR_DEV_CNT)
#endif

#ifdef WSMS100_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + WSMS100_SENSOR_OUTPUT_VALUE_SZ
#endif

#ifdef PLTFRM_LM75B_TEMP_SENSOR_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + LM75B_SENSOR_OUTPUT_VALUE_SZ
                          + DIS_DATA_SCALE_FACTOR_TLV_SZ
#endif

#ifdef PLTFRM_TSL45315_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + TSL45315_SENSOR_OUTPUT_VALUE_SZ
                          + DIS_DATA_SCALE_FACTOR_TLV_SZ
#endif

#ifdef PLTFRM_BATTV_SENSE_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + BATTV_SENSOR_OUTPUT_VALUE_SZ
                          + DIS_DATA_SCALE_FACTOR_TLV_SZ
#endif


#ifdef PLTFRM_MPL3115A2_SENSOR_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + DIS_SENSOR_TYPE_TLV_SZ + UTIL_TLV_HDR_SZ + MPL3115A2_TEMP_SENSOR_OUTPUT_VALUE_SZ
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT   - MPL3115A2 pressure
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + MPL3115A2_PRESSURE_SENSOR_OUTPUT_VALUE_SZ  // + DIS_SENSOR_TYPE_TLV_SZ
#endif

#ifdef PLTFRM_MPL115A2_SENSOR_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT   - MPL115A2 pressure
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + MPL115A2_PRESSURE_SENSOR_OUTPUT_VALUE_SZ  // + DIS_SENSOR_TYPE_TLV_SZ
#endif

#ifdef PLTFRM_CC2D33S_SENSOR_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT   - CC2D33S relative humidity
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + CC2D33S_SENSOR_RH_OUTPUT_VALUE_SZ
                          + UTIL_TLV_HDR_SZ
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + CC2D33S_SENSOR_TEMP_OUTPUT_VALUE_SZ
#endif

#ifdef PLTFRM_MMA7660FC_SENSOR_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT   - MMA7660FC tilt/tap
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + MMA7660FC_SENSOR_TILT_OUTPUT_VALUE_SZ  // + DIS_SENSOR_TYPE_TLV_SZ
#endif

#ifdef PLTFRM_CHIRP_PWLA_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT   - MMA7660FC tilt/tap
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + PLTFRM_CHIRP_PWLA_MOISTURE_LEVEL_OUTPUT_LEN
#endif

#ifdef PLTFRM_MP3V5050GP_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + MP3V5050GP_SENSOR_OUTPUT_VALUE_SZ
#endif

#ifdef PLTFRM_MP3V5004GP_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + MP3V5004GP_SENSOR_OUTPUT_VALUE_SZ
#endif

#ifdef PLTFRM_MPXV5010G_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + MPXV5010G_SENSOR_OUTPUT_VALUE_SZ
#endif

#ifdef PLTFRM_MP3V5010_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + MP3V5010_SENSOR_OUTPUT_VALUE_SZ
#ifdef PLTFRM_LOW_PRESSURE_LEVEL_TRACKING_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + PLTFRM_LOW_PRESSURE_LEVEL_TIME_VALUE_SZ
#endif
#endif



#ifdef PLTFRM_ACS712_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + ACS712_SENSOR_OUTPUT_VALUE_SZ

                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + ACS712_SENSOR_OUTPUT_VALUE_SZ

                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + ACS712_SENSOR_OUTPUT_VALUE_SZ
#endif

#ifdef PLTFRM_LLS_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + LLS_SENSOR_OUTPUT_VALUE_SZ
#endif

#ifdef PLTFRM_DHT11_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + PLTFRM_DHT11_1_DEV_HUMIDITY_VAL_OP_LEN
#endif

#ifdef PLTFRM_MQ5_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + MQ5_SENSOR_OUTPUT_VALUE_SZ
#endif
                          ;

   UINT8_t flrFlag = 0;

#ifdef FC_28_MOISTURE_SENSOR_ENA
   if (snsrRepFlag)
   {
	   pktLen +=  (UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                   + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + FC_28_MOISTURE_SENSOR_SENSOR_OUTPUT_VALUE_SZ);
   }
#endif

#ifdef PLTFRM_INA219_ENA
   if (1)  // snsrRepFlag > 0)
   {
       pktLen +=  (UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                   + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + IN219_BUS_VOLTAGE_OUTPUT_VAL_SZ);
#if 0
                   + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                   + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + IN219_SHUNT_VOLTAGE_OUTPUT_VAL_SZ);
#endif
   }
#endif

#ifdef PLTFRM_LDR_ENA
   if (snsrRepFlag == 0)
   {
       pktLen +=  (UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                   + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + LDR_SENSOR_OUTPUT_VALUE_SZ);
   }
#endif

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

#if 1
          // Send the scale factor since the output is in milli-volts
          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_DATA_SCALE_FACTOR,
                                    DIS_DATA_SCALE_FACTOR_FIELD_SZ);
          *(buff_p ++) = DIS_DATA_SCALE_MILLI;
#endif

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }
#endif


#ifdef PLTFRM_BATTV_SENSE_ENA
       {
          UINT8_t *tlvHdr_p = buff_p;
          UINT32_t battVoltage;

          buff_p += UTIL_TLV_HDR_SZ;

          // Call the driver function to sample and return the current voltage
          // at the micro's Vcc pin. The returned value is in milli-volts.
          // The driver code is in the file "pltfrm/src/vcc_sense.c".
          if (BATT_getVal(PLTFRM_BATT_1_DEV_ID, &battVoltage) != PLTFRM_STS_SUCCESS)
              battVoltage = 30000;   // 30 Volts

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_BATT_1_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    BATTV_SENSOR_OUTPUT_VALUE_SZ);
          buff_p = UTIL_htonl(buff_p, battVoltage);

#if 1
          // Send the scale factor since the output is in milli-volts
          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_DATA_SCALE_FACTOR,
                                    DIS_DATA_SCALE_FACTOR_FIELD_SZ);
          *(buff_p ++) = DIS_DATA_SCALE_MILLI;
#endif

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }
#endif

#ifdef PLTFRM_EXT_CURR_MON
       {
          UINT16_t extI, adcVal;
          PLTFRM_sts_t sts;
          UINT8_t *tlvHdr_p = buff_p;
          UINT8_t refV = PLTFRM_ADC10_REF_GEN_2PT5_VOLTS;

          buff_p += UTIL_TLV_HDR_SZ;


          sts = PLTFRM_doADC10SingleConv(PLTFRM_ADC10_CHANN_2_ID,
                                         refV,
#ifdef TIMER_CLK_ACLK
                                         PLTFRM_ADC10_SH_CYCLE_COUNT_16,   // 32 KHz (31.25 microsecs * 4)
#else
#error Not Supported !!
#endif
                                         &adcVal);
          if (sts == PLTFRM_STS_SUCCESS)
          {
              UINT32_t rt;

              rt = adcVal;
              rt *= SHUNT_RES_INV;
              rt *= (refV == PLTFRM_ADC10_REF_GEN_2PT5_VOLTS ? MSP_ADC10_REF_V_2_PT_5 : MSP_ADC10_REF_V_1_PT_5);
              rt /= 1023;
              rt /= OPAMP_GAIN;
              extI = rt;
          }
          else
              extI = 0;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_CURR_MON_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    PLTFRM_EXT_VOLT_MON_OUTPUT_LEN);
          buff_p = UTIL_htons(buff_p, extI);

#if 0
          // Send the scale factor since the output is in milli-volts
          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_DATA_SCALE_FACTOR,
                                    DIS_DATA_SCALE_FACTOR_FIELD_SZ);
          *(buff_p ++) = DIS_DATA_SCALE_MILLI;
#endif

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }
#endif

#ifdef PLTFRM_MP3V5050GP_ENA
       {
          UINT32_t snsrOp32;
          UINT8_t *tlvHdr_p = buff_p;

          buff_p += UTIL_TLV_HDR_SZ;

          if (MP3V5050GP_getSensorOutput(PLTFRM_MP3V5050GP_1_DEV_ID,
                                         &snsrOp32) != PLTFRM_STS_SUCCESS)
          {
              SYS_fatal(SYS_FATAL_ERR_548);
          }

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_MP3V5050GP_1_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    MP3V5050GP_SENSOR_OUTPUT_VALUE_SZ);
          buff_p = UTIL_htonl(buff_p, snsrOp32);

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }
#endif

#ifdef PLTFRM_LLS_ENA
       {
          UINT32_t snsrOp32;
          UINT8_t *tlvHdr_p = buff_p;

          buff_p += UTIL_TLV_HDR_SZ;

          if (LLS_getSensorOutput(PLTFRM_LLS_1_DEV_ID,
                                  &snsrOp32) != PLTFRM_STS_SUCCESS)
          {
              SYS_fatal(SYS_FATAL_ERR_548);
          }

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_LLS_1_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    LLS_SENSOR_OUTPUT_VALUE_SZ);
          buff_p = UTIL_htonl(buff_p, snsrOp32);

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }
#endif

#ifdef PLTFRM_MPXV5010G_ENA
       {
          UINT32_t snsrOp32;
          UINT8_t *tlvHdr_p = buff_p;

          buff_p += UTIL_TLV_HDR_SZ;

          if (MPXV5010G_getSensorOutput(PLTFRM_MPXV5010G_1_DEV_ID,
                                         &snsrOp32) != PLTFRM_STS_SUCCESS)
          {
              SYS_fatal(SYS_FATAL_ERR_548);
          }

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_MPXV5010G_1_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    MPXV5010G_SENSOR_OUTPUT_VALUE_SZ);
          buff_p = UTIL_htonl(buff_p, snsrOp32);

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }
#endif

#ifdef PLTFRM_MP3V5010_ENA
       {
          UINT32_t snsrOp32;
          UINT8_t *tlvHdr_p = buff_p;

          buff_p += UTIL_TLV_HDR_SZ;

          RFD_APP_snsrRepInterval = RFD_APP_MP3V5010_DFLT_REPORT_INTERVAL;

          if (MP3V5010_getSensorOutput(PLTFRM_MP3V5010_1_DEV_ID,
                                       &snsrOp32) != PLTFRM_STS_SUCCESS)
          {
              SYS_fatal(SYS_FATAL_ERR_548);
          }

          if (RFD_APP_MP3V5010_reportSkipCnt < RFD_APP_MP3V5010_MAX_REPORT_SKIP_CNT)
          {
              SINT32_t delta = snsrOp32;
              delta -= RFD_APP_MP3V5010_prevSnsrOp32;
              if (delta < 0)
            	  delta = -delta;
              if (delta < 100)   // 1 %
              {
            	  RFD_APP_MP3V5010_reportSkipCnt ++;
            	  SYS_freeMem(ADP_cntxt.txParams.bdHndl);
            	  ADP_cntxt.txParams.bdHndl = SYS_INV_BD_HDNL;
                  SYS_SET_WAKEUP_INTERVAL(RFD_APP_snsrRepInterval);
                  SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ENTER_LPM_REQUEST;
                  return;
              }
          }

          RFD_APP_MP3V5010_reportSkipCnt = 0;
          RFD_APP_MP3V5010_prevSnsrOp32 = snsrOp32;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_MP3V5010_1_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    MP3V5010_SENSOR_OUTPUT_VALUE_SZ);
          buff_p = UTIL_htonl(buff_p, snsrOp32);

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);

#ifdef PLTFRM_LOW_PRESSURE_LEVEL_TRACKING_ENA
          {
              UINT16_t timeVal = 0;
              tlvHdr_p = buff_p;

              buff_p += UTIL_TLV_HDR_SZ;

              buff_p = UTIL_buildTlvHdr(buff_p,
                                        DIS_TLV_TYPE_SENSOR_ID,
                                        DIS_SENSOR_ID_FIELD_SZ);
              *(buff_p ++) = PLTFRM_LOW_PRESSURE_TRACKER_LOGICAL_DEV;

              buff_p = UTIL_buildTlvHdr(buff_p,
                                        DIS_TLV_TYPE_VALUE,
                                        PLTFRM_LOW_PRESSURE_LEVEL_TIME_VALUE_SZ);
              buff_p = UTIL_htons(buff_p, timeVal);

              UTIL_buildTlvHdr(tlvHdr_p,
                               DIS_TLV_TYPE_SENSOR_OUTPUT,
                               buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
          }
#endif
       }
#endif


#ifdef PLTFRM_MP3V5004GP_ENA
       {
          UINT32_t snsrOp32;
          UINT8_t *tlvHdr_p = buff_p;

          buff_p += UTIL_TLV_HDR_SZ;

          if (MP3V5004GP_getSensorOutput(PLTFRM_MP3V5004GP_1_DEV_ID,
                                         &snsrOp32) != PLTFRM_STS_SUCCESS)
          {
              SYS_fatal(SYS_FATAL_ERR_548);
          }

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_MP3V5004GP_1_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    MP3V5004GP_SENSOR_OUTPUT_VALUE_SZ);
          buff_p = UTIL_htonl(buff_p, snsrOp32);

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }
#endif

#ifdef PLTFRM_EXT_VOLT_MON
       {
          UINT16_t extV, adcVal;
          PLTFRM_sts_t sts;
          UINT8_t *tlvHdr_p = buff_p;
          UINT8_t refV = battVoltage >= (MSP_ADC10_REF_V_2_PT_5 + 100) ? PLTFRM_ADC10_REF_GEN_2PT5_VOLTS : PLTFRM_ADC10_REF_GEN_1PT5_VOLTS;

          buff_p += UTIL_TLV_HDR_SZ;


          sts = PLTFRM_doADC10SingleConv(PLTFRM_ADC10_CHANN_0_ID,
                                         refV,
#ifdef TIMER_CLK_ACLK
                                         PLTFRM_ADC10_SH_CYCLE_COUNT_16,   // 32 KHz (31.25 microsecs * 4)
#else
#error Not Supported !!
#endif
                                         &adcVal);
          if (sts == PLTFRM_STS_SUCCESS)
          {
              UINT32_t rt;

              rt = adcVal;
              rt *= 11;
              rt *= (refV == PLTFRM_ADC10_REF_GEN_2PT5_VOLTS ? MSP_ADC10_REF_V_2_PT_5 : MSP_ADC10_REF_V_1_PT_5);
              rt /= 1023;
              extV = rt;
          }
          else
              extV = 0;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_GEN_VOLT_GEN_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    PLTFRM_ON_CHIP_VCC_SENSOR_OUTPUT_LEN);
          buff_p = UTIL_htons(buff_p, extV);

#if 0
          // Send the scale factor since the output is in milli-volts
          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_DATA_SCALE_FACTOR,
                                    DIS_DATA_SCALE_FACTOR_FIELD_SZ);
          *(buff_p ++) = DIS_DATA_SCALE_MILLI;
#endif

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }
#endif

#ifdef PLTFRM_ON_CHIP_TEMP_SENSOR_ENA
       {
          SINT16_t tempVal;
          UINT8_t *tlvHdr_p = buff_p;

          buff_p += UTIL_TLV_HDR_SZ;

          // Call the driver function to sample and return the current voltage
          // at the micro's Vcc pin. The returned value is in milli-volts.
          // The driver code is in the file "pltfrm/src/vcc_sense.c".
          PLTFRM_onChipTempSensorInit( );
          tempVal = PLTFRM_readOnChipTempSensor();

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_ON_CHIP_TEMP_SENSOR_DEV_ID;

          // Send the scale factor since the output is in centi-deg C
          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_DATA_SCALE_FACTOR,
                                    DIS_DATA_SCALE_FACTOR_FIELD_SZ);
          *(buff_p ++) = DIS_DATA_SCALE_CENTI;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    PLTFRM_ON_CHIP_TEMP_SENSOR_OUTPUT_LEN);
          buff_p = UTIL_htons(buff_p, tempVal);

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }
#endif

#ifdef NTC_THERMISTOR_ENA
       UINT8_t devIdx = 0;
       for (devIdx=0;  devIdx<PLTFRM_NTC_THERMISTOR_DEV_CNT; devIdx++)
       {
          UINT32_t resVal;  // Bug-fix / feb/17/16 / rkris@wisense.in
          UINT8_t *tlvHdr_p = buff_p;
          PLTFRM_sts_t sts;
          UINT8_t devId = PLTFRM_NTCALUG02A_1_DEV_ID + devIdx;

          buff_p += UTIL_TLV_HDR_SZ;

          sts = NTC_THERMISTOR_getSnsrOp(devId, &resVal);
          if (sts != PLTFRM_STS_SUCCESS)
              SYS_fatal(SYS_FATAL_ERR_813);

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = devId;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    NTC_THERMISTOR_SENSOR_OUTPUT_VALUE_SZ);
          buff_p = UTIL_htonl(buff_p, resVal);

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }
#endif

#ifdef WSMS100_ENA
       {
          UINT16_t resVal;
          UINT8_t *tlvHdr_p = buff_p;
          PLTFRM_sts_t sts;

          buff_p += UTIL_TLV_HDR_SZ;

          sts = WSMS100_getSnsrOp(PLTFRM_WSMS100_1_DEV_ID, &resVal);
          if (sts != PLTFRM_STS_SUCCESS)
              SYS_fatal(SYS_FATAL_ERR_813);

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_WSMS100_1_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    WSMS100_SENSOR_OUTPUT_VALUE_SZ);
          buff_p = UTIL_htons(buff_p, resVal);

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }
#endif

#ifdef PLTFRM_MQ5_ENA
       {
          UINT16_t snsrOp;
          UINT8_t *tlvHdr_p = buff_p;
          PLTFRM_sts_t sts;

          buff_p += UTIL_TLV_HDR_SZ;

          sts = MQ5_getSnsrOp(PLTFRM_MQ5_1_DEV_ID, &snsrOp);
          if (sts != PLTFRM_STS_SUCCESS)
        	  snsrOp = 0;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_MQ5_1_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
									MQ5_SENSOR_OUTPUT_VALUE_SZ);
          buff_p = UTIL_htons(buff_p, snsrOp);

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }
#endif

#ifdef PLTFRM_DHT11_ENA
       {
          SINT16_t tempVal, rhVal;
          UINT8_t *tlvHdr_p = buff_p;
          PLTFRM_sts_t sts;

          buff_p += UTIL_TLV_HDR_SZ;

          sts = DHT11_readSnsr(PLTFRM_DHT11_1_DEV_ID, &tempVal, &rhVal);
          if (sts != PLTFRM_STS_SUCCESS)
          {
        	  rhVal = 0;
              tempVal = 200;
          }

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_DHT11_1_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    PLTFRM_DHT11_1_DEV_HUMIDITY_VAL_OP_LEN);
          buff_p = UTIL_htons(buff_p, rhVal);

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }
#endif

#ifdef FC_28_MOISTURE_SENSOR_ENA
       if (snsrRepFlag)
       {
          UINT16_t resVal;
          UINT8_t *tlvHdr_p = buff_p;
          PLTFRM_sts_t sts;

          buff_p += UTIL_TLV_HDR_SZ;

          sts = FC_28_getSnsrOp(PLTFRM_FC_28_1_DEV_ID, &resVal);
          if (sts != PLTFRM_STS_SUCCESS)
              SYS_fatal(SYS_FATAL_ERR_813);

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_FC_28_1_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    FC_28_MOISTURE_SENSOR_SENSOR_OUTPUT_VALUE_SZ);
          buff_p = UTIL_htons(buff_p, resVal);

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }
#endif

#ifdef PLTFRM_LDR_ENA
       if (snsrRepFlag == 0)
       {
          UINT16_t opVal;
          UINT8_t *tlvHdr_p = buff_p;
          PLTFRM_sts_t sts;

          buff_p += UTIL_TLV_HDR_SZ;

          sts = LDR_getSnsrOp(PLTFRM_LDR_1_DEV_ID, &opVal);
          if (sts != PLTFRM_STS_SUCCESS)
              SYS_fatal(SYS_FATAL_ERR_813);

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_LDR_1_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    LDR_SENSOR_OUTPUT_VALUE_SZ);
          buff_p = UTIL_htons(buff_p, opVal);

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
#if 0
          if (LM75B_startSingleConv(PLTFRM_LM75B_1_DEV_ID, &temp16, NULL) != PLTFRM_STS_SUCCESS)
              SYS_fatal(SYS_FATAL_ERR_80);
#else
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
#endif

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


#ifdef PLTFRM_MPL3115A2_SENSOR_ENA
       {
          SINT16_t tempVal;
          PLTFRM_sts_t sts;
          UINT8_t *tlvHdr_p = buff_p;

          sts = MPL3115A2_readTemp(PLTFRM_MPL3115A2_1_DEV_ID, &tempVal);
          if (sts != PLTFRM_STS_SUCCESS)
          {
              SYS_fatal(SYS_FATAL_ERR_84);
          }

          buff_p += UTIL_TLV_HDR_SZ;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_MPL3115A2_1_DEV_ID;

          // Since this sensor measures multiple parameters, need to add a TLV
          // which will identify the specific parameter.
          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_TYPE,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = DIS_SENSOR_TYPE_TEMPERATURE;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    MPL3115A2_TEMP_SENSOR_OUTPUT_VALUE_SZ);
          buff_p = UTIL_htons(buff_p, tempVal);

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }

       {
          UINT32_t pressureVal;
          PLTFRM_sts_t sts;
          UINT8_t *tlvHdr_p = buff_p;

          sts = MPL3115A2_readPressure(PLTFRM_MPL3115A2_1_DEV_ID, &pressureVal);
          if (sts != PLTFRM_STS_SUCCESS)
          {
              SYS_fatal(SYS_FATAL_ERR_85);
          }

          buff_p += UTIL_TLV_HDR_SZ;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_MPL3115A2_1_DEV_ID;

#if 0
          // Since this sensor measures multiple parameters, need to add a TLV
          // which will identify the specific parameter.
          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_TYPE,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = DIS_SENSOR_TYPE_PRESSURE;
#endif

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    MPL3115A2_PRESSURE_SENSOR_OUTPUT_VALUE_SZ);
          buff_p = UTIL_htonl(buff_p, pressureVal);

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }
#endif

#ifdef PLTFRM_MPL115A2_SENSOR_ENA
       {
          UINT32_t pressureVal;
          PLTFRM_sts_t sts;
          UINT8_t *tlvHdr_p = buff_p;

          sts = MPL115A2_readPressure(PLTFRM_MPL115A2_1_DEV_ID, &pressureVal);
          if (sts != PLTFRM_STS_SUCCESS)
          {
              SYS_fatal(SYS_FATAL_ERR_4020);
          }

          buff_p += UTIL_TLV_HDR_SZ;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_MPL115A2_1_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    MPL115A2_PRESSURE_SENSOR_OUTPUT_VALUE_SZ);
          buff_p = UTIL_htonl(buff_p, pressureVal);

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }
#endif

#ifdef PLTFRM_CC2D33S_SENSOR_ENA
       {
          UINT16_t rhVal;
          SINT16_t tempVal;
          PLTFRM_sts_t sts;
          UINT8_t *tlvHdr_p = buff_p;

          sts = CC2D33S_dataFetch(PLTFRM_CC2D33S_1_RH_DEV_ID, &rhVal, &tempVal);
          if (sts != PLTFRM_STS_SUCCESS)
          {
              SYS_fatal(SYS_FATAL_ERR_86);
          }

          buff_p += UTIL_TLV_HDR_SZ;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_CC2D33S_1_RH_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    CC2D33S_SENSOR_RH_OUTPUT_VALUE_SZ);
          buff_p = UTIL_htons(buff_p, rhVal);

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);

          tlvHdr_p = buff_p;
          buff_p += UTIL_TLV_HDR_SZ;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_CC2D33S_1_TEMP_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    CC2D33S_SENSOR_TEMP_OUTPUT_VALUE_SZ);
          buff_p = UTIL_htons(buff_p, tempVal);

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);

       }
#endif

#ifdef PLTFRM_MMA7660FC_SENSOR_ENA
       {
          PLTFRM_sts_t sts;
          UINT8_t tiltInfo;
          UINT8_t *tlvHdr_p = buff_p;

          sts = MMA7660FC_getTiltSts(PLTFRM_MMA7660FC_1_DEV_ID, &tiltInfo);
          if (sts != PLTFRM_STS_SUCCESS)
          {
              SYS_fatal(SYS_FATAL_ERR_87);
          }

          buff_p += UTIL_TLV_HDR_SZ;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_MMA7660FC_1_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    MMA7660FC_SENSOR_TILT_OUTPUT_VALUE_SZ);
          *(buff_p ++) = tiltInfo;

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }
#endif

#ifdef PLTFRM_CHIRP_PWLA_ENA
       {
          UINT16_t moistureLevel;
          UINT8_t *tlvHdr_p = buff_p;
          PLTFRM_sts_t sts;

          sts = CHIRP_PWLA_getMoistureLevel(PLTFRM_CHIRP_PWLA_1_DEV_ID, &moistureLevel);
          if (sts != PLTFRM_STS_SUCCESS)
        	  moistureLevel = 0xffff;

          buff_p += UTIL_TLV_HDR_SZ;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_CHIRP_PWLA_1_DEV_ID;


          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    PLTFRM_CHIRP_PWLA_MOISTURE_LEVEL_OUTPUT_LEN);
          buff_p = UTIL_htons(buff_p, moistureLevel);

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }
#endif

#ifdef PLTFRM_INA219_ENA
       snsrRepFlag = 3;
       switch (snsrRepFlag)
       {
           case 0:
                 break;

           case 1:
                 buff_p = RFD_APP_addIN219Info(buff_p, PLTFRM_INA219_1_BV_DEV_ID, PLTFRM_INA219_1_SV_DEV_ID);
                 break;

           case 2:
                 buff_p = RFD_APP_addIN219Info(buff_p, PLTFRM_INA219_2_BV_DEV_ID, PLTFRM_INA219_2_SV_DEV_ID);
                 break;

           case 3:
                 buff_p = RFD_APP_addIN219Info(buff_p, PLTFRM_INA219_3_BV_DEV_ID, PLTFRM_INA219_3_SV_DEV_ID);
                 break;

           case 4:
                 buff_p = RFD_APP_addIN219Info(buff_p, PLTFRM_INA219_4_BV_DEV_ID, PLTFRM_INA219_4_SV_DEV_ID);
                 break;

           default:
                 SYS_fatal(SYS_FATAL_ERR_835);
                 break;
       }
#endif


#ifdef PLTFRM_ACS712_ENA
       {
           buff_p = RFD_APP_addACS712Info(buff_p, PLTFRM_ACS712_1_CURRENT_DEV_ID);

           buff_p = RFD_APP_addACS712Info(buff_p, PLTFRM_ACS712_2_CURRENT_DEV_ID);

           buff_p = RFD_APP_addACS712Info(buff_p, PLTFRM_ACS712_3_CURRENT_DEV_ID);

       }
#endif

#ifdef PLTFRM_MAINS_DETECT_SNSR_ENA

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
#ifdef PLTFRM_MP3V5010_ENA
    	   RFD_APP_MP3V5010_reportSkipCnt = RFD_APP_MP3V5010_MAX_REPORT_SKIP_CNT;
#endif
       }
       else
       {
    	   RFD_APP_lastTxMsgType = DIS_MSG_TYPE_SENSOR_OUTPUT;

    	   // snsrRepFlag ++;
    	   // if (snsrRepFlag == 5)
    	   //    snsrRepFlag = 0;
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
   }

   return;
}


#ifdef PLTFRM_TSL45315_ENA
/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_TSL45315_intTmoHndlr(void)
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
PLTFRM_sts_t RFD_APP_startTSL45315Conv(void)
{
   PLTFRM_sts_t sts;
   UINT16_t tmoVal;
   SINT32_t lux32;

   sts = TSL45315_startALSConv(PLTFRM_TSL45315_1_DEV_ID,
                               &lux32,
                               &tmoVal);
   if (sts != PLTFRM_STS_OPN_IN_PROGRESS)
   {
       sts = PLTFRM_STS_DEV_NOT_READY;
   }
   else
   {
       sts = PLTFRM_startTimerA1(0, tmoVal, RFD_APP_TSL45315_intTmoHndlr);
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
           SYS_fatal(SYS_FATAL_ERR_440);
   }

   return sts;
}
#endif

#ifdef PLTFRM_LM75B_TEMP_SENSOR_ENA
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
#endif


#ifdef PLTFRM_CC2D33S_SENSOR_ENA

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_CC2D33S_intTmoHndlr(void)
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
PLTFRM_sts_t CC2D33S_doSingleConv(void)
{
   PLTFRM_sts_t sts;
   UINT16_t tmoVal;

   sts = CC2D33S_startSingleConv(PLTFRM_CC2D33S_1_RH_DEV_ID,
                                 &tmoVal);
   if (sts != PLTFRM_STS_OPN_IN_PROGRESS)
   {
       sts = PLTFRM_STS_DEV_NOT_READY;
   }
   else
   {
       sts = PLTFRM_startTimerA1(0, tmoVal, RFD_APP_CC2D33S_intTmoHndlr);
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
           SYS_fatal(SYS_FATAL_ERR_820);
   }

   return sts;
}
#endif


#ifdef PLTFRM_MPL115A2_SENSOR_ENA

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_MPL115A2_intTmoHndlr(void)
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
PLTFRM_sts_t MPL115A2_doSingleConv(void)
{
   PLTFRM_sts_t sts;
   UINT16_t tmoVal;

   sts = MPL115A2_startSingleConv(PLTFRM_MPL115A2_1_DEV_ID,
                                  &tmoVal);
   if (sts != PLTFRM_STS_OPN_IN_PROGRESS)
   {
       sts = PLTFRM_STS_DEV_NOT_READY;
   }
   else
   {
       sts = PLTFRM_startTimerA1(0, tmoVal, RFD_APP_MPL115A2_intTmoHndlr);
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
           SYS_fatal(SYS_FATAL_ERR_4021);
   }

   return sts;
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
void RFD_APP_wakeUpProc( )
{
   if (!MAC_nodeJoined())
	   return;

#ifdef PWR_SRC_SOLAR_SUPERCAP
   {


   }
#endif

   switch (RFD_APP_state)
   {
       case RFD_APP_STATE_CONVERSION_DONE:
#ifdef PLTFRM_TSL45315_ENA
            {
               if (RFD_APP_startTSL45315Conv() == PLTFRM_STS_SUCCESS)
                   RFD_APP_state = RFD_APP_STATE_WAIT_CONVERSION;
               else
               {
                   SYS_fatal(SYS_FATAL_ERR_88);
               }
            }
            break;
#endif

       case RFD_APP_STATE_WAIT_CONVERSION:
#ifdef PLTFRM_LM75B_TEMP_SENSOR_ENA
            {
               if (RFD_APP_startLM75BConv()  == PLTFRM_STS_SUCCESS)
                   RFD_APP_state = RFD_APP_STATE_WAIT_CONVERSION_1;
               else
               {
                   SYS_fatal(SYS_FATAL_ERR_89);
               }
            }
            break;
#endif

       case RFD_APP_STATE_WAIT_CONVERSION_1:
#ifdef PLTFRM_CC2D33S_SENSOR_ENA
            {
               if (CC2D33S_doSingleConv()  == PLTFRM_STS_SUCCESS)
                   RFD_APP_state = RFD_APP_STATE_WAIT_CONVERSION_2;
               else
               {
                   SYS_fatal(SYS_FATAL_ERR_819);
               }
            }
            break;
#endif

       case RFD_APP_STATE_WAIT_CONVERSION_2:
#ifdef PLTFRM_MPL115A2_SENSOR_ENA
            {
               if (MPL115A2_doSingleConv()  == PLTFRM_STS_SUCCESS)
                   RFD_APP_state = RFD_APP_STATE_WAIT_CONVERSION_3;
               else
               {
                   SYS_fatal(SYS_FATAL_ERR_4022);
               }
            }
            break;
#endif

       case RFD_APP_STATE_WAIT_CONVERSION_3:
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
