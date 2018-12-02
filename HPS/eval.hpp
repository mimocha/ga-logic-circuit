/* Header File for Evaluation Functions
	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

#ifndef EVAL_HPP
#define EVAL_HPP



/* ========== Miscellany Functions ========== */

/* static uint64_t bitcount64 (uint64_t x);
	64-bit Hamming Weight Counting Algorithm
	https://en.wikipedia.org/wiki/Hamming_weight

	Counts how many bits are set.
	Input (Expected XNOR Observed) as value to count correct bits.
*/

/* uint16_t get_f1_max (void)
	Return F1 maximum score
*/
uint16_t get_f1_max (void);

/* uint16_t get_efficiency_max (void)
	Returns the maximum efficiency score
*/
uint16_t get_efficiency_max (void);



/* ========== Array Evaluation Functions ==========
	Evaluation function for multiple values at once.
*/

/* ===== WITHOUT MASK ===== */

/* uint32_t eval_bc_array
	(const uint64_t *const input, const uint64_t *const expect, const uint16_t &count);

	Simple bit counting evaluation for arrays.

	Evaluates the FPGA output, given:
		const uint64_t input -- 64-bit value to set the FPGA input
		const uint64_t expect -- 64-bit expected output from the FPGA
	Reads a 64-bit output from the FPGA, and compares 'output' with 'expect'.
	Repeats over 'count' iterations.

	The sum of numbers of equivalent bits is returned.
*/
uint32_t eval_bc_array
	(const uint64_t *const input, const uint64_t *const expect, const uint16_t &count);

/* uint32_t eval_f1_array
	(const uint64_t *const input, const uint64_t *const expect, const uint16_t &count);

	Evaluates with F1 scoring for arrays.
	More computationally expensive, but better at discriminating small differences.

	Evaluates the FPGA output, given:
		const uint64_t input -- 64-bit value to set the FPGA input
		const uint64_t expect -- 64-bit expected output from the FPGA
	Reads a 64-bit output from the FPGA, and compares 'output' with 'expect' using F1 scoring.
		https://en.wikipedia.org/wiki/F1_score
	Repeats over 'count' iterations. Then calculates a single F1 score for the entire array.

	uint32_t (F1 score * F1_MAX) is returned.
*/
uint32_t eval_f1_array
	(const uint64_t *const input, const uint64_t *const expect, const uint16_t &count);

/* ===== WITH MASK =====
	Masked functions are overload functions which only evaluates the masked bits.
	Masked bits are set to 0, and effectively ignored.
	These bits can express any logical function with no repercussion to the solution fitness.

	Maximum Fitness Values should be adjusted accordingly.
	ie, Masking 63 bits would leave 1 bit to be evaluated, thus maximum fitness should be out of this single bit.
*/

uint32_t eval_bc_array
	(const uint64_t *const input, const uint64_t *const expect,
		const uint16_t &count, const uint64_t &mask);

uint32_t eval_f1_array
	(const uint64_t *const input, const uint64_t *const expect,
		const uint16_t &count, const uint64_t &mask);



/* ========== Inspect Evaluation Functions ==========
	Array evaluation functions with additional stylized truth table print.
	Does not return results.
*/

/* ===== WITHOUT MASK ===== */

/* void eval_bc_insp
	(const uint64_t *const input, const uint64_t *const expect, const uint16_t &count);

	Similar to eval_bc_array().
	Except this function also prints out a stylized truth table.
	For manual inspection of input / output.
	Does not return a result.
*/
void eval_bc_insp
	(const uint64_t *const input, const uint64_t *const expect, const uint16_t &count);

/* void eval_f1_insp
	(const uint64_t *const input, const uint64_t *const expect, const uint16_t &count);

	Similar to eval_f1_array().
	Except this function also prints out a stylized truth table.
	For manual inspection of input / output.
	Does not return a result.
*/
void eval_f1_insp
	(const uint64_t *const input, const uint64_t *const expect, const uint16_t &count);

/* ===== WITH MASK =====
	Masked functions are overload functions which only evaluates the masked bits.
	Masked bits are set to 0, and effectively ignored.
	These bits can express any logical function with no repercussion to the solution fitness.

	Maximum Fitness Values should be adjusted accordingly.
	ie, Masking 63 bits would leave 1 bit to be evaluated, thus maximum fitness should be out of this single bit.
*/

void eval_bc_insp
	(const uint64_t *const input, const uint64_t *const expect,
		const uint16_t &count, const uint64_t &mask);

void eval_f1_insp
	(const uint64_t *const input, const uint64_t *const expect,
		const uint16_t &count, const uint64_t &mask);



/* ========== Efficiency Evaluation Functions ========== */

uint32_t eval_efficiency (const uint8_t *const *const grid);

#endif
