/* Main C++ file for Genetic Algorithm Program.
	Compiled using C++11 standards. -std=c++11
*/
#include "ga-main.h" // Header definition
#include "ga-class.h" // Class definition
#include "cellgen.h" // Cellular Automaton

void init (int argc, char **argv);
option_args handling (string const& inString);
void Help ();
void track_fitness (GeneticAlgorithm *pop, const uint16_t gen);
void disp_fitness ();

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

		// 5: Optionals
		if (SHOW_F) { // Fitness Tracking
			track_fitness (pop, gen);
		}
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
			printf("%ld Clicks | %f Seconds | ETA X Seconds\n",
			time_gen, ((float)time_gen)/CLOCKS_PER_SEC);
		}
	}
	// ----- End Generation Loop ----- //
	printf("\n\n <----- %d Generations Simulated -----> \n", GEN_LIM);
	printf(" %d individuals lived and died\n", uid_counter);
	if (SHOW_F) {
		disp_fitness();
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

// ----- GA Main Functions ----- //

void init (int argc, char **argv) {
	// Input Argument Handlings
	if (argc > 1) {
		for (int idx=1; idx<argc; idx++) {
			switch ( handling(argv[idx]) ) {
				case help:
					Help();
					break;
				case show_c:
					SHOW_C = 1;
					break;
				case show_f:
					SHOW_F = 1;
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
			}
		}
	} else {
		// Help();
	}

	// Initializes RNG
	srand(time(NULL));

	// Initializes Seed CA
	seed[DIM/2] = 1;

	printf("COLOR = %d | POPULATION = %d | GENERATION LIMIT = %d\n"
		"CA GRAPH = %d | FITNESS = %d | TIMER = %d | RESULTS = %d | DEBUG = %d\n",
		K, POP, GEN_LIM, SHOW_C, SHOW_F, SHOW_T, SHOW_R, SHOW_D);
	puts("\n\n----- Genetic Algorithm v0.5 Initialized -----\n\n");
}

option_args handling (std::string const& inString) {
	// Argument handling functions and enum
	if (inString == "help") return help;
	if (inString == "-h") return help;
	if (inString == "--help") return help;
	if (inString == "-c") return show_c;
	if (inString == "-f") return show_f;
	if (inString == "-t") return show_t;
	if (inString == "-r") return show_r;
	if (inString == "-d") return show_d;
	return null;
}

void Help () {
	// Displays help message
	puts(" --- Genetics Algorithm Program Help Message --- ");
	puts(" '-h' '--help' 'help' Help -> Shows this message");
	puts(" '-c' CA Graph -> Graphs Cellular Automaton array");
	puts(" '-f' Fitness Tracking -> Tracks fitness value of the population");
	puts(" '-t' Timer -> Times the program execution and display the result");
	puts(" '-r' Results -> Display the results of Genetic Algorithm");
	puts(" '-d' Debug -> Display debug prints at various points");
	printf("\n\n Please select atleast 1 option to run \n\n");

	exit(EXIT_SUCCESS);
}

void track_fitness (GeneticAlgorithm *pop, const uint16_t gen) {
	// Handles zero-index and generation starting at 1
	uint16_t idx = gen - 1;

	// Maximum Fitness of each generation
	maxfit[idx] = pop[0].getfit();
	// Minimum Fitness of each generation
	minfit[idx] = pop[POP-1].getfit();
	// Median Fitness (Doesn't get average of 2 middle indv, but close enough)
	medfit[idx] = pop[POP/2].getfit();
	// Mean Fitness
	avgfit[idx] = 0;
	for (int i=0; i<POP; i++) {
		avgfit[idx] += pop[i].getfit();
	}
	avgfit[idx] /= POP;

	printf("Max: %u | Min: %u | Median: %u | Avg: %u \n",
	maxfit[idx], minfit[idx], medfit[idx], avgfit[idx]);
}

void disp_fitness () {
	puts("\n\t --- Fitness Statistics --- ");
	puts("  Gen | Maximum | Median | Minimum | Average");
	puts("--------------------------------------------");
	// Fitness Statistics Table
	for (int i=0; i<GEN_LIM; i++) {
		printf(" %4u | %7u | %6u | %7u | %7u \n",
		i+1, maxfit[i], medfit[i], minfit[i], avgfit[i]);
	}

	// Prints pretty fitness function graph, because procrastination
	puts("\n\t --- Fitness Graph ---   ");

	int x_range = GEN_LIM;
	int y_range = *max_element(maxfit, maxfit+GEN_LIM)
	 - *min_element(minfit, minfit+GEN_LIM);
	int y_max = *max_element(maxfit, maxfit+GEN_LIM);

	float x_scale = (float)GXDIM/x_range; // Scale > 1 == Stretch
	float y_scale = (float)GYDIM/y_range; // Scale < 1 == Compress

	printf("Scale XY: %f %f\n", x_scale, y_scale);

	unsigned char graph[GYDIM][GXDIM] = {0};
	for (int y=0; y<GYDIM; y++) {
		for (int x=0; x<GXDIM; x++) {
			// Load value
			if (maxfit[x] == y_max-y) graph[y][x] += 1;
			if (medfit[x] == y_max-y) graph[y][x] += 2;
			if (minfit[x] == y_max-y) graph[y][x] += 4;
			if (avgfit[x] == y_max-y) graph[y][x] += 8;

			// Convert
			switch (graph[y][x]) {
				case 0:
					graph[y][x] = 0;
					break;
				case 1:
					graph[y][x] = 43;
					break;
				case 2:
					graph[y][x] = 109;
					break;
				case 4:
					graph[y][x] = 45;
					break;
				case 8:
					graph[y][x] = 42;
					break;
				default:
					graph[y][x] = 176;
					break;
			}
		}
	}



	// --- X Scaling --- //
	// Bin as in histogram bins
	// int bin_cnt = ceil((float)x_scale*GEN_LIM); // Bin count
	// int bin_sze = ceil((float)1/x_scale); // Bin size
	// unsigned int scaled_val[4][bin_cnt] = {0};

	// if (x_scale < 1) {
	// 	// X is compressed
	// 	for (int i=0; i<bin_cnt; i++) { // loop over each bin
	// 		for (int j=0; j<bin_sze; j++) { // sum for each bin
	// 			scaled_val[0][i] += maxfit[i*bin_sze+j];
	// 			scaled_val[1][i] += medfit[i*bin_sze+j];
	// 			scaled_val[2][i] += minfit[i*bin_sze+j];
	// 			scaled_val[3][i] += avgfit[i*bin_sze+j];
	// 		}
	// 		// take average
	// 		scaled_val[0][i] /= bin_sze;
	// 		scaled_val[1][i] /= bin_sze;
	// 		scaled_val[2][i] /= bin_sze;
	// 		scaled_val[3][i] /= bin_sze;
	// 	}
	//
	//
	// }

	// Printing
	for (int y=0; y<GYDIM; y++) {
		if (y < GYDIM-1) { // print graph edge
			printf("  |");
		} else {
			printf("  +");
		}
		for (int x=0; x<GXDIM; x++) { // print graph value
			if (y < GYDIM-1) { // graph edge
				cout << graph[y][x];
			} else {
				cout << "-";
			}
		}
		cout << endl;
	}

}
