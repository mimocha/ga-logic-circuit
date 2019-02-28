/*
 * Simple Java program for analyzing synthesized circuit
 * Repo: https://github.com/mimocha/ga-logic-circuit
 * Copyright (c) 2019 Chawit Leosrisook
 */
package circuit.analysis;

import static circuit.analysis.CircuitAnalysis.GRID;
import static circuit.analysis.CircuitAnalysis.LENGTH;
import static circuit.analysis.CircuitAnalysis.SEED;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;

/**
 * DNA Rule Usage Counting Analysis
 * This class applies DNA strings to generate Cellular Automata grids.
 * The usage of each DNA loci is counted, printed, and saved to a CSV file for analysis.
 * @author mimocha
 */
public class UsageCounting {
	
	// Wrapper function for this analysis
	public static void countingAnalysis () throws Exception {
		about ();

		// Get directory
		String[] DNA_String = Helper.readRPT (null);
		
		// DNA Array
		int[][] DNA = new int [DNA_String.length][LENGTH];
		
		// Convert DNA & Check Validity
		for (int i=0; i<DNA_String.length; i++) {
			// Convert string to int[]
			int[] buffer = Helper.convertDNA (DNA_String[i].toCharArray());
			
			// Skip invalid DNA, add valid DNA to workinglist
			boolean valid = Helper.checkDNA(buffer);
			if (!valid) {
				System.out.printf 
					(Color.YELLOW + "Skipping Invalid DNA: %s\n" + Color.RESET, DNA_String[i]);
				continue;
			} else {
				DNA[i] = buffer;
			}
		}
		
		// Check Rule Usage
		CellularAutomata.resetUsage();
		System.out.printf ("Checking %d DNA entries\n", DNA.length);
		for (int i=0; i<DNA.length; i++) {
			CellularAutomata.generateGrid (GRID, SEED, DNA[i]);
			CellularAutomata.generateGrid (GRID, DNA[i]);
		}
		int [] usage = CellularAutomata.getUsage();

		// Print Rule Usage
		printUsage (usage);
		
		// Save Rule Usage to CSV
		String filename = Helper.promptString("\nSave usage csv as: ");
		saveUsage (usage, DNA.length, filename);
	}
	
	
	// Prints information about this class
	private static void about () {
		String about = "\n\tRule Usage Counting Analysis"
			  + "\n\tThis function counts how many time each DNA loci gets used for CA generation."
			  + "\n\tThe function prints the results, and saves to CSV file."
			  + "\n\tAnalyzes entire folder with .rpt files.\n";
		System.out.printf (about);
	}
	
	
	// Save Analysis Result
	private static void saveUsage (int[] usage, int sol_count, String filename) throws IOException {
		
		// ========== CALCULATE ========== //
		
		int sum = 0;
		for (int i=0; i<usage.length; i++) {
			sum = sum + usage[i];
		}
		
		float[] percent = new float [usage.length];
		for (int i=0; i<LENGTH; i++) {
			percent[i] = (float) usage[i] / sum * 100;
		}
		
		
		// ========== SAVE ========== //

		System.out.printf ("Saving file as: %s ", filename);
		
		FileWriter fileWriter = new FileWriter(filename);
		PrintWriter printWriter = new PrintWriter(fileWriter);
		
		// Solution Count
		printWriter.printf ("%d,\n", sol_count);
		// Usage Data - Raw & Percentage
		for (int i=0; i<usage.length; i++) {
			printWriter.printf ("%d,%f,\n", usage[i], percent[i]);
		}
		
		printWriter.close();
		fileWriter.close();
		
		System.out.printf (Color.GREEN + "DONE.\n" + Color.RESET);
	}
	
	
	// Prints Rule Usage
	private static void printUsage (int[] usage) {
		System.out.printf ("\n\tRule Usage:\n"
							+ "INDEX |  USAGE  | PERCENT\n");
		
		int sum = 0;
		for (int i=0; i<usage.length; i++) {
			sum = sum + usage[i];
		}
		
		for (int i=0; i<LENGTH; i++) {
			System.out.printf ("%5d | %7d | %7.4f %%\n", i, usage[i], (float)usage[i]/sum*100);
		}
	}
	
}
