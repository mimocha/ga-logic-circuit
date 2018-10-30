/* Main C++ file for this Genetic Algorithm program. */

#include "ga-main.hpp"	/* Standard Includes & Function Prototypes */
#include "globalparm.h"	/* Global Parameters */
#include "misc.hpp"		/* Miscellaneous Functions */
#include "ga-class.hpp"	/* Genetics Algorithm Class */
#include "ca.hpp"		/* Cellular Automaton Functions */
#include "fpga.hpp"		/* FPGA Related Functions */
#include "sim.hpp"		/* Simulation Function Wrapper */

using namespace std;

/* Initializes Global UID Counter */
uint32_t GeneticAlgorithm::object_count = 0;

int main (int argc, char **argv) {
	input_argument (argc, argv);

	/* Variable for selecting menu option */
	static unsigned int sel;

	global.fpga_init = fpga_init ();

	/* Main Menu */
	while (1) {
		cout << "\033[0m";
		sel = main_menu ();

		switch (sel) {
			case 0: /* Exit */
				printf ("Exiting Program.\n");
				cleanup ();
				return EXIT_SUCCESS;
			case 1: /* About */
				help_message ();
				break;
			case 2: /* Settings */
				settings ();
				break;
			case 3: /* Initialize FPGA */
				global.fpga_init = fpga_init ();
				break;
			case 4: /* Run Simulation */
				global.run_check = run_sim ();
				break;
			case 5: /* Display Previous Results */
				results ();
				break;
			case 6: /* FPGA Circuit Verification */
				fpga_verify ();
				break;
			case 7: /* Test One Individual DNA */
			case 8: /* Set CA */
			default: /* Invalid Input */
				printf ("Invalid input: %d\n", sel);
		}
	}
}

/* ----- Other Handling Functions ----- */

unsigned int main_menu (void) {
	unsigned int var;

	/* Prints option list */
	printf ("\n\t>---- Main Menu ----<\n"
			"\t0. Exit Program\n"
			"\t1. About\n"
			"\t2. Settings\n"
			"\t3. Initialize FPGA | %d\n"
			"\t4. Run Simulation\n"
			"\t5. View Results | %d\n"
			"\t6. Verify FPGA\n"
			"\n"
			"Waiting for Input: ",
		(int)global.fpga_init, (int)global.run_check);

	/* Sanitized Scan */
	scan_uint (&var);

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
			global.DATA.TRACK, global.DATA.TIME, global.DATA.CAPRINT, global.DATA.EXPORT
		);

		/* Sanitized Scan */
		scan_uint (&var);

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
			case 9: /* global.DATA.TRACK */
				printf ("Input New Value: ");
				scan_bool (&global.DATA.TRACK);
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
			default:
				printf ("Invalid input: %d\n", var);
				break;
		}
	}
}

void results (void) {
	if (global.run_check == 0) {
		printf ("No simulation results.\n");
		return;
	}

	puts ("\n\t>>--- Simulation Results ---<<");
	puts ("  Gen | Maximum | Minimum | Median | Average");
	puts ("--------------------------------------------");
	for (uint32_t i=0; i<global.stats.gen; i++) {
		printf(" %4u | %7u | %7u | %6.1f | %7.1f \n", i + 1,
		global.stats.max [i], global.stats.min [i],
		global.stats.med [i], global.stats.avg [i]);
	}

	return;
}

void cleanup (void) {
	if (global.fpga_init == 1) close (fd);

	if (global.run_check == 1) {
		if (global.stats.avg != NULL) free (global.stats.avg);
		if (global.stats.med != NULL) free (global.stats.med);
		if (global.stats.max != NULL) free (global.stats.max);
		if (global.stats.min != NULL) free (global.stats.min);
	}


}
