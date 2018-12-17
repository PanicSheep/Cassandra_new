#include "PVSearch.h"
#include "LastFlipCounter.h"
#include "FlipFast.h"
#include "Stability.h"
#include "SortedMoves.h"
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

int PVSearch::Eval(const CPosition& pos, int alpha, int beta, int8_t depth, uint8_t selectivity) // TODO: Replace parameters with CInput.
{
	const auto empties = pos.EmptyCount();
	if (depth == empties)
	{
		if (empties <= 14)
			return PVS(CInput(pos, alpha, beta, pos.EmptyCount(), selectivity)).min;

		for (int d = 0; d < empties - 10; d++)
			PVS(CInput(pos, alpha, beta, d, 33));
		PVS(CInput(pos, alpha, beta, pos.EmptyCount(), 33)).min;
		//PVS(CInput(pos, alpha, beta, pos.EmptyCount(), 19)).min;
		//PVS(CInput(pos, alpha, beta, pos.EmptyCount(), 11)).min;
		return PVS(CInput(pos, alpha, beta, pos.EmptyCount(), selectivity)).min;
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
			default:
				break;
		}
	}
	else if (in.depth == 0)
		return Result(in, std::clamp(static_cast<int8_t>(engine->Eval(in.pos)), (int8_t)-64, (int8_t)+64), 0, 0);
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
			case 0: return Eval_d0(in);
			case 1: return Eval_d1(in);
			default:
				break;
		}
	}
	return ZWS_N(in);
}

COutput PVSearch::Eval_d0(const CInput& in)
{
	return Result(in, std::clamp(static_cast<int>(engine->Eval(in.pos)), -64, +64), 0, 0);
}

COutput PVSearch::Eval_d1(const CInput& in)
{
	node_counter++;

	CMoves moves = in.pos.PossibleMoves();
	if (moves.empty()) {
		const auto Pass = in.PlayPass();
		if (Pass.pos.HasMoves())
			return -Eval_d1(Pass);
		else
			return COutput::ExactScore(EvalGameOver(in.pos), in.pos.EmptyCount(), 0);
	}

	CStatusQuo status_quo(in);
	
	while (!moves.empty())
	{
		const auto move = moves.ExtractMove();
		const auto zws = -Eval_d0(status_quo.Play(move));
		const auto ret = status_quo.ImproveWith(zws, move);
		if (ret)
			return ret.value();
	}

	return status_quo.AllMovesTried();
}

COutput PVSearch::ZWS_A(const CInput& in)
{
	assert(in.beta == in.alpha + 1);

	node_counter++;

	const CMoves moves = in.pos.PossibleMoves();
	if (moves.empty()) {
		const auto Pass = in.PlayPass();
		if (Pass.pos.HasMoves())
			return -ZWS_A(Pass);
		else
			return COutput::ExactScore(EvalGameOver(in.pos), in.pos.EmptyCount(), 0);
	}

	CStatusQuo status_quo(in);

	const auto stability = StabilityAnalysis(in.pos);
	if (const auto ret = status_quo.ImproveWith(stability); ret) return ret.value();

	CMoves parity_moves = moves;
	parity_moves.Filter(in.pos.GetParityQuadrants());
	while (!parity_moves.empty())
	{
		const auto move = parity_moves.ExtractMove();
		const auto zws = -ZWS(status_quo.Play(move));
		const auto ret = status_quo.ImproveWith(zws, move);
		if (ret)
			return ret.value();
	}

	CMoves non_parity_moves = moves;
	non_parity_moves.Remove(in.pos.GetParityQuadrants());
	while (!non_parity_moves.empty())
	{
		const auto move = non_parity_moves.ExtractMove();
		const auto zws = -ZWS(status_quo.Play(move));
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

	if (in.pos.HasMoves() == false) {
		const auto Pass = in.PlayPass();
		if (Pass.pos.HasMoves())
			return -ZWS_N(Pass);
		else
			return COutput::ExactScore(EvalGameOver(in.pos), in.pos.EmptyCount(), 0);
	}

	CStatusQuo status_quo(in);

	const auto stability = StabilityAnalysis(in.pos);
	if (const auto ret = status_quo.ImproveWith(stability); ret) return ret.value();

	const auto transposition = TranspositionTableAnalysis(in.pos);
	if (const auto ret = status_quo.ImproveWith(transposition); ret) return ret.value();

	const auto mpc = MpcAnalysis(in);
	if (const auto ret = status_quo.ImproveWith(mpc); ret) return ret.value();

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

	CSortedMoves moves(in, PV, AV);
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

	if (in.pos.HasMoves() == false) {
		const auto Pass = in.PlayPass();
		if (Pass.pos.HasMoves())
			return -PVS_N(Pass);
		else
			return COutput::ExactScore(EvalGameOver(in.pos), in.pos.EmptyCount(), 0);
	}

	CStatusQuo status_quo(in);

	const auto stability = StabilityAnalysis(in.pos);
	if (const auto ret = status_quo.ImproveWith(stability); ret) return ret.value();

	const auto transposition = TranspositionTableAnalysis(in.pos);
	if (const auto ret = status_quo.ImproveWith(transposition); ret) return ret.value();

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
			if (zws.min >= status_quo.beta) {
				const auto ret = status_quo.ImproveWith(zws, move);
				assert(ret);
				TranspositionTableUpdate(in.pos, initial_node_count, ret.value());
				return ret.value();
			}
		}
		if (!searched_pv || (zws.min > status_quo.alpha))
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

	CSortedMoves moves(in, PV, AV);
	for (const auto& pair : moves)
	{
		const auto& move = pair.second;
		COutput zws;
		if (searched_pv)
		{
			zws = -ZWS(status_quo.PlayZWS(move));
			if (zws.min >= status_quo.beta) {
				const auto ret = status_quo.ImproveWith(zws, move);
				assert(ret);
				TranspositionTableUpdate(in.pos, initial_node_count, ret.value());
				return ret.value();
			}
		}
		if (!searched_pv || (zws.min > status_quo.alpha))
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

COutput PVSearch::StabilityAnalysis(const CPosition& pos)
{
	const auto opponents_stable_stones = engine->GetStableStones(pos);
	const auto max_score = static_cast<int>(64 - 2 * PopCount(opponents_stable_stones));
	return COutput::MaxBound(max_score, pos.EmptyCount(), 0);
}

COutput PVSearch::TranspositionTableAnalysis(const CPosition& pos)
{
	const auto ret = engine->LookUp(pos);
	if (ret.has_value())
		return COutput(ret.value().min, ret.value().max, ret.value().depth, ret.value().selectivity, ret.value().best_moves);
	else
		return COutput();
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

COutput PVSearch::MpcAnalysis(const CInput& in)
{
	if (in.selectivity == 0 || in.depth < 4)
		return COutput();

	const int D = in.depth;
	const int d = D / 2;
	const int E = in.pos.EmptyCount();
	const float sigma = Sigma(D, d, E);
	const float sigmas = 32.0f /(in.selectivity + 8.0f);
	const auto upper_bound = static_cast<int8_t>(in.beta + sigmas * sigma);
	const auto lower_bound = static_cast<int8_t>(in.alpha - sigmas * sigma);

	if (upper_bound - 1 >= -65 && upper_bound <= +65)
		if (const auto ret = ZWS(CInput(in.pos, upper_bound - 1, upper_bound, d, 0)); ret.min >= upper_bound)
		{
			//std::cout << "High ProbCut\n";
			return COutput::MinBound(in.beta, in.depth, in.selectivity, ret.best_moves);
		}

	if (lower_bound >= -65 && lower_bound + 1 <= +65)
		if (const auto ret = ZWS(CInput(in.pos, lower_bound, lower_bound + 1, d, 0)); ret.max <= lower_bound)
		{
			//std::cout << "Low ProbCut\n";
			return COutput::MaxBound(in.alpha, in.depth, in.selectivity, ret.best_moves);
		}

	return COutput();
}