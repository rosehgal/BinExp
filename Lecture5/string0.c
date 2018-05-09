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

int main(int argc, char** argv) {
    char buffer[100];
    strncpy(buffer, argv[1], 100);
    printf(buffer);
    return 0;
}
