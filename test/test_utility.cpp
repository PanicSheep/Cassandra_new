#include "utility.h"
#include "gtest/gtest.h"

TEST (UtilityTest, ABS) {
	ASSERT_EQ (ABS(-1), 1);
	ASSERT_EQ (ABS(-2), 2);
	ASSERT_EQ (ABS(-0), 0);
	ASSERT_EQ (ABS(+0), 0);
	ASSERT_EQ (ABS(+1), 1);
	ASSERT_EQ (ABS(+2), 2);
}

TEST (UtilityTest, MIN) {
	ASSERT_EQ (MIN(0, 1), 0);
}

TEST (UtilityTest, MAX) {
	ASSERT_EQ (MAX(0, 1), 1);
}

TEST (UtilityTest, CLAMP) {
	ASSERT_EQ (CLAMP(-2, -1, +1), -1);
	ASSERT_EQ (CLAMP(-1, -1, +1), -1);
	ASSERT_EQ (CLAMP( 0, -1, +1),  0);
	ASSERT_EQ (CLAMP(+1, -1, +1), +1);
	ASSERT_EQ (CLAMP(+2, -1, +1), +1);
}

TEST (UtilityTest, RoundInt) {
	ASSERT_EQ (RoundInt(-1.1f), -1);
	ASSERT_EQ (RoundInt(-1.0f), -1);
	ASSERT_EQ (RoundInt(-0.9f), -1);
	ASSERT_EQ (RoundInt(-0.8f), -1);
	ASSERT_EQ (RoundInt(-0.7f), -1);
	ASSERT_EQ (RoundInt(-0.6f), -1);
	ASSERT_EQ (RoundInt(-0.5f), -1);
	ASSERT_EQ (RoundInt(-0.4f),  0);
	ASSERT_EQ (RoundInt(-0.3f),  0);
	ASSERT_EQ (RoundInt(-0.2f),  0);
	ASSERT_EQ (RoundInt(-0.1f),  0);
	ASSERT_EQ (RoundInt( 0.0f),  0);
	ASSERT_EQ (RoundInt( 0.1f),  0);
	ASSERT_EQ (RoundInt( 0.2f),  0);
	ASSERT_EQ (RoundInt( 0.3f),  0);
	ASSERT_EQ (RoundInt( 0.4f),  0);
	ASSERT_EQ (RoundInt( 0.5f),  1);
	ASSERT_EQ (RoundInt( 0.6f),  1);
	ASSERT_EQ (RoundInt( 0.7f),  1);
	ASSERT_EQ (RoundInt( 0.8f),  1);
	ASSERT_EQ (RoundInt( 0.9f),  1);
	ASSERT_EQ (RoundInt( 1.0f),  1);
	ASSERT_EQ (RoundInt( 1.1f),  1);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
