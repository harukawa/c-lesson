#include <stdio.h>

enum NodeType {
	NUMBER,
	LITERAL_NAME,
	UNKNOWN
};

struct Node {
	enum NodeType ntype;
	union {
		int number;
		char *name;
	}u;
};

void stack_push(struct Node *node);
int stack_pop(struct Node *out_node);
