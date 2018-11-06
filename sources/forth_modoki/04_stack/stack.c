include <stdio.h>

#define STAKC_SIZE 1024

enum LexicalType {
	NUMBER,
	LITERAL_NAME,
	UNKNOWN
};

struct Node {
	enum LexicalType ltype;
	union {
		int number;
		char *name;
	}u;
};

void stack_push(struct Node node){


}

void stack_pop(struct Node *out_node){

}
