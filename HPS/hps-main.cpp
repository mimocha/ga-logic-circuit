/* Main C++ file for evaluation program ran on DE0-SoC HPS system.
*/

#include <stdio.h>
#include <string.h>
#include <iostream>

int main (int argc, char* argv[]) {
	static int ic, oc;
	// Debug info
	// printf("\nHPS Initialized | argc: %d\n", argc);
	// for (int i=0; i<argc; i++) {
	// 	std::cout << argv[i] << ' ';
	// }
	// std::cout << std::endl;

	// -i to denote input count
	for (int i=1; i<argc; i++) {
		if (strcmp(argv[i], "-i") == 0) {
			ic = strtol(argv[i+1], NULL, 10);
			printf("Input argument -i == %ud\n", ic);
			break;
		} else if (i == argc-1) { // Error Catch
			puts("Input argument -i not found!\n");
			return -1;
		}
	}
	if (ic < 0) { // Error Catch
		printf("Invalid argument -o value: %d\n", ic);
		return -1;
	}

	// -o to denote output count
	for (int i=1; i<argc; i++) {
		if (strcmp(argv[i], "-o") == 0) {
			oc = strtol(argv[i+1], NULL, 10);
			printf("Output argument -o == %ud\n", oc);
			break;
		} else if (i == argc-1) { // Error Catch
			puts("Output argument -o not found!\n");
			return -1;
		}
	}
	if (oc < 0) { // Error Catch
		printf("Invalid argument -o value: %d\n", oc);
		return -1;
	}

	// -tt to denote truth table inputs | total 2^i possible states
	int state = 2;
	for (int i=1; i<ic; i++)
		state *= 2;
	int tt[state][ic+oc] = {};
	for (int i=1; i<argc; i++) {
		if (strcmp(argv[i], "-tt") == 0) {
			static int idx;
			for (int j=0; j<state; j++) {
				for (int k=0; k<ic+oc; k++) {
					if (argv[i+1+idx] == NULL) // Undefined Catch
						break;
					tt[j][k] = strtol(argv[i+1+idx], NULL, 2);
					idx++; // index variable for keeping track of tt variable
				}
			}
			break;
		} else if (i == argc-1) {
			puts("Truth Table argument -tt not found!\n");
			return -1;
		}
	}

	puts("TT: \n");
	for (int i=0; i<state; i++) {
		for (int j=0; j<ic+oc; j++) {
			printf("%d ",tt[i][j]);
		}
		puts("\n");
	}

	// Put Evaluation Code Here
	// All testing is internal to DE-HPS
	// Return results to PC as fitness score

	return 0;
}
