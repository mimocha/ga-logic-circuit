/* Miscellaneous Functions */

#ifndef MISC_CPP
#define MISC_CPP

void help_message (void) {
	printf("\n\t--- Genetics Algorithm Program Version %6.3f ---\n", VERSION);

	/* TODO: Add comprehensive explanation / help message */
}

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

#endif
