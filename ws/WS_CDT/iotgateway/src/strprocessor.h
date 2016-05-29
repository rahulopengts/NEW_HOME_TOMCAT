//#ifndef STRPROCESSOR_H_
//#define STRPROCESSOR_H_
//

char* userid;
char* passwordValue;
char* brokerUrl;
char* inboundtopic;
char* outboundtopic;
char* serialPort;

int readFile();
char** str_split(char* a_str, const char a_delim);
int mainM();
void initializeObjects();

void processString(char* months);

//#endif
