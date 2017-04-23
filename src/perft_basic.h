#pragma once
#include <chrono>
#include <iostream>
#include "flip_fast.h"
#include "macros_hell.h"
#include "possiblemoves.h"
#include "position.h"
#include "utility.h"

// Provides two simple perft functions.
// Perft_Basic contains the simplest perft function.
// Perft_Unroll2 contains a perft function which is unrolled for the last two plies.

namespace Perft
{
	inline uint64_t Correct(uint8_t depth)
	{
		const uint64_t correct[] = {1, 4, 12, 56, 244, 1396, 8200, 55092, 390216, 3005288, 24571056, 212258216, 1939879668, 18429618408, 184041761768, 1891831332208, 20301171282452, 222742563853912, 2534535926617852, 29335558770589276, 20, 21};
		return correct[depth];
	}
}

namespace Perft_Basic
{
	void perft(const uint8_t depth);
	uint64_t perft(const uint64_t P, const uint64_t O, const uint8_t depth);
}

namespace Perft_Unroll2
{
	void perft(const uint8_t depth);
	uint64_t perft(const uint64_t P, const uint64_t O, const uint8_t depth);
}