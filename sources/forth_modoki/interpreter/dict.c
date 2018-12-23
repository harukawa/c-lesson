#include "clesson.h"
#include "assert.h"

struct HashNode {
	char *key;
	struct Node value;
	struct HashNode *next;
};

#define DICT_SIZE 16
struct HashNode *dict_array[DICT_SIZE];
struct HashNode *dict_compile[DICT_SIZE];

static int hash(char *str) {
   unsigned int val = 0;
   while(*str) {
       val += *str++;
   }
   return (int)(val% DICT_SIZE);
}

static void update_or_insert_list(struct HashNode *head, char *input_key, struct Node *input_value) {
	struct HashNode *hash_node = head;
	while(hash_node != NULL) {
		if(streq(hash_node->key, input_key)) {
			hash_node->value = *input_value;
			return;
		}
		if(hash_node->next == NULL) {
			break;
		}
		hash_node = hash_node->next;
	}

	struct HashNode *last_node;
	last_node = malloc(sizeof(struct HashNode));
	last_node->next = NULL;
	last_node->key = input_key;
	last_node->value = *input_value;
	hash_node->next = last_node;	
}


static void common_dict_put(struct HashNode **dict, char *input_key, struct Node *value) {
	int idx = hash(input_key);
	struct HashNode *head = dict[idx];
	if(head == NULL) {
		head = malloc(sizeof(struct HashNode));
		head->next = NULL;
		head->key = input_key;
		head->value = *value;
		dict[idx] = head;
	} else {
		update_or_insert_list(head, input_key, value);
	}
}

void dict_put(char *input_key, struct Node *value) {
	common_dict_put(dict_array, input_key, value);
}


void compile_dict_put(char *input_key, struct Node *value) {
	common_dict_put(dict_compile, input_key, value);
}

static int common_dict_get(struct HashNode **dict, char *input_key, struct Node *out_node) {
	int idx = hash(input_key);
	struct HashNode *head = dict[idx];
	while(head != NULL) {
		if(streq(head->key, input_key)) {
			*out_node = head->value;
			return 1;
		}
		head = head->next;
	}
	return 0;
}
			
int dict_get(char* input_key, struct Node *out_node) {
	return common_dict_get(dict_array, input_key, out_node);
}

int compile_dict_get(char* input_key, struct Node *out_node) {
	return common_dict_get(dict_compile, input_key, out_node);
}

void dict_print_all() {
	struct HashNode *hash_node;
	for(int i = 0; i < DICT_SIZE; i++) {
		if(dict_array[i] != NULL) {
			hash_node = dict_array[i];
			while(hash_node != NULL) {
				if(hash_node->value.ntype == NODE_NUMBER) {
					printf("idx: %d KEY: %s VALUE: %d\n",i, hash_node->key, hash_node->value.u.number);
				} else if(hash_node->value.ntype == NODE_LITERAL_NAME) {
					printf("idx: %d KEY: %s VALUE: %s\n",i, hash_node->key, hash_node->value.u.name);
				}
				hash_node = hash_node->next;
			}
		}
	}
}
			

void dict_init(){
	for(int i = 0; i < DICT_SIZE; i++) {
		dict_array[i] = NULL;
	}
}

// assert NodeType
void assert_type_eq(int expect, struct Node *actual) {
	assert(expect == actual->ntype);
}

// assert Node NODE_LITERAL_NAME
void assert_lname_eq(char *expect, struct Node *actual) {
	assert(expect == actual->u.name);
}

// assert Node NUMBER
void assert_num_eq(int expect, struct Node *actual) {
	assert(NODE_NUMBER == actual->ntype);
	assert(expect == actual->u.number);
}

static void test_one_put_get() {
	dict_init();
	struct Node input = {NODE_NUMBER,{1}};
	char *input_key = "one";
	int expect = 1;

	struct Node actual ={NODE_UNKNOWN,{0}};
	dict_put(input_key, &input);
	dict_get(input_key, &actual);

	assert_type_eq(NODE_NUMBER, &actual);
	assert_num_eq(expect, &actual);
}

static void test_two_put_get() {
	dict_init();
	struct Node input = {NODE_NUMBER,{1}};
	struct Node input2 = {NODE_LITERAL_NAME, {"hello"} };
	char *input_key = "one";
	char *input_key2 = "aisatu";
	int expect = 1;
	char *expect2 = "hello";

	struct Node actual ={NODE_UNKNOWN,{0}};
	struct Node actual2 ={NODE_UNKNOWN,{0}};
	dict_put(input_key, &input);
	dict_put(input_key2, &input2);
	dict_get(input_key, &actual);
	dict_get(input_key2, &actual2);

	assert_type_eq(NODE_NUMBER, &actual);
	assert_num_eq(expect, &actual);
	assert_type_eq(NODE_LITERAL_NAME, &actual2);
	assert_lname_eq(expect2, &actual2);
}

static void test_same_key() {
	dict_init();
	struct Node input = {NODE_NUMBER,{1}};
	struct Node input2 = {NODE_LITERAL_NAME, {"hello"} };
	char *input_key = "one";
	int expect = 1;
	char *expect2 = "hello";

	struct Node actual ={NODE_UNKNOWN,{0}};
	struct Node actual2 ={NODE_UNKNOWN,{0}};
	dict_put(input_key, &input);
	dict_get(input_key, &actual);
	dict_put(input_key, &input2);
	dict_get(input_key, &actual2);

	assert_type_eq(NODE_NUMBER, &actual);
	assert_num_eq(expect, &actual);
	assert_type_eq(NODE_LITERAL_NAME, &actual2);
	assert_lname_eq(expect2, &actual2);
}

static void test_none_get() {
	dict_init();
	char *input_key = "one";
	struct Node dummy ={NODE_UNKNOWN,{0}};

	int actual;
	actual = dict_get(input_key, &dummy);

	assert(actual == 0);
}

static void test_hash_collision() {
	dict_init();
	struct Node input = {NODE_NUMBER,{1}};
	struct Node input2 = {NODE_LITERAL_NAME, {"hello"} };
	struct Node input3 = {NODE_LITERAL_NAME, {"good"} };
	char *input_key = "one";
	char *input_key2 = "eno";
	char *input_key3 = "noe";
	int expect = 1;
	char *expect2 = "hello";
	char *expect3 = "good";

	struct Node actual ={NODE_UNKNOWN,{0}};
	struct Node actual2 ={NODE_UNKNOWN,{0}};
	struct Node actual3 ={NODE_UNKNOWN,{0}};
	dict_put(input_key, &input);
	dict_put(input_key2, &input2);
	dict_put(input_key3, &input3);
	dict_get(input_key, &actual);
	dict_get(input_key2, &actual2);
	dict_get(input_key3, &actual3);
   
	assert_type_eq(NODE_NUMBER, &actual);
	assert_num_eq(expect, &actual);
	assert_type_eq(NODE_LITERAL_NAME, &actual2);
	assert_lname_eq(expect2, &actual2);
	assert_type_eq(NODE_LITERAL_NAME, &actual3);
	assert_lname_eq(expect3, &actual3);
}

#if 0
int main() {
	test_one_put_get();
	test_two_put_get();
	test_same_key();
	test_none_get();
	test_hash_collision();
	return 0;
}
#endif
