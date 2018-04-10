/*
This program is used to understand the assembly language.
gcc -m32 function_call_with_buffer.c -o function_call_with_buffer -fno-stack-protector
*/

#include <stdio.h>
void foo()
{
	char ch[10];
	printf("Calling from fucntion");
}

int main()
{
	foo();
	return 0;
}
