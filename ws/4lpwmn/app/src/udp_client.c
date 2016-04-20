/*
 * main.c - UDP socket sample application
 *
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/*
 * Application Name     -   UDP socket
 * Application Overview -   This is a sample application demonstrating how to
 *                          open and use a standard UDP socket with CC3100.
 * Application Details  -   http://processors.wiki.ti.com/index.php/CC31xx_UDP_Socket_Application
 *                          doc\examples\udp_socket.pdf
 */

#ifdef RADIO_TYPE_WIFI

#include <simplelink.h>
// #include "cli_uart.h"

#define CLI_Write(arg)   \
do { \
} while (0)


#define APPLICATION_VERSION "1.0.0"

/**/
#define LOOP_FOREVER(line_number) \
            {\
                while(1); \
            }

#define ASSERT_ON_ERROR(line_number, error_code) \
            {\
                /* Handling the error-codes is specific to the application */ \
                if (error_code < 0) return error_code; \
                /* else, continue w/ execution */ \
            }

/*
 * Values for below macros shall be modified per the access-point's (AP) properties
 * SimpleLink device will connect to following AP when the application is executed
 */
#define SSID_NAME       "Paprika"      /* AP name to connect to. */
#define SEC_TYPE        SL_SEC_TYPE_WPA   /* Security type of the Access piont */
#define PASSKEY         "sagan314"               /* Password in case of secure AP */

/* IP addressed of server side socket.
 * Should be in long format, E.g: 0xc0a8010a == 192.168.1.10
 */
#define IP_ADDR         0xc0a80876
#define PORT_NUM        45002              /* Port number to be used */

#define BUF_SIZE        100
#define NO_OF_PACKETS   1000

#define SUCCESS         0

/* Status bits - These are used to set/reset the corresponding bits in a 'status_variable' */
typedef enum{
    STATUS_BIT_CONNECTION =  0, /* If this bit is:
                                 *      1 in a 'status_variable', the device is connected to the AP
                                 *      0 in a 'status_variable', the device is not connected to the AP
                                 */

    STATUS_BIT_IP_AQUIRED       /* If this bit is:
                                 *      1 in a 'status_variable', the device has acquired an IP
                                 *      0 in a 'status_variable', the device has not acquired an IP
                                 */

}e_StatusBits;

/* Application specific status/error codes */
typedef enum{
	DEVICE_NOT_IN_STATION_MODE = -0x7D0,        /* Choosing this number to avoid overlap w/ host-driver's error codes */
	BSD_UDP_CLIENT_FAILED = DEVICE_NOT_IN_STATION_MODE - 1,

	STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;

#define SET_STATUS_BIT(status_variable, bit)    status_variable |= (1<<(bit))
#define CLR_STATUS_BIT(status_variable, bit)    status_variable &= ~(1<<(bit))
#define GET_STATUS_BIT(status_variable, bit)    (0 != (status_variable & (1<<(bit))))

#define IS_CONNECTED(status_variable)           GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_CONNECTION)
#define IS_IP_AQUIRED(status_variable)          GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_IP_AQUIRED)

/*
 * GLOBAL VARIABLES -- Start
 */
UINT8 g_Status = 0;

union
{
    UINT8 BsdBuf[BUF_SIZE];
    UINT32 demobuf[BUF_SIZE/4];
} uBuf;
/*
 * GLOBAL VARIABLES -- End
 */

/*
 * STATIC FUNCTION DEFINITIONS -- Start
 */
static INT32 configureSimpleLinkToDefaultState();
static INT32 establishConnectionWithAP();
static INT32 initializeAppVariables();
static INT32 BsdUdpServer(UINT16 Port);
static INT32 BsdUdpClient(UINT16 Port);
static void displayBanner();
/*
 * STATIC FUNCTION DEFINITIONS -- End
 */

/*
 * ASYNCHRONOUS EVENT HANDLERS -- Start
 */
/*!
    \brief This function handles WLAN events

    \param[in]      pWlanEvent is the event passed to the handler

    \return         None

    \note

    \warning
*/
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    switch(pWlanEvent->Event)
    {
        case SL_WLAN_CONNECT_EVENT:
        {
        	SET_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);

            /*
             * Information about the connected AP (like name, MAC etc) will be
             * available in 'sl_protocol_wlanConnectAsyncResponse_t' - Applications
             * can use it if required
             *
             * sl_protocol_wlanConnectAsyncResponse_t *pEventData = NULL;
             * pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
             *
             */
        }
        break;

        case SL_WLAN_DISCONNECT_EVENT:
        {
            sl_protocol_wlanConnectAsyncResponse_t*  pEventData = NULL;

            CLR_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);
			CLR_STATUS_BIT(g_Status, STATUS_BIT_IP_AQUIRED);

            pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

            /* If the user has initiated 'Disconnect' request, 'reason_code' is
             * SL_USER_INITIATED_DISCONNECTION */
            if(SL_USER_INITIATED_DISCONNECTION == pEventData->reason_code)
            {
            	CLI_Write(" Device disconnected from the AP on application's request \n\r");
            }
            else
            {
            	CLI_Write(" Device disconnected from the AP on an ERROR..!! \n\r");
            }
        }
        break;

        default:
        {
        	CLI_Write(" [WLAN EVENT] Unexpected event \n\r");
        }
        break;
    }
}

/*!
    \brief This function handles events for IP address acquisition via DHCP
           indication

    \param[in]      pNetAppEvent is the event passed to the handler

    \return         None

    \note

    \warning
*/
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    switch(pNetAppEvent->Event)
    {
        case SL_NETAPP_IPV4_ACQUIRED:
        {
        	SET_STATUS_BIT(g_Status, STATUS_BIT_IP_AQUIRED);

            /*
			 * Information about the connection (like IP, gateway address etc)
			 * will be available in 'SlIpV4AcquiredAsync_t'
			 * Applications can use it if required
			 *
			 * SlIpV4AcquiredAsync_t *pEventData = NULL;
			 * pEventData = &pNetAppEvent->EventData.ipAcquiredV4;
			 *
			 */
        }
        break;

        default:
        {
        	CLI_Write(" [NETAPP EVENT] Unexpected event \n\r");
        }
        break;
    }
}

/*!
    \brief This function handles callback for the HTTP server events

    \param[in]      pServerEvent - Contains the relevant event information
    \param[in]      pServerResponse - Should be filled by the user with the
                    relevant response information

    \return         None

    \note

    \warning
*/
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
                                  SlHttpServerResponse_t *pHttpResponse)
{
    /* Unused in this application */
	CLI_Write(" [HTTP EVENT] Unexpected event \n\r");
}

/*!
    \brief This function handles general error events indication

    \param[in]      pDevEvent is the event passed to the handler

    \return         None
*/
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
    /*
     * Most of the general errors are not FATAL are are to be handled
     * appropriately by the application
     */
	CLI_Write(" [GENERAL EVENT] \n\r");
}

/*!
    \brief This function handles socket events indication

    \param[in]      pSock is the event passed to the handler

    \return         None
*/
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
	switch( pSock->Event )
	{
		case SL_NETAPP_SOCKET_TX_FAILED:
			/*
			 * TX Failed
			 *
			 * Information about the socket descriptor and status will be
			 * available in 'SlSockEventData_t' - Applications can use it if
			 * required
			 *
			 * SlSockEventData_t *pEventData = NULL;
			 * pEventData = & pSock->EventData;
			 */
			switch( pSock->EventData.status )
			{
				case SL_ECLOSE:
					CLI_Write(" [SOCK EVENT] Close socket operation failed to transmit all queued packets\n\r");
					break;
				default:
					CLI_Write(" [SOCK EVENT] Unexpected event \n\r");
					break;
			}
			break;

		default:
			CLI_Write(" [SOCK EVENT] Unexpected event \n\r");
			break;
	}
}
/*
 * ASYNCHRONOUS EVENT HANDLERS -- End
 */

unsigned char macAddressVal[SL_MAC_ADDR_LEN];
unsigned char dhcpIsOn = 0;
_NetCfgIpV4Args_t ipV4 = {0};

/*
 * Application's entry point
 */
int __udpMain(void)
{
	INT32 retVal = -1;

	retVal = initializeAppVariables();
	ASSERT_ON_ERROR(__LINE__, retVal);

    /* Stop WDT and initialize the system-clock of the MCU
       These functions needs to be implemented in PAL */
    // stopWDT();
    // initClk();


    /* Configure command line interface */
    // CLI_Configure();

    // displayBanner();

    /*
     * Following function configures the device to default state by cleaning
     * the persistent settings stored in NVMEM (viz. connection profiles &
     * policies, power policy etc)
     *
     * Applications may choose to skip this step if the developer is sure
     * that the device is in its default state at start of application
     *
     * Note that all profiles and persistent settings that were done on the
     * device will be lost
     */
    retVal = configureSimpleLinkToDefaultState();
	if(retVal < 0)
	{
		if (DEVICE_NOT_IN_STATION_MODE == retVal)
		{
			CLI_Write(" Failed to configure the device in its default state \n\r");
		}

		LOOP_FOREVER(__LINE__);
	}

	CLI_Write(" Device is configured in default state \n\r");

    /*
     * Assumption is that the device is configured in station mode already
     * and it is in its default state
     */
    /* Initializing the CC3100 device */
    retVal = sl_Start(0, 0, 0);
    if ((retVal < 0) ||
        (ROLE_STA != retVal) )
    {
        CLI_Write(" Failed to start the device \n\r");
        LOOP_FOREVER(__LINE__);
    }

    CLI_Write(" Device started as STATION \n\r");

    /* Connecting to WLAN AP - Set with static parameters defined at the top
       After this call we will be connected and have IP address */
    retVal = establishConnectionWithAP();
    if(retVal < 0)
	{
    	CLI_Write(" Failed to establish connection w/ an AP \n\r");
		LOOP_FOREVER(__LINE__);
	}

    {

        unsigned char macAddressLen = SL_MAC_ADDR_LEN;
        sl_NetCfgGet(SL_MAC_ADDRESS_GET,NULL,&macAddressLen,(unsigned char *)macAddressVal);
    }

    {
        unsigned char len = sizeof(_NetCfgIpV4Args_t);

        sl_NetCfgGet(SL_IPV4_STA_P2P_CL_GET_INFO, &dhcpIsOn, &len, (unsigned char *)&ipV4);
    }

    CLI_Write(" Connection established w/ AP and IP is acquired \n\r");

    CLI_Write(" Started sending data to UDP server \n\r");

    retVal = BsdUdpClient(PORT_NUM);
    if(retVal < 0)
    	CLI_Write(" Failed to send data to UDP sevrer\n\r");

    CLI_Write(" successfully sent data to UDP server \n\r");

    CLI_Write(" Waiting for data from UDP client \n\r");

    retVal = BsdUdpServer(PORT_NUM);
    if(retVal < 0)
    	CLI_Write(" Failed to read data from the UDP client \n\r");

    CLI_Write(" Successfully received data from UDP client \n\r");

    /* Stop the CC3100 device */
    sl_Stop(0xFF);

    return 0;
}

/*!
    \brief This function configure the SimpleLink device in its default state. It:
           - Sets the mode to STATION
           - Configures connection policy to Auto and AutoSmartConfig
           - Deletes all the stored profiles
           - Enables DHCP
           - Disables Scan policy
           - Sets Tx power to maximum
           - Sets power policy to normal
           - Unregisters mDNS services

    \param[in]      none

    \return         On success, zero is returned. On error, negative is returned
*/
static INT32 configureSimpleLinkToDefaultState()
{
    SlVersionFull   ver = {0};

    UINT8           val = 1;
    UINT8           configOpt = 0;
    UINT8           configLen = 0;
    UINT8           power = 0;

    INT32           retVal = -1;
    INT32           mode = -1;

    mode = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(__LINE__, mode);

    /* If the device is not in station-mode, try configuring it in station-mode */
    if (ROLE_STA != mode)
    {
        if (ROLE_AP == mode)
        {
            /* If the device is in AP mode, we need to wait for this event before doing anything */
            while(!IS_IP_AQUIRED(g_Status)) { _SlNonOsMainLoopTask(); }
        }

        /* Switch to STA role and restart */
        retVal = sl_WlanSetMode(ROLE_STA);
        ASSERT_ON_ERROR(__LINE__, retVal);

        retVal = sl_Stop(0xFF);
        ASSERT_ON_ERROR(__LINE__, retVal);

        retVal = sl_Start(0, 0, 0);
        ASSERT_ON_ERROR(__LINE__, retVal);

        /* Check if the device is in station again */
        if (ROLE_STA != retVal)
        {
            /* We don't want to proceed if the device is not coming up in station-mode */
            return DEVICE_NOT_IN_STATION_MODE;
        }
    }

    /* Get the device's version-information */
    configOpt = SL_DEVICE_GENERAL_VERSION;
    configLen = sizeof(ver);
    retVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &configOpt, &configLen, (unsigned char *)(&ver));
    ASSERT_ON_ERROR(__LINE__, retVal);

    /* Set connection policy to Auto + SmartConfig (Device's default connection policy) */
    retVal = sl_WlanPolicySet(SL_POLICY_CONNECTION, SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);
    ASSERT_ON_ERROR(__LINE__, retVal);

    /* Remove all profiles */
    retVal = sl_WlanProfileDel(0xFF);
    ASSERT_ON_ERROR(__LINE__, retVal);

    /*
     * Device in station-mode. Disconnect previous connection if any
     * The function returns 0 if 'Disconnected done', negative number if already disconnected
     * Wait for 'disconnection' event if 0 is returned, Ignore other return-codes
     */
    retVal = sl_WlanDisconnect();
    if(0 == retVal)
    {
        /* Wait */
        while(IS_CONNECTED(g_Status)) { _SlNonOsMainLoopTask(); }
    }

    /* Enable DHCP client*/
    retVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&val);
    ASSERT_ON_ERROR(__LINE__, retVal);

    /* Disable scan */
    configOpt = SL_SCAN_POLICY(0);
    retVal = sl_WlanPolicySet(SL_POLICY_SCAN , configOpt, NULL, 0);
    ASSERT_ON_ERROR(__LINE__, retVal);

    /* Set Tx power level for station mode
       Number between 0-15, as dB offset from max power - 0 will set maximum power */
    power = 0;
    retVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *)&power);
    ASSERT_ON_ERROR(__LINE__, retVal);

    /* Set PM policy to normal */
    retVal = sl_WlanPolicySet(SL_POLICY_PM , SL_NORMAL_POLICY, NULL, 0);
    ASSERT_ON_ERROR(__LINE__, retVal);

    /* Unregister mDNS services */
    retVal = sl_NetAppMDNSUnRegisterService(0, 0);
    ASSERT_ON_ERROR(__LINE__, retVal);

    retVal = sl_Stop(0xFF);
    ASSERT_ON_ERROR(__LINE__, retVal);

    retVal = initializeAppVariables();
    ASSERT_ON_ERROR(__LINE__, retVal);

    return retVal; /* Success */
}

/*!
    \brief Connecting to a WLAN Access point

    This function connects to the required AP (SSID_NAME).
    The function will return once we are connected and have acquired IP address

    \param[in]  None

    \return     0 on success, negative error-code on error

    \note

    \warning    If the WLAN connection fails or we don't acquire an IP address,
                We will be stuck in this function forever.
*/
static INT32 establishConnectionWithAP()
{
    SlSecParams_t secParams = {0};
    INT32 retVal = 0;

    secParams.Key = PASSKEY;
    secParams.KeyLen = strlen(PASSKEY);
    secParams.Type = SEC_TYPE;

    retVal = sl_WlanConnect(SSID_NAME, strlen(SSID_NAME), 0, &secParams, 0);
    ASSERT_ON_ERROR(__LINE__, retVal);

    /* Wait */
    while ((!IS_CONNECTED(g_Status)) || (!IS_IP_AQUIRED(g_Status)))
    {
    	_SlNonOsMainLoopTask();
    }

    return SUCCESS;
}

/*!
    \brief Opening a UDP client side socket and sending data

    This function opens a UDP socket and tries to send data to a UDP server
    IP_ADDR waiting on port PORT_NUM.
    Then the function will send 1000 UDP packets to the server.

    \param[in]      port number on which the server will be listening on

    \return         0 on success, -1 on Error.

    \note

    \warning
*/
static INT32 BsdUdpClient(UINT16 Port)
{
    SlSockAddrIn_t  Addr;
    UINT16          idx = 0;
    UINT16          AddrSize = 0;
    INT16           SockID = 0;
    INT16           Status = 0;
    UINT16          LoopCount = 0;
    int  _x = 0;

#if 0
    for (idx=0 ; idx<BUF_SIZE ; idx++)
        uBuf.BsdBuf[idx] = (char)('a' + (idx % 26));
#endif

    Addr.sin_family = SL_AF_INET;
    Addr.sin_port = sl_Htons((UINT16)Port);
    Addr.sin_addr.s_addr = sl_Htonl((UINT32)IP_ADDR);

    AddrSize = sizeof(SlSockAddrIn_t);

    SockID = sl_Socket(SL_AF_INET,SL_SOCK_DGRAM, 0);
    if( SockID < 0 )
    {
    	ASSERT_ON_ERROR(__LINE__, SockID);
    }

    while (LoopCount < NO_OF_PACKETS)
    {
        for (idx=0 ; idx<BUF_SIZE ; idx++)
        {
           uBuf.BsdBuf[idx] = 'a' + _x;
        }

        _x = (_x + 1) % 26;

        Status = sl_SendTo(SockID, uBuf.BsdBuf, BUF_SIZE, 0,
                           (SlSockAddr_t *)&Addr, AddrSize);
        if( Status <= 0 )
        {
            sl_Close(SockID);
            return BSD_UDP_CLIENT_FAILED;
        }

        LoopCount++;
    }

    sl_Close(SockID);
    return SUCCESS;
}

/*!
    \brief Opening a UDP server side socket and receiving data

    This function opens a UDP socket in Listen mode and waits for incoming
    UDP packets from the connected client.

    \param[in]      port number on which the server will be listening on

    \return         0 on success, Negative value on Error.

    \note

    \warning
*/
static INT32 BsdUdpServer(UINT16 Port)
{
    SlSockAddrIn_t  Addr;
    SlSockAddrIn_t  LocalAddr;
    UINT16          idx = 0;
    UINT16          AddrSize = 0;
    INT16           SockID = 0;
    INT16           Status = 0;
    UINT16          LoopCount = 0;

    for (idx=0 ; idx<BUF_SIZE ; idx++)
    {
        uBuf.BsdBuf[idx] = (char)(idx % 10);
    }

    LocalAddr.sin_family = SL_AF_INET;
    LocalAddr.sin_port = sl_Htons((UINT16)Port);
    LocalAddr.sin_addr.s_addr = 0;

    AddrSize = sizeof(SlSockAddrIn_t);

    SockID = sl_Socket(SL_AF_INET,SL_SOCK_DGRAM, 0);
    if( SockID < 0 )
    {
    	ASSERT_ON_ERROR(__LINE__, SockID);
    }

    Status = sl_Bind(SockID, (SlSockAddr_t *)&LocalAddr, AddrSize);
    if( Status < 0 )
    {
        sl_Close(SockID);
        ASSERT_ON_ERROR(__LINE__, Status);
    }

    while (LoopCount < NO_OF_PACKETS)
    {
        Status = sl_RecvFrom(SockID, uBuf.BsdBuf, BUF_SIZE, 0,
                            (SlSockAddr_t *)&Addr, (SlSocklen_t*)&AddrSize );
        LoopCount++;
    }

    sl_Close(SockID);
    return SUCCESS;
}

/*!
    \brief This function initializes the application variables

    \param[in]  None

    \return     0 on success, negative error-code on error
*/
static INT32 initializeAppVariables()
{
    g_Status = 0;
    memset(uBuf.BsdBuf, 0, sizeof(uBuf));

    return SUCCESS;
}

/*!
    \brief This function displays the application's banner

    \param      None

    \return     None
*/
static void displayBanner()
{
    CLI_Write("\n\r\n\r");
    CLI_Write(" UDP socket application - Version ");
    CLI_Write(APPLICATION_VERSION);
    CLI_Write("\n\r*******************************************************************************\n\r");
}

#endif
