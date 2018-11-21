#include "PVSearch.h"
#include "LastFlipCounter.h"
#include "FlipFast.h"
#include "Stability.h"
#include "SortedMoves.h"
#include <algorithm>

using namespace Search;

std::unique_ptr<CAlgorithm> PVSearch::Clone() const
{
	return std::make_unique<PVSearch>(*this);
}

CResult PVSearch::Eval(const CPosition& pos, CSpecification spec)
{
	const auto old_node_counter = node_counter;
	const auto start_time = std::chrono::high_resolution_clock::now();

	const auto score = Eval(pos, -infinity, +infinity, spec.depth, spec.selectivity);

	const auto end_time = std::chrono::high_resolution_clock::now();
	const auto duration = end_time - start_time;
	const auto node_count = node_counter - old_node_counter;
	return CResult(score, node_count, duration);
}

int PVSearch::Eval(const CPosition& pos, int alpha, int beta, int8_t depth, uint8_t selectivity)
{
	const auto empties = pos.EmptyCount();
	if (depth == empties)
	{
		if (empties <= 14)
			return PVS(InputValues(pos, alpha, beta, pos.EmptyCount(), selectivity)).GetScore();

		for (int d = 0; d < empties - 10; d++)
			PVS(InputValues(pos, alpha, beta, d, selectivity));
		return PVS(InputValues(pos, alpha, beta, pos.EmptyCount(), selectivity)).GetScore();
	}
	else
	{
		const auto in = InputValues(pos, alpha, beta, depth, selectivity);
		return PVS(in).GetScore();
	}
}

PVSearch::ReturnValues PVSearch::PVS(const InputValues& in)
{
	const auto EmptyCount = in.pos.EmptyCount();
	if (in.depth == EmptyCount)
	{
		switch (EmptyCount)
		{
			case 0: return ReturnValues(Eval_0(in.pos), 0, 0);
			case 1: return ReturnValues(PVS_1(in.pos, in.alpha, in.beta), 1, 0);
			case 2: return ReturnValues(PVS_2(in.pos, in.alpha, in.beta), 2, 0);
			default: return PVS_N(in);
		}
	}
	else
	{
		switch (in.depth)
		{
			case 0: return ReturnValues((int)engine->Eval(in.pos), 0, 0);
			default: return PVS_N(in);
		}
	}
}

PVSearch::ReturnValues PVSearch::ZWS(const InputValues& in)
{
	const auto EmptyCount = in.pos.EmptyCount();
	if (in.depth == EmptyCount)
	{
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
	else
	{
		switch (in.depth)
		{
			case 0: return ReturnValues((int)engine->Eval(in.pos), 0, 0);
			default: return ZWS_N(in);
		}
	}
}

int PVSearch::Eval_0(const CPosition& pos)
{
	node_counter++;
	return EvalGameOver(pos);
}

int PVSearch::PVS_1(const CPosition& pos, int alpha, int beta)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();

	return PVS_1(pos, alpha, beta, move1);
}

int PVSearch::PVS_2(const CPosition& pos, int alpha, int beta)
{
	auto moves = CMoves(pos.Empties());
	const auto move1 = moves.ExtractMove();
	const auto move2 = moves.ExtractMove();

	return PVS_2(pos, alpha, beta, move1, move2);
}

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

int PVSearch::PVS_1(const CPosition& pos, int alpha, int beta, const CMove move1)
{
	node_counter++;
	const int score = static_cast<int>(2 * PopCount(pos.GetP())) - 63; // == PopCount(pos.GetP()) - PopCount(pos.GetO())

	if (const auto Diff = engine->CountLastFlip(pos, move1))
	{
		node_counter++;
		return score + Diff + 1;
	}
	else if (const auto Diff = engine->CountLastFlip(pos.PlayPass(), move1))
	{
		node_counter++;
		node_counter++;
		return score - Diff - 1;
	}
	else
		return (score > 0) ? score + 1 : score - 1;
}

int PVSearch::PVS_2(const CPosition& pos, int alpha, int beta, const CMove move1, const CMove move2)
{
	node_counter++;
	int bestscore = -infinity;

	if (const auto flips = Flip(pos, move1)) {
		const auto score = -PVS_1(pos.Play(move1, flips), -beta, -alpha, move2);
		if (score >= beta) return score;
		alpha = std::max(score, alpha);
		bestscore = std::max(score, bestscore);
	}

	if (const auto flips = Flip(pos, move2)) {
		const auto score = -PVS_1(pos.Play(move2, flips), -beta, -alpha, move1);
		if (score >= beta) return score;
		alpha = std::max(score, alpha);
		bestscore = std::max(score, bestscore);
	}

	if (bestscore != -infinity)
		return bestscore;
	bestscore = infinity;

	const auto posPass = pos.PlayPass();

	if (const auto flips = Flip(posPass, move1)) {
		const auto score = PVS_1(posPass.Play(move1, flips), alpha, beta, move2);
		if (score <= alpha) return score;
		beta = std::min(score, beta);
		bestscore = std::min(score, bestscore);
	}

	if (const auto flips = Flip(posPass, move2)) {
		const auto score = PVS_1(posPass.Play(move2, flips), alpha, beta, move1);
		if (score <= alpha) return score;
		beta = std::min(score, beta);
		bestscore = std::min(score, bestscore);
	}

	if (bestscore != infinity) {
		node_counter++;
		return bestscore;
	}
	else
		return -EvalGameOver(posPass);
}

int PVSearch::ZWS_1(const CPosition& pos, int alpha, const CMove move1)
{
	node_counter++;
	const int score = static_cast<int>(2 * PopCount(pos.GetP())) - 63; // == PopCount(pos.GetP()) - PopCount(pos.GetO())

	if (const auto Diff = engine->CountLastFlip(pos, move1))
	{
		node_counter++;
		return score + Diff + 1;
	}
	else if (const auto Diff = engine->CountLastFlip(pos.PlayPass(), move1))
	{
		node_counter++;
		node_counter++;
		return score - Diff - 1;
	}
	else
		return (score > 0) ? score + 1 : score - 1;
}

int PVSearch::ZWS_2(const CPosition& pos, const int alpha, const CMove move1, const CMove move2)
{
	node_counter++;
	int bestscore = -infinity;

	if (const auto flips = Flip(pos, move1)) {
		const auto score = -ZWS_1(pos.Play(move1, flips), -alpha - 1, move2);
		if (score > alpha) return score;
		bestscore = std::max(score, bestscore);
	}

	if (const auto flips = Flip(pos, move2)) {
		return std::max(bestscore, -ZWS_1(pos.Play(move2, flips), -alpha - 1, move1));
	}

	if (bestscore != -infinity)
		return bestscore;
	bestscore = infinity;

	const auto posPass = pos.PlayPass();

	if (const auto flips = Flip(posPass, move1)) {
		const auto score = ZWS_1(posPass.Play(move1, flips), alpha, move2);
		if (score <= alpha) return score;
		bestscore = std::min(score, bestscore);
	}

	if (const auto flips = Flip(posPass, move2)) {
		return std::min(bestscore, ZWS_1(posPass.Play(move2, flips), alpha, move1));
	}

	if (bestscore != infinity) {
		node_counter++;
		return bestscore;
	}
	else
		return -EvalGameOver(posPass);
}

int PVSearch::ZWS_3(const CPosition& pos, int alpha, const CMove move1, const CMove move2, const CMove move3)
{
	node_counter++;
	int bestscore = -infinity;

	if (const auto flips = Flip(pos, move1)) {
		const auto score = -ZWS_2(pos.Play(move1, flips), -alpha - 1, move2, move3);
		if (score > alpha) return score;
		bestscore = std::max(score, bestscore);
	}

	if (const auto flips = Flip(pos, move2)) {
		const auto score = -ZWS_2(pos.Play(move2, flips), -alpha - 1, move1, move3);
		if (score > alpha) return score;
		bestscore = std::max(score, bestscore);
	}

	if (const auto flips = Flip(pos, move3)) {
		return std::max(bestscore, -ZWS_2(pos.Play(move3, flips), -alpha - 1, move1, move2));
	}

	if (bestscore != -infinity)
		return bestscore;
	bestscore = infinity;

	const auto posPass = pos.PlayPass();

	if (const auto flips = Flip(posPass, move1)) {
		const auto score = ZWS_2(posPass.Play(move1, flips), alpha, move2, move3);
		if (score <= alpha) return score;
		bestscore = std::min(score, bestscore);
	}

	if (const auto flips = Flip(posPass, move2)) {
		const auto score = ZWS_2(posPass.Play(move2, flips), alpha, move1, move3);
		if (score <= alpha) return score;
		bestscore = std::min(score, bestscore);
	}

	if (const auto flips = Flip(posPass, move3)) {
		return std::min(bestscore, ZWS_2(posPass.Play(move3, flips), alpha, move1, move2));
	}

	if (bestscore != infinity) {
		node_counter++;
		return bestscore;
	}
	else
		return -EvalGameOver(posPass);
}

int PVSearch::ZWS_4(const CPosition& pos, int alpha, const CMove move1, const CMove move2, const CMove move3, const CMove move4)
{
	node_counter++;
	int bestscore = -infinity;

	if (const auto flips = Flip(pos, move1)) {
		const auto score = -ZWS_3(pos.Play(move1, flips), -alpha - 1, move2, move3, move4);
		if (score > alpha) return score;
		bestscore = std::max(score, bestscore);
	}

	if (const auto flips = Flip(pos, move2)) {
		const auto score = -ZWS_3(pos.Play(move2, flips), -alpha - 1, move1, move3, move4);
		if (score > alpha) return score;
		bestscore = std::max(score, bestscore);
	}

	if (const auto flips = Flip(pos, move3)) {
		const auto score = -ZWS_3(pos.Play(move3, flips), -alpha - 1, move1, move2, move4);
		if (score > alpha) return score;
		bestscore = std::max(score, bestscore);
	}

	if (const auto flips = Flip(pos, move4)) {
		return std::max(bestscore, -ZWS_3(pos.Play(move4, flips), -alpha - 1, move1, move2, move3));
	}

	if (bestscore != -infinity)
		return bestscore;
	bestscore = infinity;

	const auto posPass = pos.PlayPass();

	if (const auto flips = Flip(posPass, move1)) {
		const auto score = ZWS_3(posPass.Play(move1, flips), alpha, move2, move3, move4);
		if (score <= alpha) return score;
		bestscore = std::min(score, bestscore);
	}

	if (const auto flips = Flip(posPass, move2)) {
		const auto score = ZWS_3(posPass.Play(move2, flips), alpha, move1, move3, move4);
		if (score <= alpha) return score;
		bestscore = std::min(score, bestscore);
	}

	if (const auto flips = Flip(posPass, move3)) {
		const auto score = ZWS_3(posPass.Play(move3, flips), alpha, move1, move2, move4);
		if (score <= alpha) return score;
		bestscore = std::min(score, bestscore);
	}

	if (const auto flips = Flip(posPass, move4)) {
		return std::min(bestscore, ZWS_3(posPass.Play(move4, flips), alpha, move1, move2, move3));
	}

	if (bestscore != infinity) {
		node_counter++;
		return bestscore;
	}
	else
		return -EvalGameOver(posPass);
}

PVSearch::ReturnValues PVSearch::ZWS_A(const InputValues& in)
{
	assert(in.beta == in.alpha + 1);

	node_counter++;

	const CMoves moves = in.pos.PossibleMoves();
	if (moves.empty()) {
		const auto Pass = in.PlayPass();
		if (Pass.pos.HasMoves())
			return -ZWS_A(Pass);
		else
		{
			const auto score = EvalGameOver(in.pos);
			return ReturnValues(score, score, in.pos.EmptyCount(), 0);
		}
	}

	StatusValues stat(in, NodeCount());
	if (const auto ret = stat.ImproveWith(StabilityAnalysis(in)); ret.CausesCut) return ret.Values;
	//if (const auto ret = stat.ImproveWith(TranspositionTableAnalysis(in)); ret.CausesCut) return ret.Values;

	int bestscore = -infinity;
	CMoves parity_moves = moves;
	parity_moves.Filter(in.pos.GetParityQuadrants());
	while (!parity_moves.empty())
	{
		const auto move = parity_moves.ExtractMove();
		const auto ret = stat.ImproveWith(-ZWS(stat.Play(move)), move);
		if (ret.CausesCut) {
			//UpdateTranspositionTable(stat);
			return ret.Values;
		}
	}
	CMoves non_parity_moves = moves;
	non_parity_moves.Remove(in.pos.GetParityQuadrants());
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

	node_counter++;

	if (in.pos.HasMoves() == false) {
		const auto Pass = in.PlayPass();
		if (Pass.pos.HasMoves())
			return -ZWS_N(Pass);
		else
		{
			const auto score = EvalGameOver(in.pos);
			return ReturnValues(score, score, in.pos.EmptyCount(), 0);
		}
	}

	StatusValues stat(in, NodeCount());
	if (const auto ret = stat.ImproveWith(StabilityAnalysis(in)); ret.CausesCut) return ret.Values;
	const auto TTRet = TranspositionTableAnalysis(in);
	if (const auto ret = stat.ImproveWith(TTRet); ret.CausesCut) return ret.Values;

	//if (const auto ret = stat.ImproveWith(MPCAnalysis(in)); ret.CausesCut)
	//	return ret.Values;

	if (TTRet.PV != Field::invalid)
	{
		const auto& move = TTRet.PV;
		const auto ret = stat.ImproveWith(-ZWS(stat.Play(move)), move);
		if (ret.CausesCut) {
			UpdateTranspositionTable(stat);
			return ret.Values;
		}
	}
	if (TTRet.AV != Field::invalid)
	{
		const auto& move = TTRet.AV;
		const auto ret = stat.ImproveWith(-ZWS(stat.Play(move)), move);
		if (ret.CausesCut) {
			UpdateTranspositionTable(stat);
			return ret.Values;
		}
	}

	CSortedMoves moves(in.pos, TTRet.PV, TTRet.AV);
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
	node_counter++;

	if (in.pos.HasMoves() == false) {
		const auto Pass = in.PlayPass();
		if (Pass.pos.HasMoves())
			return -PVS_N(Pass);
		else
		{
			const auto score = EvalGameOver(in.pos);
			return ReturnValues(score, score, in.pos.EmptyCount(), 0);
		}
	}

	StatusValues stat(in, NodeCount());
	if (const auto ret = stat.ImproveWith(StabilityAnalysis(in)); ret.CausesCut) return ret.Values;
	const auto TTRet = TranspositionTableAnalysis(in);

	if (TTRet.PV != Field::invalid)
	{
		const auto& move = TTRet.PV;
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
	if (TTRet.AV != Field::invalid)
	{
		const auto& move = TTRet.AV;
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

	CSortedMoves moves(in.pos, TTRet.PV, TTRet.AV);
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
	const auto opponents_stable_stones = engine->GetStableStones(in.pos);
	const auto max_score = static_cast<int>(64 - 2 * PopCount(opponents_stable_stones));
	return AnalysisReturnValues(-infinity, max_score + 1, in.pos.EmptyCount(), 0);
}

PVSearch::AnalysisReturnValues PVSearch::TranspositionTableAnalysis(const InputValues& in)
{
	const auto ret = engine->LookUp(in.pos);
	const auto& ttValue = ret.second;
	if (ret.first)
		return AnalysisReturnValues(ttValue.alpha, ttValue.beta, ttValue.depth, ttValue.selectivity, ttValue.PV, ttValue.AV);
	else
		return AnalysisReturnValues();
}

void PVSearch::UpdateTranspositionTable(const StatusValues& stat)
{
	engine->Update(stat.pos, PvsInfo(node_counter - stat.InitialNodeCount, stat.depth, stat.selectivity, stat.alpha, stat.beta, stat.PV, stat.AV));
}

float Sigma(const int D, const int d, const int E) noexcept
{
	const double alpha = -0.21311527;
	const double beta = 1.06454983;
	const double gamma = 0.26639884;
	const double delta = -0.02005392;
	const double epsilon = 2.09164003;

	return (std::exp(alpha*d) + beta) * std::pow((D - d), gamma) * (delta*E + epsilon);
}

PVSearch::AnalysisReturnValues PVSearch::MPCAnalysis(const InputValues& in)
{
	if (in.selectivity == 0 || in.depth < 4)
		return AnalysisReturnValues();

	const int D = in.depth;
	const int d = D / 2;
	const int E = in.pos.EmptyCount();
	const float sigma = Sigma(D, d, E);
	const float sigmas = in.selectivity ? in.selectivity / 10.0f : 100.0f;;
	const int upper_bound = std::round(in.beta + sigmas * sigma);
	const int lower_bound = std::round(in.alpha - sigmas * sigma);

	//if (const auto ret = ZWS(InputValues(in.pos, upper_bound - 1, upper_bound, d, 0)); ret.alpha >= upper_bound)
	//	return AnalysisReturnValues(in.beta, +infinity, in.depth, in.selectivity);

	//if (const auto ret = ZWS(InputValues(in.pos, lower_bound, lower_bound + 1, d, 0)); ret.beta <= lower_bound)
	//	return AnalysisReturnValues(-infinity, in.alpha, in.depth, in.selectivity);

	return AnalysisReturnValues();
}