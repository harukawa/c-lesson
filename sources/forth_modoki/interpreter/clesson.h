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
	NODE_UNKNOWN
};

struct Node {
	enum NodeType ntype;
	union {
		int number;
		char *name;
	}u;
};

int cl_getc();
void cl_getc_set_src(char* str);
void stack_init();
void stack_push(struct Node *node);
int stack_pop(struct Node *out_node);
int parse_one(int prev_ch, struct Token *out_token);

void dict_put(char* key, struct Node *node);
int dict_get(char* key, struct Node *out_node);
void dict_print_all();
int streq(char *s1, char *s2);

void dict_put(char* key, struct Node *node);
int dict_get(char* key, struct Node *out_node);
