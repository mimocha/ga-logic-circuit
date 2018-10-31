/* Header File for Cellular Automaton Function
*/

#ifndef CA_HPP
#define CA_HPP

/* uint8_t cellfunc (uint8_t *neighbors, const uint8_t *DNA) //
	This function takes in a given neighboring cell array, and converts it into an index number.
	The conversion is done automatically with respect to the CA Color and Neighbor count.
	This algorithm can handle any arbitrary number of Neighbor and Color. (Theoretically)

	How this works:
	The neighbor array is handled as a string of numbers in base 'global.CA.COLOR'.
	The number string is provided to this function in LSB format.
		ie, nb[0] < nb[1] < nb[2] ...
	The decimal value of this number string is calculated and stored in 'uint32_t idx'.
		uint32_t holds up to 2^32 or ~4.29 billion.
	This index number is used to access, and return the value of the given LUT.
*/
uint8_t cellfunc (uint8_t *nb, const uint8_t *DNA);

/* void cellgen (uint8_t *input, uint8_t *output, const uint8_t *DNA) //
	This function applies the rules of a 1D Cellular Automaton.
	Working row by row, this function takes the previous CA row to generate the next CA row,
	given a CA rule set - in this case, a DNA string.

	This function can only handle odd-numbered neighbor counts.
	Out-of-bound neighbor cells are replaced with zeroes.

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
void cellgen (const uint8_t *input, uint8_t *output, const uint8_t *DNA);

/* void cellprint (const uint8_t cell) //
	Prints a predetermined colored-ASCII character to terminal, based on a cell's value.
*/
void cellprint (const uint8_t cell);

/* void ca_graph (const uint8_t *array, const unsigned int length) //
	Prints a single line of CA Grid, of given length.
*/
void ca_graph (const uint8_t *array, const unsigned int length);

#include "ca.cpp"

#endif
