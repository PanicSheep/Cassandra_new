#include "game.h"

namespace PVS
{
	const bool USE_IID = false;
	
	// Functions in header file (redefinition)
	// -----------------------
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, const int selectivity, const unsigned int depth, CLine* pline);
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter,                                  const int selectivity, const unsigned int depth);
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter);
	int Eval(const uint64_t P, const uint64_t O);
	// -----------------------
	
	// Generic functions
	// -----------------------
	int ZWS(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha,                 const int selectivity, const unsigned int depth);
	int PVS(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, const int selectivity, const unsigned int depth, CLine* pline = nullptr);
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
		assert((depth <= EmptyCount(P, O)) || (depth == 99));
		assert(0 <= EmptyCount(P, O)); assert(EmptyCount(P, O) <= 60);
		
		if (depth == 99)
			return PVS(P, O, NodeCounter, alpha, beta, selectivity, EmptyCount(P, O), pline);
		else
			return PVS(P, O, NodeCounter, alpha, beta, selectivity, depth, pline);
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
	
	
	// ################################################################################################
	//  Generic functions
	// ################################################################################################
	// ------------------------------------------------------------------------------------------------
	int ZWS(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int selectivity, const unsigned int depth)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(0 <= depth); assert(depth <= 60);
		
		const auto empties = EmptyCount(P, O);
		assert(depth <= empties);

		if (depth < empties) { // Midgame
			if (depth <= Midgame_PVS::Depth_ZWS)
				return Midgame_PVS::ZWS(P, O, NodeCounter, alpha, depth);
		}
		else { // Endgame
			if (empties <= Endgame_PVS::Empties_ZWS)
				return Endgame_PVS::ZWS(P, O, NodeCounter, alpha);
		}

		int score;
		int bestscore = -64;
		uint8_t BestMove = 64;
		auto BitBoardPossible = PossibleMoves(P, O);
		const auto LocalNodeCounter = NodeCounter;
		CHashTableValueType ttValue;
		NodeCounter++;

		if (!BitBoardPossible){
			if (HasMoves(O, P))
				return -ZWS(O, P, NodeCounter, -alpha - 1, selectivity, depth);
			else
				return EvalGameOver(P, empties);
		}

		if (StabilityCutoff_ZWS(P, O, alpha, score)) return score;
		if (LookUpTT(P, O, ttValue) && UseTTValue(ttValue, alpha, alpha+1, depth, selectivity, score)) return score;
		// if (MPC(P, O, NodeCounter, alpha, selectivity, depth, empties, score)) return score; // TODO !!!

		CMoveList mvList(P, O, NodeCounter, BitBoardPossible, depth, alpha, ttValue, false);
		for (const auto& mv : mvList) // ETC
		{
			if (StabilityCutoff_ZWS(mv.P, mv.O, -alpha - 1, score)) {
				UpdateTT(P, O, 0, alpha, alpha + 1, -score, depth, NO_SELECTIVITY, mv.move, mvList.BestMove(), mvList.NextBestMove());
				return -score;
			}
			if (LookUpTT(mv.P, mv.O, ttValue) && UseTTValue(ttValue, -alpha - 1, -alpha, depth - 1, selectivity, score) && (-score > alpha)) {
				UpdateTT(P, O, 0, alpha, alpha+1, -score, depth, selectivity, mv.move, mvList.BestMove(), mvList.NextBestMove());
				return -score;
			}
		}
		for (const auto& mv : mvList)
		{
			score = -ZWS(mv.P, mv.O, NodeCounter, -alpha-1, selectivity, depth-1);
			if (score > bestscore)
			{
				bestscore = score;
				BestMove = mv.move;
				if (score > alpha) break;
			}
		}

		// TODO: Think about this optimization.
		//if (empties-1 <= B)
		//	UpdateTT(P, O, NodeCounter-LocalNodeCounter, alpha, alpha+1, bestscore, depth, NO_SELECTIVITY, BestMove, mvList.BestMove(), mvList.NextBestMove());
		//else
			UpdateTT(P, O, NodeCounter-LocalNodeCounter, alpha, alpha+1, bestscore, depth, selectivity, BestMove, mvList.BestMove(), mvList.NextBestMove());
		return bestscore;
	}

	int PVS(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, const int selectivity, const unsigned int depth, CLine* pline)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(-64 <= beta ); assert(beta  <= 64);
		assert(alpha <= beta);
		assert(0 <= depth); assert(depth <= 60);
		
		const auto empties = EmptyCount(P, O);
		assert(depth <= empties);

		if (depth < empties) { // Midgame
			if (depth <= Midgame_PVS::Depth_PVS)
				return Midgame_PVS::PVS(P, O, NodeCounter, alpha, beta, depth);
		}
		else { // Endgame
			if (empties <= Endgame_PVS::Empties_PVS)
				return Endgame_PVS::PVS(P, O, NodeCounter, alpha, beta);
		}

		int lower = alpha;
		int score;
		int bestscore = -65;
		uint8_t BestMove = 64;
		auto BitBoardPossible = PossibleMoves(P, O);
		const auto LocalNodeCounter = NodeCounter;
		CHashTableValueType ttValue;
		NodeCounter++;

		if (!BitBoardPossible){
			if (HasMoves(O, P))
				return -PVS(O, P, NodeCounter, -beta, -alpha, selectivity, depth, pline);
			else {
				if (pline) pline->NoMoves();
				return EvalGameOver(P, empties);
			}
		}

		if (pline && StabilityCutoff_PVS(P, O, alpha, score)) return score;
		if (USE_PV_TTCUT && !pline)
			if (LookUpTTPV(P, O, ttValue) || LookUpTT(P, O, ttValue))
				if (UseTTValue(ttValue, alpha, beta, depth, selectivity, score))
					return score;
		// if (USE_IID && ttValue.PV == 64) // IID TODO !!!
		// {
		// 	int reduced_depth = (depth == empties) ? depth - A : depth - 2;
		// 	if (reduced_depth >= 3)
		// 	{
		// 		PVS(P, O, NodeCounter, -64, 64, 6, reduced_depth, empties, nullptr);
		// 		if (LookUpTTPV(P, O, ttValue))
		// 			if (USE_PV_TTCUT && pline->empty() && UseTTValue(ttValue, alpha, beta, depth, selectivity, score))
		// 				return score;
		// 	}
		// }

		CLine* line = (pline ? new CLine(pline->size()-1) : nullptr);
		CMoveList mvList(P, O, NodeCounter, BitBoardPossible, depth, alpha, ttValue, true);
		for (const auto& mv : mvList)
		{
			if (bestscore == -65)
				score = -PVS(mv.P, mv.O, NodeCounter, -beta, -lower, selectivity, depth-1, line);
			else
			{
				score = -ZWS(mv.P, mv.O, NodeCounter, -lower-1, selectivity, depth-1);
				if (score > lower && score < beta)
					score = -PVS(mv.P, mv.O, NodeCounter, -beta, -lower, selectivity, depth-1, line); // OPTIMIZATION: -lower -> -score
			}
			if (score > bestscore)
			{
				bestscore = score;
				BestMove = mv.move;
				if (line) pline->NewPV(mv.move, line);
				if (score >= beta) break;
				if (score > lower) lower = score;
			}
		}

		// TODO: Think about this optimization.
		//if (empties-1 <= B)
		//{
		//	UpdateTTPV(P, O, NodeCounter - LocalNodeCounter, alpha, beta, bestscore, depth, NO_SELECTIVITY, BestMove, mvList.BestMove(), mvList.NextBestMove());
		//	UpdateTT(P, O, NodeCounter - LocalNodeCounter, alpha, beta, bestscore, depth, NO_SELECTIVITY, BestMove, mvList.BestMove(), mvList.NextBestMove());
		//}
		//else
		//{
			UpdateTTPV(P, O, NodeCounter - LocalNodeCounter, alpha, beta, bestscore, depth, selectivity, BestMove, mvList.BestMove(), mvList.NextBestMove());
			UpdateTT(P, O, NodeCounter - LocalNodeCounter, alpha, beta, bestscore, depth, selectivity, BestMove, mvList.BestMove(), mvList.NextBestMove());
		//}
		delete line;
		return bestscore;
	}
	// ------------------------------------------------------------------------------------------------
	// ################################################################################################
}