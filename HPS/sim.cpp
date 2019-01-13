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
	// Array of stats across each generations
	float *avg;
	float *med;
	unsigned int *max;
	unsigned int *min;
	unsigned int *sol_count;
};

static stats_var stats;
static stats_var stats_gate;



/* ========== Flags ========== */

static bool solution_found = 0;
static bool data_exported = 0;
static bool sim_done = 0;
static bool sim_init_flag = 0;



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
		if (solution_found)
			printf (ANSI_GREEN " << Solutions Found! (%u)" ANSI_RESET, stats.sol_count[gen]);

		putchar ('\n');
	}
}



/* ========== Simulation Functions ========== */

void sim_init (void) {
	// Checks Conditions
	if ( tt::table_is_init () == 0 ) {
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
		"\tCAPRINT = %1u | EXPORT = %1u | REPORT = %1u\n"
		"\tROW = %1u | MODE : ",
		pop_lim, gen_lim, get_ga_mutp(), get_ga_pool(),
		dimx, dimy, color, nb,
		get_data_caprint(), get_data_export(), get_data_report(),
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
	stats.avg = (float *) calloc (gen_lim, sizeof (float));
	stats.med = (float *) calloc (gen_lim, sizeof (float));
	stats.max = (unsigned int *) calloc (gen_lim, sizeof (unsigned int));
	stats.min = (unsigned int *) calloc (gen_lim, sizeof (unsigned int));
	stats.sol_count = (unsigned int *) calloc (gen_lim, sizeof (unsigned int));

	stats_gate.avg = (float *) calloc (gen_lim, sizeof (float));
	stats_gate.med = (float *) calloc (gen_lim, sizeof (float));
	stats_gate.max = (unsigned int *) calloc (gen_lim, sizeof (unsigned int));
	stats_gate.min = (unsigned int *) calloc (gen_lim, sizeof (unsigned int));

	// Set fitness limit
	fit_lim = get_score_max ();

	// Seed RNG
	seed_rng32 ();

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
		indv[i].Free ();
	}
	free (indv);

	// Free data array
	free (stats.avg);
	free (stats.med);
	free (stats.max);
	free (stats.min);
	free (stats.sol_count);

	free (stats_gate.avg);
	free (stats_gate.med);
	free (stats_gate.max);
	free (stats_gate.min);

	printf (ANSI_GREEN "DONE\n" ANSI_RESET);
}


int sim_run (uint8_t *const *const grid, const uint8_t *const seed) {

	if ( sim_init_flag == 0 ) {
		printf (ANSI_RED "\nSimulation Requires Initialization\n" ANSI_RESET);
		return -1;
	}

	printf ("\tSimulation Progress:\n");
	time (&time_start);



	// ===== MAIN SIMULATION LOOP ===== //

	// Loop over each generation
	for (unsigned int gen = 0 ; gen < gen_lim ; gen++) {
		// Perform selection, reproduction, crossover, and mutation
		GeneticAlgorithm::Selection (indv);
		GeneticAlgorithm::Repopulate (indv, seed);

		// Loop over each individual
		for (unsigned int i = 0 ; i < pop_lim ; i++) {
			// Automatically ages an individual
			indv[i].set_age();

			// Evaluate Individual -- Once per individual
			if ( indv[i].get_eval () == 0 ) {
				fpga_clear ();

				// Edit FPGA RAM
				fpga_set_grid (indv[i].get_grid());

				// Evaluate Circuit Truth Table
				uint32_t score = 0;

				// Sequential or Combinational Logic
				if (tt::get_mode() == 0) {
					// Test a few cases and gets the average score
					score += eval_com (0);
					score += eval_com (1);

					for (int j = 0 ; j < 3 ; j++) {
						score += eval_com (2);
					}

					score /= 5;
				} else {
					// Runs a single continuous test
					score = eval_seq ();
				}

				// Flags this as a viable solution, if the fitness is maxed
				indv[i].set_sol ((score == fit_lim));

				// Assign fitness score
				indv[i].set_fit (score);

				// Evaluate Efficiency
				indv[i].set_gate (eval_efficiency (indv[i].get_grid()));
				indv[i].set_eval (1);
			}
		}

		// Sort population by fitness & solution
		// Descending order, solutions, higher fitness, higher efficiency first
		GeneticAlgorithm::Sort (indv);

		// Track Statistics && Checks for solution
		statistics (indv, gen);

		// Set the solutions found flag
		solution_found = (stats.sol_count[gen] > 0);

		// Status Update
		status_print (gen);
	}



	// ===== END SIMULATION LOOP ===== //

	if ( get_data_report() ) report (grid, seed);

	sim_done = 1;
	fpga_clear ();
	return solution_found;
}


bool sim_is_done (void) {
	return sim_done;
}



/* ========== Results & Reporting Function ========== */

void statistics (GeneticAlgorithm *const array, const unsigned int &gen) {

	// ===== Fitness Score Statistics ===== //

	{
		// Average
		float average = 0.0;
		for (uint32_t i=0; i<pop_lim; i++) {
			average += array[i].get_fit();
		}
		average /= pop_lim;
		stats.avg [gen] = average;

		// Median (list is presorted)
		float median = 0.0;
		if (pop_lim % 2 == 0) {
			median = array[pop_lim/2].get_fit() + array[(pop_lim/2)-1].get_fit();
			stats.med [gen] = median/2;
		} else {
			stats.med [gen] = array[pop_lim/2].get_fit();
		}

		// Max (list is presorted)
		stats.max [gen] = array[0].get_fit();

		// Min (list is presorted)
		stats.min [gen] = array[pop_lim-1].get_fit();

		// Check for solutions found
		stats.sol_count [gen] = count_solution (array);
	}


	// ===== Gate Efficiency Score Statistics ===== //

	{
		// Average
		float average = 0.0;
		for (uint32_t i=0; i<pop_lim; i++) {
			average += array[i].get_gate();
		}
		average /= pop_lim;
		stats_gate.avg [gen] = average;


		// Max - Min - Median
		int working_array [pop_lim] = {0};

		for (uint32_t i=0; i<pop_lim; i++) {
			unsigned short gate_val = array[i].get_gate();

			if (stats_gate.max [gen] < gate_val) {
				stats_gate.max [gen] = gate_val;
			}

			if (stats_gate.min [gen] > gate_val) {
				stats_gate.min [gen] = gate_val;
			}

			working_array[i] = gate_val;
		}

		sort (working_array, working_array + pop_lim);

		// Median
		float median = 0.0;
		if (pop_lim % 2 == 0) {
			median = working_array[pop_lim/2] + working_array[(pop_lim/2)-1];
			stats_gate.med [gen] = median/2;
		} else {
			stats_gate.med [gen] = working_array[pop_lim/2];
		}

	}

}


void report (uint8_t *const *const grid, const uint8_t *const seed) {
	// How many top individuals to display at the end
	constexpr unsigned int N = 10;

	printf (ANSI_GREEN "\tDONE : %.f s\n" ANSI_RESET, difftime (time_now, time_start) );

	if (solution_found)
		printf (ANSI_GREEN "\tSolutions found!\n\n" ANSI_RESET);
	else
		printf (ANSI_RED "\tNo solution found\n\n" ANSI_RESET);

	printf ("\tFinal Fitness Statistics:\n"
			"\tAverage Fitness: %7.1f / %5d\n"
			"\tMedian Fitness:  %7.1f / %5d\n"
			"\tMaximum Fitness:   %5u / %5d\n"
			"\tMinimum Fitness:   %5u / %5d\n\n",
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


	// ===== Evaluate & Graph Top Individuals ===== //

	printf ("\n\n\tFittest Individual:\n"
		"UID: %u | FIT: %u | DNA: ", indv[0].get_uid(), indv[0].get_fit() );
	indv[0].print_dna( dna_length );
	printf ("\n");

	// Generate & Set Grid
	ca_gen_grid (grid, indv[0].get_dna(), seed);
	ca_gen_grid (grid, indv[0].get_dna());
	fpga_clear ();
	fpga_set_grid (grid);

	// Evaluate Circuit
	if ( tt::get_mode() == 0 ) {

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

	//
	printf ("\n\tEfficiency Score: %u / %u\n", eval_efficiency(grid), get_es_max());

	// Grid Print of Fittest Solution
	if ( get_data_caprint() == 1 ) {
		printf ("\n\e[100m\t-- Generated Logic Circuit --" ANSI_RESET "\n");
		ca_print_grid (grid);
		cout << endl;
	}

	fpga_clear ();
	return;
}


void sim_export (void) {
	if ( sim_is_done () == 0 ) {
		printf (ANSI_YELLOW "\tNothing to be exported.\n" ANSI_RESET);
		return;
	}

	FILE *rpt, *csv;
	char filename [64];
	char timestring [64];

	// Get Current Local Time & Convert to Time Struct
	time_t raw_time;
	struct tm *timeinfo;
	time (&raw_time);
	timeinfo = localtime (&raw_time);

	// Sets filename to ./rpt/YYYYMMDD-HHMMSS format
	strftime (timestring, 64, "%Y%m%d-%H%M%S", timeinfo);
	printf ("Exporting results as: \"%s\" ...", timestring);

	// Open Report File
	strcpy (filename, "./rpt/");
	strcat (filename, timestring);
	strcat (filename, ".rpt");
	rpt = fopen (filename, "w");
	if (rpt == NULL) {
		printf (ANSI_RED "FAILED -- Unable to open file: %s\n" ANSI_RESET, filename);
		return;
	}

	// Open CSV File
	strcpy (filename, "./rpt/");
	strcat (filename, timestring);
	strcat (filename, ".csv");
	csv = fopen (filename, "w");
	if (csv == NULL) {
		printf (ANSI_RED "FAILED -- Unable to open file: %s\n" ANSI_RESET, filename);
		fclose (rpt);
		return;
	}

	// ===== Write to Report File ===== //

	fprintf (rpt, "===== Simulation Report %s =====\n\n", timestring);

	fprintf (rpt, "Settings:\n"
		"====== Genetics Algorithm =====\n"
		"Generation Limit: %u\n"
		"Population Limit: %u\n"
		"Mutation Rate: %f\n"
		"Tournament Pool Size: %u\n"
		"====== Cellular Automaton ======\n"
		"X Dimension: %u\n"
		"Y Dimension: %u\n"
		"CA Color: %u\n"
		"CA Neighbor: %u\n"
		"====== Truth Table ======\n"
		"Row Count: %u\n"
		"MASK: 0x%016llX\n"
		"MASK Bitcount: %llu\n"
		"Mode: " ,
		gen_lim, pop_lim, get_ga_mutp(), get_ga_pool(),
		dimx, dimy, color, nb,
		tt::get_row(), tt::get_mask(), tt::get_mask_bc()
	);

	if (tt::get_mode())
		fprintf (rpt, "SEQUENTIAL\n");
	else
		fprintf (rpt, "COMBINATIONAL\n");

	uint64_t *input = tt::get_input();
	uint64_t *output = tt::get_output();

	fprintf (rpt, "\n(HEX) INPUT | EXPECTED OUTPUT\n");

	for (unsigned int i = 0 ; i < tt::get_row() ; i++) {
		fprintf (rpt, "%016llX | %016llX\n", input[i], output[i]);
	}

	fprintf (rpt, "\n\nPopulation Dump:\n"
				"    UID | SOL | FITNESS | GATE | DNA \n");

	// Prints N or the current pop size, which ever is smaller
	for (unsigned int i = 0 ; i < pop_lim ; i++) {
		fprintf (rpt, "%7u | %3u | %7u | %4u | ",
			indv[i].get_uid(), indv[i].get_sol(),
			indv[i].get_fit(), indv[i].get_gate()
		);

		indv[i].fprint_dna (rpt, dna_length);
		fputc ('\n', rpt);
	}

	fclose (rpt);



	// ===== Write to CSV File ===== //

	// Time for run identification
	fprintf (csv, "%s,\n", timestring);

	// Dump run statistics
	fprintf (csv,
			"generation,"
			"\"fit average\","
			"\"fit median\","
			"\"fit minimum\","
			"\"fit maximum\","
			"\"solution count\","
			"\"gate average\","
			"\"gate median\","
			"\"gate minimum\","
			"\"gate maximum\",\n"
	);

	for (unsigned int gen = 0 ; gen < gen_lim ; gen++) {
		fprintf (csv, "%u,%f,%f,%u,%u,%u,%f,%f,%u,%u,\n",
			gen,
			stats.avg[gen],
			stats.med[gen],
			stats.min[gen],
			stats.max[gen],
			stats.sol_count[gen],
			stats_gate.avg[gen],
			stats_gate.med[gen],
			stats_gate.min[gen],
			stats_gate.max[gen]
		);
	}

	fclose (csv);



	// ===== End ===== //

	data_exported = 1;
	printf (ANSI_GREEN " DONE\n" ANSI_RESET);
	return;
}


bool export_is_done (void) {
	return data_exported;
}
