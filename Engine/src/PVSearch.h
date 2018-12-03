#pragma once
#include "Engine.h"
#include "Position.h"
#include "Search.h"
#include "Moves.h"
#include "SearchInput.h"
#include "SearchOutput.h"
#include "SearchStatusQuo.h"
#include <memory>
#include <optional>

namespace Search
{
	class PVSearch : public CAlgorithm
	{
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
}