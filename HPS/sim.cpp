/* Simulation Wrapper File */

int run_sim (void) {

	/* ===== ELABORATION PHASE ===== */

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

	/* Initializes Random Number Generator */
	srand (time (NULL));
	/* Temporary variables, for easier reference */
	const unsigned int dimx = global.CA.DIMX;
	const unsigned int dimy = global.CA.DIMY;
	const unsigned int gen_lim = global.GA.GEN;
	const unsigned int pop_lim = global.GA.POP;
	/* Computes DNA length, the number of CA rules; Calculated by -> Color^Neighbor */
	uint32_t dna_length = pow (global.CA.COLOR, global.CA.NB);
	/* If TIME is enabled, initialize time_t */
	time_t timer;
	if (global.DATA.TIME == 1) timer = clock ();

	/* Allocates an array of individuals (population), of size "global.GA.POP" */
	GeneticAlgorithm *indv;
	indv = (GeneticAlgorithm *) calloc (pop_lim, sizeof (GeneticAlgorithm));
	/* Initialize Population */
	for (unsigned int idx=0; idx<pop_lim; idx++) {
		indv[idx] = GeneticAlgorithm (dna_length);
	}

	/* Allocates 2D working array for Cellular Automaton //
		Note:
		Make sure CALLOC gets argument 2: sizeof(uint8_t *) and not sizeof(uint8_t)
		This mistake will cause attempts to free grid[0] at the end to fail, throwing a double free.
	*/
	uint8_t **grid;
	grid = (uint8_t **) calloc (dimy, sizeof (uint8_t *));
	for (unsigned int i=0; i<dimy; i++) {
		grid[i] = (uint8_t *) calloc (dimx, sizeof (uint8_t));
	}
	/* Initialize Cellular Automaton grid seed */
	global.CA.SEED = (uint8_t *) calloc (dimx, sizeof (uint8_t));
	unsigned int mid = floor (dimx / 2);
	global.CA.SEED [mid] = 1;

	/* ===== END ELABORATION ===== */

	printf ("\tBeginning Simulation\n");

	/* ===== SIMULATION LOOP ===== */

	/* Loop over each generation */
	for (unsigned int gen=0; gen<gen_lim; gen++) {
		/* Perform selection, crossover, and mutation */
		GeneticAlgorithm::Selection (indv);

		/* Loop over each individual */
		for (unsigned int idx=0; idx<pop_lim; idx++) {
			/* Automatically ages an individual */
			indv[idx].age++;
			/* Skips evaluation, if already done for this individual */
			if (indv[idx].eval == 1) continue;

			/* Optional Print */
			if (global.DATA.CAPRINT == 1) ca_graph (global.CA.SEED, global.CA.DIMX);

			/* Generate CA Array //
			1. Generate first row, with SEED
			2. Iterate over entire grid once; generate current row with previous row.
			(Bounce Back)
			3. Generate first row again, with last row of grid
			4. Iterate over entire grid again, same as step 2.
			*/
			cellgen (global.CA.SEED, grid[0], indv[idx].dna);
			for (unsigned int y=1; y<dimy; y++) {
				cellgen (grid[y-1], grid[y], indv[idx].dna);
			}

			/* Optional Print */
			if (global.DATA.CAPRINT == 1) print_grid (grid);

			cellgen (grid[dimy-1], grid[0], indv[idx].dna);
			for (unsigned int y=1; y<dimy; y++) {
				cellgen (grid[y-1], grid[y], indv[idx].dna);
			}

			/* Optional Print */
			if (global.DATA.CAPRINT == 1) print_grid (grid);

			/* Edit FPGA RAM */
			/* Evaluate Circuit */
			indv[idx].eval = 1;

			/* Optional Print */
			if (global.DATA.CAPRINT == 1) cout << endl;
		}

		GeneticAlgorithm::Sort (indv);

		/* Status Report */
		if (gen % 10 == 0) {
			status (gen);
			if ((gen % 50 == 0) && (global.DATA.TIME == 1)) {
				cout << "| ";
				eta (gen, timer);
				timer = clock ();
			}
			cout << endl;
		}
	}

	/* ===== END SIMULATION ===== */

	printf ("\tDONE\n");

	/* ===== CLEANUP ===== */
	/* Manually Memory Management Memo //
		> Note 1:
		A custom destructor for GA Class causes a bug, throwing SEGFAULT when freeing indv.dna
		Likely due to bad memory management at some point.
		It is unclear whether or not this bug still persists.
		But to avoid this problem entirely, memory is freed manually here.

		> Note 2:
		Make sure 'grid' CALLOC gets argument 2: 'sizeof(uint8_t *)' and not 'sizeof(uint8_t)'
		This mistake will cause attempts to free grid[0] at the end to fail.
		* This will throw a double free error, if attempts are made to free 'grid'.
		* Or if attempts to free 'grid' are not made, SIGABRT will be thrown, when simulations are ran again, within the same ga-program instance.

		> Note 3:
		Mistakes in 'grid' CALLOC also led to the strange problem of SIGABRT when working with grid[4], and specifically only grid[4].
		Probably due to the specific size of (uint8_t) and memory address stuff.
		Needless to say, don't mess CALLOC up again.
	*/

	/* Free Seed Array */
	free (global.CA.SEED);

	/* Free Grid Array */
	for (unsigned int y=0; y<dimy; y++) {
		free (grid[y]);
	}
	free (grid);

	/* Free GA Class Objects */
	for (unsigned int idx=0; idx<pop_lim; idx++) {
		free (indv[idx].dna);
	}
	free (indv);

	/* ===== END CLEANUP ===== */

	return 1;
}

void status (const unsigned int gen) {
	printf ("%4u / %4u Done ", gen, global.GA.GEN);
}

void eta (const unsigned int gen, const time_t timer) {
	time_t time_diff = clock () - timer;
	float eta = ((global.GA.GEN - gen) / 50) * ((float) time_diff / CLOCKS_PER_SEC);
	printf ("ETA %5.1f s ", eta);
}

void print_grid (uint8_t **grid) {
	for (unsigned int i=0; i<global.CA.DIMY; i++) {
		ca_graph (grid [i], global.CA.DIMX);
	}
}
