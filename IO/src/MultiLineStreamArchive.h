#pragma once
#include <memory>
#include <istream>

#include "Serializable.h"

namespace IO
{
	class MultiLineStreamArchive : public Archive
	{
		std::iostream& stream;
	public:
		MultiLineStreamArchive(std::iostream& stream) : stream(stream) {}

		void Serialize(const CPuzzle&) override;
		void Serialize(const CPuzzleScore&) override;
		void Serialize(const CPuzzleScoreDepth&) override;
		void Serialize(const CPuzzleAllDepthScore&) override;
		void Serialize(const CPuzzleAllMoveScore&) override;

		std::unique_ptr<CPuzzle> Deserialize() override { return nullptr; }
	};
}