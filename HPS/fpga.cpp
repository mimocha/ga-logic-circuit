/* Main C++ File for FPGA Functions
	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

/* ========== Standard Library Include ========== */

#include <stdio.h>		/* printf */
#include <stdlib.h>		/* calloc, free */
#include <stdint.h>		/* uint definitions */

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

/* ========== Custom Header Include ========== */

#include "fpga.hpp"
#include "ansi.hpp"
#include "global.hpp"

/* ========== FPGA Define ========== */

#define HW_REGS_BASE ( ALT_STM_OFST )
#define HW_REGS_SPAN ( 0x04000000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )

/* Avalon Slave Ports
	The module contains 2 different Avalon Slave Ports.
	Port S1 -- Linux IO
	Port S2 -- RAM
*/

/* --- Linux In / Out --- */
#define S1_ADDRESS		0x1000
#define S1_ADDRESS_MAX	0x1007
#define S1_RANGE 2

/* --- Cell Row RAM --- */
#define S2_ADDRESS		0x0000
#define S2_ADDRESS_MAX	0x07FF
#define S2_RANGE 512

/* Avalon Slave Port Data Width (Bits) */
#define AVALON_PORT_WIDTH 32

/* FPGA Array Cell Data Width (Bits) -- fpga_set_grid */
#define CELL_DATA_WIDTH 4

/* Numbers of Cells able to fit in the buffer -- fpga_set_grid */
#define CELL_IN_BUFFER (AVALON_PORT_WIDTH / CELL_DATA_WIDTH)

/* ========== FPGA Global Variables ========== */

/* Virtual Memory Pointer Base */
static void *virtual_base;

/* DEV/MEM/ */
static int fd;

/* Pointer for Avalon Slave Devices */
#if (AVALON_PORT_WIDTH == 32)
	static uint32_t *s1_address;
	static uint32_t *s2_address;
#else
	static uint64_t *s1_address;
	static uint64_t *s2_address;
#endif

/* Local Copy of Global Parameters */
static uint16_t dimx;
static uint16_t dimy;

/* Internal Initialization Flag */
static bool fpga_init_flag;

using namespace std;



/* ========== STATIC FUNCTION PROTOYPES ==========
	See documentation in header file "fpga.hpp"
*/

static bool fpga_not_init (void);

static void fpga_test_fill (uint8_t *const *const grid, const uint8_t &num);

static unsigned int fpga_test (const unsigned int &mode);

static uint32_t fpga_s1_read (const uint32_t &offset);

static void fpga_s1_write (const uint32_t &offset, const uint32_t &data);

static void fpga_s2_write (const uint32_t &offset, const uint32_t &data);




/* ========== Miscellaneous Functions ========== */

void fpga_init (void) {
	printf ("Initializing FPGA... ");

	/* If FPGA was already initialized, close and reinitialize. */
	if ( fpga_init_flag == 1 ) close (fd);

	/* Creates local copy of global parameters for CA functions */
	dimx = GlobalSettings::get_ca_dimx ();
	dimy = GlobalSettings::get_ca_dimy ();

	/* Read Device Memory */
	fd = open ("/dev/mem", (O_RDWR | O_SYNC));

	/* If failed to open /dev/mem */
	if (fd == -1) {
		printf (ANSI_RED "COULD NOT OPEN /dev/mem\n" ANSI_RESET);
		return;
	}

	/* Map Memory for use */
	virtual_base = mmap
	(NULL, HW_REGS_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, HW_REGS_BASE);

	/* If failed to map memory */
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
	#if (AVALON_PORT_WIDTH == 32)
		s1_address = (uint32_t *)( (char *)(virtual_base)
			+ ((ALT_LWFPGASLVS_OFST + S1_ADDRESS) & HW_REGS_MASK) );
		s2_address = (uint32_t *)( (char *)(virtual_base)
			+ ((ALT_LWFPGASLVS_OFST + S2_ADDRESS) & HW_REGS_MASK) );
	#else
		s1_address = (uint64_t *)( (char *)(virtual_base)
			+ ((ALT_LWFPGASLVS_OFST + S1_ADDRESS) & HW_REGS_MAS) );
		s2_address = (uint64_t *)( (char *)(virtual_base)
			+ ((ALT_LWFPGASLVS_OFST + S2_ADDRESS) & HW_REGS_MASK) );
	#endif

	/* Sets Initialization Flag */
	fpga_init_flag = 1;

	/* Clears FPGA Grid & Input -- Prepares for usage */
	fpga_clear ();

	/* Prints Message and Returns */
	printf (ANSI_GREEN "DONE\n" ANSI_RESET);
	return;
}

void fpga_cleanup (void) {
	printf ("Cleaning up FPGA... ");

	/* If FPGA has been initialized, close fd */
	if (fpga_init_flag == 1) close (fd);

	/* Reset initialization flag */
	fpga_init_flag = 0;

	printf (ANSI_GREEN "DONE\n" ANSI_RESET);
	return;
}

bool fpga_not_init (void) {
	/* Returns TRUE if FPGA is uninitialized */
	if (fpga_init_flag == 0) {
		printf (ANSI_RED "FPGA Uninitialized\n" ANSI_RESET);
		return 1;
	}

	/* Otherwise, return FALSE */
	return 0;
}

bool fpga_is_init (void) {
	return fpga_init_flag;
}



/* ========== FPGA Verification ========== */

void fpga_verify (uint8_t *const *const grid) {
	/* FPGA Uninitialized Error Catch */
	if ( fpga_not_init () ) return;

	printf (ANSI_REVRS "\tVerifying FPGA Cell Array\n\n" ANSI_RESET
			"\tUtilized Dimensions: %d x %d\n"
			"\tUtilized Cell Count: %d\n\n",
			dimx, dimy, dimx * dimy);

	/* Test Score -- Test cases passed */
	unsigned int score = 0;
	/* Maximum Score -- Numbers of test cases to be checked */
	const unsigned int score_max = 12;

	/* Clears FPGA grid and IO */
	fpga_clear ();

	/* ===== NULL ===== */
	printf ("\t\t\t   " ANSI_REVRS "----- Test  NULL -----\n" ANSI_RESET);
	fpga_test_fill (grid, 0);
	fpga_set_grid (grid);
	score += fpga_test (0);

	/* ===== Pass A ===== */
	printf ("\t\t\t   " ANSI_REVRS "---- Test  PASS A ----\n" ANSI_RESET);
	fpga_test_fill (grid, 1);
	fpga_set_grid (grid);
	score += fpga_test (1);

	/* ===== Pass B ===== */
	printf ("\t\t\t   " ANSI_REVRS "---- Test  PASS B ----\n" ANSI_RESET);
	fpga_test_fill (grid, 2);
	fpga_set_grid (grid);
	score += fpga_test (2);

	/* ===== NAND ===== */
	printf ("\t\t\t   " ANSI_REVRS "----- Test  NAND -----\n" ANSI_RESET);
	fpga_test_fill (grid, 3);
	fpga_set_grid (grid);
	score += fpga_test (3);

	/* Clears FPGA grid and IO */
	fpga_clear ();

	/* Print Results */
	printf ("\n\tResults: %u / %u", score, score_max);

	if (score == score_max) {
		printf (" | " ANSI_GREEN "PASS\n" ANSI_RESET);
	} else {
		printf (" | " ANSI_YELLOW "WARN\n" ANSI_RESET);
	}

	return;
}

void fpga_test_fill (uint8_t *const *const grid, const uint8_t &num) {
	/* Iterates over every row */
	for (unsigned int y = 0 ; y < dimy ; y ++) {
		/* Iterates over every cell */
		for (unsigned int x = 0 ; x < dimx ; x ++) {
			grid [y][x] = num;
		}
	}
}

unsigned int fpga_test (const unsigned int &mode) {
	/* FPGA Testing Inputs */
	constexpr uint64_t test_input [3] =
	{ 0x0000000000000000, 0xFFFFFFFFFFFFFFFF, 0xDEADBEEFABCDEF12 };

	/* FPGA Expected Outputs */
	constexpr uint64_t test_output [4][3] = {
		{0x0000000000000000, 0x0000000000000000, 0x0000000000000000}, // NULL
		{0x0000000000000000, 0xFFFFFFFFFFFFFFFF, 0xDEADBEEFABCDEF12}, // PASS A
		{0x0000000000000000, 0x0000000000000000, 0x0000000000000000}, // PASS B
		{0x8000000000000000, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFDE0DBEEF}  // NAND
	};

	/* Test results -- Numbers of passed test */
	unsigned int results = 0;

	/* Print Table */
	printf (
	"\t             Input |      Expected      | Observed\n"
	"\t-------------------+--------------------+-------------------\n"
	);

	/* Iterates over each test case -- 3 per mode */
	for (unsigned int i = 0 ; i < 3 ; i++) {

		/* Set test case input */
		fpga_set_input (test_input [i]);

		/* Get test case output */
		uint64_t observed = fpga_get_output ();

		/* Compare result with expectation & print table */
		if ( observed == test_output [mode][i] ) {
			printf ("\t0x%016llX | 0x%016llX | " ANSI_GREEN "0x%016llX\n" ANSI_RESET,
					test_input [i], test_output [mode][i], observed );
			results += 1;
		} else {
			printf ("\t0x%016llX | 0x%016llX | " ANSI_YELLOW "0x%016llX\n" ANSI_RESET,
			test_input [i], test_output [mode][i], observed );
		}

	}

	/* Return test results */
	printf ("\n");
	return results;
}



/* ========== AVALON S1 Functions ==========
	Handles interactions with S1 port (Linux IO)
*/

void fpga_set_input (const uint64_t &write_data) {
	/* FPGA Uninitialized Error Catch */
	if ( fpga_not_init () ) return;

	/* Writes LSB First */
	for (uint32_t i = 0 ; i < S1_RANGE ; i++) {
		/* Bitshifts data to the appropriate location
			Input data is wider than the write port (64 bit -> 32 bit).
			So we shift part of the data, and align it.
		*/
		const uint32_t tmp_data = (uint32_t) (write_data >> (AVALON_PORT_WIDTH * i) );

		/* Writes the bitshifted data to the assigned offset */
		fpga_s1_write (i, tmp_data);
	}

	/* Sleep for 1 Microsecond -- 50 FPGA Clock Cycles
		As writing the input for the FPGA takes multiple actions,
		it is not required to wait a full 64 FPGA clock cycles for the grid to fully update.

		Empirical evidences show that using usleep(1) is enough for the FPGA grid to update.
	*/
	usleep (1);
}

uint64_t fpga_get_output (void) {
	/* FPGA Uninitialized Error Catch */
	if ( fpga_not_init () ) return 0;

	/* Output buffer variable */
	uint64_t results = 0;

	/* Reads MSB first, then bitshifts to make room for LSB
		Loopvar MUST be signed, or this will loop forever.
	*/
	for (int i = S1_RANGE-1; i >= 0; i--) {
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

		/* Bitwise OR to set bits */
		results |= fpga_s1_read (i);
	}

	/* Return finished reading buffer */
	return results;
}



/* ========== AVALON S2 Functions ==========
	Handles interactions with S2 port (FPGA RAM)
*/

void fpga_clear (void) {
	/* FPGA Uninitialized Error Catch */
	if ( fpga_not_init () ) return;

	/* Iterates over entire CA grid & sets to zero */
	for (uint32_t i = 0 ; i < S2_RANGE ; i++) {
		fpga_s2_write (i, 0x0);
	}

	/* Sets input to zero */
	fpga_set_input (0x0);
}

void fpga_set_grid (const uint8_t *const *const grid) {
	/* FPGA Uninitialized Error Catch */
	if ( fpga_not_init () ) return;

	uint32_t data_buffer = 0;
	uint32_t offset = S2_RANGE;

	/* Iterates through every row, from bottom to top -- loopvar must be signed */
	for (int y = dimy - 1 ; y >= 0 ; y--) {
		/* Iterates through every column, from right to left -- loopvar must be signed */
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

			/* Shifts one cell worth of data */
			data_buffer <<= CELL_DATA_WIDTH;
			/* Sets one cell worth of data */
			data_buffer |= grid [y][x];

			/* Every 32 bits filled to buffer, write to S2 port */
			if (x % CELL_IN_BUFFER == 0) {
				/* Cycles through offset, from 511 to 0
					Writes from bottom to top, to reduce waiting time
				*/
				offset--;
				/* Writes current buffer to offset */
				fpga_s2_write (offset, data_buffer);
			}
		}
	}

}



/* ========== Avalon Port Read / Write Functions ==========
	Low level read / write functions. Static.
	Limited to usage by other FPGA functions only.
	Handles simple interaction with Altera read / write functions.
	Handles address offsets.
*/

uint32_t fpga_s1_read (const uint32_t &offset) {
	return alt_read_word (s1_address + offset);
}

void fpga_s1_write (const uint32_t &offset, const uint32_t &data) {
	alt_write_word (s1_address + offset, data);
}

void fpga_s2_write (const uint32_t &offset, const uint32_t &data) {
	alt_write_word (s2_address + offset, data);
}
