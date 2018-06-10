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
			" .\\fpga\\fpga -c fpga_main",
	QFIT[] = "quartus_fit"
			" --read_settings_files=off"
			" --write_settings_files=off"
			" .\\fpga\\fpga -c fpga_main",
	QASM[] = "quartus_asm"
			" --read_settings_files=off"
			" --write_settings_files=off"
			" .\\fpga\\fpga -c fpga_main",
	QPGM[] = "quartus_pgm"
			" -c DE-SoC -m JTAG -o p"
			" .\\fpga\\output_files\\fpga_main.cdf";

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

	// printf("\nPress anything to continue\n");
	// getchar();
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
	int i;

	// Quartus command returns 0 on success; not 0 otherwise
	i = system(QMAP);
	if (i != 0) { // Error Catch
		printf("\n Error at command:\n %s\n System returns: %d\n", QMAP, i);
		// system("pause");
		return -1;
	}
	i = system(QFIT);
	if (i != 0) { // Error Catch
		printf("\n Error at command:\n %s\n System returns: %d\n", QFIT, i);
		// system("pause");
		return -1;
	}
	i = system(QASM);
	if (i != 0) { // Error Catch
		printf("\n Error at command:\n %s\n System returns: %d\n", QASM, i);
		// system("pause");
		return -1;
	}
	i = system(QPGM);
	if (i != 0) { // Error Catch
		printf("\n Error at command:\n %s\n System returns: %d\n", QPGM, i);
		// system("pause");
		return -1;
	}

	return 0;
}
