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

int search_value_tree(int value, struct Node *node) {
	/*if(value == node->value) {
		return node->value;
	} else if(value > node->value){
		if(NULL == node->right) {
			return -1;
		} else {
			return search_value_tree(value, node->right);
		}
	} else if(value < node->value){
		if(NULL == node->left) {
			return -1;
		} else {
			return search_value_tree(value, node->left);
		}
	}*/
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
	char *input_mov = "mov";
	char *input_str = "str";
	char *input_ldr = "ldr";
	int expect_mov = 2;
	int expect_str = 3;
	int expect_ldr = 4;
	int actual_mov, actual_str, actual_ldr;
	
	to_mnemonic_symbol(input_mov);
	to_mnemonic_symbol(input_str);
	to_mnemonic_symbol(input_ldr);
	actual_mov = search_name_tree(input_mov, &mnemonic_root);
	actual_str = search_name_tree(input_str, &mnemonic_root);
	actual_ldr = search_name_tree(input_ldr, &mnemonic_root);
	
	assert_number(expect_mov, actual_mov);
	assert_number(expect_str, actual_str);
	assert_number(expect_ldr, actual_ldr);
}

static void test_three_mnemonic_add() {
	init();
	char *input_mov = "mov";
	char *input_str = "str";
	char *input_ldr = "ldr";
	int expect_mov = 2;
	int expect_str = 3;
	int expect_ldr = 4;
	int actual_mov, actual_str, actual_ldr;
	
	to_mnemonic_symbol(input_mov);
	to_mnemonic_symbol(input_str);
	to_mnemonic_symbol(input_ldr);
	actual_mov = add_name_tree(input_mov, &mnemonic_root, mnemonic_id);
	actual_str = add_name_tree(input_str, &mnemonic_root, mnemonic_id);
	actual_ldr = add_name_tree(input_ldr, &mnemonic_root, mnemonic_id);
	
	assert_number(expect_mov, actual_mov);
	assert_number(expect_str, actual_str);
	assert_number(expect_ldr, actual_ldr);
}

static void test_three_mnemonic_to_symbol() {
	init();
	char *input_mov = "mov";
	char *input_str = "str";
	char *input_ldr = "ldr";
	int expect_mov = 2;
	int expect_str = 3;
	int expect_ldr = 4;
	int actual_mov, actual_str, actual_ldr;
	
	to_mnemonic_symbol(input_mov);
	to_mnemonic_symbol(input_str);
	to_mnemonic_symbol(input_ldr);
	actual_mov = to_mnemonic_symbol(input_mov);
	actual_str = to_mnemonic_symbol(input_str);
	actual_ldr = to_mnemonic_symbol(input_ldr);
	
	assert_number(expect_mov, actual_mov);
	assert_number(expect_str, actual_str);
	assert_number(expect_ldr, actual_ldr);
}

static void unit_test() {
	init();
	test_three_mnemonic_search();
	test_three_mnemonic_add();
	test_three_mnemonic_to_symbol();
}
#if 0
int main(int argc, char *argv[]) {
	unit_test();
}
#endif