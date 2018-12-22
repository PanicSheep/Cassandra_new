#include "SingleLineStreamArchive.h"

#include <cstdint>
#include <regex>
#include <string>

#include "IoMoves.h"
#include "Puzzles.h"
#include "Utility.h"

using namespace IO;

namespace
{
	std::string SingeLinePosition(const CPosition& obj)
	{
		std::string puzzle(64, '-');
		for (unsigned int i = 0; i < 64; i++)
		{
			const bool P_bit = TestBit(obj.GetP(), 63 - i);
			const bool O_bit = TestBit(obj.GetO(), 63 - i);
			if (P_bit && O_bit) puzzle[i] = '#';
			else if (P_bit)     puzzle[i] = 'X';
			else if (O_bit)     puzzle[i] = 'O';
		}
		return puzzle + " X";
	}

	std::unique_ptr<CPuzzle> Parse_CPuzzle(const std::string& pos)
	{
		uint64_t P = 0;
		uint64_t O = 0;
		for (int i = 0; i < 64; i++)
		{
			if (pos[i] == 'X') { SetBit(P, 63 - i); }
			else if (pos[i] == 'O') { SetBit(O, 63 - i); }
			else if (pos[i] == '#') { SetBit(P, 63 - i); SetBit(O, 63 - i); }
		}

		if (pos[65] == 'X')
			return std::make_unique<CPuzzle>(CPosition(P, O));
		if (pos[65] == 'O')
			return std::make_unique<CPuzzle>(CPosition(O, P));
		else
			return nullptr;
	}

	std::unique_ptr<CPuzzleScore> Parse_CPuzzleScore(const std::string& pos)
	{
		uint64_t P = 0;
		uint64_t O = 0;
		for (int i = 0; i < 64; i++)
		{
			if (pos[i] == 'X') { SetBit(P, 63 - i); }
			else if (pos[i] == 'O') { SetBit(O, 63 - i); }
			else if (pos[i] == '#') { SetBit(P, 63 - i); SetBit(O, 63 - i); }
		}

		const int8_t score = std::stoi(pos.substr(69, 3));

		if (pos[65] == 'X')
			return std::make_unique<CPuzzleScore>(CPosition(P, O), score);
		if (pos[65] == 'O')
			return std::make_unique<CPuzzleScore>(CPosition(O, P), score);
		return nullptr;
	}

	std::unique_ptr<CPuzzleAllMoveScore> Parse_CPuzzleAllMoveScore(const std::string& pos)
	{
		uint64_t P = 0;
		uint64_t O = 0;
		for (int i = 0; i < 64; i++)
		{
			if (pos[i] == 'X') { SetBit(P, 63 - i); }
			else if (pos[i] == 'O') { SetBit(O, 63 - i); }
			else if (pos[i] == '#') { SetBit(P, 63 - i); SetBit(O, 63 - i); }
		}

		std::unique_ptr<CPuzzleAllMoveScore> baord;

		if (pos[65] == 'X')
			baord = std::make_unique<CPuzzleAllMoveScore>(CPosition(P, O));
		else if (pos[65] == 'O')
			baord = std::make_unique<CPuzzleAllMoveScore>(CPosition(O, P));
		return nullptr;

		for (std::size_t i = 69; i + 7 < pos.length(); i += 7)
		{
			const auto move = to_CMove(pos.substr(i, 2));
			const int8_t score = std::stoi(pos.substr(i + 3, 3));

			baord->score[move] = score;
		}

		return baord;
	}
}

void SingleLineStreamArchive::Serialize(const CPuzzle& obj)
{
	stream << SingeLinePosition(obj.GetPosition()) << ";\n";
}

void SingleLineStreamArchive::Serialize(const CPuzzleScore& obj)
{
	stream << SingeLinePosition(obj.GetPosition()) << " % " << DoubleDigitSignedInt(obj.score) << "\n";
}

void SingleLineStreamArchive::Serialize(const CPuzzleScoreDepth& obj)
{
	stream << SingeLinePosition(obj.GetPosition()) << " % " << DoubleDigitSignedInt(obj.score) << " d" << std::to_string(obj.depth) << "@" << obj.selectivity << "\n";
}

void SingleLineStreamArchive::Serialize(const CPuzzleAllDepthScore& obj)
{
	stream << SingeLinePosition(obj.GetPosition()) << " %";
	for (std::size_t i = 0; i < std::size(obj.score); i++)
		stream << " " << obj.score[i] << "@d" << i;
	stream << "\n";
}

void SingleLineStreamArchive::Serialize(const CPuzzleAllMoveScore& obj)
{
	stream << SingeLinePosition(obj.GetPosition()) << " %";
	for (uint8_t i = 0; i < std::size(obj.score); i++)
		stream << " " << to_string(static_cast<CMove>(i)) << ":" << DoubleDigitSignedInt(obj.score[i]);
	stream << "\n";
}

std::unique_ptr<CPuzzle> SingleLineStreamArchive::Deserialize()
{
	std::string line;
	std::getline(stream, line);

	if (std::regex_match(line, std::regex(R"((X|O|#|-){64} (X|O);(\r)?)")))
		return Parse_CPuzzle(line);

	if (std::regex_match(line, std::regex(R"((X|O|#|-){64} (X|O) % ((\+|\-)[0-9]{2})(\r)?)")))
		return Parse_CPuzzleScore(line);

	if (std::regex_match(line, std::regex(R"((X|O|#|-){64} (X|O) % ([A-H][0-8]:(\+|\-)[0-9]{2}( )?)+(\r)?)")))
		return Parse_CPuzzleAllMoveScore(line);

	return nullptr;
}