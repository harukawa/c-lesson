#include "asm.h"
#if 0
int main(int argc, char *argv[]) {
	FILE *fp = NULL;
	
	if(argc > 1) {
		char *file_name = argv[1];
		if((fp=fopen(file_name, "r"))==NULL){
			fprintf(stderr, "エラー: ファイルがオープンできません: %s\n", file_name);
			exit(EXIT_FAILURE);
		}
		cl_file_set_fp(fp);
		setup_mnemonic();
		assemble("./output/output.bin");
		debug_emitter_dump();
		fclose(fp);
	}

}
#endif
