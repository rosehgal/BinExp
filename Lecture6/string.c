#include <stdio.h>
#include <string.h>
// compile with gcc -m32 temp.c

void foo(char *ch)
{
	char buf[100];
	strncpy(buf, ch, 100);
	printf(buf);
	char *name = strdup(buf);
	printf("\nHello %s", name);	
}

int main(int argc, char** argv) 
{
	foo(argv[1]);
	return 0;
}
