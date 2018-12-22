#pragma once
#include "HashtablePVS.h"
#include "Moves.h"
#include "Position.h"
#include <memory>
#include <utility>
#include <utility>

class IFlipper
{
public:
	virtual ~IFlipper() = default;
	virtual CPosition Play(const CPosition&, CMove) const = 0;
};

class ILastFlipCounter
{
public:
	virtual ~ILastFlipCounter() = default;
	virtual uint8_t CountLastFlip(const CPosition&, CMove) const = 0;
};

class IStabilityAnalyzer
{
public:
	virtual ~IStabilityAnalyzer() = default;
	virtual uint64_t GetStableStones(const CPosition&) const = 0;
};

class IEvaluator
{
public:
	virtual ~IEvaluator() = default;
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
		: flipper(std::move(std::move(flipper)))
		, last_flip_counter(std::move(std::move(last_flip_counter)))
		, hash_table(std::move(std::move(hash_table)))
		, stability_analyzer(std::move(std::move(stability_analyzer)))
		, midgame_evaluator(std::move(std::move(midgame_evaluator)))
		, game_over_evaluator(std::move(std::move(game_over_evaluator)))
	{}

	CPosition Play(const CPosition& pos, CMove move) const override { return flipper->Play(pos, move); }
	uint8_t CountLastFlip(const CPosition& pos, CMove move) const override { return last_flip_counter->CountLastFlip(pos, move); }
	void Update(const CPosition& pos, const PvsInfo& info) override { hash_table->Update(pos, info); }
	std::optional<PvsInfo> LookUp(const CPosition& pos) const override { return hash_table->LookUp(pos); }
	void Refresh(const CPosition& pos) override { hash_table->Refresh(pos); }
	void AdvanceDate() override { hash_table->AdvanceDate(); }
	void Clear() override { hash_table->Clear(); }
	uint64_t GetStableStones(const CPosition& pos) const override { return stability_analyzer->GetStableStones(pos); }
	float Eval(const CPosition& pos) const override { return midgame_evaluator->Eval(pos); }
	float EvalGameOver(const CPosition& pos) const { return game_over_evaluator->Eval(pos); }
};