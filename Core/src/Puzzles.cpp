#include "Puzzles.h"

bool CPuzzleAllDepthScore::Test() const
{
	if (CPuzzle::Test() == false)
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

bool CPuzzleAllDepthScore::IsSolved() const
{
	for (uint64_t d = 0; d <= GetPosition().EmptyCount(); d++)
		if (IsSolved(d))
			return false;
	return true;
}

bool CPuzzleAllDepthScore::IsSolved(int8_t depth) const
{
	return score[depth] == DEFAULT_SCORE;
}

void CPuzzleAllDepthScore::Solve(Search& search)
{
	for (int8_t d = 0; d < sizeof(score); d++)
		if (IsSolved(d) && (d <= pos.EmptyCount()))
			score[d] = search.Eval(pos, d, 0);
}

template <typename T> struct reversion_wrapper { T& iterable; };

template <typename T> auto begin(reversion_wrapper<T> w) { return std::rbegin(w.iterable); }

template <typename T> auto end(reversion_wrapper<T> w) { return std::rend(w.iterable); }

template <typename T> reversion_wrapper<T> reverse(T&& iterable) { return { iterable }; }

int8_t CPuzzleAllDepthScore::MaxSolvedDepth() const
{
	for (const auto& it : reverse(score))
		if (it != DEFAULT_SCORE)
			return static_cast<int8_t>(std::distance(std::begin(score), &it));
	return -1;
}

bool CPuzzleAllDepthScore::isEqual(const CPuzzle & o) const
{
	if (CPuzzle::isEqual(o) == false)
		return false;

	const auto& O = dynamic_cast<const CPuzzleAllDepthScore&>(o);
	for (std::size_t i = 0; i < sizeof(score); i++)
		if (score[i] != O.score[i])
			return false;
	return true;
}

bool CPuzzleAllMoveScore::Test() const
{
	if (CPuzzle::Test() == false)
		return false;

	const auto possibleMoves = GetPosition().PossibleMoves();
	for (const auto& it : score)
	{
		const auto move = static_cast<CMove>(std::distance(std::begin(score), &it));
		if (possibleMoves.HasMove(move))
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

bool CPuzzleAllMoveScore::IsSolved() const
{
	auto possibleMoves = GetPosition().PossibleMoves();
	while (!possibleMoves.empty())
	{
		const auto move = possibleMoves.ExtractMove();
		if (score[move] == DEFAULT_SCORE)
			return false;
	}
	return true;
}

void CPuzzleAllMoveScore::Solve(Search& search)
{
	auto moves = pos.PossibleMoves();
	while (!moves.empty())
	{
		const CMove move = moves.ExtractMove();
		if (score[move] == DEFAULT_SCORE)
			score[move] = search.Eval(pos.Play(move));
	}
}

bool CPuzzleAllMoveScore::isEqual(const CPuzzle & o) const
{
	if (CPuzzle::isEqual(o) == false)
		return false;

	const auto& O = dynamic_cast<const CPuzzleAllMoveScore&>(o);
	for (std::size_t i = 0; i < sizeof(score); i++)
		if (score[i] != O.score[i])
			return false;
	return true;
}

void CPuzzleScore::Solve(Search& search)
{
	score = search.Eval(pos);
}

bool CPuzzleScore::isEqual(const CPuzzle & o) const
{
	const auto& O = dynamic_cast<const CPuzzleScore&>(o);
	return CPuzzle::isEqual(o) && (score == O.score);
}

void CPuzzleScoreDepth::Solve(Search& search)
{
	score = search.Eval(pos, depth, selectivity);
}

bool CPuzzleScoreDepth::isEqual(const CPuzzle & o) const
{
	const auto& O = dynamic_cast<const CPuzzleScoreDepth&>(o);
	return CPuzzle::isEqual(o) && (depth == O.depth) && (selectivity == O.selectivity);
}

std::unique_ptr<CPuzzleAllDepthScore> to_PuzzleAllDepthScore(const CPuzzle & puzzle)
{
	if (const auto tmp = dynamic_cast<const CPuzzleScore*>(&puzzle))
	{
		const auto position = tmp->GetPosition();
		auto ret = std::make_unique<CPuzzleAllDepthScore>(position);
		ret->score[position.EmptyCount()] = tmp->score;
		return ret;
	}
	throw std::runtime_error("not implemented");
}
