/* Genetics Algorithm function definitions file */

/* Single Object Constructor //
	Initializes a single individual, given a uid number and a DNA length
*/
GeneticAlgorithm::GeneticAlgorithm (const uint32_t curr_uid, const uint32_t dna_length) {
	uid = curr_uid;
	dna = GeneticAlgorithm::dna_calloc (dna_length);
	fit = 0;
	rank = 0;
	age = 0;
	eval = 0;
}

/* uint8_t *GeneticAlgorithm::dna_calloc (const uint32_t dna_length)
	DNA dynamic memory allocation
	Allocates memory for the DNA sequence, for a given DNA length.

	Takes in constant unsigned 32-bit integer DNA length,

	Allocates and zero-initializes an array of unsigned characters, of length (dna_length).

	Checks whether or not memory was successfully allocated,
		throws exception if failed
		else fills the array with random number, modulo CA Color count.

	Returns the pointer to an array of unsigned chars.
*/
uint8_t *GeneticAlgorithm::dna_calloc (const uint32_t dna_length) {
	uint8_t *dna = (uint8_t*) calloc (dna_length, sizeof (uint8_t) );
	if (dna == NULL) {
		perror ("Not Enough Memory for MALLOC\n");
		exit (EXIT_FAILURE);
	}
	for (uint32_t i=0; i<dna_length; i++) {
		dna[i] = rand () % global.CA.COLOR;
	}
	return dna;
}


void GeneticAlgorithm::Reset (void) {
	// Assign new id
	// uid = uid_counter;
	// uid_counter++;

	// Zero initialize the rest
	fit = 0;
	rank = 0;
	age = 0;
	eval = 0;
}

// void GeneticAlgorithm::Eval () {
// 	// Evaluates a single individual
// 	eval_flag = 1;
// 	age += 1;
// }

void GeneticAlgorithm::Sort (GeneticAlgorithm *array) {
	sort(array, array + global.GA.POP,
		[](GeneticAlgorithm const & a, GeneticAlgorithm const & b) -> bool
		{ return a.fit > b.fit; }); // Swap sign to reverse sorting

	// Assign new rank by index
	for (unsigned int idx=0; idx<global.GA.POP; idx++)
		array[idx].rank = idx;
}

// ----- Genetic Algorithm Operations ----- //

void GeneticAlgorithm::Selection (GeneticAlgorithm *array) {
	// Vector of who live or dies - contains uid
	unsigned int idx, i, counter;
	vector<uint16_t> live, dead;

	/* Note: idx is equivalent to the rank, because array is pre-sorted.
	 We will be keeping track of the idx, because it allows easy access to the original population via array[idx] and its properties.
	*/
	for (idx=0; idx<global.GA.POP; idx++) {
		uint16_t rng = rand () % global.GA.POP;
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

		for (i=0; i<global.GA.POOL; i++) {
			/* Randomly selects POOL number of individuals from the "live" list.
			This does not guarantee POOL number of *unique* individuals; individuals may appear more than once in any pool, or even fill the entire pool with themselves (very unlikely, but possible).

			> Vector "pool" now contains the indexes from the original population, this effectively gives us a list of array->rank.
			> Smaller index = Higher rank.
			> Selection favors higher ranking individuals.
			*/
			pool_a.push_back (live[rand()%live.size()]);
			pool_b.push_back (live[rand()%live.size()]);
		}

		// Unique Parents - contains the idx for each individual
		uint16_t parent_a, parent_b;
		// Selects the fittest from each pool to be the parents
		parent_a = *min_element (pool_a.begin(), pool_a.end());
		parent_b = *min_element (pool_b.begin(), pool_b.end());
		// Removes the chosen individual from the pool
		pool_a.erase (min_element (pool_a.begin(), pool_a.end()));
		pool_b.erase (min_element (pool_b.begin(), pool_b.end()));

		// Ensure two unique parents
		counter = 0;
		while (parent_a == parent_b) {
			if (!pool_b.empty()) {
				/* if pool_b is not empty */
				parent_b = *min_element (pool_b.begin(), pool_b.end());
				pool_b.erase (min_element (pool_b.begin(), pool_b.end()));
			} else if (!pool_a.empty()) {
				/* if pool_a is not empty */
				parent_a = *min_element (pool_a.begin(), pool_a.end());
				pool_a.erase (min_element (pool_a.begin(), pool_a.end()));
			} else {
				/* if both pools are empty //
					(two entire pool was made of the same individual)
					Get entirely random individual from main population
				*/
				parent_b = live[ rand () % live.size() ];
			}

			/* Infinite Loop catch - just in case */
			counter++;
			if (counter >= 100) {
				perror ("Unique selection failed");
				break;
			}
		}

		array[dead.back()].Reset ();
		array[dead.back()].Crossover (array[parent_a].dna, array[parent_b].dna);
		array[dead.back()].Mutate ();
		dead.pop_back ();

		pool_a.clear ();
		pool_b.clear ();
	}

	live.clear();
	dead.clear();
}

void GeneticAlgorithm::Crossover (const uint8_t *dna_a, const uint8_t *dna_b) {
	uint32_t dna_length = pow(global.CA.COLOR, global.CA.NB);

	for (uint16_t i=0; i<dna_length; i++) {
		if ( rand()%1 )
			dna[i] = dna_a[i];
		else
			dna[i] = dna_b[i];
	}
}

void GeneticAlgorithm::Mutate (void) {
	uint32_t dna_length = pow(global.CA.COLOR, global.CA.NB);

	for (uint16_t i=0; i<dna_length; i++) {
		if ( (rand()%10000) < (global.GA.MUTP*10000) ) {
			// Uses another random number to decrease chance of major mutations
			uint8_t rng = rand()%50;
			switch ( rng ) {
				case 0: { // Swap (swaps this gene with another random gene)
					uint8_t tmp = dna[i];
					uint16_t swap = rand()%dna_length;
					dna[i] = dna[swap];
					dna[swap] = tmp;
					break;
				}
				case 1: { // Scramble (scramble genes in a random range)
					uint16_t range = (rand()%dna_length)/2;
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
					if (p==1) 	dna[i] = global.CA.COLOR-1;
					if (p==2)	dna[i] = rand()%global.CA.COLOR;
					break;
				}
			}
		}
	}
}

// // ----- Get Functions ----- //
// uint32_t GeneticAlgorithm::getuid () {
// 	return uid;
// }
//
// uint16_t GeneticAlgorithm::getrnk () {
// 	return rank;
// }
//
// uint16_t GeneticAlgorithm::getfit () {
// 	return fitness;
// }
//
// uint16_t GeneticAlgorithm::getage () {
// 	return age;
// }
//
// uint8_t* GeneticAlgorithm::getdna () {
// 	return dna;
// }
//
// bool GeneticAlgorithm::geteval () {
// 	return eval_flag;
// }
//
// void GeneticAlgorithm::debug () {
// 	// Prints debug information for the individual
// 	printf("UID: %u RANK: %u FITNESS: %u AGE: %u\n", uid, rank, fitness, age);
// }
//
// // ----- Set Function ----- //
// void GeneticAlgorithm::setuid (const uint32_t set) {
// 	uid = set;
// }
//
// void GeneticAlgorithm::setrnk (const uint32_t set) {
// 	rank = set;
// }
//
// void GeneticAlgorithm::setfit (const int set) {
// 	fit = set;
// }
//
// void GeneticAlgorithm::setage (const uint32_t set) {
// 	age = set;
// }
//
// void GeneticAlgorithm::setdna (const uint8_t* set) {
// 	for (int i=0; i<; i++) {
// 		if (set[i] >= K) { // Out of bound catch
// 			perror ("Set value out of bound for method GeneticAlgorithm::setdna");
// 		} else {
// 			dna[i] = set[i];
// 		}
// 	}
// }
