#include "pch.h"
#include "TestHelpers.h"
#include <functional>
#include <random>

uint64_t line(CMove move, const int dX, const int dY)
{
	uint64_t ret = 0;
	int i = (move % 8) + dX; // Starting index in x direction
	int j = (move / 8) + dY; // Starting index in y direction

	while ((i >= 0) && (i < 8) && (j >= 0) && (j < 8)) // In between boundaries
	{
		ret |= 1ULL << (j * 8 + i);
		i += dX;
		j += dY;
	}

	return ret;
}

CPosition MakeRandomPosition(const uint64_t mask)
{
	static auto rnd = std::bind(std::uniform_int_distribution<unsigned int>(0,3), std::mt19937_64(13));
	uint64_t P = 0;
	uint64_t O = 0;
	for (unsigned int i = 0; i < PopCount(mask); i++)
	{
		P <<= 1;
		O <<= 1;
		switch (rnd())
		{
			case 0: P |= 1ULL; break;
			case 1: O |= 1ULL; break;
		}
	}
	return CPosition(PDep(P, mask), PDep(O, mask));
}

CPosition MakeFullRandomPosition(const uint64_t mask)
{
	static auto rnd = std::bind(std::uniform_int_distribution<uint64_t>(0,0xFFFFFFFFFFFFFFFFULL), std::mt19937_64(13));
	const uint64_t r = rnd();
	return CPosition(r & mask, ~r & mask);
}