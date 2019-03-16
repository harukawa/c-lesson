#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define PARSE_FAIL -1


struct substring {
	char *str;
	int len;
};

struct Emitter {
	unsigned char *buf;
	int pos;
};

int mnemonics_list[124];

void cl_file_set_fp(FILE *input_fp);
int cl_getline(char **out_buf);

void assert_substreq(char *expect,char *actual,int actual_len);
void assert_streq(char *expect, char *actual);
void assert_number(int expect, int actual);

int parse_one(char *str, struct substring *out_sub);
int parse_register(char *str, int *out_register);
int skip_comma(char *str);
int is_register(char *str);
int parse_immediate(char *str, int *out_immediate);
int skip_sbracket(char *str);
int is_sbracket(char *str);

void setup_mnemonic();
int to_mnemonic_symbol(char *str);

int assemble();
void debug_emitter_dump();