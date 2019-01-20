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
		const int8_t alpha;
		const int8_t beta;
	public:
		const int8_t depth;
		const uint8_t selectivity;
		int8_t best_score = -infinity; // best score achieved
		int8_t worst_depth = 99;
		uint8_t worst_selectivity = 0;
		CBestMoves best_moves{};
		const CPosition& pos;

		CStatusQuo(const CInput&);

		int8_t CurrentAlpha() const;

		CInput Play(CMove) const;
		CInput PlayZWS(CMove) const;

		std::optional<COutput> ImproveWith(const CAnalysisOutput& novum);
		std::optional<COutput> ImproveWithZWS(const COutput& novum, CMove);
		std::optional<COutput> ImproveWith(const COutput& novum, CMove);

		COutput AllMovesTried();
	};
}