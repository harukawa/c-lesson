#include "clesson.h"
#include "assert.h"

#define MAX_NAME_OP_NUMBERS 256


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
	struct Node def_node;
	int val;
	char* literal_name;
	stack_pop(&node);
	def_node = node;
	stack_pop(&node);
	literal_name = node.u.name;
	dict_put(literal_name, &def_node);
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

static void compile_exec_array(struct Node *out_node) {
	int ch = EOF, count = 0;
	struct Token token = {UNKNOWN, {0} };
	struct Node node[MAX_NAME_OP_NUMBERS];

    do {
        ch = parse_one(ch, &token);
        switch(token.ltype) {
        	case NUMBER:
				node[count].ntype = NODE_NUMBER;
				node[count].u.number = token.u.number;
				count++;
                break;
            case SPACE:
                break;
            case OPEN_CURLY:
				compile_exec_array(&node[count]);
				count++;
                break;
            case CLOSE_CURLY:
                break;
            case EXECUTABLE_NAME:
				node[count].ntype = NODE_EXECUTABLE_NAME;
				node[count].u.name = token.u.name;
				count++;
                break;
            case LITERAL_NAME:
				node[count].ntype = NODE_LITERAL_NAME;
				node[count].u.name = token.u.name;
				count++;
                break;
            default:
                break;
        }
	}while(ch != '}');
	struct NodeArray  *node_array = (struct NodeArray*)malloc(sizeof(struct NodeArray) 
										+ sizeof(struct Node) * count);
	node_array->len = count;
	memcpy(node_array->nodes, node, sizeof(struct Node)*count);
	out_node->ntype = NODE_EXECUTABLE_ARRAY;
	out_node->u.byte_codes = node_array;
}

void eval_exec_array(struct NodeArray *byte_codes) {
	struct Node node = {NODE_UNKNOWN, {0} };
	for(int i = 0; i < byte_codes->len; i++) {
		switch(byte_codes->nodes[i].ntype) {
			case NODE_NUMBER:
				stack_push(&byte_codes->nodes[i]);
				break;

            case OPEN_CURLY:
				compile_exec_array(&byte_codes->nodes[i]);
				stack_push(&byte_codes->nodes[i]);
                break;

			case NODE_LITERAL_NAME:
				stack_push(&byte_codes->nodes[i]);
				break;

			case NODE_EXECUTABLE_NAME:
				if(dict_get(byte_codes->nodes[i].u.name, &node)) {
					if(node.ntype == NODE_C_FUNC) {
						node.u.cfunc();
					} else if(node.ntype == NODE_EXECUTABLE_ARRAY) {
						eval_exec_array(node.u.byte_codes);					
					} else {
						stack_push(&node);
					}
				}
				break;
			default:
				break;
		}
	}
}

void eval() {
	int ch = EOF;
	struct Token token = {UNKNOWN, {0} };
	struct Node node = {NODE_UNKNOWN, {0} };

    do {
        ch = parse_one(ch, &token);
        switch(token.ltype) {
        	case NUMBER:
				node.ntype = NODE_NUMBER;
				node.u.number = token.u.number;
				stack_push(&node);
                break;
            case SPACE:
                break;
            case OPEN_CURLY:
				compile_exec_array(&node);
				stack_push(&node);
                break;
            case CLOSE_CURLY:
                break;
            case EXECUTABLE_NAME:
				if(dict_get(token.u.name, &node)) {
					if(node.ntype == NODE_C_FUNC) {
						node.u.cfunc();
					} else if(node.ntype == NODE_EXECUTABLE_ARRAY) {
						eval_exec_array(node.u.byte_codes);					
					} else {
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

static void test_compile_one(){
	char *input = "{ 1 }";
    int expect = 1;
    cl_getc_set_src(input);
    eval();

    int actual = 0;
	struct Node actual_data ={UNKNOWN,{0}};
	struct Node actual_node;
	stack_pop(&actual_data);

	actual_node = actual_data.u.byte_codes->nodes[0];
	assert_type_eq(NODE_EXECUTABLE_ARRAY, &actual_data);
    assert_num_eq(expect, &actual_node);
}
	

static void test_compile_two(){
	char *input = "{ 1 2 }";
    int expect = 1;
	int expect2 = 2;
    cl_getc_set_src(input);
    eval();

    int actual = 0;
	struct Node actual_data ={UNKNOWN,{0}};
	struct Node actual_node;
	struct Node actual_node2;
	stack_pop(&actual_data);

	actual_node = actual_data.u.byte_codes->nodes[0];
	actual_node2 = actual_data.u.byte_codes->nodes[1];
	assert_type_eq(NODE_EXECUTABLE_ARRAY, &actual_data);
    assert_num_eq(expect, &actual_node);
    assert_num_eq(expect2, &actual_node2);
}

static void test_compile_nest(){
	char *input = "{ 1 { 2 } 3 }";
    int expect = 1;
	int expect2 = 2;
	int expect3 = 3;
    cl_getc_set_src(input);
    eval();

    int actual = 0;
	struct Node actual_data ={UNKNOWN,{0}};
	struct Node actual_node;
	struct Node actual_node2;
	struct Node actual_node3;
	stack_pop(&actual_data);

	actual_node = actual_data.u.byte_codes->nodes[0];
	actual_node2 = actual_data.u.byte_codes->nodes[1].u.byte_codes->nodes[0];
	actual_node3 = actual_data.u.byte_codes->nodes[2];
	assert_type_eq(NODE_EXECUTABLE_ARRAY, &actual_data);
    assert_num_eq(expect, &actual_node);
    assert_num_eq(expect2, &actual_node2);
    assert_num_eq(expect3, &actual_node3);
}

static void test_executable_array(){
	char *input = "/abc { 1 2 add } def abc";
    int expect = 3;
    cl_getc_set_src(input);
    eval();

	struct Node actual ={UNKNOWN,{0}};
	stack_pop(&actual);

	assert_type_eq(NODE_NUMBER, &actual);
    assert_num_eq(expect, &actual);
}

static void test_executable_array_nest(){
	char *input = "/ZZ { 6 } def";
	char *input2 = "/YY { 4 ZZ 5 } def";
	char *input3 = "/XX { 1 2 YY 3 } def XX";
    int expect[6] = {3,5,6,4,2,1};
    cl_getc_set_src(input);
    eval();
    cl_getc_set_src(input2);
    eval();
    cl_getc_set_src(input3);
    eval();

	struct Node actual[6];
	int i;
	for(i = 0;i<6;i++) {
		stack_pop(&actual[i]);
	}

	for(i=0;i<6;i++) {
		assert_type_eq(NODE_NUMBER, &actual[i]);
    	assert_num_eq(expect[i], &actual[i]);
	}
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
	test_compile_one();
	test_compile_two();
	test_compile_nest();
	test_executable_array();
	test_executable_array_nest();
	return 0;
}
