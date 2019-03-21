#include "asm.h"

static int list_number;
struct List *unresolved_list;

void list_init() {
	list_number  = 0;
	unresolved_list = malloc(sizeof(struct List));
	unresolved_list->next = NULL;
	unresolved_list->emitter_pos = 0;
	unresolved_list->label = 0;
	unresolved_list->code = 0;
}

void list_put(struct List *add_list) {
	struct List *list = unresolved_list;
	while(list->next != NULL) {
		list = list->next;
	}
	if(list->code == 0) {
		//初回
		list->emitter_pos = add_list->emitter_pos;
		list->label = add_list->label;
		list->code = add_list->code;
	} else {
		//既に値が入っている場合
		struct List *last_list;
		last_list = malloc(sizeof(struct List));
		last_list = add_list;
		last_list->next = NULL;
		list->next = last_list;
	}
}


int list_get(struct List *out_list) {
	struct List *list = unresolved_list;
	int i = 0;
	if(list->code == 0) return 0;
	while(i < list_number) {
		if(list->next == NULL) return 0;
		list = list->next;
		i++;
	}
	*out_list = *list;
	list_number++;
	return 1;
}

static void assert_list(struct List *expect, struct List *actual) {
	assert_number(expect->emitter_pos, actual->emitter_pos);
	assert_number(expect->label, actual->label);
	assert_number(expect->code, actual->code);
}

static void test_two_put_add() {
	list_init();
	struct List input = {NULL, 1, 2, 3};
	struct List input2 = {NULL, 4, 5, 6};
	struct List actual;
	struct List actual2;
	
	list_put(&input);
	list_put(&input2);
	list_get(&actual);
	list_get(&actual2);

	assert_list(&input, &actual);
	assert_list(&input2, &actual2);
}

static void test_three_put_add() {
	list_init();
	struct List input = {NULL, 1, 2, 3};
	struct List input2 = {NULL, 4, 5, 6};
	struct List input3 = {NULL, 7, 8, 9};
	struct List actual;
	struct List actual2;
	struct List actual3;
	
	list_put(&input);
	list_put(&input2);
	list_put(&input3);
	list_get(&actual);
	list_get(&actual2);
	list_get(&actual3);

	assert_list(&input, &actual);
	assert_list(&input2, &actual2);
	assert_list(&input3, &actual3);
}

static void test_fail_get() {
	list_init();
	int expect = 0;
	struct List input;
	
	int actual = list_get(&input);
	assert_number(expect, actual);
	
}

static void unit_tests() {
	list_init();
	test_two_put_add();
	test_three_put_add();
	test_fail_get();
}
#if 0
int main() {
	unit_tests();
}
#endif