#include "clesson.h"
#include "assert.h"

int streq(char *s1, char *s2) {
	if(strcmp(s1,s2)==0) {
		return 1;
	}else {
		return 0;
	}
}


void eval() {
	int ch = EOF;
	struct Token token = {UNKNOWN, {0} };
	struct Node node = {NODE_UNKNOWN, {0} };

    do {
        ch = parse_one(ch, &token);
        if(token.ltype != UNKNOWN) {
            switch(token.ltype) {
                case NUMBER:
					node.ntype = NODE_NUMBER;
					node.u.number = token.u.number;
					stack_push(&node);
                    break;
                case SPACE:
                    break;
                case OPEN_CURLY:
                    break;
                case CLOSE_CURLY:
                    break;
                case EXECUTABLE_NAME:
					if(streq(token.u.name, "add")) {
						int one,two;
						stack_pop(&node);
						one = node.u.number;
						stack_pop(&node);
						two = node.u.number;
						node.u.number = one+two;
						stack_push(&node);

					} else if(streq(token.u.name, "def")) {
						int val;
						char* literal_name;
						stack_pop(&node);
						val = node.u.number;
						stack_pop(&node);
						literal_name = node.u.name;
						node.ntype = NODE_NUMBER;
						node.u.number = val;
						dict_put(literal_name, &node);
					} else if(dict_get(token.u.name, &node)) {
						stack_push(&node);
					}
                    break;
                case LITERAL_NAME:
					node.ntype = NODE_LITERAL_NAME;
					node.u.name = token.u.name;
					stack_push(&node);
                    break;

                default:
                    break;
            }
        }
	}while(ch != EOF);

}

static void test_eval_num_one() {
    char *input = "123";
    int expect = 123;
    cl_getc_set_src(input);
    eval();

    int actual = 0;
	struct Node actual_data ={UNKNOWN,{0}};
	stack_pop(&actual_data);
	actual = actual_data.u.number;

    assert(expect == actual);

}

static void test_eval_num_two() {
    char *input = "123 456";
    int expect1 = 456;
    int expect2 = 123;

    cl_getc_set_src(input);
    eval();
    int actual1 = 0;
    int actual2 = 0;
	struct Node actual_data ={UNKNOWN,{0}};
	stack_pop(&actual_data);
	actual1 = actual_data.u.number;
	stack_pop(&actual_data);
	actual2 = actual_data.u.number;

    assert(expect1 == actual1);
    assert(expect2 == actual2);
}


static void test_eval_add() {
    char *input = "1 2 add";
    int expect = 3;
    cl_getc_set_src(input);
    eval();

    int actual = 0;
	struct Node actual_data ={UNKNOWN,{0}};
	stack_pop(&actual_data);

	actual = actual_data.u.number;
    assert(expect == actual);
}

static void test_def() {
    char *input = "/abc 12 def";
	char *input2 = "abc abc";
    int expect1 = 12;
    int expect2 = 12;
    cl_getc_set_src(input);
	eval();
    cl_getc_set_src(input2);
    eval();

	struct Node actual_data ={UNKNOWN,{0}};
    int actual1 = 0;
    int actual2 = 0;
	stack_pop(&actual_data);
	actual1 = actual_data.u.number;
	stack_pop(&actual_data);
	actual2 = actual_data.u.number;

    assert(expect1 == actual1);
    assert(expect2 == actual2);
	
}
#if 0
int main() {
	test_eval_num_one();
	test_eval_num_two();
	test_eval_add();
	test_def();
	return 1;
}
#endif
