/* Header file for Genetics ALgorithm Main Program.
	Contains standard library includes, defines, and other parameters.
*/

#ifndef GAMAIN_H_INCLUDED
#define GAMAIN_H_INCLUDED

// ----- Standard Library Definition ----- //

#include "stdio.h" // Standard library
#include "stdlib.h" // String to integers
#include "string.h" // String manipulation
#include "stdint.h" // uint definitions
#include "time.h" // Timing & performance
#include "math.h" // rounding
#include <iostream> // cin cout
#include <algorithm> // sort for GeneticAlgorithm::Sort()
#include <vector> // vectors
using namespace std; // namespace

// ----- Linux API Include ----- //
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

// ----- Altera HWLIB Include ----- //
#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"
#include "socal/alt_gpio.h"
#include "hps_0.h"

// ----- Parameter Definitions ----- //
#define VERSION 0.95
#define DIM 30 // Cell Array dimension
#define K 6 // Numbers of color represented in CA
#define K_CUBE K*K*K // Predefined K^3 (Sorry...)
#define GXDIM 80 // Fitness Graph X-Dimensions
#define GYDIM 40 // Fitness Graph Y-Dimensions
// Genetics Algorithm Parameter
#define POP 100 // Population size
#define GEN_LIM 50 // Generation limit
#define PM 0.05 // Mutation Probability
#define POOL 5 // Tournament Selection Poolsize

// ----- Global variables ----- // (Sue me)
clock_t time_total, time_gen;
uint32_t uid_counter = 0; // Unique ID for any individual created.

// Stuff for input argument handling
bool SHOW_C = 0; // Toggles Pretty CA Plots
bool SHOW_F = 0; // Toggles Fitness Tracking
bool SHOW_T = 0; // Toggles Performance Timing
bool SHOW_R = 0; // Toggles Result Displays
bool SHOW_D = 0; // Toggles Debug

// Fitness tracking variables: Max, Min, Median, Mean per generation
uint32_t maxfit[GEN_LIM], minfit[GEN_LIM], medfit[GEN_LIM], avgfit[GEN_LIM];

// Internal Model of Logic Gate Array
uint8_t LGA[DIM][DIM] = {0};
// CA Seed array (First Row of Generation)
uint8_t seed[DIM] = {0};
// CA Output array (Last Row of Generation)
// uint8_t output[DIM] = {0};

#endif
