/*
 * File Name: isp.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: June/15/2015
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

/*
 * Usually the CPU reads the flash to access data or to execute a program. However, sometimes
 * flash needs to be modified during program execution. During such a flash erase-and-program
 * operation (i.e., flash reprogramming) the timing generator implemented in the flash module
 * takes over control of flash. During this time, flash cannot be accessed by the CPU and,
 * consequently, program instructions must come from some other place, such as RAM, or else the
 * CPU must be sent into idle mode. After completing flash programming, the CPU again gains
 * control of flash. Each of these two methods has its specific advantages
 *
 * < Direct Flash Reprogramming >
 * A unique feature of the MSP430 flash module is self-programmability without needing
 * to copy the program into other memory. When the CPU fetches instructions from flash
 * memory during flash reprogramming, flash returns 3FFFh (JMP $) to the CPU. This sends
 * the CPU into an endless loop until flash reprogramming has been completed. On completion,
 * flash returns the next instruction and program execution continues.
 *
 * This is the easiest way to reprogram MSP430 flash memory. One disadvantage, however, is
 * that the CPU is in idle mode during flash reprogramming, so no program can be executed
 * and no interrupt can be processed. Furthermore, this flash reprogramming technique works
 * only in byte/word-program mode (bit 7 in FCTL1 is zero), and the faster segment-write mode
 * cannot be used.
 *
 * The three 16-bit control registers FCTL1, FCTL2, and FCTL3 control the complete Flash module.
 *
 * > MSP430G2955 Flash memory - 56 KB
 *             Information memory (256 bytes) -> 0x10FF to 0x1000
 *             Code memory (55.75 Kilo-bytes) -> 0x2100 to 0xffff
 *
 * > Flash memory has n segments of main memory and four segments of information memory (A to D)
 *   of 64 bytes each. Each segment in main memory is 512 bytes in size.
 *
 * > Segments 0 to n may be erased in one step, or each segment may be individually erased.
 *
 * > MSP430 flash memory is partitioned into segments. Single bits, bytes, or words can be written
 *   to flash memory, but the segment is the smallest size of flash memory that can be erased
 *
 * > Minimum VCC during flash write or erase
 *   The minimum VCC voltage during a flash write or erase operation is 2.2 V. If VCC falls
 *   below 2.2 V during write or erase, the result of the write or erase is unpredictable.
 *
 * > Segment erase time -> 4819 Tftg (where Tftg is (1 / (Fftg)))
 *
 * > The default mode of the flash memory is read mode. In read mode, the flash memory is not
 *   being erased or written, the flash timing generator and voltage generator are off, and the
 *   memory operates identically to ROM.
 *
 * > MSP430 flash memory is in-system programmable (ISP) without the need for additional external
 *   voltage. The CPU can program its own flash memory. The flash memory write and erase modes are
 *   selected with the BLKWRT, WRT, MERAS, and ERASE bits and are:
 *   -  Byte or word write
 *   - Block write
 *   - Segment erase
 *   - Mass erase (all main memory segments)
 *   - All erase (all segments)
 *
 * > Reading from or writing to flash memory while it is being programmed or erased is prohibited.
 *   If CPU execution is required during the write or erase, the code to be executed must be in RAM.
 *   Any flash update can be initiated from within flash memory or RAM.
 */

#include <msp430.h>


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void Flash_wb( char *Data_ptr, char byte )
{
  FCTL3 = 0x0A500; /* Lock = 0 */
  FCTL1 = 0x0A540; /* WRT = 1 */
  *Data_ptr=byte; /* program Flash word */
  FCTL1 = 0x0A500; /* WRT = 0 */
  FCTL3 = 0x0A510; /* Lock = 1 */
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void Flash_ww( int *Data_ptr, int word )
{
  FCTL3 = 0x0A500; /* Lock = 0 */
  FCTL1 = 0x0A540; /* WRT = 1 */
  *Data_ptr=word; /* program Flash word */
  FCTL1 = 0x0A500; /* WRT = 0 */
  FCTL3 = 0x0A510; /* Lock = 1 */
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void Flash_clr(int *dataPtr_p)
{
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

  FCTL3 = 0xa500;  // Lock = 0 (Unlocks flash memory for writing or erasing)
  FCTL1 = 0xa502;  // MERAS = 0 and ERASE = 1 (Erase individual segment only)

  *dataPtr_p = 0;  /* erase Flash segment */

  FCTL1 = 0xa500;  // MERAS = 0 and ERASE = 0 (No erase)
  FCTL3 = 0xa510;  // Lock = 1 (Lock flash memory)

  return;
}
