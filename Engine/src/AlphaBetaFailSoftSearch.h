#pragma once
#include "Engine.h"
#include "Moves.h"
#include "Position.h"
#include "NegaMaxSearch.h"
#include <memory>

namespace Search
{
	class AlphaBetaFailSoft : public NegaMax
	{
	public:
		AlphaBetaFailSoft(const std::shared_ptr<Engine>& engine) : NegaMax(engine) {}

		std::unique_ptr<CAlgorithm> Clone() const override;
		
		CResult Eval(const CPosition&, CSpecification) override;
	protected:
		int Eval  (const CPosition&, int alpha, int beta);
		int Eval_2(const CPosition&, int alpha, int beta);
		int Eval_2(const CPosition&, int alpha, int beta, CMove, CMove);
		int Eval_3(const CPosition&, int alpha, int beta);
		int Eval_3(const CPosition&, int alpha, int beta, CMove, CMove, CMove);
		int Eval_4(const CPosition&, int alpha, int beta);
		int Eval_4(const CPosition&, int alpha, int beta, CMove, CMove, CMove, CMove);
		int Eval_N(const CPosition&, int alpha, int beta);
	};
}