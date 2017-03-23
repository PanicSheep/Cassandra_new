#include "macros_hell.h"
#include "flip_fast.h"
#include "count_last_flip.h"
#include "helpers.h"
#include "gtest/gtest.h"

void TestCountLastFlip(const uint8_t move)
{
	const uint64_t mask = line(move, -1, -1)
	                    | line(move, -1,  0)
	                    | line(move, -1, +1)
	                    | line(move,  0, -1)
	                    | line(move,  0, +1)
	                    | line(move, +1, -1)
	                    | line(move, +1,  0)
	                    | line(move, +1, +1);
	
	for (unsigned int i = 0; i < 1000000; i++)
	{
		CPosition pos;
		pos.MakeRandomFull(mask);
		ASSERT_EQ (PopCount(flip(pos.P, pos.O, move)) * 2, count_last_flip(pos.P, move));
	}
	
	for (unsigned int i = 0; i < 100000; i++)
	{
		CPosition pos;
		pos.MakeRandomFull(~(1ULL << move));
		ASSERT_EQ (PopCount(flip(pos.P, pos.O, move)) * 2, count_last_flip(pos.P, move));
	}
}

TEST (CountLastFlipTest, Move_A1) { TestCountLastFlip( 0); }
TEST (CountLastFlipTest, Move_B1) { TestCountLastFlip( 1); }
TEST (CountLastFlipTest, Move_C1) { TestCountLastFlip( 2); }
TEST (CountLastFlipTest, Move_D1) { TestCountLastFlip( 3); }
TEST (CountLastFlipTest, Move_E1) { TestCountLastFlip( 4); }
TEST (CountLastFlipTest, Move_F1) { TestCountLastFlip( 5); }
TEST (CountLastFlipTest, Move_G1) { TestCountLastFlip( 6); }
TEST (CountLastFlipTest, Move_H1) { TestCountLastFlip( 7); }
TEST (CountLastFlipTest, Move_A2) { TestCountLastFlip( 8); }
TEST (CountLastFlipTest, Move_B2) { TestCountLastFlip( 9); }
TEST (CountLastFlipTest, Move_C2) { TestCountLastFlip(10); }
TEST (CountLastFlipTest, Move_D2) { TestCountLastFlip(11); }
TEST (CountLastFlipTest, Move_E2) { TestCountLastFlip(12); }
TEST (CountLastFlipTest, Move_F2) { TestCountLastFlip(13); }
TEST (CountLastFlipTest, Move_G2) { TestCountLastFlip(14); }
TEST (CountLastFlipTest, Move_H2) { TestCountLastFlip(15); }
TEST (CountLastFlipTest, Move_A3) { TestCountLastFlip(16); }
TEST (CountLastFlipTest, Move_B3) { TestCountLastFlip(17); }
TEST (CountLastFlipTest, Move_C3) { TestCountLastFlip(18); }
TEST (CountLastFlipTest, Move_D3) { TestCountLastFlip(19); }
TEST (CountLastFlipTest, Move_E3) { TestCountLastFlip(20); }
TEST (CountLastFlipTest, Move_F3) { TestCountLastFlip(21); }
TEST (CountLastFlipTest, Move_G3) { TestCountLastFlip(22); }
TEST (CountLastFlipTest, Move_H3) { TestCountLastFlip(23); }
TEST (CountLastFlipTest, Move_A4) { TestCountLastFlip(24); }
TEST (CountLastFlipTest, Move_B4) { TestCountLastFlip(25); }
TEST (CountLastFlipTest, Move_C4) { TestCountLastFlip(26); }
TEST (CountLastFlipTest, Move_D4) { TestCountLastFlip(27); }
TEST (CountLastFlipTest, Move_E4) { TestCountLastFlip(28); }
TEST (CountLastFlipTest, Move_F4) { TestCountLastFlip(29); }
TEST (CountLastFlipTest, Move_G4) { TestCountLastFlip(30); }
TEST (CountLastFlipTest, Move_H4) { TestCountLastFlip(31); }
TEST (CountLastFlipTest, Move_A5) { TestCountLastFlip(32); }
TEST (CountLastFlipTest, Move_B5) { TestCountLastFlip(33); }
TEST (CountLastFlipTest, Move_C5) { TestCountLastFlip(34); }
TEST (CountLastFlipTest, Move_D5) { TestCountLastFlip(35); }
TEST (CountLastFlipTest, Move_E5) { TestCountLastFlip(36); }
TEST (CountLastFlipTest, Move_F5) { TestCountLastFlip(37); }
TEST (CountLastFlipTest, Move_G5) { TestCountLastFlip(38); }
TEST (CountLastFlipTest, Move_H5) { TestCountLastFlip(39); }
TEST (CountLastFlipTest, Move_A6) { TestCountLastFlip(40); }
TEST (CountLastFlipTest, Move_B6) { TestCountLastFlip(41); }
TEST (CountLastFlipTest, Move_C6) { TestCountLastFlip(42); }
TEST (CountLastFlipTest, Move_D6) { TestCountLastFlip(43); }
TEST (CountLastFlipTest, Move_E6) { TestCountLastFlip(44); }
TEST (CountLastFlipTest, Move_F6) { TestCountLastFlip(45); }
TEST (CountLastFlipTest, Move_G6) { TestCountLastFlip(46); }
TEST (CountLastFlipTest, Move_H6) { TestCountLastFlip(47); }
TEST (CountLastFlipTest, Move_A7) { TestCountLastFlip(48); }
TEST (CountLastFlipTest, Move_B7) { TestCountLastFlip(49); }
TEST (CountLastFlipTest, Move_C7) { TestCountLastFlip(50); }
TEST (CountLastFlipTest, Move_D7) { TestCountLastFlip(51); }
TEST (CountLastFlipTest, Move_E7) { TestCountLastFlip(52); }
TEST (CountLastFlipTest, Move_F7) { TestCountLastFlip(53); }
TEST (CountLastFlipTest, Move_G7) { TestCountLastFlip(54); }
TEST (CountLastFlipTest, Move_H7) { TestCountLastFlip(55); }
TEST (CountLastFlipTest, Move_A8) { TestCountLastFlip(56); }
TEST (CountLastFlipTest, Move_B8) { TestCountLastFlip(57); }
TEST (CountLastFlipTest, Move_C8) { TestCountLastFlip(58); }
TEST (CountLastFlipTest, Move_D8) { TestCountLastFlip(59); }
TEST (CountLastFlipTest, Move_E8) { TestCountLastFlip(60); }
TEST (CountLastFlipTest, Move_F8) { TestCountLastFlip(61); }
TEST (CountLastFlipTest, Move_G8) { TestCountLastFlip(62); }
TEST (CountLastFlipTest, Move_H8) { TestCountLastFlip(63); }
TEST (CountLastFlipTest, Move_pass) { TestCountLastFlip(64); }


int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	
	CountLastFlip::Initialize();
	
	return RUN_ALL_TESTS();
}