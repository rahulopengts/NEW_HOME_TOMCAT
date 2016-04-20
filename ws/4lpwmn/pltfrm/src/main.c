/*
 * File Name: main.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Aug/2/2013
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

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <mac_pib.h>

/*
 * -----------------------------------------------------------------------------
 * Interrupt vectors (0xFFFF to 0xFFC0)
 * -----------------------------------------------------------------------------
 * 0xfffe           Power Up/Reset/WD+/PC OOR
 * 0xfffc   Int14   NMI/Osc fault/Flash mem access err  (NMI_VECTOR)          N
 * 0xfffa   Int13   Timer0_B3                           (TIMERB0_VECTOR)      N
 * 0xfff8   Int12   Timer0_B3                           (TIMERB1_VECTOR)      N
 * 0xfff6   Int11   Comp_A+                             (COMPARATORA_VECTOR)  N
 * 0xfff4   Int10   Watchdog                            (WDT_VECTOR)          Y
 * 0xfff2   Int09   Timer0_A3                           (TIMER0_A0_VECTOR)    Y
 * 0xfff0   Int08   Timer0_A3                           (TIMER0_A1_VECTOR)    N
 * 0xffee   Int07   USCI_A0/USCI_B0 rx                  (USCIAB0RX_VECTOR)    Y
 * 0xffec   Int06   USCI_A0/USCI_B0 tx                  (USCIAB0TX_VECTOR)   Y/N
 * 0xffea   Int05   ADC10                               (ADC10_VECTOR)        N
 * 0xffe8   Int04   Reserved
 * 0xffe6   Int03   I/O Port P2                         (PORT2_VECTOR)        Y
 * 0xffe4   Int02   I/O Port P1                         (PORT1_VECTOR)        Y
 * 0xffe2   Int01   Timer1_A3                           (TIMER1_A0_VECTOR)    Y
 * 0xffe0   Int00   Timer1_A3                           (TIMER1_A1_VECTOR)    N
 * ------------------------------------------------------------------------------
 */

#if 0
UINT16_t PLTFRM_unExpIntCnt = 0;

// trap isr assignation - put all unused ISR vector here
#pragma vector=ADC10_VECTOR, NMI_VECTOR, COMPARATORA_VECTOR,
__interrupt void TrapIsr(void)
{
   // this is a trap ISR - check for the interrupt cause here by
   // checking the interrupt flags, if necessary also clear the interrupt
   // flag
   PLTFRM_unExpIntCnt ++;
}
#endif


int main( void )
{
  PLTFRM_init();  // in pltfrm.c.
    
  SYS_loop();  // in system.c
  
  return 0; 
}  
