#include "SearchStatusQuo.h"
#include <cassert>

using namespace Search;

bool CStatusQuo::Constrained() const
{
	return (alpha < beta);
}

CStatusQuo::CStatusQuo(const CInput & in)
	: alpha(in.alpha)
	, beta(in.beta)
	, depth(in.depth)
	, selectivity(in.selectivity)
	, pos(in.pos)
{
	assert(Constrained());
}

int8_t CStatusQuo::CurrentAlpha() const
{
	return std::max(alpha, best_score);
}

CInput CStatusQuo::Play(CMove move) const
{
	return CInput(pos.Play(move), -beta, -CurrentAlpha(), depth - 1, selectivity);
}

CInput CStatusQuo::PlayZWS(CMove move) const
{
	return CInput(pos.Play(move), -CurrentAlpha() - 1, -CurrentAlpha(), depth - 1, selectivity);
}

std::optional<COutput> CStatusQuo::ImproveWith(const CAnalysisOutput & novum)
{
	assert(Constrained());
	const bool consider_novum = (novum.depth >= depth) && (novum.selectivity <= selectivity);

	if (consider_novum || (best_moves.PV == Field::invalid))
		best_moves = novum.best_moves;
	
	if (consider_novum)
	{
		if (novum.min == novum.max) // exact score
			return COutput::ExactScore(novum.min, novum.depth, novum.selectivity, best_moves);
		if (novum.min >= beta) // upper cut
			return COutput::MinBound(novum.min, novum.depth, novum.selectivity, best_moves);
		if (novum.max <= alpha)
			return COutput::MaxBound(novum.max, novum.depth, novum.selectivity, best_moves);
	}
	assert(Constrained());
	return {};
}

std::optional<COutput> CStatusQuo::ImproveWithZWS(const COutput & novum, CMove move)
{
	assert(Constrained());
	assert((novum.depth + 1 >= depth) && (novum.selectivity <= selectivity));
	if (novum.depth + 1 < worst_depth) worst_depth = novum.depth + 1;
	if (novum.selectivity > worst_selectivity) worst_selectivity = novum.selectivity;

	if (novum.min >= beta) // upper cut
		return COutput::MinBound(novum.min, worst_depth, worst_selectivity, { move, best_moves.PV });

	assert(Constrained());
	return {};
}

std::optional<COutput> CStatusQuo::ImproveWith(const COutput & novum, CMove move)
{
	assert(Constrained());
	assert((novum.depth + 1 >= depth) && (novum.selectivity <= selectivity));
	if (novum.depth + 1 < worst_depth) worst_depth = novum.depth + 1;
	if (novum.selectivity > worst_selectivity) worst_selectivity = novum.selectivity;

	if (novum.min >= beta) // upper cut
		return COutput::MinBound(novum.min, worst_depth, worst_selectivity, { move, best_moves.PV });

	if (novum.min > best_score)
	{
		best_score = novum.min;
		if (move != best_moves.PV) {
			best_moves.AV = best_moves.PV;
			best_moves.PV = move;
		}
	}
	assert(Constrained());
	return {};
}

COutput CStatusQuo::AllMovesTried()
{
	assert(Constrained());
	assert(worst_depth >= depth);
	assert(worst_selectivity <= selectivity);

	if (best_score > alpha)
		return COutput::ExactScore(best_score, worst_depth, worst_selectivity, best_moves);
	//else if (best_score != -infinity)
	//	return COutput(best_score, alpha, worst_depth, worst_selectivity, best_moves);
	return COutput::MaxBound(alpha, worst_depth, worst_selectivity, best_moves);
}
