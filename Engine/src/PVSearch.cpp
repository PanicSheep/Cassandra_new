#include "Search.h"
#include "LastFlipCounter.h"
#include "FlipFast.h"
#include "Stability.h"
#include <algorithm>

int PVSearch::Eval(const CPosition& pos)
{
	return Eval(pos, -64, 64);
}

int PVSearch::Eval(const CPosition& pos, int alpha, int beta)
{
	const auto EmptyCount = pos.EmptyCount();
	switch (EmptyCount)
	{
	case 0: return Eval_0(pos);
	case 1: return PVS_1(pos, alpha, beta);
		//case 2: return PVS_2(pos, alpha, beta);
	default: return PVS_N(pos, alpha, beta);
	}
}

int PVSearch::Eval_0(const CPosition& pos)
{
	NodeCounter(0)++;
	return EvalGameOver<0>(pos);
}

int PVSearch::PVS_1(const CPosition& pos, int alpha, int beta)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();

	return PVS_1(pos, alpha, beta, move1);
}

//int PVSearch::PVS_2(const CPosition& pos, int alpha, int beta)
//{
//	auto moves = CMoves(pos.Empties());
//	const auto move1 = moves.ExtractMove();
//	const auto move2 = moves.ExtractMove();
//
//	return PVS_2(pos, alpha, beta, move1, move2);
//}

int PVSearch::ZWS_1(const CPosition& pos, int alpha)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();

	return ZWS_1(pos, alpha, move1);
}

int PVSearch::ZWS_2(const CPosition& pos, int alpha)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();
	const auto move2 = moves.ExtractMove();

	return ZWS_2(pos, alpha, move1, move2);
}

int PVSearch::ZWS_3(const CPosition& pos, int alpha)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();
	const auto move2 = moves.ExtractMove();
	const auto move3 = moves.ExtractMove();

	return ZWS_3(pos, alpha, move1, move2, move3);
}

int PVSearch::ZWS_4(const CPosition& pos, int alpha)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();
	const auto move2 = moves.ExtractMove();
	const auto move3 = moves.ExtractMove();
	const auto move4 = moves.ExtractMove();

	return ZWS_4(pos, alpha, move1, move2, move3, move4);
}

int PVSearch::PVS_1(const CPosition& pos, int alpha, int beta, const CMove& move1)
{
	NodeCounter(1)++;
	const int score = static_cast<int>(2 * PopCount(pos.GetP())) - 63; // == PopCount(pos.GetP()) - PopCount(pos.GetO())

	if (const auto Diff = CountLastFlip(pos, move1))
	{
		NodeCounter(0)++;
		return score + Diff + 1;
	}
	else if (const auto Diff = CountLastFlip(pos.PlayPass(), move1))
	{
		NodeCounter(1)++;
		NodeCounter(0)++;
		return score - Diff - 1;
	}
	else
		return (score > 0) ? score + 1 : score - 1;
}

//int PVSearch::PVS_2(const CPosition& pos, int alpha, int beta, const CMove& move1, const CMove& move2)
//{
//	NodeCounter(2)++;
//	int bestscore = -128;
//
//	if (pos.GetO() & Neighbour(move1.field))
//		if (const auto flips = Flip(pos, move1)) {
//			const auto score = -PVS_1(pos.Play(move1, flips), -beta, -alpha, move2);
//			if (score >= beta) return score;
//			alpha = std::max(score, alpha);
//			bestscore = std::max(score, bestscore);
//		}
//
//	if (pos.GetO() & Neighbour(move2.field))
//		if (const auto flips = Flip(pos, move2)) {
//			const auto score = -PVS_1(pos.Play(move2, flips), -beta, -alpha, move1);
//			if (score >= beta) return score;
//			alpha = std::max(score, alpha);
//			bestscore = std::max(score, bestscore);
//		}
//
//	if (bestscore != -128)
//		return bestscore;
//	bestscore = 128;
//
//	const auto posPass = pos.PlayPass();
//
//	if (posPass.GetO() & Neighbour(move1.field))
//		if (const auto flips = Flip(posPass, move1)) {
//			const auto score = PVS_1(posPass.Play(move1, flips), alpha, beta, move2);
//			if (score <= alpha) return score;
//			beta = std::min(score, beta);
//			bestscore = std::min(score, bestscore);
//		}
//
//	if (posPass.GetO() & Neighbour(move2.field))
//		if (const auto flips = Flip(posPass, move2)) {
//			const auto score = PVS_1(posPass.Play(move2, flips), alpha, beta, move1);
//			if (score <= alpha) return score;
//			beta = std::min(score, beta);
//			bestscore = std::min(score, bestscore);
//		}
//
//	if (bestscore != 128) {
//		NodeCounter(2)++;
//		return bestscore;
//	}
//	else
//		return -EvalGameOver<2>(posPass);
//}

int PVSearch::ZWS_1(const CPosition& pos, int alpha, const CMove& move1)
{
	NodeCounter(1)++;
	const int score = static_cast<int>(2 * PopCount(pos.GetP())) - 63; // == PopCount(pos.GetP()) - PopCount(pos.GetO())

	if (const auto Diff = CountLastFlip(pos, move1))
	{
		NodeCounter(0)++;
		return score + Diff + 1;
	}
	else if (const auto Diff = CountLastFlip(pos.PlayPass(), move1))
	{
		NodeCounter(1)++;
		NodeCounter(0)++;
		return score - Diff - 1;
	}
	else
		return (score > 0) ? score + 1 : score - 1;
}

int PVSearch::ZWS_2(const CPosition& pos, const int alpha, const CMove& move1, const CMove& move2)
{
	NodeCounter(2)++;
	int bestscore = -128;

	if (pos.GetO() & Neighbour(move1.field))
		if (const auto flips = Flip(pos, move1)) {
			const auto score = -ZWS_1(pos.Play(move1, flips), -alpha - 1, move2);
			if (score > alpha) return score;
			bestscore = std::max(score, bestscore);
		}

	if (pos.GetO() & Neighbour(move2.field))
		if (const auto flips = Flip(pos, move2)) {
			return std::max(bestscore, -ZWS_1(pos.Play(move2, flips), -alpha - 1, move1));
		}

	if (bestscore != -128)
		return bestscore;
	bestscore = 128;

	const auto posPass = pos.PlayPass();

	if (posPass.GetO() & Neighbour(move1.field))
		if (const auto flips = Flip(posPass, move1)) {
			const auto score = ZWS_1(posPass.Play(move1, flips), alpha, move2);
			if (score <= alpha) return score;
			bestscore = std::min(score, bestscore);
		}

	if (posPass.GetO() & Neighbour(move2.field))
		if (const auto flips = Flip(posPass, move2)) {
			return std::min(bestscore, ZWS_1(posPass.Play(move2, flips), alpha, move1));
		}

	if (bestscore != 128) {
		NodeCounter(2)++;
		return bestscore;
	}
	else
		return -EvalGameOver<2>(posPass);
}

int PVSearch::ZWS_3(const CPosition& pos, int alpha, const CMove& move1, const CMove& move2, const CMove& move3)
{
	NodeCounter(3)++;
	int bestscore = -128;

	if (pos.GetO() & Neighbour(move1.field))
		if (const auto flips = Flip(pos, move1)) {
			const auto score = -ZWS_2(pos.Play(move1, flips), -alpha - 1, move2, move3);
			if (score > alpha) return score;
			bestscore = std::max(score, bestscore);
		}

	if (pos.GetO() & Neighbour(move2.field))
		if (const auto flips = Flip(pos, move2)) {
			const auto score = -ZWS_2(pos.Play(move2, flips), -alpha - 1, move1, move3);
			if (score > alpha) return score;
			bestscore = std::max(score, bestscore);
		}

	if (pos.GetO() & Neighbour(move3.field))
		if (const auto flips = Flip(pos, move3)) {
			return std::max(bestscore, -ZWS_2(pos.Play(move3, flips), -alpha - 1, move1, move2));
		}

	if (bestscore != -128)
		return bestscore;
	bestscore = 128;

	const auto posPass = pos.PlayPass();

	if (posPass.GetO() & Neighbour(move1.field))
		if (const auto flips = Flip(posPass, move1)) {
			const auto score = ZWS_2(posPass.Play(move1, flips), alpha, move2, move3);
			if (score <= alpha) return score;
			bestscore = std::min(score, bestscore);
		}

	if (posPass.GetO() & Neighbour(move2.field))
		if (const auto flips = Flip(posPass, move2)) {
			const auto score = ZWS_2(posPass.Play(move2, flips), alpha, move1, move3);
			if (score <= alpha) return score;
			bestscore = std::min(score, bestscore);
		}

	if (posPass.GetO() & Neighbour(move3.field))
		if (const auto flips = Flip(posPass, move3)) {
			return std::min(bestscore, ZWS_2(posPass.Play(move3, flips), alpha, move1, move2));
		}

	if (bestscore != 128) {
		NodeCounter(3)++;
		return bestscore;
	}
	else
		return -EvalGameOver<3>(posPass);
}

int PVSearch::ZWS_4(const CPosition& pos, int alpha, const CMove& move1, const CMove& move2, const CMove& move3, const CMove& move4)
{
	NodeCounter(4)++;
	int bestscore = -128;

	if (pos.GetO() & Neighbour(move1.field))
		if (const auto flips = Flip(pos, move1)) {
			const auto score = -ZWS_3(pos.Play(move1, flips), -alpha - 1, move2, move3, move4);
			if (score > alpha) return score;
			bestscore = std::max(score, bestscore);
		}

	if (pos.GetO() & Neighbour(move2.field))
		if (const auto flips = Flip(pos, move2)) {
			const auto score = -ZWS_3(pos.Play(move2, flips), -alpha - 1, move1, move3, move4);
			if (score > alpha) return score;
			bestscore = std::max(score, bestscore);
		}

	if (pos.GetO() & Neighbour(move3.field))
		if (const auto flips = Flip(pos, move3)) {
			const auto score = -ZWS_3(pos.Play(move3, flips), -alpha - 1, move1, move2, move4);
			if (score > alpha) return score;
			bestscore = std::max(score, bestscore);
		}

	if (pos.GetO() & Neighbour(move4.field))
		if (const auto flips = Flip(pos, move4)) {
			return std::max(bestscore, -ZWS_3(pos.Play(move4, flips), -alpha - 1, move1, move2, move3));
		}

	if (bestscore != -128)
		return bestscore;
	bestscore = 128;

	const auto posPass = pos.PlayPass();

	if (posPass.GetO() & Neighbour(move1.field))
		if (const auto flips = Flip(posPass, move1)) {
			const auto score = ZWS_3(posPass.Play(move1, flips), alpha, move2, move3, move4);
			if (score <= alpha) return score;
			bestscore = std::min(score, bestscore);
		}

	if (posPass.GetO() & Neighbour(move2.field))
		if (const auto flips = Flip(posPass, move2)) {
			const auto score = ZWS_3(posPass.Play(move2, flips), alpha, move1, move3, move4);
			if (score <= alpha) return score;
			bestscore = std::min(score, bestscore);
		}

	if (posPass.GetO() & Neighbour(move3.field))
		if (const auto flips = Flip(posPass, move3)) {
			const auto score = ZWS_3(posPass.Play(move3, flips), alpha, move1, move2, move4);
			if (score <= alpha) return score;
			bestscore = std::min(score, bestscore);
		}

	if (posPass.GetO() & Neighbour(move4.field))
		if (const auto flips = Flip(posPass, move4)) {
			return std::min(bestscore, ZWS_3(posPass.Play(move4, flips), alpha, move1, move2, move3));
		}

	if (bestscore != 128) {
		NodeCounter(4)++;
		return bestscore;
	}
	else
		return -EvalGameOver<4>(posPass);
}

int PVSearch::ZWS_N(const CPosition& pos, const int alpha)
{
	const auto EmptyCount = pos.EmptyCount();
	if (EmptyCount == 4)
		return ZWS_4(pos, alpha);

	NodeCounter(EmptyCount)++;

	CSortedMoves moves(pos);
	if (moves.empty()) {
		const auto PosPass = pos.PlayPass();
		if (PosPass.HasMoves())
			return -ZWS_N(PosPass, -alpha - 1);
		else
			return EvalGameOver(pos, EmptyCount);
	}

	const NodeInfo node(pos, alpha, alpha + 1, 0, EmptyCount, 0);
	const auto LocalNodeCount = GetNodeCount();
	if (const auto ret = StabilityCutoff(node); ret.first) return ret.second;
	if (const auto ret = TranspositionTableCutoff(node); ret.first) return ret.second;

	int bestscore = -128;
	for (const auto& pair : moves)
	{
		const auto& move = pair.second;
		const auto score = -ZWS_N(pos.Play(move), -alpha - 1);
		if (score > alpha) {
			UpdateTranspositionTable(GetNodeCount() - LocalNodeCount, NodeInfo(pos, score, +64, EmptyCount, EmptyCount, 0));
			return score;
		}
		bestscore = std::max(score, bestscore);
	}

	UpdateTranspositionTable(GetNodeCount() - LocalNodeCount, NodeInfo(pos, -64, bestscore, EmptyCount, EmptyCount, 0));
	return bestscore;
}

int PVSearch::PVS_N(const CPosition& pos, const int alpha, const int beta)
{
	const auto EmptyCount = pos.EmptyCount();
	if (EmptyCount == 1)
		return PVS_1(pos, alpha, beta);

	NodeCounter(EmptyCount)++;

	CSortedMoves moves(pos);
	if (moves.empty()) {
		const auto PosPass = pos.PlayPass();
		if (PosPass.HasMoves())
			return -PVS_N(PosPass, -beta, -alpha);
		else
			return EvalGameOver(pos, EmptyCount);
	}

	NodeInfo node(pos, alpha, beta, 0, EmptyCount, 0);
	const auto LocalNodeCount = GetNodeCount();
	if (const auto ret = StabilityCutoff(node); ret.first) return ret.second;

	// First Move
	int bestscore = -PVS_N(pos.Play(moves.ExtractMove()), -beta, -alpha);
	if (bestscore >= beta) return bestscore;
	int lower = std::max(bestscore, alpha);

	for (const auto& pair : moves)
	{
		const auto& move = pair.second;
		const auto nextPos = pos.Play(move);
		auto score = -ZWS_N(nextPos, -lower - 1);
		if (score >= beta) {
			UpdateTranspositionTable(GetNodeCount() - LocalNodeCount, NodeInfo(pos, score, +64, EmptyCount, EmptyCount, 0));
			return score;
		}
		if (score > lower) // if ZWS failed high.
			score = -PVS_N(nextPos, -beta, -score);

		lower = std::max(score, lower);
		bestscore = std::max(score, bestscore);
	}
	if (bestscore <= alpha)
		UpdateTranspositionTable(GetNodeCount() - LocalNodeCount, NodeInfo(pos, -64, bestscore, EmptyCount, EmptyCount, 0));
	else
		UpdateTranspositionTable(GetNodeCount() - LocalNodeCount, NodeInfo(pos, bestscore, bestscore, EmptyCount, EmptyCount, 0));
	return bestscore;
}

std::pair<bool, int> PVSearch::StabilityCutoff(const NodeInfo& node)
{
	const auto score = static_cast<int>(64 - 2 * PopCount(GetStableStones(node.pos)));
	return std::pair<bool, int>(score <= node.alpha, score);
}

std::pair<bool, int> PVSearch::TranspositionTableCutoff(const NodeInfo& node)
{
	const auto ret = environment->HashTable->LookUp(node.pos);
	const auto& ttValue = ret.second;
	if ((ttValue.depth >= node.depth) && (ttValue.selectivity <= node.selectivity))
	{
		if (ttValue.alpha >= node.beta) return std::pair<bool, int>(true, ttValue.alpha);
		if (ttValue.beta <= node.alpha) return std::pair<bool, int>(true, ttValue.beta);
		if (ttValue.alpha == ttValue.beta) return std::pair<bool, int>(true, ttValue.alpha);
	}
	return std::pair<bool, int>(false, 0);
}

void PVSearch::UpdateTranspositionTable(uint64_t NodeCount, const NodeInfo& node)
{
	environment->HashTable->Update(node.pos, PvsInfo(NodeCount, node.depth, node.selectivity, node.alpha, node.beta, Field::invalid, Field::invalid));
}