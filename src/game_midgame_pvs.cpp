#include "game.h"

namespace Midgame_PVS
{
	// Functions in header file (redefinition)
	// -----------------------
	int Eval  (const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, const unsigned int depth, CLine* pline);
	int Eval  (const uint64_t P, const uint64_t O, uint64_t& NodeCounter,                                  const unsigned int depth);
	int Eval_0(const uint64_t P, const uint64_t O);
	
	int  PVS(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, const unsigned int depth, CLine* pline);
	int  ZWS(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha                , const unsigned int depth);
	// -----------------------
	
	// Forward declaration
	// -----------------------
	int Eval_0(const uint64_t P, const uint64_t O, uint64_t& NodeCounter);
	int  ZWS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha);
	int  ZWS_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha);
	int  ZWS_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha);
	int  PVS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter,       int alpha, const int beta, CLine* pline);
	int  PVS_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, CLine* pline);
	// -----------------------
	
	// ################################################################################################
	//  Functions in header file
	// ################################################################################################
	// ------------------------------------------------------------------------------------------------
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, const unsigned int depth, CLine* pline)
	{
		return PVS(P, O, NodeCounter, alpha, beta, depth, pline);
	}
	
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const unsigned int depth)
	{
		return Eval(P, O, NodeCounter, -64, 64, depth);
	}
	
	int Eval_0(const uint64_t P, const uint64_t O)
	{
		uint64_t NodeCounter = 0;
		return Eval_0(P, O, NodeCounter);
	}
	
	int  PVS(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, const unsigned int depth, CLine* pline)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(-64 <= beta ); assert(beta  <= 64);
		assert(alpha <= beta);
		assert(depth <  EmptyCount(P, O));
		assert(depth <= Depth_PVS);
		assert(0 <= EmptyCount(P, O)); assert(EmptyCount(P, O) <= 60);
		
		switch (depth) {
			case 0: return Eval_0(P, O, NodeCounter);
			case 1: return PVS_1(P, O, NodeCounter, alpha, beta, pline);
			case 2: return PVS_2(P, O, NodeCounter, alpha, beta, pline);
			default: throw std::invalid_argument("depth out of range");
		}
	}
	
	int  ZWS(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const unsigned int depth)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(depth <  EmptyCount(P, O));
		assert(depth <= Depth_ZWS);
		assert(0 <= EmptyCount(P, O)); assert(EmptyCount(P, O) <= 60);
		
		switch (depth) {
			case 0: return Eval_0(P, O, NodeCounter);
			case 1: return ZWS_1(P, O, NodeCounter, alpha);
			case 2: return ZWS_2(P, O, NodeCounter, alpha);
			case 3: return ZWS_3(P, O, NodeCounter, alpha);
			default: throw std::invalid_argument("depth out of range");
		}
	}
	// ------------------------------------------------------------------------------------------------
	
	
	int Eval_0(const uint64_t P, const uint64_t O, uint64_t& NodeCounter)
	{
		assert((P & O) == 0);

		NodeCounter++;
		return EvaluatePattern(P, O);
	}

	int ZWS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);

		int bestscore = -64;
		uint64_t BitBoardPossible = PossibleMoves(P, O);
		NodeCounter++;

		if (!BitBoardPossible) {
			if (HasMoves(O, P))
				return -ZWS_1(O, P, NodeCounter, -alpha-1);
			else
				return EvalGameOver(P, EmptyCount(P, O));
		}

		while (BitBoardPossible)
		{
			const auto Move = BitScanLSB(BitBoardPossible);
			RemoveLSB(BitBoardPossible);
			const auto flipped = flip(P, O, Move);
			const auto score = -Eval_0(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter);
			if (score > alpha) return score;
			if (score > bestscore) bestscore = score;
		}

		return bestscore;
	}
	
	int ZWS_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);

		int bestscore = -64;
		uint64_t BitBoardPossible = PossibleMoves(P, O);
		NodeCounter++;

		if (!BitBoardPossible) {
			if (HasMoves(O, P))
				return -ZWS_2(O, P, NodeCounter, -alpha-1);
			else
				return EvalGameOver(P, EmptyCount(P, O));
		}

		while (BitBoardPossible)
		{
			const auto Move = BitScanLSB(BitBoardPossible);
			RemoveLSB(BitBoardPossible);
			const auto flipped = flip(P, O, Move);
			const auto score = -ZWS_1(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter, -alpha - 1);
			if (score > alpha) return score;
			if (score > bestscore) bestscore = score;
		}
		return bestscore;
	}
	
	int ZWS_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);

		int score;
		int bestscore = -64;
		uint8_t BestMove = 64;
		uint64_t BitBoardPossible = PossibleMoves(P, O);
		uint64_t LocalNodeCounter = NodeCounter;
		CHashTableValueType ttValue;
		NodeCounter++;

		if (!BitBoardPossible) {
			if (HasMoves(O, P))
				return -ZWS_3(O, P, NodeCounter, -alpha-1);
			else
				return EvalGameOver(P, EmptyCount(P, O));
		}
		
		if (StabilityCutoff_ZWS(P, O, alpha, score)) return score;
		if (LookUpTT(P, O, ttValue) && UseTTValue(ttValue, alpha, alpha+1, 3, NO_SELECTIVITY, score)) return score;

		CMoveList mvList(P, O, NodeCounter, BitBoardPossible, 3, alpha, ttValue, false);
		for (const auto& mv : mvList)
		{
			score = -ZWS_2(mv.P, mv.O, NodeCounter, -alpha-1);
			if (score > bestscore)
			{
				bestscore = score;
				BestMove = mv.move;
				if (score > alpha) break;
			}
		}
	
		UpdateTT(P, O, NodeCounter-LocalNodeCounter, alpha, alpha+1, bestscore, 3, NO_SELECTIVITY, BestMove, mvList.BestMove(), mvList.NextBestMove());
		return bestscore;
	}

	int PVS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, const int beta, CLine* pline)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(-64 <= beta ); assert(beta  <= 64);
		assert(alpha <= beta);

		int bestscore = -64;
		uint64_t BitBoardPossible = PossibleMoves(P, O);
		NodeCounter++;

		if (!BitBoardPossible) {
			if (HasMoves(O, P))
				return -PVS_1(O, P, NodeCounter, -beta, -alpha, pline);
			else {
				if (pline) pline->NoMoves();
				return EvalGameOver(P, EmptyCount(P, O));
			}
		}

		while (BitBoardPossible)
		{
			const auto Move = BitScanLSB(BitBoardPossible);
			RemoveLSB(BitBoardPossible);
			const auto flipped = flip(P, O, Move);
			const auto score = -Eval_0(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter);
			if (score > bestscore)
			{
				bestscore = score;
				if (pline) pline->NewPV(Move);
				if (score >= beta) break;
				if (score > alpha) alpha = score;
			}
		}

		return bestscore;
	}
	
	int PVS_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, CLine* pline)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(-64 <= beta ); assert(beta  <= 64);
		assert(alpha <= beta);

		int lower = alpha;
		int score;
		int bestscore = -64;
		uint8_t BestMove = 64;
		uint64_t BitBoardPossible = PossibleMoves(P, O);
		NodeCounter++;

		if (!BitBoardPossible) {
			if (HasMoves(O, P))
				return -PVS_2(O, P, NodeCounter, -beta, -alpha, pline);
			else {
				if (pline) pline->NoMoves();
				return EvalGameOver(P, EmptyCount(P, O));
			}
		}
		
		uint64_t LocalNodeCounter = NodeCounter;
		CLine * line = nullptr;
		if (pline && pline->size()) line = new CLine(pline->size()-1);
		while (BitBoardPossible)
		{
			const auto Move = BitScanLSB(BitBoardPossible);
			RemoveLSB(BitBoardPossible);
			const auto flipped = flip(P, O, Move);
			if (bestscore == -65)
				score = -PVS_1(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter, -beta, -lower, line);
			else
			{
				score = -ZWS_1(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter, -lower-1);
				if (score > lower && score < beta)
					score = -PVS_1(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter, -beta, -lower, line);
			}
			if (score > bestscore)
			{
				bestscore = score;
				BestMove = Move;
				if (line) pline->NewPV(Move, line);
				if (score >= beta) break;
				if (score > lower) lower = score;
			}
		}

		UpdateTT(P, O, NodeCounter-LocalNodeCounter, alpha, beta, bestscore, 2, NO_SELECTIVITY, BestMove, 64, 64);
		delete line;
		return bestscore;
	}
}
