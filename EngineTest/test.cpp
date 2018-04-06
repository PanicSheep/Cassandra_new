#include "pch.h"
#include <memory>
#include "TestHelpers.h"
#include "Position.h"
#include "FlipFast.h"
#include "Moves.h"
#include "LastFlipCounter.h"
#include "Search.h"
#include "Path.h"
#include "Utility.h"
#include "Boards.h"
#include "StreamQueue.h"
#include "StreamConverter.h"
#include "BoardCollection.h"
#include "PositionGenerator.h"

void TestCountLastFlip(const CMove& move)
{
	const uint64_t mask = line(move, -1, -1)
	                    | line(move, -1,  0)
	                    | line(move, -1, +1)
	                    | line(move,  0, -1)
	                    | line(move,  0, +1)
	                    | line(move, +1, -1)
	                    | line(move, +1,  0)
	                    | line(move, +1, +1);

	for (unsigned int i = 0; i < 100'000; i++)
	{
		CPosition pos = MakeFullRandomPosition(mask);
		ASSERT_EQ(PopCount(Flip(pos, move)) * 2, CountLastFlip(pos, move));
	}

	for (unsigned int i = 0; i < 10'000; i++)
	{
		CPosition pos = MakeFullRandomPosition(~(1ULL << move.field));
		ASSERT_EQ(PopCount(Flip(pos, move)) * 2, CountLastFlip(pos, move));
	}
}

TEST(CountLastFlip, Move_A1) { TestCountLastFlip(A1); }
TEST(CountLastFlip, Move_B1) { TestCountLastFlip(B1); }
TEST(CountLastFlip, Move_C1) { TestCountLastFlip(C1); }
TEST(CountLastFlip, Move_D1) { TestCountLastFlip(D1); }
TEST(CountLastFlip, Move_E1) { TestCountLastFlip(E1); }
TEST(CountLastFlip, Move_F1) { TestCountLastFlip(F1); }
TEST(CountLastFlip, Move_G1) { TestCountLastFlip(G1); }
TEST(CountLastFlip, Move_H1) { TestCountLastFlip(H1); }
TEST(CountLastFlip, Move_A2) { TestCountLastFlip(A2); }
TEST(CountLastFlip, Move_B2) { TestCountLastFlip(B2); }
TEST(CountLastFlip, Move_C2) { TestCountLastFlip(C2); }
TEST(CountLastFlip, Move_D2) { TestCountLastFlip(D2); }
TEST(CountLastFlip, Move_E2) { TestCountLastFlip(E2); }
TEST(CountLastFlip, Move_F2) { TestCountLastFlip(F2); }
TEST(CountLastFlip, Move_G2) { TestCountLastFlip(G2); }
TEST(CountLastFlip, Move_H2) { TestCountLastFlip(H2); }
TEST(CountLastFlip, Move_A3) { TestCountLastFlip(A3); }
TEST(CountLastFlip, Move_B3) { TestCountLastFlip(B3); }
TEST(CountLastFlip, Move_C3) { TestCountLastFlip(C3); }
TEST(CountLastFlip, Move_D3) { TestCountLastFlip(D3); }
TEST(CountLastFlip, Move_E3) { TestCountLastFlip(E3); }
TEST(CountLastFlip, Move_F3) { TestCountLastFlip(F3); }
TEST(CountLastFlip, Move_G3) { TestCountLastFlip(G3); }
TEST(CountLastFlip, Move_H3) { TestCountLastFlip(H3); }
TEST(CountLastFlip, Move_A4) { TestCountLastFlip(A4); }
TEST(CountLastFlip, Move_B4) { TestCountLastFlip(B4); }
TEST(CountLastFlip, Move_C4) { TestCountLastFlip(C4); }
TEST(CountLastFlip, Move_D4) { TestCountLastFlip(D4); }
TEST(CountLastFlip, Move_E4) { TestCountLastFlip(E4); }
TEST(CountLastFlip, Move_F4) { TestCountLastFlip(F4); }
TEST(CountLastFlip, Move_G4) { TestCountLastFlip(G4); }
TEST(CountLastFlip, Move_H4) { TestCountLastFlip(H4); }
TEST(CountLastFlip, Move_A5) { TestCountLastFlip(A5); }
TEST(CountLastFlip, Move_B5) { TestCountLastFlip(B5); }
TEST(CountLastFlip, Move_C5) { TestCountLastFlip(C5); }
TEST(CountLastFlip, Move_D5) { TestCountLastFlip(D5); }
TEST(CountLastFlip, Move_E5) { TestCountLastFlip(E5); }
TEST(CountLastFlip, Move_F5) { TestCountLastFlip(F5); }
TEST(CountLastFlip, Move_G5) { TestCountLastFlip(G5); }
TEST(CountLastFlip, Move_H5) { TestCountLastFlip(H5); }
TEST(CountLastFlip, Move_A6) { TestCountLastFlip(A6); }
TEST(CountLastFlip, Move_B6) { TestCountLastFlip(B6); }
TEST(CountLastFlip, Move_C6) { TestCountLastFlip(C6); }
TEST(CountLastFlip, Move_D6) { TestCountLastFlip(D6); }
TEST(CountLastFlip, Move_E6) { TestCountLastFlip(E6); }
TEST(CountLastFlip, Move_F6) { TestCountLastFlip(F6); }
TEST(CountLastFlip, Move_G6) { TestCountLastFlip(G6); }
TEST(CountLastFlip, Move_H6) { TestCountLastFlip(H6); }
TEST(CountLastFlip, Move_A7) { TestCountLastFlip(A7); }
TEST(CountLastFlip, Move_B7) { TestCountLastFlip(B7); }
TEST(CountLastFlip, Move_C7) { TestCountLastFlip(C7); }
TEST(CountLastFlip, Move_D7) { TestCountLastFlip(D7); }
TEST(CountLastFlip, Move_E7) { TestCountLastFlip(E7); }
TEST(CountLastFlip, Move_F7) { TestCountLastFlip(F7); }
TEST(CountLastFlip, Move_G7) { TestCountLastFlip(G7); }
TEST(CountLastFlip, Move_H7) { TestCountLastFlip(H7); }
TEST(CountLastFlip, Move_A8) { TestCountLastFlip(A8); }
TEST(CountLastFlip, Move_B8) { TestCountLastFlip(B8); }
TEST(CountLastFlip, Move_C8) { TestCountLastFlip(C8); }
TEST(CountLastFlip, Move_D8) { TestCountLastFlip(D8); }
TEST(CountLastFlip, Move_E8) { TestCountLastFlip(E8); }
TEST(CountLastFlip, Move_F8) { TestCountLastFlip(F8); }
TEST(CountLastFlip, Move_G8) { TestCountLastFlip(G8); }
TEST(CountLastFlip, Move_H8) { TestCountLastFlip(H8); }

class Search_Test : public Search
{
public:
	template <int EmptyCount>
	static int EvalGameOver(const CPosition& pos) { return Search::EvalGameOver<EmptyCount>(pos); }
	static int EvalGameOver(const CPosition& pos) { return Search::EvalGameOver(pos); }
	static int EvalGameOver(const CPosition& pos, const uint64_t EmptyCount) { return Search::EvalGameOver(pos, EmptyCount); }
};

TEST(Search, EvalGameOver)
{
	const auto pos1 = CPosition(0x0000000000000000ULL, 0x0000000000000000ULL); // Player: 0   Opponent: 0   Empty: 64   Score: 0-0=0
	const auto pos2 = CPosition(0x00000000000000FFULL, 0x0000000000000000ULL); // Player: 8   Opponent: 0   Empty: 56   Score: 8-0+56=64
	const auto pos3 = CPosition(0x00000000000000FFULL, 0xFF00000000000000ULL); // Player: 8   Opponent: 8   Empty: 48   Score: 8-8=0
	const auto pos4 = CPosition(0x00000000000000FFULL, 0xFF10000000000000ULL); // Player: 8   Opponent: 9   Empty: 47   Score: 8-9-47=-48
	const auto pos5 = CPosition(0x0000000000000000ULL, 0x1000000000000000ULL); // Player: 0   Opponent: 1   Empty: 63   Score: 0-1-63=-64
	const int score1 = 0;
	const int score2 = 64;
	const int score3 = 0;
	const int score4 = -48;
	const int score5 = -64;

	ASSERT_EQ(Search_Test::EvalGameOver(pos1, 64), score1);
	ASSERT_EQ(Search_Test::EvalGameOver(pos2, 56), score2);
	ASSERT_EQ(Search_Test::EvalGameOver(pos3, 48), score3);
	ASSERT_EQ(Search_Test::EvalGameOver(pos4, 47), score4);
	ASSERT_EQ(Search_Test::EvalGameOver(pos5, 63), score5);

	ASSERT_EQ(Search_Test::EvalGameOver<64>(pos1), score1);
	ASSERT_EQ(Search_Test::EvalGameOver<56>(pos2), score2);
	ASSERT_EQ(Search_Test::EvalGameOver<48>(pos3), score3);
	ASSERT_EQ(Search_Test::EvalGameOver<47>(pos4), score4);
	ASSERT_EQ(Search_Test::EvalGameOver<63>(pos5), score5);
}

class CPositionScore : public CPosition
{
public:
	int score;
	CPositionScore(uint64_t P, uint64_t O, int score) : CPosition(P, O), score(score) {}
};

std::vector<CPositionScore> TestPos = {
	CPositionScore(0xFF00FF00FF00FF00ULL, 0x00FF00FF00FF00FFULL,   0), //  0
	CPositionScore(0xFF00FF00FF00FFFFULL, 0x00FF00FF00FF0000ULL, +16), //  1
	CPositionScore(0xFF00FF00FF000000ULL, 0x00FF00FF00FFFFFFULL, -16), //  2

	CPositionScore(0xFFFFFFFFFFFFFFFEULL, 0x0000000000000000ULL, +64), //  3
	CPositionScore(0xFFFFFFFFFFFFFCFCULL, 0x0000000000000302ULL, +64), //  4
	CPositionScore(0xFFFFFFFFFFFFFF7EULL, 0x0000000000000080ULL, +48), //  5
	CPositionScore(0xFFFFFFFFFFFFFE7EULL, 0x0000000000000180ULL, +62), //  6

	CPositionScore(0xFEFFFFFFFFFFFFFEULL, 0x0000000000000000ULL, +64), //  7
	CPositionScore(0xFCFCFFFFFFFFFCFCULL, 0x0203000000000302ULL, +64), //  8
	CPositionScore(0xFEFFFFFFFFFFFF7EULL, 0x0000000000000080ULL, +22), //  9
	CPositionScore(0xFFFFFFFFFBFEFC7EULL, 0x0000000004010280ULL, +54), // 10

	CPositionScore(0xFFFFFFFFFFFFFFF8ULL, 0x0000000000000000ULL, +64), // 11
	CPositionScore(0xFFFFFFFFFFFFFDF8ULL, 0x0000000000000200ULL, +64), // 12
	CPositionScore(0xF8FFFFFFFFFFFFF8ULL, 0x0700000000000000ULL, +16), // 13
	CPositionScore(0xFFFFFFFEFBFEFC7EULL, 0x0000000104000280ULL, +58), // 14

	CPositionScore(0xFFFFFFFFFFFFFFF0ULL, 0x0000000000000000ULL, +64), // 15
	CPositionScore(0xFFFFFFFFFFFFE0E0ULL, 0x0000000000001F10ULL, +64), // 16
	CPositionScore(0xF0FFFFFFFFFFFFF0ULL, 0x0F00000000000000ULL,  +0), // 17
	CPositionScore(0xFFFFFFFFFFE7DBC3ULL, 0x0000000000182400ULL, +56), // 18

	// FForum
	CPositionScore(0x3E0C0D2B772B7000ULL, 0x0071725488140E7CULL, +18), // 19
	CPositionScore(0x7E2059000C19303EULL, 0x001E267FF3E60C00ULL, +10), // 20
	CPositionScore(0x040C0D306C707A01ULL, 0x083070CF938F853EULL,  +2), // 21
	CPositionScore(0x7EB8B9D127070C2CULL, 0x0006462E58383012ULL,  +0), // 22
	CPositionScore(0x0019D6D6C8F0A07CULL, 0x7C242829370D1C00ULL, +32), // 23
	CPositionScore(0x1C1C14223F9A047EULL, 0x20E0EADCC0647800ULL, +14), // 24
	CPositionScore(0x18BC8F9F877F3018ULL, 0x2440706078000E06ULL,  +8), // 25
	CPositionScore(0x0000081F22426064ULL, 0x14BCF7E0DCBC9C98ULL,  +8), // 26
	CPositionScore(0x10B070222E942C3CULL, 0x2C0F0F5DD16B1000ULL,  -8), // 27
	CPositionScore(0x002054A49716247CULL, 0x781CAB5B68E91800ULL, +10), // 28
};

void TestTestPos(unsigned int emptyCount, unsigned int MobilityPlayer, unsigned int MobilityOpponent, unsigned int index)
{
	ASSERT_TRUE(TestPos[index].score >= -64);
	ASSERT_TRUE(TestPos[index].score <= +64);
	ASSERT_TRUE((TestPos[index].GetP() & TestPos[index].GetO()) == 0ULL);
	ASSERT_EQ(TestPos[index].EmptyCount(), emptyCount);
	ASSERT_EQ(TestPos[index].PossibleMoves().size(), MobilityPlayer);
	ASSERT_EQ(TestPos[index].PlayPass().PossibleMoves().size(), MobilityOpponent);
}

TEST(MetaTest, TestPos00) { TestTestPos(0, 0, 0,  0); }
TEST(MetaTest, TestPos01) { TestTestPos(0, 0, 0,  1); }
TEST(MetaTest, TestPos02) { TestTestPos(0, 0, 0,  2); }
TEST(MetaTest, TestPos03) { TestTestPos(1, 0, 0,  3); }
TEST(MetaTest, TestPos04) { TestTestPos(1, 1, 0,  4); }
TEST(MetaTest, TestPos05) { TestTestPos(1, 0, 1,  5); }
TEST(MetaTest, TestPos06) { TestTestPos(1, 1, 1,  6); }
TEST(MetaTest, TestPos07) { TestTestPos(2, 0, 0,  7); }
TEST(MetaTest, TestPos08) { TestTestPos(2, 2, 0,  8); }
TEST(MetaTest, TestPos09) { TestTestPos(2, 0, 2,  9); }
TEST(MetaTest, TestPos10) { TestTestPos(2, 2, 2, 10); }
TEST(MetaTest, TestPos11) { TestTestPos(3, 0, 0, 11); }
TEST(MetaTest, TestPos12) { TestTestPos(3, 3, 0, 12); }
TEST(MetaTest, TestPos13) { TestTestPos(3, 0, 3, 13); }
TEST(MetaTest, TestPos14) { TestTestPos(3, 3, 3, 14); }
TEST(MetaTest, TestPos15) { TestTestPos(4, 0, 0, 15); }
TEST(MetaTest, TestPos16) { TestTestPos(4, 4, 0, 16); }
TEST(MetaTest, TestPos17) { TestTestPos(4, 0, 4, 17); }
TEST(MetaTest, TestPos18) { TestTestPos(4, 4, 4, 18); }

void TestPosition(unsigned int index, Search& search)
{
	const int score = search.Eval(TestPos[index]);

	ASSERT_EQ(score, TestPos[index].score);
}

TEST(NegaMax, ZeroEmpties0)  { TestPosition( 0, NegaMaxSearch()); }
TEST(NegaMax, ZeroEmpties1)  { TestPosition( 1, NegaMaxSearch()); }
TEST(NegaMax, ZeroEmpties2)  { TestPosition( 2, NegaMaxSearch()); }
TEST(NegaMax, OneEmpties0)   { TestPosition( 3, NegaMaxSearch()); }
TEST(NegaMax, OneEmpties1)   { TestPosition( 4, NegaMaxSearch()); }
TEST(NegaMax, OneEmpties2)   { TestPosition( 5, NegaMaxSearch()); }
TEST(NegaMax, OneEmpties3)   { TestPosition( 6, NegaMaxSearch()); }
TEST(NegaMax, TwoEmpties0)   { TestPosition( 7, NegaMaxSearch()); }
TEST(NegaMax, TwoEmpties1)   { TestPosition( 8, NegaMaxSearch()); }
TEST(NegaMax, TwoEmpties2)   { TestPosition( 9, NegaMaxSearch()); }
TEST(NegaMax, TwoEmpties3)   { TestPosition(10, NegaMaxSearch()); }
TEST(NegaMax, ThreeEmpties0) { TestPosition(11, NegaMaxSearch()); }
TEST(NegaMax, ThreeEmpties1) { TestPosition(12, NegaMaxSearch()); }
TEST(NegaMax, ThreeEmpties2) { TestPosition(13, NegaMaxSearch()); }
TEST(NegaMax, ThreeEmpties3) { TestPosition(14, NegaMaxSearch()); }
TEST(NegaMax, FourEmpties0)  { TestPosition(15, NegaMaxSearch()); }
TEST(NegaMax, FourEmpties1)  { TestPosition(16, NegaMaxSearch()); }
TEST(NegaMax, FourEmpties2)  { TestPosition(17, NegaMaxSearch()); }
TEST(NegaMax, FourEmpties3)  { TestPosition(18, NegaMaxSearch()); }

TEST(AlphaBetaFailHard, ZeroEmpties0)  { TestPosition( 0, AlphaBetaFailHardSearch()); }
TEST(AlphaBetaFailHard, ZeroEmpties1)  { TestPosition( 1, AlphaBetaFailHardSearch()); }
TEST(AlphaBetaFailHard, ZeroEmpties2)  { TestPosition( 2, AlphaBetaFailHardSearch()); }
TEST(AlphaBetaFailHard, OneEmpties0)   { TestPosition( 3, AlphaBetaFailHardSearch()); }
TEST(AlphaBetaFailHard, OneEmpties1)   { TestPosition( 4, AlphaBetaFailHardSearch()); }
TEST(AlphaBetaFailHard, OneEmpties2)   { TestPosition( 5, AlphaBetaFailHardSearch()); }
TEST(AlphaBetaFailHard, OneEmpties3)   { TestPosition( 6, AlphaBetaFailHardSearch()); }
TEST(AlphaBetaFailHard, TwoEmpties0)   { TestPosition( 7, AlphaBetaFailHardSearch()); }
TEST(AlphaBetaFailHard, TwoEmpties1)   { TestPosition( 8, AlphaBetaFailHardSearch()); }
TEST(AlphaBetaFailHard, TwoEmpties2)   { TestPosition( 9, AlphaBetaFailHardSearch()); }
TEST(AlphaBetaFailHard, TwoEmpties3)   { TestPosition(10, AlphaBetaFailHardSearch()); }
TEST(AlphaBetaFailHard, ThreeEmpties0) { TestPosition(11, AlphaBetaFailHardSearch()); }
TEST(AlphaBetaFailHard, ThreeEmpties1) { TestPosition(12, AlphaBetaFailHardSearch()); }
TEST(AlphaBetaFailHard, ThreeEmpties2) { TestPosition(13, AlphaBetaFailHardSearch()); }
TEST(AlphaBetaFailHard, ThreeEmpties3) { TestPosition(14, AlphaBetaFailHardSearch()); }
TEST(AlphaBetaFailHard, FourEmpties0)  { TestPosition(15, AlphaBetaFailHardSearch()); }
TEST(AlphaBetaFailHard, FourEmpties1)  { TestPosition(16, AlphaBetaFailHardSearch()); }
TEST(AlphaBetaFailHard, FourEmpties2)  { TestPosition(17, AlphaBetaFailHardSearch()); }
TEST(AlphaBetaFailHard, FourEmpties3)  { TestPosition(18, AlphaBetaFailHardSearch()); }

TEST(AlphaBetaFailSoft, ZeroEmpties0)  { TestPosition( 0, AlphaBetaFailSoftSearch()); }
TEST(AlphaBetaFailSoft, ZeroEmpties1)  { TestPosition( 1, AlphaBetaFailSoftSearch()); }
TEST(AlphaBetaFailSoft, ZeroEmpties2)  { TestPosition( 2, AlphaBetaFailSoftSearch()); }
TEST(AlphaBetaFailSoft, OneEmpties0)   { TestPosition( 3, AlphaBetaFailSoftSearch()); }
TEST(AlphaBetaFailSoft, OneEmpties1)   { TestPosition( 4, AlphaBetaFailSoftSearch()); }
TEST(AlphaBetaFailSoft, OneEmpties2)   { TestPosition( 5, AlphaBetaFailSoftSearch()); }
TEST(AlphaBetaFailSoft, OneEmpties3)   { TestPosition( 6, AlphaBetaFailSoftSearch()); }
TEST(AlphaBetaFailSoft, TwoEmpties0)   { TestPosition( 7, AlphaBetaFailSoftSearch()); }
TEST(AlphaBetaFailSoft, TwoEmpties1)   { TestPosition( 8, AlphaBetaFailSoftSearch()); }
TEST(AlphaBetaFailSoft, TwoEmpties2)   { TestPosition( 9, AlphaBetaFailSoftSearch()); }
TEST(AlphaBetaFailSoft, TwoEmpties3)   { TestPosition(10, AlphaBetaFailSoftSearch()); }
TEST(AlphaBetaFailSoft, ThreeEmpties0) { TestPosition(11, AlphaBetaFailSoftSearch()); }
TEST(AlphaBetaFailSoft, ThreeEmpties1) { TestPosition(12, AlphaBetaFailSoftSearch()); }
TEST(AlphaBetaFailSoft, ThreeEmpties2) { TestPosition(13, AlphaBetaFailSoftSearch()); }
TEST(AlphaBetaFailSoft, ThreeEmpties3) { TestPosition(14, AlphaBetaFailSoftSearch()); }
TEST(AlphaBetaFailSoft, FourEmpties0)  { TestPosition(15, AlphaBetaFailSoftSearch()); }
TEST(AlphaBetaFailSoft, FourEmpties1)  { TestPosition(16, AlphaBetaFailSoftSearch()); }
TEST(AlphaBetaFailSoft, FourEmpties2)  { TestPosition(17, AlphaBetaFailSoftSearch()); }
TEST(AlphaBetaFailSoft, FourEmpties3)  { TestPosition(18, AlphaBetaFailSoftSearch()); }

TEST(PVS, ZeroEmpties0)  { TestPosition( 0, PVSearch()); }
TEST(PVS, ZeroEmpties1)  { TestPosition( 1, PVSearch()); }
TEST(PVS, ZeroEmpties2)  { TestPosition( 2, PVSearch()); }
TEST(PVS, OneEmpties0)   { TestPosition( 3, PVSearch()); }
TEST(PVS, OneEmpties1)   { TestPosition( 4, PVSearch()); }
TEST(PVS, OneEmpties2)   { TestPosition( 5, PVSearch()); }
TEST(PVS, OneEmpties3)   { TestPosition( 6, PVSearch()); }
TEST(PVS, TwoEmpties0)   { TestPosition( 7, PVSearch()); }
TEST(PVS, TwoEmpties1)   { TestPosition( 8, PVSearch()); }
TEST(PVS, TwoEmpties2)   { TestPosition( 9, PVSearch()); }
TEST(PVS, TwoEmpties3)   { TestPosition(10, PVSearch()); }
TEST(PVS, ThreeEmpties0) { TestPosition(11, PVSearch()); }
TEST(PVS, ThreeEmpties1) { TestPosition(12, PVSearch()); }
TEST(PVS, ThreeEmpties2) { TestPosition(13, PVSearch()); }
TEST(PVS, ThreeEmpties3) { TestPosition(14, PVSearch()); }
TEST(PVS, FourEmpties0)  { TestPosition(15, PVSearch()); }
TEST(PVS, FourEmpties1)  { TestPosition(16, PVSearch()); }
TEST(PVS, FourEmpties2)  { TestPosition(17, PVSearch()); }
TEST(PVS, FourEmpties3)  { TestPosition(18, PVSearch()); }

TEST (PVS, FForum01) { TestPosition(19, PVSearch()); }
TEST (PVS, FForum02) { TestPosition(20, PVSearch()); }
TEST (PVS, FForum03) { TestPosition(21, PVSearch()); }
TEST (PVS, FForum04) { TestPosition(22, PVSearch()); }
TEST (PVS, FForum05) { TestPosition(23, PVSearch()); }
TEST (PVS, FForum06) { TestPosition(24, PVSearch()); }
TEST (PVS, FForum07) { TestPosition(25, PVSearch()); }
TEST (PVS, FForum08) { TestPosition(26, PVSearch()); }
TEST (PVS, FForum09) { TestPosition(27, PVSearch()); }
TEST (PVS, FForum10) { TestPosition(28, PVSearch()); }

void Test_Folder(
	std::string path_string,
	std::string absolute_folder_path,
	std::string folder_name,
	std::string relative_folder_path)
{
	replace_all(path_string         , WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);
	replace_all(absolute_folder_path, WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);
	replace_all(folder_name         , WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);
	replace_all(relative_folder_path, WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);

	CPath path(path_string);

	ASSERT_EQ(path.IsFile(), false);
	ASSERT_EQ(path.IsFolder(), true);

	ASSERT_EQ(path.GetAbsoluteFolderPath(), absolute_folder_path);
	ASSERT_EQ(path.GetFolderName(), folder_name);
	ASSERT_EQ(path.GetRelativeFolderPath(), relative_folder_path);

	ASSERT_ANY_THROW(path.GetRawFileName());
	ASSERT_ANY_THROW(path.GetFullFileName());
	ASSERT_ANY_THROW(path.GetExtension());
	ASSERT_ANY_THROW(path.GetAbsoluteFilePath());
	ASSERT_ANY_THROW(path.GetRelativeFilePath());
}

TEST(PathTest, RelativeFolder1)
{
	std::string path_string          = "home/";
	std::string absolute_folder_path = GetCurrentWorkingDirectory() + "/home/";
	std::string folder_name          = "home";
	std::string relative_folder_path = "home/";

	Test_Folder(path_string, absolute_folder_path, folder_name, relative_folder_path);
}

TEST(PathTest, RelativeFolder2)
{
	std::string path_string          = "etc/../home/";
	std::string absolute_folder_path = GetCurrentWorkingDirectory() + "/home/";
	std::string folder_name          = "home";
	std::string relative_folder_path = "home/";

	Test_Folder(path_string, absolute_folder_path, folder_name, relative_folder_path);
}

TEST(PathTest, AbsoluteFolder1)
{
	std::string path_string          = GetCurrentWorkingDirectory() + "/home/";
	std::string absolute_folder_path = GetCurrentWorkingDirectory() + "/home/";
	std::string folder_name          = "home";
	std::string relative_folder_path = "home/";

	Test_Folder(path_string, absolute_folder_path, folder_name, relative_folder_path);
}

TEST(PathTest, AbsoluteFolder2)
{
	std::string path_string          = "~\\home\\";
	std::string absolute_folder_path = "~\\home\\";
	std::string folder_name          = "home";
	std::string relative_folder_path = "~\\home\\";

	Test_Folder(path_string, absolute_folder_path, folder_name, relative_folder_path);
}

TEST(PathTest, AbsoluteFolder3)
{
	std::string path_string          = "\\\\computer\\home\\";
	std::string absolute_folder_path = "\\\\computer\\home\\";
	std::string folder_name          = "home";
	std::string relative_folder_path = "\\\\computer\\home\\";

	Test_Folder(path_string, absolute_folder_path, folder_name, relative_folder_path);
}


void Test_File(
	std::string path_string,
	std::string raw_file_name,
	std::string full_file_name,
	std::string extension,
	std::string absolute_file_path,
	std::string relative_file_path)
{
	replace_all(path_string       , WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);
	replace_all(raw_file_name     , WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);
	replace_all(full_file_name    , WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);
	replace_all(extension         , WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);
	replace_all(absolute_file_path, WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);
	replace_all(relative_file_path, WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);

	CPath path(path_string);

	ASSERT_EQ(path.IsFile(), true);
	ASSERT_EQ(path.IsFolder(), false);

	ASSERT_ANY_THROW(path.GetAbsoluteFolderPath());
	ASSERT_ANY_THROW(path.GetFolderName());
	ASSERT_ANY_THROW(path.GetRelativeFolderPath());

	ASSERT_EQ(path.GetRawFileName(), raw_file_name);
	ASSERT_EQ(path.GetFullFileName(), full_file_name);
	ASSERT_EQ(path.GetExtension(), extension);
	ASSERT_EQ(path.GetAbsoluteFilePath(), absolute_file_path);
	ASSERT_EQ(path.GetRelativeFilePath(), relative_file_path);
}

TEST(PathTest, RelativeFile1)
{
	std::string path_string		   = "home/text.txt";
	std::string raw_file_name	   = "text";
	std::string full_file_name	   = "text.txt";
	std::string extension		   = "txt";
	std::string absolute_file_path = GetCurrentWorkingDirectory() + "/home/text.txt";
	std::string relative_file_path = "home/text.txt";

	Test_File(path_string, raw_file_name, full_file_name, extension, absolute_file_path, relative_file_path);
}

TEST(PathTest, RelativeFile2)
{
	std::string path_string		   = "text.txt";
	std::string raw_file_name	   = "text";
	std::string full_file_name	   = "text.txt";
	std::string extension		   = "txt";
	std::string absolute_file_path = GetCurrentWorkingDirectory() + "/text.txt";
	std::string relative_file_path = "text.txt";

	Test_File(path_string, raw_file_name, full_file_name, extension, absolute_file_path, relative_file_path);
}

TEST(PathTest, RelativeFile3)
{
	std::string path_string		   = "home/../text.txt";
	std::string raw_file_name	   = "text";
	std::string full_file_name	   = "text.txt";
	std::string extension		   = "txt";
	std::string absolute_file_path = GetCurrentWorkingDirectory() + "/text.txt";
	std::string relative_file_path = "text.txt";

	Test_File(path_string, raw_file_name, full_file_name, extension, absolute_file_path, relative_file_path);
}

TEST(PathTest, AbsoluteFile1)
{
	std::string path_string		   = GetCurrentWorkingDirectory() + "/home/text.txt";
	std::string raw_file_name	   = "text";
	std::string full_file_name	   = "text.txt";
	std::string extension		   = "txt";
	std::string absolute_file_path = GetCurrentWorkingDirectory() + "/home/text.txt";
	std::string relative_file_path = "home/text.txt";

	Test_File(path_string, raw_file_name, full_file_name, extension, absolute_file_path, relative_file_path);
}

TEST(PathTest, AbsoluteFile2)
{
	std::string path_string		   = "~/home/text.txt";
	std::string raw_file_name	   = "text";
	std::string full_file_name	   = "text.txt";
	std::string extension		   = "txt";
	std::string absolute_file_path = "~/home/text.txt";
	std::string relative_file_path = "~/home/text.txt";

	Test_File(path_string, raw_file_name, full_file_name, extension, absolute_file_path, relative_file_path);
}

TEST(PathTest, AbsoluteFile3)
{
	std::string path_string		   = "//home/text.txt";
	std::string raw_file_name	   = "text";
	std::string full_file_name	   = "text.txt";
	std::string extension		   = "txt";
	std::string absolute_file_path = "//home/text.txt";
	std::string relative_file_path = "//home/text.txt";

	Test_File(path_string, raw_file_name, full_file_name, extension, absolute_file_path, relative_file_path);
}

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

	delete clone;
}

TEST(Board, Play)
{
	const CPosition pos = CPosition::StartPosition();
	CMoves moves(pos.PossibleMoves());
	const CMove move = moves.ExtractMove();
	const CBoard board(pos);
	const auto clone = board.Play(move);

	ASSERT_EQ(clone->GetPosition(), pos.Play(move));

	delete clone;
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

	delete clone;
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
	ASSERT_NE(clone->score, 4);

	delete clone;
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
		ASSERT_EQ(boards1[i]->GetPosition(), boards2[i]->GetPosition());
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
		ASSERT_EQ(boards1[i]->GetPosition(), boards2[i]->GetPosition());
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