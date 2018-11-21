#include "AlphaBetaFailHardSearch.h"

#include <algorithm>

#include "FlipFast.h"

using namespace Search;

std::unique_ptr<CAlgorithm> AlphaBetaFailHard::Clone() const
{
	return std::make_unique<AlphaBetaFailHard>(*this);
}

CResult AlphaBetaFailHard::Eval(const CPosition& pos, CSpecification spec)
{
	const auto old_node_counter = node_counter;
	const auto start_time = std::chrono::high_resolution_clock::now();

	const int score = Eval(pos, spec.alpha, spec.beta);

	const auto end_time = std::chrono::high_resolution_clock::now();
	const auto duration = end_time - start_time;
	const auto node_count = node_counter - old_node_counter;
	return CResult(score, node_count, duration);
}

int AlphaBetaFailHard::Eval(const CPosition& pos, int alpha, int beta)
{
	switch (pos.EmptyCount())
	{
		case 0: return Eval_0(pos, alpha, beta);
		case 1: return Eval_1(pos, alpha, beta);
		case 2: return Eval_2(pos, alpha, beta);
		case 3: return Eval_3(pos, alpha, beta);
		case 4: return Eval_4(pos, alpha, beta);
		default: return Eval_N(pos, alpha, beta);
	}
}

int AlphaBetaFailHard::Eval_1(const CPosition& pos, int alpha, int beta)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();

	return Eval_1(pos, alpha, beta, move1);
}

int AlphaBetaFailHard::Eval_2(const CPosition& pos, int alpha, int beta)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();
	const auto move2 = moves.ExtractMove();

	return Eval_2(pos, alpha, beta, move1, move2);
}

int AlphaBetaFailHard::Eval_3(const CPosition& pos, int alpha, int beta)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();
	const auto move2 = moves.ExtractMove();
	const auto move3 = moves.ExtractMove();

	return Eval_3(pos, alpha, beta, move1, move2, move3);
}

int AlphaBetaFailHard::Eval_4(const CPosition& pos, int alpha, int beta)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();
	const auto move2 = moves.ExtractMove();
	const auto move3 = moves.ExtractMove();
	const auto move4 = moves.ExtractMove();

	return Eval_4(pos, alpha, beta, move1, move2, move3, move4);
}

int AlphaBetaFailHard::Eval_0(const CPosition& pos, int alpha, int beta)
{
	node_counter++;
	return std::clamp(EvalGameOver(pos), alpha, beta);
}

int AlphaBetaFailHard::Eval_1(const CPosition& pos, const int alpha, const int beta, const CMove move1)
{
	node_counter++;
	const int score = static_cast<int>(2 * PopCount(pos.GetP())) - 63; // == PopCount(pos.GetP()) - PopCount(pos.GetO())

	if (const auto Diff = engine->CountLastFlip(pos, move1))
	{
		node_counter++;
		return std::clamp(score + Diff + 1, alpha, beta);
	}
	else if (const auto Diff = engine->CountLastFlip(pos.PlayPass(), move1))
	{
		node_counter += 2;
		return std::clamp(score - Diff - 1, alpha, beta);
	}
	else
		return std::clamp((score > 0) ? score + 1 : score - 1, alpha, beta);
}

int AlphaBetaFailHard::Eval_2(const CPosition& pos, int alpha, int beta, const CMove move1, const CMove move2)
{
	node_counter++;
	int score = -128;

	if (const auto flips = Flip(pos, move1)) {
		score = -Eval_1(pos.Play(move1, flips), -beta, -alpha, move2);
		if (score >= beta) return beta;
		if (score > alpha) alpha = score;
	}

	if (const auto flips = Flip(pos, move2)) {
		score = -Eval_1(pos.Play(move2, flips), -beta, -alpha, move1);
		if (score >= beta) return beta;
		if (score > alpha) alpha = score;
	}

	if (score != -128)
		return alpha;

	const auto posPass = pos.PlayPass();

	if (const auto flips = Flip(posPass, move1)) {
		score = Eval_1(posPass.Play(move1, flips), alpha, beta, move2);
		if (score <= alpha) return alpha;
		if (score < beta) beta = score;
	}

	if (const auto flips = Flip(posPass, move2)) {
		score = Eval_1(posPass.Play(move2, flips), alpha, beta, move1);
		if (score <= alpha) return alpha;
		if (score < beta) beta = score;
	}

	if (score != -128) {
		node_counter++;
		return beta;
	}
	else
		return std::clamp(-EvalGameOver(posPass), alpha, beta);
}

int AlphaBetaFailHard::Eval_3(const CPosition& pos, int alpha, int beta, const CMove move1, const CMove move2, const CMove move3)
{
	node_counter++;
	int score = -128;

	if (const auto flips = Flip(pos, move1)) {
		score = -Eval_2(pos.Play(move1, flips), -beta, -alpha, move2, move3);
		if (score >= beta) return beta;
		if (score > alpha) alpha = score;
	}

	if (const auto flips = Flip(pos, move2)) {
		score = -Eval_2(pos.Play(move2, flips), -beta, -alpha, move1, move3);
		if (score >= beta) return beta;
		if (score > alpha) alpha = score;
	}

	if (const auto flips = Flip(pos, move3)) {
		score = -Eval_2(pos.Play(move3, flips), -beta, -alpha, move1, move2);
		if (score >= beta) return beta;
		if (score > alpha) alpha = score;
	}

	if (score != -128)
		return alpha;

	const auto posPass = pos.PlayPass();

	if (const auto flips = Flip(posPass, move1)) {
		score = Eval_2(posPass.Play(move1, flips), alpha, beta, move2, move3);
		if (score <= alpha) return alpha;
		if (score < beta) beta = score;
	}

	if (const auto flips = Flip(posPass, move2)) {
		score = Eval_2(posPass.Play(move2, flips), alpha, beta, move1, move3);
		if (score <= alpha) return alpha;
		if (score < beta) beta = score;
	}

	if (const auto flips = Flip(posPass, move3)) {
		score = Eval_2(posPass.Play(move3, flips), alpha, beta, move1, move2);
		if (score <= alpha) return alpha;
		if (score < beta) beta = score;
	}

	if (score != -128) {
		node_counter++;
		return beta;
	}
	else
		return std::clamp(-EvalGameOver(posPass), alpha, beta);
}

int AlphaBetaFailHard::Eval_4(const CPosition& pos, int alpha, int beta, const CMove move1, const CMove move2, const CMove move3, const CMove move4)
{
	node_counter++;
	int score = -128;

	if (const auto flips = Flip(pos, move1)) {
		score = -Eval_3(pos.Play(move1, flips), -beta, -alpha, move2, move3, move4);
		if (score >= beta) return beta;
		if (score > alpha) alpha = score;
	}

	if (const auto flips = Flip(pos, move2)) {
		score = -Eval_3(pos.Play(move2, flips), -beta, -alpha, move1, move3, move4);
		if (score >= beta) return beta;
		if (score > alpha) alpha = score;
	}

	if (const auto flips = Flip(pos, move3)) {
		score = -Eval_3(pos.Play(move3, flips), -beta, -alpha, move1, move2, move4);
		if (score >= beta) return beta;
		if (score > alpha) alpha = score;
	}

	if (const auto flips = Flip(pos, move4)) {
		score = -Eval_3(pos.Play(move4, flips), -beta, -alpha, move1, move2, move3);
		if (score >= beta) return beta;
		if (score > alpha) alpha = score;
	}

	if (score != -128)
		return alpha;

	const auto posPass = pos.PlayPass();

	if (const auto flips = Flip(posPass, move1)) {
		score = Eval_3(posPass.Play(move1, flips), alpha, beta, move2, move3, move4);
		if (score <= alpha) return alpha;
		if (score < beta) beta = score;
	}

	if (const auto flips = Flip(posPass, move2)) {
		score = Eval_3(posPass.Play(move2, flips), alpha, beta, move1, move3, move4);
		if (score <= alpha) return alpha;
		if (score < beta) beta = score;
	}

	if (const auto flips = Flip(posPass, move3)) {
		score = Eval_3(posPass.Play(move3, flips), alpha, beta, move1, move2, move4);
		if (score <= alpha) return alpha;
		if (score < beta) beta = score;
	}

	if (const auto flips = Flip(posPass, move4)) {
		score = Eval_3(posPass.Play(move4, flips), alpha, beta, move1, move2, move3);
		if (score <= alpha) return alpha;
		if (score < beta) beta = score;
	}

	if (score != -128) {
		node_counter++;
		return beta;
	}
	else
		return std::clamp(-EvalGameOver(posPass), alpha, beta);
}

int AlphaBetaFailHard::Eval_N(const CPosition& pos, int alpha, int beta)
{
	const auto EmptyCount = pos.EmptyCount();
	if (EmptyCount == 4)
		return Eval_4(pos, alpha, beta);

	node_counter++;

	CMoves moves = pos.PossibleMoves();
	if (moves.empty()) {
		const auto PosPass = pos.PlayPass();
		if (PosPass.HasMoves())
			return -Eval_N(PosPass, -beta, -alpha);
		else
			return std::clamp(EvalGameOver(pos), alpha, beta);
	}

	int score = -128;
	while (!moves.empty())
	{
		score = -Eval_N(pos.Play(moves.ExtractMove()), -beta, -alpha);
		if (score >= beta) return beta;
		if (score > alpha) alpha = score;
	}

	return alpha;
}
