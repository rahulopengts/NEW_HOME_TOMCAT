/*
 * File Name: util.c
 * Author: ram krishnan (rkris@wisense.in)
 * Created: Sep/8/2013
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

#include <util.h>
#include <string.h>

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t *UTIL_htons(UINT8_t *buff_p, UINT16_t val)
{
   *(buff_p + 1) = val;
   val >>= 8;
   *(buff_p) = val;
   return (buff_p + 2);
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t *UTIL_htole16(UINT8_t *buff_p, UINT16_t val)
{
   *(buff_p) = val;
   val >>= 8;
   *(buff_p + 1) = val;
   return (buff_p + 2);
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t *UTIL_htole32(UINT8_t *buff_p, UINT32_t val)
{
   *(buff_p ++) = val;
   val >>= 8;
   *(buff_p ++) = val;
   val >>= 8;
   *(buff_p ++) = val;
   val >>= 8;
   *(buff_p ++) = val;
   return buff_p;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT32_t UTIL_letoh32(UINT8_t *buff_p)
{
   UINT32_t u32Val = buff_p[3];
   u32Val <<= 8;
   u32Val |= buff_p[2];
   u32Val <<= 8;
   u32Val |= buff_p[1];
   u32Val <<= 8;
   u32Val |= buff_p[0];
   return u32Val;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT16_t UTIL_letoh16(UINT8_t *buff_p)
{
   UINT16_t u16Val = buff_p[1];
   u16Val <<= 8;
   u16Val |= buff_p[0];
   return u16Val;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT16_t UTIL_ntohs(UINT8_t *buff_p)
{
   UINT16_t u16Val = *buff_p;
   u16Val = (u16Val << 8) | buff_p[1];  
   return u16Val;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT32_t UTIL_ntohl(UINT8_t *buff_p)
{
   UINT32_t u32Val = *buff_p;
   u32Val <<= 8;
   u32Val |= buff_p[1];
   u32Val <<= 8;
   u32Val |= buff_p[2];
   u32Val <<= 8;
   u32Val |= buff_p[3];
   return u32Val;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t *UTIL_htonl(UINT8_t *buff_p, UINT32_t val)
{
   *(buff_p + 3) = val;
   val >>= 8;
   *(buff_p + 2) = val;
   val >>= 8;
   *(buff_p + 1) = val;
   val >>= 8;
   *(buff_p) = val;
   return (buff_p + 4);
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t *UTIL_buildTlvHdr(UINT8_t *tlv_p, UINT8_t type, UINT8_t pyldLen)
{
   *(tlv_p ++) = type;
   *(tlv_p ++) = pyldLen;
   return tlv_p;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
UINT8_t UTIL_getTlv(UINT8_t *buff_p, UINT8_t len, UINT8_t type,
                    UINT8_t *pyldLen_p, UINT8_t **pyldBuff_pp)
{
	SINT16_t buffLen = len;
	UINT8_t rc = 0;

    if (buffLen < UTIL_TLV_HDR_SZ)
        return 0;

    // Get the tlv type

    while (buffLen >= UTIL_TLV_HDR_SZ)
    {
        UINT8_t tlvPyldLen = *(buff_p + UTIL_TLV_TYPE_FIELD_SZ);

    	if (tlvPyldLen > (buffLen - UTIL_TLV_HDR_SZ))
    	{
    	    // rkris@wisense.in / march/2/16 - This enclosed TLV extends
    		// beyond end of parent TLV !!
    		rc  = 0;
    		break;
    	}

        if (*buff_p == type)
        {


            *pyldLen_p = tlvPyldLen;
            *pyldBuff_pp = (buff_p + UTIL_TLV_HDR_SZ);
            rc = 1;
            break;
        }
        else
        {
            buff_p += (UTIL_TLV_HDR_SZ + tlvPyldLen);
            buffLen -= (UTIL_TLV_HDR_SZ + tlvPyldLen);
        }
    }

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
void UTIL_initTraceCntxt(UTIL_traceCntxt_s *cntxt_p)
{
   memset(cntxt_p, 0, sizeof(UTIL_traceCntxt_s));
   return;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void UTIL_AddTrace(UTIL_traceCntxt_s *cntxt_p, UINT8_t traceId)
{
   if (cntxt_p->_traceLogInUseCnt == UTIL_TRACE_LOG_ENTRY_MAX_CNT)
   {
       cntxt_p->_traceLogOldest ++;
       if (cntxt_p->_traceLogOldest >= UTIL_TRACE_LOG_ENTRY_MAX_CNT)
           cntxt_p->_traceLogOldest = 0;
   }
   else
   {
       cntxt_p->_traceLogInUseCnt ++;
   }

   cntxt_p->_array[cntxt_p->_traceLogLatest]._traceId = traceId;
   cntxt_p->_traceLogLatest ++;
   if (cntxt_p->_traceLogLatest >= UTIL_TRACE_LOG_ENTRY_MAX_CNT)
       cntxt_p->_traceLogLatest = 0;
   cntxt_p->_traceLogInsertCnt ++;

   return;
}
