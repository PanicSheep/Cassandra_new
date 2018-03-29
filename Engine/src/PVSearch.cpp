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
	return PVS(InputValues(pos, alpha, beta, pos.EmptyCount(), 0)).GetScore();
}

PVSearch::ReturnValues PVSearch::PVS(const InputValues& in)
{
	const auto EmptyCount = in.pos.EmptyCount();
	switch (EmptyCount)
	{
	case 0: return ReturnValues(Eval_0(in.pos), 0, 0);
	case 1: return ReturnValues(PVS_1(in.pos, in.alpha, in.beta), 1, 0);
	default: return PVS_N(in);
	}
}

PVSearch::ReturnValues PVSearch::ZWS(const InputValues& in)
{
	const auto EmptyCount = in.pos.EmptyCount();
	switch (EmptyCount)
	{
	case 0: return ReturnValues(Eval_0(in.pos), 0, 0);
	case 1: return ReturnValues(ZWS_1(in.pos, in.alpha), 1, 0);
	case 2: return ReturnValues(ZWS_2(in.pos, in.alpha), 2, 0);
	case 3: return ReturnValues(ZWS_3(in.pos, in.alpha), 3, 0);
	case 4: return ReturnValues(ZWS_4(in.pos, in.alpha), 4, 0);
	case 5:
	case 6:
	case 7:
		return ZWS_A(in);
	default: return ZWS_N(in);
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

PVSearch::ReturnValues PVSearch::ZWS_A(const InputValues& in)
{
	assert(in.beta == in.alpha + 1);

	const auto EmptyCount = in.pos.EmptyCount();
	NodeCounter(EmptyCount)++;

	CMoves moves = in.pos.PossibleMoves();
	if (moves.empty()) {
		const auto Pass = in.PlayPass();
		if (Pass.pos.HasMoves())
			return -ZWS_A(Pass);
		else
		{
			const auto score = EvalGameOver(in.pos, EmptyCount);
			return ReturnValues(score, score, EmptyCount, 0);
		}
	}

	StatusValues stat(in, GetNodeCount());
	if (in.alpha > 42 && ((in.pos.Empties() & 0x8100000000000081ULL) != 0x8100000000000081ULL))
		if (const auto ret = stat.ImproveWith(StabilityAnalysis(in)); ret.CausesCut) return ret.Values;
	//if (const auto ret = stat.ImproveWith(TranspositionTableAnalysis(in)); ret.CausesCut) return ret.Values;

	int bestscore = -128;
	CMoves parity_moves(moves, in.pos.GetParityQuadrants());
	while (!parity_moves.empty())
	{
		const auto move = parity_moves.ExtractMove();
		const auto ret = stat.ImproveWith(-ZWS(stat.Play(move)), move);
		if (ret.CausesCut) {
			//UpdateTranspositionTable(stat);
			return ret.Values;
		}
	}
	CMoves non_parity_moves(moves, ~in.pos.GetParityQuadrants());
	while (!non_parity_moves.empty())
	{
		const auto move = non_parity_moves.ExtractMove();
		const auto ret = stat.ImproveWith(-ZWS(stat.Play(move)), move);
		if (ret.CausesCut) {
			//UpdateTranspositionTable(stat);
			return ret.Values;
		}
	}

	const auto ret = stat.AllMovesSearched();
	//UpdateTranspositionTable(stat);
	return ret;
}

PVSearch::ReturnValues PVSearch::ZWS_N(const InputValues& in)
{
	assert(in.beta == in.alpha + 1);

	const auto EmptyCount = in.pos.EmptyCount();
	NodeCounter(EmptyCount)++;

	if (in.pos.HasMoves() == false) {
		const auto Pass = in.PlayPass();
		if (Pass.pos.HasMoves())
			return -ZWS_N(Pass);
		else
		{
			const auto score = EvalGameOver(in.pos, EmptyCount);
			return ReturnValues(score, score, EmptyCount, 0);
		}
	}

	StatusValues stat(in, GetNodeCount());
	if (in.alpha > 42 && ((in.pos.Empties() & 0x8100000000000081ULL) != 0x8100000000000081ULL))
		if (const auto ret = stat.ImproveWith(StabilityAnalysis(in)); ret.CausesCut) return ret.Values;
	if (const auto ret = stat.ImproveWith(TranspositionTableAnalysis(in)); ret.CausesCut) return ret.Values;

	int bestscore = -128;
	CSortedMoves moves(in.pos);
	for (const auto& pair : moves)
	{
		const auto& move = pair.second;
		const auto ret = stat.ImproveWith(-ZWS(stat.Play(move)), move);
		if (ret.CausesCut) {
			UpdateTranspositionTable(stat);
			return ret.Values;
		}
	}

	const auto ret = stat.AllMovesSearched();
	UpdateTranspositionTable(stat);
	return ret;
}

PVSearch::ReturnValues PVSearch::PVS_N(const InputValues& in)
{
	const auto EmptyCount = in.pos.EmptyCount();
	NodeCounter(EmptyCount)++;

	if (in.pos.HasMoves() == false) {
		const auto Pass = in.PlayPass();
		if (Pass.pos.HasMoves())
			return -PVS_N(Pass);
		else
		{
			const auto score = EvalGameOver(in.pos, EmptyCount);
			return ReturnValues(score, score, EmptyCount, 0);
		}
	}

	StatusValues stat(in, GetNodeCount());
	if (in.alpha > 42 && ((in.pos.Empties() & 0x8100000000000081ULL) != 0x8100000000000081ULL))
		if (const auto ret = stat.ImproveWith(StabilityAnalysis(in)); ret.CausesCut) return ret.Values;

	CSortedMoves moves(in.pos);
	// First Move
	{
		const auto move = moves.ExtractMove();
		const auto ret = stat.ImproveWith(-PVS(stat.Play(move)), move);
		if (ret.CausesCut) {
			UpdateTranspositionTable(stat);
			return ret.Values;
		}
	}

	for (const auto& pair : moves)
	{
		const auto& move = pair.second;
		const auto zws_ret = -ZWS(stat.PlayZWS(move));
		if (zws_ret.GetScore() >= stat.beta) { // TODO: Try using ImproveWith and test for a cut.
			const auto ret = stat.ImproveWith(zws_ret, move);
			UpdateTranspositionTable(stat);
			return ret.Values;
		}
		if (zws_ret.GetScore() > stat.alpha) { // if ZWS failed high.
			const auto ret = stat.ImproveWith(-PVS(stat.Play(move)), move);
			if (ret.CausesCut) {
				UpdateTranspositionTable(stat);
				return ret.Values;
			}
		}
	}

	const auto ret = stat.AllMovesSearched();
	UpdateTranspositionTable(stat);
	return ret;
}

PVSearch::AnalysisReturnValues PVSearch::StabilityAnalysis(const InputValues& in)
{
	const auto score = static_cast<int>(64 - 2 * PopCount(GetStableStones(in.pos)));
	return AnalysisReturnValues(-64, score+1, in.pos.EmptyCount(), 0);
}

PVSearch::AnalysisReturnValues PVSearch::TranspositionTableAnalysis(const InputValues& in)
{
	const auto ret = environment->HashTable->LookUp(in.pos);
	const auto& ttValue = ret.second;
	if (ret.first)
		return AnalysisReturnValues(ttValue.alpha, ttValue.beta, ttValue.depth, ttValue.selectivity, ttValue.PV, ttValue.AV);
	else
		return AnalysisReturnValues();
}

void PVSearch::UpdateTranspositionTable(const StatusValues& stat)
{
	environment->HashTable->Update(stat.pos, PvsInfo(GetNodeCount() - stat.InitialNodeCount, stat.depth, stat.selectivity, stat.alpha, stat.beta, stat.PV, stat.AV));
}