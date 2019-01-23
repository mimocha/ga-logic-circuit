/*
 * Simple Java program for analyzing synthesized circuit
 * Repo: https://github.com/mimocha/ga-logic-circuit
 * Copyright (c) 2019 Chawit Leosrisook
 */
package circuit.analysis;

import java.util.Scanner;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
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
	public static final int LENGTH = (int) Math.pow(COLOR, NB);
	
    // Define Grid Dimensions
	public static final int DIMX = 64;
	public static final int DIMY = 64;
	
	// Define Grid
	public static final int[][] GRID = new int[DIMY][DIMX];
	
	// Define Seed
	private static final int[] SEED = new int[DIMX];
	
	// Define Loop Count
	private static final int LOOP = 16;
	
	
	// ---------------------------------------------------------
	
	// INPUT and OUTPUT node definitions
	private static final int[] INPUT_NODE = new int[DIMX];
	private static final int[] OUTPUT_NODE = new int[DIMX];
	
	// Determines OUTPUT
	// 1 == Active (Considered) Node
	// 0 == Inactive (Ignored) Node
	private static final int OUT_BIT_1 = 1;
	private static final int OUT_BIT_2 = 1;
	
	// Define Cell Array
	public static Cell[][] LCA = new Cell[DIMY][DIMX];
	
	
	
	// ---------------------------------------------------------
	// MAIN METHODS
	// ---------------------------------------------------------
	
	// Main
	public static void main (String[] args) throws Exception {

		init();

		// ===== Analyzing Solution Logic ===== //
		
//		logicAnalysis ();
		
		// ===== Checking Rule Usage in batches =====//
		
//		countingAnalysis ();

		int[][] DNA = randGenDNA (8);
		
		for (int i=0; i<8; i++) {
			for (int j=0; j<64; j++) {
				System.out.printf("%1d", DNA[i][j]);
			}
			System.out.printf ("\n");
		}
		
	}
	
	// Initialize function
	private static void init () {
		// Determines SEED
		// GA Program C++ Code uses floor(DIMX/2) == 32
		// Thus, the experimental setting used was mid = 32
		int mid = Math.round(DIMX/2);
		SEED[mid] = 1;
	}
	
	
	
	// ---------------------------------------------------------
	// LOGIC ANALYSIS METHODS
	// ---------------------------------------------------------
	
	// Wrapper function for logic analysis
	private static void logicAnalysis () {
		
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
		
		for (;;) {
			// User Sets DNA
			int[] DNA = inputDNA();
			

			// ----- Cellular Automata ----- //

			// Print CA Generation
			System.out.printf ("\n\tCA Generation Output:\n");
			CellularAutomata.generateGrid (GRID, SEED, DNA);
			System.out.println ();
			CellularAutomata.generateGrid (GRID, DNA);


			// ----- Logic Simplification ----- //

			// Initialize Cell Objects
			initLCA ();

			// Connection Simulation
			System.out.printf ("\n\t Connection Simulation:\n");
			printOutput (OUTPUT_NODE);

			// Max Update Cycle = 1024
			// Max Loop Count = Max Update Cycle / DIMY == 16
			Simplify.simplify (LOOP, OUTPUT_NODE, INPUT_NODE);
			Simplify.printConnection ();

			printInput (INPUT_NODE);
		}
	}
	
	
	// Get DNA string
	private static int[] inputDNA () {
		// DNA String Length
		int dna_length = (int)Math.pow(COLOR, NB);
		
		// Get DNA String
		System.out.printf("Input DNA String (%d): ", dna_length);
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
	
	
	
	// ---------------------------------------------------------
	// USAGE COUNTING METHODS
	// ---------------------------------------------------------
	
	// Wrapper function for this analysis
	private static void countingAnalysis () throws Exception {
		
		// ===== Actual Experimental Data ===== //
		
		for (int j=0; j<18; j++) {
			String dir = null;
			String filename = null;

			setName (dir, filename, j);
			String[] DNA_String = readCSV (dir);

			int[][] DNA = new int [DNA_String.length][LENGTH];
			int solution_count = DNA_String.length;

			for (int i=0; i<solution_count; i++) {
				DNA[i] = convertDNA (DNA_String[i]);
			}

			System.out.printf ("Checking %d DNA entries\n", solution_count);
			for (int i=0; i<solution_count; i++) {
				CellularAutomata.generateGrid (GRID, SEED, DNA[i]);
				CellularAutomata.generateGrid (GRID, DNA[i]);
			}

//			saveResults (solution_count, filename);

			dispResults (solution_count);
		}



		// ===== Randomly Generate CA Rule ===== //
		
//		int[][] DNA = randGenDNA (2000);
//		int solution_count = 2000;
//
//		System.out.printf ("Checking %d DNA entries\n", solution_count);
//		for (int i=0; i<solution_count; i++) {
//			CellularAutomata.generateGrid (GRID, SEED, DNA[i]);
//			CellularAutomata.generateGrid (GRID, DNA[i]);
//		}
//
//		saveResults (solution_count, "C:/dump/random.csv");
	}
	
	
	// Automatically Set Filename
	private static void setName (String dir, String filename, int number) {
		switch (number) {
			case 0:
				dir = "C:/dump/0";
				filename = "C:/dump/0.csv";
				break;
			case 1:
				dir = "C:/dump/1";
				filename = "C:/dump/1.csv";
				break;
			case 2:
				dir = "C:/dump/2";
				filename = "C:/dump/2.csv";
				break;
			case 3:
				dir = "C:/dump/3";
				filename = "C:/dump/3.csv";
				break;
			case 4:
				dir = "C:/dump/4";
				filename = "C:/dump/4.csv";
				break;
			case 5:
				dir = "C:/dump/5";
				filename = "C:/dump/5.csv";
				break;
			case 6:
				dir = "C:/dump/6";
				filename = "C:/dump/6.csv";
				break;
			case 7:
				dir = "C:/dump/7";
				filename = "C:/dump/7.csv";
				break;
			case 8:
				dir = "C:/dump/8";
				filename = "C:/dump/8.csv";
				break;
			case 9:
				dir = "C:/dump/9";
				filename = "C:/dump/9.csv";
				break;
			case 10:
				dir = "C:/dump/a";
				filename = "C:/dump/a.csv";
				break;
			case 11:
				dir = "C:/dump/b";
				filename = "C:/dump/b.csv";
				break;
			case 12:
				dir = "C:/dump/c";
				filename = "C:/dump/c.csv";
				break;
			case 13:
				dir = "C:/dump/d";
				filename = "C:/dump/d.csv";
				break;
			case 14:
				dir = "C:/dump/e";
				filename = "C:/dump/e.csv";
				break;
			case 15:
				dir = "C:/dump/f";
				filename = "C:/dump/f.csv";
				break;
			case 16:
				dir = "C:/dump/nand";
				filename = "C:/dump/nand.csv";
				break;
			case 17:
				dir = "C:/dump/nor";
				filename = "C:/dump/nor.csv";
				break;
			default:
				dir = null;
				filename = null;
		}
	}
	
	
	// Get DNA from CSV file
	private static String[] readCSV (String inname) throws Exception {
		
		// ========== Get Directory Path ========== //
		
		String dirpath;
		if (inname == null) {
			System.out.printf ("Input Folder Absolute Path: ");
			Scanner in = new Scanner (System.in);
			dirpath = in.nextLine();
			in.close();
		} else {
			dirpath = inname;
		}
		
		File dp = new File (dirpath);
		if (dp.isDirectory()) {
			System.out.printf ("%s : IS DIRECTORY\n", dirpath);
		} else {
			System.out.printf ("%s : NOT DIRECTORY\n", dirpath);
			throw new java.lang.Error ("Invalid Directory");
		}
		
		// Get file list
		String[] filelist = dp.list();
		
		
		
		// ========== Count Solutions ========== //
		
		// Solution Counter for each file
		int counter = 0;
		
		System.out.printf ("\nFound %d files:\n", filelist.length);
		for (int i=0; i<filelist.length; i++) {
			// Set next file to read
			File fp = new File (dirpath, filelist[i]);
			if (fp.canRead() == false) {
				throw new java.lang.Exception ("Cannot Read File");
			}
			Scanner inputStream = new Scanner (fp);
			
			
			
			// Skip to line 28
			for (int j=0; j<27; j++) {
				inputStream.nextLine();
			}
			
			if (inputStream.hasNext("UID")) { // At population dump
				inputStream.nextLine();
			} else { // Goto population dump
				inputStream.nextLine();
				inputStream.nextLine();
				inputStream.nextLine();
			}
			
			
			
			while (inputStream.hasNextLine()) {
				inputStream.next();
				inputStream.next();
				if (inputStream.nextInt() == 1) counter++;
				inputStream.nextLine();
			}
			inputStream.close();
		}
		System.out.printf ("Found %d solutions\n", counter);
		
		// DNA List [Number of Solution]
		String[] DNA_List = new String[counter];
		
		// ========== Process Solutions ========== //
		
		counter = 0;
		
		for (int i=0; i<filelist.length; i++) {
			// Set next file to read
			File fp = new File (dirpath, filelist[i]);
			if (fp.canRead() == false) {
				throw new java.lang.Exception ("Cannot Read File");
			}
			Scanner inputStream = new Scanner (fp);
			
			
			
			// Skip to line 28
			for (int j=0; j<27; j++) {
				inputStream.nextLine();
			}
			
			if (inputStream.hasNext("UID")) { // At population dump
				inputStream.nextLine();
			} else { // Goto population dump
				inputStream.nextLine();
				inputStream.nextLine();
				inputStream.nextLine();
			}
			
			
			
			// Shoddy code of the year
			while (inputStream.hasNextLine()) {
				inputStream.next();
				inputStream.next();
				
				if (inputStream.nextInt() == 1) {
					inputStream.next();
					inputStream.next();
					inputStream.next();
					inputStream.next();
					inputStream.next();
					
					DNA_List [counter] = inputStream.next();

					counter++;
				} else {
					inputStream.nextLine();
				}
			}
			
			inputStream.close();
		}
		System.out.printf ("Parsed %d solutions\n\n", counter);
		
		return DNA_List;
	}
	
	
	// Randomly generate DNA
	private static int[][] randGenDNA (int number) {
		int[][] results = new int[number][LENGTH];
		
		Random rng = new Random( System.currentTimeMillis() );
		
		for (int i=0; i<number; i++) {
			for (int j=0; j<LENGTH; j++) {
				results[i][j] = rng.nextInt(COLOR);
			}
		}
		
		return results;
	}
	
	
	// Convert DNA String to int[]
	private static int[] convertDNA (String input) {
		int[] result = new int [LENGTH];
		
		char[] buffer = input.toCharArray();
		
		for (int i=0; i<buffer.length; i++) {
			result[i] = Character.getNumericValue(buffer[i]);
		}
		
		return result;
	}
	
	
	// Save Analysis Result
	private static void saveResults (int solution_count, String inname) throws IOException {
		int[] result = CellularAutomata.getUsage();
		
		int sum = 0;
		for (int i=0; i<LENGTH; i++) {
			sum += result[i];
		}
		
		float[] percent = new float [LENGTH];
		float total = 0;
		for (int i=0; i<LENGTH; i++) {
			percent[i] = (float) result[i] / sum * 100;
			total += percent[i];
		}
		
		
//		System.out.printf ("INDEX | USAGE | PERCENT\n");
//		for (int i=0; i<LENGTH; i++) {
//			System.out.printf ("%5d |%7d| %5.2f%%\n", i, result[i], percent[i]);
//		}
//		System.out.printf ("TOTAL |%7d| %6.2f%%\n", sum, total);
		
		
		// ========== SAVE ========== //
		
		String filename;
		if (inname == null) {
			filename = "C:/dump/analysis.csv";
		} else {
			filename = inname;
		}
		System.out.printf ("Saving file as: %s\n", filename);
		
		FileWriter fileWriter = new FileWriter(filename);
		PrintWriter printWriter = new PrintWriter(fileWriter);
		
//		printWriter.printf ("solution_count,", solution_count);
//		for (int i=0; i<LENGTH; i++) {
//			printWriter.printf ("%d,", i);
//		}
		
		printWriter.printf ("%d,", solution_count);
		for (int i=0; i<LENGTH; i++) {
			printWriter.printf ("%d,", result[i]);
		}
		
//		printWriter.printf ("\n");
//		
//		for (int i=0; i<LENGTH; i++) {
//			printWriter.printf ("%f,", percent[i]);
//		}
		
		printWriter.close();
		fileWriter.close();
	}
	
	
	// Display Analysis Result
	private static void dispResults (int solution_count) {
		int[] result = CellularAutomata.getUsage();
		
		int sum = 0;
		for (int i=0; i<LENGTH; i++) {
			sum += result[i];
		}
		
		float[] percent = new float [LENGTH];
		float total = 0;
		for (int i=0; i<LENGTH; i++) {
			percent[i] = (float) result[i] / sum * 100;
			total += percent[i];
		}
		
		
		System.out.printf ("INDEX | USAGE | PERCENT\n");
		for (int i=0; i<LENGTH; i++) {
			System.out.printf ("%5d |%7d| %5.2f%%\n", i, result[i], percent[i]);
		}
		System.out.printf ("TOTAL |%7d| %6.2f%%\n", sum, total);
	}
	
	
	
	// ---------------------------------------------------------
	// PRINT METHODS
	// ---------------------------------------------------------
	
	// Print Single Cell - No Line Break
	public static void printCell (int cell) {
		switch (cell) {
			case 0: System.out.printf ("0"); break;
			case 1: System.out.printf ("|"); break;
			case 2: System.out.printf ("\\"); break;
			case 3: System.out.printf ("3"); break;
			default: System.out.printf ("%d", cell); break;
		}
		System.out.print(Color.RESET);
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
	
	// Prints Rule Usage
	private static void printUsage (int[] USAGE, int[] DNA) {
		System.out.printf ("\n\nRule Usage:\n");
		System.out.printf ("INDEX | RULE | USAGE\n");
		
		for (int i=0; i<LENGTH; i++) {
			System.out.printf ("%5d | %4d | %5d\n", i, DNA[i], USAGE[i]);
		}
		
	}
	
	
    
}
