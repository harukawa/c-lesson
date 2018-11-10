#include "stack.h"
#include <stdio.h>
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
	
void stack_print_all() {
	struct Node node = {UNKNOWN,{0}};
	while(stack_pop(&node)) {
		switch(node.ntype) {
			case NUMBER:
				printf("num: %d\n", node.u.number);
				break;
			case LITERAL_NAME:
				printf("LITERAL_NAME: %s\n", node.u.name);
				break;
			default:
				printf("Unknown type %d\n", node.ntype);
				break;
		}
	}
}

static void test_one_pop() {
	struct Node expect ={UNKNOWN,{0}};
	assert( stack_pop(&expect) == 0);
}

static void test_one_push_one_pop() {
	struct Node input = {NUMBER, {1}};
	struct Node actual ={UNKNOWN,{0}};
	stack_push(&input);
	stack_pop(&actual);
	assert(actual.ntype == input.ntype);
	assert(actual.u.number == input.u.number);
}

static void test_two_push_two_pop() {
	struct Node input = {NUMBER, {1}};
	struct Node input2 = {LITERAL_NAME, {"hello"}};
	struct Node actual ={UNKNOWN,{0}};
	stack_push(&input);
	stack_push(&input2);
	stack_pop(&actual);
	assert(actual.ntype == input2.ntype);
	assert(actual.u.number == input2.u.name);
	stack_pop(&actual);
	assert(actual.ntype == input.ntype);
	assert(actual.u.number == input.u.number);
}

int main() {
	test_one_pop();
	test_one_push_one_pop();
	test_two_push_two_pop();
	stack_print_all();
	return 0;
}
