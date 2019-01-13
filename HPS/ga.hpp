/* Header File for Genetics Algorithm Class
	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

#ifndef GACLASS_HPP
#define GACLASS_HPP

class GeneticAlgorithm {

private:

	/* ========== Properties ========== */

	// Unique id for tracking the individual
	uint32_t uid;
	// DNA sequence (Cellular Automaton Rule)
	uint8_t *dna;
	// Generated Circuit for each individual
	uint8_t **grid;
	// Fitness score of the individual
	uint32_t fit;
	// Gate Efficiency Score := (Max Gate - Gates Used) ; Higher is better
	uint16_t gate;
	// For how long the individual has been alive for
	uint32_t age;
	// Evaluation Flag | Use to skip redundant evaluations
	bool eval;
	// Alive Flag | 0 = Dead | 1 = Alive
	bool alive;
	// Solution Flag | 0 = Not a solution | 1 = Is a solution
	bool sol;


	/* ========== Compare Functions ========== */

	/* static bool compfit_descend (const GeneticAlgorithm &a, const GeneticAlgorithm &b)
		Helper function for Sort(). Sorts by fitness in descending order.
	*/
	static bool compfit_descend (const GeneticAlgorithm &a, const GeneticAlgorithm &b);

	/* static bool compfit_ascend (const GeneticAlgorithm &a, const GeneticAlgorithm &b)
		Helper function for Sort(). Sorts by fitness in ascending order.
	*/
	static bool compfit_ascend (const GeneticAlgorithm &a, const GeneticAlgorithm &b);


	/* ========== Genetic Algorithm Operations ========== */

	/* void Crossover
		(const uint8_t *const dna_a, const uint8_t *const dna_b, const unsigned int &dna_length)

		Homogeneously crosses over two parents' dna string.
		Equal likelyhood any certain dna character will be chosen.
		Equal split between two parents is not guaranteed.
	*/
	void Crossover
	(const uint8_t *const dna_a, const uint8_t *const dna_b, const unsigned int &dna_length);

	/* void Mutate
		(const float &mutp, const unsigned int &color, const unsigned int &dna_length)

		Mutates the dna of a given individual.
		Iterates over every dna chunk, and compares a random number to a set probability.
	*/
	void Mutate
	(const float &mutp, const unsigned int &color, const unsigned int &dna_length);


	/* ========== Other Miscellany Operations ========== */

	/* void Reset (void)
		Resets an individual, and assigns new UID.
		Use after an individual 'dies', and is being replaced by a new individual.

		TODO: DNA REALLOC - If DNA length has changed, reallocate *dna
	*/
	void Reset (void);

	/* uint8_t *dna_calloc (const uint32_t &dna_length)
		DNA dynamic memory allocation
		Allocates memory for the DNA sequence, for a given DNA length.

		Takes a reference to an constant unsigned 32-bit int 'DNA length'.
		Allocates and zero-initializes an array of unsigned characters,
		of length (dna_length).

		Checks whether or not memory was successfully allocated,
		throws error if failed.

		Returns the pointer to an array of unsigned chars.
	*/
	uint8_t *dna_calloc (const uint32_t &dna_length);

	/* void dna_rand_fill (const uint32_t &dna_length)
		Fills the DNA of the 'this' object with random numbers, modulo CA Color Count.
		Prints error and does nothing if DNA is a nullptr.
	*/
	void dna_rand_fill (const uint32_t &dna_length);

	/* void dna_rand_fill (uint8_t *const dna, const uint32_t &dna_length)
		Fills the provided DNA with random numbers, modulo CA Color Count.
		Prints error and does nothing if DNA is a nullptr.
	*/
	void dna_rand_fill (uint8_t *const dna, const uint32_t &dna_length);

	/**/
	uint8_t **grid_calloc (void);

	/**/
	void grid_gen (const uint8_t *const seed);

public:

	/* ========== Constructors ========== */

	/* Default Constructor */
	GeneticAlgorithm (void);

	/* Single Object Constructor
		Initializes a single individual, given a DNA length
	*/
	GeneticAlgorithm (const uint32_t &dna_length);

	/* void Free (void)
		Frees allocated memory for DNA strings & CA Grid
	*/
	void Free (void);


	/* ========== Genetic Algorithm Operations ========== */

	/* static void Selection (GeneticAlgorithm *const array)
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
	static void Selection (GeneticAlgorithm *const array);

	/* static void Repopulate (GeneticAlgorithm *const array, const uint8_t *const seed)
		Using Tournament Selection Method,
		choose two parents to procreate and replace a dead individual.

		Splits the population into two vector lists, 'live' and 'dead'.
		Iterates until all 'dead' individuals are replaced by new offspring.

		>> Tournament Selection Pool
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
	static void Repopulate (GeneticAlgorithm *const array, const uint8_t *const seed);


	/* ========== Other Miscellany Operations ========== */

	/* static void Sort (GeneticAlgorithm *array)
		Sorts the entire population by fitness value, in decreasing order.
	*/
	static void Sort (GeneticAlgorithm *array);


	/* ========== Print Functions ========== */

	/* void print_dna (const uint32_t &dna_length)
		Prints the DNA string of the individual, given a DNA length
	*/
	void print_dna (const uint32_t &dna_length);

	/* fprint_dna (FILE *const fp, const uint32_t &dna_length)
		Prints the DNA string of the individual to FILE, given a DNA length
	*/
	void fprint_dna (FILE *const fp, const uint32_t &dna_length);


	/* ========== Get Functions ========== */

	uint32_t get_uid (void);

	uint8_t *get_dna (void);

	uint8_t **get_grid (void);

	uint32_t get_fit (void);

	uint16_t get_gate (void);

	uint32_t get_age (void);

	bool get_eval (void);

	bool get_alive (void);

	bool get_sol (void);


	/* ========== Set Functions ========== */

	void set_fit (const uint32_t &set_val);

	void set_gate (const uint16_t &set_val);

	void set_age (void);

	void set_eval (const bool &set_val);

	void set_sol (const bool &set_val);

};

#endif
