#include "clesson.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int _isdigit(int c){
	return  '0' <= c && c <= '9';
}

int _isExecutable(int c){
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

#define NAME_SIZE 256

int parse_one(int prev_ch, struct Token *out_token) {
    /****
     * 
     * TODO: Implement here!
     * 
    ****/
	int single_ch;
	if(prev_ch == EOF){
		single_ch = cl_getc();
	}else{
		single_ch = prev_ch;
	}
	// Number
	if(_isdigit(single_ch)){
		int number;
		number = 0;
		do {
			number = number * 10 + ( single_ch - '0');
		}while(_isdigit(single_ch = cl_getc()));
		out_token->ltype = NUMBER;
		out_token->u.number = number;
		return single_ch;
	
	//Executable
	}else if(_isExecutable(single_ch)){
		char *str;
		char tmp[NAME_SIZE];
		int i = 0,mem_size;
		do{
			tmp[i] = (char)single_ch;
			i++;
		}while(_isExecutable(single_ch = cl_getc()));
		tmp[i] = '\0';
		i++;
		mem_size = sizeof(char) * i;
		str = malloc(mem_size);
		memcpy(str, tmp, mem_size);

		out_token->ltype = EXECUTABLE_NAME;
		out_token->u.name = str;
		return single_ch;

	//Literal
	}else if(single_ch == '/'){
		char *str;
		char tmp[NAME_SIZE];
		int i = 0,mem_size;
		while(_isExecutable(single_ch = cl_getc())){
			tmp[i] = (char)single_ch;
			i++;
		}
		tmp[i] = '\0';
		i++;
		mem_size = sizeof(char) * i;
		str = malloc(mem_size);
		memcpy(str, tmp, mem_size);
		
		out_token->ltype = LITERAL_NAME;
		out_token->u.name = str;
		return single_ch;

	//SPACE	
	}else if(single_ch == ' ') { 
		do {
			single_ch = cl_getc();
		}while(single_ch == ' ');
		
		out_token->ltype = SPACE;
		out_token->u.onechar = ' ';
		return single_ch;

	//OPEN_CURLY
	}else if(single_ch == '{'){
		out_token->ltype = OPEN_CURLY;
		out_token->u.onechar = (char)single_ch;
		single_ch = cl_getc();
		return single_ch;

	//CLOSE_CURLY
	}else if(single_ch == '}'){
		out_token->ltype = CLOSE_CURLY;
		out_token->u.onechar = (char)single_ch;
		single_ch = cl_getc();
		return single_ch;

	//PARCENT
	}else if(single_ch == '%'){
		while(1){
			single_ch = cl_getc();
			if(single_ch == '\n'){
				out_token->ltype = SPACE;
				out_token->u.onechar = ' ';	
				return single_ch;
			}
			if(single_ch == EOF){
				out_token->ltype = END_OF_FILE;
				return EOF;
			}
		}

	//LINE_FEED
	}else if(single_ch == '\n'){
		do {
			single_ch = cl_getc();
		}while(single_ch == ' ');
		
		out_token->ltype = SPACE;
		out_token->u.onechar = ' ';
		return single_ch;

	//EOF
	}else if(single_ch == EOF){
		out_token->ltype = END_OF_FILE;
		return EOF;
	}

    out_token->ltype = UNKNOWN;
    return EOF;
}

void parser_print_all() {
    int ch = EOF;
    struct Token token = {
        UNKNOWN,
        {0}
    };

    do {
        ch = parse_one(ch, &token);
        if(token.ltype != UNKNOWN) {
            switch(token.ltype) {
                case NUMBER:
                    printf("num: %d\n", token.u.number);
                    break;
                case SPACE:
                    printf("space!\n");
                    break;
                case OPEN_CURLY:
                    printf("Open curly brace '%c'\n", token.u.onechar);
                    break;
                case CLOSE_CURLY:
                    printf("Close curly brace '%c'\n", token.u.onechar);
                    break;
                case EXECUTABLE_NAME:
                    printf("EXECUTABLE_NAME: %s\n", token.u.name);
                    break;
                case LITERAL_NAME:
                    printf("LITERAL_NAME: %s\n", token.u.name);
                    break;

                default:
                    printf("Unknown type %d\n", token.ltype);
                    break;
            }
        }
    }while(ch != EOF);
}




static void test_parse_one_number() {
    char *input = "123";
    int expect = 123;

    struct Token token = {UNKNOWN, {0}};
    int ch;

    cl_getc_set_src(input);

    ch = parse_one(EOF, &token);

    assert(ch == EOF);
    assert(token.ltype == NUMBER);
    assert(expect == token.u.number);
}

static void test_parse_one_empty_should_return_END_OF_FILE() {
    char *input = "";
    int expect = END_OF_FILE;

    struct Token token = {UNKNOWN, {0}};
    int ch;

    cl_getc_set_src(input);
    ch = parse_one(EOF, &token);

    assert(ch == EOF);
    assert(token.ltype == expect);
}

static void test_parse_one_executable() {
    char* input = "add";
    char* expect_name = "add";
	int expect_type = EXECUTABLE_NAME;

    struct Token token = {UNKNOWN, {0}};
    int ch;

    cl_getc_set_src(input);

    ch = parse_one(EOF, &token);
    assert(ch == EOF);
    assert(token.ltype == expect_type);
    assert( strcmp(expect_name,token.u.name)== 0);
}

static void test_parse_one_literal(){
	char* input = "/add";
	char* expect_name = "add";
	int expect_type = LITERAL_NAME;

    struct Token token = {UNKNOWN, {0}};
    int ch;

    cl_getc_set_src(input);

    ch = parse_one(EOF, &token);
    assert(ch == EOF);
    assert(token.ltype == expect_type);
    assert( strcmp(expect_name,token.u.name) == 0);
}

static void test_parse_percent(){
	char *input = "%\n";
	char* expect_name = " ";
	int expect_type = SPACE;

    struct Token token = {UNKNOWN, {0}};
    int ch;

    cl_getc_set_src(input);
    ch = parse_one(EOF, &token);

    assert(token.ltype == expect_type);
    assert( *expect_name == token.u.name);
}

static void test_parse_percent_EOF(){
	char *input = "%";
	int expect_type = END_OF_FILE;

    struct Token token = {UNKNOWN, {0}};
    int ch;

    cl_getc_set_src(input);
    ch = parse_one(EOF, &token);

    assert(token.ltype == expect_type);
    assert(ch == EOF);
}

static void test_parse_line_feed(){
	char *input = "\n";
	char* expect_name = " ";
	int expect_type = SPACE;

    struct Token token = {UNKNOWN, {0}};
    int ch;
	
    cl_getc_set_src(input);
    ch = parse_one(EOF, &token);
	
    assert(ch == EOF);
    assert(token.ltype == expect_type);
    assert( *expect_name == token.u.name);
}

static void unit_tests() {
	test_parse_one_empty_should_return_END_OF_FILE();
	test_parse_one_number();
	test_parse_one_executable();
	test_parse_one_literal();
	test_parse_percent();
	test_parse_percent_EOF();
	test_parse_line_feed();
}
#if 0
int main() {
    unit_tests();

    cl_getc_set_src("123 45 add /some { 2 3 add } def");
    parser_print_all();
	return 1;
}
#endif
