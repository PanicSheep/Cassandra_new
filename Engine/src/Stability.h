#pragma once

#include <cstdint>

#include "Engine.h"
#include "Moves.h"
#include "Position.h"

class CStabilityAnalyzer : public IStabilityAnalyzer
{
public:
	CStabilityAnalyzer();

	uint64_t GetStableEdges(const CPosition&) const;
	uint64_t GetStableStones(const CPosition&) const override; // The stable stones from the opponent.

private:
	static uint64_t Flip(const CPosition&, CMove);
	static uint64_t Flip_dir(const CPosition&, CMove, int dX);

	static uint64_t FullLineHorizontal(uint64_t discs);
	static uint64_t FullLineVertival(uint64_t discs);
	static uint64_t FullLineDiagonal(uint64_t discs);
	static uint64_t FullLineCodiagonal(uint64_t discs);

	uint8_t edge_stables[256][256]{};
};