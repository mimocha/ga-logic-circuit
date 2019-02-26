/*
 * Simple Java program for analyzing synthesized circuit
 * Repo: https://github.com/mimocha/ga-logic-circuit
 * Copyright (c) 2019 Chawit Leosrisook
 */
package circuit.analysis;

import static circuit.analysis.CircuitAnalysis.DIMX;
import static circuit.analysis.CircuitAnalysis.DIMY;
import static circuit.analysis.CircuitAnalysis.GRID;
import static circuit.analysis.CircuitAnalysis.INPUT_NODE;
import static circuit.analysis.CircuitAnalysis.LCA;
import static circuit.analysis.CircuitAnalysis.LOOP;
import static circuit.analysis.CircuitAnalysis.OUTPUT_NODE;
import static circuit.analysis.CircuitAnalysis.SEED;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;

/**
 * This class contains all methods for simplifying logical circuits.
 * This includes checking for disconnected cells, and attempting to determine cell logic.
 */
public class LogicAnalysis {
	
	// Wrapper function for logic analysis
	public static void logicAnalysis () throws IOException {
		
		about();
		
		while (true) {
			// User Sets DNA
			int[] DNA = Helper.inputDNA();
			
			// Check input DNA
			boolean valid = Helper.checkDNA(DNA);
			
			// Exits function if DNA string is invalid
			if (!valid) {
				System.out.printf (Color.YELLOW + "Invalid DNA, leaving function\n" + Color.RESET);
				return;
			}

			
			// ----- Cellular Automata ----- //

			// Print CA Generation
			System.out.printf 
				("\n\t" + Color.GREEN + "CA Generation Output (GRID):\n" + Color.RESET);
			CellularAutomata.generateGrid (GRID, SEED, DNA);
			CellularAutomata.generateGrid (GRID, DNA);
			printGrid (GRID);


			// ----- Logic Simplification ----- //

			// Initialize Cell Objects -> Sets Cell Object to current GRID values
			Helper.initLCA ();

			// Connection Simulation
			System.out.printf 
				("\n\t" + Color.GREEN + "Cell Logic Simplification (LCA):\n" + Color.RESET);
			printOutput (OUTPUT_NODE);

			// Max Cell Logic Update Iterations = 1024
			// Max Loop Count = Max Update Cycle / DIMY == 16
			simplify (LOOP, OUTPUT_NODE, INPUT_NODE);
			printConnection ();

			printInput (INPUT_NODE);
			legend ();
		}
	}
	
	
	// Prints about information
	private static void about () {
		final String about = "\n\tThis function generates a circuit from a given DNA string."
			  + "\n\tThe circuits are then simplified with the following rules:"
			  + "\n\t\t1. Cells not connected to the output are ignored."
			  + "\n\t\t2. Cells are simplified into functions of A & B."
			  + "\n\t\t3. Cells which cannot be simplified, are left as is."
			  + "\n"
			  + "\n\tInput a valid DNA string to begin."
			  + "\n\tInput anything else to return to the main menu.\n";
		
		System.out.printf (about);
	}
	
	
	// Print legends
	private static void legend () {
		// All Simplified 16 Boolean functions of (A,B) - In ascending binary order
		final String[] listSimplified = {
			" : FALSE", 
			" : NOR",
			" : A AND NOT B",
			" : NOT B",
			" : B AND NOT A",
			" : NOT A",
			" : XOR  ",
			" : NAND",
			" : AND  ",
			" : XNOR",
			" : A    ",
			" : A OR NOT B",
			" : B    ",
			" : B OR NOT A",
			" : OR   ",
			" : TRUE"};
		
		// Non-determined cell logic
		final String[] listUndetermined = {
			" : Pass A",
			" : Pass B",
			" : NAND"};
		
		System.out.printf ("\n\tSimplified Circuit Legends:");
		
		// Print Simplified Cell Legend
		for (int i=0; i<15; i=i+2) {
			System.out.printf ("\n\t");
			printConstant (i);
			System.out.print (listSimplified[i]);
			
			System.out.printf ("\t|\t");
			printConstant (i+1);
			System.out.print (listSimplified[i+1]);
		}

		// Print Undetermined Cell Legend
		System.out.printf ("\n\tUndetermined Cells:");
		for (int i=0; i<3; i++) {
			System.out.printf ("\n\t");
			printCell (i+1);
			System.out.print (listUndetermined[i]);
		}
		
		System.out.println();
	}
	
	
	
	// ---------------------------------------------------------
	// CIRCUIT SIMPLIFICATION METHODS
	// ---------------------------------------------------------
	
	// Simulates signal traversing backwards
	private static void simplify (int loop, int[] OUTPUT, int[] INPUT) {
		// Check Connections
		checkConnection(OUTPUT, loop);
		
		// Check Cell Constant
		checkConstant (INPUT, loop);
	}
	
	
	
	// ---------------------------------------------------------
	// CONNECTION CHECKING METHODS
	// ---------------------------------------------------------
	
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
	
	
	
	// ---------------------------------------------------------
	// CONSTANT LOGIC CHECKING METHODS
	// ---------------------------------------------------------
	
	// Checks for constant cells
	private static void checkConstant (int[] INPUT, int loop) {
		// Set bottom row once
		for (int x=0; x<DIMX; x++) {
			int y = DIMY-1;
			
			switch (LCA[y][x].ram) {
				// Cell is a GND cell
				case 0:
					LCA[y][x].constant = true;
					LCA[y][x].logic = Cell.FALSE;
					break;
					
				// Cell is left connection
				case 1:
					LCA[y][x].constant = true;
					// INPUT 1 == A | INPUT 2 == B
					if (INPUT[x] == 1) {
						LCA[y][x].logic = Cell.A;
					} else {
						LCA[y][x].logic = Cell.B;
					}
					break;
			}
		}
		
		// Loop
		for (int i=0; i<loop; i++) {
			// Update bottoms up
			for (int y=DIMY-1; y>=0; y--) {
				for (int x=0; x<DIMX; x++) {

					// If this cell's logic is already determined, skip
					if (LCA[y][x].constant == true) continue;

					// Set index of next cells
					int x_next = (x + 1) % DIMX;
					int y_next = (y + 1) % DIMY;

					// Inherit the state of neighbor cell, based on the current cell's RAM
					switch (LCA[y][x].ram) {

						// Cell is a GND cell
						case 0:
							LCA[y][x].constant = true;
							LCA[y][x].logic = Cell.FALSE;
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

						// Cell is NAND Gate
						case 3:
							// Different depending if it is on the bottom row or not
							if (y != DIMY-1) {
								checkNAND (x, y, x_next, y_next);
							} else {
								checkNANDBot (x, y, x_next, y_next, INPUT);
							}
							break;

					}

				}
			}
		}

	}
	
	
	// Checks NAND Gate Logic - Non bottom-row
	private static void checkNAND (int x, int y, int x_next, int y_next) {
		
		// ----------------- SHORT CIRCUITS ----------------- //
		
		// Short Circuited NAND Gate - NAND(0,X) = 1
		// Skips one-side undetermined logics, if the other is FALSE
		if (LCA[y_next][x].logic == Cell.FALSE || LCA[y_next][x_next].logic == Cell.FALSE) {
			LCA[y][x].constant = true;
			LCA[y][x].logic = Cell.TRUE;
			return;
		}
	
		// ----------------- NAND LOGICS ----------------- //
		
		NAND.logic(LCA[y_next][x].logic, LCA[y_next][x_next].logic, LCA[y][x]);
	}
	
	
	// Checks NAND Gate Logic - Bottom-row
	private static void checkNANDBot (int x, int y, int x_next, int y_next, int[] INPUT) {

		// ----------------- NAND LOGICS ----------------- //
		
		if (INPUT[x] == 1) {
			NAND.logic(Cell.A, LCA[y_next][x_next].logic, LCA[y][x]);
		} else {
			NAND.logic(Cell.B, LCA[y_next][x_next].logic, LCA[y][x]);
		}
		
	}
	
	
	
	// ---------------------------------------------------------
	// PRINT METHODS
	// ---------------------------------------------------------
	
	// ----- Print Grid -----//
	
	
	// Print entire grid - No Line Break
	private static void printGrid (int[][] grid) {
		for (int y=0; y<DIMY; y++) {
			printRow (grid[y], y);
		}
	}
	
	
	// Print Single Row + Line Break
	private static void printRow (int[] row, int rowNum) {
		for (int x=0; x<DIMX; x++) {
			printCell (row[x]);
		}
		
		System.out.printf (" : %d\n", rowNum);
	}
	
	
	// Print Single Cell - No Line Break
	private static void printCell (int cell) {
		switch (cell) {
			case 0: System.out.printf ("0"); break;
			case 1: System.out.printf ("|"); break;
			case 2: System.out.printf ("\\"); break;
			case 3: System.out.printf ("3"); break;
			default: System.out.printf ("%d", cell); break;
		}
		System.out.print(Color.RESET);
	}
	
	
	// Prints Input Row + Line Break
	private static void printInput (int[] row) {
		for (int x=0; x<DIMX; x++) {
			switch (row[x]) {
				case 1:
					System.out.printf (Color.GREEN + "A" + Color.RESET);
					break;
				case 2:
					System.out.printf (Color.BLUE + "B" + Color.RESET);
					break;
				default:
					System.out.printf ("X");
					break;
			}
		}
		System.out.printf (" : INPUT\n");
	}
	
	
	// Prints Output Row + Line Break
	private static void printOutput (int[] row) {
		for (int x=0; x<DIMX; x++) {
			switch (row[x]) {
				case 1:
					System.out.printf (Color.MAGNTA + "O" + Color.RESET);
					break;
				default:
					System.out.printf ("-");
					break;
			}
		}
		System.out.printf (" : OUTPUT\n");
	}
	
	
	// ----- Print LCA -----//
	
	
	// Prints result
	private static void printConnection () {

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
				printCell (GRID[y][x]);
			}

			System.out.printf (Color.RESET + " : %d\n", y);
		}

	}
	
	
	// Special print for constant logic cells
	private static void printConstant (int logic) {
		switch (logic) {
			default: // Incorrect Values
				System.out.printf("x");
				break;
				
			case Cell.FALSE:
				System.out.printf ("0");
				break;
			case Cell.TRUE:
				System.out.printf ("1");
				break;
				
			case Cell.A:
				System.out.printf (Color.GREEN + "A" + Color.RESET);
				break;
			case Cell.B:
				System.out.printf (Color.BLUE + "B" + Color.RESET);
				break;
			case Cell.NOT_A:
				System.out.printf (Color.RED + "a" + Color.RESET);
				break;
			case Cell.NOT_B:
				System.out.printf (Color.RED + "b" + Color.RESET);
				break;
				
			case Cell.AND:
				System.out.printf (Color.YELLOW + "&" + Color.RESET);
				break;
			case Cell.NAND:
				System.out.printf (Color.RED + "&" + Color.RESET);
				break;
				
			case Cell.OR:
				System.out.printf (Color.YELLOW + "+" + Color.RESET);
				break;
			case Cell.NOR:
				System.out.printf (Color.RED + "+" + Color.RESET);
				break;
				
			case Cell.XOR:
				System.out.printf (Color.YELLOW + "X" + Color.RESET);
				break;
			case Cell.XNOR:
				System.out.printf (Color.RED + "X" + Color.RESET);
				break;
				
			case Cell.A_OR_NB:
				System.out.printf (Color.MAGNTA + "J" + Color.RESET);
				break;
			case Cell.B_OR_NA:
				System.out.printf (Color.MAGNTA + "K" + Color.RESET);
				break;
				
			case Cell.A_AND_NB:
				System.out.printf (Color.MAGNTA + "L" + Color.RESET);
				break;
			case Cell.B_AND_NA:
				System.out.printf (Color.MAGNTA + "M" + Color.RESET);
				break;
		}
	}
	
	
	
	// ---------------------------------------------------------
	// SAVE METHODS
	// ---------------------------------------------------------
	
	// Wrapper function for generating & saving cell connection mask
	public static void saveMask () throws IOException {
		
		// User Sets DNA
		int[] DNA = Helper.inputDNA();

		// Check input DNA
		boolean valid = Helper.checkDNA(DNA);

		// Exits function if DNA string is invalid
		if (!valid) {
			System.out.printf (Color.RED + "Invalid DNA.\n" + Color.RESET);
			return;
		}


		// ----- Cellular Automata ----- //

		CellularAutomata.generateGrid (GRID, SEED, DNA);
		CellularAutomata.generateGrid (GRID, DNA);


		// ----- Logic Simplification ----- //

		// Initialize Cell Objects -> Sets Cell Object to current GRID values
		Helper.initLCA ();

		// Connection Simulation
		System.out.printf 
			("\n\t" + Color.GREEN + "Cell Logic Simplification (LCA):\n" + Color.RESET);
		printOutput (OUTPUT_NODE);

		// Max Cell Logic Update Iterations = 1024
		// Max Loop Count = Max Update Cycle / DIMY == 16
		simplify (LOOP, OUTPUT_NODE, INPUT_NODE);
		printConnection ();

		printInput (INPUT_NODE);
		legend ();
		
		
		// ----- Save Connection Mask ----- //
		
		String filename = Helper.promptString ("\nSave mask as: ");
		saveConnectionMask (filename);
		System.out.printf (Color.GREEN + "Saved as: %s\n" + Color.RESET, filename);
	}
	
	
	// Save MASK NetPBM file of connected cells
	private static void saveConnectionMask (String filename) throws IOException {

		FileWriter fileWriter = new FileWriter(filename);
		PrintWriter printWriter = new PrintWriter(fileWriter);

		printWriter.printf ("P1\n64 64\n");

		for (int y=0; y<DIMY; y++) {
			for (int x=0; x<DIMX; x++) {

				if (LCA[y][x].connect == false) {
					printWriter.printf ("0 ");	// Disconnected Cell
				} else {
					printWriter.printf ("1 ");	// Connected Cell
				}

			}
			printWriter.printf ("\n");
		}

		printWriter.close();
		fileWriter.close();

	}

}
