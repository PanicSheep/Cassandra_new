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

TEST (UtilityTest, time_format) {
	//ddd:hh:mm:ss.ccc
	ASSERT_EQ (time_format(std::chrono::milliseconds( 0)), "           0.000");
	ASSERT_EQ (time_format(std::chrono::milliseconds( 1)), "           0.001");
	ASSERT_EQ (time_format(std::chrono::     seconds( 1)), "           1.000");
	ASSERT_EQ (time_format(std::chrono::     minutes( 1)), "        1:00.000");
	ASSERT_EQ (time_format(std::chrono::       hours( 1)), "     1:00:00.000");
	ASSERT_EQ (time_format(std::chrono::       hours(24)), "  1:00:00:00.000");
	
	auto time = std::chrono::hours(26) + std::chrono::minutes(41) + std::chrono::milliseconds(1);
	ASSERT_EQ (time_format(time), "  1:02:41:00.001");
}

TEST (UtilityTest, ThousandsSeparator) {
	ASSERT_EQ (ThousandsSeparator(      0),         "0");
	ASSERT_EQ (ThousandsSeparator(      1),         "1");
	ASSERT_EQ (ThousandsSeparator(     10),        "10");
	ASSERT_EQ (ThousandsSeparator(    100),       "100");
	ASSERT_EQ (ThousandsSeparator(   1000),     "1'000");
	ASSERT_EQ (ThousandsSeparator(  10000),    "10'000");
	ASSERT_EQ (ThousandsSeparator( 100000),   "100'000");
	ASSERT_EQ (ThousandsSeparator(1000000), "1'000'000");
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
