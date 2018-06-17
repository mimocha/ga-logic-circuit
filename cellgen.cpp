/* Main C++ file for Cellular Automaton Generation Function CELLGEN
	Generates a grid of logic gates and connections based on given arguments.
*/

#include "cellgen.h"

ushort cellfunc (ushort nb[3], ushort rule);

void cellgen (ushort dim, ushort input[], ushort rule) {
	// Define
	ushort array[dim][dim];
	for (ushort i=0; i<dim; i++)
		array[0][i] = input[i];

	// Calculate
	ushort neighbor[3];
	for (ushort i=1; i<dim; i++) {
		for (ushort j=0; j<dim; j++) {
			if (j==0) {
				neighbor[0] = 0;
				neighbor[1] = array[i-1][j];
				neighbor[2] = array[i-1][j+1];
			} else if (j<dim-1) {
				neighbor[0] = array[i-1][j-1];
				neighbor[1] = array[i-1][j];
				neighbor[2] = array[i-1][j+1];
			} else {
				neighbor[0] = array[i-1][j-1];
				neighbor[1] = array[i-1][j];
				neighbor[2] = 0;
			}
			array[i][j] = cellfunc(neighbor, rule);
		}
		// cout << endl;
	}

	// Print
	// Prints only half of the array height
	for (ushort i=0; i<dim/2; i++) {
		for (ushort j=0; j<dim; j++) {
			if (array[i][j] == 1)
				cout << (unsigned char)219;
			else
				cout << (unsigned char)176;
		}
		cout << endl;
	}
}

ushort cellfunc (ushort nb[3], ushort rule) {
/* Function CELLFUNC(unsigned short neighbor[3], unsigned short rule)
	Takes in two inputs; an array of unsigned short integers - nb[3] - the neighbors, and a single
	unsigned short integer - rule. The array of integer "nb" is assumed to be length 3.

	> Main definition:
	"nb" points to the previous 3 cells directly above the currently considered cell.
		The three cells can be viewed as a series of 3 binary numbers.
			The value of this binary sequence represents the index of the "rule-bit". (See below)
	"rule" is some integer, which can be represented as a series of binary number.
		Each binary number is referred to as a "rule-bit".
		"rule_bit" index is a zero-based-index system, range [0,7]
		"rule_bit" counts from the right most digit to the left.

	> Example:
	nb[3] == {1, 0, 0} -> binary (100) -> decimal value (4)
	rule == 134 -> binary (1000 0110)
	rule_bit(0) == 0, rule_bit(1) == 1, rule_bit(2) == 1, rule_bit(6) == 0

	> How this works:
	"rule" determines whether or not some combination of neighbors (nb[3]) will output a 1 or 0.
	The "rule" can be thought of as a Look-Up-Table, with index starting from the right most digit.
	"nb" then is converted into a decimal value, representing the index number.
	We then use this index number to access the value from the Look-Up-Table "rule".

	> Example:
	nb[3] == {1, 1, 0} -> binary (110) -> decimal value (6)
	rule == 90 -> binary (0101 1010)
	We access "rule_bit" number 6 (converted value of "nb")
		rule_bit(6) == 1
	return result == rule_bit(6)
*/
	// Index for selecting rule-bit
	ushort idx;
	ushort pwr = 4*nb[0] + 2*nb[1] + nb[2];
	if (pwr > 0) {
		idx = 1;
		for (ushort i=0; i<pwr; i++)
			idx *= 2;
	} else {
		idx = 0;
	}

	// cout << (rule & idx);
	if ((rule & idx) == 0) {
		return 0;
	} else {
		return 1;
	}
}
