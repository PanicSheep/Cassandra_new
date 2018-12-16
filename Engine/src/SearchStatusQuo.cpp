#include "SearchStatusQuo.h"
#include <cassert>

using namespace Search;

bool CStatusQuo::Constrained() const
{
	return (alpha < beta) && (best_score <= alpha);
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

CInput CStatusQuo::Play(CMove move) const
{
	return CInput(pos.Play(move), -beta, -alpha, depth - 1, selectivity);
}

CInput CStatusQuo::PlayZWS(CMove move) const
{
	return CInput(pos.Play(move), -alpha - 1, -alpha, depth - 1, selectivity);
}

std::optional<COutput> CStatusQuo::ImproveWith(const COutput & novum)
{
	assert(Constrained());
	const bool novum_is_better = (novum.depth >= depth) && (novum.selectivity <= selectivity);
	if ((best_moves.PV == Field::invalid) || (novum_is_better && (novum.best_moves.PV != Field::invalid)))
		best_moves = novum.best_moves;
	if (novum_is_better)
	{
		if (novum.min == novum.max) // exact score
			return COutput::ExactScore(novum.min, novum.depth, novum.selectivity, best_moves);
		if (novum.min >= beta) // upper cut
			return COutput::MinBound(novum.min, novum.depth, novum.selectivity, best_moves);
		if (novum.max <= alpha) // lower cut
			return COutput::MaxBound(novum.max, novum.depth, novum.selectivity, best_moves);

		alpha = std::max(alpha, static_cast<int8_t>(novum.min - 1));
		beta = std::min(beta, static_cast<int8_t>(novum.max + 1));
	}
	assert(Constrained());
	return {};
}

std::optional<COutput> CStatusQuo::ImproveWith(const COutput & novum, CMove move)
{
	assert(Constrained());
	if ((novum.depth + 1 >= depth) && (novum.selectivity <= selectivity)) // TODO: Probably an irrelevant check.
	{
		if (novum.min >= beta) // upper cut
			return COutput::MinBound(novum.min, novum.depth + 1, novum.selectivity, { move, best_moves.PV });

		if (novum.min > best_score)
		{
			best_score = novum.min;
			//depth = novum.depth + 1; // TODO: implement best_depth.
			//selectivity = novum.selectivity; // TODO: implement best_selectivity.
			if (move != best_moves.PV) {
				best_moves.AV = best_moves.PV;
				best_moves.PV = move;
			}
		}
		alpha = std::max(alpha, novum.min);
	}
	assert(Constrained());
	return {};
}

COutput CStatusQuo::AllMovesTried()
{
	assert(Constrained());
	return COutput(best_score, alpha, depth, selectivity, best_moves);
}
