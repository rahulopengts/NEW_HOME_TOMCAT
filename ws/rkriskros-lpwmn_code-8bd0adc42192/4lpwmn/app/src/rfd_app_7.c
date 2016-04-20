/*
 * File Name: rfd_app_7.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Jan/19/2014
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

#ifdef DEV_TYPE_RFD
#ifdef RFD_APP_7
#if defined(PLTFRM_AD7797_ENA) \
    && defined(PLTFRM_ON_CHIP_VCC_SENSE_ENA)

#include <typedefs.h>
#include <system.h>
#include <pltfrm.h>
#include <util.h>
#include <mac_pib.h>
#include <adp.h>
#include <rfd_app.h>
#include <ad7797.h>

#define LOAD_CELL_CAL_WEIGHT_GRAMS  800
#define LC_CAL_SAMPLE_CNT_SHIFT  3
#define LC_CAL_SAMPLE_CNT (1 << LC_CAL_SAMPLE_CNT_SHIFT)

SINT32_t LC_totalCalAdcVal = 0, LC_avgCalAdcVal = 0;
UINT8_t LC_calSampleCnt, LC_calDone = 0;

RFD_APP_state_t RFD_APP_state = RFD_APP_STATE_CONVERSION_DONE;

#define RFD_APP_DATA_SEQ_NR_SZ   2
#define RFD_APP_DATA_SNSR_BIT_MSK_SZ   1

UINT16_t RFD_APP_pktSeqNr = 0,
         RFD_APP_dataTxReqCnt = 0,
         RFD_APP_dataRxReqFailCnt = 0,
         RFD_APP_cnfrmOkCnt = 0,
         RFD_APP_cnfrmFlrCnt = 0;




/*
 * Algorithm:
 * Node wakes up every 'x' seconds and triggers a single conversion of the load cell
 * output. Let latest measured weight be Wc grams. Let previous measured weight be
 * Wp grams. If ABS(Wc - Wp) > Wthresh, send latest weight to gateway. If ABS(Wc - Wp) <
 * Wthresh, go back to sleep. In both cases, update Wp = Wo.
 *
 * When node powers up, do internal and system zero scale calibration.
 *
 * Let Nadc be the ADC output at the end of each conversion.
 *
 * 2^23  ->  Vref
 * Nadc -> (Vref x Nadc) / 2^23
 *
 * Vadc can be calculated as using eqn 1
 *   Vadc = (Vref x Nadc) / 2^23
 *   Vsig = Vadc/128
 *   Vsig = (Vref x Nadc) / (128 x 2^23)  --- eqn 1
 *
 * Assuming 'S' is the sensitivity of the load sensor (mv/V output at full scale input)
 *
 *   W = (Vsig x Wfsi) / (S x Vs)     --- eqn 2
 *
 * Eqn 2 gives the weight 'W' needed to produce differential output Vs. 'Wfsi' is the
 * full scale weight/force.
 *
 * From 1 and 2,
 *
 *   W = Vref x Nadc x Wfsi / (128 x 2^23 x S x Vs)
 *
 * Assuming Vref = Vs,
 *
 *   W = (Nadc x Wfsi) / (128 x 2^23 x S)
 *
 * Note that we don't need to know Vs or Vref here when Vs = Vref.
 *
 * Also note that Nadc here has been automatically adjusted by the value zero offset register.
 */


/*
 * When a load cell is not loaded, the differential output of the wheat stone bridge
 * should ideally be 0.
 *   V+ = Vs/2 ,  V- = Vs/2
 *   dV = V+ - V- = 0
 *
 * When the load cell is loaded with weight 'W' N,
 *   V+ = (Vs*(R + dR))/(2R),   V- = (Vs*(R - dR))/2R
 *   dV = (Vs*dR)/R  -- (eqn 1)
 *
 * dR is proportional to the applied load 'W'
 *   dR = K*W -- (eqn 2)
 *
 * Load Sensor specs mention the sensor's sensitivity 'S' which is output voltage of the
 * sensor in mV per volt of excitation with full scale input.
 *
 * Let "Wfsi" be the full scale input. When the sensor is loaded with this force "Wfsi", the
 * sensor's differential output voltage (according to the sensor's spec) will be "S*Vs" where
 * "S" is the sensitivity in mV/V and Vs is the excitation voltage.
 *
 * Combining equations 1 and 2,
 *   dV = (Vs * K * W) / R  -- (eqn 3)
 *
 * when W is Wfsr, dV will be S*Vs
 *   S*Vs = (Vs * K * Wfsi) / R
 *   or
 *   K = (S*R)/Wfsi
 *
 *   From eqn 2, dR = (S*R*W)/Wfsi
 *
 *   From eqn 1, dV = Vs*S*W/Wfsi
 *   or
 *   W = (dV * Wfsi) / (Vs * S) -- (eqn 4)
 *
 *   Equation 4 can be used to calculate the applied force (weight) given the measured differential
 *   output voltage of the load sensor.
 *
 *   dV can be measured using a strain gauge AFE such as the AD7797. The AD7797 amplifies the output
 *   differential voltage of the load sensor and then uses a delta-sigma ADC to provide a 24 bit digital
 *   voltage value. The AD7797's amplifier has a fixed gain of 128.
 *
 *
 *  It is possible that a load sensor has a non zero differential output voltage at zero load. Let us
 *  assume that dR0 is the dR value at zero load.
 *
 *  Then the load sensor's output at zero load is "dV0 = (Vs * dR0) / R"
 *
 *  When the load cell is loaded, then the sensor's output voltage is "dV = (Vs * (dR + dR0)) / R".
 *
 *  dV = Vs*dR/r + Vs*dR0/R  or dV = dV' + dV0
 *
 *  The AD7797 will measure "dV" but we are interested in "dV'".
 *
 *
 *
 *  The AD7797 provide three calibration modes that can be programmed via the mode bits in the mode register.
 *  These are
 *
 *  > Internal zero-scale calibration
 *  > System zero-scale calibration, and
 *  > System full-scale calibration,
 *
 *  which effectively reduces the offset error and full-scale error to the order of the noise. After each
 *  conversion, the ADC conversion result is scaled using the ADC calibration registers before being written to
 *  the data register. The offset calibration coefficient is subtracted from the result prior to multiplication
 *  by the full-scale coefficient.
 *
 *
 *
 *  System calibration allows the converter to compensate for external system gain and offset errors, as well as
 *  its own internal errors. Calibration is basically a conversion process on two specific input voltages (zero-
 *  scale or offset calibration, and full-scale calibration) from which the offset error coefficient and full-scale
 *  error coefficient are determined. With system calibration, the zero-scale voltage and full-scale voltage must
 *  be applied to the ADC by the user.
 *
 *
 *  2^23 -> Vref
 *  Nadc -> Vadc
 *
 *  Vadc = Vref * Nadc / (2^23)
 *
 *  where Vadc is Vsig * 128
 *  So
 *  Vsig = (Vref * Nadc) / (128 * (2^23))
 *
 *  Vsig is dV where dV = dV' + dV0
 *
 *  so dV' + dV0 = (Vref * Nadc) / (128 * (2^23))
 *
 *  dV' = (Vref * (Nadc))/(128 * (2^23)) - dV0
 *
 *  dV' = (Vref * (Nadc))/(128 * (2^23)) - (Vref * (Nadc0))/(128 * (2^23))
 *
 *  dV' = (Vref / (128 * (2^23))) * (Nadc - Nadc0)
 *
 *  Nadc0 can be obtained by performing system zero-scale calibration. The measured offset coefficient is placed in the
 *  offset register. This offset value is subtracted from the output of a normal conversion at the end of each conversion.
 *
 *  Note that dV0 can be negative or positive. If positive, the Nadc0 encoding is 0x800000 + actual value.
 */

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

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_buildSendSnsrUpdate(UINT32_t loadSnsrOp)
{
   UINT8_t pktLen = ADP_ELEMENT_TYPE_LEN
                    + RFD_APP_DATA_SEQ_NR_SZ
                    + RFD_APP_DATA_SNSR_BIT_MSK_SZ
                    + PLTFRM_ON_CHIP_VCC_SENSOR_OUTPUT_LEN
                    + AD7797_ADC_OUTPUT_VAL_SZ;

   // seq-nr                            <2 bytes>
   // bit mask                          <1 byte / 2 bits valid (0 and 1)>
   // On chip Vcc sensor o/p            <2 bytes>
   // ad7797 o/p                        <4 bytes>

   ADP_cntxt.txParams.bdHndl = SYS_allocMem(pktLen);
   if (SYS_BD_HNDL_IS_VALID(ADP_cntxt.txParams.bdHndl))
   {
       UINT8_t *buff_p = SYS_GET_BD_BUFF(ADP_cntxt.txParams.bdHndl);
       UINT8_t *flag_p, snsrFlag = 0x0;

       buff_p += ADP_ELEMENT_TYPE_LEN;

       RFD_APP_pktSeqNr ++;
       buff_p = UTIL_htons(buff_p, RFD_APP_pktSeqNr);

       flag_p = buff_p;
       buff_p ++;  // skip sensor flag field ...

       {
          UINT16_t vcc = pltfrm_vccSense();
          buff_p = UTIL_htons(buff_p, vcc);
          snsrFlag |= BIT0;
       }

       buff_p = UTIL_htonl(buff_p, (UINT32_t)loadSnsrOp);
       snsrFlag |= BIT1;

       *flag_p = snsrFlag;

       ADP_cntxt.txParams.destShortAddr = LPWMN_COORD_MAC_SHORT_ADDR;
       ADP_cntxt.txParams.srcModId = ADP_TX_REQ_SRC_APP;

       RFD_APP_dataTxReqCnt ++;

       if (ADP_appPyldTxReq() != ADP_STS_SUCCESS)
       {
           RFD_APP_dataRxReqFailCnt ++;
           SYS_SET_WAKEUP_INTERVAL(RFD_APP_5_DFLT_WAKE_UP_INTERVAL_SECS);
           SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ENTER_LPM_REQUEST;
       }
   }
}

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_wakeUpProc( )
{
   PLTFRM_sts_t sts;

   if (!MAC_nodeJoined())
	   return;

   if (RFD_APP_state == RFD_APP_STATE_WAIT_CONVERSION)
   {
       SINT32_t adcVal;
       // Get the load value in grams

       sts = AD7797_getADCVal(PLTFRM_AD7797_1_DEV_ID, &adcVal);
       if (sts != PLTFRM_STS_SUCCESS)
       {
           SYS_fatal(SYS_FATAL_ERR_1);
       }

       // W = (Nadc x Wfsi) / (128 x (2^23) x S)
       // W = (Nadc x Wfsi) / ((2^30) x S)

       RFD_APP_state = RFD_APP_STATE_CONVERSION_DONE;

       if (LC_calDone == 0x0)
       {
           if (adcVal > 50000)
           {
               PLTFRM_ON_LED_1();
               PLTFRM_ON_LED_2();

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

               {
                  SINT16_t idx;
                  for (idx=0; idx<100; idx++)
                       PLTFRM_delay1000MicroSecs();
               }

               PLTFRM_OFF_LED_1();
               PLTFRM_OFF_LED_2();
           }
           else
           {
               LC_totalCalAdcVal = 0;
               LC_calSampleCnt = 0;
           }

           SYS_SET_WAKEUP_INTERVAL(RFD_APP_7_DFLT_WAKE_UP_INTERVAL_SECS);
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
   else
   {
       UINT16_t tmoVal;

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

   return;
}

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void APP_nodeJoinDoneInd(void)
{
   SYS_SET_WAKEUP_INTERVAL(RFD_APP_7_DFLT_WAKE_UP_INTERVAL_SECS);
   SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ENTER_LPM_REQUEST;
   RFD_APP_state = RFD_APP_STATE_CONVERSION_DONE;
   return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_prepareToEnterLPM(void)
{
   // Interrupts are disabled globally at this point.
   // The MSP430 will go into deep sleep mode on returning
   // from this function.
   return;
}

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_wakeUpTimerExpEvt(void)
{
   return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void APP_procGPIOEvt()
{
   return;
}

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RFD_APP_evtHndlr(UINT16_t globalEvtId, void *params_p)
{
   switch (globalEvtId)
   {
      case SYS_GLOBAL_EVT_ADP_DATA_CONFIRM:
           {
              ADP_dataCnfrmParams_s *dcParams_p = (ADP_dataCnfrmParams_s *)params_p;
              if (dcParams_p->sts == ADP_STS_SUCCESS)
              {
                  RFD_APP_cnfrmOkCnt ++;
              }
              else
              {
                  RFD_APP_cnfrmFlrCnt ++;
              }
              SYS_SET_WAKEUP_INTERVAL(RFD_APP_7_DFLT_WAKE_UP_INTERVAL_SECS);
              SYS_globalEvtMsk1 |= SYS_GLOBAL_EVT_ENTER_LPM_REQUEST;
           }
           break;

      default:
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
   SINT32_t ad7797OffSetRegVal;

   SYS_SET_WAKEUP_INTERVAL(RFD_APP_7_DFLT_WAKE_UP_INTERVAL_SECS);

   if (AD7797_startSystemOffsetCal(PLTFRM_AD7797_1_DEV_ID, &ad7797OffSetRegVal) != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_1);
   }

   return;
}


#endif
#endif
#endif
