/*
 * >>> This is the top-level file. <<<
 * Java program for analyzing results.
 * Repo: https://github.com/mimocha/ga-logic-circuit
 * Copyright (c) 2019 Chawit Leosrisook
 */
package circuit.analysis;

import java.io.FileNotFoundException;
import java.util.Random;

/**
 * This program is used to analyze the DNA of synthesized circuits.
 * 
 * The program does the following:
 *	1. Generates a circuit grid, based on the provided DNA
 *  2. Simplifies the grid with the following:
 *   2.1 Checks signal coverage (And hides or ignores cells which don't matter)
 *   2.2 Inherit constants
 *   2.3 Checks for short circuits (GND + NANDs == TRUE)
 *   2.4 Checks NAND Logic
 *  3. 
 * 
 * > Note [1]: GRID vs LCA
 * 
 * GRID is an INT array of DIMY-DIMX size.
 * GRID stores the array of number, generated with cellular automata.
 * It is a representation of a logical circuit, where each number is a single cell's state.
 * The same thing exists in the C++ Genetic Algorithm program.
 * 
 * LCA is a CELL object array of DIMX-DIMY size.
 * This is a more elaborate representation of a logical circuit.
 * The CELL object stores the CELL's state, and its *effective* logical function, among other stats.
 * This program uses the LCA to store a logically simplified circuit.
 * 
 */
public class CircuitAnalysis {

	// ---------------------------------------------------------
	// CONSTANTS & GLOBALS
	// ---------------------------------------------------------
	
	// Define CA Parameters
	public static final int COLOR = 4;
	public static final int NB = 3;
	public static final int LENGTH = (int) Math.pow(COLOR, NB);
	
    // Define Grid Dimensions
	public static final int DIMX = 64;
	public static final int DIMY = 64;
	
	// Define Grid
	// This is different from Cell[][] LCA -> See Note [1] ^(Top of this file)
	public static final int[][] GRID = new int[DIMY][DIMX];
	
	// Define Seed
	public static final int[] SEED = new int[DIMX];
	
	// Define Loop Count
	// This defines how many times the signal is allowed to loop.
	// Set this to the range 
	public static final int LOOP = 16;
	
	
	// ----- Logic Simplification Variables ----- //
	
	// INPUT and OUTPUT node definitions
	public static final int[] INPUT_NODE = new int[DIMX];
	public static final int[] OUTPUT_NODE = new int[DIMX];
	
	// Determines OUTPUT
	// 1 == Active (Considered) Node
	// 0 == Inactive (Ignored) Node
	public static final int OUT_BIT_1 = 1;
	public static final int OUT_BIT_2 = 0;
	
	// Define Logical Cell Array
	// This is different from int[][] GRID -> See Note [1] ^(Top of this file)
	public static Cell[][] LCA = new Cell[DIMY][DIMX];
	
	
	
	// ---------------------------------------------------------
	// MAIN METHODS
	// ---------------------------------------------------------
	
	// Main Function
	public static void main (String[] args) throws Exception {

		// ===== Initialize Stuff ===== //
		init();

		// ===== Main Menu ===== //
		while (true) {
			
			System.out.printf ("\n\t===== Circuit Analysis Main Menu =====\n");
			
			String menu =	"\t0. Exit Program\n" +
							"\t1. Circuit Logic Analysis\n" +
							"\t2. Generate Circuit Connection Mask\n" +
							"\t3. Generate & Print Random DNA\n" +
							"\t4. Rule Usage Counting Analysis\n" +
							"\t5. Print Solutions from RPT Files\n" +
							"\t6. \n";
			
			System.out.printf (menu);
			
			// Get User Input & Switch
			int sel = Helper.promptInt ("\nWaiting for input: ");
			switch (sel) {
				// ===== Exit Program ===== //
				case 0:
					System.out.printf ("\nExiting Program\n");
					return;
					
				// ===== Analyzing Circuit Logic ===== //
				case 1:
					LogicAnalysis.logicAnalysis ();
					break;
					
				// ===== Generate Circuit Connection Mask =====//
				case 2:
					LogicAnalysis.saveMask ();
					break;
					
				// ===== Print Random CA Rules ===== //
				case 3:
					printRandDNA ();
					break;
					
				// ===== Rule Usage Counting Analysis ===== //
				case 4:
					System.out.printf ("Selection 4\n");
					break;
					
				// ===== Extract Solutions from RPT Files ===== //
				case 5:
					extractRPT ();
					break;
					
				// ===== Syntax Error ===== //
				default:
					System.out.printf (Color.RED + "Unknown Option: %d\n" + Color.RESET, sel);
			}
		}
	}
	
	// Initialize function
	private static void init () {
		// Determines SEED
		// GA Program C++ Code uses floor(DIMX/2) == 32
		// Thus, the experimental setting used was `mid = 32`
		int mid = Math.round(DIMX/2);
		SEED[mid] = 1;
		
		// Determines OUTPUT
		// 1 == Active (Considered) Node
		// 0 == Inactive (Ignored) Node
		OUTPUT_NODE[0] = OUT_BIT_1;
		OUTPUT_NODE[4] = OUT_BIT_2;
		
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
	
	
	
	// ---------------------------------------------------------
	// GENERATE RANDOM DNA
	// ---------------------------------------------------------
	
	// Function to generate & print N random DNAs, as MATLAB cell array of strings.
	private static void printRandDNA () {

		System.out.printf ("\n\tThis function will print N number of DNA, "
					+ "in MATLAB 'cell array of strings' format.\n");
		
		int N = Helper.promptInt ("\nInput number of DNA strings to generate: ");
		int[][] DNA = randGenDNA (N);

		System.out.printf ("\nList = { ...\n");
		for (int[] DNA1 : DNA) {
			System.out.printf ("'");
			for (int j = 0; j<64; j++) {
				System.out.printf("%1d", DNA1[j]);
			}
			System.out.printf ("', ...\n");
		}
		System.out.printf ("};\n");
	}
	
	
	// Randomly generate DNA
	private static int[][] randGenDNA (int N) {
		int[][] results = new int[N][LENGTH];
		
		Random rng = new Random( System.currentTimeMillis() );
		
		for (int i=0; i<N; i++) {
			for (int j=0; j<LENGTH; j++) {
				results[i][j] = rng.nextInt(COLOR);
			}
		}
		
		return results;
	}
	
	
	
	// ---------------------------------------------------------
	// EXTRACT DNA FROM CSV
	// ---------------------------------------------------------
	
	// Function to extract DNA from preset RPT file, prints to console
	private static void extractRPT () throws FileNotFoundException {
		
		String about = "\n\tThis function extracts and prints solution DNAs from .rpt files."
			  + "\n\tThis function prints DNA into MATLAB 'cell array of strings' format."
			  + "\n\t'.rpt' files are custom text files. You can read them with a text editor."
			  + "\n\tThey contain the settings and results of an experiment."
			  + "\n\tEnter the absolute directory path to the folder to begin.\n";
		System.out.printf (about);
		
		// Read DNA from Directory
		String[] DNA_List = Helper.readRPT(null);
		
		// Empty DNA List Check
		if (DNA_List == null) {
			System.out.printf (Color.YELLOW + "No Solution DNA found.\n" + Color.RESET);
			return;
		}
		
		// Print Solution List to console as array
		System.out.printf ("\tExtracted Solutions List, MATLAB style:\n");
		System.out.printf ("Array = {\n");
		for (int i=0; i<DNA_List.length; i++) {
			System.out.printf ("'%s' ...\n", DNA_List[i]);
		}
		System.out.printf ("};");
		
		return;
	}
	
	
	// Prints Rule Usage
	private static void printUsage (int[] USAGE, int[] DNA) {
		System.out.printf ("\n\nRule Usage:\n");
		System.out.printf ("INDEX | RULE | USAGE\n");
		
		for (int i=0; i<LENGTH; i++) {
			System.out.printf ("%5d | %4d | %5d\n", i, DNA[i], USAGE[i]);
		}
		
	}
	
	
    
}
