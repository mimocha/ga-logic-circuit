/* Header File for Cellular Automaton Function
*/

#ifndef CA_H_INCLUDED
#define CA_H_INCLUDED

/* void cellgen (const uint8_t *input, const uint8_t *rule)
	Takes in atleast 2 arguments, 3 if exists:
	+ 1-D array of const unsigned char *input
	+ 1-D array of const unsigned char *rule
	+ 1-D array of unsigned char *output (optional)

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
void cellgen (const uint8_t *input, const uint8_t *rule);

/* uint8_t cellfunc (const uint8_t *nb, const uint8_t *rule)
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
uint8_t cellfunc (const uint8_t *nb, const uint8_t *rule);

/* uint16_t convert (const uint8_t *nb)
	Converts array nb[3] of base-K into a decimal value.
	Assumes nb[3] is in LSB format.

	uint16_t can handle upto 65535
	-> Which means this function can handle upto 40 colors
*/
uint16_t convert (const uint8_t *nb);

/* void cellprint (const uint8_t cell)
	Prints a predetermined ASCII character to terminal, based on a cell's value. Seperated as a function for a cleaner code in CELLGEN().
*/
void cellprint (const uint8_t cell);

/* void ca_graph (GeneticAlgorithm *pop, const int count)
	Prints the LGA of the top "count" individuals.
*/
void ca_graph (GeneticAlgorithm *pop, const int count);

#include "ga-main.h"
#include "ca.cpp"

#endif
