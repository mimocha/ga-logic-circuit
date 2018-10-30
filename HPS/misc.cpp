/* Miscellaneous Functions */

void help_message (void) {
	printf("\n\t--- Genetics Algorithm Program Version %6.3f ---\n", VERSION);
	/* TODO: Add comprehensive explanation / help message */
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
