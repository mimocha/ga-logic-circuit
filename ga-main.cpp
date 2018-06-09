/* Main C++ file for Genetic Algorithm Program.
	Controls all functions within this project.
	Expected to be replaced with Python later on.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Quartus options definition Char buffer for Windows system command.
char QMAP[] = "quartus_map"
			" --read_settings_files=on"
			" --write_settings_files=off"
			" .\\CV_GeneticAlgorithm\\GeneticAlgorithm -c GeneticAlgorithm",
	QFIT[] = "quartus_fit"
			" --read_settings_files=off"
			" --write_settings_files=off"
			" .\\CV_GeneticAlgorithm\\GeneticAlgorithm -c GeneticAlgorithm",
	QASM[] = "quartus_asm"
			" --read_settings_files=off"
			" --write_settings_files=off"
			" .\\CV_GeneticAlgorithm\\GeneticAlgorithm -c GeneticAlgorithm",
	QPGM[] = "quartus_pgm"
			" -c DE-SoC -m JTAG -o p"
			" .\\CV_GeneticAlgorithm\\GeneticAlgorithm.cdf";

void init (void);
int quartus_cmd (void);

int main (void) {
	// 0: Initialization
	init();

	// ----- Loop Generation ----- //
	// ----- Loop Speciment ----- //
	// 1: Cellular Automaton Generation

	// 2: CA to Verilog Compilation

	// 3: Compile & Load onto FPGA
	puts("\nCompile & Load\n");
	quartus_cmd();

	// 4: Evaluation
	// ----- End Speciment Loop ----- //

	// 5: Genetic Algorithm

	// ----- End Generation Loop ----- //

	printf("\nPress anything to continue\n");
	getchar();
	return 0;
}

void init () {
	puts("\nLoading ...\n");

	puts(QMAP);
	puts(QFIT);
	puts(QASM);
	puts(QPGM);

	puts("\n\n----- Genetic Algorithm v0.01 Initialized -----\n\n");
}

int quartus_cmd (void) {
	// To add error catches.
	// Return 0 or -1 when error.
	system(QMAP);
	system(QFIT);
	system(QASM);
	system(QPGM);
	return 1;
}
