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
The module contains 4 different Avalon Slave Ports.

Port S1 -- Linux IO
	Set and forget
	Could be set once and read after every settings,
	but it is best to reset this to zero, and set to desired values for every new individual.
	This would prevent any unknown / unexpected resets.

Port S2 -- RAM
	Update for every row
	Has to be updated before setting the write enable signal.

Port S3 -- State
	Set and forget
	Could be set once, and forgotten for the entire simulation as it doesn't change much.
	But it would be best to set to desired values for every new individual.
	This would prevent any unknown / unexpected resets.

Port S4 -- Write Enable
	Update for every row -- This is a trickier one.

	! Must be set to 0 while writing other ports, or data will change. !
	Can also be set to multiple rows, for identical rows.
	This possibly could save some time off.


Using this module:

	> Repeat for all Individuals
		1. Set State
		> Repeat for all rows
			2. Set RAM for current row
			3. Set Write Enable for current row
			4. Clear Write Enable
		5. Set Linux_in
		6. Read Linux_out
		> Reset
			7. Clear Linux_in, State, RAM
			8. Set All Write Enable
			9. Clear Write Enable
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

void fpga_verify (void);

/* ----- Level 1 ----- */

void fpga_clear (void);

void fpga_set_grid (uint8_t **grid);

void fpga_set_input (const uint64_t data);

uint64_t fpga_get_output (void);

/* ----- Level 2 -----
	FPGA only functions
	These functions are limited to within FPGA files.
	Users should not use them outside.
*/

static uint32_t fpga_s1_read (const uint32_t offset);

static void fpga_s1_write (const uint32_t offset, const uint32_t data);

static void fpga_s2_write (const uint32_t offset, const uint32_t data);

#include "fpga.cpp"

#endif
