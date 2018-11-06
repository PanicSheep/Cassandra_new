#pragma once
#include <memory>
#include "Engine.h"
#include "Position.h"
#include "Search.h"
#include "Moves.h"

namespace Search
{
	class NegaMax : public CAlgorithm
	{
	public:
		NegaMax(const NegaMax&) = default;
		NegaMax(const std::shared_ptr<Engine>& engine) : CAlgorithm(engine) {}
		
		std::unique_ptr<CAlgorithm> Clone() const override;
		
		CResult Eval(const CPosition&, CSpecification) override;
	private:
		int Eval_1(const CPosition&);
		int Eval_2(const CPosition&);
		int Eval_3(const CPosition&);
		int Eval_4(const CPosition&);

		int Eval_0(const CPosition&);
		int Eval_1(const CPosition&, CMove);
		int Eval_2(const CPosition&, CMove, CMove);
		int Eval_3(const CPosition&, CMove, CMove, CMove);
		int Eval_4(const CPosition&, CMove, CMove, CMove, CMove);
		int Eval_N(const CPosition&);
	};
}