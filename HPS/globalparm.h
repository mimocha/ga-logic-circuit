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
	So the number of cells on a grid, would be MAX_CA_DIM squared.
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

/* ----- Variable Global Parameters ----- */

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
	float MUTP = 0.05;		/* Mutation Probability */
	unsigned int POOL = 5;	/* Tournament Selection Poolsize */
};

struct param_ca {
	unsigned int DIMX = 10;	/* X-Axis Dimension */
	unsigned int DIMY = 10;	/* Y-Axis Dimension */
	unsigned int COLOR = 4;	/* CA Color Count */
	unsigned int NB = 3;	/* CA Neighbor Count */
}

/*	Main Global Parameters Struct
	Struct for containing all the different types of global variables we will need.
	Sub-types of global variables are defined within other structs, and declared within this struct.

	Done this way to hopefully keep variables organized.
*/
struct param_main {
	param_ga GA;
} global;


// // ----- Global variables ----- // (Sue me)
// clock_t time_total, time_gen;
// uint32_t uid_counter = 0; // Unique ID for any individual created.
//
// // Stuff for input argument handling
// bool SHOW_C = 0; // Toggles Pretty CA Plots
// bool SHOW_F = 0; // Toggles Fitness Tracking
// bool SHOW_T = 0; // Toggles Performance Timing
// bool SHOW_R = 0; // Toggles Result Displays
// bool SHOW_D = 0; // Toggles Debug
//
// // Fitness tracking variables: Max, Min, Median, Mean per generation
// uint32_t maxfit[GEN_LIM], minfit[GEN_LIM], medfit[GEN_LIM], avgfit[GEN_LIM];
//
// // Internal Model of Logic Gate Array
// uint8_t LGA[DIM][DIM] = {0};
// // CA Seed array (First Row of Generation)
// uint8_t seed[DIM] = {0};

#endif
