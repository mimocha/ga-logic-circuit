/* Header file for FPGA Functions

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

#ifndef FPGA_HPP
#define FPGA_HPP

/* ========== Standard Library Include ========== */

#include <stdio.h>		/* printf, perror */
#include <stdlib.h>		/* calloc, free */
#include <stdint.h>		/* uint definitions */
#include <iostream>		/* cout */

/* ========== Linux API Include ========== */

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

/* ========== Altera HWLIB Include ========== */

#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"
#include "socal/alt_gpio.h"
#include "hps_0.h"



/* ========== Miscellaneous Functions ========== */

/* bool fpga_init (const unsigned int dimx_in, const unsigned int dimy_in)
	Initialize FPGA variables and pointers.

	Inputs:
	dimx_in is a constant unsigned int. The values will not change in this function.
	dimy_in is a constant unsigned int. The values will not change in this function.

	Returns a boolean for initialization status.
	Returns TRUE if successfully initialized.
	Returns FALSE if initialization failed.
*/
bool fpga_init (const unsigned int dimx_in, const unsigned int dimy_in);

/* bool fpga_cleanup (void)
	Cleans up initialized FPGA variables.

	Returns FALSE on cleanup SUCCESS -> use to set flag in other files.
*/
bool fpga_cleanup (void);

/* static bool fpga_not_init (void)
	Checks if FPGA is not initialized.

	Returns TRUE if it is NOT initialized, and prints error message.
	Returns FALSE if it IS initialized.

	Use this function as a roadblock,
	to prevent usage before proper initialization.
*/
// static bool fpga_not_init (void);



/* ========== FPGA Verification ========== */

/* void fpga_verify (uint8_t *const *const grid)
	FPGA verification function.
	Runs a short test to determine if INPUT / OUTPUT is as expected.

	Takes in a double const pointer array 'grid'.
	'grid' is a double const pointer to a uint8_t.
	The values of the array will change. The pointers will not.
*/
void fpga_verify (uint8_t *const *const grid);



/* ========== AVALON S1 Functions ==========
	Handles interactions with S1 port (Linux IO)
*/
/* ========== AVALON S2 Functions ==========
	Handles interactions with S2 port (FPGA RAM)
*/
/* ========== Avalon Port Read / Write Functions ==========
	Low level read / write functions. Static.
	Limited to usage by other FPGA functions only.
	Handles simple interaction with Altera read / write functions.
	Handles address offsets.
*/





/* ----- Level 1 ----- */

/* void fpga_clear (void)
	Clears FPGA Cell Array, set all RAM to zero.
	Set Cell Array input to zero
*/
void fpga_clear (void);

/*
	Sets the FPGA Cell Array according to the given grid data.
	Writes from the bottom-most row up, from LSB to MSB, offset 511 to 0.

	Uses bitwise operations to set 4-bits at a time to the data buffer.
	At every 8 cell (32-bits), write once to the given address offset,
	then decrement the offset by one.

	Repeat this step over all rows and column.
	This ensures precisely (MAX_CA_DIMX * MAX_CA_DIMY / 8) writes. (512)
*/
void fpga_set_grid (const uint8_t *const *const grid);

/* void fpga_set_input (const uint64_t data)
	Sets input for FPGA Cell Array.

	As the slave port only allows 32-bit reads, this function will have to do multiple writes.
	Number of writes required is MAX_CA_DIMX / AVALON_PORT_WIDTH

	Be sure to handle Endianness properly. Cell Array Circuit takes data in MSB format.

	WARNING: Be sure to seperate fpga_get_output() from fpga_set_input().
	FPGA Cell Array updates with clock, and needs a few clock cycle to update completely.
	Using the two functions back-to-back is shown to cause invalid outputs.
*/
void fpga_set_input (const uint64_t data);

/* uint64_t fpga_get_output (void)
	Returns output from FPGA Cell Array.

	As the slave port only allows 32-bit reads, this function will have to do multiple reads.
	Number of reads required is MAX_CA_DIMX / AVALON_PORT_WIDTH

	Be sure to handle Endianness properly. Cell Array Circuit takes data in MSB format.

	WARNING: Be sure to seperate fpga_get_output() from fpga_set_input().
	FPGA Cell Array updates with clock, and needs a few clock cycle to update completely.
	Using the two functions back-to-back is shown to cause invalid outputs.
*/
uint64_t fpga_get_output (void);


#endif
