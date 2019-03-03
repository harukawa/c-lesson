#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define PRASE_FAIL -1

enum ByteType {
	ONE_WORD,
	ONE_BYTE
};

struct substring {
	char *str;
	int len;
};

struct Emitter {
	unsigned int array;
	enum ByteType type;
};

void cl_file_set_fp(FILE *input_fp);
int cl_getline(char **out_buf);

void assert_substreq(char *expect,char *actual,int actual_len);
void assert_streq(char *expect, char *actual);
void assert_number(int expect, int actual);

int parse_one(char *str, struct substring *out_sub);
int parse_register(char *str, int *out_register);
int skip_comma(char *str);

int assembly();