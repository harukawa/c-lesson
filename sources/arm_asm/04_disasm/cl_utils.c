#include <stdarg.h>
#include "arm_asm.h"

static char buf[100*1024];
static int to_buffer = 0;
static int pos = 0;

void cl_clear_output() {
    pos = 0;
    buf[0] = '\0';
}

char *cl_get_result(int num) {
    int null_count=0;
    int i=0;
    while (null_count != num) {
        if (buf[i] == '\0') {
            null_count++;
        }
        i++;
    }
    return &buf[i];
}

void cl_enable_buffer_mode() {
    to_buffer = 1;
}

void cl_disable_buffer_mode() {
	to_buffer = 0;
}

char *cl_get_printed_buffer() {
    return buf;
}

void cl_printf(char *fmt, ...) {
    va_list arg_ptr;
    va_start(arg_ptr, fmt);

    if(to_buffer) {
        pos += vsprintf(&buf[pos], fmt, arg_ptr);
        pos++;
    } else {
        vprintf(fmt, arg_ptr);
    }
    va_end(arg_ptr);
}

int cl_select_bit(int word, int and, int shift) {
	int rd = word & and;
	return rd >> shift * 4;
}

int cl_hex_minus(int word, int digit) {
	int value = word >> digit * 4;
	if(0x8 == (value & 0x8)) {
		return 1;
	}
	return 0;
}
