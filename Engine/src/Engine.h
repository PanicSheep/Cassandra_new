#pragma once
#include <memory>
#include "Position.h"
#include "Moves.h"
#include "HashtablePVS.h"

class IFlipper
{
public:
	virtual ~IFlipper() {}
	virtual CPosition Play(const CPosition&, CMove) const = 0;
};

class ILastFlipCounter
{
public:
	virtual ~ILastFlipCounter() {}
	virtual uint8_t CountLastFlip(const CPosition&, CMove) const = 0;
};

class IStabilityAnalyzer
{
public:
	virtual ~IStabilityAnalyzer() {}
	virtual uint64_t GetStableStones(const CPosition&) const = 0;
};

class IEvaluator
{
public:
	virtual ~IEvaluator() {}
	virtual float Eval(const CPosition&) const = 0;
};

class Engine : public IFlipper, public ILastFlipCounter, public IHashTable<CPosition, PvsInfo>, public IStabilityAnalyzer, public IEvaluator
{
	std::shared_ptr<IFlipper> flipper;
	std::shared_ptr<ILastFlipCounter> last_flip_counter;
	std::shared_ptr<IHashTable<CPosition, PvsInfo>> hash_table;
	std::shared_ptr<IStabilityAnalyzer> stability_analyzer;
	std::shared_ptr<IEvaluator> midgame_evaluator;
	std::shared_ptr<IEvaluator> game_over_evaluator;
public:
	Engine(
		std::shared_ptr<IFlipper> flipper = nullptr,
		std::shared_ptr<ILastFlipCounter> last_flip_counter = nullptr,
		std::shared_ptr<IHashTable<CPosition, PvsInfo>> hash_table = nullptr,
		std::shared_ptr<IStabilityAnalyzer> stability_analyzer = nullptr,
		std::shared_ptr<IEvaluator> midgame_evaluator = nullptr,
		std::shared_ptr<IEvaluator> game_over_evaluator = nullptr)
		: flipper(flipper)
		, last_flip_counter(last_flip_counter)
		, hash_table(hash_table)
		, stability_analyzer(stability_analyzer)
		, midgame_evaluator(midgame_evaluator)
		, game_over_evaluator(game_over_evaluator)
	{}

	CPosition Play(const CPosition& pos, CMove move) const override { return flipper->Play(pos, move); }
	uint8_t CountLastFlip(const CPosition& pos, CMove move) const override { return last_flip_counter->CountLastFlip(pos, move); }
	void Update(const CPosition& pos, const PvsInfo& info) override { hash_table->Update(pos, info); }
	std::pair<bool, PvsInfo> LookUp(const CPosition& pos) const override { return hash_table->LookUp(pos); }
	void Refresh(const CPosition& pos) override { hash_table->Refresh(pos); }
	void AdvanceDate() override { hash_table->AdvanceDate(); }
	void Clear() override { hash_table->Clear(); }
	uint64_t GetStableStones(const CPosition& pos) const override { return stability_analyzer->GetStableStones(pos); }
	float Eval(const CPosition& pos) const override { return midgame_evaluator->Eval(pos); }
};