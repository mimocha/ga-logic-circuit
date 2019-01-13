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
	// -1 = Undetermined
	// 0 = GND
	// 1 = TRUE
	// 2 = A
	// 3 = B
	// 4 = NOT A
	// 5 = NOT B
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
	// METHODS
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
	
	
	
	// ---------------------------------------------------------
	// SETTER & GETTER METHODS
	// ---------------------------------------------------------
	
//	// Coordinates
//	
//	public void setLoc (int x_in, int y_in) {
//		this.x = x_in;
//		this.y = y_in;
//	}
//	
//	public void setX (int setval) {
//		this.x = setval;
//	}
//	
//	public void setY (int setval) {
//		this.y = setval;
//	}
//	
//	public int getX () {
//		return this.x;
//	}
//	
//	public int getY () {
//		return this.y;
//	}
//	
//	// RAM
//	
//	public void setRAM (int setval) {
//		this.ram = setval;
//	}
//	
//	public int getRAM () {
//		return this.ram;
//	}
	
}
