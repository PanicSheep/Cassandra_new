#pragma once
#include <memory>
#include "Environment.h"
#include "Position.h"
#include "Search.h"
#include "Moves.h"

class AlphaBetaFailSoftSearch : public Search
{
public:
	AlphaBetaFailSoftSearch(const std::shared_ptr<Environment>& env) : Search(env) {}

	int Eval(const CPosition&) override;

	int Eval(const CPosition&, int alpha, int beta);
private:
	int Eval_1(const CPosition&, int alpha);
	int Eval_2(const CPosition&, int alpha, int beta);
	int Eval_3(const CPosition&, int alpha, int beta);
	int Eval_4(const CPosition&, int alpha, int beta);

	int Eval_0(const CPosition&);
	int Eval_1(const CPosition&, int alpha, const CMove& move1);
	int Eval_2(const CPosition&, int alpha, int beta, const CMove& move1, const CMove& move2);
	int Eval_3(const CPosition&, int alpha, int beta, const CMove& move1, const CMove& move2, const CMove& move3);
	int Eval_4(const CPosition&, int alpha, int beta, const CMove& move1, const CMove& move2, const CMove& move3, const CMove& move4);
	int Eval_N(const CPosition&, int alpha, int beta);
};