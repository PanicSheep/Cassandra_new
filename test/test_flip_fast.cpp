#include "macros_hell.h"
#include "flip_loop.h"
#include "flip_fast.h"
#include "gtest/gtest.h"
#include <functional>

class CPosition
{
public:
	uint64_t P, O;
	
	CPosition(uint64_t P, uint64_t O) : P(P), O(O) {}
	CPosition() : CPosition(0, 0) {}
	
	void MakeRandom(const uint64_t mask = 0xFFFFFFFFFFFFFFFFULL)
	{
		static auto rnd = std::bind(std::uniform_int_distribution<unsigned int>(0,3), std::mt19937_64(13));
		uint64_t p = 0;
		uint64_t o = 0;
		for (unsigned int i = 0; i < PopCount(mask); i++)
		{
			p <<= 1;
			o <<= 1;
			switch (rnd())
			{
				case 0: p |= 1ULL; break;
				case 1: o |= 1ULL; break;
			}
		}
		P = PDep(p, mask);
		O = PDep(o, mask);
	}
};

uint64_t line(const uint8_t move, const int dX, const int dY)
{
    uint64_t ret = 0;
    int i = (move % 8) + dX; // Starting index in x direction
    int j = (move / 8) + dY; // Starting index in y direction

    while ((i >= 0) && (i < 8) && (j >= 0) && (j < 8)) // In between boundaries
    {
        ret |= 1ULL << (j * 8 + i);
        i += dX;
        j += dY;
    }

    return ret;
}

TEST (FlipFastTest, Line) {
	uint64_t maskPopCount[64] = {
		21, 21, 21, 21, 21, 21, 21, 21,
		21, 23, 23, 23, 23, 23, 23, 21,
		21, 23, 25, 25, 25, 25, 23, 21,
		21, 23, 25, 27, 27, 25, 23, 21,
		21, 23, 25, 27, 27, 25, 23, 21,
		21, 23, 25, 25, 25, 25, 23, 21,
		21, 23, 23, 23, 23, 23, 23, 21,
		21, 21, 21, 21, 21, 21, 21, 21 };
	for (uint8_t move = 0; move < 64; move++)
	{
		const uint64_t mask = line(move, -1, -1)
				    | line(move, -1,  0)
				    | line(move, -1, +1)
				    | line(move,  0, -1)
				    | line(move,  0, +1)
				    | line(move, +1, -1)
				    | line(move, +1,  0)
				    | line(move, +1, +1);
		ASSERT_EQ (PopCount(line(move, -1,  0) | line(move, +1,  0)), 7);
		ASSERT_EQ (PopCount(line(move,  0, -1) | line(move,  0, +1)), 7);
		ASSERT_EQ (PopCount(mask), maskPopCount[move]);
	}
}

void TestFlip(const uint8_t move)
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
		pos.MakeRandom(mask);
		ASSERT_EQ (flip(pos.P, pos.O, move), flip_loop(pos.P, pos.O, move));
	}
}

TEST (FlipFastTest, Move_A1) { TestFlip( 0); }
TEST (FlipFastTest, Move_B1) { TestFlip( 1); }
TEST (FlipFastTest, Move_C1) { TestFlip( 2); }
TEST (FlipFastTest, Move_D1) { TestFlip( 3); }
TEST (FlipFastTest, Move_E1) { TestFlip( 4); }
TEST (FlipFastTest, Move_F1) { TestFlip( 5); }
TEST (FlipFastTest, Move_G1) { TestFlip( 6); }
TEST (FlipFastTest, Move_H1) { TestFlip( 7); }
TEST (FlipFastTest, Move_A2) { TestFlip( 8); }
TEST (FlipFastTest, Move_B2) { TestFlip( 9); }
TEST (FlipFastTest, Move_C2) { TestFlip(10); }
TEST (FlipFastTest, Move_D2) { TestFlip(11); }
TEST (FlipFastTest, Move_E2) { TestFlip(12); }
TEST (FlipFastTest, Move_F2) { TestFlip(13); }
TEST (FlipFastTest, Move_G2) { TestFlip(14); }
TEST (FlipFastTest, Move_H2) { TestFlip(15); }
TEST (FlipFastTest, Move_A3) { TestFlip(16); }
TEST (FlipFastTest, Move_B3) { TestFlip(17); }
TEST (FlipFastTest, Move_C3) { TestFlip(18); }
TEST (FlipFastTest, Move_D3) { TestFlip(19); }
TEST (FlipFastTest, Move_E3) { TestFlip(20); }
TEST (FlipFastTest, Move_F3) { TestFlip(21); }
TEST (FlipFastTest, Move_G3) { TestFlip(22); }
TEST (FlipFastTest, Move_H3) { TestFlip(23); }
TEST (FlipFastTest, Move_A4) { TestFlip(24); }
TEST (FlipFastTest, Move_B4) { TestFlip(25); }
TEST (FlipFastTest, Move_C4) { TestFlip(26); }
TEST (FlipFastTest, Move_D4) { TestFlip(27); }
TEST (FlipFastTest, Move_E4) { TestFlip(28); }
TEST (FlipFastTest, Move_F4) { TestFlip(29); }
TEST (FlipFastTest, Move_G4) { TestFlip(30); }
TEST (FlipFastTest, Move_H4) { TestFlip(31); }
TEST (FlipFastTest, Move_A5) { TestFlip(32); }
TEST (FlipFastTest, Move_B5) { TestFlip(33); }
TEST (FlipFastTest, Move_C5) { TestFlip(34); }
TEST (FlipFastTest, Move_D5) { TestFlip(35); }
TEST (FlipFastTest, Move_E5) { TestFlip(36); }
TEST (FlipFastTest, Move_F5) { TestFlip(37); }
TEST (FlipFastTest, Move_G5) { TestFlip(38); }
TEST (FlipFastTest, Move_H5) { TestFlip(39); }
TEST (FlipFastTest, Move_A6) { TestFlip(40); }
TEST (FlipFastTest, Move_B6) { TestFlip(41); }
TEST (FlipFastTest, Move_C6) { TestFlip(42); }
TEST (FlipFastTest, Move_D6) { TestFlip(43); }
TEST (FlipFastTest, Move_E6) { TestFlip(44); }
TEST (FlipFastTest, Move_F6) { TestFlip(45); }
TEST (FlipFastTest, Move_G6) { TestFlip(46); }
TEST (FlipFastTest, Move_H6) { TestFlip(47); }
TEST (FlipFastTest, Move_A7) { TestFlip(48); }
TEST (FlipFastTest, Move_B7) { TestFlip(49); }
TEST (FlipFastTest, Move_C7) { TestFlip(50); }
TEST (FlipFastTest, Move_D7) { TestFlip(51); }
TEST (FlipFastTest, Move_E7) { TestFlip(52); }
TEST (FlipFastTest, Move_F7) { TestFlip(53); }
TEST (FlipFastTest, Move_G7) { TestFlip(54); }
TEST (FlipFastTest, Move_H7) { TestFlip(55); }
TEST (FlipFastTest, Move_A8) { TestFlip(56); }
TEST (FlipFastTest, Move_B8) { TestFlip(57); }
TEST (FlipFastTest, Move_C8) { TestFlip(58); }
TEST (FlipFastTest, Move_D8) { TestFlip(59); }
TEST (FlipFastTest, Move_E8) { TestFlip(60); }
TEST (FlipFastTest, Move_F8) { TestFlip(61); }
TEST (FlipFastTest, Move_G8) { TestFlip(62); }
TEST (FlipFastTest, Move_H8) { TestFlip(63); }
TEST (FlipFastTest, Move_pass) { TestFlip(64); }

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
