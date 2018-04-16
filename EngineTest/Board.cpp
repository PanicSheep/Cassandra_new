#include "pch.h"
#include "Boards.h"
#include "StreamQueue.h"
#include "StreamConverter.h"
#include "BoardCollection.h"
#include "PositionGenerator.h"

TEST(Board, Position)
{
	const CPosition pos = CPosition::StartPosition();
	CBoard board1(pos);
	const CBoard board2(pos);

	ASSERT_EQ(board1.GetPosition(), pos);
	ASSERT_EQ(board2.GetPosition(), pos);
}

TEST(Board, ClassID)
{
	const CBoard board(CPosition::StartPosition());
	ASSERT_EQ(board.ClassId(), 1);
}

TEST(Board, Clone)
{
	const CPosition pos = CPosition::StartPosition();
	const CBoard board(pos);
	const auto clone = board.Clone();

	ASSERT_EQ(board.GetPosition(), pos);
}

TEST(Board, Play)
{
	const CPosition pos = CPosition::StartPosition();
	CMoves moves(pos.PossibleMoves());
	const CMove move = moves.ExtractMove();
	const CBoard board(pos);
	const auto clone = board.Play(move);

	ASSERT_EQ(clone->GetPosition(), pos.Play(move));
}

TEST(Board, Test)
{
	const CBoard board_legal(CPosition::StartPosition());
	const CBoard board_illegal(CPosition(0xFF,0xFF));

	ASSERT_EQ(board_legal.Test(), true);
	ASSERT_EQ(board_illegal.Test(), false);
}

TEST(Board, IsSolved)
{
	const CBoard board(CPosition::StartPosition());

	ASSERT_EQ(board.IsSolved(), false);
}

TEST(Board, Streamable_Constructor)
{
	CBoard board1(CPosition::StartPosition());
	StreamQueue queue;
	uint8_t classId;

	queue << board1;
	queue >> classId;

	CBoard board2(queue);

	ASSERT_EQ(classId, board1.ClassId());
	ASSERT_EQ(board1.GetPosition(), board2.GetPosition());
}

TEST(Board, Streamable_stream)
{
	CBoard board1(CPosition::StartPosition());
	CBoard board2(CPosition(0, 0));
	StreamQueue queue;
	uint8_t classId;

	queue << board1;
	queue >> classId;

	queue >> board2;

	ASSERT_EQ(classId, board1.ClassId());
	ASSERT_EQ(board1.GetPosition(), board2.GetPosition());
}

TEST(Board, Streamable_Deserialize)
{
	CBoard board1(CPosition::StartPosition());
	CBoard board2(CPosition(0, 0));
	StreamQueue queue;
	uint8_t classId;

	queue << board1;
	queue >> classId;

	board2.Deserialize(queue);

	ASSERT_EQ(classId, board1.ClassId());
	ASSERT_EQ(board1.GetPosition(), board2.GetPosition());
}

TEST(Board, Streamable_BoardSerializable)
{
	CBoard board1(CPosition::StartPosition());
	BoardToStreamConverter boardArchive(std::make_unique<StreamQueue>());

	boardArchive << board1;
	auto board2 = boardArchive.DeserializeBoard();

	ASSERT_EQ(board1.ClassId(), board2->ClassId());
	ASSERT_EQ(board1.GetPosition(), board2->GetPosition());
}

TEST(BoardScore, PositionScore)
{
	const CPosition pos = CPosition::StartPosition();
	const int8_t score = 4;
	CBoardScore board1(pos, score);
	const CBoardScore board2(pos, score);

	ASSERT_EQ(board1.GetPosition(), pos);
	ASSERT_EQ(board2.GetPosition(), pos);
	ASSERT_EQ(board1.score, score);
	ASSERT_EQ(board2.score, score);
}

TEST(BoardScore, ClassID)
{
	const CBoardScore board(CPosition::StartPosition());
	ASSERT_EQ(board.ClassId(), 2);
}

TEST(BoardScore, Clone)
{
	const CPosition pos = CPosition::StartPosition();
	const int8_t score = 4;
	const CBoardScore board(pos, score);
	const auto clone = board.Clone();

	ASSERT_EQ(board.GetPosition(), pos);
	ASSERT_EQ(board.score, score);
}

TEST(BoardScore, Play)
{
	const CPosition pos = CPosition::StartPosition();
	const int8_t score = 4;
	CMoves moves(pos.PossibleMoves());
	const CMove move = moves.ExtractMove();
	const CBoardScore board(pos, score);
	const auto clone = board.Play(move);

	ASSERT_EQ(clone->GetPosition(), pos.Play(move));
	ASSERT_NE(dynamic_cast<CBoardScore*>(clone.get())->score, 4);
}

TEST(BoardScore, Test)
{
	const CBoardScore board_legal(CPosition::StartPosition());
	const CBoardScore board_illegal1(CPosition::StartPosition(), +65);
	const CBoardScore board_illegal2(CPosition::StartPosition(), -65);
	const CBoardScore board_illegal3(CPosition(0xFF, 0xFF));

	ASSERT_EQ(board_legal.Test(), true);
	ASSERT_EQ(board_illegal1.Test(), false);
	ASSERT_EQ(board_illegal2.Test(), false);
	ASSERT_EQ(board_illegal3.Test(), false);
}

TEST(BoardScore, IsSolved)
{
	const int8_t score = 4;
	const CBoardScore board1(CPosition::StartPosition());
	const CBoardScore board2(CPosition::StartPosition(), score);

	ASSERT_EQ(board1.IsSolved(), false);
	ASSERT_EQ(board2.IsSolved(), true);
}

TEST(BoardScore, Streamable_Constructor)
{
	const int8_t score = 4;
	CBoardScore board1(CPosition::StartPosition(), score);
	StreamQueue queue;
	uint8_t classId;

	queue << board1;
	queue >> classId;

	CBoardScore board2(queue);

	ASSERT_EQ(classId, board1.ClassId());
	ASSERT_EQ(board1.GetPosition(), board2.GetPosition());
	ASSERT_EQ(board1.score, board2.score);
}

TEST(BoardScore, Streamable_stream)
{
	const int8_t score = 4;
	CBoardScore board1(CPosition::StartPosition(), score);
	CBoardScore board2(CPosition(0, 0));
	StreamQueue queue;
	uint8_t classId;

	queue << board1;
	queue >> classId;

	queue >> board2;

	ASSERT_EQ(classId, board1.ClassId());
	ASSERT_EQ(board1.GetPosition(), board2.GetPosition());
	ASSERT_EQ(board1.score, board2.score);
}

TEST(BoardScore, Streamable_Deserialize)
{
	const int8_t score = 4;
	CBoardScore board1(CPosition::StartPosition(), score);
	CBoardScore board2(CPosition(0, 0));
	StreamQueue queue;
	uint8_t classId;

	queue << board1;
	queue >> classId;

	board2.Deserialize(queue);

	ASSERT_EQ(classId, board1.ClassId());
	ASSERT_EQ(board1.GetPosition(), board2.GetPosition());
	ASSERT_EQ(board1.score, board2.score);
}

TEST(BoardScore, Streamable_BoardSerializable)
{
	const int8_t score = 4;
	CBoardScore board1(CPosition::StartPosition(), score);
	BoardToStreamConverter boardArchive(std::make_unique<StreamQueue>());

	boardArchive << board1;
	std::shared_ptr<CBoard> board2 = boardArchive.DeserializeBoard();

	ASSERT_EQ(board1.ClassId(), board2->ClassId());
	ASSERT_EQ(board1.GetPosition(), board2->GetPosition());
	ASSERT_TRUE(std::dynamic_pointer_cast<CBoardScore>(board2) != nullptr);
	ASSERT_EQ(board1.score, std::dynamic_pointer_cast<CBoardScore>(board2)->score);
}

// TODO: Add test for CBoardAllDepthScore!
// TODO: Add test for CBoardAllMoveScore!

TEST(BoardCollection, save_and_load_2_CBoards)
{
	const CPath test_file = "test.pos";

	CBoardCollection boards1;
	CBoardCollection boards2;
	boards1.push_back(std::make_unique<CBoard>(CPosition::StartPosition()));
	boards1.push_back(std::make_unique<CBoard>(CPosition::StartPositionETH()));

	boards1.Save(test_file);
	boards2.Load(test_file);

	for (std::size_t i = 0; i < 2; i++)
		ASSERT_EQ(boards1.Get(i)->GetPosition(), boards2.Get(i)->GetPosition());
}

TEST(BoardCollection, save_and_load_CBoard)
{
	const CPath test_file = "test.pos";
	const std::size_t size = 100;

	const auto positions = CPositionGenerator(1337).GenerateRandomPositionSet(10, size);
	CBoardCollection boards1;
	CBoardCollection boards2;
	for (const auto& pos : positions)
		boards1.push_back(std::make_unique<CBoardScore>(pos));

	boards1.Save(test_file);
	boards2.Load(test_file);

	for (std::size_t i = 0; i < size; i++)
		ASSERT_EQ(boards1.Get(i)->GetPosition(), boards2.Get(i)->GetPosition());
}

TEST(fstreamArchive, save_and_load_0_Board)
{
	const CPath test_file = "test.pos";
	  int8_t a_out, a_in;
	 uint8_t b_out, b_in;
	 int16_t c_out, c_in;
	uint16_t d_out, d_in;
	 int32_t e_out, e_in;
	uint32_t f_out, f_in;
	 int64_t g_out, g_in;
	uint64_t h_out, h_in;
	   float i_out, i_in;
	  double j_out, j_in;
	a_out = 1;
	b_out = 2;
	c_out = 3;
	d_out = 4;
	e_out = 5;
	f_out = 6;
	g_out = 7;
	h_out = 8;
	i_out = 9;
	j_out = 10;

	{
		auto stream_out = std::make_unique<fstreamArchive>(test_file, false);
		*stream_out << a_out;
		*stream_out << b_out;
		*stream_out << c_out;
		*stream_out << d_out;
		*stream_out << e_out;
		*stream_out << f_out;
		*stream_out << g_out;
		*stream_out << h_out;
		*stream_out << i_out;
		*stream_out << j_out;
	}

	{
		auto stream_in = std::make_unique<fstreamArchive>(test_file, true);
		*stream_in >> a_in;
		*stream_in >> b_in;
		*stream_in >> c_in;
		*stream_in >> d_in;
		*stream_in >> e_in;
		*stream_in >> f_in;
		*stream_in >> g_in;
		*stream_in >> h_in;
		*stream_in >> i_in;
		*stream_in >> j_in;
	}

	ASSERT_EQ(a_out, a_in);
	ASSERT_EQ(b_out, b_in);
	ASSERT_EQ(c_out, c_in);
	ASSERT_EQ(d_out, d_in);
	ASSERT_EQ(e_out, e_in);
	ASSERT_EQ(f_out, f_in);
	ASSERT_EQ(g_out, g_in);
	ASSERT_EQ(h_out, h_in);
	ASSERT_EQ(i_out, i_in);
	ASSERT_EQ(j_out, j_in);
}