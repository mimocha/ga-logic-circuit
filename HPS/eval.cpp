/* Main C++ File for Evaluation Functions
	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

/* ========== Standard Library Include ========== */

#include <stdio.h>		// Standard I/O
#include <stdint.h>		// uint definitions
#include <math.h>		// floor
#include <iostream>		// cin, cout

/* ========== Custom Header Include ========== */

#include "eval.hpp"
#include "ansi.hpp"
#include "fast.hpp"
#include "fpga.hpp"
#include "global.hpp"
#include "truth.hpp"



// FPGA Grid Output Bit Size
#define GRID_BIT_SIZE 64

// Score Scaling -- Converts float score to an integer score
#define SCORE_MAX 10000

// Max Efficiency Score -- Defined as total number of cells
#define MAX_ES (PHYSICAL_DIMX*PHYSICAL_DIMY)

// How many clock cycles to wait between each input / output pair
#define MAX_WAIT 1024
#define MIN_WAIT 256
#define RAND_WAIT 256

// The maximum loop count for testing a sequential logic
#define MAX_SEQ_LOOP 5

namespace tt = TruthTable;



/* ========== Miscellany Functions ========== */

unsigned int get_score_max (void) {
	return SCORE_MAX;
}

unsigned int get_es_max (void) {
	return MAX_ES;
}



/* ========== Evaluation Functions ========== */

unsigned int eval_com (const unsigned short &sel) {
	const uint64_t *const input = tt::get_input();
	const uint64_t *const expect = tt::get_output();
	const uint16_t count = tt::get_row();
	const uint64_t mask = tt::get_mask();

	const float max_result = tt::get_max_bit();
	float result = 0;

	// Combinational circuits should work in any order, in any timestep
	// Running a few series of tests with random run cycles might fix the overfitting problem.
	// Ironic, considering randomness had just been eliminated.

	switch (sel) {
		case 0: goto ORDER;
		case 1: goto REVERSE;
		case 2: goto RANDOM;
		default:
			printf (ANSI_RED "\nUnrecognized eval_com() input!\n" ANSI_RESET);
			goto END;
	}

	ORDER:
	// In Given Order
	for (unsigned short i = 0 ; i < count ; i++) {
		fpga_set_input (input [i]);

		// Ending up reimplementing artificial randomness... how ironic.
		fpga_wind_clock (MIN_WAIT + (fast_rng32() % RAND_WAIT));

		uint64_t observed = fpga_get_output ();

		result += tt::bitcount64 ( ~(expect [i] ^ observed) & mask );
	}
	goto END;

	REVERSE:
	// In Reverse Order
	for (short i = count-1 ; i >= 0 ; i--) {
		fpga_set_input (input [i]);

		// Ending up reimplementing artificial randomness... how ironic.
		fpga_wind_clock (MIN_WAIT + (fast_rng32() % RAND_WAIT));

		uint64_t observed = fpga_get_output ();

		result += tt::bitcount64 ( ~(expect [i] ^ observed) & mask );
	}
	goto END;

	RANDOM:
	// Random test
	for (unsigned short i = 0 ; i < count ; i++) {
		const unsigned short rng = fast_rng32() % count;
		fpga_set_input (input [rng]);

		// Ending up reimplementing artificial randomness... how ironic.
		fpga_wind_clock (MIN_WAIT + (fast_rng32() % RAND_WAIT));

		uint64_t observed = fpga_get_output ();

		result += tt::bitcount64 ( ~(expect [rng] ^ observed) & mask );
	}

	END:
	return (unsigned int) (SCORE_MAX * (result / max_result));
}

unsigned int eval_seq (void) {
	const uint64_t *const input = tt::get_input();
	const uint64_t *const expect = tt::get_output();
	const uint16_t count = tt::get_row();
	const uint64_t mask = tt::get_mask();

	const float max_result = tt::get_max_bit() * MAX_SEQ_LOOP;
	float result = 0;

	// Repeats until a mistake is found, or the loop limit is reached
	for (unsigned int j = 0 ; j < MAX_SEQ_LOOP ; j++) {
		for (unsigned int i = 0 ; i < count ; i++) {
			fpga_set_input (input [i]);

			fpga_wind_clock (MIN_WAIT + (fast_rng32() % RAND_WAIT));

			const uint64_t observed = fpga_get_output ();

			const unsigned int bits_correct = tt::bitcount64 ( ~(expect [i] ^ observed) & mask );
			result += bits_correct;

			// Ends prematurely if a mistake is found
			if (bits_correct != tt::get_mask_bc()) goto END;
		}
	}

	END:
	return (unsigned int) (SCORE_MAX * (result / max_result));
}



/* ========== Inspect Evaluation Functions ========== */

void print_table (const uint64_t &input, const uint64_t &expect, const uint64_t &observed) {
	if ( (observed & tt::get_mask()) == (expect & tt::get_mask()) ) {
		printf ("\t0x%016llX | 0x%016llX | " ANSI_GREEN "0x%016llX\n" ANSI_RESET,
			input, expect, observed & tt::get_mask() );
	} else {
		printf ("\t0x%016llX | 0x%016llX | " ANSI_YELLOW "0x%016llX\n" ANSI_RESET,
			input, expect, observed & tt::get_mask() );
	}
}

unsigned int eval_com_insp (const unsigned short &sel) {
	const uint64_t *const input = tt::get_input();
	const uint64_t *const expect = tt::get_output();
	const uint16_t count = tt::get_row();
	const uint64_t mask = tt::get_mask();

	const float max_result = tt::get_max_bit();
	float result = 0;

	switch (sel) {
		case 0:
			printf ("\n\n\t\t\t   " ANSI_REVRS "Combinational | ORDER\n" ANSI_RESET);
			break;
		case 1:
			printf ("\n\n\t\t\t   " ANSI_REVRS "Combinational | REVERSE\n" ANSI_RESET);
			break;
		case 2:
			printf ("\n\n\t\t\t   " ANSI_REVRS "Combinational | RANDOM\n" ANSI_RESET);
			break;
	}

	printf (
		"\t             Input |      Expected      | Observed\n"
		"\t-------------------+--------------------+-------------------\n"
	);

	switch (sel) {
		case 0: goto ORDER;
		case 1: goto REVERSE;
		case 2: goto RANDOM;
	}

	ORDER:
	// In Given Order
	for (unsigned short i = 0 ; i < count ; i++) {
		fpga_set_input (input [i]);

		// Ending up reimplementing artificial randomness... how ironic.
		fpga_wind_clock (MIN_WAIT + (fast_rng32() % RAND_WAIT));

		uint64_t observed = fpga_get_output ();

		result += tt::bitcount64 ( ~(expect [i] ^ observed) & mask );

		print_table (input[i], expect[i], observed);
	}
	goto END;

	REVERSE:
	// In Reverse Order
	for (short i = count-1 ; i >= 0 ; i--) {
		fpga_set_input (input [i]);

		// Ending up reimplementing artificial randomness... how ironic.
		fpga_wind_clock (MIN_WAIT + (fast_rng32() % RAND_WAIT));

		uint64_t observed = fpga_get_output ();

		result += tt::bitcount64 ( ~(expect [i] ^ observed) & mask );

		print_table (input[i], expect[i], observed);
	}
	goto END;

	RANDOM:
	// Random test
	for (unsigned short i = 0 ; i < count ; i++) {
		const unsigned short rng = fast_rng32() % count;
		fpga_set_input (input [rng]);

		// Ending up reimplementing artificial randomness... how ironic.
		fpga_wind_clock (MIN_WAIT + (fast_rng32() % RAND_WAIT));

		uint64_t observed = fpga_get_output ();

		result += tt::bitcount64 ( ~(expect [rng] ^ observed) & mask );

		print_table (input[rng], expect[rng], observed);
	}

	END:
	printf ("\tScore: %3.0f / %3.0f | %5.2f%%\n", result, max_result, (result / max_result)*100);
	return (unsigned int) (SCORE_MAX * (result / max_result));
}

unsigned int eval_seq_insp (void) {
	const uint64_t *const input = tt::get_input();
	const uint64_t *const expect = tt::get_output();
	const uint16_t count = tt::get_row();
	const uint64_t mask = tt::get_mask();

	const float max_result = tt::get_max_bit() * MAX_SEQ_LOOP;
	float result = 0;
	unsigned int loop_count = 0;

	printf (
		"\n\n\t\t\t   " ANSI_REVRS "Sequential Truth Table\n" ANSI_RESET
		"\t             Input |      Expected      | Observed\n"
		"\t-------------------+--------------------+-------------------\n" );

	// Repeats until a mistake is found, or the loop limit is reached
	for (unsigned int i = 0 ; i < MAX_SEQ_LOOP ; i++) {
		for (unsigned int i = 0 ; i < count ; i++) {
			fpga_set_input (input [i]);

			fpga_wind_clock (MIN_WAIT + (fast_rng32() % RAND_WAIT));

			const uint64_t observed = fpga_get_output ();

			const unsigned int bits_correct = tt::bitcount64 ( ~(expect [i] ^ observed) & mask );
			result += bits_correct;

			print_table (input[i], expect[i], observed);

			// Ends prematurely if a mistake is found
			if (bits_correct != tt::get_mask_bc()) goto END;
		}

		loop_count++;
	}

	END:
	printf ("\n\tSuccesfully ran %u / %u times\n", loop_count, MAX_SEQ_LOOP);
	printf ("\tScore: %5.0f / %5.0f | %5.2f%%\n", result, max_result, (result / max_result)*100);
	return (unsigned int) (SCORE_MAX * (result / max_result));
}



/* ========== Efficiency Evaluation Functions ========== */

uint32_t eval_efficiency (const uint8_t *const *const grid) {
	uint32_t score = MAX_ES;

	for (uint16_t y = 0 ; y < PHYSICAL_DIMY ; y++) {
		for (uint16_t x = 0 ; x < PHYSICAL_DIMX ; x++) {

			// Add other values in here to penalize those settings
			if (grid[y][x] == 3) score--;

		}
	}

	return score;
}



/* ========== Reference F1 Scoring Functions ==========

const uint64_t *const input = tt::get_input();
const uint64_t *const expect = tt::get_output();
const uint16_t count = tt::get_row();
const uint64_t mask = tt::get_mask();

// True Positive, False Positive, False Negative
float tpos = 0;
float fpos = 0;
float fneg = 0;

for (unsigned int i = 0 ; i < count ; i++) {
	fpga_set_input (input [i]);
	uint64_t observed = fpga_get_output ();

	// Sums True Positive, False Positive, False Negative
	// Special case: If no bits are expected (expecting 0x00000000),
	// True Positive shall always be equal to 1
	if (expect [i] == 0) {
		tpos += 1;
	} else {
		tpos += tt::bitcount64 ( (expect [i] & observed) & mask );
	}
	fpos += tt::bitcount64 ( (~expect [i] &  observed) & mask );
	fneg += tt::bitcount64 (  (expect [i] & ~observed) & mask );
}

// Calculates the F1 Score
float precision	= tpos / (tpos + fpos);
float recall	= tpos / (tpos + fneg);
float f1_score	= 2 * precision * recall / (precision + recall);

return (uint32_t) floor (F1_MAX * f1_score);

*/
