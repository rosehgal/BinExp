#include <stdio.h>
#include <stdlib.h>
int main()
{
	void *a, *b, *c, *d, *e, *f;
	a = malloc(32);     
	b = malloc(32);     
	c = malloc(32);     

	printf("%p %p %p\n", a, b, c);
	free(a);
	free(b);  
	free(a);  

	d = malloc(32);     
	e = malloc(32);     
	f = malloc(32);     
	printf("%p %p %p", d, e, f);
	return 0;
}
