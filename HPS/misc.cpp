/* Miscellaneous Functions */

void help_message (void) {
	printf ("\n\e[7m\t--- Genetics Algorithm Program Version %4.2f ---\e[0m\n\n", VERSION);

	/* Explanation & Legal */
	printf (
	"\tThis program applies Genetics Algorithm, Cellular Automaton, and more\n"
	"\tto generate logic circuits. This is create as part of a Bachelor's degree\n"
	"\tdissertation project, at Nippon Institute of Technology, 2018-2019.\n\n"
	"\t\e[7mMIT License\e[0m\n\n"
	"\t\e[7mCopyright (c) 2018 Chawit Leosrisook\e[0m\n\n"
	"\tPermission is hereby granted, free of charge, to any person obtaining a copy\n"
	"\tof this software and associated documentation files (the \"Software\"), to deal\n"
	"\tin the Software without restriction, including without limitation the rights\n"
	"\tto use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n"
	"\tcopies of the Software, and to permit persons to whom the Software is\n"
	"\tfurnished to do so, subject to the following conditions:\n\n"
	"\tThe above copyright notice and this permission notice shall be included in all\n"
	"\tcopies or substantial portions of the Software.\n\n"
	"\tTHE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
	"\tIMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
	"\tFITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
	"\tAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
	"\tLIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
	"\tOUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
	"\tSOFTWARE\n"
	);

	/* Print internal Variables for quick debug use */
	if (global.tt_init == 1) {
		printf (ANSI_REVRS "\n\t\t\tTruth Table" ANSI_RESET
				"\n\t\t\tInput \t|\t Output\n");
		for (unsigned int row = 0; row < global.truth.step; row++) {
			printf ("\t%016llX \t|\t %016llX\n",
			global.truth.input [row], global.truth.output [row]);
		}
	}
}

void input_argument (const int argc, char **argv) {
	int counter;

	printf ("%d input argument(s) given:\n", argc);
	for (counter=0; counter<argc; counter++)
		printf ("argv[%d] : %s\n", counter, argv[counter]);
}

/* ----- Sanitized Scanf ----- */

int *scan_int (int *dest) {
	scanf ("%*[^0-9]");
	scanf ("%d[0-9]", dest);
	scanf ("%*[^\n]");
	return dest;
}

unsigned int *scan_uint (unsigned int *dest) {
	scanf ("%*[^0-9]");
	scanf ("%u[0-9]", dest);
	scanf ("%*[^\n]");
	return dest;
}

float *scan_float (float *dest) {
	scanf ("%*[^0-9]");
	scanf ("%f", dest);
	scanf ("%*[^\n]");
	return dest;
}

/* This normally throws a GCC warning on compile time.
	Due to there not being a bool* type for scanf,
	so the (unsigned int*) cast is used to silence the warning.
	Ugly, but will have to do.
*/
bool *scan_bool (bool *dest) {
	scanf ("%*[^0-1]");
	scanf ("%1u[0-1]", (unsigned int*)dest);
	scanf ("%*[^\n]");
	return dest;
}
