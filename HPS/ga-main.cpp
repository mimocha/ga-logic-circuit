/* Main C++ file for this Genetic Algorithm program. */

#include "ga-main.hpp"	/* Standard Includes & Function Prototypes */
#include "globalparm.h"	/* Global Parameters */
#include "misc.hpp"		/* Miscellaneous Functions */
#include "ga-class.hpp"	/* Genetics Algorithm Class */
#include "ca.hpp"		/* Cellular Automaton Functions */
#include "sim.hpp"		/* Simulation Function Wrapper */

using namespace std;

/* Initializes Global UID Counter */
uint32_t GeneticAlgorithm::object_count = 0;

int main (int argc, char **argv) {
	input_argument (argc, argv);

	/* Variable for selecing */
	static unsigned int sel;
	/* static int run_check //
		Variable for checking whether or not a simulation has been ran.
		Also for checking the status of any simulation ran.

		 0 = No simulation has been ran
		 1 = Simulation ran successfully
		-1 = Simulation failed or aborted
	*/
	static int run_check = 0;

	/* Main Menu */
	while (1) {
		sel = main_menu ();

		switch (sel) {
			case 0: /* About */
				help_message ();
				break;
			case 1: /* Run Simulation */
				run_check = run_sim ();
				break;
			case 2: /* Settings */
				settings ();
				break;
			case 3: /* Display Previous Results */
				results (run_check);
				break;
			case 4: /* Exit */
				printf ("Exiting Program.\n");
				return EXIT_SUCCESS;
			default: /* Invalid Input */
				printf ("Invalid input: %d\n", sel);
		}
	}
}

/* ----- Other Handling Functions ----- */

void input_argument (const int argc, char **argv) {
	int counter;

	printf ("%d input argument(s) given:\n", argc);
	for (counter=0; counter<argc; counter++)
		printf ("argv[%d] : %s\n", counter, argv[counter]);
}

unsigned int main_menu (void) {
	unsigned int var;

	/* Prints option list */
	printf ("\n\t>---- Main Menu ----<\n"
			"\t0. About / Help\n"
			"\t1. Begin Simulation\n"
			"\t2. Settings\n"
			"\t3. Display Results\n"
			"\t4. Exit Program\n\n"
			"Waiting for Input: ");

	/* Sanitized Scan */
	scan_uint (&var);

	/* Prints message if input not valid */
	if (var > 5) {
		printf ("Invalid input: %d\n", var);
	}

	return var;
}

void settings (void) {
	unsigned int var;

	while (true) {
		/* Prints option list */
		printf (
			"\n\t>>--- Settings ---<<\n"
			"\t0. Back\n"
			"\t1. GA Max Population\t| Current Value: %u\n"
			"\t2. GA Max Generation\t| Current Value: %u\n"
			"\t3. GA Mutation Prob\t| Current Value: %.3f\n"
			"\t4. GA Pool Size\t\t| Current Value: %u\n"
			"\t5. CA X Axis Dimension\t| Current Value: %u\n"
			"\t6. CA Y Axis Dimension\t| Current Value: %u\n"
			"\t7. CA Color Count\t| Current Value: %u\n"
			"\t8. CA Neighbor Count\t| Current Value: %u\n"
			"\t9. DATA Fitness Track\t| Current Value: %u\n"
			"\t10. DATA Time Track\t| Current Value: %u\n"
			"\t11. DATA CA Print\t| Current Value: %u\n"
			"\t12. DATA Export\t\t| Current Value: %u\n\n"
			"Waiting for Input: ",
			global.GA.POP, global.GA.GEN, global.GA.MUTP, global.GA.POOL,
			global.CA.DIMX, global.CA.DIMY, global.CA.COLOR, global.CA.NB,
			global.DATA.FIT, global.DATA.TIME, global.DATA.CAPRINT, global.DATA.EXPORT
		);

		/* Sanitized Scan */
		scan_uint (&var);

		/* Prints message if input not valid */
		if (var > 12) {
			printf ("Invalid input: %d\n", var);
		}

		/* Changing from a simple struct of global parameters to a linked list could fix this.
			Displaying lists and rearranging could be done more flexibly.
			But it is too much of a hassle for now, dissertation must come first.
		*/
		switch (var) {
			case 0: /* Back */
				printf ("Returning to main menu\n");
				return;
			case 1: /* global.GA.POP */
				printf ("Input New Value: ");
				scan_uint (&global.GA.POP);
				if (global.GA.POP > MAX_GA_POP) {
					global.GA.POP = MAX_GA_POP;
					printf ("Value capped at: %u\n", global.GA.POP);
				} else if (global.GA.POP == 0) {
					global.GA.POP = 1;
					printf ("Minimum value: %u\n", global.GA.POP);
				}
				break;
			case 2: /* global.GA.GEN */
				printf ("Input New Value: ");
				scan_uint (&global.GA.GEN);
				if (global.GA.GEN > MAX_GA_GEN) {
					global.GA.GEN = MAX_GA_GEN;
					printf ("Value capped at: %u\n", global.GA.GEN);
				} else if (global.GA.GEN == 0) {
					global.GA.GEN = 1;
					printf ("Minimum value: %u\n", global.GA.GEN);
				}
				break;
			case 3: /* global.GA.MUTP */
				printf ("Input New Value: ");
				scan_float (&global.GA.MUTP);
				break;
			case 4: /* global.GA.POOL */
				printf ("Input New Value: ");
				scan_uint (&global.GA.POOL);
				break;
			case 5: /* global.CA.DIMX */
				printf ("Input New Value: ");
				scan_uint (&global.CA.DIMX);
				if (global.CA.DIMX > MAX_CA_DIMX) {
					global.CA.DIMX = MAX_CA_DIMX;
					printf ("Value capped at: %u\n", global.CA.DIMX);
				} else if (global.CA.DIMX == 0) {
					global.CA.DIMX = 1;
					printf ("Minimum value: %u\n", global.CA.DIMX);
				}
				break;
			case 6: /* global.CA.DIMY */
				printf ("Input New Value: ");
				scan_uint (&global.CA.DIMY);
				if (global.CA.DIMY > MAX_CA_DIMY) {
					global.CA.DIMY = MAX_CA_DIMY;
					printf ("Value capped at: %u\n", global.CA.DIMY);
				} else if (global.CA.DIMY == 0) {
					global.CA.DIMY = 1;
					printf ("Minimum value: %u\n", global.CA.DIMY);
				}
				break;
			case 7: /* global.CA.COLOR */
				printf ("Input New Value: ");
				scan_uint (&global.CA.COLOR);
				if (global.CA.COLOR > MAX_CA_COLOR) {
					global.CA.COLOR = MAX_CA_COLOR;
					printf ("Value capped at: %u\n", global.CA.COLOR);
				} else if (global.CA.COLOR <= 1) {
					global.CA.COLOR = 2;
					printf ("Minimum value: %u\n", global.CA.COLOR);
				}
				break;
			case 8: /* global.CA.Nb */
				printf ("Input New Value: ");
				scan_uint (&global.CA.NB);
				if (global.CA.NB == 0) {
					global.CA.NB = 1;
					printf ("Minimal value: %u\n", global.CA.NB);
				} else if (global.CA.NB % 2 == 0) {
					global.CA.NB -= 1;
					printf ("Even values not accepted. Set to %u\n", global.CA.NB);
				}
				break;
			case 9: /* global.DATA.FIT */
				printf ("Input New Value: ");
				scan_bool (&global.DATA.FIT);
				break;
			case 10: /* global.DATA.TIME */
				printf ("Input New Value: ");
				scan_bool (&global.DATA.TIME);
				break;
			case 11: /* global.DATA.CAPRINT */
				printf ("Input New Value: ");
				scan_bool (&global.DATA.CAPRINT);
				break;
			case 12: /* global.DATA.EXPORT */
				printf ("Input New Value: ");
				scan_bool (&global.DATA.EXPORT);
				break;
		}
	}
}

void results (const int run_check) {

	// Get results through another function, with static variables and such?
	// Instead of global parameters and return values from sim function

	if (run_check == 0) {
		printf ("No simulation has been ran.\n");
	}

	if (run_check == 1) {
		printf ("Simulation results.\n");
	}

	if (run_check == -1) {
		printf ("Simulation was aborted or failed. Showing partial results.\n");
	}

	return;
}
