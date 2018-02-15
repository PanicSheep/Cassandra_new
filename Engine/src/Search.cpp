#include "Search.h"
#include <iterator>
#include <numeric>

Search::Search()
{
	for (auto& it : NodeCounter)
		it = 0;
}

uint64_t Search::GetNodeCount() const
{
	return std::accumulate(std::begin(NodeCounter), std::end(NodeCounter), 0);
}

uint64_t Search::GetNodeCount(uint64_t EmptyCount) const
{
	return NodeCounter[EmptyCount];
}

int Search::EvalGameOver(const CPosition& pos)
{
	const int Ps = static_cast<int>(PopCount(pos.GetP()));
	const int Os = static_cast<int>(PopCount(pos.GetO()));
	if (Ps > Os)
		return 64 - 2 * Os;
	else if (Ps < Os)
		return 2 * Ps - 64;
	else
		return Ps - Os;
}

int Search::EvalGameOver(const CPosition& pos, const uint64_t EmptyCount)
{
	assert(EmptyCount >= 0);
	assert(EmptyCount <= 64);

	const int Diff = static_cast<int>(2 * PopCount(pos.GetP()) + EmptyCount) - 64;
	     if (Diff > 0) return Diff + static_cast<int>(EmptyCount);
	else if (Diff < 0) return Diff - static_cast<int>(EmptyCount);
	else               return Diff;
}