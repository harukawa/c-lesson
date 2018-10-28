#include <stdio.h>
#include <assert.h>

static const char* const input = "123 456  1203";

void split(char *input, int **output){
	int count = 0,i=0,tmp=0;
	char a;
	while(1){
		a = input[i++];
		if(a == ' '){
			if(tmp != 0){
				output[count++] = tmp;
			}
			tmp = 0;
			continue;
		}

		if(a == '\0'){
			output[count] = tmp;
			break;
		}

		tmp = tmp * 10;
		tmp += a- '0';

	}
}

int main() {
    int answer1 = 0;
    int answer2 = 0;
    int answer3 = 0;

    // write something here.
    int *output[3];

    split(input,output);
    answer1 = output[0];
    answer2 = output[1];
    answer3 = output[2];

    // verity result.
    assert(answer1 == 123);
    assert(answer2 == 456);
    assert(answer3 == 1203);

    return 1;
}
