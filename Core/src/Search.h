#pragma once
#include <cstdint>

#include "Position.h"
#include "Environment.h"

class Search
{
	uint64_t node_counter;
public:
	Search(const std::shared_ptr<Environment>&);

	virtual int Eval(const CPosition&) = 0;
	virtual int Eval(const CPosition& pos, int8_t depth, uint8_t selectivity) { return Eval(pos); }

	uint64_t GetNodeCount() const;
	uint64_t GetNodeCount(uint64_t EmptyCount) const;
	void ResetNodeCount();

protected:
	std::shared_ptr<Environment> environment;

	uint64_t& NodeCounter(std::size_t index);

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