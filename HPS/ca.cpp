/* Main C++ file for Cellular Automaton Generation Function CELLGEN
	Generates a grid of logic gates and connections based on given arguments.
*/

uint8_t cellfunc (uint8_t *nb, const uint8_t *DNA) {
	uint32_t idx = 0;
	for (unsigned int c = 0; c < global.CA.NB; c++) {
		idx += nb [c] * pow (global.CA.COLOR, c);
	}
	return DNA [idx];
}

void cellgen (const uint8_t *input, uint8_t *output, const uint8_t *DNA) {
	/* Allocate memory for neighbor array. */
	uint8_t *neighbor = (uint8_t *) calloc (global.CA.NB, sizeof(uint8_t));
	/* Calculate index offset for neighboring cells. Only work with odd-numbers */
	static int offset = ((global.CA.NB - 1) / 2);

	/* Iterate over entire row */
	for (unsigned int x = 0; x < global.CA.DIMX; x++) {
		/* Load Neighbor Array && Check Out Of Bound */
		for (unsigned int n = 0; n < global.CA.NB; n++) {
			/* Calculate the index location */
			int idx = x - offset + n;
			/* If index location is out of bound, set to 0, otherwise return normal values */
			if (idx < 0 || idx >= (int) global.CA.DIMX) {
				neighbor [n] = 0;
			} else {
				neighbor [n] = input [idx];
			}
		}
		/* Call function cellfunc() to access values from the DNA string. */
		output [x] = cellfunc (neighbor, DNA);
	}

	/* Free allocated memory before returning. */
	free (neighbor);
}

void cellprint (const uint8_t cell) {
	switch (cell) {
		case 0:
			cout << ANSI_GRAY;
			break;
		case 1:
			cout << ANSI_RED;
			break;
		case 2:
			cout << ANSI_YELLOW;
			break;
		case 3:
			cout << ANSI_LGREEN;
			break;
		case 4:
			cout << ANSI_LCYAN;
			break;
		case 5:
			cout << ANSI_LPURPLE;
			break;
		default: // Undefined
			cout << ANSI_WHITE;
			break;
	}

	printf("%X",cell);
	cout << RESET;
}

void ca_graph (const uint8_t *array, const unsigned int length) {
	for (unsigned int x=0; x<length; x++) {
		cellprint (array[x]);
	}
	std::cout << std::endl;
}

// void ca_graph (GeneticAlgorithm *pop, const int count) {
// 	printf("Array of the top %d individuals:\n", count);
//
// 	for (int i=0; i<count; i++) {
// 		printf("RANK: %4d | UID: %7d | FITNESS: %7d\n\n",
// 			pop[i].getrnk(), pop[i].getuid(), pop[i].getfit());
//
// 		cellgen (seed, pop[i].getdna());
//
// 		for (int y=0; y<DIM; y++) {
// 			for (int x=0; x<DIM; x++) {
// 				cellprint (LGA[y][x]);
// 			}
// 			cout << endl;
// 		}
//
// 		cellgen (LGA[DIM-1], pop[i].getdna());
//
// 		// for (int y=1; y<DIM; y++) {
// 		// 	for (int x=0; x<DIM; x++) {
// 		// 		cellprint (LGA[y][x]);
// 		// 	}
// 		// 	cout << endl;
// 		// }
// 		cout << endl;
//
// 		for (int y=0; y<DIM; y++) {
// 			for (int x=0; x<DIM; x++) {
// 				cellprint (LGA[y][x]);
// 			}
// 			cout << endl;
// 		}
// 		cout << endl;
//
// 	}
//
// }
