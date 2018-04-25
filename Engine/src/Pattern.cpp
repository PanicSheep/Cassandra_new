#include "Pattern.h"
#include "VectorExtension.h"
#include <mutex>

std::vector<uint32_t> CPattern::m_sumpow3_cache;

uint32_t CPattern::FullPatternIndex(const CPosition& pos, const uint64_t mask)
{
	return SumPow3(PExt(pos.GetP(), mask)) + SumPow3(PExt(pos.GetO(), mask)) * 2;
}

uint32_t CPattern::SumPow3(const uint64_t index)
{
	return m_sumpow3_cache[index]; // OPTIMIZATION: Replace look-up by small calculation if possible.
}

void CPattern::Initialize()
{
	static std::once_flag flag;
	std::call_once(flag, []() {
		const std::size_t size = (1ULL << 15);
		m_sumpow3_cache.reserve(size);
		for (std::size_t i = 0; i < size; i++)
		{
			uint64_t exp = i;
			uint64_t sum = 0;
			while (exp) {
				sum += Pow_int(3, BitScanLSB(exp));
				RemoveLSB(exp);
			}
			m_sumpow3_cache.push_back(sum);
		}
	});
}

void CPattern::For_each_configuration_in_pattern_do_fkt(const uint64_t pattern, std::function<void(const CPosition&)> fkt)
{
	const uint64_t PatternSize = 1ULL << PopCount(pattern);
	const uint64_t ExtractedCenter = PExt(0x0000001818000000ULL, pattern);
	for (uint64_t i = 0; i < PatternSize; i++)
	{
		const uint64_t P = PDep(i, pattern);
		for (uint64_t j = 0; j < PatternSize; j++)
		{
			if (i & j) continue; // fields can only be taken by one player
			if (((i | j) & ExtractedCenter) != ExtractedCenter) continue; // center fields can't be empty
			const uint64_t O = PDep(j, pattern);

			fkt(CPosition(P, O));
		}
	}
}

CPattern::CPattern(uint64_t Pattern, uint32_t FullSize, uint32_t ReducedSize, uint32_t Occurrences)
	: Pattern(Pattern)
	, m_FullSize(FullSize)
	, m_ReducedSize(ReducedSize)
	, m_Occurrences(Occurrences)
{}

CPatternH::CPatternH(uint64_t Pattern)
	: CPattern(Pattern, Pow_int(3, PopCount(Pattern)), (Pow_int(3, PopCount(Pattern & HALF & ~MID)) * Pow_int(2, PopCount(Pattern & HALF & MID))) * ((Pow_int(3, PopCount(Pattern & HALF & ~MID)) * Pow_int(2, PopCount(Pattern & HALF & MID))) + 1) / 2, 4),
	PatternC(FlipCodiagonal(Pattern)),
	PatternV(FlipVertical(Pattern)),
	PatternD(FlipDiagonal(Pattern)),
	halfSize(Pow_int(3, PopCount(Pattern & HALF & ~MID)) * Pow_int(2, PopCount(Pattern & HALF & MID)))
{
	assert(Pattern == FlipHorizontal(Pattern));
}

void CPatternH::set_weights()
{
	m_weights.resize(m_Occurrences);
	for (uint32_t i = 0; i < m_Occurrences; i++)
		m_weights[i].resize(m_FullSize, NAN);
}

void CPatternH::set_weights(const std::vector<float>& compressed_weights)
{
	assert(compressed_weights.size() == m_ReducedSize);
	set_weights();

	For_each_configuration_in_pattern_do_fkt(Pattern,  [&](const CPosition& pos) { m_weights[0][FullPatternIndex(pos, Pattern )] = compressed_weights[ReducedPatternIndex0(pos)]; });
	For_each_configuration_in_pattern_do_fkt(PatternC, [&](const CPosition& pos) { m_weights[1][FullPatternIndex(pos, PatternC)] = compressed_weights[ReducedPatternIndex1(pos)]; });
	For_each_configuration_in_pattern_do_fkt(PatternV, [&](const CPosition& pos) { m_weights[2][FullPatternIndex(pos, PatternV)] = compressed_weights[ReducedPatternIndex2(pos)]; });
	For_each_configuration_in_pattern_do_fkt(PatternD, [&](const CPosition& pos) { m_weights[3][FullPatternIndex(pos, PatternD)] = compressed_weights[ReducedPatternIndex3(pos)]; });
}

float CPatternH::Eval(const CPosition& pos) const
{
	return m_weights[0][FullPatternIndex(pos, Pattern )]
	     + m_weights[1][FullPatternIndex(pos, PatternC)]
	     + m_weights[2][FullPatternIndex(pos, PatternV)]
	     + m_weights[3][FullPatternIndex(pos, PatternD)];
}

std::vector<float> CPatternH::GetScores(const CPosition& pos) const
{
	return std::vector<float>{
		m_weights[0][FullPatternIndex(pos, Pattern )],
		m_weights[1][FullPatternIndex(pos, PatternC)],
		m_weights[2][FullPatternIndex(pos, PatternV)],
		m_weights[3][FullPatternIndex(pos, PatternD)]};
}

std::vector<uint32_t> CPatternH::GetConfigurations(const CPosition& pos) const
{
	return std::vector<uint32_t>{
		ReducedPatternIndex0(pos),
		ReducedPatternIndex1(pos),
		ReducedPatternIndex2(pos),
		ReducedPatternIndex3(pos)
	};
}

float CPatternD::Eval(const CPosition& pos) const
{
	return m_weights[0][FullPatternIndex(pos, Pattern )]
	     + m_weights[1][FullPatternIndex(pos, PatternH)]
	     + m_weights[2][FullPatternIndex(pos, PatternC)]
	     + m_weights[3][FullPatternIndex(pos, PatternV)];
}

std::vector<float> CPatternD::GetScores(const CPosition& pos) const
{
	return std::vector<float>{
		m_weights[0][FullPatternIndex(pos, Pattern )],
		m_weights[1][FullPatternIndex(pos, PatternH)],
		m_weights[2][FullPatternIndex(pos, PatternC)],
		m_weights[3][FullPatternIndex(pos, PatternV)]
	};
}

std::vector<uint32_t> CPatternD::GetConfigurations(const CPosition& pos) const
{
	return std::vector<uint32_t>{
		ReducedPatternIndex0(pos),
		ReducedPatternIndex1(pos),
		ReducedPatternIndex2(pos),
		ReducedPatternIndex3(pos)
	};
}

float CPattern0::Eval(const CPosition& pos) const
{
	return m_weights[0][FullPatternIndex(pos, Pattern  )]
	     + m_weights[1][FullPatternIndex(pos, PatternH )]
	     + m_weights[2][FullPatternIndex(pos, PatternV )]
	     + m_weights[3][FullPatternIndex(pos, PatternD )]
	     + m_weights[4][FullPatternIndex(pos, PatternC )]
	     + m_weights[5][FullPatternIndex(pos, PatternHV)]
	     + m_weights[6][FullPatternIndex(pos, PatternHD)]
	     + m_weights[7][FullPatternIndex(pos, PatternHC)];
}

std::vector<float> CPattern0::GetScores(const CPosition& pos) const
{
	return std::vector<float>{
		m_weights[0][FullPatternIndex(pos, Pattern  )],
		m_weights[1][FullPatternIndex(pos, PatternH )],
		m_weights[2][FullPatternIndex(pos, PatternV )],
		m_weights[3][FullPatternIndex(pos, PatternD )],
		m_weights[4][FullPatternIndex(pos, PatternC )],
		m_weights[5][FullPatternIndex(pos, PatternHV)],
		m_weights[6][FullPatternIndex(pos, PatternHD)],
		m_weights[7][FullPatternIndex(pos, PatternHC)]
	};
}

std::vector<uint32_t> CPattern0::GetConfigurations(const CPosition& pos) const
{
	return std::vector<uint32_t>{
		ReducedPatternIndex0(pos),
		ReducedPatternIndex1(pos),
		ReducedPatternIndex2(pos),
		ReducedPatternIndex3(pos),
		ReducedPatternIndex4(pos),
		ReducedPatternIndex5(pos),
		ReducedPatternIndex6(pos),
		ReducedPatternIndex7(pos)
	};
}

uint32_t CPatternH::ReducedPatternIndex0(CPosition pos) const
{
	const uint32_t indexA = (FullPatternIndex(pos, Pattern & HALF & ~MID) << PopCount(Pattern & HALF & MID)) + static_cast<uint32_t>(PExt(pos.GetO(), Pattern & HALF & MID));
	pos.FlipHorizontal();
	const uint32_t indexB = (FullPatternIndex(pos, Pattern & HALF & ~MID) << PopCount(Pattern & HALF & MID)) + static_cast<uint32_t>(PExt(pos.GetO(), Pattern & HALF & MID));
		
	if (indexA > indexB)
		return indexB * halfSize + indexA - ((indexB * indexB + indexB) >> 1);
	else
		return indexA * halfSize + indexB - ((indexA * indexA + indexA) >> 1);
}

uint32_t CPatternH::ReducedPatternIndex1(CPosition pos) const
{
	pos.FlipCodiagonal();
	return ReducedPatternIndex0(pos);
}

uint32_t CPatternH::ReducedPatternIndex2(CPosition pos) const
{
	pos.FlipVertical();
	return ReducedPatternIndex0(pos);
}

uint32_t CPatternH::ReducedPatternIndex3(CPosition pos) const
{
	pos.FlipDiagonal();
	return ReducedPatternIndex0(pos);
}

CPatternD::CPatternD(uint64_t Pattern)
	: CPattern(Pattern, Pow_int(3, PopCount(Pattern)), (Pow_int(3, PopCount(Pattern & DIAG & ~MID)) * Pow_int(2, PopCount(Pattern & DIAG & MID))) * (Pow_int(3, PopCount(Pattern & HALF & ~MID)) * Pow_int(2, PopCount(Pattern & HALF & MID))) * ((Pow_int(3, PopCount(Pattern & HALF & ~MID)) * Pow_int(2, PopCount(Pattern & HALF & MID))) + 1) / 2, 4),
	PatternH(FlipHorizontal(Pattern)),
	PatternC(FlipCodiagonal(Pattern)),
	PatternV(FlipVertical(Pattern)),
	halfSize(Pow_int(3, PopCount(Pattern & HALF & ~MID)) * Pow_int(2, PopCount(Pattern & HALF & MID))),
	diagSize(Pow_int(3, PopCount(Pattern & DIAG & ~MID)) * Pow_int(2, PopCount(Pattern & DIAG & MID)))
{
	assert(Pattern == FlipDiagonal(Pattern));
}

void CPatternD::set_weights()
{
	m_weights.resize(m_Occurrences);
	for (uint32_t i = 0; i < m_Occurrences; i++)
		m_weights[i].resize(m_FullSize, NAN);
}

void CPatternD::set_weights(const std::vector<float>& compressed_weights)
{
	assert(compressed_weights.size() == m_ReducedSize);
	set_weights();

	For_each_configuration_in_pattern_do_fkt(Pattern , [&](const CPosition& pos) { m_weights[0][FullPatternIndex(pos, Pattern )] = compressed_weights[ReducedPatternIndex0(pos)]; });
	For_each_configuration_in_pattern_do_fkt(PatternH, [&](const CPosition& pos) { m_weights[1][FullPatternIndex(pos, PatternH)] = compressed_weights[ReducedPatternIndex1(pos)]; });
	For_each_configuration_in_pattern_do_fkt(PatternC, [&](const CPosition& pos) { m_weights[2][FullPatternIndex(pos, PatternC)] = compressed_weights[ReducedPatternIndex2(pos)]; });
	For_each_configuration_in_pattern_do_fkt(PatternV, [&](const CPosition& pos) { m_weights[3][FullPatternIndex(pos, PatternV)] = compressed_weights[ReducedPatternIndex3(pos)]; });
}

uint32_t CPatternD::ReducedPatternIndex0(CPosition pos) const
{
	const uint32_t indexA = (FullPatternIndex(pos, Pattern & HALF & ~MID) << PopCount(Pattern & HALF & MID)) + static_cast<uint32_t>(PExt(pos.GetO(), Pattern & HALF & MID));
	const uint32_t indexD = (FullPatternIndex(pos, Pattern & DIAG & ~MID) << PopCount(Pattern & DIAG & MID)) + static_cast<uint32_t>(PExt(pos.GetO(), Pattern & DIAG & MID));
	pos.FlipDiagonal();
	const uint32_t indexB = (FullPatternIndex(pos, Pattern & HALF & ~MID) << PopCount(Pattern & HALF & MID)) + static_cast<uint32_t>(PExt(pos.GetO(), Pattern & HALF & MID));
		
	if (indexA > indexB)
		return diagSize * (indexB * halfSize + indexA - ((indexB * indexB + indexB) >> 1)) + indexD;
	else
		return diagSize * (indexA * halfSize + indexB - ((indexA * indexA + indexA) >> 1)) + indexD;
}

uint32_t CPatternD::ReducedPatternIndex1(CPosition pos) const
{
	pos.FlipHorizontal();
	return ReducedPatternIndex0(pos);
}

uint32_t CPatternD::ReducedPatternIndex2(CPosition pos) const
{
	pos.FlipCodiagonal();
	return ReducedPatternIndex0(pos);
}

uint32_t CPatternD::ReducedPatternIndex3(CPosition pos) const
{
	pos.FlipVertical();
	return ReducedPatternIndex0(pos);
}

CPattern0::CPattern0(uint64_t Pattern)
	: CPattern(Pattern, Pow_int(3, PopCount(Pattern)), Pow_int(3, PopCount(Pattern & ~MID)) * Pow_int(2, PopCount(Pattern & MID)), 8),
	PatternH(FlipHorizontal(Pattern)),
	PatternV(FlipVertical(Pattern)),
	PatternD(FlipDiagonal(Pattern)),
	PatternC(FlipCodiagonal(Pattern)),
	PatternHV(FlipVertical(FlipHorizontal(Pattern))),
	PatternHD(FlipDiagonal(FlipHorizontal(Pattern))),
	PatternHC(FlipCodiagonal(FlipHorizontal(Pattern)))
{}

void CPattern0::set_weights()
{
	m_weights.resize(m_Occurrences);
	for (uint32_t i = 0; i < m_Occurrences; i++)
		m_weights[i].resize(m_FullSize, NAN);
}

void CPattern0::set_weights(const std::vector<float>& compressed_weights)
{
	assert(compressed_weights.size() == m_ReducedSize);
	set_weights();
		
	For_each_configuration_in_pattern_do_fkt(Pattern  , [&](const CPosition& pos) { m_weights[0][FullPatternIndex(pos, Pattern  )] = compressed_weights[ReducedPatternIndex0(pos)]; });
	For_each_configuration_in_pattern_do_fkt(PatternH , [&](const CPosition& pos) { m_weights[1][FullPatternIndex(pos, PatternH )] = compressed_weights[ReducedPatternIndex1(pos)]; });
	For_each_configuration_in_pattern_do_fkt(PatternV , [&](const CPosition& pos) { m_weights[2][FullPatternIndex(pos, PatternV )] = compressed_weights[ReducedPatternIndex2(pos)]; });
	For_each_configuration_in_pattern_do_fkt(PatternD , [&](const CPosition& pos) { m_weights[3][FullPatternIndex(pos, PatternD )] = compressed_weights[ReducedPatternIndex3(pos)]; });
	For_each_configuration_in_pattern_do_fkt(PatternC , [&](const CPosition& pos) { m_weights[4][FullPatternIndex(pos, PatternC )] = compressed_weights[ReducedPatternIndex4(pos)]; });
	For_each_configuration_in_pattern_do_fkt(PatternHV, [&](const CPosition& pos) { m_weights[5][FullPatternIndex(pos, PatternHV)] = compressed_weights[ReducedPatternIndex5(pos)]; });
	For_each_configuration_in_pattern_do_fkt(PatternHD, [&](const CPosition& pos) { m_weights[6][FullPatternIndex(pos, PatternHD)] = compressed_weights[ReducedPatternIndex6(pos)]; });
	For_each_configuration_in_pattern_do_fkt(PatternHC, [&](const CPosition& pos) { m_weights[7][FullPatternIndex(pos, PatternHC)] = compressed_weights[ReducedPatternIndex7(pos)]; });
}

uint32_t CPattern0::ReducedPatternIndex0(CPosition pos) const
{
	return (FullPatternIndex(pos, Pattern & ~MID) << PopCount(Pattern & MID)) + static_cast<uint32_t>(PExt(pos.GetO(), Pattern & MID));
}

uint32_t CPattern0::ReducedPatternIndex1(CPosition pos) const
{
	pos.FlipHorizontal();
	return ReducedPatternIndex0(pos);
}

uint32_t CPattern0::ReducedPatternIndex2(CPosition pos) const
{
	pos.FlipVertical();
	return ReducedPatternIndex0(pos);
}

uint32_t CPattern0::ReducedPatternIndex3(CPosition pos) const
{
	pos.FlipDiagonal();
	return ReducedPatternIndex0(pos);
}

uint32_t CPattern0::ReducedPatternIndex4(CPosition pos) const
{
	pos.FlipCodiagonal();
	return ReducedPatternIndex0(pos);
}

uint32_t CPattern0::ReducedPatternIndex5(CPosition pos) const
{
	pos.FlipVertical();
	pos.FlipHorizontal();
	return ReducedPatternIndex0(pos);
}

uint32_t CPattern0::ReducedPatternIndex6(CPosition pos) const
{
	pos.FlipDiagonal();
	pos.FlipHorizontal();
	return ReducedPatternIndex0(pos);
}

uint32_t CPattern0::ReducedPatternIndex7(CPosition pos) const
{
	pos.FlipCodiagonal();
	pos.FlipHorizontal();
	return ReducedPatternIndex0(pos);
}

float CPatternGroup::Eval(const CPosition & pos) const
{
	float sum = 0;
	for (const auto& it : m_group)
		sum += it->Eval(pos);
	return sum;
}

std::vector<std::vector<uint32_t>> CPatternGroup::GetConfigurations(const CPosition& pos) const
{
	std::vector<std::vector<uint32_t>> ret;
	ret.reserve(m_group.size());
	for (const auto& it : m_group)
		ret.push_back(std::move(it->GetConfigurations(pos)));
	return ret;
}

CPatternCollection::CPatternCollection(std::vector<std::shared_ptr<CPatternGroup>> collection)
	: m_collection(collection)
{
}

float CPatternCollection::Eval(const CPosition & pos) const
{
	return m_collection[pos.EmptyCount()]->Eval(pos);
}

std::vector<std::vector<uint32_t>> CPatternCollection::GetConfigurations(const CPosition & pos) const
{
	return m_collection[pos.EmptyCount()]->GetConfigurations(pos);
}

std::unique_ptr<CPattern> CreatePattern(const uint64_t pattern)
{
	if (pattern == FlipHorizontal(pattern))
		return std::make_unique<CPatternH>(pattern);
	if (pattern == FlipDiagonal(pattern))
		return std::make_unique<CPatternD>(pattern);
	else
		return std::make_unique<CPattern0>(pattern);
}