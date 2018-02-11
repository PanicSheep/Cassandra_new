#include "FlipFast.h"

#pragma warning(push)
#pragma warning(disable:4146) // unary minus operator applied to unsigned type, result still unsigned.

CFlipper Flipper;

uint64_t CFlipper::Flip(const CPosition& pos, const CMove move) const
{
	return (this->*flip_fp[move.field])(pos.GetP(), pos.GetO());
}

uint64_t CFlipper::A1(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetLSB(~O & 0x0101010101010100ULL) & P;
	const auto flipped_v = (outflank_v - (outflank_v != 0)) & 0x0101010101010100ULL;

	const auto outflank_h = ((O & 0x7E) + 0x02) & P;
	const auto flipped_h = (outflank_h - (outflank_h != 0)) & 0x7E;

	const auto outflank_d = GetLSB(~O & 0x8040201008040200ULL) & P;
	const auto flipped_d = (outflank_d - (outflank_d != 0)) & 0x8040201008040200ULL;

	return flipped_v | flipped_h | flipped_d;
}

uint64_t CFlipper::B1(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetLSB(~O & 0x0202020202020200ULL) & P;
	const auto flipped_v = (outflank_v - (outflank_v != 0)) & 0x0202020202020200ULL;

	const auto outflank_h = ((O & 0x7C) + 0x04) & P;
	const auto flipped_h = (outflank_h - (outflank_h != 0)) & 0x7C;

	const auto outflank_d = GetLSB(~O & 0x0080402010080400ULL) & P;
	const auto flipped_d = (outflank_d - (outflank_d != 0)) & 0x0080402010080400ULL;

	return flipped_v | flipped_h | flipped_d;
}

uint64_t CFlipper::C1(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetLSB(~O & 0x0404040404040400ULL) & P;
	const auto flipped_v = (outflank_v - (outflank_v != 0)) & 0x0404040404040400ULL;

	const auto outflank_h = OUTFLANK_2[BExtr(O, 1, 6)] & P;
	const auto  flipped_h = FLIPPED_2_H[outflank_h] & 0xFFULL;

	const auto flipped_c = (P >> 7) & 0x0000000000000200ULL & O;

	const auto outflank_d = GetLSB(~O & 0x0000804020100800ULL) & P;
	const auto flipped_d = (outflank_d - (outflank_d != 0)) & 0x0000804020100800ULL;

	return flipped_v | flipped_h | flipped_c | flipped_d;
}

uint64_t CFlipper::D1(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetLSB(~O & 0x0808080808080800ULL) & P;
	const auto flipped_v = (outflank_v - (outflank_v != 0)) & 0x0808080808080800ULL;

	const auto outflank_h = OUTFLANK_3[BExtr(O, 1, 6)] & P;
	const auto  flipped_h = FLIPPED_3_H[outflank_h] & 0xFFULL;

	const auto outflank_c = GetLSB(~O & 0x0000000001020400ULL) & P;
	const auto flipped_c = (outflank_c - (outflank_c != 0)) & 0x0000000001020400ULL;

	const auto outflank_d = GetLSB(~O & 0x0000008040201000ULL) & P;
	const auto flipped_d = (outflank_d - (outflank_d != 0)) & 0x0000008040201000ULL;

	return flipped_v | flipped_h | flipped_c | flipped_d;
}

uint64_t CFlipper::E1(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetLSB(~O & 0x1010101010101000ULL) & P;
	const auto flipped_v = (outflank_v - (outflank_v != 0)) & 0x1010101010101000ULL;

	const auto outflank_h = OUTFLANK_4[BExtr(O, 1, 6)] & P;
	const auto  flipped_h = FLIPPED_4_H[outflank_h] & 0xFFULL;

	const auto outflank_c = GetLSB(~O & 0x0000000102040800ULL) & P;
	const auto flipped_c = (outflank_c - (outflank_c != 0)) & 0x0000000102040800ULL;

	const auto outflank_d = GetLSB(~O & 0x0000000080402000ULL) & P;
	const auto flipped_d = (outflank_d - (outflank_d != 0)) & 0x0000000080402000ULL;

	return flipped_v | flipped_h | flipped_c | flipped_d;
}

uint64_t CFlipper::F1(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetLSB(~O & 0x2020202020202000ULL) & P;
	const auto flipped_v = (outflank_v - (outflank_v != 0)) & 0x2020202020202000ULL;

	const auto outflank_h = OUTFLANK_5[BExtr(O, 1, 6)] & P;
	const auto  flipped_h = FLIPPED_5_H[outflank_h] & 0xFFULL;

	const auto outflank_c = GetLSB(~O & 0x0000010204081000ULL) & P;
	const auto flipped_c = (outflank_c - (outflank_c != 0)) & 0x0000010204081000ULL;

	const auto flipped_d = ((P >> 9) & 0x0000000000004000ULL & O);

	return flipped_v | flipped_h | flipped_c | flipped_d;
}

uint64_t CFlipper::G1(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetLSB(~O & 0x4040404040404000ULL) & P;
	const auto flipped_v = (outflank_v - (outflank_v != 0)) & 0x4040404040404000ULL;

	const auto outflank_h = OUTFLANK_7[O & 0x3E] & (P << 1);
	const auto flipped_h = (-outflank_h & 0x3E);

	const auto outflank_c = GetLSB(~O & 0x0001020408102000ULL) & P;
	const auto flipped_c = (outflank_c - (outflank_c != 0)) & 0x0001020408102000ULL;

	return flipped_v | flipped_h | flipped_c;
}

uint64_t CFlipper::H1(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetLSB(~O & 0x8080808080808000ULL) & P;
	const auto flipped_v = (outflank_v - (outflank_v != 0)) & 0x8080808080808000ULL;

	const auto outflank_h = OUTFLANK_7[BExtr(O, 1, 6)] & P;
	const auto flipped_h = (-outflank_h & 0x3F) << 1;

	const auto outflank_c = GetLSB(~O & 0x0102040810204000ULL) & P;
	const auto flipped_c = (outflank_c - (outflank_c != 0)) & 0x0102040810204000ULL;

	return flipped_v | flipped_h | flipped_c;
}

uint64_t CFlipper::A2(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = ((O | ~0x0101010101010000ULL) + 0x0000000000010000ULL) & P & 0x0101010101010000ULL;
	const auto flipped_v = (outflank_v - (outflank_v != 0)) & 0x0101010101010000ULL;

	const auto outflank_h = ((O & 0x0000000000007E00ULL) + 0x0000000000000200ULL) & P;
	const auto flipped_h = (outflank_h - (outflank_h >> 8)) & 0x0000000000007E00ULL;

	const auto outflank_d = ((O | ~0x4020100804020000ULL) + 0x0000000000020000ULL) & P & 0x4020100804020000ULL;
	const auto flipped_d = (outflank_d - (outflank_d != 0)) & 0x4020100804020000ULL;

	return flipped_v | flipped_h | flipped_d;
}

uint64_t CFlipper::B2(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = ((O | ~0x0202020202020000ULL) + 0x0000000000020000ULL) & P & 0x0202020202020000ULL;
	const auto flipped_v = (outflank_v - (outflank_v != 0)) & 0x0202020202020000ULL;

	const auto outflank_h = ((O & 0x0000000000007C00ULL) + 0x0000000000000400ULL) & P;
	const auto flipped_h = (outflank_h - (outflank_h >> 8)) & 0x0000000000007C00ULL;

	const auto outflank_d = ((O | ~0x8040201008040000ULL) + 0x0000000000040000ULL) & P & 0x8040201008040000ULL;
	const auto flipped_d = (outflank_d - (outflank_d != 0)) & 0x8040201008040000ULL;

	return flipped_v | flipped_h | flipped_d;
}

uint64_t CFlipper::C2(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetLSB(~O & 0x0404040404040000ULL) & P;
	const auto flipped_v = (outflank_v - (outflank_v != 0)) & 0x0404040404040000ULL;

	const auto outflank_h = OUTFLANK_2[BExtr(O, 9, 6)] & (P >> 8);
	const auto flipped_h = FLIPPED_2_H[outflank_h] & 0x000000000000FF00ULL;

	const auto flipped_c = ((P >> 7) & 0x0000000000020000ULL & O);

	const auto outflank_d = GetLSB(~O & 0x0080402010080000ULL) & P;
	const auto flipped_d = (outflank_d - (outflank_d != 0)) & 0x0080402010080000ULL;

	return flipped_v | flipped_h | flipped_c | flipped_d;
}

uint64_t CFlipper::D2(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetLSB(~O & 0x0808080808080000ULL) & P;
	const auto flipped_v = (outflank_v - (outflank_v != 0)) & 0x0808080808080000ULL;

	const auto outflank_h = OUTFLANK_3[BExtr(O, 9, 6)] & (P >> 8);
	const auto flipped_h = FLIPPED_3_H[outflank_h] & 0x000000000000FF00ULL;

	const auto outflank_c = GetLSB(~O & 0x0000000102040000ULL) & P;
	const auto flipped_c = (outflank_c - (outflank_c != 0)) & 0x0000000102040000ULL;

	const auto outflank_d = GetLSB(~O & 0x0000804020100000ULL) & P;
	const auto flipped_d = (outflank_d - (outflank_d != 0)) & 0x0000804020100000ULL;

	return flipped_v | flipped_h | flipped_c | flipped_d;
}

uint64_t CFlipper::E2(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetLSB(~O & 0x1010101010100000ULL) & P;
	const auto flipped_v = (outflank_v - (outflank_v != 0)) & 0x1010101010100000ULL;

	const auto outflank_h = OUTFLANK_4[BExtr(O, 9, 6)] & (P >> 8);
	const auto flipped_h = FLIPPED_4_H[outflank_h] & 0x000000000000FF00ULL;

	const auto outflank_c = GetLSB(~O & 0x0000010204080000ULL) & P;
	const auto flipped_c = (outflank_c - (outflank_c != 0)) & 0x0000010204080000ULL;

	const auto outflank_d = GetLSB(~O & 0x0000008040200000ULL) & P;
	const auto flipped_d = (outflank_d - (outflank_d != 0)) & 0x0000008040200000ULL;

	return flipped_v | flipped_h | flipped_c | flipped_d;
}

uint64_t CFlipper::F2(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetLSB(~O & 0x2020202020200000ULL) & P;
	const auto flipped_v = (outflank_v - (outflank_v != 0)) & 0x2020202020200000ULL;

	const auto outflank_h = OUTFLANK_5[BExtr(O, 9, 6)] & (P >> 8);
	const auto flipped_h = FLIPPED_5_H[outflank_h] & 0x000000000000FF00ULL;

	const auto outflank_c = GetLSB(~O & 0x0001020408100000ULL) & P;
	const auto flipped_c = (outflank_c - (outflank_c != 0)) & 0x0001020408100000ULL;

	const auto flipped_d = ((P >> 9) & 0x0000000000400000ULL & O);

	return flipped_v | flipped_h | flipped_c | flipped_d;
}

uint64_t CFlipper::G2(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetLSB(~O & 0x4040404040400000ULL) & P;
	const auto flipped_v = (outflank_v - (outflank_v != 0)) & 0x4040404040400000ULL;

	const auto outflank_h = OUTFLANK_7[(O >> 8) & 0x3E] & (P >> 7);
	const auto flipped_h = (-outflank_h & 0x3E) << 8;

	const auto outflank_c = GetLSB(~O & 0x0102040810200000ULL) & P;
	const auto flipped_c = (outflank_c - (outflank_c != 0)) & 0x0102040810200000ULL;

	return flipped_v | flipped_h | flipped_c;
}

uint64_t CFlipper::H2(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetLSB(~O & 0x8080808080800000ULL) & P;
	const auto flipped_v = (outflank_v - (outflank_v != 0)) & 0x8080808080800000ULL;

	const auto outflank_h = OUTFLANK_7[BExtr(O, 9, 6)] & (P >> 8);
	const auto flipped_h = (-outflank_h & 0x3F) << 9;

	const auto outflank_c = GetLSB(~O & 0x0204081020400000ULL) & P;
	const auto flipped_c = (outflank_c - (outflank_c != 0)) & 0x0204081020400000ULL;

	return flipped_v | flipped_h | flipped_c;
}

uint64_t CFlipper::A3(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = ((O | ~0x0101010101000000ULL) + 0x0000000001000000ULL) & P & 0x0101010101000000ULL;
	const auto flipped_v = (outflank_v - (outflank_v != 0)) & 0x0101010101000000ULL;

	const auto outflank_h = ((O & 0x00000000007e0000ULL) + 0x0000000000020000ULL) & P;
	const auto flipped_h = (outflank_h - (outflank_h >> 8)) & 0x00000000007e0000ULL;

	const auto outflank_d = ((O | ~0x2010080402000000ULL) + 0x0000000002000000ULL) & P & 0x2010080402000000ULL;
	const auto flipped_d = (outflank_d - (outflank_d != 0)) & 0x2010080402000000ULL;

	const auto flipped_c = (((P << 8) & 0x0000000000000100ULL) | ((P << 7) & 0x0000000000000200ULL)) & O;

	return flipped_v | flipped_h | flipped_d | flipped_c;
}

uint64_t CFlipper::B3(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = ((O | ~0x0202020202000000ULL) + 0x0000000002000000ULL) & P & 0x0202020202000000ULL;
	const auto flipped_v = (outflank_v - (outflank_v != 0)) & 0x0202020202000000ULL;

	const auto outflank_h = ((O & 0x00000000007c0000ULL) + 0x0000000000040000ULL) & P;
	const auto flipped_h = (outflank_h - (outflank_h >> 8)) & 0x00000000007c0000ULL;

	const auto outflank_d = ((O | ~0x4020100804000000ULL) + 0x0000000004000000ULL) & P & 0x4020100804000000ULL;
	const auto flipped_d = (outflank_d - (outflank_d != 0)) & 0x4020100804000000ULL;

	const auto flipped_c = (((P << 8) & 0x0000000000000200ULL) | ((P << 7) & 0x0000000000000400ULL)) & O;

	return flipped_v | flipped_h | flipped_d | flipped_c;
}

uint64_t CFlipper::C3(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = ((O | ~0x0404040404000000ULL) + 0x0000000004000000ULL) & P & 0x0404040404000000ULL;
	const auto flipped_v = (outflank_v - (outflank_v != 0)) & 0x0404040404000000ULL;

	const auto outflank_h = ((O & 0x0000000000780000ULL) + 0x0000000000080000ULL) & P;
	const auto flipped_h = (outflank_h - (outflank_h >> 8)) & 0x0000000000780000ULL;

	const auto outflank_d = ((O | ~0x8040201008000000ULL) + 0x0000000008000000ULL) & P & 0x8040201008000000ULL;
	const auto flipped_d = (outflank_d - (outflank_d != 0)) & 0x8040201008000000ULL;

	const auto flipped_r = PDep(PExt(P, 0x0000000100010015ULL), 0x0000000002020E00ULL) & O;

	return flipped_v | flipped_h | flipped_d | flipped_r;
}

uint64_t CFlipper::D3(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetLSB(~O & 0x0808080808000000ULL) & P;
	const auto flipped_v = (outflank_v - (outflank_v != 0)) & 0x0808080808000000ULL;

	const auto outflank_h = OUTFLANK_3[BExtr(O, 17, 6)] & (P >> 16);
	const auto flipped_h = FLIPPED_3_H[outflank_h] & 0x0000000000FF0000ULL;

	const auto outflank_c = GetLSB(~O & 0x0000010204000000ULL) & P;
	const auto flipped_c = (outflank_c - (outflank_c != 0)) & 0x0000010204000000ULL;

	const auto outflank_d = GetLSB(~O & 0x0080402010000000ULL) & P;
	const auto flipped_d = (outflank_d - (outflank_d != 0)) & 0x0080402010000000ULL;

	const auto flipped_r = (PExt(P, 0x000000000000002AULL) << 10) & O;

	return flipped_v | flipped_h | flipped_d | flipped_c | flipped_r;
}

uint64_t CFlipper::E3(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetLSB(~O & 0x1010101010000000ULL) & P;
	const auto flipped_v = (outflank_v - (outflank_v != 0)) & 0x1010101010000000ULL;

	const auto outflank_h = OUTFLANK_4[BExtr(O, 17, 6)] & (P >> 16);
	const auto flipped_h = FLIPPED_4_H[outflank_h] & 0x0000000000FF0000ULL;

	const auto outflank_c = GetLSB(~O & 0x0001020408000000ULL) & P;
	const auto flipped_c = (outflank_c - (outflank_c != 0)) & 0x0001020408000000ULL;

	const auto outflank_d = GetLSB(~O & 0x0000804020000000ULL) & P;
	const auto flipped_d = (outflank_d - (outflank_d != 0)) & 0x0000804020000000ULL;

	const auto flipped_r = (PExt(P, 0x0000000000000054ULL) << 11) & O;

	return flipped_v | flipped_h | flipped_d | flipped_c | flipped_r;
}

uint64_t CFlipper::F3(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetLSB(~O & 0x2020202020000000ULL) & P;
	const auto flipped_v = (outflank_v - (outflank_v != 0)) & 0x2020202020000000ULL;

	const auto outflank_h = OUTFLANK_5[BExtr(O, 17, 6)] & (P >> 16);
	const auto flipped_h = FLIPPED_5_H[outflank_h] & 0x0000000000FF0000ULL;

	const auto outflank_c = GetLSB(~O & 0x0102040810000000ULL) & P;
	const auto flipped_c = (outflank_c - (outflank_c != 0)) & 0x0102040810000000ULL;

	const auto flipped_r = PDep(PExt(P, 0x00000080008000A8ULL), 0x0000000040407000ULL) & O;

	return flipped_v | flipped_h | flipped_c | flipped_r;
}

uint64_t CFlipper::G3(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetLSB(~O & 0x4040404040000000ULL) & P;
	const auto flipped_v = (outflank_v - (outflank_v != 0)) & 0x4040404040000000ULL;

	const auto outflank_h = OUTFLANK_7[(O >> 16) & 0x3E] & (P >> 15);
	const auto flipped_h = (-outflank_h & 0x3E) << 16;

	const auto outflank_c = GetLSB(~O & 0x0204081020000000ULL) & P;
	const auto flipped_c = (outflank_c - (outflank_c != 0)) & 0x0204081020000000ULL;

	const auto flipped_r = (((P << 8) & 0x0000000000004000ULL) | ((P << 9) & 0x0000000000002000ULL)) & O;

	return flipped_v | flipped_h | flipped_c | flipped_r;
}

uint64_t CFlipper::H3(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetLSB(~O & 0x8080808080000000ULL) & P;
	const auto flipped_v = (outflank_v - (outflank_v != 0)) & 0x8080808080000000ULL;

	const auto outflank_h = OUTFLANK_7[BExtr(O, 17, 6)] & (P >> 16);
	const auto flipped_h = (-outflank_h & 0x3F) << 17;

	const auto outflank_c = GetLSB(~O & 0x0408102040000000ULL) & P;
	const auto flipped_c = (outflank_c - (outflank_c != 0)) & 0x0408102040000000ULL;

	const auto flipped_r = (((P << 8) & 0x0000000000008000ULL) | ((P << 9) & 0x0000000000004000ULL)) & O;

	return flipped_v | flipped_h | flipped_c | flipped_r;
}

uint64_t CFlipper::A4(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = OUTFLANK_3[PExt(O, 0x0001010101010100ULL)] & PExt(P, 0x0101010101010101ULL);
	const auto flipped_v = FLIPPED_3_V[outflank_v] & 0x0001010101010100ULL;

	const auto outflank_h = ((O & 0x000000007e000000ULL) + 0x0000000002000000ULL) & P;
	const auto flipped_h = (outflank_h - (outflank_h >> 8)) & 0x000000007e000000ULL;

	const auto outflank_x = OUTFLANK_3[PExt(O, 0x0008040201020400ULL)] & PExt(P, 0x1008040201020408ULL);
	const auto flipped_x = FLIPPED_3_V[outflank_x] & 0x0008040201020400ULL;

	return flipped_v | flipped_h | flipped_x;
}

uint64_t CFlipper::B4(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = OUTFLANK_3[PExt(O, 0x0002020202020200ULL)] & PExt(P, 0x0202020202020202ULL);
	const auto flipped_v = FLIPPED_3_V[outflank_v] & 0x0002020202020200ULL;

	const auto outflank_h = ((O & 0x000000007c000000ULL) + 0x0000000004000000ULL) & P;
	const auto flipped_h = (outflank_h - (outflank_h != 0)) & 0x000000007c000000ULL;

	const auto outflank_x = OUTFLANK_3[PExt(O, 0x0010080402040800ULL)] & PExt(P, 0x2010080402040810ULL);
	const auto flipped_x = FLIPPED_3_V[outflank_x] & 0x0010080402040800ULL;

	return flipped_v | flipped_h | flipped_x;
}

uint64_t CFlipper::C4(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = OUTFLANK_3[PExt(O, 0x0004040404040400ULL)] & PExt(P, 0x0404040404040404ULL);
	const auto flipped_v = FLIPPED_3_V[outflank_v] & 0x0004040404040400ULL;

	const auto outflank_h = OUTFLANK_2[BExtr(O, 25, 6)] & (P >> 24);
	const auto flipped_h = FLIPPED_2_H[outflank_h] & 0x00000000FF000000ULL;

	const auto outflank_x = OUTFLANK_3[PExt(O, 0x0020100804081000ULL)] & PExt(P, 0x4020100804081020ULL);
	const auto flipped_x = FLIPPED_3_V[outflank_x] & 0x0020100804081000ULL;

	const auto flipped_r = (((P << 9) & 0x00000000000020000ULL) | ((P >> 7) & 0x00000000200000000ULL)) & O;

	return flipped_v | flipped_h | flipped_x | flipped_r;
}

uint64_t CFlipper::D4(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = OUTFLANK_3[PExt(O, 0x0008080808080800ULL)] & PExt(P, 0x0808080808080808ULL);
	const auto flipped_v = FLIPPED_3_V[outflank_v] & 0x0008080808080800ULL;

	const auto outflank_h = OUTFLANK_3[BExtr(O, 25, 6)] & (P >> 24);
	const auto flipped_h = FLIPPED_3_H[outflank_h] & 0x00000000FF000000ULL;

	const auto outflank_c = OUTFLANK_3[((O & 0x0000020408102000ULL) * 0x0101010101010101ULL) >> 57] & (((P & 0x0001020408102040ULL) * 0x0101010101010101ULL) >> 56);
	const auto flipped_c = FLIPPED_3_H[outflank_c] & 0x0000020408102000ULL;

	const auto outflank_d = OUTFLANK_3[PExt(O, 0x0040201008040200ULL)] & PExt(P, 0x8040201008040201ULL);
	const auto flipped_d = FLIPPED_3_H[outflank_d] & 0x0040201008040200ULL;

	return flipped_v | flipped_h | flipped_c | flipped_d;
}

uint64_t CFlipper::E4(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = OUTFLANK_3[PExt(O, 0x0010101010101000ULL)] & PExt(P, 0x1010101010101010ULL);
	const auto flipped_v = FLIPPED_3_V[outflank_v] & 0x0010101010101000ULL;

	const auto outflank_h = OUTFLANK_4[BExtr(O, 25, 6)] & (P >> 24);
	const auto flipped_h = FLIPPED_4_H[outflank_h] & 0x00000000FF000000ULL;

	const auto outflank_c = OUTFLANK_4[((O & 0x0002040810204000ULL) * 0x0101010101010101ULL) >> 57] & (((P & 0x0102040810204080ULL) * 0x0101010101010101ULL) >> 56);
	const auto flipped_c = FLIPPED_4_H[outflank_c] & 0x0002040810204000ULL;

	const auto outflank_d = (OUTFLANK_3[PExt(O, 0x0000402010080400ULL)] & PExt(P, 0x0080402010080402ULL)) << 1;
	const auto flipped_d = FLIPPED_4_H[outflank_d] & 0x0000402010080400ULL;

	return flipped_v | flipped_h | flipped_c | flipped_d;
}

uint64_t CFlipper::F4(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = OUTFLANK_3[PExt(O, 0x0020202020202000ULL)] & PExt(P, 0x2020202020202020ULL);
	const auto flipped_v = FLIPPED_3_V[outflank_v] & 0x0020202020202000ULL;

	const auto outflank_h = OUTFLANK_5[BExtr(O, 25, 6)] & (P >> 24);
	const auto flipped_h = FLIPPED_5_H[outflank_h] & 0x00000000FF000000ULL;

	const auto outflank_x = OUTFLANK_3[PExt(O, 0x0004081020100800ULL)] & PExt(P, 0x0204081020100804ULL);
	const auto flipped_x = FLIPPED_3_V[outflank_x] & 0x0004081020100800ULL;

	const auto flipped_r = (((P << 7) & 0x0000000000400000ULL) | ((P >> 9) & 0x0000004000000000ULL)) & O;

	return flipped_v | flipped_h | flipped_x | flipped_r;
}

uint64_t CFlipper::G4(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = OUTFLANK_3[PExt(O, 0x0040404040404000ULL)] & PExt(P, 0x4040404040404040ULL);
	const auto flipped_v = FLIPPED_3_V[outflank_v] & 0x0040404040404000ULL;

	const auto outflank_h = OUTFLANK_7[(O >> 24) & 0x3E] & (P >> 23);
	const auto flipped_h = (-outflank_h & 0x3E) << 24;

	const auto outflank_x = OUTFLANK_3[PExt(O, 0x0008102040201000ULL)] & PExt(P, 0x0408102040201008ULL);
	const auto flipped_x = FLIPPED_3_V[outflank_x] & 0x0008102040201000ULL;

	return flipped_v | flipped_h | flipped_x;
}

uint64_t CFlipper::H4(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = OUTFLANK_3[PExt(O, 0x0080808080808000ULL)] & PExt(P, 0x8080808080808080ULL);
	const auto flipped_v = FLIPPED_3_V[outflank_v] & 0x0080808080808000ULL;

	const auto outflank_h = OUTFLANK_7[BExtr(O, 25, 6)] & (P >> 24);
	const auto flipped_h = (-outflank_h & 0x3F) << 25;

	const auto outflank_x = OUTFLANK_3[PExt(O, 0x0010204080402000ULL)] & PExt(P, 0x0810204080402010ULL);
	const auto flipped_x = FLIPPED_3_V[outflank_x] & 0x0010204080402000ULL;

	return flipped_v | flipped_h | flipped_x;
}

uint64_t CFlipper::A5(const uint64_t P, const uint64_t O) const
{
	const auto outflank_a1a5d8 = OUTFLANK_4[PExt(O, 0x0004020101010100ULL)] & PExt(P, 0x0804020101010101ULL);
	const auto flipped_a1a5d8 = FLIPPED_4_V[outflank_a1a5d8] & 0x0004020101010100ULL;

	const auto outflank_a8a5e1 = OUTFLANK_4[PExt(O, 0x0001010102040800ULL)] & PExt(P, 0x0101010102040810ULL);
	const auto flipped_a8a5e1 = FLIPPED_4_V[outflank_a8a5e1] & 0x0001010102040800ULL;

	const auto outflank_h = ((O & 0x0000007e00000000ULL) + 0x0000000200000000ULL) & P;
	const auto flipped_h = (outflank_h - (outflank_h != 0)) & 0x0000007e00000000ULL;

	return flipped_a1a5d8 | flipped_a8a5e1 | flipped_h;
}

uint64_t CFlipper::B5(const uint64_t P, const uint64_t O) const
{
	const auto outflank_b1b5e8 = OUTFLANK_4[PExt(O, 0x0008040202020200ULL)] & PExt(P, 0x1008040202020202ULL);
	const auto flipped_b1b5e8 = FLIPPED_4_V[outflank_b1b5e8] & 0x0008040202020200ULL;

	const auto outflank_b8b5f1 = OUTFLANK_4[PExt(O, 0x0002020204081000ULL)] & PExt(P, 0x0202020204081020ULL);
	const auto flipped_b8b5f1 = FLIPPED_4_V[outflank_b8b5f1] & 0x0002020204081000ULL;

	const auto outflank_h = ((O & 0x0000007c00000000ULL) + 0x0000000400000000ULL) & P;
	const auto flipped_h = (outflank_h - (outflank_h != 0)) & 0x0000007c00000000ULL;

	return flipped_b1b5e8 | flipped_b8b5f1 | flipped_h;
}

uint64_t CFlipper::C5(const uint64_t P, const uint64_t O) const
{
	const auto outflank_c1c5f8 = OUTFLANK_4[PExt(O, 0x0010080404040400ULL)] & PExt(P, 0x2010080404040404ULL);
	const auto flipped_c1c5f8 = FLIPPED_4_V[outflank_c1c5f8] & 0x0010080404040400ULL;

	const auto outflank_c8c5g1 = OUTFLANK_4[PExt(O, 0x0004040408102000ULL)] & PExt(P, 0x0404040408102040ULL);
	const auto flipped_c8c5g1 = FLIPPED_4_V[outflank_c8c5g1] & 0x0004040408102000ULL;

	const auto outflank_h = OUTFLANK_2[BExtr(O, 33, 6)] & (P >> 32);
	const auto flipped_h = FLIPPED_2_H[outflank_h] & 0x000000FF00000000ULL;

	const auto flipped_r = (((P << 9) & 0x0000000002000000ULL) | ((P >> 7) & 0x0000020000000000ULL)) & O;

	return flipped_c1c5f8 | flipped_c8c5g1 | flipped_h | flipped_r;
}

uint64_t CFlipper::D5(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = OUTFLANK_4[PExt(O, 0x0008080808080800ULL)] & PExt(P, 0x0808080808080808ULL);
	const auto flipped_v = FLIPPED_4_V[outflank_v] & 0x0008080808080800ULL;

	const auto outflank_h = OUTFLANK_3[BExtr(O, 33, 6)] & (P >> 32);
	const auto flipped_h = FLIPPED_3_H[outflank_h] & 0x000000FF00000000ULL;

	const auto outflank_c = OUTFLANK_3[((O & 0x0002040810204000ULL) * 0x0101010101010101ULL) >> 57] & (((P & 0x0102040810204080ULL) * 0x0101010101010101ULL) >> 56);
	const auto flipped_c = FLIPPED_3_H[outflank_c] & 0x0002040810204000ULL;

	const auto outflank_d = OUTFLANK_3[PExt(O, 0x0020100804020000ULL)] & PExt(P, 0x4020100804020100ULL);
	const auto flipped_d = FLIPPED_3_H[outflank_d] & 0x0020100804020000ULL;

	return flipped_v | flipped_h | flipped_c | flipped_d;
}

uint64_t CFlipper::E5(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = OUTFLANK_4[PExt(O, 0x0010101010101000ULL)] & PExt(P, 0x1010101010101010ULL);
	const auto flipped_v = FLIPPED_4_V[outflank_v] & 0x0010101010101000ULL;

	const auto outflank_h = OUTFLANK_4[BExtr(O, 33, 6)] & (P >> 32);
	const auto flipped_h = FLIPPED_4_H[outflank_h] & 0x000000FF00000000ULL;

	const auto outflank_c = OUTFLANK_4[((O & 0x0004081020400000ULL) * 0x0101010101010101ULL) >> 57] & (((P & 0x0204081020408000ULL) * 0x0101010101010101ULL) >> 56);
	const auto flipped_c = FLIPPED_4_H[outflank_c] & 0x0004081020400000ULL;

	const auto outflank_d = OUTFLANK_4[PExt(O, 0x0040201008040200ULL)] & PExt(P, 0x8040201008040201ULL);
	const auto flipped_d = FLIPPED_4_H[outflank_d] & 0x0040201008040200ULL;

	return flipped_v | flipped_h | flipped_c | flipped_d;
}

uint64_t CFlipper::F5(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = OUTFLANK_4[PExt(O, 0x0020202020202000ULL)] & PExt(P, 0x2020202020202020ULL);
	const auto flipped_v = FLIPPED_4_V[outflank_v] & 0x0020202020202000ULL;

	const auto outflank_h = OUTFLANK_5[BExtr(O, 33, 6)] & (P >> 32);
	const auto flipped_h = FLIPPED_5_H[outflank_h] & 0x000000FF00000000ULL;

	const auto outflank_d = OUTFLANK_4[PExt(O, 0x0008102010080400ULL)] & PExt(P, 0x0408102010080402ULL);
	const auto flipped_d = FLIPPED_4_V[outflank_d] & 0x0008102010080400ULL;

	const auto flipped_r = (((P << 7) & 0x0000000040000000ULL) | ((P >> 9) & 0x0000400000000000ULL)) & O;

	return flipped_v | flipped_h | flipped_d | flipped_r;
}

uint64_t CFlipper::G5(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = OUTFLANK_4[PExt(O, 0x0040404040404000ULL)] & PExt(P, 0x4040404040404040ULL);
	const auto flipped_v = FLIPPED_4_V[outflank_v] & 0x0040404040404000ULL;

	const auto outflank_h = OUTFLANK_7[(O >> 32) & 0x3E] & (P >> 31);
	const auto flipped_h = (-outflank_h & 0x3E) << 32;

	const auto outflank_d = OUTFLANK_4[PExt(O, 0x0010204020100800ULL)] & PExt(P, 0x0810204020100804ULL);
	const auto flipped_d = FLIPPED_4_V[outflank_d] & 0x0010204020100800ULL;

	return flipped_v | flipped_h | flipped_d;
}

uint64_t CFlipper::H5(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = OUTFLANK_4[PExt(O, 0x0080808080808000ULL)] & PExt(P, 0x8080808080808080ULL);
	const auto flipped_v = FLIPPED_4_V[outflank_v] & 0x0080808080808000ULL;

	const auto outflank_h = OUTFLANK_7[BExtr(O, 33, 6)] & (P >> 32);
	const auto flipped_h = (-outflank_h & 0x3F) << 33;

	const auto outflank_d = OUTFLANK_4[PExt(O, 0x0020408040201000ULL)] & PExt(P, 0x1020408040201008ULL);
	const auto flipped_d = FLIPPED_4_V[outflank_d] & 0x0020408040201000ULL;

	return flipped_v | flipped_h | flipped_d;
}

uint64_t CFlipper::A6(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = OUTFLANK_5[PExt(O, 0x0001010101010100ULL)] & PExt(P, 0x0101010101010101ULL);
	const auto flipped_v = FLIPPED_5_V[outflank_v] & 0x0001010101010100ULL;

	const auto outflank_h = ((O & 0x00007e0000000000ULL) + 0x0000020000000000ULL) & P;
	const auto flipped_h = (outflank_h - (outflank_h >> 8)) & 0x00007e0000000000ULL;

	const uint64_t flip_c = OUTFLANK_5[PExt(O, 0x0002010204081000ULL)] & PExt(P, 0x0402010204081020ULL);
	const auto flipped_x = FLIPPED_5_V[flip_c] & 0x0402010204081020ULL;

	return flipped_v | flipped_h | flipped_x;
}

uint64_t CFlipper::B6(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = OUTFLANK_5[PExt(O, 0x0002020202020200ULL)] & PExt(P, 0x0202020202020202ULL);
	const auto flipped_v = FLIPPED_5_V[outflank_v] & 0x0002020202020200ULL;

	const auto outflank_h = ((O & 0x00007c0000000000ULL) + 0x0000040000000000ULL) & P;
	const auto flipped_h = (outflank_h - (outflank_h >> 8)) & 0x00007c0000000000ULL;

	const uint64_t flip_c = OUTFLANK_5[PExt(O, 0x0004020408102000ULL)] & PExt(P, 0x0804020408102040ULL);
	const auto flipped_x = FLIPPED_5_V[flip_c] & 0x0804020408102040ULL;

	return flipped_v | flipped_h | flipped_x;
}

uint64_t CFlipper::C6(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = OUTFLANK_5[PExt(O, 0x0004040404040400ULL)] & PExt(P, 0x0404040404040404ULL);
	const auto flipped_v = FLIPPED_5_V[outflank_v] & 0x0004040404040400ULL;

	const auto outflank_h = OUTFLANK_2[BExtr(O, 41, 6)] & (P >> 40);
	const auto flipped_h = FLIPPED_2_H[outflank_h] & 0x0000FF0000000000ULL;

	const auto outflank_c = OUTFLANK_5[PExt(O, 0x0002040810204000ULL)] & PExt(P, 0x0102040810204080ULL);
	const auto flipped_c = FLIPPED_5_V[outflank_c] & 0x0002040810204000ULL;

	const auto flipped_d = ((P >> 9) | (P << 9)) & 0x0008000200000000ULL & O;

	return flipped_v | flipped_h | flipped_c | flipped_d;
}

uint64_t CFlipper::D6(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = OUTFLANK_5[PExt(O, 0x0008080808080800ULL)] & PExt(P, 0x0808080808080808ULL);
	const auto flipped_v = FLIPPED_5_V[outflank_v] & 0x0008080808080800ULL;

	const auto outflank_h = OUTFLANK_3[BExtr(O, 41, 6)] & (P >> 40);
	const auto flipped_h = FLIPPED_3_H[outflank_h] & 0x0000FF0000000000ULL;

	const auto outflank_d = OUTFLANK_3[((O & 0x0000001422400000ULL) * 0x0101010101010101ULL) >> 57] & (((P & 0x0000001422418000ULL) * 0x0101010101010101ULL) >> 56);
	const auto flipped_d = FLIPPED_3_H[outflank_d] & 0x0000001422400000ULL;	// A3D6H2

	const auto flipped_r = (((P >> 9) & 0x0010000000000000ULL) | ((P >> 7) & 0x0004000000000000ULL)) & O;

	return flipped_v | flipped_h | flipped_d | flipped_r;
}

uint64_t CFlipper::E6(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = OUTFLANK_5[PExt(O, 0x0010101010101000ULL)] & PExt(P, 0x1010101010101010ULL);
	const auto flipped_v = FLIPPED_5_V[outflank_v] & 0x0010101010101000ULL;

	const auto outflank_h = OUTFLANK_4[BExtr(O, 41, 6)] & (P >> 40);
	const auto flipped_h = FLIPPED_4_H[outflank_h] & 0x0000FF0000000000ULL;

	const auto outflank_d = OUTFLANK_4[((O & 0x0000002844020000ULL) * 0x0101010101010101ULL) >> 57] & (((P & 0x0000002844820100ULL) * 0x0101010101010101ULL) >> 56);
	const auto flipped_d = FLIPPED_4_H[outflank_d] & 0x0000002844020000ULL;	// A2E6H3

	const auto flipped_r = (((P >> 9) & 0x0020000000000000ULL) | ((P >> 7) & 0x0008000000000000ULL)) & O;

	return flipped_v | flipped_h | flipped_d | flipped_r;
}

uint64_t CFlipper::F6(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = OUTFLANK_5[PExt(O, 0x0020202020202000ULL)] & PExt(P, 0x2020202020202020ULL);
	const auto flipped_v = FLIPPED_5_V[outflank_v] & 0x0020202020202000ULL;

	const auto outflank_h = OUTFLANK_5[BExtr(O, 41, 6)] & (P >> 40);
	const auto flipped_h = FLIPPED_5_H[outflank_h] & 0x0000FF0000000000ULL;

	const auto flipped_c = ((P >> 7) | (P << 7)) & 0x0010004000000000ULL & O;

	const auto outflank_d = OUTFLANK_5[PExt(O, 0x0040201008040200ULL)] & PExt(P, 0x8040201008040201ULL);
	const auto flipped_d = FLIPPED_5_H[outflank_d] & 0x0040201008040200ULL;

	return flipped_v | flipped_h | flipped_c | flipped_d;
}

uint64_t CFlipper::G6(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = OUTFLANK_5[PExt(O, 0x0040404040404000ULL)] & PExt(P, 0x4040404040404040ULL);
	const auto flipped_v = FLIPPED_5_V[outflank_v] & 0x0040404040404000ULL;

	const auto outflank_h = OUTFLANK_7[(O >> 40) & 0x3E] & (P >> 39);
	const auto flipped_h = (-outflank_h & 0x3E) << 40;

	const uint64_t flip_x = OUTFLANK_5[PExt(O, 0x0020402010080400ULL)] & PExt(P, 0x1020402010080402ULL);
	const auto flipped_x = FLIPPED_5_V[flip_x] & 0x1020402010080402ULL;

	return flipped_v | flipped_h | flipped_x;
}

uint64_t CFlipper::H6(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = OUTFLANK_5[PExt(O, 0x0080808080808000ULL)] & PExt(P, 0x8080808080808080ULL);
	const auto flipped_v = FLIPPED_5_V[outflank_v] & 0x0080808080808000ULL;

	const auto outflank_h = OUTFLANK_7[BExtr(O, 41, 6)] & (P >> 40);
	const auto flipped_h = (-outflank_h & 0x3F) << 41;

	const uint64_t flip_x = OUTFLANK_5[PExt(O, 0x0040804020100800ULL)] & PExt(P, 0x2040804020100804ULL);
	const auto flipped_x = FLIPPED_5_V[flip_x] & 0x2040804020100804ULL;

	return flipped_v | flipped_h | flipped_x;
}

uint64_t CFlipper::A7(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetMSB(~O & 0x0000010101010101ULL) & P;
	const auto flipped_v = (-outflank_v * 2) & 0x0000010101010100ULL;

	const auto outflank_h = ((O & 0x007e000000000000ULL) + 0x0002000000000000ULL) & P;
	const auto flipped_h = (outflank_h - (outflank_h >> 8)) & 0x007e000000000000ULL;

	const auto outflank_c = GetMSB(~O & 0x0000020408102040ULL) & P;
	const auto flipped_c = (-outflank_c * 2) & 0x0000020408102000ULL;

	return flipped_v | flipped_h | flipped_c;
}

uint64_t CFlipper::B7(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetMSB(~O & 0x0000020202020202ULL) & P;
	const auto flipped_v = (-outflank_v * 2) & 0x0000020202020200ULL;

	const auto outflank_h = ((O & 0x007c000000000000ULL) + 0x0004000000000000ULL) & P;
	const auto flipped_h = (outflank_h - (outflank_h >> 8)) & 0x007c000000000000ULL;

	const auto outflank_c = GetMSB(~O & 0x0000040810204080ULL) & P;
	const auto flipped_c = (-outflank_c * 2) & 0x0000040810204000ULL;

	return flipped_v | flipped_h | flipped_c;
}

uint64_t CFlipper::C7(const uint64_t P, const uint64_t O) const
{
	// ############ Room for optimization ############
	const auto outflank_v = GetMSB(~O & 0x0000040404040404ULL) & P;
	const auto flipped_v = (-outflank_v * 2) & 0x0000040404040400ULL;

	const auto outflank_h = OUTFLANK_2[BExtr(O, 49, 6)] & (P >> 48);
	const auto flipped_h = FLIPPED_2_H[outflank_h] & 0x00FF000000000000ULL;

	const auto outflank_d = OUTFLANK_2[((O & 0x00000A1020400000ULL) * 0x0101010101010101ULL) >> 57] & (((P & 0x00000a1120408000ULL) * 0x0101010101010101ULL) >> 56);
	const auto flipped_d = FLIPPED_2_H[outflank_d] & 0x00000A1020400000ULL;	// A5C7H2

	return flipped_v | flipped_h | flipped_d;
}

uint64_t CFlipper::D7(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetMSB(~O & 0x0000080808080808ULL) & P;
	const auto flipped_v = (-outflank_v * 2) & 0x0000080808080800ULL;

	const auto outflank_h = OUTFLANK_3[BExtr(O, 49, 6)] & (P >> 48);
	const auto flipped_h = FLIPPED_3_H[outflank_h] & 0x00FF000000000000ULL;

	const auto outflank_d = OUTFLANK_3[((O & 0x0000142240000000ULL) * 0x0101010101010101ULL) >> 57] & (((P & 0x0000142241800000ULL) * 0x0101010101010101ULL) >> 56);
	const auto flipped_d = FLIPPED_3_H[outflank_d] & 0x0000142240000000ULL;	// A4D7H3

	return flipped_v | flipped_h | flipped_d;
}

uint64_t CFlipper::E7(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetMSB(~O & 0x0000101010101010ULL) & P;
	const auto flipped_v = (-outflank_v * 2) & 0x0000101010101000ULL;

	const auto outflank_h = OUTFLANK_4[BExtr(O, 49, 6)] & (P >> 48);
	const auto flipped_h = FLIPPED_4_H[outflank_h] & 0x00FF000000000000ULL;

	const auto outflank_d = OUTFLANK_4[((O & 0x0000284402000000ULL) * 0x0101010101010101ULL) >> 57] & (((P & 0x0000284482010000ULL) * 0x0101010101010101ULL) >> 56);
	const auto flipped_d = FLIPPED_4_H[outflank_d] & 0x0000284402000000ULL; // A3E7H4

	return flipped_v | flipped_h | flipped_d;
}

uint64_t CFlipper::F7(const uint64_t P, const uint64_t O) const
{
	// ############ Room for optimization ############
	const auto outflank_v = GetMSB(~O & 0x0000202020202020ULL) & P;
	const auto flipped_v = (-outflank_v * 2) & 0x0000202020202000ULL;

	const auto outflank_h = OUTFLANK_5[BExtr(O, 49, 6)] & (P >> 48);
	const auto flipped_h = FLIPPED_5_H[outflank_h] & 0x00FF000000000000ULL;

	const auto outflank_d = OUTFLANK_5[((O & 0x0000500804020000ULL) * 0x0101010101010101ULL) >> 57] & (((P & 0x0000508804020100ULL) * 0x0101010101010101ULL) >> 56);
	const auto flipped_d = FLIPPED_5_H[outflank_d] & 0x0000500804020000ULL;	// A2F7H5

	return flipped_v | flipped_h | flipped_d;
}

uint64_t CFlipper::G7(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetMSB(~O & 0x0000404040404040ULL) & P;
	const auto flipped_v = (-outflank_v * 2) & 0x0000404040404000ULL;

	const auto outflank_h = OUTFLANK_7[(O >> 48) & 0x3E] & (P >> 47);
	const auto flipped_h = (-outflank_h & 0x3E) << 48;

	const auto outflank_d = GetMSB(~O & 0x0000201008040201ULL) & P;
	const auto flipped_d = (-outflank_d * 2) & 0x0000201008040200ULL;

	return flipped_v | flipped_h | flipped_d;
}

uint64_t CFlipper::H7(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetMSB(~O & 0x0000808080808080ULL) & P;
	const auto flipped_v = (-outflank_v * 2) & 0x0000808080808000ULL;

	const auto outflank_h = OUTFLANK_7[BExtr(O, 49, 6)] & (P >> 48);
	const auto flipped_h = (-outflank_h & 0x3F) << 49;

	const auto outflank_d = GetMSB(~O & 0x0000402010080402ULL) & P;
	const auto flipped_d = (-outflank_d * 2) & 0x0000402010080400ULL;

	return flipped_v | flipped_h | flipped_d;
}

uint64_t CFlipper::A8(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetMSB(~O & 0x0001010101010101ULL) & P;
	const auto flipped_v = (-outflank_v * 2) & 0x0001010101010100ULL;

	const auto outflank_h = ((O & 0x7e00000000000000ULL) + 0x0200000000000000ULL) & P;
	const auto flipped_h = (outflank_h - (outflank_h >> 8)) & 0x7e00000000000000ULL;

	const auto outflank_c = GetMSB(~O & 0x0002040810204080ULL) & P;
	const auto flipped_c = (-outflank_c * 2) & 0x0002040810204000ULL;

	return flipped_v | flipped_h | flipped_c;
}

uint64_t CFlipper::B8(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetMSB(~O & 0x0002020202020202ULL) & P;
	const auto flipped_v = (-outflank_v * 2) & 0x0002020202020200ULL;

	const auto outflank_h = ((O & 0x7c00000000000000ULL) + 0x0400000000000000ULL) & P;
	const auto flipped_h = (outflank_h - (outflank_h >> 8)) & 0x7c00000000000000ULL;

	const auto outflank_c = GetMSB(~O & 0x0004081020408000ULL) & P;
	const auto flipped_c = (-outflank_c * 2) & 0x0004081020400000ULL;

	return flipped_v | flipped_h | flipped_c;
}

uint64_t CFlipper::C8(const uint64_t P, const uint64_t O) const
{
	// ############ Room for optimization ############
	const auto outflank_v = GetMSB(~O & 0x0004040404040404ULL) & P;
	const auto flipped_v = (-outflank_v * 2) & 0x0004040404040400ULL;

	const auto outflank_h = OUTFLANK_2[BExtr(O, 57, 6)] & (P >> 56);
	const auto flipped_h = FLIPPED_2_H[outflank_h] & 0xFF00000000000000ULL;

	const auto outflank_d = OUTFLANK_2[((O & 0x000a102040000000ULL) * 0x0101010101010101ULL) >> 57] & (((P & 0x000a112040800000ULL) * 0x0101010101010101ULL) >> 56);
	const auto flipped_d = FLIPPED_2_H[outflank_d] & 0x000A102040000000ULL;	// A6C8H3

	return flipped_v | flipped_h | flipped_d;
}

uint64_t CFlipper::D8(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetMSB(~O & 0x0008080808080808ULL) & P;
	const auto flipped_v = (-outflank_v * 2) & 0x0008080808080800ULL;

	const auto outflank_h = OUTFLANK_3[BExtr(O, 57, 6)] & (P >> 56);
	const auto flipped_h = FLIPPED_3_H[outflank_h] & 0xFF00000000000000ULL;

	const auto outflank_d = OUTFLANK_3[((O & 0x0014224000000000ULL) * 0x0101010101010101ULL) >> 57] & (((P & 0x0014224180000000ULL) * 0x0101010101010101ULL) >> 56);
	const auto flipped_d = FLIPPED_3_H[outflank_d] & 0x0014224000000000ULL;	// A5D8H4

	return flipped_v | flipped_h | flipped_d;
}

uint64_t CFlipper::E8(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetMSB(~O & 0x0010101010101010ULL) & P;
	const auto flipped_v = (-outflank_v * 2) & 0x0010101010101000ULL;

	const auto outflank_h = OUTFLANK_4[BExtr(O, 57, 6)] & (P >> 56);
	const auto flipped_h = FLIPPED_4_H[outflank_h] & 0xFF00000000000000ULL;

	const auto outflank_d = OUTFLANK_4[((O & 0x0028440200000000ULL) * 0x0101010101010101ULL) >> 57] & (((P & 0x0028448201000000ULL) * 0x0101010101010101ULL) >> 56);
	const auto flipped_d = FLIPPED_4_H[outflank_d] & 0x0028440200000000ULL;	// A4E8H5

	return flipped_v | flipped_h | flipped_d;
}

uint64_t CFlipper::F8(const uint64_t P, const uint64_t O) const
{
	// ############ Room for optimization ############
	const auto outflank_v = GetMSB(~O & 0x0020202020202020ULL) & P;
	const auto flipped_v = (-outflank_v * 2) & 0x0020202020202000ULL;

	const auto outflank_h = OUTFLANK_5[BExtr(O, 57, 6)] & (P >> 56);
	const auto flipped_h = FLIPPED_5_H[outflank_h] & 0xFF00000000000000ULL;

	const auto outflank_d = OUTFLANK_5[((O & 0x0050080402000000ULL) * 0x0101010101010101ULL) >> 57] & (((P & 0x0050880402010000ULL) * 0x0101010101010101ULL) >> 56);
	const auto flipped_d = FLIPPED_5_H[outflank_d] & 0x0050080402000000ULL;	// A3F8H6

	return flipped_v | flipped_h | flipped_d;
}

uint64_t CFlipper::G8(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetMSB(~O & 0x0040404040404040ULL) & P;
	const auto flipped_v = (-outflank_v * 2) & 0x0040404040404000ULL;

	const auto outflank_h = OUTFLANK_7[(O >> 56) & 0x3E] & (P >> 55);
	const auto flipped_h = (-outflank_h & 0x3E) << 56;

	const auto outflank_d = GetMSB(~O & 0x0020100804020100ULL) & P;
	const auto flipped_d = (-outflank_d * 2) & 0x0020100804020000ULL;

	return flipped_v | flipped_h | flipped_d;
}

uint64_t CFlipper::H8(const uint64_t P, const uint64_t O) const
{
	const auto outflank_v = GetMSB(~O & 0x0080808080808080ULL) & P;
	const auto flipped_v = (-outflank_v * 2) & 0x0080808080808000ULL;

	const auto outflank_h = OUTFLANK_7[BExtr(O, 57, 6)] & (P >> 56);
	const auto flipped_h = (-outflank_h & 0x3F) << 57;

	const auto outflank_d = GetMSB(~O & 0x0040201008040201ULL) & P;
	const auto flipped_d = (-outflank_d * 2) & 0x0040201008040200ULL;

	return flipped_v | flipped_h | flipped_d;
}

#pragma warning(pop)