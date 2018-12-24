#include "clesson.h"

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


static void emit_number(struct Emitter *emit, int num) {
	emit->nodes[emit->pos].ntype = NODE_NUMBER;
	emit->nodes[emit->pos].u.number = num;
	emit->pos++;
}

static void emit_name(struct Emitter *emit, char *executable_name) {
	emit->nodes[emit->pos].ntype = NODE_EXECUTABLE_NAME;
	emit->nodes[emit->pos].u.name = executable_name;
	emit->pos++;
}

static void emit_primitive(struct Emitter *emit, int primitive_name) {
	emit->nodes[emit->pos].ntype = NODE_EXEC_PRIMITIVE;
	emit->nodes[emit->pos].u.number = primitive_name;
	emit->pos++;
}

static int ifelse_compile(struct Emitter *emit) {
	emit_primitive(emit, OP_STORE); // body 2
	emit_primitive(emit, OP_STORE); // body 1
	emit_number(emit, 5);
	emit_primitive(emit, OP_JMP_NOT_IF);
	emit_number(emit, 1);
	emit_primitive(emit, OP_LOAD);
	emit_primitive(emit, OP_EXEC);
	emit_number(emit, 4);
	emit_primitive(emit, OP_JMP);
	emit_number(emit, 2);
	emit_primitive(emit, OP_LOAD);
	emit_primitive(emit, OP_EXEC);
	
	return emit->pos;
}

static int while_compile(struct Emitter *emit) {
	emit_primitive(emit, OP_STORE); // body 2
	emit_primitive(emit, OP_STORE); // cond 1
	emit_number(emit, 1);
	emit_primitive(emit, OP_LOAD);
	emit_primitive(emit, OP_EXEC);
	emit_number(emit, 6);
	emit_primitive(emit, OP_JMP_NOT_IF);
	emit_number(emit, 2);
	emit_primitive(emit, OP_LOAD);
	emit_primitive(emit, OP_EXEC);
	emit_number(emit, -10);
	emit_primitive(emit, OP_JMP);

	return emit->pos;
}

static int repeat_compile(struct Emitter *emit) {
	emit_primitive(emit, OP_STORE); // body 2
	emit_primitive(emit, OP_STORE); // cond 1
	emit_number(emit, 1);
	emit_primitive(emit, OP_LOAD);
	emit_number(emit, 10);
	emit_primitive(emit, OP_JMP_NOT_IF);
	emit_number(emit, 2);
	emit_primitive(emit, OP_LOAD);
	emit_primitive(emit, OP_EXEC);
	emit_number(emit, 1);
	emit_primitive(emit, OP_LOAD);
	emit_primitive(emit, OP_LPOP); 
	emit_number(emit, 1);
	emit_name(emit, "sub");
	emit_number(emit, -15);
	emit_primitive(emit, OP_JMP);

	return emit->pos;
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
	register_one_compile_primitive("while", while_compile);
	register_one_compile_primitive("repeat", repeat_compile);

	register_one_compile_primitive_word("exec", OP_EXEC);
	register_one_compile_primitive_word("jmp", OP_JMP);
	register_one_compile_primitive_word("jmp_not_if", OP_JMP_NOT_IF);
	register_one_compile_primitive_word("store", OP_STORE);
	register_one_compile_primitive_word("load", OP_LOAD);
	register_one_compile_primitive_word("lpop", OP_LPOP);
}


