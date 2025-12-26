#include "hellofunc.h"

int main (int argc, char **argv) {
	if (argc == 2) myPrintHelloMake (argv[1]);
	else myPrintHelloMake ("nobody");
	return 0;
}
