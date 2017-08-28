#include "game.h"

namespace IDAB // Iterative Deepening and Broadening
{	
	// Functions in header file (redefinition)
	// -----------------------
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, const int selectivity, const unsigned int depth, CLine* pline);
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter,                                  const int selectivity, const unsigned int depth);
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter);
	int Eval(const uint64_t P, const uint64_t O);
	// -----------------------
	
	// ################################################################################################
	//  Functions in header file
	// ################################################################################################
	// ------------------------------------------------------------------------------------------------
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, const int selectivity, const unsigned int depth, CLine* pline)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(-64 <= beta ); assert(beta  <= 64);
		assert(alpha <= beta);
		assert(depth <= EmptyCount(P, O));
		assert(0 <= EmptyCount(P, O)); assert(EmptyCount(P, O) <= 60);
		
		const int FULL_SELECTIVITY = 6;
		const auto empties = EmptyCount(P, O);
		int score = -99;
		
		if (empties <= 14)
		{
			score = PVS::Eval(P, O, NodeCounter, alpha, beta, selectivity, depth, pline);
		}
		else
		{
			for (unsigned int d = 0; d <= depth; d++)
				if (empties - d >= 10)
					score = PVS::Eval(P, O, NodeCounter, alpha, beta, FULL_SELECTIVITY, d, pline);
			for (int s = FULL_SELECTIVITY; s >= selectivity; s-=3)
				score = PVS::Eval(P, O, NodeCounter, alpha, beta, s, depth, pline);
		}
		assert(score != -99);
		return score;
	}
	
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int selectivity, const unsigned int depth)
	{
		return Eval(P, O, NodeCounter, -64, 64, selectivity, depth);
	}
	
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter)
	{
		return Eval(P, O, NodeCounter, NO_SELECTIVITY, EmptyCount(P, O));
	}
	
	int Eval(const uint64_t P, const uint64_t O)
	{
		uint64_t NodeCounter = 0;
		return Eval(P, O, NodeCounter);
	}
	// ------------------------------------------------------------------------------------------------
	// ################################################################################################
}
