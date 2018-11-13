/* Header File for Miscellaneous Functions

	MIT License

	Copyright (c) 2018 Chawit Leosrisook

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE

*/

#ifndef MISC_HPP
#define MISC_HPP

/* void about (void)
	Displays about message.
	Also displays certain internal variables for debug purposes.
*/
void about (void);

/* void input_argument (const int &argc, const char *const *const argv)
	This function handles input arguments.
	Should have limited use, now that a main menu is being implemented.
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

#endif
