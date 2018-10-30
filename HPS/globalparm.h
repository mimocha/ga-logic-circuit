/* Global Parameters Header File */

#ifndef GLOBALPARM_H
#define GLOBALPARM_H

/* ----- Global Parameter Definitions ----- */

/*	Totally arbitrary version number
	Should be updated every commit.
*/
#define VERSION 1.04

/*	Max Cellular Automaton Grid Size
	This defines the maximum available length of a side of the square grid.
	Determined by the FPGA file at compile time.
	Split into the X and Y axis respectively.
*/
#define MAX_CA_DIMX 64
#define MAX_CA_DIMY 64
/*	Max Cellular Automaton Color Count
	This should be defined based on the actual FPGA cell's state count.
	This should also be limited based on memory and index integer for a CA rule set.

	For a given CA of K Colors and N neighbors, there would be K^N rules for a rule set.
	EG. a 4-color, 3-neighbor CA would have 64 rules. This rule set would have a range of [0,63]

	If using an unsigned 16-bit integer to index the rule set, you could handle up to 65535 rules.
	This integer could handle up to [K=40, N=3] => 64000 rules
	So when changing MAX_CA_COLOR, keep in mind that the rule count grows exponentially.

	Integer Range Cheat Sheet:
	unsigned 8-bit int: [0,255]
	unsigned 16-bit int: [0,65535]
	unsigned 32-bit int: [0,4294967295]
	unsigned 64-bit int: [0,18446744073709551615]
*/
#define MAX_CA_COLOR 4
/*	Max Cellular Automaton Neighbor Cell Count
	Similarly to the the above variable, this should be defined based on memory and index integer.

	This variable would also cause the length of the rule set to grow exponentially.

	A length of a CA rule set is given by K^N; where K = color count, N = neighbor count
	Refer to the above comment for more info on memory and index integer limit.
*/
#define MAX_CA_NB 10
/* Max Genetic Algorithm Population Size */
#define MAX_GA_POP 10000
/* Max Genetic Algorithm Generations */
#define MAX_GA_GEN 10000

/*	Fitness Graph Dimension
	These two variables determine the size of the graph, printed to stdout in ASCII.
	This prints in colored ASCII using the Linux API if possible.

	Default is:
		GXDIM 80
		GYDIM 40
*/
#define GXDIM 80 // Fitness Graph X-Dimensions
#define GYDIM 40 // Fitness Graph Y-Dimensions

/* ----- Global Variable Parameters ----- */ // Sue me

/* TODO: Make parameter linked list? *Low Priority*
	Possibly more flexible handling of parameters
	ie. Automatically numbered parameter list, automatically managed
*/

/* Genetics Algorithm Parameters Struct */
struct param_ga {
	unsigned int POP = 100;	/* Maximum Population */
	unsigned int GEN = 100;	/* Maximum Generation */
	float MUTP = 0.05;		/* Mutation Probability in decimal, range [0.00, 1.00] */
	unsigned int POOL = 5;	/* Tournament Selection Poolsize */
};

/* Cellular Automaton Parameters Struct */
struct param_ca {
	/* X-Axis Dimension */
	unsigned int DIMX = MAX_CA_DIMX;
	/* Y-Axis Dimension */
	unsigned int DIMY = MAX_CA_DIMY;
	/* CA Color Count */
	unsigned int COLOR = MAX_CA_COLOR;
	/* CA Neighbor Count */
	unsigned int NB = MAX_CA_NB;
	/* CA Grid Seed Array */
	uint8_t *SEED;
};

/* Data Parameters Struct */
struct param_data {
	/* Fitness Data Tracking | Default = 1
		Toggles whether or not to track the fitness data across the experiment.
		Tracks Minimum, Maximum, Average, and Median fitness value from each generation.
		Results are plotted automatically at the end of the experiment.
	*/
	bool TRACK = 1;
	/* Performance and Time Tracking | Default = 1
		Tracks computation time required during the experiment.
	*/
	bool TIME = 1;
	/* Cellular Automaton Grid Print | Default = 1
		Prints the CA Grid of the fittest individual at the end of the experiment.
	*/
	bool CAPRINT = 0;
	/* Export data from the experiment to file | (Off for now)*/
	bool EXPORT = 0;
};

/* Simulation Statistics Struct
	This struct keeps some settings and results of the most recent simulation.
	Keeps a copy, so even if the settings are changed after one simulation,
	the data could be used to replicate and export results.
*/
struct stats_var {
	/* Simulation Generation Limit */
	unsigned int gen;
	/* Simulation Population Size */
	unsigned int pop;
	/* X-Axis Dimension */
	unsigned int dimx;
	/* Y-Axis Dimension */
	unsigned int dimy;
	/* CA Color Count */
	unsigned int color;
	/* CA Neighbor Count */
	unsigned int nb;

	/* Array of stats across each generations */
	float *avg;
	float *med;
	unsigned int *max;
	unsigned int *min;
};

/* Target Truth Table Struct
	Contains the desired truth table for evaluating circuits.

	Definition  | Variable 1 | Variable 2 | Variable 3 ...
	------------+------------+------------+------------
	Config 1	| Input    1 | Output A 1 | Output B 1
	Config 2	| Input    2 | Output A 2 | Output B 2
	Config 3	| Input    3 | Output A 3 | Output B 3

	Note that each variable can be set to anything. It is not limited that Input must be variable 1, or to have only a single input, etc.
	Same goes for the number of configurations. A complete truth table is not required.

	Should be set in main-menu, before simulations.
*/
struct truth_table {
	/* Number of variables to check */
	unsigned int var = 2;
	/* Number of configurations to check */
	unsigned int config = 2;
	/* Truth Table Array -- value [var][config] */
	uint64_t **table;
};

/*	Main Global Parameters Struct
	Struct for containing all the different types of global variables we will need.
	Sub-types of global variables are defined within other structs, and declared within this struct.

	Done this way to hopefully keep variables organized.
*/
struct param_main {
	param_ga GA;
	param_ca CA;
	param_data DATA;

	stats_var stats;
	truth_table truth;

	/* FPGA Initialization Flag */
	bool fpga_init = 0;
	/* Simulation Run Checking Flag
		Variable for checking whether or not a simulation has been ran.
		0 = No simulation has been ran
		1 = Simulation ran successfully
*/
	bool run_check = 0;
	/* Truth Table Initialization Flag */
	bool tt_init = 0;
} global;

#endif
