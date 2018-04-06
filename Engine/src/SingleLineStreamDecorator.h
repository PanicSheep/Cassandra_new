#pragma once
#include <string>
#include <cstdint>
#include <memory>
#include <regex>
#include "StreamSerializables.h"
#include "ObjectSerializables.h"
#include "Boards.h"

class SingleLineStreamDecorator : public BoardArchive
{
	std::unique_ptr<StreamArchive> stream;
public:
	SingleLineStreamDecorator(std::unique_ptr<StreamArchive> stream) : stream(std::move(stream)) {}
	virtual ~SingleLineStreamDecorator() {}
	
	void Serialize(const CBoard& obj) override;
	void Serialize(const CBoardScore& obj) override;
	void Serialize(const CBoardScoreDepth& obj) override;
	void Serialize(const CBoardAllDepthScore& obj) override;
	void Serialize(const CBoardAllMoveScore& obj) override;

	std::unique_ptr<CBoard> DeserializeBoard() override;
};