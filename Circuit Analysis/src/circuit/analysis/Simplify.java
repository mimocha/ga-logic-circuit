/*
 * Simple Java program for analyzing synthesized circuit
 * Repo: https://github.com/mimocha/ga-logic-circuit
 * Copyright (c) 2019 Chawit Leosrisook
 */
package circuit.analysis;

import static circuit.analysis.CircuitAnalysis.DIMX;
import static circuit.analysis.CircuitAnalysis.DIMY;
import static circuit.analysis.CircuitAnalysis.GRID;
import static circuit.analysis.CircuitAnalysis.LCA;

/**
 *
 * @author mimocha
 */
public class Simplify {
	
	// ---------------------------------------------------------
	// CIRCUIT SIMPLIFICATION METHODS
	// ---------------------------------------------------------
	
	// Simulates signal traversing backwards
	public static void simplify (int loop, int[] OUTPUT, int[] INPUT) {
		// Check Connections
		checkConnection(OUTPUT, loop);
		
		// Check Cell Constant
		checkConstant (INPUT, loop);
	}
	
	
	// Checks connection
	private static void checkConnection (int[] OUTPUT, int loop) {
		// Set top row the same as output once
		for (int x=0; x<DIMX; x++) {
			LCA[0][x].connect = (OUTPUT[x] == 1);
		}
		
		// Loop
		for (int i=0; i<loop; i++) {
			
			for (int y=0; y<DIMY; y++) {
				for (int x=0; x<DIMX; x++) {

					// Skip this cell if there are no connections to it
					if (LCA[y][x].connect == false) continue;


					// Set index of next cells
					int x_next = (x + 1) % DIMX;
					int y_next = (y + 1) % DIMY;

					// Check connections
					// If not bottom row
					if (y != DIMY-1) {

						switch (GRID[y][x]) {
							case 0: // NULL - Do nothing
								break;
							case 1: // PASS A - Next cell under
								LCA[y_next][x].connect = true;
								break;
							case 2: // PASS B - Next cell bot right
								LCA[y_next][x_next].connect = true;
								break;
							case 3: // NAND - Two cells under
								LCA[y_next][x].connect = true;
								LCA[y_next][x_next].connect = true;
								break;
						}

						continue;
					}


					// If bottom row
					switch (GRID[y][x]) {
						case 0: // NULL - Do nothing
							break;
						case 1: // PASS A - Do nothing
							break;
						case 2: // PASS B - Loop bot right
							LCA[y_next][x_next].connect = true;
							break;
						case 3: // NAND - Loop bot right
							LCA[y_next][x_next].connect = true;
							break;
					}

				}
			}
		}

	}
	
	
	// Checks for constant cells
	private static void checkConstant (int[] INPUT, int loop) {
		// Set bottom row once
		for (int x=0; x<DIMX; x++) {
			int y = DIMY-1;
			
			switch (LCA[y][x].ram) {
				// Cell is a GND cell
				case 0:
					LCA[y][x].constant = true;
					LCA[y][x].logic = 0;
					break;
					
				// Cell is left connection
				case 1:
					LCA[y][x].constant = true;
					// 2 == A | 3 == B
					LCA[y][x].logic = INPUT[x] + 1;
					break;
					
				// Cell is not yet determined
				case 2:
				case 3:
					break;
			}
		}
		
		// Loop
		for (int i=0; i<loop; i++) {
			// Update bottoms up
			for (int y=DIMY-1; y>=0; y--) {
				for (int x=0; x<DIMX; x++) {

					// If this cell is already determined, skip
					if (LCA[y][x].constant == true) continue;
					
					// Set index of next cells
					int x_next = (x + 1) % DIMX;
					int y_next = (y + 1) % DIMY;

					// Inherit the state of neighbor cell, based on the current cell's RAM
					switch (LCA[y][x].ram) {

						// Cell is a GND cell
						case 0:
							LCA[y][x].constant = true;
							LCA[y][x].logic = 0;
							break;

						// Cell is PASS Left Cell
						case 1:
							LCA[y][x].constant = LCA[y_next][x].constant;
							LCA[y][x].logic = LCA[y_next][x].logic;
							break;

						// Cell is PASS Right Cell
						case 2:
							LCA[y][x].constant = LCA[y_next][x_next].constant;
							LCA[y][x].logic = LCA[y_next][x_next].logic;
							break;

						// Cell is short-circuited NAND
						case 3:
							checkNAND (x, y, x_next, y_next);
							break;

					}

				}
			}
		}

	}
	
	
	// Checks NAND Gate Logic
	private static void checkNAND (int x, int y, int x_next, int y_next) {
		
		// Short Circuited NAND Gate - NAND(0,X) = 1
		if (LCA[y_next][x].logic == 0 || LCA[y_next][x_next].logic == 0) {
			LCA[y][x].constant = true;
			LCA[y][x].logic = 1;
			return;
		}
		
		// Short Circuited NAND Gate - NAND(1,1) = 0
		if (LCA[y_next][x].logic == 1 && LCA[y_next][x_next].logic == 1) {
			LCA[y][x].constant = true;
			LCA[y][x].logic = 0;
			return;
		}
		
		// NOT Gates - If either input is a constant TRUE
		if (LCA[y_next][x].logic == 1) {
			switch (LCA[y_next][x_next].logic) {
				// If the other input isn't determined yet, do nothing
				case -1:
					return;
					
				// If the other input is a constant A -> Become NOT A
				case 2:
					LCA[y][x].constant = true;
					LCA[y][x].logic = 4;
					return;
					
				// If the other input is a constant B -> Become NOT B
				case 3:
					LCA[y][x].constant = true;
					LCA[y][x].logic = 5;
					return;
					
				// If the other input is a constant NOT A -> Become A
				case 4:
					LCA[y][x].constant = true;
					LCA[y][x].logic = 2;
					return;
					
				// If the other input is a constant NOT B -> Become B
				case 5:
					LCA[y][x].constant = true;
					LCA[y][x].logic = 3;
					return;
					
			}
		}
		
		// Does the opposite of the previous statements
		if (LCA[y_next][x_next].logic == 1) {
			switch (LCA[y_next][x].logic) {
				// If the other input isn't determined yet, do nothing
				case -1:
					return;
					
				// If the other input is a constant A -> Become NOT A
				case 2:
					LCA[y][x].constant = true;
					LCA[y][x].logic = 4;
					return;
					
				// If the other input is a constant B -> Become NOT B
				case 3:
					LCA[y][x].constant = true;
					LCA[y][x].logic = 5;
					return;
					
				// If the other input is a constant NOT A -> Become A
				case 4:
					LCA[y][x].constant = true;
					LCA[y][x].logic = 2;
					return;
					
				// If the other input is a constant NOT B -> Become B
				case 5:
					LCA[y][x].constant = true;
					LCA[y][x].logic = 3;
					return;
					
			}
		}
		
		// Inverters - Two same signals
		if (LCA[y_next][x].logic == LCA[y_next][x_next].logic) {
			switch (LCA[y_next][x].logic) {
				case 2:
					LCA[y][x].constant = true;
					LCA[y][x].logic = 4;
					return;
				case 3:
					LCA[y][x].constant = true;
					LCA[y][x].logic = 5;
					return;
				case 4:
					LCA[y][x].constant = true;
					LCA[y][x].logic = 2;
					return;
				case 5:
					LCA[y][x].constant = true;
					LCA[y][x].logic = 3;
					return;
			}
		}
		
	}
	
	
	// Remove short-circuited connections
	private static void removeShort (int loop) {
		
	}
	
	
	// ---------------------------------------------------------
	// PRINT METHODS
	// ---------------------------------------------------------
	
	// Prints result
	public static void printConnection () {

		for (int y=0; y<DIMY; y++) {
			for (int x=0; x<DIMX; x++) {
				// Disconnected Cell
				if (LCA[y][x].connect == false) {
					System.out.printf (".");
					continue;
				}
				
				// Determined Logic Cell
				if (LCA[y][x].constant == true) {
					printConstant (LCA[y][x].logic);
					continue;
				}
				
				// Undetermined Logic Cell
				CircuitAnalysis.printCell(GRID[y][x]);
			}

			System.out.println(Color.RESET);
		}

	}
	
	
	// Special print for constant logic cells
	private static void printConstant (int logic) {
		switch (logic) {
			// False
			case 0:
				System.out.printf ("0");
				break;
			// True
			case 1:
				System.out.printf ("T");
				break;
			// A
			case 2:
				System.out.printf (Color.GREEN + "A" + Color.RESET);
				break;
			// B
			case 3:
				System.out.printf (Color.BLUE + "B" + Color.RESET);
				break;
			// NOT A
			case 4:
				System.out.printf (Color.RED + "a" + Color.RESET);
				break;
			// NOT B
			case 5:
				System.out.printf (Color.RED + "b" + Color.RESET);
				break;
		}
	}
	
}
