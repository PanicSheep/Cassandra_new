#include "Search.h"

Search::Search(const std::shared_ptr<Engine>& engine)
	: engine(engine)
	, node_counter(0)
{}

uint64_t Search::NodeCount() const
{
	return node_counter;
}

int Search::EvalGameOver(const CPosition& pos)
{
	const auto Ps = static_cast<unsigned int>(PopCount(pos.GetP()));
	const auto Os = static_cast<unsigned int>(PopCount(pos.GetO()));
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
	if (Diff > 0) 
		return Diff + static_cast<int>(EmptyCount);
	else if (Diff < 0)
		return Diff - static_cast<int>(EmptyCount);
	else
		return Diff;
}