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
	
	int single_ch,c;
	int output;
	enum LexicalType type;
	if(prev_ch == EOF){
		single_ch = cl_getc();
	}else{
		single_ch = prev_ch;
	}
	
	if(_isdigit(single_ch)){
		type = NUMBER;
		int number = 0;
		do{
				number = number * 10 + ( single_ch - '0');
				single_ch = cl_getc();
			}while(_isdigit(single_ch));
		output = number;
	}else{
		type = SPACE;
		output = single_ch;
		while(!_isdigit(single_ch)){
			single_ch = cl_getc();
		}
	}
	*out_val = output;
	*out_type = (int)type;
	return single_ch;
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
