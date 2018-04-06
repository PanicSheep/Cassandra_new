#include "Search.h"
#include "LastFlipCounter.h"
#include "FlipFast.h"
#include <algorithm>

int AlphaBetaFailSoftSearch::Eval(const CPosition& pos)
{
	return Eval(pos, -64, 64);
}

int AlphaBetaFailSoftSearch::Eval(const CPosition& pos, int alpha, int beta)
{
	const auto EmptyCount = pos.EmptyCount();
	switch (EmptyCount)
	{
		case 0: return Eval_0(pos);
		case 1: return Eval_1(pos, alpha);
		case 2: return Eval_2(pos, alpha, beta);
		case 3: return Eval_3(pos, alpha, beta);
		case 4: return Eval_4(pos, alpha, beta);
		default: return Eval_N(pos, alpha, beta);
	}
}

int AlphaBetaFailSoftSearch::Eval_1(const CPosition& pos, int alpha)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();

	return Eval_1(pos, alpha, move1);
}

int AlphaBetaFailSoftSearch::Eval_2(const CPosition& pos, int alpha, int beta)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();
	const auto move2 = moves.ExtractMove();

	return Eval_2(pos, alpha, beta, move1, move2);
}

int AlphaBetaFailSoftSearch::Eval_3(const CPosition& pos, int alpha, int beta)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();
	const auto move2 = moves.ExtractMove();
	const auto move3 = moves.ExtractMove();

	return Eval_3(pos, alpha, beta, move1, move2, move3);
}

int AlphaBetaFailSoftSearch::Eval_4(const CPosition& pos, int alpha, int beta)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();
	const auto move2 = moves.ExtractMove();
	const auto move3 = moves.ExtractMove();
	const auto move4 = moves.ExtractMove();

	return Eval_4(pos, alpha, beta, move1, move2, move3, move4);
}

int AlphaBetaFailSoftSearch::Eval_0(const CPosition& pos)
{
	NodeCounter(0)++;
	return EvalGameOver<0>(pos);
}

int AlphaBetaFailSoftSearch::Eval_1(const CPosition& pos, int alpha, const CMove& move1)
{
	NodeCounter(1)++;
	const int score = static_cast<int>(2 * PopCount(pos.GetP())) - 63; // == PopCount(pos.GetP()) - PopCount(pos.GetO())

	if (const auto Diff = CountLastFlip(pos, move1))
	{
		NodeCounter(0)++;
		return score + Diff + 1;
	}
	else
	{
		//if (score + 1 <= alpha)
		//	return score + 1;
		if (const auto Diff = CountLastFlip(pos.PlayPass(), move1))
		{
			NodeCounter(1)++;
			NodeCounter(0)++;
			return score - Diff - 1;
		}
		else
			return (score > 0) ? score + 1 : score - 1;
	}
}

int AlphaBetaFailSoftSearch::Eval_2(const CPosition& pos, int alpha, int beta, const CMove& move1, const CMove& move2)
{
	NodeCounter(2)++;
	int bestscore = -128;

	if (const auto flips = Flip(pos, move1)) {
		const auto score = -Eval_1(pos.Play(move1, flips), -beta, move2);
		if (score >= beta) return score;
		alpha = std::max(score, alpha);
		bestscore = std::max(score, bestscore);
	}

	if (const auto flips = Flip(pos, move2)) {
		const auto score = -Eval_1(pos.Play(move2, flips), -beta, move1);
		if (score >= beta) return score;
		alpha = std::max(score, alpha);
		bestscore = std::max(score, bestscore);
	}

	if (bestscore != -128)
		return bestscore;
	bestscore = 128;

	const auto posPass = pos.PlayPass();

	if (const auto flips = Flip(posPass, move1)) {
		const auto score = Eval_1(posPass.Play(move1, flips), alpha, move2);
		if (score <= alpha) return score;
		beta = std::min(score, beta);
		bestscore = std::min(score, bestscore);
	}

	if (const auto flips = Flip(posPass, move2)) {
		const auto score = Eval_1(posPass.Play(move2, flips), alpha, move1);
		if (score <= alpha) return score;
		beta = std::min(score, beta);
		bestscore = std::min(score, bestscore);
	}

	if (bestscore != 128) {
		NodeCounter(2)++;
		return bestscore;
	}
	else
		return -EvalGameOver<2>(posPass);
}

int AlphaBetaFailSoftSearch::Eval_3(const CPosition& pos, int alpha, int beta, const CMove& move1, const CMove& move2, const CMove& move3)
{
	NodeCounter(3)++;
	int bestscore = -128;

	if (const auto flips = Flip(pos, move1)) {
		const auto score = -Eval_2(pos.Play(move1, flips), -beta, -alpha, move2, move3);
		if (score >= beta) return score;
		alpha = std::max(score, alpha);
		bestscore = std::max(score, bestscore);
	}

	if (const auto flips = Flip(pos, move2)) {
		const auto score = -Eval_2(pos.Play(move2, flips), -beta, -alpha, move1, move3);
		if (score >= beta) return score;
		alpha = std::max(score, alpha);
		bestscore = std::max(score, bestscore);
	}

	if (const auto flips = Flip(pos, move3)) {
		const auto score = -Eval_2(pos.Play(move3, flips), -beta, -alpha, move1, move2);
		if (score >= beta) return score;
		alpha = std::max(score, alpha);
		bestscore = std::max(score, bestscore);
	}

	if (bestscore != -128)
		return bestscore;
	bestscore = 128;

	const auto posPass = pos.PlayPass();

	if (const auto flips = Flip(posPass, move1)) {
		const auto score = Eval_2(posPass.Play(move1, flips), alpha, beta, move2, move3);
		if (score <= alpha) return score;
		if (score <= alpha) return score;
		beta = std::min(score, beta);
		bestscore = std::min(score, bestscore);
	}

	if (const auto flips = Flip(posPass, move2)) {
		const auto score = Eval_2(posPass.Play(move2, flips), alpha, beta, move1, move3);
		if (score <= alpha) return score;
		beta = std::min(score, beta);
		bestscore = std::min(score, bestscore);
	}

	if (const auto flips = Flip(posPass, move3)) {
		const auto score = Eval_2(posPass.Play(move3, flips), alpha, beta, move1, move2);
		if (score <= alpha) return score;
		beta = std::min(score, beta);
		bestscore = std::min(score, bestscore);
	}

	if (bestscore != 128) {
		NodeCounter(3)++;
		return bestscore;
	}
	else
		return -EvalGameOver<3>(posPass);
}

int AlphaBetaFailSoftSearch::Eval_4(const CPosition& pos, int alpha, int beta, const CMove& move1, const CMove& move2, const CMove& move3, const CMove& move4)
{
	NodeCounter(4)++;
	int bestscore = -128;

	if (const auto flips = Flip(pos, move1)) {
		const auto score = -Eval_3(pos.Play(move1, flips), -beta, -alpha, move2, move3, move4);
		if (score >= beta) return score;
		alpha = std::max(score, alpha);
		bestscore = std::max(score, bestscore);
	}

	if (const auto flips = Flip(pos, move2)) {
		const auto score = -Eval_3(pos.Play(move2, flips), -beta, -alpha, move1, move3, move4);
		if (score >= beta) return score;
		alpha = std::max(score, alpha);
		bestscore = std::max(score, bestscore);
	}

	if (const auto flips = Flip(pos, move3)) {
		const auto score = -Eval_3(pos.Play(move3, flips), -beta, -alpha, move1, move2, move4);
		if (score >= beta) return score;
		alpha = std::max(score, alpha);
		bestscore = std::max(score, bestscore);
	}

	if (const auto flips = Flip(pos, move4)) {
		const auto score = -Eval_3(pos.Play(move4, flips), -beta, -alpha, move1, move2, move3);
		if (score >= beta) return score;
		alpha = std::max(score, alpha);
		bestscore = std::max(score, bestscore);
	}

	if (bestscore != -128)
		return bestscore;
	bestscore = 128;

	const auto posPass = pos.PlayPass();

	if (const auto flips = Flip(posPass, move1)) {
		const auto score = Eval_3(posPass.Play(move1, flips), alpha, beta, move2, move3, move4);
		if (score <= alpha) return score;
		beta = std::min(score, beta);
		bestscore = std::min(score, bestscore);
	}

	if (const auto flips = Flip(posPass, move2)) {
		const auto score = Eval_3(posPass.Play(move2, flips), alpha, beta, move1, move3, move4);
		if (score <= alpha) return score;
		beta = std::min(score, beta);
		bestscore = std::min(score, bestscore);
	}

	if (const auto flips = Flip(posPass, move3)) {
		const auto score = Eval_3(posPass.Play(move3, flips), alpha, beta, move1, move2, move4);
		if (score <= alpha) return score;
		beta = std::min(score, beta);
		bestscore = std::min(score, bestscore);
	}

	if (const auto flips = Flip(posPass, move4)) {
		const auto score = Eval_3(posPass.Play(move4, flips), alpha, beta, move1, move2, move3);
		if (score <= alpha) return score;
		beta = std::min(score, beta);
		bestscore = std::min(score, bestscore);
	}

	if (bestscore != 128) {
		NodeCounter(4)++;
		return bestscore;
	}
	else
		return -EvalGameOver<4>(posPass);
}

int AlphaBetaFailSoftSearch::Eval_N(const CPosition& pos, int alpha, int beta)
{
	const auto EmptyCount = pos.EmptyCount();
	if (EmptyCount == 4)
		return Eval_4(pos, alpha, beta);

	NodeCounter(EmptyCount)++;

	CMoves moves = pos.PossibleMoves();
	if (moves.empty()) {
		const auto PosPass = pos.PlayPass();
		if (PosPass.HasMoves())
			return -Eval_N(PosPass, -beta, -alpha);
		else
			return EvalGameOver(pos, EmptyCount);
	}

	int bestscore = -128;
	while (!moves.empty())
	{
		const auto score = -Eval_N(pos.Play(moves.ExtractMove()), -beta, -alpha);
		if (score >= beta) return score;
		alpha = std::max(score, alpha);
		bestscore = std::max(score, bestscore);
	}

	return bestscore;
}