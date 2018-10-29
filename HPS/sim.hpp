/* Simulation Wrapper Header File */
/* Simulation Functions seperated in its own file to keep ga-main clean. */

#ifndef SIM_HPP
#define SIM_HPP

using namespace std;

/* bool run_sim (void)
	Wrapper for running the simulation.
	Returns bool to indicate sim results.

	1 = SUCCESS
*/
bool run_sim (void);

/* void status (const unsigned int gen)
	Status indicator function.
	Indicates current progress of simulation, at minimal cpu cost.
*/
void status (const unsigned int gen);

/* void eta (const time_t timer)
	Prints estimated time to completion
*/
void eta (const unsigned int gen, const time_t timer);

/* void print_grid (uint8_t **grid)
	Prints the working array grid onto terminal.
*/
void print_grid (uint8_t **grid);

/* uint64_t popcount64 (uint64_t x)
	64-bit Hamming Weight Counting Algorithm
	https://en.wikipedia.org/wiki/Hamming_weight
*/
uint64_t popcount64 (uint64_t x);

#include "sim.cpp"

#endif
