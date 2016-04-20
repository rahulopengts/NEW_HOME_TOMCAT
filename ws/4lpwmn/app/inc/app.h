/*
 * File Name: app.h
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Aug/21/2013
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

#ifndef __APP_H__
#define __APP_H__

#include <typedefs.h>
   
   
#define APP_UDP_SRC_PORT_NR   0xf0b5
#define APP_UDP_DEST_PORT_NR  0xf0b6   

#define APP_TX_BUFF_LEN  32
#define APP_UDP_PYLD_LEN  5

extern UINT8_t APP_pyldBuffer[]; 
extern UINT8_t APP_pyldBuffOff;
   
extern void APP_init(void);

extern void APP_evtHndlr(UINT16_t globalEvtId, void *params_p);

extern void APP_nodeJoinDoneInd(void);

extern void APP_pktTxProc(void);

extern void APP_reportSensorVal(void);

extern void NC_dataReqProc(void);

extern void FFD_APP_poll(void);

extern void FFD_APP_periodicTmrExpCbFunc(void);

extern void APP_procGPIOEvt(void);

extern void FFD_APP_procPendDataReq(void);

#endif
