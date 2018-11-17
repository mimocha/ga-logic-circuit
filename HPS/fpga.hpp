/* Header File for FPGA Functions
	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

#ifndef FPGA_HPP
#define FPGA_HPP

/* ========== Miscellaneous Functions ========== */

/* void fpga_init (void)
	Initialize FPGA variables and pointers.

	Inputs:
	dimx_in is a reference to a constant unsigned int. The values may not change in this function.
	dimy_in is a reference to a constant unsigned int. The values may not change in this function.
*/
void fpga_init (void);

/* void fpga_cleanup (void)
	Cleans up initialized FPGA variables.
*/
void fpga_cleanup (void);

/* static bool fpga_not_init (void)
	Checks if FPGA is not initialized.

	Returns TRUE if it is NOT initialized, and prints error message.
	Returns FALSE if it IS initialized.

	Use this function as an internal roadblock,
	to prevent usage of certain FPGA functions before proper initialization.
*/
// static bool fpga_not_init (void);

/* bool fpga_is_init (void)
	Returns FPGA initialization status.

	This function is created for access control of the varuable 'fpga_init_flag'.
	The variable should only be available within this compilation unit, and not to other files.
*/
bool fpga_is_init (void);



/* ========== FPGA Verification ========== */

/* void fpga_verify (uint8_t *const *const grid)
	FPGA verification function.
	Runs a short test to determine if INPUT / OUTPUT is as expected.

	Takes in a double const pointer array 'grid'.
	'grid' is a double const pointer to a uint8_t.
	The values of the array will change. The pointers will not.
*/
void fpga_verify (uint8_t *const *const grid);

/* static void fpga_test_fill (uint8_t *const *const grid, const uint8_t &num)
	Fills the grid array with a given uint8_t.

	Inputs:
	'grid' is a double const pointer to a uint8_t. The values will change, but the pointers won't.
	'num' is a reference to a const uint8_t. The values may not change in this function.

	This function fills the 2D array grid, with the value num.
*/
// static void fpga_test_fill (uint8_t *const *const grid, const uint8_t &num);

/* static unsigned int fpga_test (const unsigned int &mode)
	Checks each test case for input / output, for the given setting.

	Input:
	'mode' is reference to a const unsigned int. Its value may change in this function.
	'mode' is used to select the testing mode.

	Use 'mode' to determine what input / output pair is expected.
	The function will then check if the observed results match with expectation.

	Returns an unsigned int of numbers of matching observation.
*/
// static unsigned int fpga_test (const unsigned int &mode);



/* ========== AVALON S1 Functions ==========
	Handles interactions with S1 port (Linux IO)
*/

/* void fpga_set_input (const uint64_t &data)
	Sets input for FPGA Cell Array.

	As the slave port only allows 32-bit reads, this function will have to do multiple writes.
	Number of writes required is MAX_CA_DIMX / AVALON_PORT_WIDTH

	Be sure to handle Endianness properly. Cell Array Circuit takes data in MSB format.

	WARNING: Be sure to seperate fpga_get_output() from fpga_set_input().
	FPGA Cell Array updates with clock, and needs a few clock cycle to update completely.
	Using the two functions back-to-back is shown to cause invalid outputs.
*/
void fpga_set_input (const uint64_t &data);

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



/* ========== AVALON S2 Functions ==========
	Handles interactions with S2 port (FPGA RAM)
*/

/* void fpga_clear (void)
	Clears FPGA Cell Array, set all RAM to zero.
	Set Cell Array input to zero

	A special version of fpga_set_grid,
	cycles through the entire S2 address range, and sets to zero.
*/
void fpga_clear (void);

/* void fpga_set_grid (const uint8_t *const *const grid)
	Sets the FPGA Cell Array according to the given grid data.
	Writes from the bottom-most row up, from LSB to MSB, offset 511 to 0.

	Uses bitwise operations to set 4-bits at a time to the data buffer.
	At every 8 cell (32-bits), write once to the given address offset,
	then decrement the offset by one.

	Repeat this step over all rows and column.
	This ensures precisely (MAX_CA_DIMX * MAX_CA_DIMY / 8) writes. (512)
*/
void fpga_set_grid (const uint8_t *const *const grid);



/* ========== Avalon Port Read / Write Functions ==========
	Low level read / write functions. Static.
	Limited to usage by other FPGA functions only.
	Handles simple interaction with Altera read / write functions.
	Handles address offsets.
*/

/* static uint32_t fpga_s1_read (const uint32_t &offset)
	Read 32-bit unsigned int from selected address offset.
	For S1 Avalon Slave Port
*/
// static uint32_t fpga_s1_read (const uint32_t &offset);

/* static void fpga_s1_write (const uint32_t &offset, const uint32_t &data)
	Writes 32-bit unsigned int to selected address offset.
	For S1 Avalon Slave Port
*/
// static void fpga_s1_write (const uint32_t &offset, const uint32_t &data);

/* static void fpga_s2_write (const uint32_t &offset, const uint32_t &data)
	Write 32-bit unsigned int to selected address offset.
	For S2 Avalon Slave Port

	Originally writes twice to the same address, due to FPGA circuit's setup.
	Was done to reset WREN signal properly, but has since been proven unnecessary.
*/
// static void fpga_s2_write (const uint32_t &offset, const uint32_t &data);

#endif
