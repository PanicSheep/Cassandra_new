#include "macros_hell.h"
#include "flip_loop.h"
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

TEST (FlipFastTest, Line) {
    for (uint8_t move = 0; move < 64; move++)
    {
        ASSERT_NE (PopCount(line(move, -1, -1)), 7);
        ASSERT_NE (PopCount(line(move, -1,  0)), 7);
        ASSERT_NE (PopCount(line(move, -1, +1)), 7);
        ASSERT_NE (PopCount(line(move,  0, -1)), 7);
        ASSERT_NE (PopCount(line(move,  0, +1)), 7);
        ASSERT_NE (PopCount(line(move, +1, -1)), 7);
        ASSERT_NE (PopCount(line(move, +1,  0)), 7);
        ASSERT_NE (PopCount(line(move, +1, +1)), 7);
    }
}

TEST (FlipFastTest, AllFlips) {
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
        
        ASSERT_EQ (PopCount(mask), 21);
        for (int i = 0; i < (1 << 21); i++)
        {
            const uint64_t P = PDep(i, mask);
            const int max_j = 1 << (21 - PopCount(i));
            for (int j = 0; j < max_j; j++)
            {
                const uint64_t O = PDep(j, mask ^ P);
                ASSERT_NE (flip(P, O, move), flip_loop(P, O, move))
            }
        }
    }
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
