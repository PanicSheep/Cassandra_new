#pragma once
#include <memory>

// Forward declarations
class Archive;
class CPuzzle;
class CPuzzleScore;
class CPuzzleScoreDepth;
class CPuzzleAllDepthScore;
class CPuzzleAllMoveScore;

class Serializable
{
public:
	virtual ~Serializable() {}
	virtual void Serialize(Archive&) const = 0;
};

// A generic archive to serialize and deserialize puzzles.
class Archive
{
public:
	virtual ~Archive() {}

	virtual void Serialize(const CPuzzle&) = 0;
	virtual void Serialize(const CPuzzleScore&) = 0;
	virtual void Serialize(const CPuzzleScoreDepth&) = 0;
	virtual void Serialize(const CPuzzleAllDepthScore&) = 0;
	virtual void Serialize(const CPuzzleAllMoveScore&) = 0;

	virtual std::unique_ptr<CPuzzle> Deserialize() = 0;

	Archive& operator<<(const Serializable& obj) { obj.Serialize(*this); return *this; }
};