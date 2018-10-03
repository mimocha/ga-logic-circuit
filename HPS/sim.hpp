/* Simulation Wrapper Header File */
/* Simulation Functions seperated in its own file to keep ga-main clean. */

#ifndef SIM_HPP
#define SIM_HPP

using namespace std;

/* int run_sim (void)
	Wrapper for running the simulation.
	Returns int to indicate sim results.

	 1 = SUCCESS
	-1 = FAILED or Aborted attempts
*/
int run_sim (void);

#include "sim.cpp"

#endif
