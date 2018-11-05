bool fpga_init (void) {
	/* If FPGA was already initialized, reinitialize. */
	if (global.fpga_init == 1) {
		close (fd);
	}

	std::cout << "Initializing FPGA... ";

	/* Read Device Memory */
	fd = open("/dev/mem", (O_RDWR | O_SYNC));
	/* If failed to open /dev/mem */
	if (fd == -1) {
		std::cout << ANSI_RED << "COULD NOT OPEN /dev/mem\n" << ANSI_RESET;
		return (0);
	}

	/* Map Memory for use */
	virtual_base = mmap
	(NULL, HW_REGS_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, HW_REGS_BASE);
	/* If failed to map memory */
	if (virtual_base == MAP_FAILED) {
		std::cout << ANSI_RED << "MMAP FAIL\n" << ANSI_RESET;
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

	std::cout << ANSI_GREEN << "DONE\n" << ANSI_RESET;

	return 1;
}

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

	if (global.CA.DIMX != MAX_CA_DIMX || global.CA.DIMY != MAX_CA_DIMY)
		std::cout << "\n\033[0;33m Custom Dimension Detected. Results May Be Incorrect \033[0m\n";

	// Grid array is always of max physical size. Only data on the array changes.
	uint8_t **grid;
	grid = (uint8_t **) calloc (MAX_CA_DIMY, sizeof (uint8_t *));
	for (unsigned int i=0; i<MAX_CA_DIMX; i++) {
		grid[i] = (uint8_t *) calloc (MAX_CA_DIMX, sizeof (uint8_t));
	}

	// ===== Pass A =====
	for (unsigned int y=0; y<global.CA.DIMY; y++) {
		if (y==(global.CA.DIMY-1)) {
			for (unsigned int x=0; x<global.CA.DIMX; x++) {
				grid [y][x] = 0x9;
			}
		} else {
			for (unsigned int x=0; x<global.CA.DIMX; x++) {
				grid [y][x] = 0x1;
			}
		}
	}
	fpga_set_grid (grid);
	fpga_check (1);

	// ===== Pass B =====
	for (unsigned int y=0; y<global.CA.DIMY; y++) {
		if (y==(global.CA.DIMY-1)) {
			for (unsigned int x=0; x<global.CA.DIMX; x++) {
				grid [y][x] = 0xA;
			}
		} else {
			for (unsigned int x=0; x<global.CA.DIMX; x++) {
				grid [y][x] = 0x2;
			}
		}
	}
	fpga_set_grid (grid);
	fpga_check (2);

	// ===== NAND =====
	for (unsigned int y=0; y<global.CA.DIMY; y++) {
		if (y==(global.CA.DIMY-1)) {
			for (unsigned int x=0; x<global.CA.DIMX; x++) {
				grid [y][x] = 0xB;
			}
		} else {
			for (unsigned int x=0; x<global.CA.DIMX; x++) {
				grid [y][x] = 3;
			}
		}
	}
	fpga_set_grid (grid);
	fpga_check (3);

	// ===== RAND =====
	for (unsigned int y=0; y<global.CA.DIMY; y++) {
		if (y==(global.CA.DIMY-1)) {
			for (unsigned int x=0; x<global.CA.DIMX; x++) {
				grid [y][x] = rand () % 4 + 0x8;
			}
		} else {
			for (unsigned int x=0; x<global.CA.DIMX; x++) {
				grid [y][x] = rand () % 4;
			}
		}
	}
	fpga_set_grid (grid);
	fpga_check (4);

	// ===== NULL =====
	for (unsigned int y=0; y<global.CA.DIMY; y++) {
		if (y==(global.CA.DIMY-1)) {
			for (unsigned int x=0; x<global.CA.DIMX; x++) {
				grid [y][x] = 0x8;
			}
		} else {
			for (unsigned int x=0; x<global.CA.DIMX; x++) {
				grid [y][x] = 0;
			}
		}
	}
	fpga_set_grid (grid);
	fpga_check (0);

	// ===== Cleanup =====
	for (unsigned int y=0; y<global.CA.DIMY; y++) {
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

	/* This function handles setting the input rows
		WARNING: If this is not set, the FPGA Cell Array will have no linux input cells set.
		Output may remain the same, regardless of given input.
	*/
	for (unsigned int x=0; x<MAX_CA_DIMX; x++) {
		grid [global.CA.DIMY-1][x] |= 0x8;
	}

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

	/* Force reads output twice */
	for (offset = offset-1; offset >= 0; offset--) {
		results <<=  AVALON_PORT_WIDTH;
		results |= fpga_s1_read (offset);
	}
	for (offset = offset-1; offset >= 0; offset--) {
		results <<=  AVALON_PORT_WIDTH;
		results |= fpga_s1_read (offset);
	}

	return results;
}

void fpga_check (int mode) {
	const uint64_t input[3] = {0x0000000000000000, 0xFFFFFFFFFFFFFFFF, 0xDEADBEEFABCDEF12};

	uint64_t expected [3];
	uint64_t observed;

	switch (mode) {
		case 0: // Null
			puts ("\nSet NULL");
			expected [0] = 0x0;
			expected [1] = 0x0;
			expected [2] = 0x0;
			break;
		case 1: // Pass A
			puts ("\nSet A");
			expected [0] = 0x0;
			expected [1] = 0xFFFFFFFFFFFFFFFF;
			expected [2] = 0xDEADBEEFABCDEF12;
			break;
		case 2: // Pass B
			puts ("\nSet B");
			expected [0] = 0x0;
			expected [1] = 0x0;
			expected [2] = 0x0;
			break;
		case 3: // NAND
			puts ("\nSet NAND");
			expected [0] = 0x8000000000000000;
			expected [1] = 0xFFFFFFFFFFFFFFFF;
			expected [2] = 0xFFFFFFFFDE0DBEEF;
			break;
		default: // Unknown
			puts ("\nSet RAND");
			break;
	}

	// For NULL, PASS A, PASS B, NAND
	if (mode <= 3) {
		for (int i=0; i<3; i++) {
			fpga_set_input (input [i]);
			printf (" Input: %016llX\nExpect: %016llX\n", input[i], expected[i]);
			observed = fpga_get_output ();

			printf ("Output: %016llX | ", observed);
			if (observed == expected [i])  {
				std::cout << ANSI_GREEN << "OK\n" << ANSI_RESET;
			} else {
				std::cout << ANSI_YELLOW "WARN\n" << ANSI_RESET;
			}
		}

	// RAND and others
	} else {
		for (int i=0; i<3; i++) {
			fpga_set_input (input [i]);
			printf (" Input: %016llX\n", input[i]);
			printf ("Output: %016llX\n", fpga_get_output ());
		}
	}

	return;
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
