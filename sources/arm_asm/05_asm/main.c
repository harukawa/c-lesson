#include "asm.h"


int main(int argc, char *argv[]) {
	FILE *fp = NULL;
	
	if(argc > 1) {
		char *file_name = argv[1];
		if((fp=fopen(file_name, "r"))==NULL){
			printf("error\n");
		}
		cl_file_set_fp(fp);
		assemble("./output/main");
		fclose(fp);
	}

}
