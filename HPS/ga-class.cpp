/* Genetics Algorithm function definitions file */

/* ----- Constructor & Destructor ----- */

GeneticAlgorithm::GeneticAlgorithm (const uint32_t dna_length) {
	uid = object_count;
	object_count++;
	dna = GeneticAlgorithm::dna_calloc (dna_length);
	fit = 0;
	rank = 0;
	age = 0;
	eval = 0;
}

/* Destructor //
	A custom destructor causes bugs, and segmentation faults.
	Probably due to the destructor being called automatically by the compiler at some point,
	this breaks the program and will likely cause a segmentation fault.

	Thus, memory is freed manually at the end of run_sim(), in "sim.cpp".
	This destructor is left here for future references.
*/
// GeneticAlgorithm::~GeneticAlgorithm (void) {
// 	if (dna != NULL) {
// 		free (dna);
// 	}
// }

/* ----- Genetic Algorithm Operations ----- */

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

		// Attempt to ensure two unique parents
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
		if ((rand() % 1) == 1)
			dna[i] = dna_a[i];
		else
			dna[i] = dna_b[i];
	}
}

void GeneticAlgorithm::Mutate (void) {
	uint32_t dna_length = pow(global.CA.COLOR, global.CA.NB);

	for (uint32_t i=0; i<dna_length; i++) {
		uint16_t rng = rand () % 65535;

		/* Probability Check */
		if (rng > floor (global.GA.MUTP * 65535) ) {
			continue;
		}

		/* Minor Mutations | 75% of all mutations*/
		if (rng > floor (global.GA.MUTP * 65535 * 0.25) ) {
			switch (rng % 5) {
				case 0:
					dna[i] = 0;
					break;
				case 1:
					dna[i] = global.CA.COLOR - 1;
					break;
				case 2:
					dna[i] = rand() % global.CA.COLOR;
					break;
				case 3:
					if (dna[i] != 0) {
						dna[i] -= 1;
					}
					break;
				case 4:
					if (dna[i] != global.CA.COLOR - 1) {
						dna[i] += 1;
					}
					break;
			}
			continue;
		}

		/* Swap Mutation | 20% of all mutations */
		if (rng > floor (global.GA.MUTP * 65535 * 0.05) ) {
			uint16_t dest = rand() % dna_length;
			uint8_t tmp = dna[i];
			dna[i] = dna[dest];
			dna[dest] = tmp;
			continue;
		}

		/* Scramble (Least Likely) | 5%  of all mutation */
		uint16_t range = (rand () % dna_length)/2;
		if (i > range) {
			random_shuffle (&dna[range], &dna[i]);
		} else if (range > i) {
			random_shuffle (&dna[i], &dna[range]);
		}
	}
}

/* ----- Other Miscellany Operations ----- */

void GeneticAlgorithm::Reset (void) {
	/* TODO: DNA REALLOC - If DNA length has changed, reallocate *dna */
	uid = object_count;
	object_count++;
	fit = 0;
	rank = 0;
	age = 0;
	eval = 0;
}

void GeneticAlgorithm::Sort (GeneticAlgorithm *array) {
	sort(array, array + global.GA.POP,
		[](GeneticAlgorithm const & a, GeneticAlgorithm const & b) -> bool
		{ return a.fit > b.fit; }); // Swap sign to reverse sorting

	// Assign new rank by index
	for (unsigned int idx=0; idx<global.GA.POP; idx++)
		array[idx].rank = idx;
}

uint8_t *GeneticAlgorithm::dna_calloc (const uint32_t dna_length) {
	uint8_t *dna = (uint8_t*) calloc (dna_length, sizeof (uint8_t) );
	if (dna == NULL) {
		perror ("DNA CALLOC\n");
		exit (EXIT_FAILURE);
	}
	for (uint32_t i=0; i<dna_length; i++) {
		dna[i] = rand () % global.CA.COLOR;
	}
	return dna;
}

/* ----- Set Functions ----- */

void GeneticAlgorithm::setdna (unsigned int arg_count, ...) {
	va_list valist;
	va_start (valist, arg_count);
		for (unsigned int i=0; i<arg_count; i++) {
			dna[i] = va_arg (valist, unsigned int);
		}
	va_end (valist);
}
