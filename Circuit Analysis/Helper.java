/*
 * Simple Java program for analyzing synthesized circuit
 * Repo: https://github.com/mimocha/ga-logic-circuit
 * Copyright (c) 2019 Chawit Leosrisook
 */
package circuit.analysis;

import static circuit.analysis.CircuitAnalysis.COLOR;
import static circuit.analysis.CircuitAnalysis.DIMX;
import static circuit.analysis.CircuitAnalysis.DIMY;
import static circuit.analysis.CircuitAnalysis.GRID;
import static circuit.analysis.CircuitAnalysis.LCA;
import static circuit.analysis.CircuitAnalysis.LENGTH;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;

/**
 * Helper Functions Class
 * @author mimocha
 */
public class Helper {

	// Get DNA from user input
	public static int[] inputDNA () {
		// Get DNA String
		System.out.printf("\nInput DNA String (Length:%d): ", LENGTH);
		Scanner in = new Scanner (System.in);
		String input = in.next();
		
		// Convert from String to INT array
		int[] output = convertDNA(input.toCharArray());
		
		return output;
	}
	
	
	// Convert Char array of DNA to int array
	public static int[] convertDNA (char[] input) {
		int[] output = new int [input.length];
		
		for (int i=0; i<input.length; i++) {
			output[i] = Character.getNumericValue(input[i]);
		}
		
		return output;
	}
	
	
	// Check DNA string
	public static boolean checkDNA (int[] DNA) {
		boolean result = true;
		
		// Check DNA Length
		if (DNA.length != LENGTH) {
			result = false;
			System.out.printf (Color.YELLOW + 
				  "DNA Length Mismatch: expected=%d ; actual=%d\n" + 
				  Color.RESET, LENGTH, DNA.length);
		}
		
		// Check DNA Radix
		for (int i=0; i<DNA.length; i++) {
			if (DNA[i] >= COLOR || DNA[i] < 0) {
				result = false;
				System.out.printf (Color.YELLOW + 
					  "DNA Radix Warning: idx=%d ; DNA[idx]=%d\n" + 
					  Color.RESET, i, DNA[i]);
			}
		}
		
		// Passed validity check, Valid == True
		return result;
	}
	
	
	// Initialize Virtual Logical Cell Array (LCA)
	public static void initLCA () {
		for (int y=0; y<DIMY; y++) {
			for (int x=0; x<DIMX; x++) {
				LCA[y][x] = new Cell (y, x, GRID[y][x]);
			}
		}
	}
	
	
	// Prompt User for input (INT)
	public static int promptInt (String prompt) {
		System.out.printf (prompt);
		Scanner in = new Scanner (System.in);
		int input = in.nextInt();
		return input;
	}
	
	
	// Prompt User for input (STRING)
	public static String promptString (String prompt) {
		System.out.printf (prompt);
		Scanner in = new Scanner (System.in);
		String input = in.nextLine();
		return input;
	}
	
	
	// Get DNA from all RPT files in a given folder
	public static String[] readRPT (String indir) throws FileNotFoundException {

		// Regex Pattern for checking for solutions
		String RegexSol = ".+\\s1\\s.+";
		// Regex Pattern for removing everthing before the DNA string, in that line
		String RegexDNA = ".+\\|\\s+";
		// Regex Pattern for skipping to population dump section
		String RegexSection = ".+DNA\\s$";
		
		// ========== Get Directory Path ========== //

		String dirpath;
		// Has an option to set directory by passing argument
		// Pass nullstring as argument to prompt user for a directory path
		if (indir != null) {
			dirpath = indir;
		} else {
			dirpath = promptString ("\nInput Absolute Path to CSV Folder: ");
		}

		// Checks if directory is valid
		File dp = new File (dirpath);
		if (dp.isDirectory()) {
			System.out.printf ("%s : DIRECTORY OK\n", dirpath);
		} else {
			System.out.printf (Color.RED + "%s : NOT DIRECTORY\n" + Color.RESET, dirpath);
			return null;
		}

		// Get file list
		String[] filelist = dp.list();


		// ========== Count Solutions ========== //
		// Counts how many solutions exists in directory, and allocates string memory accordingly.

		// Solution Counter for each file
		int counter = 0;

		System.out.printf ("\nFound %d files:\n", filelist.length);

		// Counts how many solution DNA exists
		for (String filelist1 : filelist) {
			// Set next file to read
			File fp = new File(dirpath, filelist1);
			System.out.printf ("Checking: %s | ", filelist1);

			// If Cannot Read File, Continue to next file
			if (!filelist1.contains(".rpt")) {
				System.out.printf (Color.YELLOW + "Non-RPT File\n" + Color.RESET);
				continue;
			}
			if (fp.canRead() == false) {
				System.out.printf (Color.RED + "Cannot Read File\n" + Color.RESET);
				continue;
			}

			Scanner inputStream = new Scanner (fp);

			// Skips to Population Dump Section
			while (inputStream.hasNextLine() && !inputStream.nextLine().matches(RegexSection)) {
				// Skips Line
			}

			// Counts Number of Solution DNA
			while (inputStream.hasNextLine()) {
				// This checks the 'solutions' flag | 1 == Is a solution | 0 == Is not a solution
				if (inputStream.nextLine().matches(RegexSol)) counter++;
			}

			inputStream.close();
			System.out.printf (Color.GREEN + "OK\n" + Color.RESET);
		}
		System.out.printf ("Found %d solutions\n", counter);


		// ========== Process Solutions ========== //

		// DNA List [Number of Solution]
		String[] DNA_List = new String[counter];
		counter = 0;

		// Same as the previous part, but loads DNA string into list.
		for (String filelist1 : filelist) {
			// Set next file to read
			File fp = new File(dirpath, filelist1);

			// If Cannot Read File, Continue to next file
			if (!filelist1.contains(".rpt")) {
				continue;
			}
			if (fp.canRead() == false) {
				continue;
			}

			Scanner inputStream = new Scanner (fp);

			// Skips to Population Dump Section
			while (inputStream.hasNextLine() && !inputStream.nextLine().matches(RegexSection)) {
				// Skips Line
			}

			// Adds DNA strings to list, one-by-one
			while (inputStream.hasNextLine()) {
				// Sets buffer for next line
				String currentLine = inputStream.nextLine();
				// If line contains a solution
				if (currentLine.matches(RegexSol)) {
					// Remove everything before DNA string
					currentLine = currentLine.replaceAll(RegexDNA, "");
					// Add DNA to list & increment counter
					DNA_List [counter] = currentLine;
					counter++;
				}

			}

			inputStream.close();
		}
		System.out.printf ("Parsed %d solutions\n\n", counter);

		return DNA_List;
	}
}
