#pragma once
#include "Moves.h"
#include "Search.h"
#include <cassert>
#include <cstdint>

namespace Search
{
	class COutput
	{
		void TestConstraints() const;
		COutput(int8_t min, int8_t max, int8_t depth, uint8_t selectivity, CBestMoves best_moves = {});
	public:
		int8_t min = -infinity;
		int8_t max = +infinity;
		int8_t depth = -1;
		uint8_t selectivity = 99;
		CBestMoves best_moves{};

		COutput() = default;

		static COutput ExactScore(int8_t score, int8_t depth, uint8_t selectivity, CBestMoves best_moves = {});
		static COutput MaxBound(int8_t max, int8_t depth, uint8_t selectivity, CBestMoves best_moves = {});
		static COutput MinBound(int8_t min, int8_t depth, uint8_t selectivity, CBestMoves best_moves = {});

		COutput operator-() const;
	};

	class CAnalysisOutput
	{
		void TestConstraints() const;
	public:
		int8_t min, max, depth;
		uint8_t selectivity;
		CBestMoves best_moves;

		CAnalysisOutput() = delete;
		CAnalysisOutput(int8_t min, int8_t max, int8_t depth, uint8_t selectivity, CBestMoves best_moves = {});

		static CAnalysisOutput MaxBound(int8_t max, int8_t depth, uint8_t selectivity, CBestMoves best_moves = {});
		static CAnalysisOutput MinBound(int8_t min, int8_t depth, uint8_t selectivity, CBestMoves best_moves = {});
	};
}
