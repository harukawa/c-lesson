#include "asm.h"

int is_valid_digit(int c){
	return  '0' <= c && c <= '9';
}

int is_valid_character(int c){
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
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

int skip_comma(char *str) {
	int length = 0;
	int single_ch;
	single_ch = str[0];

	while(single_ch == ',' || single_ch == ' '){
		if(single_ch == '\0') return -1;
		length++;
		single_ch = str[length];
	}
	
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
	char *input = "        r1, r2";
	int actual;
	int expect_len = 7;
	int expect = 1;

	int actual_len = parse_register(&input[3], &actual);

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
	char *input = "r1, r1";
	int expect = 2;

	int actual = skip_comma(&input[2]);

	assert_number(expect, actual);
}

static void unit_tests() {
	test_parse_one_upper();
	test_parse_one_lower();
	test_parse_one_colon();
	test_parse_one_fail();
	
	test_parse_register();
	test_parse_register_fail();
	
	test_skip_comma();
}

#if 0
int main(){
	unit_tests();
	return 0;
}
#endif
