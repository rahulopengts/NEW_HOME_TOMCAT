#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <strprocessor.h>


int START_INDEX_NODE_ID	=	5;
int START_INDEX_SENSOR_VALUE	=	7;



char inbound_MessageTemplate[19]	=	{'~','I','N','A','B','C','D','E','F','G','H','I','J','K','L','M','N','#','\0'};




void createInboundMessage(char* inboundMessageCurrent,int nodeId,int sensorValue){
	convertToInboundMessage(inboundMessageCurrent,nodeId,START_INDEX_NODE_ID);
	convertToInboundMessage(inboundMessageCurrent,sensorValue,START_INDEX_SENSOR_VALUE);
}


/*
	char* inboundMessageCurrent	= malloc( strlen(inbound_MessageTemplate)+1 );
	testStringFormatter(inboundMessageCurrent);
	printf("\n Return value is %s ",inboundMessageCurrent);
*/
void convertToInboundMessage(char* inboundMessage,int nodeId,int S_INDEX){
	char charSensorDataValue[11];
	sprintf(charSensorDataValue,"%ld", nodeId);
	int sensorDataLength	=	strlen(charSensorDataValue);

	inboundMessage	=	inboundMessage+S_INDEX;

	int indexCount;
	for(indexCount=0;indexCount<sensorDataLength;indexCount++){
		inboundMessage[indexCount]	=	charSensorDataValue[indexCount];
	}

	inboundMessage	=	inboundMessage-S_INDEX;
}


