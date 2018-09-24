#pragma once
#include <memory>
#include "Engine.h"
#include "Position.h"
#include "Search.h"
#include "Moves.h"

class NegaMaxSearch : public Search
{
public:
	NegaMaxSearch(const std::shared_ptr<Engine>& env) : Search(env) {}

	int Eval(const CPosition&) override;
private:
	int Eval_1(const CPosition&);
	int Eval_2(const CPosition&);
	int Eval_3(const CPosition&);
	int Eval_4(const CPosition&);

	int Eval_0(const CPosition&);
	int Eval_1(const CPosition&, CMove move1);
	int Eval_2(const CPosition&, CMove move1, CMove move2);
	int Eval_3(const CPosition&, CMove move1, CMove move2, CMove move3);
	int Eval_4(const CPosition&, CMove move1, CMove move2, CMove move3, CMove move4);
	int Eval_N(const CPosition&);
};