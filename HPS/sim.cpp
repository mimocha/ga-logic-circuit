/* Main C++ File for Simulation Functions

	MIT License

	Copyright (c) 2018 Chawit Leosrisook

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE

*/

#include "sim.hpp"
#include "fpga.hpp"
#include "ca.hpp"
#include "eval.hpp"
#include "ansi.hpp"

/* ===== Simulation File Level Global Variables =====
	Ignores the need to pass variables around.
	Should make program easier to understand.

	> Encapsulate These Variables
	Set as Static, limits scope to this compilation unit only.
*/

/* Renames variables, for easier reference */
static unsigned int dimx;
static unsigned int dimy;
static unsigned int gen_lim;
static unsigned int pop_lim;

/* Computes DNA length, the number of CA rules;
	Calculated by -> Color^Neighbor (exponent, not XOR)
*/
static uint32_t dna_length;

/* Maxmimum fitness score possible
	Calculated as: (Output bit width) * (Truth Table Steps)
	Or F1_MAX for F1 Metric.
*/
static uint32_t fit_lim;

/* Timer Variable */
static time_t timer_begin;
static time_t timer_end;

/* Time Estimate Variable */
static float time_est;

/* Genetic Algorithm Population Array */
static GeneticAlgorithm *indv;

using namespace std;

// --------------------------------------
// Simulation
// --------------------------------------

bool sim_init (void) {

	/* Fail Conditions */
	if (global.tt_init == 0) {
		printf (ANSI_RED "No truth table defined.\n" ANSI_RESET);
		return 0;
	}

	if ( fpga_is_init () == 0 ) {
		printf (ANSI_RED "FPGA not initialized.\n" ANSI_RESET);
		return 0;
	}

	if ( ca_is_init () == 0 ) {
		printf (ANSI_RED "CA not initialized.\n" ANSI_RESET);
		return 0;
	}

	/* Prints Parameter List */
	printf (ANSI_REVRS "\n\t>>>-- Initializing Simulation --<<<\n" ANSI_RESET
		"\tPOP = %4u | GEN = %4u | MUT = %0.3f | POOL = %4u\n"
		"\tDIMX = %3u | DIMY = %3u | COLOR = %3u | NEIGHBOR = %2u\n"
		"\tFIT = %1u | TIME = %1u | CAPRINT = %1u | EXPORT = %1u\n"
		"\tSEQUENTIAL = %1u | STEP = %1u | F1 SCORING = %1u\n\n",
		global.GA.POP, global.GA.GEN, global.GA.MUTP, global.GA.POOL,
		global.CA.DIMX, global.CA.DIMY, global.CA.COLOR, global.CA.NB,
		global.DATA.TRACK, global.DATA.TIME, global.DATA.CAPRINT, global.DATA.EXPORT,
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

			/* Generate CA Array
				1. Generate first row, with SEED
				2. Iterate over entire grid once; generate current row with previous row.
				3. Generate first row again, with last row of grid
				4. Iterate over entire grid again, same as step 2.
			*/
			ca_gen_grid (grid, indv[idx].dna, global.CA.SEED);
			ca_gen_grid (grid, indv[idx].dna);

			/* Edit FPGA RAM & Evaluate Circuit */
			fpga_clear ();
			fpga_set_grid (grid);

			if (global.truth.f1 == 0) {
				indv[idx].fit = evaluate ();
			} else {
				indv[idx].fit = evaluate_f1 ();
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

	fpga_clear ();
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



// --------------------------------------
// Report
// --------------------------------------

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

void report (void) {
	/* Prints Pretty Results */
	printf (ANSI_GREEN "\tDONE : %.1f s\n" ANSI_RESET, difftime (timer_end, timer_begin));

	if (global.stats.tts == 0)
		printf (ANSI_RED "No solution has been found\n" ANSI_RESET);
	else
		printf (ANSI_GREEN "First solution found in: %u gen\n" ANSI_RESET, global.stats.tts);

	printf ("Final Fitness Statistics:\n"
			"Average Fitness:\t%.1f / %d\n"
			"Median Fitness: \t%.1f / %d\n"
			"Maximum Fitness:\t%u / %d\n"
			"Minimum Fitness:\t%u / %d\n\n",
			global.stats.avg [gen_lim-1], fit_lim,
			global.stats.med [gen_lim-1], fit_lim,
			global.stats.max [gen_lim-1], fit_lim,
			global.stats.min [gen_lim-1], fit_lim );

	printf ("Best Solution: UID: %u | FIT: %u\n" "DNA: ", indv[0].uid, indv[0].fit );
	indv[0].print_dna();
	cout << "\n\n";

	/* Generate & Set Grid */
	ca_gen_grid (grid, indv[0].dna, global.CA.SEED);
	ca_gen_grid (grid, indv[0].dna);

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







// bool auto_export (GeneticAlgorithm *array) {
// 	FILE *fp;
// 	char filename [64];
//
// 	/* Get Current Local Time & Convert to Time Struct */
// 	time_t raw_time;
// 	struct tm *timeinfo;
// 	time (&raw_time);
// 	timeinfo = localtime (&raw_time);
// 	/* Sets filename to YYYYMMDD-HHMMSS format */
// 	strftime (filename, 64, "%Y%m%d-%H%M%S.rpt", timeinfo);
//
// 	printf ("Exporting results as: \"%s\" ...", filename);
//
// 	fp = fopen (filename, "w");
// 	if (fp == NULL) {
// 		printf (ANSI_RED "FAILED -- Unable to open file: %s\n" ANSI_RESET, filename);
// 		return 0;
// 	}
//
// 	/* ===== Write to File ===== */
//
// 	fprintf (fp, "Simulation Report: %s\n\n", filename);
//
// 	fprintf (fp, ">> Settings:\n"
// 		"\tGeneration Limit: %u\n"
// 		"\tPopulation Limit: %u\n"
// 		"\tX-Axis Dimension: %u\n"
// 		"\tY-Axis Dimension: %u\n"
// 		"\tCA Color: %u\n"
// 		"\tCA Neighbor: %u\n"
// 		"\tTime to first Solution: ",
// 		global.stats.gen, global.stats.pop,
// 		global.stats.dimx, global.stats.dimy,
// 		global.stats.color, global.stats.nb );
//
// 	if (global.stats.tts != 0) {
// 		fprintf (fp, "%u gens\n", global.stats.tts);
// 	} else {
// 		fprintf (fp, "Not Applicable\n");
// 	}
//
// 	fprintf (fp, "\nFitness Table\n"
// 		"  Gen | Maximum | Minimum | Median | Average\n"
// 		"--------------------------------------------\n");
// 	for (uint32_t i=0; i<global.stats.gen; i++) {
// 		fprintf (fp, " %4u | %7u | %7u | %6.1f | %7.1f \n", i + 1,
// 		global.stats.max [i], global.stats.min [i],
// 		global.stats.med [i], global.stats.avg [i]);
// 	}
//
// 	/* ===== Final Population Results ===== */
//
// 	if ( array != NULL ) {
// 		fprintf (fp, "\n\n Top 10 Individuals\n"
// 			"UID | FITNESS | DNA \n");
// 		for (uint32_t i=0; i<10; i++) {
// 			fprintf (fp, " %u | %u | ", array[i].uid, array[i].fit);
// 			array[i].fprint_dna (fp);
// 			fputc ('\n', fp);
// 		}
// 	}
//
// 	fclose (fp);
// 	printf (ANSI_GREEN " DONE\n" ANSI_RESET);
// 	return 1;
// }
