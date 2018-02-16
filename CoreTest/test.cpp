#include "pch.h"
#include "TestHelpers.h"
#include "Position.h"
#include "FlipLoop.h"
#include "FlipFast.h"
#include "Moves.h"

void TestFlip(const CMove& move)
{
	const uint64_t mask = line(move, -1, -1)
	                    | line(move, -1,  0)
	                    | line(move, -1, +1)
	                    | line(move,  0, -1)
	                    | line(move,  0, +1)
	                    | line(move, +1, -1)
	                    | line(move, +1,  0)
	                    | line(move, +1, +1);

	for (unsigned int i = 0; i < 10'000; i++)
	{
		CPosition pos = MakeRandomPosition(mask);
		ASSERT_EQ(Flip(pos, move), Flip_loop(pos, move));
	}

	for (unsigned int i = 0; i < 1'000; i++)
	{
		CPosition pos = MakeRandomPosition(~(1ULL << move.field));
		ASSERT_EQ(Flip(pos, move), Flip_loop(pos, move));
	}
}

TEST(FlipFast, Move_A1) { TestFlip(A1); }
TEST(FlipFast, Move_B1) { TestFlip(B1); }
TEST(FlipFast, Move_C1) { TestFlip(C1); }
TEST(FlipFast, Move_D1) { TestFlip(D1); }
TEST(FlipFast, Move_E1) { TestFlip(E1); }
TEST(FlipFast, Move_F1) { TestFlip(F1); }
TEST(FlipFast, Move_G1) { TestFlip(G1); }
TEST(FlipFast, Move_H1) { TestFlip(H1); }
TEST(FlipFast, Move_A2) { TestFlip(A2); }
TEST(FlipFast, Move_B2) { TestFlip(B2); }
TEST(FlipFast, Move_C2) { TestFlip(C2); }
TEST(FlipFast, Move_D2) { TestFlip(D2); }
TEST(FlipFast, Move_E2) { TestFlip(E2); }
TEST(FlipFast, Move_F2) { TestFlip(F2); }
TEST(FlipFast, Move_G2) { TestFlip(G2); }
TEST(FlipFast, Move_H2) { TestFlip(H2); }
TEST(FlipFast, Move_A3) { TestFlip(A3); }
TEST(FlipFast, Move_B3) { TestFlip(B3); }
TEST(FlipFast, Move_C3) { TestFlip(C3); }
TEST(FlipFast, Move_D3) { TestFlip(D3); }
TEST(FlipFast, Move_E3) { TestFlip(E3); }
TEST(FlipFast, Move_F3) { TestFlip(F3); }
TEST(FlipFast, Move_G3) { TestFlip(G3); }
TEST(FlipFast, Move_H3) { TestFlip(H3); }
TEST(FlipFast, Move_A4) { TestFlip(A4); }
TEST(FlipFast, Move_B4) { TestFlip(B4); }
TEST(FlipFast, Move_C4) { TestFlip(C4); }
TEST(FlipFast, Move_D4) { TestFlip(D4); }
TEST(FlipFast, Move_E4) { TestFlip(E4); }
TEST(FlipFast, Move_F4) { TestFlip(F4); }
TEST(FlipFast, Move_G4) { TestFlip(G4); }
TEST(FlipFast, Move_H4) { TestFlip(H4); }
TEST(FlipFast, Move_A5) { TestFlip(A5); }
TEST(FlipFast, Move_B5) { TestFlip(B5); }
TEST(FlipFast, Move_C5) { TestFlip(C5); }
TEST(FlipFast, Move_D5) { TestFlip(D5); }
TEST(FlipFast, Move_E5) { TestFlip(E5); }
TEST(FlipFast, Move_F5) { TestFlip(F5); }
TEST(FlipFast, Move_G5) { TestFlip(G5); }
TEST(FlipFast, Move_H5) { TestFlip(H5); }
TEST(FlipFast, Move_A6) { TestFlip(A6); }
TEST(FlipFast, Move_B6) { TestFlip(B6); }
TEST(FlipFast, Move_C6) { TestFlip(C6); }
TEST(FlipFast, Move_D6) { TestFlip(D6); }
TEST(FlipFast, Move_E6) { TestFlip(E6); }
TEST(FlipFast, Move_F6) { TestFlip(F6); }
TEST(FlipFast, Move_G6) { TestFlip(G6); }
TEST(FlipFast, Move_H6) { TestFlip(H6); }
TEST(FlipFast, Move_A7) { TestFlip(A7); }
TEST(FlipFast, Move_B7) { TestFlip(B7); }
TEST(FlipFast, Move_C7) { TestFlip(C7); }
TEST(FlipFast, Move_D7) { TestFlip(D7); }
TEST(FlipFast, Move_E7) { TestFlip(E7); }
TEST(FlipFast, Move_F7) { TestFlip(F7); }
TEST(FlipFast, Move_G7) { TestFlip(G7); }
TEST(FlipFast, Move_H7) { TestFlip(H7); }
TEST(FlipFast, Move_A8) { TestFlip(A8); }
TEST(FlipFast, Move_B8) { TestFlip(B8); }
TEST(FlipFast, Move_C8) { TestFlip(C8); }
TEST(FlipFast, Move_D8) { TestFlip(D8); }
TEST(FlipFast, Move_E8) { TestFlip(E8); }
TEST(FlipFast, Move_F8) { TestFlip(F8); }
TEST(FlipFast, Move_G8) { TestFlip(G8); }
TEST(FlipFast, Move_H8) { TestFlip(H8); }

TEST(FlipLoopTest, OneFlip) {
	const CPosition pos(0xFF00000000000000ULL, 0x00FFFFFFFFFFFF7EULL);
	const CMove move(A1);
	const uint64_t flip = Flip_loop(pos, move);

	ASSERT_EQ(pos.GetP() & flip, 0ULL);
	ASSERT_EQ(pos.GetO() & flip, flip);
	ASSERT_EQ(flip, 0x0041211109050300ULL);
}