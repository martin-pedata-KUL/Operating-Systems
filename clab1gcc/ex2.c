#include <stdio.h>
#include "ex2.h"


void printSizeOf() {
	int a = 5;
	printf("Size of int: %zu \n", sizeof(long int));
        printf("Size of float: %zu \n", sizeof(float));
        printf("Size of double: %zu \n", sizeof(double));
        printf("Size of void: %zu \n", sizeof(void));
        printf("Size of pointer: %zu \n", sizeof(&a));
	return;
}

