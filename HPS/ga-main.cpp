/*	Main C++ file for the Genetic Algorithm main file
	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

/* ========== Standard Library Include ========== */

#include <stdio.h>		/* Standard I/O */
#include <stdlib.h>		/* calloc, free */
#include <stdint.h>		/* uint definitions */
#include <time.h>		/* time */
#include <math.h>		/* pow, round, ceiling */
#include <iostream>		/* cin, cout */
#include <algorithm>	/* sort, find */
#include <vector>		/* vectors */
#include <array>		/* arrays */
#include <cstring>		/* strcmp */

/* ========== Custom File Header ========== */

#include "ga-main.hpp"	/* Standard Includes & Function Prototypes */
#include "ansi.hpp"		/* Colored Terminal Outputs */
#include "ca.hpp"		/* Cellular Automaton Functions */
#include "eval.hpp"		/* Evaluation Functions */
#include "fpga.hpp"		/* FPGA Functions */
#include "global.hpp"	/* Global Parameters */
#include "misc.hpp"		/* Miscellaneous Functions */
#include "sim.hpp"		/* Simulation Function Wrapper */
#include "truth.hpp"	/* Truth Table */

/* Cellular Automaton Virtual Grid */
static uint8_t **grid;

/* Cellular Automaton Seed Array */
static uint8_t *seed;

using namespace std;
using namespace GlobalSettings;
namespace tt = TruthTable;

/* ========== MAIN ========== */

int main (int argc, char **argv) {
	/* Main Initialization Function */
	main_init ();

	/* Initialize FPGA */
	fpga_init ();

	/* Initialize CA */
	ca_init ();

	while (1) {
		/* Variable for selecting menu option -- Prints available options and get user input */
		unsigned int sel = main_menu ();

		switch (sel) {

			case 0: /* Exit */
				printf ("\nExiting Program.\n");

				sim_cleanup ();
				tt::clear_table ();
				ca_cleanup ();
				fpga_cleanup ();
				main_cleanup ();

				return EXIT_SUCCESS;

			case 1: /* About */
				about ();
				break;

			case 2: /* Settings */
				settings ();
				break;

			case 3: /* Initialize FPGA */
				fpga_init ();
				break;

			case 4: /* FPGA Circuit Verification */
				fpga_verify (grid);
				break;

			case 5: /* Read Truth Table File */
				tt::set_table ();
				break;

			case 6: /* Run Simulation */
				sim_init ();
				sim_run (grid, seed);
				break;

			case 7: /* Display Previous Results */
				sim_results ();
				break;

			case 8: /* Inspect DNA */
				inspect (grid, seed);
				break;

			case 9: /* Export Results */
				sim_export ();
				break;

			case 10: /* Preprogrammed Routine */
				special ();
				break;

			default: /* Invalid Input */
				printf ("Invalid input: %d\n", sel);

		}
	}
}



/* ========== Main Menu / Initialize / Cleanup Functions ========== */

void main_init (void) {
	printf ("Initializing GA Program...");

	/* Allocates 2D working array for Cellular Automaton
		WARNING: Make sure CALLOC gets argument 2: sizeof(uint8_t *) and not sizeof(uint8_t)
		NOTE: **grid will always have the physical dimension allocated, not the user set dimension
	*/
	grid = (uint8_t **) calloc (PHYSICAL_DIMY, sizeof (uint8_t *));
	for (unsigned int i = 0 ; i < PHYSICAL_DIMY ; i++) {
		grid [i] = (uint8_t *) calloc (PHYSICAL_DIMX, sizeof (uint8_t));
	}

	/* Initialize Cellular Automaton grid seed */
	seed = (uint8_t *) calloc (PHYSICAL_DIMX, sizeof (uint8_t));
	unsigned int mid = floor (PHYSICAL_DIMX / 2);
	seed [mid] = 1;

	printf (ANSI_GREEN "DONE\n" ANSI_RESET);
}

void main_cleanup (void) {
	printf ("Cleaning up main... ");

	if (seed != NULL) free (seed);
	if (grid != NULL) {
		for (unsigned int y = 0 ; y < PHYSICAL_DIMY ; y++) {
			free (grid[y]);
		}
		free (grid);
	}

	printf (ANSI_GREEN "DONE\n" ANSI_RESET);
}

unsigned int main_menu (void) {
	unsigned int var;

	/* Prints option list */
	printf (ANSI_REVRS "\n\t>---- Main Menu ----<\n" ANSI_RESET
			"\t0. Exit Program\n"
			"\t1. About\n"
			"\t2. Settings\n"
			"\t3. Initialize FPGA | ");

	if (fpga_is_init () == 1) printf (ANSI_GREEN "DONE\n" ANSI_RESET);
	else printf (ANSI_RED "FAILED\n" ANSI_RESET);

	printf ("\t4. Verify FPGA\n"
			"\t5. Set Truth Table | ");

	if (tt::table_is_init () == 1) printf (ANSI_GREEN "DONE\n" ANSI_RESET);
	else printf (ANSI_YELLOW "WAITING\n" ANSI_RESET);

	printf ("\t6. Run Simulation\n"
			"\t7. View Results    | ");

	if (sim_is_done () == 1) printf (ANSI_GREEN "DONE\n" ANSI_RESET);
	else printf (ANSI_YELLOW "WAITING\n" ANSI_RESET);

	printf ("\t8. Inspect DNA\n"
			"\t9. Export Results  | ");

	if (export_is_done () == 1) printf (ANSI_GREEN "DONE\n" ANSI_RESET);
	else printf (ANSI_YELLOW "WAITING\n" ANSI_RESET);

	printf ("\t10. Special Routine\n");

	printf ("\nWaiting for Input: ");

	/* Sanitized Scan */
	scan_uint (&var);

	return var;
}



/* ========== Menu Options ========== */

void settings (void) {
	while (true) {
		unsigned int var;

		/* Prints option list */
		printf (ANSI_REVRS "\n\t>>--- Settings ---<<\n" ANSI_RESET
			"\t0. Back\n"
			ANSI_BOLD "\t===== Genetic Algorithm Parameters =====\n" ANSI_RESET
			"\t1. GA Max Population\t| Current Value: %u\n"
			"\t2. GA Max Generation\t| Current Value: %u\n"
			"\t3. GA Mutation Prob\t| Current Value: %.3f\n"
			"\t4. GA Pool Size\t\t| Current Value: %u\n"
			ANSI_BOLD "\t===== Cellular Automaton Parameters =====\n" ANSI_RESET
			ANSI_GRAY "\t5. CA X Axis Dimension\t| Current Value: %u\n" ANSI_RESET
			ANSI_GRAY "\t6. CA Y Axis Dimension\t| Current Value: %u\n" ANSI_RESET
			"\t7. CA Color Count\t| Current Value: %u\n"
			"\t8. CA Neighbor Count\t| Current Value: %u\n"
			ANSI_BOLD "\t===== Data Parameters =====\n" ANSI_RESET
			"\t9. DATA CA Print\t| Current Value: %u\n"
			"\t10. DATA Export\t\t| Current Value: %u\n"
			ANSI_BOLD "\t===== Truth Table Parameters =====\n" ANSI_RESET
			"\t11. TT Row Count\t| Current Value: %u\n"
			"\t12. TT F1 Scoring\t| Current Value: %u\n"
			"\t13. TT Mask\t\t| Current Value: %016llX | (%llu bits)\n\n"
			"Waiting for Input: ",
			get_ga_pop(), get_ga_gen(), get_ga_mutp(), get_ga_pool(),
			get_ca_dimx(), get_ca_dimy(), get_ca_color(), get_ca_nb(),
			get_data_caprint(), get_data_export(),
			tt::get_row(), tt::get_f1(), tt::get_mask(), tt::get_mask_bc()
		);

		/* Sanitized Scan */
		scan_uint (&var);

		switch (var) {

			case 0: /* Back */
				printf ("Returning to main menu\n");
				return;

			case 1: /* GA.POP */
				printf ("Input New Value: ");
				set_ga_pop ( scan_uint () );
				break;

			case 2: /* GA.GEN */
				printf ("Input New Value: ");
				set_ga_gen ( scan_uint () );
				break;

			case 3: /* GA.MUTP */
				printf ("Input New Value: ");
				set_ga_mutp ( scan_float () );
				break;

			case 4: /* GA.POOL */
				printf ("Input New Value: ");
				set_ga_pool ( scan_uint () );
				break;

			case 5: /* CA.DIMX */
				printf ("Cannot Be Changed\n");
				break;

			case 6: /* CA.DIMY */
				printf ("Cannot Be Changed\n");
				break;

			case 7: /* CA.COLOR */
				printf ("Input New Value: ");
				set_ca_color ( scan_uint () );
				break;

			case 8: /* CA.NB */
				printf ("Input New Value: ");
				set_ca_nb ( scan_uint () );
				break;

			case 9: /* DATA.CAPRINT */
				printf ("Input New Value: ");
				set_data_caprint ( scan_bool () );
				break;

			case 10: /* DATA.EXPORT */
				printf ("Input New Value: ");
				set_data_export ( scan_bool () );
				break;

			case 11: /* TRUTH.ROW */
				printf ("Input New Value: ");
				tt::set_row ( scan_uint () );
				break;

			case 12: /* TRUTH.F1 */
				printf ("Input New Value: ");
				tt::set_f1 ( scan_bool () );
				break;

			case 13: /* MASK */
				printf ("Input New Value: ");
				tt::set_mask ( scan_hex () );
				break;

			default:
				printf ("Invalid input: %d\n", var);
				break;

		}
	}
}

void inspect (uint8_t *const *const grid, const uint8_t *const seed) {
	/* Fail Conditions */
	if ( tt::table_is_init () == 0) {
		printf (ANSI_RED "No truth table defined.\n" ANSI_RESET);
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

	printf (ANSI_REVRS "\n\tDNA Inspection\n" ANSI_RESET);

	/* Enter DNA String in given base / LSB format */
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

	/* Checks input length validity */
	if ( strlen ( (char *)buffer ) != length) {
		printf (ANSI_RED "DNA length mismatch. Given length was %u\n" ANSI_RESET,
			(unsigned int) strlen ( (char *)buffer ));
		return;
	}

	/* Temporary array for this function */
	uint8_t *const dna = (uint8_t *) calloc (length, sizeof (uint8_t));
	memcpy (dna, buffer, length);

	/* Checks DNA string validity & Converts ASCII to uint8_t */
	bool invalid_flag = 0;
	for (unsigned int i = 0 ; i < length ; i++) {
		/* Adjusts ASCII input into uint8_t [0,9] */
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
		free (dna);
		return;
	} else {
		printf (ANSI_GREEN "\nDNA string OK\n" ANSI_RESET);
	}



	/* ===== FPGA SET GRID && EVALUATE ===== */

	ca_gen_grid (grid, dna, seed);
	ca_gen_grid (grid, dna);
	fpga_set_grid (grid);

	if ( tt::get_f1() == 1 ) {
		eval_f1_insp ( tt::get_input(), tt::get_output(), tt::get_row(), tt::get_mask() );
	} else {
		eval_bc_insp ( tt::get_input(), tt::get_output(), tt::get_row(), tt::get_mask() );
	}



	/* ===== PRINT CA GRID ===== */

	printf ("\n\e[100m\t\t-- Generated Logic Circuit --" ANSI_RESET "\n");

	/* Print Seed Row */
	ca_print_row (seed);

	/* Print CA First Pass */
	ca_gen_grid (grid, dna, seed);
	ca_print_grid (grid);

	/* Print Center Marker */
	printf ("\e[103;30m");
	for (unsigned int x = 0 ; x < get_ca_dimx() ; x++) {
		if (x % 4 == 0) {
			printf ("%X", x / 4 % 16);
		} else {
			putchar ('+');
		}
	}
	printf (ANSI_RESET "\n");

	/* Print CA Second Pass */
	ca_gen_grid (grid, dna);
	ca_print_grid (grid);

	/* Print Bottom Marker */
	printf ("\e[103;30m");
	for (unsigned int x = 0 ; x < get_ca_dimx() ; x++) {
		if (x % 4 == 0) {
			printf ("%X", x / 4 % 16);
		} else {
			cout << "+";
		}
	}
	printf (ANSI_RESET "\n");

	/* Flush Stream */
	cout << endl;



	/* ===== CLEANUP ===== */

	free (dna);

	return;
}



/* ========== Special Routine ========== */

void special (void) {
	unsigned int search;
	constexpr int MAX = 1000;

	printf (ANSI_REVRS "\n\t>> Special Routine <<\n" ANSI_RESET);

	printf ("What to search for: ");
	scan_uint (&search);

	set_data_caprint (0);

	int sim_flag;
	tt::set_table (search);

	for (int i = 1; i < MAX; i++) {
		printf (ANSI_REVRS "\n\tSearching for %0X | Attempt %d / %d\n" ANSI_RESET, search, i, MAX);

		sim_init ();
		sim_flag = sim_run (grid, seed);

		if (sim_flag == 1) {
			sim_export ();
			break;
		} else if (sim_flag == -1) {
			printf ("Simulation Failed.\n");
			break;
		}
	}

	printf (ANSI_REVRS "\n\t>> Special Routine Done <<\n\n ANSI_RESET");
}
