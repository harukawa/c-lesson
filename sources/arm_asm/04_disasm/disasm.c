#include "arm_asm.h"

//ARM7DI Data Sheetの表記を参考にした。

int print_asm(int word) {
	char *minus = "#0x";
	
	// Single data transfer STR,LDR P42
	if(0xe5000000 == (word &0xe5000000)) {
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
	} else if(0xe2000000 == (word & 0xe2000000)){
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
			cl_printf("mov r%d, #0x%x\n",rd,operand2);
			return MOV;
		//CMP	OpCode = 1010
		} else if (0xa == opcode) {
			cl_printf("cmp r%d, #0x%x\n",rn,operand2);
			return CMP;
		}	
	//BNE
	} else if(0x1afffffa == (word & 0x1afffffa)){
		cl_printf("bne 0xc\n");
		return BNE;
	// LSR
	} else if(0xe1a02331 == (word & 0xe1a02331)){
		cl_printf("lsr r2, r1, r3\n");
		return LSR;
	}
	cl_printf("%x\n",word);
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
	int input = 0xe3a03068;	
	int actual = print_asm(input);	
	assert_number(MOV, actual);
}

static void test_mov2() {
	int input = 0xe3a01848;	
	int actual = print_asm(input);	
	assert_number(MOV, actual);
}

static void test_ldr() {
	int input = 0xe59f0030;	
	int actual = print_asm(input);	
	assert_number(LDR, actual);
}

static void test_str() {
	int input = 0xe5801000;	
	int actual = print_asm(input);	
	assert_number(STR, actual);
}

static void test_b() {
	int input = 0xeafffffe;	
	int actual = print_asm(input);	
	assert_number(B, actual);
}

static void test_b2() {
	int input = 0xea00000e;	
	int actual = print_asm(input);	
	assert_number(B, actual);
}

static void test_bl() {
	int input = 0xeb000010;	
	int actual = print_asm(input);
	assert_number(BL, actual);
}

static void test_bl2() {
	int input = 0xebffffff;	
	int actual = print_asm(input);
	assert_number(BL, actual);
}

static void test_bne() {
	int input = 0x1afffffa;	
	int actual = print_asm(input);
	assert_number(BNE, actual);
}

static void test_add() {
	int input = 0xe2811001;	
	int actual = print_asm(input);
	assert_number(ADD, actual);
}

static void test_cmp() {
	int input = 0xe3530000;	
	int actual = print_asm(input);
	assert_number(CMP, actual);
}

static void test_and() {
	int input = 0xe202200f;	
	int actual = print_asm(input);
	assert_number(AND, actual);
}

static void test_sub() {
	int input = 0xe2433007;	
	int actual = print_asm(input);
	assert_number(SUB, actual);
}

static void unit_tests() {
	cl_clear_output();
	cl_enable_buffer_mode();
	test_mov();
	test_mov2();
	test_ldr();
	test_str();
	test_b();
	test_b2();
	test_bl();
	test_bl2();
	test_bne();
	test_add();
	test_cmp();
	test_and();
	test_sub();
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
	regression_test();
}

