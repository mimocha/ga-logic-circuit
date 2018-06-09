/* Main C++ file for Genetic Algorithm Program.
	Controls all functions within this project.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Project definitions
#define PVAR "GeneticAlgorithm" //Project Name
#define PLOC " .\\CV_GeneticAlgorithm\\GeneticAlgorithm" //Project Location
#define QMAP "quartus_map"
#define QFIT "quartus_fit"
#define QASM "quartus_asm"
#define QPGM "quartus_pgm"

char *cmdconcat (const char *QCMD, const char *LOC, const char *VAR);

int main (void) {
	puts("\n\n----- Genetic Algorithm v0.01 Initialized -----\n\n");

	puts("\n----- Running Quartus Script -----\n");

	char buffer[256];
	strcpy(buffer, cmdconcat(QMAP, PLOC, PVAR));
	system(buffer);
	strcpy(buffer, cmdconcat(QFIT, PLOC, PVAR));
	system(buffer);
	strcpy(buffer, cmdconcat(QASM, PLOC, PVAR));
	system(buffer);
	// system(cmdconcat(QMAP, PLOC, PVAR));

	system("pause");

	return 0;
}

char *cmdconcat (const char *QCMD, const char *LOC, const char *VAR) {
	char cmdbuf[256];
	char *ptr = cmdbuf;
	strcpy(cmdbuf, QCMD);
	strcat(cmdbuf, LOC);
	strcat(cmdbuf, " -c ");
	strcat(cmdbuf, VAR);
	return ptr;
}
