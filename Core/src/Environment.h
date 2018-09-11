#pragma once
#include <memory>
#include "Position.h"
#include "Moves.h"
#include "Hashtable.h"

class IFlipper
{
public:
	virtual ~IFlipper() {}
	virtual CPosition Play(const CPosition&, const CMove&) const = 0;
};

class ILastFlipCounter
{
public:
	virtual ~ILastFlipCounter() {}
	virtual uint8_t CountLastFlip(const CPosition&, const CMove&) const = 0;
};

class PvsInfo;

class IStabilityAnalyzer
{
public:
	virtual ~IStabilityAnalyzer() {}
	virtual uint64_t GetStableStones(const CPosition&) const = 0;
};

class IPattern
{
public:
	virtual ~IPattern() {}
	virtual float Eval(const CPosition&) const = 0;
};

struct Environment
{
	std::shared_ptr<IFlipper> Flipper;
	std::shared_ptr<ILastFlipCounter> LastFlipCounter;
	std::shared_ptr<IHashTable<CPosition, PvsInfo>> HashTable;
	std::shared_ptr<IStabilityAnalyzer> StabilityAnalyzer;
	std::shared_ptr<IPattern> PatternEvaluator;

	Environment() : Environment(nullptr, nullptr, nullptr, nullptr, nullptr) {}
	Environment(
		std::shared_ptr<IFlipper> Flipper,
		std::shared_ptr<ILastFlipCounter> LastFlipCounter,
		std::shared_ptr<IHashTable<CPosition, PvsInfo>> HashTable,
		std::shared_ptr<IStabilityAnalyzer> StabilityAnalyzer,
		std::shared_ptr<IPattern> PatternEvaluator)
		: Flipper(Flipper)
		, LastFlipCounter(LastFlipCounter)
		, HashTable(HashTable)
		, StabilityAnalyzer(StabilityAnalyzer)
		, PatternEvaluator(PatternEvaluator)
	{}
};