/* Header File for Simulation Functions
	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

#ifndef SIM_HPP
#define SIM_HPP

/* ========== Simulation Functions ========== */

/* void sim_init (void)
	Simulation Initialization.
	Checks prerequisites such as FPGA initialization and Truth Table, exits if failed.
	Sets local copies of current settings.
	Frees and Allocates memory for GA class and stats tracking.
	Sets sim_init_flag = 1 at end of successful initialization.
*/
void sim_init (void);

/* void sim_cleanup (void)
	Clears allocated memories
	Done before initializing simulations
*/
void sim_cleanup (void);

/* int sim_run (uint8_t *const *const grid, const uint8_t *const seed)
	Main simulation wrapper function.
	Requires a working array and a seed array as input.

	Working array 'grid' is of dimension (DIMY x DIMX)
	Seed array 'seed' is of dimension (DIMX)

	Returns simulation status at end of simulation.
	-1 = Simulation Failed (sim_init_flag == 0)
	0 = Simulation Successful, No Solutions Found
	1 = Simulation Successful, Solutions Found
*/
int sim_run (uint8_t *const *const grid, const uint8_t *const seed);

// Function for continuing a population run?
// int sim_optimize (uint8_t *const *const grid, const uint8_t *const seed);

/* bool sim_is_done (void)
	Returns bool of whether sim is completed.
	0 == No sim ran (successfully)
	1 == sim ran successfully
*/
bool sim_is_done (void);

/* static unsigned int count_solution (const GeneticAlgorithm *const array)
	Simple function to count how many solutions exist in the population.
	Solutions are defined as individuals with maximum fitness values.

	Maximum fitness value 'fit_lim' required to be set correctly.

	Returns the number of solutions within the current population.
*/



/* ========== Results & Reporting Function ========== */

/* static void statistics (const GeneticAlgorithm *const array, const unsigned int &gen)
	Statistics tracking function.
	Tracks statistical data: Maximum, Minimum, Median, and Average values of fitness.
*/

/* static void report (uint8_t *const *const grid, const uint8_t *const seed)
	Wrapper for results printing at the end of the simulation.
	Prints pretty tables and CA grid.
	Also inspects the top individual, and print its truth table.
*/

/* void sim_export (void)
	Exports simulation data to file.
	Saves file as current date & time
*/
void sim_export (void);

/* bool export_is_done (void)
	Returns bool of whether export has been done for the most recent simulation.
	0 == No export done
	1 == Export done
*/
bool export_is_done (void);



#endif
