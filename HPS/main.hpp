/* Header file for the Genetics Algorithm main file
	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

#ifndef MAIN_HPP
#define MAIN_HPP

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
