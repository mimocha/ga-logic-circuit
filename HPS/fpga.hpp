/* FPGA header file - For containing most fpga interfaces */

#ifndef FPGA_HPP
#define FPGA_HPP

/* ----- Linux API Include ----- */

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

/* ----- Altera HWLIB Include ----- */

#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"
#include "socal/alt_gpio.h"
#include "hps_0.h"

/* Pointer for virtual memory */
void *virtual_base;
int fd; // something for mmap

/* ----- Defining FPGA memory address ----- */

#define HW_REGS_BASE ( ALT_STM_OFST )
#define HW_REGS_SPAN ( 0x04000000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )

/* Avalon Slave Ports
	The module contains 2 different Avalon Slave Ports.
	Port S1 -- Linux IO
	Port S2 -- RAM
*/

// Linux In / Out
#define S1_ADDRESS		0x1000
#define S1_ADDRESS_MAX	0x1007
#define S1_RANGE 2

// Cell Row RAM
#define S2_ADDRESS		0x0000
#define S2_ADDRESS_MAX	0x07FF
#define S2_RANGE 512

// Avalon Slave Port Data Width (Bits)
#define AVALON_PORT_WIDTH	32

/* ----- Declaring address pointers ----- */

#if (AVALON_PORT_WIDTH == 32)
	uint32_t *s1_address;
	uint32_t *s2_address;
#else
	uint64_t *s1_address;
	uint64_t *s2_address;
#endif

/* ----- Function Prototypes ----- */

/* bool fpga_init (void)
	Initializes FPGA array. Return bool of initialization status.
	1 = SUCCESS | 0 = FAILURE
*/
bool fpga_init (void);

/* void fpga_verify (void)
	Hard-coded code for manually verifying FPGA Cell Array functions.
	Verification data not proven to be correct.
	TODO: Automate and verify this.
*/
void fpga_verify (void);

/* ----- Level 1 ----- */

/* void fpga_clear (void)
	Clears FPGA Cell Array, set all RAM to zero.
	Set Cell Array input to zero
*/
void fpga_clear (void);

/* void fpga_set_grid (uint8_t **grid)
	Sets the FPGA Cell Array according to the given grid data.
	Writes from the bottom-most row up, from LSB to MSB, offset 511 to 0.

	Uses bitwise operations to set 4-bits at a time to the data buffer.
	At every 8 cell (32-bits), write once to the given address offset,
	then decrement the offset by one.

	Repeat this step over all rows and column.
	This ensures precisely (MAX_CA_DIMX * MAX_CA_DIMY / 8) writes. (512)
*/
void fpga_set_grid (uint8_t **grid);

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

/* static void fpga_check (int mode)
	Wrapper for checking 3 inputs / outputs.
	Makes function fpga_verify() cleaner.
*/
static void fpga_check (int mode);

/* ----- Level 2 -----
	FPGA only functions. Declared as static.
	These functions may only be used within the "fpga.cpp" translation unit.

	Users should instead, use the "Level 1" hierachy functions.

	Altera Definitions for reference:
	 *!
	 * \addtogroup ALT_SOCAL_UTIL_RW_FUNC SoCAL Memory Read/Write Utilities
	 *
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
	 * @{
	 *
*/

/* static uint32_t fpga_s1_read (const uint32_t offset)
	Read 32-bit unsigned int from selected address offset.
	For S1 Avalon Slave Port
*/
static uint32_t fpga_s1_read (const uint32_t offset);

/* static void fpga_s1_write (const uint32_t offset, const uint32_t data)
	Writes 32-bit unsigned int to selected address offset.
	For S1 Avalon Slave Port
*/
static void fpga_s1_write (const uint32_t offset, const uint32_t data);

/* static void fpga_s2_write (const uint32_t offset, const uint32_t data)
	Write 32-bit unsigned int to selected address offset.
	For S2 Avalon Slave Port

	Originally writes twice to the same address, due to FPGA circuit's setup.
	Was done to reset WREN signal properly, but has since been proven unnecessary.
*/
static void fpga_s2_write (const uint32_t offset, const uint32_t data);

#include "fpga.cpp"

#endif
