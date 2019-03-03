#include "asm.h"

#define EMIT_SIZE 64

struct Emitter emit[EMIT_SIZE];

void emit_byte(struct Emitter* emitter, char onebyte){
	emitter->array = onebyte;
	emitter->type  = ONE_BYTE;
}

void emit_word(struct Emitter* emitter, int oneword) {
	emitter->array = oneword;
	emitter->type  = ONE_WORD;
}

int assembly() {
	int str_len;
	char *str;
	int code;
	int pos = 0;
	
	int i = 0;
	while((str_len = cl_getline(&str)) != 0){
		code = asm_one(str);
		if(4 == sizeof(code)) {
			emit_word(&emit[i], code);
		}
		i++;
	}
	
	for(int j=0; j<i; j++) {
		printf("%d: 0x%x\n",j,emit[j].array);
	}
	
	return 0;
}

int asm_one(char *str) {
	struct Emitter emit;
	struct substring op;
	int read_len;
	int code;
	
	read_len = parse_one(str, &op);
	if(!strncmp("MOV", op.str, 3) || !strncmp("mov", op.str, 3)) {
		code = asm_mov(&str[read_len]);
		return code;
	}
	return 0;	
}

int asm_mov(char *str) {
	int rd,operand;
	int len = 0, tmp;
	
	tmp = parse_register(&str[len], &rd);
	if(0 >= tmp) return 0;
	len += tmp;
	tmp = skip_comma(&str[len]);
	if(0 >= tmp) return 0;
	tmp = parse_register(&str[len], &operand);
	
	int mov = 0xe1a00000;
	rd = 0x0 + rd;
	rd = rd << 12;
	mov += rd;
	mov += operand;
	
	return mov;
}


static void test_assembly() {
	FILE *fp = NULL;
	char *file_name = "./test/test_cl_utils.s";
	if((fp=fopen(file_name, "r"))==NULL){
		printf("error\n");
	}
	cl_file_set_fp(fp);
	assembly();
	
	int expect[2];
	int expect_type = ONE_WORD;
	expect[0] = 0xe1a01002;
	expect[1] = 0xe1a02004;
	
	for(int i = 0; i< 2; i++) {
		assert_number(expect_type, emit[i].type);
		assert_number(expect[i], emit[i].array);
	}

	fclose(fp);
}

static void test_asm_one() {
	char *input = "mov r1, r2";
	int expect = 0xe1a01002;
	
	int actual = asm_one(input);
	
	assert_number(expect, actual);	
}

static void test_asm_mov() {
	char *input = "mov r1, r2";
	int input_len = 3;
	int expect = 0xe1a01002;
	int actual;
	
	actual = asm_mov(&input[input_len]);
	assert_number(expect, actual);
}

static void unit_tests() {
	test_asm_mov();
	test_asm_one();
	test_assembly();
}

#if 0
int main(){
	unit_tests();
}
#endif