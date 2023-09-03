#include <stdio.h>
#include <stdlib.h>
#include "user/syscall.h"

int main(int argc, char *argv[]){
	int n = atoi(argv[1]);
	int a = n;
	int b = atoi(argv[2]);
	int c = atoi(argv[3]);
	int d = atoi(argv[4]);
	printf("%d %d\n", fibonacci(n), max_of_four_int(a, b, c, d));

	return EXIT_SUCCESS;
}
