/* Header File for CellGen Function
*/

#ifndef CELLGEN_H_INCLUDED
#define CELLGEN_H_INCLUDED

#include "ga-main.h"
#include "cellgen.cpp"

/* CELLGEN(const uint8_t *input, const uint8_t *rule, uint8_t *output)
	Takes in atleast 2 arguments, 3 if exists:
	+ 1-D array of const unsigned char *input
	+ 1-D array of const unsigned char *rule
	+ 1-D array of unsigned char *output (optional)

	+ Using the given rule array, generate Cellular Automaton (CA) grid from given seed input. Handles only 3 nearest-neighbors for now. Calculation done using custom Look-Up-Table (LUT), explained inside CELLFUNC() function, in cellgen.cpp.
	+ If output array is given, assigns the last row of CA to the output. This array is used for bouncing, ie, generating new output from previous results. This is supposed to mimic 2-D CA generation. If output array is not given, a nullptr is assigned by default, and output function is skipped.

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

*/
void cellgen (const uint8_t *input, const uint8_t *rule, uint8_t *output);

#endif
