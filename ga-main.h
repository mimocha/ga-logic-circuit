/* Header file for Genetics ALgorithm Main Program.
	Contains all class definitions for Genetic Algorithm.
*/

#ifndef GAMAIN_H_INCLUDED
#define GAMAIN_H_INCLUDED

// Standard Library Definition
#include "stdio.h" // Standard library
#include "stdlib.h" // String to integers
#include "string.h" // String manipulation
#include "iostream" // cin / cout
#include "stdint.h" // uint definitions
// #include "bitset" // Bit manipulation
// #include "math.h" // Exponential functions
#include "time.h" // Timing & performance

// Namespace
using namespace std;

// Parameter Definitions
#define DIM 50 // Cell Array dimension
#define K 4 // Numbers of color represented in CA
#define K_CUBE K*K*K // Predefined K^3 for speed? (Sorry...)

// Timer for tracking performance
time_t tstart, tmap, tfit, tasm, tpgm, tend;

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


// Class Definitions?
class Individual {
public:
	unsigned short gene, id;
	float fitness;

	int counteval(void);
	float calcfitness(void);
	void randinit(void);
	void nullinit(void);

private:
	bool elite_flag, eval_flag;
	unsigned short age;

	// Initialize this individual as child of some parent individuals
	// unsigned short crossover(Individual *parent_a, Individual *parent_b),
	// void mutate(Individual *self.gene[]), // Mutation function
	// void update(Individual *self.gene[]); // Updating various internal members

};

// float Individual::calcfitness(void)
// unsigned short Individual::crossover(Individual *parent_a, Individual *parent_b)
// void Individual::mutate(Individual *self.gene)



// Class for handling arrays of individuals
// Possibly for handling multiple simultaneous populations
class Population {
public:
	unsigned short id, count;
};

#endif
