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

	/* Renames variables, for easier reference */
	dimx = global.CA.DIMX;
	dimy = global.CA.DIMY;
	gen_lim = global.GA.GEN;
	pop_lim = global.GA.POP;

	/* Computes DNA length, the number of CA rules;
		Calculated by -> Color^Neighbor (exponent, not XOR)
	*/
	dna_length = pow (global.CA.COLOR, global.CA.NB);

	/*Maxmimum fitness possible
		Calculated as: (Output bit width) * (Truth Table Steps)
	*/
	fit_lim = dimx * global.truth.step;

	/* Allocates an array of individuals (population), of size "global.GA.POP" */
	indv = (GeneticAlgorithm *) calloc (pop_lim, sizeof (GeneticAlgorithm));
	/* Initialize Population */
	for (unsigned int idx=0; idx<pop_lim; idx++) {
		indv[idx] = GeneticAlgorithm (dna_length);
	}

	/* Allocates 2D working array for Cellular Automaton //
		WARNING: Make sure CALLOC gets argument 2: sizeof(uint8_t *) and not sizeof(uint8_t)
		This mistake will cause attempts to free grid[0] at the end to fail, throwing a double free.

		NOTE: **grid always has the physical dimension allocated, not the user set dimension
	*/
	grid = (uint8_t **) calloc (MAX_CA_DIMY, sizeof (uint8_t *));
	for (unsigned int i=0; i<MAX_CA_DIMY; i++) {
		grid[i] = (uint8_t *) calloc (MAX_CA_DIMX, sizeof (uint8_t));
	}

	/* Initialize Cellular Automaton grid seed */
	global.CA.SEED = (uint8_t *) calloc (dimx, sizeof (uint8_t));
	unsigned int mid = floor (dimx / 2);
	global.CA.SEED [mid] = 1;

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

	/* If no Truth Table set -- Initialize randomly */
	if (global.tt_init == 0) {
		printf ("No truth table defined. Generating random truth table... ");

		/* Checks & Frees first, just in case */
		if (global.truth.input != NULL) free (global.truth.input);
		global.truth.input = (uint64_t *) calloc (global.truth.step, sizeof (uint64_t));
		if (global.truth.output != NULL) free (global.truth.output);
		global.truth.output = (uint64_t *) calloc (global.truth.step, sizeof (uint64_t));

		/* Initializing with random number
			RAND_MAX is defined as a 32-bit number.
			Thus, two bit-shifted 32-bit numbers, combine into one, 64-bit random number.

			Its Shit. I know.
		*/
		for (unsigned int i=0; i<global.truth.step; i++) {
			global.truth.input [i] |= rand ();
			global.truth.input [i] <<= 32;
			global.truth.input [i] |= rand ();
			// Right Shifts for grids narrower than 64 bit
			global.truth.input [i] >>= (64 - global.CA.DIMX);


			global.truth.output [i] |= rand ();
			global.truth.output [i] <<= 32;
			global.truth.output [i] |= rand ();
			// Right Shifts for grids narrower than 64 bit
			global.truth.output [i] >>= (64 - global.CA.DIMX);
		}

		global.tt_init = 1;
		printf ( ANSI_GREEN "DONE" ANSI_RESET "\n");
	}

	/* If TIME is enabled, initialize time_t */
	time_est = (gen_lim * pop_lim / 2 / INDV_PER_SEC);
	if (global.DATA.TIME == 1) {
		time (&timer_begin);
	}
}

bool run_sim (void) {

	/* ===== ELABORATION ===== */
	sim_init ();

	printf ("\tSimulation Progress:\n");

	/* ===== MAIN SIMULATION LOOP ===== */

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

			/* Edit FPGA RAM & Evaluate Circuit
				Only if FPGA array initialized
				Assumes correct setting

				WARNING: Getting output right after setting the grid,
				might return inaccurate results.

				If FPGA is unavailable, sets fitness as a random number.
			*/
			if (global.fpga_init == 1) {
				fpga_set_grid (grid);
				indv[idx].fit = evaluate ();
				fpga_clear ();
			} else {
				indv[idx].fit = rand () % 100;
			}

			indv[idx].eval = 1;
		}

		GeneticAlgorithm::Sort (indv);
		statistics (indv, gen);

		/* Progress Status Report */
		if ((global.stats.tts == 0) && (indv[0].fit >= fit_lim )) {
			global.stats.tts = gen;
		}
		if (gen % 10 == 0) {
			// Current Progress
			printf ("\t%4u / %4u ", gen, global.GA.GEN);

			// Estimate Time Arrival
			if (global.DATA.TIME == 1) {
				time (&timer_end);
				float eta = time_est - difftime (timer_end, timer_begin);
				if (eta > 0) {
					printf (" | ETA:%8.1f s", eta);
				} else {
					// The estimate could be wrong...
					printf (" | ETA: < 0 s...");
				}
			}

			// Flags / Warnings / Notes
			if (global.stats.tts != 0) {
				cout << ANSI_GREEN " << Solution Found!" ANSI_RESET;
			}

			cout << "\n";
		}
	}

	/* ===== END SIMULATION LOOP ===== */

	/* ===== REPORT FINAL RESULTS ===== */
	report ();

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
	sim_cleanup ();

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

// TODO: Evaluation function -- Apply F1 Scoring?
uint32_t evaluate (void) {
	uint64_t observed;
	uint32_t fitness = 0;

	/* WARNING: Possible Data Race -- This might now work with sequential logic */
	for (unsigned int i=0; i<global.truth.step; i++) {
		fpga_set_input (global.truth.input [i]);
		/* Gets output twice */
		observed = fpga_get_output ();
		/* Counts equivalent bits */
		fitness += bitcount64 ( ~( global.truth.output [i] ^ observed ) );
		/* Adjusts for grids narrower than 64-bits */
		fitness -= (64 - global.CA.DIMX);
	}

	return fitness;
}

void report (void) {
	/* Prints Pretty Results */
	cout << ANSI_GREEN "\tDONE : ";
	cout << difftime (timer_end, timer_begin) << " s\n\n" ANSI_RESET;

	/* Solution not found */
	if (global.stats.tts == 0) {
		cout << ANSI_RED "No solution has been found\n" ANSI_RESET;
	/* Solution found */
	} else {
		cout << ANSI_GREEN "First solution found in: " << global.stats.tts << " gen\n" ANSI_RESET;
	}

	printf ("Final Fitness Statistics:\n"
			"Average Fitness:\t%.1f / %d\n"
			"Median Fitness: \t%.1f / %d\n"
			"Maximum Fitness:\t%u / %d\n"
			"Minimum Fitness:\t%u / %d\n\n",
			global.stats.avg [gen_lim-1], fit_lim,
			global.stats.med [gen_lim-1], fit_lim,
			global.stats.max [gen_lim-1], fit_lim,
			global.stats.min [gen_lim-1], fit_lim
	);
	printf ("Best Solution: UID: %u | FIT: %u\n"
			"DNA: ", indv[0].uid, indv[0].fit
	);
	indv[0].print_dna();
	cout << "\n\n";

	/* Generate & Set Grid */
	cellgen (global.CA.SEED, grid[0], indv[0].dna);
	for (unsigned int y=1; y<dimy; y++) {
		cellgen (grid[y-1], grid[y], indv[0].dna);
	}
	cellgen (grid[dimy-1], grid[0], indv[0].dna);
	for (unsigned int y=1; y<dimy; y++) {
		cellgen (grid[y-1], grid[y], indv[0].dna);
	}

	/* Sets FPGA & Checks Truth Table if FPGA is set */
	if (global.fpga_init == 1) {
		fpga_set_grid (grid);

		// TODO: case for sequential logic
		printf ("\t\t\t    \e[100mChecked Logic Table\e[0m\n"
		"\t             Input |      Expected      | Observed\n"
		"\t-------------------+--------------------+-------------------\n");
		for (unsigned int i=0; i<global.truth.step; i++) {
			uint64_t output;

			fpga_set_input (global.truth.input [i]);
			printf ("\t0x%016llX | 0x%016llX | ", global.truth.input [i], global.truth.output [i]);
			output = fpga_get_output ();
			output = fpga_get_output ();
			if (output == global.truth.output [i]) {
				cout << ANSI_GREEN;
				printf ("0x%016llX", output);
				cout << ANSI_RESET;
			} else {
				cout << ANSI_RED;
				printf ("0x%016llX", output);
				cout << ANSI_RESET;
			}
			cout << endl;
		}

	/* Print error message if FPGA is not set */
	} else {
		printf (ANSI_YELLOW "\tFPGA Not Initialized. Logic Table Unavailable." ANSI_RESET "\n");
	}

	/* Optional Print of Fittest Solution */
	if (global.DATA.CAPRINT == 1) {
		printf ("\n\e[100m\t-- Generated Logic Circuit --" ANSI_RESET "\n");
		print_grid (grid);
		cout << endl;
	}
}

void sim_cleanup (void) {
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

	/* Clear FPGA Grid one last time */
	fpga_clear ();
}
