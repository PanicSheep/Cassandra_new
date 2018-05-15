#pragma once
#include <cstdint>
#include "Position.h"
#include "FlipFast.h"
#include "Moves.h"
#include "Environment.h"

class CStabilityAnalyzer : public IStabilityAnalyzer
{
public:
	CStabilityAnalyzer();

	uint64_t GetStableEdges(const CPosition&) const;
	uint64_t GetStableStones(const CPosition&) const override; // The stable stones from the opponent.

private:
	static uint64_t FullLineHorizontal(uint64_t discs);
	static uint64_t FullLineVertival(uint64_t discs);
	static uint64_t FullLineDiagonal(uint64_t discs);
	static uint64_t FullLineCodiagonal(uint64_t discs);

	uint8_t edge_stables[256][256];
};

uint64_t GetStableEdges(const CPosition&);
uint64_t GetStableStones(const CPosition&); // The stable stones from the opponent.