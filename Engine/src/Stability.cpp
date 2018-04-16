#include "Stability.h"

CStabilityAnalyzer::CStabilityAnalyzer()
{
	memset(edge_stables, 0, 256 * 256 * sizeof(uint8_t));

	for (unsigned int empty = 0; empty < 9; empty++)
		for (unsigned int P = 0; P < 256; P++)
			for (unsigned int O = 0; O < 256; O++)
			{
				if (P & O)
					continue;

				const auto pos = CPosition(P, O ^ ~0xFFULL);
				if (pos.EmptyCount() == empty)
				{
					CMoves moves(pos.Empties());
					unsigned int stables = 0xFF;
					while (!moves.empty())
					{
						const auto move = moves.ExtractMove();
						const auto moveBit = MakeBit(move.field);

						const uint64_t flips_P = Flip(pos           , move) & 0xFFULL;
						const uint64_t flips_O = Flip(pos.PlayPass(), move) & 0xFFULL;
						stables &= edge_stables[P ^ flips_P ^ moveBit][O ^ flips_P] & ~flips_P & ~moveBit;
						stables &= edge_stables[P ^ flips_O][O ^ flips_O ^ moveBit] & ~flips_O & ~moveBit;

						if (stables == 0)
							continue;
					}
					edge_stables[P][O] = stables;
				}
			}
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
{	// 11 x AND, 11 x CMP, 11 x OR
	// 33 OPs
	uint64_t full = 0;
	if (TestBits(discs, 0x0402010000000000ULL)) full |= 0x0402010000000000ULL;
	if (TestBits(discs, 0x0804020100000000ULL)) full |= 0x0804020100000000ULL;
	if (TestBits(discs, 0x1008040201000000ULL)) full |= 0x1008040201000000ULL;
	if (TestBits(discs, 0x2010080402010000ULL)) full |= 0x2010080402010000ULL;
	if (TestBits(discs, 0x4020100804020100ULL)) full |= 0x4020100804020100ULL;
	if (TestBits(discs, 0x8040201008040201ULL)) full |= 0x8040201008040201ULL;
	if (TestBits(discs, 0x0080402010080402ULL)) full |= 0x0080402010080402ULL;
	if (TestBits(discs, 0x0000804020100804ULL)) full |= 0x0000804020100804ULL;
	if (TestBits(discs, 0x0000008040201008ULL)) full |= 0x0000008040201008ULL;
	if (TestBits(discs, 0x0000000080402010ULL)) full |= 0x0000000080402010ULL;
	if (TestBits(discs, 0x0000000000804020ULL)) full |= 0x0000000000804020ULL;
	return full;
}

uint64_t CStabilityAnalyzer::FullLineCodiagonal(const uint64_t discs)
{	// 11 x AND, 11 x CMP, 11 x OR
	// 33 OPs
	uint64_t full = 0;
	if (TestBits(discs, 0x2040800000000000ULL)) full |= 0x2040800000000000ULL;
	if (TestBits(discs, 0x1020408000000000ULL)) full |= 0x1020408000000000ULL;
	if (TestBits(discs, 0x0810204080000000ULL)) full |= 0x0810204080000000ULL;
	if (TestBits(discs, 0x0408102040800000ULL)) full |= 0x0408102040800000ULL;
	if (TestBits(discs, 0x0204081020408000ULL)) full |= 0x0204081020408000ULL;
	if (TestBits(discs, 0x0102040810204080ULL)) full |= 0x0102040810204080ULL;
	if (TestBits(discs, 0x0001020408102040ULL)) full |= 0x0001020408102040ULL;
	if (TestBits(discs, 0x0000010204081020ULL)) full |= 0x0000010204081020ULL;
	if (TestBits(discs, 0x0000000102040810ULL)) full |= 0x0000000102040810ULL;
	if (TestBits(discs, 0x0000000001020408ULL)) full |= 0x0000000001020408ULL;
	if (TestBits(discs, 0x0000000000010204ULL)) full |= 0x0000000000010204ULL;
	return full;
}

static const CStabilityAnalyzer StabilityAnalyzer;

uint64_t GetStableEdges(const CPosition & pos)
{
	return StabilityAnalyzer.GetStableEdges(pos);
}

uint64_t GetStableStones(const CPosition & pos)
{
	return StabilityAnalyzer.GetStableStones(pos);
}
