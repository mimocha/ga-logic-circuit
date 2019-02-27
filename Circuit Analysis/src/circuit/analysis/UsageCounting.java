/*
 * Simple Java program for analyzing synthesized circuit
 * Repo: https://github.com/mimocha/ga-logic-circuit
 * Copyright (c) 2019 Chawit Leosrisook
 */
package circuit.analysis;

import static circuit.analysis.CircuitAnalysis.GRID;
import static circuit.analysis.CircuitAnalysis.LENGTH;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Scanner;

/**
 *
 * @author mimocha
 */
public class UsageCounting {
	
	// Wrapper function for this analysis
	public static void countingAnalysis () throws Exception {
		
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
		
		printWriter.printf ("%d,", solution_count);
		for (int i=0; i<LENGTH; i++) {
			printWriter.printf ("%d,", result[i]);
		}
		
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
	
	
	
}
