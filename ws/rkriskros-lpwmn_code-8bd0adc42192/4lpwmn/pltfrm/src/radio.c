/*
 * File Name: radio.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Sep/1/2013
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
#include <radio.h>


RADIO_state_t RADIO_state = RADIO_STATE_IDLE;


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RADIO_transferPktToTxFifo(UINT8_t bdHndl)
{
#ifdef RADIO_CC2520
#ifdef DEV_TYPE_RFD
   if (RADIO_state == RADIO_STATE_LPM)
   {
       RADIO_exitLPM();
   }
#endif
   cc2520_transferPktToTxFifo(bdHndl);
#endif
}



#if 0
/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void RADIO_transferPktBuffToTxFifo(UINT8_t *pktBuff_p, UINT16_t pktLen)
{
#ifdef RADIO_CC2520
#ifdef DEV_TYPE_RFD
   if (RADIO_state == RADIO_STATE_LPM)
   {
       RADIO_exitLPM();
   }
#endif
   cc2520_transferPktBuffToTxFifo(bdHndl);
#endif
}
#endif

