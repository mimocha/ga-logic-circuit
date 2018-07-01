// memdump

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <iostream>

#define MAP_LENG 0x00100000

int main (int argc, char **argv) {
	int fd, i;
	unsigned int *addr;
	unsigned int offset, count;

	fd = open( "/dev/mem", O_RDWR );
	if (fd == -1) {
		printf("Can't open /dev/mem.\n");
		return -1;
	}

	if (argv[1] != NULL) {
		offset = strtol(argv[1], NULL, 16);
		count = strtol(argv[2], NULL, 10);
		mmap(addr, MAP_LENG, PROT_READ | PROT_WRITE,
			 MAP_SHARED, fd, offset & 0xFFFF0000);
		if (addr == MAP_FAILED) {
			printf("Error: mmap()\n");
			return -1;
		}

		printf("MEMDUMP | ADRS(hex): LEN(dec)\n");
		for (i=0; i<count; i++) {
			printf("%08X: %08X\n",
				( (offset & 0xFFFFFFFC) + (i * 4) ),
				( addr[((offset & 0x0000FFFC) / 4) + i] ) );
		}
		printf("\n");

	} else {
		char buff[9];

		printf("MEMDUMP | ADRS(hex): LEN(dec)\n");
		while (1) {
			std::cin >> buff;
			offset = strtol(buff, NULL, 16);
			mmap(addr, MAP_LENG, PROT_READ | PROT_WRITE,
				 MAP_SHARED, fd, offset & 0xFFFF0000);
			if (addr == MAP_FAILED) {
				printf("Error: mmap()\n");
				return -1;
			}

			printf("%08X: %08X\n",
				( (offset & 0xFFFFFFFC) ),
				( addr[(offset & 0x0000FFFC)] ) );
		}
	}

	return 0;
}
