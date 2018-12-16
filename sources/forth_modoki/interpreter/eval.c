#include "clesson.h"
#include "assert.h"

#define MAX_NAME_OP_NUMBERS 256

struct Emitter {
	struct Node *nodes;
	int pos;
};

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
	stack_pop(&def_node);
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

static void eq_op() {
	struct Node node ={NODE_NUMBER,{0}};
	int one,two;
	two_number_pop(&one,&two);
	if(one == two) {
		node.u.number = 1;
	}
	stack_push(&node);
}

static void neq_op() {
	struct Node node ={NODE_NUMBER,{0}};
	int one,two;
	two_number_pop(&one,&two);
	if(one != two) {
		node.u.number = 1;
	}
	stack_push(&node);
}

static void gt_op() {
	struct Node node ={NODE_NUMBER,{0}};
	int one,two;
	two_number_pop(&one,&two);
	if(two > one) {
		node.u.number = 1;
	}
	stack_push(&node);
}

static void ge_op() {
	struct Node node ={NODE_NUMBER,{0}};
	int one,two;
	two_number_pop(&one,&two);
	if(two >= one) {
		node.u.number = 1;
	}
	stack_push(&node);
}

static void lt_op() {
	struct Node node ={NODE_NUMBER,{0}};
	int one,two;
	two_number_pop(&one,&two);
	if(two < one) {
		node.u.number = 1;
	}
	stack_push(&node);
}

static void le_op() {
	struct Node node ={NODE_NUMBER,{0}};
	int one,two;
	two_number_pop(&one,&two);
	if(two <= one) {
		node.u.number = 1;
	}
	stack_push(&node);
}

static void pop_op() {
	struct Node node ={NODE_NUMBER,{0}};
	stack_pop(&node);
}

static void exch_op() {
	struct Node node;
	struct Node node2;
	stack_pop(&node);
	stack_pop(&node2);
	stack_push(&node);
	stack_push(&node2);
}

static void dup_op() {
	struct Node node;
	stack_pop(&node);
	stack_push(&node);
	stack_push(&node);
}

static void index_op() {
	struct Node node;
	stack_pop(&node);
	stack_check(&node, node.u.number);
	stack_push(&node);
}

static void roll_op() {
	struct Node node;
	int i,j,n;
	two_number_pop(&j,&n);
	struct Node nodes[n];
	for(i = 0;i<n;i++) {
		stack_pop(&nodes[i]);
	}
	if(n < j) {
		j = j % n;
	}
	i = 0;
	int index = n - 1 - (n -j);
	while(i<n) {
		stack_push(&nodes[index]);
		i++;
		index--;
		if(index < 0) {
			index = n -1;
		}
	}
}

static void exec_op() {
	struct Node proc;
	stack_pop(&proc);
	eval_exec_array(proc.u.byte_codes);
}

static void if_op() {
	struct Node bool1;
	struct Node proc1;
	stack_pop(&proc1);
	stack_pop(&bool1);
	if(bool1.u.number) {
		eval_exec_array(proc1.u.byte_codes);
	}
}
 
static void ifelse_op() {
	struct Node bool1;
	struct Node proc1;
	struct Node proc2;
	stack_pop(&proc2);
	stack_pop(&proc1);
	stack_pop(&bool1);
	if(bool1.u.number) {
		eval_exec_array(proc1.u.byte_codes);
	} else {
		eval_exec_array(proc2.u.byte_codes);
	}
}

static void repeat_op() {
	struct Node proc;
	struct Node num;
	stack_pop(&proc);
	stack_pop(&num);
	int n = num.u.name;
	for(int i = 0; i<n; i++) {
		eval_exec_array(proc.u.byte_codes);
	}
}

static void while_op() {
	struct Node proc1;
	struct Node proc2;
	struct Node node;
	stack_pop(&proc2);
	stack_pop(&proc1);
	while(1) {
		eval_exec_array(proc1.u.byte_codes);
		stack_pop(&node);
		if(node.u.number) {
			eval_exec_array(proc2.u.byte_codes);
		} else {
			break;
		}
	}
}

static void emit_elem(struct Emitter *emit, struct Node *node) {
	emit->nodes[emit->pos].ntype = node->ntype;
	if(node->ntype == EXECUTABLE_NAME) {
		emit->nodes[emit->pos].u.name = node->u.name;
	} else {
		emit->nodes[emit->pos].u.number = node->u.number;
	}
	emit->pos++;
}

static int ifelse_compile(struct Emitter *emit) {
	struct Node number, name,exec;
	number.ntype = NODE_NUMBER;
	name.ntype = NODE_EXECUTABLE_NAME;
	exec.ntype = NODE_EXEC_PRIMITIVE;


	number.u.number = 3;
	emit_elem(emit, &number); // 3
	number.u.number = 2;
	emit_elem(emit, &number); // 2
	name.u.name = "roll";
	emit_elem(emit, &name); // roll
	number.u.number = 5;
	emit_elem(emit, &number); // 5
	exec.u.number = OP_JMP_NOT_IF;
	emit_elem(emit, &exec); // jmp_not_if
	name.u.name = "pop";
	emit_elem(emit, &name); // pop
	exec.u.number = OP_EXEC;
	emit_elem(emit, &exec); // exec
	number.u.number = 4;
	emit_elem(emit, &number); // 4
	exec.u.number = OP_JMP;
	emit_elem(emit, &exec); // jmp
	name.u.name = "exch";
	emit_elem(emit, &name); // exch
	name.u.name = "pop";
	emit_elem(emit, &name); // pop
	exec.u.number = OP_EXEC;
	emit_elem(emit, &exec); // exec

	return emit->pos;
}

static void compile_exec_array(struct Node *out_node, int prev_ch) {
	int ch = prev_ch, count = 0;
	struct Token token = {UNKNOWN, {0} };
	struct Node node[MAX_NAME_OP_NUMBERS];
	struct Node dict;
    do {
        ch = parse_one(ch, &token);
        switch(token.ltype) {
        	case NUMBER:
				node[count].ntype = NODE_NUMBER;
				node[count].u.number = token.u.number;
				count++;
                break;
            case OPEN_CURLY:
				compile_exec_array(&node[count], ch);
				count++;
				ch = parse_one('}', &token);
                break;
            case LITERAL_NAME:
				node[count].ntype = NODE_LITERAL_NAME;
				node[count].u.name = token.u.name;
				count++;
                break;
            case EXECUTABLE_NAME:
				if(compile_dict_get(token.u.name, &dict)) {
					if(dict.ntype == NODE_C_FUNC) {
						struct Emitter emit;
						emit.pos = count;
						emit.nodes = node;
						count = dict.u.compile_func(&emit);
					} else {
						node[count].ntype = dict.ntype; // exec,jmp,jmp_not_if
						node[count].u.number = dict.u.number;
						count++;
					}
				} else {
					node[count].ntype = NODE_EXECUTABLE_NAME;
					node[count].u.name = token.u.name;
					count++;
				}
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
	struct Node node;
	struct Continuation cont;
	cont.exec_array = byte_codes;
	cont.pc = 0;
	int i;
	
	co_push(&cont);

	while(co_pop(&cont)) {
		for(i = cont.pc; i < cont.exec_array->len; i++){
			switch(cont.exec_array->nodes[i].ntype) {
				case NODE_NUMBER:
					stack_push(&cont.exec_array->nodes[i]);
					break;

				case NODE_LITERAL_NAME:
					stack_push(&cont.exec_array->nodes[i]);
					break;

				case NODE_EXEC_PRIMITIVE:
					if(cont.exec_array->nodes[i].u.number == OP_JMP) {
						struct Node num;
						stack_pop(&num);
						i += num.u.number;
						if(i < 0){ i = 0;}
					}else if(cont.exec_array->nodes[i].u.number == OP_JMP_NOT_IF) {
						struct Node num, num2;
						stack_pop(&num);
						stack_pop(&num2);
						if(num2.u.number == 0){
							i += num.u.number;
							if(i < 0){ i = 0;}
						}
					} else if(cont.exec_array->nodes[i].u.number == OP_EXEC) {
						struct Node exec;
						stack_pop(&exec);
						cont.pc = i + 1;
						i = cont.exec_array->len;
						co_push(&cont);
						cont.exec_array = exec.u.byte_codes;
						cont.pc = 0;
						co_push(&cont);
					}
					break;

				case NODE_EXECUTABLE_NAME:
					if(dict_get(cont.exec_array->nodes[i].u.name, &node)) {
						if(node.ntype == NODE_C_FUNC) {
							node.u.cfunc();
						} else if(node.ntype == NODE_EXECUTABLE_ARRAY) {
							cont.pc = ++i;
							i = cont.exec_array->len;
							co_push(&cont);
							cont.exec_array = node.u.byte_codes;
							cont.pc = 0;
							co_push(&cont);
						} else {
							stack_push(&node);
						}
					} else {
							stack_push(&cont.exec_array->nodes[i]);
					}
					break;

				case NODE_EXECUTABLE_ARRAY:
					stack_push(&cont.exec_array->nodes[i]);
					break;
				default:
					break;
			}
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
				compile_exec_array(&node, ch);
				stack_push(&node);
				ch = '}';
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

static void register_one_compile_primitive(char *input_key, int (*compile_func)(struct Emitter *emit) ) {
	struct Node node ={NODE_C_FUNC, {.compile_func = compile_func}};
	compile_dict_put(input_key, &node);
}

static void register_one_compile_primitive_word(char *input_key, enum ExecWord word) {
	struct Node node = { NODE_EXEC_PRIMITIVE, { word } };
	compile_dict_put(input_key, &node); 
}

void register_primitives(){
	register_one_primitive("def", def_op);
	register_one_primitive("add", add_op);
	register_one_primitive("sub", sub_op);
	register_one_primitive("mul", mul_op);
	register_one_primitive("div", div_op);
	register_one_primitive("eq", eq_op);
	register_one_primitive("neq", neq_op);
	register_one_primitive("gt", gt_op);
	register_one_primitive("ge", ge_op);
	register_one_primitive("lt", lt_op);
	register_one_primitive("le", le_op);
	register_one_primitive("pop", pop_op);
	register_one_primitive("exch", exch_op);
	register_one_primitive("dup", dup_op);
	register_one_primitive("index", index_op);
	register_one_primitive("roll", roll_op);
	register_one_primitive("exec", exec_op);
	register_one_primitive("if", if_op);
	register_one_primitive("ifelse", ifelse_op);
	register_one_primitive("repeat", repeat_op);
	register_one_primitive("while", while_op);

	register_one_compile_primitive("ifelse", ifelse_compile);

	register_one_compile_primitive_word("exec", OP_EXEC);
	register_one_compile_primitive_word("jmp", OP_JMP);
	register_one_compile_primitive_word("jmp_not_if", OP_JMP_NOT_IF);
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


static void test_eq(){
	char *input = "1 1 eq";
    int expect = 1;
    cl_getc_set_src(input);
    eval();

	struct Node actual ={UNKNOWN,{0}};
	stack_pop(&actual);

	assert_type_eq(NODE_NUMBER, &actual);
    assert_num_eq(expect, &actual);
}

static void test_neq(){
	char *input = "2 1 neq";
    int expect = 1;
    cl_getc_set_src(input);
    eval();

	struct Node actual ={UNKNOWN,{0}};
	stack_pop(&actual);

	assert_type_eq(NODE_NUMBER, &actual);
    assert_num_eq(expect, &actual);
}

static void test_gt(){
	char *input = "2 1 gt";
    int expect = 1;
    cl_getc_set_src(input);
    eval();

	struct Node actual ={UNKNOWN,{0}};
	stack_pop(&actual);

	assert_type_eq(NODE_NUMBER, &actual);
    assert_num_eq(expect, &actual);
}

static void test_ge(){
	char *input = "2 1 ge";
    int expect = 1;
    cl_getc_set_src(input);
    eval();

	struct Node actual ={UNKNOWN,{0}};
	stack_pop(&actual);

	assert_type_eq(NODE_NUMBER, &actual);
    assert_num_eq(expect, &actual);
}

static void test_lt(){
	char *input = "1 2 lt";
    int expect = 1;
    cl_getc_set_src(input);
    eval();

	struct Node actual ={UNKNOWN,{0}};
	stack_pop(&actual);

	assert_type_eq(NODE_NUMBER, &actual);
    assert_num_eq(expect, &actual);
}

static void test_le(){
	char *input = "1 2 le";
    int expect = 1;
    cl_getc_set_src(input);
    eval();

	struct Node actual ={UNKNOWN,{0}};
	stack_pop(&actual);

	assert_type_eq(NODE_NUMBER, &actual);
    assert_num_eq(expect, &actual);
}


static void test_pop(){
	char *input = "1 2 pop";
    int expect = 1;
    cl_getc_set_src(input);
    eval();

	struct Node actual ={UNKNOWN,{0}};
	stack_pop(&actual);

	assert_type_eq(NODE_NUMBER, &actual);
    assert_num_eq(expect, &actual);
}

static void test_exch(){
	char *input = "2 1 exch";
    int expect = 2;
	int expect2 = 1;
    cl_getc_set_src(input);
    eval();

	struct Node actual;
	struct Node actual2;
	stack_pop(&actual);
	stack_pop(&actual2);

	assert_type_eq(NODE_NUMBER, &actual);
	assert_type_eq(NODE_NUMBER, &actual2);
    assert_num_eq(expect, &actual);
    assert_num_eq(expect2, &actual2);
}

static void test_dup(){
	char *input = "1 4 dup";
    int expect = 4;
	int expect2 = 4;
    cl_getc_set_src(input);
    eval();

	struct Node actual;
	struct Node actual2;
	stack_pop(&actual);
	stack_pop(&actual2);

	assert_type_eq(NODE_NUMBER, &actual);
	assert_type_eq(NODE_NUMBER, &actual2);
    assert_num_eq(expect, &actual);
    assert_num_eq(expect2, &actual2);
}

static void test_index(){
	char *input = "3 2 2 index";
    int expect = 3;
	int expect2 = 2;
    int expect3 = 3;
    cl_getc_set_src(input);
    eval();

	struct Node actual;
	struct Node actual2;
	struct Node actual3;
	stack_pop(&actual);
	stack_pop(&actual2);
	stack_pop(&actual3);

	assert_type_eq(NODE_NUMBER, &actual);
	assert_type_eq(NODE_NUMBER, &actual2);
	assert_type_eq(NODE_NUMBER, &actual3);
    assert_num_eq(expect, &actual);
    assert_num_eq(expect2, &actual2);
    assert_num_eq(expect3, &actual3);
}

static void test_roll(){
	char *input = "1 2 3 4 5 6 7 4 3 roll";
    int expect[7] = {4,7,6,5,3,2,1};
    cl_getc_set_src(input);
    eval();

	struct Node actual[7];
	int i;
	for(i =0 ; i<7;i++){
		stack_pop(&actual[i]);
	}

	for(i=0; i<7;i++) {
		assert_type_eq(NODE_NUMBER, &actual[i]);
    	assert_num_eq(expect[i], &actual[i]);
	}
}

static void test_exec(){
	char *input = "{ 1 } exec";
    int expect = 1;
    cl_getc_set_src(input);
    eval();

	struct Node actual;
	stack_pop(&actual);

	assert_type_eq(NODE_NUMBER, &actual);
    assert_num_eq(expect, &actual);
}

static void test_if(){
	char *input = "1 { 1 2 add } if";
    int expect = 3;
    cl_getc_set_src(input);
    eval();

	struct Node actual;
	stack_pop(&actual);

	assert_type_eq(NODE_NUMBER, &actual);
    assert_num_eq(expect, &actual);
}

static void test_ifelse(){
	char *input = "0 { 1 2 add } { 2 2 add} ifelse";
    int expect = 4;
    cl_getc_set_src(input);
    eval();

	struct Node actual;
	stack_pop(&actual);

	assert_type_eq(NODE_NUMBER, &actual);
    assert_num_eq(expect, &actual);
}

static void test_repeat(){
	char *input = "1 3 { 2 add } repeat";
    int expect = 7;
    cl_getc_set_src(input);
    eval();

	struct Node actual;
	stack_pop(&actual);

	assert_type_eq(NODE_NUMBER, &actual);
    assert_num_eq(expect, &actual);
}

static void test_while(){
	char *input = "5  { dup 10 lt } { 3 add } while";
    int expect = 11;
    cl_getc_set_src(input);
    eval();

	struct Node actual;
	stack_pop(&actual);
	assert_type_eq(NODE_NUMBER, &actual);
    assert_num_eq(expect, &actual);
}

static void test_while_curly(){
	char *input = "5{dup 10 lt}{3 add}while";
    int expect = 11;
    cl_getc_set_src(input);
    eval();

	struct Node actual;
	stack_pop(&actual);
	assert_type_eq(NODE_NUMBER, &actual);
    assert_num_eq(expect, &actual);
}

static void test_repeat2(){
	char *input = "{3 {123} repeat} exec";
    int expect = 123;
    cl_getc_set_src(input);
    eval();

	struct Node actual[3];
	stack_pop(&actual[0]);
	stack_pop(&actual[1]);
	stack_pop(&actual[2]);

	assert_type_eq(NODE_NUMBER, &actual[0]);
    assert_num_eq(expect, &actual[0]);
	assert_type_eq(NODE_NUMBER, &actual[1]);
    assert_num_eq(expect, &actual[1]);
	assert_type_eq(NODE_NUMBER, &actual[2]);
    assert_num_eq(expect, &actual[2]);
}

static void test_ifelse2(){
	char *input = "1 {2} {3} ifelse 4";
    int expect = 4, expect2 = 2;
    cl_getc_set_src(input);
    eval();

	struct Node actual;
	struct Node actual2;
	stack_pop(&actual);
	stack_pop(&actual2);

	assert_type_eq(NODE_NUMBER, &actual);
    assert_num_eq(expect, &actual);
	assert_type_eq(NODE_NUMBER, &actual2);
    assert_num_eq(expect2, &actual2);
}

static void test_ifelse3(){
	char *input = "/a { {345} ifelse} def 1 {123} a";
    int expect = 123;
    cl_getc_set_src(input);
    eval();

	struct Node actual;
	stack_pop(&actual);

	assert_type_eq(NODE_NUMBER, &actual);
    assert_num_eq(expect, &actual);
}

static void test_def2(){
	char *input = "/f { {1 3 add} exec 3} def f";
    int expect = 4, expect2 = 3;
    cl_getc_set_src(input);
    eval();

	struct Node actual;
	struct Node actual2;
	stack_pop(&actual2);
	stack_pop(&actual);

	assert_type_eq(NODE_NUMBER, &actual);
    assert_num_eq(expect, &actual);
	assert_type_eq(NODE_NUMBER, &actual2);
    assert_num_eq(expect2, &actual2);
}

static void test_cont_exec(){
	char *input = "{ 11 { { 3 } exec } exec } exec";
    int expect = 11, expect2 = 3;
    cl_getc_set_src(input);
    eval();
	struct Node actual;
	struct Node actual2;
	stack_pop(&actual2);
	stack_pop(&actual);

	assert_type_eq(NODE_NUMBER, &actual);
    assert_num_eq(expect, &actual);
	assert_type_eq(NODE_NUMBER, &actual2);
    assert_num_eq(expect2, &actual2);
}

static void test_cont_ifelse(){
	char *input = "{ 1 { 1 3 } { 5 6 } ifelse } exec";
    int expect = 1, expect2 = 3;
    cl_getc_set_src(input);
    eval();

	struct Node actual;
	struct Node actual2;
	stack_pop(&actual2);
	stack_pop(&actual);

	assert_type_eq(NODE_NUMBER, &actual);
    assert_num_eq(expect, &actual);
	assert_type_eq(NODE_NUMBER, &actual2);
    assert_num_eq(expect2, &actual2);
}

static void test_cont_jmp(){
	char *input = "{3 1 jmp 2 4 } exec";
    int expect = 3, expect2 = 4;
    cl_getc_set_src(input);
    eval();

	struct Node actual;
	struct Node actual2;
	stack_pop(&actual2);
	stack_pop(&actual);

	assert_type_eq(NODE_NUMBER, &actual);
    assert_num_eq(expect, &actual);
	assert_type_eq(NODE_NUMBER, &actual2);
    assert_num_eq(expect2, &actual2);
}

static void test_cont_jmp_not_if(){
	char *input = "{3 0 1 jmp_not_if 2 4 } exec";
    int expect = 3, expect2 = 4;
    cl_getc_set_src(input);
    eval();

	struct Node actual;
	struct Node actual2;
	stack_pop(&actual2);
	stack_pop(&actual);

	assert_type_eq(NODE_NUMBER, &actual);
    assert_num_eq(expect, &actual);
	assert_type_eq(NODE_NUMBER, &actual2);
    assert_num_eq(expect2, &actual2);
}

void unit_tests(){		
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
	test_eq();
	test_neq();
	test_gt();
	test_ge();
	test_lt();
	test_le();
	test_pop();
	test_exch();
	test_dup();
	test_index();
	test_roll();
	test_exec();
	test_if();
	test_ifelse();
	test_repeat();
	test_while();
	test_while_curly();
	test_repeat2();
	test_ifelse2();
	test_ifelse3();
	test_def2();
	test_cont_exec();
	test_cont_ifelse();
	test_cont_jmp();
	test_cont_jmp_not_if();
 }

static void test_file(FILE* input_fp){
    int expect = 11;
    cl_getc_set_fp(input_fp);
    eval();

	struct Node actual;
	stack_pop(&actual);
	assert_type_eq(NODE_NUMBER, &actual);
    assert_num_eq(expect, &actual);
}

int main(int argc, char *argv[]) {
	FILE *fp = NULL;
	if(argc > 1){
		char *file_name = argv[1];
		if( (fp = fopen(file_name, "r") ) == NULL){
			printf("ERROR\n");
		}
	}
	register_primitives();
	unit_tests();
	if(fp !=NULL){
		test_file(fp);
		fclose(fp);
	}
	return 0;
}

