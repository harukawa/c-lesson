#include <stdio.h>

void print_address(int address) {
    printf("address: %x\n", address);
}

void print_msg(char *str) {
    printf("We get (%s)\n", str);
}

void func3 () {
    // TODO: get main_msg from here as target someway.
    char *target;
	//func3 target
	print_address(((int)&target));
	
	print_address(((int)&target)+8);
	
	//func2 r11
	print_address(*((int*)((((int)&target)+8) )));
	
	//func2 msg address
	print_address(*((int*)((((int)&target)+8) ))-8);
	
	//*msg
	print_address(*(  (int*) (*((int*)(((int)&target)+ 8)) -8) ));
	printf("func2 %s\n",(*(  (int*) (*((int*)(((int)&target)+ 8)) -8) )));
	
	//func1 r11
	print_address(*((int*)((((int)&target)+8) )) );
	//func1 msg address
	print_address(*(  (int*) (*((int*)(((int)&target)+ 8)) ) ) -8);
	
	//func1 *msg
	print_address (*((int*)(*(  (int*) (*((int*)(((int)&target)+ 8))  ) ) -8)));
	
	printf("func1 %s\n",(*((int*)(*( (int*) (*((int*)(((int)&target)+ 8))   ) ) -8))));
    printf("We are in func3\n");
}

void func2() {
    char *msg = "func2 message.";
    printf("We are in func2, %s\n", msg);
	func3();
}

void func1() {
    char *msg = "func1 msg";
    printf("We are in func1, %s\n", msg);
    func2();
}


int main() {
    char *main_msg = "We are in main.";
    printf("We are in main, %s\n", main_msg);
    func1();
    return 0;
}

