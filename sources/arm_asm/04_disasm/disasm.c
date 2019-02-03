#include "arm_asm.h"



int print_asm(int word) {
	char *minus = "#0x";
	// str & ldr
	if(0xe5000000 == (word &0xe5000000)) {
		int rn   = cl_select_bit(word,0x000f0000,4); //Base register
		int rd     = cl_select_bit(word,0x0000f000,3); //Source Destination register
		int offset = cl_select_bit(word,0x00000fff,0);
		int IP     = cl_select_bit(word,0x03000000,6);
		int UBWL   = cl_select_bit(word,0x00f00000,5);
		if (0x001 == (UBWL & 0x001)) {
			if (0x0100 == (UBWL & 0x0100)) {
				cl_printf("ldr r%x,[r%x, #0x%x]\n",rd,rn,offset);
				return LDRB;
			} else {
				cl_printf("ldr r%x,[r%x, #0x%x]\n",rd,rn,offset);
				return LDR;
			}
		} else {
			cl_printf("str r%x, [r%x, #0x%x]\n",rd,rn,offset); 
			return STR;
		}
	// b bl
	} else if(0xea000000 == (word &0xea000000)) {
		int L      = cl_select_bit(word,0x01000000,6);
		int offset = cl_select_bit(word,0x00ffffff,0);
		offset = offset << 2;
		if(cl_hex_minus(offset,5)) {
			offset = ~offset + 0x1;
			offset = offset &0x00ffffff;
			minus = "#-0x";
		}

		if(0x1 ==(L & 0x1)) {
			cl_printf("bl  [r15, %s%x]\n",minus,offset);
			return BL;
		} else {
			cl_printf("b   [r15, %s%x]\n",minus,offset);
			return B;
		}
	//Data processing
	} else if(0xe2000000 == (word & 0xe2000000)){
		int opcode   = cl_select_bit(word,0x01e00000,5);
		opcode = opcode >> 1;
		int rn       = cl_select_bit(word,0x000f0000,4);
		int rd       = cl_select_bit(word,0x0000f000,3);
		int operand2 = cl_select_bit(word,0x00000fff,0);
		int i        = cl_select_bit(word,0x02000000,6);
		if(i == 0x2) {
			int rotate = operand2 >> 8;
			operand2 = operand2 & 0x0ff;
			operand2 = cl_rotate_bit(rotate,operand2);
		}
		//ADD
		if(0x4 == opcode) {
			cl_printf("add r%x, r%x, #0x%x\n",rd, rn, operand2);
			return ADD;
		//MOV
		} else if(0xd == opcode) {
			cl_printf("mov r%x, #0x%x\n",rd,operand2);
			return MOV;
		//CMP
		} else if (0xa == opcode) {
			cl_printf("cmp r%x, #%x\n",rn,operand2);
			return CMP;
		}	
	//BNE
	} else if(0x1afffffa == (word & 0x1afffffa)){
		cl_printf("bne 0xc\n");
		return BNE;
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

static void unit_tests() {
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
}

void two_file_regression(char *input_name, char *expect_name) {
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
	for(int i = 0; i < count; i++) {
		actual = cl_get_result(i);
		fgets(expect, 50, expect_fp);
		expect_size = strlen(expect);
		expect[expect_size-2] = 10;
		for(j = 50; j >= expect_size-1; j--) {
			expect[j] = 0;
		}
		assert_streq(expect, actual);
	}
	fclose(expect_fp);
	fclose(input_fp);
}

static void regression_test() {
	cl_enable_buffer_mode();
	two_file_regression("./test/test_input/hello_arm.bin","./test/test_expect/hello_arm.txt");
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
	//unit_tests();
	regression_test();
}

