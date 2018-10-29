
bool fpga_init (void) {
	/* If FPGA was already initialized, reinitialize. */
	if (global.fpga_init == 1) {
		close (fd);
	}

	/* Read Device Memory */
	fd = open("/dev/mem", (O_RDWR | O_SYNC));
	/* If failed to open /dev/mem */
	if (fd == -1) {
		perror ("COULD NOT OPEN /dev/mem");
		return (0);
	}

	/* Map Memory for use */
	virtual_base = mmap
	(NULL, HW_REGS_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, HW_REGS_BASE);
	/* If failed to map memory */
	if (virtual_base == MAP_FAILED) {
		perror ("MMAP FAIL");
		close (fd);
		return (0);
	}

	/* Assign address pointers - Notes on all the type casts //
	Cast virtual_base to (char *) to suppress arithmetic with (void*) warning,
	then cast results back into (unsigned long *) to work properly

	Cannot cast virtual_base directly to (unsigned long *) because pointer arithmetic:
		> long int *p + 1 == *p + 4 << Causes overflow and SEGFAULTs
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

	fpga_clear ();

	return 1;
}

/* Verifies the FPGA logic circuitry */
void fpga_verify (void) {
	/* Check FPGA Initialization */
	if (global.fpga_init == 0) {
		perror ("FPGA Uninitialized");
		return;
	}

	printf ("Verifying FPGA Cell Array\n"
			"Physical Dimensions: %d x %d\n"
			"Utilized Dimensions: %d x %d\n"
			"Utilized Cell Count: %d\n",
			MAX_CA_DIMX, MAX_CA_DIMY,
			global.CA.DIMX, global.CA.DIMY,
			global.CA.DIMX * global.CA.DIMY);

	uint8_t **grid;
	grid = (uint8_t **) calloc (64, sizeof (uint8_t *));
	for (unsigned int i=0; i<64; i++) {
		grid[i] = (uint8_t *) calloc (64, sizeof (uint8_t));
	}

	// ===== Pass A =====
	puts ("\nSet A");
	for (int y=0; y<64; y++) {
		if (y==63) {
			for (int x=0; x<64; x++) {
				grid [y][x] = 0x9;
			}
		} else {
			for (int x=0; x<64; x++) {
				grid [y][x] = 0x1;
			}
		}
	}
	fpga_set_grid (grid);

	printf (" Input: 0000000000000000 \n");
	fpga_set_input (0x0);
	printf ("Expect: 0000000000000000 \n");
	printf ("Output: %016llX\n", fpga_get_output ());

	printf (" Input: FFFFFFFFFFFFFFFF \n");
	fpga_set_input (0xFFFFFFFFFFFFFFFF);
	printf ("Expect: FFFFFFFFFFFFFFFF \n");
	printf ("Output: %016llX\n", fpga_get_output ());

	printf (" Input: DEADBEEFABCDEF12 \n");
	fpga_set_input (0xDEADBEEFABCDEF12);
	printf ("Expect: DEADBEEFABCDEF12 \n");
	printf ("Output: %016llX\n", fpga_get_output ());

	// ===== Pass B =====
	puts ("\nSet B");
	for (int y=0; y<64; y++) {
		if (y==63) {
			for (int x=0; x<64; x++) {
				grid [y][x] = 0xA;
			}
		} else {
			for (int x=0; x<64; x++) {
				grid [y][x] = 0x2;
			}
		}
	}
	fpga_set_grid (grid);

	printf (" Input: 0000000000000000 \n");
	fpga_set_input (0x0);
	printf ("Expect: 0000000000000000 \n");
	printf ("Output: %016llX\n", fpga_get_output ());

	printf (" Input: FFFFFFFFFFFFFFFF \n");
	fpga_set_input (0xFFFFFFFFFFFFFFFF);
	printf ("Expect: 0000000000000000 \n");
	printf ("Output: %016llX\n", fpga_get_output ());

	printf (" Input: DEADBEEFABCDEF12 \n");
	fpga_set_input (0xDEADBEEFABCDEF12);
	printf ("Expect: 0000000000000000 \n");
	printf ("Output: %016llX\n", fpga_get_output ());

	// ===== NAND =====
	puts ("\nSet NAND");
	for (int y=0; y<64; y++) {
		if (y==63) {
			for (int x=0; x<64; x++) {
				grid [y][x] = 0xB;
			}
		} else {
			for (int x=0; x<64; x++) {
				grid [y][x] = 3;
			}
		}
	}
	fpga_set_grid (grid);

	printf (" Input: 0000000000000000 \n");
	fpga_set_input (0x0);
	printf ("Expect: 8000000000000000 \n");
	printf ("Output: %016llX\n", fpga_get_output ());

	printf (" Input: FFFFFFFFFFFFFFFF \n");
	fpga_set_input (0xFFFFFFFFFFFFFFFF);
	printf ("Expect: FFFFFFFFFFFFFFFF \n");
	printf ("Output: %016llX\n", fpga_get_output ());

	printf (" Input: DEADBEEFABCDEF12 \n");
	fpga_set_input (0xDEADBEEFABCDEF12);
	printf ("Expect: FFFFFFFFDE0DBEEF \n");
	printf ("Output: %016llX\n", fpga_get_output ());

	// ===== RAND =====
	puts ("\nSet RAND");
	for (int y=0; y<64; y++) {
		if (y==63) {
			for (int x=0; x<64; x++) {
				grid [y][x] = rand () % 4 + 0x8;
			}
		} else {
			for (int x=0; x<64; x++) {
				grid [y][x] = rand () % 4;
			}
		}
	}
	fpga_set_grid (grid);

	printf (" Input: 0000000000000000 \n");
	fpga_set_input (0x0);
	printf ("Output: %016llX\n", fpga_get_output ());

	printf (" Input: FFFFFFFFFFFFFFFF \n");
	fpga_set_input (0xFFFFFFFFFFFFFFFF);
	printf ("Output: %016llX\n", fpga_get_output ());

	printf (" Input: DEADBEEFABCDEF12 \n");
	fpga_set_input (0xDEADBEEFABCDEF12);
	printf ("Output: %016llX\n", fpga_get_output ());

	// ===== Cleanup =====
	for (unsigned int y=0; y<64; y++) {
		free (grid[y]);
	}
	free (grid);
	fpga_clear ();
}

/* ----- Level 1 ----- */

void fpga_clear (void) {

	for (uint32_t i=0; i<S2_RANGE; i++) {
		fpga_s2_write (i, 0x00000000);
	}

	fpga_set_input (0x0);
}

void fpga_set_grid (uint8_t **grid) {
	uint32_t data = 0;
	uint32_t offset = S2_RANGE;

	for (int y=MAX_CA_DIMY-1; y>=0; y--) {
		for (int x=MAX_CA_DIMX-1; x>=0; x--) {
			data <<= 4;
			data |= grid[y][x];
			if (x % 8 == 0) {
				offset--;
				fpga_s2_write (offset, data);
				// printf ("Y: %2d | X: %2d | OFFSET: %d | DATA: %08X\n", y, x, offset, data);
			}
		}
	}

}

void fpga_set_input (const uint64_t data) {
	int offset = MAX_CA_DIMX / AVALON_PORT_WIDTH;
	uint32_t tmp[S1_RANGE];

	for (offset = offset-1; offset >= 0; offset--) {
		tmp [offset] = (uint32_t)(data >> (AVALON_PORT_WIDTH*offset));
		fpga_s1_write (offset, tmp[offset]);
	}
}

uint64_t fpga_get_output (void) {
	int offset = MAX_CA_DIMX / AVALON_PORT_WIDTH;
	uint64_t results = 0;

	for (offset = offset-1; offset >= 0; offset--) {
		results <<=  AVALON_PORT_WIDTH;
		results |= fpga_s1_read (offset);
	}

	return results;
}

/* ----- Level 2 ----- */

uint32_t fpga_s1_read (const uint32_t offset) {
	return alt_read_word (s1_address + offset);
}

void fpga_s1_write (const uint32_t offset, const uint32_t data) {
	alt_write_word (s1_address + offset, data);
}

void fpga_s2_write (const uint32_t offset, const uint32_t data) {
	alt_write_word (s2_address + offset, data);
}
