#include "pattern.h"

int64_t Pow_int(int64_t base, uint64_t exponent)
{
	if (exponent == 0) return 1;
	if (exponent % 2 == 0) { return Pow_int(base * base, exponent / 2); }
	else return base * Pow_int(base, exponent - 1);
}

namespace Pattern
{
	CBoxedPatternSet BoxedPatternSet(16);

	unsigned int SumPow3[32768]; // OPTIMIZATION: Replace look-up by small calculation if possible

	void For_each_configuration_in_pattern_do_fkt(const uint64_t Pattern, std::function<void(uint64_t, uint64_t)> fkt)
	{
		const uint64_t PatternSize = 1ULL << PopCount(Pattern);
		const uint64_t ExtractedCenter = PExt(0x0000001818000000ULL, Pattern);
		for (uint64_t i = 0; i < PatternSize; i++)
		{
			const uint64_t P = PDep(i, Pattern);
			for (uint64_t j = 0; j < PatternSize; j++)
			{
				if (i & j) continue; // fields can only be taken by one player
				if (((i | j) & ExtractedCenter) != ExtractedCenter) continue; // center fields can't be empty
				const uint64_t O = PDep(j, Pattern);

				fkt(P, O);
			}
		}
	}	
	
	// ################################################################################################
	//  CPatternH
	// ################################################################################################
	// ------------------------------------------------------------------------------------------------
	CPatternH::CPatternH(std::string name, uint64_t Pattern)
		: CPattern(std::move(name), Pattern, Pow_int(3, PopCount(Pattern)), (Pow_int(3, PopCount(Pattern & HALF & ~MID)) * Pow_int(2, PopCount(Pattern & HALF & MID))) * ((Pow_int(3, PopCount(Pattern & HALF & ~MID)) * Pow_int(2, PopCount(Pattern & HALF & MID))) + 1) / 2, 4),
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
		for (unsigned int i = 0; i < m_Occurrences; i++)
			m_weights[i].resize(m_FullSize, NAN);
	}
	void CPatternH::set_weights(const std::vector<float>& compressed_weights)
	{
		assert(compressed_weights.size() == m_ReducedSize);
		set_weights();

		For_each_configuration_in_pattern_do_fkt(Pattern,  [&](uint64_t P, uint64_t O) { m_weights[0][FullPatternIndex(P, O, Pattern )] = compressed_weights[ReducedPatternIndex0(P, O)]; });
		For_each_configuration_in_pattern_do_fkt(PatternC, [&](uint64_t P, uint64_t O) { m_weights[1][FullPatternIndex(P, O, PatternC)] = compressed_weights[ReducedPatternIndex1(P, O)]; });
		For_each_configuration_in_pattern_do_fkt(PatternV, [&](uint64_t P, uint64_t O) { m_weights[2][FullPatternIndex(P, O, PatternV)] = compressed_weights[ReducedPatternIndex2(P, O)]; });
		For_each_configuration_in_pattern_do_fkt(PatternD, [&](uint64_t P, uint64_t O) { m_weights[3][FullPatternIndex(P, O, PatternD)] = compressed_weights[ReducedPatternIndex3(P, O)]; });
	}

	unsigned int CPatternH::ReducedPatternIndex0(const uint64_t P, const uint64_t O) const
	{
		const uint64_t P_h = FlipHorizontal(P);
		const uint64_t O_h = FlipHorizontal(O);
		const unsigned int indexA = (FullPatternIndex(P  , O  , Pattern & HALF & ~MID) << PopCount(Pattern & HALF & MID)) + static_cast<unsigned int>(PExt(O  , Pattern & HALF & MID));
		const unsigned int indexB = (FullPatternIndex(P_h, O_h, Pattern & HALF & ~MID) << PopCount(Pattern & HALF & MID)) + static_cast<unsigned int>(PExt(O_h, Pattern & HALF & MID));
		return (indexA > indexB) ?
			(indexB * halfSize + indexA - ((indexB * indexB + indexB) >> 1)) :
			(indexA * halfSize + indexB - ((indexA * indexA + indexA) >> 1));
	}
	unsigned int CPatternH::ReducedPatternIndex1(const uint64_t P, const uint64_t O) const { return ReducedPatternIndex0(FlipCodiagonal(P), FlipCodiagonal(O)); }
	unsigned int CPatternH::ReducedPatternIndex2(const uint64_t P, const uint64_t O) const { return ReducedPatternIndex0(FlipVertical(P), FlipVertical(O)); }
	unsigned int CPatternH::ReducedPatternIndex3(const uint64_t P, const uint64_t O) const { return ReducedPatternIndex0(FlipDiagonal(P), FlipDiagonal(O)); }
	// ------------------------------------------------------------------------------------------------
	// ################################################################################################


	// ################################################################################################
	//  CPatternD
	// ################################################################################################
	// ------------------------------------------------------------------------------------------------
	CPatternD::CPatternD(std::string name, uint64_t Pattern)
		: CPattern(std::move(name), Pattern, Pow_int(3, PopCount(Pattern)), (Pow_int(3, PopCount(Pattern & DIAG & ~MID)) * Pow_int(2, PopCount(Pattern & DIAG & MID))) * (Pow_int(3, PopCount(Pattern & HALF & ~MID)) * Pow_int(2, PopCount(Pattern & HALF & MID))) * ((Pow_int(3, PopCount(Pattern & HALF & ~MID)) * Pow_int(2, PopCount(Pattern & HALF & MID))) + 1) / 2, 4),
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
		for (unsigned int i = 0; i < m_Occurrences; i++)
			m_weights[i].resize(m_FullSize, NAN);
	}
	void CPatternD::set_weights(const std::vector<float>& compressed_weights)
	{
		assert(compressed_weights.size() == m_ReducedSize);
		set_weights();

		For_each_configuration_in_pattern_do_fkt(Pattern , [&](uint64_t P, uint64_t O) { m_weights[0][FullPatternIndex(P, O, Pattern )] = compressed_weights[ReducedPatternIndex0(P, O)]; });
		For_each_configuration_in_pattern_do_fkt(PatternH, [&](uint64_t P, uint64_t O) { m_weights[1][FullPatternIndex(P, O, PatternH)] = compressed_weights[ReducedPatternIndex1(P, O)]; });
		For_each_configuration_in_pattern_do_fkt(PatternC, [&](uint64_t P, uint64_t O) { m_weights[2][FullPatternIndex(P, O, PatternC)] = compressed_weights[ReducedPatternIndex2(P, O)]; });
		For_each_configuration_in_pattern_do_fkt(PatternV, [&](uint64_t P, uint64_t O) { m_weights[3][FullPatternIndex(P, O, PatternV)] = compressed_weights[ReducedPatternIndex3(P, O)]; });
	}

	unsigned int CPatternD::ReducedPatternIndex0(const uint64_t P, const uint64_t O) const
	{
		const uint64_t P_d = FlipDiagonal(P);
		const uint64_t O_d = FlipDiagonal(O);
		const unsigned int indexA = (FullPatternIndex(P, O, Pattern & HALF & ~MID) << PopCount(Pattern & HALF & MID)) + static_cast<unsigned int>(PExt(O, Pattern & HALF & MID));
		const unsigned int indexB = (FullPatternIndex(P_d, O_d, Pattern & HALF & ~MID) << PopCount(Pattern & HALF & MID)) + static_cast<unsigned int>(PExt(O_d, Pattern & HALF & MID));
		const unsigned int indexD = (FullPatternIndex(P, O, Pattern & DIAG & ~MID) << PopCount(Pattern & DIAG & MID)) + static_cast<unsigned int>(PExt(O, Pattern & DIAG & MID));
		return (indexA > indexB) ?
			(diagSize * (indexB * halfSize + indexA - ((indexB * indexB + indexB) >> 1)) + indexD) :
			(diagSize * (indexA * halfSize + indexB - ((indexA * indexA + indexA) >> 1)) + indexD);
	}
	unsigned int CPatternD::ReducedPatternIndex1(const uint64_t P, const uint64_t O) const { return ReducedPatternIndex0(FlipHorizontal(P), FlipHorizontal(O)); }
	unsigned int CPatternD::ReducedPatternIndex2(const uint64_t P, const uint64_t O) const { return ReducedPatternIndex0(FlipCodiagonal(P), FlipCodiagonal(O)); }
	unsigned int CPatternD::ReducedPatternIndex3(const uint64_t P, const uint64_t O) const { return ReducedPatternIndex0(FlipVertical(P), FlipVertical(O)); }
	// ------------------------------------------------------------------------------------------------
	// ################################################################################################


	// ################################################################################################
	//  CPattern0
	// ################################################################################################
	// ------------------------------------------------------------------------------------------------
	CPattern0::CPattern0(std::string name, uint64_t Pattern)
		: CPattern(std::move(name), Pattern, Pow_int(3, PopCount(Pattern)), Pow_int(3, PopCount(Pattern & ~MID)) * Pow_int(2, PopCount(Pattern & MID)), 8),
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
		for (unsigned int i = 0; i < m_Occurrences; i++)
			m_weights[i].resize(m_FullSize, NAN);
	}
	void CPattern0::set_weights(const std::vector<float>& compressed_weights)
	{
		assert(compressed_weights.size() == m_ReducedSize);
		set_weights();
		
		For_each_configuration_in_pattern_do_fkt(Pattern  , [&](uint64_t P, uint64_t O) { m_weights[0][FullPatternIndex(P, O, Pattern  )] = compressed_weights[ReducedPatternIndex0(P, O)]; });
		For_each_configuration_in_pattern_do_fkt(PatternH , [&](uint64_t P, uint64_t O) { m_weights[1][FullPatternIndex(P, O, PatternH )] = compressed_weights[ReducedPatternIndex1(P, O)]; });
		For_each_configuration_in_pattern_do_fkt(PatternV , [&](uint64_t P, uint64_t O) { m_weights[2][FullPatternIndex(P, O, PatternV )] = compressed_weights[ReducedPatternIndex2(P, O)]; });
		For_each_configuration_in_pattern_do_fkt(PatternD , [&](uint64_t P, uint64_t O) { m_weights[3][FullPatternIndex(P, O, PatternD )] = compressed_weights[ReducedPatternIndex3(P, O)]; });
		For_each_configuration_in_pattern_do_fkt(PatternC , [&](uint64_t P, uint64_t O) { m_weights[4][FullPatternIndex(P, O, PatternC )] = compressed_weights[ReducedPatternIndex4(P, O)]; });
		For_each_configuration_in_pattern_do_fkt(PatternHV, [&](uint64_t P, uint64_t O) { m_weights[5][FullPatternIndex(P, O, PatternHV)] = compressed_weights[ReducedPatternIndex5(P, O)]; });
		For_each_configuration_in_pattern_do_fkt(PatternHD, [&](uint64_t P, uint64_t O) { m_weights[6][FullPatternIndex(P, O, PatternHD)] = compressed_weights[ReducedPatternIndex6(P, O)]; });
		For_each_configuration_in_pattern_do_fkt(PatternHC, [&](uint64_t P, uint64_t O) { m_weights[7][FullPatternIndex(P, O, PatternHC)] = compressed_weights[ReducedPatternIndex7(P, O)]; });
	}

	unsigned int CPattern0::ReducedPatternIndex0(const uint64_t P, const uint64_t O) const { return (FullPatternIndex(P, O, Pattern & ~MID) << PopCount(Pattern & MID)) + static_cast<unsigned int>(PExt(O, Pattern & MID)); }
	unsigned int CPattern0::ReducedPatternIndex1(const uint64_t P, const uint64_t O) const { return ReducedPatternIndex0(FlipHorizontal(P), FlipHorizontal(O)); }
	unsigned int CPattern0::ReducedPatternIndex2(const uint64_t P, const uint64_t O) const { return ReducedPatternIndex0(FlipVertical(P), FlipVertical(O)); }
	unsigned int CPattern0::ReducedPatternIndex3(const uint64_t P, const uint64_t O) const { return ReducedPatternIndex0(FlipDiagonal(P), FlipDiagonal(O)); }
	unsigned int CPattern0::ReducedPatternIndex4(const uint64_t P, const uint64_t O) const { return ReducedPatternIndex0(FlipCodiagonal(P), FlipCodiagonal(O)); }
	unsigned int CPattern0::ReducedPatternIndex5(const uint64_t P, const uint64_t O) const { return ReducedPatternIndex0(FlipHorizontal(FlipVertical(P)), FlipHorizontal(FlipVertical(O))); }
	unsigned int CPattern0::ReducedPatternIndex6(const uint64_t P, const uint64_t O) const { return ReducedPatternIndex0(FlipHorizontal(FlipDiagonal(P)), FlipHorizontal(FlipDiagonal(O))); }
	unsigned int CPattern0::ReducedPatternIndex7(const uint64_t P, const uint64_t O) const { return ReducedPatternIndex0(FlipHorizontal(FlipCodiagonal(P)), FlipHorizontal(FlipCodiagonal(O))); }
	// ------------------------------------------------------------------------------------------------
	// ################################################################################################

	uint64_t sumpow3(uint64_t exp)
	{
		int64_t sum = 0;
		while (exp)	{
			sum += Pow_int(3, BitScanLSB(exp));
			RemoveLSB(exp);
		}
		return sum;
	}

	uint64_t LookupPattern(const std::string& PatternName)
	{
		if (PatternName.substr(0, 2) == "0x")
			return std::stoull(PatternName, 0, 16);
		else
		{
			const std::string s = "pattern " + PatternName;
			if (gConfigurations.HasConfiguration(s))
				return std::stoull(gConfigurations.GetConfiguration(s), 0, 16);
			else
			{
				if (gConfigurations.Verbosity > -2)
					std::cerr << "ERROR: Pattern '" << PatternName << "' not defined." << std::endl;
				return 0;
			}
		}
	}

	// Pattern factory
	CPattern* NewPattern(const std::string& name, const uint64_t pattern)
	{
		if (pattern == FlipHorizontal(pattern)) return new CPatternH(name, pattern);
		if (pattern == FlipDiagonal(pattern))   return new CPatternD(name, pattern);
		else                                    return new CPattern0(name, pattern);
	}

	// Pattern factory
	CPattern* NewPattern(const std::string& name)
	{
		const uint64_t pattern = LookupPattern(name);
		if (pattern == 0)
			return nullptr;
		else
			return NewPattern(name, pattern);
	}

	std::vector<std::pair<std::string, uint64_t>> GetActivePattern()
	{
		assert(gConfigurations.HasConfiguration("active pattern"));
		
		std::vector<std::pair<std::string, uint64_t>> ActivePattern;
		std::string PatternString = gConfigurations.GetConfiguration("active pattern");
		
		const std::string delimiter = " ";
		std::string token;
		std::size_t pos;
		while ((pos = PatternString.find(delimiter)) != std::string::npos)
		{
			token = PatternString.substr(0, pos);
			ActivePattern.push_back(std::pair<std::string, uint64_t>(token, LookupPattern(token)));
			PatternString.erase(0, pos + delimiter.length());
		}
		if (!PatternString.empty())
			ActivePattern.push_back(std::pair<std::string, uint64_t>(PatternString, LookupPattern(PatternString)));

		return ActivePattern;
	}

	void Initialize(const bool weights, const bool singleton)
	{
		const uint64_t Boxes = 16;

		for (int i = 0; i < 32768; i++) SumPow3[i] = static_cast<unsigned int>(sumpow3(i));

		if (singleton)
		{
			std::vector<std::pair<std::string, uint64_t>> ActivePattern = GetActivePattern();

			if (gConfigurations.Verbosity > 1)
			{
				std::cout << std::hex;
				for (const auto& it : ActivePattern)
					std::cout << "Using Pattern " << it.first << " : " << it.second << std::endl;
				std::cout << std::dec;
			}

			std::string WeightsPath;
			if (weights)
			{
				assert(gConfigurations.HasConfiguration("ProgramFolderPath"));
				assert(gConfigurations.HasConfiguration("WeightsFolderPath"));
				WeightsPath = gConfigurations.GetConfiguration("ProgramFolderPath") + gConfigurations.GetConfiguration("WeightsFolderPath");
			}

			for (unsigned int i = 0; i < Boxes; i++)
			{
				CPatternSet set;
				for (auto& it : ActivePattern)
				{
					auto filename = WeightsPath + "range" + std::to_string(i) + "_" + it.first + ".weights";
					auto newPattern = NewPattern(it.first, it.second);
					if (weights)
					{
						std::vector<float> weightsVector;
						try
						{
							weightsVector = read_vector<float>(filename);
						}
						catch (...)
						{
							std::cerr << "WARNING: File '" << filename << "' not found." << std::endl;
							newPattern->set_weights();
							continue;
						}
						newPattern->set_weights(weightsVector);
					}
					else
					{
						newPattern->set_weights();
					}
					set.Add(newPattern);
				}
				BoxedPatternSet.Add(std::move(set));
			}
		}
	}
}

int EvaluatePattern(const uint64_t P, const uint64_t O, const uint64_t emptyCount)
{
	if (emptyCount == 0) return static_cast<int>((PopCount(P) << 1) - 64);
	const float score = Pattern::BoxedPatternSet.score(P, O, emptyCount);
	return CLAMP(RoundInt(score), -64, 64);
}