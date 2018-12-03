#include "SearchInput.h"
#include <cassert>

using namespace Search;

bool CInput::Constrained() const
{
	return alpha < beta;
}

CInput::CInput(CPosition pos, int8_t alpha, int8_t beta, int8_t depth, uint8_t selectivity)
	: pos(pos), alpha(alpha), beta(beta), depth(depth), selectivity(selectivity)
{
	assert(Constrained());
}

CInput Search::CInput::PlayPass() const
{
	return CInput(pos.PlayPass(), -beta, -alpha, depth, selectivity);
}
