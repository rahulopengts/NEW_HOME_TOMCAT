/*
 * File Name : sl_spi.c
 * Author :ram krishnan (rkris@wisense.in)
 * Created : June/16/2015
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

#include <cc3100.h>
#include <sl_spi.h>


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
Fd_t CC3100_spiOpen(char *ifName, unsigned long flags)
{
   (void)ifName;
   (void)flags;
   return 1;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
int CC3100_spiClose(Fd_t fd)
{
   (void)fd;
   return 0;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
int __CC3100_spiRead(Fd_t fd, unsigned char *buff_p, int len)
{
   (void)fd;
   return CC3100_spiRead(buff_p, len);
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
int __CC3100_spiWrite(Fd_t fd, unsigned char *buff_p, int len)
{
   (void)fd;
   return CC3100_spiWrite(buff_p, len);
}
