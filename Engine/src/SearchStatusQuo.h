#pragma once
#include "Moves.h"
#include "Position.h"
#include "Search.h"
#include "SearchInput.h"
#include "SearchOutput.h"
#include <cstdint>
#include <optional>

namespace Search
{
	class CStatusQuo
	{
		bool Constrained() const;
	public:
		int8_t alpha, beta;
		const int8_t depth;
		const uint8_t selectivity;
		int8_t best_score = -infinity; // best score achieved
		CBestMoves best_moves{};
		const CPosition& pos;

		CStatusQuo(const CInput&);

		CInput Play(CMove) const;
		CInput PlayZWS(CMove) const;

		std::optional<COutput> ImproveWith(const COutput& novum);
		std::optional<COutput> ImproveWith(const COutput& novum, CMove);

		COutput AllMovesTried();
	};
}