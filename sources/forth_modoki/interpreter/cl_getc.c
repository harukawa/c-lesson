#include "clesson.h"
#include <string.h>

static const char* input = "123 456";
static int pos = 0;
static FILE *fp;

#define TEXT_SIZE 1024

int cl_getc() {
    if(strlen(input) == pos)
        return EOF;
    return input[pos++];
}


void cl_getc_set_src(char* str){
    input = str;
    pos = 0;
}

void cl_getc_set_fp(FILE* input_fp){
	fp = input_fp;
	int i=0,mem_size;
	char ch;
	char text[TEXT_SIZE];
	char *str;
	while((ch=fgetc(input_fp)) != EOF) {
		text[i] = ch;
		i++;
	}
	text[i] = EOF;
	mem_size = sizeof(char) * i;
	str = malloc(mem_size);
	memcpy(str, text, mem_size);
	cl_getc_set_src(str);
}

