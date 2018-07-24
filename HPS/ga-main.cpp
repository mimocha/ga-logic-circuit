/* Main C++ file for Genetic Algorithm Program.
	Compiled using C++11 standards. -std=c++11
*/
#include "ga-main.h" // Header & Global Parameters
#include "ga-class.h" // Class definition
#include "misc.h" // Miscellany Functions
#include "ca.h" // Cellular Automaton

int main (int argc, char **argv) {
	// 0: Initialization
	init(argc, argv);
	// Create new population
	GeneticAlgorithm pop[POP];

	if (SHOW_T) {
		// Start Overall timer
		time_total = clock ();
	}

	// ----- Generation Loop ----- //
	for (uint16_t gen=1; gen<=GEN_LIM; gen++) {
		if (SHOW_T) {
			// Start generation timer
			time_gen = clock ();
		}
		printf("\t --- Generation %3d --- \n", gen);

		// 1: Genetic Algorithm Fuctions
		ga.Selection(pop);

		// ----- Speciment Loop ----- //
		for (uint16_t idx=0; idx<POP; idx++) {
			// Skips individual if already evaluated (from previous generation)
			if (pop[idx].geteval()) continue;

			// 2: Cellular Automaton Generation
			cellgen (seed, pop[idx].getdna());
			// Bounce Back
			cellgen (LGA[DIM-1], pop[idx].getdna());

			// 3: Edit Memory of FPGA RAM


			// 4: Evaluation
			uint16_t sum = 0;
			for (int y=0; y<DIM; y++) {
				for (int x=0; x<DIM; x++) {
					sum += LGA[y][x];
				}
			}

			pop[idx].setfit (sum);
			pop[idx].Eval ();
		}
		// ----- End Speciment Loop ----- //

		ga.Sort(pop);

		// 5: Optionals
		if (SHOW_C) { // CA Graphing
			ca_graph (pop, 1);
		}
		if (SHOW_T) { // Performance
			disp_time (time_gen, 0);
		}
		if (SHOW_F) { // Fitness Tracking
			track_fitness (pop, gen);
		}
		if (SHOW_R) { // Results
			disp_results (pop);
		}
	}
	// ----- End Generation Loop ----- //

	printf("\n\n\t <----- %d Generations Simulated -----> \n", GEN_LIM);
	printf("\t %8d individuals lived and died\n", uid_counter);

	if (SHOW_F) {
		disp_fitness();
	}
	if (SHOW_T) {
		disp_time (time_total, 1);
	}

	return EXIT_SUCCESS;
}
