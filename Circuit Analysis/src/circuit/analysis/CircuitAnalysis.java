/*
 * Simple Java program for analyzing synthesized circuit
 * Repo: https://github.com/mimocha/ga-logic-circuit
 * Copyright (c) 2019 Chawit Leosrisook
 */
package circuit.analysis;

import java.util.Scanner;

/**
 * This program is used to analyze the DNA of synthesized circuits.
 * 
 * The program does the following:
 *	1. Generates a circuit grid, based on the provided DNA
 *  2. Simplifies the grid with the following:
 *   2.1 Checks signal coverage (And hides or ignores cells which don't matter)
 *   2.2 Inherit constants
 *   2.3 Checks for short circuits (GND + NANDs == TRUE)
 *   2.4 Checks for NOT Gates (TRUE + NANDS, NAND (SAME SIGNAL, SAME SIGNAL))
 * 
 * 
 */
public class CircuitAnalysis {

	// ---------------------------------------------------------
	// CONSTANTS & GLOBALS
	// ---------------------------------------------------------
	
	// Define CA Parameters
	public static final int COLOR = 4;
	public static final int NB = 3;
	
    // Define Grid Dimensions
	public static final int DIMX = 64;
	public static final int DIMY = 64;
	
	// Define Grid
	public static final int[][] GRID = new int[DIMY][DIMX];
	
	// Define Seed
	private static final int[] SEED = new int[DIMX];
	
	
	// ---------------------------------------------------------
	
	// INPUT and OUTPUT node definitions
	private static final int[] INPUT_NODE = new int[DIMX];
	private static final int[] OUTPUT_NODE = new int[DIMX];
	
	// Define Cell Array
	public static Cell[][] LCA = new Cell[DIMY][DIMX];
	
	
	
	// ---------------------------------------------------------
	// MAIN METHODS
	// ---------------------------------------------------------
	
	// Initialize
	private static void init () {
		// Determines SEED
		// GA Program C++ Code uses floor(DIMX/2) == 32
		// Thus, the experimental setting used was mid = 32
		int mid = Math.round(DIMX/2);
		SEED[mid] = 1;
		
		// Determines OUTPUT
		// 1 == Active (Considered) Node
		// 0 == Inactive (Ignored) Node
		OUTPUT_NODE[0] = 1;
		OUTPUT_NODE[4] = 0;
		
		// Determines INPUT
		// 1 == INPUT A / S
		// 2 == INPUT B / R
		for (int i=0; i<DIMX; i++) {
			if (i%2==0) {
				INPUT_NODE[i] = 1;
			} else {
				INPUT_NODE[i] = 2;
			}
		}
		
	}
	
	
	// Get DNA string
	private static int[] inputDNA () {
		// DNA String Length
		int dna_length = (int)Math.pow(COLOR, NB);
		
		// Get DNA String
		System.out.println("Input DNA String: ");
		Scanner in = new Scanner (System.in);
		String input = in.next();
		
		// Convert from String to INT array
		char[] buffer = input.toCharArray();
		int[] output = new int[dna_length];
		
		for (int x=0; x<DIMX; x++) {
			switch (buffer[x]) {
				case '0':
					output[x] = 0;
					break;
				case '1':
					output[x] = 1;
					break;
				case '2':
					output[x] = 2;
					break;
				case '3':
					output[x] = 3;
					break;
			}
		}
		
		return output;
	}
	
	
	// Initialize Virtual Logical Cell Array (LCA)
	private static void initLCA () {
		for (int y=0; y<DIMY; y++) {
			for (int x=0; x<DIMX; x++) {
				LCA[y][x] = new Cell (y, x, GRID[y][x]);
			}
		}
	}
	
	
	// Main
	public static void main (String[] args) {
		// Initialize stuff
		init();

		// User Sets DNA
		int[] DNA = inputDNA();

		// Print CA Generation
		System.out.printf ("\n\tCA Generation Output:\n");
		CellularAutomata.generateGrid (GRID, SEED, DNA);
		CellularAutomata.generateGrid (GRID, DNA);

		// Initialize Cell Objects for Simulation
		initLCA ();

		// Connection Simulation
		System.out.printf ("\n\t Connection Simulation:\n");
		printOutput (OUTPUT_NODE);
		
		// Max Update Cycle = 1024
		// Max Loop Count = Max Update Cycle / DIMY == 16
		Simplify.simplify (16, OUTPUT_NODE, INPUT_NODE);
		Simplify.printConnection ();
		
		printInput (INPUT_NODE);
	}
	
	
	
	// ---------------------------------------------------------
	// PRINT METHODS
	// ---------------------------------------------------------
	
	// Print Single Cell - No Line Break
	public static void printCell (int cell) {
		switch (cell) {
			case 0: System.out.print(Color.DGRAY); break;
			case 1: System.out.print(Color.CYAN); break;
			case 2: System.out.print(Color.MAGNTA); break;
			case 3: System.out.print(Color.YELLOW); break;
			default: System.out.print(Color.RED); break;
		}
		System.out.printf ("%d" + Color.RESET, cell);
	}
	
	// Print Single Row + Line Break
	public static void printRow (int[] row) {
		for (int x=0; x<DIMX; x++) {
			CircuitAnalysis.printCell (row[x]);
		}
		System.out.println();
	}
	
	// Print entire grid - No Line Break
	public static void printGrid (int[][] grid) {
		for (int y=0; y<DIMY; y++) {
			CircuitAnalysis.printRow(grid[y]);
		}
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
		System.out.println();
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
		System.out.println();
	}
	
	
	
	// ---------------------------------------------------------
	// METHODS
	// ---------------------------------------------------------
	
	
    
}
