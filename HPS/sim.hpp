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

/* uint32_t evaluate (void)
	Wrapper for evaluating FPGA Logic Circuits
*/
uint32_t evaluate (void);

/* uint32_t evaluate_f1 (void)
	Evaluates circuit with F1 scoring instead of usual bitcount.
	More computationally expensive, but better at discriminating small changes.
*/
uint32_t evaluate_f1 (void);

/* void id_evaluate (uint8_t **grid)
	In-Depth Evaluate function.
	Evaluates the currently set CA Grid, and also prints the circuit logic & fitness values.
*/
void id_evaluate (void);

/* void id_evaluate_f1 (uint8_t **grid)
	In-Depth F1 Evaluate function.
	Evaluates the currently set CA Grid with F1 metrics,
	and also prints the circuit logic & fitness values.
*/
void id_evaluate_f1 (void);

/* void report (void)
	Wrapper function for end of simulation report.
*/
void report (void);

/* void sim_cleanup (void)
	Wrapper function for end of simulation cleanups.
	Dynamically allocated memories are freed here.
*/
void sim_cleanup (void);

/* void export (GeneticAlgorithm *array = NULL) */
bool export_rpt (GeneticAlgorithm *array = NULL);

/* ===== Simulation File Level Global Variables =====
	Ignores the need to pass variables around.
	Should make program easier to understand.

	> Encapsulate These Variables
	Set as Static, limits scope to this compilation unit only.
*/

/* Renames variables, for easier reference */
static unsigned int dimx;
static unsigned int dimy;
static unsigned int gen_lim;
static unsigned int pop_lim;

/* Computes DNA length, the number of CA rules;
	Calculated by -> Color^Neighbor (exponent, not XOR)
*/
static uint32_t dna_length;

/* Maxmimum fitness possible
	Calculated as: (Output bit width) * (Truth Table Steps)
*/
static uint32_t fit_lim;

/* Timer Variable
	WARNING: Possible Timer Overflow
	Overflowed @ 300 pop, 5000 gen
	Switched from clock () to time ().
	Might be fixed now.
*/
static time_t timer_begin;
static time_t timer_end;

/* Time Estimate Variable */
static float time_est;

/* Genetic Algorithm Population Array */
static GeneticAlgorithm *indv;

#include "sim.cpp"

#endif
