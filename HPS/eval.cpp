/* Main C++ File for Evaluation Functions

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

/* ========== Standard Library Include ========== */

#include <stdio.h>		/* Standard I/O */
#include <stdint.h>		/* uint definitions */
#include <math.h>		/* pow, round, ceiling */
#include <iostream>		/* cin, cout */

/* ========== Custom Header Include ========== */

#include "eval.hpp"
#include "ansi.hpp"
#include "fpga.hpp"

/* FPGA Grid Output Bit Size */
#define GRID_BIT_SIZE 64

/* ========== STATIC FUNCTION PROTOYPES ==========
	See documentation in header file "eval.hpp"
*/

static uint64_t bitcount64 (uint64_t x);



/* ========== Miscellany Functions ========== */

uint64_t bitcount64 (uint64_t x) {
	// Code shamelessly copied from https://en.wikipedia.org/wiki/Hamming_weight
	constexpr uint64_t m1  = 0x5555555555555555;
	constexpr uint64_t m2  = 0x3333333333333333;
	constexpr uint64_t m4  = 0x0f0f0f0f0f0f0f0f;

	x -= (x >> 1) & m1;             //put count of each 2 bits into those 2 bits
	x = (x & m2) + ((x >> 2) & m2); //put count of each 4 bits into those 4 bits
	x = (x + (x >> 4)) & m4;        //put count of each 8 bits into those 8 bits
	x += x >>  8;	//put count of each 16 bits into their lowest 8 bits
	x += x >> 16;	//put count of each 32 bits into their lowest 8 bits
	x += x >> 32;	//put count of each 64 bits into their lowest 8 bits
	return x & 0x7f;
}



/* ========== Basic Evaluation Functions ==========
	Evaluation functions for single values.
*/

uint32_t eval_bc (const uint64_t &input, const uint64_t &expect) {
	/* Set FPGA input */
	fpga_set_input (input);

	/* Get FPGA output
		Buffering 'fpga_set_input' and 'fpga_get_output' may be necessary.
		usleep(1) is already built into 'fpga_set_input' function, and should be enough.
	*/
	uint64_t observed = fpga_get_output ();

	/* Counts equivalent bits and return
		Using XNOR operation (bitwise equivalency), then count the equivalent bits.
		Returns the number of equivalent bits.
	*/
	return bitcount64 ( ~( expect ^ observed ) );
}

uint32_t eval_f1 (const uint64_t &input, const uint64_t &expect) {
	/* Set FPGA input */
	fpga_set_input (input);

	/* Get FPGA output
		Buffering 'fpga_set_input' and 'fpga_get_output' may be necessary.
		usleep(1) is already built into 'fpga_set_input' function, and should be enough.
	*/
	uint64_t observed = fpga_get_output ();

	/* Calculates F1 Score
	https://en.wikipedia.org/wiki/F1_score
	*/

	/* True Positive, False Positive, False Negative */
	float tpos = bitcount64 ( ~(expect ^ observed) &    (expect) );
	float fpos = bitcount64 (  (expect ^ observed) &  (observed) );
	float fneg = bitcount64 (  (expect ^ observed) & ~(observed) );

	/* Recall, Precision, F1 Score */
	float precision	= tpos / (tpos + fpos);
	float recall	= tpos / (tpos + fneg);
	float f1_score	= 2 * precision * recall / (precision + recall);

	/* Returns f1 score */
	return (uint32_t) floor (F1_MAX * f1_score);
}



/* ========== Array Evaluation Functions ==========
	Evaluation function for multiple values at once.
*/

uint32_t eval_bc_array
	(const uint64_t *const input, const uint64_t *const expect, const uint16_t &count) {

	uint32_t result = 0;

	for (unsigned int i = 0 ; i < count ; i++) {
		/* Set FPGA input */
		fpga_set_input (input [i]);

		/* Get FPGA output
			Buffering 'fpga_set_input' and 'fpga_get_output' may be necessary.
			usleep(1) is already built into 'fpga_set_input' function, and should be enough.
		*/
		uint64_t observed = fpga_get_output ();

		/* Counts equivalent bits
			Using XNOR operation (bitwise equivalency), then count the equivalent bits.
		*/
		result += bitcount64 ( ~( expect [i] ^ observed ) );
	}

	/* Return sum of equivalent bits */
	return result;
}

uint32_t eval_f1_array
	(const uint64_t *const input, const uint64_t *const expect, const uint16_t &count) {

	/* Declares & Defines True Positive, False Positive, False Negative */
	float tpos = 0;
	float fpos = 0;
	float fneg = 0;

	/* Iterates over each case */
	for (unsigned int i = 0 ; i < count ; i++) {
		/* Set FPGA input */
		fpga_set_input (input [i]);

		/* Get FPGA output
			Buffering 'fpga_set_input' and 'fpga_get_output' may be necessary.
			usleep(1) is already built into 'fpga_set_input' function, and should be enough.
		*/
		uint64_t observed = fpga_get_output ();

		/* Calculates F1 Score -- https://en.wikipedia.org/wiki/F1_score */

		/* Sums True Positive, False Positive, False Negative */
		tpos += bitcount64 ( ~(expect[i] ^ observed) &  (expect[i]) );
		fpos += bitcount64 (  (expect[i] ^ observed) &  (observed) );
		fneg += bitcount64 (  (expect[i] ^ observed) & ~(observed) );
	}

	/* Recall, Precision, F1 Score */
	float precision	= tpos / (tpos + fpos);
	float recall	= tpos / (tpos + fneg);
	float f1_score	= 2 * precision * recall / (precision + recall);

	/* Returns f1 score */
	return (uint32_t) floor (F1_MAX * f1_score);
}



/* ========== Inspect Evaluation Functions ==========
	Array evaluation functions with additional stylized truth table print.
	Does not return results.
*/

void eval_bc_insp
	(const uint64_t *const input, const uint64_t *const expect, const uint16_t &count) {

	/* Results variable */
	uint32_t result = 0;

	/* Maximum Fitness Value */
	const uint32_t fit_lim = GRID_BIT_SIZE * count;

	printf (
		"\n\n\t\t\t\t" ANSI_REVRS "Truth Table\n" ANSI_RESET
		"\t             Input |      Expected      | Observed\n"
		"\t-------------------+--------------------+-------------------\n" );

	/* Iterates over each case */
	for (unsigned int i = 0 ; i < count ; i++) {
		/* Set FPGA input */
		fpga_set_input (input [i]);

		/* Get FPGA output
			Buffering 'fpga_set_input' and 'fpga_get_output' may be necessary.
			usleep(1) is already built into 'fpga_set_input' function, and should be enough.
		*/
		uint64_t observed = fpga_get_output ();

		/* Counts equivalent bits
			Using XNOR operation (bitwise equivalency), then count the equivalent bits.
		*/
		result += bitcount64 ( ~( expect [i] ^ observed ) );

		/* Compare result with expectation & print table */
		if ( observed == expect [i] ) {
			printf ("\t0x%016llX | 0x%016llX | " ANSI_GREEN "0x%016llX\n" ANSI_RESET,
				input [i], expect [i], observed );
		} else {
			printf ("\t0x%016llX | 0x%016llX | " ANSI_YELLOW "0x%016llX\n" ANSI_RESET,
				input [i], expect [i], observed );
		}
	}

	printf ("\n\tFitness: %u / %u\n", result, fit_lim);

	return;
}

void eval_f1_insp
	(const uint64_t *const input, const uint64_t *const expect, const uint16_t &count) {

	/* Declares & Defines True Positive, False Positive, False Negative */
	float tpos = 0;
	float fpos = 0;
	float fneg = 0;

	printf (
		"\n\n\t\t\t\t" ANSI_REVRS "Truth Table\n" ANSI_RESET
		"\t             Input |      Expected      | Observed\n"
		"\t-------------------+--------------------+-------------------\n" );

	/* Iterates over each case */
	for (unsigned int i = 0 ; i < count ; i++) {
		/* Set FPGA input */
		fpga_set_input (input [i]);

		/* Get FPGA output
			Buffering 'fpga_set_input' and 'fpga_get_output' may be necessary.
			usleep(1) is already built into 'fpga_set_input' function, and should be enough.
		*/
		uint64_t observed = fpga_get_output ();

		/* Calculates F1 Score -- https://en.wikipedia.org/wiki/F1_score */

		/* Sums True Positive, False Positive, False Negative */
		tpos += bitcount64 ( ~(expect[i] ^ observed) &  (expect[i]) );
		fpos += bitcount64 (  (expect[i] ^ observed) &  (observed) );
		fneg += bitcount64 (  (expect[i] ^ observed) & ~(observed) );

		/* Compare result with expectation & print table */
		if ( observed == expect [i] ) {
			printf ("\t0x%016llX | 0x%016llX | " ANSI_GREEN "0x%016llX\n" ANSI_RESET,
				input [i], expect [i], observed );
		} else {
			printf ("\t0x%016llX | 0x%016llX | " ANSI_YELLOW "0x%016llX\n" ANSI_RESET,
				input [i], expect [i], observed );
		}
	}

	/* Recall, Precision, F1 Score */
	float precision	= tpos / (tpos + fpos);
	float recall	= tpos / (tpos + fneg);
	float f1_score	= 2 * precision * recall / (precision + recall);

	/* Scales result with F1 score */
	uint32_t result = (uint32_t) floor (F1_MAX * f1_score);
	printf ("\n\tFitness: %u / %u\n", result, F1_MAX);

	return;
}
