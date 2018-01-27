#include "position.h"

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
	{	// 4 x AND, 3 x SHIFT, 1 x MUL
		// 8 OPs
		discs &= discs >> 4;
		discs &= discs >> 2;
		discs &= discs >> 1;
		discs &= 0x0001010101010100ULL;
		return discs * 0xFFULL;
	}
	
	uint64_t FullLineVertival(uint64_t discs)
	{	// 4 x AND, 3 x SHIFT, 1 x MUL
		// 8 OPs
		discs &= discs >> 32;
		discs &= discs >> 16;
		discs &= discs >>  8;
		discs &= 0x7EULL;
		return discs * 0x0101010101010101ULL;
	}
	
	uint64_t FullLineDiagonal(const uint64_t discs)
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
	
	uint64_t FullLineCodiagonal(const uint64_t discs)
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
}

uint64_t StableEdges(const uint64_t P, const uint64_t O)
{	// 2 x AND, 2 X SHIFT, 3 x OR, 4 x PEXT, 2 X PDEP
	// 13 OPs
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

bool CPositionFullScore::Test() const 
{ 
	if (CPosition::Test() == false)
		return false;

	const auto emptyCount = EmptyCount();
	const uint64_t size = std::distance(std::begin(score), std::end(score));
	const auto UpperLimit = std::min(emptyCount, size-1);
	for (uint64_t i = 0; i <= UpperLimit; i++)
		if (!(((score[i] >= -64) && (score[i] <= 64)) || (score[i] == DEFAULT_SCORE)))
			return false;
	for (uint64_t i = emptyCount + 1; i < size; i++)
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

template <typename T>
struct reversion_wrapper { T& iterable; };

template <typename T>
auto begin(reversion_wrapper<T> w) { return std::rbegin(w.iterable); }

template <typename T>
auto end(reversion_wrapper<T> w) { return std::rend(w.iterable); }

template <typename T>
reversion_wrapper<T> reverse(T&& iterable) { return { iterable }; }

int8_t CPositionFullScore::MaxSolvedDepth() const
{
	for (const auto& it : reverse(score))
		if (it != DEFAULT_SCORE)
			return std::distance(std::begin(score), &it);
	return -1;
}

bool CPositionAllScore::Test() const
{
	if (CPosition::Test() == false)
		return false;

	const int possibleMoves = PossibleMoves();
	for (const auto& it : score)
	{
		if (TestBit(possibleMoves, std::distance(std::begin(score), &it)))
		{
			if (!(((it >= -64) && (it <= 64)) || (it == DEFAULT_SCORE)))
				return false;
		}
		else
		{
			if (it != DEFAULT_SCORE)
				return false;
		}
	}
	return true;
}

bool CPositionAllScore::IsSolved() const
{
	uint64_t pm = PossibleMoves();
	while (pm)
	{
		uint64_t LSB = BitScanLSB(pm);
		if (score[LSB] == DEFAULT_SCORE)
			return false;
		RemoveLSB(pm); 
	}
	return true;
}

std::vector<std::pair<int,int>> CPositionAllScore::GetMoves() const
{
	std::vector<std::pair<int,int>> vec; // move, score
	uint64_t pm = PossibleMoves();
	while (pm)
	{
		auto LSB = BitScanLSB(pm);
		if (score[LSB] != DEFAULT_SCORE)
			vec.push_back(std::pair<int,int>(LSB, score[LSB]));
		RemoveLSB(pm);
	}
	std::sort(vec.begin(), vec.end(), [](const std::pair<int,int>& lhs, const std::pair<int,int>& rhs){ return lhs.second > rhs.second; });
	return vec;
}

std::vector<std::unique_ptr<CPosition>> LoadVector(const CPath& filename, std::size_t size)
{
	std::unique_ptr<iPositionArchive> archive;
	StreamArchive& fstream = fstreamArchive(filename);
	const auto ext = filename.GetExtension();

	     if (ext == "pos")		archive = std::make_unique<StreamConverter>(fstream);
	else if (ext == "script")	archive = std::make_unique<SingleLineStreamDecorator>(fstream);
	else if (ext == "full")		archive = std::make_unique<SingleLineStreamDecorator>(fstream);
	else if (ext == "obf")		archive = std::make_unique<SingleLineStreamDecorator>(fstream);
	else
		return std::vector<std::unique_ptr<CPosition>>();

	std::vector<std::unique_ptr<CPosition>> positions;
	std::unique_ptr<CPosition> pos;
	while ((pos = archive->DeserializePosition()) && (size > 0)) {
		positions.push_back(std::move(pos));
		size--;
	}

	return positions;
}

void Save(const CPath& filename, const std::vector<std::unique_ptr<CPosition>>& positions)
{
	std::unique_ptr<oStreamArchive> archive;
	const auto ext = filename.GetExtension();

	if (ext == "pos") archive = std::make_unique<fstreamArchive>(filename);
	else return;

	for (auto& it : positions)
		*archive << *it;
}