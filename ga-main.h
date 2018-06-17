/* Header file for Genetics ALgorithm Main Program.
	Contains all class definitions for Genetic Algorithm.
*/

#ifndef GAMAIN_H_INCLUDED
#define GAMAIN_H_INCLUDED

class Individual {
public:
	unsigned short gene, id;
	float fitness;

	int counteval(void);
	float calcfitness(void);
	void randinit(void);
	void nullinit(void);

private:
	bool elite_flag, eval_flag;
	unsigned short age;

	// Initialize this individual as child of some parent individuals
	// unsigned short crossover(Individual *parent_a, Individual *parent_b),
	// void mutate(Individual *self.gene[]), // Mutation function
	// void update(Individual *self.gene[]); // Updating various internal members

};

// float Individual::calcfitness(void)
// unsigned short Individual::crossover(Individual *parent_a, Individual *parent_b)
// void Individual::mutate(Individual *self.gene)



// Class for handling arrays of individuals
// Possibly for handling multiple simultaneous populations
class Population {
public:
	unsigned short id, count;
};

#endif
