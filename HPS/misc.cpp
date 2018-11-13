/* Main C++ File for Miscellaneous Functions

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
#include <stdint.h>		/* uint definitions */

#include "misc.hpp"
#include "ansi.hpp"
#include "global.hpp"

void about (void) {
	printf ("\n\e[7m\t--- Genetics Algorithm Program Version %4.2f ---\e[0m\n\n", VERSION);

	/* Explanation & Legal */
	printf (
	"\tThis program applies Genetics Algorithm, Cellular Automaton, and more\n"
	"\tto generate logic circuits. This is create as part of a Bachelor's degree\n"
	"\tdissertation project, at Nippon Institute of Technology, 2018-2019.\n\n"
	"\t\e[7mMIT License\e[0m\n\n"
	"\t\e[7mCopyright (c) 2018 Chawit Leosrisook\e[0m\n\n"
	"\tPermission is hereby granted, free of charge, to any person obtaining a copy\n"
	"\tof this software and associated documentation files (the \"Software\"), to deal\n"
	"\tin the Software without restriction, including without limitation the rights\n"
	"\tto use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n"
	"\tcopies of the Software, and to permit persons to whom the Software is\n"
	"\tfurnished to do so, subject to the following conditions:\n\n"
	"\tThe above copyright notice and this permission notice shall be included in all\n"
	"\tcopies or substantial portions of the Software.\n\n"
	"\tTHE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
	"\tIMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
	"\tFITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
	"\tAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
	"\tLIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
	"\tOUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
	"\tSOFTWARE\n"
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
