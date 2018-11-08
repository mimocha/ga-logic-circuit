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

/* ========== Standard Library Include ========== */

#include <stdio.h>		/* Standard I/O */
#include <stdlib.h>		/* calloc, free, rand, srand */
#include <stdint.h>		/* uint definitions */
#include <time.h>		/* time */
#include <math.h>		/* pow, round, ceiling */
#include <iostream>		/* cin, cout */
#include <algorithm>	/* sort, find */
#include <vector>		/* vectors */
#include <array>		/* arrays */
#include <cstring>		/* strcmp */



// --------------------------------------
// Simulation
// --------------------------------------

/*
	Restricted to "sim.cpp" to prevent confusion with other initialization functions.
	Created this function because the initialization codes are getting out of hand.
*/
static void sim_init (void);

/*
	Wrapper for running the simulation.
*/
bool run_sim (void);

/*
	Wrapper function for end of simulation cleanups.
	Dynamically allocated memories are freed here.
*/
static void sim_cleanup (void);



// --------------------------------------
// Report
// --------------------------------------

/* void statistics (GeneticAlgorithm *array, const unsigned int gen)
	Stores statistics of the current simulation.
*/
void statistics (GeneticAlgorithm *array, const unsigned int gen);

/* void report (void)
	Wrapper function for end of simulation report.
*/
void report (void);

#endif
