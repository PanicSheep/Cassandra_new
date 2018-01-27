#include "StreamConverter.h"

void StreamConverter::Serialize(const CPosition& obj) { arch << obj; }
void StreamConverter::Serialize(const CPositionScore& obj) { arch << obj; }
void StreamConverter::Serialize(const CPositionScoreDepth& obj) { arch << obj; }
void StreamConverter::Serialize(const CPositionFullScore& obj) { arch << obj; }
void StreamConverter::Serialize(const CPositionAllScore& obj) { arch << obj; }

std::unique_ptr<CPosition> StreamConverter::DeserializePosition()
{
	if (arch.Good() == false)
		return nullptr;

	uint8_t ClassId;
	arch >> ClassId;
	switch (ClassId) {
		case CPosition::classId: return std::make_unique<CPosition>(arch);
		case CPositionScore::classId: return std::make_unique<CPositionScore>(arch);
		case CPositionScoreDepth::classId: return std::make_unique<CPositionScoreDepth>(arch);
		case CPositionFullScore::classId: return std::make_unique<CPositionFullScore>(arch);
		case CPositionAllScore::classId: return std::make_unique<CPositionAllScore>(arch);
		default: throw std::runtime_error("Invalid class name");
	}
}