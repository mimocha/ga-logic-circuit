/* Main C++ file for Cellular Automaton Generation Function CELLGEN
	Generates a grid of logic gates and connections based on given arguments.
*/

void cellgen (const uint8_t *input, const uint8_t *rule) {
	// Define working variables
	uint8_t neighbor[3], i, j;

	// Calculate each cell
	for (i=0; i<DIM; i++) {
		for (j=0; j<DIM; j++) {

			if (i==0) { // Loading first row as input (Seed)
				LGA[i][j] = input[j];

			} else { // Calculate the rest
				if (j==0) {
					neighbor[0] = 0;
					neighbor[1] = LGA[i-1][j];
					neighbor[2] = LGA[i-1][j+1];
				} else if (j<DIM-1) {
					neighbor[0] = LGA[i-1][j-1];
					neighbor[1] = LGA[i-1][j];
					neighbor[2] = LGA[i-1][j+1];
				} else {
					neighbor[0] = LGA[i-1][j-1];
					neighbor[1] = LGA[i-1][j];
					neighbor[2] = 0;
				}

				LGA[i][j] = cellfunc(neighbor, rule);
			}

		}
	}
}

uint8_t cellfunc (const uint8_t *nb, const uint8_t *rule) {
	// Access and return value from LUT
	return rule[ convert(nb) ];
}

uint16_t convert (const uint8_t *nb) {
	// uint16_t can handle upto 65535
	// -> Which means this function can handle upto 40 colors
	return nb[0] + (nb[1]*K) + (nb[2]*K*K);
}

void cellprint (const uint8_t cell) {
	switch (cell) {
		case 0: // NULL
			// cout << (unsigned char)176;
			cout << '.';
			break;
		case 1: // LEFT
			// cout << (unsigned char)47;
			cout << 'L';
			break;
		case 2: // RIGHT
			// cout << (unsigned char)92;
			cout << 'R';
			break;
		case 3: // NAND
			// cout << (unsigned char)219;
			cout << 'N';
			break;
		case 4: // FORWARD
			// cout << (unsigned char)179;
			cout << 'F';
			break;
		case 5: // XOR
			// cout << (unsigned char)94;
			cout << 'X';
			break;
		default: // Undefined
			cout << 'U';
	}
}

void ca_graph (GeneticAlgorithm *pop, const int count) {
	printf("Array of the top %d individuals:\n", count);

	for (int i=0; i<count; i++) {
		printf("RANK: %4d | UID: %7d | FITNESS: %7d\n\n",
			pop[i].getrnk(), pop[i].getuid(), pop[i].getfit());

		cellgen (seed, pop[i].getdna());
		cellgen (LGA[DIM-1], pop[i].getdna());

		for (int y=0; y<DIM; y++) {
			for (int x=0; x<DIM; x++) {
				cellprint (LGA[y][x]);
			}
			cout << endl;
		}
		cout << endl;

	}

}
