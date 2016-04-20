/*
 * File Name: flash.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Aug/28/2015
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
#ifdef UC_FU_ENA

#include <string.h>
#include <typedefs.h>
#include <util.h>
#include <pltfrm.h>
#include <sys_fatalErrs.h>
#include <system.h>
#include <fu.h>
#include <m24m01.h>

#define M24M01_CHIP_ENA_ADDR  0x3


#pragma RETAIN(FU_delay1MilliSec)
#pragma RETAIN(FU_delay10MicroSecs)
#pragma RETAIN(__FU_M24M01_write)
#pragma RETAIN(FU_M24M01_write)
#pragma RETAIN(__FU_M24M01_read)
#pragma RETAIN(FU_M24M01_read)


#pragma CODE_SECTION(FU_delay500MSecs, ".flashUpdate")
#pragma CODE_SECTION(__FU_fatal, ".flashUpdate")
#pragma CODE_SECTION(FU_delay1MilliSec, ".flashUpdate")
#pragma CODE_SECTION(FU_delay10MicroSecs, ".flashUpdate")
#pragma CODE_SECTION(__FU_M24M01_write, ".flashUpdate")
#pragma CODE_SECTION(FU_M24M01_write, ".flashUpdate")
#pragma CODE_SECTION(__FU_M24M01_read, ".flashUpdate")
#pragma CODE_SECTION(FU_M24M01_read, ".flashUpdate")

#pragma RETAIN(FU_delay500MSecs)
#pragma CODE_SECTION(FU_I2C_SW_start, ".flashUpdate")
#pragma CODE_SECTION(FU_I2C_SW_stop, ".flashUpdate")
#pragma CODE_SECTION(FU_I2C_SW_rdByte, ".flashUpdate")
#pragma CODE_SECTION(FU_I2C_SW_wrByte, ".flashUpdate")
#pragma CODE_SECTION(FU_I2C_SW_sendDevAddr_7, ".flashUpdate")

#pragma RETAIN(__FU_fatal)
#pragma RETAIN(FU_delay1MilliSec)
#pragma RETAIN(FU_delay10MicroSecs)
#pragma RETAIN(FU_I2C_SW_start)
#pragma RETAIN(FU_I2C_SW_stop)
#pragma RETAIN(FU_I2C_SW_rdByte)
#pragma RETAIN(FU_I2C_SW_wrByte)
#pragma RETAIN(FU_I2C_SW_sendDevAddr_7)

#pragma RETAIN(FU_I2C_SW_start)
#pragma RETAIN(FU_I2C_SW_stop)
#pragma RETAIN(FU_I2C_SW_rdByte)
#pragma RETAIN(FU_I2C_SW_wrByte)
#pragma RETAIN(FU_I2C_SW_sendDevAddr_7)



/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void FU_delay10MicroSecs(void)
{
   UINT16_t idx;
   for (idx=0; idx<16; idx++);
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void FU_delay1MilliSec(void)
{
   UINT16_t idx;
   for (idx=0; idx<1600; idx++);
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void FU_delay500MSecs(void)
{
   UINT16_t idx;
   for (idx=0; idx<500; idx++)
        FU_delay1MilliSec();
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void __FU_fatal(void)
{
   PLTFRM_OFF_LED_1();
   PLTFRM_OFF_LED_2();

   for (;;)
   {
      FU_delay500MSecs();
      PLTFRM_TOGGLE_LED_1();
      PLTFRM_TOGGLE_LED_2();
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
UINT8_t FU_I2C_SW_start(UINT8_t busId)
{
   UINT8_t rc = 1;
   volatile UINT8_t *regDir_p;
   UINT8_t sclU8, sdaU8;

   switch (busId)
   {
      case PLTFRM_I2C_SW_BUS_1_ID:
#if defined(WISENSE_HW_REV_6)
           regDir_p = &P3DIR;
           sclU8 = BIT7;  // p3.7
           sdaU8 = BIT6;  // p3.6

#else
#error hw revision not specified !!
#endif
           break;

      default:
           rc = 0;
           break;
   }

   if (rc == 0)
       return rc;

   // Make sure both SCL and SDA are high at this point.

   (*regDir_p) |= (sclU8);  // Set SCL low

   FU_delay10MicroSecs();

   (*regDir_p) &= ~(sdaU8);  // Pull SDA high
   FU_delay10MicroSecs();

   (*regDir_p) &= ~(sclU8);  // Pull SCL high

   // Start Setup Time (4 microsecs minimum)
   FU_delay10MicroSecs();  // 10 microseconds

   (*regDir_p) |= (sdaU8);  // Set SDA LOW

   // Start hold time (Data low to clock low) (min value is 4 microsecs)
   FU_delay10MicroSecs();  // 10 microseconds

   (*regDir_p) |= (sclU8);  // Set SCL low

   // Low period of SCL clock (min 4.7 microsecs)
   FU_delay10MicroSecs();  // 10 microseconds @ 100 KHz

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
UINT8_t FU_I2C_SW_stop(UINT8_t busId)
{
   UINT8_t rc = 1;
   volatile UINT8_t *regDir_p;
   UINT8_t sclU8, sdaU8;

   switch (busId)
   {
      case PLTFRM_I2C_SW_BUS_1_ID:
#if defined(WISENSE_HW_REV_6)
           regDir_p = &P3DIR;
           sclU8 = BIT7;  // p3.7
           sdaU8 = BIT6;  // p3.6
#else
#error hw revision not specified !!
#endif
           break;

      default:
           rc = 0;
           break;
   }

   if (rc == 0)
       return rc;

   /*
    * <Stop condition>
    * Once all the data frames have been sent, the master will generate a stop condition.
    * Stop conditions are defined by a 0->1 (low to high) transition on SDA after a 0->1
    * transition on SCL, with SCL remaining high. During normal data writing operation,
    * the value on SDA should not change when SCL is high, to avoid false stop conditions.
    */

   (*regDir_p) |= (sclU8);  // Set SCL low

   FU_delay10MicroSecs();

   (*regDir_p) |= (sdaU8);   // Set SDA low

   // Data in setup time (minimum wait time is at most 250 nansecs @ 100 kHz)
   FU_delay10MicroSecs();  // 10 microseconds

   // Important to set the direction of boths pins IN so that leakage through
   // the resistors is avoided. If any pin is set to OUT and it's output is
   // set to low, it will provide a path for current from Vcc through the I2C
   // pull up resistors (3.3 v / 470000 -> 70 microamps)

   (*regDir_p) &= ~(sclU8);  // Pull SCL high

   // STOP setup time (minimum wait time is at most 4 microsecs @ 100 kHz)
   FU_delay10MicroSecs();  // 10 microseconds

   (*regDir_p) &= ~(sdaU8);  // Pull SDA high

   // Bus free time between a STOP and START condition
   // Minimum wait time is 4.7 microsecs @ 100 kHz
   FU_delay10MicroSecs();  // 10 microseconds

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
UINT8_t FU_I2C_SW_rdByte(UINT8_t busId,  UINT8_t *buff_p, UINT8_t ackCntrl)
{
   UINT8_t rc = 1, idx;
   volatile UINT8_t *regDir_p;
   volatile UINT8_t *regIn_p;
   UINT8_t sclU8, sdaU8;
   UINT8_t rxData = 0;

   switch (busId)
   {
       case PLTFRM_I2C_SW_BUS_1_ID:
#if defined(WISENSE_HW_REV_6)
           regDir_p = &P3DIR;
           regIn_p = &P3IN;
           sclU8 = BIT7;  // p3.7
           sdaU8 = BIT6;  // p3.6
#else
#error hw revision not specified !!
#endif
            break;

       default:
            rc = 0;
            break;
   }

   if (rc == 0)
       return rc;

   *(regDir_p) &= ~(sdaU8);  // Pull SDA high

   for (idx=0; idx<8; idx++)
   {
        UINT8_t regVal;

        FU_delay10MicroSecs();

        *(regDir_p) &= ~(sclU8);  // Pull SCL high

        FU_delay10MicroSecs();

        // Read the SDA line

        rxData += rxData;  // <<= 1;

        regVal = *(regIn_p);
        if (regVal & (sdaU8))
            rxData |= 0x1;

        FU_delay10MicroSecs();

        *(regDir_p) |= (sclU8);  // Set SCL low
   }

   *buff_p = rxData;

   FU_delay10MicroSecs();

   /*
    * The receiver must pull down the SDA line during the acknowledge
    * clock pulse so that it remains stable LOW during the HIGH period of
    * this clock pulse. Of course, set-up and hold times must also be taken
    * into account.
    */

   if (ackCntrl == I2C_ACK)
   {
       // Set SDA low ...
       *(regDir_p) |= (sdaU8);  // Set SDA low
   }

   FU_delay10MicroSecs();

   *(regDir_p) &= ~(sclU8);  // Pull SCL high

   FU_delay10MicroSecs();

   *(regDir_p) |= (sclU8);  // Set SCL low

   FU_delay10MicroSecs();

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
UINT8_t FU_I2C_SW_wrByte(UINT8_t busId, UINT8_t data)
{
   UINT8_t rc = 1, regVal, idx;
   volatile UINT8_t *regDir_p;
   volatile UINT8_t const *regIn_p;
   UINT8_t sdaU8, sclU8;

   switch (busId)
   {
       case PLTFRM_I2C_SW_BUS_1_ID:
#if defined(WISENSE_HW_REV_6)
           regDir_p = &P3DIR;
           regIn_p = &P3IN;
           sclU8 = BIT7;
           sdaU8 = BIT6;
#else
#error hw revision not specified !!
#endif
            break;

       default:
            rc = 0;
            break;
   }

   if (rc == 0)
       return rc;

   // Data is transferred with the most significant bit (MSB) first.

   for (idx=0; idx<8; idx++)
   {
       // Data is changed when SCL is low.
       // Assume SCL is low at this point

       if (data & 0x80)
           *(regDir_p) &= ~sdaU8;  // Pull SDA high
       else
           *(regDir_p) |= sdaU8;   // Set SDA low

       // Data setup time (minimum wait value is 250 nanosecs @ 100 kHz)
       FU_delay10MicroSecs();

       // Data should be stable during SCL high
       *(regDir_p) &= ~(sclU8);  // Pull SCL high

       // SCL pulse width high (minimum wait value is max 4 microseconds @ 100 kHz)
       FU_delay10MicroSecs();

       *(regDir_p) |= sclU8;   // Set SCL low

       // SDA hold time is 0
       FU_delay10MicroSecs();

       *(regDir_p) &= ~sdaU8;  // release SDA

       // SCL pulse width low (minimum value is max 2 microseconds)
       FU_delay10MicroSecs();

       data += data;  // <<= 1;
   }

   /*
    * The 9th bit of the frame is the NACK/ACK bit. This is the case for all frames
    * (data or address). Once the first 8 bits of the frame are sent, the receiving
    * device is given control over SDA. If the receiving device does not pull the SDA
    * line low before the 9th clock pulse, it can be inferred that the receiving device
    * either did not receive the data or did not know how to parse the message. In that
    * case, the exchange halts, and itï¿½s up to the master of the system to decide how
    * to proceed.
    */

   // Read the SDA line (we have already waited 10 microseconds after setting SCL low
   // and releasing SDA)
   regVal = *(regIn_p);

   if (regVal & (sdaU8))
   {
       rc = 0;
   }

   // Data should be stable during SCL high
   *(regDir_p) &= ~(sclU8);  // Pull SCL high

   // SCL pulse width high (minimum wait value is max 4 microseconds @ 100 kHz)
   FU_delay10MicroSecs(); // hack

   *(regDir_p) |= (sclU8);  // Set SCL low

   FU_delay10MicroSecs();

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
UINT8_t FU_I2C_SW_sendDevAddr_7(UINT8_t busId, UINT8_t i2cAddr, UINT8_t opnType)
{
   UINT8_t rc = 0, slaveAddrByte = i2cAddr;

   /*
    * 8.0 FORMATS WITH 7-BIT ADDRESSES
    * After the START condition (S), a slave address is sent. This address is 7 bits
    * long followed by an eighth bit which is a data direction bit (R/W) ï¿½ a ï¿½zeroï¿½
    * indicates a transmission (WRITE), a ï¿½oneï¿½ indicates a request for data (READ).
    */

   slaveAddrByte = (slaveAddrByte << 1) & 0xfe;
   slaveAddrByte |= (opnType == I2C_OPN_TYPE_READ) ? 0x1 : 0x0;

   rc = FU_I2C_SW_wrByte(busId, slaveAddrByte);

   return rc;
}


/*
 * <WC pin>
 * This input signal is useful for protecting the entire contents of the
 * memory from inadvertent write operations. Write operations are disabled
 * to the entire memory array when Write Control (WC) is driven high. Write
 * operations are enabled when Write Control (WC) is either driven low or
 * left floating.
 */

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t __FU_M24M01_write(UINT32_t addr,
                               UINT8_t byte)
{
  PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
  UINT8_t i2cAddr;
  UINT32_t addrByte;

  /*
   * After the device select code and the address bytes, the bus master
   * sends one data byte. If the addressed location is not Write-protected,
   * the device replies with Ack. The bus master terminates the transfer by
   * generating a Stop condition.
   *
   * The 128 Kbytes (1 Mb) are addressed with 17 address bits, the 16 lower
   * address bits being defined by the 2 addr bytes and the most significant
   * address bit (A16) being included in the Device Select code.
   */

  i2cAddr = M24M01_I2C_ADDR_PREFIX_1 | (M24M01_CHIP_ENA_ADDR << 1);
  i2cAddr |= ((addr & 0x10000) ? 0x1 : 0x0); // A16 bit is 1

  /*
   * > START                               (from micro)
   * > 7 bit device address | Write bit    (from micro)
   * > ACK                                 (from M24M01)
   * > address bits 8-15                   (from micro)
   * > ACK                                 (from M24M01)
   * > address bits 0-7                    (from micro)
   * > ACK                                 (from M24M01)
   * > Write byte                          (from micro)
   * > Ack bit                             (from M24M01)
   * > STOP                                (from micro)
   */

   FU_I2C_SW_start(PLTFRM_I2C_SW_BUS_1_ID);

   if (FU_I2C_SW_sendDevAddr_7(PLTFRM_I2C_SW_BUS_1_ID, i2cAddr, I2C_OPN_TYPE_WRITE) == 0x0)
   {
       sts = PLTFRM_STS_I2C_BUS_WR_ERROR;
       goto _end;
   }

   addrByte = addr;
   // addrByte /= 256;   // (addrByte >> 8) & 0xff;
   {
      UINT8_t idx;
      for (idx=0; idx<8; idx++)
           addrByte /= 2;
   }


   if (FU_I2C_SW_wrByte(PLTFRM_I2C_SW_BUS_1_ID, (UINT8_t)addrByte) == 0x0)
   {
       sts = PLTFRM_STS_I2C_BUS_WR_ERROR;
       goto _end;
   }

   addrByte = addr;
   addrByte &= 0xff;

   if (FU_I2C_SW_wrByte(PLTFRM_I2C_SW_BUS_1_ID, (UINT8_t)addrByte) == 0x0)
   {
       sts = PLTFRM_STS_I2C_BUS_WR_ERROR;
       goto _end;
   }

   if (FU_I2C_SW_wrByte(PLTFRM_I2C_SW_BUS_1_ID, byte) == 0x0)
       sts = PLTFRM_STS_I2C_BUS_WR_ERROR;

_end:
   FU_I2C_SW_stop(PLTFRM_I2C_SW_BUS_1_ID);

   // Byte write is within 5 milli-secs
   if (sts == PLTFRM_STS_SUCCESS)
   {
       UINT8_t idx;
       for (idx=0; idx<M24M01_BYTE_WRITE_DURATION_MILLISECS; idx++)
           FU_delay1MilliSec();
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
PLTFRM_sts_t FU_M24M01_write(UINT32_t addr,
                             UINT16_t len,
                             UINT8_t *buff_p)
{
   UINT16_t idx;
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   for (idx=0; idx<len; idx++)
   {
       sts = __FU_M24M01_write(addr + idx, buff_p[idx]);
       if (sts != PLTFRM_STS_SUCCESS)
           break;
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
PLTFRM_sts_t __FU_M24M01_read(UINT32_t addr,
                              UINT16_t cnt,
                              UINT8_t *byte_p)
{
  PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
  UINT8_t i2cAddr;
  UINT32_t addrByte;

  /*
   * <Random Address Read>
   *
   * A dummy Write is first performed to load the address into this address
   * counter but without sending a Stop condition. Then, the bus master sends
   * another Start condition, and repeats the device select code, with the RW
   * bit set to 1. The device acknowledges this, and outputs the contents of
   * the addressed byte. The bus master must not acknowledge the byte, and
   * terminates the transfer with a Stop condition.
   *
   *
   * The 128 Kbytes (1 Mb) are addressed with 17 address bits, the 16 lower
   * address bits being defined by the 2 addr bytes and the most significant
   * address bit (A16) being included in the Device Select code.
   */

   i2cAddr = M24M01_I2C_ADDR_PREFIX_1 | (M24M01_CHIP_ENA_ADDR << 1);
   i2cAddr |= ((addr & 0x10000) ? 0x1 : 0x0); // A16 bit is 1

   /*
    * > START                               (from micro)
    * > 7 bit device address | Write bit    (from micro)
    * > ACK                                 (from M24M01)
    * > address bits 8-15                   (from micro)
    * > ACK                                 (from M24M01)
    * > address bits 0-7                    (from micro)
    * > ACK                                 (from M24M01)
    * > RE-START                            (from micro)
    * > 7 bit device address | Read bit     (from micro)
    * > Read byte                           (from M24M01)
    * > No Ack bit (SDA remains high)
    * > STOP                                (from micro)
    */

   FU_I2C_SW_start(PLTFRM_I2C_SW_BUS_1_ID);

   if (FU_I2C_SW_sendDevAddr_7(PLTFRM_I2C_SW_BUS_1_ID, i2cAddr, I2C_OPN_TYPE_WRITE) == 0x0)
   {
       sts = PLTFRM_STS_I2C_BUS_WR_ERROR;
       goto _end;
   }

   addrByte = addr;
   // addrByte /= 256;   // (addrByte >> 8) & 0xff;
   {
      UINT8_t idx;
      for (idx=0; idx<8; idx++)
           addrByte /= 2;
   }

   if (FU_I2C_SW_wrByte(PLTFRM_I2C_SW_BUS_1_ID, (UINT8_t)addrByte) == 0x0)
   {
       sts = PLTFRM_STS_I2C_BUS_WR_ERROR;
       goto _end;
   }

   addrByte = addr;
   addrByte &= 0xff;

   if (FU_I2C_SW_wrByte(PLTFRM_I2C_SW_BUS_1_ID, (UINT8_t)addrByte) == 0x0)
   {
       sts = PLTFRM_STS_I2C_BUS_WR_ERROR;
       goto _end;
   }

   FU_I2C_SW_start(PLTFRM_I2C_SW_BUS_1_ID);

   if (FU_I2C_SW_sendDevAddr_7(PLTFRM_I2C_SW_BUS_1_ID, i2cAddr, I2C_OPN_TYPE_READ) == 0x0)
   {
       sts = PLTFRM_STS_I2C_BUS_WR_ERROR;
       goto _end;
   }

   /*
    * Sequential Read
    * This operation can be used after a Current Address Read or a Random
    * Address Read. The bus master does acknowledge the data byte output, and
    * sends additional clock pulses so that the device continues to output the
    * next byte in sequence. To terminate the stream of bytes, the bus master
    * must not acknowledge the last byte, and must generate a Stop condition.
    *
    * The output data comes from consecutive addresses, with the internal address
    * counter automatically incremented after each byte output. After the last
    * memory address, the address counter “rolls-over”, and the device continues
    * to output data from memory address 00h.
    */

   {
      UINT16_t idx;

      for (idx=0; idx<cnt; idx++)
      {
           FU_I2C_SW_rdByte(PLTFRM_I2C_SW_BUS_1_ID,
                            byte_p + idx,
                            idx < cnt - 1 ? I2C_ACK : I2C_NO_ACK);
      }
   }

_end:
   FU_I2C_SW_stop(PLTFRM_I2C_SW_BUS_1_ID);
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
PLTFRM_sts_t FU_M24M01_read(UINT32_t addr,
                            UINT16_t len,
                            UINT8_t *byte_p)
{
   return __FU_M24M01_read(addr, len, byte_p);
}


#pragma RETAIN(PLTFRM_flashClear)
#pragma CODE_SECTION(PLTFRM_flashClear, ".flashUpdate")

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void PLTFRM_flashClear(UINT8_t *dataPtr_p)
{
  unsigned char sIE1;

  /*
   * The erased level of a flash memory bit is 1. Each bit can be programmed
   * from 1 to 0 individually but to reprogram from 0 to 1 requires an erase
   * cycle. The smallest amount of flash that can be erased is a segment.
   *
   * Any erase is initiated by a dummy write into the address range to be erased.
   * The dummy write starts the flash timing generator and the erase operation.
   *
   * When a flash segment erase operation is initiated from within flash
   * memory, all timing is controlled by the flash controller, and the CPU
   * is held while the erase cycle completes. After the erase cycle completes,
   * the CPU resumes code execution with the instruction following the dummy
   * write.
   *
   * The flash timing generator operating frequency, fFTG, must be in the range
   * from approximately 257 kHz to approximately 476 kHz.
   */

  // FWKEY (bits 8 to 15) is 0xa5 - used when writing to FCTLx
  // Writing any other value generates a PUC.

  /*
   * FCTL3:Lock.
   * This bit unlocks the flash memory for writing or erasing. The LOCK bit can
   * be set any time during a byte or word write or erase operation, and the opn
   * completes normally. In the block write mode if the LOCK bit is set while
   * BLKWRT = WAIT = 1, then BLKWRT and WAIT are reset and the mode ends normally.
   *  0 - Unlocked
   *  1 - Locked
   */

  /*
   * Presently, only one flash module is implemented on MSP430 devices for program
   * and data memory. This means that, during flash programming, the interrupt vectors
   * are unavailable and no interrupt request can be served. Therefore, all possible
   * interrupt sources, as well as the watchdog, must be disabled while flash is being
   * modified.
   */

  sIE1 = IE1;
  IE1 = 0x0;

  FCTL2 = 0xa542;

  FCTL3 = 0xa500;  // Lock = 0 (Unlocks flash memory for writing or erasing)
  FCTL1 = 0xa502;  // MERAS = 0 and ERASE = 1 (Erase individual segment only)

  *dataPtr_p = 0;  /* erase Flash segment */

  FCTL1 = 0xa500;  // MERAS = 0 and ERASE = 0 (No erase)
  FCTL3 = 0xa510;  // Lock = 1 (Lock flash memory)

  IE1 = sIE1;

  return;
}

#pragma RETAIN(PLTFRM_FlashWriteByte)
#pragma CODE_SECTION(PLTFRM_FlashWriteByte, ".flashUpdate")

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void PLTFRM_FlashWriteByte(UINT8_t *addr_p, UINT8_t byte)
{
  unsigned char sIE1;

  sIE1 = IE1;
  IE1 = 0x0;

  FCTL3 = 0x0A500; /* Lock = 0 */
  FCTL1 = 0x0A540; /* WRT = 1 */

  /*
   * Any instruction that modifies a destination can be used to modify a
   * flash location in either byte or word write mode or block write mode.
   */
  *addr_p = byte; /* program Flash byte */

  FCTL1 = 0x0A500; /* WRT = 0 */
  FCTL3 = 0x0A510; /* Lock = 1 */

  IE1 = sIE1;

  return;
}

#pragma RETAIN(PLTFRM_clearSegFlash)
#pragma CODE_SECTION(PLTFRM_clearSegFlash, ".flashUpdate")

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void PLTFRM_clearSegFlash(UC_FU_segInfo_s *segInfo_p)
{
   UINT16_t idx;
   UINT8_t *segStart_p = (UINT8_t *)(segInfo_p->segAddr);

   for (idx=0; idx<segInfo_p->segLen; idx++)
   {
        UINT16_t flashAddr = (UINT16_t)segStart_p + idx;

        if ((flashAddr & 0xff) == 0x0
            || idx == 0)
        {
            PLTFRM_flashClear((UINT8_t *)flashAddr);
        }
   }

   return;
}

#pragma RETAIN(PLTFRM_writeSegToFlash)
#pragma CODE_SECTION(PLTFRM_writeSegToFlash, ".flashUpdate")

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void PLTFRM_writeSegToFlash(UINT32_t storeOff, UC_FU_segInfo_s *segInfo_p)
{
   UINT16_t idx;
   UINT8_t *segStart_p = (UINT8_t *)(segInfo_p->segAddr);

   PLTFRM_ON_LED_1();
   PLTFRM_OFF_LED_2();

   for (idx=0; idx<segInfo_p->segLen; idx++)
   {
        UINT8_t byte;

        if ((idx & 0xff) == 0)
        {
            PLTFRM_TOGGLE_LED_1();
            PLTFRM_TOGGLE_LED_2();
        }

        // Read byte from
        if (FU_M24M01_read(storeOff + idx, 1, &byte) != PLTFRM_STS_SUCCESS)
        {
            __FU_fatal();
        }

        // Write byte to flash
        PLTFRM_FlashWriteByte(segStart_p + idx, byte);
   }

   return;
}


#pragma RETAIN(__readImageStoreFlags)
#pragma CODE_SECTION(__readImageStoreFlags, ".flashUpdate")

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void __readImageStoreFlags(UINT8_t *buff_p)
{
   PLTFRM_sts_t sts;

   sts = FU_M24M01_read(FW_STORE_BR_FLAGS_OFF,
                        FW_STORE_BR_FLAGS_LEN,
                        buff_p);
   if (sts != PLTFRM_STS_SUCCESS)
       __FU_fatal();

   return;
}


#pragma RETAIN(__writeImageStoreFlags)
#pragma CODE_SECTION(__writeImageStoreFlags, ".flashUpdate")

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void __writeImageStoreFlags(UINT8_t *buff_p)
{
   PLTFRM_sts_t sts;

   sts = FU_M24M01_write(FW_STORE_BR_FLAGS_OFF,
                         FW_STORE_BR_FLAGS_LEN,
                         buff_p);
   if (sts != PLTFRM_STS_SUCCESS)
       __FU_fatal();


   return;
}


#pragma RETAIN(PLTFRM_reflash)
#pragma CODE_SECTION(PLTFRM_reflash, ".flashUpdate")

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void PLTFRM_reflash(UINT32_t imageStartInStore, UC_FU_segInfo_s * const segInfo_p, UINT8_t segCnt)
{
   UINT8_t idx, flag, retryCnt = 0;
   UINT16_t offset = 0;
   UC_FU_segInfo_s *seg_p = segInfo_p;

   WDTCTL = WDTPW + WDTHOLD;

   PLTFRM_intSaveDisable(flag);
   (void)flag;


   while (1)
   {
      seg_p = segInfo_p;

      for (idx=0; idx<segCnt; idx++)
      {
           // Segment of len seg_p->segLen needs to be written to seg_p->segAddr
           // in the flash

           PLTFRM_clearSegFlash(seg_p);
           offset += seg_p->segLen;
           seg_p ++;
      }

      offset = 0;
      seg_p = segInfo_p;

      for (idx=0; idx<segCnt; idx++)
      {
           // Segment of len seg_p->segLen needs to be written to seg_p->segAddr
           // in the flash

           PLTFRM_writeSegToFlash(imageStartInStore + offset, seg_p);

           if (PLTFRM_checkFlashSegCRC(seg_p) == 0x0)
               break;

           offset += seg_p->segLen;
           seg_p ++;
      }

      if (idx >= segCnt)
          break;

      retryCnt ++;

      if (retryCnt >= 3)
      {
          // What can be done ??

          PLTFRM_OFF_LED_1();
          PLTFRM_OFF_LED_2();

          for (;;)
          {
               FU_delay500MSecs();
               PLTFRM_ON_LED_1();

               FU_delay500MSecs();
               PLTFRM_OFF_LED_1();

               FU_delay500MSecs();
               PLTFRM_ON_LED_1();

               FU_delay500MSecs();
               PLTFRM_OFF_LED_1();

               FU_delay500MSecs();
               PLTFRM_ON_LED_2();

               FU_delay500MSecs();
               PLTFRM_OFF_LED_2();

               FU_delay500MSecs();
               PLTFRM_ON_LED_2();

               FU_delay500MSecs();
               PLTFRM_OFF_LED_2();
          }

          PLTFRM_enterLPM();
      }
   }

   return;
}

#pragma RETAIN(PLTFRM_checkFlashSegCRC)
#pragma CODE_SECTION(PLTFRM_checkFlashSegCRC, ".flashUpdate")

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
UINT8_t PLTFRM_checkFlashSegCRC(UC_FU_segInfo_s *seg_p)
{
   UINT16_t segLen = seg_p->segLen;
   UINT8_t *flashAddr_p = (UINT8_t *)(seg_p->segAddr);
   UINT32_t ckSum = 0;

   PLTFRM_ON_LED_1();
   PLTFRM_ON_LED_2();

   while (segLen > 1)
   {
      UINT16_t tmp;

      if ((segLen & 0xff) == 0
          || (((segLen + 1) & 0xff) == 0))
      {
          PLTFRM_TOGGLE_LED_1();
          PLTFRM_TOGGLE_LED_2();
      }

      tmp = *flashAddr_p;
      flashAddr_p ++;

      {
         UINT8_t jdx;
         for (jdx=0; jdx<8; jdx++)    // tmp <<= 8;
              tmp += tmp;
      }

      tmp |= *flashAddr_p;
      flashAddr_p ++;

      ckSum = ckSum + tmp;

      segLen -= 2;
   }

   if (segLen > 0)
       ckSum += *flashAddr_p;

   while (ckSum >> 16)
          ckSum = (ckSum & 0xffff) + (ckSum >> 16);

   ckSum = ~ckSum;
   ckSum = ckSum & 0xffff;

   return ckSum == seg_p->segCrc ? 1 : 0;
}


#endif
