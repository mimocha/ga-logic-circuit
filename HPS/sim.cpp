/* Simulation Wrapper File */

void sim_init (void) {
	/* Prints Parameter List */
	printf (ANSI_REVRS "\n\t>>>-- Initializing Simulation --<<<\n" ANSI_RESET
		"\tPOP = %4u | GEN = %4u | MUT = %0.3f | POOL = %4u\n"
		"\tDIMX = %3u | DIMY = %3u | COLOR = %3u | NEIGHBOR = %2u\n"
		"\tFIT = %1u | TIME = %1u | CAPRINT = %1u | EXPORT = %1u | FPGA INIT = %1u\n\n",
		global.GA.POP, global.GA.GEN, global.GA.MUTP, global.GA.POOL,
		global.CA.DIMX, global.CA.DIMY, global.CA.COLOR, global.CA.NB,
		global.DATA.TRACK, global.DATA.TIME, global.DATA.CAPRINT, global.DATA.EXPORT,
		global.fpga_init
	);

	/* Initializes Random Number Generator */
	srand (time (NULL));

	/* Initializes Statistics Variables*/
	if (global.DATA.TRACK == 1) {
		/* Stores current setting */
		global.stats.gen = global.GA.GEN;
		global.stats.pop = global.GA.POP;
		global.stats.dimx = global.CA.DIMX;
		global.stats.dimy = global.CA.DIMY;
		global.stats.color = global.CA.COLOR;
		global.stats.nb = global.CA.NB;
		global.stats.tts = 0;

		// For data arrays, check if NULL. If not null, free()
		if (global.stats.avg != NULL) free (global.stats.avg);
		global.stats.avg = (float *) calloc (global.GA.GEN, sizeof (float));
		if (global.stats.med != NULL) free (global.stats.med);
		global.stats.med = (float *) calloc (global.GA.GEN, sizeof (float));
		if (global.stats.max != NULL) free (global.stats.max);
		global.stats.max = (unsigned int *) calloc (global.GA.GEN, sizeof (unsigned int));
		if (global.stats.min != NULL) free (global.stats.min);
		global.stats.min = (unsigned int *) calloc (global.GA.GEN, sizeof (unsigned int));
	}
}

bool run_sim (void) {

	/* ===== ELABORATION PHASE ===== */

	sim_init ();

	/* Temporary variables, for easier reference */
	const unsigned int dimx = global.CA.DIMX;
	const unsigned int dimy = global.CA.DIMY;
	const unsigned int gen_lim = global.GA.GEN;
	const unsigned int pop_lim = global.GA.POP;

	/* Computes DNA length, the number of CA rules;
		Calculated by -> Color^Neighbor (exponent, not XOR)
	*/
	uint32_t dna_length = pow (global.CA.COLOR, global.CA.NB);

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

	/* If TIME is enabled, initialize time_t */
	time_t timer;
	const float time_est = (gen_lim * pop_lim / 2 / INDV_PER_SEC);
	if (global.DATA.TIME == 1) {
		timer = clock ();
	}

	/* If no Truth Table set */
	if (global.tt_init == 0) {

	}
	uint64_t output = 0;

	/* ===== END ELABORATION ===== */

	printf ("\tSimulation Progress:\n");
	fpga_set_input (0xDEADBEEFABCDEF12);
	const uint64_t desired = 0xFFFFFFFFDE0DBEEF;

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

			cellgen (grid[dimy-1], grid[0], indv[idx].dna);
			for (unsigned int y=1; y<dimy; y++) {
				cellgen (grid[y-1], grid[y], indv[idx].dna);
			}

			/* Edit FPGA RAM - Only if FPGA array initialized - Assumes correct setting */
			if (global.fpga_init == 1) {
				fpga_set_grid (grid);
				output = fpga_get_output ();
			}

			/* Evaluate Circuit */
			// A XNOR B | aka. A == B
			indv[idx].fit = bitcount64 ( ~(desired^output) );
			indv[idx].eval = 1;
		}

		GeneticAlgorithm::Sort (indv);
		statistics (indv, gen);

		// TODO: Calculate maximum fitness value for a given TT.
		if ((global.stats.tts == 0) && (indv[0].fit == 64)) {
			global.stats.tts = gen;
		}

		/* Status Report */
		if (gen % 10 == 0) {
			// Current Progress
			printf ("\t%4u / %4u ", gen, global.GA.GEN);

			// Estimate Time Arrival
			if (global.DATA.TIME == 1) {
				float eta = time_est - ((float)(clock() - timer) / CLOCKS_PER_SEC);
				printf (" | ETA:%8.1f s", eta);
			}

			// Flags / Warnings / Notes
			// TODO: Calculate maximum fitness value for a given TT.
			if (global.stats.tts != 0) {
				cout << ANSI_GREEN " << Solution Found!" ANSI_RESET;
			}

			cout << "\n";
		}
	}

	/* ===== END SIMULATION ===== */

	/* Prints Pretty Results */
	cout << ANSI_GREEN "\tDONE : ";
	cout << ((float)(clock() - timer) / CLOCKS_PER_SEC) << " s\n\n" ANSI_RESET;

	/* Solution not found */
	if (global.stats.tts == 0) {
		cout << ANSI_RED "No solution has been found\n" ANSI_RESET;
	/* Solution found */
	} else {
		cout << ANSI_GREEN "First solution found in: " << global.stats.tts << " gen\n" ANSI_RESET;
	}

	printf ("Final Fitness Statistics:\n"
			"Average Fitness: %.1f\n"
			"Median Fitness: %.1f\n"
			"Maximum Fitness: %u\n"
			"Minimum Fitness: %u\n\n",
			global.stats.avg [gen_lim-1], global.stats.med [gen_lim-1],
			global.stats.max [gen_lim-1], global.stats.min [gen_lim-1] );

	printf ("Best Solution: UID: %u | FIT: %u\n"
			"DNA: ", indv[0].uid, indv[0].fit );
	indv[0].print_dna();
	cout << "\n\n";

	/* Optional Print of Fittest Solution */
	if (global.DATA.CAPRINT == 1) {
		printf ("\e[100m\tLogic Circuit" ANSI_RESET "\n");

		cellgen (global.CA.SEED, grid[0], indv[0].dna);
		for (unsigned int y=1; y<dimy; y++) {
			cellgen (grid[y-1], grid[y], indv[0].dna);
		}

		cellgen (grid[dimy-1], grid[0], indv[0].dna);
		for (unsigned int y=1; y<dimy; y++) {
			cellgen (grid[y-1], grid[y], indv[0].dna);
		}

		print_grid (grid);
		cout << endl;
	}

	/* ===== CLEANUP =====
	Manually Memory Management Memo //
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

void print_grid (uint8_t **grid) {
	for (unsigned int i=0; i<global.CA.DIMY; i++) {
		ca_graph (grid [i], global.CA.DIMX);
	}
}

uint64_t bitcount64 (uint64_t x) {
	// Code shamelessly copied from https://en.wikipedia.org/wiki/Hamming_weight
	const uint64_t m1  = 0x5555555555555555;
	const uint64_t m2  = 0x3333333333333333;
	const uint64_t m4  = 0x0f0f0f0f0f0f0f0f;

	x -= (x >> 1) & m1;             //put count of each 2 bits into those 2 bits
	x = (x & m2) + ((x >> 2) & m2); //put count of each 4 bits into those 4 bits
	x = (x + (x >> 4)) & m4;        //put count of each 8 bits into those 8 bits
	x += x >>  8;	//put count of each 16 bits into their lowest 8 bits
	x += x >> 16;	//put count of each 32 bits into their lowest 8 bits
	x += x >> 32;	//put count of each 64 bits into their lowest 8 bits
	return x & 0x7f;
}

void statistics (GeneticAlgorithm *array, const unsigned int gen) {
	const uint32_t pop = global.GA.POP;
	float average = 0.0;
	float median = 0;

	/* Get average value */
	for (uint32_t i=0; i<pop; i++) {
		average += array[i].fit;
	}
	average /= pop;
	global.stats.avg [gen] = average;

	/* Get median value (list is presorted) */
	if (global.GA.POP % 2 == 0) {
		median = array[pop/2].fit + array[(pop/2)-1].fit;
		global.stats.med [gen] = median/2;
	} else {
		global.stats.med [gen] = array[pop/2].fit;
	}

	/* Get max value (list is presorted) */
	global.stats.max [gen] = array[0].fit;

	/* Get min value (list is presorted) */
	global.stats.min [gen] = array[pop-1].fit;
}
