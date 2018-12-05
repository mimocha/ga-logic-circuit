/*	Main C++ File for Cellular Automaton Functions
	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

/* ========== Standard Library Include ========== */

#include <stdio.h>		// printf, perror
#include <stdint.h>		// uint definitions
#include <iostream>		// cout



/* ========== Custom Header Include ========== */

#include "ca.hpp"
#include "ansi.hpp"
#include "fast.hpp"
#include "global.hpp"



/* ========== CA Global Variables ========== */

// Local Copy of Global Parameters
static uint16_t color = 4;
static uint16_t dimx = 64;
static uint16_t dimy = 64;
static uint16_t nb_count = 3;

// Neighboring Cell Offset
static int offset = 1;

// Initialization Flag
static bool ca_init_flag = 0;

using namespace std;
using namespace GlobalSettings;



/* ========== Miscellaneous Functions ========== */

void ca_init (void) {
	printf ("Initializing CA... ");

	// Creates local copy of global parameters for CA functions
	dimx = get_ca_dimx ();
	dimy = get_ca_dimy ();
	color = get_ca_color ();
	nb_count = get_ca_nb ();

	// Calculate index offset for neighboring cells. Only work with odd-numbers
	offset = ((nb_count - 1) / 2);

	// Sets initialization flag to TRUE
	ca_init_flag = 1;

	// Prints Message and Returns
	printf (ANSI_GREEN "DONE\n" ANSI_RESET);
	return;
}

void ca_cleanup (void) {
	// Dummy
}

bool ca_is_init (void) {
	return ca_init_flag;
}

void ca_need_update (void) {
	// Resets the flag, when CA related parameters are changed
	ca_init_flag = 0;
}



/* ========== Generation Functions ========== */

uint8_t ca_func (const uint8_t *const input) {
	// DNA index number
	uint32_t idx = 0;

	// Calculates index number value
	for (uint16_t c = 0; c < nb_count; c++) {
		idx = idx + (input [c] * fast_pow (color, c));
	}

	// Returns the index number
	return idx;
}


void ca_gen_row
(const uint8_t *const input, uint8_t *const output, const uint8_t *const DNA) {
	// Iterate over entire row
	for (uint16_t x = 0 ; x < dimx ; x++) {
		// Declare Neighbor Array
		uint8_t neighbor [nb_count];

		for (uint16_t n = 0 ; n < nb_count ; n++) {
			// Calculate the index location -- Can be negative
			int16_t idx = x - offset + n;

			// Index looping
			// Guarantees index of range (0,DIMX]
			// (INDEX + DIMX) % DIMX == LOOPED INDEX
			idx = idx + dimx;
			idx = idx % dimx;

			neighbor [n] = input [idx];
		}

		// Call function ca_func() to generate a DNA index
		const unsigned int dna_index = ca_func (neighbor);

		// Use the index to access value from DNA string
		output [x] = DNA [ dna_index ];
	}
}


void ca_gen_grid
(uint8_t *const *const grid, const uint8_t *const DNA, const uint8_t *const seed) {
	// Generates first row from seed; if not provided, use bottom row as seed
	if ( seed != NULL ) {
		ca_gen_row (seed, grid [0], DNA);
	} else {
		ca_gen_row (grid [dimy-1], grid [0], DNA);
	}

	// Iterates over row in grid
	for (uint16_t y = 0 ; y < (dimy-1) ; y++) {
		ca_gen_row (grid [y], grid [y+1], DNA);
	}
}



/* ========== Printing Functions ========== */

void ca_print (const uint8_t &cell) {
	// Sets terminal output according to the cell's value
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

	printf("%X", cell);
}


void ca_print_row (const uint8_t *const array) {
	// Iterate over each cell in row
	for (unsigned int x = 0 ; x < dimx ; x++) {
		ca_print ( array [x] );
	}

	// Clears output color
	cout << ANSI_RESET << '\n';
}


void ca_print_grid (const uint8_t *const *const grid, const bool &invert) {
	// Iterate over row in grid
	if (invert == 0) {
		for (unsigned int y = 0 ; y < dimy ; y++)
			ca_print_row ( grid [y] );
	} else {
		for (int y = dimy-1 ; y > 0 ; y--)
			ca_print_row ( grid [y] );
	}
}



/* ========== Neigbor Calculation Reference ==========

int16_t idx_zero (const uint16_t &n, const uint16_t &x) {
	// Calculate the index location -- Can be negative
	int16_t idx = x - offset + n;

	// If index location is out of bound, set to 0, otherwise return normal values
	if (idx < 0 || idx >= (int) dimx) {
		return 0;
	} else {
		return idx;
	}
}


int16_t idx_loop (const uint16_t &n, const uint16_t &x) {
	// Calculate the index location -- Can be negative
	int16_t idx = x - offset + n;

	// Index looping
	// Guarantees index of range (0,DIMX]
	// (INDEX + DIMX) % DIMX == LOOPED INDEX

	idx = idx + dimx;
	idx = idx % dimx;

	return idx;
}

*/
