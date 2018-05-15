#pragma once
#include <unordered_set>
#include <random>
#include "Position.h"

class CPositionGenerator
{
public:
	CPositionGenerator();
	CPositionGenerator(std::size_t seed);

	                   CPosition  GenerateRandomPosition   (uint8_t EmptiesCount);
	std::unordered_set<CPosition> GenerateRandomPositionSet(uint8_t EmptiesCount, std::size_t size); // non deterministic.
	std::unordered_set<CPosition> GenerateAllPositions     (uint8_t EmptiesCount); // not taking symmetrie into account.
	std::unordered_set<CPosition> GenerateAllPositionsSym  (uint8_t EmptiesCount);     // taking symmetrie into account.

private:
	std::mt19937_64 rndEngine;
	std::uniform_int_distribution<unsigned int> distribution;
	unsigned int rnd();
};
