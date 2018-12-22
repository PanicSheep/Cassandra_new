#pragma once
#include <istream>
#include <memory>

#include "Serializable.h"

namespace IO
{
	class SingleLineStreamArchive : public Archive
	{
		std::iostream& stream;
	public:
		SingleLineStreamArchive(std::iostream& stream) : stream(stream) {}

		void Serialize(const CPuzzle&) override;
		void Serialize(const CPuzzleScore&) override;
		void Serialize(const CPuzzleScoreDepth&) override;
		void Serialize(const CPuzzleAllDepthScore&) override;
		void Serialize(const CPuzzleAllMoveScore&) override;

		std::unique_ptr<CPuzzle> Deserialize() override;
	};
}