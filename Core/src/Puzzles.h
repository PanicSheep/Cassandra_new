#pragma once
#include <cstdint>

#include "Position.h"
#include "Search.h"
#include "Serializable.h"

class CPuzzle : public Serializable
{
public:
	CPuzzle(CPosition pos) : pos(pos) {}

	const CPosition& GetPosition() const { return pos; }

	virtual std::unique_ptr<CPuzzle> Clone() const { return std::make_unique<CPuzzle>(*this); }
	virtual std::unique_ptr<CPuzzle> Play(CMove move) const { return std::make_unique<CPuzzle>(pos.Play(move)); }
	virtual bool Test() const { return (pos.GetP() & pos.GetO()) == 0; }
	virtual bool IsSolved() const { return false; }
	virtual void Solve(Search& search) { search.Eval(pos); }
	virtual void Reset() {}

	void Serialize(Archive& arch) const override { arch.Serialize(*this); }

	friend bool operator==(const CPuzzle& lhs, const CPuzzle& rhs) { return (typeid(lhs) == typeid(rhs)) && lhs.isEqual(rhs); }
	friend bool operator!=(const CPuzzle& lhs, const CPuzzle& rhs) { return !(lhs == rhs); }

protected:
	CPosition pos;

	virtual bool isEqual(const CPuzzle& o) const { return pos == o.pos; }
};

class CPuzzleScore : public CPuzzle
{
	static inline int8_t DEFAULT_SCORE = -99;
public:
	int8_t score;

	CPuzzleScore(CPosition pos) : CPuzzleScore(pos, DEFAULT_SCORE) {}
	CPuzzleScore(CPosition pos, int8_t score) : CPuzzle(pos), score(score) {}

	std::unique_ptr<CPuzzle> Clone() const override { return std::make_unique<CPuzzleScore>(*this); }
	std::unique_ptr<CPuzzle> Play(CMove move) const override { return std::make_unique<CPuzzleScore>(pos.Play(move)); }
	bool Test() const override { return CPuzzle::Test() && (((score >= -64) && (score <= 64)) || (score == DEFAULT_SCORE)); }
	bool IsSolved() const override { return score != DEFAULT_SCORE; }
	void Solve(Search&) override;
	void Reset() override { score = DEFAULT_SCORE; }

	void Serialize(Archive& arch) const override { arch.Serialize(*this); }

protected:
	bool isEqual(const CPuzzle& o) const override;
};

class CPuzzleScoreDepth : public CPuzzleScore
{
	static inline  int8_t DEFAULT_DEPTH = -1;
	static inline uint8_t DEFAULT_SELECTIVITY = 0;
public:
	int8_t depth;
	uint8_t selectivity;

	CPuzzleScoreDepth(CPosition pos) : CPuzzleScoreDepth(pos, DEFAULT_DEPTH, DEFAULT_SELECTIVITY) {}
	CPuzzleScoreDepth(CPosition pos, int8_t depth, uint8_t selectivity) : CPuzzleScore(pos), depth(depth), selectivity(selectivity) {}

	std::unique_ptr<CPuzzle> Clone() const override { return std::make_unique<CPuzzleScoreDepth>(*this); }
	std::unique_ptr<CPuzzle> Play(CMove move) const override { return std::make_unique<CPuzzleScoreDepth>(pos.Play(move)); }
	bool Test() const override { return CPuzzleScore::Test() && (depth >= DEFAULT_DEPTH); }
	bool IsSolved() const override { return (depth == static_cast<int8_t>(pos.EmptyCount())) && (selectivity == 0); }
	bool IsSolved(int8_t Depth, uint8_t Selectivity) const { return (depth >= Depth) || ((depth == Depth) && (selectivity <= Selectivity)); }
	void Solve(Search&) override;

	void Serialize(Archive& arch) const override { arch.Serialize(*this); }

protected:
	bool isEqual(const CPuzzle& o) const override;
};

class CPuzzleAllDepthScore : public CPuzzle
{
	static inline int8_t DEFAULT_SCORE = -99;
public:
	int8_t score[61];

	CPuzzleAllDepthScore(CPosition pos) : CPuzzle(pos) { Reset(); }

	std::unique_ptr<CPuzzle> Clone() const override { return std::make_unique<CPuzzleAllDepthScore>(*this); }
	std::unique_ptr<CPuzzle> Play(CMove move) const override { return std::make_unique<CPuzzleAllDepthScore>(pos.Play(move)); }
	bool Test() const override;
	bool IsSolved() const override;
	bool IsSolved(int8_t depth) const { return MaxSolvedDepth() >= depth; }
	void Solve(Search&) override;

	void Reset() override { std::fill(std::begin(score), std::end(score), DEFAULT_SCORE); }

	int8_t MaxSolvedDepth() const;

	void Serialize(Archive& arch) const override { arch.Serialize(*this); }

protected:
	bool isEqual(const CPuzzle& o) const override;
};

class CPuzzleAllMoveScore : public CPuzzle
{
	static inline int8_t DEFAULT_SCORE = -99;
public:
	int8_t score[64];

	CPuzzleAllMoveScore(CPosition pos) : CPuzzle(pos) { Reset(); }

	std::unique_ptr<CPuzzle> Clone() const override { return std::make_unique<CPuzzleAllMoveScore>(*this); }
	std::unique_ptr<CPuzzle> Play(CMove move) const override { return std::make_unique<CPuzzleAllMoveScore>(pos.Play(move)); }
	bool Test() const override;
	bool IsSolved() const override;
	void Solve(Search&) override;
	void Reset() override { std::fill(std::begin(score), std::end(score), DEFAULT_SCORE); }

	int8_t MaxScore() const { return *std::max_element(std::begin(score), std::end(score)); }

	void Serialize(Archive& arch) const override { arch.Serialize(*this); }

protected:
	bool isEqual(const CPuzzle& o) const override;
};

std::unique_ptr<CPuzzleAllDepthScore> to_PuzzleAllDepthScore(const CPuzzle&);