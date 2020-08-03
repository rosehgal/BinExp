/*
Understand how the fucntion call look like in assembly.
gcc -m32 function_call.c -o function_call -fno-stack-protector
When running under 64-bits machine, use '-m64' instead of '-m32'.
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
