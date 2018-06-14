/* Main C++ file for evaluation program ran on DE0-SoC HPS system.
*/

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <bitset>
#include <time.h>

time_t test_time;

char getoutput (unsigned short input, unsigned short state);
void delay (int cycles);

int main (int argc, char* argv[]) {

// ----- ----- Setup ----- ----- //

	unsigned short flag_i[2], flag_o[2], flag_d[2], flag_tt[2]; // option flags
	unsigned short input, output; // input-count, output-count
	unsigned char tt; // Truth Table in hexadecimal

	time(&test_time);
	printf("%d\n",test_time);

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
	} else {
		input = 0;
	}
	if (flag_d[0]) { // Debug Info
		if (input <= 0) {
			printf("Invalid argument -i: %d\n", input);
			// return -1;
		} else {
			printf("Argument -i: %d\n", input);
		}
	}

	// Option -o: Output
	if (flag_o[0]) { // Load Value
		output = strtol(argv[flag_o[1]+1], NULL, 10);
	} else {
		output = 0;
	}
	if (flag_d[0]) { // Debug Info
		if (output <= 0) {
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
			std::bitset<4> bit_in (tt);
			std::cout << bit_in << std::endl;
		}
	}

// ----- ----- Evaluation ----- ----- //

	// Put Evaluation Code Here
	unsigned short correct, state = 1;
	unsigned char result;
	// float accuracy = 0;

	// Calculate the number of truth table state (2^input)
	for (short i=0; i<input; i++)
		state *= 2;

	// Runs function getoutput, assign result to char result.
	// To be expanded later -> get output of multiple FPGA circuits
	result = getoutput(input, state);

	correct = ~(result^tt); // count the number of ones

	// accuracy = (float)correct/state;

	if (flag_d[0]) {
		puts("\n Evaluation Results:");
		std::bitset<4> bit_in (tt);
		std::cout << "Expected Output: " << bit_in << std::endl;
		std::bitset<4> bit_out (result);
		std::cout << "Actual Output: " << bit_out << std::endl;
		std::bitset<4> bit_cor (correct);
		std::cout << "Input XNOR Output: " << bit_cor << std::endl;
	}

	// Return results to PC as fitness score

	return 0;
}

char getoutput (unsigned short input, unsigned short state) {
	/* Changes input to FPGA circuit, and records output, bit-by-bit
	-> Concerns about clock speed, and circuit timing?
	-> wait function?
	Returns output in hexadecimal format

	Input - Output Format:
	Input [MSB : LSB] | Output [MSB : LSB]
		[1]	[0] | Output [N]
		--------------------
		0	0	| Output [3]
		0	1	| Output [2]
		1	0	| Output [1]
		1	1	| Output [0]
	*/
	unsigned short i;
	char result = 0;

	for (i=0; i<state; i++) {
		result >>= 1; // Shift 1 bit to the right
		// Set Inputs to state i
		// MSB format
		if ( i%2 == 0 ) {
			result |= 8; // result = result | 1000
		}
	}

	return result;
}

void delay (int cycles) {
	for (int i=cycles; i!=0; i--) {
		// No Operation
	}
}
