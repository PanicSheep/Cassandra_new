#include "Search.h"
#include "LastFlipCounter.h"
#include "FlipFast.h"
#include <algorithm>

int NegaMaxSearch::Eval(const CPosition& pos)
{
	const auto empties = pos.EmptyCount();
	switch (empties)
	{
		case 0: return Eval_0(pos);
		case 1: return Eval_1(pos);
		case 2: return Eval_2(pos);
		case 3: return Eval_3(pos);
		case 4: return Eval_4(pos);
	}

	NodeCounter[empties]++;

	CMoves moves = pos.PossibleMoves();
	if (moves.empty()) {
		const auto NextPos = pos.PlayPass();
		if (NextPos.HasMoves())
			return -Eval(NextPos);
		else
			return EvalGameOver(pos);
	}

	int score = -99;
	auto a = -score;
	while (!moves.empty())
		score = std::max(score, (int)-Eval(pos.Play(moves.ExtractMove())));

	return score;
}

int NegaMaxSearch::Eval_0(const CPosition& pos)
{
	NodeCounter[0]++;
	return EvalGameOver<0>(pos);
}

int NegaMaxSearch::Eval_1(const CPosition& pos)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();

	return Eval_1(pos, move1);
}

int NegaMaxSearch::Eval_2(const CPosition& pos)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();
	const auto move2 = moves.ExtractMove();

	return Eval_2(pos, move1, move2);
}

int NegaMaxSearch::Eval_3(const CPosition& pos)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();
	const auto move2 = moves.ExtractMove();
	const auto move3 = moves.ExtractMove();

	return Eval_3(pos, move1, move2, move3);
}

int NegaMaxSearch::Eval_4(const CPosition& pos)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();
	const auto move2 = moves.ExtractMove();
	const auto move3 = moves.ExtractMove();
	const auto move4 = moves.ExtractMove();

	return Eval_4(pos, move1, move2, move3, move4);
}

int NegaMaxSearch::Eval_1(const CPosition& pos, const CMove& move1)
{
	NodeCounter[1]++;
	const int score = static_cast<int>(2 * PopCount(pos.GetP())) - 63; // == PopCount(pos.GetP()) - PopCount(pos.GetO())

	if (const auto Diff = CountLastFlip(pos, move1))
	{
		NodeCounter[0]++;
		return score + Diff + 1;
	}
	else if (const auto Diff = CountLastFlip(pos.PlayPass(), move1))
	{
		NodeCounter[1]++;
		NodeCounter[0]++;
		return score - Diff - 1;
	}
	else
	{
		NodeCounter[1]++;
		return (score > 0) ? score + 1 : score - 1;
	}
}

int NegaMaxSearch::Eval_2(CPosition pos, const CMove& move1, const CMove& move2)
{
	NodeCounter[2]++;
	int score = -128;

	if (pos.GetO() & Neighbour(move1.field))
		if (const auto flips = Flip(pos, move1))
			score = std::max(score, -Eval_1(pos.Play(move1, flips), move2));

	if (pos.GetO() & Neighbour(move2.field))
		if (const auto flips = Flip(pos, move2))
			score = std::max(score, -Eval_1(pos.Play(move2, flips), move1));

	if (score != -128)
		return score;

	pos = pos.PlayPass();
	score = 128;

	if (pos.GetO() & Neighbour(move1.field))
		if (const auto flips = Flip(pos, move1))
			score = std::min(score, Eval_1(pos.Play(move1, flips), move2));

	if (pos.GetO() & Neighbour(move2.field))
		if (const auto flips = Flip(pos, move2))
			score = std::min(score, Eval_1(pos.Play(move2, flips), move1));

	if (score != 128) {
		NodeCounter[2]++;
		return score;
	}
	else
		return EvalGameOver<2>(pos);
}

int NegaMaxSearch::Eval_3(CPosition pos, const CMove& move1, const CMove& move2, const CMove& move3)
{
	NodeCounter[3]++;
	int score = -128;

	if (pos.GetO() & Neighbour(move1.field))
		if (const auto flips = Flip(pos, move1))
			score = std::max(score, -Eval_2(pos.Play(move1, flips), move2, move3));

	if (pos.GetO() & Neighbour(move2.field))
		if (const auto flips = Flip(pos, move2))
			score = std::max(score, -Eval_2(pos.Play(move2, flips), move1, move3));

	if (pos.GetO() & Neighbour(move3.field))
		if (const auto flips = Flip(pos, move3))
			score = std::max(score, -Eval_2(pos.Play(move3, flips), move1, move2));

	if (score != -128)
		return score;

	pos = pos.PlayPass();
	score = 128;

	if (pos.GetO() & Neighbour(move1.field))
		if (const auto flips = Flip(pos, move1))
			score = std::min(score, Eval_2(pos.Play(move1, flips), move2, move3));

	if (pos.GetO() & Neighbour(move2.field))
		if (const auto flips = Flip(pos, move2))
			score = std::min(score, Eval_2(pos.Play(move2, flips), move1, move3));

	if (pos.GetO() & Neighbour(move3.field))
		if (const auto flips = Flip(pos, move3))
			score = std::min(score, Eval_2(pos.Play(move3, flips), move1, move2));

	if (score != 128) {
		NodeCounter[3]++;
		return score;
	}
	else
		return EvalGameOver<3>(pos);
}

int NegaMaxSearch::Eval_4(CPosition pos, const CMove& move1, const CMove& move2, const CMove& move3, const CMove& move4)
{
	NodeCounter[4]++;
	int score = -128;

	if (pos.GetO() & Neighbour(move1.field))
		if (const auto flips = Flip(pos, move1))
			score = std::max(score, -Eval_3(pos.Play(move1, flips), move2, move3, move4));

	if (pos.GetO() & Neighbour(move2.field))
		if (const auto flips = Flip(pos, move2))
			score = std::max(score, -Eval_3(pos.Play(move2, flips), move1, move3, move4));

	if (pos.GetO() & Neighbour(move3.field))
		if (const auto flips = Flip(pos, move3))
			score = std::max(score, -Eval_3(pos.Play(move3, flips), move1, move2, move4));

	if (pos.GetO() & Neighbour(move3.field))
		if (const auto flips = Flip(pos, move3))
			score = std::max(score, -Eval_3(pos.Play(move3, flips), move1, move2, move3));

	if (score != -128)
		return score;

	pos = pos.PlayPass();
	score = 128;

	if (pos.GetO() & Neighbour(move1.field))
		if (const auto flips = Flip(pos, move1))
			score = std::min(score, Eval_3(pos.Play(move1, flips), move2, move3, move4));

	if (pos.GetO() & Neighbour(move2.field))
		if (const auto flips = Flip(pos, move2))
			score = std::min(score, Eval_3(pos.Play(move2, flips), move1, move3, move4));

	if (pos.GetO() & Neighbour(move3.field))
		if (const auto flips = Flip(pos, move3))
			score = std::min(score, Eval_3(pos.Play(move3, flips), move1, move2, move4));

	if (pos.GetO() & Neighbour(move3.field))
		if (const auto flips = Flip(pos, move3))
			score = std::min(score, Eval_3(pos.Play(move3, flips), move1, move2, move3));

	if (score != 128) {
		NodeCounter[4]++;
		return score;
	}
	else
		return EvalGameOver<4>(pos);
}