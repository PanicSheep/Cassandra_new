#pragma once
#include <memory>

// Forward declarations
class oArchive;
class CPuzzle;
class CPuzzleScore;
class CPuzzleScoreDepth;
class CPuzzleAllDepthScore;
class CPuzzleAllMoveScore;

class Serializable
{
public:
	virtual ~Serializable() = default;
	virtual void Serialize(oArchive&) const = 0;
};

class iArchive
{
public:
	virtual ~iArchive() = default;
	virtual std::unique_ptr<CPuzzle> Deserialize() = 0;
};

class oArchive
{
public:
	virtual ~oArchive() = default;

	virtual void Serialize(const CPuzzle&) = 0;
	virtual void Serialize(const CPuzzleScore&) = 0;
	virtual void Serialize(const CPuzzleScoreDepth&) = 0;
	virtual void Serialize(const CPuzzleAllDepthScore&) = 0;
	virtual void Serialize(const CPuzzleAllMoveScore&) = 0;

	oArchive& operator<<(const Serializable& obj) { obj.Serialize(*this); return *this; }
};

// A generic archive to serialize and deserialize puzzles.
class Archive : public iArchive, public oArchive
{
public:
	~Archive() override = default;
};