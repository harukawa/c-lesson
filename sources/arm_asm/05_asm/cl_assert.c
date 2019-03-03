#include <assert.h>
#include <string.h>
#include "asm.h"

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

void assert_streq(char *expect, char *actual) {
	assert(streq(expect, actual));
}

void assert_substreq(char *expect,char *actual,int actual_len) {
	for(int i = 0; i < actual_len; i++) {
		assert(expect[i] == actual[i]);
	}
}