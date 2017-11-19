#include "game.h"

namespace Endgame_PVS
{	
	// Functions in header file (redefinition)
	// -----------------------
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, CLine* pline);
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter);
	int Eval(const uint64_t P, const uint64_t O);
	
	int  PVS(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, CLine* pline);
	int  ZWS(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha);
	// -----------------------
	
	// Generic functions
	// -----------------------
	inline int Eval_0(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta);
	
	inline int ZWS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha);
	inline int ZWS_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha);
	inline int ZWS_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha);
	inline int ZWS_4(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha);
	       int ZWS_A(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha);
	       int ZWS_B(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha);
	
	inline int PVS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha,                 CLine* pline = nullptr);
	inline int PVS_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, CLine* pline = nullptr);
	       int PVS_A(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, CLine* pline = nullptr);
	// -----------------------
	
	
	// Specialized functions
	// -----------------------
	int Eval_0(const uint64_t P, uint64_t& NodeCounter);

	int ZWS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const unsigned int x);
	int ZWS_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const unsigned int x1, const unsigned int x2);
	int ZWS_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, unsigned int x1, unsigned int x2, unsigned int x3);
	int ZWS_4(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, unsigned int x1, unsigned int x2, unsigned int x3, unsigned int x4);

	int PVS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const unsigned int x, CLine* pline = nullptr);
	int PVS_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, const unsigned int x1, const unsigned int x2, CLine* pline = nullptr);
	// -----------------------
	
	
		
	// ################################################################################################
	//  Functions in header file
	// ################################################################################################
	// ------------------------------------------------------------------------------------------------
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, CLine* pline)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(-64 <= beta ); assert(beta  <= 64);
		assert(alpha <= beta);
		assert(0 <= EmptyCount(P, O)); assert(EmptyCount(P, O) <= 60);
		
		return PVS_A(P, O, NodeCounter, alpha, beta, pline);
	}
	
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter)
	{
		return Eval(P, O, NodeCounter, -64, 64);
	}
	
	int Eval(const uint64_t P, const uint64_t O)
	{
		uint64_t NodeCounter = 0;
		return Eval(P, O, NodeCounter);
	}
	
	int  PVS(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, CLine* pline)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(-64 <= beta ); assert(beta  <= 64);
		assert(alpha <= beta);
		assert(0 <= EmptyCount(P, O)); assert(EmptyCount(P, O) <= 60);
		
		return PVS_A(P, O, NodeCounter, alpha, beta, pline);
	}
	
	int  ZWS(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(0 <= EmptyCount(P, O)); assert(EmptyCount(P, O) <= 60);
		
		return ZWS_B(P, O, NodeCounter, alpha);
	}
	// ------------------------------------------------------------------------------------------------
	
	
	// ################################################################################################
	//  Generic functions
	// ################################################################################################
	// ------------------------------------------------------------------------------------------------
	inline int Eval_0(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta)
	{
		assert(EmptyCount(P, O) == 0);
		
		return Eval_0(P, NodeCounter);
	}
	
	inline int ZWS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha)
	{
		assert(EmptyCount(P, O) == 1);
		
		uint64_t Empties = ~(P | O);
		const auto x1 = BitScanLSB(Empties);
		return ZWS_1(P, O, NodeCounter, alpha, x1);
	}
	
	inline int ZWS_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha)
	{
		assert(EmptyCount(P, O) == 2);
		
		uint64_t Empties = ~(P | O);
		const auto x1 = BitScanLSB(Empties); RemoveLSB(Empties);
		const auto x2 = BitScanLSB(Empties);
		return ZWS_2(P, O, NodeCounter, alpha, x1, x2);
	}
	
	inline int ZWS_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha)
	{
		assert(EmptyCount(P, O) == 3);
		
		uint64_t Empties = ~(P | O);
		const auto x1 = BitScanLSB(Empties); RemoveLSB(Empties);
		const auto x2 = BitScanLSB(Empties); RemoveLSB(Empties);
		const auto x3 = BitScanLSB(Empties);
		return ZWS_3(P, O, NodeCounter, alpha, x1, x2, x3);
	}
	
	inline int ZWS_4(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha)
	{
		assert(EmptyCount(P, O) == 4);
		
		uint64_t Empties = ~(P | O);
		const auto x1 = BitScanLSB(Empties); RemoveLSB(Empties);
		const auto x2 = BitScanLSB(Empties); RemoveLSB(Empties);
		const auto x3 = BitScanLSB(Empties); RemoveLSB(Empties);
		const auto x4 = BitScanLSB(Empties);
		return ZWS_4(P, O, NodeCounter, alpha, x1, x2, x3, x4);
	}
	
	int ZWS_A(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);
		
		const auto empties = EmptyCount(P, O);
		switch (empties) {
			case 0: return Eval_0(P, NodeCounter);
			case 1: return ZWS_1(P, O, NodeCounter, alpha);
			case 2: return ZWS_2(P, O, NodeCounter, alpha);
			case 3: return ZWS_3(P, O, NodeCounter, alpha);
			case 4: return ZWS_4(P, O, NodeCounter, alpha);
		}

		int score;
		int bestscore = -64;
		uint64_t BitBoardPossible = PossibleMoves(P, O);
		NodeCounter++;

		if (!BitBoardPossible) {
			if (HasMoves(O, P))
				return -ZWS_A(O, P, NodeCounter, -alpha - 1);
			else
				return EvalGameOver(P, empties);
		}

		if (StabilityCutoff_ZWS(P, O, alpha, score)) return score;
		
		const auto BBParity = quadrant[Parity(P, O)];
		uint64_t BBTmp;

		BBTmp = BitBoardPossible &  BBParity;
		while (BBTmp)
		{
			const auto Move = BitScanLSB(BBTmp);
			const auto flipped = flip(P, O, Move);
			score = -ZWS_A(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter, -alpha-1);
			if (score > alpha) return score;
			if (score > bestscore) bestscore = score;
			RemoveLSB(BBTmp);
		}

		BBTmp = BitBoardPossible & ~BBParity;
		while (BBTmp)
		{
			const auto Move = BitScanLSB(BBTmp);
			const auto flipped = flip(P, O, Move);
			score = -ZWS_A(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter, -alpha-1);
			if (score > alpha) return score;
			if (score > bestscore) bestscore = score;
			RemoveLSB(BBTmp);
		}
		return bestscore;
	}
	
	int ZWS_B(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);
		
		const auto empties = EmptyCount(P, O);

		if (empties <= 8) return ZWS_A(P, O, NodeCounter, alpha);

		int score;
		int bestscore = -64;
		uint8_t BestMove = 64;
		auto BitBoardPossible = PossibleMoves(P, O);
		const auto LocalNodeCounter = NodeCounter;
		CHashTableValueType ttValue;
		NodeCounter++;

		if (!BitBoardPossible){
			if (HasMoves(O, P))
				return -ZWS_B(O, P, NodeCounter, -alpha - 1);
			else
				return EvalGameOver(P, empties);
		}

		if (StabilityCutoff_ZWS(P, O, alpha, score)) return score;
		if (LookUpTT(P, O, ttValue) && UseTTValue(ttValue, alpha, alpha+1, empties, NO_SELECTIVITY, score)) return score; 

		CMoveList mvList(P, O, NodeCounter, BitBoardPossible, empties, alpha, ttValue, false);
		for (const auto& mv : mvList)
		{
			score = -ZWS_B(mv.P, mv.O, NodeCounter, -alpha-1);
			if (score > bestscore)
			{
				bestscore = score;
				BestMove = mv.move;
				if (score > alpha) break;
			}
		}

		UpdateTT(P, O, NodeCounter-LocalNodeCounter, alpha, alpha+1, bestscore, empties, NO_SELECTIVITY, BestMove, mvList.BestMove(), mvList.NextBestMove());
		return bestscore;
	}
	
	inline int PVS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, CLine* pline) 
	{
		const auto x = BitScanLSB(~(P | O));
		return PVS_1(P, O, NodeCounter, alpha, x, pline);
	}
	
	inline int PVS_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, CLine* pline)
	{
		assert(EmptyCount(P, O) == 2);
		
		uint64_t Empties = ~(P | O);
		const auto x1 = BitScanLSB(Empties); RemoveLSB(Empties);
		const auto x2 = BitScanLSB(Empties);
		return PVS_2(P, O, NodeCounter, alpha, beta, x1, x2, pline);
	}
	
	int PVS_A(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, CLine* pline)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(-64 <= beta ); assert(beta  <= 64);
		assert(alpha <= beta);
		
		const auto empties = EmptyCount(P, O);
		switch (empties) {
			case 0: return Eval_0(P, NodeCounter);
			case 1: return PVS_1(P, O, NodeCounter, alpha, pline);
			case 2: return PVS_2(P, O, NodeCounter, alpha, beta, pline);
		}
		
		int lower = alpha;
		int score;
		int bestscore = -65;
		uint8_t BestMove = 64;
		auto BitBoardPossible = PossibleMoves(P, O);
		NodeCounter++;

		if (!BitBoardPossible){
			if (HasMoves(O, P))
				return -PVS_A(O, P, NodeCounter, -beta, -alpha, pline);
			else {
				if (pline) pline->NoMoves();
				return EvalGameOver(P, empties);
			}
		}

		CLine* line = (pline ? new CLine(pline->size()-1) : nullptr);
		
		uint64_t LocalNodeCounter = NodeCounter;
		CHashTableValueType ttValue;
		if (PVS::USE_PV_TTCUT && !pline)
			if (LookUpTTPV(P, O, ttValue) || LookUpTT(P, O, ttValue))
				if (UseTTValue(ttValue, alpha, beta, empties, NO_SELECTIVITY, score))
					return score;

		CMoveList mvList(P, O, NodeCounter, BitBoardPossible, empties, alpha, ttValue, true);
		for (const auto& mv : mvList)
		{
			if (bestscore == -65)
				score = -PVS_A(mv.P, mv.O, NodeCounter, -beta, -lower, line);
			else
			{
				score = -ZWS_A(mv.P, mv.O, NodeCounter, -lower-1);
				if (score > lower && score < beta)
					score = -PVS_A(mv.P, mv.O, NodeCounter, -beta, -lower, line);
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

		UpdateTTPV(P, O, NodeCounter - LocalNodeCounter, alpha, beta, bestscore, empties, NO_SELECTIVITY, BestMove, mvList.BestMove(), mvList.NextBestMove());
		UpdateTT(P, O, NodeCounter - LocalNodeCounter, alpha, beta, bestscore, empties, NO_SELECTIVITY, BestMove, mvList.BestMove(), mvList.NextBestMove());

		delete line;
		return bestscore;
	}
	// ------------------------------------------------------------------------------------------------
	
	
	// ################################################################################################
	//  Specialized functions
	// ################################################################################################
	// ------------------------------------------------------------------------------------------------
	int Eval_0(const uint64_t P, uint64_t& NodeCounter)
	{
		NodeCounter++;
		return EvalGameOver<0>(P);
	}
	
	int ZWS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const unsigned int x)
	{
		assert((P & O) == 0);
		assert(EmptyCount(P, O) == 1);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(x < 64);

		const int score = (PopCount(P) << 1) - 63; // == PopCount(P) - PopCount(O)

		if (const auto diff = count_last_flip(P, x))
		{
			NodeCounter += 2; // One for this, one for playing.
			return score + diff + 1;
		}
		else
		{
			if (score > 0)
			{
				if (score + 1 < alpha) return score + 1;

				if (const auto diff = count_last_flip(O, x))
				{
					NodeCounter += 3; // One for this, one for passing, one for playing.
					return score - diff - 1;
				}
				else
				{
					NodeCounter++; // One for this.
					return score + 1;
				}
			}
			else
			{
				if (score - 1 < alpha) return score - 1;

				if (const auto diff = count_last_flip(O, x))
				{
					NodeCounter += 3; // One for this, one for passing, one for playing.
					return score - diff - 1;
				}
				else
				{
					NodeCounter++; // One for this.
					return score - 1;
				}
			}
		}
	}
	
	int ZWS_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const unsigned int x1, const unsigned int x2)
	{
		assert((P & O) == 0);
		assert(EmptyCount(P, O) == 2);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(x1 < 64);
		assert(x2 < 64);

		int bestscore = -65;
		uint64_t flipped;
		NodeCounter++;

		//Play on x1
		if ((O & neighbour[x1]) && (flipped = flip(P, O, x1)))
		{
			bestscore = -ZWS_1(O ^ flipped, P ^ (1ULL << x1) ^ flipped, NodeCounter, -alpha - 1, x2);
			if (bestscore > alpha) return bestscore;
		}

		//Play on x2
		if ((O & neighbour[x2]) && (flipped = flip(P, O, x2)))
			return std::max(bestscore, -ZWS_1(O ^ flipped, P ^ (1ULL << x2) ^ flipped, NodeCounter, -alpha - 1, x1));

		if (bestscore != -65) return bestscore;
		bestscore = 65;
		NodeCounter++;

		//Play on x1
		if ((P & neighbour[x1]) && (flipped = flip(O, P, x1)))
		{
			bestscore = ZWS_1(P ^ flipped, O ^ (1ULL << x1) ^ flipped, NodeCounter, alpha, x2);
			if (bestscore <= alpha) return bestscore;
		}

		//Play on x2
		if ((P & neighbour[x2]) && (flipped = flip(O, P, x2)))
			return std::min(bestscore, ZWS_1(P ^ flipped, O ^ (1ULL << x2) ^ flipped, NodeCounter, alpha, x1));

		if (bestscore != 65) return bestscore;
		
		NodeCounter--;
		return EvalGameOver<2>(P);
	}
	
	int ZWS_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, unsigned int x1, unsigned int x2, unsigned int x3)
	{
		assert((P & O) == 0);
		assert(EmptyCount(P, O) == 3);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(x1 < 64);
		assert(x2 < 64);
		assert(x3 < 64);

		if (quadrant_id_4_bit[x1] == quadrant_id_4_bit[x2]) //Play x3 first
			std::swap(x1, x3);
		else if (quadrant_id_4_bit[x1] == quadrant_id_4_bit[x3]) //Play x2 first
			std::swap(x1, x2);
		
		int score;
		int bestscore = -65;
		uint64_t flipped;
		NodeCounter++;

		//Play on x1
		if ((O & neighbour[x1]) && (flipped = flip(P, O, x1)))
		{
			bestscore = -ZWS_2(O ^ flipped, P ^ (1ULL << x1) ^ flipped, NodeCounter, -alpha - 1, x2, x3);
			if (bestscore > alpha) return bestscore;
		}

		//Play on x2
		if ((O & neighbour[x2]) && (flipped = flip(P, O, x2)))
		{
			score = -ZWS_2(O ^ flipped, P ^ (1ULL << x2) ^ flipped, NodeCounter, -alpha - 1, x1, x3);
			if (score > alpha) return score;
			if (score > bestscore) bestscore = score;
		}

		//Play on x3
		if ((O & neighbour[x3]) && (flipped = flip(P, O, x3)))
			return std::max(bestscore, -ZWS_2(O ^ flipped, P ^ (1ULL << x3) ^ flipped, NodeCounter, -alpha - 1, x1, x2));

		if (bestscore != -65) return bestscore;
		bestscore = 65;
		NodeCounter++;

		//Play on x1
		if ((P & neighbour[x1]) && (flipped = flip(O, P, x1)))
		{
			bestscore = ZWS_2(P ^ flipped, O ^ (1ULL << x1) ^ flipped, NodeCounter, alpha, x2, x3);
			if (bestscore <= alpha) return bestscore;
		}

		//Play on x2
		if ((P & neighbour[x2]) && (flipped = flip(O, P, x2)))
		{
			score = ZWS_2(P ^ flipped, O ^ (1ULL << x2) ^ flipped, NodeCounter, alpha, x1, x3);
			if (score <= alpha) return score;
			if (score < bestscore) bestscore = score;
		}

		//Play on x3
		if ((P & neighbour[x3]) && (flipped = flip(O, P, x3)))
			return std::min(bestscore, ZWS_2(P ^ flipped, O ^ (1ULL << x3) ^ flipped, NodeCounter, alpha, x1, x2));

		if (bestscore != 65) return bestscore;
		
		NodeCounter--;
		return EvalGameOver<3>(P);
	}
	
	int ZWS_4(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, unsigned int x1, unsigned int x2, unsigned int x3, unsigned int x4)
	{
		assert((P & O) == 0);
		assert(EmptyCount(P, O) == 4);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(x1 < 64);
		assert(x2 < 64);
		assert(x3 < 64);
		assert(x4 < 64);
		
		int score;
		int bestscore = -65;
		uint64_t flipped;
		NodeCounter++;

		//if (StabilityCutoff_ZWS(P, O, alpha, score)) return score;

		//uint64_t parity = Parity(~(P, O));
		//if (!(parity & quadrant_id_4_bit[x1])) { // (..) (..) (x1 ..)
		//	if (parity & quadrant_id_4_bit[x2]) { // (x2) (..) (x1 ..)
		//		if (parity & quadrant_id_4_bit[x3]) // (x2) (x3) (x1 x4)
		//			std::swap(x1, x3); // Now playing x3 x2 x1 x4
		//		else // (x2) (x4) (x1 x3)
		//			std::swap(x1, x4); //Now playing x4 x2 x3 x1
		//	}
		//	else if (parity & quadrant_id_4_bit[x3]) // (x3) (x4) (x1 x2)
		//		{std::swap(x1, x3); std::swap(x2, x4);} // Now playing x3 x4 x1 x2
		//} 
		//else { // Either 1 or 3 in x1's quadrant
		//	if (!(parity & quadrant_id_4_bit[x2])) { // (x1) (..) (x2 ..)
		//		if (parity & quadrant_id_4_bit[x3]) // (x1) (x3) (x2 x4)
		//			std::swap(x2, x3); // Now playing x1 x3 x2 x4
		//		else // (x1) (x4) (x2 x3)
		//			std::swap(x2, x4); // Now playing x1 x4 x3 x2
		//	}
		//}

		//Play on x1
		if ((O & neighbour[x1]) && (flipped = flip(P, O, x1)))
		{
			bestscore = -ZWS_3(O ^ flipped, P ^ (1ULL << x1) ^ flipped, NodeCounter, -alpha - 1, x2, x3, x4);
			if (bestscore > alpha) return bestscore;
		}

		//Play on x2
		if ((O & neighbour[x2]) && (flipped = flip(P, O, x2)))
		{
			score = -ZWS_3(O ^ flipped, P ^ (1ULL << x2) ^ flipped, NodeCounter, -alpha - 1, x1, x3, x4);
			if (score > alpha) return score;
			if (score > bestscore) bestscore = score;
		}

		//Play on x3
		if ((O & neighbour[x3]) && (flipped = flip(P, O, x3)))
		{
			score = -ZWS_3(O ^ flipped, P ^ (1ULL << x3) ^ flipped, NodeCounter, -alpha - 1, x1, x2, x4);
			if (score > alpha) return score;
			if (score > bestscore) bestscore = score;
		}

		//Play on x4
		if ((O & neighbour[x4]) && (flipped = flip(P, O, x4)))
			return std::max(bestscore, -ZWS_3(O ^ flipped, P ^ (1ULL << x4) ^ flipped, NodeCounter, -alpha - 1, x1, x2, x3));

		if (bestscore != -65) return bestscore;
		bestscore = 65;
		NodeCounter++;

		//Play on x1
		if ((P & neighbour[x1]) && (flipped = flip(O, P, x1)))
		{
			bestscore = ZWS_3(P ^ flipped, O ^ (1ULL << x1) ^ flipped, NodeCounter, alpha, x2, x3, x4);
			if (bestscore <= alpha) return bestscore;
		}

		//Play on x2
		if ((P & neighbour[x2]) && (flipped = flip(O, P, x2)))
		{
			score = ZWS_3(P ^ flipped, O ^ (1ULL << x2) ^ flipped, NodeCounter, alpha, x1, x3, x4);
			if (score <= alpha) return score;
			if (score < bestscore) bestscore = score;
		}

		//Play on x3
		if ((P & neighbour[x3]) && (flipped = flip(O, P, x3)))
		{
			score = ZWS_3(P ^ flipped, O ^ (1ULL << x3) ^ flipped, NodeCounter, alpha, x1, x2, x4);
			if (score <= alpha) return score;
			if (score < bestscore) bestscore = score;
		}

		//Play on x4
		if ((P & neighbour[x4]) && (flipped = flip(O, P, x4)))
			return std::min(bestscore, ZWS_3(P ^ flipped, O ^ (1ULL << x4) ^ flipped, NodeCounter, alpha, x1, x2, x3));

		if (bestscore != 65) return bestscore;
		
		NodeCounter--;
		return EvalGameOver<4>(P);
	}
	
	int PVS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const unsigned int x, CLine* pline)
	{
		assert((P & O) == 0);
		assert(EmptyCount(P, O) == 1);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(x < 64);

		int score = (PopCount(P) << 1) - 63; // == PopCount(P) - PopCount(O)

		if (const auto diff = count_last_flip(P, x))
		{
			NodeCounter += 2; // One for this, one for playing.
			if (pline) pline->NewPV(x);
			return score + diff + 1;
		}
		else
		{
			// TODO: Decide what to do. Maybe you want to uncomment things.
			
			//if (!pline && (score < alpha))
			//{
			//	NodeCounter += 2; // One for this, one for playing.
			//	return alpha;
			//}
			if (const auto diff = count_last_flip(O, x))
			{
				NodeCounter += 2; // One for passing, one for playing.
				if (pline) pline->NewPV(x);
				return score - diff - 1;
			}
			else
			{
				NodeCounter++;
				if (pline) pline->NoMoves();
				return score > 0 ? score + 1 : score - 1;
			}

			//if (score > 0)
			//{
			//	if (!pline && (score + 1 < alpha)) return score + 1;

			//	if (diff = count_last_flip(O, x))
			//	{
			//		NodeCounter += 2; // One for passing, one for playing
			//		if (pline) pline->NewPV(x);
			//		return score - diff - 1;
			//	}
			//	else
			//	{
			//		if (pline) pline->NoMoves();
			//		return score + 1;
			//	}
			//}
			//else
			//{
			//	if (!pline && (score - 1 < alpha)) return score - 1;

			//	if (diff = count_last_flip(O, x))
			//	{
			//		NodeCounter += 2; // One for passing, one for playing
			//		if (pline) pline->NewPV(x);
			//		return score - diff - 1;
			//	}
			//	else
			//	{
			//		if (pline) pline->NoMoves();
			//		return score - 1;
			//	}
			//}
		}
	}
	
	int PVS_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, const unsigned int x1, const unsigned int x2, CLine* pline)
    {
        assert((P & O) == 0);
        assert(EmptyCount(P, O) == 2);
        assert(-64 <= alpha); assert(alpha <= 64);
        assert(-64 <= beta ); assert(beta  <= 64);
        assert(alpha <= beta);
        assert(x1 < 64);
        assert(x2 < 64);
                
        int lower = alpha;
        int score;
        int bestscore = -65;
        uint64_t flipped;
        NodeCounter++;

        CLine* line = (pline ? new CLine(pline->size()-1) : nullptr);
        
        //Play on x1
        if ((O & neighbour[x1]) && (flipped = flip(P, O, x1)))
        {
            if (bestscore == -65)
                score = -PVS_1(O ^ flipped, P ^ (1ULL << x1) ^ flipped, NodeCounter, -beta, x2, line);
            else
            {
                score = -ZWS_1(O ^ flipped, P ^ (1ULL << x1) ^ flipped, NodeCounter, -lower-1, x2);
                if (score > lower && score < beta)
                    score = -PVS_1(O ^ flipped, P ^ (1ULL << x1) ^ flipped, NodeCounter, -beta, x2, line);
            }
            if (score > bestscore)
            {
                bestscore = score;
                if (line) pline->NewPV(x1, line);
                if (score >= beta) {
                    delete line;
                    return bestscore;
                }
                if (score > lower) lower = score;
            }
        }
        
        //Play on x2
        if ((O & neighbour[x2]) && (flipped = flip(P, O, x2)))
        {
            if (bestscore == -65)
                score = -PVS_1(O ^ flipped, P ^ (1ULL << x2) ^ flipped, NodeCounter, -beta, x1, line);
            else
            {
                score = -ZWS_1(O ^ flipped, P ^ (1ULL << x2) ^ flipped, NodeCounter, -lower-1, x1);
                if (score > lower && score < beta)
                    score = -PVS_1(O ^ flipped, P ^ (1ULL << x2) ^ flipped, NodeCounter, -beta, x1, line);
            }
            if (score > bestscore)
            {
                bestscore = score;
                if (line) pline->NewPV(x2, line);
                if (score >= beta) {
                    delete line;
                    return bestscore;
                }
                if (score > lower) lower = score;
            }
        }
        
        if (bestscore != -65) {
            delete line;
            return bestscore;
        }
		
        bestscore = 65;
        lower = beta;
        NodeCounter++;
        
        //Play on x1
        if ((P & neighbour[x1]) && (flipped = flip(O, P, x1)))
        {
            if (bestscore == 65)
                score = PVS_1(P ^ flipped, O ^ (1ULL << x1) ^ flipped, NodeCounter, alpha, x2, line);
            else
            {
                score = ZWS_1(P ^ flipped, O ^ (1ULL << x1) ^ flipped, NodeCounter, lower+1, x2);
                if (score < lower && score > alpha)
                    score = PVS_1(P ^ flipped, O ^ (1ULL << x1) ^ flipped, NodeCounter, alpha, x2, line);
            }
            if (score < bestscore)
            {
                bestscore = score;
                if (line) pline->NewPV(x2, line);
                if (score <= alpha) {
                    delete line;
                    return bestscore;
                }
                if (score < lower) lower = score;
            }
        }
        
        //Play on x2
        if ((P & neighbour[x2]) && (flipped = flip(O, P, x2)))
        {
            if (bestscore == 65)
                score = PVS_1(P ^ flipped, O ^ (1ULL << x2) ^ flipped, NodeCounter, alpha, x1, line);
            else
            {
                score = ZWS_1(P ^ flipped, O ^ (1ULL << x2) ^ flipped, NodeCounter, lower+1, x1);
                if (score < lower && score > alpha)
                    score = PVS_1(P ^ flipped, O ^ (1ULL << x2) ^ flipped, NodeCounter, alpha, x1, line);
            }
            if (score < bestscore)
            {
                bestscore = score;
                if (line) pline->NewPV(x2, line);
                if (score <= alpha) {
                    delete line;
                    return bestscore;
                }
                if (score < lower) lower = score;
            }
        }

        delete line;
		if (bestscore != 65)
			return bestscore;
		else {
        	NodeCounter--;
        	return EvalGameOver<2>(P);
		}
    }
	// ------------------------------------------------------------------------------------------------
}