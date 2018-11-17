#include "clesson.h"
#include "assert.h"

struct HashNode {
	char *key;
	struct Node value;
	struct HashNode *next;
};

#define DICT_SIZE 16
struct HashNode *dict_array[DICT_SIZE];

int hash(char *str) {
   unsigned int val = 0;
   while(*str) {
       val += *str++;
   }
   return (int)(val% DICT_SIZE);
}

void update_or_insert_list(struct HashNode *head, char *input_key, struct Node *input_value) {
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

void dict_put(char *input_key, struct Node *value) {
	int idx = hash(input_key);
	struct HashNode *head = dict_array[idx];
	if(head == NULL) {
		head = malloc(sizeof(struct HashNode));
		head->next = NULL;
		head->key = input_key;
		head->value = *value;
		dict_array[idx] = head;
		head = dict_array[idx];
	} else {
		update_or_insert_list(head, input_key, value);
	}
}
			
int dict_get(char* input_key, struct Node *out_node) {
	int idx = hash(input_key);
	struct HashNode *head = dict_array[idx];
	while(head != NULL) {
		if(streq(head->key, input_key)) {
			*out_node = head->value;
			return 1;
		}
		head = head->next;
	}
	return 0;
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

void assert_name_eq(struct Node *expect, struct Node *actual) {
	assert(expect->ntype == actual->ntype);
	assert(expect->u.name == actual->u.name);
}

void assert_num_eq(struct Node *expect, struct Node *actual) {
	assert(expect->ntype == actual->ntype);
	assert(expect->u.number == actual->u.number);
}

void test_one_put_get() {
	dict_init();
	struct Node input = {NODE_NUMBER,{1}};
	char *input_key = "one";

	struct Node actual ={NODE_UNKNOWN,{0}};
	dict_put(input_key, &input);
	dict_get(input_key, &actual);
	assert_num_eq(&input, &actual);
}

void test_two_put_get() {
	dict_init();
	struct Node input = {NODE_NUMBER,{1}};
	struct Node input2 = {NODE_LITERAL_NAME, {"hello"} };
	char *input_key = "one";
	char *input_key2 = "aisatu";

	struct Node actual ={NODE_UNKNOWN,{0}};
	struct Node actual2 ={NODE_UNKNOWN,{0}};
	dict_put(input_key, &input);
	dict_put(input_key2, &input2);
	dict_get(input_key, &actual);
	dict_get(input_key2, &actual2);
	assert_num_eq(&input, &actual);
	assert_num_eq(&input2, &actual2);
}

void test_same_key() {
	dict_init();
	struct Node input = {NODE_NUMBER,{1}};
	struct Node input2 = {NODE_LITERAL_NAME, {"hello"} };
	char *input_key = "one";

	struct Node actual ={NODE_UNKNOWN,{0}};
	struct Node actual2 ={NODE_UNKNOWN,{0}};
	dict_put(input_key, &input);
	dict_get(input_key, &actual);
	dict_put(input_key, &input2);
	dict_get(input_key, &actual2);

	assert_num_eq(&input, &actual);
	assert_name_eq(&input2, &actual2);
}

void test_none_get() {
	dict_init();
	char *input_key = "one";
	struct Node dummy ={NODE_UNKNOWN,{0}};

	int actual;
	actual = dict_get(input_key, &dummy);

	assert(actual == 0);
}

void test_hash_collision() {
	dict_init();
	struct Node input = {NODE_NUMBER,{1}};
	struct Node input2 = {NODE_LITERAL_NAME, {"hello"} };
	struct Node input3 = {NODE_LITERAL_NAME, {"good"} };
	char *input_key = "one";
	char *input_key2 = "eno";
	char *input_key3 = "noe";

	struct Node actual ={NODE_UNKNOWN,{0}};
	struct Node actual2 ={NODE_UNKNOWN,{0}};
	struct Node actual3 ={NODE_UNKNOWN,{0}};
	dict_put(input_key, &input);
	dict_put(input_key2, &input2);
	dict_put(input_key3, &input3);
	dict_get(input_key, &actual);
	dict_get(input_key2, &actual2);
	dict_get(input_key3, &actual3);

	assert_num_eq(&input, &actual);
	assert_name_eq(&input2, &actual2);
	assert_name_eq(&input3, &actual3);
}

int main() {
	test_one_put_get();
	test_two_put_get();
	test_same_key();
	test_none_get();
	test_hash_collision();
	return 0;
}
