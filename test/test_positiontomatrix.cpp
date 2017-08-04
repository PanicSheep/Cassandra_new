#include "macros_hell.h"
#include "gtest/gtest.h"
#include "position.h"
#include "generatepositions.h"
#include "pattern.h"
#include "positiontomatrix.h"
#include <cstdint>
#include <map>

void TestPattern(const uint64_t pattern)
{
	// ------ set up ---------------
	auto rnd = std::bind(std::uniform_int_distribution<int>(-64, 64), std::mt19937_64(17));
	std::vector<CPositionScore> pos;
	Pattern::CPattern * pat = Pattern::NewPattern("none", pattern);
	auto occurences = pat->Occurrences();
	delete pat;
	
	auto rndposset = GenerateRandomPositions(100000, 20);
	std::vector<CPosition> rndpos(rndposset.begin(), rndposset.end());
	
	for (std::size_t i = 0; i < rndpos.size(); i++)
		pos.push_back(CPositionScore(rndpos[i].P, rndpos[i].O, rnd()));
		
	// ------ run ----------------------
	std::pair<CMatrix_CSR<int8_t, uint32_t>, std::vector<float>> ret = PositionToMatrix<int8_t, uint32_t, float>(pos, pattern);
	
	// ------ histograms ---------------
	std::map<int, int> Histogram1;
	for (std::size_t i = 0; i < rndpos.size(); i++)
		Histogram1[pos[i].score]++;
	
	std::map<int, int> Histogram2;
	for (std::size_t i = 0; i < rndpos.size(); i++)
		Histogram2[ret.second[i]]++;
	
	// ------ test ----------------------
	for (const auto& it : Histogram1)
		ASSERT_EQ (Histogram2[it.first], it.second);
	for (const auto& it : Histogram2)
		ASSERT_EQ (Histogram1[it.first], it.second);
	
	{
		std::vector<float> x(ret.first.m(), 1);
		std::vector<float> b(ret.first.n(), occurences);
		auto Ax = ret.first * x;
		ASSERT_EQ (Ax, b);
	}
	
	{
		std::vector<float> x(ret.first.n(), 1);
		auto ATx = ret.first.ATx(x);
		float nnz = 0;
		for (const auto& it : ATx)
			if (it != 0)
				nnz++;
		ASSERT_TRUE (nnz > 1);
	}
}

TEST (PositionToMatrixTest, Test1) { TestPattern(0x00000000000000FFULL); }
TEST (PositionToMatrixTest, Test2) { TestPattern(0x000000000000FF00ULL); }
TEST (PositionToMatrixTest, Test3) { TestPattern(0x0000000000FF0000ULL); }
TEST (PositionToMatrixTest, Test4) { TestPattern(0x00000000FF000000ULL); }
TEST (PositionToMatrixTest, Test5) { TestPattern(0x0000000000010204ULL); }
TEST (PositionToMatrixTest, Test6) { TestPattern(0x0000000001020408ULL); }
TEST (PositionToMatrixTest, Test7) { TestPattern(0x0000000102040810ULL); }
TEST (PositionToMatrixTest, Test8) { TestPattern(0x0000010204081020ULL); }
TEST (PositionToMatrixTest, Test9) { TestPattern(0x0001020408102040ULL); }
TEST (PositionToMatrixTest, Test10) { TestPattern(0x0102040810204080ULL); }
TEST (PositionToMatrixTest, Test11) { TestPattern(0x0000000000000F0FULL); }
TEST (PositionToMatrixTest, Test12) { TestPattern(0x0000000000001F1FULL); }
TEST (PositionToMatrixTest, Test13) { TestPattern(0x0000000000003F3FULL); }
TEST (PositionToMatrixTest, Test14) { TestPattern(0x0000000000070707ULL); }
TEST (PositionToMatrixTest, Test15) { TestPattern(0x0000000000003CBDULL); }
TEST (PositionToMatrixTest, Test16) { TestPattern(0x0000000000003CFFULL); }
TEST (PositionToMatrixTest, Test17) { TestPattern(0x0000000000010307ULL); }
TEST (PositionToMatrixTest, Test18) { TestPattern(0x000000000103070FULL); }
TEST (PositionToMatrixTest, Test19) { TestPattern(0x0000000103070F1FULL); }
TEST (PositionToMatrixTest, Test20) { TestPattern(0x000000000101030FULL); }
TEST (PositionToMatrixTest, Test21) { TestPattern(0x000000010101031FULL); }
TEST (PositionToMatrixTest, Test22) { TestPattern(0x000001010101033FULL); }
TEST (PositionToMatrixTest, Test23) { TestPattern(0x000000010103071FULL); }
TEST (PositionToMatrixTest, Test24) { TestPattern(0x00000000000042FFULL); }
TEST (PositionToMatrixTest, Test25) { TestPattern(0x81000000000000FFULL); }
TEST (PositionToMatrixTest, Test26) { TestPattern(0x00000000000081FFULL); }
TEST (PositionToMatrixTest, Test27) { TestPattern(0x81000000000042FFULL); }
TEST (PositionToMatrixTest, Test28) { TestPattern(0x000000000000C3FFULL); }
TEST (PositionToMatrixTest, Test29) { TestPattern(0x8040201008040303ULL); }
TEST (PositionToMatrixTest, Test30) { TestPattern(0x8040201008050307ULL); }
TEST (PositionToMatrixTest, Test31) { TestPattern(0x804020100905030FULL); }
TEST (PositionToMatrixTest, Test32) { TestPattern(0x0000000000000001ULL); }
TEST (PositionToMatrixTest, Test33) { TestPattern(0x0000000000000002ULL); }
TEST (PositionToMatrixTest, Test34) { TestPattern(0x0000000000000004ULL); }
TEST (PositionToMatrixTest, Test35) { TestPattern(0x0000000000000008ULL); }
TEST (PositionToMatrixTest, Test36) { TestPattern(0x0000000000000200ULL); }
TEST (PositionToMatrixTest, Test37) { TestPattern(0x0000000000000400ULL); }
TEST (PositionToMatrixTest, Test38) { TestPattern(0x0000000000000800ULL); }
TEST (PositionToMatrixTest, Test39) { TestPattern(0x0000000000040000ULL); }
TEST (PositionToMatrixTest, Test40) { TestPattern(0x0000000000080000ULL); }
TEST (PositionToMatrixTest, Test41) { TestPattern(0x0000000008000000ULL); }


int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	Pattern::Initialize(false, false);
	return RUN_ALL_TESTS();
}
