#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
// #include <bitset>
#include <iostream>
#include <math.h>

#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>

#include "../hwlib/include/hwlib.h"
#include "../hwlib/include/soc_cv_av/socal/socal.h"
#include "../hwlib/include/soc_cv_av/socal/hps.h"
#include "../hwlib/include/soc_cv_av/socal/alt_gpio.h"
#include "hps_0.h"

void *virtual_base;
int fd;
#define HW_REGS_BASE ( ALT_STM_OFST )
#define HW_REGS_SPAN ( 0x04000000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )

#define RAM_BASE 0x00000000
#define INPUT_IO_BASE 0x00000200
#define OUTPUT_IO_BASE 0x00000210
#define GA_IO_BASE 0x00000220

unsigned long *led_addr = NULL;
unsigned long *ram_addr = NULL;
unsigned long *input_io_addr = NULL;
unsigned long *output_io_addr = NULL;
unsigned long *ga_io_addr = NULL;
unsigned long *test = NULL;

uint32_t grid_dim = 0; // Grid dimension
uint32_t perm = 0; // input permutation == ( 2^(grid_dim*2) )

using namespace std;

void init (void);
uint32_t check_zero (void);
uint32_t check_a (void);
uint32_t check_b (void);
uint32_t check_nand (void);
uint32_t expo (const uint32_t a, const uint32_t b);
bool cell (const bool a, const bool b, const uint8_t set);

int main(int argc, char **argv) {
	init ();

	if (argc > 1) {
		grid_dim = (uint32_t)sqrt( strtol(argv[1], NULL, 10) );
		perm = expo(2,grid_dim*2);
	} else {
		printf("LOGIC BLOCK COUNT (DEC): ");
		scanf("%d", &grid_dim);
		grid_dim = (uint32_t)sqrt(grid_dim);
		perm = expo(2,grid_dim*2);
	}

	// 4 Check Cases
	// 0: All Zero
	// 1: All Send A
	// 2: All Send B
	// 3: All NAND
	printf("Verifying 4 cases @ %d iterations each\n", perm);

	for (uint8_t c=0; c<4; c++) {
		uint32_t result = 0;
		// Iterate over each case
		switch (c) {
			case 0:
				printf("NULL Output... \n");
				alt_write_word (ga_io_addr, 0x0);
				result = check_zero ();
				break;
			case 1:
				printf("A Output... \n");
				alt_write_word (ga_io_addr, 0x55555555);
				result = check_a ();
				break;
			case 2:
				printf("B Output... \n");
				alt_write_word (ga_io_addr, 0xAAAAAAAA);
				result = check_b ();
				break;
			case 3:
				printf("NAND Output... \n");
				alt_write_word (ga_io_addr, 0xFFFFFFFF);
				result = check_nand ();
				break;
		}

		// Checking Results
		if (result == 0) printf("OK\n");
		else printf ("FAILED %d TIMES\n", result);
	}

	if ( munmap( virtual_base, HW_REGS_SPAN ) != 0 ) {
		printf( "ERROR: munmap() failed...\n" );
		close( fd );
		return( 1 );

	}

	close ( fd );
	return 0;
}

void init () {
	if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
		printf( "ERROR: could not open \"/dev/mem\"...\n" );
		exit( -1 );
	}

	virtual_base = mmap( NULL, HW_REGS_SPAN, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, HW_REGS_BASE );

	if( virtual_base == MAP_FAILED ) {
		printf( "ERROR: mmap() failed...\n" );
		close( fd );
		exit( -1 );
	}

	// Cast virtual_base to (char *) to suppress arithmetic with (void*) warning
	// Cast results back into (unsigned long *) to work properly
	// Cannot cast virtual_base directly to (unsigned long *) because:
	//	> (unsigned long *)(virtual_base) + 1 == virtual_base + 4 << Seg Faults
	//	> (char *)(virtual_base) + 1 == virtual_base + 1 << OK
	// See: https://groups.google.com/forum/#!topic/comp.lang.c/RRsX0Z3MUjY%5B1-25%5D
	led_addr = (unsigned long *)( (char *)(virtual_base)
		+ ((ALT_LWFPGASLVS_OFST + LED_PIO_BASE) & HW_REGS_MASK) );
	ram_addr = (unsigned long *)( (char *)(virtual_base)
		+ ((ALT_LWFPGASLVS_OFST + RAM_BASE) & HW_REGS_MASK) );
	input_io_addr = (unsigned long *)( (char *)(virtual_base)
		+ ((ALT_LWFPGASLVS_OFST + INPUT_IO_BASE) & HW_REGS_MASK) );
	output_io_addr = (unsigned long *)( (char *)(virtual_base)
		+ ((ALT_LWFPGASLVS_OFST + OUTPUT_IO_BASE) & HW_REGS_MASK) );
	ga_io_addr = (unsigned long *)( (char *)(virtual_base)
		+ ((ALT_LWFPGASLVS_OFST + GA_IO_BASE) & HW_REGS_MASK) );
}

uint32_t check_zero (void) {
	uint32_t fail_count = 0;

	for (uint32_t i=0; i<perm; i++) {
		alt_write_word (input_io_addr, i);
		uint8_t output_get = alt_read_byte (output_io_addr);
		if (output_get != 0) fail_count += 1;
	}

	return fail_count;
}

uint32_t check_a (void) {
	uint32_t fail_count = 0;

	for (uint32_t i=0; i<perm; i++) {
		alt_write_word (input_io_addr, i);

		/* BITSET LIBRARY */
		// bitset<32> input (i);
		// bitset<32> output (alt_read_word (output_io_addr));
		// for (uint8_t j=0; j<grid_dim; j++) {
		// 	if (output[j] != input[j]) {
		// 		fail_count += 1;
		// 		cout << "O: " << output;
		// 		cout << " I: " << input;
		// 		cout << " Fail: " << j << endl;
		// 	}
		// }

		/* WITHOUT BITSET LIBRARY */
		uint8_t output = alt_read_byte (output_io_addr);
		for (uint8_t j=1; j<(expo(2,grid_dim)); j*=2) {
			if ( ((output & j) == 1) != ((i & j) == 1) ) {
				fail_count += 1;
				printf("OUTPUT: %02X I: %02X F%d\n",output, i, j);
			}
		}

		/* PROTOTYPE FUNCTION */
		// if ( ((output & 1) == 1) != ((i & 1) == 1) ) {
		// 	fail_count += 1;
		// 	printf("OUTPUT: %02X I: %02X F1\n",output, i);
		// }
		// if ( ((output & 2) == 1) != ((i & 2) == 1) ) {
		// 	fail_count += 1;
		// 	printf("OUTPUT: %02X I: %02X F2\n",output, i);
		// }
		// if ( ((output & 4) == 1) != ((i & 4) == 1) ) {
		// 	fail_count += 1;
		// 	printf("OUTPUT: %02X I: %02X F4\n",output, i);
		// }
		// if ( ((output & 8) == 1) != ((i & 8) == 1) ) {
		// 	fail_count += 1;
		// 	printf("OUTPUT: %02X I: %02X F8\n",output, i);
		// }

	}

	return fail_count;
}

uint32_t check_b (void) {
	uint32_t fail_count = 0;

	for (uint32_t i=0; i<perm; i++) {
		alt_write_word (input_io_addr, i);

		/* BITSET LIBRARY */
		// bitset<32> input (i);
		// bitset<32> output (alt_read_word (output_io_addr));
		// if (output[0] != input[grid_dim*2]) {
		// 		fail_count += 1;
		// 		cout << "O: " << output;
		// 		cout << " I: " << input << endl;
		// }

		/* WITHOUT BITSET LIBRARY */
		uint8_t output = alt_read_byte (output_io_addr);
		if ( (output == 1) && ((i & expo(2,grid_dim*2-1)) == 0) ) {
			printf("OUTPUT: %02X I: %02X | I&G: %d\n"
				,output, i, expo(2,grid_dim*2-1));
			fail_count += 1;
		}

	}

	return fail_count;
}

uint32_t check_nand (void) {
	uint32_t fail_count = 0;
	bool grid [grid_dim][grid_dim] = {0};

	for (uint32_t i=0; i<perm; i++) {
		// Configure FPGA
		alt_write_word (input_io_addr, i);

		// Simulate Logic
		// Start from the bottom row up
		// Left to right (LSB to MSB)

		/* BITSET LIBRARY */
		// bitset<32> input = (i);
		// bitset<32> output = (alt_read_word (output_io_addr));
		// for (uint8_t y=grid_dim-1; y>=0; y--) {
		// 	for (uint8_t x=0; x<grid_dim; x++) {
		//
		// 		if ((y < grid_dim-1) && (x < grid_dim-1)) {
		// 			// Every other row, every other cell
		// 			grid [y][x] = cell (grid[y+1][x], grid[y+1][x+1], 3);
		// 		} else if ((y < grid_dim-1) && (x == grid_dim-1)) {
		// 			// Every other row && Right most cell
		// 			grid [y][x] = cell (grid[y+1][x], 0, 3);
		// 		} else if (y == grid_dim-1) {
		// 			// Bottom Row
		// 			grid [y][x] = cell (input[2*x], input[2*x+1], 3);
		// 		}
		//
		// 	}
		// }

		/* WITHOUT BITSET LIBRARY */
		uint8_t output = alt_read_byte (output_io_addr);
		for (uint8_t y=grid_dim-1; y!=255; y--) {
			for (uint8_t x=0; x<grid_dim; x++) {
				if ((y < grid_dim-1) && (x < grid_dim-1)) {
					// Every other row, every other cell
					grid [y][x] = cell (grid[y+1][x], grid[y+1][x+1], 3);
				} else if ((y < grid_dim-1) && (x == grid_dim-1)) {
					// Every other row && Right most cell
					grid [y][x] = cell (grid[y+1][x], 0, 3);
				} else if (y == grid_dim-1) {
					// Bottom Row
					bool tmp_a = ( (i & expo(2,2*x)) > 0 );
					bool tmp_b = ( (i & expo(2,2*x+1)) > 0 );
					grid [y][x] = cell (tmp_a, tmp_b, 3);
				}
			}
		}

		// Compare actual results with simulation

		/* BITSET LIBRARY */
		// for (uint8_t x=0; x<grid_dim; x++) {
		// 	if (output[x] != grid[0][x]) {
		// 		fail_count += 1;
		// 		cout << "Output: " << output;
		// 		cout << " Failed @ Bit " << x << endl;
		// 	}
		// 	if (fail_count > 0) {
		// 		for (int i=0; i<grid_dim; i++)
		// 			printf("%d",grid[0][i]);
		// 		printf("\n");
		// 	}
		// }

		/* WITHOUT BITSET LIBRARY */
		for (uint8_t x=0; x<grid_dim; x++) {
			// Compare (Is bit set in output?) <-> (Is bit set in grid?)
			// > If output @ bit being checked is set:
			// >> ((output & expo(2,x)) > 0) == 1
			// >> else ((output & expo(2,x)) > 0) == 0
			// Bit being checked must be the same with simulated grid's bit
			// (Much better code with <bitset> contatiner)
			if ( ((output & expo(2,x)) > 0) != grid[0][x] ) {
				fail_count += 1;
				printf("I%02X : O%02X : bit#%d\n", i, output, x);
			}
		}

		// Debug print
		// printf(" %d %d -> %d %d\n",
		// ((output & expo(2,0)) > 0), ((output & expo(2,1)) > 0),
		// grid[0][0], grid[0][1]);
		// for (int y=0; y<grid_dim; y++) {
		// 	for (int x=0; x<grid_dim; x++) {
		// 		printf("%d ",grid[y][x]);
		// 	}
		// 	printf("\n");
		// }
	}

	return fail_count;
}

uint32_t expo (const uint32_t a, const uint32_t b) {
	uint32_t tmp = 1;

	for (uint32_t i=0; i<b; i++) {
		tmp *= a;
	}

	return tmp;
}

bool cell (const bool a, const bool b, const uint8_t set) {
	switch (set) {
		case 0: return 0;
		case 1: return a;
		case 2: return b;
		case 3: return !(a & b);
		default: return 0;
	}
}
