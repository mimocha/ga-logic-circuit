/*
 * Simple Java program for analyzing synthesized circuit
 * Repo: https://github.com/mimocha/ga-logic-circuit
 * Copyright (c) 2019 Chawit Leosrisook
 */
package circuit.analysis;

import static circuit.analysis.CircuitAnalysis.COLOR;
import static circuit.analysis.CircuitAnalysis.DIMX;
import static circuit.analysis.CircuitAnalysis.DIMY;
import static circuit.analysis.CircuitAnalysis.NB;
import static circuit.analysis.CircuitAnalysis.LENGTH;

/**
 *
 * @author mimocha
 */
public class CellularAutomata {
	
	// ---------------------------------------------------------
	// CONSTANTS & GLOBALS
	// ---------------------------------------------------------
	
	// Rule Usage Counter -- Increment in lookupDNA()
	private static int[] USE_COUNT = new int[LENGTH];
	
	
	
	// ---------------------------------------------------------
	// METHODS
	// ---------------------------------------------------------
	
	// Calculate the neighborhood indices - returns array of indices
	private static int[] calculateNB (int x) {
		int[] idx = new int[NB];
		
		for (int i=0; i<NB; i++) {
			// Calculates the index location -- Can be negative
			int temp = x - 1 + i;
			
			// Index looping
			// Guarantees index of range (0,DIMX]
			// (INDX + DIMX) % DIMX == LOOPED INDEX
			temp = temp + DIMX;
			temp = temp % DIMX;
			
			idx[i] = temp;
		}
		
		return idx;
	}
	
	
	// Convert neighbor cells into CA Rule LUT index
	private static int convertNB (int[] neighbor) {
		int idx = 0;
		for (int n=0; n<NB; n++) {
			idx += neighbor[n]*(int)(Math.pow(COLOR, n));
		}
		return idx;
	}
	
	
	// Lookup DNA with index number -- Usage Counter Increment
	private static int lookupDNA (int[] DNA, int idx) {
		USE_COUNT [idx] += 1;
		return DNA [idx];
	}
	
	
	// Apply CA rule to generate each row - Hard coded to work with 3 NB
	private static void applyCA (int[] input, int[] output, int[] DNA) {
		// Working Array Variables
		int[] neighbor = new int[NB];
		int[] nb_idx;
		
		for (int x=0; x<DIMX; x++) {
			// Get index for neighborhood
			nb_idx = calculateNB (x);
			
			// Set neighborhood
			for (int i=0; i<NB; i++) {
				neighbor[i] = input[ nb_idx[i] ];
			}
			
			// Get CA Value
			int dna_idx = convertNB (neighbor);
			output[x] = lookupDNA (DNA, dna_idx);
		}
	}
	
	
	// Generate Circuit from CA DNA -- without SEED
	public static void generateGrid (int[][] GRID, int[] DNA) {
		// Generates first row from bottom row
		applyCA (GRID[DIMY-1], GRID[0], DNA);
		
		// Generates GRID, except first row
		for (int y=0; y<(DIMY-1); y++) {
			int next = (y + 1) % DIMY;
			applyCA (GRID[y], GRID[next], DNA);
		}
	}
	
	
	// Generate Circuit from CA DNA -- with SEED
	public static void generateGrid (int[][] GRID, int[] SEED, int[] DNA) {
		// Generates first row from SEED
		applyCA (SEED, GRID[0], DNA);
		
		// Generates GRID, except first row
		for (int y=0; y<(DIMY-1); y++) {
			int next = (y + 1) % DIMY;
			applyCA (GRID[y], GRID[next], DNA);
		}
	}
	
	
	// Get Usage Count
	public static int[] getUsage () {
		return USE_COUNT;
	}
	
	
	// Reset Usage Count
	public static void resetUsage () {
		USE_COUNT = new int [LENGTH];
	}
}
