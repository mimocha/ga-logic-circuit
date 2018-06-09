#include <stdio.h>
#include <stdlib.h>

int main (void) {

	printf("\nHPS File Initialized\n");

	// char buffer[8];
	// char *ptr;
	// printf("Input Sierpinski Size: ");
	// fgets(buffer, 8, stdin);
	// int input = strtol(buffer, &ptr, 10);

	char *cmd;
	system("jtagconfig");

	printf("\nPress anything to continue");
	getchar();

	return 0;
}
