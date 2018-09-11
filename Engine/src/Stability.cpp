#include "Stability.h"
#include "MacrosHell.h"

CStabilityAnalyzer::CStabilityAnalyzer()
{
	memset(edge_stables, 0, 256 * 256 * sizeof(uint8_t));

	for (unsigned int empty = 0; empty < 9; empty++)
		for (unsigned int P = 0; P < 256; P++)
			for (unsigned int O = 0; O < 256; O++)
			{
				if (P & O)
					continue;

				if (8 - PopCount(P) - PopCount(O) == empty)
				{
					unsigned int stables = 0xFF;
					CMoves moves(~(P | O) & 0xFFULL);
					while (!moves.empty())
					{
						const auto move = moves.ExtractMove();
						const auto moveBit = MakeBit(move.field);

						const uint64_t flips_P = Flip(CPosition(P, O), move);
						const uint64_t flips_O = Flip(CPosition(O, P), move);
						stables &= edge_stables[P ^ flips_P ^ moveBit][O ^ flips_P] & ~flips_P & ~moveBit;
						stables &= edge_stables[P ^ flips_O][O ^ flips_O ^ moveBit] & ~flips_O & ~moveBit;

						if (stables == 0)
							continue;
					}
					edge_stables[P][O] = stables;
				}
			}
}

uint64_t CStabilityAnalyzer::Flip(const CPosition& pos, const CMove& move)
{
	return Flip_dir(pos, move, -1) | Flip_dir(pos, move, +1);
}

uint64_t CStabilityAnalyzer::Flip_dir(const CPosition& pos, const CMove& move, const int dX)
{
	uint64_t flips = 0;
	int x = (move.field % 8) + dX;

	while ((x >= 0) && (x < 8))
	{
		const uint64_t bit = MakeBit(x);
		if (pos.GetO() & bit)
			flips |= bit;
		else if (pos.GetP() & bit)
			return flips;
		else
			return 0;
		x += dX;
	}
	return 0;
}

uint64_t CStabilityAnalyzer::GetStableEdges(const CPosition & pos) const
{	// 2 x AND, 2 X SHIFT, 3 x OR, 4 x PEXT, 2 X PDEP
	// 13 OPs
	const auto P = pos.GetP();
	const auto O = pos.GetO();
	const uint64_t L0_Left = 0x8080808080808080ULL;
	const uint64_t L0_Right = 0x0101010101010101ULL;

	const auto StableL0_Bottom = edge_stables[P & 0xFF][O & 0xFF];
	const auto StableL0_Top = static_cast<uint64_t>(edge_stables[P >> 56][O >> 56]) << 56;
	const auto StableL0_Left  = PDep(edge_stables[PExt(P, L0_Left )][PExt(O, L0_Left )], L0_Left );
	const auto StableL0_Right = PDep(edge_stables[PExt(P, L0_Right)][PExt(O, L0_Right)], L0_Right);

	return StableL0_Bottom | StableL0_Top | StableL0_Left | StableL0_Right;
}

uint64_t CStabilityAnalyzer::GetStableStones(const CPosition & pos) const
{
	const uint64_t discs = ~pos.Empties();

	const uint64_t full_h = FullLineHorizontal(discs);
	const uint64_t full_v = FullLineVertival(discs);
	const uint64_t full_d = FullLineDiagonal(discs);
	const uint64_t full_c = FullLineCodiagonal(discs);
	uint64_t new_stables = GetStableEdges(pos) & pos.GetO();
	new_stables |= full_h & full_v & full_d & full_c & pos.GetO() & 0x007E7E7E7E7E7E00ULL;

	uint64_t stables = 0;
	while (new_stables & ~stables)
	{
		stables |= new_stables;
		const uint64_t stables_h = (stables >> 1) | (stables << 1) | full_h;
		const uint64_t stables_v = (stables >> 8) | (stables << 8) | full_v;
		const uint64_t stables_d = (stables >> 9) | (stables << 9) | full_d;
		const uint64_t stables_c = (stables >> 7) | (stables << 7) | full_c;
		new_stables = stables_h & stables_v & stables_d & stables_c & pos.GetO() & 0x007E7E7E7E7E7E00ULL;
	}
	return stables;
}

uint64_t CStabilityAnalyzer::FullLineHorizontal(uint64_t discs)
{	// 4 x AND, 3 x SHIFT, 1 x MUL
	// 8 OPs
	discs &= discs >> 4;
	discs &= discs >> 2;
	discs &= discs >> 1;
	discs &= 0x0001010101010100ULL;
	return discs * 0xFFULL;
}

uint64_t CStabilityAnalyzer::FullLineVertival(uint64_t discs)
{	// 4 x AND, 3 x SHIFT, 1 x MUL
	// 8 OPs
	discs &= discs >> 32;
	discs &= discs >> 16;
	discs &= discs >> 8;
	discs &= 0x7EULL;
	return discs * 0x0101010101010101ULL;
}

uint64_t CStabilityAnalyzer::FullLineDiagonal(const uint64_t discs)
{	// 5 x SHR, 5 x SHL, 7x AND, 10 x OR
	// 27 OPs
	const uint64_t edge = 0xFF818181818181FFULL;

	uint64_t full_l = discs & (edge | (discs >> 9));
	uint64_t full_r = discs & (edge | (discs << 9));
	uint64_t edge_l = edge | (edge >> 9);
	uint64_t edge_r = edge | (edge << 9);
	full_l &= edge_l | (full_l >> 18);
	full_r &= edge_r | (full_r << 18);
	edge_l |= edge_l >> 18;
	edge_r |= edge_r << 18;
	full_l &= edge_l | (full_l >> 36);
	full_r &= edge_r | (full_r << 36);

	return full_r & full_l;
}

uint64_t CStabilityAnalyzer::FullLineCodiagonal(const uint64_t discs)
{	// 5 x SHR, 5 x SHL, 7x AND, 10 x OR
	// 27 OPs
	const uint64_t edge = 0xFF818181818181FFULL;

	uint64_t full_l = discs & (edge | (discs >> 7));
	uint64_t full_r = discs & (edge | (discs << 7));
	uint64_t edge_l = edge | (edge >> 7);
	uint64_t edge_r = edge | (edge << 7);
	full_l &= edge_l | (full_l >> 14);
	full_r &= edge_r | (full_r << 14);
	edge_l |= edge_l >> 14;
	edge_r |= edge_r << 14;
	full_l &= edge_l | (full_l >> 28);
	full_r &= edge_r | (full_r << 28);

	return full_r & full_l;
}