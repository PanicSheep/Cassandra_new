#include "pch.h"
#include "TestHelpers.h"
#include "Position.h"
#include "FlipLoop.h"
#include "FlipFast.h"
#include "Moves.h"

uint64_t line(const CMove& move, const int dX, const int dY)
{
	uint64_t ret = 0;
	int i = (move.field % 8) + dX; // Starting index in x direction
	int j = (move.field / 8) + dY; // Starting index in y direction

	while ((i >= 0) && (i < 8) && (j >= 0) && (j < 8)) // In between boundaries
	{
		ret |= 1ULL << (j * 8 + i);
		i += dX;
		j += dY;
	}

	return ret;
}

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

TEST(FlipFastTest, Move_A1) { TestFlip(A1); }
TEST(FlipFastTest, Move_B1) { TestFlip(B1); }
TEST(FlipFastTest, Move_C1) { TestFlip(C1); }
TEST(FlipFastTest, Move_D1) { TestFlip(D1); }
TEST(FlipFastTest, Move_E1) { TestFlip(E1); }
TEST(FlipFastTest, Move_F1) { TestFlip(F1); }
TEST(FlipFastTest, Move_G1) { TestFlip(G1); }
TEST(FlipFastTest, Move_H1) { TestFlip(H1); }
TEST(FlipFastTest, Move_A2) { TestFlip(A2); }
TEST(FlipFastTest, Move_B2) { TestFlip(B2); }
TEST(FlipFastTest, Move_C2) { TestFlip(C2); }
TEST(FlipFastTest, Move_D2) { TestFlip(D2); }
TEST(FlipFastTest, Move_E2) { TestFlip(E2); }
TEST(FlipFastTest, Move_F2) { TestFlip(F2); }
TEST(FlipFastTest, Move_G2) { TestFlip(G2); }
TEST(FlipFastTest, Move_H2) { TestFlip(H2); }
TEST(FlipFastTest, Move_A3) { TestFlip(A3); }
TEST(FlipFastTest, Move_B3) { TestFlip(B3); }
TEST(FlipFastTest, Move_C3) { TestFlip(C3); }
TEST(FlipFastTest, Move_D3) { TestFlip(D3); }
TEST(FlipFastTest, Move_E3) { TestFlip(E3); }
TEST(FlipFastTest, Move_F3) { TestFlip(F3); }
TEST(FlipFastTest, Move_G3) { TestFlip(G3); }
TEST(FlipFastTest, Move_H3) { TestFlip(H3); }
TEST(FlipFastTest, Move_A4) { TestFlip(A4); }
TEST(FlipFastTest, Move_B4) { TestFlip(B4); }
TEST(FlipFastTest, Move_C4) { TestFlip(C4); }
TEST(FlipFastTest, Move_D4) { TestFlip(D4); }
TEST(FlipFastTest, Move_E4) { TestFlip(E4); }
TEST(FlipFastTest, Move_F4) { TestFlip(F4); }
TEST(FlipFastTest, Move_G4) { TestFlip(G4); }
TEST(FlipFastTest, Move_H4) { TestFlip(H4); }
TEST(FlipFastTest, Move_A5) { TestFlip(A5); }
TEST(FlipFastTest, Move_B5) { TestFlip(B5); }
TEST(FlipFastTest, Move_C5) { TestFlip(C5); }
TEST(FlipFastTest, Move_D5) { TestFlip(D5); }
TEST(FlipFastTest, Move_E5) { TestFlip(E5); }
TEST(FlipFastTest, Move_F5) { TestFlip(F5); }
TEST(FlipFastTest, Move_G5) { TestFlip(G5); }
TEST(FlipFastTest, Move_H5) { TestFlip(H5); }
TEST(FlipFastTest, Move_A6) { TestFlip(A6); }
TEST(FlipFastTest, Move_B6) { TestFlip(B6); }
TEST(FlipFastTest, Move_C6) { TestFlip(C6); }
TEST(FlipFastTest, Move_D6) { TestFlip(D6); }
TEST(FlipFastTest, Move_E6) { TestFlip(E6); }
TEST(FlipFastTest, Move_F6) { TestFlip(F6); }
TEST(FlipFastTest, Move_G6) { TestFlip(G6); }
TEST(FlipFastTest, Move_H6) { TestFlip(H6); }
TEST(FlipFastTest, Move_A7) { TestFlip(A7); }
TEST(FlipFastTest, Move_B7) { TestFlip(B7); }
TEST(FlipFastTest, Move_C7) { TestFlip(C7); }
TEST(FlipFastTest, Move_D7) { TestFlip(D7); }
TEST(FlipFastTest, Move_E7) { TestFlip(E7); }
TEST(FlipFastTest, Move_F7) { TestFlip(F7); }
TEST(FlipFastTest, Move_G7) { TestFlip(G7); }
TEST(FlipFastTest, Move_H7) { TestFlip(H7); }
TEST(FlipFastTest, Move_A8) { TestFlip(A8); }
TEST(FlipFastTest, Move_B8) { TestFlip(B8); }
TEST(FlipFastTest, Move_C8) { TestFlip(C8); }
TEST(FlipFastTest, Move_D8) { TestFlip(D8); }
TEST(FlipFastTest, Move_E8) { TestFlip(E8); }
TEST(FlipFastTest, Move_F8) { TestFlip(F8); }
TEST(FlipFastTest, Move_G8) { TestFlip(G8); }
TEST(FlipFastTest, Move_H8) { TestFlip(H8); }

//int main(int argc, char **argv)
//{
//	::testing::InitGoogleTest(&argc, argv);
//	return RUN_ALL_TESTS();
//}
