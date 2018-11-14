/* Main C++ File for Truth Table Functions

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

/* ========== Standard Library Include ========== */

#include <stdio.h>		/* Standard I/O */
#include <stdlib.h>		/* calloc, free */
#include <stdint.h>		/* uint definitions */
#include <iostream>		/* cin, cout */
#include <cstring>		/* strcmp */

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

/* Truth Table Initialization Flag */
static bool INIT = 0;

/* Number of rows to check */
static unsigned int ROW = 1;

/* Input Array */
static uint64_t *INPUT;

/* Output Array */
static uint64_t *OUTPUT;

/* Toggle F1 Metric */
static bool F1 = 1;



/* ========== Read File / Clear Table / Table Status ========== */

void TruthTable::set_table (void) {
	FILE *fp;
	char filename [128];

	/* Get filename */
	printf ("Enter file to read from: ");
	scanf ("%s", filename);

	/* Open file in read mode */
	fp = fopen (filename, "r");
	if (fp == NULL) {
		printf (ANSI_RED "FAILED -- Unable to open file: %s\n" ANSI_RESET, filename);
		return;
	}

	printf ("Parsing CSV... ");

	/* Checks Header Row */
	char buffer [64];

	/* Checks input column header */
	fscanf (fp, "%s", buffer);
	if ( strcmp (buffer, "input") != 0 ) {
		printf (ANSI_RED "FAILED -- Missing input column\n" ANSI_RESET);
		fclose (fp);
		return;
	}

	/* Checks output column header */
	fscanf (fp, "%s", buffer);
	if ( strcmp (buffer, "output") != 0 ) {
		printf (ANSI_RED "FAILED -- Missing output column\n" ANSI_RESET);
		fclose (fp);
		return;
	}

	/* Gets row count */
	fscanf (fp, "%u", &ROW);

	/* Clears any previously set truth table, then calloc row number of items */
	if (INPUT != NULL) free (INPUT);
	INPUT = (uint64_t *) calloc (ROW, sizeof (uint64_t));
	if (OUTPUT != NULL) free (OUTPUT);
	OUTPUT = (uint64_t *) calloc (ROW, sizeof (uint64_t));

	/* Gets value, one-by-one */
	for (unsigned int i = 0; i < ROW; i++) {
		fscanf (fp, "%llx", &INPUT [i]);
		fscanf (fp, "%llx", &OUTPUT [i]);

		/* Unexpected End-of-File Error */
		if ( feof (fp) ) {
			printf (ANSI_RED "FAILED -- Unexpected End of File. Read %d / %d\n" ANSI_RESET,
			i, ROW);
			fclose (fp);
			return;
		}
	}

	/* Print Completion Message */
	printf (ANSI_GREEN "DONE. Parsed %d rows.\n" ANSI_RESET, ROW);

	fclose (fp);
	INIT = 1;
	return;
}

void TruthTable::set_table (const int &input) {
	FILE *fp;
	char filename [128];

	switch (input) {
		case 0x0:
			strcpy (filename, "./tt/0");
			break;

		case 0x1:
			strcpy (filename, "./tt/1");
			break;

		case 0x2:
			strcpy (filename, "./tt/2");
			break;

		case 0x3:
			strcpy (filename, "./tt/3");
			break;

		case 0x4:
			strcpy (filename, "./tt/4");
			break;

		case 0x5:
			strcpy (filename, "./tt/5");
			break;

		case 0x6:
			strcpy (filename, "./tt/6");
			break;

		case 0x7:
			strcpy (filename, "./tt/7");
			break;

		case 0x8:
			strcpy (filename, "./tt/8");
			break;

		case 0x9:
			strcpy (filename, "./tt/9");
			break;

		case 0xA:
			strcpy (filename, "./tt/a");
			break;

		case 0xB:
			strcpy (filename, "./tt/b");
			break;

		case 0xC:
			strcpy (filename, "./tt/c");
			break;

		case 0xD:
			strcpy (filename, "./tt/d");
			break;

		case 0xE:
			strcpy (filename, "./tt/e");
			break;

		case 0xF:
			strcpy (filename, "./tt/f");
			break;

		default:
			strcpy (filename, "./tt/f");
			break;

	}

	/* Open file in read mode */
	fp = fopen (filename, "r");
	if (fp == NULL) {
		printf (ANSI_RED "FAILED -- Unable to open file: %s\n" ANSI_RESET, filename);
		return;
	}

	/* Checks Header Row */
	char buffer [64];

	/* Checks input column header */
	fscanf (fp, "%s", buffer);
	if ( strcmp (buffer, "input") != 0 ) {
		printf (ANSI_RED "FAILED -- Missing input column\n" ANSI_RESET);
		fclose (fp);
		return;
	}

	/* Checks output column header */
	fscanf (fp, "%s", buffer);
	if ( strcmp (buffer, "output") != 0 ) {
		printf (ANSI_RED "FAILED -- Missing output column\n" ANSI_RESET);
		fclose (fp);
		return;
	}

	/* Gets row count */
	fscanf (fp, "%u", &ROW);

	/* Clears any previously set truth table, then calloc row number of items */
	if (INPUT != NULL) free (INPUT);
	INPUT = (uint64_t *) calloc (ROW, sizeof (uint64_t));
	if (OUTPUT != NULL) free (OUTPUT);
	OUTPUT = (uint64_t *) calloc (ROW, sizeof (uint64_t));

	/* Gets value, one-by-one */
	for (unsigned int i = 0; i < ROW; i++) {
		fscanf (fp, "%llx", &INPUT [i]);
		fscanf (fp, "%llx", &OUTPUT [i]);

		/* Unexpected End-of-File Error */
		if ( feof (fp) ) {
			printf (ANSI_RED "FAILED -- Unexpected End of File. Read %d / %d\n" ANSI_RESET,
			i, ROW);
			fclose (fp);
			return;
		}
	}

	fclose (fp);
	INIT = 1;
	return;
}

void TruthTable::clear_table (void) {
		if (INPUT != NULL) free (INPUT);
		if (OUTPUT != NULL) free (OUTPUT);
		INIT = 0;
}

bool TruthTable::table_is_init (void) {
	return INIT;
}



/* ========== F1 ========== */

bool TruthTable::get_f1 (void) {
	return F1;
}

void TruthTable::set_f1 (const bool &set) {
	F1 = set;
}



/* ========== Input / Output Array ========== */

uint64_t *TruthTable::get_input (void) {
	return INPUT;
}

void TruthTable::set_input (const uint64_t *const set_val) {

}

uint64_t *TruthTable::get_output (void) {
	return OUTPUT;
}

void TruthTable::set_output (const uint64_t *const set_val) {

}



/* ========== Truth Table Row Count ========== */

unsigned int TruthTable::get_row (void) {
	return ROW;
}

void TruthTable::set_row (const unsigned int &set_val) {
	ROW = set_val;
}
