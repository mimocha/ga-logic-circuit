/* Main C++ File for Truth Table Functions
	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

/* ========== Standard Library Include ========== */

#include <stdio.h>		// Standard I/O
#include <stdlib.h>		// calloc, free
#include <stdint.h>		// uint definitions
#include <iostream>		// cin, cout
#include <cstring>		// strcmp



/* ========== Custom File Header ========== */

#include "truth.hpp"
#include "ansi.hpp"



/* ========== Truth Table Variables ==========
	The desired truth table for evaluating circuits.

	Definition  | Input		 | Output     | Time (optional?)
	------------+------------+------------+------------
				| Input    1 | Output   1 | Time Step 1
				| Input    2 | Output   2 | Time Step 2
				| Input    3 | Output   3 | Time Step 3

	There must be only a single input column.
	There must be only a single output column.
	There may be only one single time column?
	All column should have the same length
	Same goes for the number of configurations. A complete truth table is not required.

	Must be set in the main-menu, before simulations.
*/

// Truth Table Initialization Flag
static bool INIT = 0;

// Number of rows to check
static unsigned int ROW = 0;

// Input / Output Array
static uint64_t *INPUT;
static uint64_t *OUTPUT;

// Toggle Evaluation Mode | 0 == Combinational | 1 == Sequential
static bool MODE = 0;

// Truth Table Mask
static uint64_t MASK = 0x1;

// Truth Table Mask Bit Count
static uint64_t MASK_BC = 1;

// Truth Table Max Bit Count -- (Row Count) * (Mask Bit Count)
static unsigned int MAX_BIT = 0;


/* ========== Helper Functions ========== */

uint64_t TruthTable::bitcount64 (uint64_t x) {
	// Code shamelessly copied from https://en.wikipedia.org/wiki/Hamming_weight
	constexpr uint64_t m1  = 0x5555555555555555;
	constexpr uint64_t m2  = 0x3333333333333333;
	constexpr uint64_t m4  = 0x0f0f0f0f0f0f0f0f;

	x -= (x >> 1) & m1;             //put count of each 2 bits into those 2 bits
	x = (x & m2) + ((x >> 2) & m2); //put count of each 4 bits into those 4 bits
	x = (x + (x >> 4)) & m4;        //put count of each 8 bits into those 8 bits
	x += x >>  8;	//put count of each 16 bits into their lowest 8 bits
	x += x >> 16;	//put count of each 32 bits into their lowest 8 bits
	x += x >> 32;	//put count of each 64 bits into their lowest 8 bits
	return x & 0x7f;
}

static void update (void);

void update (void) {
	MASK_BC = TruthTable::bitcount64 (MASK);
	MAX_BIT = ROW*MASK_BC;
}



/* ========== Read File / Clear Table / Table Status ========== */

void TruthTable::set_table (void) {
	FILE *fp;
	char filename [64];

	// Get filename
	printf ("Enter file truth table name: ");
	scanf ("%s", filename);

	// Open file in read mode
	fp = fopen (filename, "r");

	// If that didn't work, try adding prefixes to filename
	if (fp == nullptr) {
		// Prefixes to try opening the files in
		constexpr char names [2][8] = {"./tt/", "./"};

		for (int i = 0 ; i < 2 ; i++) {
			char retry [80];
			strcpy (retry, names[i]);
			strcat (retry, filename);

			fp = fopen (retry, "r");

			if (fp != nullptr) {
				printf (ANSI_YELLOW "Found file at \"%s\"\n" ANSI_RESET, retry);
				break;
			}
		}

		// When all else fails, fail.
		if (fp == nullptr) {
			printf (ANSI_RED "FAILED -- Unable to open file: %s\n" ANSI_RESET, filename);
			return;
		}
	}

	printf ("Parsing CSV... ");

	// Checks Header Row
	char buffer [64];

	// Checks input column header
	fscanf (fp, "%s", buffer);
	if ( strcmp (buffer, "input") != 0 ) {
		printf (ANSI_RED "FAILED -- Missing input column\n" ANSI_RESET);
		fclose (fp);
		return;
	}

	// Checks output column header
	fscanf (fp, "%s", buffer);
	if ( strcmp (buffer, "output") != 0 ) {
		printf (ANSI_RED "FAILED -- Missing output column\n" ANSI_RESET);
		fclose (fp);
		return;
	}

	// Gets row count
	fscanf (fp, "%u", &ROW);

	// Clears any previously set truth table, then calloc row number of items
	free (INPUT);
	free (OUTPUT);
	INPUT = (uint64_t *) calloc (ROW, sizeof (uint64_t));
	OUTPUT = (uint64_t *) calloc (ROW, sizeof (uint64_t));

	// Gets value, one-by-one
	for (unsigned int i = 0; i < ROW; i++) {
		fscanf (fp, "%llx", &INPUT [i]);
		fscanf (fp, "%llx", &OUTPUT [i]);

		// Unexpected End-of-File Error
		if ( feof (fp) ) {
			printf (ANSI_RED "FAILED -- Unexpected End of File. Read %d / %d\n" ANSI_RESET,
			i, ROW);
			fclose (fp);
			return;
		}
	}

	// Update max bit count score
	update ();

	fclose (fp);
	INIT = 1;
	printf (ANSI_GREEN "DONE. Parsed %d rows.\n" ANSI_RESET, ROW);
	return;
}

int TruthTable::auto_set_table (const int &set) {
	FILE *fp;
	char filename [64];

	switch (set) {
		case 0:
			strcpy (filename, "./tt/0");
			break;
		case 1:
			strcpy (filename, "./tt/1");
			break;
		case 2:
			strcpy (filename, "./tt/2");
			break;
		case 3:
			strcpy (filename, "./tt/3");
			break;
		case 4:
			strcpy (filename, "./tt/4");
			break;
		case 5:
			strcpy (filename, "./tt/5");
			break;
		case 6:
			strcpy (filename, "./tt/6");
			break;
		case 7:
			strcpy (filename, "./tt/7");
			break;
		case 8:
			strcpy (filename, "./tt/8");
			break;
		case 9:
			strcpy (filename, "./tt/9");
			break;
		case 10:
			strcpy (filename, "./tt/a");
			break;
		case 11:
			strcpy (filename, "./tt/b");
			break;
		case 12:
			strcpy (filename, "./tt/c");
			break;
		case 13:
			strcpy (filename, "./tt/d");
			break;
		case 14:
			strcpy (filename, "./tt/e");
			break;
		case 15:
			strcpy (filename, "./tt/f");
			break;
		case 16:
			strcpy (filename, "./tt/sr-nand-latch-1");
			break;
		case 17:
			strcpy (filename, "./tt/sr-nand-latch-2");
			break;
		case 18:
			strcpy (filename, "./tt/sr-nand-latch-3");
			break;
		case 19:
			strcpy (filename, "./tt/sr-nor-latch-1");
			break;
		case 20:
			strcpy (filename, "./tt/sr-nor-latch-2");
			break;
		case 21:
			strcpy (filename, "./tt/count-1bit");
			break;
		case 22:
			strcpy (filename, "./tt/count-2bit");
			break;
		case 23:
			strcpy (filename, "./tt/count-4bit");
			break;
		default:
			printf (ANSI_RED "Unknown Input %d\n", set);
			return -1;
	}

	fp = fopen (filename, "r");
	if (fp == nullptr) {
		printf (ANSI_RED "FAILED -- Unable to open file: %s\n" ANSI_RESET, filename);
		return -1;
	}

	printf ("Parsing CSV... ");

	// Checks Header Row
	char buffer [64];

	// Checks input column header
	fscanf (fp, "%s", buffer);
	if ( strcmp (buffer, "input") != 0 ) {
		printf (ANSI_RED "FAILED -- Missing input column\n" ANSI_RESET);
		fclose (fp);
		return -1;
	}

	// Checks output column header
	fscanf (fp, "%s", buffer);
	if ( strcmp (buffer, "output") != 0 ) {
		printf (ANSI_RED "FAILED -- Missing output column\n" ANSI_RESET);
		fclose (fp);
		return -1;
	}

	// Gets row count
	fscanf (fp, "%u", &ROW);

	// Clears any previously set truth table, then calloc row number of items
	free (INPUT);
	free (OUTPUT);
	INPUT = (uint64_t *) calloc (ROW, sizeof (uint64_t));
	OUTPUT = (uint64_t *) calloc (ROW, sizeof (uint64_t));

	// Gets value, one-by-one
	for (unsigned int i = 0; i < ROW; i++) {
		fscanf (fp, "%llx", &INPUT [i]);
		fscanf (fp, "%llx", &OUTPUT [i]);

		// Unexpected End-of-File Error
		if ( feof (fp) ) {
			printf (ANSI_RED "FAILED -- Unexpected End of File. Read %d / %d\n" ANSI_RESET,
			i, ROW);
			fclose (fp);
			return -1;
		}
	}

	// Update max bit count score
	update ();

	fclose (fp);
	INIT = 1;
	return 1;
}

void TruthTable::clear_table (void) {
		free (INPUT);
		free (OUTPUT);
		INIT = 0;
}

bool TruthTable::table_is_init (void) {
	return INIT;
}



/* ========== Mode ========== */

bool TruthTable::get_mode (void) {
	return MODE;
}

void TruthTable::set_mode (const bool &set) {
	MODE = set;
}



/* ========== Input / Output Array ========== */

uint64_t *TruthTable::get_input (void) {
	return INPUT;
}

void TruthTable::set_input (const uint64_t *const set_val) {
	// Dummy
}

uint64_t *TruthTable::get_output (void) {
	return OUTPUT;
}

void TruthTable::set_output (const uint64_t *const set_val) {
	// Dummy
}



/* ========== Truth Table Row Count ========== */

unsigned int TruthTable::get_row (void) {
	return ROW;
}

void TruthTable::set_row (const unsigned int &set_val) {
	ROW = set_val;
	update ();
}



/* ========== Mask ========== */

uint64_t TruthTable::get_mask (void) {
	return MASK;
}

void TruthTable::set_mask (const uint64_t &set_val) {
	MASK = set_val;
	update ();
}

uint64_t TruthTable::get_mask_bc (const bool &inverse) {
	if (inverse) {
		return bitcount64 (~MASK);
	} else {
		return MASK_BC;
	}
}



/* ========== Max Bit Score ========== */

unsigned int TruthTable::get_max_bit (void) {
	return MAX_BIT;
}
