#pragma once
#include "datamanipulation.h"
#include "flip_fast.h"
#include "possiblemoves.h"
#include "selectivity.h"
#include "utility.h"
#include "macros_hell.h"
#include "StreamSerializables.h"
#include "ObjectSerializables.h"
#include "FileStreamArchive.h"
#include "StreamConverter.h"
#include "SingleLineStreamDecorator.h"
#include "MultiLineStreamDecorator.h"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

inline std::string Move(const uint8_t move)
{
	assert(move <= 64);
	static const std::string name[65] = {
		"A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1",
		"A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2",
		"A3", "B3", "C3", "D3", "E3", "F3", "G3", "H3",
		"A4", "B4", "C4", "D4", "E4", "F4", "G4", "H4",
		"A5", "B5", "C5", "D5", "E5", "F5", "G5", "H5",
		"A6", "B6", "C6", "D6", "E6", "F6", "G6", "H6",
		"A7", "B7", "C7", "D7", "E7", "F7", "G7", "H7",
		"A8", "B8", "C8", "D8", "E8", "F8", "G8", "H8", "--"
	};
	return name[move];
}

inline uint8_t Move(const std::string& field_name)
{
	if (field_name == "--")
		return 64;

	auto letter = field_name.substr(0, 1);
	auto number = field_name.substr(1, 1);

	uint8_t letter_index;
	if (letter == "A") letter_index = 0;
	else if (letter == "B") letter_index = 1;
	else if (letter == "C") letter_index = 2;
	else if (letter == "D") letter_index = 3;
	else if (letter == "E") letter_index = 4;
	else if (letter == "F") letter_index = 5;
	else if (letter == "G") letter_index = 6;
	else if (letter == "H") letter_index = 7;

	uint8_t number_index = std::stoi(number) - 1;

	return number_index * 8 + letter_index;
}

enum Field : uint8_t
{
	A1, B1, C1, D1, E1, F1, G1, H1,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A8, B8, C8, D8, E8, F8, G8, H8,
	pass
};

inline uint64_t FlipCodiagonal(uint64_t b)
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

inline uint64_t FlipDiagonal(uint64_t b)
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

inline uint64_t FlipHorizontal(uint64_t b)
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

inline uint64_t FlipVertical(uint64_t b)
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

const uint64_t neighbour[64] = { // Neighbours to the input-field
	0x0000000000000302ULL, 0x0000000000000705ULL, 0x0000000000000e0aULL, 0x0000000000001c14ULL,	0x0000000000003828ULL, 0x0000000000007050ULL, 0x000000000000e0a0ULL, 0x000000000000c040ULL,
	0x0000000000030203ULL, 0x0000000000070507ULL, 0x00000000000e0a0eULL, 0x00000000001c141cULL,	0x0000000000382838ULL, 0x0000000000705070ULL, 0x0000000000e0a0e0ULL, 0x0000000000c040c0ULL,
	0x0000000003020300ULL, 0x0000000007050700ULL, 0x000000000e0a0e00ULL, 0x000000001c141c00ULL,	0x0000000038283800ULL, 0x0000000070507000ULL, 0x00000000e0a0e000ULL, 0x00000000c040c000ULL,
	0x0000000302030000ULL, 0x0000000705070000ULL, 0x0000000e0a0e0000ULL, 0x0000001c141c0000ULL,	0x0000003828380000ULL, 0x0000007050700000ULL, 0x000000e0a0e00000ULL, 0x000000c040c00000ULL,
	0x0000030203000000ULL, 0x0000070507000000ULL, 0x00000e0a0e000000ULL, 0x00001c141c000000ULL,	0x0000382838000000ULL, 0x0000705070000000ULL, 0x0000e0a0e0000000ULL, 0x0000c040c0000000ULL,
	0x0003020300000000ULL, 0x0007050700000000ULL, 0x000e0a0e00000000ULL, 0x001c141c00000000ULL,	0x0038283800000000ULL, 0x0070507000000000ULL, 0x00e0a0e000000000ULL, 0x00c040c000000000ULL,
	0x0302030000000000ULL, 0x0705070000000000ULL, 0x0e0a0e0000000000ULL, 0x1c141c0000000000ULL,	0x3828380000000000ULL, 0x7050700000000000ULL, 0xe0a0e00000000000ULL, 0xc040c00000000000ULL,
	0x0203000000000000ULL, 0x0507000000000000ULL, 0x0a0e000000000000ULL, 0x141c000000000000ULL,	0x2838000000000000ULL, 0x5070000000000000ULL, 0xa0e0000000000000ULL, 0x40c0000000000000ULL
};
const uint8_t quadrant_id_4_bit[64] = {   
	1, 1, 1, 1, 2, 2, 2, 2,
	1, 1, 1, 1, 2, 2, 2, 2,
	1, 1, 1, 1, 2, 2, 2, 2,
	1, 1, 1, 1, 2, 2, 2, 2,
	4, 4, 4, 4, 8, 8, 8, 8,
	4, 4, 4, 4, 8, 8, 8, 8,
	4, 4, 4, 4, 8, 8, 8, 8,
	4, 4, 4, 4, 8, 8, 8, 8
};
const uint64_t quadrant_mask[64] = {
	0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL,
	0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL,
	0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL,
	0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL,
	0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL,
	0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL,
	0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL,
	0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL,
};
const uint64_t quadrant[16] = {   
	0x0000000000000000ULL, //0000
	0x000000000F0F0F0FULL, //0001
	0x00000000F0F0F0F0ULL, //0010
	0x00000000FFFFFFFFULL, //0011
	0x0F0F0F0F00000000ULL, //0100
	0x0F0F0F0F0F0F0F0FULL, //0101
	0x0F0F0F0FF0F0F0F0ULL, //0110
	0x0F0F0F0FFFFFFFFFULL, //0111
	0xF0F0F0F000000000ULL, //1000
	0xF0F0F0F00F0F0F0FULL, //1001
	0xF0F0F0F0F0F0F0F0ULL, //1010
	0xF0F0F0F0FFFFFFFFULL, //1011
	0xFFFFFFFF00000000ULL, //1100
	0xFFFFFFFF0F0F0F0FULL, //1101
	0xFFFFFFFFF0F0F0F0ULL, //1110
	0xFFFFFFFFFFFFFFFFULL  //1111
};

namespace Stability
{
	void Initialize();
}

inline uint64_t StableStonesCornerAndCo(const uint64_t O)
{
#ifdef HAS_PEXT
	// 1 x AND, 1 x OR, 1 x PEXT, 2 x PDEP
	// = 5 OPs
	const uint64_t tmp = PExt(O, 0x8100000000000081ULL);
	return (PDep(tmp, 0x4200000000000042ULL) | PDep(tmp, 0x0081000000008100ULL)) & O;
#else
	// 5x AND, 4x SHIFT, 4x OR
	// = 13 OPs
	return (
		((O & 0x0100000000000001ULL) << 1) |
		((O & 0x8000000000000080ULL) >> 1) |
		((O & 0x8100000000000081ULL) << 8) |
		((O & 0x8100000000000081ULL) >> 8) |
		0x8100000000000081ULL
		) & O;
#endif
}
uint64_t StableStonesFullEdges(const uint64_t P, const uint64_t O);
uint64_t StableStonesFullEdgesSecondOrder(const uint64_t P, const uint64_t O);
uint64_t StableStonesTriangles(const uint64_t O);
uint64_t StableEdges(const uint64_t P, const uint64_t O);
uint64_t StableStonesPlayer(const uint64_t P, const uint64_t O);

FORCE_INLINE uint64_t SMEAR_BITBOARD(uint64_t B)
{
	// 4x SHIFT, 4x OR, 2x AND 
	// = 10 OPs
	B |= ((B >> 1) & 0x7F7F7F7F7F7F7F7FULL) | ((B << 1) & 0xFEFEFEFEFEFEFEFEULL);
	return B | (B >> 8) | (B << 8);
}

inline uint64_t   PlayersBoarder (const uint64_t P, const uint64_t O) { return SMEAR_BITBOARD(P) & ~(P | O); } // 13 OPs
inline uint64_t OpponentsBoarder (const uint64_t P, const uint64_t O) { return SMEAR_BITBOARD(O) & ~(P | O); } // 13 OPs
inline uint64_t          Boarders(const uint64_t P, const uint64_t O) { return SMEAR_BITBOARD(P | O) & ~(P | O); } // 14 OPs

inline uint64_t   PlayersExposed (const uint64_t P, const uint64_t O) { return SMEAR_BITBOARD(~(P | O)) &  P; } // 13 OPs
inline uint64_t OpponentsExposed (const uint64_t P, const uint64_t O) { return SMEAR_BITBOARD(~(P | O)) &      O; } // 13 OPs
inline uint64_t          Exposeds(const uint64_t P, const uint64_t O) { return SMEAR_BITBOARD(~(P | O)) & (P | O); } // 14 OPs


inline uint64_t Empties(uint64_t P, uint64_t O) { return ~(P | O); } // TODO: Delete!
inline uint64_t EmptyCount(uint64_t P, uint64_t O) { return PopCount(Empties(P, O)); } // TODO: Delete!
inline uint64_t Parity(uint64_t P, uint64_t O) // TODO: Delete!
{
	uint64_t E = Empties(P, O);
	E ^= E >> 1;
	E ^= E >> 2;
	E ^= E >> 8;
	E ^= E >> 16;
	#ifdef HAS_PEXT
		return PExt(E, 0x0000001100000011ULL);
	#else
		E &= 0x0000001100000011ULL;
		E |= E >> 3;
		E |= E >> 30;
		return E & 0xFULL;
	#endif
}

class CPosition : public ObjectSerializable, public Streamable
{
public:
	static const uint8_t classId = 1;
	uint64_t P, O;
	
	CPosition() : CPosition(0, 0) {}
	CPosition(uint64_t P, uint64_t O) : P(P), O(O) {}
	CPosition(iStreamArchive& arch) { deserialize(arch); }
	
	static CPosition StartPosition   () { return CPosition(0x0000000810000000ULL, 0x0000001008000000ULL); }
	static CPosition StartPositionETH() { return CPosition(0x0000001800000000ULL, 0x0000000018000000ULL); }
	
	uint64_t Parity() const;
	uint64_t Empties() const       { return ~(P | O); }
	uint64_t EmptyCount() const    { return PopCount(Empties()); }
	uint64_t PossibleMoves() const { return ::PossibleMoves(P, O); }
	bool     HasMoves() const      { return ::HasMoves(P, O); }
	
	virtual uint8_t ClassId() const { return classId; }
	virtual CPosition* Clone() const { return new CPosition(*this); }
	virtual void PlayStone(uint8_t move);
	virtual bool Test() const { return (P & O) == 0; }
	virtual bool IsSolved() const { return false; }
	
	bool operator==(const CPosition& o) const { return (P == o.P) && (O == o.O); }
	bool operator!=(const CPosition& o) const { return (P != o.P) || (O != o.O); }
	bool operator<=(const CPosition& o) const { return (P <= o.P) || ((P == o.P) && (O <= o.O)); }
	bool operator>=(const CPosition& o) const { return (P >= o.P) || ((P == o.P) && (O >= o.O)); }
	bool operator< (const CPosition& o) const { return (P <  o.P) || ((P == o.P) && (O <  o.O)); }
	bool operator> (const CPosition& o) const { return (P >  o.P) || ((P == o.P) && (O >  o.O)); }

	bool comp (const CPosition& o) const { return this->operator< (o); }
	bool equiv(const CPosition& o) const { return this->operator==(o); }
	
	void FlipCodiagonal() { P = ::FlipCodiagonal(P); O = ::FlipCodiagonal(O); }
	void FlipDiagonal  () { P = ::FlipDiagonal  (P); O = ::FlipDiagonal  (O); }
	void FlipHorizontal() { P = ::FlipHorizontal(P); O = ::FlipHorizontal(O); }
	void FlipVertical  () { P = ::FlipVertical  (P); O = ::FlipVertical  (O); }
	
	void FlipToMin(); // Flip to minimum of all flips.

	void Serialize(oArchive&) const override { throw std::logic_error("Archive not supported"); } // TODO: Get rid of this!
	void Serialize(oStreamArchive& arch) const override { arch << classId; serialize(arch); }
	void Serialize(oPositionArchive& arch) const override { arch.Serialize(*this); }
	void Deserialize(iStreamArchive& arch) override { deserialize(arch); }

protected:
	void serialize(oStreamArchive& arch) const { arch << P << O; }
	void deserialize(iStreamArchive& arch) { arch >> P >> O; }
};

/// Position with score
class CPositionScore : public CPosition
{
	static const int8_t DEFAULT_SCORE = -99;
public:
	static const uint8_t classId = 2;
	int8_t score;
	
	CPositionScore() : CPosition(), score(DEFAULT_SCORE) {}
	CPositionScore(uint64_t P, uint64_t O, int8_t score) : CPosition(P, O), score(score) {}
	CPositionScore(CPosition pos, int8_t score) : CPosition(pos), score(score) {}
	CPositionScore(iStreamArchive& arch) : CPosition(arch) { deserialize(arch); }
	explicit CPositionScore(const CPosition& pos) : CPosition(pos), score(DEFAULT_SCORE) {}

	virtual uint8_t ClassId() const { return classId; }
	virtual CPositionScore* Clone() const { return new CPositionScore(*this); }
	virtual void ResetInformation() { score = DEFAULT_SCORE; }
	virtual void PlayStone(uint8_t move) override { CPosition::PlayStone(move); ResetInformation(); }
	virtual bool Test() const override { return CPosition::Test() && (((score >= -64) && (score <= 64)) || (score == DEFAULT_SCORE)); }
	virtual bool IsSolved() const override { return score != DEFAULT_SCORE; }

	void Serialize(oStreamArchive& arch) const override { arch << classId; serialize(arch); }
	void Serialize(oPositionArchive& arch) const override { arch.Serialize(*this); }
	void Deserialize(iStreamArchive& arch) override { deserialize(arch); }

protected:
	void serialize(oStreamArchive& arch) const { CPosition::serialize(arch); arch << score; }
	void deserialize(iStreamArchive& arch) { arch >> score; }
};

/// Position with score, depth and selectivity
class CPositionScoreDepth : public CPositionScore
{
	static const  int8_t DEFAULT_DEPTH = -1;
	static const uint8_t DEFAULT_SELECTIVITY = 0;
public:
	static const uint8_t classId = 3;
	 int8_t depth;
	uint8_t selectivity;
	
	CPositionScoreDepth() : CPositionScore(), depth(DEFAULT_DEPTH), selectivity(DEFAULT_SELECTIVITY) {}
	CPositionScoreDepth(uint64_t P, uint64_t O, int8_t score, int8_t depth, uint8_t selectivity) : CPositionScore(P, O, score), depth(depth), selectivity(selectivity) {}
	CPositionScoreDepth(iStreamArchive& arch) : CPositionScore(arch) { deserialize(arch); }
	explicit CPositionScoreDepth(const CPosition& pos) : CPositionScore(pos), depth(DEFAULT_DEPTH), selectivity(DEFAULT_SELECTIVITY) {}
	explicit CPositionScoreDepth(const CPositionScore& pos) : CPositionScore(pos), depth(DEFAULT_DEPTH), selectivity(DEFAULT_SELECTIVITY) {}

	virtual uint8_t ClassId() const { return classId; }
	virtual CPositionScoreDepth* Clone() const { return new CPositionScoreDepth(*this); }
	virtual void ResetInformation() override { CPositionScore::ResetInformation(); depth = DEFAULT_DEPTH; selectivity = DEFAULT_SELECTIVITY; }
	virtual void PlayStone(uint8_t move) override { CPositionScore::PlayStone(move); ResetInformation(); }
	virtual bool Test() const override { return CPositionScore::Test() && (depth >= DEFAULT_DEPTH); }
	virtual bool IsSolved() const override { return (depth == static_cast<int8_t>(EmptyCount())) && (selectivity == 0); }
	virtual bool IsSolved(int8_t Depth, uint8_t Selectivity) const { return (depth >= Depth) || ((depth == Depth) && (selectivity <= Selectivity)); }

	void Serialize(oStreamArchive& arch) const override { arch << classId; serialize(arch); }
	void Serialize(oPositionArchive& arch) const override { arch.Serialize(*this); }
	void Deserialize(iStreamArchive& arch) override { deserialize(arch); }

protected:
	void serialize(oStreamArchive& arch) const { CPositionScore::serialize(arch); arch << depth << selectivity; }
	void deserialize(iStreamArchive& arch) { arch >> depth >> selectivity; }
};

/// Position with score for each depth
class CPositionFullScore : public CPosition
{
	static const int8_t DEFAULT_SCORE = -99;
public:
	static const uint8_t classId = 4;
	int8_t score[61];

	CPositionFullScore() : CPosition() { ResetInformation(); }
	CPositionFullScore(uint64_t P, uint64_t O) : CPosition(P, O) { ResetInformation(); }
	CPositionFullScore(iStreamArchive& arch) : CPosition(arch) { deserialize(arch); }
	explicit CPositionFullScore(const CPosition& pos) : CPosition(pos) { ResetInformation(); }

	virtual uint8_t ClassId() const { return classId; }
	virtual CPositionFullScore* Clone() const { return new CPositionFullScore(*this); }
	virtual void ResetInformation() { std::fill(std::begin(score), std::end(score), DEFAULT_SCORE); }
	virtual void PlayStone(uint8_t move) override { CPosition::PlayStone(move); ResetInformation(); }
	virtual bool Test() const override;

	virtual bool IsSolved() const override;
	virtual bool IsSolved(int8_t depth) const { return MaxSolvedDepth() >= depth; }
	int8_t MaxSolvedDepth() const;

	void Serialize(oStreamArchive& arch) const override { arch << classId; serialize(arch); }
	void Serialize(oPositionArchive& arch) const override { arch.Serialize(*this); }
	void Deserialize(iStreamArchive& arch) override { deserialize(arch); }

protected:
	void serialize(oStreamArchive& arch) const { CPosition::serialize(arch); for (const auto& it : score) arch << it; }
	void deserialize(iStreamArchive& arch) { for (auto& it : score) arch >> it; }
};

/// Position with score for each move
class CPositionAllScore :  public CPosition
{
	static const int8_t DEFAULT_SCORE = -99;
public:
	static const uint8_t classId = 5;
	int8_t score[64];
	
	CPositionAllScore() : CPosition() { ResetInformation(); }
	CPositionAllScore(uint64_t P, uint64_t O) : CPosition(P, O) { ResetInformation(); }
	CPositionAllScore(iStreamArchive& arch) : CPosition(arch) { deserialize(arch); }
	explicit CPositionAllScore(const CPosition& pos) : CPosition(pos) { ResetInformation(); }

	virtual uint8_t ClassId() const { return classId; }
	virtual CPositionAllScore* Clone() const { return new CPositionAllScore(*this); }
	virtual void ResetInformation() { std::fill(std::begin(score), std::end(score), DEFAULT_SCORE); }
	virtual void PlayStone(uint8_t move) override { CPosition::PlayStone(move); ResetInformation(); }
	virtual bool Test() const override;

	virtual bool IsSolved() const override;
	int8_t MaxScore() const { return *std::max_element(std::begin(score), std::end(score)); }

	void Serialize(oStreamArchive& arch) const override { arch << classId; serialize(arch); }
	void Serialize(oPositionArchive& arch) const override { arch.Serialize(*this); }
	void Deserialize(iStreamArchive& arch) override { deserialize(arch); }

protected:
	void serialize(oStreamArchive& arch) const { CPosition::serialize(arch); for (const auto& it : score) arch << it; }
	void deserialize(iStreamArchive& arch) { for (auto& it : score) arch >> it; }
	
private:
	std::vector<std::pair<int,int>> GetMoves() const; ///< std::pair(move, score)
};

inline bool comp (const CPosition& lhs, const CPosition& rhs) { return lhs.comp (rhs); }
inline bool equiv(const CPosition& lhs, const CPosition& rhs) { return lhs.equiv(rhs); }

namespace std
{
	template<> struct hash<CPosition>
	{
		inline std::size_t operator()(const CPosition& pos) const
		{
			return pos.P ^ ((pos.O << 32) | (pos.O >> 32));
		}
	};
}

inline void CPosition::PlayStone(uint8_t move)
{
	const auto flipped = flip(P, O, move);
	P ^= flipped ^ MakeBit(move);
	O ^= flipped;
	std::swap(P, O);
}

inline uint64_t CPosition::Parity() const
{
	uint64_t E = Empties();
	E ^= E >>  1;
	E ^= E >>  2;
	E ^= E >>  8;
	E ^= E >> 16;
	#ifdef HAS_PEXT
		return PExt(E, 0x0000001100000011ULL);
	#else
		E &= 0x0000001100000011ULL;
		E |= E >>  3;
		E |= E >> 30;
		return E & 0xFULL;
	#endif
}

std::vector<std::unique_ptr<CPosition>> LoadVector(const CPath& filename, std::size_t size = std::numeric_limits<std::size_t>::max());

void Save(const CPath& filename, const std::vector<std::unique_ptr<CPosition>>& positions);

template <typename ITERATOR>
inline void Save(const CPath& filename, ITERATOR begin, ITERATOR end)
{
	std::unique_ptr<oPositionArchive> archive;
	oArchive& fstream = fstreamArchive(filename);
	const auto ext = filename.GetExtension();

	if (ext == "pos")		archive = std::make_unique<StreamConverter>(fstream);
	if (ext == "script")	archive = std::make_unique<SingleLineStreamDecorator>(fstream);
	if (ext == "full")		archive = std::make_unique<SingleLineStreamDecorator>(fstream);
	if (ext == "obf")		archive = std::make_unique<SingleLineStreamDecorator>(fstream);

	for (auto it = begin; it != end; ++it)
		*archive << *it;
}

template <typename T>
inline std::vector<std::unique_ptr<T>> Transform(const std::vector<std::unique_ptr<CPosition>>& positions)
{
	std::vector<std::unique_ptr<CPosition>> ret;
	for (const auto& it : positions)
	{
		switch (T::classId) {
			case CPosition           ::classId: ret.push_back(std::make_unique<CPosition>(*it)); break;
			case CPositionScore      ::classId: ret.push_back(std::make_unique<CPositionScore>(*it)); break;
			case CPositionScoreDepth ::classId: ret.push_back(std::make_unique<CPositionScoreDepth>(*it)); break;
			case CPositionFullScore  ::classId: ret.push_back(std::make_unique<CPositionFullScore>(*it)); break;
			case CPositionAllScore   ::classId: ret.push_back(std::make_unique<CPositionAllScore>(*it)); break;
			default: throw std::runtime_error("Invalid class name");
		}
	}
	return ret;
}

template <typename T, typename ITERATOR>
inline void SaveTransform(const CPath& filename, ITERATOR begin, ITERATOR end)
{
	std::unique_ptr<oStreamArchive> archive;
	const auto ext = filename.GetExtension();

	if (ext == "pos") archive = std::make_unique<fstreamArchive>(filename);

	for (auto it = begin; it != end; ++it)
	{
		switch (T::classId) {
			case CPosition           ::classId: *archive << CPosition(*it); break;
			case CPositionScore      ::classId: *archive << CPositionScore(*it); break;
			case CPositionScoreDepth ::classId: *archive << CPositionScoreDepth(*it); break;
			case CPositionFullScore  ::classId: *archive << CPositionFullScore(*it); break;
			case CPositionAllScore   ::classId: *archive << CPositionAllScore(*it); break;
			default: throw std::runtime_error("Invalid class name");
		}
	}
}