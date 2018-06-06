#include "pch.h"
#include "Search.h"
#include "NegaMaxSearch.h"
#include "AlphaBetaFailHardSearch.h"
#include "AlphaBetaFailSoftSearch.h"
#include "PVSearch.h"
#include "Stability.h"
#include "HashtablePVS.h"
#include "LastFlipCounter.h"

class CPositionScore : public CPosition
{
public:
	int score;
	CPositionScore(uint64_t P, uint64_t O, int score) : CPosition(P, O), score(score) {}
};

std::vector<CPositionScore> TestPos = {
	CPositionScore(0xFF00FF00FF00FF00ULL, 0x00FF00FF00FF00FFULL,   0), //  0
	CPositionScore(0xFF00FF00FF00FFFFULL, 0x00FF00FF00FF0000ULL, +16), //  1
	CPositionScore(0xFF00FF00FF000000ULL, 0x00FF00FF00FFFFFFULL, -16), //  2

	CPositionScore(0xFFFFFFFFFFFFFFFEULL, 0x0000000000000000ULL, +64), //  3
	CPositionScore(0xFFFFFFFFFFFFFCFCULL, 0x0000000000000302ULL, +64), //  4
	CPositionScore(0xFFFFFFFFFFFFFF7EULL, 0x0000000000000080ULL, +48), //  5
	CPositionScore(0xFFFFFFFFFFFFFE7EULL, 0x0000000000000180ULL, +62), //  6

	CPositionScore(0xFEFFFFFFFFFFFFFEULL, 0x0000000000000000ULL, +64), //  7
	CPositionScore(0xFCFCFFFFFFFFFCFCULL, 0x0203000000000302ULL, +64), //  8
	CPositionScore(0xFEFFFFFFFFFFFF7EULL, 0x0000000000000080ULL, +22), //  9
	CPositionScore(0xFFFFFFFFFBFEFC7EULL, 0x0000000004010280ULL, +54), // 10

	CPositionScore(0xFFFFFFFFFFFFFFF8ULL, 0x0000000000000000ULL, +64), // 11
	CPositionScore(0xFFFFFFFFFFFFFDF8ULL, 0x0000000000000200ULL, +64), // 12
	CPositionScore(0xF8FFFFFFFFFFFFF8ULL, 0x0700000000000000ULL, +16), // 13
	CPositionScore(0xFFFFFFFEFBFEFC7EULL, 0x0000000104000280ULL, +58), // 14

	CPositionScore(0xFFFFFFFFFFFFFFF0ULL, 0x0000000000000000ULL, +64), // 15
	CPositionScore(0xFFFFFFFFFFFFE0E0ULL, 0x0000000000001F10ULL, +64), // 16
	CPositionScore(0xF0FFFFFFFFFFFFF0ULL, 0x0F00000000000000ULL,  +0), // 17
	CPositionScore(0xFFFFFFFFFFE7DBC3ULL, 0x0000000000182400ULL, +56), // 18

	// FForum
	CPositionScore(0x3E0C0D2B772B7000ULL, 0x0071725488140E7CULL, +18), // 19
	CPositionScore(0x7E2059000C19303EULL, 0x001E267FF3E60C00ULL, +10), // 20
	CPositionScore(0x040C0D306C707A01ULL, 0x083070CF938F853EULL,  +2), // 21
	CPositionScore(0x7EB8B9D127070C2CULL, 0x0006462E58383012ULL,  +0), // 22
	CPositionScore(0x0019D6D6C8F0A07CULL, 0x7C242829370D1C00ULL, +32), // 23
	CPositionScore(0x1C1C14223F9A047EULL, 0x20E0EADCC0647800ULL, +14), // 24
	CPositionScore(0x18BC8F9F877F3018ULL, 0x2440706078000E06ULL,  +8), // 25
	CPositionScore(0x0000081F22426064ULL, 0x14BCF7E0DCBC9C98ULL,  +8), // 26
	CPositionScore(0x10B070222E942C3CULL, 0x2C0F0F5DD16B1000ULL,  -8), // 27
	CPositionScore(0x002054A49716247CULL, 0x781CAB5B68E91800ULL, +10), // 28
};

void TestTestPos(unsigned int emptyCount, unsigned int MobilityPlayer, unsigned int MobilityOpponent, unsigned int index)
{
	ASSERT_TRUE(TestPos[index].score >= -64);
	ASSERT_TRUE(TestPos[index].score <= +64);
	ASSERT_TRUE((TestPos[index].GetP() & TestPos[index].GetO()) == 0ULL);
	ASSERT_EQ(TestPos[index].EmptyCount(), emptyCount);
	ASSERT_EQ(TestPos[index].PossibleMoves().size(), MobilityPlayer);
	ASSERT_EQ(TestPos[index].PlayPass().PossibleMoves().size(), MobilityOpponent);
}

TEST(MetaTest, TestPos00) { TestTestPos(0, 0, 0,  0); }
TEST(MetaTest, TestPos01) { TestTestPos(0, 0, 0,  1); }
TEST(MetaTest, TestPos02) { TestTestPos(0, 0, 0,  2); }
TEST(MetaTest, TestPos03) { TestTestPos(1, 0, 0,  3); }
TEST(MetaTest, TestPos04) { TestTestPos(1, 1, 0,  4); }
TEST(MetaTest, TestPos05) { TestTestPos(1, 0, 1,  5); }
TEST(MetaTest, TestPos06) { TestTestPos(1, 1, 1,  6); }
TEST(MetaTest, TestPos07) { TestTestPos(2, 0, 0,  7); }
TEST(MetaTest, TestPos08) { TestTestPos(2, 2, 0,  8); }
TEST(MetaTest, TestPos09) { TestTestPos(2, 0, 2,  9); }
TEST(MetaTest, TestPos10) { TestTestPos(2, 2, 2, 10); }
TEST(MetaTest, TestPos11) { TestTestPos(3, 0, 0, 11); }
TEST(MetaTest, TestPos12) { TestTestPos(3, 3, 0, 12); }
TEST(MetaTest, TestPos13) { TestTestPos(3, 0, 3, 13); }
TEST(MetaTest, TestPos14) { TestTestPos(3, 3, 3, 14); }
TEST(MetaTest, TestPos15) { TestTestPos(4, 0, 0, 15); }
TEST(MetaTest, TestPos16) { TestTestPos(4, 4, 0, 16); }
TEST(MetaTest, TestPos17) { TestTestPos(4, 0, 4, 17); }
TEST(MetaTest, TestPos18) { TestTestPos(4, 4, 4, 18); }

void TestPosition(unsigned int index, Search& search)
{
	const int score = search.Eval(TestPos[index]);

	ASSERT_EQ(score, TestPos[index].score);
}

class NegaMax : public ::testing::Test
{
protected:
	static std::shared_ptr<Environment> environment;

	static void SetUpTestCase()
	{
		std::shared_ptr<ILastFlipCounter> LastFlipCounter = std::make_shared<CLastFlipCounter>();
		std::shared_ptr<IHashTable<CPosition, PvsInfo>> HashTable = nullptr; // TODO: Replace!
		std::shared_ptr<IStabilityAnalyzer> StabilityAnalyzer = std::make_shared<CStabilityAnalyzer>();
		std::shared_ptr<IPattern> PatternEvaluator = nullptr; // TODO: Replace!

		environment = std::make_shared<Environment>(nullptr, LastFlipCounter, HashTable, StabilityAnalyzer, PatternEvaluator);
	}
};

std::shared_ptr<Environment> NegaMax::environment;

TEST_F(NegaMax, ZeroEmpties0)  { TestPosition( 0, NegaMaxSearch(environment)); }
TEST_F(NegaMax, ZeroEmpties1)  { TestPosition( 1, NegaMaxSearch(environment)); }
TEST_F(NegaMax, ZeroEmpties2)  { TestPosition( 2, NegaMaxSearch(environment)); }
TEST_F(NegaMax, OneEmpties0)   { TestPosition( 3, NegaMaxSearch(environment)); }
TEST_F(NegaMax, OneEmpties1)   { TestPosition( 4, NegaMaxSearch(environment)); }
TEST_F(NegaMax, OneEmpties2)   { TestPosition( 5, NegaMaxSearch(environment)); }
TEST_F(NegaMax, OneEmpties3)   { TestPosition( 6, NegaMaxSearch(environment)); }
TEST_F(NegaMax, TwoEmpties0)   { TestPosition( 7, NegaMaxSearch(environment)); }
TEST_F(NegaMax, TwoEmpties1)   { TestPosition( 8, NegaMaxSearch(environment)); }
TEST_F(NegaMax, TwoEmpties2)   { TestPosition( 9, NegaMaxSearch(environment)); }
TEST_F(NegaMax, TwoEmpties3)   { TestPosition(10, NegaMaxSearch(environment)); }
TEST_F(NegaMax, ThreeEmpties0) { TestPosition(11, NegaMaxSearch(environment)); }
TEST_F(NegaMax, ThreeEmpties1) { TestPosition(12, NegaMaxSearch(environment)); }
TEST_F(NegaMax, ThreeEmpties2) { TestPosition(13, NegaMaxSearch(environment)); }
TEST_F(NegaMax, ThreeEmpties3) { TestPosition(14, NegaMaxSearch(environment)); }
TEST_F(NegaMax, FourEmpties0)  { TestPosition(15, NegaMaxSearch(environment)); }
TEST_F(NegaMax, FourEmpties1)  { TestPosition(16, NegaMaxSearch(environment)); }
TEST_F(NegaMax, FourEmpties2)  { TestPosition(17, NegaMaxSearch(environment)); }
TEST_F(NegaMax, FourEmpties3)  { TestPosition(18, NegaMaxSearch(environment)); }

class AlphaBetaFailHard : public ::testing::Test
{
protected:
	static std::shared_ptr<Environment> environment;

	static void SetUpTestCase()
	{
		std::shared_ptr<ILastFlipCounter> LastFlipCounter = std::make_shared<CLastFlipCounter>();
		std::shared_ptr<IHashTable<CPosition, PvsInfo>> HashTable = nullptr; // TODO: Replace!
		std::shared_ptr<IStabilityAnalyzer> StabilityAnalyzer = std::make_shared<CStabilityAnalyzer>();
		std::shared_ptr<IPattern> PatternEvaluator = nullptr; // TODO: Replace!

		environment = std::make_shared<Environment>(nullptr, LastFlipCounter, HashTable, StabilityAnalyzer, PatternEvaluator);
	}
};

std::shared_ptr<Environment> AlphaBetaFailHard::environment;

TEST_F(AlphaBetaFailHard, ZeroEmpties0)  { TestPosition( 0, AlphaBetaFailHardSearch(environment)); }
TEST_F(AlphaBetaFailHard, ZeroEmpties1)  { TestPosition( 1, AlphaBetaFailHardSearch(environment)); }
TEST_F(AlphaBetaFailHard, ZeroEmpties2)  { TestPosition( 2, AlphaBetaFailHardSearch(environment)); }
TEST_F(AlphaBetaFailHard, OneEmpties0)   { TestPosition( 3, AlphaBetaFailHardSearch(environment)); }
TEST_F(AlphaBetaFailHard, OneEmpties1)   { TestPosition( 4, AlphaBetaFailHardSearch(environment)); }
TEST_F(AlphaBetaFailHard, OneEmpties2)   { TestPosition( 5, AlphaBetaFailHardSearch(environment)); }
TEST_F(AlphaBetaFailHard, OneEmpties3)   { TestPosition( 6, AlphaBetaFailHardSearch(environment)); }
TEST_F(AlphaBetaFailHard, TwoEmpties0)   { TestPosition( 7, AlphaBetaFailHardSearch(environment)); }
TEST_F(AlphaBetaFailHard, TwoEmpties1)   { TestPosition( 8, AlphaBetaFailHardSearch(environment)); }
TEST_F(AlphaBetaFailHard, TwoEmpties2)   { TestPosition( 9, AlphaBetaFailHardSearch(environment)); }
TEST_F(AlphaBetaFailHard, TwoEmpties3)   { TestPosition(10, AlphaBetaFailHardSearch(environment)); }
TEST_F(AlphaBetaFailHard, ThreeEmpties0) { TestPosition(11, AlphaBetaFailHardSearch(environment)); }
TEST_F(AlphaBetaFailHard, ThreeEmpties1) { TestPosition(12, AlphaBetaFailHardSearch(environment)); }
TEST_F(AlphaBetaFailHard, ThreeEmpties2) { TestPosition(13, AlphaBetaFailHardSearch(environment)); }
TEST_F(AlphaBetaFailHard, ThreeEmpties3) { TestPosition(14, AlphaBetaFailHardSearch(environment)); }
TEST_F(AlphaBetaFailHard, FourEmpties0)  { TestPosition(15, AlphaBetaFailHardSearch(environment)); }
TEST_F(AlphaBetaFailHard, FourEmpties1)  { TestPosition(16, AlphaBetaFailHardSearch(environment)); }
TEST_F(AlphaBetaFailHard, FourEmpties2)  { TestPosition(17, AlphaBetaFailHardSearch(environment)); }
TEST_F(AlphaBetaFailHard, FourEmpties3)  { TestPosition(18, AlphaBetaFailHardSearch(environment)); }

class AlphaBetaFailSoft : public ::testing::Test
{
protected:
	static std::shared_ptr<Environment> environment;

	static void SetUpTestCase()
	{
		std::shared_ptr<ILastFlipCounter> LastFlipCounter = std::make_shared<CLastFlipCounter>();
		std::shared_ptr<IHashTable<CPosition, PvsInfo>> HashTable = nullptr; // TODO: Replace!
		std::shared_ptr<IStabilityAnalyzer> StabilityAnalyzer = std::make_shared<CStabilityAnalyzer>();
		std::shared_ptr<IPattern> PatternEvaluator = nullptr; // TODO: Replace!

		environment = std::make_shared<Environment>(nullptr, LastFlipCounter, HashTable, StabilityAnalyzer, PatternEvaluator);
	}
};

std::shared_ptr<Environment> AlphaBetaFailSoft::environment;

TEST_F(AlphaBetaFailSoft, ZeroEmpties0)  { TestPosition( 0, AlphaBetaFailSoftSearch(environment)); }
TEST_F(AlphaBetaFailSoft, ZeroEmpties1)  { TestPosition( 1, AlphaBetaFailSoftSearch(environment)); }
TEST_F(AlphaBetaFailSoft, ZeroEmpties2)  { TestPosition( 2, AlphaBetaFailSoftSearch(environment)); }
TEST_F(AlphaBetaFailSoft, OneEmpties0)   { TestPosition( 3, AlphaBetaFailSoftSearch(environment)); }
TEST_F(AlphaBetaFailSoft, OneEmpties1)   { TestPosition( 4, AlphaBetaFailSoftSearch(environment)); }
TEST_F(AlphaBetaFailSoft, OneEmpties2)   { TestPosition( 5, AlphaBetaFailSoftSearch(environment)); }
TEST_F(AlphaBetaFailSoft, OneEmpties3)   { TestPosition( 6, AlphaBetaFailSoftSearch(environment)); }
TEST_F(AlphaBetaFailSoft, TwoEmpties0)   { TestPosition( 7, AlphaBetaFailSoftSearch(environment)); }
TEST_F(AlphaBetaFailSoft, TwoEmpties1)   { TestPosition( 8, AlphaBetaFailSoftSearch(environment)); }
TEST_F(AlphaBetaFailSoft, TwoEmpties2)   { TestPosition( 9, AlphaBetaFailSoftSearch(environment)); }
TEST_F(AlphaBetaFailSoft, TwoEmpties3)   { TestPosition(10, AlphaBetaFailSoftSearch(environment)); }
TEST_F(AlphaBetaFailSoft, ThreeEmpties0) { TestPosition(11, AlphaBetaFailSoftSearch(environment)); }
TEST_F(AlphaBetaFailSoft, ThreeEmpties1) { TestPosition(12, AlphaBetaFailSoftSearch(environment)); }
TEST_F(AlphaBetaFailSoft, ThreeEmpties2) { TestPosition(13, AlphaBetaFailSoftSearch(environment)); }
TEST_F(AlphaBetaFailSoft, ThreeEmpties3) { TestPosition(14, AlphaBetaFailSoftSearch(environment)); }
TEST_F(AlphaBetaFailSoft, FourEmpties0)  { TestPosition(15, AlphaBetaFailSoftSearch(environment)); }
TEST_F(AlphaBetaFailSoft, FourEmpties1)  { TestPosition(16, AlphaBetaFailSoftSearch(environment)); }
TEST_F(AlphaBetaFailSoft, FourEmpties2)  { TestPosition(17, AlphaBetaFailSoftSearch(environment)); }
TEST_F(AlphaBetaFailSoft, FourEmpties3)  { TestPosition(18, AlphaBetaFailSoftSearch(environment)); }

class PVS : public ::testing::Test
{
protected:
	static std::shared_ptr<Environment> environment;

	static void SetUpTestCase()
	{
		std::shared_ptr<ILastFlipCounter> LastFlipCounter = std::make_shared<CLastFlipCounter>();
		std::shared_ptr<IHashTable<CPosition, PvsInfo>> HashTable = std::make_shared<CHashTablePVS>(1'000);
		std::shared_ptr<IStabilityAnalyzer> StabilityAnalyzer = std::make_shared<CStabilityAnalyzer>(); // TODO: Replace!
		std::shared_ptr<IPattern> PatternEvaluator = nullptr; // TODO: Replace!

		environment = std::make_shared<Environment>(nullptr, LastFlipCounter, HashTable, StabilityAnalyzer, PatternEvaluator);
	}
};

std::shared_ptr<Environment> PVS::environment;

TEST_F(PVS, ZeroEmpties0)  { TestPosition( 0, PVSearch(environment)); }
TEST_F(PVS, ZeroEmpties1)  { TestPosition( 1, PVSearch(environment)); }
TEST_F(PVS, ZeroEmpties2)  { TestPosition( 2, PVSearch(environment)); }
TEST_F(PVS, OneEmpties0)   { TestPosition( 3, PVSearch(environment)); }
TEST_F(PVS, OneEmpties1)   { TestPosition( 4, PVSearch(environment)); }
TEST_F(PVS, OneEmpties2)   { TestPosition( 5, PVSearch(environment)); }
TEST_F(PVS, OneEmpties3)   { TestPosition( 6, PVSearch(environment)); }
TEST_F(PVS, TwoEmpties0)   { TestPosition( 7, PVSearch(environment)); }
TEST_F(PVS, TwoEmpties1)   { TestPosition( 8, PVSearch(environment)); }
TEST_F(PVS, TwoEmpties2)   { TestPosition( 9, PVSearch(environment)); }
TEST_F(PVS, TwoEmpties3)   { TestPosition(10, PVSearch(environment)); }
TEST_F(PVS, ThreeEmpties0) { TestPosition(11, PVSearch(environment)); }
TEST_F(PVS, ThreeEmpties1) { TestPosition(12, PVSearch(environment)); }
TEST_F(PVS, ThreeEmpties2) { TestPosition(13, PVSearch(environment)); }
TEST_F(PVS, ThreeEmpties3) { TestPosition(14, PVSearch(environment)); }
TEST_F(PVS, FourEmpties0)  { TestPosition(15, PVSearch(environment)); }
TEST_F(PVS, FourEmpties1)  { TestPosition(16, PVSearch(environment)); }
TEST_F(PVS, FourEmpties2)  { TestPosition(17, PVSearch(environment)); }
TEST_F(PVS, FourEmpties3)  { TestPosition(18, PVSearch(environment)); }

TEST_F(PVS, FForum01) { TestPosition(19, PVSearch(environment)); }
TEST_F(PVS, FForum02) { TestPosition(20, PVSearch(environment)); }
TEST_F(PVS, FForum03) { TestPosition(21, PVSearch(environment)); }
TEST_F(PVS, FForum04) { TestPosition(22, PVSearch(environment)); }
TEST_F(PVS, FForum05) { TestPosition(23, PVSearch(environment)); }
TEST_F(PVS, FForum06) { TestPosition(24, PVSearch(environment)); }
TEST_F(PVS, FForum07) { TestPosition(25, PVSearch(environment)); }
TEST_F(PVS, FForum08) { TestPosition(26, PVSearch(environment)); }
TEST_F(PVS, FForum09) { TestPosition(27, PVSearch(environment)); }
TEST_F(PVS, FForum10) { TestPosition(28, PVSearch(environment)); }


TEST(GetStableStones, none)
{
	const auto stables = CStabilityAnalyzer().GetStableStones(CPosition::StartPosition());

	ASSERT_EQ(stables, 0ULL);
}

TEST(GetStableStones, none2)
{
	const auto stables = CStabilityAnalyzer().GetStableStones(CPosition(0, 0xFF));

	ASSERT_EQ(stables, 0xFF);
}