#pragma once
#include <cstdint>

#include "Position.h"

class Engine;

class Search
{
public:
	Search(const std::shared_ptr<Engine>&);

	virtual std::unique_ptr<Search> Clone() const = 0;

	virtual int Eval(const CPosition&) = 0;
	virtual int Eval(const CPosition& pos, int8_t depth, uint8_t selectivity) { return Eval(pos); }

	uint64_t NodeCount() const;

protected:
	uint64_t node_counter;
	std::shared_ptr<Engine> engine;
	
	static int EvalGameOver(const CPosition&);
};