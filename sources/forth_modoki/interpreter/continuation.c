#include "clesson.h"
#include <assert.h>

#define STACK_SIZE 1024

static struct Continuation co_stack[STACK_SIZE];
static int co_stack_pos = 0;

static void co_init(){
	co_stack_pos = 0;
}

void co_push(struct Continuation *cont){
	co_stack[co_stack_pos] = *cont;
	co_stack_pos++;
}

int co_pop(struct Continuation *cont){
	co_stack_pos--;
	if(co_stack_pos < 0) {
		co_stack_pos = 0;
		return 0;
	}else {
		*cont = co_stack[co_stack_pos];
		return 1;
	}
}


static void test_one_pop() {
	co_init();
	struct Continuation expect;
	assert( co_pop(&expect) == 0);
}

static void test_one_push_one_pop() {
	co_init();
	struct Continuation input;
	input.pc = 2;
	int expect = 2;
	
	struct Continuation actual;
	co_push(&input);	
	co_pop(&actual);

	assert(expect == actual.pc);
}

static void test_two_push_two_pop() {
	co_init();
	struct Continuation input;
	struct Continuation input2;
	input.pc = 2;
	input2.pc = 3;
	int expect = 2;
	int expect2 = 3;
	
	struct Continuation actual;
	struct Continuation actual2;
	co_push(&input);	
	co_push(&input2);	
	co_pop(&actual2);
	co_pop(&actual);

	assert(expect == actual.pc);
	assert(expect2 == actual2.pc);
}

#if 0
int main() {
	test_one_pop();
	test_one_push_one_pop();
	test_two_push_two_pop();
	return 0;
}
#endif

