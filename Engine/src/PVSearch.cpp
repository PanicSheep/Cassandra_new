#include "PVSearch.h"
#include "FlipFast.h"
#include "LastFlipCounter.h"
#include "SortedMoves.h"
#include "Stability.h"
#include <algorithm>

using namespace Search;

COutput PVSearch::Result(const CInput & in, int score, int8_t depth, uint8_t selectivity)
{
	if (score >= in.beta) // upper cut
		return COutput::MinBound(score, depth, selectivity);
	if (score <= in.alpha) // lower cut
		return COutput::MaxBound(score, depth, selectivity);

	return COutput::ExactScore(score, depth, selectivity);
}

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

int PVSearch::AspirationSearch(const CPosition& pos, int alpha, int beta, int score, int delta, int8_t depth, uint8_t selectivity)
{
	const auto new_score = PVS(CInput(pos, score - delta, score + delta, depth, selectivity));
	if (new_score.max <= score - delta) // fail low
		return PVS(CInput(pos, alpha, new_score.max + 1, depth, selectivity)).min;
	if (new_score.min >= score + delta) // fail high
		return PVS(CInput(pos, new_score.min - 1, beta, depth, selectivity)).min;
	return new_score.min;
}

int PVSearch::Eval(const CPosition& pos, int alpha, int beta, int8_t depth, uint8_t selectivity) // TODO: Replace parameters with CInput.
{
	const auto empty_count = pos.EmptyCount();
	if (depth == empty_count)
	{
		if (empty_count <= 15)
			return PVS(CInput(pos, alpha, beta, empty_count, selectivity)).min;
		
		int score = 0;
		for (int d = 3; d <= empty_count - 10; d++)
			score = PVS(CInput(pos, alpha, beta, d, 100 - 11)).min;
		
		for (uint8_t selectivity : {100 - 11, 100 - 15, 100 - 20, 100 - 26, 100 - 33})
			score = AspirationSearch(pos, alpha, beta, score, 5, empty_count, selectivity);
		return AspirationSearch(pos, alpha, beta, score, 1, empty_count, selectivity);
	}
	else
	{
		const auto in = CInput(pos, alpha, beta, depth, selectivity);
		return PVS(in).min;
	}
}

COutput PVSearch::PVS(const CInput& in)
{
	const auto EmptyCount = in.pos.EmptyCount();
	if (in.depth == EmptyCount)
	{
		switch (EmptyCount)
		{
			case 0: return Result(in, Eval_0(in.pos), 0, 0);
			case 1: return Result(in, Eval_1(in.pos), 1, 0);
			case 2: return Result(in, Eval_2(in.pos, in.alpha, in.beta), 2, 0);
			case 3: return Result(in, Eval_3(in.pos, in.alpha, in.beta), 3, 0);
			case 4: return Result(in, Eval_4(in.pos, in.alpha, in.beta), 4, 0);
			default:
				break;
		}
	}
	else
	{
		switch (in.depth)
		{
			case 0: return Result(in, std::clamp(Eval_d0(in.pos), -64, +64), 0, 0);
			case 1: return Result(in, std::clamp(Eval_d1(in.pos, in.alpha, in.beta), -64, +64), 1, 0);
			case 2: return Result(in, std::clamp(Eval_d2(in.pos, in.alpha, in.beta), -64, +64), 2, 0);
			default:
				break;
		}
	}
	return PVS_N(in);
}

COutput PVSearch::ZWS(const CInput& in)
{
	const auto EmptyCount = in.pos.EmptyCount();
	if (in.depth == EmptyCount)
	{
		switch (EmptyCount)
		{
			case 0: return Result(in, Eval_0(in.pos), 0, 0);
			case 1: return Result(in, Eval_1(in.pos), 1, 0);
			case 2: return Result(in, Eval_2(in.pos, in.alpha, in.alpha+1), 2, 0);
			case 3: return Result(in, Eval_3(in.pos, in.alpha, in.alpha+1), 3, 0);
			case 4: return Result(in, Eval_4(in.pos, in.alpha, in.alpha+1), 4, 0);
			case 5:
			case 6:
			case 7:
				return ZWS_A(in);
			default:
				break;
		}
	}
	else
	{
		switch (in.depth)
		{
			case 0: return Result(in, std::clamp(Eval_d0(in.pos), -64, +64), 0, 0);
			case 1: return Result(in, std::clamp(Eval_d1(in.pos, in.alpha, in.alpha + 1), -64, +64), 1, 0);
			case 2: return Result(in, std::clamp(Eval_d2(in.pos, in.alpha, in.alpha + 1), -64, +64), 2, 0);
			default:
				break;
		}
	}
	return ZWS_N(in);
}

int PVSearch::Eval_d0(const CPosition& pos)
{
	node_counter++;

	return static_cast<int>(engine->Eval(pos));
}

int PVSearch::Eval_d1(const CPosition& pos, int alpha, int beta)
{
	node_counter++;

	CMoves moves = pos.PossibleMoves();
	if (moves.empty()) {
		const auto PosPass = pos.PlayPass();
		if (PosPass.HasMoves())
			return -Eval_d1(PosPass, -beta, -alpha);

		return EvalGameOver(pos);
	}

	int bestscore = -infinity;
	while (!moves.empty())
	{
		const auto score = -Eval_d0(pos.Play(moves.ExtractMove()));
		if (score >= beta) return score;
		if (score > bestscore) bestscore = score;
	}

	return bestscore;
}

int PVSearch::Eval_d2(const CPosition& pos, int alpha, int beta)
{
	node_counter++;

	CMoves moves = pos.PossibleMoves();
	if (moves.empty()) {
		const auto PosPass = pos.PlayPass();
		if (PosPass.HasMoves())
			return -Eval_d2(PosPass, -beta, -alpha);

		return EvalGameOver(pos);
	}

	int bestscore = -infinity;
	while (!moves.empty())
	{
		const auto score = -Eval_d1(pos.Play(moves.ExtractMove()), -beta, -alpha);
		if (score >= beta) return score;
		if (score > alpha) alpha = score;
		if (score > bestscore) bestscore = score;
	}

	return bestscore;
}

COutput PVSearch::ZWS_A(const CInput& in)
{
	assert(in.beta == in.alpha + 1);

	if (in.pos.EmptyCount() == 4)
		return Result(in, Eval_4(in.pos, in.alpha, in.beta), 4, 0);
	node_counter++;

	const auto moves = in.pos.PossibleMoves();
	if (moves.empty()) {
		const auto Pass = in.PlayPass();
		if (Pass.pos.HasMoves())
			return -ZWS_A(Pass);
		return COutput::ExactScore(EvalGameOver(in.pos), in.pos.EmptyCount(), 0);
	}

	CStatusQuo status_quo(in);

	CMoves parity_moves = moves;
	parity_moves.Filter(in.pos.GetParityQuadrants());
	while (!parity_moves.empty())
	{
		const auto move = parity_moves.ExtractMove();
		const auto zws = -ZWS_A(status_quo.Play(move));
		const auto ret = status_quo.ImproveWith(zws, move);
		if (ret)
			return ret.value();
	}

	CMoves non_parity_moves = moves;
	non_parity_moves.Remove(in.pos.GetParityQuadrants());
	while (!non_parity_moves.empty())
	{
		const auto move = non_parity_moves.ExtractMove();
		const auto zws = -ZWS_A(status_quo.Play(move));
		const auto ret = status_quo.ImproveWith(zws, move);
		if (ret)
			return ret.value();
	}
	return status_quo.AllMovesTried();
}

COutput PVSearch::ZWS_N(const CInput& in)
{
	assert(in.alpha + 1 == in.beta);
	
	const uint64_t initial_node_count = node_counter++;

	if (!in.pos.HasMoves()) {
		const auto Pass = in.PlayPass();
		if (Pass.pos.HasMoves())
			return -ZWS_N(Pass);
		return COutput::ExactScore(EvalGameOver(in.pos), in.pos.EmptyCount(), 0);
	}

	CStatusQuo status_quo(in);

	const auto stability = StabilityAnalysis(in.pos);
	if (const auto ret = status_quo.ImproveWith(stability); ret) return ret.value();

	if (const auto transposition = TranspositionTableAnalysis(in.pos); transposition)
		if (const auto ret = status_quo.ImproveWith(transposition.value()); ret) return ret.value();

	if (const auto mpc = MpcAnalysis(in); mpc)
		if (const auto ret = status_quo.ImproveWith(mpc.value()); ret) return ret.value();

	const auto PV = status_quo.best_moves.PV;
	const auto AV = status_quo.best_moves.AV;

	if (PV != Field::invalid)
	{
		const auto& move = PV;
		const auto zws = -ZWS(status_quo.Play(move));
		const auto ret = status_quo.ImproveWith(zws, move);
		if (ret) {
			TranspositionTableUpdate(in.pos, initial_node_count, ret.value());
			return ret.value();
		}
	}
	if (AV != Field::invalid)
	{
		const auto& move = AV;
		const auto zws = -ZWS(status_quo.Play(move));
		const auto ret = status_quo.ImproveWith(zws, move);
		if (ret) {
			TranspositionTableUpdate(in.pos, initial_node_count, ret.value());
			return ret.value();
		}
	}

	CSortedMoves moves(in.pos, [&](CMove move) { return MoveOrderingScorer(move, in); }, PV, AV);
	for (const auto& pair : moves)
	{
		const auto& move = pair.second;
		const auto zws = -ZWS(status_quo.Play(move));
		const auto ret = status_quo.ImproveWith(zws, move);
		if (ret) {
			TranspositionTableUpdate(in.pos, initial_node_count, ret.value());
			return ret.value();
		}
	}

	const auto ret = status_quo.AllMovesTried();
	TranspositionTableUpdate(in.pos, initial_node_count, ret);
	return ret;
}

COutput PVSearch::PVS_N(const CInput& in)
{
	const uint64_t initial_node_count = node_counter++;

	if (!in.pos.HasMoves()) {
		const auto Pass = in.PlayPass();
		if (Pass.pos.HasMoves())
			return -PVS_N(Pass);
		return COutput::ExactScore(EvalGameOver(in.pos), in.pos.EmptyCount(), 0);
	}

	CStatusQuo status_quo(in);

	const auto stability = StabilityAnalysis(in.pos);
	if (const auto ret = status_quo.ImproveWith(stability); ret) return ret.value();

	if (const auto transposition = TranspositionTableAnalysis(in.pos); transposition)
		if (const auto ret = status_quo.ImproveWith(transposition.value()); ret) return ret.value();

	const auto PV = status_quo.best_moves.PV;
	const auto AV = status_quo.best_moves.AV;
	bool searched_pv = false;

	if (PV != Field::invalid)
	{
		const auto& move = PV;
		const auto pvs = -PVS(status_quo.Play(move));
		const auto ret = status_quo.ImproveWith(pvs, move);
		if (ret) {
			TranspositionTableUpdate(in.pos, initial_node_count, ret.value());
			return ret.value();
		}
		searched_pv = true;
	}
	if (AV != Field::invalid)
	{
		const auto& move = AV;
		COutput zws;
		if (searched_pv)
		{
			zws = -ZWS(status_quo.PlayZWS(move));
			if (const auto ret = status_quo.ImproveWithZWS(zws, move)) {
				TranspositionTableUpdate(in.pos, initial_node_count, ret.value());
				return ret.value();
			}
		}
		if (!searched_pv || (zws.min > status_quo.CurrentAlpha()))
		{
			const auto pvs = -PVS(status_quo.Play(move));
			const auto ret = status_quo.ImproveWith(pvs, move);
			if (ret) {
				TranspositionTableUpdate(in.pos, initial_node_count, ret.value());
				return ret.value();
			}
			searched_pv = true;
		}
	}

	CSortedMoves moves(in.pos, [&](CMove move) { return MoveOrderingScorer(move, in); }, PV, AV);
	for (const auto& pair : moves)
	{
		const auto& move = pair.second;
		COutput zws;
		if (searched_pv)
		{
			zws = -ZWS(status_quo.PlayZWS(move));
			if (const auto ret = status_quo.ImproveWithZWS(zws, move)) {
				TranspositionTableUpdate(in.pos, initial_node_count, ret.value());
				return ret.value();
			}
		}
		if (!searched_pv || (zws.min > status_quo.CurrentAlpha()))
		{
			const auto pvs = -PVS(status_quo.Play(move));
			const auto ret = status_quo.ImproveWith(pvs, move);
			if (ret) {
				TranspositionTableUpdate(in.pos, initial_node_count, ret.value());
				return ret.value();
			}
			searched_pv = true;
		}
	}

	const auto ret = status_quo.AllMovesTried();
	TranspositionTableUpdate(in.pos, initial_node_count, ret);
	return ret;
}

CAnalysisOutput PVSearch::StabilityAnalysis(const CPosition& pos)
{
	const auto opponents_stable_stones = engine->GetStableStones(pos);
	const auto max_score = static_cast<int>(64 - 2 * PopCount(opponents_stable_stones));
	return CAnalysisOutput::MaxBound(max_score, pos.EmptyCount(), 0);
}

std::optional<CAnalysisOutput> PVSearch::TranspositionTableAnalysis(const CPosition& pos)
{
	const auto look_up = engine->LookUp(pos);
	if (look_up.has_value())
	{
		const auto& ret = look_up.value();
		return CAnalysisOutput(ret.min, ret.max,
			ret.depth, ret.selectivity, 
			ret.best_moves);
	}
	return {};
}

void PVSearch::TranspositionTableUpdate(const CPosition& pos, std::size_t initial_node_count, const COutput& novum)
{
	engine->Update(pos, 
				   PvsInfo(novum.min, novum.max,
					   novum.depth, novum.selectivity,
					   novum.best_moves, node_counter - initial_node_count));
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

std::optional<CAnalysisOutput> PVSearch::MpcAnalysis(const CInput& in)
{
	if (in.selectivity == 0 || in.depth < 6)
		return {};

	const int zero_eval = Eval_d0(in.pos);
	const int D = in.depth;
	const int d = D / 3.0;
	const int E = in.pos.EmptyCount();
	const float sigma_d = Sigma(D, d, E);
	const float t = (100 - in.selectivity) / 10.0;

	const auto upper_bound = std::ceil(in.beta + t * sigma_d);
	const auto upper_alpha = upper_bound - 1;
	const auto upper_beta = upper_bound;

	if (zero_eval >= upper_bound && -65 <= upper_alpha && upper_beta <= +64)
		if (const auto ret = ZWS(CInput(in.pos, upper_alpha, upper_beta, d, 0)); ret.min >= upper_beta)
		{
			const auto cut_t = (ret.min - in.beta) / sigma_d;
			const auto cut_selectivity = std::ceil(100 - cut_t * 10.0);
			//std::cout << "High ProbCut: in_selectivity=" << std::to_string(in.selectivity) 
			//	<< " out_selectivity=" << std::to_string(cut_selectivity) << "\n";
			return CAnalysisOutput::MinBound(in.beta, in.depth, cut_selectivity, ret.best_moves);
		}

	const auto lower_bound = std::floor(in.alpha - t * sigma_d);
	const auto lower_alpha = lower_bound;
	const auto lower_beta = lower_bound + 1;

	if (zero_eval <= lower_bound && -64 <= lower_alpha && lower_beta <= +65)
		if (const auto ret = ZWS(CInput(in.pos, lower_alpha, lower_beta, d, 0)); ret.max <= lower_alpha)
		{
			const auto cut_t = (in.alpha - ret.max) / sigma_d;
			const auto cut_selectivity = std::ceil(100 - cut_t * 10.0);
			//std::cout << "Low ProbCut: in_selectivity=" << std::to_string(in.selectivity)
			//	<< " out_selectivity=" << std::to_string(cut_selectivity) << "\n";
			return CAnalysisOutput::MaxBound(in.alpha, in.depth, cut_selectivity, ret.best_moves);
		}
	return {};
}

constexpr uint64_t SMEAR_BITBOARD(uint64_t B) noexcept
{
	// 4x SHIFT, 4x OR, 2x AND
	// = 10 OPs
	B |= ((B >> 1) & 0x7F7F7F7F7F7F7F7Fui64) | ((B << 1) & 0xFEFEFEFEFEFEFEFEui64);
	return B | (B >> 8) | (B << 8);
}

constexpr uint64_t OpponentsExposed(const uint64_t P, const uint64_t O) noexcept { return SMEAR_BITBOARD(~(P | O)) & O; } // 13 OPs

int32_t Search::MoveOrderingScorer(CMove move, const CInput& in) noexcept
{
	static const int8_t FieldValue[64] = {
		9, 2, 8, 6, 6, 8, 2, 9,
		2, 1, 3, 4, 4, 3, 1, 2,
		8, 3, 7, 5, 5, 7, 3, 8,
		6, 4, 5, 0, 0, 5, 4, 6,
		6, 4, 5, 0, 0, 5, 4, 6,
		8, 3, 7, 5, 5, 7, 3, 8,
		2, 1, 3, 4, 4, 3, 1, 2,
		9, 2, 8, 6, 6, 8, 2, 9,
	};

	const auto next_pos = in.pos.Play(move);
	const auto next_possible_moves = next_pos.PossibleMoves();
	const auto mobility_score = next_possible_moves.size() << 17;
	const auto corner_mobility_score = ((next_possible_moves.HasMove(CMove::A1) ? 1 : 0)
		+ (next_possible_moves.HasMove(CMove::A8) ? 1 : 0) 
		+ (next_possible_moves.HasMove(CMove::H1) ? 1 : 0) 
		+ (next_possible_moves.HasMove(CMove::H8) ? 1 : 0)) << 18;
	const auto opponents_exposed_score = PopCount(OpponentsExposed(next_pos.GetP(), next_pos.GetO())) << 6;
	const auto field_score = FieldValue[move];
	return field_score - mobility_score - corner_mobility_score - opponents_exposed_score;
}
