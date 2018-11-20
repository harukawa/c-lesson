#include "clesson.h"
#include "assert.h"

int streq(char *s1, char *s2) {
	if(strcmp(s1,s2)==0) {
		return 1;
	}else {
		return 0;
	}
}

static void two_number_pop(int *out_num, int *out_num2) {
	struct Node node = {UNKNOWN,{0}};
	stack_pop(&node);
	*out_num = node.u.number;
	stack_pop(&node);
	*out_num2 = node.u.number;
}

static void def_op() {
	struct Node node = {UNKNOWN,{0}};
	int val;
	char* literal_name;
	stack_pop(&node);
	val = node.u.number;
	stack_pop(&node);
	literal_name = node.u.name;
	node.ntype = NODE_NUMBER;
	node.u.number = val;
	dict_put(literal_name, &node);
}

static void add_op() {
	struct Node node ={NODE_NUMBER,{0}};
	int one,two;
	two_number_pop(&one,&two);
	node.u.number = two + one;
	stack_push(&node);
}

static void sub_op() {
	struct Node node ={NODE_NUMBER,{0}};
	int one,two;
	two_number_pop(&one,&two);
	node.u.number = two - one;
	stack_push(&node);
}

static void mul_op() {
	struct Node node ={NODE_NUMBER,{0}};
	int one,two;
	two_number_pop(&one,&two);
	node.u.number = two * one;
	stack_push(&node);
}

static void div_op() {
	struct Node node ={NODE_NUMBER,{0}};
	int one,two;
	two_number_pop(&one,&two);
	node.u.number = (int)two / one;
	stack_push(&node);
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
					if(dict_get(token.u.name, &node)) {
						if(node.ntype == NODE_C_FUNC) {
							node.u.cfunc();
						} else if(dict_get(token.u.name, &node)) {
							stack_push(&node);
						}
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


static void register_one_primitive(char *input_key, void (*cfunc)(void)) {
	struct Node node ={NODE_C_FUNC, {.cfunc = cfunc}};
	dict_put(input_key, &node);
}

static void register_primitives(){
	register_one_primitive("def", def_op);
	register_one_primitive("add", add_op);
	register_one_primitive("sub", sub_op);
	register_one_primitive("mul", mul_op);
	register_one_primitive("div", div_op);
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

static void test_eval_sub() {
    char *input = "5 2 sub";
    int expect = 3;
    cl_getc_set_src(input);
    eval();

    int actual = 0;
	struct Node actual_data ={UNKNOWN,{0}};
	stack_pop(&actual_data);

	actual = actual_data.u.number;
    assert(expect == actual);
}

static void test_eval_mul() {
    char *input = "1 2 mul";
    int expect = 2;
    cl_getc_set_src(input);
    eval();

    int actual = 0;
	struct Node actual_data ={UNKNOWN,{0}};
	stack_pop(&actual_data);

	actual = actual_data.u.number;
    assert(expect == actual);
}

static void test_eval_div() {
    char *input = "7 2 div";
    int expect = 3;
    cl_getc_set_src(input);
    eval();

    int actual = 0;
	struct Node actual_data ={UNKNOWN,{0}};
	stack_pop(&actual_data);

	actual = actual_data.u.number;
    assert(expect == actual);
}


int main() {
	register_primitives();
	test_eval_num_one();
	test_eval_num_two();
	test_eval_add();
	test_eval_sub();
	test_eval_mul();
	test_eval_div();
	test_def();
	return 1;
}
