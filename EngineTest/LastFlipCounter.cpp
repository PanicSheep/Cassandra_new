#include "pch.h"
#include "LastFlipCounter.h"
#include "FlipFast.h"

void TestCountLastFlip(CMove move)
{
	CLastFlipCounter LFC;
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
		ASSERT_EQ(PopCount(Flip(pos, move)) * 2, LFC.CountLastFlip(pos, move));
	}

	for (unsigned int i = 0; i < 10'000; i++)
	{
		CPosition pos = MakeFullRandomPosition(~(MakeBit(move)));
		ASSERT_EQ(PopCount(Flip(pos, move)) * 2, LFC.CountLastFlip(pos, move));
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