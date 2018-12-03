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

std::optional<COutput> CStatusQuo::ImproveWith(const COutput & new_data)
{
	assert(Constrained());
	if ((new_data.depth >= depth) && (new_data.selectivity <= selectivity))
	{
		if ((new_data.min == new_data.max) // exact score
			|| (new_data.min >= beta) // upper cut
			|| (new_data.max <= alpha)) // lower cut
		{
			if (new_data.PV != Field::invalid) PV = new_data.PV; // TODO: Test this, and decide what to do.
			if (new_data.AV != Field::invalid) AV = new_data.AV; // TODO: Test this, and decide what to do.
			return COutput(new_data.min, new_data.max, new_data.depth, new_data.selectivity, PV, AV);
		}

		// You can't be sure to find a value within min and max when there is probability involved.
		if (selectivity == 0)
		{
			if (new_data.max + 1 < beta)
			{
				beta = new_data.max + 1;
				if (new_data.PV != Field::invalid) PV = new_data.PV; // TODO: Test this, and decide what to do.
				if (new_data.AV != Field::invalid) AV = new_data.AV; // TODO: Test this, and decide what to do.
			}
			if (new_data.min - 1 > alpha)
			{
				alpha = new_data.min - 1;
				if (new_data.PV != Field::invalid) PV = new_data.PV; // TODO: Test this, and decide what to do.
				if (new_data.AV != Field::invalid) AV = new_data.AV; // TODO: Test this, and decide what to do.
			}
		}
	}
	if (PV == Field::invalid) PV = new_data.PV;
	if (AV == Field::invalid) AV = new_data.AV;
	assert(Constrained());
	return {};
}

std::optional<COutput> CStatusQuo::ImproveWith(const COutput & new_data, CMove move)
{
	assert(Constrained());
	if ((new_data.depth + 1 >= depth) && (new_data.selectivity <= selectivity)) // TODO: Probably an irrelevant check.
	{
		if (new_data.min >= beta) // upper cut
			return COutput(new_data.min, +64, new_data.depth + 1, new_data.selectivity, move, PV);

		if (new_data.min > best_score)
		{
			best_score = new_data.min;
			//depth = new_data.depth + 1; // TODO: implement best_depth.
			//selectivity = new_data.selectivity; // TODO: implement best_selectivity.
			if (move != PV) {
				AV = PV;
				PV = move;
			}
		}
		alpha = std::max(alpha, new_data.min);
	}
	assert(Constrained());
	return {};
}

COutput CStatusQuo::AllMovesTried()
{
	assert(Constrained());
	return COutput(best_score, alpha, depth, selectivity, PV, AV);
}
