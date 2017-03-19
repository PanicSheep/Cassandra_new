#include "helpers.h"

uint64_t line(const uint8_t move, const int dX, const int dY)
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

void CPosition::MakeRandom(const uint64_t mask)
{
	static auto rnd = std::bind(std::uniform_int_distribution<unsigned int>(0,3), std::mt19937_64(13));
	uint64_t p = 0;
	uint64_t o = 0;
	for (unsigned int i = 0; i < PopCount(mask); i++)
	{
		p <<= 1;
		o <<= 1;
		switch (rnd())
		{
			case 0: p |= 1ULL; break;
			case 1: o |= 1ULL; break;
		}
	}
	P = PDep(p, mask);
	O = PDep(o, mask);
}

void CPosition::MakeRandomFull(const uint64_t mask)
{
	static auto rnd = std::bind(std::uniform_int_distribution<uint64_t>(0,0xFFFFFFFFFFFFFFFFULL), std::mt19937_64(13));
	const uint64_t r = rnd();
	P =  r & mask;
	O = ~r & mask;	
}