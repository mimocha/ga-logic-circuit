/* Main C++ File for Global Parameters
	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

/* ========== Standard Library Include ========== */

#include <stdio.h>		// Standard I/O



/* ========== Custom File Header ========== */

#include "global.hpp"
#include "ansi.hpp"
#include "fast.hpp"



/* ========== Global Parameters & Variable Struct ==========
	Kept in structs to group relevant parameters together.
*/

// Genetics Algorithm Parameters
struct param_ga {
	// Maximum Population
	unsigned int POP = 100;
	// Maximum Generation
	unsigned int GEN = 100;
	// Mutation Probability in decimal, range [0.000, 1.000]
	float MUTP = 0.05;
	// Tournament Selection Poolsize
	unsigned int POOL = 5;
};

// Cellular Automaton Parameters
struct param_ca {
	// X-Axis Dimension
	unsigned int DIMX = PHYSICAL_DIMX;
	// Y-Axis Dimension
	unsigned int DIMY = PHYSICAL_DIMY;
	// CA Color Count
	unsigned int COLOR = 4;
	// CA Neighbor Count
	unsigned int NB = 3;
};

// Data Parameters
struct param_data {
	// Cellular Automaton Grid Print
	bool CAPRINT = 1;
	// Export data from the experiment to file
	bool EXPORT = 0;
	// Print report at end of each simulation
	bool REPORT = 1;
};

// Declaration of Each Struct
static param_ga GA;
static param_ca CA;
static param_data DATA;

// DNA Length Variable
static unsigned int dna_length = fast_pow (CA.COLOR, CA.NB);

/* ========== Boundary Function ==========
	Compares the input value with the upper and lower boundaries.
	If the input exceeds either boundary, returns the exceeded boundary.
	Else, returns the input.
*/

static unsigned int bound (const unsigned int &, const unsigned int &, const unsigned int &);

unsigned int bound
(const unsigned int &input, const unsigned int &upper, const unsigned int &lower) {
	// Checks if input exceeds upper or lower boundary
	if (input > upper) {
		printf (ANSI_YELLOW "Max Value: %u\n" ANSI_RESET, upper);
		return upper;
	} else if (input < lower) {
		printf (ANSI_YELLOW "Min Value: %u\n" ANSI_RESET, lower);
		return lower;
	}

	// Input is within set boundaries
	return input;
}

static float bound (const float &, const float &, const float &);

float bound (const float &input, const float &upper, const float &lower) {
	// Checks if input exceeds upper or lower boundary
	if (input > upper) {
		printf (ANSI_YELLOW "Max Value: %.3f\n" ANSI_RESET, upper);
		return upper;
	} else if (input < lower) {
		printf (ANSI_YELLOW "Min Value: %.3f\n" ANSI_RESET, lower);
		return lower;
	}

	// Input is within set boundaries
	return input;
}




/* ========== Update Function ==========
	For updating certain internal variables that depends upon other settings.
	Should reduce time spent on certain more expensive calculations.
*/

static void update (void);

void update (void) {
	dna_length = fast_pow (CA.COLOR, CA.NB);
}



/* ========== Getter Functions ========== */

unsigned int GlobalSettings::get_ga_pop (void) {
	return GA.POP;
}

unsigned int GlobalSettings::get_ga_gen (void) {
	return GA.GEN;
}

float GlobalSettings::get_ga_mutp (void) {
	return GA.MUTP;
}

unsigned int GlobalSettings::get_ga_pool (void) {
	return GA.POOL;
}


unsigned int GlobalSettings::get_ca_dimx (void) {
	return CA.DIMX;
}

unsigned int GlobalSettings::get_ca_dimy (void) {
	return CA.DIMY;
}

unsigned int GlobalSettings::get_ca_color (void) {
	return CA.COLOR;
}

unsigned int GlobalSettings::get_ca_nb (void) {
	return CA.NB;
}


bool GlobalSettings::get_data_caprint (void) {
	return DATA.CAPRINT;
}

bool GlobalSettings::get_data_export (void) {
	return DATA.EXPORT;
}

bool GlobalSettings::get_data_report (void) {
	return DATA.REPORT;
}


unsigned int GlobalSettings::get_dna_length (void) {
	return dna_length;
}



/* ========== Setter Functions ========== */

void GlobalSettings::set_ga_pop (const unsigned int &set_val) {
	GA.POP = bound (set_val, MAX_GA_POP, MIN_GA_POP);

	/* Special Requirement for GA POOL
		GA POOL Should not be larger than GA POP.
		POOL > POP may cause unknown error.
		Warning Thrown if POOL > POP
	*/
	if (GA.POOL > GA.POP) {
		printf (ANSI_YELLOW "WARNING: POOL larger than POP.\n" ANSI_RESET);
	}

	return;
}

void GlobalSettings::set_ga_gen (const unsigned int &set_val) {
	GA.GEN = bound (set_val, MAX_GA_GEN, MIN_GA_GEN);
	return;
}

void GlobalSettings::set_ga_mutp (const float &set_val) {
	GA.MUTP = bound (set_val, MAX_GA_MUTP, MIN_GA_MUTP);
	return;
}

void GlobalSettings::set_ga_pool (const unsigned int &set_val) {
	GA.POOL = bound (set_val, MAX_GA_POOL, MIN_GA_POOL);
	return;
}


void GlobalSettings::set_ca_dimx (const unsigned int &set_val) {
	CA.DIMX = bound (set_val, PHYSICAL_DIMX, MIN_DIMX);
	return;
}

void GlobalSettings::set_ca_dimy (const unsigned int &set_val) {
	CA.DIMY = bound (set_val, PHYSICAL_DIMY, MIN_DIMY);
	return;
}

void GlobalSettings::set_ca_color (const unsigned int &set_val) {
	CA.COLOR = bound (set_val, MAX_CA_COLOR, MIN_CA_COLOR);

	/* Updates internal variables */
	update ();
	return;
}

void GlobalSettings::set_ca_nb (const unsigned int &set_val) {
	CA.NB = bound (set_val, MAX_CA_NB, MIN_CA_NB);

	/* Special Requirement for CA Neighbor
		Even-numbered CA neighbor count not accepted yet.
		Throws warning & automatically fixes value.
	*/
	if (CA.NB % 2 == 0) {
		CA.NB -= 1;
		printf (ANSI_YELLOW "WARNING: Even values unacceptable. Set to %u\n" ANSI_RESET, CA.NB);
	}

	/* Updates internal variables */
	update ();
	return;
}


void GlobalSettings::set_data_caprint (const bool &set_val) {
	DATA.CAPRINT = set_val;
	return;
}

void GlobalSettings::set_data_export (const bool &set_val) {
	DATA.EXPORT = set_val;
	return;
}

void GlobalSettings::set_data_report (const bool &set_val) {
	DATA.REPORT = set_val;
	return;
}
