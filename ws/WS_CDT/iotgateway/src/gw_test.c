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
#include "gw_test.h"
#include <node_attr.h>
#include <fu.h>
#include "globalcons.h"


#define RFD_LIST_ONLY  1
#define FFD_LIST_ONLY  2
#define ALL_NODES_LIST  3



//----MyHUB-----------
int myhubverboseMyHub	=	1;

int eventIdMyHub		=	0;


void setEventIdMyHub(int eventIdMyHubValue){
	eventIdMyHub	=	eventIdMyHubValue;
	printf("\n Value being set to %i \n ",eventIdMyHub);
}

int tempCounterTest	=	0;

void GW_handleNwkDataTrafficMyHub(int expShortAddr)
{
    while (1)
    {

    	//temp code for testing only
    	eventIdMyHub	=	1;

    	if(eventIdMyHub==1){


    		printf("\n Going to read the port now \n");
    		char *topic	=	"r";
    		int sensorOp	=	56;
    		int sensorId	=	12;
    		int sensorNodeId	=	12;
    		handleInboundMessage_1(sensorOp,sensorNodeId,'M');

    		tempCounterTest++;
			if(tempCounterTest>=10 && tempCounterTest<13){
				printf("\n tempCounter is ..........................%i ", tempCounterTest);

				handleInboundMessage_1(sensorOp,sensorNodeId,'M');
				printf("\n tempCounter is ................DONE..........%i ", tempCounterTest);
			}
			if(tempCounterTest==63){
				tempCounterTest	=	0;
			}
sleep(1);
//    		printf("\n MyHub Sensor Value SnsrOp <%d>", snsrOp);
//			printf("\n MyHub Sensor Id    snsrId <%d>", snsrId);
//			printf("\n MyHub Sensor Id    snsrId <%x>", MAC_ADDRESS);
    	}
    }
}


void handleInboundMessage_1(int sensorOpVal,int sensorNodeIdVal,char macId){
	printf("\n Going to read the port now \n");
	int sensorDataLength	=	19;
	char charSensorDataValue[19];
	char *topic	=	"r";

	char* inboundMessageCurrent_1	= malloc( strlen(MESSAGE_TEMPLATE)+1 );
	strcpy(inboundMessageCurrent_1,MESSAGE_TEMPLATE);
	createInboundMessage_1(inboundMessageCurrent_1,sensorNodeIdVal,sensorOpVal);
	printf("\n Final Return...1 value is as %s ",inboundMessageCurrent_1);
	int indexCount;
	for(indexCount=0;indexCount<=sensorDataLength;indexCount++){
		charSensorDataValue[indexCount]	=	inboundMessageCurrent_1[indexCount];
	}
	printf("\n data to be published is %s ",charSensorDataValue);
	publishMQTTMessage(topic,charSensorDataValue);
	//Reset Event Flag
	eventIdMyHub	=	0;

}
/*
 ********************************************************************
 *
 *
 *
 *
 ********************************************************************
 */
int mainGWMyHub(int argc, const char* argv[] )
//int main(int argc, const char* argv[] )
{
	GW_handleNwkDataTrafficMyHub(1);
}


