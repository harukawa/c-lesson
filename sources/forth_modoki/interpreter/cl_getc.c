#include "clesson.h"
#include <string.h>

static const char* input = "123 456";
static int pos = 0;
static FILE *fp = NULL;

#define TEXT_SIZE 1024

int cl_getc() {
	if(fp != NULL){
		return fgetc(fp);
	}
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
}

