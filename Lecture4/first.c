/*
The target of this exercise is to change the control flow by changing the value
of any variable :) 

Compilation command:
gcc -m32 -g -fno-stack-protector first.c -o first -Wimplicit-function-declaration

Arguments used:
	-m32: instrcuting the compiler to build 32 bit Binary.
	-g: This allow C code to visible in gdb.
	-fno-stack-protector: Disbale stack protection by canary.
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	int modified;
	char buffer[64];

	modified = 0;
	gets(buffer);

	if(modified != 0) {
		printf("you have changed the 'modified' variable\n");
	} else {
		printf("Try again?\n");
	}
}

