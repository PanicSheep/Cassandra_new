#pragma once
#include "Boards.h"
#include "ObjectSerializables.h"
#include "StreamSerializables.h"

class StreamConverter : public BoardArchive
{
	std::unique_ptr<StreamArchive> arch;
public:
	StreamConverter(std::unique_ptr<StreamArchive> arch) : arch(std::move(arch)) {}
	virtual ~StreamConverter() {}

	void Serialize(const CBoard& obj) override;
	void Serialize(const CBoardScore& obj) override;
	void Serialize(const CBoardScoreDepth& obj) override;
	void Serialize(const CBoardAllDepthScore& obj) override;
	void Serialize(const CBoardAllMoveScore& obj) override;

	std::unique_ptr<CBoard> DeserializeBoard() override;
};