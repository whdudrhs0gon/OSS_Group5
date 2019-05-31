#define _CRT_SECURE_NO_WARNINGS    

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

typedef enum {
	UNDEFIEND = 0,
	OBJECT = 1,
	ARRAY = 2,
	STRING = 3,
	PRIMITIVE = 4
}type_t;

static const char* type[] = { "UNDEFIEND", "OBJECT", "ARRAY", "STRING", "PRIMITIVE" };

typedef struct {
	type_t type;
	int start;
	int end;
	int size;
	char* string;
} TOKEN;


char* FREAD(char *file, long *file_size) {
	FILE* fp;
	fp = fopen(file, "r");
	long size;
	char* buffer;

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	*file_size = size;
	fseek(fp, 0, SEEK_SET);

	buffer = (char*)malloc(size + 1);
	memset(buffer, 0, size + 1);
	fread(buffer, 1, size + 1, fp);

	fclose(fp);

	return buffer;
}


void JSON_parse(char* buffer, long file_size, int* token_size, TOKEN *tokens) {
	int tokenIndex = 0;

	for (int i = 0; i < file_size; i++) {
		switch (buffer[i]) {
			case '{':
			{
				if (i == 0) continue;

				char* begin = buffer + i;
				tokens[tokenIndex].start = begin - buffer;

				//
				int test = 0;
				char* end;
				
				for (int j = i+1; j < file_size; j++) {
					if (buffer[j] == '{') test++;
					if (buffer[j] == '}') test--;
						
					if (test <= -1) {
						end = &buffer[j];
						test = 0;
						break;
					}
				}						
					
				
				tokens[tokenIndex].end = end - buffer + 1;
				int stringLength = end - begin + 1;

				tokens[tokenIndex].type = OBJECT;
				tokens[tokenIndex].string = (char*)malloc(stringLength + 1);
				memset(tokens[tokenIndex].string, 0, stringLength + 1);

				memcpy(tokens[tokenIndex].string, begin, stringLength);
				tokenIndex++;
				*token_size = tokenIndex;
			}
			break;

			case '[':
			{
				char* begin = buffer + i;
				tokens[tokenIndex].start = begin - buffer;
				char* end = strchr(begin, ']');
				tokens[tokenIndex].end = end - buffer + 1;
				int stringLength = end - begin + 1;

				tokens[tokenIndex].type = ARRAY;
				tokens[tokenIndex].string = (char*)malloc(stringLength + 1);
				memset(tokens[tokenIndex].string, 0, stringLength + 1);

				memcpy(tokens[tokenIndex].string, begin, stringLength);
				tokenIndex++;
				*token_size = tokenIndex;
			}
			break;

			case '"':
			{
				char* begin = buffer + i + 1;
				tokens[tokenIndex].start = begin - buffer;
				char* end = strchr(begin, '"');
				tokens[tokenIndex].end = end - buffer;
				int stringLength = end - begin;

				tokens[tokenIndex].type = STRING;
				tokens[tokenIndex].string = (char*)malloc(stringLength + 1);
				memset(tokens[tokenIndex].string, 0, stringLength + 1);
				memcpy(tokens[tokenIndex].string, begin, stringLength);

				tokenIndex++;
				*token_size = tokenIndex;

				i = i + stringLength + 1;
			}
			break;

			case 't':
			{
				char* begin = buffer + i;
				tokens[tokenIndex].start = begin - buffer;
				char* end = strchr(begin, 'e');
				tokens[tokenIndex].end = end - buffer + 1;
				int stringLength = end - begin + 1;

				tokens[tokenIndex].type = PRIMITIVE;
				tokens[tokenIndex].string = (char*)malloc(stringLength + 1);
				memset(tokens[tokenIndex].string, 0, stringLength + 1);
				memcpy(tokens[tokenIndex].string, begin, stringLength);
				tokenIndex++;
				*token_size = tokenIndex;
				i = i + stringLength + 1;
			}
			break;

			case 'f':
			{
				char* begin = buffer + i;
				tokens[tokenIndex].start = begin - buffer;
				char* end = strchr(begin, 'e');
				tokens[tokenIndex].end = end - buffer + 1;
				int stringLength = end - begin + 1;

				tokens[tokenIndex].type = PRIMITIVE;
				tokens[tokenIndex].string = (char*)malloc(stringLength + 1);
				memset(tokens[tokenIndex].string, 0, stringLength + 1);
				memcpy(tokens[tokenIndex].string, begin, stringLength);
				tokenIndex++;
				*token_size = tokenIndex;
				i = i + stringLength + 1;
			}
			break;

			case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case '-':
			{
				char* begin = buffer + i;
				tokens[tokenIndex].start = begin - buffer;
				char* end = strchr(begin, ',');
				tokens[tokenIndex].end = end - buffer;
				int stringLength = end - begin;

				tokens[tokenIndex].type = PRIMITIVE;
				tokens[tokenIndex].string = (char*)malloc(stringLength);
				memset(tokens[tokenIndex].string, 0, stringLength + 1);
				memcpy(tokens[tokenIndex].string, begin, stringLength);
				tokenIndex++;
				*token_size = tokenIndex;
				i = i + stringLength + 1;
			}
			break;
		}
	}

}

void free_tokens(TOKEN *tokens, int token_size) {
	for (int i = 0; i < token_size; i++) {
		if (tokens[i].type == STRING) {
			free(tokens[i].string);
		}
	}
}

void lower_string(char s[])
{
	int c = 0;
    while(s[c] != '\0')
    {
        if (s[c] >= 65 && s[c] <= 90)
        {
            s[c] = s[c] + 32;
        }
        c++;
    }
}

void Find_TokenSize(char* buffer, int tokens_size, TOKEN *tokens) {
	int size = 0; // token[i].size
	int key = TRUE;

	for (int i = 0; i < tokens_size; i++) {
		switch (tokens[i].string[0]) {
		case '{':
		{
			int end = tokens[i].end;

			while (1) {
				int start = tokens[++i].start;

				if (start > end || i >= tokens_size) break;
				size++;			
			}
			i = i - size - 1;			
			tokens[i].size = (size) / 2;
			size = 0;
		}
		break;

		case '[':
		{
			int end = tokens[i].end;
			int temp_size = 0;

			while (1) {
				int start = tokens[++i].start;

				if (i >= tokens_size) {
					break;
				}

				if (tokens[i].string[0] == '{') {
					int temp_end = tokens[i].end;

					while (1) {
						int temp_start = tokens[++i].start;

						if (temp_start > temp_end || i >= tokens_size) break;
						temp_size++;
					}
					i = i - 1;
				}

				if (start > end || i >= tokens_size) {
					break;
				}
				size++;
			}
			i = i - temp_size - size - 1;
			tokens[i].size = size;
			size = 0;
		}
		break;

		default:
		{
			if (buffer[tokens[i].end + 1] == ':') {
				key = TRUE;
			}
			else {
				key = FALSE;
			}

			if (key == TRUE) {
				tokens[i].size = 1;
			}
			else {
				tokens[i].size = 0;
			}
		}
		break;
		}
	}

	int object_size;
	for (int i = 0; i < tokens_size; i++) {
		if (tokens[i].string[0] == '{') {
			for (int j = i+1; j < tokens_size; j++) {
				if (tokens[j].type == OBJECT && tokens[i].end >= tokens[j].start) {
					tokens[i].size = tokens[i].size - tokens[j].size;					
					break;
				}						
			}

			object_size = 0;
			for (int j = i+1; j < tokens_size; j++) {
				if (tokens[j].string[0] == '{' && tokens[i].end >= tokens[j].start) break;

				if (tokens[j].type == OBJECT && tokens[i].end >= tokens[j].start) {
					object_size += tokens[j].size;				
				}						
			}
			tokens[i].size = (tokens[i].size*2 - object_size)/2;
			object_size = 0;							
		}
	}
}

void OptionPrint(int tokens_size, TOKEN *tokens){
	for (int i = 0; i < tokens_size; i++) {
		printf("[%d] %s (size=%d, %d~%d, %s)\n",i+1, tokens[i].string, tokens[i].size, tokens[i].start, tokens[i].end, type[tokens[i].type]);
	}
}

void OptionSearch(int tokens_size, TOKEN *tokens){
	char option;
	char search[1024];
	printf("\n\nChoose what information to search");
	printf("\n\t -f : first name");
	printf("\n\t -l : last name");
	printf("\n\t -i : student id");
	printf("\n\t -a : age");
	printf("\n\t -m : major");
	printf("\n\t -r : RC");
	printf("\n\t\t-->");
	scanf(" %c", &option);

	//printf("\n\nEnter information to search");
	//scanf("%s", search);
	if(option=='f'){
			printf("--------------------\n\n");
		for (int i = 0; i < tokens_size; i++) {	
			if(!strcmp("First name", tokens[i].string))
				printf("%s\n", tokens[i+1].string);
		}
		printf("\n--------------------\n\n");
	}
	if(option=='l'){
			printf("--------------------\n\n");
		for (int i = 0; i < tokens_size; i++) {	
			if(!strcmp("Last name", tokens[i].string))
				printf("%s\n", tokens[i+1].string);
		}
		printf("\n--------------------\n\n");
	}
	if(option=='i'){
			printf("--------------------\n\n");
		for (int i = 0; i < tokens_size; i++) {	
			if(!strcmp("Student_ID", tokens[i].string))
				printf("%s\n", tokens[i+1].string);
		}
		printf("\n--------------------\n\n");
	}
	if(option=='a'){
			printf("--------------------\n\n");
		for (int i = 0; i < tokens_size; i++) {	
			if(!strcmp("Age", tokens[i].string))
				printf("%s\n", tokens[i+1].string);
		}
		printf("\n--------------------\n\n");
	}
	if(option=='m'){
			printf("--------------------\n\n");
		for (int i = 0; i < tokens_size; i++) {	
			if(!strcmp("Major", tokens[i].string))
				printf("%s\n", tokens[i+1].string);
		}
		printf("\n--------------------\n\n");
	}
	if(option=='r'){
			printf("--------------------\n\n");
		for (int i = 0; i < tokens_size; i++) {	
			if(!strcmp("RC", tokens[i].string))
				printf("%s\n", tokens[i+1].string);
		}
		printf("\n--------------------\n\n");
	}
}

void help(){
	printf("***HELP****\n");
	printf("	option 'p': Will print token\n");
	printf("	option 's': Select the desired information of the entire student\n");
	printf("				 -> Print the desired information the entire student\n");
	printf("	option 'i': Entering a student number\n");
	printf("				 -> Will print all the information of that student\n");
	printf("	option 'n': Print the total number of students\n");
	printf("	option 'h': Explanation of how to use this application\n");
	printf("	option 'q': Exit this application\n");
}

int main(int argc, char **argv) {
	char* buffer;
	long file_size;
	int tokens_size = 0;
	char* file;
	
	
	if (argc >= 1) {
		file = argv[1];
	}
	buffer = FREAD(file,&file_size);
	/*
	printf("%s", buffer);
	printf("\n\n --Done-- \n\n");
	lower_string(buffer);

	printf("%s", buffer);
	printf("\n\n --Small case Done-- \n\n");
    printf("--Tokens-- \n\n");
	*/
	TOKEN tokens[1024];
	JSON_parse(buffer, file_size, &tokens_size, tokens);
	
	Find_TokenSize(buffer, tokens_size, tokens);

	char option;
	while(1){
		printf("**********APPLICATION**********\n\nWelcome to data management application\nChoose what you want to do\n\n");
		printf("\n -p : print token");
		printf("\n -s : search information");
		printf("\n -h : help");
		printf("\n -q : termination");
		printf("\n*******************************\n");
		printf("\t\t-->");
		scanf(" %c", &option);
		
		switch(option){
			case 'p' : OptionPrint(tokens_size,tokens);
						break;
			case 's' : OptionSearch(tokens_size,tokens);
						break;
			case 'h' : help();
						break;
		}
		if(option == 'q') break;
	}
	free_tokens(tokens, tokens_size);
	free(buffer);
	return EXIT_SUCCESS;
}
