#define _CRT_SECURE_NO_WARNINGS    

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
				char* end = strchr(begin, '}');
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
				char* end = strchr(begin, ']');
				tokens[tokenIndex].end = end - buffer + 1;
				int stringLength = end - begin + 1;

				tokens[tokenIndex].type = PRIMITIVE;
				tokens[tokenIndex].string = (char*)malloc(stringLength + 1);
				memset(tokens[tokenIndex].string, 0, stringLength + 1);
				memcpy(tokens[tokenIndex].string, begin, stringLength);
				tokenIndex++;
				*token_size = tokenIndex;
			}
			break;

			case 'f':
			{
				char* begin = buffer + i;
				tokens[tokenIndex].start = begin - buffer;
				char* end = strchr(begin, ']');
				tokens[tokenIndex].end = end - buffer + 1;
				int stringLength = end - begin + 1;

				tokens[tokenIndex].type = PRIMITIVE;
				tokens[tokenIndex].string = (char*)malloc(stringLength + 1);
				memset(tokens[tokenIndex].string, 0, stringLength + 1);
				memcpy(tokens[tokenIndex].string, begin, stringLength);
				tokenIndex++;
				*token_size = tokenIndex;
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

int main(int argc, char **argv) {
	char* buffer;
	long file_size;
	int tokens_size = 0;
	char* file;
	if (argc >= 1) {
		file = argv[1];
	}
	buffer = FREAD(file,&file_size);
	printf("%s", buffer);
	printf("\n\n --Done-- \n\n");
	lower_string(buffer);

	printf("%s", buffer);
	printf("\n\n --Small case Done-- \n\n");
    printf("--Tokens-- \n\n");

	TOKEN tokens[128];
	JSON_parse(buffer, file_size, &tokens_size, tokens);





	for (int i = 0; i < tokens_size; i++) {
		printf("[ %d] %s (%d~%d, %s)\n",i+1, tokens[i].string, tokens[i].start, tokens[i].end, type[tokens[i].type]);
	}

	free_tokens(tokens, tokens_size);
	free(buffer);
	return EXIT_SUCCESS;
}
