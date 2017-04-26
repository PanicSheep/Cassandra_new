#pragma once
#include "count_last_flip.h"
#include "position.h"
#include "line.h"
#include "move.h"
#include <cassert>
#include <cstdint>

								inline int	EvalGameOver(const uint64_t P, const unsigned int empties);
template <unsigned int empties>	inline int	EvalGameOver(const uint64_t P);

namespace Endgame_NegaMax
{
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter);
}

namespace Endgame_AlphaBeta
{
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter);
}

// PVS + ZWS + MPC + MoveOrdering + HashTable + IterativeDeepening + IterativeBroadening
namespace Endgame_PVS
{
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, const int selectivity, const unsigned int depth, CLine* pline = nullptr);
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter);
}


// ################################################################################################
// Inline section
// ################################################################################################
// ------------------------------------------------------------------------------------------------
inline int EvalGameOver(const uint64_t P, const unsigned int empties)
{
	assert(empties >= 0);
	assert(empties <= 64);

	int Diff = (PopCount(P) << 1) + empties - 64;
	     if (Diff > 0) return Diff + empties;
	else if (Diff < 0) return Diff - empties;
	else               return Diff;
}

template <unsigned int empties>
inline int EvalGameOver(const uint64_t P)
{
	if (empties % 2 == 0)
	{
		int Diff = (PopCount(P) << 1) + empties - 64;
		     if (Diff > 0) return Diff + empties;
		else if (Diff < 0) return Diff - empties;
		else               return Diff;
	}
	else
	{
		int Diff = (PopCount(P) << 1) + empties - 64;
		if (Diff > 0) return Diff + empties;
		else          return Diff - empties;
	}
}

template <> inline int EvalGameOver<0>(const uint64_t P) { return (PopCount(P) << 1) - 64; }
// ------------------------------------------------------------------------------------------------
// ################################################################################################
