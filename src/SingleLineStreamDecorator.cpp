#include "SingleLineStreamDecorator.h"

namespace
{
	std::string SingeLineBoard(const CPosition& obj)
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

	std::unique_ptr<CPosition> Parse_CPosition(const std::string& pos)
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
			return std::make_unique<CPosition>(P, O);
		else if (pos[65] == 'O')
			return std::make_unique<CPosition>(O, P);
		else
			return nullptr;
	}

	std::unique_ptr<CPositionScore> Parse_CPositionScore(const std::string& pos)
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
			return std::make_unique<CPositionScore>(P, O, score);
		else if (pos[65] == 'O')
			return std::make_unique<CPositionScore>(O, P, -score);
		else
			return nullptr;
	}

	std::unique_ptr<CPositionAllScore> Parse_CPositionAllScore(const std::string& pos)
	{
		uint64_t P = 0;
		uint64_t O = 0;
		for (int i = 0; i < 64; i++)
		{
			if (pos[i] == 'X') { SetBit(P, 63 - i); }
			else if (pos[i] == 'O') { SetBit(O, 63 - i); }
			else if (pos[i] == '@') { SetBit(P, 63 - i); SetBit(O, 63 - i); }
		}

		std::unique_ptr<CPositionAllScore> position;

		if (pos[65] == 'X')
			position = std::make_unique<CPositionAllScore>(P, O);
		else if (pos[65] == 'O')
			position = std::make_unique<CPositionAllScore>(O, P);
		else
			return nullptr;

		for (std::size_t i = 69; i + 7 < pos.length(); i += 7)
		{
			const auto move = Move(pos.substr(i, 2));
			const int8_t score = std::stoi(pos.substr(i + 3, 3));

			position->score[move] = score;
		}

		return position;
	}
}

void SingleLineStreamDecorator::Serialize(const CPosition& obj)
{
	stream << SingeLineBoard(obj) << ";\n";
}

void SingleLineStreamDecorator::Serialize(const CPositionScore& obj)
{
	stream << SingeLineBoard(obj) << " % " << DoubleDigitSignedInt(obj.score) << "\n";
}

void SingleLineStreamDecorator::Serialize(const CPositionScoreDepth& obj)
{
	stream << SingeLineBoard(obj) << " % " << DoubleDigitSignedInt(obj.score) << " d" << std::to_string(obj.depth) << "@" << obj.selectivity << "\n";
}

void SingleLineStreamDecorator::Serialize(const CPositionFullScore& obj)
{
	stream << SingeLineBoard(obj) << " %";
	for (std::size_t i = 0; i < std::size(obj.score); i++)
		stream << " " << obj.score[i] << "@d" << i;
	stream << "\n";
}

void SingleLineStreamDecorator::Serialize(const CPositionAllScore& obj)
{
	stream << SingeLineBoard(obj) << " %";
	for (uint8_t i = 0; i < std::size(obj.score); i++)
		stream << " " << Move(i) << ":" << DoubleDigitSignedInt(obj.score[i]);
	stream << "\n";
}

std::unique_ptr<CPosition> SingleLineStreamDecorator::DeserializePosition()
{
	std::string line;
	std::getline(stream, line);

	if (std::regex_match(line, std::regex(R"((X|O|-){64} (X|O);(\r)?)")))
		return Parse_CPosition(line);

	if (std::regex_match(line, std::regex(R"((X|O|-){64} (X|O) % ((\+|\-)[0-9]{2})(\r)?)")))
		return Parse_CPositionScore(line);

	if (std::regex_match(line, std::regex(R"((X|O|-){64} (X|O) % ([A-H][0-8]:(\+|\-)[0-9]{2}( )?)+(\r)?)")))
		return Parse_CPositionAllScore(line);

	return nullptr;
}