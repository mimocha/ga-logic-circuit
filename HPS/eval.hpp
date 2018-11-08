/* Header File for Evaluation Functions

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

#ifndef EVAL_HPP
#define EVAL_HPP

/* ========== Miscellany Functions ========== */

/* uint64_t bitcount64 (uint64_t x)
	64-bit Hamming Weight Counting Algorithm
	https://en.wikipedia.org/wiki/Hamming_weight

	Counts how many bits are set.
	Input (Expected XNOR Observed) as value to count correct bits.
*/
// uint64_t bitcount64 (uint64_t x);



/* ========== Basic Evaluation Functions ========== */

/* uint32_t eval_bc (const uint64_t input, const uint64_t expect)
	Simple bit counting evaluation.

	Evaluates the FPGA output, given:
		const uint64_t input -- 64-bit value to set the FPGA input
		const uint64_t expect -- 64-bit expected output from the FPGA
	Reads a 64-bit output from the FPGA, and compares 'output' with 'expect'.

	The number of equivalent bits is returned.
*/
uint32_t eval_bc (const uint64_t input, const uint64_t expect);

/* uint32_t eval_f1 (const uint64_t input, const uint64_t expect)
	Evaluates with F1 scoring.
	More computationally expensive, but better at discriminating small differences.

	Evaluates the FPGA output, given:
		const uint64_t input -- 64-bit value to set the FPGA input
		const uint64_t expect -- 64-bit expected output from the FPGA
	Reads a 64-bit output from the FPGA, and compares 'output' with 'expect' using F1 scoring.
		https://en.wikipedia.org/wiki/F1_score

	uint32_t (F1 score * F1_MAX) is returned.
*/
uint32_t eval_f1 (const uint64_t input, const uint64_t expect);



/* ========== Array Evaluation Functions ========== */

/* uint32_t eval_bc_array
	(const uint64_t *const input, const uint64_t *const expect, const uint16_t count)

	Simple bit counting evaluation for arrays.

	Evaluates the FPGA output, given:
		const uint64_t input -- 64-bit value to set the FPGA input
		const uint64_t expect -- 64-bit expected output from the FPGA
	Reads a 64-bit output from the FPGA, and compares 'output' with 'expect'.
	Repeats over 'count' iterations.

	The sum of numbers of equivalent bits is returned.
*/
uint32_t eval_bc_array
	(const uint64_t *const input, const uint64_t *const expect, const uint16_t count);

/* uint32_t eval_f1_array
	(const uint64_t *const input, const uint64_t *const expect, const uint16_t count)

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
	(const uint64_t *const input, const uint64_t *const expect, const uint16_t count);


/* ========== Inspect Evaluation Functions ========== */

/* void eval_bc_insp
	(const uint64_t *const input, const uint64_t *const expect, const uint16_t count)

	Similar to eval_bc_array().
	Except this function also prints out a stylized truth table.
	For manual inspection of input / output.
	Does not return a result.
*/
void eval_bc_insp
	(const uint64_t *const input, const uint64_t *const expect, const uint16_t count);

/* void eval_f1_insp
	(const uint64_t *const input, const uint64_t *const expect, const uint16_t count);

	Similar to eval_f1_array().
	Except this function also prints out a stylized truth table.
	For manual inspection of input / output.
	Does not return a result.
*/
void eval_f1_insp
	(const uint64_t *const input, const uint64_t *const expect, const uint16_t count);

#endif
