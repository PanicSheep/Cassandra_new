#include "Boards.h"

namespace IO
{
	bool CBoardAllDepthScore::Test() const
	{
		if (CBoard::Test() == false)
			return false;

		const auto emptyCount = GetPosition().EmptyCount();
		const uint64_t size = std::distance(std::begin(score), std::end(score));
		const auto UpperLimit = std::min(emptyCount, size - 1);
		for (uint64_t i = 0; i <= UpperLimit; i++)
			if (!(((score[i] >= -64) && (score[i] <= 64)) || (score[i] == DEFAULT_SCORE)))
				return false;
		for (uint64_t i = emptyCount + 1; i < size; i++)
			if (score[i] != DEFAULT_SCORE)
				return false;
		return true;
	}

	bool CBoardAllDepthScore::IsSolved() const
	{
		for (uint64_t i = 0; i <= GetPosition().EmptyCount(); i++)
			if (score[i] == DEFAULT_SCORE)
				return false;
		return true;
	}

	template <typename T> struct reversion_wrapper { T& iterable; };

	template <typename T> auto begin(reversion_wrapper<T> w) { return std::rbegin(w.iterable); }

	template <typename T> auto end(reversion_wrapper<T> w) { return std::rend(w.iterable); }

	template <typename T> reversion_wrapper<T> reverse(T&& iterable) { return { iterable }; }

	int8_t CBoardAllDepthScore::MaxSolvedDepth() const
	{
		for (const auto& it : reverse(score))
			if (it != DEFAULT_SCORE)
				return static_cast<int8_t>(std::distance(std::begin(score), &it));
		return -1;
	}

	bool CBoardAllMoveScore::Test() const
	{
		if (CBoard::Test() == false)
			return false;

		const auto possibleMoves = GetPosition().PossibleMoves();
		for (const auto& it : score)
		{
			if (possibleMoves.HasMove(std::distance(std::begin(score), &it)))
			{
				if (!(((it >= -64) && (it <= 64)) || (it == DEFAULT_SCORE)))
					return false;
			}
			else
			{
				if (it != DEFAULT_SCORE)
					return false;
			}
		}
		return true;
	}

	bool CBoardAllMoveScore::IsSolved() const
	{
		auto possibleMoves = GetPosition().PossibleMoves();
		while (!possibleMoves.empty())
		{
			const auto move = possibleMoves.ExtractMove();
			if (score[move.field] == DEFAULT_SCORE)
				return false;
		}
		return true;
	}
}