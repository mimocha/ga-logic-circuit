/*
 * Simple Java program for analyzing synthesized circuit
 * Repo: https://github.com/mimocha/ga-logic-circuit
 * Copyright (c) 2019 Chawit Leosrisook
 */
package circuit.analysis;

/**
 *
 * @author mimocha
 */
public class NAND {
	
//	0 | FALSE
//	1 | NOR
//	2 | A_AND_NB
//	3 | NOT_B
//	4 | B_AND_NA
//	5 | NOT_A
//	6 | XOR
//	7 | NAND
//	8 | AND
//	9 | XNOR
//	A | A
//	B | A_OR_NB
//	C | B
//	D | B_OR_NA
//	E | OR
//	F | TRUE
	
// NAND Combinations Reference
//	-|0|1|2|3|4|5|6|7|8|9|A|B|C|D|E|F|
//	----------------------------------
//	0|F|F|F|F|F|F|F|F|F|F|F|F|F|F|F|F|
//	1|F|E|F|E|F|E|F|E|F|E|F|E|F|E|F|E|
//	2|F|F|D|D|F|F|D|D|F|F|D|D|F|F|D|D|
//	3|F|E|D|C|F|E|D|C|F|E|D|C|F|E|D|C|
//	4|F|F|F|F|B|B|B|B|F|F|F|F|B|B|B|B|
//	5|F|E|F|E|B|A|B|A|F|E|F|E|B|A|B|A|
//	6|F|F|D|D|B|B|9|9|F|F|D|D|B|B|9|9|
//	7|F|E|D|C|B|A|9|8|F|E|D|C|B|A|9|8|
//	8|F|F|F|F|F|F|F|F|7|7|7|7|7|7|7|7|
//	9|F|E|F|E|F|E|F|E|7|6|7|6|7|6|7|6|
//	A|F|F|D|D|F|F|D|D|7|7|5|5|7|7|5|5|
//	B|F|E|D|C|F|E|D|C|7|6|5|4|7|6|5|4|
//	C|F|F|F|F|B|B|B|B|7|7|7|7|3|3|3|3|
//	D|F|E|F|E|B|A|B|A|7|6|7|6|3|2|3|2|
//	E|F|F|D|D|B|B|9|9|7|7|5|5|3|3|1|1|
//	F|F|E|D|C|B|A|9|8|7|6|5|4|3|2|1|0|
	
	// ---------------------------------------------------------
	// NAND LOGIC METHODS
	// ---------------------------------------------------------
	//	The following is a horrifying stack of switch-case logic,
	//	because 4-bit unsigned integers in Java is messy.
	//	Should've done this in C++ with bitwise ops.
	
	public static void logic (int logic_left, int logic_right, Cell c) {
		
		if (logic_left == Cell.UNDET) return;
		if (logic_right == Cell.UNDET) return;
		
		switch (logic_left) {
			case Cell.FALSE:
				FALSE(logic_right, c);
				return;
			case Cell.NOR:
				NOR(logic_right, c);
				return;
			case Cell.A_AND_NB:
				A_AND_NB(logic_right, c);
				return;
			case Cell.NOT_B:
				NOT_B(logic_right, c);
				return;
			case Cell.B_AND_NA:
				B_AND_NA(logic_right, c);
				return;
			case Cell.NOT_A:
				NOT_A(logic_right, c);
				return;
			case Cell.XOR:
				XOR(logic_right, c);
				return;
			case Cell.NAND:
				NAND(logic_right, c);
				return;
			case Cell.AND:
				AND(logic_right, c);
				return;
			case Cell.XNOR:
				XNOR(logic_right, c);
				return;
			case Cell.A:
				A(logic_right, c);
				return;
			case Cell.A_OR_NB:
				A_OR_NB(logic_right, c);
				return;
			case Cell.B:
				B(logic_right, c);
				return;
			case Cell.B_OR_NA:
				B_OR_NA(logic_right, c);
				return;
			case Cell.OR:
				OR(logic_right, c);
				return;
			case Cell.TRUE:
				TRUE(logic_right, c);
				return;
		}
	}
	
	private static void FALSE (int logic, Cell c) {
		c.constant = true;
		c.logic = Cell.TRUE;
	}
	
	private static void NOR (int logic, Cell c) {
		c.constant = true;
		switch (logic%2) {
			case 0:
				c.logic = Cell.TRUE;
				return;
			case 1:
				c.logic = Cell.OR;
		}
	}
	
	private static void A_AND_NB (int logic, Cell c) {
		c.constant = true;
		switch (logic) {
			case Cell.UNDET:
				c.constant = false;
				return;
			case Cell.FALSE:
				c.logic = Cell.TRUE;
				return;
			case Cell.NOR:
				c.logic = Cell.TRUE;
				return;
			case Cell.A_AND_NB:
				c.logic = Cell.B_OR_NA;
				return;
			case Cell.NOT_B:
				c.logic = Cell.B_OR_NA;
				return;
			case Cell.B_AND_NA:
				c.logic = Cell.TRUE;
				return;
			case Cell.NOT_A:
				c.logic = Cell.TRUE;
				return;
			case Cell.XOR:
				c.logic = Cell.B_OR_NA;
				return;
			case Cell.NAND:
				c.logic = Cell.B_OR_NA;
				return;
			case Cell.AND:
				c.logic = Cell.TRUE;
				return;
			case Cell.XNOR:
				c.logic = Cell.TRUE;
				return;
			case Cell.A:
				c.logic = Cell.B_OR_NA;
				return;
			case Cell.A_OR_NB:
				c.logic = Cell.B_OR_NA;
				return;
			case Cell.B:
				c.logic = Cell.TRUE;
				return;
			case Cell.B_OR_NA:
				c.logic = Cell.TRUE;
				return;
			case Cell.OR:
				c.logic = Cell.B_OR_NA;
				return;
			case Cell.TRUE:
				c.logic = Cell.B_OR_NA;
		}
	}
	
	private static void NOT_B (int logic, Cell c) {
		c.constant = true;
		switch (logic) {
			case Cell.UNDET:
				c.constant = false;
				return;
			case Cell.FALSE:
				c.logic = Cell.TRUE;
				return;
			case Cell.NOR:
				c.logic = Cell.OR;
				return;
			case Cell.A_AND_NB:
				c.logic = Cell.B_OR_NA;
				return;
			case Cell.NOT_B:
				c.logic = Cell.B;
				return;
			case Cell.B_AND_NA:
				c.logic = Cell.TRUE;
				return;
			case Cell.NOT_A:
				c.logic = Cell.OR;
				return;
			case Cell.XOR:
				c.logic = Cell.B_OR_NA;
				return;
			case Cell.NAND:
				c.logic = Cell.B;
				return;
			case Cell.AND:
				c.logic = Cell.TRUE;
				return;
			case Cell.XNOR:
				c.logic = Cell.OR;
				return;
			case Cell.A:
				c.logic = Cell.B_OR_NA;
				return;
			case Cell.A_OR_NB:
				c.logic = Cell.B;
				return;
			case Cell.B:
				c.logic = Cell.TRUE;
				return;
			case Cell.B_OR_NA:
				c.logic = Cell.OR;
				return;
			case Cell.OR:
				c.logic = Cell.B_OR_NA;
				return;
			case Cell.TRUE:
				c.logic = Cell.B;
				return;
		}
	}
	
	private static void B_AND_NA (int logic, Cell c) {
		c.constant = true;
		switch (logic) {
			case Cell.UNDET:
				c.constant = false;
				return;
			case Cell.FALSE:
				c.logic = Cell.TRUE;
				return;
			case Cell.NOR:
				c.logic = Cell.TRUE;
				return;
			case Cell.A_AND_NB:
				c.logic = Cell.TRUE;
				return;
			case Cell.NOT_B:
				c.logic = Cell.TRUE;
				return;
			case Cell.B_AND_NA:
				c.logic = Cell.A_OR_NB;
				return;
			case Cell.NOT_A:
				c.logic = Cell.A_OR_NB;
				return;
			case Cell.XOR:
				c.logic = Cell.A_OR_NB;
				return;
			case Cell.NAND:
				c.logic = Cell.A_OR_NB;
				return;
			case Cell.AND:
				c.logic = Cell.TRUE;
				return;
			case Cell.XNOR:
				c.logic = Cell.TRUE;
				return;
			case Cell.A:
				c.logic = Cell.TRUE;
				return;
			case Cell.A_OR_NB:
				c.logic = Cell.TRUE;
				return;
			case Cell.B:
				c.logic = Cell.A_OR_NB;
				return;
			case Cell.B_OR_NA:
				c.logic = Cell.A_OR_NB;
				return;
			case Cell.OR:
				c.logic = Cell.A_OR_NB;
				return;
			case Cell.TRUE:
				c.logic = Cell.A_OR_NB;
				return;
		}
	}
	
	private static void NOT_A (int logic, Cell c) {
		c.constant = true;
		switch (logic) {
			case Cell.UNDET:
				c.constant = false;
				return;
			case Cell.FALSE:
				c.logic = Cell.TRUE;
				return;
			case Cell.NOR:
				c.logic = Cell.OR;
				return;
			case Cell.A_AND_NB:
				c.logic = Cell.TRUE;
				return;
			case Cell.NOT_B:
				c.logic = Cell.OR;
				return;
			case Cell.B_AND_NA:
				c.logic = Cell.A_OR_NB;
				return;
			case Cell.NOT_A:
				c.logic = Cell.A;
				return;
			case Cell.XOR:
				c.logic = Cell.A_OR_NB;
				return;
			case Cell.NAND:
				c.logic = Cell.A;
				return;
			case Cell.AND:
				c.logic = Cell.TRUE;
				return;
			case Cell.XNOR:
				c.logic = Cell.OR;
				return;
			case Cell.A:
				c.logic = Cell.TRUE;
				return;
			case Cell.A_OR_NB:
				c.logic = Cell.OR;
				return;
			case Cell.B:
				c.logic = Cell.A_OR_NB;
				return;
			case Cell.B_OR_NA:
				c.logic = Cell.A;
				return;
			case Cell.OR:
				c.logic = Cell.A_OR_NB;
				return;
			case Cell.TRUE:
				c.logic = Cell.A;
				return;
		}
	}
	
	private static void XOR (int logic, Cell c) {
		c.constant = true;
		switch (logic) {
			case Cell.UNDET:
				c.constant = false;
				return;
			case Cell.FALSE:
				c.logic = Cell.TRUE;
				return;
			case Cell.NOR:
				c.logic = Cell.TRUE;
				return;
			case Cell.A_AND_NB:
				c.logic = Cell.B_OR_NA;
				return;
			case Cell.NOT_B:
				c.logic = Cell.B_OR_NA;
				return;
			case Cell.B_AND_NA:
				c.logic = Cell.A_OR_NB;
				return;
			case Cell.NOT_A:
				c.logic = Cell.A_OR_NB;
				return;
			case Cell.XOR:
				c.logic = Cell.XNOR;
				return;
			case Cell.NAND:
				c.logic = Cell.XNOR;
				return;
			case Cell.AND:
				c.logic = Cell.TRUE;
				return;
			case Cell.XNOR:
				c.logic = Cell.TRUE;
				return;
			case Cell.A:
				c.logic = Cell.B_OR_NA;
				return;
			case Cell.A_OR_NB:
				c.logic = Cell.B_OR_NA;
				return;
			case Cell.B:
				c.logic = Cell.A_OR_NB;
				return;
			case Cell.B_OR_NA:
				c.logic = Cell.A_OR_NB;
				return;
			case Cell.OR:
				c.logic = Cell.XNOR;
				return;
			case Cell.TRUE:
				c.logic = Cell.XNOR;
				return;
		}
	}
	
	private static void NAND (int logic, Cell c) {
		c.constant = true;
		switch (logic) {
			case Cell.UNDET:
				c.constant = false;
				return;
			case Cell.FALSE:
				c.logic = Cell.TRUE;
				return;
			case Cell.NOR:
				c.logic = Cell.OR;
				return;
			case Cell.A_AND_NB:
				c.logic = Cell.B_OR_NA;
				return;
			case Cell.NOT_B:
				c.logic = Cell.B;
				return;
			case Cell.B_AND_NA:
				c.logic = Cell.A_OR_NB;
				return;
			case Cell.NOT_A:
				c.logic = Cell.A;
				return;
			case Cell.XOR:
				c.logic = Cell.XNOR;
				return;
			case Cell.NAND:
				c.logic = Cell.AND;
				return;
			case Cell.AND:
				c.logic = Cell.TRUE;
				return;
			case Cell.XNOR:
				c.logic = Cell.OR;
				return;
			case Cell.A:
				c.logic = Cell.B_OR_NA;
				return;
			case Cell.A_OR_NB:
				c.logic = Cell.B;
				return;
			case Cell.B:
				c.logic = Cell.A_OR_NB;
				return;
			case Cell.B_OR_NA:
				c.logic = Cell.A;
				return;
			case Cell.OR:
				c.logic = Cell.XNOR;
				return;
			case Cell.TRUE:
				c.logic = Cell.AND;
				return;
		}
	}
	
	private static void AND (int logic, Cell c) {
		c.constant = true;
		switch (logic) {
			case Cell.UNDET:
				c.constant = false;
				return;
			case Cell.FALSE:
				c.logic = Cell.TRUE;
				return;
			case Cell.NOR:
				c.logic = Cell.TRUE;
				return;
			case Cell.A_AND_NB:
				c.logic = Cell.TRUE;
				return;
			case Cell.NOT_B:
				c.logic = Cell.TRUE;
				return;
			case Cell.B_AND_NA:
				c.logic = Cell.TRUE;
				return;
			case Cell.NOT_A:
				c.logic = Cell.TRUE;
				return;
			case Cell.XOR:
				c.logic = Cell.TRUE;
				return;
			case Cell.NAND:
				c.logic = Cell.TRUE;
				return;
			case Cell.AND:
				c.logic = Cell.NAND;
				return;
			case Cell.XNOR:
				c.logic = Cell.NAND;
				return;
			case Cell.A:
				c.logic = Cell.NAND;
				return;
			case Cell.A_OR_NB:
				c.logic = Cell.NAND;
				return;
			case Cell.B:
				c.logic = Cell.NAND;
				return;
			case Cell.B_OR_NA:
				c.logic = Cell.NAND;
				return;
			case Cell.OR:
				c.logic = Cell.NAND;
				return;
			case Cell.TRUE:
				c.logic = Cell.NAND;
				return;
		}
	}
	
	private static void XNOR (int logic, Cell c) {
		c.constant = true;
		switch (logic) {
			case Cell.UNDET:
				c.constant = false;
				return;
			case Cell.FALSE:
				c.logic = Cell.TRUE;
				return;
			case Cell.NOR:
				c.logic = Cell.OR;
				return;
			case Cell.A_AND_NB:
				c.logic = Cell.TRUE;
				return;
			case Cell.NOT_B:
				c.logic = Cell.OR;
				return;
			case Cell.B_AND_NA:
				c.logic = Cell.TRUE;
				return;
			case Cell.NOT_A:
				c.logic = Cell.OR;
				return;
			case Cell.XOR:
				c.logic = Cell.TRUE;
				return;
			case Cell.NAND:
				c.logic = Cell.OR;
				return;
			case Cell.AND:
				c.logic = Cell.NAND;
				return;
			case Cell.XNOR:
				c.logic = Cell.XOR;
				return;
			case Cell.A:
				c.logic = Cell.NAND;
				return;
			case Cell.A_OR_NB:
				c.logic = Cell.XOR;
				return;
			case Cell.B:
				c.logic = Cell.NAND;
				return;
			case Cell.B_OR_NA:
				c.logic = Cell.XOR;
				return;
			case Cell.OR:
				c.logic = Cell.NAND;
				return;
			case Cell.TRUE:
				c.logic = Cell.XOR;
				return;
		}
	}
	
	private static void A (int logic, Cell c) {
		c.constant = true;
		switch (logic) {
			case Cell.UNDET:
				c.constant = false;
				return;
			case Cell.FALSE:
				c.logic = Cell.TRUE;
				return;
			case Cell.NOR:
				c.logic = Cell.TRUE;
				return;
			case Cell.A_AND_NB:
				c.logic = Cell.B_OR_NA;
				return;
			case Cell.NOT_B:
				c.logic = Cell.B_OR_NA;
				return;
			case Cell.B_AND_NA:
				c.logic = Cell.TRUE;
				return;
			case Cell.NOT_A:
				c.logic = Cell.TRUE;
				return;
			case Cell.XOR:
				c.logic = Cell.B_OR_NA;
				return;
			case Cell.NAND:
				c.logic = Cell.B_OR_NA;
				return;
			case Cell.AND:
				c.logic = Cell.NAND;
				return;
			case Cell.XNOR:
				c.logic = Cell.NAND;
				return;
			case Cell.A:
				c.logic = Cell.NOT_A;
				return;
			case Cell.A_OR_NB:
				c.logic = Cell.NOT_A;
				return;
			case Cell.B:
				c.logic = Cell.NAND;
				return;
			case Cell.B_OR_NA:
				c.logic = Cell.NAND;
				return;
			case Cell.OR:
				c.logic = Cell.NOT_A;
				return;
			case Cell.TRUE:
				c.logic = Cell.NOT_A;
				return;
		}
	}
	
	private static void A_OR_NB (int logic, Cell c) {
		c.constant = true;
		switch (logic) {
			case Cell.UNDET:
				c.constant = false;
				return;
			case Cell.FALSE:
				c.logic = Cell.TRUE;
				return;
			case Cell.NOR:
				c.logic = Cell.OR;
				return;
			case Cell.A_AND_NB:
				c.logic = Cell.B_OR_NA;
				return;
			case Cell.NOT_B:
				c.logic = Cell.B;
				return;
			case Cell.B_AND_NA:
				c.logic = Cell.TRUE;
				return;
			case Cell.NOT_A:
				c.logic = Cell.OR;
				return;
			case Cell.XOR:
				c.logic = Cell.B_OR_NA;
				return;
			case Cell.NAND:
				c.logic = Cell.B;
				return;
			case Cell.AND:
				c.logic = Cell.NAND;
				return;
			case Cell.XNOR:
				c.logic = Cell.XOR;
				return;
			case Cell.A:
				c.logic = Cell.NOT_A;
				return;
			case Cell.A_OR_NB:
				c.logic = Cell.B_AND_NA;
				return;
			case Cell.B:
				c.logic = Cell.NAND;
				return;
			case Cell.B_OR_NA:
				c.logic = Cell.XOR;
				return;
			case Cell.OR:
				c.logic = Cell.NOT_A;
				return;
			case Cell.TRUE:
				c.logic = Cell.B_AND_NA;
				return;
		}
	}
	
	private static void B (int logic, Cell c) {
		c.constant = true;
		switch (logic) {
			case Cell.UNDET:
				c.constant = false;
				return;
			case Cell.FALSE:
				c.logic = Cell.TRUE;
				return;
			case Cell.NOR:
				c.logic = Cell.TRUE;
				return;
			case Cell.A_AND_NB:
				c.logic = Cell.TRUE;
				return;
			case Cell.NOT_B:
				c.logic = Cell.TRUE;
				return;
			case Cell.B_AND_NA:
				c.logic = Cell.A_OR_NB;
				return;
			case Cell.NOT_A:
				c.logic = Cell.A_OR_NB;
				return;
			case Cell.XOR:
				c.logic = Cell.A_OR_NB;
				return;
			case Cell.NAND:
				c.logic = Cell.A_OR_NB;
				return;
			case Cell.AND:
				c.logic = Cell.NAND;
				return;
			case Cell.XNOR:
				c.logic = Cell.NAND;
				return;
			case Cell.A:
				c.logic = Cell.NAND;
				return;
			case Cell.A_OR_NB:
				c.logic = Cell.NAND;
				return;
			case Cell.B:
				c.logic = Cell.NOT_B;
				return;
			case Cell.B_OR_NA:
				c.logic = Cell.NOT_B;
				return;
			case Cell.OR:
				c.logic = Cell.NOT_B;
				return;
			case Cell.TRUE:
				c.logic = Cell.NOT_B;
				return;
		}
	}
	
	private static void B_OR_NA (int logic, Cell c) {
		c.constant = true;
		switch (logic) {
			case Cell.UNDET:
				c.constant = false;
				return;
			case Cell.FALSE:
				c.logic = Cell.TRUE;
				return;
			case Cell.NOR:
				c.logic = Cell.OR;
				return;
			case Cell.A_AND_NB:
				c.logic = Cell.TRUE;
				return;
			case Cell.NOT_B:
				c.logic = Cell.OR;
				return;
			case Cell.B_AND_NA:
				c.logic = Cell.A_OR_NB;
				return;
			case Cell.NOT_A:
				c.logic = Cell.A;
				return;
			case Cell.XOR:
				c.logic = Cell.A_OR_NB;
				return;
			case Cell.NAND:
				c.logic = Cell.A;
				return;
			case Cell.AND:
				c.logic = Cell.NAND;
				return;
			case Cell.XNOR:
				c.logic = Cell.XOR;
				return;
			case Cell.A:
				c.logic = Cell.NAND;
				return;
			case Cell.A_OR_NB:
				c.logic = Cell.XOR;
				return;
			case Cell.B:
				c.logic = Cell.NOT_B;
				return;
			case Cell.B_OR_NA:
				c.logic = Cell.A_AND_NB;
				return;
			case Cell.OR:
				c.logic = Cell.NOT_B;
				return;
			case Cell.TRUE:
				c.logic = Cell.A_AND_NB;
				return;
		}
	}
	
	private static void OR (int logic, Cell c) {
		c.constant = true;
		switch (logic) {
			case Cell.UNDET:
				c.constant = false;
				return;
			case Cell.FALSE:
				c.logic = Cell.TRUE;
				return;
			case Cell.NOR:
				c.logic = Cell.TRUE;
				return;
			case Cell.A_AND_NB:
				c.logic = Cell.B_OR_NA;
				return;
			case Cell.NOT_B:
				c.logic = Cell.B_OR_NA;
				return;
			case Cell.B_AND_NA:
				c.logic = Cell.A_OR_NB;
				return;
			case Cell.NOT_A:
				c.logic = Cell.A_OR_NB;
				return;
			case Cell.XOR:
				c.logic = Cell.XNOR;
				return;
			case Cell.NAND:
				c.logic = Cell.XNOR;
				return;
			case Cell.AND:
				c.logic = Cell.NAND;
				return;
			case Cell.XNOR:
				c.logic = Cell.NAND;
				return;
			case Cell.A:
				c.logic = Cell.NOT_A;
				return;
			case Cell.A_OR_NB:
				c.logic = Cell.NOT_A;
				return;
			case Cell.B:
				c.logic = Cell.NOT_B;
				return;
			case Cell.B_OR_NA:
				c.logic = Cell.NOT_B;
				return;
			case Cell.OR:
				c.logic = Cell.NOR;
				return;
			case Cell.TRUE:
				c.logic = Cell.NOR;
				return;
		}
	}
	
	private static void TRUE (int logic, Cell c) {
		c.constant = true;
		switch (logic) {
			case Cell.UNDET:
				c.constant = false;
				return;
			case Cell.FALSE:
				c.logic = Cell.TRUE;
				return;
			case Cell.NOR:
				c.logic = Cell.OR;
				return;
			case Cell.A_AND_NB:
				c.logic = Cell.B_OR_NA;
				return;
			case Cell.NOT_B:
				c.logic = Cell.B;
				return;
			case Cell.B_AND_NA:
				c.logic = Cell.A_OR_NB;
				return;
			case Cell.NOT_A:
				c.logic = Cell.A;
				return;
			case Cell.XOR:
				c.logic = Cell.XNOR;
				return;
			case Cell.NAND:
				c.logic = Cell.AND;
				return;
			case Cell.AND:
				c.logic = Cell.NAND;
				return;
			case Cell.XNOR:
				c.logic = Cell.XOR;
				return;
			case Cell.A:
				c.logic = Cell.NOT_A;
				return;
			case Cell.A_OR_NB:
				c.logic = Cell.B_AND_NA;
				return;
			case Cell.B:
				c.logic = Cell.NOT_B;
				return;
			case Cell.B_OR_NA:
				c.logic = Cell.A_AND_NB;
				return;
			case Cell.OR:
				c.logic = Cell.NOR;
				return;
			case Cell.TRUE:
				c.logic = Cell.FALSE;
				return;
		}
	}
}
