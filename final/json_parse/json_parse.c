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
			//if (i == 0) continue;

			char* begin = buffer + i;
			tokens[tokenIndex].start = begin - buffer;

			
			int test = 0;
			char* end = begin;                          

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


			int test = 0;
			char* end = begin;                          

			for (int j = i + 1; j < file_size; j++) {
				if (buffer[j] == '[') test++;
				if (buffer[j] == ']') test--;

				if (test <= -1) {
					end = &buffer[j];
					test = 0;
					break;
				}
			}


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


void Find_TokenSize(char* buffer, int tokens_size, TOKEN * tokens) {
	int size = 0; // token[i].size
	int key = TRUE;
	int index;

	for (int i = 0; i < tokens_size; i++) {
		switch (tokens[i].string[0]) {
		case '{':
		{
			int array_end = 0;
			int object_end = 0;
			int end = tokens[i].end;
			index = i;
			index++;
			while (1) {
				int start = tokens[index].start;

				if (tokens[index].type == ARRAY) {
					array_end = tokens[index].end;
					while(1) {
						index++;
						if (array_end < tokens[index].start || index > tokens_size) {
							index--;
							break;
						}
					}
				}
				else if (tokens[index].type == OBJECT) {
					object_end = tokens[index].end;
					while(1) {
						index++;
						if (object_end < tokens[index].start || index > tokens_size) {
							index--;
							break;
						}
					}
				}

				if (start > end || index > tokens_size) break;
				size++;
				index++;
			}


			tokens[i].size = (size) / 2;
			size = 0;
		}
		break;

		case '[':
		{
			int end = tokens[i].end;
			int temp_size = 0;
			int index = 0;
			int array_size = 0;
			int count = 0;

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

				if (tokens[i].type == ARRAY && count == 0) {
					count++;
					index = i;
					int start2 = tokens[index].end;
					while (start2 > tokens[index].start) {
						if (tokens[index].type != ARRAY) array_size++;
						index++;
					}
					index = 0;
				}

				size++;
			}
			i = i - temp_size - size -1;			
			tokens[i].size = size - array_size;
			count = 0;
			size = 0;
			array_size = 0;
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
}

void OptionPrint(int tokens_size, TOKEN *tokens){
	for (int i = 0; i < tokens_size; i++) {
        printf("[%d] %s (size=%d, %d~%d, %s)\n",i, tokens[i].string, tokens[i].size, tokens[i].start, tokens[i].end, type[tokens[i].type]);
    }
}




int main(int argc, char** argv) {
	char* buffer = "";
	long file_size;
	int tokens_size = 0;
	char* file = "";
	TOKEN tokens[1024];
	char option;

	if (argc >= 1) {
		file = argv[1];
	}
	buffer = FREAD(file, &file_size);
	JSON_parse(buffer, file_size, &tokens_size, tokens);
	Find_TokenSize(buffer, tokens_size, tokens);

	OptionPrint(tokens_size, tokens);

	free_tokens(tokens, tokens_size);
	free(buffer);
}