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
		case 0x0:
			cout << ANSI_RED;
			break;
		case 0x1:
			cout << ANSI_GREEN;
			break;
		case 0x2:
			cout << ANSI_BLUE;
			break;
		case 0x3:
			cout << ANSI_PURPL;
			break;
		case 0x8:
			cout << ANSI_LRED;
			break;
		case 0x9:
			cout << ANSI_LGREEN;
			break;
		case 0xA:
			cout << ANSI_LBLUE;
			break;
		case 0xB:
			cout << ANSI_LPURPL;
			break;
		default: // Undefined
			cout << ANSI_WHITE;
			break;
	}

	printf("%X",cell);
	cout << ANSI_RESET;
}

void ca_graph (const uint8_t *array, const unsigned int length) {
	for (unsigned int x=0; x<length; x++) {
		cellprint (array[x]);
	}
	std::cout << std::endl;
}
