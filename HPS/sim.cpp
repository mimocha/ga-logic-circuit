/* Main C++ File for Simulation Functions
	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

/* ========== Standard Library Include ========== */

#include <stdio.h>		// Standard I/O
#include <stdlib.h>		// calloc, free
#include <stdint.h>		// uint definitions
#include <time.h>		// time
#include <iostream>		// cin, cout
#include <algorithm>	// sort, find
#include <cstring>		// strcmp



/* ========== Custom Header Include ========== */

#include "sim.hpp"
#include "ansi.hpp"
#include "ca.hpp"
#include "eval.hpp"
#include "fast.hpp"
#include "fpga.hpp"
#include "ga.hpp"
#include "global.hpp"
#include "truth.hpp"



/* ========== Simulation Variables ========== */

// Genetic Algorithm Population Array
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

// Fitness Limit
static unsigned int fit_lim;

// Timer Variable
static time_t time_start;
static time_t time_now;

// Time Estimate
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
static bool optimize = 0;
// static bool show_status = 1;



/* ========== Namespaces ========== */

using namespace std;
using namespace GlobalSettings;
namespace tt = TruthTable;



/* ========== STATIC PROTOTYPE FUNCTIONS ========== */

static unsigned int count_solution (GeneticAlgorithm *const array);

static void statistics (GeneticAlgorithm *const array, const unsigned int &gen);

static void report (uint8_t *const *const grid, const uint8_t *const seed);



/* ========== Miscellany Functions ========== */

unsigned int count_solution (GeneticAlgorithm *const array) {
	// Count of how many solutions exist in current population
	// 'Solutions' refer to valid solutions, individuals which produces the expected logic
	unsigned int count = 0;

	for (unsigned int i = 0 ; i < pop_lim ; i++) {
		if ( array[i].get_sol() == 1 ) count++;
	}

	return count;
}

void status_print (const unsigned int &gen) {
	// What interval to print status updates at
	constexpr unsigned int interval = 10;

	if (gen % interval == 0) {
		printf ("\t%4u / %4u ", gen, gen_lim);

		// Estimate Time Arrival
		time (&time_now);
		float eta = time_est - difftime (time_now, time_start);
		printf (" | ETA: %6.0f s", eta);

		// Flags / Warnings / Notes
		if (stats.sol_count[gen] > 0) {
			printf (ANSI_GREEN " << Solutions Found! (%u)" ANSI_RESET, stats.sol_count[gen]);
		}

		putchar ('\n');
	}
}



/* ========== Simulation Functions ========== */

void sim_init (void) {
	// Checks Conditions
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



	// Creates a local copy for sim.cpp file
	pop_lim = get_ga_pop ();
	gen_lim = get_ga_gen ();
	dimx = get_ca_dimx ();
	dimy = get_ca_dimy ();
	color = get_ca_color ();
	nb = get_ca_nb ();
	dna_length = get_dna_length ();

	printf (ANSI_REVRS "\n\t>>>-- Initializing Simulation --<<<\n" ANSI_RESET
		"\tPOP = %4u | GEN = %4u | MUT = %0.3f | POOL = %4u\n"
		"\tDIMX = %3u | DIMY = %3u | COLOR = %3u | NEIGHBOR = %2u\n"
		"\tCAPRINT = %1u | EXPORT = %1u\n"
		"\tROW = %1u | MODE : ",
		pop_lim, gen_lim, get_ga_mutp(), get_ga_pool(),
		dimx, dimy, color, nb,
		get_data_caprint(), get_data_export(),
		tt::get_row()
	);

	if (tt::get_mode() == 1) {
		printf (ANSI_REVRS "SEQUENTIAL" ANSI_RESET "\n");
	} else {
		printf (ANSI_REVRS "COMBINATIONAL" ANSI_RESET "\n");
	}

	printf ("\tTT MASK = 0x%016llX (%llu bits)\n\n", tt::get_mask(), tt::get_mask_bc());

	/* Allocates an array of individuals (population)
		new / delete unavailable for struct and classes
		/lib/libstdc++.so.6: version `CXXABI_1.3.8' not found
	*/
	indv = (GeneticAlgorithm *) calloc (pop_lim, sizeof (GeneticAlgorithm));
	for (unsigned int i = 0 ; i < pop_lim ; i++) {
		indv [i] = GeneticAlgorithm (dna_length);
	}


	// Allocate memory for data array
	stats.avg =  (float *) calloc (pop_lim, sizeof (float));
	stats.med =  (float *) calloc (pop_lim, sizeof (float));
	stats.max =  (unsigned int *) calloc (pop_lim, sizeof (unsigned int));
	stats.min =  (unsigned int *) calloc (pop_lim, sizeof (unsigned int));
	stats.sol_count =  (unsigned int *) calloc (pop_lim, sizeof (unsigned int));

	// Set fitness limit
	fit_lim = get_score_max();

	// Seed RNG
	seed_rng32();

	// Calculate time estimate
	time_est = ((float) gen_lim * pop_lim / INDV_PER_SEC);

	// Clear FPGA LCA
	fpga_clear ();

	// Set Flags
	data_exported = 0;
	solution_found = 0;
	sim_done = 0;
	sim_init_flag = 1;

	return;
}


void sim_cleanup (void) {
	printf ("Cleaning up sim... ");

	// Clear Flags
	data_exported = 0;
	solution_found = 0;
	sim_done = 0;
	sim_init_flag = 0;

	// Free GA Class Objects
	for (unsigned int i = 0 ; i < pop_lim ; i++) {
		indv[i].FreeDNA ();
	}
	free (indv);

	// Free data array
	free (stats.avg);
	free (stats.med);
	free (stats.max);
	free (stats.min);
	free (stats.sol_count);

	printf (ANSI_GREEN "DONE\n" ANSI_RESET);
}


int sim_run (uint8_t *const *const grid, const uint8_t *const seed) {

	if ( sim_init_flag == 0 ) {
		printf (ANSI_RED "\nSimulation Requires Initialization\n" ANSI_RESET);
		return -1;
	}

	printf ("\tSimulation Progress:\n");
	time (&time_start);



	/* ===== MAIN SIMULATION LOOP ===== */

	// Loop over each generation
	for (unsigned int gen = 0 ; gen < gen_lim ; gen++) {
		// Perform selection, reproduction, crossover, and mutation
		GeneticAlgorithm::Selection (indv);
		GeneticAlgorithm::Repopulate (indv);

		// Loop over each individual
		for (unsigned int i = 0 ; i < pop_lim ; i++) {

			// Automatically ages an individual
			indv[i].set_age();

			// Skips evaluation, if already done for this individual
			// if ( indv[i].get_eval() == 1 ) continue;

			// Generate CA Array
			ca_gen_grid (grid, indv[i].get_dna(), seed);
			ca_gen_grid (grid, indv[i].get_dna());

			// Edit FPGA RAM
			fpga_set_grid (grid);

			// Evaluate Circuit Truth Table
			uint32_t score = 0;

			// Sequential or Combinational Logic
			if (tt::get_mode() == 0) {
				// Resets circuit inbetween each test case
				score += eval_com (0);
				fpga_clear ();
				fpga_set_grid (grid);
				score += eval_com (1);
				fpga_clear ();
				fpga_set_grid (grid);
				score += eval_com (2);
				// Get the average score
				score /= 3;
			} else {
				score = eval_seq ();
			}

			// Flags this as a viable solution
			if (score == fit_lim) {
				indv[i].set_sol (1);

				// Print dna of first solution found
				if (solution_found == 0) {
					solution_found = 1;
					stats.tts = gen;

					printf ("\tFirst Solutions: ");
					indv[i].print_dna (get_dna_length());
					putchar ('\n');
				}
			} else {
				indv[i].set_sol (0);
			}

			// Evaluate Gate Efficiency
			if (optimize == 1) {
				score += eval_efficiency (grid);
			}

			indv[i].set_fit (score);
			// indv[i].set_eval (1);
			fpga_clear ();
		}

		// Sort population by fitness & solution
		// Descending order, highest fitness first
		GeneticAlgorithm::Sort (indv);

		// Track Statistics && Checks for solution
		statistics (indv, gen);

		// Controlled optimization
		if (stats.sol_count [gen] > 5) {
			optimize = 1;
		} else {
			optimize = 0;
		}

		// Status Update
		status_print (gen);
	}



	/* ===== END SIMULATION LOOP ===== */

	sim_done = 1;
	fpga_clear ();
	report (grid, seed);

	return solution_found;
}


bool sim_is_done (void) {
	return sim_done;
}



/* ========== Results & Reporting Function ========== */

void statistics (GeneticAlgorithm *const array, const unsigned int &gen) {

	// Get average value
	float average = 0.0;
	for (uint32_t i = 0 ; i < pop_lim ; i++) {
		average += array[i].get_fit();
	}
	average /= pop_lim;
	stats.avg [gen] = average;

	// Get median value (list is presorted)
	float median = 0;
	if (pop_lim % 2 == 0) {
		median = array[pop_lim/2].get_fit() + array[(pop_lim/2)-1].get_fit();
		stats.med [gen] = median/2;
	} else {
		stats.med [gen] = array[pop_lim/2].get_fit();
	}

	// Get max value (list is presorted)
	stats.max [gen] = array[0].get_fit();

	// Get min value (list is presorted)
	stats.min [gen] = array[pop_lim-1].get_fit();

	// Check for solutions found
	stats.sol_count [gen] = count_solution (indv);
}


void report (uint8_t *const *const grid, const uint8_t *const seed) {
	// How many top individuals to display at the end
	constexpr unsigned int N = 10;

	printf (ANSI_GREEN "\tDONE : %.f s\n" ANSI_RESET, difftime (time_now, time_start) );

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

	// Displays top 'N' individuals
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

	// Generate & Set Grid
	ca_gen_grid (grid, indv[0].get_dna(), seed);
	ca_gen_grid (grid, indv[0].get_dna());
	fpga_set_grid (grid);

	// Evaluate Circuit
	if (tt::get_mode() == 0) {

		// Check each case independently
		eval_com_insp (0);
		fpga_clear ();
		fpga_set_grid (grid);
		eval_com_insp (1);
		fpga_clear ();
		fpga_set_grid (grid);
		eval_com_insp (2);

	} else {
		eval_seq_insp ();
	}

	// Grid Print of Fittest Solution
	if ( get_data_caprint() == 1 ) {
		printf ("\n\e[100m\t-- Generated Logic Circuit --" ANSI_RESET "\n");
		ca_print_grid (grid);
		cout << endl;
	}

	return;
}


void sim_export (void) {
	FILE *fp;
	char filename [64];
	char buffer [64];

	// Get Current Local Time & Convert to Time Struct
	time_t raw_time;
	struct tm *timeinfo;
	time (&raw_time);
	timeinfo = localtime (&raw_time);

	// Sets filename to ./rpt/YYYYMMDD-HHMMSS format
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
	// How many individuals to print details of.
	constexpr int N = 30;

	fprintf (fp, "\n\n Top %u Individuals\n"
			"UID | FITNESS | DNA \n", N
	);

	// Prints N or the current pop size, which ever is smaller
	for (unsigned int i = 0 ; i < pop_lim ; i++) {
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
