#pragma once
#include "Boards.h"
#include "ObjectSerializables.h"
#include "StreamSerializables.h"

namespace IO
{
	class BoardToStreamConverter : public BoardArchive
	{
		std::unique_ptr<StreamArchive> arch;
	public:
		BoardToStreamConverter(std::unique_ptr<StreamArchive> arch) : arch(std::move(arch)) {}
		virtual ~BoardToStreamConverter() {}

		void Serialize(const CBoard& obj) override;
		void Serialize(const CBoardScore& obj) override;
		void Serialize(const CBoardScoreDepth& obj) override;
		void Serialize(const CBoardAllDepthScore& obj) override;
		void Serialize(const CBoardAllMoveScore& obj) override;

		std::unique_ptr<CBoard> DeserializeBoard() override;
	};
}