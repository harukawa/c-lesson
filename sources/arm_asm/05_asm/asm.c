#include "asm.h"

#define EMIT_SIZE 64

struct Emitter emitter;
static unsigned char g_byte_buf[100*1024];

void emit_word(struct Emitter *emitter, int oneword) {
	int pos = emitter->pos;
	int i;
	char *buf = &oneword;
	for(i =0; i<4; i++) {
		emitter->buf[pos+i] = buf[i];
	}
	emitter->pos = pos + 4;
}

int assemble(char *output_name) {
	FILE *fp;
	int str_len;
	char *str;
	int code;
	int pos = 0;
	
	emitter.buf = g_byte_buf;
	emitter.pos = 0;
	
	int count = 0;
	while((str_len = cl_getline(&str)) != 0){
		code = asm_one(str);
		emit_word(&emitter, code);
		count++;
	}

	if((fp = fopen(output_name, "wb")) == NULL) {
		fprintf(stderr, "エラー: ファイルがオープンできません: %s\n", output_name);
		exit(EXIT_FAILURE);
	}

	for(int i = 0; i< emitter.pos; i++) {
		fwrite(&emitter.buf[i], sizeof(emitter.buf[i]),1, fp);
	}
	fclose(fp);
	return 0;
}

int asm_one(char *str) {
	struct substring op;
	int read_len;
	int code;
	
	read_len = parse_one(str, &op);
	if(!strncmp("MOV", op.str, 3) || !strncmp("mov", op.str, 3)) {
		code = asm_mov(&str[read_len]);
		return code;
		
	} else if(!strncmp("LDR", op.str, 3) || !strncmp("ldr", op.str, 3)) {
		code = asm_ldr(&str[read_len]);
		return code;
	} else if(!strncmp("STR", op.str, 3) || !strncmp("str", op.str, 3)) {
		code = asm_str(&str[read_len]);
		return code;
	} else if(!strncmp(".raw", op.str, 4)) {
		code = asm_raw(&str[read_len]);
		return code;
	}
	return 0;	
}

int asm_raw(char *str) {
	int embedded,tmp;
	int len = 0;
	tmp = parse_raw(&str[len], &embedded);
	return embedded;
}

int asm_mov(char *str) {
	// Data processing P29 MOV
	// rd       0x0000f000
	// operand  0x00000fff
	int rd,operand;
	int len = 0, tmp;
	
	tmp = parse_register(&str[len], &rd);
	if(0 >= tmp) return 0;
	len += tmp;
	tmp = skip_comma(&str[len]);
	if(0 >= tmp) return 0;
	len += tmp;
	if(is_register(&str[len])) {
		tmp = parse_register(&str[len], &operand);
	} else {
		tmp = parse_immediate(&str[len], &operand);
	}
	int mov = 0xe3a00000;
	rd = 0x0 + rd;
	rd = rd << 12;
	mov += rd;
	mov += operand;
	
	return mov;
}

int asm_common_str_ldr(char *str, int *out_rn, int *out_rd, int *out_offset) {
	// Data processing P42 STR LDR 
	// rn       0x000f0000
	// rd       0x0000f000
	// offset   0x00000fff
	int rn,rd,offset = 0x0;
	int len = 0, tmp;
	
	tmp = parse_register(&str[len], &rd);
	if(0 >= tmp) return 0;
	len += tmp;
	tmp = skip_comma(&str[len]);
	if(0 >= tmp) return 0;
	len += tmp;
	
	if(is_sbracket(&str[len])) {
		tmp = skip_sbracket(&str[len]);
		len += tmp;
		tmp = parse_register(&str[len], &rn);
		if(0 >= tmp) return 0;
		len += tmp;
	}
	if(!is_sbracket(&str[len])) {
		tmp = skip_comma(&str[len]);
		if(0 >= tmp) return 0;
		len += tmp;
		tmp = parse_immediate(&str[len], &offset);
	}
	
	rn = 0x0 + rn;
	rn = rn << 16;
	*out_rn = rn;
	rd = 0x0 + rd;
	rd = rd << 12;
	*out_rd = rd;
	*out_offset = offset;
	
	return 1;
}

int asm_str(char *str) {
	int rn,rd, offset;
	asm_common_str_ldr(str, &rn, &rd, &offset);
	int str_code = 0xe5800000;
	
	str_code += rn;
	str_code += rd;
	str_code += offset;
	return str_code;
}

int asm_ldr(char *str) {
	int rn,rd, offset;
	asm_common_str_ldr(str, &rn, &rd, &offset);
	int ldr = 0xe5900000;
	
	ldr += rn;
	ldr += rd;
	ldr += offset;
	return ldr;
}

void debug_emitter_dump() {
	int j = 1;
	for(int i = 0; i< emitter.pos; i = i+4) {
		printf("%d %02x%02x%02x%02x\n",j,emitter.buf[i+3],emitter.buf[i+2],
			emitter.buf[i+1],emitter.buf[i]);
		j++;
	}
}


static void test_assemble_mov() {
	FILE *fp = NULL;
	char *file_name = "./test/test_cl_utils.s";
	if((fp=fopen(file_name, "r"))==NULL){
		fprintf(stderr, "エラー: ファイルがオープンできません: %s\n", file_name);
		exit(EXIT_FAILURE);
	}
	cl_file_set_fp(fp);
	assemble("./output/test.bin");
	fclose(fp);
	
	int expect[8] = {0x02, 0x10, 0xa0, 0xe3, 0x04, 0x20, 0xa0, 0xe3};
	
	for(int i=0; i<emitter.pos; i++) {
		assert_number(expect[i], emitter.buf[i]);
	}
}

static void test_asm_one() {
	char *input = "mov r1, r2";
	int expect = 0xe3a01002;
	
	int actual = asm_one(input);
	
	assert_number(expect, actual);	
}

static void test_asm_mov() {
	char *input = "    r1, r2";
	int expect = 0xe3a01002;
	int actual;
	
	actual = asm_mov(input);
	assert_number(expect, actual);
}


static void test_asm_mov_immediate() {
	char *input = "    r1, #0x64";
	int expect = 0xe3a01064;
	int actual;
	
	actual = asm_mov(input);
	assert_number(expect, actual);
}

static void test_asm_ldr_immediate() {
	char *input = "    r0, [r15, #0x30]";
	int expect = 0xe59f0030;
	int actual;
	
	actual = asm_ldr(input);
	assert_number(expect, actual);
}

static void test_asm_ldr() {
	char *input = "    r1, [r14]";
	int expect = 0xe59e1000;
	int actual;
	
	actual = asm_ldr(input);
	assert_number(expect, actual);
}

static void test_asm_str_immediate() {
	char *input = "    r0, [r15, #0x30]";
	int expect = 0xe58f0030;
	int actual;
	
	actual = asm_str(input);
	assert_number(expect, actual);
}

static void test_asm_str() {
	char *input = "    r1, [r14]";
	int expect = 0xe58e1000;
	int actual;
	
	actual = asm_str(input);
	assert_number(expect, actual);
}


static void test_asm_raw_number() {
	char *input = "  0x12345678";
	int expect = 0x12345678;
	int actual;
	
	actual = asm_raw(input);
	assert_number(expect, actual);
}

static void unit_tests() {
	test_asm_mov();
	test_asm_mov_immediate();
	test_asm_one();
	test_assemble_mov();
	test_asm_ldr_immediate();
	test_asm_ldr();
	test_asm_str_immediate();
	test_asm_str();
	test_asm_raw_number();
}

#if 0 
int main(){
	unit_tests();
}
#endif
