/* Header file for the Genetics Algorithm main file

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

#ifndef GAMAIN_HPP
#define GAMAIN_HPP

/* ========== Standard Library Include ========== */

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



/* ========== Main Menu / Initialize / Cleanup Functions ========== */

/* static void main_init (void)
	Initializes main file.
*/
static void main_init (void);

/* static void main_cleanup (void)
	Cleanup global variable memory before exiting program.
*/
static void main_cleanup (void);

/* static unsigned int main_menu (void)
	Main Menu function.
	Users select inputs here. Selections are unsigned decimal integers.
	The selection is then returned to main() for calling other functions.

	The actual selection and function calling should not occur within this function,
	but within the main() function to keep the code clean and easy to follow... ?
*/
static unsigned int main_menu (void);



/* ========== Menu Options ========== */

/* void settings (void)
	Handles changing of global parameters.
*/
static void settings (void);

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
static bool read_csv (void);

/* void results (void)
	Displays results from a previous simulation.
*/
// static void results (void);

/* void inspect (void)
	DNA inspection function.
	Practically a smaller version of run_sim() on its own.
	given a DNA string, this function will apply CA to it,
	generate a logic circuit, and evaluate the circuits truth table.
*/
static void inspect (void);

/* */
// static bool export_rpt (void);



#endif
