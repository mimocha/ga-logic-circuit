/*	Header File for Cellular Automaton Function

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

*/

#ifndef CA_HPP
#define CA_HPP

// ----- Standard Library Definition ----- //

#include <stdio.h>		/* Standard I/O */
#include <stdlib.h>		/* calloc, free, rand, srand */
#include <stdint.h>		/* uint definitions */
#include <math.h>		/* pow, round, ceiling */
#include <iostream>		/* cin, cout */

/* ----- Define Colors for Pretty Colored Outputs ----- */
// https://misc.flogisoft.com/bash/tip_colors_and_formatting
#define ANSI_DFLT	"\e[39m"
#define ANSI_RESET	"\e[0m"

#define ANSI_BOLD	"\e[1m"
#define ANSI_UNDERL	"\e[4m"
#define ANSI_BLINK	"\e[5m"
#define ANSI_REVRS	"\e[7m"
#define ANSI_HIDDEN	"\e[8m"

#define ANSI_BLACK	"\e[0;30m"
#define ANSI_RED	"\e[0;31m"
#define ANSI_GREEN	"\e[0;32m"
#define ANSI_YELLOW	"\e[0;33m"
#define ANSI_BLUE	"\e[0;34m"
#define ANSI_PURPL	"\e[0;35m"
#define ANSI_CYAN	"\e[0;36m"

#define ANSI_GRAY	"\e[1;30m"
#define ANSI_LRED	"\e[1;31m"
#define ANSI_LGREEN	"\e[1;32m"
#define ANSI_LBLUE	"\e[1;34m"
#define ANSI_LPURPL	"\e[1;35m"
#define ANSI_LCYAN	"\e[1;36m"
#define ANSI_WHITE	"\e[1;37m"

/* ===== PUBLIC FUNCTIONS ===== */

/* void ca_init (const unsigned int, const unsigned int, const unsigned int, const unsigned int)
	Initializes CA file's global variable.
	Done once at the beginning of every simulation.

	Takes in 4 global variables, and creates a copy for CA functions.
	In this order:
	Neighbor Count, Color Count, Grid DIMX, Grid DIMY
*/
void ca_init (const unsigned int, const unsigned int, const unsigned int, const unsigned int);

/* void ca_cleanup (void)
	Cleans up any dynamically allocated memory on the CA global level.
	Done once at the end of every simulation.
*/
void ca_cleanup (void);

/* void ca_gen (uint8_t *input, uint8_t *output, const uint8_t *DNA)
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
void ca_gen_row (const uint8_t *input, uint8_t *output, const uint8_t *DNA);

/* ===== STATIC FUNCTIONS ===== */

/* static uint8_t ca_func (const uint8_t *neighbor)
	This function takes in a given neighboring cell array, and converts it into an index number.
	The conversion is done automatically with respect to the CA Color and Neighbor count.
	This algorithm can handle any arbitrary number of Neighbor and Color. (Theoretically)

	How this works:
	The neighbor array is handled as a string of numbers, base 'Color Count'.
	The number string is provided to this function in LSB format.
		ie, neighbor[0] < neighbor[1] < neighbor[2] ...
	The decimal value of this number string is calculated and stored in 'uint32_t idx'.
		uint32_t holds up to 2^32 or ~4.29 billion.
	This index number is returned.
*/
static uint8_t ca_func (const uint8_t *neighbor);

/* static void ca_print (const uint8_t cell)
	Prints a predetermined colored-ASCII character to terminal, based on a cell's value.
*/
static void ca_print (const uint8_t cell);

/* void ca_printrow (const uint8_t *array) //
	Prints a single line of CA Grid, of given length.
*/
void ca_printrow (const uint8_t *array);

/* void print_grid (uint8_t **grid)
	Prints the working array grid onto terminal.
*/
void print_grid (uint8_t **grid);


#endif
