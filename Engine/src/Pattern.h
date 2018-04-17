#pragma once
#include "Utility.h"
#include "ConfigFile.h"
#include "Position.h"
#include <cstdint>
#include <functional>
#include <iostream>
#include <vector>

namespace Pattern
{
	class CPattern;
	class CPatternH;
	class CPatternD;
	class CPattern0;
	class CBoxedPatternSet;
	
	extern unsigned int SumPow3[32768];
	extern CBoxedPatternSet BoxedPatternSet;

	void Initialize(const bool weights = true, const bool singleton = true);

	void For_each_configuration_in_pattern_do_fkt(const uint64_t Pattern, std::function<void(uint64_t, uint64_t)> fkt);

	uint64_t LookupPattern(const std::string& PatternName);

	CPattern* NewPattern(const std::string& name, const uint64_t pattern);
	CPattern* NewPattern(const std::string& name);
	
	std::vector<std::string> GetActivePattern();
	
	inline unsigned int FullPatternIndex(const uint64_t P, const uint64_t O, const uint64_t mask);


	class CPattern
	{
	protected:
		const std::string m_name;
		const uint64_t Pattern;
		const unsigned int m_FullSize, m_ReducedSize, m_Occurrences;
	public:
		CPattern(std::string name, uint64_t Pattern, unsigned int FullSize, unsigned int ReducedSize, unsigned int Occurrences)
			: m_name(std::move(name)), Pattern(Pattern), m_FullSize(FullSize), m_ReducedSize(ReducedSize), m_Occurrences(Occurrences) {}
		virtual inline ~CPattern() {}

				inline	std::string		GetName()		const { return m_name; }
				inline	uint64_t		GetPattern()	const { return Pattern; }
				inline	unsigned int	Occurrences()	const { return m_Occurrences; }
				inline	unsigned int	FullSize()		const { return m_FullSize; }
				inline	unsigned int	ReducedSize()	const { return m_ReducedSize; }

		virtual			void						set_weights() = 0;
		virtual			void						set_weights(const std::vector<float>& compressed_weights) = 0;
		virtual 		float						score(const uint64_t P, const uint64_t O) const = 0;
		virtual 		std::vector<float>			GetScoreVec(const uint64_t P, const uint64_t O) const = 0;
		virtual 		std::vector<unsigned int>	GetConfigurationVec(const uint64_t P, const uint64_t O) const = 0;
	};

	class CPatternH : public CPattern
	{
		static const uint64_t HALF = 0x0F0F0F0F0F0F0F0FULL;
		static const uint64_t MID  = 0x0000001818000000ULL;
		const uint64_t PatternC, PatternV, PatternD;
		const unsigned int halfSize;
		std::vector<std::vector<float>> m_weights; //m_weights[Index][FullIndex]

	public:
		CPatternH(std::string name, uint64_t Pattern);

		void set_weights() override;
		void set_weights(const std::vector<float>& compressed_weights) override;
		
		inline float score(const uint64_t P, const uint64_t O) const override;
		inline std::vector<float>			GetScoreVec			(const uint64_t P, const uint64_t O) const override;
		inline std::vector<unsigned int> 	GetConfigurationVec	(const uint64_t P, const uint64_t O) const override;
	private:
		unsigned int ReducedPatternIndex0(const uint64_t P, const uint64_t O) const;
		unsigned int ReducedPatternIndex1(const uint64_t P, const uint64_t O) const;
		unsigned int ReducedPatternIndex2(const uint64_t P, const uint64_t O) const;
		unsigned int ReducedPatternIndex3(const uint64_t P, const uint64_t O) const;
	};
	
	class CPatternD : public CPattern
	{
		static const uint64_t HALF = 0x0080C0E0F0F8FCFEULL;
		static const uint64_t DIAG = 0x8040201008040201ULL;
		static const uint64_t MID  = 0x0000001818000000ULL;
		const uint64_t PatternH, PatternC, PatternV;
		const unsigned int halfSize, diagSize;
		std::vector<std::vector<float>> m_weights; //m_weights[Index][FullIndex]

	public:
		CPatternD(std::string name, uint64_t Pattern);

		void set_weights() override;
		void set_weights(const std::vector<float>& compressed_weights) override;

		inline float score(const uint64_t P, const uint64_t O) const override;
		inline std::vector<float>			GetScoreVec			(const uint64_t P, const uint64_t O) const override;
		inline std::vector<unsigned int> 	GetConfigurationVec	(const uint64_t P, const uint64_t O) const override;
	private:
		unsigned int ReducedPatternIndex0(const uint64_t P, const uint64_t O) const;
		unsigned int ReducedPatternIndex1(const uint64_t P, const uint64_t O) const;
		unsigned int ReducedPatternIndex2(const uint64_t P, const uint64_t O) const;
		unsigned int ReducedPatternIndex3(const uint64_t P, const uint64_t O) const;
	};
	
	class CPattern0 : public CPattern
	{
		static const uint64_t MID = 0x0000001818000000ULL;
		const uint64_t PatternH, PatternV, PatternD, PatternC, PatternHV, PatternHD, PatternHC;
		std::vector<std::vector<float>> m_weights; //m_weights[Index][FullIndex]
		
	public:
		CPattern0(std::string name, uint64_t Pattern);

		void set_weights() override;
		void set_weights(const std::vector<float>& compressed_weights) override;

		inline float score(const uint64_t P, const uint64_t O) const override;
		inline std::vector<float>			GetScoreVec			(const uint64_t P, const uint64_t O) const override;
		inline std::vector<unsigned int> 	GetConfigurationVec	(const uint64_t P, const uint64_t O) const override;
	private:
		unsigned int ReducedPatternIndex0(const uint64_t P, const uint64_t O) const;
		unsigned int ReducedPatternIndex1(const uint64_t P, const uint64_t O) const;
		unsigned int ReducedPatternIndex2(const uint64_t P, const uint64_t O) const;
		unsigned int ReducedPatternIndex3(const uint64_t P, const uint64_t O) const;
		unsigned int ReducedPatternIndex4(const uint64_t P, const uint64_t O) const;
		unsigned int ReducedPatternIndex5(const uint64_t P, const uint64_t O) const;
		unsigned int ReducedPatternIndex6(const uint64_t P, const uint64_t O) const;
		unsigned int ReducedPatternIndex7(const uint64_t P, const uint64_t O) const;
	};

	class CPatternSet
	{
		std::vector<CPattern*> m_pattern;
	public:
		CPatternSet() = default;
		CPatternSet(const CPatternSet& o) = delete;
		inline CPatternSet(CPatternSet&& o);
		inline ~CPatternSet();
		
		inline const CPattern* operator[](const std::size_t i) const;
		inline void Add(CPattern* pattern); /// Takes ownership of pattern.
		inline float score(const uint64_t P, const uint64_t O) const;
		inline std::vector<std::vector<unsigned int>> GetConfigurationVecs(const uint64_t P, const uint64_t O) const;
	};

	class CBoxedPatternSet
	{
		uint64_t Boxes;
		std::vector<CPatternSet> m_patternset;
	public:
		CBoxedPatternSet(uint64_t Boxes) : Boxes(Boxes) {}
		
		inline const CPatternSet& operator[](const std::size_t i) const;
		inline void Add(CPatternSet&& patternset);
		inline float score(const uint64_t P, const uint64_t O) const;
		inline float score(const uint64_t P, const uint64_t O, const uint64_t emptyCount) const;
		inline std::vector<std::vector<unsigned int>> GetConfigurationVecs(const uint64_t P, const uint64_t O) const;
		inline std::vector<std::vector<unsigned int>> GetConfigurationVecs(const uint64_t P, const uint64_t O, const uint64_t emptyCount) const;
	};
}

       int EvaluatePattern(const uint64_t P, const uint64_t O, const uint64_t emptyCount);
inline int EvaluatePattern(const uint64_t P, const uint64_t O);

// ################################################################################################
// Inline section
// ################################################################################################
// ------------------------------------------------------------------------------------------------
namespace Pattern
{
	inline unsigned int FullPatternIndex(const uint64_t P, const uint64_t O, const uint64_t mask)
	{ 
		assert(PopCount(mask) <= 15);
		return SumPow3[PExt(P, mask)] + (SumPow3[PExt(O, mask)] << 1); 
	}

	inline float CPatternH::score(const uint64_t P, const uint64_t O) const
	{
		return m_weights[0][FullPatternIndex(P, O, Pattern )]
			 + m_weights[1][FullPatternIndex(P, O, PatternC)]
			 + m_weights[2][FullPatternIndex(P, O, PatternV)]
			 + m_weights[3][FullPatternIndex(P, O, PatternD)];
	}

	inline std::vector<float> CPatternH::GetScoreVec(const uint64_t P, const uint64_t O) const
	{
		return std::vector<float>{
			m_weights[0][FullPatternIndex(P, O, Pattern )],
			m_weights[1][FullPatternIndex(P, O, PatternC)],
			m_weights[2][FullPatternIndex(P, O, PatternV)],
			m_weights[3][FullPatternIndex(P, O, PatternD)]};
	}

	inline std::vector<unsigned int> CPatternH::GetConfigurationVec(const uint64_t P, const uint64_t O) const
	{
		return std::vector<unsigned int>{
			ReducedPatternIndex0(P, O),
			ReducedPatternIndex1(P, O),
			ReducedPatternIndex2(P, O),
			ReducedPatternIndex3(P, O)};
	}

	inline float CPatternD::score(const uint64_t P, const uint64_t O) const
	{
		return m_weights[0][FullPatternIndex(P, O, Pattern )]
			 + m_weights[1][FullPatternIndex(P, O, PatternH)]
			 + m_weights[2][FullPatternIndex(P, O, PatternC)]
			 + m_weights[3][FullPatternIndex(P, O, PatternV)];
	}

	inline std::vector<float> CPatternD::GetScoreVec(const uint64_t P, const uint64_t O) const
	{
		return std::vector<float>{
			m_weights[0][FullPatternIndex(P, O, Pattern )],
			m_weights[1][FullPatternIndex(P, O, PatternH)],
			m_weights[2][FullPatternIndex(P, O, PatternC)],
			m_weights[3][FullPatternIndex(P, O, PatternV)]};
	}
	
	inline std::vector<unsigned int> CPatternD::GetConfigurationVec(const uint64_t P, const uint64_t O) const
	{
		return std::vector<unsigned int>{
			ReducedPatternIndex0(P, O),
			ReducedPatternIndex1(P, O),
			ReducedPatternIndex2(P, O),
			ReducedPatternIndex3(P, O)};
	}

	inline float CPattern0::score(const uint64_t P, const uint64_t O) const
	{
		return m_weights[0][FullPatternIndex(P, O, Pattern  )]
			 + m_weights[1][FullPatternIndex(P, O, PatternH )]
			 + m_weights[2][FullPatternIndex(P, O, PatternV )]
			 + m_weights[3][FullPatternIndex(P, O, PatternD )]
			 + m_weights[4][FullPatternIndex(P, O, PatternC )]
			 + m_weights[5][FullPatternIndex(P, O, PatternHV)]
			 + m_weights[6][FullPatternIndex(P, O, PatternHD)]
			 + m_weights[7][FullPatternIndex(P, O, PatternHC)];
	}

	inline std::vector<float> CPattern0::GetScoreVec(const uint64_t P, const uint64_t O) const
	{
		return std::vector<float>{
			m_weights[0][FullPatternIndex(P, O, Pattern  )],
			m_weights[1][FullPatternIndex(P, O, PatternH )],
			m_weights[2][FullPatternIndex(P, O, PatternV )],
			m_weights[3][FullPatternIndex(P, O, PatternD )],
			m_weights[4][FullPatternIndex(P, O, PatternC )],
			m_weights[5][FullPatternIndex(P, O, PatternHV)],
			m_weights[6][FullPatternIndex(P, O, PatternHD)],
			m_weights[7][FullPatternIndex(P, O, PatternHC)]};
	}

	inline std::vector<unsigned int> CPattern0::GetConfigurationVec(const uint64_t P, const uint64_t O) const
	{
		return std::vector<unsigned int>{
			ReducedPatternIndex0(P, O),
			ReducedPatternIndex1(P, O),
			ReducedPatternIndex2(P, O),
			ReducedPatternIndex3(P, O),
			ReducedPatternIndex4(P, O),
			ReducedPatternIndex5(P, O),
			ReducedPatternIndex6(P, O),
			ReducedPatternIndex7(P, O)};
	}
	
	inline const CPattern* CPatternSet::operator[](const std::size_t i) const
	{
		return m_pattern[i];
	}

	inline CPatternSet::CPatternSet(CPatternSet&& o)
	{ 
		std::swap(m_pattern, o.m_pattern); 
	}
	
	inline CPatternSet::~CPatternSet()
	{
		for (auto it : m_pattern)
			delete it;
	}
	
	inline void CPatternSet::Add(CPattern* pattern)
	{
		m_pattern.push_back(pattern);
	}
	
	inline float CPatternSet::score(const uint64_t P, const uint64_t O) const
	{
		float sum = 0.0;
		for (const auto& it : m_pattern)
			sum += it->score(P, O);
		return sum;
	}
	
	inline std::vector<std::vector<unsigned int>> CPatternSet::GetConfigurationVecs(const uint64_t P, const uint64_t O) const
	{
		std::vector<std::vector<unsigned int>> ret;
		ret.reserve(m_pattern.size());
		for (auto it : m_pattern)
			ret.push_back(std::move(it->GetConfigurationVec(P, O)));
		return ret;
	}
	
	inline const CPatternSet& CBoxedPatternSet::operator[](const std::size_t i) const
	{
		return m_patternset[i];
	}
	
	inline void CBoxedPatternSet::Add(CPatternSet&& patternset)
	{
		m_patternset.push_back(std::move(patternset));
	}
	
	inline float CBoxedPatternSet::score(const uint64_t P, const uint64_t O) const
	{
		return score(P, O, CPosition(P, O).EmptyCount());
	}
	
	inline float CBoxedPatternSet::score(const uint64_t P, const uint64_t O, const uint64_t emptyCount) const
	{
		assert(emptyCount > 0);
		const uint64_t BoxIndex = std::min((emptyCount - 1) / 3, Boxes - 1);
		return m_patternset[BoxIndex].score(P, O);
	}
	
	inline std::vector<std::vector<unsigned int>> CBoxedPatternSet::GetConfigurationVecs(const uint64_t P, const uint64_t O) const
	{
		return GetConfigurationVecs(P, O, CPosition(P, O).EmptyCount());
	}
	
	inline std::vector<std::vector<unsigned int>> CBoxedPatternSet::GetConfigurationVecs(const uint64_t P, const uint64_t O, const uint64_t emptyCount) const
	{
		assert(emptyCount > 0);
		const uint64_t BoxIndex = std::min((emptyCount - 1) / 3, Boxes - 1);
		return m_patternset[BoxIndex].GetConfigurationVecs(P, O);
	}
}

inline int EvaluatePattern(const uint64_t P, const uint64_t O)
{
	return EvaluatePattern(P, O, CPosition(P, O).EmptyCount());
}
// ------------------------------------------------------------------------------------------------
// ################################################################################################

