#include "clesson.h"
#include <stdio.h>
#if 0
int main(int argc, char *argv[]) {
	FILE *fp = NULL;
	register_primitives();
	if(argc > 1){
		char *file_name = argv[1];
		if( (fp = fopen(file_name, "r") ) == NULL){
			printf("ERROR\n");
		}
		cl_getc_set_fp(fp);
	
		eval();
		stack_print_all();
		fclose(fp);
	}
}
#endif
