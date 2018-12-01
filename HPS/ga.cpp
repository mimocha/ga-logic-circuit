/* Genetics Algorithm Class Functions File
	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

/* ========== Standard Library Include ========== */

#include <stdio.h>		// Standard I/O
#include <stdlib.h>		// calloc, free
#include <stdint.h>		// uint definitions
#include <math.h>		// floor
#include <algorithm>	// sort
#include <vector>		// vectors
#include <iostream>		// cout



/* ========== Custom File Header ========== */

#include "ga.hpp"
#include "ansi.hpp"
#include "global.hpp"
#include "fast.hpp"



using namespace std;
using namespace GlobalSettings;

/* Max Probability int
	Set this to some number large enough to express the probability range sufficiently.
	Mutation probability 'mutp' is a float of range [0, 1.0]
	mutp is multiplied with a random number, then modulo by this number.
	A number too small could cause the program to become faulty.
	But a number too large would be computationally expensive.
*/
#define P_MAX 100000

// Global Object Counter for UID
static uint32_t object_count = 0;



// =====================================================
// GENETIC ALGORITHM CLASS METHODS
// =====================================================

/* ========== Compare Functions ========== */

bool GeneticAlgorithm::compfit_descend (const GeneticAlgorithm &a, const GeneticAlgorithm &b) {
	return (a.fit > b.fit);
}

bool GeneticAlgorithm::compfit_ascend (const GeneticAlgorithm &a, const GeneticAlgorithm &b) {
	return (a.fit < b.fit);
}



/* ========== Constructors ========== */

GeneticAlgorithm::GeneticAlgorithm (void) {
	uid = 0;
	dna = nullptr;
	fit = 0;
	age = 0;
	eval = 0;
	alive = 1;
	sol = 0;
}

GeneticAlgorithm::GeneticAlgorithm (const uint32_t &dna_length) {
	uid = object_count;
	dna = GeneticAlgorithm::dna_calloc (dna_length);
	GeneticAlgorithm::dna_rand_fill (dna, dna_length);
	fit = 0;
	age = 0;
	eval = 0;
	alive = 1;
	sol = 0;
	object_count++;
}

void GeneticAlgorithm::FreeDNA (void) {
	// free (this->dna);
	delete [] this->dna;
}



/* ========== Genetic Algorithm Operations ========== */

void GeneticAlgorithm::Selection (GeneticAlgorithm *const array) {
	/* Gets these variable values once per population
		Accessing local memory is faster than a function call.
		Increases performance by almost 2x.
	*/
	const unsigned int pop = get_ga_pop ();

	/* 	========== Natural Selection ==========
		Selection is done by ranking and age.
		It is required that the population be sorted by fitness before selection.

		>> Rank-proportional Survivability
		The higher the rank of an individual, the higher chance of survival.
		Done by simple probability: p of death = rank. eg:
		Rank 1% == 1% p of death
		Rank 100% == 100% p of death

		>> Aging
		The older the individual, the lower the chance of survival.
		This heuristics hypothetically prevents the search from getting stuck in local optimas.
		As a fit candidate could be alive indefinitely, aging forces new solutions to be considered.
		This should hopefully increase the likelihood of a population getting 'unstuck'.

		The possible downside of this is the possible loss of a solution.
		As the fittest individual's trait is not guaranteed to get passed on,
		simple probability might cause a newly found solution to be lossed.
		This is still relatively unlikely, but a possibility to keep in mind.

		NOTE:
		The array index is equivalent to the rank, because the array is pre-sorted.
		We will be keeping track of the index, because it allows easy access to the original population and its properties, via array[i].
	*/

	for (unsigned int rank = 0 ; rank < pop ; rank++) {
		uint32_t rng = fast_rng32 () % pop;

		/* Checks if an individual dies
			Use the RNG to pick a number between [0,POP)
			If RNG is less than the individual's rank index, the individual dies.

			As the individual's rank index gets higher (0 is highest),
			the likelihood of death decreases p(RNG < smaller number) < p(RNG < larger number).
		*/
		if ( rng < rank ) {
			array [rank].alive = 0;
		}
	}

	return;
}

void GeneticAlgorithm::Repopulate (GeneticAlgorithm *const array) {
	/* Vector of who live or dies -- contains population array index */
	vector <uint16_t> live, dead;

	/* Gets the variable values once -- per individual */
	const unsigned int pop = get_ga_pop ();
	const unsigned int pool = get_ga_pool ();
	const unsigned int dna_length = get_dna_length();
	const float mutp = get_ga_mutp ();
	const unsigned int color = get_ca_color ();

	/* Puts the array index of alive / dead individuals into their respective vectors  */
	for (unsigned int i = 0 ; i < pop ; i++) {
		if ( array[i].alive ) {
			live.push_back ( i );
		} else {
			dead.push_back ( i );
		}
	}



	/* ========== TOURNAMENT SELECTION ==========
		Using the results from the previous part (selecting who live & dies), we will next select who will reproduce with whom, via tournament selection strategy.

		This loop iterates until the vector 'dead' is empty
	*/

	while ( !dead.empty() ) {
		uint16_t parent [2];

		/* Tournament Selection Pool
			Iterates POOL number of times, per parent.
			In each iteration, pick a random alive individual's index number to compare.

			Start with the weakest individual as current pick.
			Generate new pick randomly.
			Compares the new pick's fitness with the current's fitness.
			Keep the pick with the higher fitness value.
			If both has equal fitness, pick the one with lower age.
			If both also has equal age, pick one by random.

			At the end of the iterations, you should have two array index numbers.
			These are the parents.

			Special case:
			If both parents happen to be the same individual (the individual won twice),
			generate a random individual entirely.
		*/
		for (unsigned int j = 0 ; j < 2 ; j++) {
			/* pcur == Current Pick | pnew == New Pick */
			uint16_t pcur = live.back();
			uint16_t pnew;

			/* Tournament Pool Loop */
			for (unsigned int i = 0 ; i < pool ; i++) {
				/* Pick a random live individual */
				pnew = live [ fast_rng32 () % live.size() ];

				/* Compare by fitness -- fittest wins */
				if ( array[pnew].fit > array[pcur].fit ) {
					/* New pick is fitter, Keep new pick */
					pcur = pnew;
				} else if ( array[pnew].fit < array[pcur].fit ) {
					/* Current pick is fitter, Keep current pick */
					// pcur = pcur; /* Do Nothing */
				} else {

					/* Compare by age -- youngest wins */
					if (array[pnew].age > array[pcur].age) {
						/* Current pick is younger, Keep current pick */
						// pcur = pcur; /* Do Nothing */
					} else if (array[pnew].age < array[pcur].age) {
						/* New pick is younger, Keep new pick */
						pcur = pnew;
					} else {

						/* Pick one by random */
						if ( fast_rng32() % 1 ) {
							/* Keep new pick */
							pcur = pnew;
						} else {
							/* Keep current pick */
							// pcur = pcur; /* Do Nothing */
						}
					}
				}
			}

			/* Assign current pick as parent */
			parent [j] = pcur;
		}

		/* Resets a dead individual */
		array[dead.back()].Reset ();

		/* Confirms two different parents */
		if ( parent [0] != parent [1] ) {
			/* Reproduce Normally */
			array[dead.back()].Crossover
			(array[parent[0]].dna, array[parent[1]].dna, dna_length);
		} else {
			/* If both picks are the same, generate new DNA randomly */
			array[dead.back()].dna_rand_fill (dna_length);
		}

		/* Mutate DNA */
		array[dead.back()].Mutate (mutp, color, dna_length);

		/* Remove new individual from list of the dead */
		dead.pop_back ();
	}

	/* Empties vector at the end */
	live.clear();
	dead.clear();
	return;
}

void GeneticAlgorithm::Crossover
(const uint8_t *const dna_a, const uint8_t *const dna_b, const unsigned int &dna_length) {
	/* Using RNG, determine which DNA comes from which parent */
	for (uint16_t i = 0 ; i < dna_length ; i++) {
		if ( (fast_rng32 () % 1) )
			this->dna [i] = dna_a [i];
		else
			this->dna [i] = dna_b [i];
	}

	return;
}

void GeneticAlgorithm::Mutate
(const float &mutp, const unsigned int &color, const unsigned int &dna_length) {
	/* Iterates over each DNA  */
	for (uint32_t i = 0 ; i < dna_length ; i++) {
		uint16_t rng = fast_rng32 () % P_MAX;

		/* Do Nothing | 1 - MUTP probability */
		if (rng > floor (mutp * P_MAX) ) {
			continue;
		}

		/* Minor Mutations | 75% of all mutations*/
		if (rng > floor (mutp * P_MAX * 0.25) ) {
			switch (rng % 5) {
				case 0:
					this->dna [i] = 0;
					break;
				case 1:
					this->dna [i] = color - 1;
					break;
				case 2:
					this->dna [i] = fast_rng32 () % color;
					break;
				case 3:
					if (this->dna [i] > 0) {
						this->dna [i] -= 1;
					}
					break;
				case 4:
					if (this->dna [i] < color - 1) {
						this->dna [i] += 1;
					}
					break;
			}
			continue;
		}

		/* Swap Mutation | 20% of all mutations */
		if (rng > floor (mutp * P_MAX * 0.05) ) {
			uint16_t dest = fast_rng32 () % dna_length;
			uint8_t tmp = this->dna [i];
			this->dna [i] = this->dna [dest];
			this->dna [dest] = tmp;
			continue;
		}

		/* Scramble (Least Likely) | 5%  of all mutation
			Picks two anchor points,
			The first is the current DNA index (i)
			The second is a random point within the DNA string (fast_rng32 () % dna_length)

		*/
		uint16_t anchor = fast_rng32 () % dna_length;
		if (i > anchor) {
			random_shuffle (&dna [anchor], &dna [i]);
		} else if (anchor > i) {
			random_shuffle (&dna [i], &dna [anchor]);
		}
	}

	return;
}



/* ========== Other Miscellany Operations ========== */

void GeneticAlgorithm::Reset (void) {
	/* TODO: DNA REALLOC - If DNA length has changed, reallocate *dna */
	uid = object_count;
	fit = 0;
	age = 0;
	eval = 0;
	alive = 1;
	sol = 0;
	object_count++;
}

uint8_t *GeneticAlgorithm::dna_calloc (const uint32_t &dna_length) {
	// Allocates memory for DNA string -- Possible Bug?
	uint8_t *dna = new (nothrow) uint8_t [dna_length] ();

	// If Memory Allocation Failed
	if (dna == NULL) {
		printf (ANSI_RED "\nERROR: DNA CALLOC FAILED\n" ANSI_RESET);
		return nullptr;
	}

	return dna;
}

void GeneticAlgorithm::dna_rand_fill (const uint32_t &dna_length) {
	// Checks if DNA is NULL
	if (this -> dna == NULL) {
		printf (ANSI_RED "\nERROR: DNA IS NULLPTR\n" ANSI_RESET);
		return;
	}

	// Fills DNA with Random Number, modulo CA Color Count
	const unsigned int ca_color = get_ca_color();
	for (uint32_t i = 0 ; i < dna_length ; i++) {
		this -> dna [i] = fast_rng32 () % ca_color;
	}

	return;
}

void GeneticAlgorithm::dna_rand_fill (uint8_t *const dna, const uint32_t &dna_length) {
	// Checks if DNA is NULL
	if (dna == NULL) {
		printf (ANSI_RED "\nERROR: DNA IS NULLPTR\n" ANSI_RESET);
		return;
	}

	// Fills DNA with Random Number, modulo CA Color Count
	const unsigned int ca_color = get_ca_color();
	for (uint32_t i = 0 ; i < dna_length ; i++) {
		dna [i] = fast_rng32 () % ca_color;
	}

	return;
}

void GeneticAlgorithm::Sort (GeneticAlgorithm *const array) {
	sort (	array, array + get_ga_pop(), compfit_descend );
	return;
}



/* ========== Print Functions ========== */

void GeneticAlgorithm::print_dna (const uint32_t &dna_length) {
	// Iterates over entire DNA length
	for (uint32_t i = 0; i < dna_length; i++) {
		// Prints DNA to terminal
		cout << (unsigned int) dna [i];
	}
}

void GeneticAlgorithm::fprint_dna (FILE *const fp, const uint32_t &dna_length) {
	// Iterates over entire DNA length
	for (uint32_t i = 0; i < dna_length; i++) {
		// Adjusts each character from uint8_t to an ASCII number
		char buff = dna [i] + 48;
		// Writes to file
		fputc (buff, fp);
	}
}



/* ========== Get Functions ========== */

uint32_t GeneticAlgorithm::get_uid (void) {
	return this -> uid;
}

uint8_t *GeneticAlgorithm::get_dna (void) {
	return this -> dna;
}

uint32_t GeneticAlgorithm::get_fit (void) {
	return this -> fit;
}

uint32_t GeneticAlgorithm::get_age (void) {
	return this -> age;
}

bool GeneticAlgorithm::get_eval (void) {
	return this -> eval;
}

bool GeneticAlgorithm::get_sol (void) {
	return this -> sol;
}



/* ========== Set Functions ========== */

void GeneticAlgorithm::set_fit (const uint32_t &set_val) {
	this -> fit = set_val;
}

void GeneticAlgorithm::set_eval (void) {
	this -> eval = 1;
}

void GeneticAlgorithm::set_age (void) {
	this -> age++;
}

void GeneticAlgorithm::set_sol (void) {
	this -> sol = 1;
}
