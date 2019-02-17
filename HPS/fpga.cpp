/* Main C++ File for FPGA Functions
	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

/* Notes on PC_BUILD
	This file compiles differently based on whether or not the flag PC_BUILD was defined.
	Define the flag with makefile to compile a different version for PC.
	The PC_BUILD version will not have access to the FPGA.
	This allows debug builds to be ran on PC, instead of on the DE0-nano-SoC
*/

/* Notes on Endianness - FPGA module is designed as little-endian
	Becareful when editing these functions.
	The FPGA module "Logical Cell Array" is designed as a little-endian device.
	The Least-Significant-Bit is set first, in the lower memory address.
	The Most-Significant-Bit is set last, in the higher memory address.
*/

/* ========== Standard Library Include ========== */

#include <stdio.h>		// printf
#include <stdlib.h>		// calloc, free, itoa
#include <stdint.h>		// uint definitions


/* ========== Linux API Include ========== */

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>


/* ========== Altera HWLIB Include ========== */

#ifndef PC_BUILD

	#include "hwlib.h"
	#include "socal/socal.h"
	#include "socal/hps.h"
	#include "socal/alt_gpio.h"
	#include "hps_0.h"

#endif


/* ========== Custom Header Include ========== */

#include "fpga.hpp"
#include "ansi.hpp"
#include "global.hpp"


/* ========== FPGA Define ========== */

#ifndef PC_BUILD

	#define HW_REGS_BASE ( ALT_STM_OFST )
	#define HW_REGS_SPAN ( 0x04000000 )
	#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )

	/* Avalon Slave Ports
		The Cell Array module contains these Avalon Slave Ports.
		Port 1 -- Linux IO
		Port 2 -- RAM
		Port 3 -- Clock Control
	*/

	// --- S1 | Linux In / Out --- //
	#define S_IO		0x1000
	#define SIO_RANGE 2

	// --- S2 | Cell RAM --- //
	#define S_RAM		0x2000
	#define SRAM_RANGE 512

	// --- S3 | Wind-up Clock --- //
	#define S_CLK		0x0000
	#define CYCLES_PER_USEC 100

	/* --- Version ROM ---
		This ROM module is used to keep track of what the FPGA configuration is.
		Each FPGA config build will have a unique version number, embedded at compile time.
	*/
	#define VROM_ADDR		0x10000
	#define VROM_RANGE 16

	// Avalon Slave Port Data Width (Bits)
	#define AVALON_PORT_WIDTH 32

	// FPGA Array Cell Data Width (Bits) -- fpga_set_grid
	#define CELL_DATA_WIDTH 4

	// Numbers of Cells able to fit in the buffer -- fpga_set_grid
	#define CELL_IN_BUFFER (AVALON_PORT_WIDTH / CELL_DATA_WIDTH)

#endif


/* ========== FPGA Global Variables ========== */

#ifndef PC_BUILD

	// Virtual Memory Pointer Base
	static void *virtual_base;

	// DEV/MEM/
	static int fd;

	// Pointer for Avalon Slave Devices
	static uint32_t *sio_addr;
	static uint32_t *sram_addr;
	static uint16_t *sclk_addr;

	// Version ROM pointer
	static uint8_t *vrom_address;

#endif

// Local Copy of Global Parameters
static uint16_t dimx;
static uint16_t dimy;

// Internal Initialization Flag
static bool fpga_init_flag;


using namespace std;



/* ========== Primitive Avalon Port Read / Write Functions ==========
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

#ifndef PC_BUILD

uint32_t fpga_s1_read (const uint32_t &offset) {
	return alt_read_word (sio_addr + offset);
}

void fpga_s1_write (const uint32_t &offset, const uint32_t &data) {
	alt_write_word (sio_addr + offset, data);
}

void fpga_s2_write (const uint32_t &offset, const uint32_t &data) {
	alt_write_word (sram_addr + offset, data);
}

void fpga_s3_write (const uint16_t &data) {
	alt_write_hword (sclk_addr, data);
}

uint8_t fpga_vrom_read (const uint32_t &offset) {
	return alt_read_byte (vrom_address + offset);
}

#endif


/* ========== Main Functions ========== */

#ifdef PC_BUILD

void fpga_init (void) {
	printf ("PC Build FPGA Init ");

	dimx = GlobalSettings::get_ca_dimx ();
	dimy = GlobalSettings::get_ca_dimy ();
	fpga_init_flag = 1;

	printf (ANSI_GREEN "OK\n" ANSI_RESET);
	return;
}

void fpga_cleanup (void) {
	printf ("PC Build FPGA Cleanup ");

	fpga_init_flag = 0;

	printf (ANSI_GREEN "OK\n" ANSI_RESET);
	return;
}

#else

void fpga_init (void) {
	printf ("Initializing FPGA... ");

	// If FPGA was already initialized, close and reinitialize.
	if ( fpga_init_flag == 1 ) {
		close (fd);
		fpga_init_flag = 0;
	}

	// Creates local copy of global parameters for CA functions
	dimx = GlobalSettings::get_ca_dimx ();
	dimy = GlobalSettings::get_ca_dimy ();

	// Read Device Memory
	fd = open ("/dev/mem", (O_RDWR | O_SYNC));

	// If failed to open /dev/mem
	if (fd == -1) {
		printf (ANSI_RED "COULD NOT OPEN /dev/mem\n" ANSI_RESET);
		return;
	}

	// Map Memory for use
	virtual_base = mmap
	(NULL, HW_REGS_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, HW_REGS_BASE);

	// If failed to map memory
	if (virtual_base == MAP_FAILED) {
		printf (ANSI_RED "MMAP FAIL\n" ANSI_RESET);
		close (fd);
		return;
	}

	/* Assign address pointers
		Notes on all the type casts
		Cast virtual_base to (char *) to suppress arithmetic with (void*) warning,
		then cast results back into (unsigned long *) to work properly

		Cannot cast virtual_base directly to (unsigned long *) because pointer arithmetic:
			> long int *p + 1 == *p + 4 << Causes int overflow and SEGFAULTs
			> char *q + 1 == *q + 1
		See: http://www.cplusplus.com/doc/tutorial/pointers/#arithmetics
		Also: https://groups.google.com/forum/#!topic/comp.lang.c/RRsX0Z3MUjY%5B1-25%5D
	*/
	sio_addr = (uint32_t *)( (char *)(virtual_base)
		+ ((ALT_LWFPGASLVS_OFST + S_IO) & HW_REGS_MASK) );
	sram_addr = (uint32_t *)( (char *)(virtual_base)
		+ ((ALT_LWFPGASLVS_OFST + S_RAM) & HW_REGS_MASK) );
	sclk_addr = (uint16_t *)( (char *)(virtual_base)
		+ ((ALT_LWFPGASLVS_OFST + S_CLK) & HW_REGS_MASK) );

	vrom_address = (uint8_t *)( (char *)(virtual_base)
		+ ((ALT_LWFPGASLVS_OFST + VROM_ADDR) & HW_REGS_MASK) );

	fpga_init_flag = 1;
	fpga_clear ();
	printf (ANSI_GREEN "DONE\n" ANSI_RESET);
	return;
}

void fpga_cleanup (void) {
	printf ("Cleaning up FPGA... ");

	// If FPGA has been initialized, close fd
	if (fpga_init_flag == 1) close (fd);

	fpga_init_flag = 0;
	printf (ANSI_GREEN "DONE\n" ANSI_RESET);
	return;
}

bool fpga_not_init (void) {
	// Returns TRUE if FPGA is uninitialized
	if (fpga_init_flag == 0) {
		printf (ANSI_RED "FPGA Uninitialized\n" ANSI_RESET);
		return 1;
	}

	// Otherwise, return FALSE
	return 0;
}

#endif

bool fpga_is_init (void) {
	return fpga_init_flag;
}



/* ========== FPGA Verification ========== */

#ifdef PC_BUILD

void fpga_verify (uint8_t *const *const grid) {
	printf ("\tPC Build FPGA Verify\n\n"
			"\tSet Dimensions: %d x %d\n"
			"\tSet Cell Count: %d\n\n",
			dimx, dimy, dimx * dimy);

	return;
}

#else

void fpga_test_fill (uint8_t *const *const grid, const uint8_t &num) {
	for (unsigned int y = 0 ; y < dimy ; y ++) {
		for (unsigned int x = 0 ; x < dimx ; x ++) {
			grid [y][x] = num;
		}
	}
}

unsigned int fpga_test (const unsigned int &mode, const unsigned int &x) {
	// FPGA Testing Inputs
	constexpr uint64_t test_input [3] =
	{ 0x0000000000000000, 0xFFFFFFFFFFFFFFFF, 0xDEADBEEFABCDEF12 };

	// FPGA Expected Outputs
	constexpr uint64_t test_output [4][3] = {
		{0x0000000000000000, 0x0000000000000000, 0x0000000000000000}, // NULL
		{0x0000000000000000, 0xFFFFFFFFFFFFFFFF, 0xDEADBEEFABCDEF12}, // PASS A
		{0x0000000000000000, 0x0000000000000000, 0x0000000000000000}, // PASS B
		{0x0000000000000000, 0xFFFFFFFFFFFFFFFF, 0xFFDFFF37FFFFFFFF}  // NAND
	};

	// Set test case input
	fpga_set_input (test_input [x]);

	// Run and wait for this specific amount of clock cycles
	fpga_wind_clock (100);

	// Get test case output
	uint64_t observed = fpga_get_output ();

	// Compare result with expectation & print table
	if ( observed == test_output [mode][x] ) {
		printf ("\t0x%016llX | 0x%016llX | " ANSI_GREEN "0x%016llX\n" ANSI_RESET,
				test_input [x], test_output [mode][x], observed );
		return 1;
	} else {
		printf ("\t0x%016llX | 0x%016llX | " ANSI_YELLOW "0x%016llX\n" ANSI_RESET,
				test_input [x], test_output [mode][x], observed );
		return 0;
	}
}

void fpga_verify (uint8_t *const *const grid) {
	// FPGA Uninitialized Error Catch
	if ( fpga_not_init () ) return;

	fpga_config_version ();

	printf (ANSI_REVRS "\tVerifying FPGA Cell Array\n\n" ANSI_RESET
			"\tUtilized Dimensions: %d x %d\n"
			"\tUtilized Cell Count: %d\n\n",
			dimx, dimy, dimx * dimy);

	// Test Score -- Test cases passed
	unsigned int score = 0;
	// Maximum Score -- Numbers of test cases to be checked
	constexpr unsigned int score_max = 12;

	// Clears FPGA grid and IO
	fpga_clear ();

	// ===== NULL ===== //
	printf ("\t\t\t   " ANSI_REVRS "----- Test  NULL -----\n" ANSI_RESET);
	fpga_test_fill (grid, 0);
	printf ("\t             Input |      Expected      | Observed\n"
			"\t-------------------+--------------------+-------------------\n" );
	for (int x = 0 ; x < 3 ; x++) {
		fpga_set_grid (grid);
		score += fpga_test (0, x);
		fpga_clear ();
	}
	putchar ('\n');


	// ===== Pass A ===== //
	printf ("\t\t\t   " ANSI_REVRS "---- Test  PASS A ----\n" ANSI_RESET);
	fpga_test_fill (grid, 1);
	printf ("\t             Input |      Expected      | Observed\n"
			"\t-------------------+--------------------+-------------------\n" );
	for (int x = 0 ; x < 3 ; x++) {
		fpga_set_grid (grid);
		score += fpga_test (1, x);
		fpga_clear ();
	}
	putchar ('\n');

	// ===== Pass B ===== //
	printf ("\t\t\t   " ANSI_REVRS "---- Test  PASS B ----\n" ANSI_RESET);
	fpga_test_fill (grid, 2);
	printf ("\t             Input |      Expected      | Observed\n"
			"\t-------------------+--------------------+-------------------\n" );
	for (int x = 0 ; x < 3 ; x++) {
		fpga_set_grid (grid);
		score += fpga_test (2, x);
		fpga_clear ();
	}
	putchar ('\n');

	// ===== NAND ===== //
	printf ("\t\t\t   " ANSI_REVRS "----- Test  NAND -----\n" ANSI_RESET);
	fpga_test_fill (grid, 3);
	printf ("\t             Input |      Expected      | Observed\n"
			"\t-------------------+--------------------+-------------------\n" );
	for (int x = 0 ; x < 3 ; x++) {
		fpga_set_grid (grid);
		score += fpga_test (3, x);
		fpga_clear ();
	}
	putchar ('\n');

	// Print Results
	printf ("\n\tResults: %u / %u", score, score_max);

	if (score == score_max) {
		printf (" | " ANSI_GREEN "PASS\n" ANSI_RESET);
	} else {
		printf (" | " ANSI_YELLOW "WARN\n" ANSI_RESET);
	}

	return;
}

#endif



/* ========== AVALON S1 Functions ========== */

#ifdef PC_BUILD

void fpga_set_input (const uint64_t &write_data) {
	return;
}

uint64_t fpga_get_output (void) {
	return 0;
}

#else

void fpga_set_input (const uint64_t &write_data) {
	// FPGA Uninitialized Error Catch
	if ( fpga_not_init () ) return;

	// Writes LSB First
	for (uint32_t i = 0 ; i < SIO_RANGE ; i++) {
		/* Bitshifts data to the appropriate location
			Input data is wider than the write port (64 bit -> 32 bit).
			So we shift part of the data, and align it.
		*/
		const uint32_t tmp_data = (uint32_t) (write_data >> (AVALON_PORT_WIDTH * i) );

		// Writes the bitshifted data to the assigned offset
		fpga_s1_write (i, tmp_data);
	}
}

uint64_t fpga_get_output (void) {
	// FPGA Uninitialized Error Catch
	if ( fpga_not_init () ) return 0;

	// Output buffer variable
	uint64_t results = 0;

	/* Reads MSB first, then bitshifts to make room for LSB
		Loopvar MUST be signed, or this will loop forever.
	*/
	for (int i = SIO_RANGE-1; i >= 0; i--) {
		/* Shifts bit
			Shift bits first, so the LSB would not get displaced.
			Like this:
			0. [XXXX_XXXX XXXX_XXXX] << Start
			1. [XXXX_XXXX 0000_0000] << Bitshift (to the left)
			2. [0000_0000 DATA_MSB0] << Set MSB
			3. [DATA_MSB0 0000_0000] << Bitshift (to the left)
			4. [DATA_MSB0 DATA_LSB1] << Set LSB
			5. Done
		*/
		results <<=  AVALON_PORT_WIDTH;

		// Bitwise OR to set bits
		results |= fpga_s1_read (i);
	}

	return results;
}

#endif



/* ========== AVALON S2 Functions ========== */

#ifdef PC_BUILD

void fpga_clear (void) {
	return;
}

void fpga_set_grid (const uint8_t *const *const grid) {
	return;
}

#else

void fpga_clear (void) {
	// FPGA Uninitialized Error Catch
	if ( fpga_not_init () ) return;

	// Iterates over entire CA grid & sets to zero
	for (uint32_t i = 0 ; i < SRAM_RANGE ; i++) {
		fpga_s2_write (i, 0x0);
	}

	// Sets input to zero
	fpga_set_input (0x0);

	// Update the states a little bit
	fpga_wind_clock (2);
}

void fpga_set_grid (const uint8_t *const *const grid) {
	// FPGA Uninitialized Error Catch
	if ( fpga_not_init () ) return;

	uint32_t data_buffer = 0;
	uint32_t offset = SRAM_RANGE;

	// Iterates through every row, from bottom to top -- loopvar must be signed
	for (int y = dimy - 1 ; y >= 0 ; y--) {
		// Iterates through every column, from right to left -- loopvar must be signed
		for (int x = dimx - 1 ; x >= 0 ; x--) {
			/* How this works:
				Shift bits first, so the LSB would not get displaced.
				0. [XXXX XXXX XXXX XXXX] << Start
				1. [XXXX XXXX XXXX 0000] << Bitshift (to the left)
				2. [XXXX XXXX XXXX CA00] << Set Cell
				3. [XXXX XXXX CA00 0000] << Bitshift (to the left)
				4. [XXXX XXXX CA00 CA01] << Set Cell
				5. Repeat Until Buffer Filled
				6. [CA00 CA01 CA02 CA03] << Set this to S2 @ Offset
				7. Repeat for entire grid
			*/

			// Shifts one cell worth of data
			data_buffer <<= CELL_DATA_WIDTH;
			// Sets one cell worth of data
			data_buffer |= grid [y][x];

			// Every 32 bits filled to buffer, write to S2 port
			if (x % CELL_IN_BUFFER == 0) {
				/* Cycles through offset, from 511 to 0
					Writes from bottom to top, to reduce waiting time
				*/
				offset--;
				// Writes current buffer to offset
				fpga_s2_write (offset, data_buffer);
			}
		}
	}

}

#endif



/* ========== AVALON S3 Functions ========== */

#ifdef PC_BUILD

void fpga_wind_clock (const uint16_t &cycles) {
	return;
}

#else

void fpga_wind_clock (const uint16_t &cycles) {
	fpga_s3_write (cycles);

	// Wait for the Cell Array to finish running
	usleep ( 1 + (cycles / CYCLES_PER_USEC) );
}

#endif



/* ========== Version ROM Functions ========== */

#ifdef PC_BUILD

void fpga_config_version (void) {
	printf ("\tPC BUILD\n");
}

#else

void fpga_config_version (void) {
	unsigned char ver_num [VROM_RANGE];
	for (int i = 0 ; i < VROM_RANGE ; i++) {
		ver_num [i] = fpga_vrom_read (i);
	}
	printf ("\tFPGA Configuration <%s>\n", ver_num);
}

#endif
