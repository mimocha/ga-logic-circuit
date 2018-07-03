/* Main C++ file for Genetic Algorithm Program.
	Compiled using C++11 standards. -std=c++11
*/
#include "ga-main.h" // Header definition
#include "ga-class.h" // Class definition
#include "cellgen.h" // Cellular Automaton

void init (int argc, char **argv);
option_args handling (string const& inString);
void Help ();

// Main program
int main (int argc, char **argv) {
	// 0: Initialization
	init(argc, argv);
	if (SHOW_T) time_total = clock();

	// Create new population
	GeneticAlgorithm pop[POP];

	// ----- Loop Generation ----- //
	for (uint16_t gen=1; gen<=GEN_LIM; gen++) {
		if (SHOW_T) time_gen = clock();
		printf("\n --- Generation %3d --- \n", gen);

		// 1: Genetic Algorithm Fuctions
		ga.Selection(pop);

		// ----- Loop Speciment ----- //
		for (uint16_t idx=0; idx<POP; idx++) {
			// 2: Cellular Automaton Generation
			cellgen(seed, pop[idx].getdna());

			// 3: Edit Memory of FPGA RAM

			// 4: Evaluation
			pop[idx].Eval();
		}
		// ----- End Speciment Loop ----- //
		ga.Sort(pop);

		// 5: (Optional) Display Results
		if (SHOW_R) { // Results
			puts(" RANK |   UID   | FITNESS | DNA");
			for (int i=0; i<POP; i++) {
				printf(" %4d | %7d | %7d | ",
				pop[i].getrnk(), pop[i].getuid(), pop[i].getfit());
				uint8_t *output = pop[i].getdna();
				for (int j=0; j<K_CUBE; j++) {
					if (output[j] < 16) {
						printf("%X", output[j]);
					} else {
						cout << 'X';
					}
					if (j >= 64) {
						cout << " ... ";
						break;
					}
				}
				cout << endl;
				if (i >= 10) {
					printf(" .\n .\n .\n");
					break;
				}
			}
		}
		if (SHOW_T) { // Performance
			time_gen = clock() - time_gen;
			printf("%ld Clicks | %f Seconds \n",
			time_gen, ((float)time_gen)/CLOCKS_PER_SEC );
		}
	}
	// ----- End Generation Loop ----- //
	if (SHOW_R) {
		printf("\n\n <----- %d Generations Simulated -----> \n", GEN_LIM);
		printf("\t %d individuals lived and died\n", uid_counter);
	}
	if (SHOW_T) {
		time_total = clock() - time_total;
		cout << endl;
		printf("\t Performance: %ld Clicks | %f Seconds \n",
		time_total, ((float)time_total)/CLOCKS_PER_SEC );
		printf("\t Average %f Secs per generation\n", ((float)time_total)/CLOCKS_PER_SEC/GEN_LIM);
	}

	return EXIT_SUCCESS;
}

void init (int argc, char **argv) {
	// Input Argument Handlings
	if (argc > 1) {
		for (int idx=1; idx<argc; idx++) {
			switch ( handling(argv[idx]) ) {
				case help:
					Help();
					break;
				case show_g:
					SHOW_G = 1;
					break;
				case show_t:
					SHOW_T = 1;
					break;
				case show_r:
					SHOW_R = 1;
					break;
				case show_d:
					SHOW_D = 1;
					break;
				default:
					break;
					// do nothing
			}
		}
	}

	// Initializes RNG
	srand(time(NULL));

	// Initializes Seed CA
	seed[DIM/2] = 1;

	printf("COLOR = %d | POPULATION = %d | GENERATION LIMIT = %d | "
		"GRAPH = %d | TIMER = %d | RESULTS = %d | DEBUG = %d\n",
		K, POP, GEN_LIM, SHOW_G, SHOW_T, SHOW_R, SHOW_D);
	puts("\n\n----- Genetic Algorithm v0.4 Initialized -----\n\n");
}

option_args handling (std::string const& inString) {
	// Argument handling functions and enum
	if (inString == "help") return help;
	if (inString == "-g") return show_g;
	if (inString == "-t") return show_t;
	if (inString == "-r") return show_r;
	if (inString == "-d") return show_d;
	return null;
}

void Help () {
	// Displays help message
	puts("Genetics Algorithm Main Program");
	puts(" '-g' Graph -> Graphs Cellular Automaton array");
	puts(" '-t' Timer -> Times the program execution and display the result");
	puts(" '-r'	Results -> Display the results of Genetic Algorithm");
	puts(" '-d'	Debug -> Display debug prints at various points");

	exit(EXIT_SUCCESS);
}
