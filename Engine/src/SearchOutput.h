#pragma once
#include "Moves.h"
#include <cassert>
#include <cstdint>

namespace Search
{
	class COutput
	{
		bool Constrained() const;
	public:
		int8_t min, max;
		int8_t depth;
		uint8_t selectivity;
		CMove PV, AV;

		COutput();
		COutput(int8_t min, int8_t max, int8_t depth, uint8_t selectivity, CMove PV = CMove::invalid, CMove AV = CMove::invalid);
		COutput(int8_t score, int8_t depth, uint8_t selectivity, CMove PV = CMove::invalid, CMove AV = CMove::invalid);

		friend COutput operator-(const COutput&);
	};
}