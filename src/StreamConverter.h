#pragma once
#include "position.h"
#include "ObjectSerializables.h"
#include "StreamSerializables.h"

class StreamConverter : public PositionArchive
{
	StreamArchive& arch;
public:
	StreamConverter(StreamArchive& arch) : arch(arch) {}

	void Serialize(const CPosition& obj) override;
	void Serialize(const CPositionScore& obj) override;
	void Serialize(const CPositionScoreDepth& obj) override;
	void Serialize(const CPositionFullScore& obj) override;
	void Serialize(const CPositionAllScore& obj) override;

	std::unique_ptr<CPosition> DeserializePosition() override;
};