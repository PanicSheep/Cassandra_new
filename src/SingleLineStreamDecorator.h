#pragma once
#include <string>
#include <cstdint>
#include <memory>
#include <regex>
#include "StreamSerializables.h"
#include "ObjectSerializables.h"
#include "position.h"

class SingleLineStreamDecorator : public PositionArchive
{
	StreamArchive& stream;
public:
	SingleLineStreamDecorator(StreamArchive& stream) : stream(stream) {}
	
	void Serialize(const CPosition& obj) override;
	void Serialize(const CPositionScore& obj) override;
	void Serialize(const CPositionScoreDepth& obj) override;
	void Serialize(const CPositionFullScore& obj) override;
	void Serialize(const CPositionAllScore& obj) override;

	std::unique_ptr<CPosition> DeserializePosition() override;
};