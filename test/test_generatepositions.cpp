#include "generatepositions.h"
#include "gtest/gtest.h"

TEST (GeneratePositionsTest, GenerateRandomPositions) {
	const std::size_t numPos = 1000;
	const uint8_t numEmpties = 30;
	const bool ETH = false;
	
	auto RndPos = GenerateRandomPositions(numPos, numEmpties, ETH);
	
	ASSERT_EQ (RndPos.size(), numPos);
	for (const auto& it : RndPos)
		ASSERT_EQ (it.EmptyCount(), numEmpties);
}

TEST (GeneratePositionsTest, GenerateAllPositions) {
	const uint8_t numEmpties = 55;
	const bool ETH = false;
	
	auto RndPos = GenerateAllPositions(numEmpties, ETH);
	
	for (const auto& it : RndPos)
		ASSERT_EQ (it.EmptyCount(), numEmpties);
}

TEST (GeneratePositionsTest, All60) { ASSERT_EQ (GenerateAllPositions(60, false).size(),      1u); }
TEST (GeneratePositionsTest, All59) { ASSERT_EQ (GenerateAllPositions(59, false).size(),      1u); }
TEST (GeneratePositionsTest, All58) { ASSERT_EQ (GenerateAllPositions(58, false).size(),      3u); }
TEST (GeneratePositionsTest, All57) { ASSERT_EQ (GenerateAllPositions(57, false).size(),     14u); }
TEST (GeneratePositionsTest, All56) { ASSERT_EQ (GenerateAllPositions(56, false).size(),     60u); }
TEST (GeneratePositionsTest, All55) { ASSERT_EQ (GenerateAllPositions(55, false).size(),    322u); }
TEST (GeneratePositionsTest, All54) { ASSERT_EQ (GenerateAllPositions(54, false).size(),   1773u); }
TEST (GeneratePositionsTest, All53) { ASSERT_EQ (GenerateAllPositions(53, false).size(),  10649u); }
TEST (GeneratePositionsTest, All52) { ASSERT_EQ (GenerateAllPositions(52, false).size(),  67245u); }
TEST (GeneratePositionsTest, All51) { ASSERT_EQ (GenerateAllPositions(51, false).size(), 434029u); }

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
