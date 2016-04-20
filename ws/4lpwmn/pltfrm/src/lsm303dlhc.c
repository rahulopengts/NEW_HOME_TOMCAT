/*
 * File Name: lm75b.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Oct/23/2013
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

#ifdef PLTFRM_LSM303DLHC_ENA


#include <typedefs.h>
#include <pltfrm.h>
#include <system.h>
#include <i2c_sw.h>
#include <lsm303dlhc.h>


static const char _snsrName[] = "LSM303DLHC";
static const char _snsrMfr[] = "ST";

LSM303DLHC_cntxt_s LSM303DLHC_cntxtList[PLTFRM_LSM303DLHC_CNT] =
{
   {PLTFRM_LSM303DLHC_1_DEV_ID, LSM303DLHC_OPN_MODE_UNKNOWN, {_snsrName, _snsrMfr}},
};

/*
 * Data sheet: 
 * http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/DM00027543.pdf
 */


#endif
