/* Simulation Wrapper File */

int run_sim (void) {
	#define SUCCESS 1
	#define FAILED -1

	/* Prints Parameter List */
	printf (
		"\n\t>>>-- Beginning Simulation --<<<\n"
		"\tPOP = %4u | GEN = %4u | MUT = %0.3f | POOL = %4u\n"
		"\tDIMX = %3u | DIMY = %3u | COLOR = %3u | NEIGHBOR = %2u\n"
		"\tFIT = %1u | TIME = %1u | CAPRINT = %1u | EXPORT = %1u\n\n",
		global.GA.POP, global.GA.GEN, global.GA.MUTP, global.GA.POOL,
		global.CA.DIMX, global.CA.DIMY, global.CA.COLOR, global.CA.NB,
		global.DATA.FIT, global.DATA.TIME, global.DATA.CAPRINT, global.DATA.EXPORT
	);

	/* Initializes Random Number Generator */
	srand (time (NULL));

	/* Computes DNA length, the number of CA rules; Calculated by -> Color^Neighbor */
	uint32_t dna_length = pow (global.CA.COLOR, global.CA.NB);

	/* Allocates an array of individuals (population), of size "global.GA.POP" */
	GeneticAlgorithm *indv = (GeneticAlgorithm *) calloc (global.GA.POP, sizeof (GeneticAlgorithm));

	/* Initialize Population */
	for (unsigned int idx=0; idx<global.GA.POP; idx++) {
		indv[idx] = GeneticAlgorithm (idx, dna_length);
	}

	/* ----- Run Simulation ----- */
	for (unsigned int gen=0; gen<global.GA.GEN; gen++) {
		ga.Selection (indv);
	}

	/* Free population memory and return to main menu */
	for (unsigned int idx=0; idx<global.GA.POP; idx++) {
		free (indv[idx].dna);
	}
	free (indv);
	return SUCCESS;
}
