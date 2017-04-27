#include "game.h"

namespace Endgame_AlphaBeta
{
	// Function in header file
	// This is not a strict hard fail implementation but it is not a soft fail implementation either
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter);
	
	// Generic functions
	inline int Eval_0(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta);
	inline int Eval_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta);
	inline int Eval_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta);
	inline int Eval_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta);
	inline int Eval_4(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta);
	
	// Specialized functions
	int Eval_0(const uint64_t P, uint64_t& NodeCounter);
	int Eval_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha,           const unsigned int x);
	int Eval_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta, const unsigned int x1, const unsigned int x2);
	int Eval_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta, const unsigned int x1, const unsigned int x2, const unsigned int x3);
	int Eval_4(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta, const unsigned int x1, const unsigned int x2, const unsigned int x3, const unsigned int x4);
	int Eval  (const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta);
	
	
	
	// ################################################################################################
	//  Function in header file
	// ################################################################################################
	// ------------------------------------------------------------------------------------------------
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter)
	{
		return Eval(P, O, NodeCounter, -64, 64);
	}
	// ------------------------------------------------------------------------------------------------
	// ################################################################################################
	
	
	// ################################################################################################
	//  Generic functions
	// ################################################################################################
	// ------------------------------------------------------------------------------------------------
	inline int Eval_0(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta)
	{
		return Eval_0(P, NodeCounter);
	}
	
	inline int Eval_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta)
	{
		uint64_t empties = ~(P | O);
		const unsigned int x1 = BitScanLSB(empties);

		return Eval_1(P, O, NodeCounter, alpha, x1);
	}
	
	inline int Eval_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta)
	{
		uint64_t empties = ~(P | O);
		const unsigned int x1 = BitScanLSB(empties); RemoveLSB(empties);
		const unsigned int x2 = BitScanLSB(empties);

		return Eval_2(P, O, NodeCounter, alpha, beta, x1, x2);
	}
	
	inline int Eval_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta)
	{
		uint64_t empties = ~(P | O);
		const unsigned int x1 = BitScanLSB(empties); RemoveLSB(empties);
		const unsigned int x2 = BitScanLSB(empties); RemoveLSB(empties);
		const unsigned int x3 = BitScanLSB(empties);

		return Eval_3(P, O, NodeCounter, alpha, beta, x1, x2, x3);
	}
	
	inline int Eval_4(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta)
	{
		uint64_t BitBoardEmpty = ~(P | O);
		const unsigned int x1 = BitScanLSB(BitBoardEmpty); RemoveLSB(BitBoardEmpty);
		const unsigned int x2 = BitScanLSB(BitBoardEmpty); RemoveLSB(BitBoardEmpty);
		const unsigned int x3 = BitScanLSB(BitBoardEmpty); RemoveLSB(BitBoardEmpty);
		const unsigned int x4 = BitScanLSB(BitBoardEmpty);

		return Eval_4(P, O, NodeCounter, alpha, beta, x1, x2, x3, x4);
	}
	// ------------------------------------------------------------------------------------------------
	// ################################################################################################
	
	
	// ################################################################################################
	//  Specialized functions
	// ################################################################################################
	// ------------------------------------------------------------------------------------------------
	int Eval_0(const uint64_t P, uint64_t& NodeCounter)
	{
		NodeCounter++;
		return EvalGameOver<0>(P);
	}
	
	int Eval_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, const unsigned int x)
	{
		const int score = (PopCount(P) << 1) - 63; // == PopCount(P) - PopCount(O)
		
		if (const auto DiffCount = count_last_flip(P, x))
		{
			NodeCounter += 2; // One for this, one for playing.
			return score + DiffCount + 1;
		}
		else
		{
			if (score < alpha)
			{
				NodeCounter += 2; // One for this, one for playing.
				return alpha;
			}
			if (const auto DiffCount = count_last_flip(O, x))
			{
				NodeCounter += 2; // One for passing, one for playing.
				return score - DiffCount - 1;
			}
			else
			{
				NodeCounter++;
				return (score > 0) ? score + 1 : score - 1;
			}
		}
	}
	
	int Eval_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta, const unsigned int x1, const unsigned int x2)
	{
		int score = -128;
		uint64_t flipped;

		//Play on x1
		if ((O & neighbour[x1]) && (flipped = flip(P, O, x1)))
		{
			score = -Eval_1(O ^ flipped, P ^ (1ULL << x1) ^ flipped, NodeCounter, -beta, x2);
			if (score >= beta) return beta;
			if (score > alpha) alpha = score;
		}

		//Play on x2
		if ((O & neighbour[x2]) && (flipped = flip(P, O, x2)))
		{
			score = -Eval_1(O ^ flipped, P ^ (1ULL << x2) ^ flipped, NodeCounter, -beta, x1);
			if (score >= beta) return beta;
			if (score > alpha) alpha = score;
			return alpha;
		}

		if (score != -128) {
			NodeCounter++;
			return alpha;
		}

		//Play on x1
		if ((P & neighbour[x1]) && (flipped = flip(O, P, x1)))
		{
			score = Eval_1(P ^ flipped, O ^ (1ULL << x1) ^ flipped, NodeCounter, alpha, x2);
			if (score <= alpha) return alpha;
			if (score < beta) beta = score;
		}

		//Play on x2
		if ((P & neighbour[x2]) && (flipped = flip(O, P, x2)))
		{
			score = Eval_1(P ^ flipped, O ^ (1ULL << x2) ^ flipped, NodeCounter, alpha, x1);
			if (score <= alpha) return alpha;
			if (score < beta) beta = score;
			return beta;
		}

		if (score != -128) {
			NodeCounter += 2; // One for this, one for passing.
			return beta;
		}
		else {
			NodeCounter++; // One for this.
			return EvalGameOver<2>(P);
		}
	}
	
	int Eval_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta, const unsigned int x1, const unsigned int x2, const unsigned int x3)
	{
		int score = -128;
		uint64_t flipped;

		//Play on x1
		if ((O & neighbour[x1]) && (flipped = flip(P, O, x1)))
		{
			score = -Eval_2(O ^ flipped, P ^ (1ULL << x1) ^ flipped, NodeCounter, -beta, -alpha, x2, x3);
			if (score >= beta) return beta;
			if (score > alpha) alpha = score;
		}

		//Play on x2
		if ((O & neighbour[x2]) && (flipped = flip(P, O, x2)))
		{
			score = -Eval_2(O ^ flipped, P ^ (1ULL << x2) ^ flipped, NodeCounter, -beta, -alpha, x1, x3);
			if (score >= beta) return beta;
			if (score > alpha) alpha = score;
		}

		//Play on x3
		if ((O & neighbour[x3]) && (flipped = flip(P, O, x3)))
		{
			score = -Eval_2(O ^ flipped, P ^ (1ULL << x3) ^ flipped, NodeCounter, -beta, -alpha, x1, x2);
			if (score >= beta) return beta;
			if (score > alpha) alpha = score;
			return alpha;
		}

		if (score != -128) {
			NodeCounter++;
			return alpha;
		}

		//Play on x1
		if ((P & neighbour[x1]) && (flipped = flip(O, P, x1)))
		{
			score = Eval_2(P ^ flipped, O ^ (1ULL << x1) ^ flipped, NodeCounter, alpha, beta, x2, x3);
			if (score <= alpha)	return alpha;
			if (score < beta) beta = score;
		}

		//Play on x2
		if ((P & neighbour[x2]) && (flipped = flip(O, P, x2)))
		{
			score = Eval_2(P ^ flipped, O ^ (1ULL << x2) ^ flipped, NodeCounter, alpha, beta, x1, x3);
			if (score <= alpha)	return alpha;
			if (score < beta) beta = score;
		}

		//Play on x3
		if ((P & neighbour[x3]) && (flipped = flip(O, P, x3)))
		{
			score = Eval_2(P ^ flipped, O ^ (1ULL << x3) ^ flipped, NodeCounter, alpha, beta, x1, x2);
			if (score <= alpha)	return alpha;
			if (score < beta) beta = score;
			return beta;
		}

		if (score != -128) {
			NodeCounter += 2; // One for this, one for passing.
			return beta;
		}
		else {
			NodeCounter++; // One for this.
			return EvalGameOver<3>(P);
		}
	}
	
	int Eval_4(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta, const unsigned int x1, const unsigned int x2, const unsigned int x3, const unsigned int x4)
	{
		int score = -128;
		uint64_t flipped;
		
		//Play on x1
		if ((O & neighbour[x1]) && (flipped = flip(P, O, x1)))
		{
			score = -Eval_3(O ^ flipped, P ^ (1ULL << x1) ^ flipped, NodeCounter, -beta, -alpha, x2, x3, x4);
			if (score >= beta) return beta;
			if (score > alpha) alpha = score;
		}

		//Play on x2
		if ((O & neighbour[x2]) && (flipped = flip(P, O, x2)))
		{
			score = -Eval_3(O ^ flipped, P ^ (1ULL << x2) ^ flipped, NodeCounter, -beta, -alpha, x1, x3, x4);
			if (score >= beta) return beta;
			if (score > alpha) alpha = score;
		}

		//Play on x3
		if ((O & neighbour[x3]) && (flipped = flip(P, O, x3)))
		{
			score = -Eval_3(O ^ flipped, P ^ (1ULL << x3) ^ flipped, NodeCounter, -beta, -alpha, x1, x2, x4);
			if (score >= beta) return beta;
			if (score > alpha) alpha = score;
		}

		//Play on x4
		if ((O & neighbour[x4]) && (flipped = flip(P, O, x4)))
		{
			score = -Eval_3(O ^ flipped, P ^ (1ULL << x4) ^ flipped, NodeCounter, -beta, -alpha, x1, x2, x3);
			if (score >= beta) return beta;
			if (score > alpha) alpha = score;
			return alpha;
		}

		if (score != -128) {
			NodeCounter++;
			return alpha;
		}

		//Play on x1
		if ((P & neighbour[x1]) && (flipped = flip(O, P, x1)))
		{
			score = Eval_3(P ^ flipped, O ^ (1ULL << x1) ^ flipped, NodeCounter, alpha, beta, x2, x3, x4);
			if (score <= alpha)	return alpha;
			if (score < beta) beta = score;
		}

		//Play on x2
		if ((P & neighbour[x2]) && (flipped = flip(O, P, x2)))
		{
			score = Eval_3(P ^ flipped, O ^ (1ULL << x2) ^ flipped, NodeCounter, alpha, beta, x1, x3, x4);
			if (score <= alpha)	return alpha;
			if (score < beta) beta = score;
		}

		//Play on x3
		if ((P & neighbour[x3]) && (flipped = flip(O, P, x3)))
		{
			score = Eval_3(P ^ flipped, O ^ (1ULL << x3) ^ flipped, NodeCounter, alpha, beta, x1, x2, x4);
			if (score <= alpha)	return alpha;
			if (score < beta) beta = score;
		}

		//Play on x4
		if ((P & neighbour[x4]) && (flipped = flip(O, P, x4)))
		{
			score = Eval_3(P ^ flipped, O ^ (1ULL << x4) ^ flipped, NodeCounter, alpha, beta, x1, x2, x3);
			if (score <= alpha)	return alpha;
			if (score < beta) beta = score;
			return beta;
		}

		if (score != -128) {
			NodeCounter += 2; // One for this, one for passing.
			return beta;
		}
		else {
			NodeCounter++; // One for this.
			return EvalGameOver<4>(P);
		}
	}
	
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta)
	{
		const uint64_t empties = EmptyCount(P, O);
		switch (empties)
		{
			case 0: return Eval_0(P, O, NodeCounter, alpha, beta);
			case 1: return Eval_1(P, O, NodeCounter, alpha, beta);
			case 2: return Eval_2(P, O, NodeCounter, alpha, beta);
			case 3: return Eval_3(P, O, NodeCounter, alpha, beta);
			case 4: return Eval_4(P, O, NodeCounter, alpha, beta);
		}

		uint64_t BitBoardPossible = PossibleMoves(P, O);
		int score = -128;
		NodeCounter++;

		if (!BitBoardPossible){
			if (HasMoves(O, P))
				return -Eval(O, P, NodeCounter, -beta, -alpha);
			else //Game is over
				return CLAMP(EvalGameOver(P, empties), alpha, beta);
		}

		while (BitBoardPossible)
		{
			const auto Move = BitScanLSB(BitBoardPossible);
			RemoveLSB(BitBoardPossible);
			const auto flipped = flip(P, O, Move);
			score = -Eval(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter, -beta, -alpha);
			if (score >= beta) return beta;
			if (score > alpha) alpha = score;
		}

		return alpha;
	}
	// ------------------------------------------------------------------------------------------------
	// ################################################################################################
}