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

TEST (GeneratePositionsTest, GeneratePerftPositions1) {
	const uint8_t numEmpties = 55;
	const bool ETH = false;
	
	auto RndPos = GeneratePerftPositions(numEmpties, ETH);
	
	for (const auto& it : RndPos)
		ASSERT_EQ (it.EmptyCount(), numEmpties);
}

TEST (GeneratePositionsTest, GeneratePerftPositions2) {
	auto RndPos = GeneratePerftPositions(59, false);
	ASSERT_EQ (GeneratePerftPositions(60, false).size(),      1u);
	ASSERT_EQ (GeneratePerftPositions(59, false).size(),      1u);
	ASSERT_EQ (GeneratePerftPositions(58, false).size(),      3u);
	ASSERT_EQ (GeneratePerftPositions(57, false).size(),     14u);
	ASSERT_EQ (GeneratePerftPositions(56, false).size(),     60u);
	ASSERT_EQ (GeneratePerftPositions(55, false).size(),    322u);
	ASSERT_EQ (GeneratePerftPositions(54, false).size(),   1773u);
	ASSERT_EQ (GeneratePerftPositions(53, false).size(),  10649u);
	ASSERT_EQ (GeneratePerftPositions(52, false).size(),  67245u);
	ASSERT_EQ (GeneratePerftPositions(51, false).size(), 434029u);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
