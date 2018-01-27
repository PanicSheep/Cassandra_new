#include "perft_basic.h"
#include "gtest/gtest.h"

TEST (PerftTest, Basic) {
	for (uint8_t depth = 0; depth < 11; depth++)
		ASSERT_EQ (Perft_Basic::perft(CPosition::StartPosition().P, CPosition::StartPosition().O, depth), Perft::Correct(depth));
}

TEST (PerftTest, Unroll2) {
	for (uint8_t depth = 2; depth < 12; depth++)
		ASSERT_EQ (Perft_Unroll2::perft(CPosition::StartPosition().P, CPosition::StartPosition().O, depth), Perft::Correct(depth));
}


int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);	
	return RUN_ALL_TESTS();
}
