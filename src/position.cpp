#include "position.h"

uint64_t FlipDiagonal(uint64_t b)
{	// 9 x XOR, 6 x SHIFT, 3 x AND
	// 18 OPs
	
	// \ # # # # # # #
	// # \ # # # # # #
	// # # \ # # # # #
	// # # # \ # # # #
	// # # # # \ # # #
	// # # # # # \ # #
	// # # # # # # \ #
	// # # # # # # # \.<-LSB
	uint64_t t;
	t  = (b ^ (b >>  7)) & 0x00AA00AA00AA00AAULL;
	b ^=  t ^ (t <<  7);
	t  = (b ^ (b >> 14)) & 0x0000CCCC0000CCCCULL;
	b ^=  t ^ (t << 14);
	t  = (b ^ (b >> 28)) & 0x00000000F0F0F0F0ULL;
	b ^=  t ^ (t << 28);
	return b;
}
uint64_t FlipCodiagonal(uint64_t b)
{	// 9 x XOR, 6 x SHIFT, 3 x AND
	// 18 OPs
	
	// # # # # # # # /
	// # # # # # # / #
	// # # # # # / # #
	// # # # # / # # #
	// # # # / # # # #
	// # # / # # # # #
	// # / # # # # # #
	// / # # # # # # #<-LSB
	uint64_t t;
	t  =  b ^ (b << 36);
	b ^= (t ^ (b >> 36)) & 0xF0F0F0F00F0F0F0FULL;
	t  = (b ^ (b << 18)) & 0xCCCC0000CCCC0000ULL;
	b ^=  t ^ (t >> 18);
	t  = (b ^ (b <<  9)) & 0xAA00AA00AA00AA00ULL;
	b ^=  t ^ (t >>  9);
	return b;
}
uint64_t FlipVertical(uint64_t b)
{	// 1 x BSwap
	// 1 OPs
	
	// # # # # # # # #
	// # # # # # # # #
	// # # # # # # # #
	// # # # # # # # #
	// ---------------
	// # # # # # # # #
	// # # # # # # # #
	// # # # # # # # #
	// # # # # # # # #<-LSB
	return BSwap(b);
	//b = ((b >>  8) & 0x00FF00FF00FF00FFULL) | ((b <<  8) & 0xFF00FF00FF00FF00ULL);
	//b = ((b >> 16) & 0x0000FFFF0000FFFFULL) | ((b << 16) & 0xFFFF0000FFFF0000ULL);
	//b = ((b >> 32) & 0x00000000FFFFFFFFULL) | ((b << 32) & 0xFFFFFFFF00000000ULL);
	//return b;
}
uint64_t FlipHorizontal(uint64_t b)
{	// 6 x SHIFT, 6 x AND, 3 x OR
	// 15 OPs
	
	// # # # #|# # # #
	// # # # #|# # # #
	// # # # #|# # # #
	// # # # #|# # # #
	// # # # #|# # # #
	// # # # #|# # # #
	// # # # #|# # # #
	// # # # #|# # # #<-LSB
	b = ((b >> 1) & 0x5555555555555555ULL) | ((b << 1) & 0xAAAAAAAAAAAAAAAAULL);
	b = ((b >> 2) & 0x3333333333333333ULL) | ((b << 2) & 0xCCCCCCCCCCCCCCCCULL);
	b = ((b >> 4) & 0x0F0F0F0F0F0F0F0FULL) | ((b << 4) & 0xF0F0F0F0F0F0F0F0ULL);
	return b;
}

std::string board1D(const uint64_t P, const uint64_t O)
{
	std::string s;
	for (unsigned int i = 0; i < 8; i++)
		for (unsigned int j = 0; j < 8; j++)
		{
			const uint64_t bitMask = 0x8000000000000000ULL >> (i * 8 + j);
			bool b_P = ((P & bitMask) != 0);
			bool b_O = ((O & bitMask) != 0);
			     if (b_P && b_O) s.append("#");
			else if (b_P)        s.append("X");
			else if (b_O)        s.append("O");
			else                 s.append("-");
		}
	return s;
}


std::string board2D(const uint64_t P, const uint64_t O, const uint64_t possibleMoves)
{
	std::string s = "  H G F E D C B A  \n";
	for (unsigned int i = 0; i < 8; i++)
	{
		s.append(std::to_string(8-i));
		for (unsigned int j = 0; j < 8; j++)
		{
			const uint64_t bitMask = 0x8000000000000000ULL >> (i * 8 + j);
			bool b_P = ((P & bitMask) != 0);
			bool b_O = ((O & bitMask) != 0);
			bool b_M = ((possibleMoves & bitMask) != 0);
			     if (b_P && b_O) s.append(" #");
			else if (b_P)        s.append(" X");
			else if (b_O)        s.append(" O");
			else if (b_M)        s.append(" +");
			else                 s.append(" -");
		}
		s.append(" " + std::to_string(8-i) + "\n");
	}
	return s.append("  H G F E D C B A  ");
}
std::string board2D(const uint64_t P, const uint64_t O) { return board2D(P, O, 0); }

inline bool TestBits(const uint64_t bitfield, const uint64_t mask) { return (bitfield & mask) == mask; }

uint64_t StableStonesFullEdges(const uint64_t P, const uint64_t O)
{
	uint64_t mask = 0;
    const uint64_t Occ = P | O;
	const uint64_t L0_bot = 0x00000000000000FFULL;
	const uint64_t L0_top = 0xFF00000000000000ULL;
	const uint64_t L0_lhs = 0x8080808080808080ULL;
	const uint64_t L0_rhs = 0x0101010101010101ULL;
    if (TestBits(Occ, L0_bot)) mask |= L0_bot;
    if (TestBits(Occ, L0_top)) mask |= L0_top;
    if (TestBits(Occ, L0_lhs)) mask |= L0_lhs;
    if (TestBits(Occ, L0_rhs)) mask |= L0_rhs;
    return O & mask;
}
uint64_t StableStonesFullEdgesSecondOrder(const uint64_t P, const uint64_t O)
{
	uint64_t mask = 0;
	uint64_t Occ = P | O;
	if (TestBits(Occ, 0x000000000000FFFFULL)) mask |= (O << 7) & (O << 8) & (O << 9) & 0x0000000000007E00ULL;
	if (TestBits(Occ, 0x0303030303030303ULL)) mask |= (O << 9) & (O << 1) & (O >> 7) & 0x0002020202020200ULL;
	if (TestBits(Occ, 0xFFFF000000000000ULL)) mask |= (O >> 7) & (O >> 8) & (O >> 9) & 0x007E000000000000ULL;
	if (TestBits(Occ, 0xC0C0C0C0C0C0C0C0ULL)) mask |= (O << 7) & (O >> 1) & (O >> 9) & 0x0040404040404000ULL;
    return O & mask;
}

uint64_t OneQuadrantTriangleTest(uint64_t O, uint64_t T1, uint64_t T2, uint64_t T3, uint64_t T4, uint64_t T5, uint64_t T6, uint64_t T7, uint64_t T8)
{
	if (!TestBits(O, T1)) return 0;
	if (!TestBits(O, T2)) return T1;
	if (!TestBits(O, T3)) return T2;
	if (!TestBits(O, T4)) return T3;
	if (!TestBits(O, T5)) return T4;
	if (!TestBits(O, T6)) return T5;
	if (!TestBits(O, T7)) return T6;
	if (!TestBits(O, T8)) return T7;
	return T8;
}

uint64_t StableStonesTriangles(const uint64_t O)
{
	const uint64_t Q0T1 = 0x0000000000000001ULL;
	const uint64_t Q0T2 = 0x0000000000000103ULL;
	const uint64_t Q0T3 = 0x0000000000010307ULL;
	const uint64_t Q0T4 = 0x000000000103070FULL;
	const uint64_t Q0T5 = 0x0000000103070F1FULL;
	const uint64_t Q0T6 = 0x00000103070F1F3FULL;
	const uint64_t Q0T7 = 0x000103070F1F3F7FULL;
	const uint64_t Q0T8 = 0x0103070F1F3F7FFFULL;
	const uint64_t Q1T1 = 0x0000000000000080ULL;
	const uint64_t Q1T2 = 0x00000000000080C0ULL;
	const uint64_t Q1T3 = 0x000000000080C0E0ULL;
	const uint64_t Q1T4 = 0x0000000080C0E0F0ULL;
	const uint64_t Q1T5 = 0x00000080C0E0F0F8ULL;
	const uint64_t Q1T6 = 0x000080C0E0F0F8FCULL;
	const uint64_t Q1T7 = 0x0080C0E0F0F8FCFEULL;
	const uint64_t Q1T8 = 0x80C0E0F0F8FCFEFFULL;
	const uint64_t Q2T1 = 0x8000000000000000ULL;
	const uint64_t Q2T2 = 0xC080000000000000ULL;
	const uint64_t Q2T3 = 0xE0C0800000000000ULL;
	const uint64_t Q2T4 = 0xF0E0C08000000000ULL;
	const uint64_t Q2T5 = 0xF8F0E0C080000000ULL;
	const uint64_t Q2T6 = 0xFCF8F0E0C0800000ULL;
	const uint64_t Q2T7 = 0xFEFCF8F0E0C08000ULL;
	const uint64_t Q2T8 = 0xFFFEFCF8F0E0C080ULL;
	const uint64_t Q3T1 = 0x0100000000000000ULL;
	const uint64_t Q3T2 = 0x0301000000000000ULL;
	const uint64_t Q3T3 = 0x0703010000000000ULL;
	const uint64_t Q3T4 = 0x0F07030100000000ULL;
	const uint64_t Q3T5 = 0x1F0F070301000000ULL;
	const uint64_t Q3T6 = 0x3F1F0F0703010000ULL;
	const uint64_t Q3T7 = 0x7F3F1F0F07030100ULL;
	const uint64_t Q3T8 = 0xFF7F3F1F0F070301ULL;
	
    const auto Q0 = OneQuadrantTriangleTest(O, Q0T1, Q0T2, Q0T3, Q0T4, Q0T5, Q0T6, Q0T7, Q0T8);
	const auto Q1 = OneQuadrantTriangleTest(O, Q1T1, Q1T2, Q1T3, Q1T4, Q1T5, Q1T6, Q1T7, Q1T8);
	const auto Q2 = OneQuadrantTriangleTest(O, Q2T1, Q2T2, Q2T3, Q2T4, Q2T5, Q2T6, Q2T7, Q2T8);
	const auto Q3 = OneQuadrantTriangleTest(O, Q3T1, Q3T2, Q3T3, Q3T4, Q3T5, Q3T6, Q3T7, Q3T8);

	return Q0 | Q1 | Q2 | Q3;
}

namespace Stability
{
	uint8_t edge_stables[256][256];

	void Initialize()
	{
		memset(edge_stables, 0, 256 * 256 * sizeof(uint8_t));

		for (unsigned int empty = 0; empty < 9; empty++)
			for (unsigned int P = 0; P < 256; P++)
				for (unsigned int O = 0; O < 256; O++)
				{
					if (P & O)
						continue;
					
					unsigned int empties = (P | O) ^ 0xFF;
					if (PopCount(empties) == empty)
					{
						unsigned int stables = 0xFF;
						while (empties)
						{
							const auto move = BitScanLSB(empties);
							const auto moveBit = MakeBit(move);
							empties &= empties - 1; // RemoveLSB

							const uint64_t flipped_P = flip(P, O, move) & 0xFFULL;
							const uint64_t flipped_O = flip(O, P, move) & 0xFFULL;
							stables &= edge_stables[P ^ flipped_P ^ moveBit][O ^ flipped_P          ] & ~flipped_P & ~moveBit;
							stables &= edge_stables[P ^ flipped_O          ][O ^ flipped_O ^ moveBit] & ~flipped_O & ~moveBit;

							if (stables == 0)
								continue;
						}
						edge_stables[P][O] = stables;
					}
				}
	}
	
	uint64_t FullLineHorizontal(uint64_t discs)
	{
		// 4 x AND, 3 x SHIFT, 1 x MUL
		discs &= discs >> 4;
		discs &= discs >> 2;
		discs &= discs >> 1;
		discs &= 0x0001010101010100ULL;
		return discs * 0xFFULL;
	}
	
	uint64_t FullLineVertival(uint64_t discs)
	{
		// 4 x AND, 3 x SHIFT, 1 x MUL
		discs &= discs >> 32;
		discs &= discs >> 16;
		discs &= discs >>  8;
		discs &= 0x7EULL;
		return discs * 0x0101010101010101ULL;
	}
	
	uint64_t FullLineDiagonal(const uint64_t discs)
	{
		// 11 x AND, 11 x CMP, 11 x OR
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
	
	uint64_t FullLineCodiagonal(const uint64_t discs)
	{
		// 11 x AND, 11 x CMP, 11 x OR
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
}

uint64_t StableEdges(const uint64_t P, const uint64_t O)
{
	// 2 x AND, 2 X SHIFT, 3 x OR, 4 x PEXT, 2 X PDEP
	const uint64_t L0_Left = 0x8080808080808080ULL;
	const uint64_t L0_Right = 0x0101010101010101ULL;
	const auto StableL0_Bottom = Stability::edge_stables[P & 0xFF][O & 0xFF];
	const auto StableL0_Top = static_cast<uint64_t>(Stability::edge_stables[P >> 56][O >> 56]) << 56;
	const auto StableL0_Left  = PDep(Stability::edge_stables[PExt(P, L0_Left )][PExt(O, L0_Left )], L0_Left );
	const auto StableL0_Right = PDep(Stability::edge_stables[PExt(P, L0_Right)][PExt(O, L0_Right)], L0_Right);
	return StableL0_Bottom | StableL0_Top | StableL0_Left | StableL0_Right;
}

uint64_t StableStonesPlayer(const uint64_t P, const uint64_t O)
{
	const uint64_t discs = P | O;
	const uint64_t full_h = Stability::FullLineHorizontal(discs);
	const uint64_t full_v = Stability::FullLineVertival  (discs);
	const uint64_t full_d = Stability::FullLineDiagonal  (discs);
	const uint64_t full_c = Stability::FullLineCodiagonal(discs);
	uint64_t new_stables = StableEdges(P, O) & P;
	new_stables |= full_h & full_v & full_d & full_c & P & 0x007E7E7E7E7E7E00ULL;

	uint64_t stables = 0;
	while (new_stables & ~stables)
	{
		stables |= new_stables;
		const uint64_t stables_h = (stables >> 1) | (stables << 1) | full_h;
		const uint64_t stables_v = (stables >> 8) | (stables << 8) | full_v;
		const uint64_t stables_d = (stables >> 9) | (stables << 9) | full_d;
		const uint64_t stables_c = (stables >> 7) | (stables << 7) | full_c;
		new_stables = stables_h & stables_v & stables_d & stables_c & P & 0x007E7E7E7E7E7E00ULL;
	}
	return stables;
}



// ################################################################################################
//  CPosition
// ################################################################################################
// ------------------------------------------------------------------------------------------------
const std::string CPosition::FILENAME_EXTENSION = "pos";

std::string CPosition::to_string_1D() const { return board1D(P, O); }
std::string CPosition::to_string_2D() const { return board2D(P, O); }
std::string CPosition::to_string_2D_PM() const { return board2D(P, O, PossibleMoves()); }

void CPosition::FlipToMin()
{
	CPosition pos = *this;
	
	pos.FlipVertical();		if (pos < *this) *this = pos;
	pos.FlipHorizontal();	if (pos < *this) *this = pos;
	pos.FlipVertical();		if (pos < *this) *this = pos;
	pos.FlipDiagonal();		if (pos < *this) *this = pos;
	pos.FlipVertical();		if (pos < *this) *this = pos;
	pos.FlipHorizontal();	if (pos < *this) *this = pos;
	pos.FlipVertical();		if (pos < *this) *this = pos;
}
// ------------------------------------------------------------------------------------------------
// ################################################################################################



// ################################################################################################
//  CPositionScore
// ################################################################################################
// ------------------------------------------------------------------------------------------------
const std::string CPositionScore::FILENAME_EXTENSION = "psc";

std::string CPositionScore::to_string_1D() const { return board1D(P, O) + " " + SignedInt(score); }
std::string CPositionScore::to_string_2D() const
{
	std::string str = board2D(P, O);
	std::size_t found1 = str.find("\n");
	std::size_t found2 = str.find("\n", found1 + 1);
	str.replace(found2, 0, "  score: " + SignedInt(score));
	return str;
}
std::string CPositionScore::to_string_2D_PM() const
{
	std::string str = board2D(P, O, PossibleMoves());
	std::size_t found1 = str.find("\n");
	std::size_t found2 = str.find("\n", found1 + 1);
	str.replace(found2, 0, "  score: " + SignedInt(score));
	return str;
}
// ------------------------------------------------------------------------------------------------
// ################################################################################################



// ################################################################################################
//  CPositionFullScore
// ################################################################################################
// ------------------------------------------------------------------------------------------------
const std::string CPositionFullScore::FILENAME_EXTENSION = "pfs";

bool CPositionFullScore::Test() const 
{ 
	if ((P & O) != 0) return false;

	const uint64_t emptyCount = EmptyCount();
	for (uint64_t i = 0; i <= std::min(emptyCount, static_cast<uint64_t>(60)); i++)
		if (!(((score[i] >= -64) && (score[i] <= 64)) || (score[i] == DEFAULT_SCORE)))
			return false;
	for (uint64_t i = emptyCount + 1; i < 61; i++)
		if (score[i] != DEFAULT_SCORE)
			return false;
	return true;
}

bool CPositionFullScore::IsSolved() const
{ 
	for (uint64_t i = 0; i <= EmptyCount(); i++) 
		if (score[i] == DEFAULT_SCORE)
			return false; 
	return true; 
}

bool CPositionFullScore::IsSolved(const int8_t depth) const
{
	return MaxSolvedDepth() >= depth;
}

int8_t CPositionFullScore::MaxSolvedDepth() const
{
	int8_t ret = -1;
	for (int i = 0; i < 61; i++)
		if (score[i] != DEFAULT_SCORE)
			ret = i;
	return ret;
}

std::string CPositionFullScore::to_string_1D() const
{
	std::string ret = board1D(P, O);
	for (uint64_t i = 0; i <= EmptyCount(); i++)
		ret += " " + SignedInt(score[i]);
	return ret;
}
std::string CPositionFullScore::to_string_2D() const
{
	std::string ret = board2D(P, O) + "\nscore:";
	for (uint64_t i = 0; i <= EmptyCount(); i++)
		ret += " " + SignedInt(score[i]);
	return ret;
}
std::string CPositionFullScore::to_string_2D_PM() const
{
	std::string ret = board2D(P, O, PossibleMoves()) + "\nscore:";
	for (uint64_t i = 0; i <= EmptyCount(); i++)
		ret += " " + SignedInt(score[i]);
	return ret;
}
// ------------------------------------------------------------------------------------------------
// ################################################################################################



// ################################################################################################
//  CPositionScoreDepth
// ################################################################################################
// ------------------------------------------------------------------------------------------------
const std::string CPositionScoreDepth::FILENAME_EXTENSION = "psd";

std::string CPositionScoreDepth::GetDepthSelectivity() const
{
	return "d" + std::to_string(depth) + "@" + std::to_string(Selectivity::GetPercentile(selectivity)) + "%";
}

std::string CPositionScoreDepth::GetScoreDepthSelectivity() const
{
	if (depth == static_cast<int8_t>(EmptyCount()))
	{
		if (selectivity == NO_SELECTIVITY)
			return SignedInt(score);
		else
			return SignedInt(score) + "@" + std::to_string(Selectivity::GetPercentile(selectivity)) + "%";
	}
	else
	{
		if (selectivity == NO_SELECTIVITY)
			return SignedInt(score) + "@d" + std::to_string(depth);
		else
			return SignedInt(score) + "@d" + std::to_string(depth) + "@" + std::to_string(Selectivity::GetPercentile(selectivity)) + "%";
	}
}
std::string CPositionScoreDepth::to_string_1D() const 
{
	return board1D(P, O) + " " + GetScoreDepthSelectivity();
}
std::string CPositionScoreDepth::to_string_2D() const
{
	std::string str = board2D(P, O);
	std::size_t found1 = str.find("\n");
	std::size_t found2 = str.find("\n", found1 + 1);
	str.replace(found2, 0, "  score: " + GetScoreDepthSelectivity());
	return str;
}
std::string CPositionScoreDepth::to_string_2D_PM() const
{
	std::string str = board2D(P, O, PossibleMoves());
	std::size_t found1 = str.find("\n");
	std::size_t found2 = str.find("\n", found1 + 1);
	str.replace(found2, 0, "  score: " + GetScoreDepthSelectivity());
	return str;
}
// ------------------------------------------------------------------------------------------------
// ################################################################################################



// ################################################################################################
//  CPositionAllScore
// ################################################################################################
// ------------------------------------------------------------------------------------------------
const std::string CPositionAllScore::FILENAME_EXTENSION = "pas";

CPositionAllScore::CPositionAllScore(std::string s) : CPositionAllScore()
{
	// ROOM FOR OPTIMIZATION: make argument 'const std::string& s' and replace s.erase(...)
	assert(P == 0);
	assert(O == 0);
	
	// Read in position
	for (unsigned int i = 0; i < 64; i++)
	{
		     if (s.substr(i, 1) == "X") SetBit(P, 63 - i);
		else if (s.substr(i, 1) == "O") SetBit(O, 63 - i);
	}
	if (s.substr(65, 1) == "O") std::swap(P, O);
	
	// parse scores
	std::string delimiter = ";";
	std::string token;
	size_t pos = 0;
	while ((pos = s.find(delimiter)) != std::string::npos)
	{
		token = s.substr(0, pos);
		s.erase(0, pos + delimiter.length());

		for (int i = 0; i < 64; i++) // ROOM FOR OPTIMIZATION: interpret field and avoid looping
			if (token.substr(1, 2) == field_name(i))
				score[i] = std::stoi(token.substr(4, 3));
	}
}

bool CPositionAllScore::Test() const
{
	if ((P & O) != 0) return false;

	const int possibleMoves = PossibleMoves();
	for (int i = 0; i <= 64; i++)
	{
		if (GetBit(possibleMoves, i))
		{
			if (!(((score[i] >= -64) && (score[i] <= 64)) || (score[i] == DEFAULT_SCORE)))
				return false;
		}
		else
		{
			if (score[i] != DEFAULT_SCORE)
				return false;
		}
	}
	return true;
}

std::vector<std::pair<int,int>> CPositionAllScore::GetMoves() const
{
	std::vector<std::pair<int,int>> vec; // move, score
	uint64_t pm = PossibleMoves();
	while (pm)
	{
		uint64_t LSB = BitScanLSB(pm);
		if (score[LSB] != DEFAULT_SCORE)
			vec.push_back(std::pair<int,int>(LSB, score[LSB]));
		RemoveLSB(pm);
	}
	std::sort(vec.begin(), vec.end(), [](const std::pair<int,int>& lhs, const std::pair<int,int>& rhs){ return lhs.second > rhs.second; });
	return vec;
}
std::string CPositionAllScore::to_string_1D() const
{
	std::string ret = board1D(P, O) + " X;";
	for (const auto& it : GetMoves())
		ret += std::string(" ") + field_name(it.first) + ":" + SignedInt(it.second) + ";";
	return ret;
}
std::string CPositionAllScore::to_string_2D() const
{
	std::string ret = board2D(P, O) + "\n";
	for (const auto& it : GetMoves())
		ret += std::string(" ") + field_name(it.first) + ":" + SignedInt(it.second) + ";";
	return ret;
}
std::string CPositionAllScore::to_string_2D_PM() const
{
	std::string ret = board2D(P, O, PossibleMoves()) + "\n";
	for (const auto& it : GetMoves())
		ret += std::string(" ") + field_name(it.first) + ":" + SignedInt(it.second) + ";";
	return ret;
}
// ------------------------------------------------------------------------------------------------
// ################################################################################################


bool HasValidFilenameExtension(const std::string& filename)
{
	std::string filename_extension = filename.substr(filename.rfind(".") + 1);

	if (filename_extension == CPosition::FILENAME_EXTENSION)				return true;
	if (filename_extension == CPositionScore::FILENAME_EXTENSION)			return true;
	if (filename_extension == CPositionFullScore::FILENAME_EXTENSION)		return true;
	if (filename_extension == CPositionScoreDepth::FILENAME_EXTENSION)		return true;
	if (filename_extension == CPositionAllScore::FILENAME_EXTENSION)		return true;
	if (filename_extension == "obf")										return true;
	return false;
}

std::vector<CPositionAllScore> read_vector_OBF(const std::string& filename, std::size_t size)
{
	std::fstream file(filename, std::ios::in | std::ios::binary);
	if (!file.is_open())
		throw std::iostream::failure("File '" + filename + "' could not be opened.");

	std::vector<CPositionAllScore> vec;
	std::string line;
	while (std::getline(file, line))
		vec.push_back(CPositionAllScore(line));

	file.close();
	return vec;
}

void write_to_file_OBF(const std::string& filename, const std::vector<CPositionAllScore>& vec)
{
	std::fstream file(filename, std::ios::out | std::ios::binary);
	if (!file.is_open())
		throw std::iostream::failure("File '" + filename + "' could not be opened.");

	for (const auto& it : vec)
		file << it.to_string_1D();

	file.close();
}

template <> std::vector<CPositionAllScore> LoadVector(const std::string& filename, std::size_t size)
{
	std::string filename_extension = filename.substr(filename.rfind(".") + 1);
	if (filename_extension == "obf")
		return read_vector_OBF(filename, size);
	if (filename_extension != CPositionAllScore::FILENAME_EXTENSION)
		std::cerr << "WARNING: Filename extension '" << filename_extension << "' does not match data type when loading." << std::endl;
	return read_vector<CPositionAllScore>(filename, size);
}

template <> void SaveVector<CPositionAllScore>(const std::string& filename, const std::vector<CPositionAllScore>& vec)
{
	std::string filename_extension = filename.substr(filename.rfind(".") + 1);
	if (filename_extension == "obf")
		write_to_file_OBF(filename, vec);
	else
	{
		if (filename_extension != CPositionAllScore::FILENAME_EXTENSION)
			std::cerr << "WARNING: Filename extension '" << filename_extension << "' does not match data type when saving." << std::endl;
		write_to_file(filename, vec);
	}
}
