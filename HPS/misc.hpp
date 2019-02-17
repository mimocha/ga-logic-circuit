/* Header File for Miscellaneous Functions
	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

#ifndef MISC_HPP
#define MISC_HPP

/* void about (void)
	About message. Displays version & copyright info.
	Can be changed to display other messages, or removed entirely.
*/
void about (void);

/* void input_argument (const int &argc, const char *const *const argv)
	This function handles input arguments.

	> Not necessary.
	> Can be removed.
*/
void input_argument (const int &argc, const char *const *const argv);



/* ========== Sanitized Scanf & Overloads ==========
	Sanitized Scanf to parse input.
	Takes in a const pointer to a specified type of variable,
	ignores all but the first valid value.

	Can set values by passing pointers as argument, or by using return values.
	If a pointer is passed, no value will be returned.
	If no pointer is passed, the overload function returns the value of the type.

	The scanf's must be in seperated lines to work properly.
	1. Removes / ignores any preceeding non-digit characters from stdin
	2. Strictly takes in the first decimal digit, into an unsigned integer variable.
	3. Then clears stdin stream to remove any remaining characters, prevents looping
*/

void scan_int (int *const dest);
int scan_int (void);


void scan_uint (unsigned int *const dest);
unsigned int scan_uint (void);


void scan_float (float *const dest);
float scan_float (void);


/* This normally throws a GCC warning on compile time.
	Due to there not being a bool* type for scanf,
	so the (unsigned int*) cast is used to silence the warning.
	Ugly, but will have to do.
*/
void scan_bool (bool *const dest);
bool scan_bool (void);


void scan_hex (uint64_t *const dest);
uint64_t scan_hex (void);

#endif
