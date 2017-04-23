#include "pattern.h"
#include "gtest/gtest.h"

TEST (PatternTest, pow_int_1) { ASSERT_EQ (Pow_int(0, 0), 1); }
TEST (PatternTest, pow_int_2) { ASSERT_EQ (Pow_int(10, 3), 1000); }
TEST (PatternTest, pow_int_3) { ASSERT_EQ (Pow_int(-10, 3), -1000); }

TEST (PatternTest, sumPow3_1) { ASSERT_EQ (Pattern::SumPow3[0], 0u); }
TEST (PatternTest, sumPow3_2) { ASSERT_EQ (Pattern::SumPow3[1], 1u); }
TEST (PatternTest, sumPow3_3) { ASSERT_EQ (Pattern::SumPow3[2], 3u); }
TEST (PatternTest, sumPow3_4) { ASSERT_EQ (Pattern::SumPow3[15], 27u + 9u + 3u + 1u); }

TEST (PatternTest, LookupPattern_Hex1) { ASSERT_EQ (Pattern::LookupPattern("0x000000000000FFFF"), 0x000000000000FFFFULL); }
TEST (PatternTest, LookupPattern_Hex2) { ASSERT_EQ (Pattern::LookupPattern("0x000000000000FFFFULL"), 0x000000000000FFFFULL); }

TEST (PatternTest, LookupPattern_Fail)
{
	int tmp = -2;
	std::swap (gConfigurations.Verbosity, tmp);
	
	ASSERT_EQ (Pattern::LookupPattern("INVALID"), 0u);
	
	std::swap (gConfigurations.Verbosity, tmp);
}

TEST (PatternTest, LookupPattern_FromFakeEntries)
{
	// Set up
	gConfigurations.SetConfiguration("pattern BigL", "0x01010101010101FFULL");
	gConfigurations.SetConfiguration("pattern Corner", "0x0000000000000001ULL");
	gConfigurations.SetConfiguration("active pattern", "Corner BigL");
	
	 // Test
	ASSERT_EQ (Pattern::LookupPattern("Corner"), 0x0000000000000001ULL);
	ASSERT_EQ (Pattern::LookupPattern("BigL"), 0x01010101010101FFULL);
	
	// Reset
	gConfigurations = CConfigurations();
}

TEST (PatternTest, Initialize_FromFakePattern)
{
	// Set up
	gConfigurations.SetConfiguration("pattern BigL", "0x01010101010101FFULL");
	gConfigurations.SetConfiguration("pattern Corner", "0x0000000000000001ULL");
	gConfigurations.SetConfiguration("active pattern", "Corner BigL");
	Pattern::Initialize(false, true);
	
	// Test
	ASSERT_EQ (Pattern::BoxedPatternSet[0][0]->GetName(), "Corner");
	ASSERT_EQ (Pattern::BoxedPatternSet[0][1]->GetName(), "BigL");
	
	// Reset
	gConfigurations = CConfigurations();
}

TEST (PatternTest, NewPatternH)
{
	Pattern::CPattern* pattern = Pattern::NewPattern("name", 0x000000000000007EULL);
	
	ASSERT_TRUE (dynamic_cast<Pattern::CPatternH*>(pattern) != nullptr);
	ASSERT_EQ (pattern->GetName(), "name");
	ASSERT_EQ (pattern->GetPattern(), 0x000000000000007EULL);
	ASSERT_EQ (pattern->Occurrences(), 4u);
	ASSERT_EQ (pattern->FullSize(), 729u);
	ASSERT_EQ (pattern->ReducedSize(), 378u);
}

TEST (PatternTest, NewPatternD)
{
	Pattern::CPattern* pattern = Pattern::NewPattern("name", 0x0000000000000303ULL);
	
	ASSERT_TRUE (dynamic_cast<Pattern::CPatternD*>(pattern) != nullptr);
	ASSERT_EQ (pattern->GetName(), "name");
	ASSERT_EQ (pattern->GetPattern(), 0x0000000000000303ULL);
	ASSERT_EQ (pattern->Occurrences(), 4u);
	ASSERT_EQ (pattern->FullSize(), 81u);
	ASSERT_EQ (pattern->ReducedSize(), 54u);
}

TEST (PatternTest, NewPattern0)
{
	Pattern::CPattern* pattern = Pattern::NewPattern("name", 0x000000100000000FULL);
	
	ASSERT_TRUE (dynamic_cast<Pattern::CPattern0*>(pattern) != nullptr);
	ASSERT_EQ (pattern->GetName(), "name");
	ASSERT_EQ (pattern->GetPattern(), 0x000000100000000FULL);
	ASSERT_EQ (pattern->Occurrences(), 8u);
	ASSERT_EQ (pattern->FullSize(), 243u);
	ASSERT_EQ (pattern->ReducedSize(), 162u);
}

void Test_LegalWeights(uint64_t bitpattern, Pattern::CPattern* pattern)
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
		Pattern::For_each_configuration_in_pattern_do_fkt(bitpattern | mid, 
			[&pattern](uint64_t P, uint64_t O)
			{
				auto vec1 = pattern->GetScoreVec(P, O);
				auto vec2 = pattern->GetConfigurationVec(P, O);
				ASSERT_EQ (vec1.size(), vec2.size());
				for (std::size_t i = 0; i < vec1.size(); i++){
					ASSERT_TRUE (vec1[i] > 0);
					ASSERT_EQ (vec1[i], vec2[i] + 1);
				}
			});
	}
}

void Test_SymmetryIndependance(uint64_t bitpattern, Pattern::CPattern* pattern)
{
	std::vector<float> compressed_weights;
	compressed_weights.reserve(pattern->ReducedSize());
	for (std::size_t i = 0; i < pattern->ReducedSize(); i++)
		compressed_weights.push_back(i + 1);
	
	pattern->set_weights(compressed_weights);
	
	// Assert score's independance of flips
	uint64_t mid = ((bitpattern & 0x0000001818000000ULL) != 0ULL) ? 0x0000001818000000ULL : 0ULL;
	Pattern::For_each_configuration_in_pattern_do_fkt(bitpattern | mid, 
		[&](uint64_t P, uint64_t O)
		{
			auto score = pattern->score(P, O);
			for (std::size_t i = 1; i < 8; i++)
			{
				switch (i)
				{
					case 1: P = FlipHorizontal(P);	O = FlipHorizontal(O);	break;
					case 2: P = FlipVertical(P);	O = FlipVertical(O);	break;
					case 3: P = FlipHorizontal(P);	O = FlipHorizontal(O);	break;
					case 4: P = FlipDiagonal(P);	O = FlipDiagonal(O);	break;
					case 5: P = FlipHorizontal(P);	O = FlipHorizontal(O);	break;
					case 6: P = FlipVertical(P);	O = FlipVertical(O);	break;
					case 7: P = FlipHorizontal(P);	O = FlipHorizontal(O);	break;
				}
				auto other_score = pattern->score(P, O);
				ASSERT_EQ (score, other_score);
			}
		});
}

void Test_IndexCovering(uint64_t bitpattern, Pattern::CPattern* pattern)
{
	std::vector<float> compressed_weights;
	compressed_weights.reserve(pattern->ReducedSize());
	for (std::size_t i = 0; i < pattern->ReducedSize(); i++)
		compressed_weights.push_back(i + 1);
	
	pattern->set_weights(compressed_weights);
	
	// Assert full index covering
	Pattern::For_each_configuration_in_pattern_do_fkt(bitpattern, 
		[&](uint64_t P, uint64_t O)
		{
			auto vec = pattern->GetScoreVec(P, O);
			compressed_weights[vec[0]-1] = 0;
		});
	
	for (std::size_t i = 0; i < pattern->ReducedSize(); i++)
		ASSERT_EQ (compressed_weights[i], 0);
}

TEST (PatternTest, PatternH_LegalWeight1)
{
	uint64_t bitpattern = 0x000000000000007EULL;
	Pattern::CPatternH pattern("name", bitpattern);
	Test_LegalWeights(bitpattern, &pattern);
}

TEST (PatternTest, PatternH_LegalWeight2)
{
	uint64_t bitpattern = 0x000000001800817EULL;
	Pattern::CPatternH pattern("name", bitpattern);
	Test_LegalWeights(bitpattern, &pattern);
}

TEST (PatternTest, PatternH_LegalWeight3)
{
	uint64_t bitpattern = 0x00000000FF000000ULL;
	Pattern::CPatternH pattern("name", bitpattern);
	Test_LegalWeights(bitpattern, &pattern);
}

TEST (PatternTest, PatternH_SymmetryIndependance1)
{
	uint64_t bitpattern = 0x000000000000007EULL;
	Pattern::CPatternH pattern("name", bitpattern);
	Test_SymmetryIndependance(bitpattern, &pattern);
}

TEST (PatternTest, PatternH_SymmetryIndependance2)
{
	uint64_t bitpattern = 0x000000001800817EULL;
	Pattern::CPatternH pattern("name", bitpattern);
	Test_SymmetryIndependance(bitpattern, &pattern);
}

TEST (PatternTest, PatternH_SymmetryIndependance3)
{
	uint64_t bitpattern = 0x00000000FF000000ULL;
	Pattern::CPatternH pattern("name", bitpattern);
	Test_SymmetryIndependance(bitpattern, &pattern);
}

TEST (PatternTest, PatternH_IndexCovering1)
{
	uint64_t bitpattern = 0x000000000000007EULL;
	Pattern::CPatternH pattern("name", bitpattern);
	Test_IndexCovering(bitpattern, &pattern);
}

TEST (PatternTest, PatternH_IndexCovering2)
{
	uint64_t bitpattern = 0x000000001800817EULL;
	Pattern::CPatternH pattern("name", bitpattern);
	Test_IndexCovering(bitpattern, &pattern);
}

TEST (PatternTest, PatternH_IndexCovering3)
{
	uint64_t bitpattern = 0x00000000FF000000ULL;
	Pattern::CPatternH pattern("name", bitpattern);
	Test_IndexCovering(bitpattern, &pattern);
}

TEST (PatternTest, PatternD_LegalWeight1)
{
	uint64_t bitpattern = 0x0000000000070707ULL;
	Pattern::CPatternD pattern("name", bitpattern);
	Test_LegalWeights(bitpattern, &pattern);
}

TEST (PatternTest, PatternD_LegalWeight2)
{
	uint64_t bitpattern = 0x8040201008040201ULL;
	Pattern::CPatternD pattern("name", bitpattern);
	Test_LegalWeights(bitpattern, &pattern);
}

TEST (PatternTest, PatternD_LegalWeight3)
{
	uint64_t bitpattern = 0x8040241818240201ULL;
	Pattern::CPatternD pattern("name", bitpattern);
	Test_LegalWeights(bitpattern, &pattern);
}

TEST (PatternTest, PatternD_SymmetryIndependance1)
{
	uint64_t bitpattern = 0x0000000000070707ULL;
	Pattern::CPatternD pattern("name", bitpattern);
	Test_SymmetryIndependance(bitpattern, &pattern);
}

TEST (PatternTest, PatternD_SymmetryIndependance2)
{
	uint64_t bitpattern = 0x8040201008040201ULL;
	Pattern::CPatternD pattern("name", bitpattern);
	Test_SymmetryIndependance(bitpattern, &pattern);
}

TEST (PatternTest, PatternD_SymmetryIndependance3)
{
	uint64_t bitpattern = 0x8040241818240201ULL;
	Pattern::CPatternD pattern("name", bitpattern);
	Test_SymmetryIndependance(bitpattern, &pattern);
}

TEST (PatternTest, PatternD_IndexCovering1)
{
	uint64_t bitpattern = 0x0000000000070707ULL;
	Pattern::CPatternD pattern("name", bitpattern);
	Test_IndexCovering(bitpattern, &pattern);
}

TEST (PatternTest, PatternD_IndexCovering2)
{
	uint64_t bitpattern = 0x8040201008040201ULL;
	Pattern::CPatternD pattern("name", bitpattern);
	Test_IndexCovering(bitpattern, &pattern);
}

TEST (PatternTest, PatternD_IndexCovering3)
{
	uint64_t bitpattern = 0x8040241818240201ULL;
	Pattern::CPatternD pattern("name", bitpattern);
	Test_IndexCovering(bitpattern, &pattern);
}

TEST (PatternTest, Pattern0_LegalWeight1)
{
	uint64_t bitpattern = 0x000000080001000FULL;
	Pattern::CPattern0 pattern("name", bitpattern);
	Test_LegalWeights(bitpattern, &pattern);
}

TEST (PatternTest, Pattern0_LegalWeight2)
{
	uint64_t bitpattern = 0x0000000008000008ULL;
	Pattern::CPattern0 pattern("name", bitpattern);
	Test_LegalWeights(bitpattern, &pattern);
}

TEST (PatternTest, Pattern0_LegalWeight3)
{
	uint64_t bitpattern = 0x81810000000000C0ULL;
	Pattern::CPattern0 pattern("name", bitpattern);
	Test_LegalWeights(bitpattern, &pattern);
}

TEST (PatternTest, Pattern0_SymmetryIndependance1)
{
	uint64_t bitpattern = 0x000000080001000FULL;
	Pattern::CPattern0 pattern("name", bitpattern);
	Test_SymmetryIndependance(bitpattern, &pattern);
}

TEST (PatternTest, Pattern0_SymmetryIndependance2)
{
	uint64_t bitpattern = 0x0000000008000008ULL;
	Pattern::CPattern0 pattern("name", bitpattern);
	Test_SymmetryIndependance(bitpattern, &pattern);
}

TEST (PatternTest, Pattern0_SymmetryIndependance3)
{
	uint64_t bitpattern = 0x81810000000000C0ULL;
	Pattern::CPattern0 pattern("name", bitpattern);
	Test_SymmetryIndependance(bitpattern, &pattern);
}

TEST (PatternTest, Pattern0_IndexCovering1)
{
	uint64_t bitpattern = 0x000000080001000FULL;
	Pattern::CPattern0 pattern("name", bitpattern);
	Test_IndexCovering(bitpattern, &pattern);
}

TEST (PatternTest, Pattern0_IndexCovering2)
{
	uint64_t bitpattern = 0x0000000008000008ULL;
	Pattern::CPattern0 pattern("name", bitpattern);
	Test_IndexCovering(bitpattern, &pattern);
}

TEST (PatternTest, Pattern0_IndexCovering3)
{
	uint64_t bitpattern = 0x81810000000000C0ULL;
	Pattern::CPattern0 pattern("name", bitpattern);
	Test_IndexCovering(bitpattern, &pattern);
}


int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	
	Pattern::Initialize(false, false);
	
	return RUN_ALL_TESTS();
}
