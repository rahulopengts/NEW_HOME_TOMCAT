/*
 * File Name: cc3100.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: June/16/2015
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


#ifdef RADIO_CC3100

#include <pltfrm.h>
#include <cc3100.h>

CC3100_intHndlr_t CC3100_simpleLinkIntCbFn = NULL;

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void CC3100_disable(void)
{
   // Enter hibernation mode
   PLTFRM_setGPIOPinOutput(PLTFRM_GPIO_PORT_2,
                           PLTFRM_GPIO_PORT_PIN_1,
                           0);

   {
      UINT16_t idx;
      // Table 4-4 : Minimum time in hibernation is 10 milli-secs.
      for (idx=0; idx<50; idx++)
           PLTFRM_delay1MilliSec();
   }
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void CC3100_enable(void)
{
   // Exit hibernation mode
   PLTFRM_setGPIOPinOutput(PLTFRM_GPIO_PORT_2,
                           PLTFRM_GPIO_PORT_PIN_1,
                           1);
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void CC3100_registerInterruptHandler(CC3100_intHndlr_t intHndlrFn, void *val_p)
{
   CC3100_simpleLinkIntCbFn = intHndlrFn;
   PLTFRM_gpioPinIntCtrlOpn(PLTFRM_GPIO_PORT_2,
                            PLTFRM_GPIO_PORT_PIN_0,
                            PLTFRM_ENABLE_GPIO_PIN_INT, 1);
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void CC3100_intCbFn(UINT8_t devId)
{
   if (CC3100_simpleLinkIntCbFn != NULL)
   {
       (*(CC3100_simpleLinkIntCbFn))(NULL);
   }
}


/*
 * Ensure that nCS (active-low signa)l is asserted 10 ns before the clock is
 * toggled. nCS can be deasserted 10 ns after the clock edge
 */

/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
#define CC3100_spiEnd( )  \
do \
{  \
   PLTFRM_delay1MicroSecs(); \
   P4OUT |= (BIT0);  \
} while (0)


/*
 ********************************************************************
 *
 *
 *
 *
 *
 ********************************************************************
 */
#define CC3100_spiBegin( ) \
do \
{  \
   P4OUT &= ~(BIT0);  \
   PLTFRM_delay1MicroSecs(); \
} while (0)


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t CC3100_spiTxRx(UINT8_t val)
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
 *
 ********************************************************************
 */
SINT32_t CC3100_spiWrite(UINT8_t *buff_p, SINT16_t len)
{
   SINT16_t idx;

   CC3100_spiBegin();

   for (idx=0; idx<len; idx++)
   {
        CC3100_spiTxRx(buff_p[idx]);
        PLTFRM_delay1MicroSecs();
   }

   CC3100_spiEnd();

   return len;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
SINT32_t CC3100_spiRead(UINT8_t *buff_p, SINT16_t len)
{
   SINT16_t idx;

   CC3100_spiBegin();

   for (idx=0; idx<len; idx++)
   {
        buff_p[idx] = CC3100_spiTxRx(0x0);
        PLTFRM_delay1MicroSecs();
   }

   CC3100_spiEnd();

   return len;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t CC3100_spiInit(void)
{
   PLTFRM_sts_t sts;

   // CC3100 CSn - default HIGH

   sts = PLTFRM_allocGPIOPin(PLTFRM_CC1101_1_DEV_ID,
                             PLTFRM_GPIO_PORT_4,
                             PLTFRM_GPIO_PORT_PIN_0,
                             PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                             PLTFRM_GPIO_OUTPUT_HIGH,
                             PLTFRM_GPIO_INT_EDGE_NA,
                             NULL);
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
PLTFRM_sts_t CC3100_init(void)
{
   PLTFRM_sts_t sts;

   sts = PLTFRM_allocGPIOPin(PLTFRM_CC3100_1_DEV_ID,
                             PLTFRM_GPIO_PORT_2,
                             PLTFRM_GPIO_PORT_PIN_0,
                             PLTFRM_GPIO_PIN_DIRECTION_INPUT,
                             PLTFRM_GPIO_OUTPUT_NA,
                             PLTFRM_GPIO_INT_EDGE_POSITIVE,
                             CC3100_intCbFn);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   // Control the active low nHIB pin
   // Active-low signal that commands the CC3100 device to enter
   // hibernate mode (lowest power state)

   sts = PLTFRM_allocGPIOPin(PLTFRM_CC3100_1_DEV_ID,
                             PLTFRM_GPIO_PORT_2,
                             PLTFRM_GPIO_PORT_PIN_1,
                             PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                             PLTFRM_GPIO_OUTPUT_HIGH,
                             PLTFRM_GPIO_INT_EDGE_NA,
                             NULL);

   return sts;
}

#endif
