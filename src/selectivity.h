#pragma once
#include "position.h"
#include "utility.h"
#include <cstdint>
#include <string>

const int NO_SELECTIVITY = 0;

class CSelectivity
{
public:
	float sigmaFactor;
	int percentile;
	
	CSelectivity(const float sigmaFactor, const int percentile) : sigmaFactor(sigmaFactor), percentile(percentile) {}
};

namespace details
{
	const CSelectivity SelectivityTable[10] = {
		CSelectivity(99.f, 100), // 0
		CSelectivity(3.3f, 99), // 1
		CSelectivity(2.6f, 98), // 2
		CSelectivity(2.0f, 95), // 3
		CSelectivity(1.5f, 87), // 4
		CSelectivity(1.2f, 77), // 5
		CSelectivity(1.1f, 73), // 6
		CSelectivity(0.7f, 52), // 7
		CSelectivity(0.5f, 38), // 8
		CSelectivity(0.2f, 16), // 9
	};
}

namespace Selectivity
{
	inline float GetSigmaFactor(const unsigned int selectivity) { return details::SelectivityTable[selectivity].sigmaFactor; }
	inline int   GetPercentile (const unsigned int selectivity) { return details::SelectivityTable[selectivity].percentile; }
	inline std::string GetDepthSelectivity(const int depth, const unsigned int selectivity, const int emptiesCount)
	{
		//if (depth == emptiesCount) 
		//	return std::string("      ");
		//else
		{
			if (selectivity == NO_SELECTIVITY)
			{
				if (depth < 10)
					return std::string(" ") + std::to_string(depth) + "    ";
				else
					return std::to_string(depth) + "    ";
			}
			else
			{
				if (depth < 10)
					return std::string(" ") + std::to_string(depth) + "@" + std::to_string(GetPercentile(selectivity));
				else
					return std::to_string(depth) + "@" + std::to_string(GetPercentile(selectivity));
			}
		}
	}
}