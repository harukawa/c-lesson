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
					}
                    break;
                case LITERAL_NAME:
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

    /* TODO: write code to pop stack top element */
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

    /* TODO: write code to pop stack top and second top element */
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

    /* TODO: write code to pop stack top element */
    int actual = 0;
	struct Node actual_data ={UNKNOWN,{0}};
	stack_pop(&actual_data);
	actual = actual_data.u.number;
    assert(expect == actual);
}


int main() {
	test_eval_num_one();
	test_eval_num_two();
	test_eval_add();

	return 1;
}
