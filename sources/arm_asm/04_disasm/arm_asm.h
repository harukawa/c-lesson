#include <stdio.h>
#include <assert.h>

enum OperationType {
	MOV,
	LDR,
	STR,
	BL,
	B,
	UNKNOWN
};

void cl_enable_buffer_mode();
void cl_disable_buffer_mode();
char *cl_get_printed_buffer();
void cl_printf(char *fmt, ...);

int cl_select_bit(int word, int and, int shift);
int cl_hex_minus(int word, int digit);

void assert_number(int expect, int actual);
void assert_char(char *expect, char *actual);
