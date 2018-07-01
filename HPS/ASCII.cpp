/*	ASCII Test Program
*/

#include "stdio.h"
#include "iostream"

int main (int argc, char **argv) {

	for (int i=0; i<256; i++) {
		printf("%3d: ",i);
		std::cout << (unsigned char)i << std::endl;
	}

	return 1;
}
