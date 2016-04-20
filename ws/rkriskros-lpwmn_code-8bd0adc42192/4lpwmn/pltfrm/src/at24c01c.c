/*
 * File Name: at24c01c.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Sep/22/2013
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


#ifdef PLTFRM_AT24C01C_ENA
   
#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <at24c10c.h>


/*
 * Atmel AT24C01C, 1K Serial EEPROM: 
 * Internally organized with 16 pages of eight bytes each, the 1K requires a 7-bit 
 * data word address for random word addressing.
 *
 * Device address: msb                     lsb  
 *                 1  0  1  0  a2  a1  a0  r/w    
 */
 
volatile UINT8_t AT24C01C_wrCycleTmo = 0;


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */ 
UINT8_t AT24C01C_read(UINT8_t devId,
                      UINT8_t rdOff,
                      UINT8_t rdCnt,
                      UINT8_t *buff_p)
{
   UINT8_t idx, rc = 1, i2cAddr = AT24C10C_I2C_ADDR_PREFIX, busId;
   UINT16_t endOff;

   endOff = rdOff;
   endOff += rdCnt;
   if (endOff >= AT24C01C_EEPROM_LEN)
       return 0;
   
   /*
    * <Sequential read>
    * Sequential Reads are initiated by either a Current Address Read or a Random 
    * Address Read. After the micro-controller receives a data word, it responds
    * with an acknowledge. As long as the EEPROM receives an  acknowledge, it will 
    * continue to increment the data word address and serially clock out sequential 
    * data words. When the memory address limit is reached, the data word address 
    * will roll-over and the Sequential Read will continue. The Sequential Read 
    * operation is terminated when the microcontroller does not respond with a zero 
    * but does generate a following stop condition    
    *
    *   <Start(1)>  
    *   <Dev Addr(7) | W(1)>  <S-ACK(1)> 
    *   <Word Addr(8)>  <S-ACK(1)>
    *   <Start(1)>  
    *   <Dev Addr(7) | R(1)>  <S-ACK(1)> 
    *   <Data(8)>  <M-ACK(1)>       // First data byte from AT24C01C
    *   <Data(8)>  <M-ACK(1)>       // Second data byte from AT24C01C
    *   ..........
    *   ..........
    *   <Data(8)>  <M-NO ACK(1)>    // Last data byte from AT24C01C
    *   <Stop(1)>
    */
  
   switch (devId)
   {
      case PLTFRM_AT24C01C_1_DEV_ID: 
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           i2cAddr |= PLTFRM_AT24C01C_1_I2C_ADDR;
           break;
           
      default:
           rc = 0;
           break;
   } 
   
   if (rc == 0)
       return rc;
  
   I2C_SW_start(busId);
   
   I2C_SW_sendDevAddr_7(busId, i2cAddr, I2C_OPN_TYPE_WRITE);
   
   I2C_SW_wrByte(busId, rdOff);
   
   I2C_SW_start(busId);
   
   I2C_SW_sendDevAddr_7(busId, i2cAddr, I2C_OPN_TYPE_READ);
   
   for (idx=0; idx<rdCnt; idx++)
   {
      I2C_SW_rdByte(busId,
                    buff_p,
                    idx < (rdCnt - 1) ? I2C_ACK : I2C_NO_ACK);
      buff_p ++;
   }
   
   I2C_SW_stop(busId);
   
   return rc;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */ 
void AT24C01C_writeCycleTmoHndlr(void)
{
   AT24C01C_wrCycleTmo = 1;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */ 
UINT8_t AT24C01C_write(UINT8_t devId,
                       UINT8_t wrOff,
                       UINT8_t wrCnt,
                       UINT8_t *buff_p)
{
   UINT8_t idx, rc = 1, i2cAddr = AT24C10C_I2C_ADDR_PREFIX, busId;
   UINT16_t endOff;

   endOff = wrOff;
   endOff += wrCnt;
   if (endOff >= AT24C01C_EEPROM_LEN)
       return 0;
   
   /*
    * <Byte Write> 
    * A Byte Write operation requires an 8-bit data word address following the 
    * device address word and acknowledgment. Upon receipt of this address, the 
    * EEPROM will again respond with a zero and then clock in the first 8-bit data 
    * word. Following receipt of the 8-bit data word, the EEPROM will output a 
    * zero and the addressing device, such as a micro-controller, must terminate
    * the write sequence with a Stop condition. At this time, the EEPROM enters an 
    * internally timed write cycle, tWR, to the non volatile memory. All inputs are
    * disabled during this write cycle and the  EEPROM will not respond until the 
    * write is complete.
    *
    *   <Start(1)>  
    *   <Dev Addr(7) | W(1)>  <S-ACK(1)> 
    *   <Word Addr(8)>  <S-ACK(1)>
    *   <Data(8)>  <S-ACK(1)>   
    *   <Stop(1)>    
    *
    * tWR - The write cycle time tWR is the time from a valid Stop condition of a 
    * write sequence to the end of the internal clear/write cycle. tWR is typically 
    * 5 milliseconds.
    */
  
   switch (devId)
   {
      case PLTFRM_AT24C01C_1_DEV_ID:
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           i2cAddr |= PLTFRM_AT24C01C_1_I2C_ADDR;
           break;
           
      default:
           rc = 0;
           break;
   } 
   
   if (rc == 0)
       return rc;
   
   for (idx=0; idx<wrCnt; idx++)
   {
      UINT8_t byte;
      
      I2C_SW_start(busId);
  
      I2C_SW_sendDevAddr_7(busId, i2cAddr, I2C_OPN_TYPE_WRITE);
   
      I2C_SW_wrByte(busId, wrOff);
   
      I2C_SW_wrByte(busId, *buff_p);
      
      I2C_SW_stop(busId);
      
      /*
       * Acknowledge Polling: Once the internally timed write cycle has started 
       * and the EEPROM inputs are disabled, acknowledge polling can be initiated. 
       * This involves sending a Start condition followed by the device address 
       * word. The read/write bit is representative of the operation desired. Only 
       * if the internal write cycle has completed will the EEPROM respond with a 
       * zero allowing the read or write sequence to continue.
       */
      
       msp430g2553_startTimerA1(0, AT24C01C_WRITE_CYCLE_TMO_MSECS, 
                                AT24C01C_writeCycleTmoHndlr);
       
       while ((SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_TIMER_A1_EXPIRY) == 0x0);
       SYS_globalEvtMsk0 &= ~SYS_GLOBAL_EVT_TIMER_A1_EXPIRY;
       
       rc = AT24C01C_read(busId, wrOff, 0x1, &byte);
       if ((rc == 0) || (byte != *buff_p))
       {
           rc = 0;
           break;
       }
       
       buff_p ++;
       wrOff ++;
   }
   
   return rc;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t AT24C01C_init(UINT8_t devId)
{
   UINT8_t busId, rc = 1;
   
   switch (devId)
   {
      case PLTFRM_AT24C01C_1_DEV_ID:
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           break;
           
      default:
           rc = 0;
           break;
   }   
   
   (void)busId;

   return rc;
}

#endif
