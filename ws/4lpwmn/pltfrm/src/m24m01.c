/*
 * File Name: m24m01.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Jan/22/2015
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


#ifdef PLTFRM_M24M01_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <m24m01.h>

// #define M24M01_TEST_ENA


const M24M01_cntxt_s M24M01_cntxtList[PLTFRM_M24M01_DEV_CNT] =
{
   {PLTFRM_M24M01_1_DEV_ID,  PLTFRM_I2C_SW_BUS_1_ID, 0x3},
};




/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
const M24M01_cntxt_s *M24M01_getDevCntxt(UINT8_t devId)
{
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_M24M01_DEV_CNT; idx++)
   {
      if (M24M01_cntxtList[idx].devId == devId)
          return (&(M24M01_cntxtList[idx]));
   }

   return NULL;
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
PLTFRM_sts_t __M24M01_write(const M24M01_cntxt_s *devCntxt_p,
                            UINT32_t addr,
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

  i2cAddr = M24M01_I2C_ADDR_PREFIX_1 | (devCntxt_p->chipEnaAddr << 1);
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

   I2C_SW_start(devCntxt_p->busId);

   if (I2C_SW_sendDevAddr_7(devCntxt_p->busId, i2cAddr, I2C_OPN_TYPE_WRITE) == 0x0)
   {
       sts = PLTFRM_STS_I2C_BUS_WR_ERROR;
       goto _end;
   }

   addrByte = addr;
   addrByte = (addrByte >> 8) & 0xff;

   if (I2C_SW_wrByte(devCntxt_p->busId, (UINT8_t)addrByte) == 0x0)
   {
       sts = PLTFRM_STS_I2C_BUS_WR_ERROR;
       goto _end;
   }

   addrByte = addr;
   addrByte &= 0xff;

   if (I2C_SW_wrByte(devCntxt_p->busId, (UINT8_t)addrByte) == 0x0)
   {
       sts = PLTFRM_STS_I2C_BUS_WR_ERROR;
       goto _end;
   }

   if (I2C_SW_wrByte(devCntxt_p->busId, byte) == 0x0)
       sts = PLTFRM_STS_I2C_BUS_WR_ERROR;

_end:
   I2C_SW_stop(devCntxt_p->busId);

   // Byte write is within 5 milli-secs
   if (sts == PLTFRM_STS_SUCCESS)
   {
       UINT8_t idx;
       for (idx=0; idx<M24M01_BYTE_WRITE_DURATION_MILLISECS; idx++)
           PLTFRM_delay1MilliSec();
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
PLTFRM_sts_t M24M01_write(UINT8_t devId,
                          UINT32_t addr,
						  UINT16_t len,
                          UINT8_t *buff_p)
{
   UINT16_t idx;
   const M24M01_cntxt_s *devCntxt_p = M24M01_getDevCntxt(devId);
   PLTFRM_sts_t sts;

   for (idx=0; idx<len; idx++)
   {
       sts = __M24M01_write(devCntxt_p, addr + idx, buff_p[idx]);
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
PLTFRM_sts_t __M24M01_read(const M24M01_cntxt_s *devCntxt_p,
                           UINT32_t addr,
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

   i2cAddr = M24M01_I2C_ADDR_PREFIX_1 | (devCntxt_p->chipEnaAddr << 1);
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

   I2C_SW_start(devCntxt_p->busId);

   if (I2C_SW_sendDevAddr_7(devCntxt_p->busId, i2cAddr, I2C_OPN_TYPE_WRITE) == 0x0)
   {
       sts = PLTFRM_STS_I2C_BUS_WR_ERROR;
       goto _end;
   }

   addrByte = addr;
   addrByte = (addrByte >> 8) & 0xff;

   if (I2C_SW_wrByte(devCntxt_p->busId, (UINT8_t)addrByte) == 0x0)
   {
       sts = PLTFRM_STS_I2C_BUS_WR_ERROR;
       goto _end;
   }

   addrByte = addr;
   addrByte &= 0xff;

   if (I2C_SW_wrByte(devCntxt_p->busId, (UINT8_t)addrByte) == 0x0)
   {
       sts = PLTFRM_STS_I2C_BUS_WR_ERROR;
       goto _end;
   }

   I2C_SW_start(devCntxt_p->busId);

   if (I2C_SW_sendDevAddr_7(devCntxt_p->busId, i2cAddr, I2C_OPN_TYPE_READ) == 0x0)
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
           I2C_SW_rdByte(devCntxt_p->busId,
                         byte_p + idx,
				 	     idx < cnt - 1 ? I2C_ACK : I2C_NO_ACK);
      }
   }

_end:
   I2C_SW_stop(devCntxt_p->busId);
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
PLTFRM_sts_t M24M01_read(UINT8_t devId,
                         UINT32_t addr,
						 UINT16_t len,
                         UINT8_t *byte_p)
{
	const M24M01_cntxt_s *devCntxt_p = M24M01_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   return __M24M01_read(devCntxt_p, addr, len, byte_p);
}


// #define M24M01_TEST_ENA


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t M24M01_init(const UINT8_t devId)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   const M24M01_cntxt_s *devCntxt_p = M24M01_getDevCntxt(devId);
   UINT8_t i2cAddr;

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   // Address for accessing memory (lower 64 Kilo-bytes (A16 is 0))
   i2cAddr = M24M01_I2C_ADDR_PREFIX_1 | (devCntxt_p->chipEnaAddr << 1);
   sts = PLTFRM_regI2CDev(devId, devCntxt_p->busId, i2cAddr);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   // Address for accessing memory (upper 64 Kilo-bytes (A16 is 1))
   i2cAddr |= 0x1;
   sts = PLTFRM_regI2CDev(devId, devCntxt_p->busId, i2cAddr);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   // Address for accessing id page (DC bit is 0)
   i2cAddr = M24M01_I2C_ADDR_PREFIX_2 | (devCntxt_p->chipEnaAddr << 1);
   sts = PLTFRM_regI2CDev(devId, devCntxt_p->busId, i2cAddr);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   // Address for accessing id page (DC bit is 1)
   i2cAddr |= 0x1;
   sts = PLTFRM_regI2CDev(devId, devCntxt_p->busId, i2cAddr);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

#ifdef M24M01_TEST_ENA
   {
      UINT32_t idx = 0;
      UINT8_t c;

      for (idx=0; idx<10; idx++)
      {
          sts = __M24M01_write(devCntxt_p, idx, '0' + (idx % 10));
          if (sts != PLTFRM_STS_SUCCESS)
          {
              SYS_fatal(SYS_FATAL_ERR_278);
          }
      }

      for (idx=0; idx<10; idx++)
      {
          sts = __M24M01_read(devCntxt_p, idx, 1, &c);
          if (sts != PLTFRM_STS_SUCCESS)
          {
              SYS_fatal(SYS_FATAL_ERR_278);
          }

          if (c != ('0' + (idx % 10)))
          {
              SYS_fatal(SYS_FATAL_ERR_609);
          }
      }
   }
#endif
   return sts;
}

#endif
