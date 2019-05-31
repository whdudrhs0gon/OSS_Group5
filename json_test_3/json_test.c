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


char* FREAD(char* file, long* file_size) {
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


void JSON_parse(char* buffer, long file_size, int* token_size, TOKEN * tokens) {
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
			char* end = begin;                            /////////////////////////////////////

			for (int j = i + 1; j < file_size; j++) {
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
			
			if (end == NULL) {
				end = strchr(begin, '}');
				if (end == NULL) end = strchr(begin, ']');
			}

			
			int stringLength = end - begin;

			tokens[tokenIndex].type = PRIMITIVE;


			tokens[tokenIndex].string = (char*)malloc(stringLength);
			memset(tokens[tokenIndex].string, 0, stringLength + 1);
			memcpy(tokens[tokenIndex].string, begin, stringLength);

			int word_length = 0;
			for (int c = 0; c < stringLength; c++) {
				if (tokens[tokenIndex].string[c] == ' ') {
					word_length = c-1;
					memset(tokens[tokenIndex].string, 0, word_length + 1);
					memcpy(tokens[tokenIndex].string, begin, word_length);
					tokens[tokenIndex].end = tokens[tokenIndex].start + word_length;
					break;
				}
			}

			tokenIndex++;
			*token_size = tokenIndex;
			i = i + stringLength + 1;
		}
		break;
		}
	}

}

void free_tokens(TOKEN * tokens, int token_size) {
	for (int i = 0; i < token_size; i++) {
		if (tokens[i].type == STRING) {
			free(tokens[i].string);
		}
	}
}

void lower_string(char s[])
{
	int c = 0;
	while (s[c] != '\0')
	{
		if (s[c] >= 65 && s[c] <= 90)
		{
			s[c] = s[c] + 32;
		}
		c++;
	}
}

void Find_TokenSize(char* buffer, int tokens_size, TOKEN * tokens) {
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
			for (int j = i + 1; j < tokens_size; j++) {
				if (tokens[j].type == OBJECT && tokens[i].end >= tokens[j].start) {
					tokens[i].size = tokens[i].size - tokens[j].size;
					break;
				}
			}

			object_size = 0;
			for (int j = i + 1; j < tokens_size; j++) {
				if (tokens[j].string[0] == '{' && tokens[i].end >= tokens[j].start) break;

				if (tokens[j].type == OBJECT && tokens[i].end >= tokens[j].start) {
					object_size += tokens[j].size;
				}
			}
			tokens[i].size = (tokens[i].size * 2 - object_size) / 2;
			object_size = 0;
		}
	}
}
void OptionPrint(int tokens_size, TOKEN *tokens){
	for (int i = 0; i < tokens_size; i++) {
		printf("[%d] %s (size=%d, %d~%d, %s)\n",i+1, tokens[i].string, tokens[i].size, tokens[i].start, tokens[i].end, type[tokens[i].type]);
	}
}

void OptionPrintS(int tokens_size, TOKEN * tokens) {
	for (int i = 0; i < tokens_size; i++) {
		//printf("%s (size=%d, %d~%d, %s)\n", tokens[i].string, tokens[i].size, tokens[i].start, tokens[i].end, type[tokens[i].type]);
		char* str = tokens[i].string;
		for(int a = 0; a < tokens[i].end - tokens[i].start; a++)
		{
		if (str[a]>='A' && str[a]<='Z' && a==0)
		{	str[a] = str[a] + 32;}

		if (str[a]>='a' && str[a]<='z' && a!=0 && str[a-1]==' ')
		{	str[a] = str[a] - 32;}

		if (str[0]>='a' && str[0]<='z')
		{	str[0] = str[0] - 32;}
		}

		switch (i%2)
		{
			case 0 :
				printf("     - %s : ", str);
				break;
			case 1 :
				
				printf("%s\n", str);
				break;
		}
	}
}

void selectOption(char* option, int* option_index) {
	printf("\n\nChoose what information to search(to stop enter zero)");
	printf("\n\t -f : first name");
	printf("\n\t -l : last name");
	printf("\n\t -i : student id");
	printf("\n\t -a : age");
	printf("\n\t -m : major");
	printf("\n\t -r : RC");
	printf("\n\t\t-->");

	int index = *option_index;
	while (1) {
		char op;
		scanf(" %c", &op);
		if (op == 'f' || op == 'l' || op == 'i' || op == 'a' || op == 'm' || op == 'r') {
			option[index] = op;
			index++;
		}
		else
			break;
	}
	*option_index = index;
}

void OptionSearch(int tokens_size, TOKEN *tokens, char* option, int option_index){
	
    for(int j=0; j<option_index; j++){
        if(option[j]=='f'){
                //printf("--------------------\n\n");
            for (int i = 0; i < tokens_size; i++) {	
                if(!strcmp("First name", tokens[i].string))
                    printf("First name: %s\n", tokens[i+1].string);
            }
            //printf("\n--------------------\n\n");
        }
        if(option[j]=='l'){
                //printf("--------------------\n\n");
            for (int i = 0; i < tokens_size; i++) {	
                if(!strcmp("Last name", tokens[i].string))
                    printf("Last name: %s\n", tokens[i+1].string);
            }
            //printf("\n--------------------\n\n");
        }
        if(option[j]=='i'){
                //printf("--------------------\n\n");
            for (int i = 0; i < tokens_size; i++) {	
                if(!strcmp("Student_ID", tokens[i].string))
                    printf("Student_ID: %s\n", tokens[i+1].string);
            }
            //printf("\n--------------------\n\n");
        }
        if(option[j]=='a'){
                //printf("--------------------\n\n");
            for (int i = 0; i < tokens_size; i++) {	
                if(!strcmp("Age", tokens[i].string))
                    printf("Age: %s\n", tokens[i+1].string);
            }
            //printf("\n--------------------\n\n");
        }
        if(option[j]=='m'){
                //printf("--------------------\n\n");
            for (int i = 0; i < tokens_size; i++) {	
                if(!strcmp("Major", tokens[i].string))
                    printf("Major: %s\n", tokens[i+1].string);
            }
            //printf("\n--------------------\n\n");
        }
        if(option[j]=='r'){
                //printf("--------------------\n\n");
            for (int i = 0; i < tokens_size; i++) {	
                if(!strcmp("RC", tokens[i].string))
                    printf("RC: %s\n", tokens[i+1].string);
            }
            printf("\n--------------------\n\n");
        }
    }
}


int OptionID(int tokens_size, TOKEN * tokens, char* idnumber) {
	int counter = 0;
	for (int i = 0; i < tokens_size; i++) {
		if (!strncmp(idnumber, tokens[i].string,8) && strlen(tokens[i].string)==8 ) {
			OptionPrintS(tokens_size, tokens);
			counter++;
		}
	}
	if(counter==0)
		return 0;
	else
	{
		return 1;
	}
	
}

int main(int argc, char** argv) {
	char* buffer = "";
	long file_size;
	int tokens_size = 0;
	char* file = "";
	TOKEN tokens[1024];
	char option;
	while (1) {
		printf("**********APPLICATION**********\n\nWelcome to data management application\nChoose what you want to do\n\n");
		printf("\n -p : print token");
		printf("\n -s : list selected data");
		printf("\n -i : search information using student_ID");
		printf("\n -n : show number of students");
		printf("\n -q : termination");
		printf("\n*******************************\n");
		printf("\t\t-->");
		scanf(" %c", &option);

		switch (option) {
		case 'p':
		{
			for (int filenumber = 1; filenumber < argc; filenumber++)
			{
				if (argc >= 1) {
					file = argv[filenumber];
				}

				buffer = FREAD(file, &file_size);
				JSON_parse(buffer, file_size, &tokens_size, tokens);

				Find_TokenSize(buffer, tokens_size, tokens);
				OptionPrint(tokens_size, tokens);

				free_tokens(tokens, tokens_size);
				free(buffer);
				printf("\n");
			}
		}
		break;

		case 's':
		{
			char option2[128] = { '\0' };
			int option_index = 0;

			selectOption(option2, &option_index);

			for (int filenumber = 1; filenumber < argc; filenumber++)
			{
				if (argc >= 1) {
					file = argv[filenumber];
				}

				buffer = FREAD(file, &file_size);
				JSON_parse(buffer, file_size, &tokens_size, tokens);

				Find_TokenSize(buffer, tokens_size, tokens);
				OptionSearch(tokens_size, tokens, option2, option_index);

				free_tokens(tokens, tokens_size);
				printf("\n");
				free(buffer);
			}
			option_index = 0;
		}
		break;

		case 'i':
		{
			int counter = 0,checker = 0;
			char* idnumber = malloc(sizeof(char) * 20);
			printf("\n\n Enter student ID : ");
			scanf("%s", idnumber);

			for (int filenumber = 1; filenumber < argc; filenumber++)
			{
				if (argc >= 1) {
					file = argv[filenumber];
				}

				buffer = FREAD(file, &file_size);
				JSON_parse(buffer, file_size, &tokens_size, tokens);

				Find_TokenSize(buffer, tokens_size, tokens);
				checker = OptionID(tokens_size, tokens, idnumber);
				counter= counter + checker;
				free_tokens(tokens, tokens_size);
				free(buffer);
			}
			if(counter==0)
			{printf("\n    %s Not Found\n", idnumber);}
			free(idnumber);
		}
		break;

		case 'n':
		{
			printf("\n There are total %d students in the data.\n",argc-1);
		}
		break;

		case 'q':
		{
			printf("**********APPLICATION**********\n\n");
			printf(" Thank you for Using Our Program XD\n");
			printf(" Have a nice day! Bye~\n");
			printf("\n*******************************\n");
			return EXIT_SUCCESS;
		}
		}
	
	printf("\n Press Enter to continue. \n");
	while (getchar() != '\r' && getchar() != '\n');
}
}
