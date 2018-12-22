#pragma once
#include <cstdint>

#include "Position.h"
#include "Search.h"
#include "Serializable.h"

namespace Puzzle
{
	static inline  int8_t DEFAULT_SCORE = -99;
	static inline  int8_t DEFAULT_DEPTH = -1;
	static inline uint8_t DEFAULT_SELECTIVITY = 0;
}

class IPuzzle
{
public:
	virtual const CPosition& GetPosition() const = 0;
	virtual std::unique_ptr<CPuzzle> Clone() const = 0;
	virtual std::unique_ptr<CPuzzle> Play(CMove move) const = 0;
	virtual bool Test() const = 0;
	virtual bool IsSolved() const = 0;
	virtual void Solve(Search::CAlgorithm&, Search::ILog&, Search::CSpecification) = 0;
	virtual void Solve(Search::CAlgorithm&, Search::ILog&) = 0;
	virtual void Solve(Search::CAlgorithm&) = 0;
	virtual void Reset() = 0;
};

class CPuzzle : public IPuzzle, public Serializable
{
public:
	CPuzzle(CPosition pos) : pos(pos) {}

	const CPosition& GetPosition() const override { return pos; }
	std::unique_ptr<CPuzzle> Clone() const override { return std::make_unique<CPuzzle>(*this); }
	std::unique_ptr<CPuzzle> Play(CMove move) const override { return std::make_unique<CPuzzle>(pos.Play(move)); }
	bool Test() const override { return (pos.GetP() & pos.GetO()) == 0; }
	bool IsSolved() const override { return false; }
	void Solve(Search::CAlgorithm&, Search::ILog&) override;
	void Solve(Search::CAlgorithm&, Search::ILog&, Search::CSpecification) override;
	void Solve(Search::CAlgorithm& alg) override { Search::CLogNull log; Solve(alg, log); }
	void Reset() override {}

	void Serialize(oArchive& arch) const override { arch.Serialize(*this); }

	friend bool operator==(const CPuzzle& lhs, const CPuzzle& rhs) { return (typeid(lhs) == typeid(rhs)) && lhs.isEqual(rhs); }
	friend bool operator!=(const CPuzzle& lhs, const CPuzzle& rhs) { return !(lhs == rhs); }

protected:
	CPosition pos;

	virtual bool isEqual(const CPuzzle& o) const { return pos == o.pos; }
};

class CPuzzleScore : public CPuzzle
{
public:
	int8_t score;

	CPuzzleScore(CPosition pos) : CPuzzleScore(pos, Puzzle::DEFAULT_SCORE) {}
	CPuzzleScore(CPosition pos, int8_t score) : CPuzzle(pos), score(score) {}

	std::unique_ptr<CPuzzle> Clone() const override { return std::make_unique<CPuzzleScore>(*this); }
	std::unique_ptr<CPuzzle> Play(CMove move) const override { return std::make_unique<CPuzzleScore>(pos.Play(move)); }
	bool Test() const override { return CPuzzle::Test() && (((score >= -64) && (score <= 64)) || (score == Puzzle::DEFAULT_SCORE)); }
	bool IsSolved() const override { return score != Puzzle::DEFAULT_SCORE; }
	void Solve(Search::CAlgorithm&, Search::ILog&) override;
	void Solve(Search::CAlgorithm&, Search::ILog&, Search::CSpecification) override;
	void Solve(Search::CAlgorithm& alg) override { Search::CLogNull log; Solve(alg, log); }
	void Reset() override { score = Puzzle::DEFAULT_SCORE; }

	void Serialize(oArchive& arch) const override { arch.Serialize(*this); }

protected:
	bool isEqual(const CPuzzle& o) const override;
};

class CPuzzleScoreDepth : public CPuzzleScore
{
public:
	int8_t depth;
	uint8_t selectivity;

	CPuzzleScoreDepth(CPosition pos) : CPuzzleScoreDepth(pos, Puzzle::DEFAULT_DEPTH, Puzzle::DEFAULT_SELECTIVITY) {}
	CPuzzleScoreDepth(CPosition pos, int8_t depth, uint8_t selectivity) : CPuzzleScore(pos), depth(depth), selectivity(selectivity) {}

	std::unique_ptr<CPuzzle> Clone() const override { return std::make_unique<CPuzzleScoreDepth>(*this); }
	std::unique_ptr<CPuzzle> Play(CMove move) const override { return std::make_unique<CPuzzleScoreDepth>(pos.Play(move)); }
	bool Test() const override { return CPuzzleScore::Test() && (depth >= Puzzle::DEFAULT_DEPTH); }
	bool IsSolved() const override { return (depth == static_cast<int8_t>(pos.EmptyCount())) && (selectivity == 0); }
	bool IsSolved(int8_t Depth, uint8_t Selectivity) const { return (depth >= Depth) || ((depth == Depth) && (selectivity <= Selectivity)); }
	void Solve(Search::CAlgorithm&, Search::ILog&) override;
	void Solve(Search::CAlgorithm&, Search::ILog&, Search::CSpecification) override;
	void Solve(Search::CAlgorithm& alg) override { Search::CLogNull log; Solve(alg, log); }

	void Serialize(oArchive& arch) const override { arch.Serialize(*this); }

protected:
	bool isEqual(const CPuzzle& o) const override;
};

class CPuzzleAllDepthScore : public CPuzzle
{
public:
	int8_t score[61]{};

	CPuzzleAllDepthScore(CPosition pos) : CPuzzle(pos) { Reset(); }

	std::unique_ptr<CPuzzle> Clone() const override { return std::make_unique<CPuzzleAllDepthScore>(*this); }
	std::unique_ptr<CPuzzle> Play(CMove move) const override { return std::make_unique<CPuzzleAllDepthScore>(pos.Play(move)); }
	bool Test() const override;
	bool IsSolved() const override;
	bool IsSolved(int8_t depth) const;
	void Solve(Search::CAlgorithm&, Search::ILog&) override;
	void Solve(Search::CAlgorithm&, Search::ILog&, Search::CSpecification) override;
	void Solve(Search::CAlgorithm& alg) override { Search::CLogNull log; Solve(alg, log); }

	void Reset() override { std::fill(std::begin(score), std::end(score), Puzzle::DEFAULT_SCORE); }

	int8_t MaxSolvedDepth() const;

	void Serialize(oArchive& arch) const override { arch.Serialize(*this); }

protected:
	bool isEqual(const CPuzzle& o) const override;
};

class CPuzzleAllMoveScore : public CPuzzle
{
public:
	int8_t score[64]{};

	CPuzzleAllMoveScore(CPosition pos) : CPuzzle(pos) { Reset(); }

	std::unique_ptr<CPuzzle> Clone() const override { return std::make_unique<CPuzzleAllMoveScore>(*this); }
	std::unique_ptr<CPuzzle> Play(CMove move) const override { return std::make_unique<CPuzzleAllMoveScore>(pos.Play(move)); }
	bool Test() const override;
	bool IsSolved() const override;
	void Solve(Search::CAlgorithm&, Search::ILog&) override;
	void Solve(Search::CAlgorithm&, Search::ILog&, Search::CSpecification) override; // TODO: Implement!
	void Solve(Search::CAlgorithm& alg) override { Search::CLogNull log; Solve(alg, log); }
	void Reset() override { std::fill(std::begin(score), std::end(score), Puzzle::DEFAULT_SCORE); }

	int8_t MaxScore() const { return *std::max_element(std::begin(score), std::end(score)); }

	void Serialize(oArchive& arch) const override { arch.Serialize(*this); }

protected:
	bool isEqual(const CPuzzle& o) const override;
};

std::unique_ptr<CPuzzleAllDepthScore> to_PuzzleAllDepthScore(const CPuzzle&);