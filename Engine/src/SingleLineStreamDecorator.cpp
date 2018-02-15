#include "SingleLineStreamDecorator.h"
#include "Utility.h"

namespace
{
	std::string Move(const uint8_t move)
	{
		assert(move <= 64);
		static const std::string name[65] = {
			"A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1",
			"A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2",
			"A3", "B3", "C3", "D3", "E3", "F3", "G3", "H3",
			"A4", "B4", "C4", "D4", "E4", "F4", "G4", "H4",
			"A5", "B5", "C5", "D5", "E5", "F5", "G5", "H5",
			"A6", "B6", "C6", "D6", "E6", "F6", "G6", "H6",
			"A7", "B7", "C7", "D7", "E7", "F7", "G7", "H7",
			"A8", "B8", "C8", "D8", "E8", "F8", "G8", "H8", "--"
		};
		return name[move];
	}

	uint8_t Move(const std::string& field_name)
	{
		if (field_name == "--")
			return 64;

		auto letter = field_name.substr(0, 1);
		auto number = field_name.substr(1, 1);

		uint8_t letter_index;
		if (letter == "A") letter_index = 0;
		else if (letter == "B") letter_index = 1;
		else if (letter == "C") letter_index = 2;
		else if (letter == "D") letter_index = 3;
		else if (letter == "E") letter_index = 4;
		else if (letter == "F") letter_index = 5;
		else if (letter == "G") letter_index = 6;
		else if (letter == "H") letter_index = 7;

		uint8_t number_index = std::stoi(number) - 1;

		return number_index * 8 + letter_index;
	}

	std::string SingeLinePosition(const CPosition& obj)
	{
		std::string board(64, '-');
		for (unsigned int i = 0; i < 64; i++)
		{
			const bool P_bit = TestBit(obj.P, 63 - i);
			const bool O_bit = TestBit(obj.O, 63 - i);
			if (P_bit && O_bit) board[i] = '#';
			else if (P_bit)     board[i] = 'X';
			else if (O_bit)     board[i] = 'O';
		}
		return board + " X";
	}

	std::unique_ptr<CBoard> Parse_CBoard(const std::string& pos)
	{
		uint64_t P = 0;
		uint64_t O = 0;
		for (int i = 0; i < 64; i++)
		{
			if (pos[i] == 'X') { SetBit(P, 63 - i); }
			else if (pos[i] == 'O') { SetBit(O, 63 - i); }
			else if (pos[i] == '@') { SetBit(P, 63 - i); SetBit(O, 63 - i); }
		}

		if (pos[65] == 'X')
			return std::make_unique<CBoard>(CPosition(P, O));
		else if (pos[65] == 'O')
			return std::make_unique<CBoard>(CPosition(O, P));
		else
			return nullptr;
	}

	std::unique_ptr<CBoardScore> Parse_CBoardScore(const std::string& pos)
	{
		uint64_t P = 0;
		uint64_t O = 0;
		for (int i = 0; i < 64; i++)
		{
			if (pos[i] == 'X') { SetBit(P, 63 - i); }
			else if (pos[i] == 'O') { SetBit(O, 63 - i); }
			else if (pos[i] == '@') { SetBit(P, 63 - i); SetBit(O, 63 - i); }
		}

		const int8_t score = std::stoi(pos.substr(69, 3));

		if (pos[65] == 'X')
			return std::make_unique<CBoardScore>(CPosition(P, O), score);
		else if (pos[65] == 'O')
			return std::make_unique<CBoardScore>(CPosition(O, P), -score);
		else
			return nullptr;
	}

	std::unique_ptr<CBoardAllMoveScore> Parse_CBoardAllMoveScore(const std::string& pos)
	{
		uint64_t P = 0;
		uint64_t O = 0;
		for (int i = 0; i < 64; i++)
		{
			if (pos[i] == 'X') { SetBit(P, 63 - i); }
			else if (pos[i] == 'O') { SetBit(O, 63 - i); }
			else if (pos[i] == '@') { SetBit(P, 63 - i); SetBit(O, 63 - i); }
		}

		std::unique_ptr<CBoardAllMoveScore> baord;

		if (pos[65] == 'X')
			baord = std::make_unique<CBoardAllMoveScore>(CPosition(P, O));
		else if (pos[65] == 'O')
			baord = std::make_unique<CBoardAllMoveScore>(CPosition(O, P));
		else
			return nullptr;

		for (std::size_t i = 69; i + 7 < pos.length(); i += 7)
		{
			const auto move = Move(pos.substr(i, 2));
			const int8_t score = std::stoi(pos.substr(i + 3, 3));

			baord->score[move] = score;
		}

		return baord;
	}
}

void SingleLineStreamDecorator::Serialize(const CBoard& obj)
{
	*stream << SingeLinePosition(obj.GetPosition()) << ";\n";
}

void SingleLineStreamDecorator::Serialize(const CBoardScore& obj)
{
	*stream << SingeLinePosition(obj.GetPosition()) << " % " << DoubleDigitSignedInt(obj.score) << "\n";
}

void SingleLineStreamDecorator::Serialize(const CBoardScoreDepth& obj)
{
	*stream << SingeLinePosition(obj.GetPosition()) << " % " << DoubleDigitSignedInt(obj.score) << " d" << std::to_string(obj.depth) << "@" << obj.selectivity << "\n";
}

void SingleLineStreamDecorator::Serialize(const CBoardAllDepthScore& obj)
{
	*stream << SingeLinePosition(obj.GetPosition()) << " %";
	for (std::size_t i = 0; i < std::size(obj.score); i++)
		*stream << " " << obj.score[i] << "@d" << i;
	*stream << "\n";
}

void SingleLineStreamDecorator::Serialize(const CBoardAllMoveScore& obj)
{
	*stream << SingeLinePosition(obj.GetPosition()) << " %";
	for (uint8_t i = 0; i < std::size(obj.score); i++)
		*stream << " " << Move(i) << ":" << DoubleDigitSignedInt(obj.score[i]);
	*stream << "\n";
}

std::unique_ptr<CBoard> SingleLineStreamDecorator::DeserializeBoard()
{
	std::string line;
	std::getline(*stream, line);

	if (std::regex_match(line, std::regex(R"((X|O|-){64} (X|O);(\r)?)")))
		return Parse_CBoard(line);

	if (std::regex_match(line, std::regex(R"((X|O|-){64} (X|O) % ((\+|\-)[0-9]{2})(\r)?)")))
		return Parse_CBoardScore(line);

	if (std::regex_match(line, std::regex(R"((X|O|-){64} (X|O) % ([A-H][0-8]:(\+|\-)[0-9]{2}( )?)+(\r)?)")))
		return Parse_CBoardAllMoveScore(line);

	return nullptr;
}