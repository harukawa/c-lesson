#include <assert.h>
#include "arm_asm.h"

int streq(char *s1, char *s2) {
	if(strcmp(s1,s2)==0) {
		return 1;
	}else {
		return 0;
	}
}

void assert_number(int expect, int actual) {
	assert(expect == actual);
}

void assert_char(char *expect, char *actual) {
	assert(strcmp(expect, actual));
}
