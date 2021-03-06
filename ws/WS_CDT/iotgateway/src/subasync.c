#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTClient.h"
#include "strprocessor.h"
#include "gw.h"
#include "gw_test.h"


#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "test"
#define TOPIC       "g"
//#define TOPIC_PUBLISH       "r"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L

//**********************************************************************

// 	subasync.c
// 	gw.c
//	stroricessor.hs
//	strprocessor.c
//************************Date-18-May-2016
//**********Modified subasync.c and pubasycn.c*************************
//****************
//**********************************************************************

int SUBASYNCLOGGER	=	1;

//char* TOPIC_PUBLISH	=	'r';

//MQTTClient client;
//MQTTClient_connectOptions conn_opts_sub = MQTTClient_connectOptions_initializer;

volatile MQTTClient_deliveryToken deliveredtoken;

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    int i;
    char* payloadptr;

    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: ");

    payloadptr = message->payload;
    for(i=0; i<message->payloadlen; i++)
    {
        putchar(*payloadptr++);
    }
    putchar('\n');
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    //writePortTest(NULL,NULL);
    char *sendData	=	"nc";
    GW_sendAndReceiveRequest();
    return 1;
}

void connlost(void *context, char *cause)
{
    if(SUBASYNCLOGGER){
		printf("\nConnection lost\n");
		printf("     cause: %s\n", cause);
    }
//    MQTTAsync client = (MQTTAsync)context;
//           MQTTAsync_connectOptions conn_opts_sub = MQTTAsync_connectOptions_initializer;
//           int rc;
//
//           printf("\nConnection lost\n");
//           printf("     cause: %s\n", cause);
//
//           printf("Reconnecting\n");
//           conn_opts_sub.keepAliveInterval = 20;
//           conn_opts_sub.cleansession = 1;
//           if ((rc = MQTTAsync_connect(client, &conn_opts_sub)) != MQTTASYNC_SUCCESS)
//           {
//                   printf("Failed to start connect, return code %d\n", rc);
//               finished = 1;
//           }
}

int main(int argc, char* argv[])
//int mainSubasync(int argc, char* argv[])
{

	initializeObjects();
	readFile();
	initiliazeMQTT(argc,argv);

    MQTTClient client;
    MQTTClient_connectOptions conn_opts_sub = MQTTClient_connectOptions_initializer;
    int rc;
    int ch;
    //ADDRESS =	brokerUrl;
    MQTTClient_create(&client, brokerUrl, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts_sub.keepAliveInterval = 20;
    conn_opts_sub.cleansession = 1;

    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

    if ((rc = MQTTClient_connect(client, &conn_opts_sub)) != MQTTCLIENT_SUCCESS)
    {
        if(SUBASYNCLOGGER){
        	printf("Failed to connect, return code %d\n", rc);
        }
        exit(-1);
    }
//    if(SUBASYNCLOGGER){
//		printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
//			   "Press Q<Enter> to quit\n\n", TOPIC, CLIENTID, QOS);
//    }
    MQTTClient_subscribe(client, TOPIC, QOS);

//    if(SUBASYNCLOGGER){
//    	printf("In Main pthread_create getpid: %d getpthread_self: %lu \n",getpid(), pthread_self());
//    }
    argv[1]	=	serialPort;
    argv[2]	=	"mon";
    mainGW(argc,argv);
    //mainGWMyHub(argc,argv);
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}

void writeToPort(){
	printf("\n Writing Message To Serial Port \n");
}

//*****************************MyHub********************************//
int GW_sendAndReceiveRequest(void)
{
   int rc;

   //rc = GW_buildSendHdr(LPWMN_GW_MSG_TYPE_NODE_CNT_REQ, NULL, 0x0);
   setEventIdMyHub(1);
   if(SUBASYNCLOGGER){
	   printf("before calling pthread_create getpid: %d getpthread_self: %lu \n",getpid(), pthread_self());
   }
//   if (rc != 1)
//   {
//       printf("failed !! \n");
//       return 5;
//   }
//
//   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_NODE_CNT_REQ, 0);

   return rc;
}

