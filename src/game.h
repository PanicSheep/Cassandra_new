#pragma once
#include "count_last_flip.h"
#include "pattern.h"
#include "position.h"
#include "line.h"
#include "move.h"
#include "hashtable.h"
#include <cassert>
#include <cstdint>

inline int	EvalGameOver(const uint64_t P, const unsigned int empties);
template <unsigned int empties>
inline int	EvalGameOver(const uint64_t P);

namespace Endgame_NegaMax
{
	int Eval(const uint64_t P, const uint64_t O);
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter);
}

namespace Endgame_AlphaBeta
{
	int Eval(const uint64_t P, const uint64_t O);
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter);
}

// PVS + ZWS + MoveOrdering + HashTable
namespace Endgame_PVS
{
	const int Empties_PVS = 8;  ///< The number of empties at which Eval is specialized for PVS endgame. (This parameter can be tuned.)
	const int Empties_ZWS = 12; ///< The number of empties at which Eval is specialized for ZWS endgame. (This parameter can be tuned.)
	
	/// Evaluates a position with PVS specialized for empties <= Empties_PVS. Will work for bigger empties but may performe poor.
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, CLine* pline = nullptr);
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter);
	int Eval(const uint64_t P, const uint64_t O);
	
	/// For exlicit calling of PVS. Specialized for empties <= Emtpies_PVS. Will work for bigger empties but may performe poor.
	int  PVS(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, CLine* pline = nullptr);
	
	/// For exlicit calling of ZWS. Specialized for empties <= Empties_ZWS. Will work for bigger empties but may performe poor.
	int  ZWS(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha);
}

namespace Midgame_PVS
{
	const int Depth_PVS = 2; ///< The depth at which Eval will work for PVS midgame.
	const int Depth_ZWS = 3; ///< The depth at which Eval will work for ZWS midgame.
	
	/// Evaluates a position with PVS for depth <= Depth_PVS. Will not work for bigger depth!
	int Eval  (const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, const unsigned int depth, CLine* pline = nullptr);
	int Eval  (const uint64_t P, const uint64_t O, uint64_t& NodeCounter,                                  const unsigned int depth);
	int Eval_0(const uint64_t P, const uint64_t O);
	
	/// For exlicit calling of PVS for depth <= Depth_PVS. Will not work for bigger depth!
	int  PVS(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, const unsigned int depth, CLine* pline = nullptr);
	
	/// For exlicit calling of ZWS for depth <= Depth_ZWS. Will not work for bigger depth!
	int  ZWS(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha                , const unsigned int depth);
}

namespace PVS
{
	const bool USE_PV_TTCUT = false;
	
	/// Evaluates a position with PVS.
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, const int selectivity, const unsigned int depth, CLine* pline = nullptr);
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter,                                  const int selectivity, const unsigned int depth);
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter);
	int Eval(const uint64_t P, const uint64_t O);
}

namespace IDAB // Iterative Deepening and Broadening
{
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, const int selectivity, const unsigned int depth, CLine* pline = nullptr);
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter,                                  const int selectivity, const unsigned int depth);
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter);
	int Eval(const uint64_t P, const uint64_t O);
}

inline int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int selectivity, const unsigned int depth);
inline int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter);
inline int Eval(const uint64_t P, const uint64_t O);
	
// Helper functions
// -----------------------
inline bool UseTTValue(const CHashTableValueType& ttValue, const int alpha, const int beta, const unsigned int depth, const int selectivity, int & score);
inline void UpdateTT(const uint64_t P, const uint64_t O, const uint64_t NodeCounter, const int alpha, const int beta, const int score, const unsigned int depth, const int selectivity, const uint8_t BestMove, const uint8_t PV, const uint8_t AV);
inline void UpdateTTPV(const uint64_t P, const uint64_t O, const uint64_t NodeCounter, const int alpha, const int beta, const int score, const unsigned int depth, const int selectivity, const uint8_t BestMove, const uint8_t PV, const uint8_t AV);
inline bool LookUpTT(const uint64_t P, const uint64_t O, CHashTableValueType& ttValue);
inline bool LookUpTTPV(const uint64_t P, const uint64_t O, CHashTableValueType& ttValue);
inline bool StabilityCutoff_ZWS(const uint64_t P, const uint64_t O, const int alpha, int& score);
inline bool StabilityCutoff_PVS(const uint64_t P, const uint64_t O, const int alpha, int& score);
// -----------------------

int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int selectivity, const unsigned int depth)
{
	return IDAB::Eval(P, O, NodeCounter, selectivity, depth);
}
int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter)
{
	return IDAB::Eval(P, O, NodeCounter);
}
int Eval(const uint64_t P, const uint64_t O)
{
	return IDAB::Eval(P, O);
}

// ################################################################################################
//  Helper functions
// ################################################################################################
// ------------------------------------------------------------------------------------------------
// Use Transposition Table Value
inline bool UseTTValue(const CHashTableValueType& ttValue, const int alpha, const int beta, const unsigned int depth, const int selectivity, int & score)
{
	if ((ttValue.depth >= static_cast<int>(depth)) && (ttValue.selectivity <= selectivity))
	{
		if (ttValue.alpha >= beta)         { score = ttValue.alpha; return true; }
		if (ttValue.beta <= alpha)         { score = ttValue.beta;  return true; }
		if (ttValue.alpha == ttValue.beta) { score = ttValue.alpha; return true; }
	}
	return false;
}

// Update Transposition Table
inline void UpdateTT(const uint64_t P, const uint64_t O, const uint64_t NodeCounter, const int alpha, const int beta, const int score, const unsigned int depth, const int selectivity, const uint8_t BestMove, const uint8_t PV, const uint8_t AV)
{ 
	if (score >= beta)
		gTT.Update(P, O, CHashTableValueType(NodeCounter, depth, selectivity, score, +64, BestMove, BestMove == PV ? AV : PV));
	else if (score <= alpha)
		gTT.Update(P, O, CHashTableValueType(NodeCounter, depth, selectivity, -64, score, BestMove, BestMove == PV ? AV : PV));
	else
		gTT.Update(P, O, CHashTableValueType(NodeCounter, depth, selectivity, score, score, BestMove, BestMove == PV ? AV : PV));
}

// Update Transposition Table
inline void UpdateTTPV(const uint64_t P, const uint64_t O, const uint64_t NodeCounter, const int alpha, const int beta, const int score, const unsigned int depth, const int selectivity, const uint8_t BestMove, const uint8_t PV, const uint8_t AV)
{ 
	if (score >= beta)
		gTTPV.Update(P, O, CHashTableValueType(NodeCounter, depth, selectivity, score, +64, BestMove, BestMove == PV ? AV : PV));
	else if (score <= alpha)
		gTTPV.Update(P, O, CHashTableValueType(NodeCounter, depth, selectivity, -64, score, BestMove, BestMove == PV ? AV : PV));
	else
		gTTPV.Update(P, O, CHashTableValueType(NodeCounter, depth, selectivity, score, score, BestMove, BestMove == PV ? AV : PV));
}

// Look Up Transposition Table
inline bool LookUpTT(const uint64_t P, const uint64_t O, CHashTableValueType& ttValue)
{ 
	return gTT.LookUp(P, O, ttValue);
}

// Look Up Transposition Table
inline bool LookUpTTPV(const uint64_t P, const uint64_t O, CHashTableValueType& ttValue)
{ 
	return gTTPV.LookUp(P, O, ttValue);
}

inline bool StabilityCutoff_ZWS(const uint64_t P, const uint64_t O, const int alpha, int& score)
{
	static const int stability_cutoff_limits[64] = {
		 99, 99, 99, 99,  6,  8, 10, 12,
		 14, 16, 20, 22, 24, 26, 28, 30,
		 32, 34, 36, 38, 40, 42, 44, 46,
		 48, 48, 50, 50, 52, 52, 54, 54,
		 56, 56, 58, 58, 60, 60, 62, 62,
		 64, 64, 64, 64, 64, 64, 64, 64,
		 99, 99, 99, 99, 99, 99, 99, 99,
	};
	if (alpha >= stability_cutoff_limits[EmptyCount(P, O)]) //Worth checking stability
	{
		int value = static_cast<int>(64 - 2 * PopCount(StableStonesPlayer(O, P)));
		if (value <= alpha)
		{
			score = value;
			return true;
		}
	}
	return false;
}

inline bool StabilityCutoff_PVS(const uint64_t P, const uint64_t O, const int alpha, int& score)
{
	static const int stability_cutoff_limits[64] = {
		 99, 99, 99, 99, -2,  0,  2,  4,
		  6,  8, 12, 14, 16, 18, 20, 22,
		 24, 26, 28, 30, 32, 34, 36, 38,
		 40, 40, 42, 42, 44, 44, 46, 46,
		 48, 48, 50, 50, 52, 52, 54, 54,
		 56, 56, 58, 58, 60, 60, 62, 62,
		 99, 99, 99, 99, 99, 99, 99, 99,
	};
	if (alpha >= stability_cutoff_limits[EmptyCount(P, O)]) //Worth checking stability
	{
		int value = static_cast<int>(64 - 2 * PopCount(StableStonesPlayer(O, P)));
		if (value <= alpha)
		{
			score = value;
			return true;
		}
	}
	return false;
}
// ------------------------------------------------------------------------------------------------
// ################################################################################################
	

// ################################################################################################
// Inline section
// ################################################################################################
// ------------------------------------------------------------------------------------------------
inline int EvalGameOver(const uint64_t P, const unsigned int empties)
{
	assert(empties >= 0);
	assert(empties <= 64);

	const int Diff = static_cast<int>(2 * PopCount(P) + empties - 64);
	     if (Diff > 0) return Diff + empties;
	else if (Diff < 0) return Diff - empties;
	else               return Diff;
}

template <unsigned int empties>
inline int EvalGameOver(const uint64_t P)
{
	const int Diff = static_cast<int>(2 * PopCount(P) + empties - 64);
	if (empties % 2 == 0)
	{
		     if (Diff > 0) return Diff + empties;
		else if (Diff < 0) return Diff - empties;
		else               return Diff;
	}
	else
	{
		if (Diff > 0) return Diff + empties;
		else          return Diff - empties;
	}
}

template <> inline int EvalGameOver<0>(const uint64_t P) { return static_cast<int>(2 * PopCount(P) - 64); }
// ------------------------------------------------------------------------------------------------
// ################################################################################################
