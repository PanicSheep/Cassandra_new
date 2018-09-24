#pragma once
#include <memory>
#include "Engine.h"
#include "Position.h"
#include "Search.h"
#include "Moves.h"

class AlphaBetaFailSoftSearch : public Search
{
public:
	AlphaBetaFailSoftSearch(const std::shared_ptr<Engine>& env) : Search(env) {}

	int Eval(const CPosition&) override;

	int Eval(const CPosition&, int alpha, int beta);
private:
	int Eval_1(const CPosition&, int alpha);
	int Eval_2(const CPosition&, int alpha, int beta);
	int Eval_3(const CPosition&, int alpha, int beta);
	int Eval_4(const CPosition&, int alpha, int beta);

	int Eval_0(const CPosition&);
	int Eval_1(const CPosition&, int alpha, CMove move1);
	int Eval_2(const CPosition&, int alpha, int beta, CMove move1, CMove move2);
	int Eval_3(const CPosition&, int alpha, int beta, CMove move1, CMove move2, CMove move3);
	int Eval_4(const CPosition&, int alpha, int beta, CMove move1, CMove move2, CMove move3, CMove move4);
	int Eval_N(const CPosition&, int alpha, int beta);
};