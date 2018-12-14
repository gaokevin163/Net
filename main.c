#include <stdio.h>
#include <stdlib.h>

struct test{
	const char* a;
};




int main(){
	const char* a;
	const char* b ="q";
	b="c";
	struct test s;
	const int c =10;
	c =11;
	//s.a ="a";
	//s.a = malloc(10);
	//strcpy(s.a,"b");
	//s.a ="c";
	//memcpy(s.a,"a",10);
	return 0;
}

