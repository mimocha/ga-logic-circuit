/* Header file for the Genetics Algorithm main file

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

#ifndef GAMAIN_HPP
#define GAMAIN_HPP

/* ========== Main Menu / Initialize / Cleanup Functions ========== */

/* static void main_init (void)
	Initializes main file.
*/
static void main_init (void);

/* static void main_cleanup (void)
	Cleanup global variable memory before exiting program.
*/
static void main_cleanup (void);

/* static unsigned int main_menu (void)
	Main Menu function.
	Users select inputs here. Selections are unsigned decimal integers.
	The selection is then returned to main() for calling other functions.

	The actual selection and function calling should not occur within this function,
	but within the main() function to keep the code clean and easy to follow... ?
*/
static unsigned int main_menu (void);



/* ========== Main Menu Option ========== */

/* static void settings (void)
	Handles changing of global parameters.
*/
static void settings (void);

/* static void inspect (uint8_t *const *const grid, const uint8_t *const seed)
	DNA inspection function.
	Practically a smaller version of run_sim() on its own.
	given a DNA string, this function will apply CA to it,
	generate a logic circuit, and evaluate the circuits truth table.
*/
static void inspect (uint8_t *const *const grid, const uint8_t *const seed);



/* ========== Special Routine ========== */

/* static void special (void)
	Special pre-programmed routines.
	Program things here for longer experiments.
*/
static void special (void);

#endif
