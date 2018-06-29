/* Main C++ file for Cellular Automaton Generation Function CELLGEN
	Generates a grid of logic gates and connections based on given arguments.
*/

#include "cellgen.h"

inline uint8_t cellfunc (uint8_t nb[], uint8_t *rule);
inline uint16_t convert (uint8_t nb[]);

void cellgen (uint8_t input[], uint8_t *rule) {
	// Define
	uint8_t array[DIM][DIM];
	// array[0][] = input[];
	for (uint8_t i=0; i<DIM; i++)
		array[0][i] = input[i];

	// Calculate each cell
	uint8_t neighbor[3];
	for (uint8_t i=1; i<DIM; i++) {
		for (uint8_t j=0; j<DIM; j++) {
			if (j==0) {
				neighbor[0] = 0;
				neighbor[1] = array[i-1][j];
				neighbor[2] = array[i-1][j+1];
			} else if (j<DIM-1) {
				neighbor[0] = array[i-1][j-1];
				neighbor[1] = array[i-1][j];
				neighbor[2] = array[i-1][j+1];
			} else {
				neighbor[0] = array[i-1][j-1];
				neighbor[1] = array[i-1][j];
				neighbor[2] = 0;
			}
			array[i][j] = cellfunc(neighbor, rule);
		}
		// cout << endl;
	}

	// Print
	// Predefined ASCII Terminal output
	for (uint8_t i=0; i<DIM; i++) {
		for (uint8_t j=0; j<DIM; j++) {
			switch (array[i][j]) {
				case 0: // NULL
					cout << (unsigned char)176;
					break;
				case 1: // NAND
					cout << (unsigned char)219;
					break;
				case 2: // LEFT
					cout << (unsigned char)47;
					break;
				case 3: // RIGHT
					cout << (unsigned char)92;
					break;
				default: // OOB
					cout << 'X';
			}
		}
		cout << endl;
	}
}

// Consider using function strtol() or strtoul() to handle multiple bases.
inline uint8_t cellfunc (uint8_t nb[], uint8_t *rule) {
/* Function CELLFUNC(uint8_t neighbor[3], uint8_t *rule)
	Takes in two inputs:
	+ An array of unsigned char of length 3 - the neighbors
	+ An array of unsigned char of length (K^3) - the rules;
		++ where K is the number of colors, the possible states each cell can be in.

	> Main definition:
	+ "The Neighbors" refer to the 3 cells from the previous row, nearest the currently considered cell. The three cells can be viewed as a sequence of 3 integers, of base-k; where k is the number of possible states each cell can be in.
		** Neighbors are also now in LSB format. See below, under "The Rule" section for an explanation of why.
	+ "The Rule" is a string of length (K^3), of base-k, in **LSB format**.
		++ Each char in this string is referred to as a "rule-bit".
		++ Each "rule-bit" can be any value in the range of [0, (K^3)-1]
		** "rule_bit" is in Least-Significant-Bit format, meaning the smallest digit is on the left-most bit of the string. This is changed to allow easier handling in C-arrays.

	> Example:
	K == 2 -> 2 possible states on each cell.
	nb[3] == {1, 0, 0} -> base-2 LSB (100) -> decimal value (1)
	rule[8] == {0, 1, 1, 0, 0, 0, 0, 1} -> base-2 (0110 0001)
	rule_bit(0) == 0, rule_bit(1) == 1, rule_bit(2) == 1, rule_bit(6) == 0

	> Least-Significant-Bit format
	rule_bit --> 0110 0001
	             ^^^^ ^^^^
	index    --> 0123 4567

	> How this works:
	1. The array nb[3] is converted from base-k to some decimal value as index.
	2. This index number is used to access the string at position index.
	3. The value inside array(index) is the output value.

	+ We can think of "rule" as a Look-Up-Table (LUT), of size K^3, value range [0, K-1]. The permutation of each neighbor determines which array in this LUT to use as output. If we consider only 3 neighbors at a time, there will be a total of K^3 permutations, thus the LUT size.
	+ We can then convert each permutation into some decimal value, so we can access each array on the LUT easily in C programming.

	  Base-K   | Index range == LUT length | LUT permutations
	------------------------------------------
	> Base-2   | 8   | 256
	> Base-3   | 27  | 7 625 597 484 987
	> Base-4   | 64  | 3.402 823 669 e38
	... (LUT permutations grow exponentially) ...
	> Base-8   | 512 | 2.410 312 426 e462
	> Base-10  | 1000 | 10 e1000
	... (Impossibly-large-permutations) ...
	> Base-16  | 4096 | ...
	> Base-32  | 32768 | ...
	> Base-64  | 262144 | ...
	> Base-128 | 2-million | ...
	> Base-256 | 16-million | 1.196 e40 403 562
	-------------------------------------------

	> More Examples:
	K == 3
	nb[3] == {2, 1, 0} -> base-3 LSB (210) -> decimal (5)
	rule[27] == {0000 0122 ...}
	rule_bit(5) == 1

	K == 4
	nb[3] == {1, 2, 3} -> base-4 LSB (321) -> decimal (57)
	rule[64] == {... 0000 1233 3333}
	rule_bit(57) == 2
*/

	// Get index number
	uint16_t idx = convert(nb);

	// Out of Bound Error Catch
	if (idx > K_CUBE) {
		puts ("K_CUBE ERROR CATCH");
		printf("IDX: %d K_CUBE: %d\n", idx, K_CUBE);
		printf("NB: %d %d %d\n", nb[0], nb[1], nb[2]);
		system("pause");
		return -1;
	}

	// Access and return value from LUT
	return rule[idx];
}

inline uint16_t convert (uint8_t nb[]) {
	/* inline uint16_t CONVERT (uint8_t nb[3])
		Converts array nb[3] of base-K into a decimal value.
		Assumes nb[3] is in LSB format.
		Inline specifier helps make this more efficient for the compiler.
	*/
	uint16_t result = 0;
	result += nb[0] + (nb[1]*K) + (nb[2]*K*K);
	return result;
}
