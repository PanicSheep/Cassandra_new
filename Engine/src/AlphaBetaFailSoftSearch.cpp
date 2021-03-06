#include "AlphaBetaFailSoftSearch.h"

#include <algorithm>

#include "FlipFast.h"

using namespace Search;

std::unique_ptr<CAlgorithm> AlphaBetaFailSoft::Clone() const
{
	return std::make_unique<AlphaBetaFailSoft>(*this);
}

CResult AlphaBetaFailSoft::Eval(const CPosition& pos, CSpecification)
{
	const auto old_node_counter = node_counter;
	const auto start_time = std::chrono::high_resolution_clock::now();
	
	const auto score = Eval(pos, -infinity, infinity);

	const auto end_time = std::chrono::high_resolution_clock::now();
	const auto duration = end_time - start_time;
	const auto node_count = node_counter - old_node_counter;
	return CResult(score, node_count, duration);
}

int AlphaBetaFailSoft::Eval(const CPosition& pos, int alpha, int beta)
{
	switch (pos.EmptyCount())
	{
		case 0: return Eval_0(pos);
		case 1: return Eval_1(pos);
		case 2: return Eval_2(pos, alpha, beta);
		case 3: return Eval_3(pos, alpha, beta);
		case 4: return Eval_4(pos, alpha, beta);
		default: return Eval_N(pos, alpha, beta);
	}
}

int AlphaBetaFailSoft::Eval_2(const CPosition& pos, int alpha, int beta)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();
	const auto move2 = moves.ExtractMove();

	return Eval_2(pos, alpha, beta, move1, move2);
}

int AlphaBetaFailSoft::Eval_3(const CPosition& pos, int alpha, int beta)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();
	const auto move2 = moves.ExtractMove();
	const auto move3 = moves.ExtractMove();

	return Eval_3(pos, alpha, beta, move1, move2, move3);
}

int AlphaBetaFailSoft::Eval_4(const CPosition& pos, int alpha, int beta)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();
	const auto move2 = moves.ExtractMove();
	const auto move3 = moves.ExtractMove();
	const auto move4 = moves.ExtractMove();

	return Eval_4(pos, alpha, beta, move1, move2, move3, move4);
}

int AlphaBetaFailSoft::Eval_2(const CPosition& pos, int alpha, int beta, const CMove move1, const CMove move2)
{
	node_counter++;
	int bestscore = -infinity;

	if (const auto flips = Flip(pos, move1)) {
		bestscore = -Eval_1(pos.Play(move1, flips), move2);
		if (bestscore >= beta) return bestscore;
	}

	if (const auto flips = Flip(pos, move2))
		return std::max(bestscore, -Eval_1(pos.Play(move2, flips), move1));

	if (bestscore != -infinity)
		return bestscore;
	bestscore = infinity;

	const auto posPass = pos.PlayPass();
	node_counter++;

	if (const auto flips = Flip(posPass, move1)) {
		bestscore = Eval_1(posPass.Play(move1, flips), move2);
		if (bestscore <= alpha) return bestscore;
	}

	if (const auto flips = Flip(posPass, move2))
		return std::min(bestscore, Eval_1(posPass.Play(move2, flips), move1));

	if (bestscore != infinity)
		return bestscore;
	
	node_counter--;
	return -EvalGameOver(posPass);
}

int AlphaBetaFailSoft::Eval_3(const CPosition& pos, int alpha, int beta, const CMove move1, const CMove move2, const CMove move3)
{
	node_counter++;
	int bestscore = -infinity;

	if (const auto flips = Flip(pos, move1)) {
		bestscore = -Eval_2(pos.Play(move1, flips), -beta, -alpha, move2, move3);
		if (bestscore >= beta) return bestscore;
		if (bestscore > alpha) alpha = bestscore;
	}

	if (const auto flips = Flip(pos, move2)) {
		const auto score = -Eval_2(pos.Play(move2, flips), -beta, -alpha, move1, move3);
		if (score >= beta) return score;
		if (score > alpha) alpha = score;
		if (score > bestscore) bestscore = score;
	}

	if (const auto flips = Flip(pos, move3))
		return std::max(bestscore, -Eval_2(pos.Play(move3, flips), -beta, -alpha, move1, move2));

	if (bestscore != -infinity)
		return bestscore;
	bestscore = infinity;

	const auto posPass = pos.PlayPass();
	node_counter++;

	if (const auto flips = Flip(posPass, move1)) {
		bestscore = Eval_2(posPass.Play(move1, flips), alpha, beta, move2, move3);
		if (bestscore <= alpha) return bestscore;
		if (bestscore < beta) beta = bestscore;
	}

	if (const auto flips = Flip(posPass, move2)) {
		const auto score = Eval_2(posPass.Play(move2, flips), alpha, beta, move1, move3);
		if (score <= alpha) return score;
		if (score < beta) beta = score;
		if (score < bestscore) bestscore = score;
	}

	if (const auto flips = Flip(posPass, move3))
		return std::min(bestscore, Eval_2(posPass.Play(move3, flips), alpha, beta, move1, move2));

	if (bestscore != infinity)
		return bestscore;

	node_counter--;
	return -EvalGameOver(posPass);
}

int AlphaBetaFailSoft::Eval_4(const CPosition& pos, int alpha, int beta, const CMove move1, const CMove move2, const CMove move3, const CMove move4)
{
	node_counter++;
	int bestscore = -infinity;

	if (const auto flips = Flip(pos, move1)) {
		bestscore = -Eval_3(pos.Play(move1, flips), -beta, -alpha, move2, move3, move4);
		if (bestscore >= beta) return bestscore;
		if (bestscore > alpha) alpha = bestscore;
	}

	if (const auto flips = Flip(pos, move2)) {
		const auto score = -Eval_3(pos.Play(move2, flips), -beta, -alpha, move1, move3, move4);
		if (score >= beta) return score;
		if (score > alpha) alpha = score;
		if (score > bestscore) bestscore = score;
	}

	if (const auto flips = Flip(pos, move3)) {
		const auto score = -Eval_3(pos.Play(move3, flips), -beta, -alpha, move1, move2, move4);
		if (score >= beta) return score;
		if (score > alpha) alpha = score;
		if (score > bestscore) bestscore = score;
	}

	if (const auto flips = Flip(pos, move4))
		return std::max(bestscore, -Eval_3(pos.Play(move4, flips), -beta, -alpha, move1, move2, move3));

	if (bestscore != -infinity)
		return bestscore;
	bestscore = infinity;

	const auto posPass = pos.PlayPass();
	node_counter++;

	if (const auto flips = Flip(posPass, move1)) {
		bestscore = Eval_3(posPass.Play(move1, flips), alpha, beta, move2, move3, move4);
		if (bestscore <= alpha) return bestscore;
		if (bestscore < beta) beta = bestscore;
	}

	if (const auto flips = Flip(posPass, move2)) {
		const auto score = Eval_3(posPass.Play(move2, flips), alpha, beta, move1, move3, move4);
		if (score <= alpha) return score;
		if (score < beta) beta = score;
		if (score < bestscore) bestscore = score;
	}

	if (const auto flips = Flip(posPass, move3)) {
		const auto score = Eval_3(posPass.Play(move3, flips), alpha, beta, move1, move2, move4);
		if (score <= alpha) return score;
		if (score < beta) beta = score;
		if (score < bestscore) bestscore = score;
	}

	if (const auto flips = Flip(posPass, move4))
		return std::min(bestscore, Eval_3(posPass.Play(move4, flips), alpha, beta, move1, move2, move3));

	if (bestscore != infinity)
		return bestscore;

	node_counter--;
	return -EvalGameOver(posPass);
}

int AlphaBetaFailSoft::Eval_N(const CPosition& pos, int alpha, int beta)
{
	node_counter++;

	CMoves moves = pos.PossibleMoves();
	if (moves.empty()) {
		const auto PosPass = pos.PlayPass();
		if (PosPass.HasMoves())
			return -Eval(PosPass, -beta, -alpha);
		
		return EvalGameOver(pos);
	}

	int bestscore = -infinity;
	while (!moves.empty())
	{
		const auto score = -Eval(pos.Play(moves.ExtractMove()), -beta, -alpha);
		if (score >= beta) return score;
		if (score > alpha) alpha = score;
		if (score > bestscore) bestscore = score;
	}

	return bestscore;
}
