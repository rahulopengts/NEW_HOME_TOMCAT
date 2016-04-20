/*
 * File Name: at24mac602.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Nov/9/2013
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


#ifdef PLTFRM_AT24MAC602_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <at24mac602.h>


AT24MAC602_cntxt_s AT24MAC602_cntxtList[PLTFRM_AT24MAC602_DEV_CNT] =
{
   {PLTFRM_AT24MAC602_1_DEV_ID, 0x0},
};


/*
 * Datasheet : 
 * http://www.atmel.com/images/atmel-8807-seeprom-at24mac402-602-datasheet.pdf
 */

/*
 * The Atmel AT24MAC402 and AT24MAC602 provides 2048 bits of Serial Electrically-
 * Erasable Programmable Read-Only Memory (EEPROM) organized as 256 words of eight
 * bits each and is accessed via an I2C-compatible (2-wire) serial interface. In
 * addition, AT24MAC402/602 incorporates an easy and inexpensive method to obtain
 * a globally unique MAC or EUI address (EUI-48 or EUI-64). The AT24MAC602 is an
 * EUI-64 compatible device that contains a 64-bit EUI address.
 */

/*
 * The 2-Kbit memory array is internally organized as 16 pages of 16 bytes of EEPROM
 * each. Random word addressing requires a 8-bit data word address.
 */

volatile UINT8_t AT24MAC602_wrCycleTmo = 0;

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
AT24MAC602_cntxt_s *AT24MAC602_getDevCntxt(UINT8_t devId)
{
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_AT24MAC602_DEV_CNT; idx++)
   {
      if (AT24MAC602_cntxtList[idx].devId == devId)
          return (&(AT24MAC602_cntxtList[idx]));
   }

   return NULL;
}


/*
 ***********************************************************************
 *
 *
 *
 ***********************************************************************
 */
PLTFRM_sts_t AT24MAC602_read(UINT8_t busId,
                             UINT8_t i2cAddr,
                             UINT8_t rdOff,
                             UINT8_t rdCnt,
                             UINT8_t *buff_p)
{
   UINT8_t idx;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   /*
    * <Sequential read>
    * Sequential Reads are initiated by either a Current Address Read or a Random
    * Address Read. After the micro-controller receives a data word, it responds
    * with an acknowledge. As long as the EEPROM receives an  acknowledge, it will
    * continue to increment the data word address and serially clock out sequential
    * data words. When the memory address limit is reached, the data word address
    * will roll-over and the Sequential Read will continue. The Sequential Read
    * operation is terminated when the micro-controller does not respond with a zero
    * but does generate a following stop condition in the subsequent clock cycle.
    *
    *   <Start(1)>
    *   <Dev Address(7) | W(1)>  <S-ACK(1)>
    *   <Word Address(8)>  <S-ACK(1)>
    *   <Start(1)>
    *   <Dev Address(7) | R(1)>  <S-ACK(1)>
    *   <Data(8)>  <M-ACK(1)>       // First data byte from AT24MAC602
    *   <Data(8)>  <M-ACK(1)>       // Second data byte from AT24MAC602
    *   ..........
    *   ..........
    *   <Data(8)>  <M-NO ACK(1)>    // Last data byte from AT24MAC602
    *   <Stop(1)>
    */

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
void AT24MAC602_writeCycleTmoHndlr(void)
{
   AT24MAC602_wrCycleTmo = 1;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t AT24MAC602_write(UINT8_t devId,
                              UINT8_t wrOff,
                              UINT8_t wrCnt,
                              UINT8_t *buff_p)
{
   UINT8_t idx, i2cAddr = AT24MAC602_I2C_ADDR_PREFIX_1, busId;
   UINT16_t endOff;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   AT24MAC602_cntxt_s *devCntxt_p = AT24MAC602_getDevCntxt(devId);
   
   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   if (devCntxt_p->initDone == 0x0)
       return PLTFRM_STS_DEV_UNINITIALIZED;

   endOff = wrOff;
   endOff += wrCnt;
   if (endOff >= AT24MAC602_EEPROM_LEN)
       return PLTFRM_STS_INV_PARAM;

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
      case PLTFRM_AT24MAC602_1_DEV_ID:
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           i2cAddr |= PLTFRM_AT24MAC602_1_I2C_ADDR;
           break;

      default:
           sts = PLTFRM_STS_INV_DEV_ID;
           break;
   }

   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

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
#if 0
       msp430g2553_startTimerA1(0, AT24MAC602_WRITE_CYCLE_TMO_MSECS,
                                AT24MAC602_writeCycleTmoHndlr);

       while ((SYS_globalEvtMsk0 & SYS_GLOBAL_EVT_TIMER_A1_EXPIRY) == 0x0)
       {
#ifdef DEV_TYPE_RFD
          PLTFRM_enterLPM( );
#endif
       }

       SYS_globalEvtMsk0 &= ~SYS_GLOBAL_EVT_TIMER_A1_EXPIRY;
#else
       {
          UINT16_t idx;
          for (idx=0; idx<AT24MAC602_WRITE_CYCLE_TMO_MSECS; idx++)
               PLTFRM_delay1MilliSec();
       }
#endif
       sts = AT24MAC602_read(busId, i2cAddr, wrOff, 0x1, &byte);
       if ((sts != PLTFRM_STS_SUCCESS) || (byte != *buff_p))
       {
           break;
       }

       buff_p ++;
       wrOff ++;
   }

   return sts;
}


/*
 *********************************************************************************
 * < EUI-64 Support >
 * For applications that utilize an EUI-64 standard, the EUI-64 address is stored
 * in the last eight bytes of the AT24MAC602�s extended memory block. Similar to
 * EUI-48, the EUI-64 standard consists of the same three byte OUI coupled with a
 * five byte extension identifier (see Table 3-2). Atmel generates this unique 40-
 * bit data value coupled with the OUI to guarantee a globally unique 64-bit EUI
 * value and requires no additional data manipulation like other solutions where
 * the application must manually insert a two byte FFFEh value in between the OUI
 * and Extension Identifier.
 *
 * -------------------------------------------------------------------------------
 * Address        :   98h   99h   9Ah   9Bh      9Ch      9Dh      9Eh      9Fh
 * -------------------------------------------------------------------------------
 * EUI data value :   FCh   C2h   3Dh   byte 1   byte 2   byte 3   byte 4   byte 5
 * -------------------------------------------------------------------------------
 *
 * Where the first 3 bytes comprise the 24 bit OUI and the remaining five bytes
 * comprise the 40 bit extension identifier.
 *********************************************************************************
 */
PLTFRM_sts_t AT24MAC602_readEUI64(UINT8_t devId,
                                  UINT8_t *buff_p)
{
   UINT8_t i2cAddr = AT24MAC602_I2C_ADDR_PREFIX_3, busId;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   AT24MAC602_cntxt_s *devCntxt_p = AT24MAC602_getDevCntxt(devId);
   
   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   if (devCntxt_p->initDone == 0x0)
       return PLTFRM_STS_DEV_UNINITIALIZED;

   switch (devId)
   {
      case PLTFRM_AT24MAC602_1_DEV_ID:
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           i2cAddr |= PLTFRM_AT24MAC602_1_I2C_ADDR;
           break;

      default:
           sts = PLTFRM_STS_INV_DEV_ID;
           break;
   }

   if (sts == PLTFRM_STS_SUCCESS)
   {
       sts = AT24MAC602_read(busId, i2cAddr,
                             AT24MAC602_EUI64_OFF,
                             AT24MAC602_EUI64_LEN,
                            buff_p);
   }

   return sts;
}


/*
 ***********************************************************************
 *
 *
 *
 ***********************************************************************
 */
PLTFRM_sts_t AT24MAC602_readMem(UINT8_t devId,
                                UINT8_t rdOff,
                                UINT8_t rdCnt,
                                UINT8_t *buff_p)
{
   UINT8_t i2cAddr = AT24MAC602_I2C_ADDR_PREFIX_1, busId;
   UINT16_t endOff;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   AT24MAC602_cntxt_s *devCntxt_p = AT24MAC602_getDevCntxt(devId);
   
   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   if (devCntxt_p->initDone == 0x0)
       return PLTFRM_STS_DEV_UNINITIALIZED;

   endOff = rdOff;
   endOff += rdCnt;
   if (endOff >= AT24MAC602_EEPROM_LEN)
       return PLTFRM_STS_INV_PARAM;

   switch (devId)
   {
      case PLTFRM_AT24MAC602_1_DEV_ID:
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           i2cAddr |= PLTFRM_AT24MAC602_1_I2C_ADDR;
           break;

      default:
    	   sts = PLTFRM_STS_INV_DEV_ID;
           break;
   }

   if (sts == PLTFRM_STS_SUCCESS)
   {
       sts = AT24MAC602_read(busId, i2cAddr,
                             rdOff, rdCnt, buff_p);
   }

   return sts;
}


/*
 *****************************************************************************
 * Serial Number Access: The AT24MAC402/602 incorporates an extended memory
 * block containing a factory-programmed 128-bit serial number. Access to
 * this memory location is obtained by beginning the device address word with
 * a "1011"(Bh) sequence. The behavior of the next three bits (A2, A1, and A0)
 * remain the same as during a standard memory addressing sequence.
 *****************************************************************************
 */
PLTFRM_sts_t AT24MAC602_readSerialNr(UINT8_t devId,
                                     UINT8_t *buff_p)
{
   UINT8_t i2cAddr = AT24MAC602_I2C_ADDR_PREFIX_3, busId;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   AT24MAC602_cntxt_s *devCntxt_p = AT24MAC602_getDevCntxt(devId);
   
   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   if (devCntxt_p->initDone == 0x0)
       return PLTFRM_STS_DEV_UNINITIALIZED;

   switch (devId)
   {
      case PLTFRM_AT24MAC602_1_DEV_ID:
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           i2cAddr |= PLTFRM_AT24MAC602_1_I2C_ADDR;
           break;

      default:
    	   sts = PLTFRM_STS_INV_DEV_ID;
           break;
   }

   if (sts == PLTFRM_STS_SUCCESS)
   {
       sts = AT24MAC602_read(busId, i2cAddr,
                             AT24MAC602_SERIAL_NR_OFF,
                             AT24MAC602_SERIAL_NR_LEN,
                             buff_p);
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
PLTFRM_sts_t AT24MAC602_init(const UINT8_t devId)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   AT24MAC602_cntxt_s *devCntxt_p = AT24MAC602_getDevCntxt(devId);
   UINT8_t i2cAddr, busId;
   
   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   switch (devId)
   {
      case PLTFRM_AT24MAC602_1_DEV_ID:
           busId = PLTFRM_I2C_SW_BUS_1_ID;
           i2cAddr = AT24MAC602_I2C_ADDR_PREFIX_1 | PLTFRM_AT24MAC602_1_I2C_ADDR;
           break;

      default:
           SYS_fatal(SYS_FATAL_ERR_533);
           break;
   }

   sts = PLTFRM_regI2CDev(devId, busId, i2cAddr);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   devCntxt_p->initDone = 1;

   return sts;
}

#endif
