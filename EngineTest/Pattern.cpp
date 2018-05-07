#include "pch.h"
#include "Pattern.h"

class Pattern : public ::testing::Test
{
protected:
	static void SetUpTestCase()
	{
		CPattern::Initialize();
	}
};

TEST_F(Pattern, NewPatternH)
{
	const auto pattern = CreatePattern(0x000000000000007EULL);

	ASSERT_TRUE(dynamic_cast<CPatternH*>(pattern.get()) != nullptr);
	ASSERT_EQ(pattern->GetPattern(), 0x000000000000007EULL);
	ASSERT_EQ(pattern->Occurrences(), 4u);
	ASSERT_EQ(pattern->FullSize(), 729u);
	ASSERT_EQ(pattern->ReducedSize(), 378u);
}

TEST_F(Pattern, NewPatternD)
{
	const auto pattern = CreatePattern(0x0000000000000303ULL);

	ASSERT_TRUE(dynamic_cast<CPatternD*>(pattern.get()) != nullptr);
	ASSERT_EQ(pattern->GetPattern(), 0x0000000000000303ULL);
	ASSERT_EQ(pattern->Occurrences(), 4u);
	ASSERT_EQ(pattern->FullSize(), 81u);
	ASSERT_EQ(pattern->ReducedSize(), 54u);
}

TEST_F(Pattern, NewPattern0)
{
	const auto pattern = CreatePattern(0x000000100000000FULL);

	ASSERT_TRUE(dynamic_cast<CPattern0*>(pattern.get()) != nullptr);
	ASSERT_EQ(pattern->GetPattern(), 0x000000100000000FULL);
	ASSERT_EQ(pattern->Occurrences(), 8u);
	ASSERT_EQ(pattern->FullSize(), 243u);
	ASSERT_EQ(pattern->ReducedSize(), 162u);
}

void Test_LegalWeights(uint64_t bitpattern, CPattern* pattern)
{
	std::vector<float> compressed_weights;
	compressed_weights.reserve(pattern->ReducedSize());
	for (std::size_t i = 0; i < pattern->ReducedSize(); i++)
		compressed_weights.push_back(i + 1);

	pattern->set_weights(compressed_weights);

	// Assert all configurations result in legal weight
	for (std::size_t i = 0; i < 8; i++)
	{
		switch (i)
		{
		case 0: break;
		case 1: bitpattern = FlipHorizontal(bitpattern); break;
		case 2: bitpattern = FlipVertical(bitpattern); break;
		case 3: bitpattern = FlipHorizontal(bitpattern); break;
		case 4: bitpattern = FlipDiagonal(bitpattern); break;
		case 5: bitpattern = FlipHorizontal(bitpattern); break;
		case 6: bitpattern = FlipVertical(bitpattern); break;
		case 7: bitpattern = FlipHorizontal(bitpattern); break;
		}
		uint64_t mid = ((bitpattern & 0x0000001818000000ULL) != 0ULL) ? 0x0000001818000000ULL : 0ULL;
		CPattern::For_each_configuration_in_pattern_do_fkt(bitpattern | mid, [&pattern](const CPosition& pos)
		{
			auto vec1 = pattern->GetScores(pos);
			auto vec2 = pattern->GetConfigurations(pos);
			ASSERT_EQ(vec1.size(), vec2.size());
			for (std::size_t i = 0; i < vec1.size(); i++)
			{
				ASSERT_TRUE(vec1[i] > 0);
				ASSERT_EQ(vec1[i], vec2[i] + 1);
			}
		});
	}
}

void Test_SymmetryIndependance(const uint64_t bitpattern, CPattern* pattern)
{
	std::vector<float> compressed_weights;
	compressed_weights.reserve(pattern->ReducedSize());
	for (std::size_t i = 0; i < pattern->ReducedSize(); i++)
		compressed_weights.push_back(i + 1);

	pattern->set_weights(compressed_weights);

	// Assert score's independance of flips
	uint64_t mid = ((bitpattern & 0x0000001818000000ULL) != 0ULL) ? 0x0000001818000000ULL : 0ULL;
	CPattern::For_each_configuration_in_pattern_do_fkt(bitpattern | mid, [&](CPosition pos)
	{
		auto score = pattern->Eval(pos);
		for (std::size_t i = 1; i < 8; i++)
		{
			switch (i)
			{
			case 1: pos.FlipHorizontal();	break;
			case 2: pos.FlipVertical();		break;
			case 3: pos.FlipHorizontal();	break;
			case 4: pos.FlipDiagonal();		break;
			case 5: pos.FlipHorizontal();	break;
			case 6: pos.FlipVertical();		break;
			case 7: pos.FlipHorizontal();	break;
			}
			auto other_score = pattern->Eval(pos);
			ASSERT_EQ(score, other_score);
		}
	});
}

void Test_IndexCovering(const uint64_t bitpattern, CPattern* pattern)
{
	std::vector<float> compressed_weights;
	compressed_weights.reserve(pattern->ReducedSize());
	for (std::size_t i = 0; i < pattern->ReducedSize(); i++)
		compressed_weights.push_back(i + 1);

	pattern->set_weights(compressed_weights);

	// Assert full index covering
	CPattern::For_each_configuration_in_pattern_do_fkt(bitpattern, [&](const CPosition& pos)
	{
		auto vec = pattern->GetScores(pos);
		compressed_weights[vec[0] - 1] = 0;
	});

	for (std::size_t i = 0; i < pattern->ReducedSize(); i++)
		ASSERT_EQ(compressed_weights[i], 0);
}

TEST_F(Pattern, PatternH_LegalWeight1)
{
	const uint64_t bitpattern = 0x000000000000007EULL;
	CPatternH pattern(bitpattern);
	Test_LegalWeights(bitpattern, &pattern);
}

TEST_F(Pattern, PatternH_LegalWeight2)
{
	const uint64_t bitpattern = 0x000000001800007EULL;
	CPatternH pattern(bitpattern);
	Test_LegalWeights(bitpattern, &pattern);
}

TEST_F(Pattern, PatternH_LegalWeight3)
{
	const uint64_t bitpattern = 0x00000000FF000000ULL;
	CPatternH pattern(bitpattern);
	Test_LegalWeights(bitpattern, &pattern);
}

TEST_F(Pattern, PatternH_SymmetryIndependance1)
{
	const uint64_t bitpattern = 0x000000000000007EULL;
	CPatternH pattern(bitpattern);
	Test_SymmetryIndependance(bitpattern, &pattern);
}

TEST_F(Pattern, PatternH_SymmetryIndependance2)
{
	const uint64_t bitpattern = 0x000000001800817EULL;
	CPatternH pattern(bitpattern);
	Test_SymmetryIndependance(bitpattern, &pattern);
}

TEST_F(Pattern, PatternH_SymmetryIndependance3)
{
	const uint64_t bitpattern = 0x00000000FF000000ULL;
	CPatternH pattern(bitpattern);
	Test_SymmetryIndependance(bitpattern, &pattern);
}

TEST_F(Pattern, PatternH_IndexCovering1)
{
	const uint64_t bitpattern = 0x000000000000007EULL;
	CPatternH pattern(bitpattern);
	Test_IndexCovering(bitpattern, &pattern);
}

TEST_F(Pattern, PatternH_IndexCovering2)
{
	const uint64_t bitpattern = 0x000000001800817EULL;
	CPatternH pattern(bitpattern);
	Test_IndexCovering(bitpattern, &pattern);
}

TEST_F(Pattern, PatternH_IndexCovering3)
{
	const uint64_t bitpattern = 0x00000000FF000000ULL;
	CPatternH pattern(bitpattern);
	Test_IndexCovering(bitpattern, &pattern);
}

TEST_F(Pattern, PatternD_LegalWeight1)
{
	const uint64_t bitpattern = 0x0000000000070707ULL;
	CPatternD pattern(bitpattern);
	Test_LegalWeights(bitpattern, &pattern);
}

TEST_F(Pattern, PatternD_LegalWeight2)
{
	const uint64_t bitpattern = 0x8040201008040201ULL;
	CPatternD pattern(bitpattern);
	Test_LegalWeights(bitpattern, &pattern);
}

TEST_F(Pattern, PatternD_LegalWeight3)
{
	const uint64_t bitpattern = 0x0000241818240201ULL;
	CPatternD pattern(bitpattern);
	Test_LegalWeights(bitpattern, &pattern);
}

TEST_F(Pattern, PatternD_SymmetryIndependance1)
{
	const uint64_t bitpattern = 0x0000000000070707ULL;
	CPatternD pattern(bitpattern);
	Test_SymmetryIndependance(bitpattern, &pattern);
}

TEST_F(Pattern, PatternD_SymmetryIndependance2)
{
	const uint64_t bitpattern = 0x8040201008040201ULL;
	CPatternD pattern(bitpattern);
	Test_SymmetryIndependance(bitpattern, &pattern);
}

TEST_F(Pattern, PatternD_SymmetryIndependance3)
{
	const uint64_t bitpattern = 0x8040241818240201ULL;
	CPatternD pattern(bitpattern);
	Test_SymmetryIndependance(bitpattern, &pattern);
}

TEST_F(Pattern, PatternD_IndexCovering1)
{
	const uint64_t bitpattern = 0x0000000000070707ULL;
	CPatternD pattern(bitpattern);
	Test_IndexCovering(bitpattern, &pattern);
}

TEST_F(Pattern, PatternD_IndexCovering2)
{
	const uint64_t bitpattern = 0x8040201008040201ULL;
	CPatternD pattern(bitpattern);
	Test_IndexCovering(bitpattern, &pattern);
}

TEST_F(Pattern, PatternD_IndexCovering3)
{
	const uint64_t bitpattern = 0x8040241818240201ULL;
	CPatternD pattern(bitpattern);
	Test_IndexCovering(bitpattern, &pattern);
}

TEST_F(Pattern, Pattern0_LegalWeight1)
{
	const uint64_t bitpattern = 0x000000080001000FULL;
	CPattern0 pattern(bitpattern);
	Test_LegalWeights(bitpattern, &pattern);
}

TEST_F(Pattern, Pattern0_LegalWeight2)
{
	const uint64_t bitpattern = 0x0000000008000008ULL;
	CPattern0 pattern(bitpattern);
	Test_LegalWeights(bitpattern, &pattern);
}

TEST_F(Pattern, Pattern0_LegalWeight3)
{
	const uint64_t bitpattern = 0x81810000000000C0ULL;
	CPattern0 pattern(bitpattern);
	Test_LegalWeights(bitpattern, &pattern);
}

TEST_F(Pattern, Pattern0_SymmetryIndependance1)
{
	const uint64_t bitpattern = 0x000000080001000FULL;
	CPattern0 pattern(bitpattern);
	Test_SymmetryIndependance(bitpattern, &pattern);
}

TEST_F(Pattern, Pattern0_SymmetryIndependance2)
{
	const uint64_t bitpattern = 0x0000000008000008ULL;
	CPattern0 pattern(bitpattern);
	Test_SymmetryIndependance(bitpattern, &pattern);
}

TEST_F(Pattern, Pattern0_SymmetryIndependance3)
{
	const uint64_t bitpattern = 0x81810000000000C0ULL;
	CPattern0 pattern(bitpattern);
	Test_SymmetryIndependance(bitpattern, &pattern);
}

TEST_F(Pattern, Pattern0_IndexCovering1)
{
	const uint64_t bitpattern = 0x000000080001000FULL;
	CPattern0 pattern(bitpattern);
	Test_IndexCovering(bitpattern, &pattern);
}

TEST_F(Pattern, Pattern0_IndexCovering2)
{
	const uint64_t bitpattern = 0x0000000008000008ULL;
	CPattern0 pattern(bitpattern);
	Test_IndexCovering(bitpattern, &pattern);
}

TEST_F(Pattern, Pattern0_IndexCovering3)
{
	const uint64_t bitpattern = 0x81810000000000C0ULL;
	CPattern0 pattern(bitpattern);
	Test_IndexCovering(bitpattern, &pattern);
}