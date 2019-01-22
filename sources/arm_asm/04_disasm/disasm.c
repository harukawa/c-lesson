#include "arm_asm.h"


int print_asm(int word) {
	// mov
	if(0xe3000000 == (word & 0xe3000000)){
		int rd       = cl_select_bit(word,0x0000f000,3);
		int operand2 = cl_select_bit(word,0x00000fff,0);
		if(cl_hex_minus(operand2,2)) {
			operand2 = ~operand2 + 0x00000001;
			operand2 = 0x00000fff & operand2;
			cl_printf("mov r%x, #-0x%x\n",rd,operand2);
		} else {
			cl_printf("mov r%x, #0x%x\n",rd,operand2);
		}
		return MOV;
	// str & ldr
	} else if(0xe5000000 == (word &0xe5000000)) {
		int rn   = cl_select_bit(word,0x000f0000,4); //Base register
		int rd     = cl_select_bit(word,0x0000f000,3); //Source Destination register
		int offset = cl_select_bit(word,0x00000fff,0);
		int IP     = cl_select_bit(word,0x03000000,6);
		int UBWL   = cl_select_bit(word,0x00f00000,5);
		if (0x001 == (UBWL & 0x001)) {
			cl_printf("ldr r%x,[r%x, #0x%x]\n",rd,rn,offset);
			return LDR;
		} else {
			cl_printf("str r%x [r%x, #0x%x]\n",rd,rn,offset); 
			return STR;
		}
	// b bl
	} else if(0xea000000 == (word &0xea000000)) {
		int L      = cl_select_bit(word,0x01000000,6);
		int offset = cl_select_bit(word,0x00ffffff,0);
		char *minus = "#0x";
		offset = offset << 2;
		if(cl_hex_minus(offset,5)) {
			offset = ~offset + 0x1;
			offset = offset &0x00ffffff;
			minus = "#-0x";
		}

		if(0x001 ==(L & 0x001)) {
			cl_printf("bl  [r15, %s%x]\n",minus,offset);
			return BL;
		} else {
			cl_printf("b   [r15, %s%x]\n",minus,offset);
			return B;
		}
	}
	
	cl_printf("%x\n",word);
	return UNKNOWN;
}

void read_file_byte(FILE *input_fp) {
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

static void test_mov() {
	int input = 0xe3a01068;	
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

static void test_bl() {
	int input = 0xe5801000;	
	int actual = print_asm(input);
	assert_number(BL, actual);
}

static void unit_tests() {
	test_mov();
	test_ldr();
	test_str();
	test_b();
}

int main(int argc, char *argv[]) {
	FILE *fp = NULL;
	
	if(argc > 1) {
		char *file_name = argv[1];
		if((fp=fopen(file_name, "rb"))==NULL){
			printf("error\n");
		}
		read_file_byte(fp);
		fclose(fp);
	}
	//unit_tests();
}
