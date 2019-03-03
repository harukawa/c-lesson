#include "asm.h"

#define BUF_SIZE 514
static char buf[BUF_SIZE];
static FILE *fp = NULL;

void cl_file_set_fp(FILE *input_fp) {
	fp = input_fp;
}

int cl_getline(char **out_buf) {
	if(fp != NULL) {
		if(fgets(buf, BUF_SIZE, fp) == NULL){
			return 0;
		}
	}
	int len = strlen(buf);
	// 最後の行以外の改行はLF(=10)で行われるため\0に入れ替える
	if(10 == (int)buf[len-1]) {
		buf[len-1] = '\0';
		len = strlen(buf);
	}
	*out_buf = buf;
	return len;
	
}

static void test_getline() {
	char *expect = "mov r1, r2";
	int expect_len = 10;
	char *expect2 = "mov r2,  r4";
	int expect_len2 = 11;
	
	char *actual;
	char *actual2;
	int actual_len, actual_len2;
	actual_len = cl_getline(&actual);
	
	assert_number(expect_len, actual_len);
	assert_streq(expect, actual);
	
	actual_len2 = cl_getline(&actual2);
	
	assert_number(expect_len2, actual_len2);
	assert_streq(expect2, actual2);
}

static void unit_test() {
	char *file_name = "./test/test_cl_utils.s";
	if((fp=fopen(file_name, "r"))==NULL){
		printf("error\n");
	}
	cl_file_set_fp(fp);
	test_getline();
	fclose(fp);
}

#if 0
int main(){
	unit_test();
	return 0;
}
#endif