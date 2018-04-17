/*
The target of this exdrcise is to change the control flow to one of the fucntion
named as target.

Compilation command:
gcc -m32 -g -fno-stack-protector second.c -o second -Wimplicit-function-declaration

Arguments used:
	-m32: instrcuting the compiler to build 32 bit Binary.
	-g: This allow C code to visible in gdb.
	-fno-stack-protector: Disbale stack protection by canary.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void target()
{
    printf("code flow successfully changed\n");
}

int main(int argc, char **argv)
{
    int (*fp)();
    char buffer[64];

    fp = 0;

    gets(buffer);

    if(fp) {
        printf("calling function pointer, jumping to 0x%08x\n", fp);
        fp();
    }
    return 0;
}

