/* Main C++ file for Cellular Automaton Generation Function CELLGEN
	Generates a grid of logic gates and connections based on given arguments.
*/

// uint16_t convert (const uint8_t *nb) {
// 	// uint16_t can handle upto 65535
// 	// -> Which means this function can handle upto 40 colors
// 	return nb[0] + (nb[1]*global.CA.COLOR) + (nb[2]*global.CA.COLOR*global.CA.COLOR);
// }

uint8_t cellfunc (const uint8_t *nb, const uint8_t *rule) {
	uint32_t idx = nb[0] + (nb[1]*global.CA.COLOR) + (nb[2]*global.CA.COLOR*global.CA.COLOR);
	return rule [idx];
}

// uint8_t cellfunc (const uint8_t *first, const uint32_t offset, const uint8_t *LUT) {
// 	/* LUT index number */
// 	uint32_t idx = 0;
// 	/* Iterates through given pointer range, then calculate value for each digit */
// 	for (uint32_t i=0; i<offset; i++) {
// 		idx += *(first + i) * pow (global.CA.COLOR, i);
// 	}
// 	/* Return value at given LUT index */
// 	return LUT[idx];
// }

void cellgen (uint8_t *input, uint8_t *output, const uint8_t *LUT) {
	uint8_t *neighbor = (uint8_t *) calloc (global.CA.NB, sizeof(uint8_t));

	neighbor [0] = 0;
	neighbor [1] = input [0];
	neighbor [2] = input [1];
	output [0] = cellfunc (neighbor, LUT);

	for (unsigned int x = 1; x < global.CA.DIMX - 1; x++) {
		neighbor [0] = input [x - 1];
		neighbor [1] = input [x];
		neighbor [2] = input [x + 1];
		output [x] = cellfunc (neighbor, LUT);
	}

	neighbor [0] = input [global.CA.DIMX - 2];
	neighbor [1] = input [global.CA.DIMX - 1];
	neighbor [2] = 0;
	output [global.CA.DIMX - 1] = cellfunc (neighbor, LUT);

	free (neighbor);

	// uint32_t idx, x;
	//
	// /* Left Edge Special Case */
	// idx = input[0]*pow (global.CA.COLOR, 1) + input[1]*pow (global.CA.COLOR, 2);
	// printf ("LIDX: %u\n", idx);
	// output[0] = LUT[idx];
	//
	// /* Normal Cases */
	// for (x=1; x<global.CA.DIMX-1; x++) {
	// 	output[x] = cellfunc (&input[x-1], global.CA.NB, LUT);
	// }
	//
	// /* Right Edge Special Case */
	// idx = input[x] + input[x+1]*pow (global.CA.COLOR, 1);
	// printf ("RIDX: %u\n", idx);
	// output[x+1] = LUT[idx];
}

void cellprint (const uint8_t cell) {
	// Colored ANSI codes
	switch (cell) {
		case 0: // NULL
			std::cout << ANSI_GRAY;
			// std::cout << '.';
			break;
		case 1: // LEFT
			cout << ANSI_RED;
			// std::cout << 'L';
			break;
		case 2: // RIGHT
			cout << ANSI_YELLOW;
			// std::cout << 'R';
			break;
		case 3: // NAND
			cout << ANSI_LGREEN;
			// std::cout << 'N';
			break;
		case 4: // FORWARD
			std::cout << ANSI_LCYAN;
			// std::cout << 'F';
			break;
		case 5: // XOR
			std::cout << ANSI_LPURPLE;
			// std::cout << 'X';
			break;
		default: // Undefined
			std::cout << ANSI_WHITE;
			// std::cout << 'U';
			break;
	}

	printf("%X",cell);
	std::cout << ANSI_WHITE;
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
