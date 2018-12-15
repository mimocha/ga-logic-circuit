/*	Main C++ file for the Genetic Algorithm main file
	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

/* ========== Standard Library Include ========== */

#include <stdio.h>		// Standard I/O
#include <stdlib.h>		// calloc, free
#include <stdint.h>		// uint definitions
#include <math.h>		// pow, floor
#include <iostream>		// cin, cout
#include <string.h>		// strlen, memcpy



/* ========== Custom File Header ========== */

#include "main.hpp"		// Standard Includes & Function Prototypes
#include "ansi.hpp"		// Colored Terminal Outputs
#include "ca.hpp"		// Cellular Automaton Functions
#include "eval.hpp"		// Evaluation Functions
#include "fast.hpp"		// Initialize RNG Seed
#include "fpga.hpp"		// FPGA Functions
#include "global.hpp"	// Global Parameters
#include "misc.hpp"		// Miscellaneous Functions
#include "sim.hpp"		// Simulation Function Wrapper
#include "truth.hpp"	// Truth Table



// Cellular Automaton Virtual Grid
static uint8_t **grid;

// Cellular Automaton Seed Array
static uint8_t *seed;

using namespace std;
using namespace GlobalSettings;
namespace tt = TruthTable;

/* ========== MAIN ========== */

int main (int argc, char **argv) {
	// Initialization Functions
	main_init ();
	fpga_init ();
	ca_init ();

	while (1) {
		unsigned int sel = main_menu ();

		switch (sel) {

			case 0: // Exit
				printf ("\nExiting Program.\n");

				tt::clear_table ();
				fpga_cleanup ();
				main_cleanup ();

				return EXIT_SUCCESS;

			case 1: // About
				about ();
				break;

			case 2: // Settings
				settings ();
				break;

			case 3: // Initialize other modules
				fpga_init ();
				ca_init ();
				break;

			case 4: // FPGA Circuit Verification
				fpga_verify (grid);
				break;

			case 5: // Read Truth Table File
				tt::set_table ();
				break;

			case 6: // Run Simulation
				sim_cleanup ();
				sim_init ();
				sim_run (grid, seed);
				break;

			case 7: // Export CA Grid
				export_ca_grid ();
				break;

			case 8: // Inspect DNA
				inspect ();
				break;

			case 9: // Export Simulation Data
				sim_export ();
				break;

			case 10: // Preprogrammed Routine
				special ();
				break;

			default: // Invalid Input
				printf ("Invalid input: %d\n", sel);

		}
	}
}



/* ========== Main Menu / Initialize / Cleanup Functions ========== */

void main_init (void) {
	printf ("Initializing GA Program...");

	// Allocates 2-dimensional array pointer for grid and initializes to zero
	grid = (uint8_t **) calloc (PHYSICAL_DIMY, sizeof (uint8_t *));
	for (uint16_t y = 0 ; y < PHYSICAL_DIMY ; y++) {
		grid [y] = (uint8_t *) calloc (PHYSICAL_DIMX, sizeof (uint8_t));
	}

	// Allocates CA seed and initializes to zero
	seed = (uint8_t *) calloc (PHYSICAL_DIMX, sizeof (uint8_t));
	uint16_t mid = floor (PHYSICAL_DIMX / 2);
	seed [mid] = 1;

	seed_rng32 ();

	printf (ANSI_GREEN "DONE\n" ANSI_RESET);
}

void main_cleanup (void) {
	printf ("Cleaning up main... ");

	free (seed);

	for (uint16_t y = 0 ; y < PHYSICAL_DIMY ; y++) {
		free (grid[y]);
	}
	free (grid);

	printf (ANSI_GREEN "DONE\n" ANSI_RESET);
}

unsigned int main_menu (void) {
	unsigned int var;

	// Prints option list
	printf (ANSI_REVRS "\n\t>---- Main Menu ----<\n" ANSI_RESET
			"\t0. Exit Program\n"
			"\t1. About\n"
			"\t2. Settings\n"
			"\t3. Initialize Modules | ");

	if (fpga_is_init () == 1) {
		printf (ANSI_GREEN "FPGA OK" ANSI_RESET);
	} else {
		printf (ANSI_RED "FPGA NO" ANSI_RESET);
	}

	printf (" | ");

	if (ca_is_init () == 1) {
		printf (ANSI_GREEN "CA OK\n" ANSI_RESET);
	} else {
		printf (ANSI_RED "CA NO\n" ANSI_RESET);
	}

	printf ("\t4. Verify FPGA\n"
			"\t5. Set Truth Table | ");

	if (tt::table_is_init () == 1) printf (ANSI_GREEN "DONE\n" ANSI_RESET);
	else printf (ANSI_YELLOW "WAITING\n" ANSI_RESET);

	printf ("\t6. Run Simulation\n"
			"\t7. Export CA Grid\n");

	printf ("\t8. Inspect DNA\n"
			"\t9. Export Results  | ");

	if (export_is_done () == 1) printf (ANSI_GREEN "DONE\n" ANSI_RESET);
	else printf (ANSI_YELLOW "WAITING\n" ANSI_RESET);

	printf ("\t10. Special Routine\n");

	printf ("\nWaiting for Input: ");

	// Sanitized Scan
	scan_uint (&var);

	return var;
}



/* ========== Menu Options ========== */

void settings (void) {
	while (true) {
		unsigned int var;

		// Prints option list
		printf (ANSI_REVRS "\n\t>>--- Settings ---<<\n" ANSI_RESET
			"\t0. Back\n"
			ANSI_BOLD "\t===== Genetic Algorithm Parameters =====\n" ANSI_RESET
			"\t1. GA Max Population\t| Current Value: %u\n"
			"\t2. GA Max Generation\t| Current Value: %u\n"
			"\t3. GA Mutation Prob\t| Current Value: %.3f\n"
			"\t4. GA Pool Size\t\t| Current Value: %u\n"
			ANSI_BOLD "\t===== Cellular Automaton Parameters =====\n" ANSI_RESET
			"\t5. CA X Axis Dimension\t| Current Value: %u\n"
			"\t6. CA Y Axis Dimension\t| Current Value: %u\n"
			"\t7. CA Color Count\t| Current Value: %u\n"
			"\t8. CA Neighbor Count\t| Current Value: %u\n"
			ANSI_BOLD "\t===== Data Parameters =====\n" ANSI_RESET
			"\t9. DATA CA Print\t| Current Value: %u\n"
			"\t10. DATA Export\t\t| Current Value: %u\n"
			"\t11. DATA Report\t\t| Current Value: %u\n"
			ANSI_BOLD "\t===== Truth Table Parameters =====\n" ANSI_RESET
			"\t12. TT Row Count\t| Current Value: %u\n"
			"\t13. TT Mode (0 Combinational | 1 Sequential) | Current Value: %u\n"
			"\t14. TT Mask\t\t| Current Value: %016llX | (%llu bits)\n\n"
			"Waiting for Input: ",
			get_ga_pop(), get_ga_gen(), get_ga_mutp(), get_ga_pool(),
			get_ca_dimx(), get_ca_dimy(), get_ca_color(), get_ca_nb(),
			get_data_caprint(), get_data_export(), get_data_report(),
			tt::get_row(), tt::get_mode(), tt::get_mask(), tt::get_mask_bc()
		);

		// Sanitized Scan
		scan_uint (&var);

		switch (var) {
			case 0: // Back
				printf ("Returning to main menu\n");
				return;

			case 1: // GA.POP
				printf ("Input New Value: ");
				set_ga_pop ( scan_uint () );
				break;

			case 2: // GA.GEN
				printf ("Input New Value: ");
				set_ga_gen ( scan_uint () );
				break;

			case 3: // GA.MUTP
				printf ("Input New Value: ");
				set_ga_mutp ( scan_float () );
				break;

			case 4: // GA.POOL
				printf ("Input New Value: ");
				set_ga_pool ( scan_uint () );
				break;

			case 5: // CA.DIMX
				printf ("Input New Value: ");
				set_ca_dimx ( scan_uint () );
				ca_need_update ();
				break;

			case 6: // CA.DIMY
				printf ("Input New Value: ");
				set_ca_dimy ( scan_uint () );
				ca_need_update ();
				break;

			case 7: // CA.COLOR
				printf ("Input New Value: ");
				set_ca_color ( scan_uint () );
				ca_need_update ();
				break;

			case 8: // CA.NB
				printf ("Input New Value: ");
				set_ca_nb ( scan_uint () );
				ca_need_update ();
				break;

			case 9: // DATA.CAPRINT
				printf ("Input New Value: ");
				set_data_caprint ( scan_bool () );
				break;

			case 10: // DATA.EXPORT
				printf ("Input New Value: ");
				set_data_export ( scan_bool () );
				break;

			case 11: // DATA.EXPORT
				printf ("Input New Value: ");
				set_data_report ( scan_bool () );
				break;

			case 12: // TRUTH.ROW
				printf ("Input New Value: ");
				tt::set_row ( scan_uint () );
				break;

			case 13: // TRUTH.MODE
				printf ("Input New Value: ");
				tt::set_mode ( scan_bool () );
				break;

			case 14: // MASK
				printf ("Input New Value: ");
				tt::set_mask ( scan_hex () );
				break;

			default:
				printf ("Invalid input: %d\n", var);
				break;

		}
	}
}

void export_ca_grid (void) {
	// Required initialization
	if ( ca_is_init () == 0 ) {
		printf (ANSI_RED "CA not initialized.\n" ANSI_RESET);
		return;
	}

	printf (ANSI_REVRS "\n\tCA Grid Export to CSV and PPM image\n" ANSI_RESET);

	// Enter DNA String in given base / LSB format
	const unsigned int length = (unsigned int) pow (get_ca_color(), get_ca_nb());

	printf ("Input DNA string in base %u | LSB | Expected length - %u :\n", get_ca_color(), length);

	// Buffer to prevent overflows
	uint8_t buffer [length*2];
	scanf ("%s", buffer);

	// Checks input length validity
	if ( strlen ( (char *)buffer ) != length) {
		printf (ANSI_RED "DNA length mismatch. Given length was %u\n" ANSI_RESET,
			(unsigned int) strlen ( (char *)buffer ));
		return;
	}

	// Temporary array for this function
	uint8_t *const dna = new uint8_t [length] ();
	memcpy (dna, buffer, length);

	// Checks DNA string validity & Converts ASCII to uint8_t
	bool invalid_flag = 0;
	for (unsigned int i = 0 ; i < length ; i++) {
		// Adjusts ASCII input into uint8_t [0,9]
		dna [i] -= 48;

		if (dna [i] >= get_ca_color()) {
			printf ("\e[41;97m" "^" ANSI_RESET);
			invalid_flag = 1;
		} else {
			putchar (' ');
		}
	}

	if (invalid_flag) {
		printf (ANSI_RED "\nDNA string invalid at marked position\n" ANSI_RESET);
		delete[] dna;
		return;
	} else {
		printf (ANSI_GREEN "\nDNA string OK\n" ANSI_RESET);
	}


	/* ===== CA GRID SET ===== */

	ca_gen_grid (grid, dna, seed);
	ca_gen_grid (grid, dna);


	/* ===== Get File Name ===== */

	FILE *fp;
	char filename [64];

	printf ("Filename: ");
	scanf ("%s", filename);


	/* ===== Write to CSV ===== */

	// Append file extention
	char csvfile [64];
	strcpy (csvfile, "./export/");
	strcat (csvfile, filename);
	strcat (csvfile, ".csv");

	fp = fopen (csvfile, "w");
	if (fp == NULL) {
		printf (ANSI_RED "FAILED -- Unable to open file: %s\n" ANSI_RESET, csvfile);
		return;
	}

	printf ("Writing to \"%s\" ...", csvfile);

	for (unsigned int y = 0 ; y < PHYSICAL_DIMY ; y++) {
		for (unsigned int x = 0 ; x < PHYSICAL_DIMX ; x++) {
			fprintf (fp, "%d,", grid[y][x]);
		}
		fprintf (fp, "\n");
	}

	fclose (fp);
	printf (ANSI_GREEN " DONE\n" ANSI_RESET);


	/* ===== Write to NetPBM ===== */

	// Append file extention
	char imgfile [64];
	strcpy (imgfile, "./export/");
	strcat (imgfile, filename);
	strcat (imgfile, ".ppm");

	fp = fopen (imgfile, "w");
	if (fp == NULL) {
		printf (ANSI_RED "FAILED -- Unable to open file: %s\n" ANSI_RESET, imgfile);
		return;
	}

	printf ("Writing to \"%s\" ...", imgfile);

	// PPM File Header
	fprintf (fp, "P3\n%u %u\n255\n", PHYSICAL_DIMX, PHYSICAL_DIMY);

	// Additional Comments
	fprintf (fp, "# GENERATED WITH DNA STRING:\n# ");
	for (unsigned int i = 0 ; i < length ; i++) {
		fprintf (fp, "%d", dna[i]);
	}
	fprintf (fp, "\n");

	// Actual Data -- CMYK Color Scheme
	for (unsigned int y = 0 ; y < PHYSICAL_DIMY ; y++) {
		for (unsigned int x = 0 ; x < PHYSICAL_DIMX ; x++) {
			switch (grid[y][x]) {
				case 0: // KEY
					fprintf (fp, "35 31 32 ");
					break;
				case 1: // CYAN
					fprintf (fp, "0 174 239 ");
					break;
				case 2: // MAGENTA
					fprintf (fp, "236 0 140 ");
					break;
				case 3: // YELLOW
					fprintf (fp, "255 242 0 ");
					break;
			}
		}
		fprintf (fp, "\n");
	}

	fclose (fp);
	printf (ANSI_GREEN " DONE\n" ANSI_RESET);


	delete[] dna;
	return;
}

void inspect (void) {
	// Required initialization
	if ( ca_is_init () == 0 ) {
		printf (ANSI_RED "CA not initialized.\n" ANSI_RESET);
		return;
	}

	printf (ANSI_REVRS "\n\tDNA Inspection\n" ANSI_RESET);

	// Enter DNA String in given base / LSB format
	const unsigned int length = (unsigned int) pow (get_ca_color(), get_ca_nb());

	printf ("Input DNA string in base %u | LSB | Expected length - %u :\n", get_ca_color(), length);

	/* Creates an input buffer
		BUGFIX:
		Previously, inputting a string of length "DNA LENGTH + 4" would cause error.

		This was because "uint8_t *dna" only allocated memory for:
			(uint8_t *) calloc (length, sizeof (uint8_t))
		But the given string used more memory than was allocated.

		When pointer "*dna" inevitably fails the length validity check,
		the code attempts to free the size of the input string,
		and not the size of memory allocated.
		This would result in either a Segmentation Fault, or a double free.

		Thus, now inputs are first put into a buffer, and the buffer is validated first.
		If the buffer passes the validation test, then memory is passed into "*dna".
	*/
	uint8_t buffer [length*2];
	scanf ("%s", buffer);

	// Checks input length validity
	if ( strlen ( (char *)buffer ) != length) {
		printf (ANSI_RED "DNA length mismatch. Given length was %u\n" ANSI_RESET,
			(unsigned int) strlen ( (char *)buffer ));
		return;
	}

	// Temporary array for this function
	uint8_t *const dna = new uint8_t [length] ();
	memcpy (dna, buffer, length);

	// Checks DNA string validity & Converts ASCII to uint8_t
	bool invalid_flag = 0;
	for (unsigned int i = 0 ; i < length ; i++) {
		// Adjusts ASCII input into uint8_t [0,9]
		dna [i] -= 48;

		if (dna [i] >= get_ca_color()) {
			printf ("\e[41;97m" "^" ANSI_RESET);
			invalid_flag = 1;
		} else {
			putchar (' ');
		}
	}

	if (invalid_flag) {
		printf (ANSI_RED "\nDNA string invalid at marked position\n" ANSI_RESET);
		goto INSPECT_END;
	} else {
		printf (ANSI_GREEN "\nDNA string OK\n" ANSI_RESET);
	}



	// ===== PRINT CA GRID ===== //

	if ( get_data_caprint () == 0 ) goto INSPECT_GRID;

	printf ("\n\e[100m\t\t-- Generated Logic Circuit --" ANSI_RESET "\n");

	// Print Seed Row
	ca_print_row (seed);

	// Print CA First Pass
	ca_gen_grid (grid, dna, seed);
	ca_print_grid (grid);

	// Print Center Marker
	printf ("\e[103;30m");
	for (unsigned int x = 0 ; x < get_ca_dimx() ; x++) {
		if (x % 4 == 0) {
			printf ("%X", x / 4 % 16);
		} else {
			putchar ('+');
		}
	}
	printf (ANSI_RESET "\n");

	// Print CA Second Pass
	ca_gen_grid (grid, dna);
	ca_print_grid (grid);

	// Print Bottom Marker
	printf ("\e[103;30m");
	for (unsigned int x = 0 ; x < get_ca_dimx() ; x++) {
		if (x % 4 == 0) {
			printf ("%X", x / 4 % 16);
		} else {
			cout << "+";
		}
	}
	printf (ANSI_RESET "\n\n");



	// ===== FPGA SET GRID && EVALUATE ===== //

	INSPECT_GRID:
	if ( tt::table_is_init () == 0 ) {
		printf (ANSI_YELLOW "No truth table defined.\n" ANSI_RESET);
		goto INSPECT_END;
	}
	if ( fpga_is_init () == 0 ) {
		printf (ANSI_YELLOW "FPGA not initialized.\n" ANSI_RESET);
		goto INSPECT_END;
	}

	// Wrap code in an explicit block -- jump to label ‘INSPECT_END’ [-fpermissive]
	// Ensures the compiler that it is not jumping over the initialization of any variables.
	{
		// How many test cases to run
		constexpr unsigned int CHECK_NUM = 100;

		fpga_clear ();
		ca_gen_grid (grid, dna, seed);
		ca_gen_grid (grid, dna);
		fpga_set_grid (grid);

		// Evaluate Circuit
		const unsigned int max_score = get_score_max() * CHECK_NUM;
		unsigned int score = 0;

		if (tt::get_mode() == 0) {

			// Check each case independently
			eval_com_insp (0);
			fpga_clear ();
			fpga_set_grid (grid);
			eval_com_insp (1);
			fpga_clear ();
			fpga_set_grid (grid);
			eval_com_insp (2);

			// Check random cases
			printf ("\n\tChecking %u Random Cases... ", CHECK_NUM);

			fpga_clear ();
			fpga_set_grid (grid);
			for (unsigned int i = 0 ; i < CHECK_NUM ; i++) {
				score += eval_com (2);
			}

		} else {
			// Inspect sequential truth table
			eval_seq_insp ();

			// Run continuous test
			printf ("\n\tChecking %u Iterations... ", CHECK_NUM);

			fpga_clear ();
			fpga_set_grid (grid);
			for (unsigned int i = 0 ; i < CHECK_NUM ; i++) {
				score += eval_seq ();
			}
		}

		if (score == max_score) {
			printf (ANSI_GREEN "%u / %u | PASS\n" ANSI_RESET, score, max_score);
		} else {
			printf (ANSI_YELLOW "%u / %u | FAIL\n" ANSI_RESET, score, max_score);
		}

		unsigned int efficiency = eval_efficiency (grid);
		printf ("\n\tEfficiency Score: %u / %u\n", efficiency, get_es_max());
	}


	// ===== CLEANUP ===== //

	INSPECT_END:
	delete[] dna;
	fpga_clear ();
	return;
}



/* ========== Special Routine ========== */

void special (void) {
	printf (ANSI_REVRS "\n\t>> Special Routine <<\n" ANSI_RESET);

	set_data_caprint (0);
	set_data_report (0);
	tt::set_mode(1);

	constexpr int MAX = 10;

	for (int j = 0 ; j < 5 ; j++) {
		tt::auto_set_table(16+j);

		for (int i = 0 ; i < MAX ; i++) {
			int sim_flag;
			printf (ANSI_REVRS "\n\t%d | %d / %d\n" ANSI_RESET, j, i, MAX);

			sim_cleanup();
			sim_init();
			sim_flag = sim_run (grid, seed);
			sim_export ();

			if (sim_flag == -1) {
				printf (ANSI_RED "\n\tSimulation Failed.\n");
				return;
			}
		}

	}


	printf (ANSI_REVRS "\n\t>> Special Routine Done <<\n\n ANSI_RESET");
}
