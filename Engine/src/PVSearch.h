#pragma once
#include <memory>
#include "Environment.h"
#include "Position.h"
#include "Search.h"
#include "Moves.h"

class PVSearch : public Search
{
	struct InputValues
	{
		CPosition pos;
		int8_t alpha;
		int8_t beta;
		int8_t depth;
		uint8_t selectivity;

		InputValues(CPosition pos, int8_t alpha, int8_t beta, int8_t depth, uint8_t selectivity)
			: pos(pos), alpha(alpha), beta(beta), depth(depth), selectivity(selectivity)
		{}

		InputValues PlayPass() const { return InputValues(pos.PlayPass(), -beta, -alpha, depth, selectivity); }
		InputValues ToZWS() const { return InputValues(pos, alpha - 1, alpha, depth, selectivity); }
	};
	struct AnalysisReturnValues
	{
		int8_t alpha;
		int8_t beta;
		int8_t depth;
		uint8_t selectivity;
		CMove PV;
		CMove AV;

		AnalysisReturnValues() : AnalysisReturnValues(0, 0, -1, 0) {}
		AnalysisReturnValues(int8_t alpha, int8_t beta, int8_t depth, uint8_t selectivity, CMove PV = Field::invalid, CMove AV = Field::invalid)
			: alpha(alpha), beta(beta), depth(depth), selectivity(selectivity), PV(PV), AV(AV)
		{}

		bool empty() const { return depth == -1; }
	};
	struct ReturnValues
	{
		int8_t alpha;
		int8_t beta;
		int8_t depth;
		uint8_t selectivity;

		ReturnValues(int8_t alpha, int8_t beta, int8_t depth, uint8_t selectivity)
			: alpha(alpha), beta(beta), depth(depth), selectivity(selectivity)
		{}
		ReturnValues(int8_t score, int8_t depth, uint8_t selectivity)
			: ReturnValues(score, score, depth, selectivity)
		{}
		ReturnValues() {}

		friend ReturnValues operator-(ReturnValues o) { std::swap(o.alpha, o.beta); o.alpha = -o.alpha; o.beta = -o.beta; return o; }

		int GetScore() const {
			if (alpha == -64)
				return beta;
			return alpha;
		}
	};
	struct AAA
	{
		bool CausesCut;
		ReturnValues Values;

		AAA(bool CausesCut, ReturnValues Values) : CausesCut(CausesCut), Values(Values) {}
	};
	struct StatusValues
	{
		uint64_t InitialNodeCount;
		const CPosition& pos;
		int8_t alpha;
		int8_t beta;
		int8_t depth;
		uint8_t selectivity;
		CMove PV;
		CMove AV;
		bool improved = false;

		StatusValues(const InputValues& in, uint64_t NodeCount)
			: alpha(in.alpha), beta(in.beta), depth(in.depth), selectivity(in.selectivity), PV(Field::invalid), AV(Field::invalid)
			, InitialNodeCount(NodeCount), pos(in.pos)
		{}

		InputValues Play(CMove move) const { return InputValues(pos.Play(move), -beta, -alpha, depth - 1, selectivity); }
		InputValues PlayZWS(CMove move) const { return InputValues(pos.Play(move), -alpha - 1, -alpha, depth - 1, selectivity); }

		AAA ImproveWith(const AnalysisReturnValues& ret)
		{
			if ((ret.depth >= depth) && (ret.selectivity <= selectivity))
			{
				if (ret.beta <= alpha) // Alpha Cut.
					return AAA(true, ReturnValues(-64, ret.beta, ret.depth, ret.selectivity));
				if (ret.alpha >= beta) // Beta Cut.
					return AAA(true, ReturnValues(ret.alpha, +64, ret.depth, ret.selectivity));
				else if (ret.alpha > alpha)
				{
					alpha = alpha;
					//beta = std::min(beta, ret.beta);
					depth = ret.depth;
					selectivity = ret.selectivity;
					if (ret.PV != Field::invalid) PV = ret.PV; // TODO: Test this, and decide what to do.
					if (ret.AV != Field::invalid) AV = ret.AV; // TODO: Test this, and decide what to do.
				}
			}
			return AAA(false, ReturnValues());
		}
		AAA ImproveWith(const ReturnValues& ret, CMove move)
		{
			if ((ret.depth + 1 >= depth) && (ret.selectivity <= selectivity))
			{
				if (ret.alpha >= beta) { // Beta Cut.
					alpha = ret.alpha;
					beta = +64;
					depth = ret.depth + 1;
					selectivity = ret.selectivity;
					if (move != PV) {
						AV = PV;
						PV = move;
					}
					improved = true;
					return AAA(true, ReturnValues(alpha, beta, depth, selectivity));
				}
				else if (ret.alpha > alpha)
				{
					alpha = ret.alpha;
					depth = ret.depth + 1;
					selectivity = ret.selectivity;
					if (move != PV) {
						AV = PV;
						PV = move;
					}
					improved = true;
				}
			}
			return AAA(false, ReturnValues());
		}
		ReturnValues AllMovesSearched()
		{
			beta = alpha;
			if (!improved)
				alpha = -64;
			return ReturnValues(alpha, beta, depth, selectivity);
		}
	};
public:
	PVSearch(const std::shared_ptr<Environment>& env) : Search(env) {}

	int Eval(const CPosition&) override;
	int Eval(const CPosition&, int8_t depth, uint8_t selectivity) override;
private:
	// --- Triage ---------
	int Eval(const CPosition& pos, int alpha, int beta, int8_t depth, uint8_t selectivity);
	ReturnValues PVS(const InputValues&);
	ReturnValues ZWS(const InputValues&);
	// --------------------

	ReturnValues PVS_N(const InputValues&);
	ReturnValues ZWS_N(const InputValues&);

	// --- Optimized ------
	int Eval_0(const CPosition& pos);

	int PVS_1(const CPosition& pos, int alpha, int beta);
	int PVS_1(const CPosition& pos, int alpha, int beta, CMove move1);
	int PVS_2(const CPosition& pos, int alpha, int beta);
	int PVS_2(const CPosition& pos, int alpha, int beta, CMove move1, CMove move2);

	int ZWS_1(const CPosition& pos, int alpha);
	int ZWS_1(const CPosition& pos, int alpha, CMove move1);
	int ZWS_2(const CPosition& pos, int alpha);
	int ZWS_2(const CPosition& pos, int alpha, CMove move1, CMove move2);
	int ZWS_3(const CPosition& pos, int alpha);
	int ZWS_3(const CPosition& pos, int alpha, CMove move1, CMove move2, CMove move3);
	int ZWS_4(const CPosition& pos, int alpha);
	int ZWS_4(const CPosition& pos, int alpha, CMove move1, CMove move2, CMove move3, CMove move4);
	ReturnValues ZWS_A(const InputValues&);
	// --------------------

	AnalysisReturnValues StabilityAnalysis(const InputValues&);
	AnalysisReturnValues TranspositionTableAnalysis(const InputValues&);

	void UpdateTranspositionTable(const StatusValues&);
};