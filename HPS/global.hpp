/* Global Parameters Header File
	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

#ifndef GLOBAL_HPP
#define GLOBAL_HPP

/* ----- Global Parameter Definitions ----- */

/*	Totally arbitrary version number
	Semantic Versioning -- https://semver.org/
	Should be updated every commit.
*/
#define VERSION "1.15.00"

/* Physical FPGA Cell Array Dimension */
#define PHYSICAL_DIMX 64
#define PHYSICAL_DIMY 64

/* Minimum Cell Array Dimension */
#define MIN_DIMX 1
#define MIN_DIMY 1

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
#define MAX_CA_COLOR 64
#define MIN_CA_COLOR 2

/*	Max Cellular Automaton Neighbor Cell Count
	Similarly to the the above variable, this should be defined based on memory and index integer.

	This variable would also cause the length of the rule set to grow exponentially.

	A length of a CA rule set is given by K^N; where K = color count, N = neighbor count
	Refer to the above comment for more info on memory and index integer limit.
*/
#define MAX_CA_NB 9
#define MIN_CA_NB 1

/* Max Genetic Algorithm Population Size */
#define MAX_GA_POP 12800
#define MIN_GA_POP 10

/* Max Genetic Algorithm Generations */
#define MAX_GA_GEN 128000
#define MIN_GA_GEN 1

/* Max Genetic Algorithm Mutation Rate (Decimal) */
#define MAX_GA_MUTP 1
#define MIN_GA_MUTP 0

/* Max Genetic Algorithm Pool Size
	Sets the max GA pool size as the current GA POP size.
	A level of safety net -- Setting POOL > POP may cause unknown errors.

	This net could be easily be broken by setting the POOL size first,
	then reducing the size of POP, to smaller than POOL.
*/
#define MAX_GA_POOL GA.POP
#define MIN_GA_POOL 1

/* Estimated Number of Individuals Evaluated per Second */
#define INDV_PER_SEC 430



/* ========== Settings & Global Variable Handler Namespace ========== */

namespace GlobalSettings {

	/* ========== Getter Functions ========== */

	unsigned int get_ga_pop (void);
	unsigned int get_ga_gen (void);
	float get_ga_mutp (void);
	unsigned int get_ga_pool (void);

	unsigned int get_ca_dimx (void);
	unsigned int get_ca_dimy (void);
	unsigned int get_ca_color (void);
	unsigned int get_ca_nb (void);

	bool get_data_caprint (void);
	bool get_data_export (void);

	unsigned int get_dna_length (void);

	/* ========== Setter Functions ========== */

	void set_ga_pop (const unsigned int &set_val);
	void set_ga_gen (const unsigned int &set_val);
	void set_ga_mutp (const float &set_val);
	void set_ga_pool (const unsigned int &set_val);

	// Although unused now, left in for future usage
	void set_ca_dimx (const unsigned int &set_val);
	void set_ca_dimy (const unsigned int &set_val);
	void set_ca_color (const unsigned int &set_val);
	void set_ca_nb (const unsigned int &set_val);

	void set_data_caprint (const bool &set_val);
	void set_data_export (const bool &set_val);

};

#endif
