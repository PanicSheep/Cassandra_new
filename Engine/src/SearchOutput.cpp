#include "SearchOutput.h"

using namespace Search;

bool COutput::Constrained() const
{
	return (min >= -64) && (max <= +64) && (min <= max);
}

COutput::COutput()
	: min(-64)
	, max(+64)
	, depth(-1)
	, selectivity(0)
	, PV(CMove::invalid)
	, AV(CMove::invalid)
{
	assert(Constrained());
}

COutput::COutput(int8_t min, int8_t max, int8_t depth, uint8_t selectivity, CMove PV, CMove AV)
	: min(min)
	, max(max)
	, depth(depth)
	, selectivity(selectivity)
	, PV(PV)
	, AV(AV)
{
	assert(Constrained());
}

COutput::COutput(int8_t score, int8_t depth, uint8_t selectivity, CMove PV, CMove AV)
	: COutput(score, score, depth, selectivity, PV, AV)
{
	assert(Constrained());
}

COutput Search::operator-(const COutput & o)
{
	return COutput(-o.max, -o.min, o.depth, o.selectivity, o.PV, o.AV);
}