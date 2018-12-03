#pragma once
#include "Moves.h"
#include "Position.h"
#include <cstdint>

namespace Search
{
	class CInput
	{
		bool Constrained() const;
	public:
		const CPosition pos;
		const int8_t alpha, beta;
		const int8_t depth;
		const uint8_t selectivity;

		CInput(CPosition, int8_t alpha, int8_t beta, int8_t depth, uint8_t selectivity);

		CInput PlayPass() const;
	};
}