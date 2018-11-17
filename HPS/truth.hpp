/* Header File for Truth Table Functions
	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

#ifndef TRUTH_HPP
#define TRUTH_HPP

namespace TruthTable {

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
	void set_table (const int &input);

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



	/* ========== F1 ========== */

	/* bool get_f1 (void)
		Returns bool for current F1 setting.
	*/
	bool get_f1 (void);

	/* void set_f1 (const bool &set_val)
		Sets F1 toggle.
	*/
	void set_f1 (const bool &set_val);



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
}


#endif
