/* Main C++ File for Evaluation Functions
	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
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



/* ========== STATIC FUNCTION PROTOYPES ========== */

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

/* ===== WITHOUT MASK ===== */

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

	/* Calculates F1 Score -- https://en.wikipedia.org/wiki/F1_score */

	/* Sums True Positive, False Positive, False Negative
		Special case: If no bits are expected (expecting 0x00000000),
		True Positive shall always be equal to 1
	*/
	float tpos;
	if (expect == 0) {
		tpos = 1;
	} else {
		tpos = bitcount64 ( expect & observed ); // ~(expect ^ observed) & (expect)
	}
	float fpos = bitcount64 ( ~expect &  observed ); // (expect ^ observed) & (observed)
	float fneg = bitcount64 (  expect & ~observed ); // (expect ^ observed) & ~(observed)

	/* Recall, Precision, F1 Score */
	float precision	= tpos / (tpos + fpos);
	float recall	= tpos / (tpos + fneg);
	float f1_score	= 2 * precision * recall / (precision + recall);

	/* Returns f1 score */
	return (uint32_t) floor (F1_MAX * f1_score);
}

/* ===== WITH MASK ===== */

uint32_t eval_bc (const uint64_t &input, const uint64_t &expect, const uint64_t &mask) {
	fpga_set_input (input);

	uint64_t observed = fpga_get_output ();

	return bitcount64 ( ~( expect ^ observed ) & mask );
}

uint32_t eval_f1 (const uint64_t &input, const uint64_t &expect, const uint64_t &mask) {
	fpga_set_input (input);

	uint64_t observed = fpga_get_output ();

	float tpos;
	if (expect == 0) {
		tpos = 1;
	} else {
		tpos = bitcount64 ( (expect & observed) & mask );
	}
	float fpos = bitcount64 ( (~expect &  observed) & mask );
	float fneg = bitcount64 (  (expect & ~observed) & mask );

	float precision	= tpos / (tpos + fpos);
	float recall	= tpos / (tpos + fneg);
	float f1_score	= 2 * precision * recall / (precision + recall);

	return (uint32_t) floor (F1_MAX * f1_score);
}



/* ========== Array Evaluation Functions ==========
	Evaluation function for multiple values at once.
*/

/* ===== WITHOUT MASK ===== */

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

		/* Sums True Positive, False Positive, False Negative
			Special case: If no bits are expected (expecting 0x00000000),
			True Positive shall always be equal to 1
		*/
		if (expect [i] == 0) {
			tpos += 1;
		} else {
			tpos += bitcount64 ( expect [i] & observed );
		}

		fpos += bitcount64 ( ~expect [i] &  observed );
		fneg += bitcount64 (  expect [i] & ~observed );
	}

	/* Recall, Precision, F1 Score */
	float precision	= tpos / (tpos + fpos);
	float recall	= tpos / (tpos + fneg);
	float f1_score	= 2 * precision * recall / (precision + recall);

	/* Returns f1 score */
	return (uint32_t) floor (F1_MAX * f1_score);
}

/* ===== WITH MASK ===== */

uint32_t eval_bc_array
	(const uint64_t *const input, const uint64_t *const expect,
		const uint16_t &count, const uint64_t &mask) {
	uint32_t result = 0;

	for (unsigned int i = 0 ; i < count ; i++) {
		fpga_set_input (input [i]);
		uint64_t observed = fpga_get_output ();
		result += bitcount64 ( ~( expect [i] ^ observed ) & mask );
	}

	return result;
}

uint32_t eval_f1_array
	(const uint64_t *const input, const uint64_t *const expect,
		const uint16_t &count, const uint64_t &mask) {
	float tpos = 0;
	float fpos = 0;
	float fneg = 0;

	for (unsigned int i = 0 ; i < count ; i++) {
		fpga_set_input (input [i]);

		uint64_t observed = fpga_get_output ();

		if (expect [i] == 0) {
			tpos += 1;
		} else {
			tpos += bitcount64 ( (expect [i] & observed) & mask );
		}

		fpos += bitcount64 ( (~expect [i] &  observed) & mask );
		fneg += bitcount64 (  (expect [i] & ~observed) & mask );
	}

	float precision	= tpos / (tpos + fpos);
	float recall	= tpos / (tpos + fneg);
	float f1_score	= 2 * precision * recall / (precision + recall);

	return (uint32_t) floor (F1_MAX * f1_score);
}



/* ========== Inspect Evaluation Functions ==========
	Array evaluation functions with additional stylized truth table print.
	Does not return results.
*/

/* ===== WITHOUT MASK ===== */

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

		/* Sums True Positive, False Positive, False Negative
			Special case: If no bits are expected (expecting 0x00000000),
			True Positive shall always be equal to 1
		*/
		if (expect [i] == 0) {
			tpos += 1;
		} else {
			tpos += bitcount64 ( expect [i] & observed );
		}
		fpos += bitcount64 ( ~expect [i] &  observed );
		fneg += bitcount64 (  expect [i] & ~observed );

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

/* ===== WITH MASK ===== */

void eval_bc_insp
	(const uint64_t *const input, const uint64_t *const expect,
		const uint16_t &count, const uint64_t &mask) {
	uint32_t result = 0;
	const uint32_t fit_lim = (GRID_BIT_SIZE - bitcount64 (~mask)) * count ;

	printf (
		"\n\n\t\t\t\t" ANSI_REVRS "Truth Table\n" ANSI_RESET
		"\t             Input |      Expected      | Observed\n"
		"\t-------------------+--------------------+-------------------\n" );

	for (unsigned int i = 0 ; i < count ; i++) {
		fpga_set_input (input [i]);
		uint64_t observed = fpga_get_output ();
		result += bitcount64 ( ~( expect [i] ^ observed ) & mask );

		if ( (observed & mask) == (expect [i] & mask) ) {
			printf ("\t0x%016llX | 0x%016llX | " ANSI_GREEN "0x%016llX\n" ANSI_RESET,
				input [i], expect [i], observed & mask );
		} else {
			printf ("\t0x%016llX | 0x%016llX | " ANSI_YELLOW "0x%016llX\n" ANSI_RESET,
				input [i], expect [i], observed & mask );
		}
	}
	printf ("\n\tFitness: %u / %u\n", result, fit_lim);
	printf ("\n\tMask: %016llX (%llu bits)\n", mask, bitcount64(mask) );

	return;
}

void eval_f1_insp
	(const uint64_t *const input, const uint64_t *const expect,
		const uint16_t &count, const uint64_t &mask) {
	float tpos = 0;
	float fpos = 0;
	float fneg = 0;

	printf (
		"\n\n\t\t\t\t" ANSI_REVRS "Truth Table\n" ANSI_RESET
		"\t             Input |      Expected      | Observed\n"
		"\t-------------------+--------------------+-------------------\n" );

	for (unsigned int i = 0 ; i < count ; i++) {
		fpga_set_input (input [i]);
		uint64_t observed = fpga_get_output ();

		if (expect [i] == 0) {
			tpos += 1;
		} else {
			tpos += bitcount64 ( (expect [i] & observed) & mask );
		}
		fpos += bitcount64 ( (~expect [i] &  observed) & mask );
		fneg += bitcount64 (  (expect [i] & ~observed) & mask );

		if ( (observed & mask) == (expect [i] & mask) ) {
			printf ("\t0x%016llX | 0x%016llX | " ANSI_GREEN "0x%016llX\n" ANSI_RESET,
				input [i], expect [i], observed & mask );
		} else {
			printf ("\t0x%016llX | 0x%016llX | " ANSI_YELLOW "0x%016llX\n" ANSI_RESET,
				input [i], expect [i], observed & mask );
		}
	}

	float precision	= tpos / (tpos + fpos);
	float recall	= tpos / (tpos + fneg);
	float f1_score	= 2 * precision * recall / (precision + recall);

	uint32_t result = (uint32_t) floor (F1_MAX * f1_score);
	printf ("\n\tFitness: %u / %u\n", result, F1_MAX);
	printf ("\n\tMask: %016llX (%llu bits)\n", mask, bitcount64(mask) );

	return;
}
