/* Simulation Wrapper File */

int run_sim (void) {
	#define SUCCESS 1
	#define FAILED -1

	/* Prints Parameter List */
	printf (
		"\n\t>>>-- Initializing Simulation --<<<\n"
		"\tPOP = %4u | GEN = %4u | MUT = %0.3f | POOL = %4u\n"
		"\tDIMX = %3u | DIMY = %3u | COLOR = %3u | NEIGHBOR = %2u\n"
		"\tFIT = %1u | TIME = %1u | CAPRINT = %1u | EXPORT = %1u\n\n",
		global.GA.POP, global.GA.GEN, global.GA.MUTP, global.GA.POOL,
		global.CA.DIMX, global.CA.DIMY, global.CA.COLOR, global.CA.NB,
		global.DATA.FIT, global.DATA.TIME, global.DATA.CAPRINT, global.DATA.EXPORT
	);

	/* Temporary variables, for easier reference */
	const unsigned int dimx = global.CA.DIMX;
	const unsigned int dimy = global.CA.DIMY;
	const unsigned int gen_lim = global.GA.GEN;
	const unsigned int pop_lim = global.GA.POP;
	/* Initializes Random Number Generator */
	srand (time (NULL));
	/* Initializes Global UID Counter */
	global.UID = 0;

	/* Computes DNA length, the number of CA rules; Calculated by -> Color^Neighbor */
	uint32_t dna_length = pow (global.CA.COLOR, global.CA.NB);

	/* Allocates an array of individuals (population), of size "global.GA.POP" */
	GeneticAlgorithm *indv = (GeneticAlgorithm *) calloc (pop_lim, sizeof (GeneticAlgorithm));

	/* Initialize Population */
	for (unsigned int idx=0; idx<pop_lim; idx++) {
		indv[idx] = GeneticAlgorithm (global.UID, dna_length);
		global.UID++;
	}

	/* Allocates 2D working array for Cellular Automaton */
	uint8_t **grid = (uint8_t **) calloc (dimy, sizeof (uint8_t));
	for (unsigned int i=0; i<dimy; i++) {
		grid[i] = (uint8_t *) calloc (dimx, sizeof (uint8_t));
	}

	/* Initialize Cellular Automaton grid seed */
	global.CA.SEED = (uint8_t *) calloc (dimx, sizeof (uint8_t));
	unsigned int mid = floor (dimx / 2);
	global.CA.SEED [mid] = 1;

	/* ===== SIMULATION LOOP ===== */
	printf ("\tBeginning Simulation\n");

	/* Loop over each generation */
	for (unsigned int gen=0; gen<gen_lim; gen++) {
		/* Perform selection, crossover, and mutation */
		ga.Selection (indv);

		/* Loop over each individual */
		for (unsigned int idx=0; idx<pop_lim; idx++) {
			/* Skips evaluation, if already done for this individual */
			if (indv[idx].eval == 1) continue;
			printf ("\nID: %u | DNA: ", idx);
			for (int i=0; i<dna_length; i++)
				printf ("%u ", indv[idx].dna[i]);
			printf ("\n");

			/* Generate CA Array //
			1. Generate first row, with SEED
			2. Iterate over entire grid once; generate current row with previous row.
			(Bounce Back)
			3. Generate first row again, with last row of grid
			4. Iterate over entire grid again, same as step 2.
			*/
			cellgen (global.CA.SEED, grid[0], indv[idx].dna);
			ca_graph (grid[0], dimx);

			for (unsigned int y=1; y<dimy; y++) {
				cellgen (grid[y-1], grid[y], indv[idx].dna);
				ca_graph (grid[y], dimx);
			}

			cellgen (grid[dimy-1], grid[0], indv[idx].dna);
			ca_graph (grid[0], dimx);

			for (unsigned int y=1; y<dimy; y++) {
				cellgen (grid[y-1], grid[y], indv[idx].dna);
				ca_graph (grid[y], dimx);
			}

			/* Edit FPGA RAM */
			/* Evaluate Circuit */
		}

		ga.Sort (indv);
		status (gen);
	}

	/* ===== SIMULATION LOOP ===== */

	/* Free Seed Array */
	// free (global.CA.SEED);
	//
	// /* Manually free memory and return to main menu
	// 	A custom destructor for GA Class will cause bugs, and segmentation faults.
	// 	Probably due to the destructor being called automatically by the compiler at some point,
	// 	this breaks the program and will likely cause a segmentation fault.
	//
	// 	Thus, memory is freed manually here. (For GeneticAlgorithm Class Objects)
	//
	// 	Presumably, CA Grid is freed at the end of the function automatically;
	// 	as free (grid) throws a double free error.
	// */
	// for (unsigned int idx=0; idx<pop_lim; idx++) {
	// 	/* Frees member variable first to prevent orphans. Not sure if this is correct. */
	// 	if (indv[idx].dna != NULL) free (indv[idx].dna);
	// }
	// free (indv);

	return SUCCESS;
}

void status (const unsigned int gen) {
	if (gen % 10 == 0) {
		printf ("%4u / %4u Done \r", gen, global.GA.GEN);
	}
}

// void generate_ca (uint8_t **grid, GeneticAlgorithm indv) {
//
// }
