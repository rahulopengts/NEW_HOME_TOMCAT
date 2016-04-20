/*
 * File Name: chirp_wla.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Feb/3/2015
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

#ifdef PLTFRM_CHIRP_PWLA_ENA

#include <typedefs.h>
#include <system.h>
#include <pltfrm.h>
#include <chirp_pwla.h>

static const char _snsrName[] = "CHIRP_PWLA";
static const char _snsrMfr[] = "WMT";   // WeMakeThings (http://wemakethings.net/)

CHIRP_PWLA_cntxt_s CHIRP_PWLA_cntxtList[PLTFRM_LMP75B_DEV_CNT] =
{
   {
     PLTFRM_CHIRP_PWLA_1_DEV_ID,
     PLTFRM_I2C_SW_BUS_1_ID,
     CHIRP_PWLA_I2C_ADDR,
     PLTFRM_GPIO_PORT_2,
     PLTFRM_GPIO_PORT_PIN_1,
     {_snsrName, _snsrMfr}
   },
};

/*
 * The Chirp plant water level alarm It’s based on ATTINY44A microcontroller. A standard
 * AVR 6 pin ISP programming header is available on the board for programming and for
 * serial communication.
 * The device acts as a I2C slave, the header can be used to read the moisture and light
 * levels. Another microcontroller or a board such as Arduino can be used as I2C master
 * to read those levels.
 *
 * pin 1 - MISO
 * pin 2 - VCC
 * pin 3 - SCK / SCL – I2C clock
 * pin 4 - MOSI / SDA – I2C data
 * pin 5 - RESET
 * pin 6 - GND
 *
 * The default I2C address of the Chirp is 0x20.
 *
 * The Chirp has two modes of operation.
 * - Stand alone mode
 * - Sensor mode
 *   To enter this mode, the master has to send a byte to the Chirp right after resetting it.
 *   Once it enters this mode, it will not chirp. It will just respond to I2C requests.
 *   Cmd 0x0 - Request new measurement and get it from the Chirp in one shot.
 *             (16 bit read operation).
 *   Cmd 0x1 - Configure new I2C address. New I2C address will be stored in the EEPROM.
 *             New I2C address will come into effect only after reset.
 *             (8 bit I2C write operation).
 *   Cmd 0x2 - Read I2C address from EEPROM
               (8 bit read operation).
 *   Cmd 0x3 - Request light measurement
 *             (8 bit write operation).
 *   Cmd 0x4 - Request Chirp to send light measurement
 *             (16 bit read operation).
 *
 */
PLTFRM_sts_t CHIRP_PWLA_read16Bits(CHIRP_PWLA_cntxt_s *devCntxt_p,
                                   UINT8_t *buff_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   /*
    * > START                               (from micro)
    * > 7 bit device address | read bit     (from micro)
    * > ACK                                 (from CHIRP)
    * > 8 bit value                         (from CHIRP)
    * > ACK                                 (from micro)
    * > 8 bit value                         (from CHIRP)
    * > NACK                                (from micro)
    * > STOP
    */

   I2C_SW_start(devCntxt_p->i2cBusId);

   if (I2C_SW_sendDevAddr_7(devCntxt_p->i2cBusId,
                            devCntxt_p->i2cAddr,
                            I2C_OPN_TYPE_READ) == 0x0)
       sts = PLTFRM_STS_I2C_BUS_WR_ERROR;
   else
   {
       I2C_SW_rdByte(devCntxt_p->i2cBusId, buff_p, I2C_ACK);

       I2C_SW_rdByte(devCntxt_p->i2cBusId, buff_p + 1, I2C_NO_ACK);

       I2C_SW_stop(devCntxt_p->i2cBusId);
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
PLTFRM_sts_t CHIRP_PWLA_write8Bits(CHIRP_PWLA_cntxt_s *devCntxt_p,
                                   UINT8_t val)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;

   /*
    * > START                               (from micro)
    * > 7 bit device address | Write bit    (from micro)
    * > ACK                                 (from CHIRP)
    * > 8 bit value                         (from micro)
    * > ACK                                 (from CHIRP)
    * > STOP                                (from micro)
    */

   I2C_SW_start(devCntxt_p->i2cBusId);

   PLTFRM_delay1MilliSec();

   if (I2C_SW_sendDevAddr_7(devCntxt_p->i2cBusId,
                            devCntxt_p->i2cAddr,
                            I2C_OPN_TYPE_WRITE) == 0x0)
       return PLTFRM_STS_I2C_BUS_WR_ERROR;

   if (I2C_SW_wrByte(devCntxt_p->i2cBusId, val) == 0x0)
       return PLTFRM_STS_I2C_BUS_WR_ERROR;

   I2C_SW_stop(devCntxt_p->i2cBusId);

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
CHIRP_PWLA_cntxt_s *CHIRP_PWLA_getDevCntxt(UINT8_t devId)
{
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_CHIRP_PWLA_DEV_CNT; idx++)
   {
      if (CHIRP_PWLA_cntxtList[idx].devId == devId)
          return (&(CHIRP_PWLA_cntxtList[idx]));
   }

   return NULL;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t CHIRP_PWLA_getMoistureLevel(UINT16_t devId, UINT16_t *opVal_p)
{
   UINT8_t valBuff[2];
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   CHIRP_PWLA_cntxt_s *devCntxt_p = CHIRP_PWLA_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   sts = CHIRP_PWLA_write8Bits(devCntxt_p, CHIRP_PWLA_CMD_MOISTURE_MEAS);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       UINT16_t idx;

       // Wait for 1 second !!
       for (idx=0; idx<1000; idx++)
            PLTFRM_delay1MilliSec();

       sts = CHIRP_PWLA_read16Bits(devCntxt_p, valBuff);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           *opVal_p = valBuff[0];
           (*opVal_p) <<= 8;
           *opVal_p |= valBuff[1];
       }
   }

   return sts;
}

UINT16_t minLevel = 0xffff, maxLevel = 0, cnt = 0;

// #define CHIRP_PWLA_TEST_ENA

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t CHIRP_PWLA_init(UINT8_t devId)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   CHIRP_PWLA_cntxt_s *devCntxt_p = CHIRP_PWLA_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   sts = PLTFRM_regI2CDev(devId, devCntxt_p->i2cBusId, devCntxt_p-> i2cAddr);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   // Allocate GPIO for resetting the CHIRP
   sts = PLTFRM_allocGPIOPin(PLTFRM_GENERIC_DEV_ID,
                             devCntxt_p->resetPort,
                             devCntxt_p->resetPin,
                             PLTFRM_GPIO_PIN_DIRECTION_OUTPUT,
                             PLTFRM_GPIO_OUTPUT_HIGH,
                             PLTFRM_GPIO_INT_EDGE_NA,
                             NULL);
   if (sts != PLTFRM_STS_SUCCESS)
       SYS_fatal(SYS_FATAL_ERR_546);

   // Reset the device
   PLTFRM_setGPIOPinOutput(devCntxt_p->resetPort, devCntxt_p->resetPin, 0x1);
   PLTFRM_delay1MilliSec();
   PLTFRM_setGPIOPinOutput(devCntxt_p->resetPort, devCntxt_p->resetPin, 0x0);
   {
      UINT8_t idx;
      for (idx=0; idx<CHIRP_PWLA_RESET_TIME_MILLISECS; idx++)
           PLTFRM_delay1MilliSec();
   }
   PLTFRM_setGPIOPinOutput(devCntxt_p->resetPort, devCntxt_p->resetPin, 0x1);
   {
      UINT8_t idx;
      for (idx=0; idx<100; idx++)
           PLTFRM_delay1MilliSec();
   }
   // Write some byte to the CHIRP
   sts = CHIRP_PWLA_write8Bits(devCntxt_p, 0x6);
   if (sts != PLTFRM_STS_SUCCESS)
       return sts;

   {
      UINT16_t idx;
      for (idx=0; idx<CHIRP_PWLA_RESET_HOLD_TIME_MILLISECS; idx++)
           PLTFRM_delay1MilliSec();
   }

#ifdef CHIRP_PWLA_TEST_ENA

   for (;;)
   {
      UINT16_t level;
      sts = CHIRP_PWLA_getMoistureLevel(devId, &level);
      if (sts != PLTFRM_STS_SUCCESS)
          return sts;
      if (level < minLevel)
          minLevel = level;
      if (level > maxLevel)
          maxLevel = level;
      cnt ++;
   }

   for (;;)
   {
      UINT8_t buff[2];

      sts = CHIRP_PWLA_write8Bits(devCntxt_p,
                                  CHIRP_PWLA_CMD_START_LIGHT_MEAS);
      if (sts != PLTFRM_STS_SUCCESS)
          break;

      {
    	 UINT8_t idx;
    	 for (idx=0; idx<10; idx++)
              PLTFRM_delay1MilliSec();
      }

      sts = CHIRP_PWLA_write8Bits(devCntxt_p,
                                  CHIRP_PWLA_CMD_GET_LIGHT_OUTPUT);
      if (sts != PLTFRM_STS_SUCCESS)
          break;

      sts = CHIRP_PWLA_read16Bits(devCntxt_p, buff);
      if (sts != PLTFRM_STS_SUCCESS)
          break;
   }
#endif

   return sts;
}

#endif
