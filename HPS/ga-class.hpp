/* Header file for Genetics Algorithm Class
	Contains the class definition and functions protoypes.

	A single instance of class GeneticAlgorithm is referred to as an individual.
	A collection of individuals is referred to as a population.
*/

#ifndef GACLASS_HPP
#define GACLASS_HPP

using namespace std;

class GeneticAlgorithm {
/* Everything is public //
	Bad form, but this must do for now. Finishing the dissertation comes first.
	If time permits, reintroduce Get() and Set(), along with privacy controls.
*/
public:
	static uint32_t object_count;

	uint32_t uid;	/* Unique id for tracking the individual */
	uint8_t *dna;	/* DNA sequence (Cellular Automaton Rule) */
	int fit;		/* Fitness score of the individual */
	uint32_t rank;	/* Fitness Ranking of the individual */
	uint32_t age;	/* For how long the individual has been alive for */
	bool eval;		/* Evaluation Flag | 0 = not evaluated | 1 = evaluated */

	/* ----- Constructors & Destructors ----- */

	/* Default Constructor */
	GeneticAlgorithm (void) {}
	/* Single Object Constructor //
		Initializes a single individual, given a DNA length
	*/
	GeneticAlgorithm (const uint32_t dna_length);
	/* Destructor //
		A custom destructor will cause bugs, and segmentation faults.
		Probably due to the destructor being called automatically by the compiler at some point,
		this breaks the program and will likely cause a segmentation fault.

		Thus, memory is freed manually at the end of run_sim(), in "sim.cpp".
		This destructor is left here for future references.
	*/
	// ~GeneticAlgorithm (void);

	/* ----- Genetic ALgorithm Operations ----- */

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

		Static allows this method to be called, even with no objects.
	*/
	static void Selection (GeneticAlgorithm *array);
	/* CROSSOVER (const uint8_t *DNA_A, const uint8_t DNA_B)
		Homogeneously crossesover two parents' dna string.
		Equal likelyhood any certain dna character will be chosen.
		Equal split between two parents is not guaranteed.
	*/
	void Crossover (const uint8_t *dna_a, const uint8_t *dna_b);
	/* MUTATE (void)
		Mutates the dna of a given individual.
		Iterates over every dna chunk, and compares a random number to a set probability.
	*/
	void Mutate (void);

	/* ----- Other Miscellany Operations ----- */

	/* RESET (void)
		Resets an individual, and assigns new UID.
		For after an individual 'dies', and is being replaced by a new individual.

		TODO: DNA REALLOC - If DNA length has changed, reallocate *dna
	*/
	void Reset (void);
	/* SORT (GeneticAlgorithm *array)
		Sorts the entire population by fitness value, in decreasing order.
		Static allows this method to be called, even with no objects.
	*/
	static void Sort (GeneticAlgorithm *array);
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

	/* ----- Set Functions ----- */

	/* SETDNA (unsigned int arg_count, ...)
		Takes an unsigned int of number of arguments to consider,
		and a comma seperated list of uint8_t to set as the DNA.
		Requires <stdarg.h>
	*/
	void setdna (unsigned int arg_count, ...);

};

#include "ga-class.cpp"

#endif
