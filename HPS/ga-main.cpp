/* Main C++ file for this Genetic Algorithm program.
*/

#include "ga-main.h"	/* Standard Includes & Function Prototypes */
#include "globalparm.h"	/* Global Parameter Header */
#include "misc.h"		/* Miscellaneous Functions Header */

using namespace std;

int main (int argc, char **argv) {
	input_argument (argc, argv);

	/* Main Menu */
	static unsigned int sel;
	while (1) {
		sel = mainmenu ();
		switch (sel) {
			case 0: // About / Help
				help_message ();
				break;
			case 1: // Run Experiment
				run_sim ();
				break;
			case 2: // Settings
				settings ();
				break;
			case 3: // Display Previous Results
				printf ("Display Results\n");
				break;
			case 4: // Exit
				printf ("Exiting Program.\n");
				return EXIT_SUCCESS;
			default:
				printf ("Invalid input: %d\n", sel);
		}
	}
}

void input_argument (const int argc, char **argv) {
	int counter;

	printf("%d input argument(s) given:\n", argc);
	for (counter=0; counter<argc; counter++)
		printf("argv[%d] : %s\n", counter, argv[counter]);


}

unsigned int mainmenu (void) {
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

int run_sim () {

	printf (	"\n\t>>>-- Beginning Simulation --<<<\n"
				"\tPOP = %4u | GEN = %4u | MUT = %0.4f | POOL = %4u\n",
			global.GA.POP, global.GA.GEN, global.GA.MUTP, global.GA.POOL);
	// wrapper for calling all the other functions

	return EXIT_SUCCESS;
}

void settings (void) {
	unsigned int var;

	while (true) {
		/* Prints option list */
		printf ("\n\t>>--- Settings ---<<\n"
				"\t0. Exit\n"
				"\t1. Max Population\t| Current Value: %u\n"
				"\t2. Max Generation\t| Current Value: %u\n"
				"\t3. Mutation Prob\t| Current Value: %f\n"
				"\t4. Pool Size\t\t| Current Value: %u\n\n"
				"Waiting for Input: ",
			global.GA.POP, global.GA.GEN, global.GA.MUTP, global.GA.POOL);

		/* Sanitized Scan */
		scan_uint (&var);

		/* Prints message if input not valid */
		if (var > 5) {
			printf ("Invalid input: %d\n", var);
		}

		switch (var) {
			case 0:
				printf ("Returning to main menu\n");
				return;
			case 1:
				printf ("Input New Value: ");
				scan_uint (&global.GA.POP);
				break;
			case 2:
				printf ("Input New Value: ");
				scan_uint (&global.GA.GEN);
				break;
			case 3:
				printf ("Input New Value: ");
				scan_float (&global.GA.MUTP);
				break;
			case 4:
				printf ("Input New Value: ");
				scan_uint (&global.GA.POOL);
		}
	}
}

/* ----- Sanitized Scanf ----- */
