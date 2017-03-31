#include "game.h"

namespace Endgame_NegaMax
{
	// Function in header file
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter);
	
	// Generic functions
	inline int Eval_0(const uint64_t P, const uint64_t O, uint64_t& NodeCounter);
	inline int Eval_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter);
	inline int Eval_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter);
	inline int Eval_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter);
	inline int Eval_4(const uint64_t P, const uint64_t O, uint64_t& NodeCounter);
	
	// Specialized functions
	int Eval_0(const uint64_t P, uint64_t& NodeCounter);
	int Eval_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const unsigned int x);
	int Eval_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const unsigned int x1, const unsigned int x2);
	int Eval_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const unsigned int x1, const unsigned int x2, const unsigned int x3);
	int Eval_4(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const unsigned int x1, const unsigned int x2, const unsigned int x3, const unsigned int x4);
	
	
	
	// ################################################################################################
	//  Function in header file
	// ################################################################################################
	// ------------------------------------------------------------------------------------------------
	int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter)
	{
		const uint64_t empties = EmptyCount(P, O);
		if (empties <= 4)
		{
			switch (empties)
			{
				case 0 : return Eval_0(P, O, NodeCounter);
				case 1 : return Eval_1(P, O, NodeCounter);
				case 2 : return Eval_2(P, O, NodeCounter);
				case 3 : return Eval_3(P, O, NodeCounter);
				case 4 : return Eval_4(P, O, NodeCounter);
			}
			
		}

		uint64_t BitBoardPossible = PossibleMoves(P, O);
		int Score = -64;
		NodeCounter++;

		if (!BitBoardPossible){
			if (HasMoves(O, P))
				return -Eval(O, P, NodeCounter);
			else //Game is over
				return EvalGameOver(P, empties);
		}

		while (BitBoardPossible)
		{
			const auto Move = BitScanLSB(BitBoardPossible);
			RemoveLSB(BitBoardPossible);
			const uint64_t flipped = flip(P, O, Move);
			Score = MAX(Score, -Eval(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter));
		}

		return Score;
	}
	// ------------------------------------------------------------------------------------------------
	// ################################################################################################
	
	
	// ################################################################################################
	//  Generic functions
	// ################################################################################################
	// ------------------------------------------------------------------------------------------------
	inline int Eval_0(const uint64_t P, const uint64_t O, uint64_t& NodeCounter)
	{
		return Eval_0(P, NodeCounter);
	}
	
	inline int Eval_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter)
	{
		uint64_t empties = ~(P | O);
		const unsigned int x1 = BitScanLSB(empties);

		return Eval_1(P, O, NodeCounter, x1);
	}
	
	inline int Eval_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter)
	{
		uint64_t empties = ~(P | O);
		const unsigned int x1 = BitScanLSB(empties); RemoveLSB(empties);
		const unsigned int x2 = BitScanLSB(empties);

		return Eval_2(P, O, NodeCounter, x1, x2);
	}
	
	inline int Eval_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter)
	{
		uint64_t empties = ~(P | O);
		const unsigned int x1 = BitScanLSB(empties); RemoveLSB(empties);
		const unsigned int x2 = BitScanLSB(empties); RemoveLSB(empties);
		const unsigned int x3 = BitScanLSB(empties);

		return Eval_3(P, O, NodeCounter, x1, x2, x3);
	}
	
	inline int Eval_4(const uint64_t P, const uint64_t O, uint64_t& NodeCounter)
	{
		uint64_t BitBoardEmpty = ~(P | O);
		const unsigned int x1 = BitScanLSB(BitBoardEmpty); RemoveLSB(BitBoardEmpty);
		const unsigned int x2 = BitScanLSB(BitBoardEmpty); RemoveLSB(BitBoardEmpty);
		const unsigned int x3 = BitScanLSB(BitBoardEmpty); RemoveLSB(BitBoardEmpty);
		const unsigned int x4 = BitScanLSB(BitBoardEmpty);

		return Eval_4(P, O, NodeCounter, x1, x2, x3, x4);
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
	
	int Eval_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const unsigned int x)
	{
		int Score, DiffCount;
		Score = (PopCount(P) << 1) - 63; // == PopCount(P) - PopCount(O)
		
		NodeCounter++;
		if ((DiffCount = count_last_flip(P, x)))
		{
			NodeCounter++;
			return Score + DiffCount + 1;
		}
		else if ((DiffCount = count_last_flip(O, x)))
		{
			NodeCounter += 2; // One for passing, one for playing
			return Score - DiffCount - 1;
		}
		else
			return (Score > 0) ? Score + 1 : Score - 1;
	}
	
	int Eval_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const unsigned int x1, const unsigned int x2)
	{
		uint64_t flipped;
		int Score = -64;
		bool played = false;
		NodeCounter++;

		//Play on x1
		if ((O & neighbour[x1]) && (flipped = flip(P, O, x1)))
		{
			Score = MAX(Score, -Eval_1(O ^ flipped, P ^ (1ULL << x1) ^ flipped, NodeCounter, x2));
			played = true;
		}

		//Play on x2
		if ((O & neighbour[x2]) && (flipped = flip(P, O, x2)))
		{
			Score = MAX(Score, -Eval_1(O ^ flipped, P ^ (1ULL << x2) ^ flipped, NodeCounter, x1));
			played = true;
		}

		if (played) return Score;

		NodeCounter++;
		Score = 64;

		//Play on x1
		if ((P & neighbour[x1]) && (flipped = flip(O, P, x1)))
		{
			Score = MIN(Score, Eval_1(P ^ flipped, O ^ (1ULL << x1) ^ flipped, NodeCounter, x2));
			played = true;
		}

		//Play on x2
		if ((P & neighbour[x2]) && (flipped = flip(O, P, x2)))
		{
			Score = MIN(Score, Eval_1(P ^ flipped, O ^ (1ULL << x2) ^ flipped, NodeCounter, x1));
			played = true;
		}

		if (played)
			return Score;
		else
			return EvalGameOver<2>(P);
	}
	
	int Eval_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const unsigned int x1, const unsigned int x2, const unsigned int x3)
	{
		uint64_t flipped;
		int Score = -64;
		bool played = false;
		NodeCounter++;

		//Play on x1
		if ((O & neighbour[x1]) && (flipped = flip(P, O, x1)))
		{
			Score = MAX(Score, -Eval_2(O ^ flipped, P ^ (1ULL << x1) ^ flipped, NodeCounter, x2, x3));
			played = true;
		}

		//Play on x2
		if ((O & neighbour[x2]) && (flipped = flip(P, O, x2)))
		{
			Score = MAX(Score, -Eval_2(O ^ flipped, P ^ (1ULL << x2) ^ flipped, NodeCounter, x1, x3));
			played = true;
		}

		//Play on x3
		if ((O & neighbour[x3]) && (flipped = flip(P, O, x3)))
		{
			Score = MAX(Score, -Eval_2(O ^ flipped, P ^ (1ULL << x3) ^ flipped, NodeCounter, x1, x2));
			played = true;
		}

		if (played) return Score;

		NodeCounter++;
		Score = 64;

		//Play on x1
		if ((P & neighbour[x1]) && (flipped = flip(O, P, x1)))
		{
			Score = MIN(Score, Eval_2(P ^ flipped, O ^ (1ULL << x1) ^ flipped, NodeCounter, x2, x3));
			played = true;
		}

		//Play on x2
		if ((P & neighbour[x2]) && (flipped = flip(O, P, x2)))
		{
			Score = MIN(Score, Eval_2(P ^ flipped, O ^ (1ULL << x2) ^ flipped, NodeCounter, x1, x3));
			played = true;
		}

		//Play on x3
		if ((P & neighbour[x3]) && (flipped = flip(O, P, x3)))
		{
			Score = MIN(Score, Eval_2(P ^ flipped, O ^ (1ULL << x3) ^ flipped, NodeCounter, x1, x2));
			played = true;
		}

		if (played)
			return Score;
		else
			return EvalGameOver<3>(P);
	}
	
	int Eval_4(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const unsigned int x1, const unsigned int x2, const unsigned int x3, const unsigned int x4)
	{
		uint64_t flipped;
		int Score = -64;
		bool played = false;
		++NodeCounter;

		//Play on x1
		if ((O & neighbour[x1]) && (flipped = flip(P, O, x1)))
		{
			Score = MAX(Score, -Eval_3(O ^ flipped, P ^ (1ULL << x1) ^ flipped, NodeCounter, x2, x3, x4));
			played = true;
		}

		//Play on x2
		if ((O & neighbour[x2]) && (flipped = flip(P, O, x2)))
		{
			Score = MAX(Score, -Eval_3(O ^ flipped, P ^ (1ULL << x2) ^ flipped, NodeCounter, x1, x3, x4));
			played = true;
		}

		//Play on x3
		if ((O & neighbour[x3]) && (flipped = flip(P, O, x3)))
		{
			Score = MAX(Score, -Eval_3(O ^ flipped, P ^ (1ULL << x3) ^ flipped, NodeCounter, x1, x2, x4));
			played = true;
		}

		//Play on x4
		if ((O & neighbour[x4]) && (flipped = flip(P, O, x4)))
		{
			Score = MAX(Score, -Eval_3(O ^ flipped, P ^ (1ULL << x4) ^ flipped, NodeCounter, x1, x2, x3));
			played = true;
		}

		if (played) return Score;

		++NodeCounter;
		Score = 64;

		//Play on x1
		if ((P & neighbour[x1]) && (flipped = flip(O, P, x1)))
		{
			Score = MIN(Score, Eval_3(P ^ flipped, O ^ (1ULL << x1) ^ flipped, NodeCounter, x2, x3, x4));
			played = true;
		}

		//Play on x2
		if ((P & neighbour[x2]) && (flipped = flip(O, P, x2)))
		{
			Score = MIN(Score, Eval_3(P ^ flipped, O ^ (1ULL << x2) ^ flipped, NodeCounter, x1, x3, x4));
			played = true;
		}

		//Play on x3
		if ((P & neighbour[x3]) && (flipped = flip(O, P, x3)))
		{
			Score = MIN(Score, Eval_3(P ^ flipped, O ^ (1ULL << x3) ^ flipped, NodeCounter, x1, x2, x4));
			played = true;
		}

		//Play on x4
		if ((P & neighbour[x4]) && (flipped = flip(O, P, x4)))
		{
			Score = MIN(Score, Eval_3(P ^ flipped, O ^ (1ULL << x4) ^ flipped, NodeCounter, x1, x2, x3));
			played = true;
		}

		if (played)
			return Score;
		else
			return EvalGameOver<4>(P);
	}
	// ------------------------------------------------------------------------------------------------
	// ################################################################################################
}