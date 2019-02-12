#pragma once
#include "AlphaBetaFailSoftSearch.h"
#include "Engine.h"
#include "Moves.h"
#include "Position.h"
#include "Search.h"
#include "SearchInput.h"
#include "SearchOutput.h"
#include "SearchStatusQuo.h"
#include <memory>
#include <optional>

namespace Search
{
	class PVSearch : public AlphaBetaFailSoft
	{
		int AspirationSearch(const CPosition& pos, int alpha, int beta, int score, int delta, int8_t depth, uint8_t selectivity);
	public:
		PVSearch(const PVSearch&) = default;
		PVSearch(const std::shared_ptr<Engine>& engine) : AlphaBetaFailSoft(engine) {}

		std::unique_ptr<CAlgorithm> Clone() const override;

		CResult Eval(const CPosition&, CSpecification) override;
		using CAlgorithm::Eval;
	private:
		int Eval(const CPosition&, int alpha, int beta, int8_t depth, uint8_t selectivity);
		COutput PVS(const CInput&);
		COutput ZWS(const CInput&);

		COutput PVS_N(const CInput&);
		COutput ZWS_N(const CInput&);
		COutput ZWS_A(const CInput&);

		int Eval_d0(const CPosition&);
		int Eval_d1(const CPosition&, int alpha, int beta);
		int Eval_d2(const CPosition&, int alpha, int beta);

		CAnalysisOutput StabilityAnalysis(const CPosition&);
		std::optional<CAnalysisOutput> TranspositionTableAnalysis(const CPosition&);
		std::optional<CAnalysisOutput> MpcAnalysis(const CInput&);

		void TranspositionTableUpdate(const CPosition&, std::size_t initial_node_count, const COutput&);

		COutput Result(const CInput& in, int score, int8_t depth, uint8_t selectivity);
	};

	int32_t MoveOrderingScorer(CMove, const CInput&) noexcept;
}