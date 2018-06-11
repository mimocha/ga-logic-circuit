/* Main C++ file for evaluation program ran on DE0-SoC HPS system.
*/

#include <stdio.h>
#include <string.h>
#include <iostream>

int main (int argc, char* argv[]) {
	static int flag_i[2], flag_o[2], flag_d[2], flag_tt[2]; // option flags
	static int ic, oc;

	/* Option Arguments:
		flag_x[0] == Option "-x" was used
		flag_x[1] == Index location for option "-x"
	*/
	for (int idx=1; idx<argc; idx++) {
		if (strcmp(argv[idx], "-i") == 0) {
			flag_i[0] = 1; // Option "-i" exists
			flag_i[1] = idx; // Option is at array index "idx"
			continue;
		} else if (strcmp(argv[idx], "-o") == 0) {
			flag_o[0] = 1;
			flag_o[1] = idx;
			continue;
		} else if (strcmp(argv[idx], "-tt") == 0) {
			flag_tt[0] = 1;
			flag_tt[1] = idx;
			continue;
		} else if (strcmp(argv[idx], "-d") == 0) {
			flag_d[0] = 1;
			flag_d[1] = idx;
			continue;
		}
	}

	// Option -d: Debug
	if (flag_d[0]) {
		// Debug info
		printf("\n----- HPS Initialized | argc: %d -----\n", argc);
		for (int i=0; i<argc; i++) {
			std::cout << argv[i] << ' ';
		}
		std::cout << std::endl;
	}

	// Option -i: Input
	if (flag_i[0]) { // Load Value
		ic = strtol(argv[flag_i[1]+1], NULL, 10);
	}
	if (flag_d[0]) { // Debug Info
		if (flag_i[0] == 0) {
			printf("Input argument -i not found!\n");
			// return -1;
		} else if (ic <= 0) {
			printf("Invalid argument -i : %d\n", ic);
			// return -1;
		} else {
			printf("Argument -i: %d\n", ic);
		}
	}

	// Option -o: Output
	if (flag_o[0]) { // Load Value
		oc = strtol(argv[flag_o[1]+1], NULL, 10);
	}
	if (flag_d[0]) { // Debug Info
		if (flag_o[0] == 0) {
			printf("Input argument -o not found!\n");
			// return -1;
		} else if (oc <= 0) {
			printf("Invalid argument -o : %d\n", oc);
			// return -1;
		} else {
			printf("Argument -o: %d\n", oc);
		}
	}

	// Option -tt: Truth Table
	if (flag_tt[0]) {
		// Load truth table
	}
	if (flag_d[0]) {
		printf("Place holder for -tt\n");
	}

	// -tt to denote truth table inputs | total 2^i possible states
	// int state = 2;
	// for (int i=1; i<ic; i++)
	// 	state *= 2;
	// int tt[state][ic+oc] = {};
	// for (int i=1; i<argc; i++) {
	// 	if (strcmp(argv[i], "-tt") == 0) {
	// 		static int idx;
	// 		for (int j=0; j<state; j++) {
	// 			for (int k=0; k<ic+oc; k++) {
	// 				if (argv[i+1+idx] == NULL) // Undefined Catch
	// 					break;
	// 				tt[j][k] = strtol(argv[i+1+idx], NULL, 2);
	// 				idx++; // index variable for keeping track of tt variable
	// 			}
	// 		}
	// 		break;
	// 	} else if (i == argc-1) {
	// 		puts("Truth Table argument -tt not found!\n");
	// 		return -1;
	// 	}
	// }
	//
	// puts("TT: \n");
	// for (int i=0; i<state; i++) {
	// 	for (int j=0; j<ic+oc; j++) {
	// 		printf("%d ",tt[i][j]);
	// 	}
	// 	puts("\n");
	// }

	// Put Evaluation Code Here
	// All testing is internal to DE-HPS
	// Return results to PC as fitness score

	return 0;
}
