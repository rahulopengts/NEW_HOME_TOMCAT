#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <strings.h>
#include <string.h>
#include <wsuart.h>
#include <dis.h>
#include <pltfrm.h>
#include <gw.h>
#include <node_attr.h>
#include <fu.h>


#define RFD_LIST_ONLY  1
#define FFD_LIST_ONLY  2
#define ALL_NODES_LIST  3

unsigned char GW_nodeListReqType = ALL_NODES_LIST;
int GW_processEvt(unsigned char *buff_p, int msgLen);

int verbose = 0;
int myhubverbose	=	1;

#define SER_BUFF_LEN  256

#define LPWMN_EXT_ADDR_LEN  8

FW_segInfo_s FW_segList[FW_MAX_SEG_CNT];

unsigned char serTxBuff[SER_BUFF_LEN];
unsigned char serRxBuff[SER_BUFF_LEN];

int FW_segCnt = 0;

char *LPWMN_macMod[ ] = 
{
  "NONE",
  "APP",
  "AODVL",
  "FWD_PATH",
  "ASSOC_REQ_RELAY",
  "ASSOC_RESP_TO_RELAY",
  "GW"               
};


char *UC_FU_nodeStateStr[ ] =
{
  "IDLE",
  "INIT_RCVD",
  "INIT_RESP_SENT",
  "ABORT_RCVD",
  "RECEIVING_IMAGE",
  "RECEIVED_IMAGE",
  "WRITING_SEG"
};

char *UC_FU_rcStr[ ] = 
{
  "SUCCESS",
  "NO_STORAGE1",
  "INV_REQUEST2",
  "INV_PAGE_SZ3",
  "INV_TOO_MANY_SEGMENTS",
  "SEG_TOO_BIG5",
  "INV_SEG_ADDR6",
  "EEPROM_ACCESS_ERROR",
  "NO_ALTERNATE_IMAGE",
  "MALFORMED_INIT_REQ",
  "WRONG_PAGE_RCVD",
  "SEG_CRC_MISMATCH",
  "NO_STANDBY_IMAGE",
  "STORE_IMAGE_INFO_CORRUPT"
};


float minAdcVal = 1000000;
float maxAdcVal = -1000000;

float __latestVcc;
int __latestVccSet = 0;

static cntxt_s uart_cntxt;

int listTblIdx = 0;
                  
int hdrAcked = 0, 
    allNLEntriesListed = 0,
    allWLEntriesListed = 0,
    allBLEntriesListed = 0;
                

int __calWt1Flag = 0;
int __calWt2Flag = 0;
                        
int __loadCellMVal = 0;
int __loadCellCVal = 0;


int __calSampleCnt = 0;
float __calTotal = 0;

#define POLY 0x1021  // 0x8408
/*
//                                      16   12   5
// this is the CCITT CRC 16 polynomial X  + X  + X  + 1.
// This works out to be 0x1021, but the way the algorithm works
// lets us use 0x8408 (the reverse of the bit pattern).  The high
// bit is always assumed to be set, thus we only use 16 bits to
// represent the 17 bit value.
*/


unsigned char expDisMsgType = 0xff;
unsigned short expDisMsgSrcShortAddr = LPWMN_BROADCAST_SHORT_ADDR;

/*
 * Rcvd message format -
 * Msg type (2 bytes)
 * Flags (1 byte)
 * Seq Nr (1 byte)
 * Pyld len (2 bytes)
 * Hdr crc (2 bytes)
 * Pyld crc (2 bytes)
 * Pyld (N bytes)
 */

#define UART_MSG_HDR_MSG_TYPE_FIELD_LEN  2
#define UART_MSG_HDR_FLAGS_FIELD_LEN     1
#define UART_MSG_HDR_SEQ_NR_FIELD_LEN    1
#define UART_MSG_HDR_PYLD_LEN_FIELD_LEN  2
#define UART_MSG_HDR_HDR_CRC_FIELD_LEN   2
#define UART_MSG_HDR_PYLD_CRC_FIELD_LEN  2


#define UART_MSG_HDR_MSG_TYPE_FIELD_OFF  0
#define UART_MSG_HDR_FLAGS_FIELD_OFF     2
#define UART_MSG_HDR_SEQ_NR_FIELD_OFF    3
#define UART_MSG_HDR_PYLD_LEN_FIELD_OFF  4
#define UART_MSG_HDR_HDR_CRC_FIELD_OFF   6
#define UART_MSG_HDR_PYLD_CRC_FIELD_OFF  8


#define UART_MSG_HDR_LEN (UART_MSG_HDR_PYLD_CRC_FIELD_OFF + UART_MSG_HDR_PYLD_CRC_FIELD_LEN)


#define SER_INPUT_BUFF_LEN  1024

unsigned char serInputBuff[SER_INPUT_BUFF_LEN];

char battV_dsId[ ] = "battVoltage";
char msp430Temp_dsId[ ] = "TempSnsr_msp430";
char tmp102Temp_dsId[ ] = "TempSnsr_tmp102";
char als_dsId[ ] = "LightSensor_tsl45315";
char llSnsr_dsId[ ] = "OvhdTankWL_mp3v5050";
char LoadSnsr_dsId[ ] = "Weight";


unsigned short crc16(unsigned char *buff_p, unsigned int len)
{
   unsigned int ckSum = 0;

   while (len > 1)
   {
      unsigned short tmp = *buff_p;
      tmp = (tmp << 8) | (*(buff_p + 1));
      ckSum = ckSum + tmp;
      buff_p += 2;
      len -= 2;
   }

   if (len > 0)
       ckSum += (*buff_p);

   while (ckSum >> 16)
   {
      ckSum = (ckSum & 0xffff) + (ckSum >> 16);
   }

   return (~ckSum);
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
unsigned char TLV_get(unsigned char *buff_p, unsigned char len, unsigned char type,
                      unsigned char *pyldLen_p, unsigned char **pyldBuff_pp)
{
    int buffLen = len;
    unsigned char rc = 0;

    if (buffLen < DIS_TLV_HDR_SZ){

    	return 0;
    }

    // Get the tlv type

    while (buffLen >= DIS_TLV_HDR_SZ)
    {
    	if(myhubverbose){
    	printf("\n MyHub - TLV_get 56 \n");
    	}
    	unsigned char tlvPyldLen = *(buff_p + DIS_TLV_TYPE_FIELD_LEN);

        if (*buff_p == type)
        {
        	//Sensor Event Into this Block
        	if(myhubverbose){
        		printf("\n MyHub - TLV_get 57 \n");
        	}

            *pyldLen_p = tlvPyldLen;
            *pyldBuff_pp = (buff_p + DIS_TLV_HDR_SZ);
            rc = 1;
            break;
        }
        else
        {

            buff_p += (DIS_TLV_HDR_SZ + tlvPyldLen);
            buffLen -= (DIS_TLV_HDR_SZ + tlvPyldLen);
        }
    }
    if(myhubverbose){
    	printf("\n MyHub - TLV_get 61 \n");
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
void print_usage()
{
    printf("Usage: ./gw.exe <serial-device> <cmd-string>\n");
    printf("On Cywgin, the serial port device is named /dev/ttySX if the COM port is COMY where X = Y - 1 \n");
    printf("Example: ./gw.exe /dev/ttyS20 nc \n");
    printf("For list of command strings: ./gw help or ./gw ? \n");
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
unsigned short __ntohs(unsigned char *buff_p)
{
   short u16Val = *buff_p;
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
void __htonl(unsigned char *buff_p, unsigned int val)
{
   buff_p[0] = (val >> 24) & 0xff;
   buff_p[1] = (val >> 16) & 0xff;
   buff_p[2] = (val >> 8) & 0xff;
   buff_p[3] = (val) & 0xff;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void __htons(unsigned char *buff_p, unsigned short val)
{
   buff_p[0] = (val >> 8) & 0xff;
   buff_p[1] = (val) & 0xff;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
unsigned int __ntohl(unsigned char *buff_p)
{
   unsigned int u32Val = *buff_p;
   u32Val <<= 8;
   u32Val |= buff_p[1];
   u32Val <<= 8;
   u32Val |= buff_p[2];
   u32Val <<= 8;
   u32Val |= buff_p[3];
   return u32Val;
}


#ifdef __CYGWIN__
    

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
int cfgPort(char *serDevName_p, int baudRate)
{
    struct termios newtio;
    struct termios oldtio;
    struct termios latesttio;
    cntxt_s *serialCntxt_p = &uart_cntxt;
    int rc;

    serialCntxt_p->serialFd = open(serDevName_p, O_RDWR | O_NOCTTY );
    if (serialCntxt_p->serialFd < 0)
    {
        printf("Failed to open serial device <%s> - errno<%d> !!\n",
               serDevName_p, errno);  
        return -1;
    }

    // printf("Opened serial device <%s> \n", serDevName_p); 
 
    rc = tcgetattr(serialCntxt_p->serialFd, &oldtio); /* save current port settings */
    if (rc < 0)
    {
        printf("\n tcgetattr() failed !! - rc<%d>, errno<%d> \n", rc, errno);
        return -1;
    }

    bzero(&newtio, sizeof(newtio));

    rc = cfsetspeed(&newtio, baudRate);
    if (rc < 0)
    {
        printf("\n cfsetspeed() failed !! - rc<%d>, errno<%d> \n", rc, errno);
        return -1;
    }

    newtio.c_cflag = CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    // if ((rc = fcntl(serialCntxt_p->serialFd, F_SETOWN, getpid())) < 0)
    // {
    //     printf("\n fcntl failed !! - rc<%d>, errno<%d> \n", rc, errno);
    //     return -1;
    // }

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 10 chars received */

    rc = tcsetattr(serialCntxt_p->serialFd, TCSANOW, &newtio);
    if (rc < 0)
    {
        printf("\n tcsetattr() failed !! - rc<%d> / errno<%d> \n", rc, errno);
        return -1;
    }

    rc = tcflush(serialCntxt_p->serialFd, TCIFLUSH);
    if (rc < 0)
    {
        printf("\n tcflush() failed !! - rc<%d> \n", rc);
        return -1;
    }
    
    tcgetattr(serialCntxt_p->serialFd, &latesttio); 
    if (rc < 0)
    {
        printf("\n tcgetattr() failed !! - rc<%d> \n", rc);
        return -1;
    }

    // printf("\nispeed<%d> / ospeed<%d> \n", latesttio.c_ispeed, latesttio.c_ospeed);
    // printf("\niflag<0x%x>/oflag<0x%x>/cflag<0x%x> \n", latesttio.c_iflag, latesttio.c_oflag, latesttio.c_cflag);

    return 1;
}



#else 
                             
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
int cfgPort(char *serDevName_p, int baudRate)
{
   cntxt_s *serialCntxt_p = &uart_cntxt;
   int rc;

   memset(serialCntxt_p, 0, sizeof(cntxt_s));

   serialCntxt_p->serialFd = open((char *)serDevName_p, O_RDWR | O_NOCTTY | O_NDELAY);
   if (serialCntxt_p->serialFd < 0)
   {
       printf("\n<%s> open(%s) failed !! - errno<%d> \n",
              __FUNCTION__, serDevName_p, errno);
       return -1;
   }
   
   // Zero out port status flags
   if (fcntl(serialCntxt_p->serialFd, F_SETFL, 0) != 0x0)
   {
       return -1;
   }

   bzero(&(serialCntxt_p->dcb), sizeof(serialCntxt_p->dcb));

   // serialCntxt_p->dcb.c_cflag |= serialCntxt_p->baudRate;  // Set baud rate first time
   serialCntxt_p->dcb.c_cflag |= baudRate;  // Set baud rate first time
   serialCntxt_p->dcb.c_cflag |= CLOCAL;  // local - don't change owner of port
   serialCntxt_p->dcb.c_cflag |= CREAD;  // enable receiver

   // Set to 8N1
   serialCntxt_p->dcb.c_cflag &= ~PARENB;  // no parity bit
   serialCntxt_p->dcb.c_cflag &= ~CSTOPB;  // 1 stop bit
   serialCntxt_p->dcb.c_cflag &= ~CSIZE;  // mask character size bits
   serialCntxt_p->dcb.c_cflag |= CS8;  // 8 data bits

   // Set output mode to 0
   serialCntxt_p->dcb.c_oflag = 0;
 
   serialCntxt_p->dcb.c_lflag &= ~ICANON;  // disable canonical mode
   serialCntxt_p->dcb.c_lflag &= ~ECHO;  // disable echoing of input characters
   serialCntxt_p->dcb.c_lflag &= ~ECHOE;
 
   // Set baud rate
   serialCntxt_p->baudRate = baudRate;  
   cfsetispeed(&serialCntxt_p->dcb, serialCntxt_p->baudRate);
   cfsetospeed(&serialCntxt_p->dcb, serialCntxt_p->baudRate);

   serialCntxt_p->dcb.c_cc[VTIME] = 0;  // timeout = 0.1 sec
   serialCntxt_p->dcb.c_cc[VMIN] = 1;
 
   if ((tcsetattr(serialCntxt_p->serialFd, TCSANOW, &(serialCntxt_p->dcb))) != 0)
   {
       printf("\ntcsetattr(%s) failed !! - errno<%d> \n",
              serDevName_p, errno);
       close(serialCntxt_p->serialFd);
       return -1;
   }

   // flush received data
   tcflush(serialCntxt_p->serialFd, TCIFLUSH);
   tcflush(serialCntxt_p->serialFd, TCOFLUSH);

   return 1;
}

#endif




/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
int readPort(unsigned char *buff_p, unsigned int len)
{
   int rdLen, readLeft = len, totRead = 0;

   while (readLeft > 0)
   {
      rdLen = read(uart_cntxt.serialFd, buff_p + totRead, readLeft);
      // printf("\n<%s> rdLen<%d> \n", __FUNCTION__, rdLen);
      if (rdLen > 0)
      {
          totRead += rdLen;
          readLeft -= rdLen;
      }
      else
      {
          printf("\n<%s> read() failed  - %d !! \n", __FUNCTION__, rdLen);
          return rdLen;
      }
   }

   return totRead;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
int readPortTmo(unsigned char *buff_p, unsigned int len, int timeOutInMilliSecs)
{
   int rc;
   fd_set set;
   struct timeval timeout;

   FD_ZERO(&set); /* clear the set */
   FD_SET(uart_cntxt.serialFd, &set); /* add our file descriptor to the set */

   timeout.tv_sec = (timeOutInMilliSecs / 1000);
   timeout.tv_usec = (timeOutInMilliSecs % 1000) * 1000;

   rc = select(uart_cntxt.serialFd + 1, &set, NULL, NULL, &timeout);
   if (rc == -1)
   {
       printf("\n<%s> select() failed - errno<%d> !! \n", __FUNCTION__, errno);
   }
   else
   {
       // if rc is 0, select has timed out !!
       if (rc > 0)
       {
          rc = readPort(buff_p, len);
          return rc;
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
int writePort(unsigned char *buff_p, unsigned int cnt)
{
   int rc, bytesLeft = cnt, bytesWritten = 0;
    
   // printf("\n<%s> cnt<%d> \n", __FUNCTION__, cnt);
   
   while (bytesLeft > 0)
   {
      rc = write(uart_cntxt.serialFd, buff_p + bytesWritten, bytesLeft);
      if (rc <= 0)
          return -1;
      else
      {
          bytesLeft -= rc;
          bytesWritten += rc;
      }
   }

   return 1;
}
   

UC_FU_cntxt_s  UC_FU_cntxt;
                
                             

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
int UC_FU_buildSendImagePage(void)
{
   int off, rc, pyldLen, pageSz;
   unsigned char *pyld_p;
   FW_segInfo_s *seg_p = UC_FU_cntxt.segList + UC_FU_cntxt.currSegIdx;

   printf("<%s> segIdx<%u/%u> / pageIdx<%u/%u> \n",
          __FUNCTION__, 
          UC_FU_cntxt.currSegIdx, UC_FU_cntxt.segCnt,
          UC_FU_cntxt.currPageIdx, seg_p->pageCnt);

   if (UC_FU_cntxt.currPageIdx < (seg_p->pageCnt - 1))
       pageSz = FW_UPG_DFLT_PAGE_SZ;
   else
   {
       pageSz = seg_p->lastPageSz;
   }

   /*
    * First two bytes have the page index and segment index
    * Rest of the bytes contain "page sz" number of bytes of
    * page data (except for the last page in a segment).
    */
   pyldLen = LPWMN_MAC_SHORT_ADDR_LEN 
             + DIS_MSG_TYPE_SZ
             + FU_PAGE_MSG_SEG_IDX_PAGE_IDX_FIELD_LEN
             + pageSz;

   pyld_p = (unsigned char *)malloc(pyldLen);
   if (pyld_p == NULL)
   {
       printf("malloc(%d) failed !! \n", pyldLen);
       UC_FU_cntxt.nextEvt = UC_FU_EVT_SYS_ERR;
       return 255;
   }

   __htons(pyld_p, UC_FU_cntxt.shortAddr);
   pyld_p[LPWMN_MAC_SHORT_ADDR_LEN] = DIS_MSG_TYPE_UC_FU_IMAGE_PAGE;
   off = LPWMN_MAC_SHORT_ADDR_LEN + DIS_MSG_TYPE_SZ;
  
   {
      unsigned int u16Idx, segOff;
  
      u16Idx = UC_FU_cntxt.currSegIdx;
      u16Idx <<= UC_FU_PAGE_MSG_SEG_IDX_FIELD_SHIFT;
      u16Idx |= UC_FU_cntxt.currPageIdx;

      __htons(pyld_p + off, u16Idx);

      off += FU_PAGE_MSG_SEG_IDX_PAGE_IDX_FIELD_LEN;

      memcpy(pyld_p + off, 
             seg_p->segBuff_p + (UC_FU_cntxt.currPageIdx * FW_UPG_DFLT_PAGE_SZ),
             pageSz);
      
      off += pageSz;
   } 

   if (off != pyldLen)
   {
       printf("\n off<%d> does not match pyldLen<%d> \n", off, pyldLen);
       exit(1);
   }

   do
   {
       rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_RELAY_TO_NODE, 
                            pyld_p, pyldLen);
       if (rc != 1)
           return rc;

       rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
       if (rc != 0)
           return rc;

       if (hdrAcked == 0x0)
       {
           printf("<%s> Header not acked !! \n", __FUNCTION__);
           // UC_FU_cntxt.nextEvt = UC_FU_EVT_GW_COMM_ERR;
           // return 255;
           return 1;
       }
      
       if (verbose)
           printf("Header acked \n");
   
       // Send payload
       rc = writePort(pyld_p, pyldLen);
       if (rc != 1)
       {
           printf("<%s> writePort(%d) failed !! \n", 
                  __FUNCTION__, pyldLen);
           UC_FU_cntxt.nextEvt = UC_FU_EVT_GW_COMM_ERR;
           return 255;
       }
       else
           printf("<%s> Send payload of length <%u> to node<%u> \n",
                  __FUNCTION__, pyldLen, UC_FU_cntxt.shortAddr);
   } while (0);

   return 1;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
int UC_FU_buildSendInit1Msg(void)
{
   int idx, rc, pyldLen, segInfoLen;
   unsigned char *pyld_p;
   int off = 0, segCnt = UC_FU_cntxt.segCnt;
   time_t currTime;

   UC_FU_cntxt.txTryCnt = 0;

   pyldLen = LPWMN_MAC_SHORT_ADDR_LEN 
             + DIS_MSG_TYPE_SZ
             + DIS_TLV_HDR_SZ    // DIS_TLV_TYPE_UC_FU_SESSION_PARAMS
             + DIS_FU_PAGE_SZ_TLV_SZ  
             + DIS_TIME_STAMP_TLV_SZ
             + DIS_TLV_HDR_SZ;   // DIS_TLV_TYPE_FW_SEG_LIST
  
   segInfoLen = DIS_TLV_HDR_SZ // DIS_TLV_TYPE_FW_SEG_INFO
                + DIS_FW_SEG_START_TLV_SZ;            // DIS_TLV_TYPE_FW_SEG_START

   pyldLen += (segInfoLen * segCnt);

   printf("<%s> msg pyld len <%u> \n", __FUNCTION__, pyldLen);

   pyld_p = (unsigned char *)malloc(pyldLen);
   if (pyld_p == NULL)
   {
       printf("malloc(%d) failed !! \n", pyldLen);
       UC_FU_cntxt.nextEvt = UC_FU_EVT_SYS_ERR;
       return 255;
   }

   __htons(pyld_p, UC_FU_cntxt.shortAddr);
   pyld_p[LPWMN_MAC_SHORT_ADDR_LEN] = DIS_MSG_TYPE_UC_FU_INIT_1;
   off = LPWMN_MAC_SHORT_ADDR_LEN + DIS_MSG_TYPE_SZ;

   pyld_p[off ++] = DIS_TLV_TYPE_UC_FU_SESSION_PARAMS;
   pyld_p[off ++] = DIS_FU_PAGE_SZ_TLV_SZ
                    + DIS_TLV_HDR_SZ
                    + (segInfoLen * segCnt);

   pyld_p[off ++] = DIS_TLV_TYPE_FU_PAGE_SZ;
   pyld_p[off ++] = 1;
   pyld_p[off ++] = FW_UPG_DFLT_PAGE_SZ;
   
   
   time(&currTime);
   
   pyld_p[off ++] = DIS_TLV_TYPE_TIME_STAMP;
   pyld_p[off ++] = DIS_TIME_STAMP_TLV_VAL_LEN;
   __htonl(pyld_p + off, *((unsigned int *)&currTime));
   printf("\n %u 0x%x 0x%x 0x%x 0x%x \n", 
          (unsigned int)currTime, pyld_p[off], pyld_p[off + 1], pyld_p[off + 2], pyld_p[off + 3]);
   off += DIS_TIME_STAMP_TLV_VAL_LEN;
   
   pyld_p[off ++] = DIS_TLV_TYPE_FW_SEG_LIST;
   pyld_p[off ++] = (segInfoLen * segCnt);

   for (idx=0; idx<FW_segCnt; idx++)
   {
      pyld_p[off ++] = DIS_TLV_TYPE_FW_SEG_INFO;
      pyld_p[off ++] = segInfoLen - DIS_TLV_HDR_SZ;

      pyld_p[off ++] = DIS_TLV_TYPE_FW_SEG_START;
      pyld_p[off ++] = 2;
      __htons(pyld_p + off, UC_FU_cntxt.segList[idx].segStartAddr);
      off += 2;
   }

   if (off != pyldLen)
   {
       printf("\n off<%d> does not match pyldLen<%d> \n", off, pyldLen);
       exit(1);
   }

   do
   {
       rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_RELAY_TO_NODE, 
                            pyld_p, pyldLen);
       if (rc != 1)
           return rc;

       rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
       if (rc != 0)
           return rc;

       if (hdrAcked == 0x0)
       {
           printf("<%s> Header not acked !! \n", __FUNCTION__);
           // UC_FU_cntxt.nextEvt = UC_FU_EVT_GW_COMM_ERR;
           // return 255;
           sleep(2);
           UC_FU_cntxt.txTryCnt ++;
           return 1;
       }
      
       if (verbose)
           printf("Header acked \n");
   
       // Send payload
       rc = writePort(pyld_p, pyldLen);
       if (rc != 1)
       {
           printf("<%s> writePort(%d) failed !! \n", 
                  __FUNCTION__, pyldLen);
           return 255;
       }

       printf("<%s> Request sent ..... \n", __FUNCTION__);

       printf("<%s> Request sent. Waiting for response ..... \n", __FUNCTION__);
                      
       expDisMsgType = DIS_MSG_TYPE_NODE_UC_FU_MODULE_INFO;
       expDisMsgSrcShortAddr = UC_FU_cntxt.shortAddr;
   
       rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_RELAY_FROM_NODE, 5);
       if (rc == 0)
       { 
           rc = 1;
           break;
       }
       else
       {
           // timed out - no response !!

           printf("<%s> Timed out .... <%u/%u>\n", 
                  __FUNCTION__, UC_FU_cntxt.txTryCnt, FW_UPG_MAX_TX_ATTEMPT_CNT);

           UC_FU_cntxt.txTryCnt ++;

           if (UC_FU_cntxt.txTryCnt >= FW_UPG_MAX_TX_ATTEMPT_CNT)
           {
               printf("<%s> No response to UC_FU message from node <%05u> - Giving Up !! \n",
                      __FUNCTION__, UC_FU_cntxt.shortAddr);
               UC_FU_cntxt.nextEvt = UC_FU_EVT_NO_RESP_FROM_NODE;
               rc = 1;
               break;
           }
           else
           {
               printf("<%s> No response to UC_FU message from node <%05u> - retrying <%u/%u> !! \n",
                      __FUNCTION__, UC_FU_cntxt.shortAddr, 
                      UC_FU_cntxt.txTryCnt + 1, FW_UPG_MAX_TX_ATTEMPT_CNT);
           }
       }
   } while (1);

   return rc;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
int UC_FU_buildSendInit2Msg(void)
{
   int idx, rc, pyldLen, segInfoLen;
   unsigned char *pyld_p;
   int off = 0, segCnt = UC_FU_cntxt.segCnt;

   UC_FU_cntxt.txTryCnt = 0;

   pyldLen = LPWMN_MAC_SHORT_ADDR_LEN 
             + DIS_MSG_TYPE_SZ
             + DIS_TLV_HDR_SZ    // DIS_TLV_TYPE_UC_FU_SESSION_PARAMS
             + DIS_TLV_HDR_SZ;   // DIS_TLV_TYPE_FW_SEG_LIST
  
   segInfoLen = DIS_TLV_HDR_SZ // DIS_TLV_TYPE_FW_SEG_INFO
                + DIS_FW_SEG_LEN_TLV_SZ              // DIS_TLV_TYPE_FW_SEG_LEN          
                + DIS_FW_SEG_CRC_TLV_SZ;             // DIS_TLV_TYPE_FW_SEG_CRC

   pyldLen += (segInfoLen * segCnt);

   printf("<%s> msg pyld len <%u> \n", __FUNCTION__, pyldLen);

   pyld_p = (unsigned char *)malloc(pyldLen);
   if (pyld_p == NULL)
   {
       printf("malloc(%d) failed !! \n", pyldLen);
       UC_FU_cntxt.nextEvt = UC_FU_EVT_SYS_ERR;
       return 255;
   }

   __htons(pyld_p, UC_FU_cntxt.shortAddr);
   pyld_p[LPWMN_MAC_SHORT_ADDR_LEN] = DIS_MSG_TYPE_UC_FU_INIT_2;
   off = LPWMN_MAC_SHORT_ADDR_LEN + DIS_MSG_TYPE_SZ;

   pyld_p[off ++] = DIS_TLV_TYPE_UC_FU_SESSION_PARAMS;
   pyld_p[off ++] = DIS_TLV_HDR_SZ
                    + (segInfoLen * segCnt);
   
   pyld_p[off ++] = DIS_TLV_TYPE_FW_SEG_LIST;
   pyld_p[off ++] = (segInfoLen * segCnt);

   for (idx=0; idx<FW_segCnt; idx++)
   {
      pyld_p[off ++] = DIS_TLV_TYPE_FW_SEG_INFO;
      pyld_p[off ++] = segInfoLen - DIS_TLV_HDR_SZ;

      pyld_p[off ++] = DIS_TLV_TYPE_FW_SEG_LEN;
      pyld_p[off ++] = 2;
      __htons(pyld_p + off, UC_FU_cntxt.segList[idx].segLen);
      off += 2;

      pyld_p[off ++] = DIS_TLV_TYPE_FW_SEG_CRC;
      pyld_p[off ++] = 2;
      __htons(pyld_p + off, UC_FU_cntxt.segList[idx].segCRC);
      off += 2;
   }

   if (off != pyldLen)
   {
       printf("\n off<%d> does not match pyldLen<%d> \n", off, pyldLen);
       exit(1);
   }

   do
   {
       rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_RELAY_TO_NODE, 
                            pyld_p, pyldLen);
       if (rc != 1)
           return rc;

       rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
       if (rc != 0)
           return rc;

       if (hdrAcked == 0x0)
       {
           printf("<%s> Header not acked !! \n", __FUNCTION__);
           // UC_FU_cntxt.nextEvt = UC_FU_EVT_GW_COMM_ERR;
           // return 255;
           sleep(2);
           UC_FU_cntxt.txTryCnt ++;
           return 1;
       }
      
       if (verbose)
           printf("Header acked \n");
   
       // Send payload
       rc = writePort(pyld_p, pyldLen);
       if (rc != 1)
       {
           printf("<%s> writePort(%d) failed !! \n", 
                  __FUNCTION__, pyldLen);
           return 255;
       }

       printf("<%s> Request sent ..... \n", __FUNCTION__);

       printf("<%s> Request sent. Waiting for response ..... \n", __FUNCTION__);
                      
       expDisMsgType = DIS_MSG_TYPE_NODE_UC_FU_MODULE_INFO;
       expDisMsgSrcShortAddr = UC_FU_cntxt.shortAddr;
   
       rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_RELAY_FROM_NODE, 5);
       if (rc == 0)
       { 
           rc = 1;
           break;
       }
       else
       {
           // timed out - no response !!

           printf("<%s> Timed out .... <%u/%u>\n", 
                  __FUNCTION__, UC_FU_cntxt.txTryCnt, FW_UPG_MAX_TX_ATTEMPT_CNT);

           UC_FU_cntxt.txTryCnt ++;

           if (UC_FU_cntxt.txTryCnt >= FW_UPG_MAX_TX_ATTEMPT_CNT)
           {
               printf("<%s> No response to UC_FU message from node <%05u> - Giving Up !! \n",
                      __FUNCTION__, UC_FU_cntxt.shortAddr);
               UC_FU_cntxt.nextEvt = UC_FU_EVT_NO_RESP_FROM_NODE;
               rc = 1;
               break;
           }
           else
           {
               printf("<%s> No response to UC_FU message from node <%05u> - retrying <%u/%u> !! \n",
                      __FUNCTION__, UC_FU_cntxt.shortAddr, UC_FU_cntxt.txTryCnt + 1, FW_UPG_MAX_TX_ATTEMPT_CNT);
           }
       }
   } while (1);

   return rc;
}



/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void UC_FU_sendNextPage(void)
{
   printf("------------------------------------------------------------------- \n");
   printf("<%s> Entry ... \n", __FUNCTION__);

   if (UC_FU_buildSendImagePage() == 1)
   {
       int rc, respTmo = UC_FU_PAGE_RESP_DFLT_TMO_SECS;

       // Page Sent ...
       
       expDisMsgType = DIS_MSG_TYPE_NODE_UC_FU_MODULE_INFO;
       expDisMsgSrcShortAddr = UC_FU_cntxt.shortAddr;

       UC_FU_cntxt.currState = UC_FU_STATE_IMAGE_PAGE_SENT;
   
       if (UC_FU_cntxt.currPageIdx == UC_FU_cntxt.segList[UC_FU_cntxt.currSegIdx].pageCnt - 1)
       { 
           respTmo = 60;
           printf("<%s> will wait for %u seconds for response \n", 
                  __FUNCTION__, respTmo);
       }
                                  
       rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_RELAY_FROM_NODE, respTmo);
       if (rc > 0)
       {
           printf("<%s> No response from node - rc<%d> !! \n", __FUNCTION__, rc);
           if (rc == 100)
               UC_FU_cntxt.nextEvt = UC_FU_EVT_NO_RESP_FROM_NODE;
       }
       else
       {
           printf("<%s> response received from node \n", __FUNCTION__);
           UC_FU_cntxt.nextEvt = UC_FU_EVT_RCVD_NODE_FU_MOD_STS;
       }
   }
   else
       UC_FU_cntxt.currState = UC_FU_STATE_ERROR;
   
   printf("<%s> Exit ... \n", __FUNCTION__);

   return;
}


/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
void UC_FU_evtProc(UC_FU_cntxt_s *cntxt_p,
                   UC_FU_evt_t event)
{
   switch (event)
   {
       case UC_FU_EVT_START:
       case UC_FU_EVT_NODE_REG_CHECK_SUCCESSFUL:
            {
                // Send FU_INIT message to the node
                UC_FU_cntxt.currState = UC_FU_STATE_WAIT_INIT_1_RESP;
                UC_FU_buildSendInit1Msg();
                break;
            }
            break;
                                     
      case UC_FU_EVT_NO_RESP_FROM_NODE:
           {
              switch (UC_FU_cntxt.currState)
              {
                 case UC_FU_STATE_IMAGE_PAGE_SENT:
                      {
                          printf("<%s> Timed out .... <%u/%u>\n", 
                                 __FUNCTION__, UC_FU_cntxt.txTryCnt, FW_UPG_MAX_TX_ATTEMPT_CNT);

                          UC_FU_cntxt.txTryCnt ++;

                          if (UC_FU_cntxt.txTryCnt >= FW_UPG_MAX_TX_ATTEMPT_CNT)
                          {
                              printf("<%s> No response to PAGE message from node <%05u> - Giving Up !! \n",
                                     __FUNCTION__, UC_FU_cntxt.shortAddr);
                              UC_FU_cntxt.txTryCnt = 0;
                              UC_FU_cntxt.currState = UC_FU_STATE_ERROR;
                          }
                          else
                          {
                              printf("<%s> No response to PAGE message from node <%05u> - retrying <%u/%u> !! \n",
                                     __FUNCTION__, UC_FU_cntxt.shortAddr, UC_FU_cntxt.txTryCnt + 1, 
                                     FW_UPG_MAX_TX_ATTEMPT_CNT);
                              UC_FU_sendNextPage();
                          }
                      }
                      break;

                 default:
                      {
                      }
                      break;
              }
           }
           break;

      case UC_FU_EVT_RCVD_NODE_FU_MOD_STS:
           {
              switch (UC_FU_cntxt.currState)
              {
                 case UC_FU_STATE_IMAGE_PAGE_SENT:
                      {
                         FW_segInfo_s *seg_p = UC_FU_cntxt.segList + UC_FU_cntxt.currSegIdx;

                         printf("<%s> rcvd response from node<%u> - nodeState<%u>/rc<%u>/segIdx<%u>/pageIdx<%u> \n",
                                __FUNCTION__,
                                UC_FU_cntxt.shortAddr,
                                UC_FU_cntxt.lastResp.nodeFUState,
                                UC_FU_cntxt.lastResp.retCode,
                                UC_FU_cntxt.lastResp.segIdx,
                                UC_FU_cntxt.lastResp.pageIdx);

                         if (UC_FU_cntxt.lastResp.nodeFUState == UC_FU_NODE_STATE_RECEIVING_IMAGE
                             || UC_FU_cntxt.lastResp.nodeFUState == UC_FU_NODE_STATE_RECEIVED_IMAGE)
                         {
                             FW_segInfo_s *seg_p = UC_FU_cntxt.segList + UC_FU_cntxt.currSegIdx;
                             int expSegIdx = UC_FU_cntxt.currSegIdx;
                             int expPageIdx;
                                 
                             if (UC_FU_cntxt.currPageIdx == seg_p->pageCnt - 1)
                             {
                                 expSegIdx ++;
                                 expPageIdx = 0;
                             }
                             else
                             {
                                 expPageIdx = UC_FU_cntxt.currPageIdx + 1;
                             }

                             if (UC_FU_cntxt.lastResp.nodeFUState == UC_FU_NODE_STATE_RECEIVING_IMAGE)
                             {
                                 printf("<%s> seg<%u/%u> page<%u/%u> \n",
                                        __FUNCTION__, UC_FU_cntxt.lastResp.segIdx, expSegIdx,
                                        UC_FU_cntxt.lastResp.pageIdx, expPageIdx );

                                 if (expSegIdx == UC_FU_cntxt.lastResp.segIdx
                                     && expPageIdx == UC_FU_cntxt.lastResp.pageIdx)
                                 {
                                     unsigned char *pageS_p = seg_p->segBuff_p 
                                                              + (UC_FU_cntxt.currPageIdx*FW_UPG_DFLT_PAGE_SZ);
                                     unsigned int calcGpCRC;
                                     int dataLen = FW_UPG_DFLT_PAGE_SZ;

                                     // Check the CRC of the last page
                                     if (UC_FU_cntxt.currPageIdx >= (seg_p->pageCnt - 1))
                                         dataLen = seg_p->lastPageSz;
                                     calcGpCRC = crc16(pageS_p, dataLen);

                                     printf("<%s> calc page crc<0x%x> [%p/%u] / rcvd page crc<0x%x> \n",
                                            __FUNCTION__, calcGpCRC, pageS_p, dataLen,
                                            UC_FU_cntxt.lastResp.pageCRC);
                                                
                                     if (UC_FU_cntxt.lastResp.pageCRC == calcGpCRC)
                                     {
                                         printf("<%s> page CRC matches ... \n", __FUNCTION__);
                                     }
                                     else
                                     {
                                         printf("<%s> page CRC mismatch !! \n", __FUNCTION__);
                                         UC_FU_cntxt.crcMismatchCnt ++;
                                         if (UC_FU_cntxt.crcMismatchCnt >= FW_UPG_MAX_CRC_MISMATCH_CNT)
                                         {
                                             printf("<%s> max crc mismatch allowed reached - upgrade aborted !! \n", __FUNCTION__);
                                             UC_FU_cntxt.currState = UC_FU_STATE_ERROR;
                                             break;
                                         }
                                         else
                                         {
                                             printf("<%s> crc mismatch <%u/%u> - resending same page \n",
                                                    __FUNCTION__, UC_FU_cntxt.crcMismatchCnt, FW_UPG_MAX_CRC_MISMATCH_CNT);
                                             UC_FU_sendNextPage();
                                             break;
                                         }
                                     }
                                 }
                                 else
                                 {
                                     UC_FU_cntxt.currState = UC_FU_STATE_ERROR;
                                     break;
                                 }
                             }
                             else
                             {
                                 // Node is indicating that it has received image ...

                                 printf("<%s> expSegIdx<%u/%u> \n",
                                        __FUNCTION__, expSegIdx, UC_FU_cntxt.segCnt);
                                      
                                 if (expSegIdx == UC_FU_cntxt.segCnt + 1)
                                 {
                                     printf("<%s> Node has received the complete image ... \n", __FUNCTION__);
                                     UC_FU_cntxt.currState = UC_FU_STATE_DONE;
                                 }
                                 else
                                     UC_FU_cntxt.currState = UC_FU_STATE_ERROR;

                                 break;
                             }
                         }

                         UC_FU_cntxt.txTryCnt = 0;
                         UC_FU_cntxt.currPageIdx ++;

                         if (UC_FU_cntxt.currPageIdx >= seg_p->pageCnt)
                         {
                             UC_FU_cntxt.currPageIdx = 0;
                             UC_FU_cntxt.currSegIdx ++;
                         }

                         // sleep(1);
                                         
                         UC_FU_cntxt.crcMismatchCnt = 0;
                         UC_FU_sendNextPage();
                      }
                      break;
                 
                 case UC_FU_STATE_WAIT_INIT_1_RESP:
                      {
                         UC_FU_cntxt.currState = UC_FU_STATE_WAIT_INIT_2_RESP;
                         UC_FU_buildSendInit2Msg();
                         break;
                      }
                      break;

                 case UC_FU_STATE_WAIT_INIT_2_RESP:
                      {
                         printf("<%s> Received response to INIT request ... \n", __FUNCTION__);

                         if (verbose)
                         {
                             printf("Node-FU-State <%u> \n", UC_FU_cntxt.lastResp.nodeFUState);
                             printf("Error Code <%u> \n", UC_FU_cntxt.lastResp.retCode);
                         }

                         if (UC_FU_cntxt.lastResp.nodeFUState != UC_FU_NODE_STATE_IDLE)
                         {
                             if (verbose)
                             {
                                 printf("segIdx <%u> \n", UC_FU_cntxt.lastResp.segIdx);
                                 printf("pageIdx <%u> \n", UC_FU_cntxt.lastResp.pageIdx);
                             }

                             if (UC_FU_cntxt.lastResp.segIdx != 0x0
                                 || UC_FU_cntxt.lastResp.pageIdx != 0x0)
                             {
                                 UC_FU_cntxt.currState = UC_FU_STATE_ERROR;
                                 break;
                             }

                             UC_FU_cntxt.currSegIdx = 0;
                             UC_FU_cntxt.currPageIdx = 0;
                             UC_FU_cntxt.txTryCnt = 0;
                             UC_FU_cntxt.crcMismatchCnt = 0;

                             UC_FU_sendNextPage();
                         }
                      }
                      break;

                 default:
                      {
                      }
                      break;
              }
           }
           break;

#if 0
       case UC_FU_EVT_START:
            {
                if (cntxt_p->currState == UC_FU_STATE_INIT)
                {
                    // Check node is registered or not 
                    cntxt_p->currState = UC_FU_STATE_CHECK_NODE_REGN;
                }
            }
            break;
#endif

       default:
            {
            }
            break;
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
int GW_startUcProc(unsigned short shortAddr)
{
   unsigned char evt;

   UC_FU_cntxt.shortAddr = shortAddr;
   UC_FU_cntxt.currState = UC_FU_STATE_INIT;
   UC_FU_cntxt.segCnt = FW_segCnt;
   memcpy(UC_FU_cntxt.segList, FW_segList, sizeof(FW_segList));
     
   UC_FU_cntxt.nextEvt = UC_FU_EVT_START; 

   do
   {
      UC_FU_evtProc(&UC_FU_cntxt, UC_FU_cntxt.nextEvt);
      
      if (UC_FU_cntxt.nextEvt == UC_FU_EVT_SYS_ERR)
      { 
          printf("Gateway comm error !! - quitting \n");
          break;
      }
 
      if (UC_FU_cntxt.nextEvt == UC_FU_EVT_GW_COMM_ERR)
      { 
          printf("Gateway comm error !! - quitting \n");
          break;
      }

      if (UC_FU_cntxt.nextEvt == UC_FU_EVT_RCVD_MALFORMED_MSG)
      { 
          printf("Malformed message received !! - quitting \n");
          UC_FU_sendNextPage();
          // break;
      }
 
   } while (UC_FU_cntxt.currState != UC_FU_STATE_DONE
            && UC_FU_cntxt.currState != UC_FU_STATE_ERROR);
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
int GW_procFwHexFile(char *buff_p, int fileSize)
{
   int idx, idy, error = 0, FW_segIdx = -1;
   int segStartAddr, segStart = 0;
           
   FW_segCnt = 0;
   memset(&FW_segList, 0, sizeof(FW_segList));

   printf("Parsing the firmware image .. \n");

   if (buff_p[0] != '@')
   {
       printf("Image file not in required format [0] !! \n");
       return 0;
   }

   // printf("\n %x   %x   %x  \n", buff_p[fileSize - 1], buff_p[fileSize - 2], buff_p[fileSize - 3]);

   if (buff_p[fileSize - 2] != 'q'
       && buff_p[fileSize - 3] != 'q')
   {
       printf("Image file not in required format [1] !! \n");
       return 0;
   }

   for (idx=0; idx<fileSize; idx++)
   {
       if (idx > 1 && (idx % 1024 == 0))
           printf("read %u bytes\n", idx);

       if (buff_p[idx] == '\r')
           continue;
       
       if (buff_p[idx] == '\n')
           continue;

       if (buff_p[idx] >= '0' && buff_p[idx] <= '9'
           || buff_p[idx] >= 'a' && buff_p[idx] <= 'f'
           || buff_p[idx] >= 'A' && buff_p[idx] <= 'F')
       {
           unsigned int byte;

           if (FW_segCnt < 1)
           {
               error = 1;
               break;
           }

           if (sscanf(buff_p + idx, "%x", &byte) != 1)
           {
               error = 1;
               break;
           }

           FW_segList[FW_segIdx].segBuff_p[FW_segList[FW_segIdx].segLen] = byte;
           FW_segList[FW_segIdx].segLen ++;
           
           // printf("Seg Idx<%d> / Seg Len <%u> / Byte <0x%x> \n",
           //         FW_segIdx,
           //         FW_segList[FW_segIdx].segLen,
           //         byte);
           
           idx += 2;
       }

       if (buff_p[idx] == 'q')
           break;

       if (buff_p[idx] == '@')
       {
           int rc;

           if (FW_segCnt > 0)
           {
               // printf("Number of bytes in previous segment - %u \n", FW_segList[FW_segIdx].segLen);
           }

           segStart = 1;
           rc = sscanf(buff_p + idx, "@%x", &segStartAddr);
           if (rc == 1)
           {
               if (segStartAddr < FW_SEG_MIN_START_ADDR
                   || segStartAddr > FW_SEG_MAX_START_ADDR)
               {
                   printf("Segment address <0x%x> is invalid !! \n");
                   error = 1;
                   break;
               }

               FW_segCnt ++;
               FW_segIdx ++;

               FW_segList[FW_segIdx].segStartAddr = segStartAddr;

               FW_segList[FW_segIdx].segBuff_p = malloc(FW_MAX_SEG_SIZE);
               if (FW_segList[FW_segIdx].segBuff_p == NULL)
               {
                   printf("Failed to allocate memory for segment buffer !! \n");
                   error = 1;
                   break;
               }

               // printf("Segment #%d : start address<0x%x> \n", FW_segCnt, segStartAddr); 
               idx ++;
               while (buff_p[idx] >= '0' && buff_p[idx] <= '9'
                      || buff_p[idx] >= 'a' && buff_p[idx] <= 'f')
               {
                    idx ++;
                    if (idx >= fileSize)
                    {
                        error = 1;
                        break;
                    }
               }

               if (error)
                   break;
           }
           else
           {
               error = 1;
               break;
           }
       }

       if (error)
           break;

   }
  
   printf("\n");
 
   printf("-------------------------------------------------------------- \n"); 


   printf("-------------------------------------------------------------- \n"); 

   if (FW_segCnt < 0)
   {
       printf("No segments found !! \n");
       error = 1;
   }

   for (idx=0; idx<FW_segCnt; idx++)
   {
        FW_segList[idx].segCRC = crc16(FW_segList[idx].segBuff_p,
                                       FW_segList[idx].segLen);

        FW_segList[idx].lastPageSz = FW_segList[idx].segLen % FW_UPG_DFLT_PAGE_SZ;
        FW_segList[idx].pageCnt = FW_segList[idx].segLen / FW_UPG_DFLT_PAGE_SZ;
        if (FW_segList[idx].lastPageSz)
            FW_segList[idx].pageCnt ++;
   }

   for (idy=FW_segCnt-1; idy > 0; idy --)
   {
       for (idx=0; idx<idy; idx++)
       {
           FW_segInfo_s *seg1_p = &FW_segList[idx];
           FW_segInfo_s *seg2_p = seg1_p + 1;

           // printf("<%s> idy<%u>/idx<%u>/p1<%u>/p2<%u> \n", 
           //         __FUNCTION__, idy, idx, seg1_p->pageCnt, seg2_p->pageCnt);

           if (seg1_p->pageCnt > seg2_p->pageCnt)
           {
                FW_segInfo_s tmp;

                tmp = *seg1_p;
                *seg1_p = *seg2_p;
                *seg2_p = tmp;
           }
       }
   }

   for (idx=0; idx<FW_segCnt; idx++)
   {
        printf("Segment Idx<%u>  / start address<0x%04x> / length <%u> / CRC <0x%x> \n",
               idx, FW_segList[idx].segStartAddr, FW_segList[idx].segLen, FW_segList[idx].segCRC);
   }

   if (error)
       return;

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
int GW_processRcvdMsg(unsigned char *buff_p, int offset, int currMsgType, int pyldLen)
{
   int rc = 0;

   if (verbose)
       printf("<%s> msgType<0x%x> / pyldLen <%d> \n", __FUNCTION__, currMsgType, pyldLen);

   if (verbose)
   {
       int idx;
        
       for (idx = 0; idx<pyldLen; idx++)
       {
            if (idx % 8 == 0)
                printf("\n <%s> : ", __FUNCTION__);
            printf(" 0x%x ", buff_p[offset + idx]);
       }
       printf("\n");
   }

   switch (currMsgType)
   {
      case LPWMN_GW_MSG_TYPE_EVENT:
            {
              rc = GW_processEvt(buff_p + offset, pyldLen); 
            }
            break;

      case LPWMN_GW_MSG_TYPE_RELAY_TO_NODE:
           {
              buff_p += offset;
              
              if (pyldLen >= 2)
              {
                  unsigned short rslt = __ntohs(buff_p);

                  if ((rslt & 0x00ff) == 0)
                      printf("Broadcast frame transmitted .... \n");
                  else
                      printf("Coord returned error module<%u> / sts<%u> !! \n",
                             (rslt >> 8) & 0xff, rslt & 0xff);
              }

              rc = 1;
           }
           break;

      case LPWMN_GW_MSG_TYPE_RELAY_FROM_NODE:
           {
              if (verbose)
                  printf("<%s> rcvd msg type RELAY_FROM_NODE \n", __FUNCTION__);

              buff_p += offset;

              if (pyldLen >= LPWMN_MAC_SHORT_ADDR_LEN 
                             + LPWMN_MAC_EXT_ADDR_LEN
                             + LPWMN_MSG_RSSI_LEN 
                             + LPWMN_MSG_CORR_LQI_LEN)
              {
                  unsigned short srcShortAddr = __ntohs(buff_p);
                  unsigned char disMsgType;

                  if (verbose)
                      printf("<%s> from node<0x%x> \n", __FUNCTION__, srcShortAddr);

                  buff_p += (LPWMN_MAC_SHORT_ADDR_LEN 
                             + LPWMN_MAC_EXT_ADDR_LEN 
                             + LPWMN_MSG_RSSI_LEN 
                             + LPWMN_MSG_CORR_LQI_LEN);

                  pyldLen -= (LPWMN_MAC_SHORT_ADDR_LEN 
                              + LPWMN_MAC_EXT_ADDR_LEN 
                              + LPWMN_MSG_RSSI_LEN 
                              + LPWMN_MSG_CORR_LQI_LEN);

                  if (verbose)
                      printf("<%s> pyldLen left<0x%x> \n", __FUNCTION__, pyldLen);

                  if (pyldLen >= DIS_MSG_TYPE_SZ)
                  {
                      disMsgType = *buff_p;
                      buff_p += DIS_MSG_TYPE_SZ;
                      pyldLen -= DIS_MSG_TYPE_SZ;


                      if (verbose)
                      {
                          printf("<%s> disMsgType<0x%x>/expDisMsgType<0x%x> \n", __FUNCTION__, disMsgType, expDisMsgType);
                          printf("<%s> src<0x%x>/expSrc<0x%x> \n", __FUNCTION__, srcShortAddr, expDisMsgSrcShortAddr);
                      }

                      if (disMsgType == expDisMsgType
                          && srcShortAddr == expDisMsgSrcShortAddr)
                      {
                          rc = 1;

                          if (verbose)
                              printf("<%s> dis-msg-type<0x%x> \n", __FUNCTION__, disMsgType);

                          switch (disMsgType)
                          {
                             case DIS_MSG_TYPE_NODE_UC_FU_MODULE_INFO:
                                  {
                                     unsigned char tlvLen1, *buff1_p;
                                   
                                     UC_FU_cntxt.nextEvt = UC_FU_EVT_RCVD_MALFORMED_MSG;

                                     if (verbose)
                                         printf("<%s> rcvd dis msg type NODE_UC_FU_MODULE_INFO \n", __FUNCTION__);

                                     rc = TLV_get(buff_p, pyldLen, 
                                                  DIS_TLV_TYPE_NODE_UC_FU_MODULE_INFO, 
                                                  &tlvLen1, &buff1_p);
                                     if (rc)
                                     {
                                         unsigned char tlvLen2, *buff2_p;
                                         unsigned char nodeFUState, fuRetCode, segIdx;
                                         unsigned int pageIdx, pageCRC;

                                         if (verbose)
                                         {
                                             int idx;
                                             printf("<%s> TLV_TYPE_NODE_UC_FU_MODULE_INFO found .. val-field_len<%u> \n",
                                                    __FUNCTION__, tlvLen1);
                                             for (idx=0; idx<tlvLen1; idx++)
                                             {
                                                  printf(" 0x%02x ", buff1_p[idx]);
                                                  if (idx % 8 == 0)
                                                      printf("\n");
                                             }
                                             printf("\n");
                                         }

                                         rc = TLV_get(buff1_p, tlvLen1, 
                                                      DIS_TLV_TYPE_UC_FU_STATE, 
                                                      &tlvLen2, &buff2_p);
                                         if (rc)
                                         {
                                             nodeFUState = *buff2_p;
                                             if (verbose)
                                                 printf("node-state<%u> \n", nodeFUState);
                                         }
                                         else
                                         {
                                             printf("<%s> Could not find DIS_TLV_TYPE_UC_FU_STATE !!\n",
                                                    __FUNCTION__);
                                             UC_FU_cntxt.currState = UC_FU_STATE_ERROR;
                                             rc = 1;
                                             break;
                                         }

                                         rc = TLV_get(buff1_p, tlvLen1, 
                                                      DIS_TLV_TYPE_UC_FU_RET_CODE, 
                                                      &tlvLen2, &buff2_p);
                                         if (rc)
                                         {
                                             fuRetCode = *buff2_p;
                                             if (verbose)
                                                 printf("ret-code<%u> \n", fuRetCode);
                                         }
                                         else
                                         {
                                             printf("<%s> Could not find DIS_TLV_TYPE_UC_FU_RET_CODE \n",
                                                   __FUNCTION__);
                                             UC_FU_cntxt.currState = UC_FU_STATE_ERROR;
                                             rc = 1;
                                             break;
                                         }
                                        
                                         if (verbose) 
                                             printf("state <%u> \n", UC_FU_cntxt.currState);

                                         if (UC_FU_cntxt.currState == UC_FU_STATE_SENT_IMAGE_SWITCH_REQUEST)
                                         {
                                             printf("Return code <%u : %s> \n", fuRetCode,
                                                    fuRetCode <= UC_FU_RC_STORE_IMAGE_INFO_CORRUPT ? \
                                                    UC_FU_rcStr[fuRetCode] : "Unknown Error");
                                             rc = 1;
                                             break;
                                         }

                                         if (nodeFUState != UC_FU_NODE_STATE_IDLE)
                                         {
                                             rc = TLV_get(buff1_p, tlvLen1, 
                                                          DIS_TLV_TYPE_UC_FU_SEG_IDX, 
                                                          &tlvLen2, &buff2_p);
                                             if (rc)
                                             {
                                                 segIdx = *buff2_p;
                                                 if (verbose)
                                                     printf("Segment Idx <%u> \n", segIdx);
                                             }
                                             else
                                             {
                                                 // UC_FU_cntxt.currState = UC_FU_STATE_ERROR;
                                                 printf("<%s> Could not find DIS_TLV_TYPE_UC_FU_SEG_IDX !!\n",
                                                        __FUNCTION__);
                                                 rc = 0;
                                                 break;
                                             }
                                            
                                             rc = TLV_get(buff1_p, tlvLen1, 
                                                          DIS_TLV_TYPE_UC_FU_PAGE_IDX, 
                                                          &tlvLen2, &buff2_p);
                                             if (rc)
                                             {
                                                 pageIdx = __ntohs(buff2_p);
                                                 if (verbose)
                                                     printf("Page Idx <%u> \n", pageIdx);
                                             }
                                             else
                                             {
                                                 // UC_FU_cntxt.currState = UC_FU_STATE_ERROR;
                                                 printf("<%s> Could not find DIS_TLV_TYPE_UC_FU_PAGE_IDX !!\n",
                                                        __FUNCTION__);
                                                 rc = 0;
                                                 break;
                                             }

                                             rc = TLV_get(buff1_p, tlvLen1, 
                                                          DIS_TLV_TYPE_UC_FU_PAGE_CRC,
                                                          &tlvLen2, &buff2_p);
                                             if (rc)
                                             {
                                                 pageCRC = __ntohs(buff2_p);
                                                 if (verbose)
                                                     printf("Page Idx <%u> \n", pageIdx);
                                             }
                                             else
                                             {
                                                 // UC_FU_cntxt.currState = UC_FU_STATE_ERROR;
                                                 printf("<%s> Could not find DIS_TLV_TYPE_UC_FU_PAGE_CRC !!\n",
                                                        __FUNCTION__);
                                                 rc = 0;
                                                 break;
                                             }

                                             UC_FU_cntxt.lastResp.nodeFUState = nodeFUState;
                                             UC_FU_cntxt.lastResp.retCode = fuRetCode;
                                             if (nodeFUState != UC_FU_NODE_STATE_IDLE)
                                             {
                                                 UC_FU_cntxt.lastResp.segIdx = segIdx;
                                                 UC_FU_cntxt.lastResp.pageIdx = pageIdx;
                                                 UC_FU_cntxt.lastResp.pageCRC = pageCRC;
                                             }
                                             UC_FU_cntxt.lastResp.rcvd = 1;
            
                                             UC_FU_cntxt.nextEvt = UC_FU_EVT_RCVD_NODE_FU_MOD_STS;
 
                                             rc = 1;
                                         }
                                         else
                                             UC_FU_cntxt.nextEvt = UC_FU_EVT_RCVD_NODE_FU_MOD_STS;
                                     }
                                     else
                                     {
                                         UC_FU_cntxt.currState = UC_FU_STATE_ERROR;
                                         printf("<%s> Could not find DIS_TLV_TYPE_NODE_UC_FU_MODULE_INFO !!\n",
                                                __FUNCTION__);
                                         rc = 1;
                                         break;
                                     }
                                  }
                                  break;

                             case DIS_MSG_TYPE_GET_LIST_ATTR_VAL:
                                  {
                                     if (verbose)
                                         printf("<%s> rcvd dis msg type GET_NODE_LIST_ATTR_VAL \n", __FUNCTION__);
                                     
                                     if (pyldLen >= DIS_TLV_HDR_SZ)
                                     {
                                         unsigned char tlvLen1, *buff1_p;
                                         rc = TLV_get(buff_p, pyldLen, DIS_TLV_TYPE_LIST_ATTR_INFO, &tlvLen1, &buff1_p);
                                         if (rc == 0)
                                         {
                                             printf("\n Could not find DIS_TLV_TYPE_LIST_ATTR_INFO  !!\n"); 
                                         } 
                                         else 
                                         {
                                             if (verbose)
                                                 printf("Found DIS_TLV_TYPE_LIST_ATTR_INFO - pyldLen<%d>/tlvLen1<%d> \n", 
                                                        pyldLen, tlvLen1);
                                             
                                             pyldLen -= DIS_TLV_HDR_SZ;
                                             if (pyldLen >= tlvLen1)
                                             {
                                                 unsigned char tlvLen2, *buff2_p;
                                                 rc = TLV_get(buff1_p, pyldLen, DIS_TLV_TYPE_LIST_ATTR_ID, &tlvLen2, &buff2_p);
                                                 if (rc == 0)
                                                 {
                                                     printf("\n Could not find DIS_TLV_TYPE_LIST_ATTR_ID  !!\n");
                                                 }
                                                 else
                                                 {
                                                     if (verbose)
                                                         printf("\n Found DIS_TLV_TYPE_LIST_ATTR_ID ... \n");

                                                     if (tlvLen2 == DIS_LIST_ATTR_ID_FIELD_SZ)
                                                     {
                                                         unsigned int attrId = __ntohs(buff2_p);

                                                         printf("AttrId<%u>  ", attrId);

                                                         rc = TLV_get(buff1_p, pyldLen, DIS_TLV_TYPE_LIST_ATTR_IDX, &tlvLen2, &buff2_p);
                                                         if (rc == 0)
                                                         {
                                                             printf("\n Could not find DIS_TLV_TYPE_LIST_ATTR_IDX  !!\n");
                                                         }
                                                         else
                                                         {
                                                             unsigned int attrIdx = __ntohs(buff2_p);

                                                             printf("AttrIdx<%u>  ", attrIdx);
                                                         
                                                             rc = TLV_get(buff1_p, pyldLen, DIS_TLV_TYPE_ATTR_VAL, &tlvLen2, &buff2_p);
                                                             if (rc == 0)
                                                             {
                                                                 printf("\n Could not find DIS_TLV_TYPE_ATTR_VAL  !!\n");
                                                             }
                                                             else
                                                             {
                                                                 if (verbose)
                                                                     printf("\n Found DIS_TLV_TYPE_ATTR_VAL ... %u \n", tlvLen2);

                                                                 switch (attrId)
                                                                 {
                                                                    case NWK_ROUTING_TABLE_ENTRY_ATTR_ID: 
                                                                         {
                                                                            if (tlvLen2 == (LPWMN_MAC_SHORT_ADDR_LEN * 2))
                                                                            {
                                                                                unsigned int val = __ntohl(buff2_p);
                                                                                unsigned int destAddr, nextHop;

                                                                                destAddr = (val >> 16) & 0xffff;
                                                                                nextHop= (val) & 0xffff;
                                                                               
                                                                                printf("Dest Addr<%04d> / Next Hop<%04d>\n",
                                                                                       destAddr, nextHop);
                                                                            }
                                                                         }
                                                                         break;

                                                                    default:
                                                                         break;
                                                                 }
                                                             }
                                                         }
                                                     }
                                                 }
                                             }
                                         } 
                                     }
                                  }
                                  break;

                             case DIS_MSG_TYPE_GET_ATTR_VAL:
                                  {
                                     if (verbose)
                                         printf("<%s> rcvd dis msg type GET_NODE_ATTR_VAL \n", __FUNCTION__);
                                     
                                     if (pyldLen >= DIS_TLV_HDR_SZ)
                                     {
                                         unsigned char tlvLen1, *buff1_p;
                                         rc = TLV_get(buff_p, pyldLen, DIS_TLV_TYPE_ATTR_INFO, &tlvLen1, &buff1_p);
                                         if (rc == 0)
                                         {
                                             printf("\n Could not find DIS_TLV_TYPE_ATTR_INFO  !!\n");
                                         }
                                         else 
                                         {
                                             if (verbose)
                                                 printf("Found DIS_TLV_TYPE_ATTR_INFO - pyldLen<%d>/tlvLen1<%d> \n", pyldLen, tlvLen1);

                                             pyldLen -= DIS_TLV_HDR_SZ;
                                             if (pyldLen >= tlvLen1)
                                             {
                                                 unsigned char tlvLen2, *buff2_p;
                                                 rc = TLV_get(buff1_p, pyldLen, DIS_TLV_TYPE_ATTR_ID, &tlvLen2, &buff2_p);
                                                 if (rc == 0)
                                                 {
                                                     printf("\n Could not find DIS_TLV_TYPE_ATTR_ID  !!\n");
                                                 }
                                                 else
                                                 {
                                                     if (verbose)
                                                         printf("\n Found DIS_TLV_TYPE_ATTR_ID ... \n");

                                                     if (tlvLen2 == DIS_ATTR_ID_FIELD_SZ)
                                                     {
                                                         unsigned int attrId = __ntohs(buff2_p);

                                                         printf("AttrId<%u>  ", attrId);

                                                         rc = TLV_get(buff1_p, pyldLen, DIS_TLV_TYPE_ATTR_VAL, &tlvLen2, &buff2_p);
                                                         if (rc == 0)
                                                         {
                                                             printf("\n Could not find DIS_TLV_TYPE_ATTR_ID  !!\n");
                                                         }
                                                         else
                                                         {
                                                             if (verbose)
                                                                 printf("\n Found DIS_TLV_TYPE_ATTR_VAL ... %u \n", tlvLen2);

                                                             switch (tlvLen2)
                                                             {
                                                                 case 1:
                                                                    printf("AttrVal<%u> \n", *buff2_p);
                                                                    break;

                                                                 case 2:
                                                                    printf("AttrVal<%u> \n", __ntohs(buff2_p));
                                                                    break;

                                                                 case 4:
                                                                    {
                                                                       switch (attrId)
                                                                       {
                                                                          case FU_IMAGE_STORE_IMAGE_FLAGS_ATTR_ID:
                                                                               {
                                                                                   if (buff2_p[0] == 0x0)
                                                                                       printf("No storage for images on this node !! \n");
                                                                                   else
                                                                                   {
                                                                                       int idx;
                                                                                       printf("AttrVal: 0x%x 0x%x 0x%x 0x%x\n",
                                                                                              buff2_p[0], buff2_p[1],
                                                                                              buff2_p[2], buff2_p[3]);
                                                                                       for (idx=0; idx<7; idx++)
                                                                                       {
                                                                                            if (!(buff2_p[0] & (1 << idx)))
                                                                                                continue;
                                                                                            printf("Image #%d : %s    %s    %s\n",
                                                                                                   idx + 1, (buff2_p[1] & (1 << idx)) ? "Valid  " : "Invalid",
                                                                                                   (buff2_p[2] & (1 << idx)) ? "Active    " : "Not Active",
                                                                                                   (buff2_p[3] & (1 << idx)) ? "Update in Progress" : "");
                                                                                       }
                                                                                   }
                                                                               }
                                                                               break;

                                                                          case FU_IMAGE_STORE_IMAGE_1_IMAGE_RCVD_TIME_STAMP_ATTR_ID:
                                                                          case FU_IMAGE_STORE_IMAGE_2_IMAGE_RCVD_TIME_STAMP_ATTR_ID:
                                                                               {
                                                                                  char timeBuff[32];
                                                                                  unsigned int timeStamp = __ntohl(buff2_p);

                                                                                  ctime_r((time_t *)&timeStamp, timeBuff);

                                                                                  printf("AttrVal: %s", timeBuff);
                                                                               }
                                                                               break;

                                                                          default:
                                                                               {
                                                                                  printf("AttrVal<%u> \n", __ntohl(buff2_p));
                                                                               }
                                                                               break;
                                                                       }
                                                                    }
                                                                    break;
   
                                                                 default:
                                                                    if (attrId == FW_BUILD_DATE_ATTR_ID || attrId  == FW_BUILD_TIME_ATTR_ID)
                                                                    {
                                                                        printf("%s%s%s \n", 
                                                                               attrId == FW_BUILD_DATE_ATTR_ID ? "build date <" : "build time <" , buff2_p, ">");
                                                                    }
                                                                    else
                                                                        printf("AttrVal<%d byte response> \n", tlvLen2);
                                                                    break;
                                                             }
                                                         }
                                                     }
                                                     else
                                                     {
                                                         printf("\n Bad DIS_TLV_TYPE_ATTR_ID TLV !!\n");
                                                     }
                                                 }
                                             }
                                         }
                                     }
                                     else
                                     {
                                         printf("\n Could not find DIS_TLV_TYPE_ATTR_INFO  !!\n");
                                     }
                                  }
                                  break;

                             case DIS_MSG_TYPE_GET_NEIGHBOR_TBL_ENTRY:
                                  {
                                     unsigned char tlvLen1, *buff1_p;

                                     if (verbose)
                                         printf("<%s> rcvd dis msg type GET_NEIGHBOR_TBL_ENTRY\n", __FUNCTION__);
                                         
                                     rc = TLV_get(buff_p, pyldLen, DIS_TLV_TYPE_MAC_NEIGHBOR_TBL_ENTRY, &tlvLen1, &buff1_p);
                                     if (rc == 0)
                                     {
                                         printf("\nCould not find MAC_NEIGHBOR_TBL_ENTRY TLV !! \n");
                                         break;
                                     }
                                     else
                                     {
                                         unsigned char tlvLen2, *buff2_p;

                                         rc = TLV_get(buff1_p, tlvLen1, DIS_TLV_TYPE_SHORT_ADDR, &tlvLen2, &buff2_p);
                                         if (rc == 0)
                                         {
                                             printf("\nCould not find DIS_TLV_TYPE_SHORT_ADDR !! \n");
                                             break;
                                         }
                                         else
                                         {
                                             if (verbose)
                                                 printf("Found DIS_TLV_TYPE_SHORT_ADDR \n");
                                             printf("-------------------------------------------------- \n");
                                             printf("Neighbor table entry for %05d on node %05d \n", __ntohs(buff2_p), srcShortAddr);
                                         }

                                         rc = TLV_get(buff1_p, tlvLen1, DIS_TLV_TYPE_RCVD_PKT_CNT, &tlvLen2, &buff2_p);
                                         if (rc == 0)
                                         {
                                             printf("\nCould not find DIS_TLV_TYPE_RCVD_PKT_CNT !! \n");
                                             printf("-------------------------------------------------- \n");
                                             break;
                                         }
                                         else
                                         {
                                             int rcvdPktCnt;

                                             if (verbose)
                                                 printf("Found DIS_TLV_TYPE_RCVD_PKT_CNT \n");

                                             rcvdPktCnt = __ntohs(buff2_p);

                                             printf("Received packet count : %u \n", rcvdPktCnt);
                                         }

                                         rc = TLV_get(buff1_p, tlvLen1, DIS_TLV_TYPE_RSSI, &tlvLen2, &buff2_p);
                                         if (rc == 0)
                                         {
                                             printf("\nCould not find DIS_TLV_TYPE_RSSI !! \n");
                                             printf("-------------------------------------------------- \n");
                                             break;
                                         }
                                         else
                                         {
                                             int rssi;

                                             if (verbose)
                                                 printf("Found DIS_TLV_TYPE_RSSI\n");

                                             rssi = *((signed char *)buff2_p);
                                             printf("RSSI (last received packet) : %d \n", rssi);
                                         }

                                         rc = TLV_get(buff1_p, tlvLen1, DIS_TLV_TYPE_LQI, &tlvLen2, &buff2_p);
                                         if (rc == 0)
                                         {
                                             printf("\nCould not find DIS_TLV_TYPE_LQI!! \n");
                                             printf("-------------------------------------------------- \n");
                                             break;
                                         }
                                         else
                                         {
                                             int lqi;

                                             if (verbose)
                                                 printf("Found DIS_TLV_TYPE_LQI \n");

                                             lqi = *((unsigned char *)buff2_p);
                                             printf("LQI (last received packet) : %d \n", lqi);
                                         }

                                         rc = TLV_get(buff1_p, tlvLen1, DIS_TLV_TYPE_SECONDS_SINCE_PKT_RX, &tlvLen2, &buff2_p);
                                         if (rc == 0)
                                         {
                                             printf("\nCould not find DIS_TLV_TYPE_SECONDS_SINCE_PKT_RX!! \n");
                                             printf("-------------------------------------------------- \n");
                                             break;
                                         }
                                         else
                                         {
                                             int timeSinceLastRx;

                                             if (verbose)
                                                 printf("Found DIS_TLV_TYPE_SECONDS_SINCE_PKT_RX \n");

                                             timeSinceLastRx = __ntohs(buff2_p);

                                             printf("Last packet received %u seconds ago \n", timeSinceLastRx);
                                             printf("-------------------------------------------------- \n");
                                         }

                                     }
                                  }
                                  break;


#if 0
                             case DIS_MSG_TYPE_GET_NEIGHBOR_TBL_ENTRY:
                                  {
                                     unsigned char tlvLen1, *buff1_p;

                                     if (verbose)
                                         printf("<%s> rcvd dis msg type GET_NEIGHBOR_TBL_ENTRY\n", __FUNCTION__);
                                         
                                     rc = TLV_get(buff_p, pyldLen, DIS_TLV_TYPE_MAC_NEIGHBOR_TBL_ENTRY, &tlvLen1, &buff1_p);
                                     if (rc == 0)
                                     {
                                         printf("\nCould not find MAC_NEIGHBOR_TBL_ENTRY TLV !! \n");
                                         break;
                                     }
                                     else
                                     {
                                         unsigned char tlvLen2, *buff2_p;

                                         rc = TLV_get(buff1_p, tlvLen1, DIS_TLV_TYPE_SHORT_ADDR, &tlvLen2, &buff2_p);
                                         if (rc == 0)
                                         {
                                             printf("\nCould not find DIS_TLV_TYPE_SHORT_ADDR !! \n");
                                             break;
                                         }
                                         else
                                         {
                                             if (verbose)
                                                 printf("Found DIS_TLV_TYPE_SHORT_ADDR \n");
                                             printf("-------------------------------------------------- \n");
                                             printf("Neighbor table entry for 0x%x on node 0x%x \n", __ntohs(buff2_p), srcShortAddr);
                                         }

                                         rc = TLV_get(buff1_p, tlvLen1, DIS_TLV_TYPE_RCVD_PKT_CNT, &tlvLen2, &buff2_p);
                                         if (rc == 0)
                                         {
                                             printf("\nCould not find DIS_TLV_TYPE_RCVD_PKT_CNT !! \n");
                                             printf("-------------------------------------------------- \n");
                                             break;
                                         }
                                         else
                                         {
                                             int rcvdPktCnt;

                                             if (verbose)
                                                 printf("Found DIS_TLV_TYPE_RCVD_PKT_CNT \n");

                                             rcvdPktCnt = __ntohs(buff2_p);

                                             printf("Received packet count : %u \n", rcvdPktCnt);
                                         }

                                         rc = TLV_get(buff1_p, tlvLen1, DIS_TLV_TYPE_RSSI, &tlvLen2, &buff2_p);
                                         if (rc == 0)
                                         {
                                             printf("\nCould not find DIS_TLV_TYPE_RSSI !! \n");
                                             printf("-------------------------------------------------- \n");
                                             break;
                                         }
                                         else
                                         {
                                             int rssi;

                                             if (verbose)
                                                 printf("Found DIS_TLV_TYPE_RSSI\n");

                                             rssi = *((signed char *)buff2_p);
                                             printf("RSSI (last received packet) : %d \n", rssi);
                                         }

                                         rc = TLV_get(buff1_p, tlvLen1, DIS_TLV_TYPE_LQI, &tlvLen2, &buff2_p);
                                         if (rc == 0)
                                         {
                                             printf("\nCould not find DIS_TLV_TYPE_LQI!! \n");
                                             printf("-------------------------------------------------- \n");
                                             break;
                                         }
                                         else
                                         {
                                             int lqi;

                                             if (verbose)
                                                 printf("Found DIS_TLV_TYPE_LQI \n");

                                             lqi = *((unsigned char *)buff2_p);
                                             printf("LQI (last received packet) : %d \n", lqi);
                                         }

                                         rc = TLV_get(buff1_p, tlvLen1, DIS_TLV_TYPE_SECONDS_SINCE_PKT_RX, &tlvLen2, &buff2_p);
                                         if (rc == 0)
                                         {
                                             printf("\nCould not find DIS_TLV_TYPE_SECONDS_SINCE_PKT_RX!! \n");
                                             printf("-------------------------------------------------- \n");
                                             break;
                                         }
                                         else
                                         {
                                             int timeSinceLastRx;

                                             if (verbose)
                                                 printf("Found DIS_TLV_TYPE_SECONDS_SINCE_PKT_RX \n");

                                             timeSinceLastRx = __ntohs(buff2_p);

                                             printf("Last packet received %u seconds ago \n", timeSinceLastRx);
                                             printf("-------------------------------------------------- \n");
                                         }

                                     }
                                  }
                                  break;
#endif

                             case DIS_MSG_TYPE_GET_RT_NEXT_HOP:
                                  {
                                     if (verbose)
                                         printf("<%s> rcvd dis msg type GET_RT_NEXT_HOP \n", __FUNCTION__);

                                     if (pyldLen == DIS_SHORT_ADDR_TLV_SZ)
                                     {
                                         unsigned char tlvLen1, *buff1_p;
                                         rc = TLV_get(buff_p, pyldLen, DIS_TLV_TYPE_SHORT_ADDR, &tlvLen1, &buff1_p);
                                         if (rc == 0)
                                         {
                                             printf("\nCould not find DIS_TLV_TYPE_SHORT_ADDR !! \n");
                                         }
                                         else
                                         {
                                             if (verbose)
                                                 printf("Found DIS_TLV_TYPE_SHORT_ADDR \n");
                                             printf("Next Hop - %d \n", __ntohs(buff1_p));
                                         }
                                     }
                                  }
                                  break;

                             case DIS_MSG_TYPE_NODE_BCN_REQ_HANDLING_CFG:
                                  {
                                   
                                  }
                                  break;
                             
                             case DIS_MSG_TYPE_GET_GPIO_PORT_OP_STATE:
                                  {
                                     if (verbose)
                                         printf("<%s> rcvd dis msg type GET_GPIO_PORT_OP_STATE \n", __FUNCTION__);
                                   
                                     if (pyldLen > DIS_TLV_HDR_SZ)
                                     {
                                         unsigned char tlvLen1, *buff1_p;
                                         rc = TLV_get(buff_p, pyldLen, DIS_TLV_TYPE_GPIO_PORT_INFO_LIST, &tlvLen1, &buff1_p);
                                         if (rc == 0)
                                         {
                                             if (verbose)
                                                 printf("\nCould not find DIS_TLV_TYPE_GPIO_PORT_INFO_LIST !! \n");
                                         }
                                         else
                                         {
                                             if (verbose)
                                                 printf("Found DIS_TLV_TYPE_GPIO_PORT_INFO_LIST \n");

                                             pyldLen -= DIS_TLV_HDR_SZ;

                                             while (pyldLen > 0)
                                             {
                                                 unsigned char tlvLen2, *buff2_p;
                                                 
                                                 rc = TLV_get(buff1_p, tlvLen1, DIS_TLV_TYPE_GPIO_PORT_INFO, &tlvLen2, &buff2_p);
                                                 if (rc == 0)
                                                 {
                                                     break;
                                                 }
                                                 else
                                                 {
                                                     unsigned char tlvLen3, *buff3_p, *buff4_p;
                                            
                                                     rc = TLV_get(buff2_p, tlvLen2, DIS_TLV_TYPE_GPIO_PORT_ID, &tlvLen3, &buff3_p);
                                                     if (rc)
                                                     {
                                                         rc = TLV_get(buff2_p, tlvLen2, DIS_TLV_TYPE_GPIO_PORT_OP_STATE, &tlvLen3, &buff4_p);
                                                         if (rc)
                                                         { 
                                                             int idx;
                                                             printf("Port<%u>  ", *(buff3_p) + 1, *(buff4_p));
                                                             for (idx=0; idx<8; idx++)
                                                                printf(" %d[%d]  ", 7-idx, (*(buff4_p)) & (1 << (7-idx)) ? 1 : 0);
                                                             printf("\n");
                                                         }
                                                     }
                                                 }

                                                 pyldLen -= (tlvLen2 + DIS_TLV_HDR_SZ);
                                                 buff1_p += (tlvLen2 + DIS_TLV_HDR_SZ);
                                             } 
                                         }
                                     }
                                  }
                                  break;

                             case DIS_MSG_TYPE_NODE_SENSOR_COUNT:
                                  {
                                     if (verbose)
                                         printf("<%s> rcvd dis msg type NODE_SENSOR_CNT \n", __FUNCTION__);

                                     if (pyldLen == DIS_SENSOR_COUNT_TLV_SZ)
                                     {
                                         unsigned char tlvLen1, *buff1_p;
                                         rc = TLV_get(buff_p, pyldLen, DIS_TLV_TYPE_SENSOR_COUNT, &tlvLen1, &buff1_p);
                                         if (rc == 0)
                                         {
                                             if (verbose)
                                                 printf("\nCould not find DIS_TLV_TYPE_SENSOR_COUNT !! \n");
                                         }
                                         else
                                         {
                                             if (verbose)
                                                 printf("Found DIS_TLV_TYPE_SENSOR_COUNT \n");
                                             printf("Sensor count - %u \n", *buff1_p);
                                         }
                                     }
                                  }
                                  break;

                             default:
                                  {
                                  }
                                  break;
                          }
                      }
                  }
              }
           }
           break;

      case LPWMN_GW_MSG_TYPE_SET_RADIO_CHANNEL:
           {
              if (pyldLen == 0x1)
              {
                  if (buff_p[offset] == 0x1)
                      printf("done \n");
                  else
                      printf("request denied !!\n");
              }
              rc = 1;
           }
           break;

      case LPWMN_GW_MSG_TYPE_GET_BLACK_LIST_ENTRY:
           {
              if (pyldLen == (2 + LPWMN_EXT_ADDR_LEN))
              {
                  int idx;
                  unsigned char *b_p = buff_p + offset;
                  printf("Tbl Index <%d> / ext addr <0x%x",
                         __ntohs(b_p), *(b_p + 2));
                  for (idx=1; idx<LPWMN_EXT_ADDR_LEN; idx++)
                       printf(":0x%02x", b_p[2 + idx]);
                  listTblIdx = __ntohs(b_p);
                  printf(">\n");
                  rc = 1;
              }
              else
              {
                  if (pyldLen == 2)
                  {
                      rc = 1;
                      allBLEntriesListed = 1;
                  }
              }
           }
           break;

      case LPWMN_GW_MSG_TYPE_GET_WHITE_LIST_ENTRY:
           {
              if (pyldLen == (2 + LPWMN_EXT_ADDR_LEN))
              {
                  int idx;
                  unsigned char *b_p = buff_p + offset;
                  printf("Tbl Index <%d> / ext addr <0x%x",
                         __ntohs(b_p), *(b_p + 2));
                  for (idx=1; idx<LPWMN_EXT_ADDR_LEN; idx++)
                       printf(":0x%02x", b_p[2 + idx]);
                  listTblIdx = __ntohs(b_p);
                  printf(">\n");
                  rc = 1;
              }
              else
              {
                  if (pyldLen == 2)
                  {
                      rc = 1;
                      allWLEntriesListed = 1;
                  }
              }
           }
           break;

      case LPWMN_GW_MSG_TYPE_NODE_LIST_REQ:
           {
              if (pyldLen == 17)
              {
                  int idx;
                  unsigned char *b_p = buff_p + offset;
                  unsigned char capFlags =  *(b_p + 14);

                  if (GW_nodeListReqType == RFD_LIST_ONLY
                      && ((capFlags & NODE_CAP_DEV_TYPE_BIT_SHIFT_MSK) == 0)
                      || (GW_nodeListReqType == FFD_LIST_ONLY
                          && (capFlags & NODE_CAP_DEV_TYPE_BIT_SHIFT_MSK))
                      || GW_nodeListReqType == ALL_NODES_LIST)
                  {
                       printf("Node Idx <%03d> / short addr <%05u> / parent <%05u> / ext addr <%02x",
                              __ntohs(b_p), 
                              __ntohs(b_p + 2), 
                              __ntohs(b_p + 4), *(b_p + 6));
                       for (idx=1; idx<LPWMN_EXT_ADDR_LEN; idx++)
                            printf(":%02x", b_p[6 + idx]);
                       printf("> / cap <0x%02x> ", b_p[14]);
                       printf("  %05u \n", __ntohs(b_p + 15));
                  }
                  listTblIdx = __ntohs(b_p);
                  rc = 1;
              }
              else
              {
                  if (pyldLen == 2)
                  {
                      rc = 1;
                      allNLEntriesListed = 1;
                  }
              }
           }
           break;

      case UART_MSG_TYPE_ACK:
           {
              unsigned char hdrFlags = buff_p[UART_FRAME_HDR_FLAGS_FIELD_OFF];
              if (verbose)
                  printf("Hdr Flags <0x%x> \n", hdrFlags);
              if (hdrFlags & UART_ACK_STS_OK_BM)
                  hdrAcked = 1;
              else
              {
                  printf("<%s> hdr flags <0x%x> \n", __FUNCTION__, hdrFlags);
                  hdrAcked = 0;
              }
              rc = 1;
           }
           break;

      case LPWMN_GW_MSG_TYPE_GET_BCN_REQ_HANDLING_CFG:
           { 
              if (pyldLen == 0x1)
              {
                  if (buff_p[offset] == 0x0)
                      printf("Coord is dropping beacon requests \n");
                  else
                      printf("Coord is accepting beacon requests \n");
              }
              rc = 1;
           }
           break;

      case LPWMN_GW_MSG_TYPE_GET_NWK_JOIN_CTRL_STATE:
           { 
              if (pyldLen == 0x1)
              {
                  if (buff_p[offset] == 0x0)
                      printf("network joins disabled \n");
                  else
                      printf("network joins enabled \n");
              }
              rc = 1;
           }
           break;

      case LPWMN_GW_MSG_TYPE_START_COORD:
           {
              if (pyldLen == 0x1)
              {
                  if (buff_p[offset] == 0x1)
                      printf("done \n");
                  else
                      printf("request denied !!\n");
              }
              rc = 1;
           }
           break;


      case LPWMN_GW_MSG_TYPE_SET_COORD_ATTR_VAL:
           {
              if (pyldLen == 0x2)
                  printf("status <%u> \n", __ntohs(buff_p + offset));
              else
                  printf("invalid response !!\n");
              rc = 1;
           }
           break;

      case LPWMN_GW_MSG_TYPE_GET_COORD_ATTR_VAL:
           {
              if (verbose)
                  printf("\n pyldLen - %u \n", pyldLen);

              switch (pyldLen)
              {
                 case 1:
                    printf("%u\n", buff_p[offset]);
                    break;

                 case 2:
                    printf("%u\n", __ntohs(buff_p + offset));
                    break;

                 case 4:
                    printf("%u\n", __ntohl(buff_p + offset));
                    break;
   
                 default:
                    printf("%s\n", buff_p + offset);
                    break;
              }
              rc = 1;
           }
           break;

      case LPWMN_GW_MSG_TYPE_GET_RADIO_FREQ_BAND:
           {
              printf("%s\n", buff_p + offset);
              rc = 1;
           }
           break;

      case LPWMN_GW_MSG_TYPE_GET_RADIO_MOD_FMT:
           {
              printf("%s\n", buff_p + offset);
              rc = 1;
           }
           break;
      
      case LPWMN_GW_MSG_TYPE_GET_RADIO_BAUD_RATE:
           {
              if (pyldLen == LPWMN_GW_MSG_RADIO_BAUD_RATE_FIELD_LEN)
              {
                  int br = __ntohl(buff_p + offset);
                  printf("%d baud \n", br);
              }
              rc = 1;
           }
           break;
 
      case LPWMN_GW_MSG_TYPE_GET_RADIO_TX_PWR:
           {
              if (pyldLen == LPWMN_GW_MSG_RADIO_TX_PWR_FIELD_LEN)
              {
                  int txPower = __ntohs(buff_p + offset);
                  float txPf = txPower;
                  txPf /= 100;
                  printf("%c%2.2f dBm \n", txPower < 0 ? '-' : '+', txPf);
                  rc = 1;
              }
           }
           break;
 
      case LPWMN_GW_MSG_TYPE_GET_RADIO_PART_NR:
           {
              printf("%s\n", buff_p + offset);
              rc = 1;
           }
           break;

      case LPWMN_GW_MSG_TYPE_GET_COORD_SW_BUILD_TIME:
      case LPWMN_GW_MSG_TYPE_GET_COORD_SW_BUILD_DATE:
           {
              printf("%s\n", buff_p + offset);
              rc = 1;
           }
           break;
 
      case LPWMN_GW_MSG_TYPE_GET_NWK_COORD_EXT_ADDR:
           {
              if (pyldLen == LPWMN_EXT_ADDR_LEN)
              {
                  int idx;
                  printf("0x%x", buff_p[offset]);
                  for (idx=1; idx<LPWMN_EXT_ADDR_LEN; idx++)
                       printf(":0x%x", buff_p[offset + idx]);
                  printf("\n");
                  rc = 1;
              }
           }
           break;

      case LPWMN_MSG_TYPE_GET_RT_NH_ON_COORD:
           {
              if (pyldLen == 2)
              {
                  unsigned int shortAddr = __ntohs(buff_p + offset);
                  printf("%05d\n", shortAddr);
                  rc = 1;
              }
           }
           break;

      case LPWMN_GW_MSG_TYPE_GET_NODE_LAST_FATAL_ERR_ID:
           {
              if (pyldLen == 2)
              {
                  int fatalErrId = __ntohs(buff_p + offset);
                  printf("%d\n", fatalErrId);
                  rc = 1;
              }
           }
           break;
     
      case LPWMN_GW_MSG_TYPE_NODE_CNT_REQ:
           {
              if (pyldLen == 2)
              {
                  int nodeCnt = __ntohs(buff_p + offset);
                  printf("Number of nodes in the network - %d\n", nodeCnt);
                  rc = 1;
              }
           }
           break;

      case LPWMN_GW_MSG_TYPE_MON_CHANN:
           {
              if (pyldLen == (6 + 18))
              {
                  short idx, minRSSI, maxRSSI, avgRSSI;
                  unsigned short u16Val;

                  u16Val = __ntohs(buff_p + offset); 
                  offset += 2;
                  minRSSI = (signed short)u16Val;
 
                  u16Val = __ntohs(buff_p + offset);
                  offset += 2;
                  maxRSSI = (signed short)u16Val;
                  
                  u16Val = __ntohs(buff_p + offset);
                  offset += 2;
                  avgRSSI = (signed short)u16Val;

                  printf("Min RSSI <%d> / Max RSSI <%d> / Avg RSSI <%d> \n",
                         minRSSI, maxRSSI, avgRSSI);

                  for (idx=0; idx<3; idx++)
                  {
                       short start, stop, perc;
                       
                       u16Val = __ntohs(buff_p + offset);
                       offset += 2;
                       start = (signed short)u16Val;

                       u16Val = __ntohs(buff_p + offset);
                       offset += 2;
                       stop = (signed short)u16Val;

                       u16Val = __ntohs(buff_p + offset);
                       offset += 2;
                       perc = (signed short)u16Val;

                       printf("(%d dBm to %d dBm)  [%d] \n", start, stop, perc);
                  }

                  rc = 1;
              }
           } 
           break;

      case LPWMN_GW_MSG_TYPE_GET_COORD_UP_TIME:
           {
              if (pyldLen == 4)
              {
                  unsigned int timeInSecs = __ntohl(buff_p + offset);
                  unsigned int days = 0;
                  unsigned int hours = 0;
                  unsigned int minutes = timeInSecs / 60;

                  if (timeInSecs >= 60)
                  {
                      minutes = timeInSecs / 60;
                      timeInSecs = timeInSecs % 60;

                      if (minutes >= 60)
                      {
                          hours = minutes / 60;
                          minutes = minutes % 60;

                          if (hours >= 24)
                          {
                              days = hours / 24;
                              hours = hours % 24;
                          }
                      }
                  }

                  printf("%u Day%c : %u Hour%c : %u Minute%c : %u Second%c \n", 
                         days, 
                         days == 1 ? ' ' : 's',
                         hours, 
                         hours == 1 ? ' ' : 's',
                         minutes, 
                         minutes == 1 ? ' ' : 's',
                         timeInSecs,
                         timeInSecs == 1 ? ' ' : 's');
                  rc = 1;
              }
           }
           break;

      case LPMWN_GW_MSG_TYPE_GET_COORD_FREE_MEM_DESC_CNT:
           {
              if (pyldLen == 2)
              {
                  unsigned int cnt = __ntohs(buff_p + offset);
                  printf("%d \n", cnt);
                  rc = 1;
              }
           }
           break;

      case LPWMN_GW_MSG_TYPE_GET_NWK_ID:
           {
              if (pyldLen == 2)
              {
                  unsigned int nwkId = __ntohs(buff_p + offset);
                  printf("Network Id - 0x%x\n", nwkId);
                  rc = 1;
              }
           }
           break;

      case LPWMN_GW_MSG_TYPE_GET_RADIO_CARRIER_FREQ:
           {
              if (pyldLen == LPWMN_GW_MSG_RADIO_CARRIER_FREQ_FIELD_LEN)
              {
                  unsigned int u32 = __ntohl(buff_p + offset);
                  float rcf = u32;
                  rcf /= 1000000;
                  printf("%f MHz", rcf);
                  rc = 1;
              }
           }
           break;

      case LPWMN_GW_MSG_TYPE_GET_NWK_CHANNEL:
           {
              if (pyldLen == LPWMN_GW_MSG_RADIO_CHANN_FIELD_LEN)
              {
                  unsigned int chann = __ntohs(buff_p + offset);
                  printf("Radio Channel - %d\n", chann);
                  rc = 1;
              }
           }
           break;

      default:
          {
             printf("Message type<%d> not handled !! \n", currMsgType);
          }
          break;
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
int GW_buildSendHdr(int msgType, unsigned char *pyldBuff_p, int pyldLen)
{
   unsigned char *buff_p = serTxBuff;
   unsigned short calcCrc16;
   static unsigned char seqNr = 0x0;
   int rc;
  
   if (verbose) 
       printf("<%s> msgType<0x%x> \n", __FUNCTION__, msgType);

   __htons(buff_p, msgType);
   buff_p += UART_FRAME_HDR_MSG_TYPE_FIELD_LEN;

   *buff_p = 0x0;
   buff_p += UART_FRAME_HDR_FLAGS_FIELD_LEN;

   *buff_p = seqNr ++;
   buff_p += UART_FRAME_HDR_SEQ_NR_FIELD_LEN;

   __htons(buff_p, pyldLen);
   buff_p += UART_FRAME_HDR_PYLD_LEN_FIELD_LEN;

   calcCrc16 = crc16(serTxBuff, UART_FRAME_HDR_HDR_CRC_FIELD_OFF);
   __htons(buff_p, calcCrc16);  // no payload
   buff_p += UART_FRAME_HDR_HDR_CRC_FIELD_LEN;

   if (pyldLen > 0)
   {
       calcCrc16 = crc16(pyldBuff_p, pyldLen);
       __htons(buff_p, calcCrc16);  // payload crc 
   }
   else
       __htons(buff_p, 0x0);  // no payload 

   if (verbose)
   {
       int idx;
       
       printf("\n -------------------------- \n");

       for (idx=0; idx<UART_FRAME_HDR_LEN; idx++)
            printf(" 0x%02x ", serTxBuff[idx]);

       printf("\n -------------------------- \n");
   }

   rc = writePort(serTxBuff, UART_FRAME_HDR_LEN);
   if (rc != 1)
   {
       printf("\nwritePort() failed !!\n");
       rc = 20;
   }

   if (verbose) 
       printf("\nwritePort() done !!\n");

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
int GW_readSerIntf(int expMsgType, int tmoSecs)
{
   int rc = 0, off = 0, readLen, pyldLen, totReadLen = 0;
   int currMsgType = 0xffff, done = 0;
   time_t startTime;

   time(&startTime); 

   readLen = UART_FRAME_HDR_PYLD_CRC_FIELD_OFF;
   memset(serRxBuff, 0, sizeof(serRxBuff));

   do
   {
      if (verbose) 
          printf("\noff<%d>/readLen<%d>/totReadLen<%d> \n", off, readLen, totReadLen);

      if (tmoSecs > 0)
      {
          rc = readPortTmo(serRxBuff + off, readLen, tmoSecs*1000);
          if (tmoSecs > 0)
          {
              if (rc == 0)
              {
                  time_t currTime;
    
                  // No data read from port !!
                  time(&currTime);
                  if (currTime - startTime > tmoSecs)
                      return 100;
                  continue;
              }
          }
      }
      else
      {
          rc = readPort(serRxBuff + off, readLen);
      }

      if (rc != readLen)
      {
          printf("\nreadPort() failed !! ................. \n");
          close(uart_cntxt.serialFd);
          UC_FU_cntxt.nextEvt = UC_FU_EVT_GW_COMM_ERR;
          rc = 10;
          break;
      }

      totReadLen += readLen;
      off += readLen;

      switch (totReadLen)
      {
         case UART_FRAME_HDR_PYLD_CRC_FIELD_OFF: 
              {
                  int idx;
                  unsigned short calcCrc16, rxdCrc16;

                  // Get the message length

                  if (verbose)
                  {
                      printf("\nRead <%d> bytes --- ", readLen);
                      for (idx=0; idx<totReadLen; idx++)
                           printf(" <0x%x> ", serRxBuff[idx]);
                  }

                  calcCrc16 = crc16(serRxBuff, UART_FRAME_HDR_HDR_CRC_FIELD_OFF);
                  rxdCrc16 = serRxBuff[UART_FRAME_HDR_HDR_CRC_FIELD_OFF];
                  rxdCrc16 = (rxdCrc16 << 8) + serRxBuff[UART_FRAME_HDR_HDR_CRC_FIELD_OFF + 1];
      
                  if (verbose)
                      printf("\ncalc-crc16<0x%x> rcvd-crc16<0x%x>\n", calcCrc16, rxdCrc16);

                  if (calcCrc16 != rxdCrc16)
                  {
                      for (idx=0; idx<UART_FRAME_HDR_PYLD_CRC_FIELD_OFF-1; idx++)
                           serRxBuff[idx] = serRxBuff[idx+1];
                      off = UART_FRAME_HDR_PYLD_CRC_FIELD_OFF - 1;
                      readLen = 1;
                      totReadLen = off;
                      break;
                  }

                  pyldLen = serRxBuff[UART_FRAME_HDR_PYLD_LEN_FIELD_OFF];
                  pyldLen = (pyldLen << 8) |  serRxBuff[UART_FRAME_HDR_PYLD_LEN_FIELD_OFF + 1];

                  currMsgType = serRxBuff[UART_FRAME_HDR_MSG_TYPE_FIELD_OFF];
                  currMsgType = (currMsgType << 8) | serRxBuff[UART_FRAME_HDR_MSG_TYPE_FIELD_OFF + 1];

                  if (verbose)
                      printf("\nMessage Type<%d> / Length<%d>", currMsgType, pyldLen);                   

                  readLen = pyldLen + UART_FRAME_HDR_PYLD_CRC_FIELD_LEN;
              }
              break;

         default:
              {
                  if (currMsgType == expMsgType)
                  {
                      if (GW_processRcvdMsg(serRxBuff, UART_FRAME_HDR_LEN, currMsgType, pyldLen) == 1)
                          done = 1;
                  }
        
                  readLen = UART_FRAME_HDR_PYLD_CRC_FIELD_OFF;
                  memset(serRxBuff, 0, sizeof(serRxBuff));
                  totReadLen = 0;
                  off = 0;

                  if (tmoSecs > 0 && done == 0)
                  {
                      time_t currTime;
                      time(&currTime);
                      if (currTime - startTime > tmoSecs)
                          return 100;
                  }
              }
              break;
      }
   } while (done == 0x0);

   if (done == 1)
       rc = 0;
   else
       rc = 6;

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
int GW_bcFFDNoOp(unsigned int hopCnt)
{
   int rc, pyldLen;
   unsigned char *pyld_p;

   pyldLen = LPWMN_MAC_SHORT_ADDR_LEN + DIS_MSG_TYPE_SZ;

   pyld_p = (unsigned char *)malloc(pyldLen);
   if (pyld_p == NULL)
   {
       printf("malloc(%d) failed !! \n", pyldLen);
       return 1;
   }

   __htons(pyld_p, LPWMN_MESH_LEVEL_BROADCAST_ADDR | hopCnt);
   pyld_p[LPWMN_MAC_SHORT_ADDR_LEN] = DIS_MSG_TYPE_NO_OP;;

   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_RELAY_TO_NODE, 
                        pyld_p, pyldLen);
   if (rc != 1)
       return rc;
     
   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 2;
   }
      
   if (verbose)
       printf("Header acked \n");
   
   // Send payload
   rc = writePort(pyld_p, pyldLen);
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", 
              __FUNCTION__, pyldLen);
       return 3;
   }

   printf("Request sent ..... waiting for transmit confirmation \n");
   
   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_RELAY_TO_NODE, 0);

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
int GW_bcFFDReboot(unsigned int hopCnt, unsigned int delay)
{
   int rc, pyldLen, offset;
   unsigned char *pyld_p;

   pyldLen = LPWMN_MAC_SHORT_ADDR_LEN + DIS_MSG_TYPE_SZ + DIS_REBOOT_DELAY_TLV_SZ;

   pyld_p = (unsigned char *)malloc(pyldLen);
   if (pyld_p == NULL)
   {
       printf("malloc(%d) failed !! \n", pyldLen);
       return 1;
   }

   __htons(pyld_p, LPWMN_MESH_LEVEL_BROADCAST_ADDR | hopCnt);
   offset = LPWMN_MAC_SHORT_ADDR_LEN;
   pyld_p[offset++] = DIS_MSG_TYPE_DELAYED_REBOOT_NODE_REQ;
   pyld_p[offset++] = DIS_TLV_TYPE_REBOOT_DELAY;
   pyld_p[offset++] = DIS_REBOOT_DELAY_TLV_SZ - DIS_TLV_HDR_SZ;
   __htons(pyld_p + offset, delay);

   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_RELAY_TO_NODE, 
                        pyld_p, pyldLen);
   if (rc != 1)
       return rc;
     
   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 2;
   }
      
   if (verbose)
       printf("Header acked \n");
   
   // Send payload
   rc = writePort(pyld_p, pyldLen);
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", 
              __FUNCTION__, pyldLen);
       return 3;
   }

   printf("Request sent ..... waiting for transmit confirmation \n");
   
   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_RELAY_TO_NODE, 0);

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
int GW_rebootNodeReq(unsigned short shortAddr)
{
   int rc, pyldLen;
   unsigned char *pyld_p;

   pyldLen = LPWMN_MAC_SHORT_ADDR_LEN + DIS_MSG_TYPE_SZ;

   pyld_p = (unsigned char *)malloc(pyldLen);
   if (pyld_p == NULL)
   {
       printf("malloc(%d) failed !! \n", pyldLen);
       return 1;
   }

   __htons(pyld_p, shortAddr);
   pyld_p[LPWMN_MAC_SHORT_ADDR_LEN] = DIS_MSG_TYPE_NODE_REBOOT_REQ;
    
   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_RELAY_TO_NODE, 
                        pyld_p, pyldLen);
   if (rc != 1)
       return rc;

   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 2;
   }
      
   if (verbose)
       printf("Header acked \n");
   
   // Send payload
   rc = writePort(pyld_p, pyldLen);
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", 
              __FUNCTION__, pyldLen);
       return 3;
   }

   printf("Request sent ..... \n");

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
int GW_rebootCoordReq(void)
{
   int rc;

   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_REBOOT_COORD, NULL, 0x0);
   if (rc != 1)
   {
       printf("failed !! \n");
       rc = 5;
   }
   else
       printf("Request sent ... \n");

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
int GW_startChannMon(int sampleCnt, int samplingInterval)
{
   int rc = 1;
   unsigned char pyld[8];

   __htonl(pyld, sampleCnt);
   __htonl(pyld + 4, samplingInterval);
   
   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_MON_CHANN, 
                        pyld, sizeof(pyld));
   if (rc != 1)
       return rc;

   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 41;
   }
      
   if (verbose)
       printf("Header acked \n");

   // Send payload
   rc = writePort(pyld, sizeof(pyld));
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", __FUNCTION__, sizeof(pyld));
       return 42;
   }

   printf("Request sent. Waiting for response ..... \n");
                      
   expDisMsgType = 0xff;
   expDisMsgSrcShortAddr = LPWMN_BROADCAST_SHORT_ADDR;
   
   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_MON_CHANN, 0);

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
int GW_startCoordReq(void)
{
   int rc;

   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_START_COORD, NULL, 0x0);
   if (rc != 1)
   {
       printf("failed !! \n");
       return 5;
   }

   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_START_COORD, 0); 

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
int GW_getNodeCntReqHndlr(void)
{
   int rc;
   
   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_NODE_CNT_REQ, NULL, 0x0);
   if (rc != 1)
   {
       printf("failed !! \n");
       return 5;
   }

   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_NODE_CNT_REQ, 0); 

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
int GW_getCoordUpTime(void)
{
   int rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_GET_COORD_UP_TIME, NULL, 0x0);
   if (rc != 1)
       return 5;

   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_GET_COORD_UP_TIME, 0);

   return rc;
}
        

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
int GW_getCoordFreeMemDescCnt(void)
{
   int rc = GW_buildSendHdr(LPMWN_GW_MSG_TYPE_GET_COORD_FREE_MEM_DESC_CNT, NULL, 0x0);
   if (rc != 1)
       return 5;

   rc = GW_readSerIntf(LPMWN_GW_MSG_TYPE_GET_COORD_FREE_MEM_DESC_CNT, 0);

   return rc;
}
        

/*
 ********************************************************************
 *
 *
 *
 ********************************************************************
 */
int GW_getLPWMNIdReqHndlr(void)
{
   int rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_GET_NWK_ID, NULL, 0x0);
   if (rc != 1)
       return 5;

   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_GET_NWK_ID, 0);

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
int GW_getRadioFreqBand(void)
{
   int rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_GET_RADIO_FREQ_BAND, NULL, 0x0);
   if (rc != 1)
       return 5;

   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_GET_RADIO_FREQ_BAND, 0);

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
int GW_getRadioBaudRate(void)
{
   int rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_GET_RADIO_BAUD_RATE, NULL, 0x0);
   if (rc != 1)
       return 5;

   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_GET_RADIO_BAUD_RATE, 0);

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
int GW_getRadioModFmt(void)
{
   int rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_GET_RADIO_MOD_FMT, NULL, 0x0);
   if (rc != 1)
       return 5;

   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_GET_RADIO_MOD_FMT, 0);

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
int GW_getCoordRadioTxPwr( )
{
   int rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_GET_RADIO_TX_PWR, NULL, 0x0);
   if (rc != 1)
       return 5;

   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_GET_RADIO_TX_PWR, 0);

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
int GW_getRadioCarrierFreq( )
{
   int rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_GET_RADIO_CARRIER_FREQ, NULL, 0x0);
   if (rc != 1)
       return 5;

   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_GET_RADIO_CARRIER_FREQ, 0);

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
int GW_getRadioChannReqHndlr( )
{
   int rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_GET_NWK_CHANNEL, NULL, 0x0);
   if (rc != 1)
       return 5;

   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_GET_NWK_CHANNEL, 0);

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
int GW_setRadioChannReqHndlr(const char *arg_p)
{
   int rc, channNr;
   unsigned char pyldBuff[LPWMN_GW_MSG_RADIO_CHANN_FIELD_LEN];

   rc = sscanf(arg_p, "%d", &channNr);
   if (rc != 1 || channNr < 0 || channNr > 31)
   {
       printf("You have entered an invalid channel number !! \n");
       return 60; 
   }

   __htons(pyldBuff, channNr);

   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_SET_RADIO_CHANNEL, 
                        pyldBuff, LPWMN_GW_MSG_RADIO_CHANN_FIELD_LEN);
   if (rc != 1)
       return rc;

   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 61;
   }
      
   if (verbose)
       printf("Header acked \n");

   // Send payload
   rc = writePort(pyldBuff, LPWMN_GW_MSG_RADIO_CHANN_FIELD_LEN);
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", 
              __FUNCTION__, LPWMN_GW_MSG_RADIO_CHANN_FIELD_LEN);
       return 62;
   }

   if (verbose)
       printf("Request sent ... \n");
       
   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_SET_RADIO_CHANNEL, 0);

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
int GW_getBlackListReqHndlr( )
{
   int nodeCnt = 0, rc = 0, nodeIdx = 0, calcCrc16, pyldLen = 2;
   unsigned char pyldBuff[2];

   allBLEntriesListed = 0;
   
   do
   {
      __htons(pyldBuff, nodeIdx);
      calcCrc16 = crc16(pyldBuff, pyldLen);

      rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_GET_BLACK_LIST_ENTRY, pyldBuff, pyldLen);
      if (rc != 1)
          return 5;

      rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
      if (rc != 0)
          break;

      if (hdrAcked == 0x0)
      {
          printf("Header not acked !! \n");
          rc = 31;
          break;
      }

      if (verbose)
          printf("Header acked \n");

      // Send payload
      rc = writePort(pyldBuff, pyldLen);
      if (rc != 1)
      {
          rc = 20;
          printf("<%s> writePort(%d) failed !! \n", __FUNCTION__, pyldLen);
          break;
      }

      rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_GET_BLACK_LIST_ENTRY, 0);
      if (rc != 0)
          break;
                      
      if (allBLEntriesListed)
          break; 

      nodeCnt ++;

      nodeIdx = listTblIdx + 1;

   } while (1);

   if (nodeCnt == 0)
       printf("The Black list is empty !! \n");

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
int GW_getWhiteListReqHndlr( )
{
   int nodeCnt = 0, rc = 0, nodeIdx = 0, calcCrc16, pyldLen = 2;
   unsigned char pyldBuff[2];

   allWLEntriesListed = 0;
   
   do
   {
      __htons(pyldBuff, nodeIdx);
      calcCrc16 = crc16(pyldBuff, pyldLen);

      rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_GET_WHITE_LIST_ENTRY, pyldBuff, pyldLen);
      if (rc != 1)
          return 5;

      rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
      if (rc != 0)
          break;

      if (hdrAcked == 0x0)
      {
          printf("Header not acked !! \n");
          rc = 31;
          break;
      }

      if (verbose)
          printf("Header acked \n");

      // Send payload
      rc = writePort(pyldBuff, pyldLen);
      if (rc != 1)
      {
          rc = 20;
          printf("<%s> writePort(%d) failed !! \n", __FUNCTION__, pyldLen);
          break;
      }

      rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_GET_WHITE_LIST_ENTRY, 0);
      if (rc != 0)
          break;
                      
      if (allWLEntriesListed)
          break; 

      nodeCnt ++;

      nodeIdx = listTblIdx + 1;

   } while (1);

   if (nodeCnt == 0)
       printf("The white list is empty !! \n");

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
int GW_getNodeListReqHndlr(unsigned char reqType)
{
   int nodeCnt = 0, rc = 0, nodeIdx = 0, calcCrc16, pyldLen = 2;
   unsigned char pyldBuff[2];

   allNLEntriesListed = 0;

   GW_nodeListReqType = reqType;

   do
   {
      __htons(pyldBuff, nodeIdx);
      // calcCrc16 = crc16(pyldBuff, pyldLen);

      rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_NODE_LIST_REQ, pyldBuff, pyldLen);
      if (rc != 1)
          return 5;

      rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
      if (rc != 0)
          break;

      if (hdrAcked == 0x0)
      {
          printf("Header not acked !! \n");
          rc = 21;
          break;
      }
      
      if (verbose)
          printf("Header acked \n");

      // Send payload
      rc = writePort(pyldBuff, pyldLen);
      if (rc != 1)
      {
          rc = 20;
          printf("<%s> writePort(%d) failed !! \n", __FUNCTION__, pyldLen);
          break;
      }
       
      rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_NODE_LIST_REQ, 0);
      if (rc != 0)
          break;
                      
      if (allNLEntriesListed)
          break; 

      nodeCnt ++;

      nodeIdx = listTblIdx + 1;

   } while (1);

   if (nodeCnt == 0)
       printf("There are no nodes in the network !! \n");

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
int GW_getCoordExtAddrReqHndlr(void)
{
   int rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_GET_NWK_COORD_EXT_ADDR, NULL, 0);
   if (rc != 1)
       return 5;

   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_GET_NWK_COORD_EXT_ADDR, 0);

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
int GW_getCoordFwBuildDate(void) 
{
   int rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_GET_COORD_SW_BUILD_DATE, NULL, 0);
   if (rc != 1)
       return 5;

   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_GET_COORD_SW_BUILD_DATE, 0);

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
int GW_getCoordFwBuildTime(void) 
{
   int rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_GET_COORD_SW_BUILD_TIME, NULL, 0);
   if (rc != 1)
       return 5;

   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_GET_COORD_SW_BUILD_TIME, 0);

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
int GW_getCoordRtTblValidEntryCnt(void)
{
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
int GW_getCoordRadioPartNr(void)
{
   int rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_GET_RADIO_PART_NR, NULL, 0);
   if (rc != 1)
       return 5;

   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_GET_RADIO_PART_NR, 0);

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
int GW_enableNwkJoins(void)
{
   int rc;
  
   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_ENABLE_NWK_JOINS, NULL, 0);
   if (rc != 1)
   {
       printf("failed !!\n");
       return 5;
   }
   else
       printf("done \n");
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
int GW_disableNwkJoins(void)
{
   int rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_DISABLE_NWK_JOINS, NULL, 0);
   if (rc != 1)
   {
       printf("failed !!\n");
       return 5;
   }
   else
       printf("done \n");

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
int GW_getNwkJoinCtrlSts( )
{
   int rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_GET_NWK_JOIN_CTRL_STATE, NULL, 0);
   if (rc != 1)
       return 5;

   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_GET_NWK_JOIN_CTRL_STATE, 0);

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
int GW_getNTEntryOnFFD(unsigned int ffdSA, unsigned int nteSA)
{
   int rc, pyldLen, off = 0;
   unsigned char *pyld_p;

   pyldLen = LPWMN_MAC_SHORT_ADDR_LEN + DIS_MSG_TYPE_SZ + DIS_SHORT_ADDR_TLV_SZ; 

   pyld_p = (unsigned char *)malloc(pyldLen);
   if (pyld_p == NULL)
   {
       printf("malloc(%d) failed !! \n", pyldLen);
       return 1;
   }

   __htons(pyld_p, ffdSA);
   off = LPWMN_MAC_SHORT_ADDR_LEN;
   pyld_p[off ++] = DIS_MSG_TYPE_GET_NEIGHBOR_TBL_ENTRY;
   pyld_p[off ++] = DIS_TLV_TYPE_SHORT_ADDR;
   pyld_p[off ++] = LPWMN_MAC_SHORT_ADDR_LEN;
   __htons(pyld_p + off, nteSA);
    
   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_RELAY_TO_NODE, 
                        pyld_p, pyldLen);
   if (rc != 1)
       return rc;
      
   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 2;
   }
      
   if (verbose)
       printf("Header acked \n");
   
   // Send payload
   rc = writePort(pyld_p, pyldLen);
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", 
              __FUNCTION__, pyldLen);
       return 3;
   }

   printf("Request sent. Waiting for response ..... \n");
                      
   expDisMsgType = DIS_MSG_TYPE_GET_NEIGHBOR_TBL_ENTRY;
   expDisMsgSrcShortAddr = ffdSA;
   
   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_RELAY_FROM_NODE, 0);

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
int GW_getRtNextHopOnFFD(unsigned int ffdSA, unsigned int rtToSA)
{
   int rc, pyldLen, off = 0;
   unsigned char *pyld_p;

   pyldLen = LPWMN_MAC_SHORT_ADDR_LEN + DIS_MSG_TYPE_SZ + DIS_SHORT_ADDR_TLV_SZ; 

   pyld_p = (unsigned char *)malloc(pyldLen);
   if (pyld_p == NULL)
   {
       printf("malloc(%d) failed !! \n", pyldLen);
       return 1;
   }

   __htons(pyld_p, ffdSA);
   off = LPWMN_MAC_SHORT_ADDR_LEN;
   pyld_p[off ++] = DIS_MSG_TYPE_GET_RT_NEXT_HOP;
   pyld_p[off ++] = DIS_TLV_TYPE_SHORT_ADDR;
   pyld_p[off ++] = LPWMN_MAC_SHORT_ADDR_LEN;
   __htons(pyld_p + off, rtToSA);
    
   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_RELAY_TO_NODE, 
                        pyld_p, pyldLen);
   if (rc != 1)
       return rc;
      
   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 2;
   }
      
   if (verbose)
       printf("Header acked \n");
   
   // Send payload
   rc = writePort(pyld_p, pyldLen);
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", 
              __FUNCTION__, pyldLen);
       return 3;
   }

   printf("Request sent. Waiting for response ..... \n");
                      
   expDisMsgType = DIS_MSG_TYPE_GET_RT_NEXT_HOP;
   expDisMsgSrcShortAddr = ffdSA;
   
   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_RELAY_FROM_NODE, 0);

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
int GW_getRtNextHopOnCoord(unsigned int shortAddr)
{
   int rc, pyldLen, off = 0;
   unsigned char *pyld_p;

   pyldLen = LPWMN_MAC_SHORT_ADDR_LEN;

   pyld_p = (unsigned char *)malloc(pyldLen);
   if (pyld_p == NULL)
   {
       printf("malloc(%d) failed !! \n", pyldLen);
       return 1;
   }

   __htons(pyld_p, shortAddr);
    
   rc = GW_buildSendHdr(LPWMN_MSG_TYPE_GET_RT_NH_ON_COORD, 
                        pyld_p, pyldLen);
   if (rc != 1)
       return rc;
      
   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 2;
   }
      
   if (verbose)
       printf("Header acked \n");
   
   // Send payload
   rc = writePort(pyld_p, pyldLen);
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", 
              __FUNCTION__, pyldLen);
       return 3;
   }

   printf("Request sent. Waiting for response ..... \n");
                      
   expDisMsgType = 0xff;
   expDisMsgSrcShortAddr = LPWMN_BROADCAST_SHORT_ADDR;
   
   rc = GW_readSerIntf(LPWMN_MSG_TYPE_GET_RT_NH_ON_COORD, 0);
       
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
int GW_setNodeAttrVal(unsigned int shortAddr, 
                       unsigned int attrId, 
                       unsigned int attrVal)
{
   int rc = 1, pyldLen, off = 0, attrValLen = 0;
   unsigned char *pyld_p;
  
   pyldLen = LPWMN_MAC_SHORT_ADDR_LEN
             + DIS_MSG_TYPE_SZ
             + DIS_TLV_HDR_SZ
             + DIS_ATTR_ID_TLV_SZ
             + DIS_TLV_HDR_SZ;

   switch (attrId)
   {
      case NM_MAC_ACK_TMO_DELTA_ATTR_ID:
           attrValLen = 2;
           break;

      case NM_MAC_TX_ON_CCA_ENA_FLAG_ATTR_ID:
           attrValLen = 1;
           break;

      case MAC_LPWMN_ID_ATTR_ID:
           attrValLen = 2;
           break;
      
      case MESH_TRIGGER_PATH_DISC_ATTR_ID:
           attrValLen = 2;
           break;

      default:
           rc = 0;
           printf("attribute id <%u> not supported !! \n", attrId);
           break;
   }

   if (rc == 0)
       return rc;
           
   pyldLen += attrValLen;

   pyld_p = (unsigned char *)malloc(pyldLen);
   if (pyld_p == NULL)
   {
       printf("malloc(%d) failed !! \n", pyldLen);
       return 1;
   }

   __htons(pyld_p, shortAddr);
   off = LPWMN_MAC_SHORT_ADDR_LEN;
   pyld_p[off ++] = DIS_MSG_TYPE_SET_ATTR_VAL;
   pyld_p[off ++] = DIS_TLV_TYPE_ATTR_INFO;
   pyld_p[off ++] = DIS_ATTR_ID_TLV_SZ + DIS_TLV_HDR_SZ + attrValLen;
   pyld_p[off ++] = DIS_TLV_TYPE_ATTR_ID;
   pyld_p[off ++] = DIS_ATTR_ID_FIELD_SZ;
   __htons(pyld_p + off, attrId);
   off += DIS_ATTR_ID_FIELD_SZ;
   pyld_p[off ++] = DIS_TLV_TYPE_ATTR_VAL;
   pyld_p[off ++] = attrValLen;
   switch (attrValLen)
   {
      case 1:
          pyld_p[off] = attrVal;
          break;

      case 2:
          __htons(pyld_p + off, attrVal);
          break;

      case 4:
          __htonl(pyld_p + off, attrVal);
          break;

      default:
          break;
   } 

   off += attrValLen;
   
   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_RELAY_TO_NODE, 
                        pyld_p, pyldLen);
   if (rc != 1)
       return rc;
      
   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 2;
   }
      
   if (verbose)
       printf("Header acked \n");
   
   // Send payload
   rc = writePort(pyld_p, pyldLen);
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", 
              __FUNCTION__, pyldLen);
       return 3;
   }

   printf("Request sent. Waiting for response ..... \n");

   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_SET_COORD_ATTR_VAL, 0);

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
int GW_setCoordAttrVal(unsigned int shortAddr, 
                       unsigned int attrId, 
                       unsigned int attrVal)
{
   int rc = 1, pyldLen, off = 0;
   unsigned char *pyld_p;

   // printf("\n attr-id<%u> / attr-val<%u> \n", attrId, attrVal);

   pyldLen = DIS_ATTR_ID_FIELD_SZ;

   switch (attrId)
   {
      case NM_MAC_ACK_TMO_DELTA_ATTR_ID:
           pyldLen += 2;
           break;

      case MAC_LPWMN_ID_ATTR_ID:
           pyldLen += 2;
           break;

      case PHY_RF_CHANN_ID_ATTR_ID:
           pyldLen += 1;
           break;

      case MESH_TRIGGER_PATH_DISC_ATTR_ID:
           pyldLen += 2;
           break;


      default:
           rc = 0;
           printf("attribute id <%u> not supported !! \n", attrId);
           break;
   }

   if (rc == 0)
       return rc;

   pyld_p = (unsigned char *)malloc(pyldLen);
   if (pyld_p == NULL)
   {
       printf("malloc(%d) failed !! \n", pyldLen);
       return 1;
   }

   __htons(pyld_p, attrId);

   switch (attrId)
   {
      case NM_MAC_ACK_TMO_DELTA_ATTR_ID:
      case MAC_LPWMN_ID_ATTR_ID:
      case MESH_TRIGGER_PATH_DISC_ATTR_ID:
           __htons(pyld_p + DIS_ATTR_ID_FIELD_SZ, attrVal);
           break;

      case PHY_RF_CHANN_ID_ATTR_ID:
           *(pyld_p + DIS_ATTR_ID_FIELD_SZ) = attrVal;
           break;

      default:
           break;
   }

   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_SET_COORD_ATTR_VAL, 
                        pyld_p, pyldLen);
   if (rc != 1)
       return rc;
      
   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 2;
   }
      
   if (verbose)
       printf("Header acked \n");
   
   // Send payload
   rc = writePort(pyld_p, pyldLen);
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", 
              __FUNCTION__, pyldLen);
       return 3;
   }

   if (attrId == MESH_TRIGGER_PATH_DISC_ATTR_ID)
   {
       printf("Request sent. Waiting for response ..... \n");

       rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_EVENT, 0);
   }
   else
   {
       printf("Request sent ... \n");
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
int GW_cfgCoordToDropBcnReqs(void)
{
   int rc;
  
   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_CFG_DROP_BCN_REQS, NULL, 0);
   if (rc != 1)
   {
       printf("failed !!\n");
       return 5;
   }
   else
       printf("done \n");

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
int GW_cfgNodeToAcceptBcnReqs(unsigned int shortAddr)
{
   int rc, pyldLen;
   unsigned char *pyld_p;

   pyldLen = LPWMN_MAC_SHORT_ADDR_LEN + DIS_MSG_TYPE_SZ;

   pyld_p = (unsigned char *)malloc(pyldLen);
   if (pyld_p == NULL)
   {
       printf("malloc(%d) failed !! \n", pyldLen);
       return 1;
   }

   __htons(pyld_p, shortAddr);
   pyld_p[LPWMN_MAC_SHORT_ADDR_LEN] = DIS_MSG_TYPE_CFG_NODE_ACCEPT_BCN_REQS;
    
   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_RELAY_TO_NODE, 
                        pyld_p, pyldLen);
   if (rc != 1)
       return rc;

   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 2;
   }
      
   if (verbose)
       printf("Header acked \n");
   
   // Send payload
   rc = writePort(pyld_p, pyldLen);
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", 
              __FUNCTION__, pyldLen);
       return 3;
   }

   printf("Request sent ..... \n");

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
int GW_cfgNodeToDropBcnReqs(unsigned int shortAddr)
{
   int rc, pyldLen;
   unsigned char *pyld_p;

   pyldLen = LPWMN_MAC_SHORT_ADDR_LEN + DIS_MSG_TYPE_SZ;

   pyld_p = (unsigned char *)malloc(pyldLen);
   if (pyld_p == NULL)
   {
       printf("malloc(%d) failed !! \n", pyldLen);
       return 1;
   }

   __htons(pyld_p, shortAddr);
   pyld_p[LPWMN_MAC_SHORT_ADDR_LEN] = DIS_MSG_TYPE_CFG_NODE_DROP_BCN_REQS;
    
   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_RELAY_TO_NODE, 
                        pyld_p, pyldLen);
   if (rc != 1)
       return rc;

   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 2;
   }
      
   if (verbose)
       printf("Header acked \n");
   
   // Send payload
   rc = writePort(pyld_p, pyldLen);
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", 
              __FUNCTION__, pyldLen);
       return 3;
   }

   printf("Request sent ..... \n");

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
int GW_cfgCoordToAcceptBcnReqs(void)
{
   int rc;
  
   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_CFG_ACCEPT_BCN_REQS, NULL, 0);
   if (rc != 1)
   {
       printf("failed !!\n");
       return 5;
   }
   else
       printf("done \n");

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
int GW_getCoordBcnReqHndlingCfg(void)
{
   int rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_GET_BCN_REQ_HANDLING_CFG, 
                            NULL, 0);
   if (rc != 1)
       return 5;

   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_GET_BCN_REQ_HANDLING_CFG, 0);

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
int GW_getNodeBcnReqHndlingCfg(unsigned int shortAddr)
{
   int rc, pyldLen;
   unsigned char *pyld_p;

   pyldLen = LPWMN_MAC_SHORT_ADDR_LEN + DIS_MSG_TYPE_SZ;

   pyld_p = (unsigned char *)malloc(pyldLen);
   if (pyld_p == NULL)
   {
       printf("malloc(%d) failed !! \n", pyldLen);
       return 1;
   }

   __htons(pyld_p, shortAddr);
   pyld_p[LPWMN_MAC_SHORT_ADDR_LEN] = DIS_MSG_TYPE_NODE_BCN_REQ_HANDLING_CFG;
    
   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_RELAY_TO_NODE, 
                        pyld_p, pyldLen);
   if (rc != 1)
       return rc;

   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 2;
   }
      
   if (verbose)
       printf("Header acked \n");
   
   // Send payload
   rc = writePort(pyld_p, pyldLen);
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", 
              __FUNCTION__, pyldLen);
       return 3;
   }

   printf("Request sent. Waiting for response ..... \n");
                      
   expDisMsgType = DIS_MSG_TYPE_NODE_BCN_REQ_HANDLING_CFG;
   expDisMsgSrcShortAddr = shortAddr;
   
   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_RELAY_FROM_NODE, 0);


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
int GW_getCoordListAttrVal(unsigned int attrId, unsigned int attrIdx)
{
   int rc = 0, pyldLen, off = 0;
   unsigned char *pyld_p;
  
   // TODO
 
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
int GW_getCoordAttrVal(unsigned int attrId)
{
   int rc, pyldLen, off = 0;
   unsigned char *pyld_p;

   pyldLen = DIS_ATTR_ID_FIELD_SZ;

   pyld_p = (unsigned char *)malloc(pyldLen);
   if (pyld_p == NULL)
   {
       printf("malloc(%d) failed !! \n", pyldLen);
       return 1;
   }

   __htons(pyld_p, attrId);

   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_GET_COORD_ATTR_VAL, 
                        pyld_p, pyldLen);
   if (rc != 1)
       return rc;
      
   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 2;
   }
      
   if (verbose)
       printf("Header acked \n");
   
   // Send payload
   rc = writePort(pyld_p, pyldLen);
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", 
              __FUNCTION__, pyldLen);
       return 3;
   }

   printf("Request sent. Waiting for response ..... \n");

   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_GET_COORD_ATTR_VAL, 0);

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
int GW_getNodeListAttrVal(unsigned int shortAddr, 
                          unsigned int attrId, 
                          unsigned int attrIdx)
{
   int rc, pyldLen, off = 0;
   unsigned char *pyld_p;

   if (1) //verbose)
       printf("<%s> <%d / %d / %d> \n", __FUNCTION__, shortAddr, attrId, attrIdx);

   pyldLen = LPWMN_MAC_SHORT_ADDR_LEN
             + DIS_MSG_TYPE_SZ
             + DIS_TLV_HDR_SZ
             + DIS_LIST_ATTR_ID_TLV_SZ 
             + DIS_LIST_ATTR_IDX_TLV_SZ;

   pyld_p = (unsigned char *)malloc(pyldLen);
   if (pyld_p == NULL)
   {
       printf("malloc(%d) failed !! \n", pyldLen);
       return 1;
   }

   __htons(pyld_p, shortAddr);
   off = LPWMN_MAC_SHORT_ADDR_LEN;
   pyld_p[off ++] = DIS_MSG_TYPE_GET_LIST_ATTR_VAL;
   pyld_p[off ++] = DIS_TLV_TYPE_LIST_ATTR_INFO;
   pyld_p[off ++] = DIS_LIST_ATTR_ID_TLV_SZ + DIS_LIST_ATTR_IDX_TLV_SZ;

   pyld_p[off ++] = DIS_TLV_TYPE_LIST_ATTR_ID;
   pyld_p[off ++] = DIS_LIST_ATTR_ID_FIELD_SZ;
   __htons(pyld_p + off, attrId);
   off += DIS_LIST_ATTR_ID_FIELD_SZ;
   
   pyld_p[off ++] = DIS_TLV_TYPE_LIST_ATTR_IDX;
   pyld_p[off ++] = DIS_LIST_ATTR_IDX_FIELD_SZ;
   __htons(pyld_p + off, attrIdx);

   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_RELAY_TO_NODE, 
                        pyld_p, pyldLen);
   if (rc != 1)
       return rc;
      
   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 2;
   }
      
   if (verbose)
       printf("Header acked \n");
   
   // Send payload
   rc = writePort(pyld_p, pyldLen);
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", 
              __FUNCTION__, pyldLen);
       return 3;
   }

   printf("Request sent. Waiting for response ..... \n");
                      
   expDisMsgType = DIS_MSG_TYPE_GET_LIST_ATTR_VAL;
   expDisMsgSrcShortAddr = shortAddr;
   
   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_RELAY_FROM_NODE, 0);

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
int GW_getNodeAttrVal(unsigned int shortAddr, unsigned int attrId)
{
   int rc, pyldLen, off = 0;
   unsigned char *pyld_p;
   
   pyldLen = LPWMN_MAC_SHORT_ADDR_LEN
             + DIS_MSG_TYPE_SZ
             + DIS_ATTR_ID_TLV_SZ;

   pyld_p = (unsigned char *)malloc(pyldLen);
   if (pyld_p == NULL)
   {
       printf("malloc(%d) failed !! \n", pyldLen);
       return 1;
   }

   __htons(pyld_p, shortAddr);
   off = LPWMN_MAC_SHORT_ADDR_LEN;
   pyld_p[off ++] = DIS_MSG_TYPE_GET_ATTR_VAL;
   pyld_p[off ++] = DIS_TLV_TYPE_ATTR_ID;
   pyld_p[off ++] = DIS_ATTR_ID_FIELD_SZ;
   __htons(pyld_p + off, attrId);

   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_RELAY_TO_NODE, 
                        pyld_p, pyldLen);
   if (rc != 1)
       return rc;
      
   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 2;
   }
      
   if (verbose)
       printf("Header acked \n");
   
   // Send payload
   rc = writePort(pyld_p, pyldLen);
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", 
              __FUNCTION__, pyldLen);
       return 3;
   }

   printf("Request sent. Waiting for response ..... \n");
                      
   expDisMsgType = DIS_MSG_TYPE_GET_ATTR_VAL;
   expDisMsgSrcShortAddr = shortAddr;
   
   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_RELAY_FROM_NODE, 0);

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
int GW_cfgNodeDataPushInterval(unsigned int shortAddr, unsigned int pi)
{
   int rc, pyldLen, off = 0;
   unsigned char *pyld_p;

   pyldLen = LPWMN_MAC_SHORT_ADDR_LEN
             + DIS_MSG_TYPE_SZ
             + DIS_TLV_HDR_SZ   // DIS_TLV_TYPE_PUSH_INTERVAL
             + LPWMN_GW_MSG_NODE_DATA_PUSH_INTERVAL_FIELD_LEN;

   pyld_p = (unsigned char *)malloc(pyldLen);
   if (pyld_p == NULL)
   {
       printf("malloc(%d) failed !! \n", pyldLen);
       return 1;
   }

   __htons(pyld_p, shortAddr);
   off = LPWMN_MAC_SHORT_ADDR_LEN;
   pyld_p[off ++] = DIS_MSG_TYPE_CFG_NODE_DATA_PUSH_INTERVAL;
   pyld_p[off ++] = DIS_TLV_TYPE_PUSH_INTERVAL;
   pyld_p[off ++] = LPWMN_GW_MSG_NODE_DATA_PUSH_INTERVAL_FIELD_LEN;
   __htons(pyld_p + off, pi);
    
   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_RELAY_TO_NODE, 
                        pyld_p, pyldLen);
   if (rc != 1)
       return rc;
      
   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 2;
   }
      
   if (verbose)
       printf("Header acked \n");
   
   // Send payload
   rc = writePort(pyld_p, pyldLen);
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", 
              __FUNCTION__, pyldLen);
       return 3;
   }

   printf("Request sent ... \n");
       
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
int GW_clrBlackList( )
{
   int rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_DELETE_BLACK_LIST, NULL, 0);
   if (rc != 1)
       return 5;
   printf("Request sent ... \n");
}
        

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
int GW_delNodeFromBlackList(const char *arg_p)
{
   unsigned int eui64[LPWMN_EXT_ADDR_LEN];
   unsigned char eui64_8[LPWMN_EXT_ADDR_LEN];
   int rc, idx;

   rc = sscanf(arg_p, "%x:%x:%x:%x:%x:%x:%x:%x", 
                &eui64[0], &eui64[1], &eui64[2], &eui64[3], 
                &eui64[4], &eui64[5], &eui64[6], &eui64[7]);
   if (rc != LPWMN_EXT_ADDR_LEN)
   {
       rc = sscanf(arg_p, "0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x", 
                   &eui64[0], &eui64[1], &eui64[2], &eui64[3], 
                   &eui64[4], &eui64[5], &eui64[6], &eui64[7]);
       if (rc != LPWMN_EXT_ADDR_LEN)
       {
           printf("You have entered an invalid eui64 address !! \n");
           return 10;
       }
   }

   for (idx=0; idx<LPWMN_EXT_ADDR_LEN; idx++)
   {
      if (eui64[idx] > 255)
      {
          printf("You have entered an invalid eui64 address !! \n");
          return 11;
      }
      eui64_8[idx] = eui64[idx];
   }
   
   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_DEL_NODE_FROM_BLACK_LIST, 
                        eui64_8, LPWMN_EXT_ADDR_LEN);
   if (rc != 1)
       return rc;

   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 41;
   }
      
   if (verbose)
       printf("Header acked \n");

   // Send payload
   rc = writePort(eui64_8, LPWMN_EXT_ADDR_LEN);
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", __FUNCTION__, LPWMN_EXT_ADDR_LEN);
       return 42;
   }

   printf("Request sent ... \n");
       
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
int GW_clrWhiteList( )
{
   int rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_DELETE_WHITE_LIST, NULL, 0);
   if (rc != 1)
       return 5;
   printf("Request sent ... \n");
}
        

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
int GW_delNodeFromWhiteList(const char *arg_p)
{
   unsigned int eui64[LPWMN_EXT_ADDR_LEN];
   unsigned char eui64_8[LPWMN_EXT_ADDR_LEN];
   int rc, idx;

   rc = sscanf(arg_p, "%x:%x:%x:%x:%x:%x:%x:%x", 
                &eui64[0], &eui64[1], &eui64[2], &eui64[3], 
                &eui64[4], &eui64[5], &eui64[6], &eui64[7]);
   if (rc != LPWMN_EXT_ADDR_LEN)
   {
       rc = sscanf(arg_p, "0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x", 
                   &eui64[0], &eui64[1], &eui64[2], &eui64[3], 
                   &eui64[4], &eui64[5], &eui64[6], &eui64[7]);
       if (rc != LPWMN_EXT_ADDR_LEN)
       {
           printf("You have entered an invalid eui64 address !! \n");
           return 10;
       }
   }

   for (idx=0; idx<LPWMN_EXT_ADDR_LEN; idx++)
   {
      if (eui64[idx] > 255)
      {
          printf("You have entered an invalid eui64 address !! \n");
          return 11;
      }
      eui64_8[idx] = eui64[idx];
   }
   
   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_DEL_NODE_FROM_WHITE_LIST, 
                        eui64_8, LPWMN_EXT_ADDR_LEN);
   if (rc != 1)
       return rc;

   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 41;
   }
      
   if (verbose)
       printf("Header acked \n");

   // Send payload
   rc = writePort(eui64_8, LPWMN_EXT_ADDR_LEN);
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", __FUNCTION__, LPWMN_EXT_ADDR_LEN);
       return 42;
   }

   printf("Request sent ... \n");
       
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
int GW_getNodeLastFEId(const char *arg_p)
{
   int rc, pyldLen, off = 0;
   unsigned char *pyld_p;
   unsigned int shortAddr;

   rc = sscanf(arg_p, "%u", &shortAddr);
   if (rc != 1 || shortAddr < 0x1 || shortAddr > LPWMN_MAX_UNICAST_SHORT_ADDR)
   {
       rc = sscanf(arg_p, "0x%x", &shortAddr);
       if (rc != 1 || shortAddr < 0x1 || shortAddr > LPWMN_MAX_UNICAST_SHORT_ADDR)
       {
           printf("please enter a valid short address (> 1 & <= %u) !! \n", 
                  LPWMN_MAX_UNICAST_SHORT_ADDR);
           return 9;
       }
   } 

   // printf("Short addr <%u> \n", shortAddr);

   pyldLen = LPWMN_MAC_SHORT_ADDR_LEN;

   pyld_p = (unsigned char *)malloc(pyldLen);
   if (pyld_p == NULL)
   {
       printf("malloc(%d) failed !! \n", pyldLen);
       return 1;
   }

   __htons(pyld_p, shortAddr);
   off = LPWMN_MAC_SHORT_ADDR_LEN;
    
   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_GET_NODE_LAST_FATAL_ERR_ID, 
                        pyld_p, pyldLen);
   if (rc != 1)
       return rc;
      
   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 2;
   }
      
   if (verbose)
       printf("Header acked \n");
   
   // Send payload
   rc = writePort(pyld_p, pyldLen);
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", 
              __FUNCTION__, pyldLen);
       return 3;
   }

   printf("Request sent. Waiting for response ..... \n");
                      
   expDisMsgType = 0xff;
   expDisMsgSrcShortAddr = LPWMN_BROADCAST_SHORT_ADDR;
   
   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_GET_NODE_LAST_FATAL_ERR_ID, 0);

   return rc;
}
   

#define SPARSH_PANEL_CONTROL_MSG_LEN  12
#define SPARSH_PANEL_CONTROL_LIGHT_GP_ID  250
#define SPARSH_PANEL_CONTROL_FAN_GP_ID  249
#define SPARSH_PANEL_CONTROL_DIMMER_GP_ID  248

#define SPARSH_PANEL_LIGHT_GP_CTRL_MSG_LEN  12
#define SPARSH_PANEL_FAN_GP_CTRL_MSG_LEN  13
#define SPARSH_PANEL_DIMMER_GP_CTRL_MSG_LEN  13


#define SPARSH_LIGHT_TURN_ON_CMD  101
#define SPARSH_LIGHT_TURN_OFF_CMD  100


        
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
int GW_sendSparshPanelCtrlCmd(const char *arg1_p,
                              const char *arg2_p)
{
   int rc, pyldLen, msgLen, off = 0, pointId, groupId;
   unsigned char *pyld_p;
   int cmdByte = 0;
    
   switch (arg1_p[0])
   {
      case 'l':
           {
              rc = sscanf(arg1_p, "l%d", &pointId);
              if (rc != 1 || pointId < 1 || pointId > 6)
              {
                  printf("please enter valid light point id (l1 to l6) !! \n");
                  return 9;
              }

              if (arg2_p != NULL)
              {
                  if (strcmp(arg2_p, "on") == 0)
                      cmdByte = SPARSH_LIGHT_TURN_ON_CMD;
                  else
                  {
                      if (strcmp(arg2_p, "off") == 0)
                          cmdByte = SPARSH_LIGHT_TURN_OFF_CMD;
                  }
              }
              else
              {
                  printf("please specify operation (on / off) !! \n");
                  return 10;
              }

              groupId = SPARSH_PANEL_CONTROL_LIGHT_GP_ID;
              msgLen = SPARSH_PANEL_LIGHT_GP_CTRL_MSG_LEN;
           }
           break;

      case 'f':
           {
              rc = sscanf(arg1_p, "f%d", &pointId);
              if (rc != 1 || pointId < 1 || pointId > 2)
              {
                  printf("please enter valid fan point id (l1 to l6) !! \n");
                  return 9;
              }

              if (arg2_p != NULL)
              {
                  if (strcmp(arg2_p, "on") == 0)
                      cmdByte = 11;
                  else
                  {
                      if (strcmp(arg2_p, "off") == 0)
                          cmdByte = 0;
                      else
                      {
                          int level;
                          if (sscanf(arg2_p, "%d", &level) == 1)
                          {
                              if (level >= 1 && level <= 10)
                                  cmdByte = level;
                              else
                              {
                                  printf("Fan speed setting range is 1 to 10 !! \n");
                                  return 10;
                              }
                          }
                          else
                          {
                              printf("Please enter on, off or speed level (1 - 10) !! \n");
                              return 11;
                          }
                      }
                  }
              }

              groupId = SPARSH_PANEL_CONTROL_FAN_GP_ID;
              msgLen = SPARSH_PANEL_FAN_GP_CTRL_MSG_LEN;
           }
           break;
      
      case 'd':
           {
              rc = sscanf(arg1_p, "d%d", &pointId);
              if (rc != 1 || pointId < 1 || pointId > 2)
              {
                  printf("please enter valid dimmer point id (l1 to l6) !! \n");
                  return 9;
              }

              if (arg2_p != NULL)
              {
                  if (strcmp(arg2_p, "on") == 0)
                      cmdByte = 11;
                  else
                  {
                      if (strcmp(arg2_p, "off") == 0)
                          cmdByte = 0;
                      else
                      {
                          int dimmerLvl;
                          rc = sscanf(arg2_p, "%d", &dimmerLvl);
                          if (rc != 1 || dimmerLvl < 1 || dimmerLvl > 10)
                          {
                              printf("Enter valid dimmer level (on / off / 1 - 10) \n"); 
                              return 11;
                          }
                          else
                              cmdByte = dimmerLvl;
                      }
                  }
              }

              groupId = SPARSH_PANEL_CONTROL_DIMMER_GP_ID;
              msgLen = SPARSH_PANEL_DIMMER_GP_CTRL_MSG_LEN;
           }
           break;

      default:
           {
              printf("please enter valid point id (l1-l6 or f1-f2 or d1-d2) !! \n");
              return 9;
           }
           break;
   }
   

   pyldLen = LPWMN_MAC_SHORT_ADDR_LEN + msgLen;
   
   pyld_p = (unsigned char *)malloc(pyldLen);
   if (pyld_p == NULL)
   {
       printf("malloc(%d) failed !! \n", pyldLen);
       return 1;
   }

   printf("\n gp %d / point %d / pyld len %d / cmd byte %d \n", 
          groupId, pointId, pyldLen, cmdByte);

   __htons(pyld_p, 0x2);
   off = LPWMN_MAC_SHORT_ADDR_LEN;
   pyld_p[off ++] = 2;   // 1
   pyld_p[off ++] = 0;   // 2
   pyld_p[off ++] = 0;   // 3
   pyld_p[off ++] = 0;   // 4
   pyld_p[off ++] = 0;   // 5
   pyld_p[off ++] = 0;   // 6
   pyld_p[off ++] = 1;   // 7
   pyld_p[off ++] = 1;   // 8
   pyld_p[off ++] = 1;   // 9
   pyld_p[off ++] = groupId;  // 10
   pyld_p[off ++] = pointId;  // 11
   switch (groupId)
   {
      case SPARSH_PANEL_CONTROL_LIGHT_GP_ID:
      case SPARSH_PANEL_CONTROL_FAN_GP_ID:
      case SPARSH_PANEL_CONTROL_DIMMER_GP_ID:
           pyld_p[off ++] = cmdByte;
           break;

      default:
           break;
   }
   
   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_RELAY_TO_NODE, pyld_p, pyldLen);

   if (rc != 1)
       return rc;
      
   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 2;
   }
      
   if (verbose)
       printf("Header acked \n");
   
   // Send payload
   rc = writePort(pyld_p, pyldLen);
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", 
              __FUNCTION__, pyldLen);
       return 3;
   }

   printf("Sent request to panel \n");
                      
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
int GW_getPOS(const char *argv[])
{
   int rc, pyldLen, off = 0;
   unsigned int shortAddr, portId;
   unsigned char *pyld_p;

   rc = sscanf(argv[3], "%u", &shortAddr);
   if (rc != 1 || shortAddr <= 0x1 || shortAddr > LPWMN_MAX_UNICAST_SHORT_ADDR)
   {
       rc = sscanf(argv[3], "0x%x", &shortAddr);
       if (rc != 1 || shortAddr <= 0x1 || shortAddr > LPWMN_MAX_UNICAST_SHORT_ADDR)
       {
           printf("please enter a valid short address (> 1 & <= %u) !! \n", 
                  LPWMN_MAX_UNICAST_SHORT_ADDR);
           return 9;
       }
   }
 
   pyldLen = LPWMN_MAC_SHORT_ADDR_LEN + DIS_MSG_TYPE_SZ;

   pyld_p = (unsigned char *)malloc(pyldLen);
   if (pyld_p == NULL)
   {
       printf("malloc(%d) failed !! \n", pyldLen);
       return 1;
   }

   __htons(pyld_p, shortAddr);
   off = LPWMN_MAC_SHORT_ADDR_LEN;
   pyld_p[off ++] = DIS_MSG_TYPE_GET_GPIO_PORT_OP_STATE;
    
   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_RELAY_TO_NODE, 
                        pyld_p, pyldLen);
   if (rc != 1)
       return rc;
      
   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 2;
   }
      
   if (verbose)
       printf("Header acked \n");
   
   // Send payload
   rc = writePort(pyld_p, pyldLen);
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", 
              __FUNCTION__, pyldLen);
       return 3;
   }

#if 0
   printf("Request sent. Not Waiting for response ..... \n");

#else
   printf("Request sent. Waiting for response ..... \n");
                      
   expDisMsgType = DIS_MSG_TYPE_GET_GPIO_PORT_OP_STATE;
   expDisMsgSrcShortAddr = shortAddr;
   
   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_RELAY_FROM_NODE, 0);
#endif

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
int GW_digitalIOCtrl(const char *argv[])
{
   int rc, pyldLen, off = 0;
   unsigned int shortAddr, portId, pinNr, val;
   unsigned char *pyld_p;

   rc = sscanf(argv[3], "%u", &shortAddr);
   if (rc != 1 || shortAddr <= 0x1 || shortAddr > LPWMN_MAX_UNICAST_SHORT_ADDR)
   {
       rc = sscanf(argv[3], "0x%x", &shortAddr);
       if (rc != 1 || shortAddr <= 0x1 || shortAddr > LPWMN_MAX_UNICAST_SHORT_ADDR)
       {
           printf("please enter a valid short address (> 1 & <= %u) !! \n", 
                  LPWMN_MAX_UNICAST_SHORT_ADDR);
           return 9;
       }
   }
 
   rc = sscanf(argv[4], "%u", &portId);
   if (rc != 1 || portId < 0x1 || portId > 16)
   {
       rc = sscanf(argv[4], "0x%x", &portId);
       if (rc != 1 || portId < 0x1 || portId > 16)
       {
           printf("please enter valid port id (>= 1 & <= 16) !! \n");
           return 10;
       }
   }

   rc = sscanf(argv[5], "%u", &pinNr);
   if (rc != 1 || pinNr > 7)
   {
       rc = sscanf(argv[5], "0x%x", &pinNr);
       if (rc != 1 || pinNr > 7)
       {
           printf("please enter valid pin nr (0 to 7) !! \n");
           return 11;
       }
   }

   rc = sscanf(argv[6], "%u", &val);
   if (rc != 1 || val > 1)
   {
       rc = sscanf(argv[6], "0x%x", &val);
       if (rc != 1 || val > 1)
       {
           printf("please enter valid value (0 or 1) !! \n");
           return 11;
       }
   }

   pyldLen = LPWMN_MAC_SHORT_ADDR_LEN
             + DIS_MSG_TYPE_SZ
             + DIS_TLV_HDR_SZ   // DIS_TLV_TYPE_CTRL_DIGITAL_IO
             + DIS_DIGITAL_IO_PORT_TLV_SZ
             + DIS_DIGITAL_IO_PIN_TLV_SZ
             + DIS_DIGITAL_IO_VAL_TLV_SZ;

   pyld_p = (unsigned char *)malloc(pyldLen);
   if (pyld_p == NULL)
   {
       printf("malloc(%d) failed !! \n", pyldLen);
       return 1;
   }

   __htons(pyld_p, shortAddr);
   off = LPWMN_MAC_SHORT_ADDR_LEN;
   pyld_p[off ++] = DIS_MSG_TYPE_CTRL_DIGITAL_IO;

   pyld_p[off ++] = DIS_TLV_TYPE_DIGITAL_IO;
   pyld_p[off ++] = (DIS_DIGITAL_IO_PORT_TLV_SZ
                     + DIS_DIGITAL_IO_PIN_TLV_SZ
                     + DIS_DIGITAL_IO_VAL_TLV_SZ);

   pyld_p[off ++] = DIS_TLV_TYPE_DIGITAL_IO_PORT;
   pyld_p[off ++] = DIS_DIGITAL_IO_PORT_FIELD_SZ;
   pyld_p[off ++] = portId;

   pyld_p[off ++] = DIS_TLV_TYPE_DIGITAL_IO_PIN;
   pyld_p[off ++] = DIS_DIGITAL_IO_PIN_FIELD_SZ;
   pyld_p[off ++] = pinNr;

   pyld_p[off ++] = DIS_TLV_TYPE_DIGITAL_IO_VAL;
   pyld_p[off ++] = DIS_DIGITAL_IO_VAL_FIELD_SZ;
   pyld_p[off ++] = val;

#if 0
   {
      int idx;
      for (idx=0; idx<off; idx++)
      {
          if (idx % 8 == 0)
              printf("\n");
          printf(" 0x%x ", pyld_p[idx]);
      }
   }
#endif

   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_RELAY_TO_NODE, 
                        pyld_p, pyldLen);
   if (rc != 1)
       return rc;
      
   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 2;
   }
      
   if (verbose)
       printf("Header acked \n");
   
   // Send payload
   rc = writePort(pyld_p, pyldLen);
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", 
              __FUNCTION__, pyldLen);
       return 3;
   }

   printf("Sent request to remote node \n");
                      
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
int GW_sendImageSwReq(unsigned short shortAddr)
{
   int rc, pyldLen, off = 0;
   unsigned char *pyld_p;

   pyldLen = LPWMN_MAC_SHORT_ADDR_LEN + DIS_MSG_TYPE_SZ;

   pyld_p = (unsigned char *)malloc(pyldLen);
   if (pyld_p == NULL)
   {
       printf("malloc(%d) failed !! \n", pyldLen);
       return 1;
   }
   
   UC_FU_cntxt.currState = UC_FU_STATE_SENT_IMAGE_SWITCH_REQUEST;

   __htons(pyld_p, shortAddr);
   off = LPWMN_MAC_SHORT_ADDR_LEN;
   pyld_p[off ++] = DIS_MSG_TYPE_SWITCH_TO_SBY_IMAGE;
    
   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_RELAY_TO_NODE, 
                        pyld_p, pyldLen);
   if (rc != 1)
       return rc;
      
   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 2;
   }
      
   if (verbose)
       printf("Header acked \n");
   
   // Send payload
   rc = writePort(pyld_p, pyldLen);
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", 
              __FUNCTION__, pyldLen);
       return 3;
   }

   printf("Request sent. Waiting for response ..... \n");
                                             
                      
   expDisMsgType = DIS_MSG_TYPE_NODE_UC_FU_MODULE_INFO;
   expDisMsgSrcShortAddr = shortAddr;
   
   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_RELAY_FROM_NODE, 0);

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
int GW_getNodeSensorCnt(const char *arg_p)
{
   int rc, pyldLen, off = 0;
   unsigned char *pyld_p;
   unsigned int shortAddr;

   rc = sscanf(arg_p, "%u", &shortAddr);
   if (rc != 1 || shortAddr <= 0x1 || shortAddr > LPWMN_MAX_UNICAST_SHORT_ADDR)
   {
       rc = sscanf(arg_p, "0x%x", &shortAddr);
       if (rc != 1 || shortAddr <= 0x1 || shortAddr > LPWMN_MAX_UNICAST_SHORT_ADDR)
       {
           printf("please enter a valid short address (> 1 & <= %u) !! \n", 
                  LPWMN_MAX_UNICAST_SHORT_ADDR);
           return 9;
       }
   } 

   pyldLen = LPWMN_MAC_SHORT_ADDR_LEN + DIS_MSG_TYPE_SZ;

   pyld_p = (unsigned char *)malloc(pyldLen);
   if (pyld_p == NULL)
   {
       printf("malloc(%d) failed !! \n", pyldLen);
       return 1;
   }

   __htons(pyld_p, shortAddr);
   off = LPWMN_MAC_SHORT_ADDR_LEN;
   pyld_p[off ++] = DIS_MSG_TYPE_GET_NODE_SENSOR_COUNT;
    
   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_RELAY_TO_NODE, 
                        pyld_p, pyldLen);
   if (rc != 1)
       return rc;
      
   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 2;
   }
      
   if (verbose)
       printf("Header acked \n");
   
   // Send payload
   rc = writePort(pyld_p, pyldLen);
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", 
              __FUNCTION__, pyldLen);
       return 3;
   }

   printf("Request sent. Waiting for response ..... \n");
                      
   expDisMsgType = DIS_MSG_TYPE_NODE_SENSOR_COUNT;
   expDisMsgSrcShortAddr = shortAddr;
   
   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_RELAY_FROM_NODE, 0);

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
int GW_addNodeToBlackList(const char *arg_p)
{
   unsigned int eui64[LPWMN_EXT_ADDR_LEN];
   unsigned char eui64_8[LPWMN_EXT_ADDR_LEN];
   int rc, idx;

   rc = sscanf(arg_p, "%x:%x:%x:%x:%x:%x:%x:%x", 
                &eui64[0], &eui64[1], &eui64[2], &eui64[3], 
                &eui64[4], &eui64[5], &eui64[6], &eui64[7]);
   if (rc != LPWMN_EXT_ADDR_LEN)
   {
       rc = sscanf(arg_p, "0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x", 
                   &eui64[0], &eui64[1], &eui64[2], &eui64[3], 
                   &eui64[4], &eui64[5], &eui64[6], &eui64[7]);
       if (rc != LPWMN_EXT_ADDR_LEN)
       {
           printf("You have entered an invalid eui64 address !! \n");
           return 10;
       }
   }

   for (idx=0; idx<LPWMN_EXT_ADDR_LEN; idx++)
   {
      if (eui64[idx] > 255)
      {
          printf("You have entered an invalid eui64 address !! \n");
          return 11;
      }
      eui64_8[idx] = eui64[idx];
   }
   
   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_ADD_NODE_TO_BLACK_LIST, 
                        eui64_8, LPWMN_EXT_ADDR_LEN);
   if (rc != 1)
       return rc;

   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 41;
   }
      
   if (verbose)
       printf("Header acked \n");

   // Send payload
   rc = writePort(eui64_8, LPWMN_EXT_ADDR_LEN);
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", __FUNCTION__, LPWMN_EXT_ADDR_LEN);
       return 42;
   }

   printf("Request sent ... \n");
       
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
int GW_addNodeToWhiteList(const char *arg_p)
{
   unsigned int eui64[LPWMN_EXT_ADDR_LEN];
   unsigned char eui64_8[LPWMN_EXT_ADDR_LEN];
   int rc, idx;

   rc = sscanf(arg_p, "%x:%x:%x:%x:%x:%x:%x:%x", 
                &eui64[0], &eui64[1], &eui64[2], &eui64[3], 
                &eui64[4], &eui64[5], &eui64[6], &eui64[7]);
   if (rc != LPWMN_EXT_ADDR_LEN)
   {
       rc = sscanf(arg_p, "0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x", 
                   &eui64[0], &eui64[1], &eui64[2], &eui64[3], 
                   &eui64[4], &eui64[5], &eui64[6], &eui64[7]);
       if (rc != LPWMN_EXT_ADDR_LEN)
       {
           printf("You have entered an invalid eui64 address !! \n");
           return 10;
       }
   }

   for (idx=0; idx<LPWMN_EXT_ADDR_LEN; idx++)
   {
      if (eui64[idx] > 255)
      {
          printf("You have entered an invalid eui64 address !! \n");
          return 11;
      }
      eui64_8[idx] = eui64[idx];
   }
   
   rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_ADD_NODE_TO_WHITE_LIST, 
                        eui64_8, LPWMN_EXT_ADDR_LEN);
   if (rc != 1)
       return rc;

   rc = GW_readSerIntf(UART_MSG_TYPE_ACK, 0);
   if (rc != 0)
       return rc;

   if (hdrAcked == 0x0)
   {
       printf("Header not acked !! \n");
       return 41;
   }
      
   if (verbose)
       printf("Header acked \n");

   // Send payload
   rc = writePort(eui64_8, LPWMN_EXT_ADDR_LEN);
   if (rc != 1)
   {
       printf("<%s> writePort(%d) failed !! \n", __FUNCTION__, LPWMN_EXT_ADDR_LEN);
       return 42;
   }

   printf("Request sent ... \n");
       
   return 0;
}


#define UART_MSG_HDR_PYLD_CRC_FIELD_OFF  8
#define UART_MSG_HDR_LEN (UART_MSG_HDR_PYLD_CRC_FIELD_OFF + UART_MSG_HDR_PYLD_CRC_FIELD_LEN)

unsigned int GW_rcvdMsgCnt = 0;

LPWMN_nodeListEntry_s  *LPWMN_nodeListHead_p = NULL;


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
LPWMN_nodeListEntry_s *GW_lookUpShortAddr(unsigned int shortAddr)
{
   LPWMN_nodeListEntry_s *node_p = LPWMN_nodeListHead_p;

   while (node_p != NULL)
   {
      if (node_p->shortAddr == shortAddr)
          break;
      else
      {
          node_p = node_p->next_p;
      }
   }

   return node_p;
}

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
LPWMN_nodeListEntry_s *GW_allocNode(void)
{
   LPWMN_nodeListEntry_s *new_p = (LPWMN_nodeListEntry_s *)malloc(sizeof(LPWMN_nodeListEntry_s));
   if (new_p == NULL)
   {
       printf("\n**************malloc() failed !!**************\n");
       exit(1);
   }

   memset(new_p, 0, sizeof(LPWMN_nodeListEntry_s));


   if (LPWMN_nodeListHead_p != NULL)
       new_p->next_p = LPWMN_nodeListHead_p;
   else
       new_p->next_p = NULL;
       
   LPWMN_nodeListHead_p = new_p;

   return new_p;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
LPWMN_nodeListEntry_s *GW_lookUpExtAddr(unsigned char *extAddr_p)
{
   LPWMN_nodeListEntry_s *node_p = LPWMN_nodeListHead_p;

   while (node_p != NULL)
   {
      if (memcmp(extAddr_p, node_p->extAddr, LPWMN_MAC_EXT_ADDR_LEN) == 0x0)
          break;
      else
      {
          node_p = node_p->next_p;
      }
   }

   return node_p;
}


/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
int GW_processEvt(unsigned char *buff_p, int msgLen)
{
   int rc = 0;
   time_t frameRxTime;
   char *timeStr_p;

   time(&frameRxTime);
   timeStr_p = ctime(&frameRxTime);

   timeStr_p[strlen(timeStr_p) - 1] = '\0';

   if (verbose) 
       printf("\nevtLen<%d>\n", msgLen);

   if (msgLen >= LPWMN_GW_EVT_ID_LEN)
   {
       unsigned int evtId = buff_p[0];

       msgLen -= LPWMN_GW_EVT_ID_LEN;
       buff_p += LPWMN_GW_EVT_ID_LEN;
   
       if (verbose) 
           printf("\nevt-Id<%d>\n", evtId);

       switch (evtId)
       {
          case LPWMN_GW_EVT_TYPE_PATH_DISC_STARTED:
               {
                  printf("----------------------------------------------------------------------------------- \n");
                  printf("Timestamp (%s)\n", timeStr_p);
                  
                  if (msgLen >= LPWMN_MAC_SHORT_ADDR_LEN) 
                  {
                      unsigned int shortAddr = __ntohs(buff_p);
                      printf("Event - Path Discovery To Node %u Started \n", shortAddr);
                  }
                  else
                  {
                      printf("Received malformed \"Path Discovery Started Event\" !! \n");
                  }
                  printf("----------------------------------------------------------------------------------- \n");
               }
               break;


          case LPWMN_GW_EVT_TYPE_PATH_DISC_OVER:
               {
                  unsigned int badMsg = 0;

                  rc = 1;

                  printf("----------------------------------------------------------------------------------- \n");
                  printf("Timestamp (%s)\n", timeStr_p);
                  
                  if (msgLen >= LPWMN_MAC_SHORT_ADDR_LEN + LPWMN_GW_PATH_DISC_EVT_STS_FIELD_LEN) 
                  {
                      unsigned int idx, tgtAddr = __ntohs(buff_p);
                      printf("Event - Path Discovery To Node %u Over \n", tgtAddr);

                      if (buff_p[LPWMN_MAC_SHORT_ADDR_LEN] != 0x0)
                      {
                          printf("Path reply not received - ");
                          switch (buff_p[LPWMN_MAC_SHORT_ADDR_LEN])
                          {
                             case ADP_STS_NO_ROUTE:
                                  printf("<No route to specified node !!> \n");
                                  break;

                             case ADP_STS_PATH_DISC_TIMED_OUT:
                                  printf("<Path discovery session timed out !!> \n");
                                  break;
 
                             case ADP_STS_NODE_NOT_FOUND:
                                  printf("<Specified node is not registered !!> \n");
                                  break;

                             default:
                                  printf("<Unknown error [%d] !!> \n", buff_p[LPWMN_MAC_SHORT_ADDR_LEN]);
                                  break;
                          }
                          printf("----------------------------------------------------------------------------------- \n");
                          break;
                      }

                      printf("\nPath reply received .... \n\n");
                      
                      
                      // printf("\n [0] msgLen %d \n", msgLen);
                      // for (idx=0; idx<msgLen; idx++)
                      //     printf("\n 0x%02x", buff_p[idx]);
                      // printf("\n");

                      buff_p += (LPWMN_MAC_SHORT_ADDR_LEN + LPWMN_GW_PATH_DISC_EVT_STS_FIELD_LEN);
                      msgLen -= (LPWMN_MAC_SHORT_ADDR_LEN + LPWMN_GW_PATH_DISC_EVT_STS_FIELD_LEN);
                      
                      // printf("\n [1] msgLen %d \n", msgLen);

                      if (msgLen >= (LPWMN_MAC_SHORT_ADDR_LEN*2 + LPWMN_GW_PATH_DISC_EVT_HOP_CNT_FIELD_LEN))
                      {
                          int fwdHopCnt;

                          buff_p += (LPWMN_MAC_SHORT_ADDR_LEN * 2);
                            
                          fwdHopCnt = *buff_p;
                          buff_p ++;
                         
                          printf("Forward Hop Cnt <%u> \n", fwdHopCnt);

                          msgLen -= (LPWMN_MAC_SHORT_ADDR_LEN*2 + LPWMN_GW_PATH_DISC_EVT_HOP_CNT_FIELD_LEN); 
                      
                          // printf("\n [2] msgLen %d \n", msgLen);

                          if (msgLen >= (fwdHopCnt * LPWMN_MAC_SHORT_ADDR_LEN))
                          {
                              int idx;
                              
                              for (idx=0; idx<fwdHopCnt; idx++)
                              {
                                   printf("  Forward Hop[%d] <%u> \n", idx + 1, __ntohs(buff_p));
                                   buff_p += LPWMN_MAC_SHORT_ADDR_LEN;
                              }

                             msgLen -= (LPWMN_MAC_SHORT_ADDR_LEN * fwdHopCnt);

                             if (msgLen >= LPWMN_GW_PATH_DISC_EVT_STS_FIELD_LEN
                                           + LPWMN_GW_PATH_DISC_EVT_HOP_CNT_FIELD_LEN)
                             {
                                 
                                 int fwdSts = *buff_p, revHopCnt;
                                 buff_p ++;
                                 msgLen -= (LPWMN_GW_PATH_DISC_EVT_STS_FIELD_LEN);
   
                                 if (fwdSts == 0)
                                     printf("\nPath reply received from %u \n", tgtAddr);
                                 else
                                     printf("\nIncomplete path obtained ... sts<%d> !!\n", fwdSts);
   
                                 revHopCnt = *buff_p;
                                 buff_p ++;
                                 msgLen -= LPWMN_GW_PATH_DISC_EVT_HOP_CNT_FIELD_LEN;
   
                                 printf("\nReverse Hop Cnt <%u> \n", revHopCnt);
    
                                 // printf("\n [3] msgLen %d \n", msgLen);
                                  
                                 if (msgLen >= (revHopCnt * LPWMN_MAC_SHORT_ADDR_LEN))
                                 {
                                     int idx;
                                 
                                     for (idx=0; idx<revHopCnt; idx++)
                                     {
                                          printf("  Reverse Hop[%d] <%u> \n", idx + 1, __ntohs(buff_p));
                                          buff_p += LPWMN_MAC_SHORT_ADDR_LEN;
                                     }
                                 }
                                 else
                                     badMsg = 1;
                             }
                         } 
                         else
                             badMsg = 1;
                     }
                     else
                         badMsg = 1;
                  }
                  else
                     badMsg = 1;
                 
                  if (badMsg) 
                  {
                      printf("Received malformed \"Path Discovery Over Event\" !! \n");
                  }
                  printf("----------------------------------------------------------------------------------- \n");
               }
               break;

          case LPWMN_GW_EVT_TYPE_NODE_REG:
               {
                  int idx;

                  printf("-----------------------------------------------------------------------------------\n");
                  printf("Timestamp (%s)\n", timeStr_p);

                  printf("Event - Node Registered .... \n");

                  if (msgLen == (LPWMN_MAC_SHORT_ADDR_LEN 
                                 + LPWMN_MAC_EXT_ADDR_LEN
                                 + LPWMN_MAC_NODE_CAPABILITY_INFO_LEN))
                  {
                      unsigned int shortAddr;
                      LPWMN_nodeListEntry_s *node_p;

                      shortAddr = __ntohs(buff_p);
                      printf("Node short address <0x%x> \n", shortAddr);
                      buff_p += LPWMN_MAC_SHORT_ADDR_LEN;

                      printf("Ext addr: ");
                      for (idx=0; idx<LPWMN_MAC_EXT_ADDR_LEN; idx++)
                           printf("<0x%x> ", buff_p[idx]);
                      printf("\n");
                      
                      node_p = GW_lookUpShortAddr(shortAddr);
                      if (node_p != NULL)
                      {
                          printf("\nDup short address <0x%x> detected !!!!!! \n", shortAddr);
                          printf("\nNode with ext addr ");
                          for (idx=0; idx<LPWMN_MAC_EXT_ADDR_LEN; idx++)
                               printf("<0x%x>", node_p->extAddr[idx]);
                          printf("\n\n");
                          printf("\nQuitting ..................................... \n");
                          exit(1);
                      }

                      node_p = GW_lookUpExtAddr(buff_p);
                      if (node_p == NULL)
                      {
                          node_p = GW_allocNode();
                          memcpy(node_p->extAddr, buff_p, LPWMN_MAC_EXT_ADDR_LEN);
                      }

                      node_p->shortAddr = shortAddr;
                      node_p->regCnt ++;

                      printf("Node reg count - %d\n", node_p->regCnt);

                      buff_p += LPWMN_MAC_EXT_ADDR_LEN;
                  }
                  printf("----------------------------------------------------------------------------------- \n");
               }
               break;
          
          case LPWMN_GW_EVT_TYPE_PURGE_RT_TBL:
               {
               }
               break;

          case LPWMN_GW_EVT_TYPE_RCVD_RT_REQ_WITH_INV_SRC_ADDR:
               {
                  unsigned int shortAddr;

                  shortAddr = __ntohs(buff_p);
                  printf("-----------------------------------------------------------------------------------\n");
                  printf("Timestamp (%s)\n", timeStr_p);
                  printf("Event - Received route request from non-registered node <%04u> \n", shortAddr);
                  printf("----------------------------------------------------------------------------------- \n");
               }
               break;

          case LPWMN_GW_EVT_TYPE_RCVD_PKT_WITH_INV_SRC_ADDR:
               {
                  unsigned int shortAddr;

                  shortAddr = __ntohs(buff_p);
                  printf("-----------------------------------------------------------------------------------\n");
                  printf("Timestamp (%s)\n", timeStr_p);
                  printf("Event - Received packet from non-registered node <%04u> \n", shortAddr);
                  printf("----------------------------------------------------------------------------------- \n");
               }
               break;
          
          case LPWMN_GW_EVT_TYPE_ASSOC_REQ_DROPPED:
               {
                  int idx;

                  printf("-----------------------------------------------------------------------------------\n");
                  printf("Timestamp (%s)\n", timeStr_p);
                  printf("Event - Dropped assoc request from node ");
                  for (idx=0; idx<LPWMN_MAC_EXT_ADDR_LEN-1; idx++)
                       printf("<0x%x>:", buff_p[idx]);
                  printf("<0x%x>", buff_p[idx]);
                  printf("\n");
                  printf("Time to assoc allowed <%d seconds> \n", *(buff_p + LPWMN_MAC_EXT_ADDR_LEN));
                  printf("----------------------------------------------------------------------------------- \n");
               }
               break;
          
          case LPWMN_GW_EVT_TYPE_LOCAL_RT_DISC_STARTED:
               {
                  unsigned int shortAddr, attemptNr;

                  shortAddr = __ntohs(buff_p);
                  attemptNr = __ntohs(buff_p + LPWMN_MAC_SHORT_ADDR_LEN);
                  printf("-----------------------------------------------------------------------------------\n");
                  printf("Timestamp (%s)\n", timeStr_p);
                  printf("Event - Route discovery to 0x%04x started (attempt %d)  \n", 
                         shortAddr, attemptNr);
                  printf("----------------------------------------------------------------------------------- \n");
               }
               break;

          case LPWMN_GW_EVT_TYPE_LOCAL_RT_DISC_OVER:
               {
                  unsigned int shortAddr, nextHop;

                  shortAddr = __ntohs(buff_p);
                  nextHop = __ntohs(buff_p + LPWMN_MAC_SHORT_ADDR_LEN);
                  printf("-----------------------------------------------------------------------------------\n");
                  printf("Timestamp (%s)\n", timeStr_p);
                  printf("Event - Route discovery (to 0x%04x) over .... next-hop <0x%04x> \n", 
                         shortAddr, nextHop);
                  printf("----------------------------------------------------------------------------------- \n");
               }
               break;

          case LPWMN_GW_EVT_TYPE_BCN_REQ_RCVD:
               {
                  unsigned int cnt;

                  cnt = __ntohs(buff_p);
                  printf("-----------------------------------------------------------------------------------\n");
                  printf("Timestamp (%s) \n", timeStr_p);
                  printf("Event - Beacon Request received (# %u) \n", cnt);
                  printf("----------------------------------------------------------------------------------- \n");
               }
               break;

          case LPWMN_GW_EVT_TYPE_MAC_CNFRM:
               {
                  unsigned int cnt, result, modIdx;

                  cnt = __ntohs(buff_p);
                  buff_p +=2;
                  result = *buff_p;
                  buff_p ++;
                  modIdx = *buff_p;
                  printf("-----------------------------------------------------------------------------------\n");
                  printf("Timestamp (%s) \n", timeStr_p);
                  printf("Event - MAC Tx Cnfrm / result <%u> / module <%u:%s> / (# %u) \n", 
                         result, modIdx, modIdx <= 6 ? LPWMN_macMod[modIdx] : "Unknown" , cnt);
                  printf("----------------------------------------------------------------------------------- \n");
               }
               break;

          case LPWMN_GW_EVT_TYPE_SYS_BOOT:
               {
                  printf("-----------------------------------------------------------------------------------\n");
                  printf("Timestamp (%s) \n", timeStr_p);
                  printf("Event - LPWMN coord has rebooted .... \n");
                  printf("----------------------------------------------------------------------------------- \n");
               }
               break;
       }
   }

   return rc;
}


int loadCellCalDone = 1;
int loadCellMFactor = 60538;//Sensor Event Into this Block
int loadCellMFactorF = 60538/1000;
int totalSnsrOp = 0;
int calSampleCnt = 0;

/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
void GW_processNodeMsg(int expShortAddr, unsigned char *buff_p, int msgLen)
{
   int srcShortAddr, off = 0;
   LPWMN_nodeListEntry_s *node_p;
   int rssi;
   unsigned int lqi_corr;
   time_t frameRxTime;
   char *timeStr_p;
   unsigned char disMsgType;

   time(&frameRxTime);
   timeStr_p = ctime(&frameRxTime);

   timeStr_p[strlen(timeStr_p) - 1] = '\0';

   if (msgLen < LPWMN_MAC_SHORT_ADDR_LEN 
                + LPWMN_MAC_EXT_ADDR_LEN 
                + LPWMN_MSG_RSSI_LEN 
                + LPWMN_MSG_CORR_LQI_LEN)
       return;

   srcShortAddr = buff_p[off];
   srcShortAddr = (srcShortAddr << 8) | buff_p[off + 1];

   if (expShortAddr != 0x0)
   {
       if (srcShortAddr != expShortAddr)
           return;
   }
                
   printf("----------------------------------------------------------------------------------- \n");

   GW_rcvdMsgCnt ++;

#if 0
   node_p = GW_lookUpShortAddr(srcShortAddr);
   if (node_p == NULL)
   {
       printf("[%u] Received message from node <0x%04x> \n", GW_rcvdMsgCnt, srcShortAddr);
   }
   else
       printf("[%u] Received msg from node <%05u / %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x> \n",
              GW_rcvdMsgCnt,
              srcShortAddr, 
              (unsigned int)node_p->extAddr[LPWMN_MAC_EXT_ADDR_LEN-8],
              (unsigned int)node_p->extAddr[LPWMN_MAC_EXT_ADDR_LEN-7],
              (unsigned int)node_p->extAddr[LPWMN_MAC_EXT_ADDR_LEN-6],
              (unsigned int)node_p->extAddr[LPWMN_MAC_EXT_ADDR_LEN-5],
              (unsigned int)node_p->extAddr[LPWMN_MAC_EXT_ADDR_LEN-4],
              (unsigned int)node_p->extAddr[LPWMN_MAC_EXT_ADDR_LEN-3],
              (unsigned int)node_p->extAddr[LPWMN_MAC_EXT_ADDR_LEN-2],
              (unsigned int)node_p->extAddr[LPWMN_MAC_EXT_ADDR_LEN-1]);  
   if (node_p != NULL)
   {
       node_p->msgRcvdCnt ++;
   }
#endif

   
   printf("Timestamp (%s) \n", timeStr_p);


   // drop every other packet
   // if ((node_p->msgRcvdCnt % 5) != 0x0)
   //      return;

   off += LPWMN_MAC_SHORT_ADDR_LEN;
       
   //MyHub
   unsigned char MAC_ADDRESS[9];
   getNodeMACInChar(buff_p,off,MAC_ADDRESS);

   printf("[%u] MyHub++ Received msg from node <%05u / %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x> \n",
          GW_rcvdMsgCnt,
          srcShortAddr, 
          (unsigned int)buff_p[off],
          (unsigned int)buff_p[off+1],
          (unsigned int)buff_p[off+2],
          (unsigned int)buff_p[off+3],
          (unsigned int)buff_p[off+4],
          (unsigned int)buff_p[off+5],
          (unsigned int)buff_p[off+6],
          (unsigned int)buff_p[off+7]);

   buff_p += LPWMN_MAC_EXT_ADDR_LEN;

   rssi = (signed char)buff_p[off];
   lqi_corr = buff_p[off + 1];
 
   printf("RSSI %d dBm / LQI %u\n", (int)rssi, lqi_corr);
                       
   off += (LPWMN_MSG_RSSI_LEN + LPWMN_MSG_CORR_LQI_LEN);

   msgLen -= ( LPWMN_MAC_SHORT_ADDR_LEN + LPWMN_MSG_RSSI_LEN + LPWMN_MSG_CORR_LQI_LEN);

   if (msgLen < 1)
       return;

   disMsgType = buff_p[off];



   if (verbose) 
       printf("\nDIS message type - 0x%x", disMsgType);

   off += DIS_MSG_TYPE_SZ;
   msgLen -= DIS_MSG_TYPE_SZ;
   buff_p += off;
   if(myhubverbose){
	   printf("\n MyHub - 1 \n");
   }
   if (msgLen <= 0xff)
   {
	   if(myhubverbose){
		   printf("\n MyHub - 2 \n");
	   }
       unsigned char rc, tlvLen1, *buff1_p;

       // hack !!!!!!!!!!!!!! 
       if (verbose) 
           printf("\ntlv type - 0x%x", *buff_p);

       if (disMsgType == DIS_MSG_TYPE_PING)
       {
    	   //Sensor Event Not Into this Block

           if (msgLen == 2)
           { 

               unsigned short pingSeqNr = __ntohs(buff_p); 
               printf("Ping sequence number - %u \n", pingSeqNr);
           }
           else
           {

               printf("Malformed ping message !! \n");
           }

           return;
       }

       if(myhubverbose){
    	   printf("\n MyHub - 7 \n");
       }
       *buff_p = DIS_TLV_TYPE_SENSOR_OUTPUT_LIST;

       rc = TLV_get(buff_p, msgLen, DIS_TLV_TYPE_SENSOR_OUTPUT_LIST, &tlvLen1, &buff1_p);
       if (rc == 0)
       {
    	   //Sensor Event Not Into this Block

           if (verbose) 
               printf("\nCould not find DIS_TLV_TYPE_SENSOR_OUTPUT_LIST !! \n");
           return;
       } 
       else
       {
    	   //Sensor Event Into this Block
    	   //Got TLV for Sensor Event
    	   if(myhubverbose){
    		   printf("\n MyHub - 9 \n");
    	   }
           if (verbose)
               printf("\nFound DIS_TLV_TYPE_SENSOR_OUTPUT_LIST");

           while (1)
           {
        	   //Sensor Event Into this Block
        	   if(myhubverbose){
        	   printf("\n MyHub - 10 \n");
        	   }
              unsigned char tlvLen2, *buff2_p;

              rc = TLV_get(buff1_p, tlvLen1, DIS_TLV_TYPE_SENSOR_OUTPUT, &tlvLen2, &buff2_p);
              if (rc == 0)
              {

                  if (verbose)
                      printf("\nCould not find another DIS_TLV_TYPE_SENSOR_OUTPUT TLV !! \n");
                  break;
              } 
              else
              {
            	  if(myhubverbose){
            	  printf("\n MyHub - 12 \n");
            	  }
                  unsigned char tlvLen3, *buff3_p;
                  int snsrId, scaleFactor = DIS_DATA_SCALE_CENTI;

                  buff1_p += (tlvLen2 + DIS_TLV_HDR_SZ);

                  if (verbose)
                      printf("\nFound DIS_TLV_TYPE_SENSOR_OUTPUT TLV .... val-fld-len<%d>", tlvLen2);
                  
                  rc = TLV_get(buff2_p, tlvLen2, DIS_TLV_TYPE_SENSOR_ID, &tlvLen3, &buff3_p);
                  if (rc == 0)
                      continue;
                  else
                  {
                	  if(myhubverbose){
                	  printf("\n MyHub - 13 \n");
                	  }
                      if (tlvLen3 == DIS_SENSOR_ID_FIELD_SZ) 
                      {
                    	  if(myhubverbose){
                    		  printf("\n MyHub - 14 \n");
                    	  }

                          snsrId = *buff3_p;
                          if (verbose)
                              printf("\nSensor Id <0x%x>", snsrId);
                      }
                      else
                         continue;
                  }
              if(myhubverbose){
            	  printf("\n MyHub - 15 \n");
              }
                  rc = TLV_get(buff2_p, tlvLen2, DIS_TLV_TYPE_DATA_SCALE_FACTOR, &tlvLen3, &buff3_p);
                  if (rc)
                  {
                	  if(myhubverbose){
                		  printf("\n MyHub - 16 \n");
                	  }
                      if (tlvLen3 == DIS_DATA_SCALE_FACTOR_FIELD_SZ)
                      {
                    	  if(myhubverbose){
                    		  printf("\n MyHub - 17 \n");
                    	  }
                          scaleFactor = *buff3_p;
                          if (verbose)
                              printf("\nFound Scale factor <%d>", scaleFactor); 
                          if (!(scaleFactor >= DIS_DATA_SCALE_TERA && scaleFactor <= DIS_DATA_SCALE_FEMTO))
                               scaleFactor = DIS_DATA_SCALE_NONE;
                      }
                  }
                  if(myhubverbose){
                	  printf("\n MyHub - 18 \n");
                  }
                  rc = TLV_get(buff2_p, tlvLen2, DIS_TLV_TYPE_VALUE, &tlvLen3, &buff3_p);
                  if (rc == 0)
                      continue;
                  else
                  {
                	  if(myhubverbose){
                		  printf("\n MyHub - 19 \n");
                	  }
                      int snsrOp;
                      signed short snsrOp16;

                      char *unit_p = " ";

                      if (verbose)
                          printf("\nFound DIS_TLV_TYPE_VALUE TLV .... val-fld-len<%d>", tlvLen3);
                
                      switch(tlvLen3)
                      {
                         case 1:
                        	 if(myhubverbose){
                        		 printf("\n MyHub - 20 \n");
                        	 }
                              snsrOp = (int)(*buff3_p);
                              break;
                    
                         case 2:
                              {
                            	  if(myhubverbose){
                            		  printf("\n MyHub - 21 \n");
                            	  }
                                snsrOp16 = __ntohs(buff3_p);
                                snsrOp = snsrOp16;
                              }
                              break;

                         case 4:
                              snsrOp = (int)__ntohl(buff3_p);
                              // printf("\n snsrOp : %d 0x%x \n", snsrOp, (unsigned  int)__ntohl(buff3_p));
                              break;

                         default:
                              break;
                      }


                      switch (snsrId)
                      {
                         case PLTFRM_DUMMY_DEV_ID:
                              printf("+[Sequence Nr]    ");
                              unit_p = "";
                              scaleFactor = DIS_DATA_SCALE_NONE;
                              break;
                              
                         case PLTFRM_ON_CHIP_VCC_SENSOR_DEV_ID:
                        	 if(myhubverbose){
                        		 printf("\n MyHub - 25 \n");
                        	 }
                              printf("+[Node_Voltage] ");
                              unit_p = "Volts";
                              break;

                         case PLTFRM_GEN_VOLT_MON_DEV_ID:
                              printf("+[Ext Voltage]   ");
                              unit_p = "Volts";
                              break;

                         case PLTFRM_GEN_CURRENT_MON_DEV_ID:
                              printf("+[Ext Current]   ");
                              unit_p = "mA";
                              break;
                         
                         case PLTFRM_AD7797_1_DEV_ID:
                              printf("+[Load Cell]   ");
                              unit_p = "grams";
                              scaleFactor = DIS_DATA_SCALE_NONE;
                              break;
 
                         case PLTFRM_MP3V5050GP_1_DEV_ID:
                              printf("+[P_MP3V5050GP]   ");
                              unit_p = "kPa";
                              break;

                         case PLTFRM_MP3V5010_1_DEV_ID:
                              printf("+[P_MP3V5010]   ");
                              unit_p = "%";
                              scaleFactor = DIS_DATA_SCALE_CENTI;
                              break;
 
                         case PLTFRM_MPXV5010G_1_DEV_ID:
                              printf("+[P_MPXV5010G]   ");
                              unit_p = "mm of water";
                              scaleFactor = DIS_DATA_SCALE_MILLI;
                              break;

                         case PLTFRM_MP3V5004GP_1_DEV_ID:
                              printf("+[P_MP3V5004GP]   ");
                              scaleFactor = DIS_DATA_SCALE_MILLI;
                              unit_p = "kPa";
                              break;

                         case PLTFRM_MPL115A2_1_DEV_ID:
                              printf("+[P_MPL115A2]   ");
                              scaleFactor = DIS_DATA_SCALE_NONE;
                              unit_p = "Pa";
                              break;

                         case PLTFRM_LLS_1_DEV_ID:
                              printf("+[LLS_POT]   ");
                              scaleFactor = DIS_DATA_SCALE_DECI;
                              unit_p = "Ohms";
                              break;

                         case PLTFRM_LM75B_1_DEV_ID:

                        	 if(myhubverbose){
                        		 printf("\n MyHub - 35 \n");
                        	 }
                              printf("+[Temp_LM75B]  ");
                              unit_p = "Deg C";
                              break;

                         case PLTFRM_MAG3110_1_DEV_ID:

                              printf("+[MFS_MAG3110]     ");
                              unit_p = "uT";
                              break;

                         case PLTFRM_MDS_1_DEV_ID:

                              printf("+[Mains Monitor]  ");
                              unit_p = "";
                              break;
                         
                         case PLTFRM_VIBRATION_SNSR_1_DEV_ID:

                        	 printf("+[Vibration Monitor]  ");
                              unit_p = "";
                              break;

                         case PLTFRM_EKMC160111X_1_DEV_ID:

                              printf("+[Motion Sensed Count]     ");
                              scaleFactor = DIS_DATA_SCALE_NONE;
                              unit_p = "";
                              break;

                         case PLTFRM_SHT10_1_TEMP_DEV_ID:

                              printf("+[Temp (SHT10)]   ");
                              unit_p = "Deg C";
                              scaleFactor = DIS_DATA_SCALE_CENTI;
                              break;

                         case PLTFRM_SHT10_1_RH_DEV_ID:

                              printf("+[RH (SHT10)]     ");
                              unit_p = "%";
                              scaleFactor = DIS_DATA_SCALE_CENTI;
                              break;

                         case PLTFRM_INA219_1_BV_DEV_ID:

                              printf("+[INA219_BUS_V]     ");
                              unit_p = "milli-volts";
                              scaleFactor = DIS_DATA_SCALE_MILLI;
                              break;
                         
                         case PLTFRM_ACS712_1_CURRENT_DEV_ID:

                              printf("+[ACS712_1_S_V]     ");
                              unit_p = "milli-volts";
                              scaleFactor = DIS_DATA_SCALE_NONE;
                              break;

                         case PLTFRM_ACS712_2_CURRENT_DEV_ID:

                              printf("+[ACS712_2_S_V]     ");
                              unit_p = "milli-volts";
                              scaleFactor = DIS_DATA_SCALE_NONE;
                              break;

                         case PLTFRM_ACS712_3_CURRENT_DEV_ID:

                              printf("+[ACS712_3_S_V]     ");
                              unit_p = "milli-volts";
                              scaleFactor = DIS_DATA_SCALE_NONE;
                              break;

                         case PLTFRM_INA219_1_SV_DEV_ID:
                              printf("+[INA219_CURRENT]   ");
                              unit_p = "milli-amps";
                              break;

                         case PLTFRM_NTCALUG02A_1_DEV_ID:
                              printf("+[NTC_THERM (Vishay NTCALUG02A)]   ");
                              unit_p = "deg C";
                              break;

                         case PLTFRM_NTCALUG02A_2_DEV_ID:
                              printf("+[NTC_THERM (Vishay NTCALUG02A)]   ");
                              unit_p = "deg C";
                              break;
                               
                         // printf("+[NTC_THERM (Semtech 103AT-4)]   ");

                         case PLTFRM_BAR_CODE_SCANNER_1_DEV_ID:
                              printf("+[Bar Code]     ");
                              unit_p = "";
                              break;

                         case PLTFRM_MAX_SONAR_1_DEV_ID:
                              printf("+[Dist_MaxSonar]  ");
                              unit_p = "Inches";
                              break;
                         
                         case PLTFRM_CHIRP_PWLA_1_DEV_ID:
                              printf("+[Moisture_CHIRP] ");
                              unit_p = "na";
                              break;

                         case PLTFRM_WSMS100_1_DEV_ID:
                              printf("+[Moisture_WSMS100]   ");
                              scaleFactor = DIS_DATA_SCALE_CENTI;
                              unit_p = "%";
                              break;

                         case PLTFRM_ON_CHIP_TEMP_SENSOR_DEV_ID:

                              printf("+[Temp_MSP430]    ");
                              unit_p = "deg C";
                              break;

                         case PLTFRM_TSL45315_1_DEV_ID:
                        	 if(myhubverbose){
                        		 printf("\n MyHub - 50 \n");
                        	 }
                              printf("+[Light_TSL45315] ");
                              unit_p = "Lux";
                              break;

                         case PLTFRM_BATT_1_DEV_ID:

                              printf("+[Batt Voltage] ");
                              unit_p = "Volts";
                              break;

                         case PLTFRM_EXT_VOLTAGE_MON_DEV_ID:

                              printf("+[Ext Voltage] ");
                              scaleFactor = DIS_DATA_SCALE_MILLI;
                              unit_p = "Volts";
                              break;

                         case PLTFRM_HE055T01_1_DEV_ID:
                              printf("+[Current_HE055T01] ");
                              unit_p = "mA";
                              break;

                         default:

                              printf("[Unknown]");
                              break;
                      }

                      if (snsrId == PLTFRM_GEN_VOLT_MON_DEV_ID
                          || snsrId == PLTFRM_ON_CHIP_VCC_SENSOR_DEV_ID)
                          scaleFactor = DIS_DATA_SCALE_MILLI;
                      
                      if (snsrId == PLTFRM_GEN_CURRENT_MON_DEV_ID)
                          scaleFactor = DIS_DATA_SCALE_NONE;

                      if (snsrId == PLTFRM_LM75B_1_DEV_ID
                          || snsrId == PLTFRM_BATT_1_DEV_ID
                          || snsrId == PLTFRM_AD7797_1_DEV_ID
                          || snsrId == PLTFRM_GEN_CURRENT_MON_DEV_ID
                          || snsrId == PLTFRM_GEN_VOLT_MON_DEV_ID
                          || snsrId == PLTFRM_ON_CHIP_VCC_SENSOR_DEV_ID
                          || snsrId == PLTFRM_ON_CHIP_TEMP_SENSOR_DEV_ID
                          || snsrId == PLTFRM_MP3V5050GP_1_DEV_ID
                          || snsrId == PLTFRM_MP3V5010_1_DEV_ID
                          || snsrId == PLTFRM_MPXV5010G_1_DEV_ID
                          || snsrId == PLTFRM_MP3V5004GP_1_DEV_ID
                          || snsrId == PLTFRM_FC_28_1_DEV_ID
                          || snsrId == PLTFRM_ACS712_1_CURRENT_DEV_ID
                          || snsrId == PLTFRM_ACS712_2_CURRENT_DEV_ID
                          || snsrId == PLTFRM_LLS_1_DEV_ID
                          || snsrId == PLTFRM_ACS712_3_CURRENT_DEV_ID
                          || snsrId == PLTFRM_SHT10_1_RH_DEV_ID
                          || snsrId == PLTFRM_SHT10_1_TEMP_DEV_ID
                          || snsrId == PLTFRM_WSMS100_1_DEV_ID
                          || snsrId == PLTFRM_EXT_VOLTAGE_MON_DEV_ID)
                      {
                          float valF = snsrOp;

                          printf("\n MyHub scale  factor - %d / snsrOp %u \n", scaleFactor, snsrOp);
                          
                          switch (scaleFactor)
                          {
                             case DIS_DATA_SCALE_MICRO:
                                  valF /= 1000;
                                  valF /= 1000;
                                  break;

                             case DIS_DATA_SCALE_MILLI:
                                  valF /= 1000;
                                  break;

                             case DIS_DATA_SCALE_CENTI:
                                  valF /= 100;
                                  break;

                             case DIS_DATA_SCALE_DECI:
                                  valF /= 10;
                                  break;

                             default:
                                  break;
                          }

                          if (snsrId == PLTFRM_AD7797_1_DEV_ID)
                          { 
                              if (__calWt1Flag)
                              {
                                  float avgW1;
                                  __calSampleCnt ++;
                                  __calTotal += valF;
                                  avgW1 = __calTotal;
                                  avgW1 /= __calSampleCnt;

                                  if (valF < minAdcVal)
                                      minAdcVal = valF;
                                  if (valF > maxAdcVal)
                                      maxAdcVal = valF;

                                  printf("ADC output for weight 1 <%f> \n", valF);
                                  printf("               Min ADC output for weight 1 <%f> \n", minAdcVal);
                                  printf("               Max ADC output for weight 1 <%f> \n", maxAdcVal);
                                  printf("               Average ADC output for weight 1 <%f> \n", avgW1);
                                  if (__calSampleCnt == 8)
                                      exit(0);
                              }
                              else
                              {
                                  if (__calWt2Flag)
                                  {
                                      float avgW1;
                                      __calSampleCnt ++;
                                      __calTotal += valF;
                                      avgW1 = __calTotal;
                                      avgW1 /= __calSampleCnt;
                                  
                                      if (valF < minAdcVal)
                                          minAdcVal = valF;
                                      if (valF > maxAdcVal)
                                          maxAdcVal = valF;
                                      printf("ADC output for weight 2 <%f> \n", valF);
                                      printf("               Min ADC output for weight 2 <%f> \n", minAdcVal);
                                      printf("               Max ADC output for weight 2 <%f> \n", maxAdcVal);
                                      printf("               Average ADC output for weight 2 <%f> \n", avgW1);
                                      if (__calSampleCnt == 8)
                                          exit(0);
                                  }
                                  else
                                  {
                                      if (__loadCellMVal != 0)
                                      {
                                          float temp = valF;
                                          // printf("\n adcVal - %f \n", valF);
                                          temp -= __loadCellCVal;
                                          temp /= __loadCellMVal;
                                          unit_p = "Kilo-grams";
                                          if (temp < 0)
                                              temp = 0;
                                          printf(" <%f %s> \n", temp, unit_p);
                                      }
                                      else
                                      {
                                          unit_p = "adc count";
                                          printf(" <%f %s> \n", valF, unit_p);
                                      }
                                  }
                              }
                          }

                          if (snsrId == PLTFRM_MP3V5004GP_1_DEV_ID)
                          {
#if 1
                              if (__latestVccSet)
                              {
                                  float temp;

                                  // Vout = Vs*(0.2*p + 0.2) +/- (2.5 % of VFSS)

                                  // p = ((Vout / Vs) - 0.2) * 5
                                  // Inches of water = p * 101.97

                                  printf("\n valF %f / latestVcc %f \n", valF, __latestVcc);

                                  temp = valF;
                                  temp /= __latestVcc;
                                  temp -= 0.2187;   // -0.2
                                  temp /= .176;  // / 0.2
                                  temp *= 101.97;  // in mm of water
                                   
                                  printf(" <%f mm of water> \n", temp); 
                              }
                              else
                              {
                                  printf(" <%f %s> \n", valF, "milli-volts");
                              }
#else
                              float temp;
                              printf("\n valF %f / sensor vcc %f \n", valF, 5.0);
                              temp = valF;
                              temp /= 5;  // __latestVcc;
                              temp -= 0.2;
                              temp *= 5; 
                              temp *= 101.97;  // in mm of water
                                   
                              printf(" <%f mm of water> \n", temp); 

#endif
                          }
                          
                          if (snsrId == PLTFRM_MPXV5010G_1_DEV_ID)
                          {
                              const float cVal = .27494;
                              const float mVal = 3.882;
                              // V = M*(Height in m) + C
                              // H = (V - C) / M 
                              // H = (mV/1000 - C) / M

                              valF -= cVal;
                              valF /= mVal;

                              // Height in mm
                              valF *= 1000;

                              // printf("%f \n", valF);
                          }

                          if (snsrId == PLTFRM_MP3V5050GP_1_DEV_ID)
                          {
                              if (__latestVccSet)
                              {
                                  float temp, minP, maxP;

                                  // printf("Snsr Op - %f volts \n", valF);
                                  // printf("Supply - %f volts \n", __latestVcc);

                                  // Vout = Vcc*(0.018*P +  0.04) + ERRORv
                                  // P (in kPa) = (Vout - (Vcc * .04) - ERRORv) / (Vcc * .018)
                                  // ERROR = (Presure_error * 1 * .018 * Vcc)
                                  // Where "pressure_error" is max +/- 1.25 kPa

                                  // Vout is in milli-volts
                                  // Vcc is in milli-volts 

                                  // P = ((Vout - Vcc/25)/(Vcc * .018)) - ERRORv
                                  // P = ((Vout - Vcc/25)/(Vcc * .018)) - Pressure_error

                                  // Vout/Vcc = .017217*P + 0.04732
                                  // P = (Vout/Vcc - .04732) / 0.017217 
                                  
#if 0 
                                  temp = __latestVcc;
                                  temp /= 25;
                                  temp = valF - temp;
                                  temp /= __latestVcc;
                                  temp /= .018;
#else
                                  temp = valF;
                                  temp /= __latestVcc;
                                  temp -= .0490512; // .04732;
                                  temp /= 0.014156;  // 0.017217;
#endif 
                                  minP = temp - 1.25;
                                  maxP = temp + 1.25;
                              
                                  printf(" <Snsr Op %f V / Supply %f V>  \n",
                                         valF, __latestVcc);

                                  printf("               <%f %s < %f %s < %f %s> \n", 
                                         minP, unit_p, temp, unit_p, maxP, unit_p);

                                  printf("               <%f mm < %f mm < %f mm \n",
                                         minP*101.971621298, temp*101.9716212984, maxP*101.971621298); 
                                  
                              }
                              else
                              {
                                  printf(" <%f %s> \n", valF, "milli-volts");
                              }
                          }

                          if (snsrId != PLTFRM_AD7797_1_DEV_ID
                              && snsrId != PLTFRM_MP3V5050GP_1_DEV_ID
                              && snsrId != PLTFRM_MP3V5004GP_1_DEV_ID)
                          {
                              printf(" <%f %s> \n", valF, unit_p);
                          }
                          
                          if (snsrId == PLTFRM_ON_CHIP_VCC_SENSOR_DEV_ID)
                          {
                              __latestVcc = valF;
                              __latestVccSet = 1;
                          }
                      }
                      else
                      {
                          if (snsrId == PLTFRM_BAR_CODE_SCANNER_1_DEV_ID)
                          {
                              buff3_p[tlvLen3] = '\0';
                              printf(" <%s %s> \n", buff3_p, unit_p);
                          }
                          else
                          {
                              if (snsrId == PLTFRM_NTCALUG02A_1_DEV_ID
                                  || snsrId == PLTFRM_NTCALUG02A_2_DEV_ID)
                              {
                                  double _r, _rl;
                                  double t25 = 25 + 273.15;
                                  double r_t = (double)snsrOp; // 6794;
                                  // printf("\n  sensor op : %d  \n",  snsrOp);
                                  int b25by85;

                                  switch (snsrId)
                                  {
                                     case PLTFRM_NTCALUG02A_1_DEV_ID:
                                     case PLTFRM_NTCALUG02A_2_DEV_ID:
                                          _r = NTC_THERM_NTCALUG02A_R25_VAL;
                                          b25by85 = NTC_THERM_NTCALUG02A_B_25_85_VAL;
                                          break;

                                          // _r = NTC_THERM_103AT_4_R25_VAL;
                                          // b25by85 = NTC_THERM_103AT_4_B_25_85_VAL;
                                          // break;

                                     default:
                                          break;
                                  }

                                  // loge(R25/RT) = B * (1/T25 - 1/T)
                                  // 1/T = 1/T25 - loge(R25/RT)/B
                                  // T = 1 / (1/T25 - loge(R25/RT)/B)
    
                                  _r /= r_t;
                                  _rl = log(_r);
                                  _rl /= b25by85;
                                  _r = 1;
                                  _r /= t25;
                                  _r -= _rl;
                                  _r = (1 / _r);
                                  printf(" <%f %s> \n", _r - 273.15, unit_p);

#if 0
                                  _r = NTC_THERM_103AT_4_R25_VAL + 100;
                                  _r /= r_t;
                                  _rl = log(_r);
                                  _rl /= b25by85;
                                  _r = 1;
                                  _r /= t25;
                                  _r -= _rl;
                                  _r = (1 / _r);
                                  printf(" [+1] <%f %s> \n",  _r - 273.15, unit_p);
                                  _r = NTC_THERM_103AT_4_R25_VAL + 100;
                                  _r /= r_t;
                                  _rl = log(_r);
                                  _rl /= b25by85 - (b25by85/100);
                                  _r = 1;
                                  _r /= t25;
                                  _r -= _rl;
                                  _r = (1 / _r);
                                  printf(" [+2] <%f %s> \n",  _r - 273.15, unit_p);
#endif
                              }
                              else
                              {
                                  if (snsrId == PLTFRM_INA219_1_SV_DEV_ID)
                                  {
                                      // Reported value is voltage in mv * 100
                                      float opVal = (int)snsrOp;
                                      opVal /= 100;
                                      opVal *= 10;  // shunt resistance of 0.1 ohms
                                      printf(" <%f %s> \n", opVal, unit_p);

                                  }
                                  else
                                  {
                                      if (snsrId == PLTFRM_MDS_1_DEV_ID)
                                          printf(" <%s> \n", snsrOp ? "On" : "Off");
                                      else
                                      {
                                          if (snsrId == PLTFRM_VIBRATION_SNSR_1_DEV_ID)
                                              printf(" <%s> \n", snsrOp ? "Yes" : "No");
                                          else 
                                          { 
                                              if (snsrId == PLTFRM_MAG3110_1_DEV_ID)
                                              {
                                                  if (tlvLen3 == 6)
                                                  {
                                                      short magFldInt = (short)__ntohs(buff3_p);
                                                      float magFldIntF = magFldInt;
                                                      magFldIntF *= 0.1;
                                                      // printf("x: %d/%f uT, ", magFldInt, magFldIntF);
                                                      printf("x: %f uT, ", magFldIntF);

                                                      magFldInt = (short)__ntohs(buff3_p + 2);
                                                      magFldIntF = magFldInt;
                                                      magFldIntF *= 0.1;
                                                      // printf("y: %d/%f uT, ", magFldInt, magFldIntF);
                                                      printf("y: %f uT, ", magFldIntF);

                                                      magFldInt = (short)__ntohs(buff3_p + 4);
                                                      magFldIntF = magFldInt;
                                                      magFldIntF *= 0.1;
                                                      // printf("z: %d/%f uT \n", magFldInt, magFldIntF);
                                                      printf("z: %f uT \n", magFldIntF);
                                                  }
                                              }
                                              else
                                              {
                                                  if (strlen(unit_p) > 0)  
                                                      printf(" <%d %s> \n", snsrOp, unit_p);
                                                  else
                                                      printf(" <%d> \n", snsrOp);
                                              }
                                          }   
                                      }
                                  }
                              }
                          }
                      
                          if (verbose){
                               printf("\nSnsrOp<%d>", snsrOp);
                          }


                        }


                      if(myhubverbose){
        				printf("\n MyHub Sensor Value SnsrOp <%d>", snsrOp);
        				printf("\n MyHub Sensor Id    snsrId <%d>", snsrId);
        				printf("\n MyHub Sensor Id    snsrId <%x>", MAC_ADDRESS);

                      }


                  }



              }
              if(myhubverbose){
            	  printf("\n MyHub End Of Sensor Data Read \n");
              }

          }//End Of Sensor Event Processing-While Ends Here
       }
   }


   printf("----------------------------------------------------------------------------------- \n");

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
void GW_processRcvdMsg_1(int expShortAddr, unsigned char *buff_p, 
                         int offset, int gwMsgType, int msgLen)
{
    /*
     * GW payload format - 
     * Type (2 bytes)
     */ 

    switch (gwMsgType)
    {
       case LPWMN_GW_MSG_TYPE_RELAY_FROM_NODE:
            {
                // printf("----------------------------------------------------------------------------------- \n");
                GW_processNodeMsg(expShortAddr, buff_p + offset, msgLen); 
                // printf("----------------------------------------------------------------------------------- \n");
            }
            break;

       case LPWMN_GW_MSG_TYPE_EVENT:
            {
                printf("\n Received  event ... \n");
                GW_processEvt(buff_p + offset, msgLen); 
            }
            break;

       default:
            break;
    }


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
void GW_handleNwkDataTraffic(int expShortAddr)
{
    int off = 0, readLen, pyldLen, totReadLen = 0;
    int currMsgType = 0xffff;
    readLen = UART_MSG_HDR_PYLD_CRC_FIELD_OFF;

    while (1)
    {
       int rc;

       if (verbose)
           printf("\noff<%d>/readLen<%d>/totReadLen<%d> \n", off, readLen, totReadLen);

       rc = readPort(serInputBuff + off, readLen);
       if (rc != readLen)
       {
           printf("\nreadPort() failed !! ................. \n");
           close(uart_cntxt.serialFd);
           break;
       }

       totReadLen += readLen;
       off += readLen;

       if (verbose)
           printf("\n rc<%d> \n", rc);

       switch (totReadLen)
       {
          case UART_MSG_HDR_PYLD_CRC_FIELD_OFF: 
               {
                   int idx;
                   unsigned short calcCrc16, rxdCrc16;

                   // Get the message length

                   if (verbose)
                       printf("\n-------------------------------------------------------------------------------------------------");

                   if (verbose)
                   {
                       printf("\nRead <%d> bytes --- ", readLen);
                       for (idx=0; idx<totReadLen; idx++)
                            printf(" <0x%x> ", serInputBuff[idx]);
                   }

                   calcCrc16 = crc16(serInputBuff, UART_MSG_HDR_HDR_CRC_FIELD_OFF);
                   rxdCrc16 = serInputBuff[UART_MSG_HDR_HDR_CRC_FIELD_OFF];
                   rxdCrc16 = (rxdCrc16 << 8) + serInputBuff[UART_MSG_HDR_HDR_CRC_FIELD_OFF + 1];
                   if (verbose)
                       printf("\ncalc-crc16<0x%x> rcvd-crc16<0x%x>\n", calcCrc16, rxdCrc16);

                   if (calcCrc16 != rxdCrc16)
                   {
                       for (idx=0; idx<UART_MSG_HDR_PYLD_CRC_FIELD_OFF-1; idx++)
                            serInputBuff[idx] = serInputBuff[idx+1];
                       off = UART_MSG_HDR_PYLD_CRC_FIELD_OFF - 1;
                       readLen = 1;
                       totReadLen = off;
                       break;
                   }

                   pyldLen = serInputBuff[UART_MSG_HDR_PYLD_LEN_FIELD_OFF];
                   pyldLen = (pyldLen << 8) |  serInputBuff[UART_MSG_HDR_PYLD_LEN_FIELD_OFF + 1];

                   currMsgType = serInputBuff[UART_MSG_HDR_MSG_TYPE_FIELD_OFF];
                   currMsgType = (currMsgType << 8) | serInputBuff[UART_MSG_HDR_MSG_TYPE_FIELD_OFF + 1];

                   if (verbose)
                       printf("\nMessage Type<%d> / Length<%d>", currMsgType, pyldLen);                   

                   readLen = pyldLen + UART_MSG_HDR_PYLD_CRC_FIELD_LEN;
               }
               break;

          default:
               {
                   GW_processRcvdMsg_1(expShortAddr, serInputBuff, UART_MSG_HDR_LEN, currMsgType, pyldLen);
                   memset(serInputBuff, 0, sizeof(serInputBuff));
                   readLen = UART_MSG_HDR_PYLD_CRC_FIELD_OFF;
                   totReadLen = 0;
                   off = 0;
               }
               break;
       }
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
int mainT(int argc, const char* argv[] )
{
    int rc = 0;
    int currMsgType = 0xffff;

    if (argc < 2)
    {
        print_usage();
        return 1;
    }

    if (strstr(argv[0], "gwd") != NULL)
    {
        if (daemon(1, 1) < 0)
        {
            printf("\n daemon() failed !! - errno<%d> \n", errno);
            return 1;
        }
    }
      
    if (strcmp(argv[1], "help") == 0x0
        || argv[1][0] == '?'
        || (argc >= 3 && (strcmp(argv[2], "help") == 0x0
                          || argv[2][0] == '?')))
    {
        printf("List of supported command strings :- \n");
        printf("------------------------------------------------------------------------------ \n");
        printf("* sc - start the coordinator \n");
        printf("* mon - display messages sent by sensor nodes in the network\n");
        printf("* mon_node - display messages sent by specifed sensor node\n             example: mon_node 5 \n");
        printf("* chm  - request gateway to monitor the RSSI on specified channel \n");
        printf("* nc - get number of nodes in the network \n");
        printf("* nid - get the 16 bit network id \n");
        printf("* rfb - get the radio frequency band \n");
        printf("* grch - get the radio channel number \n");
        printf("* srch - set the radio channel number \n");
        printf("* rcf - get the radio carrier frequency \n");
        printf("* grbr - get the radio baud rate \n");
        printf("* grm - get the radio modulation format \n");
        printf("* rtp - get the transmit power of the radio on the coordinator \n");
        printf("* cea - get the coordinator's 64 bit address \n");
        printf("* nl - list all nodes in the network \n");
        printf("* rfl - list all RFDs in the network \n");
        printf("* ffl - list all FFDs in the network \n");
        printf("* ut - coord up time (in seconds) \n");
        printf("* rpn - get the part number of the radio on the coordinator \n");
        printf("* rstc - request coordinator to reboot itself \n");
        printf("* rstn - request specified node to reboot itself \n");
        printf("* rstbc - broadcast request to all FFDs to reboot \n");
        printf("* noopbc - broadcast dummy request to all FFDs \n");
        printf("* nj-ena - allow nodes to join the coordinator \n");
        printf("* nj-dis - do not allow nodes to join the coordinator \n");
        printf("* bcn-r-a - Configure node to accept beacon requests \n");
        printf("* bcn-r-d - Configure node to drop beacon requests \n");
        printf("* bcn-r-s - Is node accepting or dropping beacon request currently ? \n");
        printf("* nj-sts - are nodes currently allowed to join the coordinator ? \n");
        printf("* wl-get - Get white list (list of all nodes allowed to join this nwk) \n");
        printf("* wl-add - Add node to white list \n           example: wl-add  fc:c2:3d:0:0:0:e9:54 \n");
        printf("* wl-del - Remove node from white list \n           example: wl-del  fc:c2:3d:0:0:0:e9:54 \n");
        printf("* wl-clr - Delete the white list \n");
        printf("* bl-add - Add node to black list \n           example: bl-add  fc:c2:3d:0:0:0:e9:54 \n");
        printf("* bl-del - Remove node from black list \n           example: bl-del  fc:c2:3d:0:0:0:e9:54 \n");
        printf("* bl-clr - Delete the black list \n");
        printf("* gcrtvec - Get number of valid entries in the coordinator's routing table \n");
        printf("* gcrtvem - Get max entry count in the coordinator's routing table \n");
        printf("* grnhc  - Get next hop in route to specified node (on coord)  \n           example: grnhc 5 \n");
        printf("* grnhf  - Get next hop in route to specified node (on any FFD) \n           example: grnhf 2 5 \n");
        printf("* gnten - Get neighbor table entry on specified node \n          format: gnte <short-address> <short-address> \n");
        printf("* cfg-dpi - Configure node's data push interval \n            format: cfg-dpi  <short-address>  <interval in seconds> \n            example: cfg-dpi 5 3 \n");
        printf("* gnsc - Node sensor count \n         format: gnsc <short-address> \n         example: gnsc 0x5 \n");
        printf("* gcfbd - Get the build date of the firmware on the coordinator \n");
        printf("* gcfbt - Get the build time of the firmware on the coordinator \n");
        printf("* gnlfe - Get specified node's last fatal error id \n          format: gnlfe <short-address> \n          example: gnlfe 5 \n");
        printf("* dioc - Control digital output on remote node \n         format: dioc <short-address> <port (1-n)> <pin (0-7)> <val (0 or 1)> \n");
        printf("* gpos - Get output state of all GPIO ports on specified remote node \n         format: gpos <short-address> \n");
        printf("* gav - Get attribute value on specified node \n        format: gav <short-address> <attr-id> \n");
        printf("* glav - Get list attribute value on specified node \n        format: glav <short-address> <attr-id> <attr-idx>\n");
        printf("* sav - Set attribute value on specified node \n        format: sav <short-address> <attr-id> <attr-val> \n");
        printf("* gcfmdc - Get free mem descriptor count on the coordinator  \n");
        printf("* uc_fu - Upgrade firmware on specified node \n          format: uc_fu <short-address> <file-name> \n");
        printf("* sw_sby_i - Request node to run standby image \n          format: sw_sby_i <short-address> \n");
        printf("------------------------------------------------------------------------------ \n");
        return 0;
    }

    if (argc < 3)
    {
        print_usage();
        return 1;
    }

    if (cfgPort((char *)argv[1], B38400) < 0)
        return 2;

    if (strcmp(argv[2], "sc") == 0x0)
    {
        rc = GW_startCoordReq( );
        return rc;
    }

    if (strcmp(argv[2], "nc") == 0x0)
    {
        rc = GW_getNodeCntReqHndlr( );
        return rc;
    }

    if (strcmp(argv[2], "ut") == 0x0)
    {
        rc = GW_getCoordUpTime( );
        return rc;
    }

    if (strcmp(argv[2], "nid") == 0x0)
    {
        rc = GW_getLPWMNIdReqHndlr( );
        return rc;
    }

    if (strcmp(argv[2], "rfb") == 0x0)
    {
        rc = GW_getRadioFreqBand( );
        return rc;
    }

    if (strcmp(argv[2], "grbr") == 0x0)
    {
        rc = GW_getRadioBaudRate( );
        return rc;
    }

    if (strcmp(argv[2], "grmf") == 0x0)
    {
        rc = GW_getRadioModFmt( );
        return rc;
    }

    if (strcmp(argv[2], "grch") == 0x0)
    {
        rc = GW_getRadioChannReqHndlr( );
        return rc;
    }

    if (strcmp(argv[2], "grm") == 0x0)
    {
        rc = GW_getRadioModFmt( );
        return rc;
    }

#if 1
    // use gw.exe sav 1 10 0-9
    if (strcmp(argv[2], "srch") == 0x0)
    {
        if (argc < 4)
        {
            printf("please enter valid channel number (> 0) !! \n");
            rc = 9;
        }
        else
            rc = GW_setRadioChannReqHndlr(argv[3]);
        return rc;
    }
#endif

    if (strcmp(argv[2], "rcf") == 0x0)
    {
        rc = GW_getRadioCarrierFreq( );
        return rc;
    }

    if (strcmp(argv[2], "cea") == 0x0)
    {
        rc = GW_getCoordExtAddrReqHndlr( );
        return rc;
    }

    if (strcmp(argv[2], "rtp") == 0x0)
    {
        rc = GW_getCoordRadioTxPwr();
        return rc;
    }

    if (strcmp(argv[2], "nl") == 0x0)
    {
        rc = GW_getNodeListReqHndlr(ALL_NODES_LIST);
        return rc;
    }

    if (strcmp(argv[2], "rfl") == 0x0)
    {
        rc = GW_getNodeListReqHndlr(RFD_LIST_ONLY);
        return rc;
    }

    if (strcmp(argv[2], "ffl") == 0x0)
    {
        rc = GW_getNodeListReqHndlr(FFD_LIST_ONLY);
        return rc;
    }

    if (strcmp(argv[2], "rpn") == 0x0)
    {
        rc = GW_getCoordRadioPartNr( );
        return rc;
    }

    if (strcmp(argv[2], "rstc") == 0x0)
    {
        rc = GW_rebootCoordReq( );
        return rc;
    }

    if (strcmp(argv[2], "noopbc") == 0x0)
    {
        if (argc < 4)
        {
            printf("Please enter valid broadcast hop count (>= 1 & <= 15) !! \n");
            rc = 8;
        }
        else 
        {
            unsigned int hopCnt;

            rc = sscanf(argv[3], "%u", &hopCnt);
            if (rc != 1 || hopCnt < 1 || hopCnt > 15)
            {
                printf("Please enter valid broadcast hop count (>= 1 & <= 15) !! \n");
                rc = 9;
            }
            else
                rc = GW_bcFFDNoOp(hopCnt);
        }

        return rc;
    }

    if (strcmp(argv[2], "rstbc") == 0x0)
    {
        if (argc < 5)
        {
            printf("Please enter valid broadcast hop count (>= 1 & <= 15) and reboot delay !! \n");
            rc = 8;
        }
        else 
        {
            unsigned int hopCnt;

            rc = sscanf(argv[3], "%u", &hopCnt);
            if (rc != 1 || hopCnt < 1 || hopCnt > 15)
            {
                printf("Please enter valid broadcast hop count (>= 1 & <= 15) !! \n");
                rc = 9;
            }
            else
            {
                unsigned int delay;
               
                rc = sscanf(argv[4], "%u", &delay);
                if (rc != 1 || delay < 1 || delay> 65535)
                {
                    printf("Please enter valid reboot delay (in seconds) (> 1 & <= 65535) !! \n");
                    rc = 10;
                }
                else
                    rc = GW_bcFFDReboot(hopCnt, delay);
            }
        }

        return rc;
    }

    if (strcmp(argv[2], "rstn") == 0x0)
    {
        if (argc < 4)
        {
            printf("Please enter a valid short address (> 1 & <= %u) !! \n",
                   LPWMN_MAX_UNICAST_SHORT_ADDR);
            rc = 8;
        }
        else
        {
            unsigned int shortAddr;
 
            rc = sscanf(argv[3], "%u", &shortAddr);
            if (rc != 1 || shortAddr < 2)
            {
                printf("please enter a valid short address (> 1 & <= %u) !! \n", 
                       LPWMN_MAX_UNICAST_SHORT_ADDR);
                rc = 9;
            }
            else
            {
                rc = GW_rebootNodeReq(shortAddr);
            }
        }

        return rc;
    }

    if (strcmp(argv[2], "nj-ena") == 0x0)
    {
        rc = GW_enableNwkJoins( );
        return rc;
    }

    if (strcmp(argv[2], "nj-dis") == 0x0)
    {
        rc = GW_disableNwkJoins( );
        return rc;
    }

    if (strcmp(argv[2], "nj-sts") == 0x0)
    {
        rc = GW_getNwkJoinCtrlSts( );
        return rc;
    }

    if (strcmp(argv[2], "bl-add") == 0x0)
    {
        if (argc < 4)
        {
            printf("please enter an eui-64 address !! \n");
            rc = 8;
        }
        else
            rc = GW_addNodeToBlackList(argv[3]);
        return rc;
    }

    if (strcmp(argv[2], "bl-del") == 0x0)
    {
        if (argc < 4)
        {
            printf("please enter an eui-64 address !! \n");
            rc = 8;
        }
        else
            rc = GW_delNodeFromBlackList(argv[3]);
        return rc;
    }

    if (strcmp(argv[2], "bl-clr") == 0x0)
    {
        rc = GW_clrBlackList( );
        return rc;
    }

    if (strcmp(argv[2], "bl-get") == 0x0)
    {
        rc = GW_getBlackListReqHndlr( );
        return rc;
    }
    
    if (strcmp(argv[2], "wl-add") == 0x0)
    {
        if (argc < 4)
        {
            printf("please enter an eui-64 address !! \n");
            rc = 8;
        }
        else
            rc = GW_addNodeToWhiteList(argv[3]);
        return rc;
    }

    if (strcmp(argv[2], "wl-del") == 0x0)
    {
        if (argc < 4)
        {
            printf("please enter an eui-64 address !! \n");
            rc = 8;
        }
        else
            rc = GW_delNodeFromWhiteList(argv[3]);
        return rc;
    }

    if (strcmp(argv[2], "wl-clr") == 0x0)
    {
        rc = GW_clrWhiteList( );
        return rc;
    }

    if (strcmp(argv[2], "wl-get") == 0x0)
    {
        rc = GW_getWhiteListReqHndlr( );
        return rc;
    }
    
    if (strcmp(argv[2], "sc") == 0x0)
    {
        rc = GW_startCoordReq( );
        return rc;
    }

    if (strcmp(argv[2], "chm") == 0x0)
    {
        int error = 0, samplingInterval, sampleCnt;

        if (argc >= 4)
        {
            rc = sscanf(argv[3], "%u", &sampleCnt);
            if (rc != 1 || sampleCnt <= 0 || sampleCnt > 65535)
            {
                printf("Please specify valid sample count (> 0 && <= 65535) !! \n");
                error = 1;
                rc = 8;
            }
            else
            {
                if (argc >= 5)
                {
             
                    rc = sscanf(argv[4], "%u", &samplingInterval);
                    if (rc != 1 || samplingInterval <= 0)
                    {
                        printf("Please specify valid sampling interval (in milliseconds) !! \n");
                        error = 1;
                        rc = 8;
                    }
                }
                else
                    samplingInterval = 1;
            }
        }
        else
        {
            sampleCnt = 1000;
            samplingInterval = 1;
        }

        if (error == 0)
            rc = GW_startChannMon(sampleCnt, samplingInterval);

        return rc;
    }

    if (strcmp(argv[2], "gnten") == 0x0)
    {
        if (argc >= 4)
        {
            unsigned int ffdSA;

            rc = sscanf(argv[3], "%u", &ffdSA);
            if (rc != 1 || ffdSA < 2 || ffdSA > LPWMN_MAX_UNICAST_SHORT_ADDR)
            {
                printf("please enter a valid FFD address (> 1 & <= %u) !! \n", 
                       LPWMN_MAX_UNICAST_SHORT_ADDR);
                rc = 8;
            }
            else
            {
                if (argc >= 5)
                {
                    unsigned int nteSA;

                    rc = sscanf(argv[4], "%u", &nteSA);
                    if (rc != 1 || nteSA < 1 || nteSA > LPWMN_MAX_UNICAST_SHORT_ADDR)
                    {
                        printf("please enter short address whose corresponding entry is required (> 1 & <= %u) !! \n", 
                               LPWMN_MAX_UNICAST_SHORT_ADDR);
                    }
                    else
                    {
                        rc = 9;
                        if (nteSA == ffdSA)
                        {
                            printf("Both addresses are 0x%x !! \n", nteSA);
                            rc = 12;
                        }
                        else
                            rc = GW_getNTEntryOnFFD(ffdSA, nteSA);
                    }
                }
            }
        }
        else
        {
            printf("please enter valid FFD address and entry short address (> 1 & <= %u) !! \n", 
                   LPWMN_MAX_UNICAST_SHORT_ADDR);
            rc = 10;
        }
 
        return rc;
    }


    if (strcmp(argv[2], "grnhf") == 0x0)
    {
        if (argc >= 4)
        {
            unsigned int ffdSA;

            rc = sscanf(argv[3], "%u", &ffdSA);
            if (rc != 1 || ffdSA < 2 || ffdSA > LPWMN_MAX_UNICAST_SHORT_ADDR)
            {
                printf("please enter a valid FFD address (> 1 & <= %u) !! \n", 
                       LPWMN_MAX_UNICAST_SHORT_ADDR);
                rc = 8;
            }
            else
            {
                if (argc >= 5)
                {
                    unsigned int nodeSA;

                    rc = sscanf(argv[4], "%u", &nodeSA);
                    if (rc != 1 || nodeSA < 1 || nodeSA > LPWMN_MAX_UNICAST_SHORT_ADDR)
                    {
                        printf("please enter a valid route-to node's address (> 1 & <= %u) !! \n", 
                               LPWMN_MAX_UNICAST_SHORT_ADDR);
                    }
                    else
                    {
                        rc = 9;
                        if (nodeSA == ffdSA)
                        {
                            printf("FFD address and route-to node's address can't be same !! \n");
                            rc = 12;
                        }
                        else
                            rc = GW_getRtNextHopOnFFD(ffdSA, nodeSA);
                    }
                }
            }
        }
        else
        {
            printf("please enter valid FFD address and route-to node's address (> 1 & <= %u) !! \n", 
                   LPWMN_MAX_UNICAST_SHORT_ADDR);
            rc = 10;
        }
 
        return rc;
    }
    
    if (strcmp(argv[2], "gcrtvec") == 0x0)
    {
        rc = GW_getCoordAttrVal(NWK_ROUTING_TABLE_VALID_ENTRY_CNT_ATTR_ID);
        return rc;
    }

    if (strcmp(argv[2], "gcrtmec") == 0x0)
    {
        rc = GW_getCoordAttrVal(NWK_ROUTING_TABLE_MAX_ENTRY_CNT_ATTR_ID);
        return rc;
    }

    if (strcmp(argv[2], "grnhc") == 0x0)
    {
        unsigned int shortAddr;

        if (argc >= 4)
        {
            rc = sscanf(argv[3], "%u", &shortAddr);
            if (rc != 1 || shortAddr < 2)
            {
                printf("please enter a valid short address (> 1 & <= %u) !! \n", 
                       LPWMN_MAX_UNICAST_SHORT_ADDR);
                rc = 8;
            }
            else
            {
                rc = GW_getRtNextHopOnCoord(shortAddr);
            }
        }
        else
        {
            printf("please enter valid node short address (> 0x1) !! \n");
            rc = 8;
        }

        return rc;
    }
    
    
    if (strcmp(argv[2], "bcn-r-a") == 0x0)
    {
        unsigned int shortAddr, attrId;
        if (argc >= 4)
        {
            rc = sscanf(argv[3], "%u", &shortAddr);
            if (rc != 1 || shortAddr < 1 || shortAddr > LPWMN_MAX_UNICAST_SHORT_ADDR)
            {
                printf("please enter a valid short address (>= 1 & <= %u) !! \n", 
                       LPWMN_MAX_UNICAST_SHORT_ADDR);
                rc = 8;
            }
            else
            {
                if (shortAddr == LPWMN_COORD_SHORT_ADDR)
                    rc = GW_cfgCoordToAcceptBcnReqs();
                else
                    rc = GW_cfgNodeToAcceptBcnReqs(shortAddr);
            }
        }
        else
        {
            printf("please enter a valid short address (>= 1 & <= %u) !! \n", 
                   LPWMN_MAX_UNICAST_SHORT_ADDR);
            rc = 9;
        }

        return rc;
    }


    if (strcmp(argv[2], "bcn-r-d") == 0x0)
    {
        unsigned int shortAddr, attrId;
        if (argc >= 4)
        {
            rc = sscanf(argv[3], "%u", &shortAddr);
            if (rc != 1 || shortAddr < 1 || shortAddr > LPWMN_MAX_UNICAST_SHORT_ADDR)
            {
                printf("please enter a valid short address (>= 1 & <= %u) !! \n", 
                       LPWMN_MAX_UNICAST_SHORT_ADDR);
                rc = 8;
            }
            else
            {
                if (shortAddr == LPWMN_COORD_SHORT_ADDR)
                    rc = GW_cfgCoordToDropBcnReqs();
                else
                    rc = GW_cfgNodeToDropBcnReqs(shortAddr);
            }
        }
        else
        {
            printf("please enter short address (>= 1 & <= %u) !! \n", 
                   LPWMN_MAX_UNICAST_SHORT_ADDR);
            rc = 9;
        }

        return rc;
    }


    if (strcmp(argv[2], "bcn-r-s") == 0x0)
    {
        unsigned int shortAddr, attrId;
        if (argc >= 4)
        {
            rc = sscanf(argv[3], "%u", &shortAddr);
            if (rc != 1 || shortAddr < 1 || shortAddr > LPWMN_MAX_UNICAST_SHORT_ADDR)
            {
                printf("please enter a valid short address (>= 1 & <= %u) !! \n", 
                       LPWMN_MAX_UNICAST_SHORT_ADDR);
                rc = 8;
            }
            else
            {
                if (shortAddr == LPWMN_COORD_SHORT_ADDR)
                    rc = GW_getCoordBcnReqHndlingCfg();
                else
                    rc = GW_getNodeBcnReqHndlingCfg(shortAddr);
            }
        }
        else
        {
            printf("please enter short address (>= 1 & <= %u) !! \n", 
                   LPWMN_MAX_UNICAST_SHORT_ADDR);
            rc = 9;
        }


        return rc;
    }
    
    if (strcmp(argv[2], "gcfmdc") == 0x0)
    {
        rc = GW_getCoordFreeMemDescCnt( );
        return rc;
    }
    
    if (strcmp(argv[2], "sw_sby_i") == 0x0)
    {
        if (argc >= 4)
        {
            unsigned int shortAddr;
            rc = sscanf(argv[3], "%u", &shortAddr);
            if (shortAddr <= 1 || shortAddr > LPWMN_MAX_UNICAST_SHORT_ADDR)
            {
                printf("please enter a valid short address (> 1 & <= %u) !! \n", 
                       LPWMN_MAX_UNICAST_SHORT_ADDR);
                rc = 8;
            }
            else
            {
                GW_sendImageSwReq(shortAddr);

                return 0;
            }
        }
        else
        {
            printf("please enter short address (> 1 & <= %u) and hex file !! \n", 
                   LPWMN_MAX_UNICAST_SHORT_ADDR);
            rc = 200;
        }
    }

    if (strcmp(argv[2], "uc_fu") == 0x0)
    {
        unsigned int shortAddr;
        
        if (argc >= 5)
        {
            rc = sscanf(argv[3], "%u", &shortAddr);
            if (rc != 1 || shortAddr < 1 || shortAddr > LPWMN_MAX_UNICAST_SHORT_ADDR)
            {
                printf("please enter a valid short address (>= 1 & <= %u) !! \n", 
                       LPWMN_MAX_UNICAST_SHORT_ADDR);
                rc = 8;
            }
            else
            {
                char const *file_p = argv[4];
                int fileFd;
                struct stat statBuff;
                char *fileBuff_p;

                fileFd = open(file_p, O_RDONLY);
                if (fileFd < 0)
                {
                    printf("Failed to open specified hex file <%s> !! - errno<%d> \n", 
                           file_p, errno);
                    return 500;
                }

                printf("Opened specified file <%s> fd<%d>\n", file_p, fileFd);
    
                if (fstat(fileFd, &statBuff) < 0)
                {
                    printf("Failed to stat specified hex file <%s> - errno<%d> !! \n", 
                           file_p, errno);
                    return 501;
                }

                printf("File length <%u bytes> \n", statBuff.st_size);

                fileBuff_p = (char *)malloc(statBuff.st_size);
                if (fileBuff_p == NULL)
                {
                    printf("malloc(%d) failed !! \n", statBuff.st_size);
                    return 502;
                }

                printf("Allocated buffer of size <%d bytes> \n", statBuff.st_size);

                {
                   int rdCnt = 0;
                   char *buff_p = fileBuff_p;
                   do
                   {
                      int rc, toRead = statBuff.st_size - rdCnt > 1024 ? 1024 : statBuff.st_size - rdCnt;
                      rc = read(fileFd, buff_p + rdCnt, toRead);
                      if (rc <= 0)
                      {
                          printf("read(%d) failed !! - rc<%d> / errno<%d> \n",
                                 statBuff.st_size - rdCnt, errno);
                          close (fileFd);
                          return 503;
                      }
                      rdCnt += rc;
                   } while (rdCnt < statBuff.st_size);
                }

                printf("Read file into RAM ... \n");

                close(fileFd);

                if (GW_procFwHexFile(fileBuff_p, statBuff.st_size) == 0)
                    return 504;

                GW_startUcProc(shortAddr);

                return 0;
            }
        }
        else
        {
            printf("please enter short address (>= 1 & <= %u) and hex file !! \n", 
                   LPWMN_MAX_UNICAST_SHORT_ADDR);
            rc = 200;
        }
    }

    if (strcmp(argv[2], "glav") == 0x0)
    {
        unsigned int shortAddr, attrId, attrIdx;
        if (argc >= 4)
        {
            rc = sscanf(argv[3], "%u", &shortAddr);
            if (rc != 1 || shortAddr < 1 || shortAddr > LPWMN_MAX_UNICAST_SHORT_ADDR)
            {
                printf("please enter a valid short address (>= 1 & <= %u) !! \n", 
                       LPWMN_MAX_UNICAST_SHORT_ADDR);
                rc = 8;
            }
            else
            {
                if (argc >= 5)
                {
                    rc = sscanf(argv[4], "%u", &attrId);
                    if (rc != 1 || attrId < 1 || attrId > 65535)
                    {
                        printf("please enter a valid attribute Id (>= 1 & <= 65535) !! \n");
                        rc = 9;
                    }
                    else
                    {
                        if (argc >= 6)
                        {
                            rc = sscanf(argv[5], "%u", &attrIdx);
                            if (rc != 1 || attrIdx > 65535)
                            {
                                printf("please enter valid attribute index (>= 0 & <= 65535) !! \n");
                                rc = 10;
                            }
                            else
                            {
                                if (shortAddr == LPWMN_COORD_SHORT_ADDR)
                                    rc = GW_getCoordListAttrVal(attrId, attrIdx);
                                else
                                    rc = GW_getNodeListAttrVal(shortAddr, attrId, attrIdx);
                            }
                        }
                        else
                        {
                            printf("please enter atribute index (>= 0) !! \n");
                            rc = 11;
                        }
                    }
                }
                else
                {
                    printf("please enter valid atribute id (>= 1) !! \n");
                    rc = 12;
                }
            }
        } 
        else
        { 
            printf("please enter short address (>= 1 & <= %u), attribute id and index !! \n", 
                   LPWMN_MAX_UNICAST_SHORT_ADDR);
            rc = 11;
        } 

        return rc;
    }


    if (strcmp(argv[2], "gav") == 0x0)
    {
        unsigned int shortAddr, attrId;
        if (argc >= 4)
        {
            rc = sscanf(argv[3], "%u", &shortAddr);
            if (rc != 1 || shortAddr < 1 || shortAddr > LPWMN_MAX_UNICAST_SHORT_ADDR)
            {
                printf("please enter a valid short address (>= 1 & <= %u) !! \n", 
                       LPWMN_MAX_UNICAST_SHORT_ADDR);
                rc = 8;
            }
            else
            {
                if (argc >= 5)
                {
                    rc = sscanf(argv[4], "%u", &attrId);
                    if (rc != 1 || attrId < 1 || attrId > 65535)
                    {
                        printf("please enter a valid attribute Id (>= 1 & <= 65535) !! \n");
                        rc = 9;
                    }
                    else
                    {
                        if (shortAddr == LPWMN_COORD_SHORT_ADDR)
                            rc = GW_getCoordAttrVal(attrId);
                        else
                            rc = GW_getNodeAttrVal(shortAddr, attrId);
                    }
                }
                else
                {
                    printf("please enter valid atribute id (>= 1) !! \n");
                    rc = 10;
                }
            }
        } 
        else
        { 
            printf("please enter short address (>= 1 & <= %u) and attribute id !! \n", 
                   LPWMN_MAX_UNICAST_SHORT_ADDR);
            rc = 11;
        } 

        return rc;
    }


    if (strcmp(argv[2], "sav") == 0x0)
    {
        unsigned int shortAddr, attrId, attrVal;
        if (argc >= 6)
        {
            rc = sscanf(argv[3], "%u", &shortAddr);
            if (rc != 1 || shortAddr < 1 || shortAddr > LPWMN_MAX_UNICAST_SHORT_ADDR)
            {
                printf("please enter a valid short address (>= 1 & <= %u) !! \n", 
                       LPWMN_MAX_UNICAST_SHORT_ADDR);
                rc = 8;
            }
            else
            {
               rc = sscanf(argv[4], "%u", &attrId);
               if (rc != 1 || attrId < 1 || attrId > 65535)
               {
                   printf("please enter a valid attribute Id (>= 1 & <= 65535) !! \n");
                   rc = 9;
               }
               else
               {
                   rc = sscanf(argv[5], "%u", &attrVal);
                   if (rc != 1)
                   {
                       printf("please enter a valid attribute value !! \n");
                       rc = 9;
                   }
                   else
                   {
                       if (shortAddr == LPWMN_COORD_SHORT_ADDR)
                           rc = GW_setCoordAttrVal(shortAddr, attrId, attrVal);
                       else
                           rc = GW_setNodeAttrVal(shortAddr, attrId, attrVal);
                   }
               }
            }
        }
        else
        {
            printf("Please enter valid short address, attribute id and attribute value !! \n");
            rc = 10;
        }

        return rc;
    }

    if (strcmp(argv[2], "cfg-dpi") == 0x0)
    {
        unsigned int shortAddr, pi;
        if (argc >= 4)
        {
            rc = sscanf(argv[3], "%u", &shortAddr);
            if (rc != 1 || shortAddr < 2)
            {
                printf("please enter a valid short address (> 1 & <= %u) !! \n", 
                       LPWMN_MAX_UNICAST_SHORT_ADDR);
                rc = 8;
            }
            else
            {
                if (argc >= 5)
                {
                    rc = sscanf(argv[4], "%u", &pi);
                    if (rc != 1 || pi == 0 || pi > 65535)
                    {
                        printf("please enter sensor interval (1 to 65535 secs) !! \n");
                        rc = 8;
                    }
                }
                else
                {
                    printf("please enter sensor interval (1 to 65535 secs) !! \n");
                    rc = 8;
                }
            }
        }
        else
        {
            printf("please enter sensor node's short address (> 0x1) !! \n");
        }

        if (rc == 1)
            rc = GW_cfgNodeDataPushInterval(shortAddr, pi);
        return rc;
    }

    if (strcmp(argv[2], "gcfbd") == 0x0)
    {
        rc = GW_getCoordFwBuildDate( );
        return rc;
    }

    if (strcmp(argv[2], "gcfbt") == 0x0)
    {
        rc = GW_getCoordFwBuildTime( );
        return rc;
    }

    if (strcmp(argv[2], "gnsc") == 0x0)
    {
        if (argc < 4)
        {
            printf("please enter a short address (> 0x1) !! \n");
            rc = 8;
        }
        else
            rc = GW_getNodeSensorCnt(argv[3]);

        return rc;
    }

    if (strcmp(argv[2], "panel_ctrl") == 0x0)
    {
        if (argc < 4)
        {
            printf("please enter point to control (l1-l6 or f1-f2 or d1-d2) !! \n");
            rc = 8;
        }
        else
            rc = GW_sendSparshPanelCtrlCmd(argv[3], argv[4]);

        return rc;
    }

    if (strcmp(argv[2], "gnlfe") == 0x0)
    {
        if (argc < 4)
        {
            printf("please enter a short address (>= 0x1) !! \n");
            rc = 8;
        }
        else
            rc = GW_getNodeLastFEId(argv[3]);

        return rc;
    }

    if (strcmp(argv[2], "gpos") == 0x0)
    {
        if (argc < 4)
        {
            printf("please specify short address (> 0x1) !! \n");
            rc = 8;
        }
        else
            rc = GW_getPOS(argv);

        return rc;
    }

    if (strcmp(argv[2], "dioc") == 0x0)
    {
        if (argc < 7)
        {
            printf("please specify short address (> 0x1), port (1-255), pin (0-7) and value (0 or 1) !! \n");
            rc = 8;
        }
        else
            rc = GW_digitalIOCtrl(argv);

        return rc;
    }

    if (strcmp(argv[2], "mon_node") == 0x0)
    {
        if (argc >= 4)
        {
            int rc, shortAddr;

            rc = sscanf(argv[3], "%u", &shortAddr);
            if (rc != 1 || shortAddr < 2 || shortAddr > LPWMN_MAX_UNICAST_SHORT_ADDR)
            {
                printf("Please specify valid short address to monitor (2 to %d) !! \n",
                       LPWMN_MAX_UNICAST_SHORT_ADDR);
                return 10;
            }

                    
            if (argv[4] != NULL && argv[5] != NULL)
            {
                __loadCellMVal = atoi(argv[4]);
                __loadCellCVal = atoi(argv[5]);
            } 

            printf("Serial port <%s> configuration done ... \n", argv[1]);
            printf("Waiting for events / data traffic from the LPWMN node %d... \n", shortAddr);
            GW_handleNwkDataTraffic(shortAddr);
            rc = 0;
        }
        else
        {
            printf("Please specify valid short address to monitor (2 to %d) !! \n",
                   LPWMN_MAX_UNICAST_SHORT_ADDR);
            return 11;
        }

    }


    if (strcmp(argv[2], "mon") == 0x0)
    {
        if (argc >= 4)
        {
            if (strcmp(argv[3], "cuw1") == 0x0)
                __calWt1Flag = 1;
            else
            {
                if (strcmp(argv[3], "cuw2") == 0x0)
                    __calWt2Flag = 1;
                else
                {
                    if (argv[3] != NULL && argv[4] != NULL)
                    {
                        __loadCellMVal = atoi(argv[3]);
                        __loadCellCVal = atoi(argv[4]);
                    } 
                }
            }
        }

        printf("Serial port <%s> configuration done ... \n", argv[1]);
        printf("Waiting for events / data traffic from the LPWMN ... \n");
        GW_handleNwkDataTraffic(0);
        rc = 0;
    }

    printf("Command not supported - try ? or help\n");

    return 7;
}


void getNodeMACInChar(unsigned char *buff_p,int off, unsigned char MAC_ADDRESS1){
	  unsigned char firstMAC	=	(unsigned char)buff_p[off];
	  if(myhubverbose){
	  printf("\n MyHub String converted MAC is %02x \n",firstMAC);
	  }
	  unsigned char MAC_ADDRESS[9]	=	{(unsigned char)buff_p[off],
										(unsigned char)buff_p[off+1],
										(unsigned char)buff_p[off+2],
										(unsigned char)buff_p[off+3],
										(unsigned char)buff_p[off+4],
										(unsigned char)buff_p[off+5],
										(unsigned char)buff_p[off+6],
										(unsigned char)buff_p[off+7],
										'\0'};

	  if(myhubverbose){
		  printf("\n MyHub String converted MAC is %02x  \n",MAC_ADDRESS[0]);
	  }


   	   // Move Pointer by 8 - LPWMN_MAC_EXT_ADDR_LEN
   	   buff_p += LPWMN_MAC_EXT_ADDR_LEN;
   	   int rssi = (signed char)buff_p[off];
   	if(myhubverbose){
   		printf("\n MyHub RSSI %d dBm / \n", (int)rssi);
   	}


   	   //buff_p += LPWMN_MAC_EXT_ADDR_LEN;
   	   off += (LPWMN_MSG_RSSI_LEN + LPWMN_MSG_CORR_LQI_LEN);

   	   unsigned char disMsgType	=	(unsigned char)buff_p[off];
   	if(myhubverbose){
   		printf("\n MyHub DIS message type - 0x%x \n", disMsgType);
   	}


}
