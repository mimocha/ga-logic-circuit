/* Simulation Wrapper Header File */
/* Simulation Functions seperated in its own file to keep ga-main clean. */

#ifndef SIM_HPP
#define SIM_HPP

using namespace std;

/* static void sim_init (void)
	Restricted to "sim.cpp" to prevent confusion with other initialization functions.
	Created this function because the initialization codes are getting out of hand.
*/
static void sim_init (void);

/* bool run_sim (void)
	Wrapper for running the simulation.
	Returns bool to indicate sim results.

	1 = SUCCESS
*/
bool run_sim (void);

/* void print_grid (uint8_t **grid)
	Prints the working array grid onto terminal.
*/
void print_grid (uint8_t **grid);

/* uint64_t bitcount64 (uint64_t x)
	64-bit Hamming Weight Counting Algorithm
	https://en.wikipedia.org/wiki/Hamming_weight

	Counts how many bits are set.
	Input (Expected XNOR Observed) as value to count correct bits.
*/
uint64_t bitcount64 (uint64_t x);

/* void statistics (GeneticAlgorithm *array, const unsigned int gen)
	Stores statistics of the current simulation.
*/
void statistics (GeneticAlgorithm *array, const unsigned int gen);

#include "sim.cpp"

#endif
