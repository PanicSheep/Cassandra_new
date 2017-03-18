#include "macros_hell.h"
#include "flip_loop.h"
#include "flip_fast.h"
#include "gtest/gtest.h"

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

	ASSERT_EQ (PopCount(mask), 21);
	for (int i = 0; i < (1 << 21); i++)
	{
		const uint64_t P = PDep(i, mask);
		const int max_j = 1 << (21 - PopCount(i));
		for (int j = 0; j < max_j; j++)
		{
			const uint64_t O = PDep(j, mask ^ P);
			ASSERT_EQ (flip(P, O, move), flip_loop(P, O, move));
		}
	}
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

TEST (FlipFastTest, Move_00) { TestFlip( 0); }
TEST (FlipFastTest, Move_01) { TestFlip( 1); }
TEST (FlipFastTest, Move_02) { TestFlip( 2); }
TEST (FlipFastTest, Move_03) { TestFlip( 3); }
TEST (FlipFastTest, Move_04) { TestFlip( 4); }
TEST (FlipFastTest, Move_05) { TestFlip( 5); }
TEST (FlipFastTest, Move_06) { TestFlip( 6); }
TEST (FlipFastTest, Move_07) { TestFlip( 7); }
TEST (FlipFastTest, Move_08) { TestFlip( 8); }
TEST (FlipFastTest, Move_09) { TestFlip( 9); }
TEST (FlipFastTest, Move_10) { TestFlip(10); }
TEST (FlipFastTest, Move_11) { TestFlip(11); }
TEST (FlipFastTest, Move_12) { TestFlip(12); }
TEST (FlipFastTest, Move_13) { TestFlip(13); }
TEST (FlipFastTest, Move_14) { TestFlip(14); }
TEST (FlipFastTest, Move_15) { TestFlip(15); }
TEST (FlipFastTest, Move_16) { TestFlip(16); }
TEST (FlipFastTest, Move_17) { TestFlip(17); }
TEST (FlipFastTest, Move_18) { TestFlip(18); }
TEST (FlipFastTest, Move_19) { TestFlip(19); }
TEST (FlipFastTest, Move_20) { TestFlip(20); }
TEST (FlipFastTest, Move_21) { TestFlip(21); }
TEST (FlipFastTest, Move_22) { TestFlip(22); }
TEST (FlipFastTest, Move_23) { TestFlip(23); }
TEST (FlipFastTest, Move_24) { TestFlip(24); }
TEST (FlipFastTest, Move_25) { TestFlip(25); }
TEST (FlipFastTest, Move_26) { TestFlip(26); }
TEST (FlipFastTest, Move_27) { TestFlip(27); }
TEST (FlipFastTest, Move_28) { TestFlip(28); }
TEST (FlipFastTest, Move_29) { TestFlip(29); }
TEST (FlipFastTest, Move_30) { TestFlip(30); }
TEST (FlipFastTest, Move_31) { TestFlip(31); }
TEST (FlipFastTest, Move_32) { TestFlip(32); }
TEST (FlipFastTest, Move_33) { TestFlip(33); }
TEST (FlipFastTest, Move_34) { TestFlip(34); }
TEST (FlipFastTest, Move_35) { TestFlip(35); }
TEST (FlipFastTest, Move_36) { TestFlip(36); }
TEST (FlipFastTest, Move_37) { TestFlip(37); }
TEST (FlipFastTest, Move_38) { TestFlip(38); }
TEST (FlipFastTest, Move_39) { TestFlip(39); }
TEST (FlipFastTest, Move_40) { TestFlip(40); }
TEST (FlipFastTest, Move_41) { TestFlip(41); }
TEST (FlipFastTest, Move_42) { TestFlip(42); }
TEST (FlipFastTest, Move_43) { TestFlip(43); }
TEST (FlipFastTest, Move_44) { TestFlip(44); }
TEST (FlipFastTest, Move_45) { TestFlip(45); }
TEST (FlipFastTest, Move_46) { TestFlip(46); }
TEST (FlipFastTest, Move_47) { TestFlip(47); }
TEST (FlipFastTest, Move_48) { TestFlip(48); }
TEST (FlipFastTest, Move_49) { TestFlip(49); }
TEST (FlipFastTest, Move_50) { TestFlip(50); }
TEST (FlipFastTest, Move_51) { TestFlip(51); }
TEST (FlipFastTest, Move_52) { TestFlip(52); }
TEST (FlipFastTest, Move_53) { TestFlip(53); }
TEST (FlipFastTest, Move_54) { TestFlip(54); }
TEST (FlipFastTest, Move_55) { TestFlip(55); }
TEST (FlipFastTest, Move_56) { TestFlip(56); }
TEST (FlipFastTest, Move_57) { TestFlip(57); }
TEST (FlipFastTest, Move_58) { TestFlip(58); }
TEST (FlipFastTest, Move_59) { TestFlip(59); }
TEST (FlipFastTest, Move_60) { TestFlip(60); }
TEST (FlipFastTest, Move_61) { TestFlip(61); }
TEST (FlipFastTest, Move_62) { TestFlip(62); }
TEST (FlipFastTest, Move_63) { TestFlip(63); }
TEST (FlipFastTest, Move_pass) { TestFlip(64); }

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
