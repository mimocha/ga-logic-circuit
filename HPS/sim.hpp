/* Header File for Simulation Functions

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

#ifndef SIM_HPP
#define SIM_HPP

/* ========== Simulation Functions ========== */

void sim_init (void);

void sim_cleanup (void);

void sim_run (uint8_t *const *const grid, const uint8_t *const seed);

bool sim_is_done (void);

// static unsigned int count_solution (const GeneticAlgorithm *const array);



/* ========== Results & Reporting Function ========== */

// static void statistics (const GeneticAlgorithm *const array, const unsigned int &gen);

// static void report (uint8_t *const *const grid, const uint8_t *const seed);

void sim_results (void);

void sim_export (void);

bool export_is_done (void);



#endif
