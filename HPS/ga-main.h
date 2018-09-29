/* Header file for Genetics ALgorithm Main Program.
	Contains standard library includes, defines, and other parameters.
*/

#ifndef GAMAIN_H
#define GAMAIN_H

// ----- Standard Library Definition ----- //

#include <stdio.h>		/* Standard io */
#include <stdlib.h>		/* String to integers */
#include <string.h>		/* String manipulation */
#include <stdint.h>		/* uint definitions */
#include <time.h>		/* Timing & performance */
#include <math.h>		/* floor round ceiling */
#include <iostream>		/* cin cout */
#include <algorithm>	/* sort and find */
#include <vector>		/* vectors */
// #include <ctype.h>		/* isdigit */

// // ----- Linux API Include ----- //
// #include <unistd.h>
// #include <fcntl.h>
// #include <sys/mman.h>
//
// // ----- Altera HWLIB Include ----- //
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
unsigned int mainmenu (void);

int run_sim ();

void settings ();

#endif
