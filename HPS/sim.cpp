/* Simulation Wrapper File */

void sim_init (void) {

	/* If no Truth Table set -- Fails */
	if (global.tt_init == 0) {
		printf (ANSI_RED "No truth table defined.\n" ANSI_RESET);
		return;
	}

	/* Prints Parameter List */
	printf (ANSI_REVRS "\n\t>>>-- Initializing Simulation --<<<\n" ANSI_RESET
		"\tPOP = %4u | GEN = %4u | MUT = %0.3f | POOL = %4u\n"
		"\tDIMX = %3u | DIMY = %3u | COLOR = %3u | NEIGHBOR = %2u\n"
		"\tFIT = %1u | TIME = %1u | CAPRINT = %1u | EXPORT = %1u | FPGA INIT = %1u\n"
		"\tSEQUENTIAL = %1u | STEP = %1u | F1 SCORING = %1u\n\n",
		global.GA.POP, global.GA.GEN, global.GA.MUTP, global.GA.POOL,
		global.CA.DIMX, global.CA.DIMY, global.CA.COLOR, global.CA.NB,
		global.DATA.TRACK, global.DATA.TIME, global.DATA.CAPRINT, global.DATA.EXPORT, global.fpga_init,
		global.truth.time, global.truth.step, global.truth.f1
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

	/* Allocates an array of individuals (population), of size "global.GA.POP" */
	indv = (GeneticAlgorithm *) calloc (pop_lim, sizeof (GeneticAlgorithm));
	/* Initialize Population */
	for (unsigned int idx=0; idx<pop_lim; idx++) {
		indv[idx] = GeneticAlgorithm (dna_length);
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

	/* Maxmimum fitness possible
		Calculated as: (Output bit width) * (Truth Table Steps)
		Or as defined by F1_MAX
		F1 Scoring in computationally expensive, takes more time
	*/
	if (global.truth.f1 == 1) {
		fit_lim = F1_MAX;
		time_est = ((float) gen_lim * pop_lim / (INDV_PER_SEC - 20));
	} else {
		fit_lim = dimx * global.truth.step;
		time_est = ((float) gen_lim * pop_lim / INDV_PER_SEC);
	}

	/* Resets Data Export flag */
	global.export_check = 0;

	/* If TIME is enabled, initialize time_t */
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
			ca_gen_row (global.CA.SEED, grid[0], indv[idx].dna);
			for (unsigned int y=1; y<dimy; y++) {
				ca_gen_row (grid[y-1], grid[y], indv[idx].dna);
			}
			ca_gen_row (grid[dimy-1], grid[0], indv[idx].dna);
			for (unsigned int y=1; y<dimy; y++) {
				ca_gen_row (grid[y-1], grid[y], indv[idx].dna);
			}

			/* Edit FPGA RAM & Evaluate Circuit
				Only if FPGA array initialized
				Assumes correct setting

				WARNING: Getting output right after setting the grid,
				might return inaccurate results.

				If FPGA is unavailable, sets fitness as a random number.
			*/
			if (global.fpga_init == 1) {
				fpga_clear ();
				fpga_set_grid (grid);

				if (global.truth.f1 == 0) {
					indv[idx].fit = evaluate ();
				} else {
					indv[idx].fit = evaluate_f1 ();
				}

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

			for (int i=0; i<MAX_CA_DIMX; i++) {
				saved_dna [i] = indv[0].dna[i];
			}
		}

		if (gen % 10 == 0) {
			// Current Progress
			printf ("\t%4u / %4u ", gen, global.GA.GEN);

			// Estimate Time Arrival
			if (global.DATA.TIME == 1) {
				time (&timer_end);
				float eta = time_est - difftime (timer_end, timer_begin);
				if (eta > 0) {
					printf (" | ETA: %6.0f s", eta);
				} else {
					// The estimate could be wrong...
					printf (" | ETA: < 0 s ...");
				}
			}

			// Flags / Warnings / Notes
			if (global.stats.tts != 0) {
				printf (ANSI_GREEN " << Solution Found! (%u)" ANSI_RESET, global.stats.tts);
			}

			cout << "\n";
		}
	}

	/* ===== END SIMULATION LOOP ===== */

	/* ===== REPORT FINAL RESULTS ===== */

	report ();

	if (global.DATA.EXPORT == 1)
		global.export_check = auto_export (indv);

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

uint32_t evaluate (void) {
	uint32_t fitness = 0;
	uint64_t observed;

	for (unsigned int i=0; i<global.truth.step; i++) {
		fpga_set_input (global.truth.input [i]);
		observed = fpga_get_output ();

		/* Counts equivalent bits */
		fitness += bitcount64 ( ~( global.truth.output [i] ^ observed ) );

		/* Adjusts for grids narrower than 64-bits */
		fitness -= (64 - global.CA.DIMX);
	}

	return fitness;
}

uint32_t evaluate_f1 (void) {
	uint64_t observed;
	/* True Positive, False Positive, False Negative */
	float tpos = 0;
	float fpos = 0;
	float fneg = 0;

	for (unsigned int i=0; i<global.truth.step; i++) {
		fpga_set_input (global.truth.input [i]);
		observed = fpga_get_output ();

		/* Calculates F1 Score */
		tpos += bitcount64
			( ~( global.truth.output [i] ^ observed ) & global.truth.output [i] );
		fpos += bitcount64
			( ( global.truth.output [i] ^ observed ) & observed );
		fneg += bitcount64
			( ( global.truth.output [i] ^ observed ) & ~(observed) );
	}

	/* Recall, Precision, F1 Score */
	float precision = tpos / (tpos + fpos);
	float recall = tpos / (tpos + fneg);
	float f1_score = 2 * precision * recall / (precision + recall);

	/* Returns f1 score */
	return (uint32_t) floor (F1_MAX * f1_score);
}

void id_evaluate (void) {
	uint32_t fitness = 0;
	const uint32_t fit_lim = global.CA.DIMX * global.truth.step;
	uint64_t observed;

	fpga_set_grid (grid);

	printf ("\n\t\t\t    \e[100mChecked Logic Table\e[0m\n"
	"\t             Input |      Expected      | Observed\n"
	"\t-------------------+--------------------+-------------------\n");

	for (unsigned int i=0; i<global.truth.step; i++) {
		fpga_set_input (global.truth.input [i]);
		observed = fpga_get_output ();

		/* Counts equivalent bits */
		fitness += bitcount64 ( ~( global.truth.output [i] ^ observed ) );

		/* Adjusts for grids narrower than 64-bits */
		fitness -= (64 - global.CA.DIMX);

		/* Prints In-Depth Results */
		printf ("\t0x%016llX | 0x%016llX | ", global.truth.input [i], global.truth.output [i]);
		if (observed == global.truth.output [i]) {
			printf (ANSI_GREEN "0x%016llX\n" ANSI_RESET, observed);
		} else {
			printf (ANSI_RED "0x%016llX\n" ANSI_RESET, observed);
		}
	}

	printf ("\n\tFitness: %u / %u\n", fitness, fit_lim);

	return;
}

void id_evaluate_f1 (void) {
	uint32_t fitness;
	uint64_t observed;
	/* True Positive, False Positive, False Negative */
	float tpos = 0;
	float fpos = 0;
	float fneg = 0;

	fpga_set_grid (grid);

	printf ("\n\t\t\t    \e[100mChecked Logic Table\e[0m\n"
	"\t             Input |      Expected      | Observed\n"
	"\t-------------------+--------------------+-------------------\n");

	for (unsigned int i=0; i<global.truth.step; i++) {
		fpga_set_input (global.truth.input [i]);
		observed = fpga_get_output ();

		/* Calculates F1 Score */
		tpos += bitcount64
			( ~( global.truth.output [i] ^ observed ) & global.truth.output [i] );
		fpos += bitcount64
			( ( global.truth.output [i] ^ observed ) & observed );
		fneg += bitcount64
			( ( global.truth.output [i] ^ observed ) & ~(observed) );

		/* Prints In-Depth Results */
		printf ("\t0x%016llX | 0x%016llX | ", global.truth.input [i], global.truth.output [i]);
		if (observed == global.truth.output [i]) {
			printf (ANSI_GREEN "0x%016llX\n" ANSI_RESET, observed);
		} else {
			printf (ANSI_RED "0x%016llX\n" ANSI_RESET, observed);
		}
	}

	/* Recall, Precision, F1 Score */
	float precision = tpos / (tpos + fpos);
	float recall = tpos / (tpos + fneg);
	float f1_score = 2 * precision * recall / (precision + recall);

	/* Scales fitness with F1 score */
	fitness = (uint32_t) floor (F1_MAX * f1_score);
	printf ("\n\tFitness: %u / %u\n", fitness, F1_MAX);

	return;
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
		printf ("First Solution:\n");

		for (int i=0; i<MAX_CA_DIMX; i++) {
			printf ("%X", saved_dna [i]);
		}

		cout << "\n";
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
	ca_gen_row (global.CA.SEED, grid[0], indv[0].dna);
	for (unsigned int y=1; y<dimy; y++) {
		ca_gen_row (grid[y-1], grid[y], indv[0].dna);
	}
	ca_gen_row (grid[dimy-1], grid[0], indv[0].dna);
	for (unsigned int y=1; y<dimy; y++) {
		ca_gen_row (grid[y-1], grid[y], indv[0].dna);
	}

	/* Sets FPGA & Checks Truth Table if FPGA is set */
	if ((global.fpga_init == 1) && (global.tt_init == 1)) {

		if (global.truth.f1 == 1) {
			id_evaluate_f1 ();
		} else {
			id_evaluate ();
		}

	/* Print error message if FPGA or Truth Table is not set */
	} else {
		if (global.fpga_init == 0) {
			printf (ANSI_YELLOW "\tFPGA Not Initialized\n" ANSI_RESET);
		}
		if (global.tt_init == 0) {
			printf (ANSI_YELLOW "\tTruth Table Not Initialized\n" ANSI_RESET);
		}
		printf (ANSI_YELLOW "\tLogic Table Unavailable\n" ANSI_RESET);
	}

	/* Optional Print of Fittest Solution */
	if (global.DATA.CAPRINT == 1) {
		printf ("\n\e[100m\t-- Generated Logic Circuit --" ANSI_RESET "\n");
		ca_print_grid (grid);
		cout << endl;
	}
}

void sim_cleanup (void) {
	/* Free Seed Array */
	free (global.CA.SEED);

	/* Free GA Class Objects */
	for (unsigned int idx=0; idx<pop_lim; idx++) {
		free (indv[idx].dna);
	}
	free (indv);

	/* Clear FPGA Grid one last time */
	fpga_clear ();
}

bool auto_export (GeneticAlgorithm *array) {
	FILE *fp;
	char filename [64];

	/* Get Current Local Time & Convert to Time Struct */
	time_t raw_time;
	struct tm *timeinfo;
	time (&raw_time);
	timeinfo = localtime (&raw_time);
	/* Sets filename to YYYYMMDD-HHMMSS format */
	strftime (filename, 64, "%Y%m%d-%H%M%S.rpt", timeinfo);

	printf ("Exporting results as: \"%s\" ...", filename);

	fp = fopen (filename, "w");
	if (fp == NULL) {
		printf (ANSI_RED "FAILED -- Unable to open file: %s\n" ANSI_RESET, filename);
		return 0;
	}

	/* ===== Write to File ===== */

	fprintf (fp, "Simulation Report: %s\n\n", filename);

	fprintf (fp, ">> Settings:\n"
		"\tGeneration Limit: %u\n"
		"\tPopulation Limit: %u\n"
		"\tX-Axis Dimension: %u\n"
		"\tY-Axis Dimension: %u\n"
		"\tCA Color: %u\n"
		"\tCA Neighbor: %u\n"
		"\tTime to first Solution: ",
		global.stats.gen, global.stats.pop,
		global.stats.dimx, global.stats.dimy,
		global.stats.color, global.stats.nb );

	if (global.stats.tts != 0) {
		fprintf (fp, "%u gens\n", global.stats.tts);
	} else {
		fprintf (fp, "Not Applicable\n");
	}

	fprintf (fp, "\nFitness Table\n"
		"  Gen | Maximum | Minimum | Median | Average\n"
		"--------------------------------------------\n");
	for (uint32_t i=0; i<global.stats.gen; i++) {
		fprintf (fp, " %4u | %7u | %7u | %6.1f | %7.1f \n", i + 1,
		global.stats.max [i], global.stats.min [i],
		global.stats.med [i], global.stats.avg [i]);
	}

	/* ===== Final Population Results ===== */

	if ( array != NULL ) {
		fprintf (fp, "\n\n Top 10 Individuals\n"
			"UID | FITNESS | DNA \n");
		for (uint32_t i=0; i<10; i++) {
			fprintf (fp, " %u | %u | ", array[i].uid, array[i].fit);
			array[i].fprint_dna (fp);
			fputc ('\n', fp);
		}
	}

	fclose (fp);
	printf (ANSI_GREEN " DONE\n" ANSI_RESET);
	return 1;
}
