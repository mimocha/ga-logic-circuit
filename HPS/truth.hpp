/* Header File for Truth Table Functions
	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

#ifndef TRUTH_HPP
#define TRUTH_HPP

namespace TruthTable {

	/* ========== Helper Functions ========== */

	/* uint64_t bitcount64 (uint64_t x);
		64-bit Hamming Weight Counting Algorithm
		https://en.wikipedia.org/wiki/Hamming_weight

		Counts how many bits are set.
		Input (Expected XNOR Observed) as value to count correct bits.
	*/
	uint64_t bitcount64 (uint64_t x);



	/* ========== Read File / Clear Table / Table Status ========== */

	/* void set_table (void)
		Set Truth Table from file

		File must be in the following format:
		-----------------
		input output <row count>
		<input value 1> <output value 1>
		<input value 2> <output value 2>
		<input value 3> <output value 3>
		...
		-----------------

		Whitespaces are automatically trimmed.
		Header row is Case Sensitive!
		Missing columns will return a failure.
		Input / Output values are required to be in hexadecimals.
	*/
	void set_table (void);

	/* int auto_set_table (const int &set);
		Automatically sets preprogrammed truth tables.
	*/
	int auto_set_table (const int &set);

	/* void clear_table (void)
		Clears currently set truth table.
		Frees INPUT and OUTPUT arrays.
		Sets INIT to zero.
	*/
	void clear_table (void);

	/* bool table_is_init (void)
		Returns Initialization status of truth table.
	*/
	bool table_is_init (void);



	/* ========== Mode ========== */

	/* bool get_mode (void)
		Returns bool for current mode setting, combinational or sequential.
		0 == Combinational
		1 == Sequential
	*/
	bool get_mode (void);

	/* void set_mode (const bool &set_val)
		Toggles between combinational and sequential logic mode.
		0 == Combinational
		1 == Sequential
	*/
	void set_mode (const bool &set_val);



	/* ========== Input / Output Array ========== */

	/* uint64_t *get_input (void)
		Returns current INPUT array.
	*/
	uint64_t *get_input (void);

	/* void set_input (const uint64_t *const set_val)
		Sets new INPUT array.
		Currently does nothing.
	*/
	void set_input (const uint64_t *const set_val);

	/* uint64_t *get_output (void)
		Returns current OUTPUT array.
	*/
	uint64_t *get_output (void);

	/* void set_output (const uint64_t *const set_val)
		Sets new OUTPUT array.
		Currently does nothing.
	*/
	void set_output (const uint64_t *const set_val);



	/* ========== Truth Table Row Count ========== */

	/* unsigned int get_row (void)
		Returns current unsigned int ROW value.
	*/
	unsigned int get_row (void);

	/* void set_row (const unsigned int &set_val)
		Sets new unsigned int ROW value.
	*/
	void set_row (const unsigned int &set_val);



	/* ========== Mask ========== */

	/* uint64_t get_mask (void)
		Returns uint64_t MASK value.
	*/
	uint64_t get_mask (void);

	/* void set_mask (const uint64_t &set_val)
		Sets uint64_t MASK value, and calculates how many bits are set for MASK.
	*/
	void set_mask (const uint64_t &set_val);

	/* uint64_t get_mask_bc (const bool &inverse)
		Returns how many bits are set for MASK.

		Input bool is 0 -> Returns MASK bitcount
		Input bool is 1 -> Returns ~MASK bitcount (Inversed)

		Default input value is 0. Return normal bitcount.
	*/
	uint64_t get_mask_bc (const bool &inverse = 0);



	/* ========== Max Bit Score ========== */

	/* unsigned int get_max_bit (void)
		Returns maximum bitcount score.
		Calculated as (Number of truth table rows) * (Mask bitcount)
	*/
	unsigned int get_max_bit (void);
}


#endif
