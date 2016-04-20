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
#ifdef FFD_APP_1

#include <typedefs.h>
#include <system.h>
#include <pltfrm.h>
#include <util.h>
#include <mac_pib.h>
#include <adp.h>
#include <dis.h>
#include <ffd_app_common.h>
#include <lm75b.h>
#include <tsl45315.h>
#ifdef PLTFRM_CHIRP_PWLA_ENA
#include <chirp_pwla.h>
#endif
#ifdef PLTFRM_SHT10_ENA
#include <sht10.h>
#endif
#ifdef PLTFRM_BATTV_SENSE_ENA
#include <battv.h>
#endif
#ifdef PLTFRM_INA219_ENA
#include <ina219.h>
#endif
#ifdef PLTFRM_LM75B_TEMP_SENSOR_ENA
#include <lm75b.h>
#endif
#ifdef PLTFRM_TSL45315_ENA
#include <tsl45315.h>
#endif
#ifdef PLTFRM_MAINS_DETECT_SNSR_ENA
#include <mds.h>
#endif

typedef enum
{
   FFD_APP_STATE_CONV_DONE = 0,
   FFD_APP_STATE_WAIT_CONV_1 = 1,
   FFD_APP_STATE_WAIT_CONV_2 = 2,
   FFD_APP_STATE_WAIT_CONV_3 = 3
} FFD_APP_state_t;

FFD_APP_state_t FFD_APP_state = FFD_APP_STATE_CONV_DONE;


#ifdef PLTFRM_SHT10_ENA
UINT16_t sht10_RHRawVal;
SINT16_t sht10_tempVal;
#endif

UINT32_t APP_msgSeqNr = 0;


#ifdef PLTFRM_INA219_ENA
/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
UINT8_t *FFD_APP_addIN219Info(UINT8_t *buff_p, UINT8_t bvDevId, UINT8_t svDevId)
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
   UINT8_t pktLen = ADP_ELEMENT_TYPE_LEN
                          + DIS_MSG_TYPE_SZ
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT_LIST

#ifdef PLTFRM_ON_CHIP_VCC_SENSE_ENA
						  + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
						  + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + PLTFRM_ON_CHIP_VCC_SENSOR_OUTPUT_LEN + DIS_DATA_SCALE_FACTOR_TLV_SZ
#endif

#ifdef PLTFRM_EXT_VOLTAGE_MON_ENA
						  + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
						  + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + PLTFRM_EXT_VOLTAGE_MON_OP_LEN
#endif

#ifdef PLTFRM_DUMMY_DEV_ENA
						  + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
						  + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + sizeof(APP_msgSeqNr)
#endif

#ifdef PLTFRM_SHT10_ENA
                         + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                         + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + SHT10_TEMP_OUTPUT_LEN
                         + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                         + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + SHT10_RH_OUTPUT_LEN
#endif

#ifdef PLTFRM_ON_CHIP_TEMP_SENSOR_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + PLTFRM_ON_CHIP_TEMP_SENSOR_OUTPUT_LEN + + DIS_DATA_SCALE_FACTOR_TLV_SZ
#endif

#ifdef PLTFRM_LM75B_TEMP_SENSOR_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + LM75B_SENSOR_OUTPUT_VALUE_SZ
#endif

#ifdef PLTFRM_TSL45315_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + TSL45315_SENSOR_OUTPUT_VALUE_SZ
#endif


#ifdef PLTFRM_MPL3115A2_SENSOR_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + DIS_SENSOR_TYPE_TLV_SZ + UTIL_TLV_HDR_SZ + MPL3115A2_TEMP_SENSOR_OUTPUT_VALUE_SZ
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT   - MPL3115A2 pressure
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + MPL3115A2_PRESSURE_SENSOR_OUTPUT_VALUE_SZ  // + DIS_SENSOR_TYPE_TLV_SZ
#endif

#ifdef PLTFRM_CC2D33S_SENSOR_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT   - CC2D33S relative humidity
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + CC2D33S_SENSOR_RH_OUTPUT_VALUE_SZ  // + DIS_SENSOR_TYPE_TLV_SZ
#endif

#ifdef PLTFRM_MMA7660FC_SENSOR_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT   - MMA7660FC tilt/tap
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + MMA7660FC_SENSOR_TILT_OUTPUT_VALUE_SZ  // + DIS_SENSOR_TYPE_TLV_SZ
#endif

#ifdef PLTFRM_CHIRP_PWLA_ENA
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT   - MMA7660FC tilt/tap
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + PLTFRM_CHIRP_PWLA_MOISTURE_LEVEL_OUTPUT_LEN
#endif

#ifdef PLTFRM_MAINS_DETECT_SNSR_ENA
						  + UTIL_TLV_HDR_SZ
				          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + MDS_OUTPUT_VALUE_SZ
#endif

#if defined(PLTFRM_BATTV_SENSE_ENA)
                          + UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                          + DIS_SENSOR_ID_TLV_SZ + UTIL_TLV_HDR_SZ + BATTV_SENSOR_OUTPUT_VALUE_SZ
                          + DIS_DATA_SCALE_FACTOR_TLV_SZ
#endif
	                     ;


#ifdef PLTFRM_INA219_ENA
   {
       pktLen += (UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                  + DIS_SENSOR_ID_TLV_SZ
				  + UTIL_TLV_HDR_SZ
                  + IN219_BUS_VOLTAGE_OUTPUT_VAL_SZ);

       pktLen += (UTIL_TLV_HDR_SZ   // DIS_TLV_TYPE_SENSOR_OUTPUT
                  + DIS_SENSOR_ID_TLV_SZ
				  + UTIL_TLV_HDR_SZ
				  + IN219_SHUNT_VOLTAGE_OUTPUT_VAL_SZ);
   }
#endif


#ifdef APP_MAX_THROUGHPUT_TEST
   pktLen = 51;
#endif

   if (pktLen == 0x0)
       SYS_fatal(SYS_FATAL_ERR_530);
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

#ifdef PLTFRM_DUMMY_DEV_ENA
       {
           UINT8_t *tlvHdr_p = buff_p;

           buff_p += UTIL_TLV_HDR_SZ;

           buff_p = UTIL_buildTlvHdr(buff_p,
                                     DIS_TLV_TYPE_SENSOR_ID,
                                     DIS_SENSOR_ID_FIELD_SZ);
           *(buff_p ++) = PLTFRM_DUMMY_DEV_ID;

           buff_p = UTIL_buildTlvHdr(buff_p,
                                     DIS_TLV_TYPE_VALUE,
                                     sizeof(APP_msgSeqNr));
           buff_p = UTIL_htonl(buff_p, APP_msgSeqNr);

           UTIL_buildTlvHdr(tlvHdr_p,
                            DIS_TLV_TYPE_SENSOR_OUTPUT,
                            buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);

           APP_msgSeqNr ++;
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

#ifdef PLTFRM_SHT10_ENA
       {
          UINT8_t *tlvHdr_p = buff_p;

          buff_p += UTIL_TLV_HDR_SZ;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_SHT10_1_TEMP_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    SHT10_TEMP_OUTPUT_LEN);
          buff_p = UTIL_htons(buff_p, sht10_tempVal);

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }

       {
          UINT8_t *tlvHdr_p = buff_p;

          buff_p += UTIL_TLV_HDR_SZ;

          if (SHT10_readMeasRHVal(PLTFRM_SHT10_1_TEMP_DEV_ID, &sht10_RHRawVal) != PLTFRM_STS_SUCCESS)
              sht10_RHRawVal = 0;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_SHT10_1_RH_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    SHT10_RH_OUTPUT_LEN);
          buff_p = UTIL_htons(buff_p, sht10_RHRawVal);

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
              SYS_fatal(SYS_FATAL_ERR_71);
#else
          if (LM75B_getTempVal(PLTFRM_LM75B_1_DEV_ID, &temp16) != PLTFRM_STS_SUCCESS)
              SYS_fatal(SYS_FATAL_ERR_72);
#if 0
          if (LM75B_shutDown(PLTFRM_LM75B_1_DEV_ID) != PLTFRM_STS_SUCCESS)
              SYS_fatal(SYS_FATAL_ERR_73);
#endif
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
              SYS_fatal(SYS_FATAL_ERR_74);

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
              SYS_fatal(SYS_FATAL_ERR_75);
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
              SYS_fatal(SYS_FATAL_ERR_76);
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

#ifdef PLTFRM_CC2D33S_SENSOR_ENA
       {
          UINT16_t rhVal;
          SINT16_t tempVal;
          PLTFRM_sts_t sts;
          UINT8_t *tlvHdr_p = buff_p;

          sts = CC2D33S_dataFetch(PLTFRM_CC2D33S_1_DEV_ID, &rhVal, &tempVal);
          if (sts != PLTFRM_STS_SUCCESS)
          {
              SYS_fatal(SYS_FATAL_ERR_77);
          }

          buff_p += UTIL_TLV_HDR_SZ;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_CC2D33S_1_DEV_ID;

#if 0
          // Since this sensor measures multiple parameters, need to add a TLV
          // which will identify the specific parameter.
          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_TYPE,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = DIS_SENSOR_TYPE_RELATIVE_HUMIDITY;
#endif

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    CC2D33S_SENSOR_RH_OUTPUT_VALUE_SZ);
          buff_p = UTIL_htons(buff_p, rhVal);

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
              SYS_fatal(SYS_FATAL_ERR_78);
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

#ifdef PLTFRM_EXT_VOLTAGE_MON_ENA
       {
          UINT16_t adcVal, voltMV = MSP_ADC10_REF_V_2_PT_5/2;
          PLTFRM_sts_t sts;
          UINT8_t *tlvHdr_p = buff_p;
          UINT8_t refV = PLTFRM_ADC10_REF_GEN_2PT5_VOLTS;

          buff_p += UTIL_TLV_HDR_SZ;


          sts = PLTFRM_doADC10SingleConv(PLTFRM_ADC10_CHANN_1_ID,
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
              rt *= (refV == PLTFRM_ADC10_REF_GEN_2PT5_VOLTS ? MSP_ADC10_REF_V_2_PT_5 : MSP_ADC10_REF_V_1_PT_5);
              rt /= 1023;
              rt <<= 1;   // voltage  divided by 2 using op-amp
              voltMV = (UINT16_t)rt;
          }
          else
          {
        	  SYS_fatal(SYS_FATAL_ERR_6003);
          }

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_EXT_VOLTAGE_MON_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    PLTFRM_EXT_VOLTAGE_MON_OP_LEN);
          buff_p = UTIL_htons(buff_p, voltMV);

          UTIL_buildTlvHdr(tlvHdr_p,
                           DIS_TLV_TYPE_SENSOR_OUTPUT,
                           buff_p - tlvHdr_p - UTIL_TLV_HDR_SZ);
       }
#endif

#ifdef PLTFRM_INA219_ENA
       {
          buff_p = FFD_APP_addIN219Info(buff_p, PLTFRM_INA219_3_BV_DEV_ID, PLTFRM_INA219_3_SV_DEV_ID);
       }
#endif

#ifdef PLTFRM_MAINS_DETECT_SNSR_ENA
       {
    	  UINT8_t mainsCurrState;
    	  UINT8_t *tlvHdr_p = buff_p;

          if (MDS_getCurrState(PLTFRM_MDS_1_DEV_ID, &mainsCurrState) != PLTFRM_STS_SUCCESS)
              SYS_fatal(SYS_FATAL_ERR_3031);

          buff_p += UTIL_TLV_HDR_SZ;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_SENSOR_ID,
                                    DIS_SENSOR_ID_FIELD_SZ);
          *(buff_p ++) = PLTFRM_MDS_1_DEV_ID;

          buff_p = UTIL_buildTlvHdr(buff_p,
                                    DIS_TLV_TYPE_VALUE,
                                    MDS_OUTPUT_VALUE_SZ);
          *(buff_p ++) = mainsCurrState;

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

       UTIL_buildTlvHdr(tlvList_p,
                        DIS_TLV_TYPE_SENSOR_OUTPUT_LIST,
                        buff_p - tlvList_p - UTIL_TLV_HDR_SZ);

       FFD_APP_dataTxReqCnt ++;

       ADP_cntxt.txParams.destShortAddr = LPWMN_COORD_MAC_SHORT_ADDR;  // The destination of this message
       ADP_cntxt.txParams.srcModId = ADP_TX_REQ_SRC_APP;

#ifdef CC1101_TIMING_DBG_ENA
       P1OUT |= BIT7;
#endif
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
               SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_APP_DATA_REQ;
#endif
       }
   }
   else
   {
       FFD_APP_memAllocFlrCnt ++;
       APP_txInterval = FFD_APP_commIntervalInSecs;
       if (FFD_appRespTxFlag == 1)
           SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_APP_DATA_REQ;
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
void FFD_APP_collectSensorData (void)
{
   switch (FFD_APP_state)
   {
       case FFD_APP_STATE_CONV_DONE:
#ifdef PLTFRM_SHT10_ENA
            {
               UINT16_t tmoValMilliSecs;

               if (SHT10_startTempMeas(PLTFRM_SHT10_1_TEMP_DEV_ID,
                                       &tmoValMilliSecs) != PLTFRM_STS_SUCCESS)
               {
                   SYS_fatal(SYS_FATAL_ERR_902);
               }
               else
               {
            	   PLTFRM_sts_t sts = PLTFRM_startTimerA1(0, tmoValMilliSecs, FFD_APP_collectSensorData);
                   if (sts != PLTFRM_STS_SUCCESS)
                       SYS_fatal(SYS_FATAL_ERR_903);

                   FFD_APP_state = FFD_APP_STATE_WAIT_CONV_1;
               }
            }
            break;
#endif

       case FFD_APP_STATE_WAIT_CONV_1:
#ifdef PLTFRM_SHT10_ENA
            {
               UINT16_t tmoValMilliSecs;

               if (SHT10_readMeasTempVal(PLTFRM_SHT10_1_TEMP_DEV_ID, &sht10_tempVal) != PLTFRM_STS_SUCCESS)
                   sht10_tempVal = 0;

               if (SHT10_startRHMeas(PLTFRM_SHT10_1_TEMP_DEV_ID,
                                     &tmoValMilliSecs) != PLTFRM_STS_SUCCESS)
               {
                   SYS_fatal(SYS_FATAL_ERR_902);
               }
               else
               {
                   PLTFRM_sts_t sts = PLTFRM_startTimerA1(0, tmoValMilliSecs, FFD_APP_collectSensorData);
                   if (sts != PLTFRM_STS_SUCCESS)
                       SYS_fatal(SYS_FATAL_ERR_903);

                   FFD_APP_state = FFD_APP_STATE_WAIT_CONV_2;
               }
            }
            break;
#endif

       case FFD_APP_STATE_WAIT_CONV_2:
            {
               FFD_APP_state = FFD_APP_STATE_CONV_DONE;

               SYS_globalEvtMsk2 |= SYS_GLOBAL_EVT_PENDING_APP_DATA_REQ;
               FFD_snsrDataTxPendFlag = 1;

               //__buildSendSnsrUpdate();
            }
            break;

       default:
            {
               SYS_fatal(SYS_FATAL_ERR_904);
            }
            break;
   }
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
