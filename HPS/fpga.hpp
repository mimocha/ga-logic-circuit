/* Header File for FPGA Functions
	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

#ifndef FPGA_HPP
#define FPGA_HPP



/* ========== Primitive Avalon Port Read / Write Functions ==========
	Low level read / write functions. Static.
	Limited to usage by other FPGA functions only.
	Handles simple interaction with Altera read / write functions.
	Handles address offsets.

	Reference for Altera Functions:
	 * This section implements read and write functionality for various
	 * memory untis. The memory unit terms used for these functions are
	 * consistent with those used in the ARM Architecture Reference Manual
	 * ARMv7-A and ARMv7-R edition manual. The terms used for units of memory are:
	 *
	 *  Unit of Memory | Abbreviation | Size in Bits
	 * :---------------|:-------------|:------------:
	 *  Byte           | byte         |       8
	 *  Half Word      | hword        |      16
	 *  Word           | word         |      32
	 *  Double Word    | dword        |      64
	 *
*/

/* uint32_t fpga_s1_read (const uint32_t &offset)
	Read 32-bit unsigned int from selected address offset of the Cell Array module
*/

/* void fpga_s1_write (const uint32_t &offset, const uint32_t &data)
	Writes 32-bit unsigned int to selected address offset of the Cell Array module
*/

/* void fpga_s2_write (const uint32_t &offset, const uint32_t &data)
	Write 32-bit unsigned int to selected address offset of the RAM module
*/

/* void fpga_s3_write (const uint16_t &data)
	Write 16-bit unsigned int to the windup_clock module
*/

/* uint8_t fpga_vrom_read (const uint32_t &offset)
	Readds 8-bit unsigned char from the Version ROM module
*/



/* ========== Main Functions ========== */

/* void fpga_init (void)
	Initialize FPGA address pointers and variables.

	Sets the flag 'fpga_init_flag' to 1, if successful.
	Otherwise, set flag to 0.
*/
void fpga_init (void);

/* void fpga_cleanup (void)
	Cleans up initialized FPGA pointers
	Practically does nothing, kept for furture usage.
*/
void fpga_cleanup (void);

/* bool fpga_not_init (void)
	Checks if FPGA is NOT initialized.

	Returns TRUE if it is NOT initialized, and prints error message.
	Returns FALSE if it IS initialized.

	Use this function as an error catch function,
	to prevent usage of certain FPGA functions before proper initialization.
*/

/* bool fpga_is_init (void)
	Returns FPGA initialization status.

	This function is created for access control of the variable 'fpga_init_flag'.
	Encapsulates the variable.
*/
bool fpga_is_init (void);



/* ========== FPGA Verification ========== */

/* void fpga_verify (uint8_t *const *const grid)
	FPGA verification function.
	Runs a short test to determine if INPUT / OUTPUT is as expected.
*/
void fpga_verify (uint8_t *const *const grid);

/* void fpga_test_fill (uint8_t *const *const grid, const uint8_t &num)
	This function fills the 2D array grid, with the value num.
	Used in fpga_test function.
*/

/* unsigned int fpga_test (const unsigned int &mode)
	Checks each test case for input / output, for the given setting.
	Expected Input / Output pair is hardcoded.

	Use 'mode' to determine what input / output pair is expected.
	The function will then check if the observed results match with expectation.

	Returns an unsigned int of numbers of matching observation.
*/



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



/* ========== AVALON S3 Functions ========== */

/* void fpga_wind_clock (const uint16_t &cycles)
	Runs the specified number of clock cycles for the Cell Array.
	Should fix the problem of unreliability and irreproducibility in timing.
*/
void fpga_wind_clock (const uint16_t &cycles);



/* ========== Version ROM Functions ========== */

/* void fpga_config_version (void)
	Prints current FPGA Configuration Version.
*/
void fpga_config_version (void);





#endif
