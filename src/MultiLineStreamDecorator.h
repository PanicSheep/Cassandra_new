#pragma once
#include <string>
#include <cstdint>
#include <memory>
#include <regex>
#include "StreamSerializables.h"
#include "ObjectSerializables.h"
#include "position.h"

class MultiLineStreamDecorator : public oPositionArchive
{
	oStreamArchive& stream;
public:
	MultiLineStreamDecorator(oStreamArchive& stream) : stream(stream) {}
	
	void Serialize(const CPosition& obj) override;
	void Serialize(const CPositionScore& obj) override;
	void Serialize(const CPositionScoreDepth& obj) override;
	void Serialize(const CPositionFullScore& obj) override;
	void Serialize(const CPositionAllScore& obj) override;
};