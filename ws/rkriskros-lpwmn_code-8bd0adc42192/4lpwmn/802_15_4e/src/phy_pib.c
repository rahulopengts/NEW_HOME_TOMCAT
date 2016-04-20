/*
 * File Name: phy_pib.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: 8/26/2013
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

#include <string.h>
#include <typedefs.h>
#include <sys_fatalErrs.h>
#include <util.h>
#include <system.h>
#include <phy_defs.h>
#include <phy_pib.h>
#include <radio.h>
#include <nvm.h>

PHY_PIB_s PHY_PIB;

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void PHY_readCfgInfo(void)
{
   UINT8_t tempBd = SYS_allocMem(NVM_PHY_CFG_DATA_LEN), errFlag = 0;

   if (SYS_BD_HNDL_IS_VALID(tempBd))
   {
       PLTFRM_sts_t sts;
       UINT8_t *buff_p = SYS_GET_BD_BUFF(tempBd);

       sts = NVM_readMem(PLTFRM_NVM_DEV_ID,
                         NVM_PHY_CFG_DATA_OFFSET,
                         NVM_PHY_CFG_DATA_LEN,
                         buff_p);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           UINT8_t tlvLen1, rc, *buff1_p;

           rc = UTIL_getTlv(buff_p, NVM_PHY_CFG_DATA_LEN, PHY_TLV_TYPE_CFG_DATA, &tlvLen1, &buff1_p);
           if (rc && (tlvLen1 > 0) && (tlvLen1 <= (NVM_PHY_CFG_DATA_LEN - PHY_TLV_HDR_SZ)))
           {
               do
               {
#ifdef LPWMN_COORD
            	   UINT8_t tlvLen2, *buff2_p;

                   rc = UTIL_getTlv(buff1_p, tlvLen1, PHY_TLV_TYPE_RF_CHANN_ID, &tlvLen2, &buff2_p);
                   if (rc)
                   {
                       if (tlvLen2 == PHY_RF_CHANN_ID_TLV_LEN)
                       {
                           PHY_PIB.phyCurrentChannel = *buff2_p;
                           if (!((PHY_PIB.phyCurrentChannel >= PHY_865_867_MHZ_BAND_CHAN_1)
                                 && (PHY_PIB.phyCurrentChannel <= PHY_865_867_MHZ_BAND_CHAN_9)))
                           {
                               errFlag = 1;
                           }
                       }
                       else
                           errFlag = 1;
                   }
                   else
                       errFlag = 1;

                   if (errFlag)
                       break;
#endif
               } while (0);
           }
           else
               errFlag = 1;
       }
       else
       {
           SYS_fatal(SYS_FATAL_ERR_894);
       }

       if (errFlag)
       {
           UINT8_t *buff1_p = buff_p;
           UINT16_t topTLVLen = 0;

#ifdef LPWMN_COORD
           topTLVLen += (PHY_TLV_HDR_SZ + PHY_RF_CHANN_ID_TLV_LEN);
#endif

           // Initialize the PHY info in the EEPROM
           *(buff1_p ++) = PHY_TLV_TYPE_CFG_DATA;
           *(buff1_p ++) = topTLVLen;
#ifdef LPWMN_COORD
           *(buff1_p ++) = PHY_TLV_TYPE_RF_CHANN_ID;
           *(buff1_p ++) = PHY_RF_CHANN_ID_TLV_LEN;
#ifdef RADIO_CC1101
           *(buff1_p ++) = PHY_865_867_MHZ_BAND_CHAN_1;
#else
#error radio part not specified !!
#endif
#endif
            sts = NVM_writeMem(PLTFRM_NVM_DEV_ID,
                               NVM_PHY_CFG_DATA_OFFSET,
                               buff1_p - buff_p,
                               buff_p);
            if (sts != PLTFRM_STS_SUCCESS)
                SYS_fatal(SYS_FATAL_ERR_895);

            PLTFRM_mcuReset();
       }

       SYS_freeMem(tempBd);
   }
   else
   {
      SYS_fatal(SYS_FATAL_ERR_892);
   }

   return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void PHY_updateCfgInfo(UINT8_t tlvId,
                       UINT8_t tlvValLen,
                       UINT8_t *val_p)
{
   UINT8_t tempBd = SYS_allocMem(NVM_PHY_CFG_DATA_LEN), errFlag = 1;

   switch (tlvId)
   {
      case PHY_TLV_TYPE_RF_CHANN_ID:
           {
              if (tlvValLen == PHY_RF_CHANN_ID_TLV_LEN)
              {
                  if (((*val_p) >= PHY_865_867_MHZ_BAND_CHAN_1)
                      && ((*val_p) <= PHY_865_867_MHZ_BAND_CHAN_9))
                  {
                      errFlag = 0;
                  }
              }
           }
           break;

      default:
           break;
   }

   if (errFlag)
       return;

   if (SYS_BD_HNDL_IS_VALID(tempBd))
   {
       PLTFRM_sts_t sts;
       UINT8_t *buff_p = SYS_GET_BD_BUFF(tempBd);

       sts = NVM_readMem(PLTFRM_NVM_DEV_ID,
                         NVM_PHY_CFG_DATA_OFFSET,
                         NVM_PHY_CFG_DATA_LEN,
                         buff_p);
       if (sts == PLTFRM_STS_SUCCESS)
       {
           UINT8_t tlvLen1, rc, *buff1_p;

           rc = UTIL_getTlv(buff_p, NVM_PHY_CFG_DATA_LEN, PHY_TLV_TYPE_CFG_DATA, &tlvLen1, &buff1_p);
           if (rc)
           {
               UINT8_t tlvLen2, *buff2_p;

               rc = UTIL_getTlv(buff1_p, tlvLen1, PHY_TLV_TYPE_RF_CHANN_ID, &tlvLen2, &buff2_p);
               if (rc)
               {
                   if (tlvLen2 == tlvValLen)
                   {
                       switch (tlvValLen)
                       {
                          case 4:
                              UTIL_htonl(buff2_p, *((UINT32_t *)val_p));
                              break;

                          case 2:
                              UTIL_htons(buff2_p, *((UINT16_t *)val_p));
                              break;

                          case 1:
                              *(buff2_p) = *(val_p);
                              break;

                          default:
                              memcpy(buff2_p, val_p, tlvValLen);
                              break;
                       }

                       sts = NVM_writeMem(PLTFRM_NVM_DEV_ID,
                                          NVM_PHY_CFG_DATA_OFFSET + (buff2_p - buff_p),
                                          tlvValLen, buff2_p);
                       if (sts != PLTFRM_STS_SUCCESS)
                           SYS_fatal(SYS_FATAL_ERR_896);

                       if (tlvId == PHY_TLV_TYPE_RF_CHANN_ID)
                       {
                           PLTFRM_mcuReset();
                       }
                   }
               }
           }
       }

       SYS_freeMem(tempBd);
   }
   else
   {
      SYS_fatal(SYS_FATAL_ERR_897);
   }

   return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void PHY_PIB_init(void)
{
   memset(&PHY_PIB, 0, sizeof(PHY_PIB_s));
   
   // O-QPSK PHY @ 2450 MHz (channels 11 to 26)
#if 0   
   // This implementation support page 0 only 
   PHY_PIB.phyCurrentPage = PHY_2450_MHZ_BAND_PAGE_0;
#endif   

#ifdef RADIO_CC2520
   PHY_PIB.phyCurrentChannel = PHY_2450_MHZ_BAND_PAGE_0_CHAN_18;
#elif defined(RADIO_CC1200)
   PHY_PIB.phyCurrentChannel = PHY_865_MHZ_BAND_CHAN_1;
#elif defined(RADIO_CC1101)
#ifdef RADIO_FREQ_BAND_868_MHZ
   PHY_PIB.phyCurrentChannel = PHY_868_MHZ_BAND_CHAN_1;
#else
#if defined(DEV_TYPE_RFD) || defined(DEV_TYPE_FFD)
   PHY_PIB.phyCurrentChannel = PHY_865_867_MHZ_BAND_CHAN_INV;
#else
   PHY_PIB.phyCurrentChannel = PHY_865_867_MHZ_BAND_CHAN_1;
#endif
#endif
#elif defined(RADIO_CC2500)
   PHY_PIB.phyCurrentChannel = PHY_2400_MHZ_BAND_CHAN_4;
// <TODO>
#else
#error radio part not specified !!
#endif
   

#ifdef RADIO_CC1200
   PHY_PIB.phyTXPower = 14;
   PHY_PIB.phyTXPower *= 10;
   PHY_PIB.phyTXPowerDecPart = 0;
#elif defined (RADIO_CC2520)
   PHY_PIB.phyTXPower = 5;
   PHY_PIB.phyTXPowerDecPart = 0;
#elif defined (RADIO_CC1101)
   /*
    * From Design Note DN025 -
    * JTI Matched Filter Balun (0868BM15C0001) at 868 MHz
    * Balun + LC(filter) + C(blocking cap)
    * Figure 3.3.2.2
    */
   PHY_PIB.phyTXPower = 10;
   PHY_PIB.phyTXPowerDecPart = 80;
#ifdef RFD_NO_RESCAN_ON_REASSOC
   PHY_PIB.workingChannel = PHY_865_867_MHZ_BAND_CHAN_INV;
#endif
#elif defined (RADIO_CC2500)
   PHY_PIB.phyTXPower = 1;
   PHY_PIB.phyTXPowerDecPart = 0;
#ifdef RFD_NO_RESCAN_ON_REASSOC
   PHY_PIB.workingChannel = PHY_2400_MHZ_BAND_CHAN_INV;
#endif
#else
#error Radio not specified !!
#endif

#ifdef LPWMN_COORD
   PHY_readCfgInfo();
#endif


   return;
}


