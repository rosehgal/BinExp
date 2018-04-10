/*
Understand how the fucntion call look like in assembly.
gcc -m32 function_call.c -o fucntion_call -fno-stack-protector
*/

#include <stdio.h>
#include <unistd.h>

void foo(){
	printf("Printing from foo");
}

int main(){
	int x;
	foo();
	return 0;
}

