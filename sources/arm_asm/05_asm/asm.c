#include "asm.h"

#define EMIT_SIZE 64

struct Emitter emitter;
static unsigned char g_byte_buf[100*1024];

void ensure_four_byte_align(struct Emitter *emitter) {
	//　４バイト区切りになるように0x0を埋め込みます
	// 文だけで４バイト区切りになっているときは0x0を4つ埋め込みます
	if(emitter->byte_remain != -1) {
		for(int i = 0; i< 4- emitter->byte_remain;i++) {
			emitter->buf[emitter->pos+i] = 0x0;
		}
		emitter->pos = emitter->pos + (4 - emitter->byte_remain);
		emitter->byte_remain = -1;
	}
}

void emit_word(struct Emitter *emitter, int oneword) {
	// 文字の埋め込み後の場合、位置を４バイトに合わせる
	ensure_four_byte_align(emitter);
	char *buf = &oneword;
	int pos = emitter->pos;
	for(int i =0; i<4; i++) {
		emitter->buf[pos+i] = buf[i];
	}
	emitter->pos = pos + 4;
}

void update_word(struct Emitter *emitter,int pos,int  oneword) {
	char *buf = &oneword;
	for(int i =0; i<4; i++) {
		emitter->buf[pos+i] = buf[i];
	}
}

void emit_embedded(struct Emitter *emitter, char *one_embedded, int length) {
	int pos = emitter->pos;
	int i;
	char *buf = one_embedded;
	
	for(i = 0; i< length; i++){;
		emitter->buf[pos+i] = buf[i];
	}
	pos = pos + length;
	
	emitter->buf[pos] = 0x0;
	//文と0x0一つ分だけ先に進めます
	pos = pos + 1;
	emitter->pos = pos;
	int remain = pos % 4;
	emitter->byte_remain = remain;
}

void common_unresolved_list_put(int emitter_pos, int label, int code, int immediate) {
	struct List *list;
	list = malloc(sizeof(list));
	list->emitter_pos = emitter_pos;
	list->label = label;
	list->code = code;
	list->immediate = immediate;
	unresolved_list_put(list);
}

int assemble(char *output_name) {
	FILE *fp;
	int str_len;
	char *str;
	int code;
	int pos = 0;
	
	emitter.buf = g_byte_buf;
	emitter.pos = 0;
	emitter.byte_remain = -1;
	for(int i=0; i<100*1024; i++) {
		emitter.buf[i] = 0x3;
	}
	setup_mnemonic();
	dict_init();
	unresolved_list_init();
	emitter.byte_remain = -1;
	while((str_len = cl_getline(&str)) != 0){
		code = asm_one(str);
		if(code != 0) {
			emit_word(&emitter, code);
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

	//list : mnemonic   keyValue: label
	while(unresolved_list_get(&list)) {
		// case: b bne bl blt
		if(0xea000000 == list.code || 0x1a000000 == list.code
			|| 0xeb000000 == list.code || 0xba000000 == list.code) {
			if(dict_get(list.label, &keyValue)) {
				int pos = keyValue.value - list.emitter_pos;
				pos = pos - 8;
				pos = pos >> 2;
				pos = 0xffffff & pos;
				code = list.code + pos;
			}
		// case: ldr
		} else if(0xe59f0000 == (list.code & 0xffff0000)) {
			
			int pos = emitter.pos - 8 - list.emitter_pos;
				
			//文字の埋め込みで４バイト区切りからずれている場合、位置を修正します
			if(emitter.byte_remain != -1) {
				pos +=  4 - emitter.byte_remain;
			}
			code = list.code + pos;
			if(dict_get(list.label, &keyValue)) {	
				// labelの場合　文のアドレスを入れます
				int last_code = 0x00010000;
				last_code += keyValue.value;
				emit_word(&emitter, last_code);
			} else {
				//即値の場合
				emit_word(&emitter, list.immediate);
			}
		}
		update_word(&emitter, list.emitter_pos, code);
	}
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
		//ldr	
		} else if(g_ldr == mnemonic || g_LDR == mnemonic) {
			return asm_ldr(&str[read_len], &emitter);
		// str
		} else if(g_str == mnemonic || g_STR == mnemonic) {
			return asm_str(&str[read_len]);
		// add
		} else if(g_add == mnemonic || g_ADD == mnemonic) {
			return asm_add(&str[read_len]);
		// bne
		} else if(g_bne == mnemonic || g_BNE == mnemonic) {
			return asm_bne(&str[read_len], &emitter);
		// cmp
		} else if(g_cmp == mnemonic || g_CMP == mnemonic) {
			return asm_cmp(&str[read_len]);
		// .raw
		} else if(g_raw == mnemonic) {
			return asm_raw(&str[read_len], &emitter);
		// b
		} else if(g_b == mnemonic || g_B == mnemonic) {
			return asm_b(&str[read_len], &emitter);
		// bl
		} else if(g_bl == mnemonic || g_BL == mnemonic) {
			return asm_bl(&str[read_len], &emitter);
		// blt
		} else if(g_blt == mnemonic || g_BLT == mnemonic) {
			return asm_blt(&str[read_len], &emitter);
		// ldrb
		} else if(g_ldrb == mnemonic || g_LDRB == mnemonic) {
			return asm_ldrb(&str[read_len], &emitter);
			
		// ldmia
		} else if(g_ldmia == mnemonic || g_LDMIA == mnemonic) {
			return asm_ldmia(&str[read_len], &emitter);
		// stmdb
		} else if(g_stmdb == mnemonic || g_STMDB == mnemonic) {
			return asm_stmdb(&str[read_len], &emitter);
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
		emit_embedded(emitter, embedded_str, len);
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

	common_unresolved_list_put(emitter->pos, label, b, NULL);
	return b;
}

int asm_bl(char *str, struct Emitter *emitter) {
	// Data processing P27 bl
	// offset  0x00ffffff
	int read_len, label;
	struct substring op;
	int bl = 0xeb000000;
	
	read_len = parse_one(str, &op);
	label = to_label_symbol(op.str, op.len);

	common_unresolved_list_put(emitter->pos, label, bl, NULL);
	return bl;
}

int asm_bne(char *str, struct Emitter *emitter) {
	// Data processing bne
	// offset  0x00ffffff
	int read_len, label;
	struct substring op;
	int bne = 0x1a000000;
	read_len = parse_one(str, &op);
	label = to_label_symbol(op.str, op.len);

	common_unresolved_list_put(emitter->pos, label, bne, NULL);
	return bne;
}

int asm_blt(char *str, struct Emitter *emitter) {
	// Data processing
	// offset  0x00ffffff
	int read_len, label;
	struct substring op;
	int blt = 0xba000000;
	
	read_len = parse_one(str, &op);
	label = to_label_symbol(op.str, op.len);

	common_unresolved_list_put(emitter->pos, label, blt, NULL);
	return blt;
}

int get_register_list(char *str, int length) {
	int register_list = 0x0;
	int tmp, reg, tmp_reg,pre_reg,len;
	int flag = 0;
	len = length;
	if(is_braces(&str[len])) {
		tmp = skip_braces(&str[len]);
		len += tmp;
		while(is_braces(&str[len]) == 0) {
			tmp_reg = 0x1;
			tmp = parse_register(&str[len], &reg);
			len += tmp;
			if(flag== 1) {
				for(int i=pre_reg+1;i<=reg; i++) {
					tmp_reg = 0x1;
					tmp_reg = tmp_reg << i;
					register_list += tmp_reg;
				}
				flag = 0;
			} else {
				tmp_reg = tmp_reg << reg;
				register_list += tmp_reg;
			}
			pre_reg = reg;
			if(is_one_char(&str[len], '-')) {
				tmp = skip_one_char(&str[len], '-');
				len += tmp;
				flag = 1;
			} else if(is_braces(&str[len])) {
				break;
			} else {
				tmp = skip_comma(&str[len]);
				len += tmp;
			}
		}
	}
	return register_list;
}

int asm_ldmia(char *str) {
	// Data processing P48 LDMIA
	// rn             0x000f0000
	// register list  0x0000ffff
	int rn, register_list, tmp;
	int len = 0;
	tmp = parse_register(&str[len], &rn);
	rn = rn << 16;
	if(0 >= tmp) return 0;
	len += tmp;
	if(is_one_char(&str[len], '!')) {
		tmp = skip_one_char(&str[len], '!');
		len += tmp;
		tmp = skip_comma(&str[len]);
		if(0 >= tmp) return 0;
		len += tmp;
		register_list = get_register_list(str, len);
	}
	int ldmia = 0xe8b00000;
	ldmia += rn;
	ldmia += register_list;
	return ldmia;
}


int asm_stmdb(char *str) {
	// Data processing P48 STMDB
	// rn             0x000f0000
	// register list  0x0000ffff
	int rn, register_list, tmp;
	int len = 0;
	tmp = parse_register(&str[len], &rn);
	rn = rn << 16;
	if(0 >= tmp) return 0;
	len += tmp;
	if(is_one_char(&str[len], '!')) {
		tmp = skip_one_char(&str[len], '!');
		len += tmp;
		tmp = skip_comma(&str[len]);
		if(0 >= tmp) return 0;
		len += tmp;
		register_list = get_register_list(str, len);
	}
	int stmdb = 0xe9200000;
	stmdb += rn;
	stmdb += register_list;
	return stmdb;
}

int asm_mov(char *str) {
	// Data processing P29 MOV
	// rd       0x0000f000
	// operand  0x00000fff
	int rd,operand;
	int len = 0, tmp;
	int mov;
	
	tmp = parse_register(&str[len], &rd);
	if(0 >= tmp) return 0;
	len += tmp;
	tmp = skip_comma(&str[len]);
	if(0 >= tmp) return 0;
	len += tmp;
	if(is_register(&str[len])) {
		tmp = parse_register(&str[len], &operand);
		mov = 0xe1a00000;
	} else {
		tmp = parse_immediate(&str[len], &operand, '#');
		mov = 0xe3a00000;
	}

	rd = rd << 12;
	mov += rd;
	mov += operand;
	
	return mov;
}

int asm_cmp(char *str) {
	// Data processing P29 CMP
	// rn       0x000f0000
	// operand  0x00000fff
	int rn,operand;
	int len = 0, tmp;
	
	tmp = parse_register(&str[len], &rn);
	if(0 >= tmp) return 0;
	len += tmp;
	tmp = skip_comma(&str[len]);
	if(0 >= tmp) return 0;
	len += tmp;
	tmp = parse_immediate(&str[len], &operand, '#');
	int cmp = 0xe3500000;
	rn = rn << 16;
	cmp += rn;
	cmp += operand;
	
	return cmp;
}

int asm_add(char *str) {
	// Data processing P29 ADD
	// rn       0x000f0000
	// rd       0x0000f000
	// operand  0x00000fff
	int rd,rn,operand;
	int len = 0, tmp;
	// rd
	tmp = parse_register(&str[len], &rd);
	if(0 >= tmp) return 0;
	len += tmp;
	// ,
	tmp = skip_comma(&str[len]);
	if(0 >= tmp) return 0;
	len += tmp;
	// rn
	tmp = parse_register(&str[len], &rn);
	if(0 >= tmp) return 0;
	len += tmp;
	// ,
	tmp = skip_comma(&str[len]);
	if(0 >= tmp) return 0;
	len += tmp;
	// operand
	tmp = parse_immediate(&str[len], &operand, '#');
	int add = 0xe2800000;
	rn = rn << 16;
	rd = rd << 12;
	add += rd;
	add += rn;
	add += operand;
	
	return add;
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
		int code = 0xe59f0000;
		code = code + rd;
		rn = 15;
		rn = rn << 16;
		
		if(is_equals_next_number(&str[len])) {
			// 即値の場合
			int number;
			int length = parse_immediate(&str[len], &number, '=');
			common_unresolved_list_put(emitter->pos, -1, code, number);
		} else {
			//　ラベルの場合
			int label_len = parse_string(&str[len], '=', &label);
			int label_number = to_label_symbol(label, label_len);
			common_unresolved_list_put(emitter->pos, label_number, code, NULL);
		}
	
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
			tmp = parse_immediate(&str[len], &offset, '#');
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

int asm_ldrb(char *str, struct Emitter *emitter) {
	int rn,rd, offset;
	asm_common_str_ldr(str, &rn, &rd, &offset, emitter);
	int ldrb = 0xe5d00000;
	
	ldrb += rn;
	ldrb += rd;
	ldrb += offset;
	return ldrb;
}

void debug_emitter_dump() {
	int j = 0;
	for(int i = 0; i< emitter.pos; i = i+4) {
		printf("%02x %02x%02x%02x%02x\n",j,emitter.buf[i+3],emitter.buf[i+2],
			emitter.buf[i+1],emitter.buf[i]);
		j+=4;
	}
}


static void test_assemble_mov() {
	FILE *fp = NULL;
	char *file_name = "./test/unit_test/test_cl_utils.ks";
	if((fp=fopen(file_name, "r"))==NULL){
		fprintf(stderr, "エラー: ファイルがオープンできません: %s\n", file_name);
		exit(EXIT_FAILURE);
	}
	cl_file_set_fp(fp);
	assemble("./output/test.bin");
	fclose(fp);
	int expect[8] = {0x02, 0x10, 0xa0, 0xe1, 0x04, 0x20, 0xa0, 0xe1};
	
	for(int i=0; i<emitter.pos; i++) {
		assert_number(expect[i], emitter.buf[i]);
	}
}

static void test_asm_one() {
	emitter.pos = 0;
	char *input = "mov r1, r2";
	int expect = 0xe1a01002;
	
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
	int expect = 0xe1a01002;
	
	int actual = asm_one(input);
	
	assert_number(expect, actual);	
}

static void test_asm_mov() {
	char *input = "    r1, r2";
	int expect = 0xe1a01002;
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
	emitter.byte_remain = -1;
	emitter.pos = 0;
	char *input = "st\n\\\"ring";
	int expect_pos = 10;
	int expect[10] ={0x73, 0x74, 0x0a, 0x5c, 0x22, 0x72, 0x69, 0x6e, 0x67,0x00};
	debug_emitter_dump();
	emit_embedded(&emitter, input, 9);
	assert_number(expect_pos, emitter.pos);
	for(int i=0; i<10; i++) {
		assert_number(expect[i],emitter.buf[i]);
	}
}

static void test_two_emit_embedded() {
	emitter.byte_remain = -1;
	emitter.pos = 0;
	char *input = "st\n\\\"ring";
	char *input2 = "str";
	int expect_pos = 14;
	int expect[13] ={0x73, 0x74, 0x0a, 0x5c, 0x22, 0x72, 0x69, 0x6e, 0x67,0x00, 0x73,0x74, 0x72};
	//int expect[13] ={0x5c, 0x0a, 0x74, 0x73, 0x6e, 0x69, 0x72, 0x22, 0x74,0x73, 0x0,0x67, 0x72};
	
	emit_embedded(&emitter, input, 9);
	emit_embedded(&emitter, input2, 3);
	assert_number(expect_pos, emitter.pos);
	for(int i=0; i<13; i++) {
		assert_number(expect[i],emitter.buf[i]);
	}
}

static void test_emit_embedded_word() {
	emitter.byte_remain = -1;
	emitter.pos = 0;
	char *input = "st\n\\\"ring";
	int  input2 = 0x101f1000;
	int expect_pos = 16;
	int expect[16] ={0x73, 0x74, 0x0a, 0x5c, 0x22, 0x72, 0x69, 0x6e, 0x67,0x0, 0x0, 0x0,0x00, 0x10, 0x1f, 0x10};
	
	emit_embedded(&emitter, input, 9);
	emit_word(&emitter, input2);

	assert_number(expect_pos, emitter.pos);
	for(int i=0; i<16; i++) {
		assert_number(expect[i],emitter.buf[i]);
	}
}

static void test_asm_raw_string() {
	emitter.pos = 0;
	emitter.byte_remain = -1;
	char *input;
	FILE *fp = NULL;
	char *file_name = "./test/unit_test/test_parser_raw_string.ks";
	if((fp=fopen(file_name, "r"))==NULL){
		fprintf(stderr, "エラー: ファイルがオープンできません: %s\n", file_name);
		exit(EXIT_FAILURE);
	}
	cl_file_set_fp(fp);
	
	int expect_pos = 10;
	int expect[10] ={0x73, 0x74, 0x0a, 0x5c, 0x22, 0x72, 0x69, 0x6e, 0x67, 0x0};
	
	cl_getline(&input);
	asm_raw(input, &emitter);
	fclose(fp);

	assert_number(expect_pos, emitter.pos);
	for(int i=0; i<10; i++) {
		assert_number(expect[i],emitter.buf[i]);
	}
	
}


static void test_address_fix() {
	emitter.byte_remain = -1;
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
	emitter.byte_remain = -1;
	unresolved_list_init();
	dict_init();
	emitter.pos = 0;
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
	emitter.byte_remain = -1;
	emitter.pos = 0;
	dict_init();
	unresolved_list_init();
	
	char *input  = "ldr r1, =label";
	char *input2 = "label:";
	char *input3 = ".raw 0x101f1000";
	int expect[4] = {0x00, 0x10, 0x9f, 0xe5};
	int expect2[4] = {0x04, 0x00, 0x01, 0x00};
	
	int code;
	code = asm_one(input);
	emit_word(&emitter, code);
	code = asm_one(input2);
	code = asm_one(input3);
	emit_word(&emitter, code);
	address_fix();
	int i;
	for(i=0; i<4; i++) {
		assert_number(expect[i], emitter.buf[i]);
	}
	for(i=8; i<emitter.pos; i++) {
		assert_number(expect2[i-8], emitter.buf[i]);
	}
}

static void test_address_fix_ldr_string() {
	emitter.byte_remain = -1;
	emitter.pos = 0;
	dict_init();
	unresolved_list_init();
	
	//ldr r1, =label
	//label:
	//.raw "hello\n"
	
	char *input;
	FILE *fp = NULL;
	char *file_name = "./test/unit_test/test_ldr_label.ks";
	if((fp=fopen(file_name, "r"))==NULL){
		fprintf(stderr, "エラー: ファイルがオープンできません: %s\n", file_name);
		exit(EXIT_FAILURE);
	}
	cl_file_set_fp(fp);
	
	int expect[4] = {0x04, 0x10, 0x9f, 0xe5};
	int expect2[4] = {0x04, 0x00, 0x01, 0x00};
	
	cl_getline(&input);
	int code = asm_one(input);
	emit_word(&emitter, code);
	cl_getline(&input);
	code = asm_one(input);
	cl_getline(&input);
	code = asm_one(input);
	address_fix();
	fclose(fp);

	int i;
	for(i=0; i<4; i++) {
		assert_number(expect[i], emitter.buf[i]);
	}
	for(i=12; i<emitter.pos; i++) {
		assert_number(expect2[i-12], emitter.buf[i]);
	}
}

static void test_asm_ldrb() {
	char *input = "    r3, [r1]";
	int expect = 0xe5d13000;
	int actual;
	
	actual = asm_ldrb(input, &emitter);
	assert_number(expect, actual);
}

static void test_asm_add() {
	char *input = "    r1, r2, #0x3";
	int expect = 0xe2821003;
	int actual;
	
	actual = asm_add(input);
	assert_number(expect, actual);
}

static void test_asm_cmp() {
	char *input = "    r3, #0x0";
	int expect = 0xe3530000;
	int actual;
	
	actual = asm_cmp(input);
	assert_number(expect, actual);
}

static void test_asm_bne() {
	unresolved_list_init();
	emitter.pos = 8;
	char *input = "    label";
	
	int label;
	int expect = 0x1a000000;

	struct List actual_list;
	
	int actual = asm_bne(input, &emitter);
	unresolved_list_get(&actual_list);
	label = to_label_symbol("label", 5);
	assert_number(expect, actual);
	assert_number(expect, actual_list.code);
	assert_number(emitter.pos, actual_list.emitter_pos);
	assert_number(label, actual_list.label);
}

static void test_asm_bl() {
	unresolved_list_init();
	emitter.pos = 8;
	char *input = "    label";
	
	int label;
	int expect = 0xeb000000;

	struct List actual_list;
	
	int actual = asm_bl(input, &emitter);
	unresolved_list_get(&actual_list);
	label = to_label_symbol("label", 5);
	assert_number(expect, actual);
	assert_number(expect, actual_list.code);
	assert_number(emitter.pos, actual_list.emitter_pos);
	assert_number(label, actual_list.label);
}

static void test_asm_blt() {
	unresolved_list_init();
	emitter.pos = 8;
	char *input = "    label";
	
	int label;
	int expect = 0xba000000;

	struct List actual_list;
	
	int actual = asm_blt(input, &emitter);
	unresolved_list_get(&actual_list);
	label = to_label_symbol("label", 5);
	assert_number(expect, actual);
	assert_number(expect, actual_list.code);
	assert_number(emitter.pos, actual_list.emitter_pos);
	assert_number(label, actual_list.label);
}

static void test_asm_ldmia() {
	char *input = "    r13!, {r1-r3,r14}";
	int expect = 0xe8bd400e;
	int actual;
	
	actual = asm_ldmia(input);
	assert_number(expect, actual);
}

static void test_asm_stmdb() {
	char *input = "    r13!, {r1-r3,r14}";
	int expect = 0xe92d400e;
	int actual;
	
	actual = asm_stmdb(input);
	assert_number(expect, actual);
}


static void test_ldr_immediate() {
	emitter.buf = g_byte_buf;
	emitter.byte_remain = -1;
	emitter.pos = 0;
	dict_init();
	unresolved_list_init();
	
	char *input = "    r1, =0x101f1000";
	int expect[4]  = { 0xfc, 0x0f, 0x9f, 0xe5 };
	int expect2[4] = { 0x00, 0x10, 0x1f, 0x10 };
	
	int code = asm_ldr(input, &emitter);
	emit_word(&emitter, code);
	address_fix();
	
	int i;
	for(i=0; i<4; i++) {
		assert_number(expect[i], emitter.buf[i]);
	}
	for(i=4; i<emitter.pos; i++) {
		assert_number(expect2[i-4], emitter.buf[i]);
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
	test_two_emit_embedded();
	test_emit_embedded_word();
	test_asm_raw_string();
	test_asm_ldr_label();
	test_address_fix_ldr();
	test_address_fix_ldr_string();
	test_asm_ldrb();
	test_asm_add();
	test_asm_cmp();
	test_asm_bne();
	test_asm_bl();
	test_asm_blt();
	test_asm_ldmia();
	test_asm_stmdb();
	test_ldr_immediate();
}

#if 0
int main(){
	unit_tests();
}
#endif
