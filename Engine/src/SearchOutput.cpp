#include "SearchOutput.h"

using namespace Search;

void COutput::TestConstraints() const // TODO: Test if this bothers performance.
{
	bool constraints = (min >= -infinity) && (max <= infinity) && (min <= max);
	constraints &= (min == -infinity) || (max == infinity) || (min == max);
	if (!constraints)
		throw std::runtime_error("Constraint violated!");
}

COutput::COutput(int8_t min, int8_t max, int8_t depth, uint8_t selectivity, CBestMoves best_moves)
	: min(min)
	, max(max)
	, depth(depth)
	, selectivity(selectivity)
	, best_moves(best_moves)
{
	TestConstraints();
}

COutput COutput::ExactScore(int8_t score, int8_t depth, uint8_t selectivity, CBestMoves best_moves)
{
	return COutput(score, score, depth, selectivity, best_moves);
}

COutput COutput::MaxBound(int8_t max, int8_t depth, uint8_t selectivity, CBestMoves best_moves)
{
	return COutput(-infinity, max, depth, selectivity, best_moves);
}

COutput COutput::MinBound(int8_t min, int8_t depth, uint8_t selectivity, CBestMoves best_moves)
{
	return COutput(min, infinity, depth, selectivity, best_moves);
}

COutput COutput::operator-() const
{
	return COutput(-max, -min, depth, selectivity, best_moves);
}


void CAnalysisOutput::TestConstraints() const // TODO: Test if this bothers performance.
{
	const bool constraints = (min >= -infinity) && (max <= infinity) && (min <= max);
	if (!constraints)
		throw std::runtime_error("Constraint violated!");
}

CAnalysisOutput::CAnalysisOutput(int8_t min, int8_t max, int8_t depth, uint8_t selectivity, CBestMoves best_moves)
	: min(min)
	, max(max)
	, depth(depth)
	, selectivity(selectivity)
	, best_moves(best_moves)
{
	TestConstraints();
}

CAnalysisOutput CAnalysisOutput::MaxBound(int8_t max, int8_t depth, uint8_t selectivity, CBestMoves best_moves)
{
	return CAnalysisOutput(-infinity, max, depth, selectivity, best_moves);
}

CAnalysisOutput CAnalysisOutput::MinBound(int8_t min, int8_t depth, uint8_t selectivity, CBestMoves best_moves)
{
	return CAnalysisOutput(min, +infinity, depth, selectivity, best_moves);
}