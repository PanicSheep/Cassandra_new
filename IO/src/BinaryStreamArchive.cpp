#include "BinaryStreamArchive.h"

#include <string>
#include <cstdint>

#include "Utility.h"
#include "Puzzles.h"
#include "Position.h"

namespace IO
{
	std::iostream& operator>>(std::iostream& stream,  int8_t  & var) { stream.read(reinterpret_cast<char*>(&var), sizeof(var)); return stream; }
	std::iostream& operator>>(std::iostream& stream, uint8_t  & var) { stream.read(reinterpret_cast<char*>(&var), sizeof(var)); return stream; }
	std::iostream& operator>>(std::iostream& stream,  int16_t & var) { stream.read(reinterpret_cast<char*>(&var), sizeof(var)); return stream; }
	std::iostream& operator>>(std::iostream& stream, uint16_t & var) { stream.read(reinterpret_cast<char*>(&var), sizeof(var)); return stream; }
	std::iostream& operator>>(std::iostream& stream,  int32_t & var) { stream.read(reinterpret_cast<char*>(&var), sizeof(var)); return stream; }
	std::iostream& operator>>(std::iostream& stream, uint32_t & var) { stream.read(reinterpret_cast<char*>(&var), sizeof(var)); return stream; }
	std::iostream& operator>>(std::iostream& stream,  int64_t & var) { stream.read(reinterpret_cast<char*>(&var), sizeof(var)); return stream; }
	std::iostream& operator>>(std::iostream& stream, uint64_t & var) { stream.read(reinterpret_cast<char*>(&var), sizeof(var)); return stream; }
	std::iostream& operator>>(std::iostream& stream, float    & var) { stream.read(reinterpret_cast<char*>(&var), sizeof(var)); return stream; }
	std::iostream& operator>>(std::iostream& stream, double   & var) { stream.read(reinterpret_cast<char*>(&var), sizeof(var)); return stream; }

	std::iostream& operator<<(std::iostream& stream,  int8_t  var) { stream.write(reinterpret_cast<const char*>(&var), sizeof(var)); return stream; }
	std::iostream& operator<<(std::iostream& stream, uint8_t  var) { stream.write(reinterpret_cast<const char*>(&var), sizeof(var)); return stream; }
	std::iostream& operator<<(std::iostream& stream,  int16_t var) { stream.write(reinterpret_cast<const char*>(&var), sizeof(var)); return stream; }
	std::iostream& operator<<(std::iostream& stream, uint16_t var) { stream.write(reinterpret_cast<const char*>(&var), sizeof(var)); return stream; }
	std::iostream& operator<<(std::iostream& stream,  int32_t var) { stream.write(reinterpret_cast<const char*>(&var), sizeof(var)); return stream; }
	std::iostream& operator<<(std::iostream& stream, uint32_t var) { stream.write(reinterpret_cast<const char*>(&var), sizeof(var)); return stream; }
	std::iostream& operator<<(std::iostream& stream,  int64_t var) { stream.write(reinterpret_cast<const char*>(&var), sizeof(var)); return stream; }
	std::iostream& operator<<(std::iostream& stream, uint64_t var) { stream.write(reinterpret_cast<const char*>(&var), sizeof(var)); return stream; }
	std::iostream& operator<<(std::iostream& stream, float    var) { stream.write(reinterpret_cast<const char*>(&var), sizeof(var)); return stream; }
	std::iostream& operator<<(std::iostream& stream, double   var) { stream.write(reinterpret_cast<const char*>(&var), sizeof(var)); return stream; }

	static const uint8_t CPuzzle_classId = 1;
	static const uint8_t CPuzzleScore_classId = 2;
	static const uint8_t CPuzzleScoreDepth_classId = 3;
	static const uint8_t CPuzzleAllDepthScore_classId = 4;
	static const uint8_t CPuzzleAllMoveScore_classId = 5;

	void BinaryStreamArchive::Serialize(const CPuzzle& obj)
	{
		stream << CPuzzle_classId;
		stream << obj.GetPosition().GetP() << obj.GetPosition().GetO();
	}

	void BinaryStreamArchive::Serialize(const CPuzzleScore& obj)
	{
		stream << CPuzzleScore_classId;
		stream << obj.GetPosition().GetP() << obj.GetPosition().GetO();
		stream << obj.score;
	}

	void BinaryStreamArchive::Serialize(const CPuzzleScoreDepth& obj)
	{
		stream << CPuzzleScoreDepth_classId;
		stream << obj.GetPosition().GetP() << obj.GetPosition().GetO();
		stream << obj.score << obj.depth << obj.selectivity;
	}

	void BinaryStreamArchive::Serialize(const CPuzzleAllDepthScore& obj)
	{
		stream << CPuzzleAllDepthScore_classId;
		stream << obj.GetPosition().GetP() << obj.GetPosition().GetO();
		for (const auto& sc : obj.score)
			stream << sc;
	}

	void BinaryStreamArchive::Serialize(const CPuzzleAllMoveScore& obj)
	{
		stream << CPuzzleAllMoveScore_classId;
		stream << obj.GetPosition().GetP() << obj.GetPosition().GetO();
		for (const auto& sc : obj.score)
			stream << sc;
	}

	std::unique_ptr<CPuzzle> BinaryStreamArchive::Deserialize_CPuzzle()
	{
		uint64_t P, O;
		stream >> P >> O;
		return std::make_unique<CPuzzle>(CPosition(P, O));
	}

	std::unique_ptr<CPuzzle> BinaryStreamArchive::Deserialize_CPuzzleScore()
	{
		uint64_t P, O;
		int8_t score;
		stream >> P >> O >> score;
		return std::make_unique<CPuzzleScore>(CPosition(P, O), score);
	}

	std::unique_ptr<CPuzzle> BinaryStreamArchive::Deserialize_CPuzzleScoreDepth()
	{
		uint64_t P, O;
		int8_t depth;
		uint8_t selectivity;
		stream >> P >> O >> depth >> selectivity;
		return std::make_unique<CPuzzleScoreDepth>(CPosition(P, O), depth, selectivity);
	}

	std::unique_ptr<CPuzzle> BinaryStreamArchive::Deserialize_CPuzzleAllDepthScore()
	{
		uint64_t P, O;
		stream >> P >> O;
		auto obj = std::make_unique<CPuzzleAllDepthScore>(CPosition(P, O));
		for (auto& sc : obj->score)
			stream >> sc;
		return obj;
	}

	std::unique_ptr<CPuzzle> BinaryStreamArchive::Deserialize_CPuzzleAllMoveScore()
	{
		uint64_t P, O;
		stream >> P >> O;
		auto obj = std::make_unique<CPuzzleAllMoveScore>(CPosition(P, O));
		for (auto& sc : obj->score)
			stream >> sc;
		return obj;
	}


	std::unique_ptr<CPuzzle> BinaryStreamArchive::Deserialize()
	{
		if (!stream.good())
			return nullptr;
		if (stream.peek() == EOF)
			return nullptr;

		uint8_t classId;
		stream >> classId;
		switch (classId)
		{
		case CPuzzle_classId:
			return Deserialize_CPuzzle();
		case CPuzzleScore_classId:
			return Deserialize_CPuzzleScore();
		case CPuzzleScoreDepth_classId:
			return Deserialize_CPuzzleScoreDepth();
		case CPuzzleAllDepthScore_classId:
			return Deserialize_CPuzzleAllDepthScore();
		case CPuzzleAllMoveScore_classId:
			return Deserialize_CPuzzleAllMoveScore();
		default:
			throw std::runtime_error("Invalid class name");
		}
	}
}