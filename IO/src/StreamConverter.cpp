#include "StreamConverter.h"

namespace IO
{
	void BoardToStreamConverter::Serialize(const CBoard& obj) { *arch << obj; }
	void BoardToStreamConverter::Serialize(const CBoardScore& obj) { *arch << obj; }
	void BoardToStreamConverter::Serialize(const CBoardScoreDepth& obj) { *arch << obj; }
	void BoardToStreamConverter::Serialize(const CBoardAllDepthScore& obj) { *arch << obj; }
	void BoardToStreamConverter::Serialize(const CBoardAllMoveScore& obj) { *arch << obj; }

	std::unique_ptr<CBoard> BoardToStreamConverter::DeserializeBoard()
	{
		if (arch->Good() == false)
			return nullptr;

		uint8_t ClassId;
		*arch >> ClassId;
		switch (ClassId) {
		case CBoard::classId: return std::make_unique<CBoard>(*arch);
		case CBoardScore::classId: return std::make_unique<CBoardScore>(*arch);
		case CBoardScoreDepth::classId: return std::make_unique<CBoardScoreDepth>(*arch);
		case CBoardAllDepthScore::classId: return std::make_unique<CBoardAllDepthScore>(*arch);
		case CBoardAllMoveScore::classId: return std::make_unique<CBoardAllMoveScore>(*arch);
		default: throw std::runtime_error("Invalid class name");
		}
	}
}