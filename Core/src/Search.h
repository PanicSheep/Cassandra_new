#pragma once
#include <cstdint>

#include "Position.h"

class Engine;

class Search
{
public:
	Search(const std::shared_ptr<Engine>&);

	virtual int Eval(const CPosition&) = 0;
	virtual int Eval(const CPosition& pos, int8_t depth, uint8_t selectivity) { return Eval(pos); }

	uint64_t NodeCount() const;

protected:
	uint64_t node_counter;
	std::shared_ptr<Engine> engine;
	
	template <int EmptyCount>
	static int EvalGameOver(const CPosition&);
	static int EvalGameOver(const CPosition&);
	static int EvalGameOver(const CPosition&, const uint64_t EmptyCount);
};

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