#include "asm.h"

struct Node {
    char *name;
    int value;
    struct Node *left;
    struct Node *right;
};

struct Node mnemonic_root;
struct Node label_root;

int mnemonic_id = 1;
int label_id = 10000;

int search_name_tree(char *str, struct Node *node) {
	int i = strcmp(str, node->name);
	if(i == 0) {
		return node->value;
	} else if(i >= 1){
		if(NULL == node->right) {
			return -1;
		} else {
			return search_name_tree(str, node->right);
		}
	} else if(i <= -1){
		if(NULL == node->left) {
			return -1;
		} else {
			return search_name_tree(str, node->left);
		}
	}
}

int add_name_tree(char *str, struct Node *node, int value) {
	int i = strcmp(str, node->name);
	if(i == 0) {
		return node->value;
	} else if(i >= 1){
		if(NULL == node->right) {
			char *tmp;
			int mem_size = strlen(str) + 1;
			tmp = malloc(mem_size);
			memcpy(tmp, str, mem_size);
			
			struct Node *right_node =malloc(sizeof(struct Node));
			right_node->name = tmp;
			right_node->value = value;
			node->right = right_node;
			return right_node->value;
		} else {
			return add_name_tree(str, node->right, value);
		}
	} else if(i <= -1){
		if(NULL == node->left) {
			char *tmp;
			int mem_size = strlen(str) + 1;
			tmp = malloc(mem_size);
			memcpy(tmp, str, mem_size);
			
			struct Node *left_node = malloc(sizeof(struct Node));
			left_node->name = tmp;
			left_node->value = value;
			node->left = left_node;
			return left_node->value;
		} else {
			return add_name_tree(str, node->left, value);
		}
	}
}

int search_value_tree(int value, struct Node *node, char **out_name) {
	if(value == node->value) {
		*out_name = node->name;
		return 1;
	} else {
		int result = -1;
		char *output = "";
		if(NULL != node->left) {
			result = search_value_tree(value, node->left, &output);
		}
		if(-1 == result && NULL != node->right) {
			result = search_value_tree(value, node->right, &output);
		}
		*out_name = output;
		return result;
	}
}


int to_mnemonic_symbol(char *str) {
	int value;
	
	value = search_name_tree(str, &mnemonic_root);
	if(value != -1) {
		return value;
	} else {
		mnemonic_id++;
		value = add_name_tree(str, &mnemonic_root, mnemonic_id);
		return value;
	}
}

static void init() {
	mnemonic_root.name = "root";
	mnemonic_id = 1;
	mnemonic_root.value = mnemonic_id;
	mnemonic_root.right = NULL;
	mnemonic_root.left  = NULL;
}

void setup_mnemonic() {
	init();
	mnemonics_list[0] = to_mnemonic_symbol(".raw");
	mnemonics_list[1] = to_mnemonic_symbol("mov");
	mnemonics_list[2] = to_mnemonic_symbol("MOV");
	mnemonics_list[3] = to_mnemonic_symbol("ldr");
	mnemonics_list[4] = to_mnemonic_symbol("LDR");
	mnemonics_list[5] = to_mnemonic_symbol("str");
	mnemonics_list[6] = to_mnemonic_symbol("STR");
	
}

static void test_three_mnemonic_search() {
	init();
	int expect_mov = 2;
	int expect_str = 3;
	int expect_ldr = 4;
	int actual_mov, actual_str, actual_ldr;
	
	to_mnemonic_symbol("mov");
	to_mnemonic_symbol("str");
	to_mnemonic_symbol("ldr");
	actual_mov = search_name_tree("mov", &mnemonic_root);
	actual_str = search_name_tree("str", &mnemonic_root);
	actual_ldr = search_name_tree("ldr", &mnemonic_root);
	
	assert_number(expect_mov, actual_mov);
	assert_number(expect_str, actual_str);
	assert_number(expect_ldr, actual_ldr);
}

static void test_three_mnemonic_add() {
	init();
	int expect_mov = 2;
	int expect_str = 3;
	int expect_ldr = 4;
	int actual_mov, actual_str, actual_ldr;
	
	to_mnemonic_symbol("mov");
	to_mnemonic_symbol("str");
	to_mnemonic_symbol("ldr");
	actual_mov = add_name_tree("mov", &mnemonic_root, mnemonic_id);
	actual_str = add_name_tree("str", &mnemonic_root, mnemonic_id);
	actual_ldr = add_name_tree("ldr", &mnemonic_root, mnemonic_id);
	
	assert_number(expect_mov, actual_mov);
	assert_number(expect_str, actual_str);
	assert_number(expect_ldr, actual_ldr);
}

static void test_three_mnemonic_to_symbol() {
	init();
	int expect_mov = 2;
	int expect_str = 3;
	int expect_ldr = 4;
	int actual_mov, actual_str, actual_ldr;
	
	to_mnemonic_symbol("mov");
	to_mnemonic_symbol("str");
	to_mnemonic_symbol("ldr");
	actual_mov = to_mnemonic_symbol("mov");
	actual_str = to_mnemonic_symbol("str");
	actual_ldr = to_mnemonic_symbol("ldr");
	
	assert_number(expect_mov, actual_mov);
	assert_number(expect_str, actual_str);
	assert_number(expect_ldr, actual_ldr);
}

static void test_search_value_tree() {
	init();
	char *expect_mov = "mov";
	char *expect_str = "str";
	char *actual_mov;
	char *actual_str;
	
	int input_mov = to_mnemonic_symbol("mov");
	int input_str = to_mnemonic_symbol("str");
	
	search_value_tree(input_mov,  &mnemonic_root, &actual_mov);
	search_value_tree(input_str,  &mnemonic_root, &actual_str);

	assert_streq(expect_mov, actual_mov);
	assert_streq(expect_str, actual_str);
}

static void test_search_value_tree_fail() {
	init();
	char *input_str;
	int expect = -1;
	char *actual;
	
	to_mnemonic_symbol("mov");
	to_mnemonic_symbol("str");
	
	actual = search_value_tree(-1, &mnemonic_root, &input_str);
	assert_number(expect, actual);
}

static void unit_test() {
	init();
	test_three_mnemonic_search();
	test_three_mnemonic_add();
	test_three_mnemonic_to_symbol();
	test_search_value_tree();
	test_search_value_tree_fail();
}

int main(int argc, char *argv[]) {
	unit_test();
}
