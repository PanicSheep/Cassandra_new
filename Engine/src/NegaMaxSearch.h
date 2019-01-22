#pragma once
#include "Engine.h"
#include "Moves.h"
#include "Position.h"
#include "Search.h"
#include <memory>

namespace Search
{
	class NegaMax : public CAlgorithm
	{
	public:
		NegaMax(const std::shared_ptr<Engine>& engine) : CAlgorithm(engine) {}
		
		std::unique_ptr<CAlgorithm> Clone() const override;
		
		CResult Eval(const CPosition&, CSpecification) override;
		using CAlgorithm::Eval;
	private:
		int Eval_ (const CPosition&);
	protected:
		int Eval_0(const CPosition&);
		int Eval_1(const CPosition&);
		int Eval_1(const CPosition&, CMove);
	private:
		int Eval_2(const CPosition&);
		int Eval_2(const CPosition&, CMove, CMove);
		int Eval_3(const CPosition&);
		int Eval_3(const CPosition&, CMove, CMove, CMove);
		int Eval_4(const CPosition&);
		int Eval_4(const CPosition&, CMove, CMove, CMove, CMove);
		int Eval_N(const CPosition&);
	};
}