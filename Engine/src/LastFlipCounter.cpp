#include "LastFlipCounter.h"
#include "MacrosHell.h"
#include "FlipFast.h"

CLastFlipCounter::CLastFlipCounter()
{
	for (int i = 0; i < 256; i++) CLF_0[i] = 2 * CountFlip(i, A1);
	for (int i = 0; i < 256; i++) CLF_1[i] = 2 * CountFlip(i, B1);
	for (int i = 0; i < 256; i++) CLF_2[i] = 2 * CountFlip(i, C1);
	for (int i = 0; i < 256; i++) CLF_3[i] = 2 * CountFlip(i, D1);
	for (int i = 0; i < 256; i++) CLF_4[i] = 2 * CountFlip(i, E1);
	for (int i = 0; i < 256; i++) CLF_5[i] = 2 * CountFlip(i, F1);
	for (int i = 0; i < 256; i++) CLF_6[i] = 2 * CountFlip(i, G1);
	for (int i = 0; i < 256; i++) CLF_7[i] = 2 * CountFlip(i, H1);
}

uint8_t CLastFlipCounter::CountFlip(const uint8_t P, const CMove& move)
{
	if (P & MakeBit(move.field))
		return 0;
	const uint8_t O = P ^ 0xFF ^ MakeBit(move.field);
	const auto flips = Flip(CPosition(P, O), move);
	return static_cast<uint8_t>(PopCount(flips));
}

uint64_t CLastFlipCounter::Flip(const CPosition& pos, const CMove& move)
{
	return Flip_dir(pos, move, -1) | Flip_dir(pos, move, +1);
}

uint64_t CLastFlipCounter::Flip_dir(const CPosition& pos, const CMove& move, const int dX)
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

uint8_t CLastFlipCounter::CountLastFlip(const CPosition& pos, const CMove& move) const
{
	const auto P = pos.GetP();
	switch (move.field)
	{
		case A1: return CLF_0[P & 0xFFULL] + CLF_0[PExt(P, 0x0101010101010101ULL)] + CLF_0[PExt(P, 0x8040201008040201ULL)];
		case B1: return CLF_1[P & 0xFFULL] + CLF_0[PExt(P, 0x0202020202020202ULL)] + CLF_0[PExt(P, 0x0080402010080402ULL)];
		case C1: return CLF_2[P & 0xFFULL] + CLF_0[PExt(P, 0x0404040404040404ULL)] + CLF_2[((P & 0x0000804020110A04ULL) * 0x0101010101010101ULL) >> 56];
		case D1: return CLF_3[P & 0xFFULL] + CLF_0[PExt(P, 0x0808080808080808ULL)] + CLF_3[((P & 0x0000008041221408ULL) * 0x0101010101010101ULL) >> 56];
		case E1: return CLF_4[P & 0xFFULL] + CLF_0[PExt(P, 0x1010101010101010ULL)] + CLF_4[((P & 0x0000000182442810ULL) * 0x0101010101010101ULL) >> 56];
		case F1: return CLF_5[P & 0xFFULL] + CLF_0[PExt(P, 0x2020202020202020ULL)] + CLF_5[((P & 0x0000010204885020ULL) * 0x0101010101010101ULL) >> 56];
		case G1: return CLF_6[P & 0xFFULL] + CLF_0[PExt(P, 0x4040404040404040ULL)] + CLF_0[PExt(P, 0x0001020408102040ULL)];
		case H1: return CLF_7[P & 0xFFULL] + CLF_0[PExt(P, 0x8080808080808080ULL)] + CLF_0[PExt(P, 0x0102040810204080ULL)];

		case A2: return CLF_0[BExtr(P, 8, 8)] + CLF_1[PExt(P, 0x0101010101010101ULL)] + CLF_0[PExt(P, 0x4020100804020100ULL)];
		case B2: return CLF_1[BExtr(P, 8, 8)] + CLF_1[PExt(P, 0x0202020202020202ULL)] + CLF_0[PExt(P, 0x8040201008040200ULL)];
		case C2: return CLF_2[BExtr(P, 8, 8)] + CLF_1[PExt(P, 0x0404040404040404ULL)] + CLF_2[((P & 0x00804020110A0400ULL) * 0x0101010101010101ULL) >> 56];
		case D2: return CLF_3[BExtr(P, 8, 8)] + CLF_1[PExt(P, 0x0808080808080808ULL)] + CLF_3[((P & 0x0000804122140800ULL) * 0x0101010101010101ULL) >> 56];
		case E2: return CLF_4[BExtr(P, 8, 8)] + CLF_1[PExt(P, 0x1010101010101010ULL)] + CLF_4[((P & 0x0000018244281000ULL) * 0x0101010101010101ULL) >> 56];
		case F2: return CLF_5[BExtr(P, 8, 8)] + CLF_1[PExt(P, 0x2020202020202020ULL)] + CLF_5[((P & 0x0001020488502000ULL) * 0x0101010101010101ULL) >> 56];
		case G2: return CLF_6[BExtr(P, 8, 8)] + CLF_1[PExt(P, 0x4040404040404040ULL)] + CLF_0[PExt(P, 0x0102040810204000ULL)];
		case H2: return CLF_7[BExtr(P, 8, 8)] + CLF_1[PExt(P, 0x8080808080808080ULL)] + CLF_0[PExt(P, 0x0204081020408000ULL)];

		case A3: return CLF_0[BExtr(P, 16, 8)] + CLF_2[PExt(P, 0x0101010101010101ULL)] + CLF_2[PExt(P, 0x2010080402010204ULL)];
		case B3: return CLF_1[BExtr(P, 16, 8)] + CLF_2[PExt(P, 0x0202020202020202ULL)] + CLF_2[PExt(P, 0x4020100804020408ULL)];
		case C3: return CLF_2[BExtr(P, 16, 8)] + CLF_2[PExt(P, 0x0404040404040404ULL)] + CLF_2[PExt(P, 0x8040201008040201ULL)] + CLF_2[PExt(P, 0x0000000102040810ULL)];
		case D3: return CLF_3[BExtr(P, 16, 8)] + CLF_2[PExt(P, 0x0808080808080808ULL)] + CLF_2[PExt(P, 0x0080402010080402ULL)] + CLF_2[PExt(P, 0x0000010204081020ULL)];
		case E3: return CLF_4[BExtr(P, 16, 8)] + CLF_2[PExt(P, 0x1010101010101010ULL)] + CLF_2[PExt(P, 0x0000804020100804ULL)] + CLF_2[PExt(P, 0x0001020408102040ULL)];
		case F3: return CLF_5[BExtr(P, 16, 8)] + CLF_2[PExt(P, 0x2020202020202020ULL)] + CLF_2[PExt(P, 0x0000008040201008ULL)] + CLF_2[PExt(P, 0x0102040810204080ULL)];
		case G3: return CLF_6[BExtr(P, 16, 8)] + CLF_2[PExt(P, 0x4040404040404040ULL)] + CLF_2[PExt(P, 0x0204081020402010ULL)];
		case H3: return CLF_7[BExtr(P, 16, 8)] + CLF_2[PExt(P, 0x8080808080808080ULL)] + CLF_2[PExt(P, 0x0408102040804020ULL)];

		case A4: return CLF_0[BExtr(P, 24, 8)] + CLF_3[PExt(P, 0x0101010101010101ULL)] + CLF_3[PExt(P, 0x1008040201020408ULL)];
		case B4: return CLF_1[BExtr(P, 24, 8)] + CLF_3[PExt(P, 0x0202020202020202ULL)] + CLF_3[PExt(P, 0x2010080402040810ULL)];
		case C4: return CLF_2[BExtr(P, 24, 8)] + CLF_3[PExt(P, 0x0404040404040404ULL)] + CLF_2[PExt(P, 0x4020100804020100ULL)] + CLF_3[PExt(P, 0x0000010204081020ULL)];
		case D4: return CLF_3[BExtr(P, 24, 8)] + CLF_3[PExt(P, 0x0808080808080808ULL)] + CLF_3[PExt(P, 0x8040201008040201ULL)] + CLF_3[PExt(P, 0x0001020408102040ULL)];
		case E4: return CLF_4[BExtr(P, 24, 8)] + CLF_3[PExt(P, 0x1010101010101010ULL)] + CLF_3[PExt(P, 0x0080402010080402ULL)] + CLF_3[PExt(P, 0x0102040810204080ULL)];
		case F4: return CLF_5[BExtr(P, 24, 8)] + CLF_3[PExt(P, 0x2020202020202020ULL)] + CLF_3[PExt(P, 0x0000804020100804ULL)] + CLF_2[PExt(P, 0x0204081020408000ULL)];
		case G4: return CLF_6[BExtr(P, 24, 8)] + CLF_3[PExt(P, 0x4040404040404040ULL)] + CLF_3[PExt(P, 0x0408102040201008ULL)];
		case H4: return CLF_7[BExtr(P, 24, 8)] + CLF_3[PExt(P, 0x8080808080808080ULL)] + CLF_3[PExt(P, 0x0810204080402010ULL)];

		case A5: return CLF_0[BExtr(P, 32, 8)] + CLF_4[PExt(P, 0x0101010101010101ULL)] + CLF_4[PExt(P, 0x0804020102040810ULL)];
		case B5: return CLF_1[BExtr(P, 32, 8)] + CLF_4[PExt(P, 0x0202020202020202ULL)] + CLF_4[PExt(P, 0x1008040204081020ULL)];
		case C5: return CLF_2[BExtr(P, 32, 8)] + CLF_4[PExt(P, 0x0404040404040404ULL)] + CLF_2[PExt(P, 0x2010080402010000ULL)] + CLF_4[PExt(P, 0x0001020408102040ULL)];
		case D5: return CLF_3[BExtr(P, 32, 8)] + CLF_4[PExt(P, 0x0808080808080808ULL)] + CLF_3[PExt(P, 0x4020100804020100ULL)] + CLF_4[PExt(P, 0x0102040810204080ULL)];
		case E5: return CLF_4[BExtr(P, 32, 8)] + CLF_4[PExt(P, 0x1010101010101010ULL)] + CLF_4[PExt(P, 0x8040201008040201ULL)] + CLF_3[PExt(P, 0x0204081020408000ULL)];
		case F5: return CLF_5[BExtr(P, 32, 8)] + CLF_4[PExt(P, 0x2020202020202020ULL)] + CLF_4[PExt(P, 0x0080402010080402ULL)] + CLF_2[PExt(P, 0x0408102040800000ULL)];
		case G5: return CLF_6[BExtr(P, 32, 8)] + CLF_4[PExt(P, 0x4040404040404040ULL)] + CLF_4[PExt(P, 0x0810204020100804ULL)];
		case H5: return CLF_7[BExtr(P, 32, 8)] + CLF_4[PExt(P, 0x8080808080808080ULL)] + CLF_4[PExt(P, 0x1020408040201008ULL)];

		case A6: return CLF_0[BExtr(P, 40, 8)] + CLF_5[PExt(P, 0x0101010101010101ULL)] + CLF_5[PExt(P, 0x0402010204081020ULL)];
		case B6: return CLF_1[BExtr(P, 40, 8)] + CLF_5[PExt(P, 0x0202020202020202ULL)] + CLF_5[PExt(P, 0x0804020408102040ULL)];
		case C6: return CLF_2[BExtr(P, 40, 8)] + CLF_5[PExt(P, 0x0404040404040404ULL)] + CLF_2[PExt(P, 0x1008040201000000ULL)] + CLF_5[PExt(P, 0x0102040810204080ULL)];
		case D6: return CLF_3[BExtr(P, 40, 8)] + CLF_5[PExt(P, 0x0808080808080808ULL)] + CLF_3[PExt(P, 0x2010080402010000ULL)] + CLF_4[PExt(P, 0x0204081020408000ULL)];
		case E6: return CLF_4[BExtr(P, 40, 8)] + CLF_5[PExt(P, 0x1010101010101010ULL)] + CLF_4[PExt(P, 0x4020100804020100ULL)] + CLF_3[PExt(P, 0x0408102040800000ULL)];
		case F6: return CLF_5[BExtr(P, 40, 8)] + CLF_5[PExt(P, 0x2020202020202020ULL)] + CLF_5[PExt(P, 0x8040201008040201ULL)] + CLF_2[PExt(P, 0x0810204080000000ULL)];
		case G6: return CLF_6[BExtr(P, 40, 8)] + CLF_5[PExt(P, 0x4040404040404040ULL)] + CLF_5[PExt(P, 0x1020402010080402ULL)];
		case H6: return CLF_7[BExtr(P, 40, 8)] + CLF_5[PExt(P, 0x8080808080808080ULL)] + CLF_5[PExt(P, 0x2040804020100804ULL)];

		case A7: return CLF_0[BExtr(P, 48, 8)] + CLF_6[PExt(P, 0x0101010101010101ULL)] + CLF_6[PExt(P, 0x0001020408102040ULL)];
		case B7: return CLF_1[BExtr(P, 48, 8)] + CLF_6[PExt(P, 0x0202020202020202ULL)] + CLF_6[PExt(P, 0x0002040810204080ULL)];
		case C7: return CLF_2[BExtr(P, 48, 8)] + CLF_6[PExt(P, 0x0404040404040404ULL)] + CLF_2[((P & 0x00040A1120408000ULL) * 0x0101010101010101ULL) >> 56];
		case D7: return CLF_3[BExtr(P, 48, 8)] + CLF_6[PExt(P, 0x0808080808080808ULL)] + CLF_3[((P & 0x0008142241800000ULL) * 0x0101010101010101ULL) >> 56];
		case E7: return CLF_4[BExtr(P, 48, 8)] + CLF_6[PExt(P, 0x1010101010101010ULL)] + CLF_4[((P & 0x0010284482010000ULL) * 0x0101010101010101ULL) >> 56];
		case F7: return CLF_5[BExtr(P, 48, 8)] + CLF_6[PExt(P, 0x2020202020202020ULL)] + CLF_5[((P & 0x0020508804020100ULL) * 0x0101010101010101ULL) >> 56];
		case G7: return CLF_6[BExtr(P, 48, 8)] + CLF_6[PExt(P, 0x4040404040404040ULL)] + CLF_6[PExt(P, 0x0040201008040201ULL)];
		case H7: return CLF_7[BExtr(P, 48, 8)] + CLF_6[PExt(P, 0x8080808080808080ULL)] + CLF_6[PExt(P, 0x0080402010080402ULL)];

		case A8: return CLF_0[P >> 56] + CLF_7[PExt(P, 0x0101010101010101ULL)] + CLF_7[PExt(P, 0x0102040810204080ULL)];
		case B8: return CLF_1[P >> 56] + CLF_7[PExt(P, 0x0202020202020202ULL)] + CLF_6[PExt(P, 0x0204081020408000ULL)];
		case C8: return CLF_2[P >> 56] + CLF_7[PExt(P, 0x0404040404040404ULL)] + CLF_2[((P & 0x040A112040800000ULL) * 0x0101010101010101ULL) >> 56];
		case D8: return CLF_3[P >> 56] + CLF_7[PExt(P, 0x0808080808080808ULL)] + CLF_3[((P & 0x0814224180000000ULL) * 0x0101010101010101ULL) >> 56];
		case E8: return CLF_4[P >> 56] + CLF_7[PExt(P, 0x1010101010101010ULL)] + CLF_4[((P & 0x1028448201000000ULL) * 0x0101010101010101ULL) >> 56];
		case F8: return CLF_5[P >> 56] + CLF_7[PExt(P, 0x2020202020202020ULL)] + CLF_5[((P & 0x2050880402010000ULL) * 0x0101010101010101ULL) >> 56];
		case G8: return CLF_6[P >> 56] + CLF_7[PExt(P, 0x4040404040404040ULL)] + CLF_6[PExt(P, 0x4020100804020100ULL)];
		case H8: return CLF_7[P >> 56] + CLF_7[PExt(P, 0x8080808080808080ULL)] + CLF_7[PExt(P, 0x8040201008040201ULL)];

		default: return 0;
	}
}