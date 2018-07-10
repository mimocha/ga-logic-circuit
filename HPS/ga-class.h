/* Class header file for Genetics Algorithm main program.
	Contains all classes and methods definitions.

	A single instance of class GeneticAlgorithm is referred to as an individual.
	A collection of individuals is referred to as a population.
*/

#ifndef GACLASS_H_INCLUDED
#define GACLASS_H_INCLUDED

class GeneticAlgorithm {
private:
	uint32_t	uid; // Unique id for tracking an individual
	uint16_t	rank; // Fitness Ranking of the individual
	uint8_t		dna[K_CUBE]; // DNA sequence (Cellular Automaton Rule)
	uint16_t	fitness; // Fitness score for an individual
	uint16_t	age; // For how long an individual has been alive for
	bool		elite_flag; // 0 for not-elite; 1 for elite
	bool		eval_flag; // 0 for not-yet-evaluated; 1 for evaluated

	void Reset (); // Resets the individual

public:
	GeneticAlgorithm (); // Constructor
	void Eval (); // Signle-Evaluation function
	void Sort (GeneticAlgorithm *array); // Sorting function

	// ----- Genetic Algorithm Functions ----- //
	void Selection (GeneticAlgorithm *array);
	void Crossover (const uint8_t *dna_a, const uint8_t *dna_b);
	void Mutate ();

	// ----- Get Functions ----- //
	uint32_t	getuid ();
	uint16_t	getrnk ();
	uint16_t	getfit ();
	uint16_t	getage ();
	uint8_t*	getdna ();
	void		debug ();

	// ----- Set Functions ----- //
	void setuid (const uint32_t set);
	void setrnk (const uint16_t set);
	void setfit (const uint16_t set);
	void setage (const uint16_t set);
	void setdna (const uint8_t* set);

} ga; // use ga.function() instead of the instance used, for certain functions.

void GeneticAlgorithm::Reset () {
	// Assign new id
	uid = uid_counter;
	uid_counter++;

	// Zero initialize the rest
	fitness = 0;
	rank = 0;
	eval_flag = 0;
	elite_flag = 0;
	age = 0;
}

GeneticAlgorithm::GeneticAlgorithm () {
	// Assign new id
	uid = uid_counter;
	uid_counter++;

	// Randomly generate dna
	for (int i=0; i<K_CUBE; i++)
		dna[i] = rand() % K;

	// Zero initialize the rest
	fitness = 0;
	rank = 0;
	eval_flag = 0;
	elite_flag = 0;
	age = 0;
}

void GeneticAlgorithm::Eval () {
	// Evaluates a single individual
	if (eval_flag == 0) {
		// --- Insert Evaluation Code Here --- //
		fitness = 0;
		for (int i=0; i<K_CUBE; i++)
			fitness += dna[i];

		eval_flag = 1;
	}
	age += 1;
}

void GeneticAlgorithm::Sort (GeneticAlgorithm *array) {
	// Sorts the entire population by fitness value, in decreasing order
	sort(array, array + POP,
		[](GeneticAlgorithm const & a, GeneticAlgorithm const & b) -> bool
		{ return a.fitness > b.fitness; }); // Swap sign to reverse sorting

	// Assign new rank by index
	for (int i=0; i<POP; i++)
		array[i].rank = i;
}

// ----- Genetic Algorithm Functions ----- //
void GeneticAlgorithm::Selection (GeneticAlgorithm *array) {
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

	*/
	// Vector of who live or dies - contains uid
	unsigned int idx, i, counter;
	vector<uint16_t> live, dead;

	/* Note: idx is equivalent to the rank, because array is pre-sorted.
	 We will be keeping track of the idx, because it allows easy access to the original population via array[idx] and its properties.
	*/
	for (idx=0; idx<POP; idx++) {
		if ( array[idx].elite_flag == 1 ) { // Elite Selection
			live.push_back(idx);
			continue;
		}
		uint16_t rng = rand()%POP;
		if ( rng >= idx ) { // 'Normal' selection
			live.push_back(idx); // lives
		} else {
			dead.push_back(idx); // dies
		}
	}

	/* TOURNAMENT SELECTION
	Using the results from the previous part (selecting who live & dies), we will next select who will reproduce with whom, via tournament selection strategy.
	*/
	while ( !dead.empty() ) { // Repeats over all dead individuals
		// Tournament selection pool - Vector because size is random
		vector<uint16_t> pool_a, pool_b;
		vector<uint16_t> debug_a, debug_b;
		for (i=0; i<POOL; i++) {
			/* Randomly selects POOL number of individuals from the "live" list.
			This does not guarantee POOL number of *unique* individuals; individuals may appear more than once in any pool, or even fill the entire pool with themselves (very unlikely, but possible).

			> Vector "pool" now contains the indexes from the original population, this effectively gives us a list of array->rank.
			> Smaller index = Higher rank.
			> Selection favors higher ranking individuals.
			*/
			pool_a.push_back( live[ rand()%live.size() ]);
			pool_b.push_back( live[ rand()%live.size() ]);
		}

		// Debug print
		if (SHOW_D) {
			debug_a = pool_a;
			debug_b = pool_b;
		}

		// Unique Parents - contains the idx for each individual
		uint16_t parent_a, parent_b;
		// Selects the fittest from each pool to be the parents
		parent_a = *min_element(pool_a.begin(), pool_a.end());
		parent_b = *min_element(pool_b.begin(), pool_b.end());
		// Removes the chosen individual from the pool
		pool_a.erase( min_element(pool_a.begin(), pool_a.end()) );
		pool_b.erase( min_element(pool_b.begin(), pool_b.end()) );

		// Ensure two unique parents
		counter = 0;
		while (parent_a == parent_b) {
			if (!pool_b.empty()) { // if pool_b is not empty
				parent_b = *min_element(pool_b.begin(), pool_b.end());
				pool_b.erase( min_element(pool_b.begin(), pool_b.end()) );
			} else if (!pool_a.empty()) { // if pool_a is not empty
				parent_a = *min_element(pool_a.begin(), pool_a.end());
				pool_a.erase( min_element(pool_a.begin(), pool_a.end()) );
			} else { // if both pools are empty
				// (two entire pool was made of the same individual)
				// Get entirely random individual from main population
				parent_b = live[ rand()%live.size() ];
			}
			counter++;
			if (counter >= 100) { // Infinite Loop catch - just in case
				puts("Unique selection failed");
				break;
			}
		}

		// Debug print
		if (SHOW_D) {
			puts ("POOL A");
			puts ("RANK | UID | FITNESS");
			for (i=0; i<debug_a.size(); i++) {
				printf("%4d | %3d | %7d",
					debug_a[i], array[debug_a[i]].uid, array[debug_a[i]].fitness);
				if (debug_a[i] == parent_a) cout << " -> A";
				if (debug_a[i] == parent_b) cout << " -> B";
				cout << endl;
			}
			puts ("POOL B");
			puts ("RANK | UID | FITNESS");
			for (i=0; i<debug_b.size(); i++) {
				printf("%4d | %3d | %7d",
					debug_b[i], array[debug_b[i]].uid, array[debug_b[i]].fitness);
				if (debug_b[i] == parent_a) cout << " -> A";
				if (debug_b[i] == parent_b) cout << " -> B";
				cout << endl;
			}
			printf("%4d | %3d | %7d -> Replaced\n",
			dead.back(), array[dead.back()].uid, array[dead.back()].fitness);
		}

		array[dead.back()].Reset();
		array[dead.back()].Crossover(array[parent_a].dna, array[parent_b].dna);
		array[dead.back()].Mutate();
		dead.pop_back();

		pool_a.clear();
		pool_b.clear();
		if (SHOW_D) {
			debug_a.clear();
			debug_b.clear();
		}
	}

	live.clear();
	dead.clear();
}

void GeneticAlgorithm::Crossover (const uint8_t *dna_a, const uint8_t *dna_b) {
	/* CROSSOVER (const uint8_t *DNA_A, const uint8_t DNA_B)
	Homogeneously crossesover two parents' dna string.
	Equal likelyhood any certain dna character will be chosen.
	Equal split between two parents is not guaranteed.
	*/
	for (uint16_t i=0; i<K_CUBE; i++) {
		if ( rand()%1 )
			dna[i] = dna_a[i];
		else
			dna[i] = dna_b[i];
	}
}

void GeneticAlgorithm::Mutate () {
	for (uint16_t i=0; i<K_CUBE; i++) {
		if ( (rand()%10000) < (PM*10000) ) {
			// Uses another random number to decrease chance of major mutations
			uint8_t rng = rand()%50;
			switch ( rng ) {
				case 0: { // Swap (swaps this gene with another random gene)
					uint8_t tmp = dna[i];
					uint16_t swap = rand()%K_CUBE;
					dna[i] = dna[swap];
					dna[swap] = tmp;
					break;
				}
				case 1: { // Scramble (scramble genes in a random range)
					uint16_t range = (rand()%K_CUBE)/2;
					if (i > range) {
						random_shuffle (&dna[range], &dna[i]);
					} else if (range > i) {
						random_shuffle (&dna[i], &dna[range]);
					}
					// Does not scramble if range == i
					break;
				}
				default: { // Single-gene mutations
					uint8_t p = rand() % 3;
					if (p==0) 	dna[i] = 0;
					if (p==1) 	dna[i] = K-1;
					if (p==2)	dna[i] = rand()%K;
					break;
				}
			}
		}
	}
}

// ----- Get Functions ----- //
uint32_t GeneticAlgorithm::getuid () {
	return uid;
}

uint16_t GeneticAlgorithm::getrnk () {
	return rank;
}

uint16_t GeneticAlgorithm::getfit () {
	return fitness;
}

uint16_t GeneticAlgorithm::getage () {
	return age;
}

uint8_t* GeneticAlgorithm::getdna () {
	return dna;
}

void GeneticAlgorithm::debug () {
	// Prints debug information for the individual
	printf("UID: %u RANK: %u FITNESS: %u AGE: %u\n", uid, rank, fitness, age);
}

// ----- Set Function ----- //
void GeneticAlgorithm::setuid (const uint32_t set) {
	uid = set;
}

void GeneticAlgorithm::setrnk (const uint16_t set) {
	rank = set;
}

void GeneticAlgorithm::setfit (const uint16_t set) {
	fitness = set;
}

void GeneticAlgorithm::setage (const uint16_t set) {
	age = set;
}

void GeneticAlgorithm::setdna (const uint8_t* set) {
	for (int i=0; i<K_CUBE; i++) {
		if (set[i] >= K) { // Out of bound catch
			perror ("Set value out of bound for method GeneticAlgorithm::setdna");
		} else {
			dna[i] = set[i];
		}
	}
}

#endif
