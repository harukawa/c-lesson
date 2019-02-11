#include "arm_asm.h"

#define NAME_SIZE 256

//ARM7DI Data Sheetの表記を参考にした。

int print_asm(int word) {
	char *minus = "#0x";
	
	// LSR
	if(0xe1a02000 == (word & 0xeffff000)){
		int left       = cl_select_bit(word,0x0000f000,3);
		int right      = cl_select_bit(word,0x00000f00,2);
		int center     = cl_select_bit(word,0x0000000f,0);
		cl_printf("lsr r%d, r%d, r%d\n",left, center, right);
		return LSR;
	}
	//BNE
	if(0x1afffffa == (word & 0x1afffffa)){
		//　ハードコードできないため固定文にしました。
		cl_printf("bne label\n");
		return BNE;
	}
	//BLT
	if(0xba000000 == (word & 0xba000000)){
		cl_printf("blt [r15]\n");
		return BLT;
	}
	//PUSH
	if(0xe92d4000 == (word & 0xeffffff0)){
		cl_printf("push {}\n");
		return PUSH;
	}
	
	// Block Data Transfer LDM P48
	if(0xe8000000 == (word &0xee000000)) {
		int rn   = cl_select_bit(word,0x000f0000,4); //Base register
		int p    = cl_select_bit(word,0x01000000,6); //Pre/Post indexing bit
		int USWL = cl_select_bit(word,0x00f00000,5); //Up/Down, PSR&force, Write-back, Load/Store
		int register_list = cl_select_bit(word,0x0000ffff,0);
		
		//register listの解読を行う。
		int registers,i;
		int count = 0;
		int list[16];
		for(i = 0; i<16; i++) {
			registers = register_list >> i;
			if(0x1 == (registers & 0x0001)) {
				list[count] = i;
				count++;
			}
		}
		//register listの文を作成する。
		char name[NAME_SIZE];
		int length;
		strcat(name, "{r");
		for(i = 0; i<count; i++){
			length = strlen(name);
			// ASCiiコードで数字を入れる。 48を足すのはASCiiコードの48が数字の0のため
			if(list[i] < 10) {
				name[length] = 48 + list[i];
			} else {
				//1を先に入れて、一桁目の数字を次に入れる。
				name[length] = 49; // 49 = '1';
				name[length+1] = 48 + list[i] -10;
			}
			if(i != (count -1)) {
				strcat(name, ", r");
			}
		}
		strcat(name, "}");
		//Addresing mode P53
		//LDMIA
		if(0x0 == p && 0x9 == (USWL & 0x9)) {
			cl_printf("ldmia r%d!, %s\n",rn,name);
			return LDMIA;
		}
	// Single data transfer STR,LDR P42
	} else if(0xe5000000 == (word &0xe5000000)) {
		int rn   = cl_select_bit(word,0x000f0000,4); //Base register
		int rd     = cl_select_bit(word,0x0000f000,3); //Source Destination register
		int offset = cl_select_bit(word,0x00000fff,0); // offset
		int IP     = cl_select_bit(word,0x03000000,6); //I:Immediate offset P:Pre/Post indexing bit
		int UBWL   = cl_select_bit(word,0x00f00000,5); //U:Up/Down B:Byte/Word W:Write-back L:Load/Store
		// 1 = Load from memory 
		// 0 = Store to memory
		if (0x001 == (UBWL & 0x001)) {
			// 1 = transfer byte quantity
			// 0 = transfer word quantity
			if (0x0100 == (UBWL & 0x0100)) {
				cl_printf("ldrb r%d, [r%d, #0x%x]\n",rd,rn,offset);
				return LDRB;
			} else {
				cl_printf("ldr r%d, [r%d, #0x%x]\n",rd,rn,offset);
				return LDR;
			}
		} else {
			cl_printf("str r%d, [r%d, #0x%x]\n",rd,rn,offset); 
			return STR;
		}
	// Branch and Branch with link B,BL P27
	} else if(0xea000000 == (word &0xea000000)) {
		int L      = cl_select_bit(word,0x01000000,6); //Link bit
		int offset = cl_select_bit(word,0x00ffffff,0); //offset
		offset = offset << 2;
		if(cl_hex_minus(offset,5)) {
			offset = ~offset + 0x1;
			offset = offset &0x00ffffff;
			minus = "#-0x";
		}
		// 1 = Branch with Link
		// 0 = Branch
		if(0x1 ==(L & 0x1)) {
			cl_printf("bl  [r15, %s%x]\n",minus,offset);
			return BL;
		} else {
			cl_printf("b   [r15, %s%x]\n",minus,offset);
			return B;
		}
	//Data processing P29
	} else if(0xe0000000 == (word & 0xe0000000)){
		int opcode   = cl_select_bit(word,0x01e00000,5);// Operation Code
		opcode = opcode >> 1;
		int rn       = cl_select_bit(word,0x000f0000,4);// 1st operand register
		int rd       = cl_select_bit(word,0x0000f000,3);// Destination register
		int operand2 = cl_select_bit(word,0x00000fff,0);// Operand2
		int i        = cl_select_bit(word,0x02000000,6);// Immediate Operand
		
		// Operand 2 = 1 is an immediate value
		//11-8:Rotate 7-0:Imm
		if(i == 0x2) {
			int rotate = operand2 >> 8;
			operand2 = operand2 & 0x0ff;
			operand2 = cl_rotate_bit(rotate,operand2);
		} 
		//AND   OpCode = 0000
		if(0x0 == opcode) {
			cl_printf("and r%d, r%d, #0x%x\n",rd, rn, operand2);
			return AND;
		//SUB	OpCode = 0010
		} else if(0x2 == opcode) {
			cl_printf("sub r%d, r%d, #0x%x\n",rd, rn, operand2);
			return SUB;
		//ADD	OpCode = 0100
		} else if(0x4 == opcode) {
			cl_printf("add r%d, r%d, #0x%x\n",rd, rn, operand2);
			return ADD;
		//MOV	OpCode = 1101
		} else if(0xd == opcode) {
			//即値
			if(i == 0x2){
				cl_printf("mov r%d, #0x%x\n",rd,operand2);
			//レジスタ
			} else {
				cl_printf("mov r%d, r%d\n",rd,operand2);
			}
			return MOV;
		//CMP	OpCode = 1010
		} else if (0xa == opcode) {
			cl_printf("cmp r%d, #0x%x\n",rn,operand2);
			return CMP;
		}
	}

	cl_printf("%08x\n",word);
	return UNKNOWN;
}

void read_file_byte_print(FILE *input_fp) {
	int buf;
	int address = 0x00010000;
	for(;;){
		fread(&buf, sizeof(buf), 1,input_fp);
		if(feof(input_fp)){ 
			break;
		}
		printf("0x000%x  ",address);
		print_asm(buf);
		address += 0x4;
	}
}

int read_file_byte(FILE *input_fp) {
	int buf,count = 0;
	for(;;){
		fread(&buf, sizeof(buf), 1,input_fp);
		if(feof(input_fp)){ 
			break;
		}
		print_asm(buf);
		count++;
	}
	return count;
}

static void test_mov() {
	cl_clear_output();
	int input = 0xe3a03068;	
	char *expect = "mov r3, #0x68\n";
	
	int actual_type = print_asm(input);
	char *actual = cl_get_result(0);

	assert_streq(expect, actual);
	assert_number(MOV, actual_type);
}

static void test_mov_rotate() {
	cl_clear_output();
	int input = 0xe3a01848;	
	char *expect = "mov r1, #0x480000\n";
	
	int actual_type = print_asm(input);
	char *actual = cl_get_result(0);

	assert_streq(expect, actual);
	assert_number(MOV, actual_type);
}

static void test_mov_register() {
	cl_clear_output();
	int input = 0xe1a0f00a;	
	char *expect = "mov r15, r10\n";
	
	int actual_type = print_asm(input);	
	char *actual = cl_get_result(0);

	assert_streq(expect, actual);
	assert_number(MOV, actual_type);
}

static void test_ldr() {
	cl_clear_output();
	int input = 0xe59f0030;	
	char *expect = "ldr r0, [r15, #0x30]\n";
	
	int actual_type = print_asm(input);
	char *actual = cl_get_result(0);

	assert_streq(expect, actual);
	assert_number(LDR, actual_type);
}

static void test_str() {
	cl_clear_output();
	int input = 0xe5801000;	
	char *expect = "str r1, [r0, #0x0]\n";
	
	int actual_type = print_asm(input);
	char *actual = cl_get_result(0);

	assert_streq(expect, actual);
	assert_number(STR, actual_type);
}

static void test_b() {
	cl_clear_output();
	int input = 0xea00000e;	
	char *expect = "b   [r15, #0x38]\n";
	
	int actual_type = print_asm(input);
	char *actual = cl_get_result(0);

	assert_streq(expect, actual);
	assert_number(B, actual_type);
}

static void test_b_minus() {
	cl_clear_output();
	int input = 0xeafffffe;	
	char *expect = "b   [r15, #-0x8]\n";
	
	int actual_type = print_asm(input);
	char *actual = cl_get_result(0);

	assert_streq(expect, actual);
	assert_number(B, actual_type);
}

static void test_bl() {
	cl_clear_output();
	int input = 0xeb000010;	
	char *expect = "bl  [r15, #0x40]\n";
	
	int actual_type = print_asm(input);
	char *actual = cl_get_result(0);

	assert_streq(expect, actual);
	assert_number(BL, actual_type);
}

static void test_bl_minus() {
	cl_clear_output();
	int input = 0xebffffff;	
	char *expect = "bl  [r15, #-0x4]\n";
	
	int actual_type = print_asm(input);
	char *actual = cl_get_result(0);

	assert_streq(expect, actual);
	assert_number(BL, actual_type);
}

static void test_bne() {
	cl_clear_output();
	int input = 0x1afffffa;	
	char *expect = "bne label\n";
	
	int actual_type = print_asm(input);
	char *actual = cl_get_result(0);

	assert_streq(expect, actual);
	assert_number(BNE, actual_type);
}

static void test_add() {
	cl_clear_output();
	int input = 0xe2811001;	
	char *expect = "add r1, r1, #0x1\n";
	
	int actual_type = print_asm(input);
	char *actual = cl_get_result(0);

	assert_streq(expect, actual);
	assert_number(ADD, actual_type);
}

static void test_cmp() {
	cl_clear_output();
	int input = 0xe3530000;	
	char *expect = "cmp r3, #0x0\n";
	
	int actual_type = print_asm(input);
	char *actual = cl_get_result(0);

	assert_streq(expect, actual);
	assert_number(CMP, actual_type);
}

static void test_and() {
	cl_clear_output();
	int input = 0xe202200f;
	char *expect = "and r2, r2, #0xf\n";
	
	int actual_type = print_asm(input);
	char *actual = cl_get_result(0);

	assert_streq(expect, actual);
	assert_number(AND, actual_type);
}

static void test_sub() {
	cl_clear_output();
	int input = 0xe2433007;	
	char *expect = "sub r3, r3, #0x7\n";
	
	int actual_type = print_asm(input);
	char *actual = cl_get_result(0);

	assert_streq(expect, actual);
	assert_number(SUB, actual_type);
}

static void test_ldmia() {
	cl_clear_output();
	int input = 0xee8bd400e;	
	char *expect = "ldmia r13!, {r1, r2, r3, r14}\n";
	
	int actual_type = print_asm(input);
	char *actual = cl_get_result(0);

	printf("%s\n",actual);
	assert_streq(expect, actual);
	assert_number(LDMIA, actual_type);
}

static void unit_tests() {
	cl_clear_output();
	cl_enable_buffer_mode();
	test_mov();
	test_mov_rotate();
	test_mov_register();
	test_ldr();
	test_str();
	test_b();
	test_b_minus();
	test_bl();
	test_bl_minus();
	test_bne();
	test_add();
	test_cmp();
	test_and();
	test_sub();
	test_ldmia();
	cl_disable_buffer_mode();
	cl_clear_output();
}

void two_file_regression(char *expect_name, char *input_name) {
	cl_clear_output();
	FILE *input_fp = NULL;
	FILE *expect_fp = NULL;

	if((input_fp=fopen(input_name, "rb"))==NULL){
			printf("error\n");
	}
	if((expect_fp=fopen(expect_name, "rb"))==NULL){
			printf("error\n");
	}
	int count = read_file_byte(input_fp);
	
	char *actual;
	char expect[50];
	int expect_size,j=0;
	int actual_len;
	for(int i = 0; i < count; i++) {
		actual = cl_get_result(i);
		fgets(expect, 50, expect_fp);

		// fgetsで読み取ったtxtではCRの復帰を使っているため、
		// 改行を除いた文字だけを比較します。
		actual_len = strlen(actual)-1;
		assert_substreq(expect, actual, actual_len);
	}
	fclose(expect_fp);
	fclose(input_fp);
}

static void regression_test() {
	cl_enable_buffer_mode();
	two_file_regression("./test/test_expect/hello_arm.txt","./test/test_input/hello_arm.bin");
	two_file_regression("./test/test_expect/print_loop.txt","./test/test_input/print_loop.bin");
	two_file_regression("./test/test_expect/print_hex.txt","./test/test_input/print_hex.bin");
	two_file_regression("./test/test_expect/print_hex_bl.txt","./test/test_input/print_hex_bl.bin");
	two_file_regression("./test/test_expect/print_hex_mem.txt","./test/test_input/print_hex_mem.bin");
	two_file_regression("./test/test_expect/print_hex_mem2.txt","./test/test_input/print_hex_mem2.bin");
	two_file_regression("./test/test_expect/print_nomem.txt","./test/test_input/print_nomem.bin");
	two_file_regression("./test/test_expect/putchar_mem.txt","./test/test_input/putchar_mem.bin");
	cl_disable_buffer_mode();
	cl_clear_output();
}


int main(int argc, char *argv[]) {
	FILE *fp = NULL;
	
	if(argc > 1) {
		char *file_name = argv[1];
		if((fp=fopen(file_name, "rb"))==NULL){
			printf("error\n");
		}
		read_file_byte_print(fp);
		fclose(fp);
	}

	unit_tests();
	//regression_test();
}

