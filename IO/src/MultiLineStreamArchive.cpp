#include "MultiLineStreamArchive.h"

#include <regex>
#include <string>
#include <cstdint>

#include "Utility.h"
#include "Puzzles.h"

namespace IO
{
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

		std::string MultilinePosition(const CPosition& obj)
		{
			const CMoves Moves = obj.PossibleMoves();
			std::string puzzle =
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
				const bool isP = TestBit(obj.GetP(), 63 - i);
				const bool isO = TestBit(obj.GetO(), 63 - i);
				const bool isM = Moves.HasMove(63 - i);
				auto& field = puzzle[22 + 2 * i + 4 * (i / 8)];
				if (isP && isO) field = '@';
				else if (isP)   field = 'X';
				else if (isO)   field = 'O';
				else if (isM)   field = '+';
			}
			return puzzle;
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
			else if (pos[65] == 'O')
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
			else if (pos[65] == 'O')
				return std::make_unique<CPuzzleScore>(CPosition(O, P), -score);
			else
				return nullptr;
		}

		std::unique_ptr<CPuzzleAllDepthScore> Parse_CPuzzleAllDepthScore(const std::string& pos)
		{
			uint64_t P = 0;
			uint64_t O = 0;
			for (int i = 0; i < 64; i++)
			{
				if (pos[i] == 'X') { SetBit(P, 63 - i); }
				else if (pos[i] == 'O') { SetBit(O, 63 - i); }
				else if (pos[i] == '#') { SetBit(P, 63 - i); SetBit(O, 63 - i); }
			}

			std::unique_ptr<CPuzzleAllDepthScore> position;

			if (pos[65] == 'X')
				position = std::make_unique<CPuzzleAllDepthScore>(CPosition(P, O));
			else if (pos[65] == 'O')
				position = std::make_unique<CPuzzleAllDepthScore>(CPosition(O, P));
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

	void MultiLineStreamArchive::Serialize(const CPuzzle& obj)
	{
		stream << MultilinePosition(obj.GetPosition()) << "\n";
	}

	void MultiLineStreamArchive::Serialize(const CPuzzleScore& obj)
	{
		auto puzzle = MultilinePosition(obj.GetPosition());
		puzzle.insert(39, "  score: " + DoubleDigitSignedInt(obj.score));
		stream << puzzle << "\n";
	}

	void MultiLineStreamArchive::Serialize(const CPuzzleScoreDepth& obj)
	{
		stream << MultilinePosition(obj.GetPosition()) << "\n";
	}

	void MultiLineStreamArchive::Serialize(const CPuzzleAllDepthScore& obj)
	{
		stream << MultilinePosition(obj.GetPosition()) << "\n";
		//stream <<"\nscore:";
		//for (uint64_t i = 0; i <= EmptyCount(); i++)
		//	stream << " " << SignedInt(obj.score[i]);
	}

	void MultiLineStreamArchive::Serialize(const CPuzzleAllMoveScore& obj)
	{
		stream << MultilinePosition(obj.GetPosition()) << "\n";
	}
}