#pragma once
#include "macros_hell.h"
#include <functional>
#include <random>

uint64_t line(const uint8_t move, const int dX, const int dY);

class CPosition
{
public:
	uint64_t P, O;
	
	CPosition(uint64_t P, uint64_t O) : P(P), O(O) {}
	CPosition() : CPosition(0, 0) {}
	
	void MakeRandom(const uint64_t mask = 0xFFFFFFFFFFFFFFFFULL);
	void MakeRandomFull(const uint64_t mask = 0xFFFFFFFFFFFFFFFFULL);
};
