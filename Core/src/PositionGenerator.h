#pragma once
#include <unordered_set>
#include <random>
#include "position.h"

class CPositionGenerator
{
public:
	CPositionGenerator();
	CPositionGenerator(std::size_t seed);

	CPosition CreateRandomPosition(uint8_t EmptiesCount);
	std::unordered_set<CPosition> CreateRandomPositionSet(uint8_t EmptiesCount, std::size_t size);
	std::unordered_set<CPosition> GenerateAllPositions(uint8_t EmptiesCount);

private:
	std::mt19937_64 rndEngine;
	std::uniform_int_distribution<unsigned int> distribution;
	unsigned int rnd();
};
