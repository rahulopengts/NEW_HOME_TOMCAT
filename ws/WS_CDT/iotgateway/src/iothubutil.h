


void createInboundMessage(char* inboundMessageCurrent,int nodeId,int sensorValue);


/*
	char* inboundMessageCurrent	= malloc( strlen(inbound_MessageTemplate)+1 );
	testStringFormatter(inboundMessageCurrent);
	printf("\n Return value is %s ",inboundMessageCurrent);
*/
void convertToInboundMessage(char* inboundMessage,int nodeId,int S_INDEX);
