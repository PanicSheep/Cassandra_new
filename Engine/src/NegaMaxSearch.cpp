#include "NegaMaxSearch.h"

#include <algorithm>

#include "LastFlipCounter.h"
#include "FlipFast.h"

using namespace Search;

std::unique_ptr<CAlgorithm> NegaMax::Clone() const
{
	return std::make_unique<NegaMax>(*this);
}

CResult NegaMax::Eval(const CPosition& pos, CSpecification)
{
	const auto old_node_counter = node_counter;
	const auto start_time = std::chrono::high_resolution_clock::now();
	
	const auto score = Eval_(pos);

	const auto end_time = std::chrono::high_resolution_clock::now();
	const auto duration = end_time - start_time;
	const auto node_count = node_counter - old_node_counter;
	return CResult(score, node_count, duration);
}

int NegaMax::Eval_(const CPosition& pos)
{
	switch (pos.EmptyCount())
	{
		case 0: return Eval_0(pos);
		case 1: return Eval_1(pos);
		case 2: return Eval_2(pos);
		case 3: return Eval_3(pos);
		case 4: return Eval_4(pos);
		default: return Eval_N(pos);;
	}
}

int NegaMax::Eval_1(const CPosition& pos)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();

	return Eval_1(pos, move1);
}

int NegaMax::Eval_2(const CPosition& pos)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();
	const auto move2 = moves.ExtractMove();

	return Eval_2(pos, move1, move2);
}

int NegaMax::Eval_3(const CPosition& pos)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();
	const auto move2 = moves.ExtractMove();
	const auto move3 = moves.ExtractMove();

	return Eval_3(pos, move1, move2, move3);
}

int NegaMax::Eval_4(const CPosition& pos)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();
	const auto move2 = moves.ExtractMove();
	const auto move3 = moves.ExtractMove();
	const auto move4 = moves.ExtractMove();

	return Eval_4(pos, move1, move2, move3, move4);
}

int NegaMax::Eval_0(const CPosition& pos)
{
	node_counter++;
	return EvalGameOver(pos);
}

int NegaMax::Eval_1(const CPosition& pos, const CMove move1)
{
	node_counter++;
	const int score = static_cast<int>(2 * PopCount(pos.GetP())) - 63; // == PopCount(pos.GetP()) - PopCount(pos.GetO())

	if (const auto diff = engine->CountLastFlip(pos, move1))
	{
		node_counter++;
		return score + diff + 1;
	}
	else if (const auto diff = engine->CountLastFlip(pos.PlayPass(), move1))
	{
		node_counter += 2;
		return score - diff - 1;
	}
	else
		return (score > 0) ? score + 1 : score - 1;
}

int NegaMax::Eval_2(const CPosition& pos, const CMove move1, const CMove move2)
{
	node_counter++;
	int score = -infinity;

	if (const auto flips = Flip(pos, move1))
		score = std::max(score, -Eval_1(pos.Play(move1, flips), move2));

	if (const auto flips = Flip(pos, move2))
		score = std::max(score, -Eval_1(pos.Play(move2, flips), move1));

	if (score != -infinity)
		return score;

	const auto posPass = pos.PlayPass();
	score = infinity;

	if (const auto flips = Flip(posPass, move1))
		score = std::min(score, Eval_1(posPass.Play(move1, flips), move2));

	if (const auto flips = Flip(posPass, move2))
		score = std::min(score, Eval_1(posPass.Play(move2, flips), move1));

	if (score != infinity) {
		node_counter++;
		return score;
	}
	else
		return -EvalGameOver(posPass);
}

int NegaMax::Eval_3(const CPosition& pos, const CMove move1, const CMove move2, const CMove move3)
{
	node_counter++;
	int score = -infinity;

	if (const auto flips = Flip(pos, move1))
		score = std::max(score, -Eval_2(pos.Play(move1, flips), move2, move3));

	if (const auto flips = Flip(pos, move2))
		score = std::max(score, -Eval_2(pos.Play(move2, flips), move1, move3));

	if (const auto flips = Flip(pos, move3))
		score = std::max(score, -Eval_2(pos.Play(move3, flips), move1, move2));

	if (score != -infinity)
		return score;

	const auto posPass = pos.PlayPass();
	score = infinity;

	if (const auto flips = Flip(posPass, move1))
		score = std::min(score, Eval_2(posPass.Play(move1, flips), move2, move3));

	if (const auto flips = Flip(posPass, move2))
		score = std::min(score, Eval_2(posPass.Play(move2, flips), move1, move3));

	if (const auto flips = Flip(posPass, move3))
		score = std::min(score, Eval_2(posPass.Play(move3, flips), move1, move2));

	if (score != infinity) {
		node_counter++;
		return score;
	}
	else
		return -EvalGameOver(posPass);
}

int NegaMax::Eval_4(const CPosition& pos, const CMove move1, const CMove move2, const CMove move3, const CMove move4)
{
	node_counter++;
	int score = -infinity;

	if (const auto flips = Flip(pos, move1))
		score = std::max(score, -Eval_3(pos.Play(move1, flips), move2, move3, move4));

	if (const auto flips = Flip(pos, move2))
		score = std::max(score, -Eval_3(pos.Play(move2, flips), move1, move3, move4));

	if (const auto flips = Flip(pos, move3))
		score = std::max(score, -Eval_3(pos.Play(move3, flips), move1, move2, move4));

	if (const auto flips = Flip(pos, move4))
		score = std::max(score, -Eval_3(pos.Play(move4, flips), move1, move2, move3));

	if (score != -infinity)
		return score;

	const auto posPass = pos.PlayPass();
	score = infinity;

	if (const auto flips = Flip(posPass, move1))
		score = std::min(score, Eval_3(posPass.Play(move1, flips), move2, move3, move4));

	if (const auto flips = Flip(posPass, move2))
		score = std::min(score, Eval_3(posPass.Play(move2, flips), move1, move3, move4));

	if (const auto flips = Flip(posPass, move3))
		score = std::min(score, Eval_3(posPass.Play(move3, flips), move1, move2, move4));

	if (const auto flips = Flip(posPass, move4))
		score = std::min(score, Eval_3(posPass.Play(move4, flips), move1, move2, move3));

	if (score != infinity) {
		node_counter++;
		return score;
	}
	else
		return -EvalGameOver(posPass);
}

int NegaMax::Eval_N(const CPosition& pos)
{
	node_counter++;

	CMoves moves = pos.PossibleMoves();
	if (moves.empty()) {
		const auto PosPass = pos.PlayPass();
		if (PosPass.HasMoves())
			return -Eval_(PosPass);
		else
			return EvalGameOver(pos);
	}

	int score = -infinity;
	while (!moves.empty())
		score = std::max(score, -Eval_(pos.Play(moves.ExtractMove())));

	return score;
}
