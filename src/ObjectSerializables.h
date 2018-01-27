#pragma once
#include <memory>
#include <cassert>
#include "Serializables.h"

class CPosition;
class CPositionScore;
class CPositionScoreDepth;
class CPositionFullScore;
class CPositionAllScore;
class iPositionArchive;
class oPositionArchive;

class ObjectSerializable : public virtual Serializable
{
public:
	        void Serialize(oArchive&) const override;
	virtual void Serialize(oPositionArchive&) const = 0;
};

class iPositionArchive
{
public:
	//iPositionArchive & operator>>(Deserializable& obj) override { dynamic_cast<CPosition&>(obj) = *DeserializePosition(); return *this; }
	virtual std::unique_ptr<CPosition> DeserializePosition() = 0;
};

class oPositionArchive : public oArchive
{
public:
	virtual void Serialize(const CPosition& obj) = 0;
	virtual void Serialize(const CPositionScore& obj) = 0;
	virtual void Serialize(const CPositionScoreDepth& obj) = 0;
	virtual void Serialize(const CPositionFullScore& obj) = 0;
	virtual void Serialize(const CPositionAllScore& obj) = 0;

	        oPositionArchive& operator<<(const Serializable& obj) override { return *this << dynamic_cast<const ObjectSerializable&>(obj); }
	virtual oPositionArchive& operator<<(const ObjectSerializable& obj) { obj.Serialize(*this); return *this; }
};

class PositionArchive : public iPositionArchive, public oPositionArchive
{};

inline void ObjectSerializable::Serialize(oArchive& arch) const { Serialize(dynamic_cast<PositionArchive&>(arch)); }
