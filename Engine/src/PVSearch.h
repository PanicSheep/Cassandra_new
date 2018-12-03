#pragma once
#include "Engine.h"
#include "Position.h"
#include "Search.h"
#include "Moves.h"
#include <memory>
#include <optional>

namespace Search
{
	class PVSearch : public CAlgorithm
	{
		class CInput;
		class COutput;
		class CStatusQuo;
		
		COutput Result(const CInput& in, int score, int8_t depth, uint8_t selectivity);

	public:
		PVSearch(const PVSearch&) = default;
		PVSearch(const std::shared_ptr<Engine>& engine) : CAlgorithm(engine) {}

		std::unique_ptr<CAlgorithm> Clone() const override;

		CResult Eval(const CPosition&, CSpecification) override;
	private:
		// --- Triage ---------
		int Eval(const CPosition&, int alpha, int beta, int8_t depth, uint8_t selectivity);
		COutput PVS(const CInput&);
		COutput ZWS(const CInput&);
		// --------------------

		COutput PVS_N(const CInput&);
		COutput ZWS_N(const CInput&);

		// --- Optimized ------
		int Eval_0(const CPosition&);

		int PVS_1(const CPosition&, int alpha, int beta);
		int PVS_1(const CPosition&, int alpha, int beta, CMove);
		int PVS_2(const CPosition&, int alpha, int beta);
		int PVS_2(const CPosition&, int alpha, int beta, CMove, CMove);

		int ZWS_1(const CPosition&, int alpha);
		int ZWS_1(const CPosition&, int alpha, CMove);
		int ZWS_2(const CPosition&, int alpha);
		int ZWS_2(const CPosition&, int alpha, CMove, CMove);
		int ZWS_3(const CPosition&, int alpha);
		int ZWS_3(const CPosition&, int alpha, CMove, CMove, CMove);
		int ZWS_4(const CPosition&, int alpha);
		int ZWS_4(const CPosition&, int alpha, CMove, CMove, CMove, CMove);
		COutput ZWS_A(const CInput&);
		// --------------------

		COutput StabilityAnalysis(const CPosition&);
		COutput TranspositionTableAnalysis(const CPosition&);
		COutput MpcAnalysis(const CInput&);

		void TranspositionTableUpdate(const CPosition&, std::size_t initial_node_count, const COutput&);
	};


	class PVSearch::CInput
	{
	public:
		const CPosition pos;
		const int8_t alpha, beta;
		const int8_t depth;
		const uint8_t selectivity;

		CInput(CPosition pos, int8_t alpha, int8_t beta, int8_t depth, uint8_t selectivity)
			: pos(pos), alpha(alpha), beta(beta), depth(depth), selectivity(selectivity)
		{
			assert(alpha < beta);
		}

		CInput PlayPass() const { return CInput(pos.PlayPass(), -beta, -alpha, depth, selectivity); }
	};

	class PVSearch::COutput
	{
		bool Constrained() const { return (min >= -64) && (max <= +64) && (min <= max); }
	public:
		int8_t min, max;
		int8_t depth;
		uint8_t selectivity;
		CMove PV, AV;

		COutput() : min(-64), max(+64), depth(-1), selectivity(0), PV(CMove::invalid), AV(CMove::invalid)
		{
			assert(Constrained());
		}

		COutput(int8_t min, int8_t max, int8_t depth, uint8_t selectivity, CMove PV = CMove::invalid, CMove AV = CMove::invalid)
			: min(min), max(max), depth(depth), selectivity(selectivity), PV(PV), AV(AV)
		{
			assert(Constrained());
		}

		COutput(int8_t score, int8_t depth, uint8_t selectivity, CMove PV = CMove::invalid, CMove AV = CMove::invalid)
			: COutput(score, score, depth, selectivity, PV, AV)
		{
			assert(Constrained());
		}

		friend COutput operator-(const COutput& o) { return COutput(-o.max, -o.min, o.depth, o.selectivity, o.PV, o.AV); }
	};

	class PVSearch::CStatusQuo
	{
	public:
		int8_t alpha, beta;
		const int8_t depth;
		const uint8_t selectivity;
		int8_t best_score = -infinity; // best score achieved
		CMove PV = Field::invalid;
		CMove AV = Field::invalid;
		const CPosition& pos;

		CStatusQuo(const CInput& in)
			: alpha(in.alpha)
			, beta(in.beta)
			, depth(in.depth)
			, selectivity(in.selectivity)
			, pos(in.pos)
		{
			assert(alpha < beta);
		}

		CInput Play(CMove move) const { return CInput(pos.Play(move), -beta, -alpha, depth - 1, selectivity); }
		CInput PlayZWS(CMove move) const { return CInput(pos.Play(move), -alpha - 1, -alpha, depth - 1, selectivity); }

		std::optional<COutput> ImproveWith(const COutput& new_data)
		{
			assert(alpha < beta);
			assert(best_score <= alpha);
			if ((new_data.depth >= depth) && (new_data.selectivity <= selectivity))
			{
				if ((new_data.min == new_data.max) // exact score
					|| (new_data.min >= beta) // upper cut
					|| (new_data.max <= alpha)) // lower cut
				{
					if (new_data.PV != Field::invalid) PV = new_data.PV; // TODO: Test this, and decide what to do.
					if (new_data.AV != Field::invalid) AV = new_data.AV; // TODO: Test this, and decide what to do.
					return COutput(new_data.min, new_data.max, new_data.depth, new_data.selectivity, PV, AV);
				}

				// You can't be sure to find a value within min and max when there is probability involved.
				if (selectivity == 0)
				{
					if (new_data.max + 1 < beta)
					{
						beta = new_data.max + 1;
						if (new_data.PV != Field::invalid) PV = new_data.PV; // TODO: Test this, and decide what to do.
						if (new_data.AV != Field::invalid) AV = new_data.AV; // TODO: Test this, and decide what to do.
					}
					if (new_data.min - 1 > alpha)
					{
						alpha = new_data.min - 1;
						if (new_data.PV != Field::invalid) PV = new_data.PV; // TODO: Test this, and decide what to do.
						if (new_data.AV != Field::invalid) AV = new_data.AV; // TODO: Test this, and decide what to do.
					}
				}
			}
			if (PV == Field::invalid) PV = new_data.PV;
			if (AV == Field::invalid) AV = new_data.AV;
			assert(alpha < beta);
			assert(best_score <= alpha);
			return {};
		}
		std::optional<COutput> ImproveWith(const COutput& new_data, CMove move)
		{
			assert(alpha < beta);
			assert(best_score <= alpha);
			if ((new_data.depth + 1 >= depth) && (new_data.selectivity <= selectivity)) // TODO: Probably an irrelevant check.
			{
				if (new_data.min >= beta) // upper cut
					return COutput(new_data.min, +64, new_data.depth + 1, new_data.selectivity, move, PV);

				if (new_data.min > best_score)
				{
					best_score = new_data.min;
					//depth = new_data.depth + 1; // TODO: implement best_depth.
					//selectivity = new_data.selectivity; // TODO: implement best_selectivity.
					if (move != PV) {
						AV = PV;
						PV = move;
					}
				}
				alpha = std::max(alpha, new_data.min);
			}
			assert(alpha < beta);
			assert(best_score <= alpha);
			return {};
		}

		COutput AllMovesTried()
		{
			assert(alpha < beta);
			assert(best_score <= alpha);
			return COutput(best_score, alpha, depth, selectivity, PV, AV);
		}
	};
}