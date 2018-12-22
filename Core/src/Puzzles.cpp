#include "Puzzles.h"
#include <stdexcept>

void CPuzzle::Solve(Search::CAlgorithm& algorithm, Search::ILog& log)
{
	return Solve(algorithm, log, Search::CSpecification::SolveExact(pos));
}

void CPuzzle::Solve(Search::CAlgorithm& algorithm, Search::ILog& log, Search::CSpecification spec)
{
	if (spec.depth > pos.EmptyCount())
		spec.depth = pos.EmptyCount();

	const auto result = algorithm.Eval(pos, spec);
	log.push_back(pos, Puzzle::DEFAULT_SCORE, spec, result);
}

bool CPuzzleAllDepthScore::Test() const
{
	if (!CPuzzle::Test())
		return false;

	const auto emptyCount = GetPosition().EmptyCount();
	const uint64_t size = std::distance(std::begin(score), std::end(score));
	const auto UpperLimit = std::min(emptyCount, size - 1);
	for (uint64_t i = 0; i <= UpperLimit; i++)
		if (!(((score[i] >= -64) && (score[i] <= 64)) || (score[i] == Puzzle::DEFAULT_SCORE)))
			return false;
	for (uint64_t i = emptyCount + 1; i < size; i++)
		if (score[i] != Puzzle::DEFAULT_SCORE)
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
	return score[depth] == Puzzle::DEFAULT_SCORE;
}

void CPuzzleAllDepthScore::Solve(Search::CAlgorithm& algorithm, Search::ILog& log)
{
	return Solve(algorithm, log, Search::CSpecification::SolveExact(pos));
}

void CPuzzleAllDepthScore::Solve(Search::CAlgorithm& algorithm, Search::ILog& log, Search::CSpecification spec)
{
	if (spec.depth > pos.EmptyCount())
		spec.depth = pos.EmptyCount();
	const auto original_depth = spec.depth;

	for (int8_t d = 0; d <= original_depth; d++)
	{
		const int original_score = score[d];
		spec.depth = d;
		const auto result = algorithm.Eval(pos, spec);
		score[d] = result.score;
		log.push_back(pos, original_score, spec, result);
	}
}

template <typename T> struct reversion_wrapper { T& iterable; };

template <typename T> auto begin(reversion_wrapper<T> w) { return std::rbegin(w.iterable); }

template <typename T> auto end(reversion_wrapper<T> w) { return std::rend(w.iterable); }

template <typename T> reversion_wrapper<T> reverse(T&& iterable) { return { iterable }; }

int8_t CPuzzleAllDepthScore::MaxSolvedDepth() const
{
	for (const auto& it : reverse(score))
		if (it != Puzzle::DEFAULT_SCORE)
			return static_cast<int8_t>(std::distance(std::begin(score), &it));
	return -1;
}

bool CPuzzleAllDepthScore::isEqual(const CPuzzle & o) const
{
	if (!CPuzzle::isEqual(o))
		return false;

	const auto& O = dynamic_cast<const CPuzzleAllDepthScore&>(o);
	for (std::size_t i = 0; i < sizeof(score); i++)
		if (score[i] != O.score[i])
			return false;
	return true;
}

void CPuzzleScore::Solve(Search::CAlgorithm& algorithm, Search::ILog& log)
{
	return Solve(algorithm, log, Search::CSpecification::SolveExact(pos));
}

void CPuzzleScore::Solve(Search::CAlgorithm& algorithm, Search::ILog& log, Search::CSpecification spec)
{
	if (spec.depth > pos.EmptyCount())
		spec.depth = pos.EmptyCount();

	const int original_score = score;
	const auto result = algorithm.Eval(pos, spec);
	score = result.score;
	log.push_back(pos, original_score, spec, result);
}

bool CPuzzleScore::isEqual(const CPuzzle & o) const
{
	const auto& O = dynamic_cast<const CPuzzleScore&>(o);
	return CPuzzle::isEqual(o) && (score == O.score);
}

void CPuzzleScoreDepth::Solve(Search::CAlgorithm& algorithm, Search::ILog& log)
{
	return Solve(algorithm, log, Search::CSpecification(depth, selectivity));
}

void CPuzzleScoreDepth::Solve(Search::CAlgorithm& algorithm, Search::ILog& log, Search::CSpecification spec)
{
	if (spec.depth > pos.EmptyCount())
		spec.depth = pos.EmptyCount();

	const int original_score = score;
	const auto result = algorithm.Eval(pos, spec);
	score = result.score;
	depth = spec.depth;
	selectivity = spec.selectivity;
	log.push_back(pos, original_score, spec, result);
}

bool CPuzzleScoreDepth::isEqual(const CPuzzle & o) const
{
	const auto& O = dynamic_cast<const CPuzzleScoreDepth&>(o);
	return CPuzzleScore::isEqual(o) && (depth == O.depth) && (selectivity == O.selectivity);
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

bool CPuzzleAllMoveScore::Test() const
{
	if (!CPuzzle::Test())
		return false;

	const auto possibleMoves = GetPosition().PossibleMoves();
	for (const auto& it : score)
	{
		const auto move = static_cast<CMove>(std::distance(std::begin(score), &it));
		if (possibleMoves.HasMove(move))
		{
			if (!(((it >= -64) && (it <= 64)) || (it == Puzzle::DEFAULT_SCORE)))
				return false;
		}
		else
		{
			if (it != Puzzle::DEFAULT_SCORE)
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
		if (score[move] == Puzzle::DEFAULT_SCORE)
			return false;
	}
	return true;
}

void CPuzzleAllMoveScore::Solve(Search::CAlgorithm& algorithm, Search::ILog& log) // TODO: Remove!
{
	auto moves = pos.PossibleMoves();
	while (!moves.empty())
	{
		const CMove move = moves.ExtractMove();
		if (score[move] == Puzzle::DEFAULT_SCORE)
		{
			const auto original_pos = pos.Play(move);
			const int original_score = score[move];
			const auto spec = Search::CSpecification::SolveExact(original_pos);
			const auto result = algorithm.Eval(original_pos, spec);
			score[move] = result.score;
			log.push_back(original_pos, original_score, spec, result);
		}
	}
}

void CPuzzleAllMoveScore::Solve(Search::CAlgorithm& algorithm, Search::ILog& log, Search::CSpecification spec)
{
	if (spec.depth > pos.EmptyCount())
		spec.depth = pos.EmptyCount();
	spec.depth--;

	auto moves = pos.PossibleMoves();
	while (!moves.empty())
	{
		const CMove move = moves.ExtractMove();
		if (score[move] == Puzzle::DEFAULT_SCORE)
		{
			const auto original_pos = pos.Play(move);
			const int original_score = score[move];
			const auto result = algorithm.Eval(original_pos, spec);
			score[move] = result.score;
			log.push_back(original_pos, original_score, spec, result);
		}
	}
}

bool CPuzzleAllMoveScore::isEqual(const CPuzzle & o) const
{
	if (!CPuzzle::isEqual(o))
		return false;

	const auto& O = dynamic_cast<const CPuzzleAllMoveScore&>(o);
	for (std::size_t i = 0; i < sizeof(score); i++)
		if (score[i] != O.score[i])
			return false;
	return true;
}
