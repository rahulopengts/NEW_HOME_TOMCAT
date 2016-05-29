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

//char* TOPIC_PUBLISH	=	'r';

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
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int main(int argc, char* argv[])
//int mainSubasync(int argc, char* argv[])
{

	initializeObjects();
	readFile();
	printf("\n Broker URL is := %s \n",brokerUrl);

	initiliazeMQTT(argc,argv);

    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;
    int ch;

    MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }
    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n", TOPIC, CLIENTID, QOS);
    MQTTClient_subscribe(client, TOPIC, QOS);


    printf("In Main pthread_create getpid: %d getpthread_self: %lu \n",getpid(), pthread_self());
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
   //   eventId	=	1;
//   printf("\n Global Event Id Set to %i \n",eventId);
   printf("before calling pthread_create getpid: %d getpthread_self: %lu \n",getpid(), pthread_self());

//   if (rc != 1)
//   {
//       printf("failed !! \n");
//       return 5;
//   }
//
//   rc = GW_readSerIntf(LPWMN_GW_MSG_TYPE_NODE_CNT_REQ, 0);

   return rc;
}

