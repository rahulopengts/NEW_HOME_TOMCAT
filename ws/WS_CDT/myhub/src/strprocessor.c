#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "strprocessor.h"

char* userid;
char* passwordValue;
char* brokerUrl;

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

int mainSTR()
{
    //char months[] = "JAN,FEB,MAR,APR,MAY,JUN,JUL,AUG,SEP,OCT,NOV,DEC";
	char months[] = "username=FEB";

	//processString(&months);
	initializeObjects();
	readFile();
	printf("\n User ID %s \n",userid);
	printf("\n Pwd  ID %s \n",passwordValue);
	printf("\n brokerUrl %s \n",brokerUrl);
	//userName
    return 0;
}
void initializeObjects(){
	userid	=	 malloc( 20 );
	passwordValue	=	malloc( 20 );
	brokerUrl	=	malloc(50);
}


//int readFile(char *userName,char *password){
	int readFile(){

	FILE *ptr_file;
	char buf[1000];

	ptr_file =fopen("input.txt","r");
	if (!ptr_file)
		return 1;

	while (fgets(buf,1000, ptr_file)!=NULL){
		int size	=	sizeof(buf);
		printf("\n Lenght %i ",size);
		printf("%s",buf);
		processString(buf);
	}

	fclose(ptr_file);
	return 0;
	}



void processString(char* months){
	char** tokens;
	tokens = str_split(months, '=');
	char* keyName		= malloc( strlen("valuesforkey")+1 );


	char userName[]	=	"username";
    if (tokens)
    {
        int i;
        for (i = 0; *(tokens + i); i++)
        {
            if(i==0){
            	strcpy(keyName,*(tokens + i));

            } else if(i==1) {
            	if(strcmp(keyName,userName)==0){
//					printf("\n KeyName 	=%s \n ",keyName);
//					printf("\n VALUE	=%s \n ",*(tokens + i));
					//userid	=	*(tokens + i);
					strcpy(userid,*(tokens + i));
            	} else if(strcmp(keyName,"password")==0){
//					printf("\n KeyName 	=%s \n ",keyName);
//					printf("   VALUE	=%s \n ",*(tokens + i));

					strcpy(passwordValue,*(tokens + i));
            	} else if(strcmp(keyName,"brokerurl")==0){
					strcpy(brokerUrl,*(tokens + i));
            	}
            }

            free(*(tokens + i));
        }
        printf("\n");
        free(tokens);
        free(keyName);
    }

}
