#pragma once
#include <cstdint>
#include "Position.h"

class Search
{
public:
	Search();

	virtual int Eval(const CPosition&) = 0;

	uint64_t GetNodeCount() const;
	uint64_t GetNodeCount(uint64_t EmptyCount) const;

protected:
	uint64_t NodeCounter[60];

	template <int EmptyCount>
	static int EvalGameOver(const CPosition&);
	static int EvalGameOver(const CPosition&);
	static int EvalGameOver(const CPosition&, const uint64_t EmptyCount);
};

class NegaMaxSearch : public Search
{
public:
	NegaMaxSearch() : Search() {}

	int Eval(const CPosition&) override;
private:
	int Eval_0(const CPosition&);
	int Eval_1(const CPosition&);
	int Eval_2(const CPosition&);
	int Eval_3(const CPosition&);
	int Eval_4(const CPosition&);

	int Eval_1(const CPosition&, const CMove& move1);
	int Eval_2(CPosition, const CMove& move1, const CMove& move2);
	int Eval_3(CPosition, const CMove& move1, const CMove& move2, const CMove& move3);
	int Eval_4(CPosition, const CMove& move1, const CMove& move2, const CMove& move3, const CMove& move4);
};

inline uint64_t Neighbour(Field field)
{
	const uint64_t neighbour[64] = { // Neighbours to the input-field
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

template <int EmptyCount>
inline int Search::EvalGameOver(const CPosition& pos)
{
	const int Diff = 2 * static_cast<int>(PopCount(pos.GetP())) + EmptyCount - 64;
	if (EmptyCount % 2 == 0)
	{
		if (Diff > 0) return Diff + EmptyCount;
		else if (Diff < 0) return Diff - EmptyCount;
		else               return Diff;
	}
	else
	{
		if (Diff > 0) return Diff + EmptyCount;
		else          return Diff - EmptyCount;
	}
}

template <>
inline int Search::EvalGameOver<0>(const CPosition& pos)
{
	return static_cast<int>(2 * PopCount(pos.GetP())) - 64;
}