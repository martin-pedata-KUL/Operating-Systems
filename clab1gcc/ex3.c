# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include "ex3.h" //NEEDED FOR DECLARATION. ENSURES THE COMPILER KNOWS THE FUNCTIONS BELOW BEFORE EXECUTION

void stringManipulation() {
	char a[50];
        char b[50];
	printf("Type in your first name: \n");
	scanf("%s", a);
	printf("Type in your family name: \n");
	scanf("%s", b); //NEVER TWO BACK TO BACK SCANS
	char* str = toUppercase(b);
	printf("Your surname in caps: %s \n", str);
}

char* toUppercase(char b[]) {
	int len = strlen(b);
        char* str = (char*)malloc(len+1);
	for (int i = 0; i<len; i++) {
		str[i] = b[i] - 32; 
	}
	str[len] = '\0';
	return str;
}
