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
#include <bitset>

// ----- Linux API Include ----- //
// #include <unistd.h>
// #include <fcntl.h>
// #include <sys/mman.h>

// ----- Altera HWLIB Include ----- //
// #include "hwlib.h"
// #include "socal/socal.h"
// #include "socal/hps.h"
// #include "socal/alt_gpio.h"
// #include "hps_0.h"

// ----- Function Prototypes && Documentation ----- //

/* void input_argument (const int argc, char **argv)
	This function handles input arguments.
	Should have limited use, now that a main menu is being implemented.
*/
void input_argument (const int argc, char **argv);

/* unsigned int mainmenu (void)
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

/* void results (const int run_check)
	Displays results from a previous simulation.
*/
void results (const int run_check);

#endif
