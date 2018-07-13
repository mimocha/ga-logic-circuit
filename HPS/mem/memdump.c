// memdump

// Standard C libraries
#include <stdio.h>
#include <stdlib.h>
// Linux Kernel API
#include <fcntl.h>
#include <sys/mman.h>
// Altera API
#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"
#include "socal/alt_gpio.h"
#include "hps_0.h"

#define MAP_LENG 0x00100000

#define HW_REGS_BASE ( ALT_STM_OFST )
#define HW_REGS_SPAN ( 0x04000000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )

void main (int argc, char **argv) {
	int fd, i;
	unsigned int *addr;
	unsigned int offset, count;

	fd = open( "/dev/mem", (O_RDWR | O_SYNC) );
	if (fd == -1) {
		printf("Can't open /dev/mem.\n");
		return;
	}

	offset = strtol(argv[1], NULL, 16);
	count = strtol(argv[2], NULL, 10);

	addr = mmap( NULL, MAP_LENG, PROT_READ | PROT_WRITE,
		 MAP_SHARED, fd, offset & 0xFFFF0000);
	if (addr == MAP_FAILED) {
		printf("Error: mmap()\n");
		return;
	}

	printf("MEMDUMP | ADRS(hex): LEN(dec)\n");
	for (i=0; i<count; i++) {
		printf("%08X: %08X\n",
			( (offset & 0xFFFFFFFC) + (i * 4) ),
			( addr[((offset & 0x0000FFFC) / 4) + i] ) );
	}
	printf("\n");

	return;
}
