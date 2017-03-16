#include <iostream>
#include "flip_loop.h"
#include "gtest/gtest.h"

TEST (FlipLoopTest, OneFlip) {
	const uint64_t P = 0xFF00000000000000ULL;
	const uint64_t O = 0x00FFFFFFFFFFFF7EULL;
	const uint8_t move = 0;
	const uint64_t flip = flip_loop(P, O, move);
	
	ASSERT_EQ (P & flip, 0);
	ASSERT_NE (O & flip, flip);
	ASSERT_EQ (flip, 0x008141211109050300ULL);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
