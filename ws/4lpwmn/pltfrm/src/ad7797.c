/*
 * File Name: ad7797.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Jan/16/2014
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

#ifdef PLTFRM_AD7797_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <ad7797.h>
#ifndef AD7797_DIRECT_INTERFACE
#include <uart.h>
#endif

/*
 * -------------------------------
 * Epoch load cell color codes:
 * -------------------------------
 * - White: Sig -ve
 * - Green: Sig +ve
 * - Black: Gnd
 * - Red: Vcc
 * -------------------------------
 */

const UINT16_t AD7797_singleConvTime[  ] =
{
      0,    // X
      0,    // X
      0,    // X
     17,    // 123 HZ -> 2000/123 -> 16.26 msecs -> 17 msecs
     33,    // 62 HZ -> 2000/62 -> 32.26 -> 33 msecs
     40,    // 50 HZ -> 2000/50 -> 40 -> 40 msecs
      0,    // X
     61,    // 33.2 HZ -> 2000/33.2 -> 60.24 -> 61 msecs
      0,    // X
    120,    // 16.7 HZ -> 2000/16.7 -> 119.76 -> 120
    120,    // 16.7 HZ -> 2000/16.7 -> 119.76 -> 120
    160,    // 12.5 HZ -> 2000/12.5 -> 160 -> 160
    200,    // 10 HZ -> 2000/10 -> 200 -> 200 msecs
    241,    // 8.33 HZ -> 2000/8.33 -> 240.09 -> 241 msecs
    320,    // 6.25 HZ -> 2000/6.25 -> 320 -> 320 msecs
    480     // 4.17 HZ -> 2000/4.17 -> 479.61 -> 480 msecs
};

/*
 *-------------------------------------------------------------------------------------
 * <Note 1>
 * To avoid the possibility of interface issues due to spurious clocks, it is
 * advised to take the DIN line of the converter to a logic high as soon as each
 * write to the converter is complete. Since these converters default to waiting
 * for a write to the communications register, taking DIN high when it has completed
 * a sequence prevents invalid data being written to the communications register if
 * spurious clocks occur. At this time, the ADC is waiting for a 0 on DIN before it
 * enables further data into the register. If the part is set up for a write to the
 * communications register with the DIN high, it is effectively immune to spurious
 * serial clocks. This will not prevent spurious clocks received during a write opn
 *  from corrupting the interface.
 *-------------------------------------------------------------------------------------
 */

/*
 *-------------------------------------------------------------------------------------
 * <Note 2>
 * Write access to any of the other registers on the part begins with a write
 * operation to the communication register followed by a write to the selected
 * register. A read operation from any other register (except when continuous
 * read mode is selected) starts with a write to the communication register
 * followed by a read operation from the selected register.
 *-------------------------------------------------------------------------------------
 */

/*
 *-------------------------------------------------------------------------------------
 * <Note 3>
 * In single conversion mode, the AD7796/AD7797 are placed in shutdown mode between
 * conversions. When a single conversion is initiated by setting MD2, MD1, and MD0
 * in the mode register to 0, 0, and 1, respectively, the part powers up, performs
 * a single conversion, and then returns to shutdown mode. The on-chip oscillator
 * requires 1 ms to power-up. A conversion requires a time period of 2 × tADC. DOUT/RDY
 * goes low to indicate the completion of a conversion. When the data-word has been read
 * from the data register, DOUT/RDY goes high. If CS is low, DOUT/RDY remains high until
 * another conversion is initiated and completed. The data register can be read several
 * times, if required, even when DOUT/RDY has gone high.
 *-------------------------------------------------------------------------------------
 */

/*
 *-------------------------------------------------------------------------------------
 * <Note 4>
 * When the ADC is configured for bipolar operation, the output code is offset binary with
 *  - A negative full-scale voltage resulting in a code of 000...000,
 *  - A zero differential input voltage resulting in a code of 100...000, and
 *  - A positive full-scale input voltage resulting in a code of 111...111.
 *
 * The output code for any analog input voltage can be represented as
 *         Code = (2^(N – 1)) × [((AIN × 128)/Vref) + 1]
 * N is 24 for the AD7797.
 *
 *-------------------------------------------------------------------------------------
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
 * Combining equations 1 and 2,
 *   dV = (Vs * K * W) / R  -- (eqn 3)
 *
 *   dV = (Vs x Nadc) / (128 x 2^23)  --- (eqn 4)
 *
 *   Or
 *
 *   (K * W)/R = Nadc / (128 * 2^23)   --- (eqn 5) - Note Vs (supply voltage is not there in the equation).
 *
 *   Let us caliberate the load cell by using known weigh of 1 Kg.
 *
 *   Then K/R = Nadc1 / (128 * 2^23)  =>  (K * 128 * 2^23 / R) = Nadc1
 *
 *
 *   Now we find unknown weight Wu given ADC output is Nadcu
 *
 *   (K * Wu)/R = Nadcu / (128 * 2^23)   => Wu = (Nadcu * (R/K) ) / (128 * 2^23)
 *
 *   Or
 *
 *   Wu = Nadc / Nadc1  since R / (K * 128 * 2^23) = 1/Nadc1
 *
 *
 *   This calculation does not depend on changing supply voltage.
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
 * SPI interface requirement -
 * > Clock polarity (inactive state is high)
 * > Clock phase (Data is changed on the first UCLK edge and captured on the
 *   following edge)
 */

AD7797_cntxt_s AD7797_cntxtList[PLTFRM_AD7797_DEV_CNT] =
{
   {
     PLTFRM_AD7797_1_DEV_ID,
     PLFRM_SPI_HW_BUS_1_ID,
     PLTFRM_GPIO_PORT_1,
     PLTFRM_GPIO_PORT_PIN_4,
     AD7797_CLK_SRC_INT_64_KHZ,
     AD7797_UPDATE_RATE_4_POINT_17_HZ,
     AD7797_ADC_INPUT_BI_POLAR,
     AD7797_CHANN_AINP_MINUS_AINN
   }
};

const SINT32_t __OFFSET_BINARY_ZERO_ENC = (SINT32_t)0x800000;


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
AD7797_cntxt_s *AD7797_getDevCntxt(UINT8_t devId)
{
#if PLTFRM_AD7797_DEV_CNT==1
   if (AD7797_cntxtList[0].devId == devId)
       return (&(AD7797_cntxtList[0]));
#else
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_AD7797_DEV_CNT; idx++)
   {
       if (AD7797_cntxtList[idx].devId == devId)
           return (&(AD7797_cntxtList[idx]));
   }
#endif

   return NULL;
}

#ifdef AD7797_DIRECT_INTERFACE

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void AD7797_spiCsOn(AD7797_cntxt_s *devCntxt_p)
{
   (*(devCntxt_p->csReg_p)) &= (~(1 << devCntxt_p->portPinNr));
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void AD7797_spiCsOff(AD7797_cntxt_s *devCntxt_p)
{
   (*(devCntxt_p->csReg_p)) |= (1 << devCntxt_p->portPinNr);
}


/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t AD7797_spiTxRx(UINT8_t busId, UINT8_t val)
{
    UINT8_t rdVal, globalIntFlag;

    PLTFRM_intSaveDisable(globalIntFlag);

    // USCI busy. This bit indicates if a transmit or receive
    // operation is in progress.
    //  > 0b = USCI inactive
    //  > 1b = USCI transmitting or receiving
    while ((UCB0STAT & UCBUSY));

    /*
     * If this line is uncommented, UART is getting stuck !!
     */
    // IFG2 &= ~(UCB0RXIFG);  // clear the char received bit

    // UCTXIFG is set when UCAxTXBUF empty.
    // Data written to UCxTXBUF when UCTXIFG = 0 may result in
    // erroneous data transmission.
    // A set transmit interrupt flag, UCTXIFG, indicates that data
    // has moved from UCxTXBUF to the TX shift register and UCxTXBUF
    // is ready for new data. It does not indicate RX/TX completion.
    while (!(IFG2 & UCB0TXIFG));     // USCI_A3 TX buffer ready?

    // UCTXIFG is automatically reset if a character is written to UCxTXBUF
    UCB0TXBUF = val;

    // UCRXIFG is set when UCAxRXBUF has received a complete character.
    // Data on UCxSOMI is shifted into the receive shift register on the
    // opposite clock edge. When the character is received, the receive
    // data is moved from the receive (RX) shift register to the received
    // data buffer UCxRXBUF and the receive interrupt flag UCRXIFG is set,
    // indicating the RX/TX operation is complete
    while (!(IFG2 & UCB0RXIFG));

    // UCRXIFG is automatically reset when UCxRXBUF is read.
    rdVal = UCB0RXBUF;

    PLTFRM_intRestore(globalIntFlag);

    return rdVal;
}



/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void __AD7797_writeReg(AD7797_cntxt_s *devCntxt_p, UINT8_t regAddr, UINT16_t regVal)
{
   UINT16_t commRegVal = 0;

   if (regVal == AD7797_STS_REG_ADDR
       || regVal == AD7797_DATA_REG_ADDR
       || regVal == AD7797_ID_REG_ADDR)
   {
       // These are read only registers !!
       SYS_fatal(SYS_FATAL_ERR_210);
   }

   // Set up the communication register for writing to "regAddr"
   commRegVal = regAddr;
   commRegVal = (commRegVal << AD7797_COMM_REG_RA_SHIFT) & AD7797_COMM_REG_RA_BM;
   commRegVal &= ~AD7797_COMM_REG_RW_BM;

   AD7797_spiCsOn(devCntxt_p);

   PLTFRM_delay10MicroSecs();

   AD7797_spiTxRx(devCntxt_p->busId, (UINT8_t)commRegVal);

   PLTFRM_delay10MicroSecs();

   commRegVal = regVal >> 8;
   AD7797_spiTxRx(devCntxt_p->busId, (UINT8_t)commRegVal);

   PLTFRM_delay10MicroSecs();

   AD7797_spiTxRx(devCntxt_p->busId, (UINT8_t)regVal);

   PLTFRM_delay10MicroSecs();

   AD7797_spiCsOff(devCntxt_p);

   return;
}

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
UINT32_t __AD7797_readReg(AD7797_cntxt_s *devCntxt_p, UINT8_t regAddr)
{
   UINT8_t commRegVal = 0;
   UINT32_t devReg;

   /*
    * All communication to the part must start with a write operation to this register. The
    * data written to the communication register determines whether the next operation is a
    * read or write operation, and selects the register where this operation takes place.
    */
   commRegVal = regAddr;
   commRegVal = (commRegVal << AD7797_COMM_REG_RA_SHIFT) & AD7797_COMM_REG_RA_BM;
   commRegVal |= AD7797_COMM_REG_RW_BM;

   AD7797_spiCsOn(devCntxt_p);

   // T8 <CSn falling edge to SCLK active edge setup time> - min val (0 nanosecs)
   PLTFRM_delay10MicroSecs();

   AD7797_spiTxRx(devCntxt_p->busId, commRegVal);

   /*
    * If a 1 is the first bit written, the part does not clock onto subsequent bits in the
    * register; it stays at this bit location until a 0 is written.
    */
   commRegVal = AD7797_COMM_REG_WEN_BM | 0x1;  // Why LSB is set to 0x1 ? See "Note 1" in this file.

   PLTFRM_delay10MicroSecs();

   devReg = AD7797_spiTxRx(devCntxt_p->busId, commRegVal);

   if (regAddr != AD7797_STS_REG_ADDR
       && regAddr != AD7797_ID_REG_ADDR)
   {
       UINT8_t spiRdVal;

       devReg = (devReg << 8) & 0xff00;
       PLTFRM_delay10MicroSecs();
       spiRdVal = AD7797_spiTxRx(devCntxt_p->busId, commRegVal);
       devReg |= spiRdVal;

       if (regAddr == AD7797_DATA_REG_ADDR
           || regAddr >= AD7797_OFFSET_REG_ADDR)
       {
           // These registers are 24 bit
           devReg = (devReg << 8) & 0xffff00;
           PLTFRM_delay10MicroSecs();
           spiRdVal = AD7797_spiTxRx(devCntxt_p->busId, commRegVal);
           devReg |= spiRdVal;
       }
   }

   PLTFRM_delay10MicroSecs();
   AD7797_spiCsOff(devCntxt_p);

   // Write 0xff to bus without enabling CSn of any attached device
   // AD7797_spiTxRx(devCntxt_p->busId, 0xff);

   return devReg;
}
#endif

#ifndef AD7797_DIRECT_INTERFACE
UINT8_t AD7797_rcvBuff[UART_RX_CIRC_BUFF_SZ];
#endif

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t AD7797_getADCVal(UINT8_t devId, SINT32_t *adcVal_p)
{
   AD7797_cntxt_s *devCntxt_p = AD7797_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

#ifndef AD7797_DIRECT_INTERFACE
   {
	  UINT8_t dataLen;
      SINT32_t adcVal;

      // Check if conversion value received over UART or not !!
      dataLen = UART_getRxBuffData(AD7797_rcvBuff);
      if (dataLen == 0x0)
      {
          adcVal = 0x7ffffffe;
    	  goto _skip;
      }
      if ((AD7797_rcvBuff[0] == '<')
          && (AD7797_rcvBuff[6] == '>')
          && ((AD7797_rcvBuff[1] == '+') || (AD7797_rcvBuff[1] == '-')))
      {
          adcVal = AD7797_rcvBuff[2];
          adcVal <<= 8;
          adcVal |= AD7797_rcvBuff[3];
          adcVal <<= 8;
          adcVal |= AD7797_rcvBuff[4];
          adcVal <<= 8;
          adcVal |= AD7797_rcvBuff[5];

          if (AD7797_rcvBuff[1] == '-')
              adcVal = -adcVal;
      }
      else
          adcVal = 0x7fffffff;

_skip:
      *adcVal_p = adcVal;
   }
#else
   /*
    * Read the RDYn bit in the status register. This bit is cleared by the AD7797
    * when data is written to the ADC data register. Set automatically after the
    * ADC data register has been read or before the data register is updated with
    * a new conversion result to indicate to the user not to read the conversion
    * data. It is also set when the part is placed in power-down mode.
    */
   UINT32_t regVal = __AD7797_readReg(devCntxt_p, AD7797_STS_REG_ADDR);
   if (regVal & AD7797_STS_REG_RDY_BM)
   {
       return PLTFRM_STS_DEV_DATA_NOT_READY;
   }

   regVal = __AD7797_readReg(devCntxt_p, AD7797_DATA_REG_ADDR);

   /*
    * In bi-polar mode, the output code for any analog input voltage (Vai) can be represented
    * as
    *       Code = 2^(N – 1) × [(Vai × 128 /VREF) + 1]
    *       where N = 24 for ADC7797.
    *
    *       Code = (2^23) x [(Vai x 128/Vref) + 1]
    *
    *       Vai = (((Code / (2^23)) - 1) x Vref)/128
    */

   *adcVal_p = regVal;

   if (devCntxt_p->polarity == AD7797_ADC_INPUT_BI_POLAR)
   {
       /*
        * When the ADC is configured for bipolar operation, the output code is offset binary with
        * a negative full-scale voltage resulting in a code of 000...000, a zero differential input
        * voltage resulting in a code of 100...000, and a positive full-scale input voltage
        * resulting in a code of 111...111.
        */
       *adcVal_p -= __OFFSET_BINARY_ZERO_ENC;
   }
   else
   {
       /*
        * When the ADC is configured for uni-polar operation, the output code is natural (straight)
        * binary with a zero differential input voltage resulting in a code of 00...00, a mid scale
        * voltage resulting in a code of 100...000, and a full-scale input voltage resulting in a
        * code of 111...111.
        */
   }
#endif

   return PLTFRM_STS_SUCCESS;
}

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t AD7797_startSingleConv(UINT8_t devId, UINT16_t *tmoVal_p)
{
   AD7797_cntxt_s *devCntxt_p = AD7797_getDevCntxt(devId);
#ifdef AD7797_DIRECT_INTERFACE
   UINT16_t regVal, opnMode;
#endif

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

#ifdef AD7797_DIRECT_INTERFACE

   // See note 3
   // The mode register is a 16-bit read/write register.
   regVal = (UINT16_t)__AD7797_readReg(devCntxt_p, AD7797_MODE_REG_ADDR);
   opnMode = (regVal >> AD7797_MODE_REG_MODE_SEL_SHIFT) & AD7797_MODE_REG_MODE_SEL_SHIFT_BM;
   if (opnMode != AD7797_MODE_POWER_DOWN)
       return PLTFRM_STS_DEV_INV_OPN_MODE;

   /*
    * Single Conversion Mode. When single conversion mode is selected, the ADC powers up and
    * a single conversion. The oscillator requires 1 ms to power up and settle. The ADC then
    * performs the conversion, which takes a time of 2/fADC. The conversion result is placed
    * in the data register, RDY goes low, and the ADC returns to power-down mode. The conversion
    * remains in the data register and RDY remains active (low) until the data is read or another
    * conversion is performed.
    */
   opnMode = AD7797_MODE_SINGLE_COV;
   opnMode = (opnMode << AD7797_MODE_REG_MODE_SEL_SHIFT) & AD7797_MODE_REG_MODE_SEL_BM;
   regVal &= ~(AD7797_MODE_REG_MODE_SEL_BM);
   regVal |= opnMode;
   __AD7797_writeReg(devCntxt_p, AD7797_MODE_REG_ADDR, regVal);

   PLTFRM_delay1000MicroSecs();

   regVal = (UINT16_t)__AD7797_readReg(devCntxt_p, AD7797_STS_REG_ADDR);
   if (!(regVal & AD7797_STS_REG_RDY_BM))
   {
       return PLTFRM_STS_DEV_REG_UPDATE_ERR;
   }
#else

   // The AD7797 is handle by another MSP430. Send trigger to the other MSP430
   // to perform a single conversion. Active high trigger.

#ifdef PLTFRM_UART_ENA
   UART_flushRcvdData();
#else
#error UART not enabled !!
#endif

#ifdef AD7797_TRIGGER_THROUGH_INTR
   if (PLTFRM_setGPIOPinOutput(devCntxt_p->convTriggerPortId,
                               devCntxt_p->convTriggerPortPinNr, 0x0) != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_618);
   }

   PLTFRM_delay100MicroSecs();

   if (PLTFRM_setGPIOPinOutput(devCntxt_p->convTriggerPortId,
                               devCntxt_p->convTriggerPortPinNr, 0x1) != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_618);
   }

   PLTFRM_delay100MicroSecs();

   if (PLTFRM_setGPIOPinOutput(devCntxt_p->convTriggerPortId,
                               devCntxt_p->convTriggerPortPinNr, 0x0) != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_619);
   }
#else
   UART_wrByte('S');
#endif
#endif

   *tmoVal_p = (AD7797_singleConvTime[devCntxt_p->updateRate] + ADC7797_SINGLE_CONV_SETTLING_TIME_MSECS) + 100;

#ifndef AD7797_DIRECT_INTERFACE
   *tmoVal_p += 500;  // Wait for the other micro to send the conversion output over UART.
#endif

   return PLTFRM_STS_OPN_IN_PROGRESS;
}

#ifdef AD7797_DIRECT_INTERFACE

UINT16_t AD7797_offCalLoopCnt = 0;
SINT32_t offsetRegVal0 = 0;
/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t AD7797_startSystemOffsetCal(UINT8_t devId, SINT32_t *offsetRegVal_p)
{
   AD7797_cntxt_s *devCntxt_p = AD7797_getDevCntxt(devId);
   UINT16_t regVal, opnMode;

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   // The mode register is a 16-bit read/write register.
   regVal = (UINT16_t)__AD7797_readReg(devCntxt_p, AD7797_MODE_REG_ADDR);
   opnMode = (regVal >> AD7797_MODE_REG_MODE_SEL_SHIFT) & AD7797_MODE_REG_MODE_SEL_SHIFT_BM;
   if (opnMode != AD7797_MODE_POWER_DOWN)
       return PLTFRM_STS_DEV_INV_OPN_MODE;

   offsetRegVal0 = __AD7797_readReg(devCntxt_p, AD7797_OFFSET_REG_ADDR);

   /*
    * User should connect the system zero-scale input to the channel input pins.
    *
    * To start a calibration, write the relevant value to the MD2 to MD0 bits in the mode
    * register. DOUT/RDYn goes high when the calibration is initiated. After the calibration
    * is complete, the contents of the corresponding calibration registers are updated, the
    * RDY bit in the status register is set, the DOUT/RDYn pin goes low (if CS is low), and
    * the AD7796/AD7797 revert to idle mode.
    *
    * Both an internal offset calibration and system offset calibration takes two conversion
    * cycles.
    */

   opnMode = AD7797_MODE_ZERO_SCALE_CAL;
   opnMode = (opnMode << AD7797_MODE_REG_MODE_SEL_SHIFT) & AD7797_MODE_REG_MODE_SEL_BM;
   regVal &= ~(AD7797_MODE_REG_MODE_SEL_BM);
   regVal |= opnMode;
   __AD7797_writeReg(devCntxt_p, AD7797_MODE_REG_ADDR, regVal);

   PLTFRM_delay1MilliSec();

   do
   {
       SINT16_t idx;
       regVal = (UINT16_t)__AD7797_readReg(devCntxt_p, AD7797_STS_REG_ADDR);
       if (!(regVal & AD7797_STS_REG_RDY_BM))
           break;
       for (idx=0; idx<1; idx++)
           PLTFRM_delay1MilliSec();
       AD7797_offCalLoopCnt ++;
   } while (1);

   regVal = (UINT16_t)__AD7797_readReg(devCntxt_p, AD7797_MODE_REG_ADDR);
   opnMode = (regVal >> AD7797_MODE_REG_MODE_SEL_SHIFT) & AD7797_MODE_REG_MODE_SEL_SHIFT_BM;
   if (opnMode != AD7797_MODE_IDLE)
       return PLTFRM_STS_DEV_INV_OPN_MODE;

   opnMode = AD7797_MODE_POWER_DOWN;
   opnMode = (opnMode << AD7797_MODE_REG_MODE_SEL_SHIFT) & AD7797_MODE_REG_MODE_SEL_BM;
   regVal &= ~(AD7797_MODE_REG_MODE_SEL_BM);
   regVal |= opnMode;
   __AD7797_writeReg(devCntxt_p, AD7797_MODE_REG_ADDR, regVal);

   PLTFRM_delay1MilliSec();

   *offsetRegVal_p = __AD7797_readReg(devCntxt_p, AD7797_OFFSET_REG_ADDR);

   return PLTFRM_STS_SUCCESS;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t AD7797_spiInit(UINT8_t devId)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   AD7797_cntxt_s *devCntxt_p = AD7797_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   sts = PLTFRM_allocGPIOPin(PLTFRM_AD7797_1_DEV_ID,
                             devCntxt_p->portId,
                             devCntxt_p->portPinNr,
                             PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                             PLTFRM_GPIO_OUTPUT_HIGH,
                             PLTFRM_GPIO_INT_EDGE_NA,
                             NULL);
   if (sts != PLTFRM_STS_SUCCESS)
   {
       SYS_fatal(SYS_FATAL_ERR_211);
   }

   switch (devCntxt_p->portId)
   {
       case PLTFRM_GPIO_PORT_1:
            devCntxt_p->csReg_p = &(P1OUT);
            break;

       case PLTFRM_GPIO_PORT_2:
            devCntxt_p->csReg_p = &(P2OUT);
            break;

       case PLTFRM_GPIO_PORT_3:
            devCntxt_p->csReg_p = &(P3OUT);
            break;

       case PLTFRM_GPIO_PORT_4:
            devCntxt_p->csReg_p = &(P4OUT);
            break;

       default:
            sts = PLTFRM_STS_INV_HW_RESOURCE;
            break;
   }

   return sts;
}
#endif

SINT32_t adcVal;
SINT32_t minAdcVal = 0x7fffffff, maxAdcVal = 0x80000000;
UINT32_t sampleCnt;



/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t AD7797_init(UINT8_t devId)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   AD7797_cntxt_s *devCntxt_p = AD7797_getDevCntxt(devId);


#ifndef AD7797_DIRECT_INTERFACE
   {
#ifdef AD7797_TRIGGER_THROUGH_INTR
	   sts = PLTFRM_allocGPIOPin(PLTFRM_AD7797_1_DEV_ID,
	                             devCntxt_p->convTriggerPortId,
	                             devCntxt_p->convTriggerPortPinNr,
	                             PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
	                             PLTFRM_GPIO_OUTPUT_HIGH,
	                             PLTFRM_GPIO_INT_EDGE_NA,
	                             NULL);
	   if (sts != PLTFRM_STS_SUCCESS)
	   {
	       SYS_fatal(SYS_FATAL_ERR_211);
	   }

       switch (devCntxt_p->convTriggerPortId)
       {
           case PLTFRM_GPIO_PORT_1:
                devCntxt_p->conTriggerReg_p = &(P1OUT);
                break;

           case PLTFRM_GPIO_PORT_2:
                devCntxt_p->conTriggerReg_p = &(P2OUT);
                break;

           case PLTFRM_GPIO_PORT_3:
                devCntxt_p->conTriggerReg_p = &(P3OUT);
                break;

           case PLTFRM_GPIO_PORT_4:
                devCntxt_p->conTriggerReg_p = &(P4OUT);
                break;

           default:
                sts = PLTFRM_STS_INV_HW_RESOURCE;
                break;
        }
#endif
   }
#else
   UINT16_t regVal;

   AD7797_spiCsOff(devCntxt_p);

   PLTFRM_delay1MilliSec();

   /*
    * Writing 32 ones to the interface will reset the serial interface into a known
    * state where the ADC is waiting for a write to the communications register.
    * This resets the interface and all of its internal registers to their power-on
    * reset values.
    * This is a write operation of at least 32 serial clock cycles with DIN high.
    */
   {
      SINT16_t idx;
      AD7797_spiCsOn(devCntxt_p);
      // T8 <CSn falling edge to SCLK active edge setup time> - min val (0 nanosecs)
      PLTFRM_delay1MicroSecs();
      for (idx=0; idx<4; idx++)
           AD7797_spiTxRx(devCntxt_p->busId, 0xff);
      PLTFRM_delay1MicroSecs();
      AD7797_spiCsOff(devCntxt_p);
   }

   regVal = __AD7797_readReg(devCntxt_p, AD7797_ID_REG_ADDR);
   if (regVal != AD7797_ID_REG_VAL)
       return PLTFRM_STS_DEV_PART_NR_MISMATCH;

   {
      UINT16_t currRegVal;

      // The mode register is a 16-bit read/write register.
      regVal = __AD7797_readReg(devCntxt_p, AD7797_MODE_REG_ADDR);
      currRegVal = regVal;

      {
         UINT16_t opnMode = (regVal >> AD7797_MODE_REG_MODE_SEL_SHIFT) & AD7797_MODE_REG_MODE_SEL_SHIFT_BM;
         /*
          * Default power up mode is continuous conversion mode. Put the AD7797 in
          * shut down mode.
          */
         if (opnMode != AD7797_MODE_POWER_DOWN)
         {
             opnMode = AD7797_MODE_POWER_DOWN;
             opnMode = (opnMode << AD7797_MODE_REG_MODE_SEL_SHIFT) & AD7797_MODE_REG_MODE_SEL_BM;
             regVal &= ~(AD7797_MODE_REG_MODE_SEL_BM);
             regVal |= opnMode;
         }
      }

      {
         UINT16_t clkSel = (regVal >> AD7797_MODE_REG_CLK_SRC_SHIFT) & AD7797_MODE_REG_CLK_SRC_SHIFT_BM;
         if (clkSel != devCntxt_p->clkSrc)
         {
             clkSel = devCntxt_p->clkSrc;
             clkSel = (clkSel << AD7797_MODE_REG_CLK_SRC_SHIFT) & AD7797_MODE_REG_CLK_SRC_BM;
             regVal &= ~(AD7797_MODE_REG_CLK_SRC_BM);
             regVal |= clkSel;
         }
      }

      {
         UINT16_t updRate = (regVal >> AD7797_MODE_REG_FSX_SHIFT) & AD7797_MODE_REG_FSX_SHIFT_BM;
         if (updRate != devCntxt_p->updateRate)
         {
             updRate = devCntxt_p->updateRate;
             updRate = (updRate << AD7797_MODE_REG_FSX_SHIFT) & AD7797_MODE_REG_FSX_BM;
             regVal &= ~(AD7797_MODE_REG_FSX_BM);
             regVal |= updRate;
         }
      }

      if (regVal != currRegVal)
      {
          __AD7797_writeReg(devCntxt_p, AD7797_MODE_REG_ADDR, regVal);
          currRegVal = __AD7797_readReg(devCntxt_p, AD7797_MODE_REG_ADDR);
          if (currRegVal != regVal)
          {
              return PLTFRM_STS_DEV_REG_UPDATE_ERR;
          }
      }
   }

   {
      UINT16_t currRegVal;

      // The config register is a 16-bit read/write register.
      regVal = (UINT16_t)__AD7797_readReg(devCntxt_p, AD7797_CFG_REG_ADDR);
      currRegVal = regVal;

      {
         UINT16_t polarity = (regVal >> AD7797_CFG_REG_POLARITY_SHIFT) & AD7797_CFG_REG_POLARITY_SHIFT_BM;
         if (polarity != devCntxt_p->polarity)
         {
             regVal ^= AD7797_CFG_REG_POLARITY_BM;
         }
      }

      {
         UINT16_t adcChann = (regVal >> AD7797_CFG_REG_CHAN_SHIFT) & AD7797_CFG_REG_CHAN_SHIFT_BM;
         if (adcChann != devCntxt_p->adcChann)
         {
             adcChann = devCntxt_p->adcChann;
             adcChann = (adcChann << AD7797_CFG_REG_CHAN_SHIFT) & AD7797_CFG_REG_CHAN_BM;
             regVal &= ~AD7797_CFG_REG_CHAN_SHIFT_BM;
             regVal |= adcChann;
         }
      }

      if (regVal != currRegVal)
      {
          __AD7797_writeReg(devCntxt_p, AD7797_CFG_REG_ADDR, regVal);
          currRegVal = __AD7797_readReg(devCntxt_p, AD7797_CFG_REG_ADDR);
          if (currRegVal != regVal)
          {
              return PLTFRM_STS_DEV_REG_UPDATE_ERR;
          }
      }
   }

   {
       UINT16_t tmoVal = 0, idx;


       sts = AD7797_startSingleConv(PLTFRM_AD7797_1_DEV_ID, &tmoVal);
       if (sts != PLTFRM_STS_OPN_IN_PROGRESS)
       {
           SYS_fatal(SYS_FATAL_ERR_1);
       }

       for (idx=0; idx<tmoVal + 5; idx++)
            PLTFRM_delay1MilliSec();


       // Get the load value in grams
       sts = AD7797_getADCVal(PLTFRM_AD7797_1_DEV_ID, &adcVal);
       if (sts != PLTFRM_STS_SUCCESS)
       {
           SYS_fatal(SYS_FATAL_ERR_1);
       }

       if (adcVal < minAdcVal)
           minAdcVal = adcVal;

       if (adcVal > maxAdcVal)
           maxAdcVal = adcVal;

       sampleCnt ++;
   }
#endif

   return sts;
}

#endif
