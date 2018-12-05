/* Header File for Cellular Automaton Function
	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

#ifndef CA_HPP
#define CA_HPP

/* ========== Miscellaneous Functions ========== */

/* void ca_init (void)
	Initializes CA file's global variable.
	Done once at the beginning of every simulation.
*/
void ca_init (void);

/* void ca_cleanup (void)
	Dummy function, does nothing for now.
*/
void ca_cleanup (void);

/* bool ca_is_init (void)
	Returns CA initialization status.

	This function is created for access control of the varuable 'ca_init_flag'.
	The variable should only be available within this compilation unit, and not to other files.
*/
bool ca_is_init (void);

/* void ca_need_update (void)
	Used to reset initialization flag, when related parameters are changed,
	and CA local variables need updating.
*/
void ca_need_update (void);


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

/* static void ca_gen_row
	(const uint8_t *const input, uint8_t *const output, const uint8_t *const DNA)

	Generates a 1-Dimensional CA array, given a DNA string.
	Works cell-by-cell, generating the output array, using the input array cells.
	Left-Right Edges loop.

	Inputs:
	input is a const pointer to a const uint8_t. Neither the pointer nor the value will change.
	output is a const pointer. The values of the array will change, but the pointer wont.
	DNA is a const pointer to a const uint8_t. Neither the pointer nor the value will change.
*/

/* void ca_gen_grid
(uint8_t *const *const grid, const uint8_t *const DNA, const uint8_t *const seed = NULL)

	Wrapper function.

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
void ca_gen_grid
(uint8_t *const *const grid, const uint8_t *const DNA, const uint8_t *const seed = NULL);



/* ========== Printing Functions ========== */

/* static void ca_print (const uint8_t &cell)
	Prints a predetermined colored-ASCII character to terminal, based on a cell's value.

	Input is a reference to a const uint8_t 'cell'
	The value of 'cell' may not be changed within this function.

	Does not clear styling at the end.
*/

/* void ca_print_row (const uint8_t *const array)
	Prints a 1-Dimensional CA array.

	Input is a constant pointer to a constant value,
	neither the value pointed to, nor the pointer itself can be changed inside this function.

	Clears styling at the end.
*/
void ca_print_row (const uint8_t *const array);

/* void ca_print_grid (const uint8_t *const *const grid, const bool &invert)
	Wrapper function.
	Prints a 2-Dimensional CA array.

	Input 'grid' is a double constant pointer to a constant value.
	ie: A constant pointer, to a constant pointer, to a constant value.
	None of the pointers, nor the values pointed to can be changed inside this function.

	Input 'invert' is a constant bool. Its value may not change.
	Invert flips the direction of printing when set (invert == 1).
	Invert is set to 0 by default (prints from top down)

	Does not clear styling at the end (already handled by 'ca_print_row()').
*/
void ca_print_grid (const uint8_t *const *const grid, const bool &invert = 0);

#endif
