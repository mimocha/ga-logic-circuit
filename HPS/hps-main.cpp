/* Main C++ file for evaluation program ran on DE0-SoC HPS system.
*/

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <bitset>

int evaluate (int input, char tt);

int main (int argc, char* argv[]) {
	static int flag_i[2], flag_o[2], flag_d[2], flag_tt[2]; // option flags
	static int input, output; // input-count, output-count, truth-table
	static char tt;

	/* Option Arguments:
		flag_x[0] == Option "-x" was used
		flag_x[1] == Index location for option "-x"
	*/
	for (int idx=1; idx<argc; idx++) {
		if (strcmp(argv[idx], "-i") == 0) {
			flag_i[0] = 1; // Option "-i" exists
			flag_i[1] = idx; // Option is at array index "idx"
			continue;
		} else if (strcmp(argv[idx], "-o") == 0) {
			flag_o[0] = 1;
			flag_o[1] = idx;
			continue;
		} else if (strcmp(argv[idx], "-tt") == 0) {
			flag_tt[0] = 1;
			flag_tt[1] = idx;
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
		printf("\n----- HPS Initialized | argc: %d -----\n", argc);
		for (int i=0; i<argc; i++) {
			std::cout << argv[i] << ' ';
		}
		std::cout << std::endl;
	}

	// Option -i: Input
	if (flag_i[0]) { // Load Value
		input = strtol(argv[flag_i[1]+1], NULL, 10);
	}
	if (flag_d[0]) { // Debug Info
		if (flag_i[0] == 0) {
			printf("Input argument -i not found!\n");
			// return -1;
		} else if (input <= 0) {
			printf("Invalid argument -i: %d\n", input);
			// return -1;
		} else {
			printf("Argument -i: %d\n", input);
		}
	}

	// Option -o: Output
	if (flag_o[0]) { // Load Value
		output = strtol(argv[flag_o[1]+1], NULL, 10);
	}
	if (flag_d[0]) { // Debug Info
		if (flag_o[0] == 0) {
			printf("Input argument -o not found!\n");
			// return -1;
		} else if (output <= 0) {
			printf("Invalid argument -o: %d\n", output);
			// return -1;
		} else {
			printf("Argument -o: %d\n", output);
		}
	}

	// Option -tt: Truth Table
	if (flag_tt[0]) { // Load Value
		// Load truth table as hexadecimal 0-F
		// Quick & dirty code for now,
		// Handles 2-bit input, 1-bit output = 4 x 1-bit output
		tt = strtol(argv[flag_tt[1]+1], NULL, 16);
	} else {
		tt = 0;
	}
	if (flag_d[0]) { // Debug Info
		if (flag_tt[0] == 0) {
			printf("Input argument -tt not found. Default to: 0000.\n");
		} else {
			printf("Argument -tt: ");
			std::bitset<4> foo (tt);
			std::cout << foo << std::endl;
		}
	}

	// Put Evaluation Code Here
	evaluate(input, tt);

	// Return results to PC as fitness score

	return 0;
}

int evaluate (int input, char tt) {
	// Evaluates 2^input state
}
