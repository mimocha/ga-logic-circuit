/* Header file for Genetics ALgorithm Main Program.
	Contains standard library includes, defines, and other parameters.
*/

#ifndef GAMAIN_H_INCLUDED
#define GAMAIN_H_INCLUDED

// Standard Library Definition
#include "stdio.h" // Standard library
#include "stdlib.h" // String to integers
#include "string.h" // String manipulation
#include "stdint.h" // uint definitions
#include "time.h" // Timing & performance

#include <iostream> // cin cout
#include <algorithm> // sort for GeneticAlgorithm::Sort()
#include <vector> // dynamic arrays in GeneticAlgorithm::Selection()

// Namespace
using namespace std;

// Parameter Definitions
#define DIM 50 // Cell Array dimension
#define K 3 // Numbers of color represented in CA
#define K_CUBE K*K*K // Predefined K^3 for speed (Sorry...)

// Genetics Algorithm Parameter
#define POP 100 // Population size
#define GEN_LIM 10 // Generation limit
#define PC 0.5 // Crossover Probability
#define PM 0.05 // Mutation Probability
#define POOL 5 // Tournament Selection Poolsize

// Enum for terminal input arguments
enum option_args {
	help, // help
	show_g, // graphics
	show_t, // time
	show_r, // results
	show_d, // debug
	null // null
};

// Global variables
clock_t time_total, time_gen; // Performance timing variables
uint32_t uid_counter = 0; // Unique ID for any individual created.
uint8_t seed[DIM] = {0}; // CA Seed array

bool SHOW_G = 0; // Toggles Pretty CA Plots
bool SHOW_T = 0; // Toggles Performance Timing
bool SHOW_R = 1; // Toggles Result Displays
bool SHOW_D = 0; // Toggles Debug

#endif
