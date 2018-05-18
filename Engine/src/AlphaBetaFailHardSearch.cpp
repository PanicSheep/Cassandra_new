#include "AlphaBetaFailHardSearch.h"

#include <algorithm>

#include "LastFlipCounter.h"
#include "FlipFast.h"

int AlphaBetaFailHardSearch::Eval(const CPosition& pos)
{
	return Eval(pos, -64, 64);
}

int AlphaBetaFailHardSearch::Eval(const CPosition& pos, int alpha, int beta)
{
	const auto EmptyCount = pos.EmptyCount();
	switch (EmptyCount)
	{
		case 0: return Eval_0(pos, alpha, beta);
		case 1: return Eval_1(pos, alpha, beta);
		case 2: return Eval_2(pos, alpha, beta);
		case 3: return Eval_3(pos, alpha, beta);
		case 4: return Eval_4(pos, alpha, beta);
		default: return Eval_N(pos, alpha, beta);
	}
}

int AlphaBetaFailHardSearch::Eval_1(const CPosition& pos, int alpha, int beta)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();

	return Eval_1(pos, alpha, beta, move1);
}

int AlphaBetaFailHardSearch::Eval_2(const CPosition& pos, int alpha, int beta)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();
	const auto move2 = moves.ExtractMove();

	return Eval_2(pos, alpha, beta, move1, move2);
}

int AlphaBetaFailHardSearch::Eval_3(const CPosition& pos, int alpha, int beta)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();
	const auto move2 = moves.ExtractMove();
	const auto move3 = moves.ExtractMove();

	return Eval_3(pos, alpha, beta, move1, move2, move3);
}

int AlphaBetaFailHardSearch::Eval_4(const CPosition& pos, int alpha, int beta)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();
	const auto move2 = moves.ExtractMove();
	const auto move3 = moves.ExtractMove();
	const auto move4 = moves.ExtractMove();

	return Eval_4(pos, alpha, beta, move1, move2, move3, move4);
}

int AlphaBetaFailHardSearch::Eval_0(const CPosition& pos, int alpha, int beta)
{
	NodeCounter(0)++;
	return std::clamp(EvalGameOver<0>(pos), alpha, beta);
}

int AlphaBetaFailHardSearch::Eval_1(const CPosition& pos, const int alpha, const int beta, const CMove& move1)
{
	NodeCounter(1)++;
	const int score = static_cast<int>(2 * PopCount(pos.GetP())) - 63; // == PopCount(pos.GetP()) - PopCount(pos.GetO())

	if (const auto Diff = CountLastFlip(pos, move1))
	{
		NodeCounter(0)++;
		return std::clamp(score + Diff + 1, alpha, beta);
	}
	else if (const auto Diff = CountLastFlip(pos.PlayPass(), move1))
	{
		NodeCounter(1)++;
		NodeCounter(0)++;
		return std::clamp(score - Diff - 1, alpha, beta);
	}
	else
		return std::clamp((score > 0) ? score + 1 : score - 1, alpha, beta);
}

int AlphaBetaFailHardSearch::Eval_2(const CPosition& pos, int alpha, int beta, const CMove& move1, const CMove& move2)
{
	NodeCounter(2)++;
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
		NodeCounter(2)++;
		return beta;
	}
	else
		return std::clamp(-EvalGameOver<2>(posPass), alpha, beta);
}

int AlphaBetaFailHardSearch::Eval_3(const CPosition& pos, int alpha, int beta, const CMove& move1, const CMove& move2, const CMove& move3)
{
	NodeCounter(3)++;
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
		NodeCounter(3)++;
		return beta;
	}
	else
		return std::clamp(-EvalGameOver<3>(posPass), alpha, beta);
}

int AlphaBetaFailHardSearch::Eval_4(const CPosition& pos, int alpha, int beta, const CMove& move1, const CMove& move2, const CMove& move3, const CMove& move4)
{
	NodeCounter(4)++;
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
		NodeCounter(4)++;
		return beta;
	}
	else
		return std::clamp(-EvalGameOver<4>(posPass), alpha, beta);
}

int AlphaBetaFailHardSearch::Eval_N(const CPosition& pos, int alpha, int beta)
{
	const auto EmptyCount = pos.EmptyCount();
	if (EmptyCount == 4)
		return Eval_4(pos, alpha, beta);

	NodeCounter(EmptyCount)++;

	CMoves moves = pos.PossibleMoves();
	if (moves.empty()) {
		const auto PosPass = pos.PlayPass();
		if (PosPass.HasMoves())
			return -Eval(PosPass, -beta, -alpha);
		else
			return std::clamp(EvalGameOver(pos, EmptyCount), alpha, beta);
	}

	int score = -128;
	while (!moves.empty())
	{
		score = -Eval(pos.Play(moves.ExtractMove()), -beta, -alpha);
		if (score >= beta) return beta;
		if (score > alpha) alpha = score;
	}

	return alpha;
}
