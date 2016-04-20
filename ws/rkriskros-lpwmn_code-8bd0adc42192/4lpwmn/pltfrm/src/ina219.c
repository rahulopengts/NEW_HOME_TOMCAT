/*
 * File Name: ina219.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: april/25/2015
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

#ifdef PLTFRM_INA219_ENA

#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <ina219.h>

// #define INA219_UT_ENA

static const char _snsrName_1[] = "INA219";
static const char _snsrMfr_1[] = "TI";

INA219_cntxt_s  INA219_cntxtList[PLTFRM_INA219_DEV_CNT] =
{
   {
      PLTFRM_INA219_1_BV_DEV_ID,
      PLTFRM_INA219_1_SV_DEV_ID,
      0x41,  // A0 High, A1 low
      PLTFRM_I2C_SW_BUS_1_ID,
      INA219_BUS_VOLTAGE_RANGE_UNKNOWN,
      INA219_MODE_SHUNT_AND_BUS_CONTINUOUS,
      INA219_SV_RANGE_SETTING_320_MV,
      {_snsrName_1, _snsrMfr_1}
   },

   {
      PLTFRM_INA219_2_BV_DEV_ID,
      PLTFRM_INA219_2_SV_DEV_ID,
      0x45,  // A0 High, A1 high
      PLTFRM_I2C_SW_BUS_1_ID,
      INA219_BUS_VOLTAGE_RANGE_UNKNOWN,
      INA219_MODE_SHUNT_AND_BUS_CONTINUOUS,
      INA219_SV_RANGE_SETTING_320_MV,
      {_snsrName_1, _snsrMfr_1}
   },

   {
      PLTFRM_INA219_3_BV_DEV_ID,
      PLTFRM_INA219_3_SV_DEV_ID,
      0x40,  // A0 low, A1 low
      PLTFRM_I2C_SW_BUS_1_ID,
      INA219_BUS_VOLTAGE_RANGE_UNKNOWN,
      INA219_MODE_SHUNT_AND_BUS_CONTINUOUS,
      INA219_SV_RANGE_SETTING_320_MV,
      {_snsrName_1, _snsrMfr_1}
   },

   {
      PLTFRM_INA219_4_BV_DEV_ID,
      PLTFRM_INA219_4_SV_DEV_ID,
      0x44,  // A0 low, A1 high
      PLTFRM_I2C_SW_BUS_1_ID,
      INA219_BUS_VOLTAGE_RANGE_UNKNOWN,
      INA219_MODE_SHUNT_AND_BUS_CONTINUOUS,
      INA219_SV_RANGE_SETTING_320_MV,
      {_snsrName_1, _snsrMfr_1}
   }
};


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
INA219_cntxt_s *INA219_getDevCntxt(UINT8_t devId)
{
   UINT16_t idx;

   for (idx=0; idx<PLTFRM_INA219_DEV_CNT; idx++)
   {
      if (INA219_cntxtList[idx].devId1 == devId
          || INA219_cntxtList[idx].devId2 == devId)
          return (&(INA219_cntxtList[idx]));
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
PLTFRM_sts_t INA219_readReg(INA219_cntxt_s *devCntxt_p,
                            UINT8_t regAddr, UINT8_t rdCnt,
                            UINT8_t *rdBuff_p)
{
    PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
    UINT8_t busId = devCntxt_p->i2cBusId;
    UINT8_t i2cAddr = devCntxt_p->i2cDevId;

    /*
     * Accessing a particular register on the INA219 is accomplished by writing
     * the appropriate value to the register pointer. The value for the register
     * pointer is the first byte transferred after the slave address byte with the
     * R/W bit LOW.
     * The master then generates a START condition and sends the slave address byte
     * with the R/W bit HIGH to initiate the read command. The next byte is transmitted
     * by the slave and is the most significant byte of the register indicated by the
     * register pointer. This byte is followed by an Ack from the master; then the slave
     * transmits the least significant byte. The The master acknowledges receipt of the
     * data byte. The master may terminate data transfer by generating a Not-Acknowledge
     * after receiving any data byte, or generating a START or STOP condition.
     */

   /*
    * Read a register.
    *
    * > START                               (from micro)
    * > 7 bit device address | Write bit    (from micro)
    * > ACK                                 (from INA219)
    * > 8 bit register address              (from micro)
    * > ACK                                 (from INA219)
    * > RE-START                            (from micro)
    * > 7 bit device address | Read bit     (from micro)
    * > Read Byte (MSB)                     (from INA219)
    * > Ack bit                             (from micro)
    * > Read Byte  (LSB)                    (from INA219)
    * > Ack bit                             (from micro)
    * > STOP                                (from micro)
    */

   I2C_SW_start(busId);

   if (I2C_SW_sendDevAddr_7(busId, i2cAddr, I2C_OPN_TYPE_WRITE) == 0x0)
   {
       I2C_SW_stop(busId);
       return PLTFRM_STS_I2C_BUS_WR_ERROR;
   }

   if (I2C_SW_wrByte(busId, regAddr) == 0x0)
   {
       I2C_SW_stop(busId);
       return PLTFRM_STS_I2C_BUS_WR_ERROR;
   }

   I2C_SW_start(busId);

   if (I2C_SW_sendDevAddr_7(busId, i2cAddr, I2C_OPN_TYPE_READ) == 0x0)
   {
       I2C_SW_stop(busId);
       return PLTFRM_STS_I2C_BUS_WR_ERROR;
   }

   if (rdCnt == 2)
   {
       I2C_SW_rdByte(busId, rdBuff_p, I2C_ACK);
       rdBuff_p ++;
   }

   I2C_SW_rdByte(busId, rdBuff_p, I2C_NO_ACK);

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
PLTFRM_sts_t INA219_writeReg(INA219_cntxt_s *devCntxt_p,
                             UINT8_t regAddr, UINT8_t wrCnt,
                             UINT8_t *wrBuff_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t busId = devCntxt_p->i2cBusId;
   UINT8_t i2cAddr = devCntxt_p->i2cDevId;

   /*
    * Writing to a register begins with the first byte transmitted by the
    * master. This byte is the slave address, with the R/W bit LOW. The
    * INA219 then acknowledges receipt of a valid address. The next
    * byte transmitted by the master is the address of the register to
    * which data will be written. This register address value updates the
    * register pointer to the desired register. The next two bytes are
    * written to the register addressed by the register pointer. The INA219
    * acknowledges receipt of each data byte. The master may terminate data
    * transfer by generating a START or STOP condition
    */

   /*
    * Write to a register.
    *
    * > START                               (from micro)
    * > 7 bit device address | Write bit    (from micro)
    * > ACK                                 (from IN219)
    * > 8 bit register address              (from micro)
    * > ACK                                 (from IN219)
    * > Write MSB                           (from micro)
    * > ACK                                 (from IN219)
    * > Write LSB                           (from micro)
    * > ACK                                 (from IN219)
    * > STOP                                (from micro)
    */

   I2C_SW_start(busId);

   if (I2C_SW_sendDevAddr_7(busId, i2cAddr, I2C_OPN_TYPE_WRITE) == 0x0)
       return PLTFRM_STS_I2C_BUS_WR_ERROR;

   if (I2C_SW_wrByte(busId, regAddr) == 0x0)
       return PLTFRM_STS_I2C_BUS_WR_ERROR;

   if (wrCnt == 2)
   {
       if (I2C_SW_wrByte(busId, *wrBuff_p) == 0x0)
       {
           I2C_SW_stop(busId);
           return PLTFRM_STS_I2C_BUS_WR_ERROR;
       }
       wrBuff_p ++;
   }

   if (I2C_SW_wrByte(busId, *wrBuff_p) == 0x0)
       sts = PLTFRM_STS_I2C_BUS_WR_ERROR;

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
PLTFRM_sts_t INA219_readShuntVoltage(UINT8_t devId, SINT16_t *voltage_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t regBuff[INA219_REG_LEN];
   INA219_cntxt_s *devCntxt_p = INA219_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   // Assuming INA219 mode is 'INA219_MODE_SHUNT_AND_BUS_CONTINUOUS".

   /*
    * The Shunt Voltage Register stores the current shunt voltage reading, VSHUNT. Shunt
    * Voltage Register bits are shifted according to the PGA setting selected in the Conf
    * Register (00h). When multiple sign bits are present, they will all be the same value.
    * Negative numbers are represented in two's complement format. Generate the two's complement
    * of a negative number by complementing the absolute value binary number and adding 1. Extend
    * the sign, denoting a negative number by setting the MSB = '1'. Extend the sign to any
    * additional sign bits to form the 16-bit word.
    */

   sts = INA219_readReg(devCntxt_p, INA219_SHUNT_V_REG_ADDR,
                        INA219_REG_LEN, regBuff);

   if (sts == PLTFRM_STS_SUCCESS)
   {
      UINT16_t tempU16;

      // The voltage value in the shunt voltage register is the actual voltage accross
      // the shunt resistor. No need to divide by the configured PG value.
      // Shunt voltage is calculated by multiplying the Shunt Voltage Register contents
      // with the Shunt Voltage LSB of 10 micro-Volts.
      // Example: If register value is 3250, this translates to 3250 * 10 * (10^-6)
      // == 3250 * (10^-5) Volts == 3250 * (10^-5) * 1000 milli-volts === 3250 / 100 = 32.50 mV.

      tempU16 = regBuff[0];
      tempU16 <<= 8;
      tempU16 |= regBuff[1];
      *voltage_p = (SINT16_t)tempU16;
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
PLTFRM_sts_t INA219_readBusVoltage(UINT8_t devId, SINT16_t *voltage_p)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   UINT8_t regBuff[INA219_REG_LEN];
   SINT16_t busV;
   INA219_cntxt_s *devCntxt_p = INA219_getDevCntxt(devId);

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   /*
    * The Bus Voltage Register stores the most recent bus voltage reading,
    * VBUS. At full-scale range = 32V (decimal = 8000, hex = 1F40), and LSB
    * = 4mV.
    */

   sts = INA219_readReg(devCntxt_p, INA219_BUS_V_REG_ADDR,
                        INA219_REG_LEN, regBuff);

   busV = regBuff[0];
   busV <<= 8;
   busV |= regBuff[1];

   if (busV & 0x1)
   {
       /*
        * OVF: Math Overflow Flag
        * Bit 0 The Math Overflow Flag (OVF) is set when the Power or Current calculations
        * are out of range. It indicates that current and power data may be meaningless.
        */
       sts = PLTFRM_STS_SNSR_OUTPUT_OVFL;
   }
   else
   {

       busV >>= 3;   // Skip bits 0, 1 add 2
       // LSB is 4 mV
       busV *= 4;
       *voltage_p = busV;
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
PLTFRM_sts_t INA219_cfgBusVoltageRange(UINT8_t devId, UINT8_t busVRCfg)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   INA219_cntxt_s *devCntxt_p = INA219_getDevCntxt(devId);
   UINT8_t regBuff[INA219_REG_LEN];

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   // Just read a register to make sure device is present.
   sts = INA219_readReg(devCntxt_p, INA219_CONF_REG_ADDR,
                        INA219_REG_LEN, regBuff);

   if (sts == PLTFRM_STS_SUCCESS)
   {
       UINT16_t regVal = regBuff[0];
       UINT8_t currBusVRCfg;

       regVal <<= 8;
       regVal |= regBuff[1];

       currBusVRCfg = regVal & INA219_CONF_REG_BRNG_BIT_MSK ? \
                      INA219_BUS_VOLTAGE_RANGE_32V: INA219_BUS_VOLTAGE_RANGE_16V;
       if (currBusVRCfg != busVRCfg)
       {
           if (busVRCfg == INA219_BUS_VOLTAGE_RANGE_32V)
               regVal |= INA219_CONF_REG_BRNG_BIT_MSK;
           else
               regVal &= ~INA219_CONF_REG_BRNG_BIT_MSK;

           regBuff[1] = (regVal & 0xff);
           regVal >>= 8;
           regBuff[0] = (regVal & 0xff);

           sts = INA219_writeReg(devCntxt_p, INA219_CONF_REG_ADDR, 2, regBuff);
           if (sts == PLTFRM_STS_SUCCESS)
           {
               devCntxt_p->busVRCfg = busVRCfg;
           }
       }
       else
           devCntxt_p->busVRCfg = currBusVRCfg;
   }

   return sts;
}


// #define INA219_UT_ENA

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
PLTFRM_sts_t INA219_init(UINT8_t devId)
{
   PLTFRM_sts_t sts = PLTFRM_STS_SUCCESS;
   INA219_cntxt_s *devCntxt_p = INA219_getDevCntxt(devId);
   UINT8_t regBuff[INA219_REG_LEN];

   if (devCntxt_p == NULL)
       return PLTFRM_STS_INV_DEV_ID;

   sts = INA219_readReg(devCntxt_p, INA219_CONF_REG_ADDR,
                        INA219_REG_LEN, regBuff);
   if (sts == PLTFRM_STS_SUCCESS)
   {
       UINT16_t regVal = regBuff[0];
       regVal = (regVal << 8) | regBuff[1];

       devCntxt_p->busVRCfg = (regVal & INA219_CONF_REG_BRNG_BIT_MSK) \
                              ? INA219_BUS_VOLTAGE_RANGE_32V : INA219_BUS_VOLTAGE_RANGE_16V;

       devCntxt_p->mode = (regVal & INA219_CONF_REG_MODE_BIT_MSK);

       regVal >>= INA219_CONF_REG_PG_BIT_SHIFT;
       devCntxt_p->shuntVRCfg = regVal & 0x3;
   }
   else
       return sts;

#ifdef INA219_UT_ENA
   for (;;)
   {
      SINT16_t busV, shuntV;

      sts = INA219_readBusVoltage(devId, &busV);
      if (sts != PLTFRM_STS_SUCCESS)
          break;

      sts = INA219_readShuntVoltage(devId, &shuntV);
      if (sts != PLTFRM_STS_SUCCESS)
          break;
   }
#endif
   return sts;
}


#endif
