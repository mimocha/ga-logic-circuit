/* Header File for Fast Functions
	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

#ifndef FAST_HPP
#define FAST_HPP


/* uint32_t fast_pow (const uint16_t &base, const uint16_t &expo)
	Fast exponent function. Much faster than std::pow()
	For positive integers only.
	Returns 32-bit unsigned integer result of function: base^expo
*/
uint32_t fast_pow (const uint16_t &base, const uint16_t &expo);


/* ========== FAST RNG - XOSHIRO128** ========== */
// Code from: http://xoshiro.di.unimi.it/

/* uint64_t fast_rng32 (void)
	Wrapper function for calling xoshiro128** PRNG.
	Returns uint32_t result

	32-bit PRNG, with 128-bit state size.
	Prioritizing speed; much faster than std::rand() and faster than Mersenne Twister.
	Also should be atleast somewhat reliable.
*/
uint32_t fast_rng32 (void);

/* void seed_rng32 (void)
	Wrapper function for setting PRNG seeds.

	1. Seed std:rand() with time()
	2. Seed splitmix64() with std::rand()
	3. Seed xoroshiro128** with splitmix64()
	4. Call xoroshiro128** 2^64 times to randomize some more

	Done outside of simulation loop, so computational cost should not be a problem.
*/
void seed_rng32 (void);



#endif
