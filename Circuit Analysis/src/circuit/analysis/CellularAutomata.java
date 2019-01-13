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

/**
 *
 * @author mimocha
 */
public class CellularAutomata {
	
	// Calculate the neighborhood indices - returns array of indices
	private static int[] calculateNB (int x) {
		int[] idx = new int[3];
		
		for (int i=0; i<3; i++) {
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
		
		// 
		for (int n=0; n<NB; n++) {
			idx += neighbor[n]*(int)(Math.pow(COLOR, n));
		}
		
		return idx;
	}
	
	
	// Lookup DNA with index number
	private static int lookupDNA (int[] DNA, int idx) {
		return DNA[idx];
	}
	
	
	// Apply CA rule to generate each row - Hard coded to work with 3 NB
	private static void applyCA (int[] input, int[] output, int[] DNA) {
		// Working Array
		int[] neighbor = new int[NB];
		int[] nb_idx = new int[NB];
		
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
		CircuitAnalysis.printRow (GRID[0]);
		
		// Generates GRID, except first row
		for (int y=0; y<(DIMY-1); y++) {
			int next = (y + 1) % DIMY;
			
			applyCA (GRID[y], GRID[next], DNA);
			CircuitAnalysis.printRow (GRID[next]);
		}
	}
	
	
	// Generate Circuit from CA DNA -- with SEED
	public static void generateGrid (int[][] GRID, int[] SEED, int[] DNA) {
		// Generates first row from SEED
		applyCA (SEED, GRID[0], DNA);
		CircuitAnalysis.printRow (GRID[0]);
		
		// Generates GRID, except first row
		for (int y=0; y<(DIMY-1); y++) {
			int next = (y + 1) % DIMY;
			
			applyCA (GRID[y], GRID[next], DNA);
			CircuitAnalysis.printRow (GRID[next]);
		}
	}
}
