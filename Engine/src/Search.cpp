#include "Search.h"
#include <iterator>
#include <numeric>

Search::Search() : node_counter(0)
{}

uint64_t Search::GetNodeCount() const
{
	return node_counter;
}

uint64_t Search::GetNodeCount(uint64_t EmptyCount) const
{
	return node_counter;
}

void Search::ResetNodeCount()
{
	node_counter = 0;
}

uint64_t & Search::NodeCounter(std::size_t index)
{
	return node_counter;
}

uint64_t Neighbour(Field field)
{
	static const uint64_t neighbour[64] = { // Neighbours to the input-field
		0x0000000000000302ULL, 0x0000000000000705ULL, 0x0000000000000e0aULL, 0x0000000000001c14ULL,	0x0000000000003828ULL, 0x0000000000007050ULL, 0x000000000000e0a0ULL, 0x000000000000c040ULL,
		0x0000000000030203ULL, 0x0000000000070507ULL, 0x00000000000e0a0eULL, 0x00000000001c141cULL,	0x0000000000382838ULL, 0x0000000000705070ULL, 0x0000000000e0a0e0ULL, 0x0000000000c040c0ULL,
		0x0000000003020300ULL, 0x0000000007050700ULL, 0x000000000e0a0e00ULL, 0x000000001c141c00ULL,	0x0000000038283800ULL, 0x0000000070507000ULL, 0x00000000e0a0e000ULL, 0x00000000c040c000ULL,
		0x0000000302030000ULL, 0x0000000705070000ULL, 0x0000000e0a0e0000ULL, 0x0000001c141c0000ULL,	0x0000003828380000ULL, 0x0000007050700000ULL, 0x000000e0a0e00000ULL, 0x000000c040c00000ULL,
		0x0000030203000000ULL, 0x0000070507000000ULL, 0x00000e0a0e000000ULL, 0x00001c141c000000ULL,	0x0000382838000000ULL, 0x0000705070000000ULL, 0x0000e0a0e0000000ULL, 0x0000c040c0000000ULL,
		0x0003020300000000ULL, 0x0007050700000000ULL, 0x000e0a0e00000000ULL, 0x001c141c00000000ULL,	0x0038283800000000ULL, 0x0070507000000000ULL, 0x00e0a0e000000000ULL, 0x00c040c000000000ULL,
		0x0302030000000000ULL, 0x0705070000000000ULL, 0x0e0a0e0000000000ULL, 0x1c141c0000000000ULL,	0x3828380000000000ULL, 0x7050700000000000ULL, 0xe0a0e00000000000ULL, 0xc040c00000000000ULL,
		0x0203000000000000ULL, 0x0507000000000000ULL, 0x0a0e000000000000ULL, 0x141c000000000000ULL,	0x2838000000000000ULL, 0x5070000000000000ULL, 0xa0e0000000000000ULL, 0x40c0000000000000ULL
	};
	return neighbour[field];
}

int Search::EvalGameOver(const CPosition& pos)
{
	const unsigned int Ps = static_cast<int>(PopCount(pos.GetP()));
	const unsigned int Os = static_cast<int>(PopCount(pos.GetO()));
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