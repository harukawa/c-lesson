#include "clesson.h"
#include <assert.h>

#define STACK_SIZE 1024

static struct Node stack[STACK_SIZE];
static int pos = 0;

void stack_init(){
	pos = 0;
}

void stack_push(struct Node *node){
	stack[pos] = *node;
	pos++;
}

int stack_pop(struct Node *out_node) {
	pos--;
	if(pos < 0) {
		pos = 0;
		return 0;
	}else {
		*out_node = stack[pos];
		return 1;
	}
}

void stack_check(struct Node *out_node, int n) {
	struct Node pop_nodes[n];
	int i;
	for(i = 0; i<n; i++) {
		stack_pop(&pop_nodes[i]);
	}
	*out_node = pop_nodes[n-1];
	for(i = n-1; i >= 0; i--){
		stack_push(&pop_nodes[i]);
	}
}
	
void stack_print_all() {
	struct Node node = {UNKNOWN,{0}};
	while(stack_pop(&node)) {
		switch(node.ntype) {
			case NODE_NUMBER:
				printf("num: %d\n", node.u.number);
				break;
			case NODE_LITERAL_NAME:
				printf("LITERAL_NAME: %s\n", node.u.name);
				break;
			case NODE_EXECUTABLE_NAME:
				printf("EXECUTABLE_NAME: %s\n", node.u.name);
				break;
			case NODE_EXECUTABLE_ARRAY:
				printf("EXECUTABLE_ARRAY: %s\n", node.u.name);
				break;
			case NODE_C_FUNC:
				printf("C_FUNC: %s\n", node.u.name);
				break;
			default:
				printf("Unknown type %d\n", node.ntype);
				break;
		}
	}
}

static void test_one_pop() {
	struct Node expect ={NODE_UNKNOWN,{0}};
	assert( stack_pop(&expect) == 0);
}

static void test_one_push_one_pop() {
	struct Node input = {NODE_NUMBER, {1}};
	struct Node actual ={NODE_UNKNOWN,{0}};
	stack_push(&input);
	stack_pop(&actual);
	assert(actual.ntype == input.ntype);
	assert(actual.u.number == input.u.number);
}

static void test_two_push_two_pop() {
	struct Node input = {NODE_NUMBER, {1}};
	struct Node input2 = {NODE_LITERAL_NAME, {"hello"}};
	struct Node actual ={NODE_UNKNOWN,{0}};
	stack_push(&input);
	stack_push(&input2);
	stack_pop(&actual);
	assert(actual.ntype == input2.ntype);
	assert(actual.u.name == input2.u.name);
	stack_pop(&actual);
	assert(actual.ntype == input.ntype);
	assert(actual.u.number == input.u.number);
}

#if 0
int main() {
	test_one_pop();
	test_one_push_one_pop();
	test_two_push_two_pop();
	stack_print_all();
	return 0;
}
#endif

