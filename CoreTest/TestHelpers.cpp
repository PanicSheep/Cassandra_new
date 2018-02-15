#include "pch.h"
#include "TestHelpers.h"
#include <functional>
#include <random>

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