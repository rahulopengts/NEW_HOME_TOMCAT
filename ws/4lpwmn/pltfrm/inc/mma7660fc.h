/*
 * File Name : mma7660fc.h
 * Author : ram krishnan (rkris@wisense.in)
 * Created : Feb/19/2014
 *
 *
 * Copyright (c) <2014>, <ram krishnan>
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

#ifndef __MMA7660FC_H__
#define __MMA7660FC_H__

#include <typedefs.h>
#include <i2c_sw.h>

#define MMA7660FC_I2C_ADDR_PREFIX   0x4c    // 0 1001 100  (7 bit address)

/*
 * Table 9 - User Register Summary
 */
#define MMA7660FC_XOUT_REG_ADDR    0x0
#define MMA7660FC_YOUT_REG_ADDR    0x1
#define MMA7660FC_ZOUT_REG_ADDR    0x2
#define MMA7660FC_TILT_REG_ADDR    0x3
#define MMA7660FC_SRST_REG_ADDR    0x4
#define MMA7660FC_SPCNT_REG_ADDR   0x5
#define MMA7660FC_INTSU_REG_ADDR   0x6
#define MMA7660FC_MODE_REG_ADDR    0x7
#define MMA7660FC_SR_REG_ADDR      0x8
#define MMA7660FC_PDET_REG_ADDR    0x9
#define MMA7660FC_PD_REG_ADDR      0xa


// Mode register bit definitions (Read/Write)
#define MMA7660FC_MODE_REG_MODE_BIT  (1 << 0)
#define MMA7660FC_MODE_REG_TON_BIT   (1 << 2)
#define MMA7660FC_MODE_REG_AWE_BIT   (1 << 3)
#define MMA7660FC_MODE_REG_ASE_BIT   (1 << 4)
#define MMA7660FC_MODE_REG_SCPS_BIT  (1 << 5)
#define MMA7660FC_MODE_REG_IPP_BIT   (1 << 6)
#define MMA7660FC_MODE_REG_IAH_BIT   (1 << 7)


// Sample rate register bit definitions (Read/Write)
#define MMA7660FC_SR_REG_AMSR_BM   0x07   // 0 - 2
#define MMA7660FC_SR_REG_AWSR_BM   0x18   // 3 - 4
#define MMA7660FC_SR_REG_FILT_BM   0xe0   // 5 - 7

#define MMA7660FC_SR_REG_AMSR_SHIFT   0x0
#define MMA7660FC_SR_REG_AWSR_SHIFT   0x3
#define MMA7660FC_SR_REG_FILT_SHIFT   0x5

#define MMA7660FC_SR_REG_AMSR_SHIFT_BM   0x7   // 3 bits
#define MMA7660FC_SR_REG_AWSR_SHIFT_BM   0x3   // 2 bits
#define MMA7660FC_SR_REG_FILT_SHIFT_BM   0x7   // 3 bits

#define MMA7660FC_SAMPLE_RATE_AMPD   0x0  // Tap Detection Mode and 120 Samples/Second Active and Auto-Sleep Mode
#define MMA7660FC_SAMPLE_RATE_AM64   0x1  // 64 Samples/Second Active and Auto-Sleep Mode
#define MMA7660FC_SAMPLE_RATE_AM32   0x2  // 32 Samples/Second Active and Auto-Sleep Mode
#define MMA7660FC_SAMPLE_RATE_AM16   0x3  // 16 Samples/Second Active and Auto-Sleep Mode
#define MMA7660FC_SAMPLE_RATE_AM8    0x4  // 8 Samples/Second Active and Auto-Sleep Mode
#define MMA7660FC_SAMPLE_RATE_AM4    0x5  // 4 Samples/Second Active and Auto-Sleep Mode
#define MMA7660FC_SAMPLE_RATE_AM2    0x6  // 2 Samples/Second Active and Auto-Sleep Mode
#define MMA7660FC_SAMPLE_RATE_AM1    0x7  // 1 Sample/Second Active and Auto-Sleep Mode


#define MMA7660FC_SENSOR_TILT_OUTPUT_VALUE_SZ  1

typedef enum
{
	MMA7660FC_OPN_MODE_UNKNOWN,

   /*
    * The Off Mode offers the lowest power consumption, approximately 0.4
    * micro-amps and can only be reached by powering down the analog supply.
    * In this mode, there is no analog supply and all I2C activity is ignored.
    */
   MMA7660FC_OPN_MODE_OFF,
   MMA7660FC_OPN_MODE_STANDBY,

   /*
    * MMA7660FC can be put into Test Mode, which disables accelerometer measurements
    * and instead allows the user to write 6-bit values directly to the three axis
    * data registers, thus simulating real time accelerometer measurements. The state
    * machine will respond to these values according to the enabled features and
    * functions, allowing them to be validated.
    */
   MMA7660FC_OPN_MODE_TEST,

   /*
    * During the Active Mode, continuous measurement on all three axes is enabled.
    * In addition, the user can choose to enable: Shake Detection, Tap Detection,
    * Orientation Detection, and/or Auto-Wake/Sleep Feature and in this mode the
    * digital analysis for any of these functions is done.
    */
   MMA7660FC_OPN_MODE_ACTIVE

} MMA7660FC_opnMode_t;

/*
 * The user can configure the samples per second to any of the following:
 * - 1 sample/second,
 * - 2 samples/second,
 * - 4 samples/second,
 * - 8 samples/second,
 * - 16 samples/second,
 * - 32 samples/second,
 * - 64 samples/second,
 * - and 120 samples/second, for the Auto-Sleep state.
 */
typedef enum
{
	MMA7660FC_AUTO_SLEEP_SAMPLING_RATE_1_HZ,
	MMA7660FC_AUTO_SLEEP_SAMPLING_RATE_2_HZ,
	MMA7660FC_AUTO_SLEEP_SAMPLING_RATE_4_HZ,
	MMA7660FC_AUTO_SLEEP_SAMPLING_RATE_8_HZ,
	MMA7660FC_AUTO_SLEEP_SAMPLING_RATE_16_HZ,
	MMA7660FC_AUTO_SLEEP_SAMPLING_RATE_32_HZ,
	MMA7660FC_AUTO_SLEEP_SAMPLING_RATE_64_HZ,
	MMA7660FC_AUTO_SLEEP_SAMPLING_RATE_120_HZ
} MMA7660FC_autoSleepSamplingRate_t;

typedef enum
{
	MMA7660FC_AUTO_WAKE_SAMPLING_RATE_1_HZ,
	MMA7660FC_AUTO_WAKE_SAMPLING_RATE_8_HZ,
	MMA7660FC_AUTO_WAKE_SAMPLING_RATE_16_HZ,
	MMA7660FC_AUTO_WAKE_SAMPLING_RATE_32_HZ
} MMA7660FC_autoWakeSamplingRate_t;


typedef struct
{
   UINT8_t devId;
   MMA7660FC_opnMode_t  opnMode;
} MMA7660FC_cntxt_s;


extern PLTFRM_sts_t MMA7660FC_init(UINT8_t devId);

extern PLTFRM_sts_t MMA7660FC_getTiltSts(UINT8_t devId,
                                         UINT8_t *tiltSts_p);

#endif
