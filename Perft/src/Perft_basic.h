#pragma once
#include "Position.h"

namespace Perft
{
	uint64_t Correct(uint8_t depth);

	namespace Basic
	{
		uint64_t perft(const CPosition&, uint8_t depth);
		uint64_t perft(uint8_t depth);
	}

	namespace Unrolled2
	{
		uint64_t perft(const CPosition&, uint8_t depth);
		uint64_t perft(uint8_t depth);
	}

	namespace HashTableMap
	{
		uint64_t perft(const CPosition&, uint8_t depth, uint64_t BytesRAM = 8 * 1024 * 1024);
		uint64_t perft(uint8_t depth, uint64_t BytesRAM = 8 * 1024 * 1024);
	}
}
