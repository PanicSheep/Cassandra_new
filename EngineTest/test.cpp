#include "pch.h"
#include "TestHelpers.h"
#include "Position.h"
#include "FlipFast.h"
#include "Moves.h"
#include "LastFlipCounter.h"
#include "Search.h"

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