#include "MultiLineStreamArchive.h"

#include <cstdint>
#include <regex>
#include <string>

#include "IoMoves.h"
#include "Puzzles.h"
#include "Utility.h"

using namespace IO;

namespace
{
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
			const bool isM = Moves.HasMove(static_cast<CMove>(63 - i));
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
		if (pos[65] == 'O')
			return std::make_unique<CPuzzle>(CPosition(O, P));
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
			return std::make_unique<CPuzzleScore>(CPosition(O, P), -score);
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
			const auto move = to_CMove(pos.substr(i, 2));
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