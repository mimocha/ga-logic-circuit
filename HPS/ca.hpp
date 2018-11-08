/* Header File for Cellular Automaton Function

	MIT License

	Copyright (c) 2018 Chawit Leosrisook

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE

	DNA Length Cheat Sheet
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

#ifndef CA_HPP
#define CA_HPP

/* ========== Miscellaneous Functions ========== */

/* void ca_init (const unsigned int nbin, const unsigned int color_in,
	const unsigned int dimx_in, const unsigned int dimy_in)

	Initializes CA file's global variable.
	Done once at the beginning of every simulation.

	Takes in 4 global variables, and creates a copy for CA functions.
	In this order:
	Neighbor Count, Color Count, Grid DIMX, Grid DIMY
*/
void ca_init (const unsigned int nbin, const unsigned int color_in,
	const unsigned int dimx_in, const unsigned int dimy_in);

/* void ca_cleanup (void)
	Cleans up any dynamically allocated memory on the CA global level.
	Done once at the end of every simulation.
*/
void ca_cleanup (void);

/* static bool ca_not_init (void)
	Checks if CA is not initialized.

	Prints error message and returns TRUE when it is NOT initialized.
	Returns FALSE when it IS initialized.

	Use this function as an internal roadblock,
	to prevent usage of certain CA functions without proper initializations.
*/
// static bool ca_not_init (void);

/* bool ca_is_init (void)
	Returns CA initialization status.

	This function is created for access control of the varuable 'ca_init_flag'.
	The variable should only be available within this compilation unit, and not to other files.
*/
bool ca_is_init (void);



/* ========== Generation Functions ========== */

/* static uint8_t ca_func (const uint8_t *const neighbor)
	Takes in a given neighboring cell array, and converts it into an index number.
	The neighbor array is handled as a string of numbers, of base 'Color Count'.
	The number string is provided to this function in LSB format.
		ie, neighbor[0] < neighbor[1] < neighbor[2] ...
	This algorithm can handle any arbitrary number of Neighbor and Color. (Theoretically)
	The decimal value of this number string is calculated and returned as uint32_t.
		uint32_t holds up to 2^32 or ~4.29 billion. -> DNA length of ~4.29 bil.

	Input is a const pointer to const uint8_t.
	The values of the array, and the pointer does not change within this function.
*/
// static uint8_t ca_func (const uint8_t *const neighbor);

/* void ca_gen_row (const uint8_t *const input, uint8_t *const output,
	const uint8_t *const DNA)

	Generates a 1-Dimensional CA array, given a DNA string.
	Works cell-by-cell, generating the output array, using the input array cells.
	Out-of-bound neighbor cells are replaced with zeroes.

	Inputs:
	input is a const pointer to a const uint8_t. Neither the pointer nor the value will change.
	output is a const pointer. The values of the array will change, but the pointer wont.
	DNA is a const pointer to a const uint8_t. Neither the pointer nor the value will change.
*/
void ca_gen_row (const uint8_t *const input, uint8_t *const output,
	const uint8_t *const DNA);

/* void ca_gen_grid (uint8_t *const *const grid, const uint8_t *const DNA,
	const uint8_t *const seed = NULL)

	Generates a 2-Dimensional CA grid, given a dna string.
	A seed is optional.
	If provided, the first row is generated using the seed.
	If not, the first row is generated using the last row of the grid.

	Inputs:
	grid is a double const pointer. The values of the grid will change, but the pointer wont.
	DNA is a const pointer to a const uint8_t. Neither the pointer nor the value will change.
	seed is a const pointer to a const uint8_t. Neither the pointer nor the value will change.
	If seed is not provided, it is defaulted to NULL.
*/
void ca_gen_grid (uint8_t *const *const grid, const uint8_t *const DNA,
	const uint8_t *const seed = NULL);



/* ========== Printing Functions ========== */

/* static void ca_print (const uint8_t cell)
	Prints a predetermined colored-ASCII character to terminal, based on a cell's value.

	Input is a constant unsigned char 'cell'
	The value of 'cell' cannot be changed within this function.

	Does not clear styling at the end.
*/
// static void ca_print (const uint8_t cell);

/* void ca_print_row (const uint8_t *const array)
	Prints a 1-Dimensional CA array.

	Input is a constant pointer to a constant value,
	neither the value pointed to, nor the pointer itself can be changed inside this function.

	Clears styling at the end.
*/
void ca_print_row (const uint8_t *const array);

/* void ca_print_grid (const uint8_t *const *const grid)
	Prints a 2-Dimensional CA array.

	Input is a double constant pointer to a constant value.
	ie: A constant pointer, to a constant pointer, to a constant value.
	None of the pointers, nor the values pointed to can be changed inside this function.

	Does not clear styling at the end (already handled by 'ca_print_row()').
*/
void ca_print_grid (const uint8_t *const *const grid);

#endif
