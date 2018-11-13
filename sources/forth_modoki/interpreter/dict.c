#include "clesson.h"
#include "assert.h"

static int dict_pos = 0;
struct KeyValue {
    char *key;
    struct Node value;
};

#define DICT_SIZE 1024
static struct KeyValue dict_array[DICT_SIZE];

void dict_put(char* key, struct Node *node) {
	int found = 1;
	for(int i = 0; i < dict_pos; i++){
		if(streq(key, dict_array[i].key)) {
			dict_array[i].value = *node;
			found= 0;
		}
	}
	if(found) {
		dict_array[dict_pos].key = key;
		dict_array[dict_pos].value = *node;
		dict_pos++;
	}
}
			
int dict_get(char* key, struct Node *out_node) {
	for(int i = 0; i < dict_pos; i++) {
		if(streq(key, dict_array[i].key)) {
			*out_node = dict_array[i].value;
			return 1;
		}
	}
	return 0;
}
void dict_print_all() {
	for(int i = 0; i < dict_pos; i++) {
		if(dict_array[i].value.ntype == NODE_NUMBER) {
			printf("KEY: %s VALUE %d\n",dict_array[i].key, dict_array[i].value.u.number);
		}else if(dict_array[i].value.ntype == NODE_LITERAL_NAME) {
			printf("KEY: %s VALUE %s\n",dict_array[i].key, dict_array[i].value.u.name);
		}
	}
}
			
void dict_init() {
	dict_pos = 0;
	struct Node none ={NODE_UNKNOWN,{0}};
	dict_array[dict_pos].key = "\0";
	dict_array[dict_pos].value = none;
}

void test_one_put_get() {
	dict_init();
	struct Node input = {NUMBER,{1}};
	char *input_key = "one";

	struct Node actual ={NODE_UNKNOWN,{0}};
	dict_put(input_key, &input);
	dict_get(input_key, &actual);

	assert(actual.ntype == input.ntype);
	assert(actual.u.number == input.u.number);
}

void test_two_put_get() {
	dict_init();
	struct Node input = {NUMBER,{1}};
	struct Node input2 = {NODE_LITERAL_NAME, {"hello"} };
	char *input_key = "one";
	char *input_key2 = "aisatu";

	struct Node actual ={NODE_UNKNOWN,{0}};
	struct Node actual2 ={NODE_UNKNOWN,{0}};
	dict_put(input_key, &input);
	dict_put(input_key2, &input2);
	dict_get(input_key, &actual);
	dict_get(input_key2, &actual2);

	assert(actual.ntype == input.ntype);
	assert(actual.u.number == input.u.number);
	assert(actual2.ntype == input2.ntype);
	assert(actual2.u.number == input2.u.number);
}

void test_same_key() {
	dict_init();
	struct Node input = {NUMBER,{1}};
	struct Node input2 = {NODE_LITERAL_NAME, {"hello"} };
	char *input_key = "one";

	struct Node actual ={NODE_UNKNOWN,{0}};
	struct Node actual2 ={NODE_UNKNOWN,{0}};
	dict_put(input_key, &input);
	dict_get(input_key, &actual);
	dict_put(input_key, &input2);
	dict_get(input_key, &actual2);

	assert(actual.ntype == input.ntype);
	assert(actual.u.number == input.u.number);
	assert(actual2.ntype == input2.ntype);
	assert(actual2.u.number == input2.u.number);
}

void test_none_get() {
	int get;
	dict_init();
	char *input_key = "one";

	struct Node actual ={NODE_UNKNOWN,{0}};
	get = dict_get(input_key, &actual);

	assert(get == 0);
}

#if 0
int main() {
	test_one_put_get();
	test_two_put_get();
	test_same_key();
	test_none_get();
	return 0;
}
#endif
