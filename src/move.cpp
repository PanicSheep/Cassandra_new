#include "move.h"

CMoveList::CMoveList(uint64_t P, uint64_t O, uint64_t& NodeCounter, uint64_t BitBoardPossible, int depth, int alpha, const CHashTableValueType& ttValue, const bool pvs)
{
	//if (!pvs) depth -= 2;
	const int A = 9;
	const int C = 8;
	const int H = 7;
	const int D = 6;
	const int I = 5;
	const int G = 4;
	const int F = 3;
	const int B = 2;
	const int E = 1;
	const int J = 0;
	static const int FIELD_VALUE[64] = {
		A, B, C, D, D, C, B, A,
		B, E, F, G, G, F, E, B,
		C, F, H, I, I, H, F, C,
		D, G, I, J, J, I, G, D,
		D, G, I, J, J, I, G, D,
		C, F, H, I, I, H, F, C,
		B, E, F, G, G, F, E, B,
		A, B, C, D, D, C, B, A,
	};
	static const int PARITY_VALUE[16] = { 0, 20, 0, 10, 1, 10, 2, 10, 3, 5, 3, 4, 3, 4, 3, 4 };
	const uint64_t BBEmpties = ~(P | O);
	const uint64_t empties = EmptyCount(P, O);

	m_Moves = std::vector<CMove>();
	m_Moves.reserve(PopCount(BitBoardPossible));
	int index = 0;

	int sort_depth;
	int min_depth = 9;
    if (empties <= 27) min_depth += (30 - static_cast<int>(empties)) / 3;
    if (depth >= min_depth)
	{
		sort_depth = (depth - 15) / 3;
		if (ttValue.beta < alpha) sort_depth -= 2; 
		sort_depth = CLAMP(sort_depth, 0, 6);
    }
	else
        sort_depth = -1;

	int sort_alpha = std::max(-64, alpha - 8);

	// Fill MoveList
	while (BitBoardPossible)
	{
		CMove Move;
		Move.move = BitScanLSB(BitBoardPossible);
		RemoveLSB(BitBoardPossible);
		const uint64_t flipped = flip(P, O, Move.move);
		Move.P = O ^ flipped;
		Move.O = P ^ flipped ^ (1ULL << Move.move);
		     if (flipped == O) Move.Value = 1 << 31;
		else if ((ttValue.depth > sort_depth) && (Move.move == ttValue.PV)) Move.Value = 1 << 30;
		else if ((ttValue.depth > sort_depth) && (Move.move == ttValue.AV)) Move.Value = 1 << 29;
		else
		{
			uint64_t PossMoves = PossibleMoves(Move.P, Move.O);
			Move.Value = FIELD_VALUE[Move.move];
			Move.Value += PARITY_VALUE[PopCount(BBEmpties & quadrant_mask[Move.move])];
			Move.Value -= static_cast<int>(PopCount(PossMoves) << 17);
			Move.Value -= static_cast<int>(PopCount(PossMoves & 0x8100000000000081UL) << 18);
			if (sort_depth < 0)
				Move.Value += static_cast<int>(PopCount(StableStonesCornerAndCo(Move.O)) << 12);
			else
				Move.Value += static_cast<int>(PopCount(StableEdges(Move.O, Move.P) & Move.O) << 12);
			Move.Value -= static_cast<int>(PopCount(OpponentsExposed(Move.P, Move.O)) << 6);
			switch (sort_depth)
			{
			case -1:
				break;
			case 0:
				Move.Value -= EvaluatePattern(Move.P, Move.O) << 16;
				break;
			case 1:
			case 2:
				Move.Value -= PVS::Eval(Move.P, Move.O, NodeCounter, -64, -sort_alpha, NO_SELECTIVITY, sort_depth) << 17;
				break;
			default:
				Move.Value -= PVS::Eval(Move.P, Move.O, NodeCounter, -64, -sort_alpha, NO_SELECTIVITY, sort_depth) << 18;
				CHashTableValueType ttValue2;
				if (LookUpTT(Move.P, Move.O, ttValue2)) Move.Value += 1 << 21;
				break;
			}
		}
		m_Moves.push_back(Move);
	}
	sort();
	return;
}
