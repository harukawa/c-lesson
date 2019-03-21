#include "asm.h"

static int dict_pos = 0;

#define DICT_SIZE 1024

static struct KeyValue dict_array[DICT_SIZE];

void dict_put(struct KeyValue *keyValue) {
	int existence = 0;
	for(int i=0; i < dict_pos; i++) {
		if(keyValue->key == dict_array[i].key) {
			dict_array[i] = *keyValue;
			existence = 1;
		}
	}
	if(!existence) {
		dict_array[dict_pos] = *keyValue;
		dict_pos++;
	}
}

int dict_get(int key, struct KeyValue *out_keyValue) {
	for(int i = 0; i<dict_pos; i++) {
		if(key == dict_array[i].key) {
			*out_keyValue = dict_array[i];
			return 1;
		}
	}
	return 0;
}

void dict_init() {
	dict_pos = 0;
	dict_array[dict_pos].key = 0;
	dict_array[dict_pos].value = 0;
}

void test_one_put_get() {
	dict_init();
	struct KeyValue input = {1, 2};
	
	struct KeyValue actual;
	dict_put(&input);
	dict_get(input.key, &actual);
	
	assert_number(input.key, actual.key);
	assert_number(input.value, actual.value);
}

void test_two_put_get() {
	dict_init();
	struct KeyValue input = {1, 2};
	struct KeyValue input2= {5, 6};
	
	struct KeyValue actual;
	struct KeyValue actual2;
	dict_put(&input);
	dict_put(&input2);
	dict_get(input.key, &actual);
	dict_get(input2.key, &actual2);
	
	assert_number(input.key, actual.key);
	assert_number(input.value, actual.value);
	assert_number(input2.key, actual2.key);
	assert_number(input2.value, actual2.value);
}

void test_same_key_put_get() {
	dict_init();
	struct KeyValue input = {1, 2};
	struct KeyValue input2= {1, 6};
	
	struct KeyValue actual;
	struct KeyValue actual2;
	dict_put(&input);
	dict_get(input.key, &actual);
	dict_put(&input2);
	dict_get(input2.key, &actual2);
	
	assert_number(input.key, actual.key);
	assert_number(input.value, actual.value);
	assert_number(input2.key, actual2.key);
	assert_number(input2.value, actual2.value);
}

static void unit_tests() {
	test_one_put_get();
	test_two_put_get();
	test_same_key_put_get();
}
#if 0
int main() {
	unit_tests();
}
#endif