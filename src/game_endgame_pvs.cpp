#include "game.h"
#include "hashtable.h"

namespace Endgame_PVS
{
	const bool USE_IID = true;
	const bool USE_PV_TTCUT = true;

	const int A = 8;
	const int B = 12;
	
	// Function in header file
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, const int selectivity, const unsigned int depth, CLine* pline);
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter);
	
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
	       int PVS_A(const uint64_t P, const uint64_t O, uint64_t& NodeCounter,       int alpha, const int beta, CLine* pline = nullptr);
	
	       int ZWS(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha,                 const int selectivity, const unsigned int depth);
	       int PVS(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, const int selectivity, const unsigned int depth, CLine* pline = nullptr);
	// -----------------------
	
	
	// Specialized functions
	// -----------------------
	int Eval_0(const uint64_t P, uint64_t& NodeCounter);

	int ZWS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const unsigned int x);
	int ZWS_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const unsigned int x1, const unsigned int x2);
	int ZWS_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, unsigned int x1, unsigned int x2, unsigned int x3);
	int ZWS_4(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, unsigned int x1, unsigned int x2, unsigned int x3, unsigned int x4);

	int PVS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const unsigned int x, CLine* pline = nullptr);
	// -----------------------
	
	
		
	// ################################################################################################
	//  Function in header file
	// ################################################################################################
	// ------------------------------------------------------------------------------------------------
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, const int selectivity, const unsigned int depth, CLine* pline)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(-64 <= beta ); assert(beta  <= 64);
		assert(alpha <= beta);
		assert(0 <= depth); assert(depth <= 60);
		assert(0 <= EmptyCount(P, O)); assert(EmptyCount(P, O) <= 60);
		assert(depth <= EmptyCount(P, O));
		
		return PVS(P, O, NodeCounter, alpha, beta, selectivity, depth, pline);
	}
	
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter)
	{
		return Eval(P, O, NodeCounter, -64, 64, NO_SELECTIVITY, EmptyCount(P, O));
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
		if (empties <= 4)
		{
			switch (empties) {
				case 0: return Eval_0(P, NodeCounter);
				case 1: return ZWS_1(P, O, NodeCounter, alpha);
				case 2: return ZWS_2(P, O, NodeCounter, alpha);
				case 3: return ZWS_3(P, O, NodeCounter, alpha);
				case 4: return ZWS_4(P, O, NodeCounter, alpha);
			}
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
		
		const uint64_t BBParity = quadrant[Parity(P, O)];
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

		if (empties <= A) return ZWS_A(P, O, NodeCounter, alpha);

		int score;
		int bestscore = -64;
		uint8_t BestMove = 64;
		uint64_t BitBoardPossible = PossibleMoves(P, O);
		uint64_t LocalNodeCounter = NodeCounter;
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
	
	int PVS_A(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, const int beta, CLine* pline)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(-64 <= beta ); assert(beta  <= 64);
		assert(alpha <= beta);
		
		const auto empties = EmptyCount(P, O);

		if (empties <= 1) {
			switch (empties) {
				case 0: return Eval_0(P, NodeCounter);
				case 1: return PVS_1(P, O, NodeCounter, alpha, pline);
			}
		}
		
		int lower = alpha;
		int score;
		int bestscore = -65;
		uint8_t BestMove = 64;
		uint64_t BitBoardPossible = PossibleMoves(P, O);
		NodeCounter++;

		if (!BitBoardPossible){
			if (HasMoves(O, P))
				return -PVS_A(O, P, NodeCounter, -beta, -alpha, pline);
			else {
				if (pline) pline.NoMoves();
				return EvalGameOver(P, EmptyCount(P, O));
			}
		}

		CLine* line = (pline ? new CLine(pline.size()-1) : nullptr);
		
		uint64_t LocalNodeCounter = NodeCounter;
		CHashTableValueType ttValue;
		if (LookUpTTPV(P, O, ttValue) || LookUpTT(P, O, ttValue))
			if (USE_PV_TTCUT && !pline && UseTTValue(ttValue, alpha, beta, empties, NO_SELECTIVITY, score))
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
	
	int ZWS(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int selectivity, const unsigned int depth)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(0 <= depth); assert(depth <= 60);
		
		const auto empties = EmptyCount(P, O);
		assert(depth <= empties);

		if (depth <= 3 && depth < empties) {
			throw "Not implemented yet";
			// if (depth == 3) return Midgame::ZWS_3(P, O, NodeCounter, alpha);
			// if (depth == 2) return Midgame::ZWS_2(P, O, NodeCounter, alpha);
			// if (depth == 1) return Midgame::ZWS_1(P, O, NodeCounter, alpha);
			// if (depth == 0) return Midgame::Eval_0(P, O, NodeCounter);
		}
		if (empties <= B && depth == empties) return ZWS_B(P, O, NodeCounter, alpha);

		int score;
		int bestscore = -64;
		uint8_t BestMove = 64;
		uint64_t BitBoardPossible = PossibleMoves(P, O);
		uint64_t LocalNodeCounter = NodeCounter;
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

		if (empties-1 <= B)
			UpdateTT(P, O, NodeCounter-LocalNodeCounter, alpha, alpha+1, bestscore, depth, NO_SELECTIVITY, BestMove, mvList.BestMove(), mvList.NextBestMove());
		else
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

		// if (depth <= 2 && depth < empties) { // TODO !!!
		// 	if (depth == 2) return Midgame::PVS_2(P, O, NodeCounter, alpha, beta, pline);
		// 	if (depth == 1) return Midgame::PVS_1(P, O, NodeCounter, alpha, beta, pline);
		// 	if (depth == 0) return Midgame::Eval_0(P, O, NodeCounter);
		// }
		if (empties <= A && depth == empties) 
			return PVS_A(P, O, NodeCounter, alpha, beta, pline);

		int lower = alpha;
		int score;
		int bestscore = -65;
		uint8_t BestMove = 64;
		uint64_t BitBoardPossible = PossibleMoves(P, O);
		uint64_t LocalNodeCounter = NodeCounter;
		CHashTableValueType ttValue;
		NodeCounter++;

		if (!BitBoardPossible){
			if (HasMoves(O, P))
				return -PVS(O, P, NodeCounter, -beta, -alpha, selectivity, depth, pline);
			else {
				if (!pline.empty()) pline.NoMoves();
				return EvalGameOver(P, empties);
			}
		}

		if (!pline.empty() && StabilityCutoff_PVS(P, O, alpha, score)) return score;
		if (LookUpTTPV(P, O, ttValue) || LookUpTT(P, O, ttValue))
			if (USE_PV_TTCUT && pline.empty() && UseTTValue(ttValue, alpha, beta, depth, selectivity, score))
				return score;
		// if (USE_IID && ttValue.PV == 64) // IID TODO !!!
		// {
		// 	int reduced_depth = (depth == empties) ? depth - A : depth - 2;
		// 	if (reduced_depth >= 3)
		// 	{
		// 		PVS(P, O, NodeCounter, -64, 64, 6, reduced_depth, empties, nullptr);
		// 		if (LookUpTTPV(P, O, ttValue))
		// 			if (USE_PV_TTCUT && pline.empty() && UseTTValue(ttValue, alpha, beta, depth, selectivity, score))
		// 				return score;
		// 	}
		// }

		CLine* line = (pline ? new CLine(pline.size()-1) : nullptr);
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
				if (line) pline.NewPV(mv.move, line);
				if (score >= beta) break;
				if (score > lower) lower = score;
			}
		}

		if (empties-1 <= B)
		{
			UpdateTTPV(P, O, NodeCounter - LocalNodeCounter, alpha, beta, bestscore, depth, NO_SELECTIVITY, BestMove, mvList.BestMove(), mvList.NextBestMove());
			UpdateTT(P, O, NodeCounter - LocalNodeCounter, alpha, beta, bestscore, depth, NO_SELECTIVITY, BestMove, mvList.BestMove(), mvList.NextBestMove());
		}
		else
		{
			UpdateTTPV(P, O, NodeCounter - LocalNodeCounter, alpha, beta, bestscore, depth, selectivity, BestMove, mvList.BestMove(), mvList.NextBestMove());
			UpdateTT(P, O, NodeCounter - LocalNodeCounter, alpha, beta, bestscore, depth, selectivity, BestMove, mvList.BestMove(), mvList.NextBestMove());
		}
		delete line;
		return bestscore;
	}
	// ------------------------------------------------------------------------------------------------
	
	
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
		static const char stability_cutoff_limits[64] = {
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
			int value = 64 - 2 * PopCount(StableStonesPlayer(O, P));
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
		static const char stability_cutoff_limits[64] = {
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
			int value = 64 - 2 * PopCount(StableStonesPlayer(O, P));
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

		int score = (PopCount(P) << 1) - 63; // == PopCount(P) - PopCount(O)
		NodeCounter++;

		if (const auto diff = count_last_flip(P, x))
		{
			NodeCounter++;
			return score + diff + 1;
		}
		else
		{
			if (score > 0)
			{
				if (score + 1 < alpha) return score + 1;

				if (const auto diff = count_last_flip(O, x))
				{
					NodeCounter += 2; // One for passing, one for playing
					return score - diff - 1;
				}
				else
					return score + 1;
			}
			else
			{
				if (score - 1 < alpha) return score - 1;

				if (const auto diff = count_last_flip(O, x))
				{
					NodeCounter += 2; // One for passing, one for playing
					return score - diff - 1;
				}
				else
					return score - 1;
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
			return MAX(bestscore, -ZWS_1(O ^ flipped, P ^ (1ULL << x2) ^ flipped, NodeCounter, -alpha - 1, x1));

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
			return MIN(bestscore, ZWS_1(P ^ flipped, O ^ (1ULL << x2) ^ flipped, NodeCounter, alpha, x1));

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
			return MAX(bestscore, -ZWS_2(O ^ flipped, P ^ (1ULL << x3) ^ flipped, NodeCounter, -alpha - 1, x1, x2));

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
			return MIN(bestscore, ZWS_2(P ^ flipped, O ^ (1ULL << x3) ^ flipped, NodeCounter, alpha, x1, x2));

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

		if (StabilityCutoff_ZWS(P, O, alpha, score)) return score;

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
			return MAX(bestscore, -ZWS_3(O ^ flipped, P ^ (1ULL << x4) ^ flipped, NodeCounter, -alpha - 1, x1, x2, x3));

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
			return MIN(bestscore, ZWS_3(P ^ flipped, O ^ (1ULL << x4) ^ flipped, NodeCounter, alpha, x1, x2, x3));

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

		NodeCounter++;
		if (const auto diff = count_last_flip(P, x))
		{
			NodeCounter++;
			if (pline) pline->NewPV(x);
			return score + diff + 1;
		}
		else
		{
			// TODO: Decide what to do. Maybe you want to uncomment things.
			
			//if (!pline && (score + 1 < alpha)) return score + 1;

			if (const auto diff = count_last_flip(O, x))
			{
				NodeCounter += 2; // One for passing, one for playing
				if (pline) pline->NewPV(x);
				return score - diff - 1;
			}
			else
			{
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
	// ------------------------------------------------------------------------------------------------
}