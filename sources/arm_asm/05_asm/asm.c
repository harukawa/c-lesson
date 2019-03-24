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

void emit_embedded(struct Emitter *emitter, char *one_embedded) {
	int pos = emitter->pos;
	int i;
	char *buf = one_embedded;
	int length = strlen(buf);
	for(i = 0; i< length; i++){;
		emitter->buf[pos+i] = buf[i];
	}
	
	pos = pos + length;
	int remaining = length % 4;
	if(remaining != 0) {
		for(i=0; i < 4-remaining; i++) {
			emitter->buf[pos+i] = NULL;
		}
		pos = pos + (4-remaining);
	}
	emitter->pos = pos;
}

int assemble(char *output_name) {
	FILE *fp;
	int str_len;
	char *str;
	int code;
	int pos = 0;
	
	emitter.buf = g_byte_buf;
	emitter.pos = 0;
	
	setup_mnemonic();
	dict_init();
	unresolved_list_init();
	
	while((str_len = cl_getline(&str)) != 0){
		code = asm_one(str);
		if(code != 0) {
			if(code > 0xffffffff) {
				
			} else {
				emit_word(&emitter, code);
			}
		}
	}
	
	address_fix();

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

int address_fix() {
	struct List list;
	struct KeyValue keyValue;
	int code;
	int tmp_pos = emitter.pos;
	
	//list : mnemonic   keyValue: label
	while(unresolved_list_get(&list)) {
		// case: b
		if(0xea000000 == list.code) {
			if(dict_get(list.label, &keyValue)) {
				int pos = keyValue.value - list.emitter_pos;
				pos = pos - 8;
				pos = pos >> 2;
				pos = 0xffffff & pos;
				code = list.code + pos;
			}
		} else if(0xe59f0000 == (list.code & 0xffff0000)) {
			if(dict_get(list.label, &keyValue)) {
				int pos = keyValue.value - list.emitter_pos;
				pos = 0xfff & pos;
				code = list.code + pos;
			}
		}
		emitter.pos = list.emitter_pos;
		emit_word(&emitter, code);
	}
	emitter.pos = tmp_pos;
}

int asm_one(char *str) {
	struct substring op;
	int read_len;
	read_len = parse_one(str, &op);
	
	//case of label
	if(':' == str[op.len-1]) {
		int label;
		label = to_label_symbol(op.str, op.len - 1);
		struct KeyValue keyValue;
		keyValue.key = label;
		keyValue.value = emitter.pos;
		dict_put(&keyValue);
		return 0;
	//case of mnemonic
	} else {
		int mnemonic;
		mnemonic = to_mnemonic_symbol(op.str, op.len);
		if(g_mov == mnemonic || g_MOV == mnemonic) {
			return asm_mov(&str[read_len]);
			
		} else if(g_ldr == mnemonic || g_LDR == mnemonic) {
			return asm_ldr(&str[read_len]);
			
		} else if(g_str == mnemonic || g_STR == mnemonic) {
			return asm_str(&str[read_len]);
			
		} else if(g_raw == mnemonic) {
			return asm_raw(&str[read_len], &emitter);
		} else if(g_b == mnemonic || g_B == mnemonic) {
			return asm_b(&str[read_len], &emitter);
		}
	}
	return 0;	
}

int asm_raw(char *str, struct Emitter *emitter) {
	// 文字の場合
	if(is_raw_string(str)) {
		char *embedded_str = "";
		int len;
		len = parse_string(str, '"', &embedded_str);
		emit_embedded(emitter, embedded_str);
		return 0;
	//　数字の場合
	} else {
		int embedded;
		parse_raw_number(str, &embedded);
		return embedded;
	}
}

int asm_b(char *str, struct Emitter *emitter) {
	// Data processing P27 b
	// offset  0x00ffffff
	int read_len, label;
	struct substring op;
	int b = 0xea000000;
	
	read_len = parse_one(str, &op);
	label = to_label_symbol(op.str, op.len);

	struct List *list;
	list = malloc(sizeof(list));
	list->emitter_pos = emitter->pos;
	list->label = label;
	list->code = b;
	unresolved_list_put(list);
	return b;
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
	rd = rd << 12;
	mov += rd;
	mov += operand;
	
	return mov;
}

int asm_common_str_ldr(char *str, int *out_rn, int *out_rd, 
	int *out_offset, struct Emitter *emitter) {
	// Data processing P42 STR LDR 
	// rn       0x000f0000
	// rd       0x0000f000
	// offset   0x00000fff
	int rn = 0,rd = 0,offset = 0x0;
	int len = 0, tmp;
	
	tmp = parse_register(&str[len], &rd);
	rd = rd << 12;
	if(0 >= tmp) return 0;
	len += tmp;
	tmp = skip_comma(&str[len]);
	if(0 >= tmp) return 0;
	len += tmp;
	// ラベルの場合 ldr
	if(is_equals_sign(&str[len])) {
		char *label;
		int label_len, label_number;
		rn = 15;
		rn = rn << 16;
		label_len = parse_string(&str[len], '=', &label);
		label_number = to_label_symbol(label, label_len);

		struct List *list;
		int code = 0xe59f0000;
		code = code + rd;
		list = malloc(sizeof(list));
		list->emitter_pos = emitter->pos;
		list->label = label_number;
		list->code = code;
		unresolved_list_put(list);
	
	// レジスタと即値の場合
	} else {
		if(is_sbracket(&str[len])) {
			tmp = skip_sbracket(&str[len]);
			len += tmp;
			tmp = parse_register(&str[len], &rn);
			rn = rn << 16;
			if(0 >= tmp) return 0;
			len += tmp;
		}
		if(!is_sbracket(&str[len])) {
			tmp = skip_comma(&str[len]);
			if(0 >= tmp) return 0;
			len += tmp;
			tmp = parse_immediate(&str[len], &offset);
		}
	}
	
	*out_rn = rn;
	*out_rd = rd;
	*out_offset = offset;
	
	return 1;
}

int asm_str(char *str) {
	int rn,rd, offset;
	asm_common_str_ldr(str, &rn, &rd, &offset, &emitter);
	int str_code = 0xe5800000;
	
	str_code += rn;
	str_code += rd;
	str_code += offset;
	return str_code;
}

int asm_ldr(char *str, struct Emitter *emitter) {
	int rn,rd, offset;
	asm_common_str_ldr(str, &rn, &rd, &offset, emitter);
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
	emitter.pos = 0;
	char *input = "mov r1, r2";
	int expect = 0xe3a01002;
	
	int actual = asm_one(input);
	
	assert_number(expect, actual);	
}

static void test_asm_one_label() {
	emitter.pos = 0;
	char *input = "label:";
	char *input_label = "label";
	int expect_value = 0;
	struct KeyValue actual;
	
	int label = asm_one(input);
	int actual_key = to_label_symbol(input_label, 5);
	dict_get(actual_key, &actual);

	assert_number(expect_value, actual.value);
}

static void test_asm_one_space() {
	char *input = "    mov r1, r2";
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
	
	actual = asm_ldr(input, &emitter);
	assert_number(expect, actual);
}

static void test_asm_ldr() {
	char *input = "    r1, [r14]";
	int expect = 0xe59e1000;
	int actual;
	
	actual = asm_ldr(input, &emitter);
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
	
	actual = asm_raw(input, &emitter);

	assert_number(expect, actual);
}

static void test_asm_b() {
	unresolved_list_init();
	emitter.pos = 8;
	char *input = "    label";
	
	int label;
	int expect = 0xea000000;

	struct List actual_list;
	
	int actual = asm_b(input, &emitter);
	unresolved_list_get(&actual_list);
	label = to_label_symbol("label", 5);
	
	assert_number(expect, actual);
	assert_number(expect, actual_list.code);
	assert_number(emitter.pos, actual_list.emitter_pos);
	assert_number(label, actual_list.label);
}

static void test_emit_embedded() {
	emitter.pos = 0;
	char *input = "st\n\\\"ring";
	int expect_pos = 12;
	int expect[12] ={0x73, 0x74, 0x0a, 0x5c, 0x22, 0x72, 0x69, 0x6e, 0x67, 0x0, 0x0, 0x0};
	
	emit_embedded(&emitter, input);
	
	assert_number(expect_pos, emitter.pos);
	for(int i=0; i<12; i++) {
		assert_number(expect[i],emitter.buf[i]);
	}
}

static void test_asm_raw_string() {
	emitter.pos = 0;
	char *input;
	FILE *fp = NULL;
	char *file_name = "./test/test_parser_raw_string.s";
	if((fp=fopen(file_name, "r"))==NULL){
		fprintf(stderr, "エラー: ファイルがオープンできません: %s\n", file_name);
		exit(EXIT_FAILURE);
	}
	cl_file_set_fp(fp);
	
	int expect_pos = 12;
	int expect[12] ={0x73, 0x74, 0x0a, 0x5c, 0x22, 0x72, 0x69, 0x6e, 0x67, 0x0, 0x0, 0x0};
	
	cl_getline(&input);
	asm_raw(input, &emitter);
	fclose(fp);

	assert_number(expect_pos, emitter.pos);
	for(int i=0; i<12; i++) {
		assert_number(expect[i],emitter.buf[i]);
	} 
}


static void test_address_fix() {
	emitter.pos = 0;
	dict_init();
	unresolved_list_init();
	
	char *input  = "label:";
	char *input2 = "b label";
	int expect[4] = {0xfe, 0xff, 0xff, 0xea};
	
	int code = asm_one(input);
	code = asm_one(input2);
	emit_word(&emitter, code);
	address_fix();
	
	
	for(int i=0; i<emitter.pos; i++) {
		assert_number(expect[i], emitter.buf[i]);
	}
}

static void test_asm_ldr_label() {
	unresolved_list_init();
	dict_init();
	emitter.pos = 8;
	char *input = "    r1, =label";
	int expect = 0xe59f1000;
	int actual, label;
	
	struct List actual_list;
	
	actual = asm_ldr(input, &emitter);
	unresolved_list_get(&actual_list);
	label = to_label_symbol("label", 5);
	
	assert_number(expect, actual);
	assert_number(expect, actual_list.code);
	assert_number(emitter.pos, actual_list.emitter_pos);
	assert_number(label, actual_list.label);
}

static void test_address_fix_ldr() {
	emitter.pos = 0;
	dict_init();
	unresolved_list_init();
	
	char *input  = "ldr r0, =label";
	char *input2 = "label:";
	char *input3 = ".raw 0x101f1000";
	int expect[4] = {0x04, 0x00, 0x9f, 0xe5};
	
	int code = asm_one(input);
	emit_word(&emitter, code);
	code = asm_one(input2);
	code = asm_one(input3);
	emit_word(&emitter, code);
	address_fix();
	
	for(int i=0; i<4; i++) {
		assert_number(expect[i], emitter.buf[i]);
	}
}

static void unit_tests() {
	setup_mnemonic();
	test_asm_mov();
	test_asm_mov_immediate();
	test_asm_one();
	test_asm_one_space();
	test_assemble_mov();
	test_asm_ldr_immediate();
	test_asm_ldr();
	test_asm_str_immediate();
	test_asm_str();
	test_asm_raw_number();
	test_asm_one_label();
	test_asm_b();
	test_address_fix();
	test_emit_embedded();
	test_asm_raw_string();
	test_asm_ldr_label();
	test_address_fix_ldr();
}
//#if 0
int main(){
	unit_tests();
}
//#endif
