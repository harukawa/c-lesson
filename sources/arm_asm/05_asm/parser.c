#include "asm.h"

int is_valid_digit(int c){
	return  '0' <= c && c <= '9';
}

int is_valid_character(int c){
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_' || c == '.';
}

int is_valid_hexdigit(int c) {
	return  ('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F') ;
}

int is_register(char *str) {
	int single_ch, length = 0;
	single_ch = str[0];

	while(!is_valid_character(single_ch) && !is_valid_digit(single_ch)) {
		if('\0' == single_ch) return -1;
		length++;
		single_ch = str[length];
	}
	if('r' == single_ch || 'R' == single_ch) {
		return 1;
	}
	return 0;
}

int convert_character_to_digit(int c) {
	if('a' <= c && c <= 'f') {
		return c - 'a' + 10;
	} else if('A' <= c && c <= 'F') {
		return c - 'A' + 10;
	}
	return 0;
}
	
int is_sbracket(char *str) {
	int single_ch, length = 0;
	single_ch = str[0];

	while(single_ch == ' ') {
		if('\0' == single_ch) return -1;
		length++;
		single_ch = str[length];
	}
	if('[' == single_ch || ']' == single_ch) {
		return 1;
	}
	return 0;
}

int parse_one(char *str, struct substring *out_sub) {
	
	int single_ch, i = 0;
	int length = 0;
	
	single_ch = str[0];
	
	//先頭の空白をスキップ
	while(!is_valid_character(single_ch) && !is_valid_digit(single_ch)) {
		if(single_ch == '\0') return -1;
		i++;
		single_ch = str[i];
	}
	while(is_valid_character(single_ch) || is_valid_digit(single_ch)) {
		if(single_ch == '\0') return -1;
		length++;
		single_ch = str[i + length];
	}
	
	if(single_ch == ' ') {
		out_sub->str = &str[i];
		out_sub->len = length;
		return i + length;
	//　コンマが付いていた時
	} else if(single_ch == ':') {
		length++;
		out_sub->str = &str[i];
		out_sub->len = length;
		return i + length;
	}
	
	return PRASE_FAIL;
}

int parse_register(char *str, int *out_register) {
	int single_ch, length = 0;
	single_ch = str[0];

	while('r' != single_ch && 'R' != single_ch) {
		if(single_ch == '\0') return PRASE_FAIL;
		length++;
		single_ch = str[length];
	}
	length++;
	single_ch = str[length];

	int number = 0;
	do {
		if(single_ch == '\0') return PRASE_FAIL;
		number = number * 10 +(single_ch-'0');
		length++;
		single_ch = str[length];
	}while(is_valid_digit(single_ch));
	
	*out_register = number;
	
	return length;
}

int parse_immediate(char *str, int *out_immediate) {
	int single_ch, length = 0;
	single_ch = str[0];
	int minus = 0;

	while('#' != single_ch) {
		if(single_ch == '\0') return PRASE_FAIL;
		length++;
		single_ch = str[length];
	}
	single_ch = str[length+1];
	if(single_ch == '-') {
		length++;
		minus = 1;
	}
	length = length + 3;
	single_ch = str[length];

	int number = 0x0;
	int digit = 0;
	do {
		if(single_ch == '\0') return PRASE_FAIL;
		if(is_valid_character(single_ch)) {
			single_ch = convert_character_to_digit(single_ch);
			number = number * 16 +(single_ch);
		} else {
			number = number * 16 +(single_ch-'0');
		}
		length++;
		digit++;
		if(0 == (single_ch - '0') && 1 == digit) {
				digit = 0;
		}
		single_ch = str[length];
	}while(is_valid_hexdigit(single_ch));
	
	if(minus) {
		int m = 0x0;
		for(int i=0; i<digit; i++) {
			m = m * 16 + 0xf;
		}
		number = ~number + 0x1;
		number = number & m;
	}
	
	*out_immediate = number;
	
	return length;
}

int parse_raw(char *str, int *out_embedded) {
	int single_ch, length = 0;
	single_ch = str[0];
	int minus = 0;

	while(' ' == single_ch) {
		if(single_ch == '\0') return PRASE_FAIL;
		length++;
		single_ch = str[length];
	}
	if(single_ch == '-') {
			length++;
			minus = 1;
			single_ch = str[length];
	}
	if('0' == single_ch) {
		single_ch = str[length-1];
		length = length + 2;
		single_ch = str[length];

		int number = 0x0;
		int digit = 0;
		do {
			if(single_ch == '\0') return PRASE_FAIL;
			if(is_valid_character(single_ch)) {
				single_ch = convert_character_to_digit(single_ch);
				number = number * 16 +(single_ch);
			} else {
				number = number * 16 +(single_ch-'0');
			}
			length++;
			digit++;
			if(0 == (single_ch - '0') && 1 == digit) {
				digit = 0;
			}
			single_ch = str[length];
		}while(is_valid_hexdigit(single_ch));
		if(minus) {
			int m = 0x0;
			for(int i=0; i<digit; i++) {
				m = m * 16 + 0xf;
			}
			number = ~number + 0x1;
			number = number & m;
		}
		*out_embedded = number;
	} else {
		//後で文字のケースを追加
	}
	return length;
}

int skip_sbracket(char *str) {
	int length = 0;
	int single_ch;
	single_ch = str[0];

	while(single_ch != '['){
		if(single_ch == '\0') return PRASE_FAIL;
		length++;
		single_ch = str[length];
	}
	length++;
	return length;
}

int skip_comma(char *str) {
	int length = 0;
	int single_ch;
	single_ch = str[0];

	while(single_ch != ','){
		if(single_ch == '\0') return PRASE_FAIL;
		length++;
		single_ch = str[length];
	}
	length++;
	return length;
}

static void test_parse_one_upper() {
	char *input = " MOV  r1, r2";
	char *expect = "MOV";
	int expect_len = 4;
	int expect_str_len = 3;
	
	struct substring actual;
	int actual_len = parse_one(input, &actual);
	
	assert_number(expect_len, actual_len);
	assert_number(expect_str_len, actual.len);
	assert_substreq(expect, actual.str, actual.len);
}

static void test_parse_one_lower() {
	char *input = "mov  r1, r2";
	char *expect = "mov";
	int expect_len = 3;
	int expect_str_len = 3;
	
	struct substring actual;
	int actual_len = parse_one(input, &actual);
	
	assert_number(expect_len, actual_len);
	assert_number(expect_str_len, actual.len);
	assert_substreq(expect, actual.str, actual.len);
}

static void test_parse_one_colon() {
	char *input = " mov:  r1, r2";
	char *expect = "mov:";
	int expect_len = 5;
	int expect_str_len = 4;
	
	struct substring actual;
	int actual_len = parse_one(input, &actual);
	
	assert_number(expect_len, actual_len);
	assert_number(expect_str_len, actual.len);
	assert_substreq(expect, actual.str, actual.len);
}

static void test_parse_one_fail() {
	char *input = "      ";
	int expect = -1;
	
	struct substring actual;
	int actual_len = parse_one(input, &actual);
	
	assert_number(expect, actual_len);
}

static void test_parse_register() {
	char *input = "     r1, r2";
	int actual;
	int expect_len = 7;
	int expect = 1;

	int actual_len = parse_register(input, &actual);

	assert_number(expect_len, actual_len);
	assert_number(expect, actual);
}

static void test_parse_register_fail() {
	char *input = "mov     r";
	int actual;
	int expect = -1;

	int actual_len = parse_register(&input[3], &actual);

	assert_number(expect, actual_len);
}

static void test_skip_comma() {
	char *input = "r1 , r1";
	int expect = 2;

	int actual = skip_comma(&input[2]);

	assert_number(expect, actual);
}

static void test_parse_immediate() {
	char *input = "  #0x64";
	int actual;
	int expect_len = 7;
	int expect = 0x64;

	int actual_len = parse_immediate(input, &actual);

	assert_number(expect_len, actual_len);
	assert_number(expect, actual);
}

static void test_parse_immediate_minus() {
	char *input = "  #-0x64";
	int actual;
	int expect_len = 8;
	int expect = 0x9c;

	int actual_len = parse_immediate(input, &actual);

	assert_number(expect_len, actual_len);
	assert_number(expect, actual);
}

static void test_parse_immediate_fail() {
	char *input = "   #0x";
	int actual;
	int expect = -1;

	int actual_len = parse_immediate(input, &actual);

	assert_number(expect, actual_len);
}

static void test_is_register() {
	char *input = "   r1";
	int actual;
	int expect = 1;

	int actual_len = is_register(input);

	assert_number(expect, actual_len);
}

static void test_is_sbracket() {
	char *input = "   ]";
	int actual;
	int expect = 1;

	int actual_len = is_sbracket(input);
	
	assert_number(expect, actual_len);
}

static void test_skip_sbracket() {
	char *input = "   [r1, r2]";
	int expect = 4;

	int actual = skip_sbracket(input);

	assert_number(expect, actual);
}

static void test_parse_raw_immediate() {
	char *input = "  0x64";
	int actual;
	int expect = 0x64;

	int actual_len = parse_raw(input, &actual);

	assert_number(expect, actual);
}

static void test_parse_raw_immediate_minus() {
	char *input = "  -0x064";
	int actual;
	int expect = 0x9c;

	int actual_len = parse_raw(input, &actual);

	assert_number(expect, actual);
}

static void unit_tests() {
	test_parse_one_upper();
	test_parse_one_lower();
	test_parse_one_colon();
	test_parse_one_fail();
	
	test_parse_register();
	test_parse_register_fail();
	
	test_parse_immediate();
	test_parse_immediate_fail();
	
	test_is_register();
	test_skip_comma();
	test_is_sbracket();
	test_skip_sbracket();
	
	test_parse_immediate_minus();
	test_parse_raw_immediate();
	test_parse_raw_immediate_minus();
}
#if 0
int main(){
	unit_tests();
	return 0;
}
#endif
