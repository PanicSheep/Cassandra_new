#pragma once
#include <memory>
#include <cassert>
#include "Serializables.h"

namespace IO
{
	class CBoard;
	class CBoardScore;
	class CBoardScoreDepth;
	class CBoardAllDepthScore;
	class CBoardAllMoveScore;

	class iBoardArchive;
	class oBoardArchive;

	class ObjectSerializable : public virtual Serializable
	{
	public:
		void Serialize(oArchive&) const override;
		virtual void Serialize(oBoardArchive&) const = 0;
	};

	class iBoardArchive
	{
	public:
		virtual ~iBoardArchive() {}
		//iBoardArchive & operator>>(Deserializable& obj) override { dynamic_cast<CPosition&>(obj) = *DeserializeBoard(); return *this; }
		virtual std::unique_ptr<CBoard> DeserializeBoard() = 0;
	};

	class oBoardArchive : public oArchive
	{
	public:
		virtual ~oBoardArchive() {}

		virtual void Serialize(const CBoard& obj) = 0;
		virtual void Serialize(const CBoardScore& obj) = 0;
		virtual void Serialize(const CBoardScoreDepth& obj) = 0;
		virtual void Serialize(const CBoardAllDepthScore& obj) = 0;
		virtual void Serialize(const CBoardAllMoveScore& obj) = 0;

		oBoardArchive& operator<<(const Serializable& obj) override { return *this << dynamic_cast<const ObjectSerializable&>(obj); }
		virtual oBoardArchive& operator<<(const ObjectSerializable& obj) { obj.Serialize(*this); return *this; }
	};

	class BoardArchive : public iBoardArchive, public oBoardArchive
	{};

	inline void ObjectSerializable::Serialize(oArchive& arch) const { Serialize(dynamic_cast<BoardArchive&>(arch)); }
}