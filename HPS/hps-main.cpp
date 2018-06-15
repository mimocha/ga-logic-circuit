/* Main C++ file for evaluation program ran on DE0-SoC HPS system.
*/

#include <stdio.h>
#include <string.h>
#include <iostream>

char getoutput (unsigned short state);

int main (int argc, char* argv[]) {

// ----- ----- Setup ----- ----- //

	// Option Flag & input/output-count
	static unsigned short flag_i[2], flag_o[2],
						flag_d[2], flag_tt[2],
						input, output;
	// Truth Table in hexadecimal
	// Expect = Correct Answer | Actual = FPGA Output Answer | XNOR = Check answer
	static unsigned char tt_expect, tt_actual, tt_xnor;

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
	if (flag_d[0] == 1) {
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
		input = 2;
	}
	if (flag_d[0] == 1) { // Debug Info
		if (input <= 0) {
			printf("Invalid argument -i: %d. Default to 2.\n", input);
		} else {
			printf("Argument -i: %d\n", input);
		}
	}

	// Option -o: Output
	if (flag_o[0]) { // Load Value
		output = strtol(argv[flag_o[1]+1], NULL, 10);
	} else {
		output = 1;
	}
	if (flag_d[0] == 1) { // Debug Info
		if (output <= 0) {
			printf("Invalid argument -o: %d. Default to: 1\n", output);
		} else {
			printf("Argument -o: %d\n", output);
		}
	}

	// Option -tt: Truth Table
	if (flag_tt[0]) { // Load Value
		// Load truth table as hexadecimal 0-F
		// Quick & dirty code for now,
		// Handles 2-bit input, 1-bit output = 4 x 1-bit output
		tt_expect = strtol(argv[flag_tt[1]+1], NULL, 16);
	} else {
		tt_expect = 0;
	}
	if (flag_d[0] == 1) { // Debug Info
		if (flag_tt[0] == 0) {
			printf("Input argument -tt not found. Default to: 0000.\n");
		} else {
			printf("Argument -tt: %X\n", tt_expect);
		}
	}

// ----- ----- Evaluation ----- ----- //
	unsigned short correct = 0, state = 4;
	float accuracy;

	// If more than 2 inputs -> set state count
	if (input != 2) {
		state = 1;
		for (int i=0; i<input; i++) {
			state *= 2;
		}
	}

	tt_actual = getoutput(state);

	// XNOR function sets bit which are the same to 1 (aka. A == B)
	tt_xnor = ~(tt_expect ^ tt_actual);

	if ((tt_xnor & 8) != 0) correct++;
	if ((tt_xnor & 4) != 0) correct++;
	if ((tt_xnor & 2) != 0) correct++;
	if ((tt_xnor & 1) != 0) correct++;

	accuracy = (float) correct / state;
	printf("Correct: %d | Accuracy: %.2f\n", correct, accuracy);

	return correct;
}

char getoutput (unsigned short state) {
	/* Changes input to FPGA circuit, and records output, bit-by-bit
	-> Concerns about clock speed, and circuit timing?
	-> wait function?
	Returns output as char, 4-bit result

	Input - Output Format:
	Input [MSB : LSB] | Output [MSB : LSB]
		[1]	[0] | Output [N]
		--------------------
		0	0	| Output [3]
		0	1	| Output [2]
		1	0	| Output [1]
		1	1	| Output [0]

	Return in MSB format
	*/
	volatile unsigned short n, i = 1;
	char result = 0;

	for (n=1; n<=state; n++) {
		if ( 1 ) // Evaluation Condition
			result |= i; // Set result bit "n" to 1
		i *= 2;
	}

	return result;
}
