#pragma once
#include <string>
#include <cstdint>
#include <memory>
#include <regex>
#include "StreamSerializables.h"
#include "ObjectSerializables.h"
#include "Boards.h"

class MultiLineStreamDecorator : public oBoardArchive
{
	oStreamArchive& stream;
public:
	MultiLineStreamDecorator(oStreamArchive& stream) : stream(stream) {}

	void Serialize(const CBoard& obj) override;
	void Serialize(const CBoardScore& obj) override;
	void Serialize(const CBoardScoreDepth& obj) override;
	void Serialize(const CBoardAllDepthScore& obj) override;
	void Serialize(const CBoardAllMoveScore& obj) override;
};