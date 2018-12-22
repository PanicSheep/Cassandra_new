#include "SearchOutput.h"

using namespace Search;

constexpr int8_t max_score = +64;
constexpr int8_t min_score = -64;

bool COutput::Constrained() const
{
	return (min >= min_score) && (max <= max_score) && (min <= max);
}

COutput::COutput()
	: min(min_score)
	, max(max_score)
	, best_moves({})
{
	assert(Constrained());
}

COutput::COutput(int8_t min, int8_t max, int8_t depth, uint8_t selectivity, CBestMoves best_moves)
	: min(min)
	, max(max)
	, depth(depth)
	, selectivity(selectivity)
	, best_moves(best_moves)
{
	assert(Constrained());
}

COutput COutput::ExactScore(int8_t score, int8_t depth, uint8_t selectivity, CBestMoves best_moves)
{
	return COutput(score, score, depth, selectivity, best_moves);
}

COutput COutput::MaxBound(int8_t max, int8_t depth, uint8_t selectivity, CBestMoves best_moves)
{
	return COutput(min_score, max, depth, selectivity, best_moves);
}

COutput COutput::MinBound(int8_t min, int8_t depth, uint8_t selectivity, CBestMoves best_moves)
{
	return COutput(min, max_score, depth, selectivity, best_moves);
}

COutput Search::operator-(const COutput & o)
{
	return COutput(-o.max, -o.min, o.depth, o.selectivity, o.best_moves);
}