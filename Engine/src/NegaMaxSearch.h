#pragma once
#include <memory>
#include "Environment.h"
#include "Position.h"
#include "Search.h"
#include "Moves.h"

class NegaMaxSearch : public Search
{
public:
	NegaMaxSearch(const std::shared_ptr<Environment>& env) : Search(env) {}

	int Eval(const CPosition&) override;
private:
	int Eval_1(const CPosition&);
	int Eval_2(const CPosition&);
	int Eval_3(const CPosition&);
	int Eval_4(const CPosition&);

	int Eval_0(const CPosition&);
	int Eval_1(const CPosition&, const CMove& move1);
	int Eval_2(const CPosition&, const CMove& move1, const CMove& move2);
	int Eval_3(const CPosition&, const CMove& move1, const CMove& move2, const CMove& move3);
	int Eval_4(const CPosition&, const CMove& move1, const CMove& move2, const CMove& move3, const CMove& move4);
	int Eval_N(const CPosition&);
};