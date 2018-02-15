#pragma once
#include <algorithm>
#include <cstdint>
#include "Position.h"
#include "Path.h"
#include "StreamSerializables.h"
#include "ObjectSerializables.h"

class CBoard : public ObjectSerializable, public Streamable
{
public:
	static const uint8_t classId = 1;

	CBoard(CPosition pos) : pos(pos) {}
	CBoard(iStreamArchive& arch) { deserialize(arch); }

	const CPosition& GetPosition() const { return pos; }
	      CPosition& GetPosition()       { return pos; }

	virtual uint8_t ClassId() const { return classId; }
	virtual CBoard* Clone() const { return new CBoard(*this); }
	virtual CBoard* Play(const CMove& move) const { return new CBoard(pos.Play(move)); }
	virtual bool Test() const { return (pos.GetP() & pos.GetO()) == 0; }
	virtual bool IsSolved() const { return false; }

	void Serialize(oArchive&) const override { throw std::logic_error("Archive not supported"); } // TODO: Get rid of this!
	void Serialize(oStreamArchive& arch) const override { arch << classId; serialize(arch); }
	void Serialize(oBoardArchive& arch) const override { arch.Serialize(*this); }
	void Deserialize(iStreamArchive& arch) override { deserialize(arch); }

protected:
	CPosition pos;

	void serialize(oStreamArchive& arch) const { arch << pos.GetP() << pos.GetO(); }
	void deserialize(iStreamArchive& arch) { uint64_t P, O; arch >> P >> O; pos = CPosition(P, O); }
};

class CBoardScore : public CBoard
{
	static const int8_t DEFAULT_SCORE = -99;
public:
	static const uint8_t classId = 2;
	int8_t score;

	CBoardScore(CPosition pos) : CBoardScore(pos, DEFAULT_SCORE) {}
	CBoardScore(CPosition pos, int8_t score) : CBoard(pos), score(score) {}
	CBoardScore(iStreamArchive& arch) : CBoard(arch) { deserialize(arch); }

	virtual uint8_t ClassId() const override { return classId; }
	virtual CBoardScore* Clone() const override { return new CBoardScore(*this); }
	virtual CBoardScore* Play(const CMove& move) const override { return new CBoardScore(pos.Play(move)); }
	virtual bool Test() const override { return CBoard::Test() && (((score >= -64) && (score <= 64)) || (score == DEFAULT_SCORE)); }
	virtual bool IsSolved() const override { return score != DEFAULT_SCORE; }

	void Serialize(oStreamArchive& arch) const override { arch << classId; serialize(arch); }
	void Serialize(oBoardArchive& arch) const override { arch.Serialize(*this); }
	void Deserialize(iStreamArchive& arch) override { deserialize(arch); }

protected:
	void serialize(oStreamArchive& arch) const { CBoard::serialize(arch); arch << score; }
	void deserialize(iStreamArchive& arch) { arch >> score; }
};

class CBoardScoreDepth : public CBoardScore
{
	static const  int8_t DEFAULT_DEPTH = -1;
	static const uint8_t DEFAULT_SELECTIVITY = 0;
public:
	static const uint8_t classId = 3;
	int8_t depth;
	uint8_t selectivity;

	CBoardScoreDepth(CPosition pos) : CBoardScore(pos), depth(DEFAULT_DEPTH), selectivity(DEFAULT_SELECTIVITY) {}
	CBoardScoreDepth(iStreamArchive& arch) : CBoardScore(arch) { deserialize(arch); }

	virtual uint8_t ClassId() const override { return classId; }
	virtual CBoardScoreDepth* Clone() const override { return new CBoardScoreDepth(*this); }
	virtual CBoardScoreDepth* Play(const CMove& move) const override { return new CBoardScoreDepth(pos.Play(move)); }
	virtual bool Test() const override { return CBoardScore::Test() && (depth >= DEFAULT_DEPTH); }
	virtual bool IsSolved() const override { return (depth == static_cast<int8_t>(pos.EmptyCount())) && (selectivity == 0); }
	virtual bool IsSolved(int8_t Depth, uint8_t Selectivity) const { return (depth >= Depth) || ((depth == Depth) && (selectivity <= Selectivity)); }

	void Serialize(oStreamArchive& arch) const override { arch << classId; serialize(arch); }
	void Serialize(oBoardArchive& arch) const override { arch.Serialize(*this); }
	void Deserialize(iStreamArchive& arch) override { deserialize(arch); }

protected:
	void serialize(oStreamArchive& arch) const { CBoardScore::serialize(arch); arch << depth << selectivity; }
	void deserialize(iStreamArchive& arch) { arch >> depth >> selectivity; }
};

class CBoardAllDepthScore : public CBoard
{
	static const int8_t DEFAULT_SCORE = -99;
public:
	static const uint8_t classId = 4;
	int8_t score[61];

	CBoardAllDepthScore(CPosition pos) : CBoard(pos) { ResetInformation(); }
	CBoardAllDepthScore(iStreamArchive& arch) : CBoard(arch) { deserialize(arch); }

	virtual uint8_t ClassId() const override { return classId; }
	virtual CBoardAllDepthScore* Clone() const override { return new CBoardAllDepthScore(*this); }
	virtual CBoardAllDepthScore* Play(const CMove& move) const override { return new CBoardAllDepthScore(pos.Play(move)); }
	virtual bool Test() const override;
	virtual bool IsSolved() const override;
	virtual bool IsSolved(int8_t depth) const { return MaxSolvedDepth() >= depth; }

	virtual void ResetInformation() { std::fill(std::begin(score), std::end(score), DEFAULT_SCORE); }

	int8_t MaxSolvedDepth() const;

	void Serialize(oStreamArchive& arch) const override { arch << classId; serialize(arch); }
	void Serialize(oBoardArchive& arch) const override { arch.Serialize(*this); }
	void Deserialize(iStreamArchive& arch) override { deserialize(arch); }

protected:
	void serialize(oStreamArchive& arch) const { CBoard::serialize(arch); for (const auto& it : score) arch << it; }
	void deserialize(iStreamArchive& arch) { for (auto& it : score) arch >> it; }
};

class CBoardAllMoveScore : public CBoard
{
	static const int8_t DEFAULT_SCORE = -99;
public:
	static const uint8_t classId = 5;
	int8_t score[64];

	CBoardAllMoveScore(CPosition pos) : CBoard(pos) { ResetInformation(); }
	CBoardAllMoveScore(iStreamArchive& arch) : CBoard(arch) { deserialize(arch); }

	virtual uint8_t ClassId() const override { return classId; }
	virtual CBoardAllMoveScore* Clone() const override { return new CBoardAllMoveScore(*this); }
	virtual CBoardAllMoveScore* Play(const CMove& move) const override { return new CBoardAllMoveScore(pos.Play(move)); }
	virtual void ResetInformation() { std::fill(std::begin(score), std::end(score), DEFAULT_SCORE); }
	virtual bool Test() const override;
	virtual bool IsSolved() const override;

	int8_t MaxScore() const { return *std::max_element(std::begin(score), std::end(score)); }

	void Serialize(oStreamArchive& arch) const override { arch << classId; serialize(arch); }
	void Serialize(oBoardArchive& arch) const override { arch.Serialize(*this); }
	void Deserialize(iStreamArchive& arch) override { deserialize(arch); }

protected:
	void serialize(oStreamArchive& arch) const { CBoard::serialize(arch); for (const auto& it : score) arch << it; }
	void deserialize(iStreamArchive& arch) { for (auto& it : score) arch >> it; }
};