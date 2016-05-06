#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <strprocessor.h>




int START_INDEX_NODE_ID1	=	5;
int START_INDEX_SENSOR_VALUE1	=	7;
int START_INDEX_SENSOR_ID1	=	5;
char inboundMessageTemplate[19]	=	{'~','I','N','A','B','C','D','E','F','G','H','I','J','K','L','M','N','#','\0'};





int mainMyTest() {
//int main(int argc, char* argv[]) {
//	testPointer();
//	testArray();
//	testArrayPointers();

	//testConstantchar();
	//readFile();

/*
	//Test File Read for Property
	initializeObjects();
	readFile();
	printf("\n User ID %s \n",userid);
	printf("\n Pwd  ID %s \n",passwordValue);
	printf("\n brokerUrl %s \n",brokerUrl);


*/

//-----------Test function testStringFormatter-------------------------------------------
//	char* inboundMessageCurrent	= malloc( strlen(inboundMessageTemplate)+1 );
//	strcpy(inboundMessageCurrent,inboundMessageTemplate);
//	testStringFormatter(inboundMessageCurrent,12,START_INDEX_NODE_ID1);
//	printf("\n Return value is %s ",inboundMessageCurrent);
//
//	//char* inboundMessageCurrent_1	=	malloc( strlen(inboundMessageCurrent)+1 );
//	testStringFormatter(inboundMessageCurrent,56,START_INDEX_SENSOR_VALUE1);
//	printf("\n Return value is %s ",inboundMessageCurrent);
//---------------------------------------------------------------------------------------

//--------------Test function createInboundMessage---------------------------------------
//	char* inboundMessageCurrent_1	= malloc( strlen(inboundMessageTemplate)+1 );
//	strcpy(inboundMessageCurrent_1,inboundMessageTemplate);
//	createInboundMessage_1(inboundMessageCurrent_1,12,56);
//	printf("\n Final Return...1 value is as %s ",inboundMessageCurrent_1);
//	return 0;
//---------------------------------------------------------------------------------------



}


void createInboundMessage_1(char* inboundMessageCurrent,int nodeId,int sensorValue){
//	char* inboundMessageCurrent	= malloc( strlen(inboundMessageTemplate)+1 );
//	strcpy(inboundMessageCurrent,inboundMessageTemplate);
	testStringFormatter(inboundMessageCurrent,nodeId,START_INDEX_NODE_ID1);
	printf("\n Return value is %s ",inboundMessageCurrent);

	//char* inboundMessageCurrent_1	=	malloc( strlen(inboundMessageCurrent)+1 );
	testStringFormatter(inboundMessageCurrent,sensorValue,START_INDEX_SENSOR_VALUE1);
	printf("\n Return...1 value is %s ",inboundMessageCurrent);

}


void testStringFormatter(char* inboundMessage,int nodeId,int S_INDEX){
	//char* inboundMessage	= malloc( strlen(inboundMessageTemplate)+1 );
	//strcpy(inboundMessage,inboundMessageTemplate);
	char charSensorDataValue[11];
	//int intSensorDataValue	=	1234;



//	printf("\n MyHub Memory Location for inboundMessageTemplate %i ",inboundMessageTemplate);
//	printf("\n MyHub Memory Location for inboundMessage %i ",inboundMessage);
//
	sprintf(charSensorDataValue,"%ld", nodeId);
	int sensorDataLength	=	strlen(charSensorDataValue);

//	printf("\n sensorData character is %i \n",sensorDataLength);
//	printf("\n Value in character is %d \n",inboundMessage);

	//increment pointer to next START_INDEX address and start to replace.

	inboundMessage	=	inboundMessage+S_INDEX;

//	printf("\n Value in character after increasing to START_INDEX is %d \n",inboundMessage);
	int indexCount;
	for(indexCount=0;indexCount<sensorDataLength;indexCount++){
//		printf("\n Replacing Values buffer at %i with %c ",indexCount,inboundMessage[indexCount]);
		inboundMessage[indexCount]	=	charSensorDataValue[indexCount];
	}

//	printf("\n Value in character is %s \n",inboundMessage);
	inboundMessage	=	inboundMessage-S_INDEX;
//	printf("\n Value in character is %s \n",inboundMessage);
//	printf("\n Value in character inboundMessagTemplate is %s \n",inboundMessageTemplate);

}

void testPointer(){

	   int  var = 20;   /* actual variable declaration */
	   int  *ip;        /* pointer variable declaration */

	   ip = &var;  /* store address of var in pointer variable*/

	   printf("Address of var variable: %x\n", &var  );

	   /* address stored in pointer variable */
	   printf("Address stored in ip variable: %x\n", ip );

	   /* access the value using the pointer */
	   printf("Value of *ip variable: %d\n", *ip );

}

void testArray(){
	char greeting[6] = {'H', 'e', 'l', 'l', 'o', '\0'};
	   printf("Greeting message: %s\n", greeting );
	   printf("Memory Location Greeting message: %x \n", &greeting[0] );
	   printf("Memory Location Greeting message: %x \n", &greeting[1] );
}


void testArrayPointers(){
	char greeting[6] = {'H', 'e', 'l', 'l', 'o', '\0'};
	   printf("Greeting message: %s\n", greeting );
	   printf("Memory Location Greeting message: %x \n", &greeting[0] );
	   printf("Memory Location Greeting message: %x \n", &greeting[1] );
}


void testConstantchar(){
	const char* host;
	//*host	= 't';
	printf("\n character is %c \n");
}


int readFileTemp(char *userName,char *password){


	FILE *ptr_file;
	char buf[1000];

	ptr_file =fopen("input.txt","r");
	if (!ptr_file)
		return 1;

	while (fgets(buf,1000, ptr_file)!=NULL){
		int size	=	sizeof(buf);
		printf("\n Lenght %i ",size);
		printf("%s",buf);
		//processLine(buf);
	}

	fclose(ptr_file);
	return 0;
	}


