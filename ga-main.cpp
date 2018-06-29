/* Main C++ file for Genetic Algorithm Program.
	Controls all functions within this project.
*/

// Custom Function Definitions
#include "ga-main.h" // Class & Header definition
#include "cellgen.h" // Cellular Automaton Generation

inline void init (void);
int quartus_cmd (void);
uint8_t dtor (uint16_t decval, uint8_t base);

int main (int argc, int *argv) {
	// 0: Initialization
	init();

	// ----- Loop Generation ----- //
	// ----- Loop Speciment ----- //.
	// 1: Cellular Automaton Generation

	uint8_t seed[DIM] = {0};
	seed[DIM/2] = 1;

	// Randomly creates rule
	uint8_t rule[K_CUBE];
	for (int r = 0; r < 3; r++) {
		for (int i = 0; i < K_CUBE; i++) {
			rule[i] = rand() % K;
			printf(" %d", rule[i]);
		}
		cout << endl;
		cellgen(seed, rule);
	}


	// cout << (unsigned char) 7;

	// 2: CA to Verilog Compilation

	// 3: Compile & Load onto FPGA
	// puts("\nCompile & Load\n");
	// time(&tstart);
	// quartus_cmd();
	// time(&tend);

	// 4: Evaluation

	// ----- End Speciment Loop ----- //

	// 5: Genetic Algorithm

	// ----- End Generation Loop ----- //

	// printf("\n\n----- Performance Results -----\n"
	// 	"Start Time: %s \n"
	// 	"End Time: %s \n"
	// 	"Total Compile Time: %4.0f seconds\n"
	// 	"QMAP Time: %3.0f seconds\n"
	// 	"QFIT Time: %3.0f seconds\n"
	// 	"QASM Time: %3.0f seconds\n"
	// 	"QPGM Time: %3.0f seconds\n",
	// 	ctime(&tstart), ctime(&tend), difftime(tpgm, tstart),
	// 	difftime(tmap, tstart), difftime(tfit, tmap),
	// 	difftime(tasm, tfit), difftime(tpgm, tasm));
	return 0;
}

inline void init () {
	puts("\nLoading ...\n");

	// Initializes RNG
	srand(time(NULL));

	// puts(QMAP);
	// puts(QFIT);
	// puts(QASM);
	// puts(QPGM);

	puts("\n\n----- Genetic Algorithm v0.10 Initialized -----\n\n");
}

int quartus_cmd (void) {
	int i;

	// Quartus command returns 0 on success; not 0 otherwise
	i = system(QMAP);
	time(&tmap);
	if (i != 0) { // Error Catch
		printf("\n Error at command:\n %s\n System returns: %d\n", QMAP, i);
		// system("pause");
		return -1;
	}
	i = system(QFIT);
	time(&tfit);
	if (i != 0) { // Error Catch
		printf("\n Error at command:\n %s\n System returns: %d\n", QFIT, i);
		// system("pause");
		return -1;
	}
	i = system(QASM);
	time(&tasm);
	if (i != 0) { // Error Catch
		printf("\n Error at command:\n %s\n System returns: %d\n", QASM, i);
		// system("pause");
		return -1;
	}
	i = system(QPGM);
	time(&tpgm);
	if (i != 0) { // Error Catch
		printf("\n Error at command:\n %s\n System returns: %d\n", QPGM, i);
		// system("pause");
		return -1;
	}

	return 0;
}

uint8_t dtor (uint16_t decval, uint8_t base=0) {
	return 0;
}
