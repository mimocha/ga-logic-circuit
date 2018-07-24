/* Miscellany Functions file
*/

// ----- Initialize ----- //

enum option_args {
	help, // help
	show_c, // CA graph
	show_f, // fitness tracking
	show_t, // time
	show_r, // results
	show_d, // debug
	null // null
};

option_args handling (string const& inString) {
	// Argument handling functions and enum
	if (inString == "help") return help;
	if (inString == "-h") return help;
	if (inString == "--help") return help;
	if (inString == "-c") return show_c;
	if (inString == "-f") return show_f;
	if (inString == "-t") return show_t;
	if (inString == "-r") return show_r;
	if (inString == "-d") return show_d;
	return null;
}

void Msg (void) {
	// Displays help message
	puts("\n\n Please select atleast 1 option to run \n");
	puts(" 'Option' Name\t-> Short Explanation");
	puts(" '-c' CA Graph\t-> Graphs Cellular Automaton");
	puts(" '-d' Debug\t-> Display debug prints at various points");
	puts(" '-f' Fitness Tracking\t-> Tracks fitness value of the population");
	puts(" '-h' Help\t-> Shows in-depth help message");
	puts(" '-r' Results\t-> Display the results of Genetic Algorithm");
	puts(" '-t' Timer\t-> Times the program execution and display the result");
	puts("\n\n");
	exit(EXIT_SUCCESS);
}

void Help (void) {
	printf(" --- Genetics Algorithm Version %6.3f ---\n", VERSION);

	printf(" 'c'\tCellular Automaton Display\n"
			"\tPrints Cellular Automaton grid of generated cells\n"
	);

	exit(EXIT_SUCCESS);
}

void init (int argc, char **argv) {
	// Input Argument Handlings
	if (argc > 1) {
		for (int idx=1; idx<argc; idx++) {
			switch ( handling(argv[idx]) ) {
				case help:
					Help();
					break;
				case show_c:
					SHOW_C = 1;
					break;
				case show_f:
					SHOW_F = 1;
					break;
				case show_t:
					SHOW_T = 1;
					break;
				case show_r:
					SHOW_R = 1;
					break;
				case show_d:
					SHOW_D = 1;
					break;
				default:
					break;
			}
		}
	} else {
		// Displays short help message if no options are chosen
		Msg();
	}

	// Initializes RNG
	srand(time(NULL));

	// Initializes Seed CA
	seed[DIM/2] = K-1;

	printf("COLOR = %d | POPULATION = %d | GENERATION LIMIT = %d\n"
		"CA GRAPH = %d | FITNESS = %d | TIMER = %d | RESULTS = %d | DEBUG = %d\n",
		K, POP, GEN_LIM, SHOW_C, SHOW_F, SHOW_T, SHOW_R, SHOW_D);
	printf("\n\n----- Genetic Algorithm v%5.3f Initialized -----\n\n", VERSION);
}

// ----- Trackers ----- //

void track_fitness (GeneticAlgorithm *pop, const uint16_t gen) {
	// Handles zero-index and generation starting at 1
	const uint16_t idx = gen - 1;

	// Maximum Fitness of each generation
	maxfit[idx] = pop[0].getfit();
	// Minimum Fitness of each generation
	minfit[idx] = pop[POP-1].getfit();
	// Median Fitness
	if (POP%2==0) {
		medfit[idx] = ( pop[POP/2].getfit() + pop[(POP/2)-1].getfit() )/2;
	} else {
		medfit[idx] = pop[ (uint16_t)floor(POP/2) ].getfit();
	}
	// Mean Fitness
	avgfit[idx] = 0;
	for (int i=0; i<POP; i++) {
		avgfit[idx] += pop[i].getfit();
	}
	avgfit[idx] /= POP;

	printf("Max: %u | Min: %u | Median: %u | Avg: %u \n",
	maxfit[idx], minfit[idx], medfit[idx], avgfit[idx]);
}

void disp_fitness (void) {
	uint16_t x, y, i, j;

	// ----- Fitness Statistics Table ----- //
	puts("\n\t <--- Fitness Statistics --->\n");
	puts("  Gen | Maximum | Median | Minimum | Average");
	puts("--------------------------------------------");
	for (i=0; i<GEN_LIM; i++) {
		printf(" %4u | %7u | %6u | %7u | %7u \n",
		i+1, maxfit[i], medfit[i], minfit[i], avgfit[i]);

		// Fancy shorten break in the middle
		if ((GEN_LIM > 50) && (i > 10)) {
			puts("\t\t.\n\t\t.\n\t\t.\n");

			for (j=GEN_LIM-21; j<GEN_LIM; j++) {
					printf(" %4u | %7u | %6u | %7u | %7u \n",
					j+1, maxfit[j], medfit[j], minfit[j], avgfit[j]);
			}

			break;
		}
	}

	// ----- Prints pretty fitness function graph ----- //
	/* The hard part was figuring out a universal way to scale up and down both axis.
	Done by using a scale constant x_scale and y_scale to perform operations on the working array.

	> Scales the y-axis by dividing the values of the array (similar to image shrinking),
	then subtract the smallest value of the array to zero the graph on the origin.
	Use the floor function to ensure data stays within output range.

	> Scales the x-axis by dividing the index of the array by x_scale,
	then rounding down with floor(). This effectively "scales" the array itself,
	by changing which grid on the graph some data point is loaded onto.

	> (GYDIM - y) is used as a loading index. As loading is done from top to bottom of the array,
	Using this is effectively the same as using the loop for(y=GYDIM-1; y>=0; y--)

	> Graph is loaded onto as first a 4-bit binary value.
	Where each category of value - max, med, min, avg - would flip the bits for each cell,
	if they are chosen to be displayed on the cell. This is done with the bitwise OR function.
	This bit value is then passed through a switch-case
	which translates the bit value into some ASCII character, to be printed.
	*/

	// Use a working array because process is destructive to data
	// (Leaves an intact copy of data if we decide to use it later.)
	float workarray[4][GEN_LIM] = {0};

	// Load into work array in this order: MAX - MED - MIN - AVG
	for (x=0; x<GEN_LIM; x++) {
		workarray[0][x] = maxfit[x];
		workarray[1][x] = medfit[x];
		workarray[2][x] = minfit[x];
		workarray[3][x] = avgfit[x];
	}

	const uint16_t x_range = GEN_LIM;
	const uint16_t y_range = *max_element(maxfit, maxfit + GEN_LIM)
	 - *min_element(minfit, minfit + GEN_LIM);
	const uint16_t y_max = *max_element(maxfit, maxfit + GEN_LIM);
	const uint16_t y_min = *min_element(minfit, minfit + GEN_LIM);

	// Scale > 1 == Stretch | Scale == 1 == Maintain | Scale < 1 == Compress
	const float x_scale = (float)GXDIM/x_range;
	const float y_scale = (float)GYDIM/y_range;

	unsigned char graph[GYDIM][GXDIM] = {0};

	// Scale Y
	// Multiply by scaling factor to scale input range to output domain.
	// Subtract the minimum value to translate plot down to origin (0).
	for (y=0; y<GEN_LIM; y++) {
		workarray[0][y] = floor (y_scale * workarray[0][y] - (y_min * y_scale) );
		workarray[1][y] = floor (y_scale * workarray[1][y] - (y_min * y_scale) );
		workarray[2][y] = floor (y_scale * workarray[2][y] - (y_min * y_scale) );
		workarray[3][y] = floor (y_scale * workarray[3][y] - (y_min * y_scale) );
	}

	// Scale X & load onto graph
	for (y=0; y<GYDIM; y++) {
		for (x=0; x<GXDIM; x++) {
			// Load value
			// Scaled X index; skips or merges x-axis based on scale value.
			i = (int)floor(x/x_scale);
			if (workarray[0][i] == GYDIM-y) graph[y][x] |= 1;
			if (workarray[1][i] == GYDIM-y) graph[y][x] |= 2;
			if (workarray[2][i] == GYDIM-y) graph[y][x] |= 4;
			if (workarray[3][i] == GYDIM-y) graph[y][x] |= 8;

			// Convert to ASCII
			switch (graph[y][x]) {
				case 0: // NULL
					graph[y][x] = 0;
					break;
				case 1: // MAX
					graph[y][x] = '+';
					break;
				case 2: // MEDIAN
					graph[y][x] = 'M';
					break;
				case 4: // MIN
					graph[y][x] = '-';
					break;
				case 8: // AVG
					graph[y][x] = 'A';
					break;
				default: // Other mixture
					graph[y][x] = '*';
					break;
			}
		}
	}

	// Printing
	// This part is pretty hacky, lots of hard-coded stuff
	puts("\n\t <--- Fitness Graph --->\n");
	printf("\tLegend: + Max | - Min | M Median | A Average | * Mixed |"
	" Scale XY: %f %f\n\n", x_scale, y_scale);

	for (y=0; y<=GYDIM+2; y++) {
		for (x=0; x<GXDIM+2; x++) {

			// Graph Actual
			if (y < GYDIM) {
				switch (x) {
					case 0: // Y-Axis Name - 2 Spaces
						if (y <= (GYDIM/2+3) && y >= (GYDIM/2-3)) {
							switch (y - (GYDIM/2-3)) {
								case 0:
									cout << " F";
									break;
								case 1:
									cout << " I";
									break;
								case 2:
									cout << " T";
									break;
								case 3:
									cout << " N";
									break;
								case 4:
									cout << " E";
									break;
								case 5:
									cout << " S";
									break;
								case 6:
									cout << " S";
									break;
							}
						} else {
							cout << "  ";
						}
						break;
					case 1: // Y-Axis Label - 9 Spaces
						// Prints label every set interval
						if (y%5 == 0) printf(" %6.0f >", y_max - (y/y_scale));
						else printf("        |");
						break;
					default: // Graph Actual - GXDIM Spaces
						cout << graph[y][x-2];
						break;
				}

			// Graph Border - 11 Spaces total
			} else if (y == GYDIM) {
				switch (x) {
					case 0: // Y-Axis Name's 2 Spaces
						cout << "  ";
						break;
					case 1: // Y-Axis Corner Label - 9 Spaces
						printf(" %6.0f +", y_max - (y/y_scale));
						break;
					default: // Graph Border - GXDIM Spaces
						if ((x-1)%10 == 0 ) cout << '^';
						else cout << '-';
						break;
				}

			// X-Axis Value Labels
			} else if (y == GYDIM+1) {
				switch (x) {
					case 0: // Y-Axis Name's 2 Spaces
						cout << "  "; // 2 Spaces
						break;
					case 1: // Y-Axis Label's 9 Spaces
						cout << "        0"; // 8 Spaces + 1
						break;
					default: // X-Axis Label - GXDIM Spaces
						if ((x-1)%10 == 0) printf("%10.0f",floor((x-1)/x_scale));
				}

			// X-Axis Name
			} else if (y == GYDIM+2) {
				if (x < (GXDIM)/2 + 11) {
					cout << ' ';
				} else {
					printf("GENERATION");
					break;
				}
			}
		}
		// Endline for Row Y
		cout << endl;
	}

}

void disp_time (clock_t timer, const int mode) {
	switch (mode) {
		case 0: // Stop & display timer
			timer = clock () - timer;
			printf("\t%ld Clicks | %f Seconds\n",
				timer, ((float)timer)/CLOCKS_PER_SEC);
			return;
		case 1: // Stop & display timer stats
			timer = clock () - timer;
			printf("\t%ld Clicks | %f Seconds\n",
				timer, ((float)timer)/CLOCKS_PER_SEC);
			printf ("\tAverage %f Clicks per GEN\n",
				((float)timer)/GEN_LIM);
			printf ("\tAverage %f Seconds per GEN\n",
				((float)timer)/CLOCKS_PER_SEC/GEN_LIM);
			return;
	}
}

void disp_results (GeneticAlgorithm *pop) {
	puts(" RANK |   UID   | FITNESS | DNA");

	for (int i=0; i<10; i++) {
		// Prints the status for each individual
		printf(" %4d | %7d | %7d | ",
		pop[i].getrnk(), pop[i].getuid(), pop[i].getfit());
		// Prints the dna string
		uint8_t *dna = pop[i].getdna();
		for (int j=0; j<K_CUBE; j++) {
			if (dna[j] < 16) {
				printf("%X", dna[j]);
			} else {
				cout << 'X';
			}
			// Limits to 64 character...
			if (j >= 64) {
				cout << " ... ";
				break;
			}
		}
		cout << endl;
	}

	printf("\t\t.\n\t\t.\n\t\t.\n");
	cout << endl;
}
