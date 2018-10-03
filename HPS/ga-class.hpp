/* Header file for Genetics Algorithm Class
	Contains the class definition and functions protoypes.

	A single instance of class GeneticAlgorithm is referred to as an individual.
	A collection of individuals is referred to as a population.
*/

#ifndef GACLASS_H_INCLUDED
#define GACLASS_H_INCLUDED

using namespace std;

class GeneticAlgorithm {

public:
	uint32_t uid;	/* Unique id for tracking the individual */
	uint8_t *dna;	/* DNA sequence (Cellular Automaton Rule) */
	int fit;		/* Fitness score of the individual */
	uint32_t rank;	/* Fitness Ranking of the individual */
	uint32_t age;	/* For how long the individual has been alive for */
	bool eval;		/* Evaluation Flag | 0 = not evaluated | 1 = evaluated */

public:
	/* ----- Constructors & Destructors ----- */

	/* Default Constructor */
	GeneticAlgorithm (void) {}
	/* Single Object Constructor //
		Initializes a single individual, given a uid number and a DNA length
	*/
	GeneticAlgorithm (const uint32_t curr_uid, const uint32_t dna_length);

	/* ----- Genetic ALgorithm Operations ----- */

	/* RESET (void)
		TODO: Fix UID & Add documentation
	*/
	void Reset (void);
	/* SORT (GeneticAlgorithm *array)
		Sorts the entire population by fitness value, in decreasing order
	*/
	void Sort (GeneticAlgorithm *array);
	/* SELECTION (GeneticAlgorithm *array)
		The likelyhood of any individual being selected, and living on, passing their genes onto the next generation, is inversely-proportional to its rank in the population. The higher the rank, the less likely they'll survive.

		Using the ( rand()%POP ), we get a random number in the range of [0, POP). We then compare this result with the rank of any given individual. If its rank is smaller than or equal to the resulting number, it survives. Thus the higher ranking individual - 1,2,3... - will have a higher chance of surviving than those of lower ranks - 70,80,90...

		Selection of who live or dies is kept in vectors "live" and "dead" respectively. Using vector instead of arrays, because the numbers for how many live or dies is undetermined (random).

		[ <----- ----- Actual Population ----- -----> ]
			VVV   VVV   VVV  (IDX)  VVV   VVV   VVV
		[ <----- LIVE -----> ]  [ <----- DEAD -----> ]
			VV (IDX) VV
		[ <- POOL 1 -> ] ---> Parents A & B ---> Offspring 1
		[ <- POOL 2 -> ] ---> Parents A & B ---> Offspring 2
		[ <- POOL 3 -> ] ---> Parents A & B ---> Offspring 3

		TODO: Apply aging to the calculation; older individuals are more likely to die off.
			Purpose: to avoid getting trapped in local optimas?
	*/
	void Selection (GeneticAlgorithm *array);
	/* CROSSOVER (const uint8_t *DNA_A, const uint8_t DNA_B)
		Homogeneously crossesover two parents' dna string.
		Equal likelyhood any certain dna character will be chosen.
		Equal split between two parents is not guaranteed.
	*/
	void Crossover (const uint8_t *dna_a, const uint8_t *dna_b);
	/* MUTATE (void)
		TODO: Refactor & Add documentation
	*/
	void Mutate (void);

private:
	/* DNA dynamic memory allocation //
		Allocates memory for the DNA sequence, for a given DNA length.

		Takes in constant unsigned 32-bit integer DNA length,

		Allocates and zero-initializes an array of unsigned characters, of length (dna_length).

		Checks whether or not memory was successfully allocated,
			throws exception if failed
			else fills the array with random number, modulo CA Color count.

		Returns the pointer to an array of unsigned chars.
	*/
	uint8_t *dna_calloc (const uint32_t dna_length);

} ga;

#include "ga-class.cpp"

#endif
