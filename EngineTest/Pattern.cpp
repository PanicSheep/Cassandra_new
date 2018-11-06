#include "pch.h"
#include "Pattern.h"

#include <numeric>
#include <random>

using namespace Pattern;
using namespace Pattern::Eval;
using namespace Pattern::Configurations;

constexpr uint64_t PatternH = 0x00000000000000E7ui64; // HorizontalSymmetric
constexpr uint64_t PatternD = 0x8040201008040303ui64; // DiagonalSymmetric
constexpr uint64_t PatternA = 0x000000000000000Fui64; // Asymmetric
static const auto WeightsH = CWeights(CreatePattern(PatternH)->ReducedSize(), 0);
static const auto WeightsD = CWeights(CreatePattern(PatternD)->ReducedSize(), 0);
static const auto WeightsA = CWeights(CreatePattern(PatternA)->ReducedSize(), 0);

TEST(Pattern_MetaTest, HorizontalSymmetric)
{
	ASSERT_EQ(PatternH, FlipHorizontal(PatternH));
}

TEST(Pattern_MetaTest, DiagonalSymmetric)
{
	ASSERT_EQ(PatternD, FlipDiagonal(PatternD));
}

TEST(Pattern_MetaTest, Asymmetric)
{
	ASSERT_NE(PatternA, FlipHorizontal(PatternA));
	ASSERT_NE(PatternA, FlipDiagonal(PatternA));
}

TEST(Pattern_Configurations, CreatePattern_HorizontalSymmetric)
{
	const auto p = CreatePattern(PatternH);

	ASSERT_EQ(p->Pattern, PatternH);
	ASSERT_EQ(p->Occurrences(), 4);
	ASSERT_EQ(p->Configurations(CPosition()).size(), 4);
}

TEST(Pattern_Configurations, CreatePattern_DiagonalSymmetric)
{
	const auto p = CreatePattern(PatternD);

	ASSERT_EQ(p->Pattern, PatternD);
	ASSERT_EQ(p->Occurrences(), 4);
	ASSERT_EQ(p->Configurations(CPosition()).size(), 4);
}

TEST(Pattern_Configurations, CreatePattern_Asymmetric)
{
	const auto p = CreatePattern(PatternA);

	ASSERT_EQ(p->Pattern, PatternA);
	ASSERT_EQ(p->Occurrences(), 8);
	ASSERT_EQ(p->Configurations(CPosition()).size(), 8);
}

TEST(Pattern_Eval, CreatePattern_HorizontalSymmetric)
{
	const auto p = CreatePattern(PatternH, WeightsH);

	ASSERT_EQ(p->Pattern, PatternH);
}

TEST(Pattern_Eval, CreatePattern_DiagonalSymmetric)
{
	const auto p = CreatePattern(PatternD, WeightsD);

	ASSERT_EQ(p->Pattern, PatternD);
}

TEST(Pattern_Eval, CreatePattern_Asymmetric)
{
	const auto p = CreatePattern(PatternA, WeightsA);

	ASSERT_EQ(p->Pattern, PatternA);
}

TEST(Pattern_Eval, Create_Pack1)
{
	auto pack = CPack();
	pack.Add(CreatePattern(PatternH, WeightsH));
	pack.Add(CreatePattern(PatternD, WeightsD));
	pack.Add(CreatePattern(PatternA, WeightsA));

	ASSERT_EQ(pack.size(), 3);
	const auto p = pack.Pattern();
	ASSERT_TRUE(std::find(std::begin(p), std::end(p), PatternH) != std::end(p));
	ASSERT_TRUE(std::find(std::begin(p), std::end(p), PatternD) != std::end(p));
	ASSERT_TRUE(std::find(std::begin(p), std::end(p), PatternA) != std::end(p));
}

TEST(Pattern_Eval, Create_Pack2)
{
	auto in = {
		std::make_pair(PatternH, WeightsH),
		std::make_pair(PatternD, WeightsD),
		std::make_pair(PatternA, WeightsA),
	};
	auto pack = CPack(in);

	ASSERT_EQ(pack.size(), 3);
	const auto p = pack.Pattern();
	ASSERT_TRUE(std::find(std::begin(p), std::end(p), PatternH) != std::end(p));
	ASSERT_TRUE(std::find(std::begin(p), std::end(p), PatternD) != std::end(p));
	ASSERT_TRUE(std::find(std::begin(p), std::end(p), PatternA) != std::end(p));
}

TEST(Pattern_Eval, Create_Ensemble)
{
	CEnsemble ensemble;
	auto pair1 = {
		std::make_pair(PatternH, WeightsH)
	};
	auto pair2 = {
		std::make_pair(PatternH, WeightsH),
		std::make_pair(PatternD, WeightsD)
	};
	auto pair3 = {
		std::make_pair(PatternH, WeightsH),
		std::make_pair(PatternD, WeightsD),
		std::make_pair(PatternA, WeightsA)
	};
	auto pack1 = std::make_shared<CPack>(pair1);
	auto pack2 = std::make_shared<CPack>(pair2);
	auto pack3 = std::make_shared<CPack>(pair3);
	ensemble.Set(0, pack1);
	ensemble.Set(1, pack2);
	ensemble.Set(2, pack3);

	ASSERT_EQ(ensemble.Eval(CPosition(0xFFFFFFFFFFFFFFFFui64, 0x0000000000000000ui64)), 0);
	ASSERT_EQ(ensemble.Eval(CPosition(0x7FFFFFFFFFFFFFFFui64, 0x0000000000000000ui64)), 0);
	ASSERT_EQ(ensemble.Eval(CPosition(0x3FFFFFFFFFFFFFFFui64, 0x0000000000000000ui64)), 0);
}

void Test_SymmetryIndependance(const uint64_t bitpattern, const CWeights& compressed)
{
	const auto pattern = CreatePattern(bitpattern, compressed);

	// Assert score's independance of flips
	const uint64_t mid = ((bitpattern & 0x0000001818000000ui64) != 0ui64) ? 0x0000001818000000ui64 : 0ui64;
	For_each_config(bitpattern | mid,
		[&](CPosition pos) {
			auto score = pattern->Eval(pos);
			for (std::size_t i = 1; i < 8; i++)
			{
				switch (i)
				{
					case 1: pos.FlipHorizontal(); break;
					case 2: pos.FlipVertical(); break;
					case 3: pos.FlipHorizontal(); break;
					case 4: pos.FlipDiagonal(); break;
					case 5: pos.FlipHorizontal(); break;
					case 6: pos.FlipVertical(); break;
					case 7: pos.FlipHorizontal(); break;
				}
				auto other_score = pattern->Eval(pos);
				ASSERT_EQ(score, other_score);
			}
		});
}

TEST(Pattern_Eval, SymmetryIndependance_HorizontalSymmetric)
{
	Test_SymmetryIndependance(PatternH, WeightsH);
}

TEST(Pattern_Eval, SymmetryIndependance_DiagonalSymmetric)
{
	Test_SymmetryIndependance(PatternD, WeightsD);
}

TEST(Pattern_Eval, SymmetryIndependance_Asymmetric)
{
	Test_SymmetryIndependance(PatternA, WeightsA);
}


CPosition CreateRandomPosition()
{
	static auto rnd1 = std::bind(std::uniform_int_distribution<unsigned int>(0, 1), std::mt19937_64(1111));
	static auto rnd2 = std::bind(std::uniform_int_distribution<unsigned int>(0, 2), std::mt19937_64(2222));
	uint64_t mid = 0x0000001818000000ui64;
	uint64_t non_mid = 0xFFFFFFFFFFFFFFFFui64 ^ mid;
	uint64_t P = 0;
	uint64_t O = 0;

	while (mid)
	{
		switch (rnd1())
		{
			case 0: P |= GetLSB(mid); break;
			case 1: O |= GetLSB(mid); break;
			default: throw;
		}
		RemoveLSB(mid);
	}

	while (non_mid)
	{
		switch (rnd2())
		{
			case 0: P |= GetLSB(non_mid); break;
			case 1: O |= GetLSB(non_mid); break;
			default: break;
		}
		RemoveLSB(non_mid);
	}

	return CPosition(P, O);
}

void Test_LegalWeights(uint64_t bitpattern)
{
	auto config_pattern = CreatePattern(bitpattern);

	CWeights compressed(config_pattern->ReducedSize());
	std::iota(compressed.begin(), compressed.end(), 1);
	auto eval_pattern = CreatePattern(bitpattern, compressed);

	for (std::size_t i = 0; i < 100'000; i++)
	{
		const auto pos = CreateRandomPosition();

		auto configs = config_pattern->Configurations(pos);
		float sum = 0;
		for (auto it : configs)
			sum += compressed[it];

		auto score = eval_pattern->Eval(pos);

		ASSERT_EQ(sum, score);
	}
}

TEST(Pattern_Eval, LegalWeights_HorizontalSymmetric)
{
	Test_LegalWeights(PatternH);
}

TEST(Pattern_Eval, LegalWeights_DiagonalSymmetric)
{
	Test_LegalWeights(PatternD);
}

TEST(Pattern_Eval, LegalWeights_Asymmetric)
{
	Test_LegalWeights(PatternA);
}

// TODO: Implement with new patterns.
//void Test_IndexCovering(const uint64_t bitpattern, CBase* pattern)
//{
//	std::vector<float> compressed_weights;
//	compressed_weights.reserve(pattern->ReducedSize());
//	for (std::size_t i = 0; i < pattern->ReducedSize(); i++)
//		compressed_weights.push_back(i + 1);
//
//	pattern->set_weights(compressed_weights);
//
//	// Assert full index covering
//	CBase::For_each_configuration_in_pattern_do_fkt(bitpattern, [&](const CPosition& pos)
//	{
//		auto vec = pattern->GetScores(pos);
//		compressed_weights[vec[0] - 1] = 0;
//	});
//
//	for (std::size_t i = 0; i < pattern->ReducedSize(); i++)
//		ASSERT_EQ(compressed_weights[i], 0);
//}