#include "Pattern.h"

namespace Pattern
{
	static CSumPow3Cache sum_pow_3_cache;

	uint64_t sum_pow3(uint64_t exp)
	{
		uint64_t sum = 0;
		while (exp) {
			sum += Pow_int(3, BitScanLSB(exp));
			RemoveLSB(exp);
		}
		return sum;
	}

	CSumPow3Cache::CSumPow3Cache()
	{
		for (std::size_t i = 0; i < std::size(m_cache); i++)
			m_cache[i] = sum_pow3(i);
	}

	uint64_t CSumPow3Cache::SumPow3(uint64_t exp) const
	{
		return m_cache[exp];
	}

	uint32_t FullIndex(const CPosition& pos, const uint64_t pattern)
	{
		return sum_pow_3_cache.SumPow3(PExt(pos.GetP(), pattern)) + sum_pow_3_cache.SumPow3(PExt(pos.GetO(), pattern)) * 2;
	}

	namespace Configurations
	{
		CHorizontalSymmetric::CHorizontalSymmetric(uint64_t pattern)
			: CBase(pattern)
			, m_patternC(FlipCodiagonal(pattern))
			, m_patternV(FlipVertical(pattern))
			, m_patternD(FlipDiagonal(pattern))
			, m_half_size(Pow_int(3, PopCount(pattern & HALF & ~MID)) * Pow_int(2, PopCount(pattern & HALF & MID)))
		{
			assert(pattern == FlipHorizontal(pattern));
		}

		std::vector<uint32_t> CHorizontalSymmetric::Configurations(const CPosition& pos) const
		{
			return { ReducedIndex0(pos), ReducedIndex1(pos), ReducedIndex2(pos), ReducedIndex3(pos) };
		}

		uint32_t CHorizontalSymmetric::Configuration(const CPosition& pos, std::size_t index) const
		{
			switch (index)
			{
				case 0: return ReducedIndex0(pos);
				case 1: return ReducedIndex1(pos);
				case 2: return ReducedIndex2(pos);
				case 3: return ReducedIndex3(pos);
				default: throw;
			}
		}

		std::vector<uint64_t> CHorizontalSymmetric::Patterns() const
		{
			return { Pattern, m_patternC, m_patternV, m_patternD };
		}

		uint32_t CHorizontalSymmetric::ReducedSize() const
		{
			return m_half_size * (m_half_size + 1) / 2;
		}

		uint32_t CHorizontalSymmetric::Occurrences() const
		{
			return 4;
		}

		uint32_t CHorizontalSymmetric::ReducedIndex0(CPosition pos) const
		{
			const uint32_t indexA = (FullIndex(pos, Pattern & HALF & ~MID) << PopCount(Pattern & HALF & MID)) + static_cast<uint32_t>(PExt(pos.GetO(), Pattern & HALF & MID));
			pos.FlipHorizontal();
			const uint32_t indexB = (FullIndex(pos, Pattern & HALF & ~MID) << PopCount(Pattern & HALF & MID)) + static_cast<uint32_t>(PExt(pos.GetO(), Pattern & HALF & MID));

			if (indexA > indexB)
				return indexB * m_half_size + indexA - ((indexB * indexB + indexB) / 2);
			else
				return indexA * m_half_size + indexB - ((indexA * indexA + indexA) / 2);
		}

		uint32_t CHorizontalSymmetric::ReducedIndex1(CPosition pos) const
		{
			pos.FlipCodiagonal();
			return ReducedIndex0(pos);
		}

		uint32_t CHorizontalSymmetric::ReducedIndex2(CPosition pos) const
		{
			pos.FlipVertical();
			return ReducedIndex0(pos);
		}

		uint32_t CHorizontalSymmetric::ReducedIndex3(CPosition pos) const
		{
			pos.FlipDiagonal();
			return ReducedIndex0(pos);
		}

		CDiagonalSymmetric::CDiagonalSymmetric(uint64_t pattern)
			: CBase(pattern)
			, m_patternH(FlipHorizontal(pattern))
			, m_patternC(FlipCodiagonal(pattern))
			, m_patternV(FlipVertical(pattern))
			, m_half_size(Pow_int(3, PopCount(pattern & HALF & ~MID)) * Pow_int(2, PopCount(pattern & HALF & MID)))
			, m_diag_size(Pow_int(3, PopCount(pattern & DIAG & ~MID)) * Pow_int(2, PopCount(pattern & DIAG & MID)))
		{
			assert(pattern == FlipDiagonal(pattern));
		}

		std::vector<uint32_t> CDiagonalSymmetric::Configurations(const CPosition& pos) const
		{
			return { ReducedIndex0(pos), ReducedIndex1(pos), ReducedIndex2(pos), ReducedIndex3(pos) };
		}

		uint32_t CDiagonalSymmetric::Configuration(const CPosition& pos, std::size_t index) const
		{
			switch (index)
			{
				case 0: return ReducedIndex0(pos);
				case 1: return ReducedIndex1(pos);
				case 2: return ReducedIndex2(pos);
				case 3: return ReducedIndex3(pos);
				default: throw;
			}
		}

		std::vector<uint64_t> CDiagonalSymmetric::Patterns() const
		{
			return { Pattern, m_patternH, m_patternC, m_patternV };
		}

		uint32_t CDiagonalSymmetric::ReducedSize() const
		{
			return m_diag_size * m_half_size * (m_half_size + 1) / 2;
		}

		uint32_t CDiagonalSymmetric::Occurrences() const
		{
			return 4;
		}

		uint32_t CDiagonalSymmetric::ReducedIndex0(CPosition pos) const
		{
			const uint32_t indexA = (FullIndex(pos, Pattern & HALF & ~MID) << PopCount(Pattern & HALF & MID)) + static_cast<uint32_t>(PExt(pos.GetO(), Pattern & HALF & MID));
			const uint32_t indexD = (FullIndex(pos, Pattern & DIAG & ~MID) << PopCount(Pattern & DIAG & MID)) + static_cast<uint32_t>(PExt(pos.GetO(), Pattern & DIAG & MID));
			pos.FlipDiagonal();
			const uint32_t indexB = (FullIndex(pos, Pattern & HALF & ~MID) << PopCount(Pattern & HALF & MID)) + static_cast<uint32_t>(PExt(pos.GetO(), Pattern & HALF & MID));

			if (indexA > indexB)
				return m_diag_size * (indexB * m_half_size + indexA - ((indexB * indexB + indexB) / 2)) + indexD;
			else
				return m_diag_size * (indexA * m_half_size + indexB - ((indexA * indexA + indexA) / 2)) + indexD;
		}

		uint32_t CDiagonalSymmetric::ReducedIndex1(CPosition pos) const
		{
			pos.FlipHorizontal();
			return ReducedIndex0(pos);
		}

		uint32_t CDiagonalSymmetric::ReducedIndex2(CPosition pos) const
		{
			pos.FlipCodiagonal();
			return ReducedIndex0(pos);
		}

		uint32_t CDiagonalSymmetric::ReducedIndex3(CPosition pos) const
		{
			pos.FlipVertical();
			return ReducedIndex0(pos);
		}

		CAsymmetric::CAsymmetric(uint64_t pattern)
			: CBase(pattern)
			, m_patternH(FlipHorizontal(pattern))
			, m_patternV(FlipVertical(pattern))
			, m_patternD(FlipDiagonal(pattern))
			, m_patternC(FlipCodiagonal(pattern))
			, m_patternHV(FlipVertical(FlipHorizontal(pattern)))
			, m_patternHD(FlipDiagonal(FlipHorizontal(pattern)))
			, m_patternHC(FlipCodiagonal(FlipHorizontal(pattern)))
		{}

		std::vector<uint32_t> CAsymmetric::Configurations(const CPosition& pos) const
		{
			return {
				ReducedIndex0(pos), ReducedIndex1(pos), ReducedIndex2(pos), ReducedIndex3(pos),
				ReducedIndex4(pos), ReducedIndex5(pos), ReducedIndex6(pos), ReducedIndex7(pos)
			};
		}

		uint32_t CAsymmetric::Configuration(const CPosition& pos, std::size_t index) const
		{
			switch (index)
			{
				case 0: return ReducedIndex0(pos);
				case 1: return ReducedIndex1(pos);
				case 2: return ReducedIndex2(pos);
				case 3: return ReducedIndex3(pos);
				case 4: return ReducedIndex4(pos);
				case 5: return ReducedIndex5(pos);
				case 6: return ReducedIndex6(pos);
				case 7: return ReducedIndex7(pos);
				default: throw;
			}
		}

		std::vector<uint64_t> CAsymmetric::Patterns() const
		{
			return { Pattern, m_patternH, m_patternV, m_patternD, m_patternC, m_patternHV, m_patternHD, m_patternHC };
		}

		uint32_t CAsymmetric::ReducedSize() const
		{
			return Pow_int(3, PopCount(Pattern & ~MID)) * Pow_int(2, PopCount(Pattern & MID));
		}

		uint32_t CAsymmetric::Occurrences() const
		{
			return 8;
		}

		uint32_t CAsymmetric::ReducedIndex0(CPosition pos) const
		{
			return (FullIndex(pos, Pattern & ~MID) << PopCount(Pattern & MID)) + static_cast<uint32_t>(PExt(pos.GetO(), Pattern & MID));
		}

		uint32_t CAsymmetric::ReducedIndex1(CPosition pos) const
		{
			pos.FlipHorizontal();
			return ReducedIndex0(pos);
		}

		uint32_t CAsymmetric::ReducedIndex2(CPosition pos) const
		{
			pos.FlipVertical();
			return ReducedIndex0(pos);
		}

		uint32_t CAsymmetric::ReducedIndex3(CPosition pos) const
		{
			pos.FlipDiagonal();
			return ReducedIndex0(pos);
		}

		uint32_t CAsymmetric::ReducedIndex4(CPosition pos) const
		{
			pos.FlipCodiagonal();
			return ReducedIndex0(pos);
		}

		uint32_t CAsymmetric::ReducedIndex5(CPosition pos) const
		{
			pos.FlipVertical();
			pos.FlipHorizontal();
			return ReducedIndex0(pos);
		}

		uint32_t CAsymmetric::ReducedIndex6(CPosition pos) const
		{
			pos.FlipDiagonal();
			pos.FlipHorizontal();
			return ReducedIndex0(pos);
		}

		uint32_t CAsymmetric::ReducedIndex7(CPosition pos) const
		{
			pos.FlipCodiagonal();
			pos.FlipHorizontal();
			return ReducedIndex0(pos);
		}

		std::unique_ptr<CBase> CreatePattern(const uint64_t pattern)
		{
			if (pattern == FlipHorizontal(pattern))
				return std::make_unique<CHorizontalSymmetric>(pattern);
			if (pattern == FlipDiagonal(pattern))
				return std::make_unique<CDiagonalSymmetric>(pattern);
			else
				return std::make_unique<CAsymmetric>(pattern);
		}
}

	namespace Eval
	{
		CHorizontalSymmetric::CHorizontalSymmetric(uint64_t pattern, std::vector<CWeights> weights)
			: CBase(pattern)
			, m_patternC(FlipCodiagonal(pattern))
			, m_patternV(FlipVertical(pattern))
			, m_patternD(FlipDiagonal(pattern))
			, m_w0(std::move(weights[0]))
			, m_w1(std::move(weights[1]))
			, m_w2(std::move(weights[2]))
			, m_w3(std::move(weights[3]))
		{}

		float CHorizontalSymmetric::Eval(const CPosition & pos) const
		{
			return m_w0[FullIndex(pos, Pattern)]
				+ m_w1[FullIndex(pos, m_patternC)]
				+ m_w2[FullIndex(pos, m_patternV)]
				+ m_w3[FullIndex(pos, m_patternD)];
		}

		CDiagonalSymmetric::CDiagonalSymmetric(uint64_t pattern, std::vector<CWeights> weights)
			: CBase(pattern)
			, m_patternH(FlipHorizontal(pattern))
			, m_patternC(FlipCodiagonal(pattern))
			, m_patternV(FlipVertical(pattern))
			, m_w0(std::move(weights[0]))
			, m_w1(std::move(weights[1]))
			, m_w2(std::move(weights[2]))
			, m_w3(std::move(weights[3]))
		{}

		float CDiagonalSymmetric::Eval(const CPosition & pos) const
		{
			return m_w0[FullIndex(pos, Pattern)]
				+ m_w1[FullIndex(pos, m_patternH)]
				+ m_w2[FullIndex(pos, m_patternC)]
				+ m_w3[FullIndex(pos, m_patternV)];
		}

		Pattern::Eval::CAsymmetric::CAsymmetric(uint64_t pattern, std::vector<CWeights> weights)
			: CBase(pattern)
			, m_patternH(FlipHorizontal(pattern))
			, m_patternV(FlipVertical(pattern))
			, m_patternD(FlipDiagonal(pattern))
			, m_patternC(FlipCodiagonal(pattern))
			, m_patternHV(FlipVertical(FlipHorizontal(pattern)))
			, m_patternHD(FlipDiagonal(FlipHorizontal(pattern)))
			, m_patternHC(FlipCodiagonal(FlipHorizontal(pattern)))
			, m_w0(std::move(weights[0]))
			, m_w1(std::move(weights[1]))
			, m_w2(std::move(weights[2]))
			, m_w3(std::move(weights[3]))
			, m_w4(std::move(weights[4]))
			, m_w5(std::move(weights[5]))
			, m_w6(std::move(weights[6]))
			, m_w7(std::move(weights[7]))
		{}

		float CAsymmetric::Eval(const CPosition & pos) const
		{
			return m_w0[FullIndex(pos, Pattern)]
				+ m_w1[FullIndex(pos, m_patternH)]
				+ m_w2[FullIndex(pos, m_patternV)]
				+ m_w3[FullIndex(pos, m_patternD)]
				+ m_w4[FullIndex(pos, m_patternC)]
				+ m_w5[FullIndex(pos, m_patternHV)]
				+ m_w6[FullIndex(pos, m_patternHD)]
				+ m_w7[FullIndex(pos, m_patternHC)];
		}

		std::unique_ptr<CBase> CreatePattern(uint64_t pattern, std::vector<CWeights> weights)
		{
			if (pattern == FlipHorizontal(pattern))
				return std::make_unique<CHorizontalSymmetric>(pattern, std::move(weights));
			if (pattern == FlipDiagonal(pattern))
				return std::make_unique<CDiagonalSymmetric>(pattern, std::move(weights));
			else
				return std::make_unique<CAsymmetric>(pattern, std::move(weights));
		}

		void For_each_config(const uint64_t pattern, std::function<void(CPosition)> fkt)
		{
			const uint64_t MID = 0x0000001818000000ui64;
			const uint64_t size = 1ui64 << PopCount(pattern);
			const uint64_t extracted_center = PExt(MID, pattern);
			for (uint64_t i = 0; i < size; i++)
			{
				const uint64_t P = PDep(i, pattern);
				for (uint64_t j = 0; j < size; j++)
				{
					if (i & j) continue; // fields can only be taken by one player
					if (((i | j) & extracted_center) != extracted_center) continue; // center fields can't be empty
					const uint64_t O = PDep(j, pattern);

					fkt(CPosition(P, O));
				}
			}
		}

		uint32_t FullSize(uint64_t pattern)
		{
			return Pow_int(3, PopCount(pattern));
		}

		std::unique_ptr<CBase> CreatePattern(const uint64_t pattern, const CWeights& compressed)
		{
			auto config = Configurations::CreatePattern(pattern);

			// Reserve memory
			const auto occurrences = config->Occurrences();
			std::vector<CWeights> weights(occurrences);
			for (auto& weight : weights)
				weight.resize(FullSize(pattern));

			//Decompress
			const auto patterns = config->Patterns();
			for (std::size_t i = 0; i < occurrences; i++)
			{
				For_each_config(patterns[i],
					[&](const CPosition& pos) { weights[i][FullIndex(pos, patterns[i])] = compressed[config->Configuration(pos, i)]; }
				);
			}

			return Eval::CreatePattern(pattern, std::move(weights));
		}

		CPack::CPack(const std::vector<std::pair<uint64_t, CWeights>>& pair)
		{
			for (const auto& it : pair)
				Add(CreatePattern(it.first, it.second));
		}

		void CPack::Add(std::unique_ptr<CBase>&& elem)
		{
			m_pack.emplace_back(std::move(elem));
		}

		std::size_t CPack::size() const
		{
			return m_pack.size();
		}

		std::vector<uint64_t> CPack::Pattern() const
		{
			std::vector<uint64_t> ret;
			ret.reserve(m_pack.size());
			for (const auto& it : m_pack)
				ret.push_back(it->Pattern);
			return ret;
		}

		float CPack::Eval(const CPosition& pos) const
		{
			float sum = 0;
			for (const auto& it : m_pack)
				sum += it->Eval(pos);
			return sum;
		}

		void CEnsemble::Set(uint8_t empty_count, std::shared_ptr<CPack> pack)
		{
			m_packs[empty_count] = std::move(pack);
		}

		float CEnsemble::Eval(const CPosition& pos) const
		{
			return m_packs[pos.EmptyCount()]->Eval(pos);
		}
	}
}