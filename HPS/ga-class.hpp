/* Header file for Genetics Algorithm Class
	Contains the class definition and functions protoypes.

	A single instance of class GeneticAlgorithm is referred to as an individual.
	A collection of individuals is referred to as a population.
*/

#ifndef GACLASS_H_INCLUDED
#define GACLASS_H_INCLUDED

class GeneticAlgorithm {
	public:
		unsigned int	uid;	/* Unique id for tracking the individual */
		unsigned int	rank;	/* Fitness Ranking of the individual */
		uint8_t			*dna;	/* DNA sequence (Cellular Automaton Rule) */
		unsigned int	fit;	/* Fitness score of the individual */
		unsigned int	age;	/* For how long the individual has been alive for */
		bool			eval;	/* Evaluation Flag | 0 = not evaluated | 1 = evaluated */

		/* ----- Constructors & Destructors ----- */
	public:
		/* Default Constructor
			Constructs a single individual with default values of everything
		*/
		GeneticAlgorithm ();
		/* Single Object Constructor
			Constructs a single individual, given a uid number and a DNA length
		*/
		GeneticAlgorithm (unsigned int curr_uid, unsigned int dna_length);
		/* Multi-Object Constructor
			Constructs an array of individuals (a population), given:
			a starting UID number to count from
			a DNA length
			a desired array size (how many individuals to construct)
		*/
		GeneticAlgorithm (unsigned int curr_uid, unsigned int dna_length, unsigned int count);
		/* Destructor */
		~GeneticAlgorithm ();
	// 	void Reset (); // Resets the individual
	//
	// public:
	// 	GeneticAlgorithm (); // Constructor
	// 	void Eval (); // Signle-Evaluation function
	// 	void Sort (GeneticAlgorithm *array); // Sorting function
	//
	// 	// ----- Genetic Algorithm Functions ----- //
	// 	void Selection (GeneticAlgorithm *array);
	// 	void Crossover (const uint8_t *dna_a, const uint8_t *dna_b);
	// 	void Mutate ();
	//
	// 	// ----- Get Functions ----- //
	// 	uint32_t	getuid ();
	// 	uint16_t	getrnk ();
	// 	uint16_t	getfit ();
	// 	uint16_t	getage ();
	// 	uint8_t*	getdna ();
	// 	bool		geteval ();
	// 	void		debug ();
	//
	// 	// ----- Set Functions ----- //
	// 	void setuid (const uint32_t set);
	// 	void setrnk (const uint16_t set);
	// 	void setfit (const uint16_t set);
	// 	void setage (const uint16_t set);
	// 	void setdna (const uint8_t* set);

} ga; // use ga.function() instead of the instance used, for certain functions.

/* Constructor */

GeneticAlgorithm::GeneticAlgorithm () {
	uid = 0;
	rank = 0;
	fit = 0;
	age = 0;
	eval = 0;
}

GeneticAlgorithm::GeneticAlgorithm (unsigned int curr_uid, unsigned int dna_length) {
	uid = curr_uid;

	dna = (uint8_t*) malloc (dna_length);
	if (dna == NULL) {
		perror ("Not Enough Memory for MALLOC\n");
		exit (EXIT_FAILURE);
	} else {
		for (unsigned int i=0; i<dna_length; i++) {
			dna[i] = i;
		}
	}

	curr_uid++;
}

GeneticAlgorithm::GeneticAlgorithm (
	unsigned int curr_uid, unsigned int dna_length, unsigned int count) {

}

GeneticAlgorithm::~GeneticAlgorithm () {
	free (dna);
}

// #include "ga-class.cpp"

#endif
