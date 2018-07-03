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

public:
	// ----- Get Functions ----- //
	uint32_t	getuid ();
	uint16_t	getrnk ();
	uint16_t	getfit ();
	uint16_t	getage ();
	uint8_t*	getdna ();

	// ----- Set Functions ----- //
	void setuid (uint32_t set);
	void setrnk (uint16_t set);
	void setfit (uint16_t set);
	void setage (uint16_t set);
	void setdna (uint8_t* set);
} ga;

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
	while ( !dead.empty() ) { // Repeats over all dead
		// Tournament selection pool - Vector because size changes
		vector<uint16_t> pool, tmp;
		for (i=0; i<POOL; i++) {
			/* Randomly selects POOL number of individuals from the "live" list.
			This does not guarantee POOL number of *unique* individuals; individuals may appear more than once in any pool, or even fill the entire pool with themselves (very unlikely, but possible).

			> Vector "pool" now contains the indexes from the original population, this effectively gives us a list of array->rank.
			> Smaller index = Higher rank.
			> Selection favors higher ranking individuals.
			*/
			pool.push_back( live[ rand()%live.size() ]);
		}

		// Debug print
		if (SHOW_D) tmp = pool;

		// Unique Parents - contains the idx for each individual
		uint16_t parent_a, parent_b;
		parent_a = *min_element(pool.begin(), pool.end());
		pool.erase( min_element(pool.begin(), pool.end()) );
		counter = 0;
		do { // Repeat to ensure two unique parents
			if (!pool.empty()) { // if pool is not empty
				parent_b = *min_element(pool.begin(), pool.end());
				pool.erase( min_element(pool.begin(), pool.end()) );
			} else { // if pool is empty
				// (entire pool was made of the same individual)
				// Get entirely random individual from main population
				parent_b = live[ rand()%live.size() ];
			}
			counter++;
			if (counter >= 100) { // Loop too long catch - just in case
				puts("Unique selection failed");
				break;
			}
		} while (parent_a == parent_b);

		// Debug print
		if (SHOW_D) {
			puts ("RANK | UID | FITNESS");
			for (i=0; i<tmp.size(); i++) {
				printf("%4d | %3d | %7d",
					tmp[i], array[tmp[i]].uid, array[tmp[i]].fitness);

				if (tmp[i] == parent_a) cout << " -> A";
				if (tmp[i] == parent_b) cout << " -> B";
				cout << endl;
			}
			printf("%4d | %3d | %7d -> Replaced\n",
			dead.back(), array[dead.back()].uid, array[dead.back()].fitness);
		}

		array[dead.back()].Reset();
		array[dead.back()].Crossover(array[parent_a].dna, array[parent_b].dna);
		array[dead.back()].Mutate();
		dead.pop_back();

		pool.clear();
		if (SHOW_D) tmp.clear();
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

// ----- Set Function ----- //
void GeneticAlgorithm::setuid (uint32_t set) {
	uid = set;
}

void GeneticAlgorithm::setrnk (uint16_t set) {
	rank = set;
}

void GeneticAlgorithm::setfit (uint16_t set) {
	fitness = set;
}

void GeneticAlgorithm::setage (uint16_t set) {
	age = set;
}

void GeneticAlgorithm::setdna (uint8_t* set) {
	for (int i=0; i<K_CUBE; i++) {
		if (set[i] >= K) { // Out of bound catch
			perror ("Set value out of bound for method GeneticAlgorithm::setdna");
		} else {
			dna[i] = set[i];
		}
	}
}

#endif
