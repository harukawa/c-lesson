#include "clesson.h"
#include <string.h>

static char* input = "123 456";
static int pos = 0;

void cl_getc_set_src(char* str){
    pos = 0;
    input = str;
}

int cl_getc() {
    if(strlen(input) == pos)
        return EOF;
    return input[pos++];
}
