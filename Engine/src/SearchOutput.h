#pragma once
#include "Moves.h"
#include <cassert>
#include <cstdint>

namespace Search
{
	class COutput
	{
		bool Constrained() const;
	public:
		int8_t min, max;
		int8_t depth;
		uint8_t selectivity;
		CBestMoves best_moves;

		COutput();
		COutput(int8_t min, int8_t max, int8_t depth, uint8_t selectivity, CBestMoves best_moves = {});

		static COutput ExactScore(int8_t score, int8_t depth, uint8_t selectivity, CBestMoves best_moves = {});
		static COutput MaxBound(int8_t max, int8_t depth, uint8_t selectivity, CBestMoves best_moves = {});
		static COutput MinBound(int8_t min, int8_t depth, uint8_t selectivity, CBestMoves best_moves = {});

		friend COutput operator-(const COutput&);
	};
}