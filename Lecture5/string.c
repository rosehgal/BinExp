/* 
Target of this program is to understand the what are format string
vulnerabilities.

Compilation command:
    gcc -m32 -g string.c

ASLR mode OFF

*/

#include<stdio.h>
#include<string.h>
#include<stdio.h>

int myvar;

int main(int argc, char** argv)
{
    char buffer[100];
    gets(buffer);
    printf(buffer);
    if(myvar)
        printf("Cool you changed it .. :) ");
    return 0;
}
