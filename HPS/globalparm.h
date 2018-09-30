/* Global Parameters Header File */

#ifndef GLOBALPARM_H
#define GLOBALPARM_H

/* ----- Global Parameter Definitions ----- */

/*	Totally arbitrary version number
	Should be updated every commit.
*/
#define VERSION 0.98

/* TODO: Implement upper / lower bounds for global variables? */

/*	Max Cellular Automaton Grid Size
	This defines the maximum length of a side of the square grid.
	Split into the X and Y axis respectively.
*/
#define MAX_CA_DIMX 100
#define MAX_CA_DIMY 100
/*	Max Cellular Automaton Color (State) Count
	This should be defined based on memory and index integer for a CA rule set.

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
#define MAX_CA_COLOR 40
/*	Max Cellular Automaton Neighbor Cell Count
	Similarly to the the above variable, this should be defined based on memory and index integer.

	This variable would also cause the length of the rule set to grow exponentially.

	A length of a CA rule set is given by K^N; where K = color count, N = neighbor count
	Refer to the above comment for more info on memory and index integer limit.
*/
#define MAX_CA_NB 10
#define MAX_GA_POP 10000
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

/* ----- Variable Global Parameters ----- */ // Sue me

/* TODO: Make parameter linked list? *Low Priority*
	Possibly more flexible handling of parameters
	ie. Automatically numbered parameter list, automatically managed
*/
// typedef struct param {
// 	unsigned int idx;
//
//
// 	struct param *next;
// 	struct param *prev;
// } global;

/* Genetics Algorithm Parameters Struct */
struct param_ga {
	unsigned int POP = 100;	/* Maximum Population */
	unsigned int GEN = 100;	/* Maximum Generation */
	float MUTP = 0.05;		/* Mutation Probability in decimal, range [0.00, 1.00] */
	unsigned int POOL = 5;	/* Tournament Selection Poolsize */
};

/* Cellular Automaton Parameters Struct */
struct param_ca {
	unsigned int DIMX = 10;	/* X-Axis Dimension */
	unsigned int DIMY = 10;	/* Y-Axis Dimension */
	unsigned int COLOR = 4;	/* CA Color Count */
	unsigned int NB = 3;	/* CA Neighbor Count */
};

/* Data Parameters Struct */
struct param_data {
	/* Fitness Data Tracking | Default = 1
		Toggles whether or not to track the fitness data across the experiment.
		Tracks Minimum, Maximum, Average, and Median fitness value from each generation.
		Results are plotted automatically at the end of the experiment.
	*/
	bool FIT = 1;
	/* Performance and Time Tracking | Default = 1
		Tracks computation time required during the experiment.
	*/
	bool TIME = 0;
	/* Cellular Automaton Grid Print | Default = 1
		Prints the CA Grid of the fittest individual at the end of the experiment.
	*/
	bool CAPRINT = 0;
	/* Export data from the experiment to file | (Off for now)*/
	bool EXPORT = 0;
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
} global;


// // ----- Global variables ----- //
// clock_t time_total, time_gen;
// uint32_t uid_counter = 0; // Unique ID for any individual created.
//
// // Fitness tracking variables: Max, Min, Median, Mean per generation
// uint32_t maxfit[GEN_LIM], minfit[GEN_LIM], medfit[GEN_LIM], avgfit[GEN_LIM];
//
// // Internal Model of Logic Gate Array
// uint8_t LGA[DIM][DIM] = {0};
// // CA Seed array (First Row of Generation)
// uint8_t seed[DIM] = {0};

#endif
