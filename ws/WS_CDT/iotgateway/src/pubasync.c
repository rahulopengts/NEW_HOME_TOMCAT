#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTClient.h"
#include "strprocessor.h"


//#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "test1"
#define TOPIC       "r"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L

int PUBASYNCLOGGER	=	1;
char* ADDRESS  =   "tcp://localhost:1884";

volatile MQTTClient_deliveryToken deliveredtoken;

MQTTClient client;
MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
MQTTClient_message pubmsg = MQTTClient_message_initializer;
MQTTClient_deliveryToken token;

int rc;

void deliveredpub(void *context, MQTTClient_deliveryToken dt)
{
    if(PUBASYNCLOGGER){
    	printf("Message with token value %d delivery confirmed\n", dt);
    }
    deliveredtoken = dt;
}

int msgarrvdpub(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    int i;
    char* payloadptr;

    if(PUBASYNCLOGGER){
		printf("Message arrived\n");
		printf("on topic: %s\n", topicName);
    }
    payloadptr = message->payload;
    for(i=0; i<message->payloadlen; i++)
    {
        putchar(*payloadptr++);
    }
    putchar('\n');
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void connlostpub(void *context, char *cause)
{
	if(PUBASYNCLOGGER){
		printf("\n .......pubasync...Connectionpub lost...................\n ...............\n");
		printf("     cause: %s\n", cause);
	}

	ADDRESS =	brokerUrl;
	if(PUBASYNCLOGGER){
		printf("\n pubasync Re-initliazeMQTT - Broker URL is %s ",brokerUrl);
		printf("\n pubasync Re-initliazeMQTT - InboundTopic is %s ",inboundtopic);
		printf("\n pubasync Re-initliazeMQTT - OutTopic is %s ",outboundtopic);
	}
    MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    MQTTClient_setCallbacks(client, NULL, connlostpub, msgarrvdpub, deliveredpub);
    rc = MQTTClient_connect(client, &conn_opts);

    while(rc!=MQTTCLIENT_SUCCESS){
    	rc = MQTTClient_connect(client, &conn_opts);
    }

}



//int main(int argc, char* argv[]){
//int main(int argc, char* argv[]){
//
//}

int mainFunction(int argc, char* argv[]){
	initiliazeMQTT(argc,argv);

	while (1) {
			int c = getchar();
			if (c == 'e'){
				break; /* end of file */
			} else if(c=='s'){
				if(PUBASYNCLOGGER){
					printf("\n Sending again \n");
				}
				//publishMessage(userid);

				char *inboundMessage	=	"~I123456789ABCDEFG#";

				int sensorDataLength	=	19;
				char charSensorDataValue[sensorDataLength];


				int indexCount;
				for(indexCount=0;indexCount<=sensorDataLength;indexCount++){
			//		printf("\n Replacing Values buffer at %i with %c ",indexCount,inboundMessage[indexCount]);
					charSensorDataValue[indexCount]	=	inboundMessage[indexCount];
				}
				if(PUBASYNCLOGGER){
					printf("\n data to be published is %s ",charSensorDataValue);
				}
				publishMQTTMessage(userid,charSensorDataValue);
			}
	 }

	disconnectMQTT();
}
void initiliazeMQTT(int argc, char* argv[])
{

	//ADDRESS =	"tcp://localhost:1883";
	ADDRESS =	brokerUrl;
//	if(PUBASYNCLOGGER){
//		printf("\n pubasync initliazeMQTT - Broker URL is %s ",brokerUrl);
//		printf("\n pubasync initliazeMQTT - InboundTopic is %s ",inboundtopic);
//		printf("\n pubasync initliazeMQTT - OutTopic is %s ",outboundtopic);
//	}
    MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    MQTTClient_setCallbacks(client, NULL, connlostpub, msgarrvdpub, deliveredpub);

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
    	if(PUBASYNCLOGGER){
    		printf("Failed to connect, return code %d\n", rc);
    	}
        exit(-1);
    }
}

int publishMessage(char *newTopicName){
    pubmsg.payload = PAYLOAD;
    pubmsg.payloadlen = strlen(PAYLOAD);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    deliveredtoken = 0;
    printf("\n Topic for publish TOPIC %s ",TOPIC);



    char topicName[]	=	"rahulpcc";
    printf("\n newTopicName is as %s",newTopicName);

//    snprintf(topicName,9 + 1,"%s",userid);
//
//    printf("\n topicName is %i",strlen(topicName));
//    char* newTopicName	=	malloc(25);
//    strcpy(newTopicName,userid);
    //strncpy(topicName, userid, sizeof topicName - 1);
    //topicName[9] = '\0';
    MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
    if(PUBASYNCLOGGER){
		printf("Waiting for publication of %s\n"
				"on topic %s for client with ClientID: %s\n",
				PAYLOAD, TOPIC, CLIENTID);
    }
    //while(deliveredtoken != token);
//	while (1) {
//			int c = getchar();
//			if (c == 'e'){
//				break;
//			} else if(c=='s'){
//				printf("\n Sending again \n");
//				MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
//			}
//	 }

    return rc;
}


int publishMQTTMessage(char *newTopicName,char charSensorDataValue1[]){

    pubmsg.payload = charSensorDataValue1;
    pubmsg.payloadlen = strlen(charSensorDataValue1);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    deliveredtoken = 0;
    if(PUBASYNCLOGGER){
    	printf("\n publishMQTTMessage - Topic for publish TOPIC %s ",inboundtopic);
    }

    if(PUBASYNCLOGGER){
		if(&client==NULL){
			printf("\n client is null \n");
		} else {
			printf("\n client is Not NULL \n");
		}
    }
    MQTTClient_publishMessage(client, inboundtopic, &pubmsg, &token);
    if(PUBASYNCLOGGER){
		printf("Waiting for publication of %s\n"
				"on topic %s for client with ClientID: %s\n",
				PAYLOAD, inboundtopic, CLIENTID);
    }

    return rc;
}

void  disconnectMQTT(){
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);

}

