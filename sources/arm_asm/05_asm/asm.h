#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define PARSE_FAIL -1

int g_mov;
int g_MOV;
int g_ldr;
int g_LDR;
int g_str;
int g_STR;
int g_raw;
int g_b;
int g_B;
int g_ldrb;
int g_LDRB;
int g_add;
int g_ADD;
int g_cmp;
int g_CMP;
int g_bne;
int g_BNE;
int g_bl;
int g_BL;
int g_blt;
int g_BLT;
int g_ldmia;
int g_LDMIA;
int g_stmdb;
int g_STMDB;

struct List {
	struct List *next;
	int emitter_pos;
	int label;
	int code;
	int immediate;
};

struct substring {
	char *str;
	int len;
};

struct Emitter {
	unsigned char *buf;
	int pos;
	int byte_remain;
};

struct KeyValue {
	int key;
	int value;
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
int parse_immediate(char *str, int *out_immediate, char target);
int skip_sbracket(char *str);
int is_sbracket(char *str);
int parse_string(char *str, char stopchar, char **out_str);
int is_raw_string(char *str);
int parse_raw_number(char *str, int *out_embedded);
int is_equals_sign(char *str);
int is_one_char(char *str, char target);
int is_braces(char *str);
int skip_braces(char *str);
int skip_one_char(char *str, char target);
int is_equals_next_number(char *str);

void setup_mnemonic();
int to_mnemonic_symbol(char *str, int len);
int to_label_symbol(char *str, int len);

int assemble();
void debug_emitter_dump();

int dict_get(int key, struct KeyValue *out_keyValue);
void dict_put(struct KeyValue *keyValue);
void dict_init();

void unresolved_list_put(struct List *add_list);
int unresolved_list_get(struct List *out_list);
void unresolved_list_init();
