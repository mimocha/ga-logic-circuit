/* Main C++ File for Miscellaneous Functions
	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

/* ========== Standard Library Include ========== */

#include <stdio.h>		/* Standard I/O */
#include <stdint.h>		/* uint definitions */

#include "misc.hpp"
#include "ansi.hpp"
#include "global.hpp"

void about (void) {
	printf ("\n\e[7m\t--- Genetics Algorithm Program Version %4.2f ---\e[0m\n\n", VERSION);

	/* Explanation & License */
	printf (
	"\tThis program applies Genetics Algorithm, Cellular Automaton, and more\n"
	"\tto generate logic circuits. This is create as part of a Bachelor's degree\n"
	"\tdissertation project, at Nippon Institute of Technology, 2018-2019.\n\n"
	"\t\e[7mCopyright (c) 2018 Chawit Leosrisook\e[0m\n"
	"\tCode Repository: https://github.com/mimocha/ga-logic-circuit\n"
	);

}

void input_argument (const int &argc, const char *const *const argv) {
	int counter;

	printf ("%d input argument(s) given:\n", argc);
	for (counter = 0 ; counter < argc ; counter++)
		printf ("argv[%d] : %s\n", counter, argv [counter]);
}



/* ========== Sanitized Scanf ==========
	Sanitized Scanf to parse input.
	Takes in a const pointer to a specified type of variable,
	ignores all but the first valid value.

	Can set values by passing pointers as argument.

	The scanf's must be in seperated lines to work properly.
	1. Removes / ignores any preceeding non-digit characters from stdin
	2. Strictly takes in the first decimal digit, into an unsigned integer variable.
	3. Then clears stdin stream to remove any remaining characters, prevents looping
*/

void scan_int (int *const dest) {
	scanf ("%*[^0-9]");
	scanf ("%d[0-9]", dest);
	scanf ("%*[^\n]");
}

int scan_int (void) {
	int dest;

	scanf ("%*[^0-9]");
	scanf ("%d[0-9]", &dest);
	scanf ("%*[^\n]");

	return dest;
}

void scan_uint (unsigned int *const dest) {
	scanf ("%*[^0-9]");
	scanf ("%u[0-9]", dest);
	scanf ("%*[^\n]");
}

unsigned int scan_uint (void) {
	unsigned int dest;

	scanf ("%*[^0-9]");
	scanf ("%u[0-9]", &dest);
	scanf ("%*[^\n]");

	return dest;
}

void scan_float (float *const dest) {
	scanf ("%*[^0-9]");
	scanf ("%f", dest);
	scanf ("%*[^\n]");
}

float scan_float (void) {
	float dest;

	scanf ("%*[^0-9]");
	scanf ("%f", &dest);
	scanf ("%*[^\n]");

	return dest;
}

/* This normally throws a GCC warning on compile time.
	Due to there not being a bool* type for scanf,
	so the (unsigned int*) cast is used to silence the warning.
	Ugly, but will have to do.
*/
void scan_bool (bool *const dest) {
	scanf ("%*[^0-1]");
	scanf ("%1u[0-1]", (unsigned int*) dest);
	scanf ("%*[^\n]");
}

bool scan_bool (void) {
	unsigned int dest;

	scanf ("%*[^0-1]");
	scanf ("%1u[0-1]", &dest);
	scanf ("%*[^\n]");

	return (bool) dest;
}
