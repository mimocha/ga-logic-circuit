/*	Main C++ file for Cellular Automaton Functions

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

#include "ca.hpp"
#include "ansi.hpp"

/* ===== CA Global Variables =====
	Creates a local copy of global variables, so functions can have access to some variables immediately, without having to pass them around as argument so often.
*/

/* Neighbor Cell Array */
static uint8_t *neighbor;

/* Neighboring Cell Offset */
static int offset;

/* Local Copy of Global Parameters */
static uint16_t color;
static uint16_t dimx;
static uint16_t dimy;
static uint16_t nb_count;

/* Initialization Flag */
static bool ca_init_flag;

using namespace std;



/* ========== STATIC FUNCTION PROTOYPES ==========
	See documentation in header file "ca.hpp"
*/

static bool ca_not_init (void);

static uint8_t ca_func (const uint8_t *const neighbor);

static void ca_print (const uint8_t cell);



/* ========== Miscellaneous Functions ========== */

void ca_init (const unsigned int nbin, const unsigned int color_in,
	const unsigned int dimx_in, const unsigned int dimy_in) {
	printf ("Initializing CA... ");

	/* Allocate memory for neighbor array */
	neighbor = (uint8_t *) calloc (nbin, sizeof(uint8_t));

	/* Calculate index offset for neighboring cells. Only work with odd-numbers */
	offset = ((nbin - 1) / 2);

	/* Creates local copy of global parameters for CA functions */
	nb_count = nbin;
	color = color_in;
	dimx = dimx_in;
	dimy = dimy_in;

	/* Sets initialization flag to TRUE */
	ca_init_flag = 1;

	/* Prints Message and Returns */
	printf (ANSI_GREEN "DONE\n" ANSI_RESET);
	return;
}

void ca_cleanup (void) {
	printf ("Cleaning up CA... ");

	/* Deallocate memory for neighbor array */
	if (neighbor != NULL) free (neighbor);

	/* Sets initialization flag to FALSE
		ca_init() needs to be ran again after this,
		before using CA functions again.
	*/
	ca_init_flag = 0;

	printf (ANSI_GREEN "DONE\n" ANSI_RESET);
	return;
}

bool ca_not_init (void) {
	/* Returns TRUE if CA is uninitialized */
	if (ca_init_flag == 0) {
		printf (ANSI_RED "CA Uninitialized\n" ANSI_RESET);
		return 1;
	}

	/* Otherwise, return FALSE */
	return 0;
}



/* ========== Generation Functions ========== */

uint8_t ca_func (const uint8_t *const neighbor) {
	/* DNA index number */
	uint32_t idx = 0;

	/* Calculates index number value */
	for (unsigned int c = 0; c < nb_count; c++) {
		idx = idx + (neighbor [c] * pow (color, c));
	}

	/* Returns the index number */
	return idx;
}

void ca_gen_row (const uint8_t *const input, uint8_t *const output,
	const uint8_t *const DNA) {
	/* CA Uninitialized Error Catch */
	if ( ca_not_init () ) return;

	/* Iterate over entire row */
	for (unsigned int x = 0 ; x < dimx ; x++) {

		/* Load Neighbor Array && Check Out Of Bound */
		for (uint16_t n = 0 ; n < nb_count ; n++) {
			/* Calculate the index location -- Can be negative */
			int idx = x - offset + n;

			/* If index location is out of bound, set to 0, otherwise return normal values */
			if (idx < 0 || idx >= (int) dimx) {
				neighbor [n] = 0;
			} else {
				neighbor [n] = input [idx];
			}
		}

		/* Call function ca_func() to generate a DNA index */
		const unsigned int dna_index = ca_func (neighbor);

		/* Use the index to access value from DNA string */
		output [x] = DNA [ dna_index ];
	}
}

void ca_gen_grid (uint8_t *const *const grid, const uint8_t *const DNA,
	const uint8_t *const seed) {
	/* CA Uninitialized Error Catch */
	if ( ca_not_init () ) return;

	/* Generates first row from seed, if provided */
	if ( seed != NULL ) {
		ca_gen_row (seed, grid [0], DNA);
	} else {
		ca_gen_row (grid [dimy-1], grid [0], DNA);
	}

	/* Iterates over row in grid */
	for (uint16_t y = 0 ; y < (dimy-1) ; y++) {
		ca_gen_row (grid [y], grid [y+1], DNA);
	}
}



/* ========== Printing Functions ========== */

void ca_print (const uint8_t cell) {
	/* Sets terminal output according to the cell's value */
	switch ( cell ) {
		case 0x0:
			cout << ANSI_RED;
			break;
		case 0x1:
			cout << ANSI_GREEN;
			break;
		case 0x2:
			cout << ANSI_BLUE;
			break;
		case 0x3:
			cout << ANSI_PURPL;
			break;
		case 0x8:
			cout << ANSI_LRED;
			break;
		case 0x9:
			cout << ANSI_LGREEN;
			break;
		case 0xA:
			cout << ANSI_LBLUE;
			break;
		case 0xB:
			cout << ANSI_LPURPL;
			break;
		default: // Undefined
			cout << ANSI_WHITE;
			break;
	}

	/* Prints the cell value in hexadecimal */
	printf("%X", cell);
}

void ca_print_row (const uint8_t *const array) {
	/* CA Uninitialized Error Catch */
	if ( ca_not_init () ) return;

	/* Iterate over each cell in row */
	for (unsigned int x = 0 ; x < dimx ; x++) {
		ca_print ( array [x] );
	}

	/* Clears output color */
	cout << ANSI_RESET << '\n';
}

void ca_print_grid (const uint8_t *const *const grid) {
	/* CA Uninitialized Error Catch */
	if ( ca_not_init () ) return;

	/* Iterate over row in grid */
	for (unsigned int y = 0 ; y < dimy ; y++) {
		ca_print_row ( grid [y] );
	}
}
