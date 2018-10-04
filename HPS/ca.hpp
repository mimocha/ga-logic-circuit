/* Header File for Cellular Automaton Function
*/

#ifndef CA_HPP
#define CA_HPP

// ----- Define Colors for Colored CA graph output ----- //
#define ANSI_BLACK	"\033[0;30m"
#define ANSI_RED	"\033[0;31m"
#define ANSI_GREEN	"\033[0;32m"
#define ANSI_YELLOW	"\033[0;33m"
#define ANSI_BLUE	"\033[0;34m"
#define ANSI_PURPLE	"\033[0;35m"
#define ANSI_CYAN	"\033[0;36m"

#define ANSI_GRAY	"\033[1;30m"
#define ANSI_LRED	"\033[1;31m"
#define ANSI_LGREEN	"\033[1;32m"
#define ANSI_LBLUE	"\033[1;34m"
#define ANSI_LPURPLE	"\033[1;35m"
#define ANSI_LCYAN	"\033[1;36m"
#define ANSI_WHITE	"\033[1;37m"

/* CONVERT NAME && DESCRIPTOR */
uint16_t convert (const uint8_t *nb);

/* CELLFUNC NAME && DESCRIPTOR
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
uint8_t cellfunc (const uint8_t *nb, const uint8_t *rule);

/* CELLGEN NAME && DESCRIPTOR
	Takes in 2 arguments:
	+ 1-D array of const unsigned char *input
	+ 1-D array of const unsigned char *rule

	+ Using the given rule array, generate Cellular Automaton (CA) grid from given seed input. Handles only 3 nearest-neighbors for now. Calculation done using custom Look-Up-Table (LUT), explained inside CELLFUNC() function, in cellgen.cpp.

	  Base-K   | Index range == LUT length | LUT permutations
	------------------------------------------
	> Base-2   | 8   | 256
	> Base-3   | 27  | 7 625 597 484 987
	> Base-4   | 64  | 3.402 823 669 e38
	... (LUT permutations grow exponentially) ...
	> Base-8   | 512 | 2.410 312 426 e462
	> Base-10  | 1000 | 10 e1000
	... (Very-large-permutations) ...
	> Base-16  | 4096 | ...
	> Base-32  | 32768 | ...
	> Base-40  | 64000 | ... << Max for 16-bit number
	> Base-64  | 262144 | ...
	> Base-128 | 2-million | ...
	> Base-256 | 16-million | 1.196 e40 403 562
	-------------------------------------------
*/
void cellgen (uint8_t *input, uint8_t *output, const uint8_t *rule);

/* void cellprint (const uint8_t cell)
	Prints a predetermined ASCII character to terminal, based on a cell's value. Seperated as a function for a cleaner code in CELLGEN().
*/
void cellprint (const uint8_t cell);

/* FUNCTION NAME && DESCRIPTOR */
void ca_graph (const uint8_t *array, const unsigned int length);

#include "ca.cpp"

#endif
