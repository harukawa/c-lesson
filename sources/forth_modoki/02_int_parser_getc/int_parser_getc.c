#include "clesson.h"
#include <assert.h>

/*
cc cl_getc.c int_parser_getc.c
*/

enum LexicalType {
	NUMBER,
	SPACE 
};

int _isdigit(int c){
	if('0' <= c && c <= '9'){
		return 1;
	}else{
		return 0;
	}
}

int parse_one(int prev_ch, int *out_val, int *out_type){
	
	int ch;
	enum LexicalType type;
	if(prev_ch == EOF){
		ch = cl_getc();
	}else{
		ch = prev_ch;
	}

	if(_isdigit(ch) == 1){
		type = NUMBER;
		ch = ch - '0';
	}else{
		type = SPACE;
	}
	int c;
	while(1){
		c = cl_getc();
		if(c == EOF){
			break;
		}

		if(_isdigit(c) == 1){
			// If prev_ch is NUMBER
			if(type == SPACE){
				break;
			}
			ch = ch * 10 + (c - '0');
		}else{
			// If prev_ch is SPACE
			if(type == NUMBER){
				break;
			}
			continue;
		}
	}
	*out_val = ch;
	*out_type = (int)type;
	return c;
}

void test_parse_one_123(){
	cl_getc_set_src("123");
	int answer = 0;
	enum LexicalType type;
	int ch;
	ch = parse_one(EOF,&answer,&type);
	assert(answer == 123);
}

void test_parse_one_123_456(){
	cl_getc_set_src("123 456");
	int answer1 = 0;
	int answer2 = 0;
	int ch,tmp;
	enum LexicalType type;
	type = SPACE;
	ch = parse_one(EOF,&answer1,&type);
	ch = parse_one(ch,&tmp,&type);
	ch = parse_one(ch,&answer2,&type);
    
    // verity result.
	assert(answer1 == 123);
	assert(answer2 == 456);
}
int main() {
	int answer1 = 0;
	int answer2 = 0;
	
	test_parse_one_123();
	test_parse_one_123_456();
	cl_getc_set_src("123 456");

	int ch,tmp;
	enum LexicalType type;
	type = SPACE;
	ch = parse_one(EOF,&answer1,&type);
	ch = parse_one(ch,&tmp,&type);
	ch = parse_one(ch,&answer2,&type);
    
    // verity result.
	assert(answer1 == 123);
	assert(answer2 == 456);

    return 1;
}
