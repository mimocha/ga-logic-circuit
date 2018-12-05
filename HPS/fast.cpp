/*	Main C++ file for Fast Functions
	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

/* ========== Standard Library Include ========== */

#include <stdint.h>		// uint definitions
#include <stdlib.h>		// rand, srand
#include <time.h>		// time



/* ========== Custom Header Include ========== */

#include "fast.hpp"



/* ========== Fast Exponential ========== */

uint32_t fast_pow (const uint16_t &base, const uint16_t &expo) {
	uint32_t result = 1;

	/* Simple exponent calculations -- positive integers only */
	for (int16_t e = 0 ; e < expo ; e++) {
		result = result * base;
	}

	return result;
}



/* ========== FAST RNG - XOSHIRO128** ========== */

/*  Written in 2018 by David Blackman and Sebastiano Vigna (vigna@acm.org)

	To the extent possible under law, the author has dedicated all copyright
	and related and neighboring rights to this software to the public domain
	worldwide. This software is distributed without any warranty.

	See <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

/* This is xoshiro128** 1.0, our 32-bit all-purpose, rock-solid generator. It
	has excellent (sub-ns) speed, a state size (128 bits) that is large
	enough for mild parallelism, and it passes all tests we are aware of.

	For generating just single-precision (i.e., 32-bit) floating-point
	numbers, xoshiro128+ is even faster.

	The state must be seeded so that it is not everywhere zero.
*/


static inline uint32_t rotl (const uint32_t x, int k) {
	return (x << k) | (x >> (32 - k));
}

static uint32_t s[4];

static uint32_t next (void) {
	const uint32_t result_starstar = rotl(s[0] * 5, 7) * 9;

	const uint32_t t = s[1] << 9;

	s[2] ^= s[0];
	s[3] ^= s[1];
	s[1] ^= s[2];
	s[0] ^= s[3];

	s[2] ^= t;

	s[3] = rotl(s[3], 11);

	return result_starstar;
}

/* This is the jump function for the generator. It is equivalent
   to 2^64 calls to next(); it can be used to generate 2^64
   non-overlapping subsequences for parallel computations.
*/

static void jump (void) {
	static const uint32_t JUMP[] = { 0x8764000b, 0xf542d2d3, 0x6fa035c3, 0x77f2db5b };

	uint32_t s0 = 0;
	uint32_t s1 = 0;
	uint32_t s2 = 0;
	uint32_t s3 = 0;
	for(uint32_t i = 0; i < sizeof JUMP / sizeof *JUMP; i++)
		for(int b = 0; b < 32; b++) {
			if (JUMP[i] & UINT32_C(1) << b) {
				s0 ^= s[0];
				s1 ^= s[1];
				s2 ^= s[2];
				s3 ^= s[3];
			}
			next();
		}

	s[0] = s0;
	s[1] = s1;
	s[2] = s2;
	s[3] = s3;
}


/* ========== RNG SEED GENERATOR - SPLITMIX64 ========== */

/*  Written in 2015 by Sebastiano Vigna (vigna@acm.org)

	To the extent possible under law, the author has dedicated all copyright
	and related and neighboring rights to this software to the public domain
	worldwide. This software is distributed without any warranty.

	See <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

/* This is a fixed-increment version of Java 8's SplittableRandom generator
	See http://dx.doi.org/10.1145/2714064.2660195 and
	http://docs.oracle.com/javase/8/docs/api/java/util/SplittableRandom.html

	It is a very fast generator passing BigCrush, and it can be useful if
	for some reason you absolutely want 64 bits of state; otherwise, we
	rather suggest to use a xoroshiro128+ (for moderately parallel
	computations) or xorshift1024* (for massively parallel computations)
	generator.
*/

/* Split Mix State -- The state can be seeded with any value. */
static uint64_t sms;

static uint64_t splitmix64 (void) {
	uint64_t z = (sms += 0x9e3779b97f4a7c15);
	z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
	z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
	return z ^ (z >> 31);
}


/* ========== HANDLING FUNCTIONS ========== */

uint32_t fast_rng32 (void) {
	return next ();
}

void seed_rng32 (void) {
	// Set std::rand() seed
	srand(time(NULL));

	// Set SplitMix64 seed
	sms = rand ();

	// Use SplitMix64 to set Xoshiro128** seeds
	s[0] = splitmix64 ();
	s[1] = splitmix64 ();
	s[2] = splitmix64 ();
	s[3] = splitmix64 ();

	// Jump once to randomize Xoshiro128** even more
	jump ();
}
