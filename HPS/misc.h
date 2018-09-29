/* Header file for miscellaneous functions */

#ifndef MISC_H
#define MISC_H

/* void help_message (void)
	Displays help message
*/
void help_message (void);

/* ----- Sanitized Scanf -----
	Sanitized Scanf to parse input.
	Takes in a pointer to a specified type of variable, ignores all but the first valid value.
	Sanitizes the input, and sets the value at the specified location.

	Can set values by passing pointers as argument, or with return.

	The scanf's must be in seperated lines to work properly.
	1. Removes / ignores any preceeding non-digit characters from stdin
	2. Strictly takes in the first decimal digit, into an unsigned integer variable.
	3. Then clears stdin stream to remove any remaining characters, prevents looping
*/

int *scan_int (int *dest);

unsigned int *scan_uint (unsigned int *dest);

float *scan_float (float *dest);

#include "misc.cpp"

#endif
