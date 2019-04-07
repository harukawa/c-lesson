#include "asm.h"


static void two_file_regression(char *expect_name, char *file_name) {
	FILE *input_fp = NULL;
	FILE *output_fp = NULL;
	FILE *expect_fp = NULL;
	
	char input_pass[50] = "./test/test_input/";
	char output_pass[50] = "./test/test_output/";
	char ks[] = ".ks";
	char bin[] = ".bin";
	strcat(input_pass, file_name);
	strcat(input_pass, ks);
	strcat(output_pass, file_name);
	strcat(output_pass, bin);
	
	if((input_fp=fopen(input_pass, "r"))==NULL){
			fprintf(stderr, "エラー: ファイルがオープンできません: %s\n", input_pass);
			exit(EXIT_FAILURE);
	}
	cl_file_set_fp(input_fp);
	assemble(output_pass);
	fclose(input_fp);
	
	if((expect_fp=fopen(expect_name, "rb"))==NULL){
		fprintf(stderr, "エラー: ファイルがオープンできません: %s\n", expect_name);
		exit(EXIT_FAILURE);
	}
	if((output_fp=fopen(output_pass, "rb"))==NULL){
		fprintf(stderr, "エラー: ファイルがオープンできません: %s\n", output_pass);
		exit(EXIT_FAILURE);
	}
	int expect, actual, i=1;
	while(fread(&expect, 4, 1, expect_fp)) {
		//ファイルを最後まで読み込んだら終了する
		if(feof(expect_fp)) {
			break;
		}
		fread(&actual, 4, 1, output_fp);
		if(expect != actual) {
			printf("ファイル:%s\n",output_pass);
			printf("%d:expect %x actual %x\n",i,expect, actual);
		}
		i++;
	}
	fclose(expect_fp);
	fclose(output_fp);
}
static void regression_test() {
	setup_mnemonic();
	two_file_regression("./test/test_expect/test.bin", "test");
	two_file_regression("./test/test_expect/test_hello.bin", "test_hello");
	two_file_regression("./test/test_expect/test_print_loop.bin", "test_print_loop");
	two_file_regression("./test/test_expect/test_print_nomem.bin", "test_print_nomem");
	two_file_regression("./test/test_expect/test_ldr_immediate.bin", "test_ldr_immediate");
	two_file_regression("./test/test_expect/test_putchar_mem.bin", "test_putchar_mem");
}

//#if 0
int main(int argc, char *argv[]) {
	regression_test();
}
//#endif
