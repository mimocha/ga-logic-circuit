/* Header file for miscellany functions
	Separated from ga-main to keep code clean.
*/

#ifndef MISC_H_INCLUDED
#define MISC_H_INCLUDED

#include "misc.cpp"

// ----- Initialization Functions ----- //

/* option_args handling (string const& inString)
	This function handles input options.
	It takes in input of type strings, and output type enum "option_args".

	Done this way to use switch case in the function init().
	Also to mess with whomever is using this program after me. (sue me)
*/
option_args handling (string const& inString);

/* void Msg (void)
	This function displays a short help message, if the program is executed with no input arguments.
	The program terminates at the end of this function.
*/
void Msg (void);

/* void Help (void)
	This function displays an in-depth message explaining how to use this program.
*/
void Help (void);

/* void init (int argc, char **argv)
	Initialization function. Handles all the input arguments.

	Need to fix this to allow setting population, generation, and other params.
*/
void init (int argc, char **argv);

// ----- Tracker Functions ----- //

/* void track_fitness (GeneticAlgorithm *pop, const uint16_t gen)
	Takes in a population and the current generation number.

	Using the input generation as the index number,
	tracks the statistics of fitness of the input population.

	This function currently tracks:
		mean, median, maximum, and minimum fitness for each generation.
	The tracked values are saved in their own global 1-D arrays.
		These are defined in the ga-main.h header.
*/
void track_fitness (GeneticAlgorithm *pop, const uint16_t gen);

/* void disp_fitness (void)
	This function displays the tracked fitness values.
	This is presumably done only once, at the end of the program, after the experiment.

	This function first prints out a table containing all tracked values
	Included is the fancy function of shortening a long table.

	This function then creates a scaled graph in ASCII and outputs to terminal.
	The graph is automatically scaled to whatever size it is set to.
	The graph size is defined in ga-main.h as GXDIM and GYDIM.
	Recommended size for the graph is: GXDIM 80 and GYDIM 40
	Size is determined in characters.
*/
void disp_fitness (void);

/* void disp_time (clock_t timer, const int mode)
	This function measures and displays time.
	The timer must be started manually outside the function with: timer = clock();

	Mode is used to select display mode, with there being 2 modes.
		Mode 0: displays # of clicks from timer start to function call (now).
				displays # secs (calculated) from timer start to now.
		Mode 1: displays the same things as Mode 1,
				displays the average # of clicks and secs per Generations.
*/
void disp_time (clock_t timer, const int mode);

/* void disp_results (GeneticAlgorithm *pop)
	This function shows the status of the top 10 individuals of a population.
	Displays the RANK, UID, FITNESS, and DNA.

	The DNA string is shortened if longer than 64 characters.
*/
void disp_results (GeneticAlgorithm *pop);

#endif
