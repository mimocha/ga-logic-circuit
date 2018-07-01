/* Program to generate truth table of N x M dimensions.
	Assuming:
		Truth Table with N-bit input
		2^N state output
		Each output M-bit wide
		MSB, 1 line per output, 0 beginning of file -> 2^N end of file

	On hold for now.
*/

#include <stdio.h>
#include <string.h>
#include <iostream>

int main (int argc, char* argv) {
	static int flag_n[2], flag_m[2], flag_d[2];
	static int N, M;

	for (int idx=1; idx<argc; idx++) {
		if (strcmp(argv[idx], "-n") == 0) {
			flag_n[0] = 1; // Option "-n" exists
			flag_n[1] = idx; // Option is at array index "idx"
			continue;
		} else if (strcmp(argv[idx], "-m") == 0) {
			flag_m[0] = 1;
			flag_m[1] = idx;
			continue;
		} else if (strcmp(argv[idx], "-d") == 0) {
			flag_d[0] = 1;
			flag_d[1] = idx;
			continue;
		}
	}

	// Option -d: Debug
	if (flag_d[0]) {
		// Debug info
		printf("\n----- Truth Table Generation Begin | argc: %d -----\n", argc);
		for (int i=0; i<argc; i++) {
			std::cout << argv[i] << ' ';
		}
		std::cout << std::endl;
	}

	// Option -n: Input bit
	if (flag_n[0]) { // Load Value
		N = strtol(argv[flag_n[1]+1], NULL, 10);
	} else {
		N = 2;
	}
	if (flag_d[0]) { // Debug Info
		if (flag_n[0] == 0) {
			printf("Input argument -n not found. Default to 2 bit!\n");
		} else if (N <= 0) {
			printf("Invalid argument -n : %d\n", N);
			return -1;
		} else {
			printf("Argument -n: %d\n", N);
		}
	}

	// Option -o: Output bit
	if (flag_m[0]) { // Load Value
		M = strtol(argv[flag_m[1]+1], NULL, 10);
	} else {
		M = 1;
	}
	if (flag_d[0]) { // Debug Info
		if (flag_m[0] == 0) {
			printf("Input argument -m not found. Default to 1 bit!\n");
		} else if (M <= 0) {
			printf("Invalid argument -m : %d\n", M);
			return -1;
		} else {
			printf("Argument -m: %d\n", M);
		}
	}

	FILE *tt;
	tt = fopen ("tt.txt", "w");

	for (int n=0; n<N; n++) {
		for (int m=0; m<M; m++) {

		}

	}

	return 0;
}
