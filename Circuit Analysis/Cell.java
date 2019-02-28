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
public class Cell {
	
	// ---------------------------------------------------------
	// CELL LOGIC INT
	// ---------------------------------------------------------
	
	//	This defines all the possible state a cell's logic can be set to.
	//	Use this instead of hard-coding the int, for easy maintenance
	
	// Shorted Cells -> TODO: Low Priority
	public static final int SHORT = -2;
	
	// Undetermined Logic
	public static final int UNDET = -1;
	
	// All 16 Boolean functions of (A,B) - In ascending binary order
	public static final int FALSE = 0;
	public static final int NOR = 1;
	public static final int A_AND_NB = 2;
	public static final int NOT_B = 3;
	public static final int B_AND_NA = 4;
	public static final int NOT_A = 5;
	public static final int XOR = 6;
	public static final int NAND = 7;
	public static final int AND = 8;
	public static final int XNOR = 9;
	public static final int A = 10;
	public static final int A_OR_NB = 11;
	public static final int B = 12;
	public static final int B_OR_NA = 13;
	public static final int OR = 14;
	public static final int TRUE = 15;
	
	// ---------------------------------------------------------
	// CELL PROPERTIES
	// ---------------------------------------------------------
	
	// Size of Grid
	public static final int DIMX = 64;
	public static final int DIMY = 64;
	
	// Location of cell
	public int x;
	public int y;
	
	// State of each cell
	public int ram;
	
	// Input signals of cell
	public boolean in[] = new boolean[2];
	
	// Output signal of cell
	public boolean out;
	
	// Connection status of cell
	// True == Connected | False == Disconnected (Ignored)
	public boolean connect;
	
	// Constancy of cell logic
	// (Whether the logic if a cell is determined or not)
	// True == Cell Output is Constant (To A/S, B/R, 1, or 0)
	// False == Cell Output is Variable
	public boolean constant;
	
	// Determined logic of cell (if cell is constant)
	public int logic;
	
	
	// ---------------------------------------------------------
	// CONSTRUCTOR
	// ---------------------------------------------------------
	
	// Single Constructor
	public Cell (int y_val, int x_val, int ram_val) {
		this.x = x_val;
		this.y = y_val;
		this.ram = ram_val;
		
		this.in[0] = false;
		this.in[1] = false;
		this.out = false;
		
		this.connect = false;
		this.constant = false;
		this.logic = -1;
	}
	
	
	
	// ---------------------------------------------------------
	// UPDATE METHODS
	// ---------------------------------------------------------
	
	// Updates input for each cell
	public void updateInput (boolean left, boolean right) {
		this.in[0] = left;
		this.in[1] = right;
	}
	
	// Updates output for each cell
	public void updateOutput () {
		
		switch (this.ram) {
			case 0:
				this.out = false;
				break;
			case 1:
				this.out = this.in[0];
				break;
			case 2:
				this.out = this.in[1];
				break;
			case 3:
				this.out = !(this.in[0] & this.in[1]);
				break;
			default:
				System.out.println("Bad Cell State");
				break;
		}
		
	}
	
	
	
}
