#pragma once
#include <cstdint>
#include <istream>
#include <memory>

#include "Puzzles.h"
#include "Serializable.h"

namespace IO
{
	class BinaryStreamArchive : public Archive
	{
		std::iostream& stream;
	public:
		BinaryStreamArchive(std::iostream& stream) : stream(stream) {}

		void Serialize(const CPuzzle&) override;
		void Serialize(const CPuzzleScore&) override;
		void Serialize(const CPuzzleScoreDepth&) override;
		void Serialize(const CPuzzleAllDepthScore&) override;
		void Serialize(const CPuzzleAllMoveScore&) override;

		std::unique_ptr<CPuzzle> Deserialize() override;
	private:
		std::unique_ptr<CPuzzle> Deserialize_CPuzzle();
		std::unique_ptr<CPuzzle> Deserialize_CPuzzleScore();
		std::unique_ptr<CPuzzle> Deserialize_CPuzzleScoreDepth();
		std::unique_ptr<CPuzzle> Deserialize_CPuzzleAllDepthScore();
		std::unique_ptr<CPuzzle> Deserialize_CPuzzleAllMoveScore();
	};
}