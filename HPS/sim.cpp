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

/* ========== Standard Library Include ========== */

#include <stdio.h>		/* Standard I/O */
#include <stdlib.h>		/* calloc, free, rand, srand */
#include <stdint.h>		/* uint definitions */
#include <time.h>		/* time */
#include <math.h>		/* pow, round, ceiling */
#include <iostream>		/* cin, cout */
#include <algorithm>	/* sort, find */
#include <vector>		/* vectors */
#include <array>		/* arrays */
#include <cstring>		/* strcmp */

/* ========== Custom Header Include ========== */

#include "sim.hpp"
#include "ansi.hpp"
#include "ca.hpp"
#include "eval.hpp"
#include "fpga.hpp"
#include "ga.hpp"
#include "global.hpp"
#include "truth.hpp"



/* ========== Simulation Variables ========== */

/* Genetic Algorithm Population Array */
static GeneticAlgorithm *indv;

/* General Settings
	Keeping a copy for performance,
	accessing these are almost 2x faster than calling functions from GlobalSettings.
*/
static unsigned int  pop_lim;
static unsigned int  gen_lim;

static unsigned int  dimx;
static unsigned int  dimy;
static unsigned int  color;
static unsigned int  nb;

static unsigned int  dna_length;

/* Maxmimum fitness score possible
	Calculated as: (Output bit width) * (Truth Table Steps)
	Or F1_MAX for F1 Scores.

	Used to determine if a solution has been found.
*/
static uint32_t fit_lim;

/* Timer Variable */
static time_t timer_begin;
static time_t timer_end;

/* Time Estimate */
static float time_est;



/* Simulation Statistics Struct
	This struct keeps some settings and results of the most recent simulation.
	Keeps a copy, so even if the settings are changed after one simulation,
	the data could be used to replicate and export results.
*/
struct stats_var {
	/* Array of stats across each generations */
	float *avg;
	float *med;
	unsigned int *max;
	unsigned int *min;
	unsigned int *sol_count;

	/* Time to (first) solution */
	unsigned int tts;
};

static stats_var stats;



/* ========== Flags ========== */

static bool solution_found = 0;
static bool data_exported = 0;
static bool sim_done = 0;
static bool sim_init_flag = 0;
static bool data_track = 1;
static bool show_status = 1;


/* ========== Namespaces ========== */

using namespace std;
using namespace GlobalSettings;
namespace tt = TruthTable;



/* ========== STATIC PROTOTYPE FUNCTIONS ========== */

static unsigned int count_solution (GeneticAlgorithm *const array);

static void statistics (GeneticAlgorithm *const array, const unsigned int &gen);

static void report (uint8_t *const *const grid, const uint8_t *const seed);



/* ========== Simulation Functions ========== */

void sim_init (void) {

	/* Checks Conditions */
	if ( tt::table_is_init () == 0) {
		printf (ANSI_RED "Truth table not defined.\n" ANSI_RESET);
		return;
	}

	if ( fpga_is_init () == 0 ) {
		printf (ANSI_RED "FPGA not initialized.\n" ANSI_RESET);
		return;
	}

	if ( ca_is_init () == 0 ) {
		printf (ANSI_RED "CA not initialized.\n" ANSI_RESET);
		return;
	}

	/* Creates a local copy for sim.cpp file */
	pop_lim = get_ga_pop ();
	gen_lim = get_ga_gen ();
	dimx = get_ca_dimx ();
	dimy = get_ca_dimy ();
	color = get_ca_color ();
	nb = get_ca_nb ();
	dna_length = get_dna_length ();

	/* Prints Parameter List */
	printf (ANSI_REVRS "\n\t>>>-- Initializing Simulation --<<<\n" ANSI_RESET
		"\tPOP = %4u | GEN = %4u | MUT = %0.3f | POOL = %4u\n"
		"\tDIMX = %3u | DIMY = %3u | COLOR = %3u | NEIGHBOR = %2u\n"
		"\tCAPRINT = %1u | EXPORT = %1u\n"
		"\tROW = %1u | F1 SCORING = %1u\n\n",
		pop_lim, gen_lim, get_ga_mutp(), get_ga_pool(),
		dimx, dimy, color, nb,
		get_data_caprint(), get_data_export(),
		tt::get_row(), tt::get_f1()
	);

	/* Free array of previously created population
		WARNING: BUG: Run the simulation once, then changing the population size.
		This causes either incomplete memory deallocation; orphans.
		Or causes out-of-bound free attempts; Segmentation Faults.
	*/
	if (indv != NULL) {
		for (unsigned int idx = 0 ; idx < pop_lim ; idx++) {
			indv[idx].FreeDNA ();
		}
		free (indv);
	}

	/* Allocates an array of individuals (population) */
	indv = (GeneticAlgorithm *) calloc (pop_lim, sizeof (GeneticAlgorithm));
	for (unsigned int i = 0 ; i < pop_lim ; i++) {
		indv [i] = GeneticAlgorithm (dna_length);
	}

	/* Free data array -- if not empty */
	if (stats.avg != NULL) free (stats.avg);
	if (stats.med != NULL) free (stats.med);
	if (stats.max != NULL) free (stats.max);
	if (stats.min != NULL) free (stats.min);
	if (stats.sol_count != NULL) free (stats.sol_count);

	/* Allocate memory for data array */
	stats.avg = (float *) calloc (gen_lim, sizeof (float));
	stats.med = (float *) calloc (gen_lim, sizeof (float));
	stats.max = (unsigned int *) calloc (gen_lim, sizeof (unsigned int));
	stats.min = (unsigned int *) calloc (gen_lim, sizeof (unsigned int));
	stats.sol_count = (unsigned int *) calloc (gen_lim, sizeof (unsigned int));

	/* Maxmimum fitness possible
		Calculated as: (Output bit width) * (Truth Table Steps)
		Or as defined by F1_MAX
		F1 Scoring in computationally expensive, takes more time
	*/
	if (tt::get_f1() == 1) {
		fit_lim = F1_MAX;
	} else {
		fit_lim = dimx * tt::get_row();
	}

	/* Calculate time estimate */
	time_est = ((float) gen_lim * pop_lim / INDV_PER_SEC);

	fpga_clear ();

	/* Set Flags */
	data_exported = 0;
	solution_found = 0;
	sim_done = 0;
	sim_init_flag = 1;

	time (&timer_begin);
	return;
}

void sim_cleanup (void) {
	printf ("Cleaning up sim... ");

	/* Clear Flags */
	data_exported = 0;
	solution_found = 0;
	sim_done = 0;
	sim_init_flag = 0;

	/* Free GA Class Objects */
	for (unsigned int idx = 0 ; idx < pop_lim ; idx++) {
		indv[idx].FreeDNA ();
	}
	free (indv);

	/* Free data array */
	if (stats.avg != NULL) free (stats.avg);
	if (stats.med != NULL) free (stats.med);
	if (stats.max != NULL) free (stats.max);
	if (stats.min != NULL) free (stats.min);
	if (stats.sol_count != NULL) free (stats.sol_count);

	printf (ANSI_GREEN "DONE\n" ANSI_RESET);
}

int sim_run (uint8_t *const *const grid, const uint8_t *const seed) {
	if ( sim_init_flag == 0 ) {
		printf (ANSI_RED "\nSimulation Requires Initialization\n" ANSI_RESET);
		return -1;
	}

	printf ("\tSimulation Progress:\n");



	/* ===== MAIN SIMULATION LOOP ===== */

	/* Loop over each generation */
	for (unsigned int gen = 0 ; gen < gen_lim ; gen++) {
		/* Perform selection, reproduction, crossover, and mutation */
		GeneticAlgorithm::Selection (indv);
		GeneticAlgorithm::Repopulate (indv);

		/* Loop over each individual */
		for (unsigned int idx = 0 ; idx < pop_lim ; idx++) {
			/* Automatically ages an individual */
			indv[idx].set_age();

			/* Skips evaluation, if already done for this individual */
			if ( indv[idx].get_eval() == 1 ) continue;

			/* Generate CA Array
				1. Generate first row, with SEED
				2. Iterate over entire grid once; generate current row with previous row.
				3. Generate first row again, with last row of grid
				4. Iterate over entire grid again, same as step 2.
			*/
			ca_gen_grid (grid, indv[idx].get_dna(), seed);
			ca_gen_grid (grid, indv[idx].get_dna());

			/* Edit FPGA RAM */
			fpga_set_grid (grid);

			/* Evaluate Circuit */
			if (tt::get_f1() == 1) {
				indv[idx].set_fit (
					eval_f1_array (tt::get_input(), tt::get_output(), tt::get_row())
				);
			} else {
				indv[idx].set_fit (
					eval_bc_array (tt::get_input(), tt::get_output(), tt::get_row())
				);
			}

			indv[idx].set_eval();
			fpga_clear ();
		}

		/* Sort population by fitness -- descending order, highest fitness first */
		GeneticAlgorithm::Sort (indv);

		/* Track Statistics && Checks for solution */
		statistics (indv, gen);



		/* ===== Report ===== */

		if (gen % 10 == 0) {
			/* Current Progress */
			printf ("\t%4u / %4u ", gen, gen_lim);

			/* Estimate Time Arrival */
			time (&timer_end);
			float eta = time_est - difftime (timer_end, timer_begin);
			if (eta > 0) printf (" | ETA: %6.0f s", eta);
			else printf (" | ETA: < 0 s ...");

			/* Flags / Warnings / Notes */
			if (solution_found) {
				printf (ANSI_GREEN " << Solution Found! (%u)" ANSI_RESET, stats.sol_count[gen]);
			}

			putchar ('\n');
		}
	}



	/* ===== END SIMULATION LOOP ===== */

	fpga_clear ();

	sim_done = 1;

	/* ===== REPORT FINAL RESULTS ===== */

	report (grid, seed);

	if (solution_found == 1) {
		return 1;
	} else {
		return 0;
	}
}

bool sim_is_done (void) {
	return sim_done;
}

unsigned int count_solution (GeneticAlgorithm *const array) {
	/* Count of how many solutions exist in current population */
	unsigned int count = 0;

	for (unsigned int i = 0 ; i < pop_lim ; i++) {
		if ( array[i].get_fit() >= fit_lim ) count++;
	}

	return count;
}



/* ========== Results & Reporting Function ========== */

void statistics (GeneticAlgorithm *const array, const unsigned int &gen) {
	/* Get average value */
	float average = 0.0;
	for (uint32_t i = 0 ; i < pop_lim ; i++) {
		average += array[i].get_fit();
	}
	average /= pop_lim;
	stats.avg [gen] = average;

	/* Get median value (list is presorted) */
	float median = 0;
	if (pop_lim % 2 == 0) {
		median = array[pop_lim/2].get_fit() + array[(pop_lim/2)-1].get_fit();
		stats.med [gen] = median/2;
	} else {
		stats.med [gen] = array[pop_lim/2].get_fit();
	}

	/* Get max value (list is presorted) */
	stats.max [gen] = array[0].get_fit();

	/* Get min value (list is presorted) */
	stats.min [gen] = array[pop_lim-1].get_fit();

	/* Check for solutions found */
	stats.sol_count [gen] = count_solution (indv);

	/* Checks for first solution */
	if ((solution_found == 0) && (stats.sol_count[gen] != 0)) {
		stats.tts = gen;
		solution_found = 1;
	}
}

void report (uint8_t *const *const grid, const uint8_t *const seed) {
	/* How many top individuals to display at the end */
	constexpr unsigned int N = 10;

	printf (ANSI_GREEN "\tDONE : %.1f s\n" ANSI_RESET, difftime (timer_end, timer_begin));

	if (solution_found) {
		printf (ANSI_GREEN "\tFirst solution found in: %u gen\n\n" ANSI_RESET, stats.tts);
	} else {
		printf (ANSI_RED "\tNo solution has been found\n\n" ANSI_RESET);
	}


	printf ("\tFinal Fitness Statistics:\n"
			"\tAverage Fitness: %7.1f / %5d\n"
			"\tMedian Fitness:  %7.1f / %5d\n"
			"\tMaximum Fitness: %5u / %5d\n"
			"\tMinimum Fitness: %5u / %5d\n\n",
			stats.avg [gen_lim-1], fit_lim,
			stats.med [gen_lim-1], fit_lim,
			stats.max [gen_lim-1], fit_lim,
			stats.min [gen_lim-1], fit_lim
	);


	/* Displays top 'N' individuals */
	printf ("\nTop %u Individuals:\n", N);
	for (unsigned int i = 0 ; i < N ; i++) {
		printf ("[%1u] UID: %u | FIT: %u | DNA: ", i, indv[i].get_uid(), indv[i].get_fit() );
		indv[i].print_dna( dna_length );
		printf ("\n");
	}


	/* ===== Evaluate & Graph Number 1 Individual ===== */

	printf ("\n\n\tFittest Individual:\n"
		"UID: %u | FIT: %u | DNA: ", indv[0].get_uid(), indv[0].get_fit() );
	indv[0].print_dna( dna_length );
	printf ("\n");

	/* Generate & Set Grid */
	ca_gen_grid (grid, indv[0].get_dna(), seed);
	ca_gen_grid (grid, indv[0].get_dna());
	fpga_set_grid (grid);

	/* Evaluate Circuit */
	if ( tt::get_f1() == 1 ) {
		eval_f1_insp (tt::get_input(), tt::get_output(), tt::get_row());
	} else {
		eval_bc_insp (tt::get_input(), tt::get_output(), tt::get_row());
	}

	/* Optional Print of Fittest Solution */
	if ( get_data_caprint() == 1 ) {
		printf ("\n\e[100m\t-- Generated Logic Circuit --" ANSI_RESET "\n");
		ca_print_grid (grid);
		cout << endl;
	}

	return;
}

void sim_results (void) {
	if (sim_done == 0) {
		printf ("No simulation results.\n");
		return;
	}

	printf (ANSI_REVRS "\n\t>>--- Simulation Results ---<<\n" ANSI_RESET
		"\n\t\tFitness Table\n"
		"  Gen | Maximum | Minimum | Median | Average | Solutions\n"
		"--------------------------------------------------------\n");
	for (unsigned int i = 0 ; i < gen_lim; i++) {
		printf(" %4u | %7u | %7u | %6.1f | %7.1f | %5u\n",
			i + 1, stats.max [i], stats.min [i],
			stats.med [i], stats.avg [i], stats.sol_count [i]
		);
	}

	cout << endl;
	return;
}

void sim_export (void) {
	FILE *fp;
	char filename [64];
	char buffer [64];

	/* Get Current Local Time & Convert to Time Struct */
	time_t raw_time;
	struct tm *timeinfo;
	time (&raw_time);
	timeinfo = localtime (&raw_time);

	/* Sets filename to ./rpt/YYYYMMDD-HHMMSS format */
	strcpy (filename, "./rpt/");
	strftime (buffer, 64, "%Y%m%d-%H%M%S.rpt", timeinfo);
	strcat (filename, buffer);

	printf ("Exporting results as: \"%s\" ...", filename);

	fp = fopen (filename, "w");
	if (fp == NULL) {
		printf (ANSI_RED "FAILED -- Unable to open file: %s\n" ANSI_RESET, filename);
		return;
	}



	/* ===== Write to File ===== */

	fprintf (fp, "Simulation Report: %s\n\n", filename);

	fprintf (fp, "Settings:\n"
		"\tGeneration Limit: %u\n"
		"\tPopulation Limit: %u\n"
		"\tX-Axis Dimension: %u\n"
		"\tY-Axis Dimension: %u\n"
		"\tCA Color: %u\n"
		"\tCA Neighbor: %u\n"
		"\tTime to first Solution: ",
		gen_lim, pop_lim,
		dimx, dimy,
		color, nb );

	if (solution_found) fprintf (fp, "%u gens\n", stats.tts);
	else fprintf (fp, "Not Found\n");

	fprintf (fp, "\nFitness Table\n"
		"  Gen | Maximum | Minimum | Median | Average | Solutions\n"
		"--------------------------------------------------------\n");

	for (unsigned int i = 0 ; i < gen_lim; i++) {
		fprintf(fp, " %4u | %7u | %7u | %6.1f | %7.1f | %5u\n",
			i + 1,
			stats.max [i], stats.min [i],
			stats.med [i], stats.avg [i],
			stats.sol_count [i]
		);
	}



	/* ===== Final Population Results ===== */
	constexpr int N = 10;

	fprintf (fp, "\n\n Top %u Individuals\n"
			"UID | FITNESS | DNA \n", N
	);

	for (unsigned int i = 0 ; i < N ; i++) {
		fprintf (fp, " %u | %u | ", indv[i].get_uid(), indv[i].get_fit());
		indv[i].fprint_dna (fp, dna_length);
		fputc ('\n', fp);
	}

	fclose (fp);

	data_exported = 1;

	printf (ANSI_GREEN " DONE\n" ANSI_RESET);

	return;
}

bool export_is_done (void) {
	return data_exported;
}
