/*
 * File Name: MAX_SONAR.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Feb/12/2015
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


#ifdef PLTFRM_MAX_SONAR_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <max_sonar.h>


static const char _snsrName[] = "LV_MAX_SONAR_EZ";
static const char _snsrMfr[] = "MAXBOTIX";

MAX_SONAR_cntxt_s MAX_SONAR_cntxtList[PLTFRM_MAX_SONAR_DEV_CNT] =
{
   {
     PLTFRM_MAX_SONAR_1_DEV_ID,
     PLTFRM_GPIO_PORT_2,
     PLTFRM_GPIO_PORT_PIN_1,
     0, 0, 0, 0,
     {_snsrName, _snsrMfr}
   },
};


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
MAX_SONAR_cntxt_s *MAX_SONAR_getDevCntxt(UINT8_t devId)
{
   return devId == PLTFRM_MAX_SONAR_1_DEV_ID ? &(MAX_SONAR_cntxtList[0]) : 0x0;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t __UART_init(void)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   /*
    * USCI_Ax - (UART/SPI/IrDA/LIN)
    * USCI_Bx - (SPI/I2C)
    */

   /*
    * Using the USCI_A0 USCI module.
    *
    * USCI Introduction: UART Mode
    * In asynchronous mode, the USCI_Ax modules connect the MSP430 to an
    * external system via two external pins, UCAxRXD and UCAxTXD. UART mode
    * is selected when the UCSYNC bit is cleared.
    */

#if defined(__MSP430G2955__)
   {
	  PLTFRM_sts_t sts;

      // P3.5 - UCA0RXD
      // P3.4 - UCA0TXD
      sts = PLTFRM_reserveGPIOPin(PLTFRM_UART_HW_1_DEV_ID,
                                  PLTFRM_GPIO_PORT_3,
                                  PLTFRM_GPIO_PORT_PIN_4);
      if (sts != PLTFRM_STS_SUCCESS)
          return sts;

      sts = PLTFRM_reserveGPIOPin(PLTFRM_UART_HW_1_DEV_ID,
                                  PLTFRM_GPIO_PORT_3,
                                  PLTFRM_GPIO_PORT_PIN_5);
      if (sts != PLTFRM_STS_SUCCESS)
          return sts;
   }
   P3SEL |= BIT4 + BIT5;                    // P1.1 = RXD, P1.2 = TXD
   P3SEL2 &= ~(BIT4 + BIT5);
#else
#error micro variant not specified !!
#endif

   UCA0CTL1 |= UCSSEL_2;      // CLK = SMCLK (8 or 16 MHz)

#if (CPU_ACTIVE_CLOCK_IN_MHZ == 8)
   UCA0BR1 = 3;
   UCA0BR0 = 65;
   UCA0MCTL = UCBRS1;         // Modulation UCBRSx
#elif (CPU_ACTIVE_CLOCK_IN_MHZ == 16)
   UCA0BR1 = 0x6;
   UCA0BR0 = 0x82;
   UCA0MCTL = UCBRS1;         // Modulation UCBRSx
#else
#error not supported !!
#endif

   /*
    * The USCI module is enabled by clearing the UCSWRST bit and the transmitter
    * is ready and in an idle state. The transmit baud rate generator is ready but
    * is not clocked nor producing any clocks.
    */
   UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**

   /*
    * The USCI has one interrupt vector for transmission and one interrupt
    * vector for reception.
    *
    * USCI_Ax and USCI_Bx share the same interrupt vectors. The receive interrupt
    * flags UCAxRXIFG and UCBxRXIFG are routed to one interrupt vector, the transmit
    * interrupt flags UCAxTXIFG and UCBxTXIFG share another interrupt vector.
    *
    * UCA0TXIFG Bit 1 : USCI_A0 transmit interrupt flag.
    *                   UCA0TXIFG is set when UCA0TXBUF is empty.
    *                   UCAxTXIFG is automatically reset if a character is written to UCAxTXBUF.
    *                   UCAxTXIFG is set after a PUC or when UCSWRST = 1.
    *                    - 0 No interrupt pending
    *                    - 1 Interrupt pending
    *
    * UCA0RXIFG Bit 0 : USCI_A0 receive interrupt flag.
    *                   UCA0RXIFG is set when UCA0RXBUF has received a complete character.
    *                   UCAxRXIFG is automatically reset when UCAxRXBUF is read.
    *                   UCAxRXIFG is reset by a system reset PUC signal or when UCSWRST = 1.
    *                    - 0 No interrupt pending
    *                    - 1 Interrupt pending
    */

   IE2 &= ~UCA0RXIE;  // Disable USCI_A0 RX interrupt
   IE2 &= ~UCA0TXIE;  // Disable USCI_A0 RX interrupt

   IFG2 &= ~(UCA0RXIFG | UCA0TXIFG);

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
#ifdef ECLIPSE_IDE
static void __attribute__((__interrupt__(USCIAB0RX_VECTOR))) MAX_SONAR_UART_usci0RxISR (void)
#else
#pragma vector=USCIAB0RX_VECTOR
__interrupt void MAX_SONAR_UART_usci0RxISR(void)
#endif
{
   UINT8_t UART_rxByte;
   MAX_SONAR_cntxt_s *devCntxt_p = &(MAX_SONAR_cntxtList[0]);

   /*
    * Note that interrupts are disabled at this point since GIE bit in SR is
    * zero.
    */

   /*
    * The UCAxRXIFG interrupt flag is set each time a character is received
    * and loaded into UCAxRXBUF. An interrupt request is generated if UCAxRXIE
    * and GIE are also set. UCAxRXIFG is automatically reset when UCAxRXBUF is
    * read.
    *
    * Note that the USCI_Ax and USCI_Bx share the same interrupt vectors. The
    * receive interrupt flags UCAxRXIFG and UCBxRXIFG are routed to one interrupt
    * vector, the transmit interrupt flags UCAxTXIFG and UCBxTXIFG share another
    * interrupt vector.
    */

   // UCAxRXIFG is automatically reset when UCAxRXBUF is read
   UART_rxByte = UCA0RXBUF;

   if (devCntxt_p->rangingInitiated == 0x0)
       return;

   devCntxt_p->opBuff[devCntxt_p->opBuffIdx ++] = UART_rxByte;

   if (devCntxt_p->opBuffIdx >= MAX_SONAR_SERIAL_OP_LEN)
   {
       devCntxt_p->rangingInitiated = 0;
       devCntxt_p->opRcvd = 1;
       PLTFRM_exitLPM();  // Wake up micro if sleeping in LPM3
   }

   return;
}


/*
 * 250 mS after power-up, the LV-MaxSonar-EZ is ready to accept the RX command.
 * If the RX pin is left open or held high, the sensor will first run a calibration
 * cycle (49 mS), and then it will take a range reading (49mS). After the power up
 * delay, the first reading will take an additional ~100 mS. Subsequent readings
 * will take 49 mS. The LV-MaxSonar-EZ checks the RX pin at the end of every cycle.
 * Range data can be acquired once every 49 mS.
 */

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t MAX_SONAR_getLatestRangeReading(const UINT8_t devId,
                                             UINT16_t *rangeInInches)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   MAX_SONAR_cntxt_s *devCntxt_p = MAX_SONAR_getDevCntxt(devId);

   /*
    * When the *BW is open or held low, the TX output delivers asynchronous
    * serial with an RS232 format, except voltages are 0-Vcc. The output is
    * an ASCII capital “R”, followed by three ASCII character digits
    * representing the range in inches up to a maximum of 255, followed by
    * a carriage return (ASCII 13). The baud rate is 9600, 8 bits, no parity,
    * with one stop bit.
    */

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   if (devCntxt_p->rangingInitiated || devCntxt_p->opRcvd == 0)
       return PLTFRM_STS_DEV_NOT_READY;

   if (devCntxt_p->opBuffIdx != MAX_SONAR_SERIAL_OP_LEN)
   {
       SYS_fatal(SYS_FATAL_ERR_551);
       return PLTFRM_STS_DEV_DRIVER_INTERNAL_ERROR;
   }

   if (devCntxt_p->opBuff[0] == 'R'
       && devCntxt_p->opBuff[MAX_SONAR_SERIAL_OP_LEN - 1] == 0xd)
   {
       UINT8_t *r_p = devCntxt_p->opBuff + 1;
       UINT16_t idx, range = 0;

       for (idx=0; idx < MAX_SONAR_RANGE_DIGIT_CNT; idx++)
       {
            if (r_p[idx] >= '0' && r_p[idx] <= '9')
            {
                UINT16_t temp, f;

                temp = r_p[idx] - '0';
                f = (idx == 0 ? 100 : (idx == 1) ? 10 : 1);

                temp *= f;
                range += temp;
            }
            else
                break;
       }

       if (idx >= MAX_SONAR_RANGE_DIGIT_CNT)
           *rangeInInches = range;
       else
          sts = PLTFRM_STS_DEV_OP_ERR;
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
PLTFRM_sts_t MAX_SONAR_startRanging(const UINT8_t devId, UINT16_t *tmoMilliSecs_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   MAX_SONAR_cntxt_s *devCntxt_p = MAX_SONAR_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   if (devCntxt_p->initDone == 0)
       return PLTFRM_STS_DEV_NOT_INITIALIZED;

   if (devCntxt_p->rangingInitiated)
       return PLTFRM_STS_DEV_NOT_READY;

   devCntxt_p->rangingInitiated = 1;
   devCntxt_p->opRcvd = 0;
   devCntxt_p->opBuffIdx = 0;

   // Bring RX high for 20 microsecs or more to initiate ranging.
   sts = PLTFRM_setGPIOPinOutput(devCntxt_p->port, devCntxt_p->pin, 0x1);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   PLTFRM_delay100MicroSecs();

   sts = PLTFRM_setGPIOPinOutput(devCntxt_p->port, devCntxt_p->pin, 0x0);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   *tmoMilliSecs_p = MAX_SONAR_RANGING_TIME_MILLISECS;

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
PLTFRM_sts_t MAX_SONAR_UART_start(UINT8_t devId)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   MAX_SONAR_cntxt_s *devCntxt_p = MAX_SONAR_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   if (devCntxt_p->initDone)
   {
       SYS_fatal(SYS_FATAL_ERR_553);
       return PLTFRM_STS_DEV_ALREADY_INITIALIZED;
   }

   /*
    * USCI_Ax - (UART/SPI/IrDA/LIN)
    * USCI_Bx - (SPI/I2C)
    */

   IFG2 &= ~(UCA0RXIFG);
#ifdef UART_TX_INT_ENA
   IFG2 &= ~(UCA0TXIFG);
#endif

   IE2 |= UCA0RXIE;  // Enable USCI_A0 RX interrupt

#ifdef UART_TX_INT_ENA
   IE2 |= UCA0TXIE;  // Enable USCI_A0 RX interrupt
#endif

   devCntxt_p->initDone = 1;

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
PLTFRM_sts_t MAX_SONAR_init(const UINT8_t devId)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   MAX_SONAR_cntxt_s *devCntxt_p = MAX_SONAR_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   if (devCntxt_p->initDone)
   {
       SYS_fatal(SYS_FATAL_ERR_550);
       return PLTFRM_STS_DEV_ALREADY_INITIALIZED;
   }

   /*
    * Pin 4-RX– This pin is internally pulled high. The LV-MaxSonar-EZ will
    * continually measure range and output if RX data is left unconnected
    * or held high. If held low the sensor will stop ranging. Bring high for
    * 20 microsecs or more to command a range reading.
    */
   sts = PLTFRM_allocGPIOPin(PLTFRM_GENERIC_DEV_ID,
                             devCntxt_p->port,
                             devCntxt_p->pin,
                             PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                             PLTFRM_GPIO_OUTPUT_LOW,
                             PLTFRM_GPIO_INT_EDGE_NA,
                             NULL);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   sts = __UART_init();
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   return sts;
}

#endif
