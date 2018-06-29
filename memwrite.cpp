//memwrite.c

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <iostream>

#define MAP_LENG 0x00100000

int main (int argc, char **argv) {
	int fd, i;
	unsigned int *addr;
	unsigned int offset, data;

	fd = open( "/dev/mem", O_RDWR );
	if (fd == -1) {
		printf("Can't open /dev/mem.\n");
		return -1;
	}

	offset = strtol(argv[1], NULL, 16);
	data = strtol(argv[2], NULL, 16);
	addr = mmap(NULL, MAP_LENG, PROT_READ | PROT_WRITE,
		 MAP_SHARED, fd, offset & 0xFFFF0000);
	if (addr == MAP_FAILED) {
		printf("Error: mmap()\n");
		return -1;
	}

	addr[(offset & 0x0000FFFC) / 4] = data;

	printf("MEMWRITE | ADRS(hex): LEN(dec)\n");
	printf("%08X: %08X\n",
		( offset & 0xFFFFFFFC ),
		( addr[(offset & 0x0000FFFC) / 4] ) );

	return 0;
}
