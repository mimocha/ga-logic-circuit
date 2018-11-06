/* Header file for Genetics ALgorithm Main Program.
	Contains standard library includes, defines, and other parameters.
*/

#ifndef GAMAIN_HPP
#define GAMAIN_HPP

// ----- Standard Library Definition ----- //

#include <stdio.h>		/* Standard I/O */
#include <stdlib.h>		/* calloc, free, rand, srand */
#include <stdint.h>		/* uint definitions */
#include <stdarg.h>		/* va_list vastart vaend */
#include <time.h>		/* time */
#include <math.h>		/* pow, round, ceiling */
#include <iostream>		/* cin, cout */
#include <algorithm>	/* sort, find */
#include <vector>		/* vectors */
#include <array>		/* arrays */
#include <cstring>		/* strcmp */

// ----- Function Prototypes && Documentation ----- //

/* unsigned int main_menu (void)
	Main Menu function.
	Users select valid inputs here. Selections are single-digit unsigned decimal integers.
	The selection is then returned to main() for calling other functions.

	The actual selection and function calling should not occur within this function,
	but within the main() function to keep the code clean and easy to follow.
*/
unsigned int main_menu (void);

/* void settings (void)
	Handles changing of global parameters.
*/
void settings (void);

/* bool read_csv (void)
	CSV file must be in the following format:
	-----------------
	input output <row count>
	<input value 1> <output value 1>
	<input value 2> <output value 2>
	<input value 3> <output value 3>
	...
	-----------------

	Spaces and tabs are automatically trimmed.
	Header row is Case Sensitive!
	Missing columns will return a failure.
	Input / Output values are required to be in hexadecimals.
*/
bool read_csv (void);

/* void results (void)
	Displays results from a previous simulation.
*/
void results (void);

/* void inspect (void)
	DNA inspection function.
	Practically a smaller version of run_sim() on its own.
	given a DNA string, this function will apply CA to it,
	generate a logic circuit, and evaluate the circuits truth table.
*/
void inspect (void);

/* */
bool export_rpt (void);

/* void cleanup (void)
	Cleanup global variable memory before exiting program.
*/
void cleanup (void);

#endif
