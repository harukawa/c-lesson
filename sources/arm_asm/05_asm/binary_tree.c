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

int search_name_tree(char *str, int len, struct Node *node) {
	int i = strncmp(str, node->name, len);
	if(i == 0) {
		return node->value;
	} else if(i >= 1){
		if(NULL == node->right) {
			return -1;
		} else {
			return search_name_tree(str, len, node->right);
		}
	} else if(i <= -1){
		if(NULL == node->left) {
			return -1;
		} else {
			return search_name_tree(str, len, node->left);
		}
	}
}

int add_name_tree(char *str, int len, struct Node *node, int value) {
	int i = strncmp(str, node->name, len);
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
			right_node->left = NULL;
			right_node->right = NULL;
			node->right = right_node;
			return right_node->value;
		} else {
			return add_name_tree(str, len, node->right, value);
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
			left_node->left = NULL;
			left_node->right = NULL;
			node->left = left_node;
			return left_node->value;
		} else {
			return add_name_tree(str, len, node->left, value);
		}
	}
}

int search_value_tree(int value, struct Node *node, char **out_name) {
	if(value == node->value) {
		*out_name = node->name;
		return 1;
	} else {
		int result = -1;
		char *output;
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


int to_mnemonic_symbol(char *str, int len) {
	int value;
	value = search_name_tree(str, len, &mnemonic_root);
	if(value != -1) {
		return value;
	} else {
		mnemonic_id++;
		value = add_name_tree(str, len, &mnemonic_root, mnemonic_id);
		return value;
	}
}

int to_label_symbol(char *str, int len) {
	int value;
	value = search_name_tree(str, len, &label_root);
	if(value != -1) {
		return value;
	} else {
		label_id++;
		value = add_name_tree(str, len, &label_root,label_id);
		return value;
	}
}

static void binary_init() {
	mnemonic_root.name = "root";
	mnemonic_id = 1;
	mnemonic_root.value = mnemonic_id;
	mnemonic_root.right = NULL;
	mnemonic_root.left  = NULL;
	
	label_root.name = "root";
	label_id = 1;
	label_root.value = label_id;
	label_root.right = NULL;
	label_root.left  = NULL;
}

void setup_mnemonic() {
	binary_init();
	g_raw = to_mnemonic_symbol(".raw", 4);
	g_mov = to_mnemonic_symbol("mov", 3);
	g_MOV = to_mnemonic_symbol("MOV", 3);
	g_ldr = to_mnemonic_symbol("ldr", 3);
	g_LDR = to_mnemonic_symbol("LDR", 3);
	g_str = to_mnemonic_symbol("str", 3);
	g_STR = to_mnemonic_symbol("STR", 3);
	g_b = to_mnemonic_symbol("b", 1);
	g_B = to_mnemonic_symbol("B", 1);
	g_ldrb = to_mnemonic_symbol("ldrb", 4);
	g_LDRB = to_mnemonic_symbol("LDRB", 4);
	g_add = to_mnemonic_symbol("add", 3);
	g_ADD = to_mnemonic_symbol("ADD", 3);
	g_cmp = to_mnemonic_symbol("cmp", 3);
	g_CMP = to_mnemonic_symbol("CMP", 3);
	g_bne = to_mnemonic_symbol("bne", 3);
	g_BNE = to_mnemonic_symbol("BNE", 3);
	g_bl = to_mnemonic_symbol("bl", 2);
	g_BL = to_mnemonic_symbol("BL", 2);
	g_blt = to_mnemonic_symbol("blt", 3);
	g_BLT = to_mnemonic_symbol("BLT", 3);
	g_ldmia = to_mnemonic_symbol("ldmia", 5);
	g_LDMIA = to_mnemonic_symbol("LDMIA", 5);
	g_stmdb = to_mnemonic_symbol("stmdb", 5);
	g_STMDB = to_mnemonic_symbol("STMDB", 5);
	g_lsr = to_mnemonic_symbol("lsr", 3);
	g_LSR = to_mnemonic_symbol("LSR", 3);
	g_and = to_mnemonic_symbol("and", 3);
	g_AND = to_mnemonic_symbol("AND", 3);
	g_sub = to_mnemonic_symbol("sub", 3);
	g_SUB = to_mnemonic_symbol("SUB", 3);
	g_bge = to_mnemonic_symbol("bge", 3);
	g_BGE = to_mnemonic_symbol("BGE", 3);
}

static void test_three_mnemonic_search() {
	binary_init();
	int expect_mov = 2;
	int expect_str = 3;
	int expect_ldr = 4;
	int actual_mov, actual_str, actual_ldr;
	
	to_mnemonic_symbol("mov   ", 3);
	to_mnemonic_symbol("str   ", 3);
	to_mnemonic_symbol("ldr   ", 3);
	actual_mov = search_name_tree("mov", 3, &mnemonic_root);
	actual_str = search_name_tree("str", 3, &mnemonic_root);
	actual_ldr = search_name_tree("ldr", 3, &mnemonic_root);
	
	assert_number(expect_mov, actual_mov);
	assert_number(expect_str, actual_str);
	assert_number(expect_ldr, actual_ldr);
}

static void test_three_mnemonic_add() {
	binary_init();
	int expect_mov = 2;
	int expect_str = 3;
	int expect_ldr = 4;
	int actual_mov, actual_str, actual_ldr;
	
	to_mnemonic_symbol("mov", 3);
	to_mnemonic_symbol("str", 3);
	to_mnemonic_symbol("ldr", 3);
	actual_mov = add_name_tree("mov", 3, &mnemonic_root, mnemonic_id);
	actual_str = add_name_tree("str", 3, &mnemonic_root, mnemonic_id);
	actual_ldr = add_name_tree("ldr", 3, &mnemonic_root, mnemonic_id);
	
	assert_number(expect_mov, actual_mov);
	assert_number(expect_str, actual_str);
	assert_number(expect_ldr, actual_ldr);
}

static void test_three_mnemonic_to_symbol() {
	binary_init();
	int expect_mov = 2;
	int expect_str = 3;
	int expect_ldr = 4;
	int actual_mov, actual_str, actual_ldr;
	
	to_mnemonic_symbol("mov", 3);
	to_mnemonic_symbol("str", 3);
	to_mnemonic_symbol("ldr", 3);
	actual_mov = to_mnemonic_symbol("mov", 3);
	actual_str = to_mnemonic_symbol("str", 3);
	actual_ldr = to_mnemonic_symbol("ldr", 3);
	
	assert_number(expect_mov, actual_mov);
	assert_number(expect_str, actual_str);
	assert_number(expect_ldr, actual_ldr);
}

static void test_search_value_tree() {
	binary_init();
	char *expect_mov = "mov";
	char *expect_str = "str";
	char *actual_mov;
	char *actual_str;
	
	int input_mov = to_mnemonic_symbol("mov", 3);
	int input_str = to_mnemonic_symbol("str", 3);
	
	search_value_tree(input_mov,  &mnemonic_root, &actual_mov);
	search_value_tree(input_str,  &mnemonic_root, &actual_str);

	assert_streq(expect_mov, actual_mov);
	assert_streq(expect_str, actual_str);
}

static void test_search_value_tree_fail() {
	binary_init();
	char *input_str;
	int expect = -1;
	char *actual;
	
	to_mnemonic_symbol("mov", 3);
	to_mnemonic_symbol("str", 3);
	
	actual = search_value_tree(-1, &mnemonic_root, &input_str);
	assert_number(expect, actual);
}

static void unit_test() {
	binary_init();
	test_three_mnemonic_search();
	test_three_mnemonic_add();
	test_three_mnemonic_to_symbol();
	test_search_value_tree();
	test_search_value_tree_fail();
}

#if 0
int main(int argc, char *argv[]) {
	unit_test();
}
#endif
