#include <stdio.h>
#include <string.h>

enum LexicalType {
    NUMBER,
    SPACE,
    EXECUTABLE_NAME,
    LITERAL_NAME,
    OPEN_CURLY,
    CLOSE_CURLY, 
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    enum LexicalType ltype;
    union {
        int number;
        char onechar;
        char *name;
    } u;
};

enum NodeType {
	NODE_NUMBER,
	NODE_LITERAL_NAME,
	NODE_EXECUTABLE_NAME,
	NODE_EXECUTABLE_ARRAY,
	NODE_UNKNOWN,
	NODE_C_FUNC,
	NODE_EXEC_PRIMITIVE
};

enum ExecWord {
	OP_EXEC,
	OP_JMP,
	OP_JMP_NOT_IF
};

enum ContType {
	NODE,
	CONTINUATION,
};

struct Node {
	enum NodeType ntype;
	union {
		int number;
		char *name;
		void (*cfunc)();
		int (*compile_func)(struct Emitter *emit);
		struct NodeArray *byte_codes;
	}u;
};

struct NodeArray {
	int len;
	struct Node nodes[0];
};

struct Continuation {
	//enum ContType ctype;
	union {
		struct Node *node;
		struct NodeArray *exec_array;
	}u;
	int pc;
};

struct Emitter {
	struct Node *nodes;
	int pos;
};


int cl_getc();
void cl_getc_set_src(char* str);
void cl_getc_set_fp(FILE* input_fp);

void stack_init();
void stack_push(struct Node *node);
int stack_pop(struct Node *out_node);
void stack_check(struct Node *out_node, int n);
void stack_print_all();
int parse_one(int prev_ch, struct Token *out_token);

void dict_put(char* key, struct Node *node);
int dict_get(char* key, struct Node *out_node);
void compile_dict_put(char* key, struct Node *node);
int compile_dict_get(char* key, struct Node *out_node);
void dict_print_all();
int streq(char *s1, char *s2);
void register_primitives();
void eval();
void test_units();

void assert_lname_eq(char *expect, struct Node *actual);
void assert_num_eq(int expect, struct Node *actual);
void assert_type_eq(int expect, struct Node *actual);

void co_push(struct Continuation *cont);
int co_pop(struct Continuation *cont);
