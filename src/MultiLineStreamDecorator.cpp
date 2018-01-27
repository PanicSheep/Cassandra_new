#include "MultiLineStreamDecorator.h"

namespace
{
	std::string MultilineBoard(const CPosition& obj)
	{
		const uint64_t Moves = obj.PossibleMoves();
		std::string board =
			"  H G F E D C B A  \n"
			"8 - - - - - - - - 8\n"
			"7 - - - - - - - - 7\n"
			"6 - - - - - - - - 6\n"
			"5 - - - - - - - - 5\n"
			"4 - - - - - - - - 4\n"
			"3 - - - - - - - - 3\n"
			"2 - - - - - - - - 2\n"
			"1 - - - - - - - - 1\n"
			"  H G F E D C B A  ";

		for (unsigned int i = 0; i < 64; i++)
		{
			const bool P_bit = TestBit(obj.P, 63 - i);
			const bool O_bit = TestBit(obj.O, 63 - i);
			const bool M_bit = TestBit(Moves, 63 - i);
			auto& field = board[22 + 2 * i + 4 * (i / 8)];
			if (P_bit && O_bit) field = '@';
			else if (P_bit)     field = 'X';
			else if (O_bit)     field = 'O';
			else if (M_bit)     field = '+';
		}
		return board;
	}

	std::unique_ptr<CPosition> Parse_CPosition(const std::string& pos)
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
			else if (pos[i] == '#') { SetBit(P, 63 - i); SetBit(O, 63 - i); }
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
			else if (pos[i] == '#') { SetBit(P, 63 - i); SetBit(O, 63 - i); }
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

void MultiLineStreamDecorator::Serialize(const CPosition& obj)
{
	stream << MultilineBoard(obj) << "\n";
}

void MultiLineStreamDecorator::Serialize(const CPositionScore& obj)
{
	auto board = MultilineBoard(obj);
	board.insert(39, "  score: " + DoubleDigitSignedInt(obj.score));
	stream << board << "\n";
}

void MultiLineStreamDecorator::Serialize(const CPositionScoreDepth& obj)
{
	stream << MultilineBoard(obj) << "\n";
}

void MultiLineStreamDecorator::Serialize(const CPositionFullScore& obj)
{
	stream << MultilineBoard(obj) << "\n";
	//stream <<"\nscore:";
	//for (uint64_t i = 0; i <= EmptyCount(); i++)
	//	stream << " " << SignedInt(obj.score[i]);
}

void MultiLineStreamDecorator::Serialize(const CPositionAllScore& obj)
{
	stream << MultilineBoard(obj) << "\n";
}