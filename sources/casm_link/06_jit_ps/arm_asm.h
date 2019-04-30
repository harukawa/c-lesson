#include <stdio.h>
#include <assert.h>
#include <string.h>

enum OperationType {
	MOV,
	CMP,
	AND,
	SUB,
	ADD,
	LDR,
	LDRB,
	STR,
	BL,
	B,
	BNE,
	BGE,
	LSR,
	BLT,
	PUSH,
	LDMIA,
        STMDB,
	MUL,
	UNKNOWN
};

void cl_clear_output();
void cl_enable_buffer_mode();
void cl_disable_buffer_mode();
char *cl_get_printed_buffer();
char *cl_get_result(int num);
void cl_printf(char *fmt, ...);

int cl_select_bit(int word, int and, int shift);
int cl_hex_minus(int word, int digit);
int cl_rotate_bit(int shift, int word);

int print_asm(int word);

void assert_number(int expect, int actual);
void assert_streq(char *expect, char *actual);
void assert_substreq(char *expect,char *actual,int actual_len);
