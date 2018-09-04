#include "pch.h"

#include <fstream>

#include "BinaryStreamArchive.h"
#include "Puzzles.h"
#include "PositionGenerator.h"
#include "IoPuzzleCollection.h"

TEST(puzzle, Position)
{
	const CPosition pos = CPosition::StartPosition();
	CPuzzle puzzle1(pos);
	const CPuzzle puzzle2(pos);

	ASSERT_EQ(puzzle1.GetPosition(), pos);
	ASSERT_EQ(puzzle2.GetPosition(), pos);
}

TEST(puzzle, Clone)
{
	const CPosition pos = CPosition::StartPosition();
	const CPuzzle puzzle(pos);
	const auto clone = puzzle.Clone();

	ASSERT_EQ(puzzle.GetPosition(), pos);
}

TEST(puzzle, Play)
{
	const CPosition pos = CPosition::StartPosition();
	CMoves moves(pos.PossibleMoves());
	const CMove move = moves.ExtractMove();
	const CPuzzle puzzle(pos);
	const auto clone = puzzle.Play(move);

	ASSERT_EQ(clone->GetPosition(), pos.Play(move));
}

TEST(puzzle, Test)
{
	const CPuzzle puzzle_legal(CPosition::StartPosition());
	const CPuzzle puzzle_illegal(CPosition(0xFF,0xFF));

	ASSERT_EQ(puzzle_legal.Test(), true);
	ASSERT_EQ(puzzle_illegal.Test(), false);
}

TEST(puzzle, IsSolved)
{
	const CPuzzle puzzle(CPosition::StartPosition());

	ASSERT_EQ(puzzle.IsSolved(), false);
}

TEST(puzzleScore, PositionScore)
{
	const CPosition pos = CPosition::StartPosition();
	const int8_t score = 4;
	CPuzzleScore puzzle1(pos, score);
	const CPuzzleScore puzzle2(pos, score);

	ASSERT_EQ(puzzle1.GetPosition(), pos);
	ASSERT_EQ(puzzle2.GetPosition(), pos);
	ASSERT_EQ(puzzle1.score, score);
	ASSERT_EQ(puzzle2.score, score);
}

TEST(puzzleScore, Clone)
{
	const CPosition pos = CPosition::StartPosition();
	const int8_t score = 4;
	const CPuzzleScore puzzle(pos, score);
	const auto clone = puzzle.Clone();

	ASSERT_EQ(puzzle.GetPosition(), pos);
	ASSERT_EQ(puzzle.score, score);
}

TEST(puzzleScore, Play)
{
	const CPosition pos = CPosition::StartPosition();
	const int8_t score = 4;
	CMoves moves(pos.PossibleMoves());
	const CMove move = moves.ExtractMove();
	const CPuzzleScore puzzle(pos, score);
	const auto clone = puzzle.Play(move);

	ASSERT_EQ(clone->GetPosition(), pos.Play(move));
	ASSERT_NE(dynamic_cast<CPuzzleScore*>(clone.get())->score, 4);
}

TEST(puzzleScore, Test)
{
	const CPuzzleScore puzzle_legal(CPosition::StartPosition());
	const CPuzzleScore puzzle_illegal1(CPosition::StartPosition(), +65);
	const CPuzzleScore puzzle_illegal2(CPosition::StartPosition(), -65);
	const CPuzzleScore puzzle_illegal3(CPosition(0xFF, 0xFF));

	ASSERT_EQ(puzzle_legal.Test(), true);
	ASSERT_EQ(puzzle_illegal1.Test(), false);
	ASSERT_EQ(puzzle_illegal2.Test(), false);
	ASSERT_EQ(puzzle_illegal3.Test(), false);
}

TEST(puzzleScore, IsSolved)
{
	const int8_t score = 4;
	const CPuzzleScore puzzle1(CPosition::StartPosition());
	const CPuzzleScore puzzle2(CPosition::StartPosition(), score);

	ASSERT_EQ(puzzle1.IsSolved(), false);
	ASSERT_EQ(puzzle2.IsSolved(), true);
}

// TODO: Add test for CPuzzleAllDepthScore!
// TODO: Add test for CPuzzleAllMoveScore!

TEST(puzzleCollection, save_and_load_2_CPuzzles)
{
	const CPath test_file = "test.pos";

	IO::FilePuzzleCollection puzzles1;
	IO::FilePuzzleCollection puzzles2;
	puzzles1.push_back(std::make_unique<CPuzzle>(CPosition::StartPosition()));
	puzzles1.push_back(std::make_unique<CPuzzle>(CPosition::StartPositionETH()));

	puzzles1.Save(test_file);
	puzzles2.Load(test_file);

	for (std::size_t i = 0; i < 2; i++) {
		ASSERT_EQ(*puzzles1.Get(i).get(), *puzzles2.Get(i).get());
	}

	std::remove(test_file.GetAbsoluteFilePath().c_str());
}

TEST(puzzleCollection, save_and_load_CPuzzle)
{
	const CPath test_file = "test.pos";
	const std::size_t size = 100;

	const auto positions = CPositionGenerator(1337).GenerateRandomPositionSet(10, size);
	IO::FilePuzzleCollection puzzles1;
	IO::FilePuzzleCollection puzzles2;
	for (const auto& pos : positions)
		puzzles1.push_back(std::make_unique<CPuzzleScore>(pos));

	puzzles1.Save(test_file);
	puzzles2.Load(test_file);

	for (std::size_t i = 0; i < size; i++) {
		ASSERT_TRUE(*puzzles1[i] == *puzzles2[i]);
	}

	std::remove(test_file.GetAbsoluteFilePath().c_str());
}

TEST(BinaryStreamArchive, save_and_load_0_puzzle)
{
	const CPath test_file = "test.pos";

	CPuzzle              p1_out(CPosition{});
	CPuzzleScore         p2_out(CPosition{});
	CPuzzleScoreDepth    p3_out(CPosition{});
	CPuzzleAllDepthScore p4_out(CPosition{});
	CPuzzleAllMoveScore  p5_out(CPosition{});

	std::unique_ptr<CPuzzle> p1_in;
	std::unique_ptr<CPuzzle> p2_in;
	std::unique_ptr<CPuzzle> p3_in;
	std::unique_ptr<CPuzzle> p4_in;
	std::unique_ptr<CPuzzle> p5_in;

	{
		std::fstream fstream(test_file.GetAbsoluteFilePath(), std::ios::out | std::ios::binary);
		auto stream = IO::BinaryStreamArchive(fstream);
		stream << p1_out;
		stream << p2_out;
		stream << p3_out;
		stream << p4_out;
		stream << p5_out;
	}

	{
		std::fstream fstream(test_file.GetAbsoluteFilePath(), std::ios::in | std::ios::binary);
		auto stream = IO::BinaryStreamArchive(fstream);
		p1_in = stream.Deserialize();
		p2_in = stream.Deserialize();
		p3_in = stream.Deserialize();
		p4_in = stream.Deserialize();
		p5_in = stream.Deserialize();
	}

	std::remove(test_file.GetAbsoluteFilePath().c_str());

	ASSERT_TRUE(dynamic_cast<CPuzzle             *>(p1_in.get()));
	ASSERT_TRUE(dynamic_cast<CPuzzleScore        *>(p2_in.get()));
	ASSERT_TRUE(dynamic_cast<CPuzzleScoreDepth   *>(p3_in.get()));
	ASSERT_TRUE(dynamic_cast<CPuzzleAllDepthScore*>(p4_in.get()));
	ASSERT_TRUE(dynamic_cast<CPuzzleAllMoveScore *>(p5_in.get()));

	ASSERT_TRUE(*p1_in == p1_out);
	ASSERT_TRUE(*p2_in == p2_out);
	ASSERT_TRUE(*p3_in == p3_out);
	ASSERT_TRUE(*p4_in == p4_out);
	ASSERT_TRUE(*p5_in == p5_out);
}