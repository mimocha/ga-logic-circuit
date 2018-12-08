/* Header File for Evaluation Functions
	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

#ifndef EVAL_HPP
#define EVAL_HPP

/* ========== Miscellany Functions ========== */

/* unsigned int get_score_max (void)
	Return the maximum score scale
*/
unsigned int get_score_max (void);

/* unsigned int get_efficiency_max (void)
	Returns the maximum efficiency score
*/
unsigned int get_es_max (void);



/* ========== Evaluation Functions ========== */

/* unsigned int eval_com (const unsigned short &sel);
	Evaluation for combinational logic.
	Tests the truth table in random different orders, should be random enough to stop overfitting

	Counts the number of correct bits over the entire test,
	divided by the maximum possible correct bit count,
	returns a scaled score of the tested solution.

	Considerably more time consuming than any other tests before this,
	hopefully it will be comprehensive enough.
*/
unsigned int eval_com (const unsigned short &sel);

/* unsigned int eval_seq (void);
	Evaluation for sequential logic.
	Repeats the truth table over and over until the unit fails, or until the limit is reached.

	This should be a reliable enough test to see if the system is actually a sequential logic.
*/
unsigned int eval_seq (void);



/* ========== Inspect Evaluation Functions ==========
	Evaluation functions with additional stylized truth table print.
*/

/* unsigned int eval_com_insp (const unsigned short &sel);
	Similar to the above function, while printing a stylized truth table.
*/
unsigned int eval_com_insp (const unsigned short &sel);

/* unsigned int eval_seq_insp (void);
	Similar to the above function, while printing a stylized truth table.
*/
unsigned int eval_seq_insp (void);



/* ========== Efficiency Evaluation Functions ========== */

/* uint16_t eval_efficiency (const uint8_t *const *const grid);
	Efficiency evaluation metric.
	Set to evaluate which solution is more gate efficient.
	Using less number of gate is better.
*/
uint16_t eval_efficiency (const uint8_t *const *const grid);

#endif
