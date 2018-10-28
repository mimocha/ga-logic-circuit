
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

/*!
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
 */

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
		for (int x=0; x<64; x++) {
			grid [y][x] = 1;
		}
	}
	fpga_set_grid (grid);

	printf ("Set Input: 00000000 00000000 \n");
	fpga_set_input (0x0);
	fpga_get_output ();
	fpga_get_output ();
	fpga_get_output ();

	printf ("Set Input: FFFFFFFF FFFFFFFF \n");
	fpga_set_input (0xFFFFFFFFFFFFFFFF);
	fpga_get_output ();
	fpga_get_output ();
	fpga_get_output ();

	printf ("Set Input: DEADBEEF ABCDEF12 \n");
	fpga_set_input (0xDEADBEEFABCDEF12);
	fpga_get_output ();
	fpga_get_output ();
	fpga_get_output ();

	// ===== Pass B =====
	puts ("\nSet B");
	for (int y=0; y<64; y++) {
		for (int x=0; x<64; x++) {
			grid [y][x] = 2;
		}
	}
	fpga_set_grid (grid);

	printf ("Set Input: 00000000 00000000 \n");
	fpga_set_input (0x0);
	fpga_get_output ();
	fpga_get_output ();
	fpga_get_output ();

	printf ("Set Input: FFFFFFFF FFFFFFFF \n");
	fpga_set_input (0xFFFFFFFFFFFFFFFF);
	fpga_get_output ();
	fpga_get_output ();
	fpga_get_output ();

	printf ("Set Input: DEADBEEF ABCDEF12 \n");
	fpga_set_input (0xDEADBEEFABCDEF12);
	fpga_get_output ();
	fpga_get_output ();
	fpga_get_output ();

	// ===== NAND =====
	puts ("\nSet NAND");
	for (int y=0; y<64; y++) {
		for (int x=0; x<64; x++) {
			grid [y][x] = 3;
		}
	}
	fpga_set_grid (grid);
	fpga_get_output ();
	fpga_get_output ();
	fpga_get_output ();

	printf ("Set Input: 00000000 00000000 \n");
	fpga_set_input (0x0);
	fpga_get_output ();
	fpga_get_output ();
	fpga_get_output ();

	printf ("Set Input: FFFFFFFF FFFFFFFF \n");
	fpga_set_input (0xFFFFFFFFFFFFFFFF);
	fpga_get_output ();
	fpga_get_output ();
	fpga_get_output ();

	printf ("Set Input: DEADBEEF ABCDEF12 \n");
	fpga_set_input (0xDEADBEEFABCDEF12);
	fpga_get_output ();
	fpga_get_output ();
	fpga_get_output ();

	// ===== Cleanup =====
	for (unsigned int y=0; y<64; y++) {
		free (grid[y]);
	}
	free (grid);
}

/* ----- Level 1 ----- */

void fpga_clear (void) {
	// Set all cells to 0
	for (uint32_t i=0; i<S2_RANGE; i++) {
		fpga_s2_write (i, 0x00000000);
	}
}

void fpga_set_grid (uint8_t **grid) {
	uint32_t data;

	switch (grid[0][0]) {
		case 0: data = 0x00000000; break;
		case 1: data = 0x11111111; break;
		case 2: data = 0x22222222; break;
		case 3: data = 0x33333333; break;
		default:  data = 0xFEFEFEFE; break;
	}

	for (int i=0; i<S2_RANGE; i++) {
		if (i == 504) data |= 0x88888888;
		fpga_s2_write (i, data);
	}
}

void fpga_set_input (const uint64_t data) {
	uint32_t tmp[2];

	tmp [0] = (uint32_t)data;
	tmp [1] = (uint32_t)(data>>32);

	fpga_s1_write (0, tmp[0]);
	fpga_s1_write (1, tmp[1]);
}

uint64_t fpga_get_output (void) {
	printf ("Output: %08X %08X\n",
	fpga_s1_read(1), fpga_s1_read(0));

	return 1;
}

/* ----- Level 2 ----- */

uint32_t fpga_s1_read (const uint32_t offset) {
	return alt_read_word (s1_address + offset);
}

void fpga_s1_write (const uint32_t offset, const uint32_t data) {
	alt_write_word (s1_address + offset, data);
}

void fpga_s2_write (const uint32_t offset, const uint32_t data) {
	// Write 32-bit unsigned int to selected offset.
	// Writing only once has been proven fine.
	alt_write_word (s2_address + offset, data);
}
