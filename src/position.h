#pragma once
#include "datamanipulation.h"
#include "flip_fast.h"
#include "possiblemoves.h"
#include "selectivity.h"
#include "utility.h"
#include "macros_hell.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#define PLAY_STONE_SWAP

/// Returns an int with either a '+' or a '-' prefix. Zero is represented '+0'.
inline std::string SignedInt(int score) { return std::string((score >= 0) ? "+" : "") + std::to_string(score); }

inline std::string field_name(const uint8_t move)
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
const uint8_t quadrant_id_2_bit[64] = {   
	0, 0, 0, 0, 1, 1, 1, 1,
	0, 0, 0, 0, 1, 1, 1, 1,
	0, 0, 0, 0, 1, 1, 1, 1,
	0, 0, 0, 0, 1, 1, 1, 1,
	2, 2, 2, 2, 3, 3, 3, 3,
	2, 2, 2, 2, 3, 3, 3, 3,
	2, 2, 2, 2, 3, 3, 3, 3,
	2, 2, 2, 2, 3, 3, 3, 3
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

uint64_t FlipDiagonal  (uint64_t b);
uint64_t FlipCodiagonal(uint64_t b);
uint64_t FlipVertical  (uint64_t b);
uint64_t FlipHorizontal(uint64_t b);

std::string board1D(const uint64_t P, const uint64_t O);
std::string board2D(const uint64_t P, const uint64_t O);
std::string board2D(const uint64_t P, const uint64_t O, const uint64_t possibleMoves);


const uint64_t START_POSITION_P = 0x0000000810000000ULL;
const uint64_t START_POSITION_O = 0x0000001008000000ULL;
const uint64_t START_POSITION_ETH_P = 0x0000001800000000ULL;
const uint64_t START_POSITION_ETH_O = 0x0000000018000000ULL;

inline uint64_t StartPositionP(const bool ETH) { return ETH ? START_POSITION_ETH_P : START_POSITION_P; }
inline uint64_t StartPositionO(const bool ETH) { return ETH ? START_POSITION_ETH_O : START_POSITION_O; }

inline void ResetPosition(uint64_t & P, uint64_t & O, const bool ETH)
{
	P = StartPositionP(ETH);
	O = StartPositionO(ETH);
}

FORCE_INLINE uint64_t EmptiesCount(const uint64_t P, const uint64_t O) { return PopCount(~(P | O)); }

inline uint64_t Parity(uint64_t E)
{
	#ifdef HAS_PEXT
		// 4 x SHIFT, 4 x XOR, 1 x PEXT
		// = 9 OPs
		E ^= E >>  1;
		E ^= E >>  2;
		E ^= E >>  8;
		E ^= E >> 16;
		return PExt(E, 0x0000001100000011ULL);
	#else
		// 6 x SHIFT, 4 x XOR, 2 x AND, 2 x OR 
		// = 14 OPs
		E ^= E >>  1;
		E ^= E >>  2;
		E ^= E >>  8;
		E ^= E >> 16;
		E &= 0x0000001100000011ULL;
		E |= E >>  3;
		E |= E >> 30;
		return E & 0xFULL;
	#endif
}
inline uint64_t Parity(const uint64_t P, const uint64_t O) { return Parity(~(P | O)); }

inline void PlayStone(uint64_t & P, uint64_t & O, const int move)
{
#ifdef PLAY_STONE_SWAP
	if (move < 64)
	{
		const uint64_t flipped = flip(P, O, move);
		P ^= flipped ^ (1ULL << move);
		O ^= flipped;
	}
	std::swap(P, O);
#else
	if (move < 64)
	{
		const uint64_t flipped = flip(P, O, move);
		O ^= P;                   // O' == O ^ (P)
		P ^= O ^ flipped;         // P' == P ^ (O ^ P ^ flipped) == O ^ flipped
		O ^= P ^ (1ULL << move);  // O' == O' ^ (P' ^ (1ULL << move)) == (O ^ P) ^ (O ^ flipped) ^ (1ULL << move) == P ^ flipped ^ (1ULL << move)
	}
	else
		std::swap(P, O);
#endif
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
//inline uint64_t StableStones_affectables(const uint64_t P, const uint64_t O) { return O & ~AFFECTABLE(~(P | O)); }
uint64_t StableStonesFullEdges(const uint64_t P, const uint64_t O);
uint64_t StableStonesFullEdgesSecondOrder(const uint64_t P, const uint64_t O);
uint64_t StableStonesTriangles(const uint64_t O);
uint64_t StableStonesSkyline(const uint64_t O);
uint64_t StableEdges(const uint64_t P, const uint64_t O);
uint64_t StableStonesPlayer(const uint64_t P, const uint64_t O);
//uint64_t StableStones(const uint64_t P, const uint64_t O);
//{
//	return StableStonesFullEdges(P, O) | StableStonesFullEdgesSecondOrder(P, O) | StableStonesTriangles(O);
//}

FORCE_INLINE uint64_t SMEAR_BITBOARD(uint64_t B)
{
	// 4x SHIFT, 4x OR, 2x AND 
	// = 10 OPs
	B |= (B >> 1) & 0x7F7F7F7F7F7F7F7FULL | (B << 1) & 0xFEFEFEFEFEFEFEFEULL;
	return B | (B >> 8) | (B << 8);
}

inline uint64_t   PlayersBoarder (const uint64_t P, const uint64_t O) { return SMEAR_BITBOARD(P    ) & ~(P | O); } // 13 OPs
inline uint64_t OpponentsBoarder (const uint64_t P, const uint64_t O) { return SMEAR_BITBOARD(    O) & ~(P | O); } // 13 OPs
inline uint64_t          Boarders(const uint64_t P, const uint64_t O) { return SMEAR_BITBOARD(P | O) & ~(P | O); } // 14 OPs

inline uint64_t   PlayersExposed (const uint64_t P, const uint64_t O) { return SMEAR_BITBOARD(~(P | O)) &  P     ; } // 13 OPs
inline uint64_t OpponentsExposed (const uint64_t P, const uint64_t O) { return SMEAR_BITBOARD(~(P | O)) &      O ; } // 13 OPs
inline uint64_t          Exposeds(const uint64_t P, const uint64_t O) { return SMEAR_BITBOARD(~(P | O)) & (P | O); } // 14 OPs

inline bool comp (const uint64_t P1, const uint64_t O1, const uint64_t P2, const uint64_t O2) { return (P1 < P2) || ((P1 == P2) && (O1 < O2)); }
inline bool equiv(const uint64_t P1, const uint64_t O1, const uint64_t P2, const uint64_t O2) { return (P1 == P2) && (O1 == O2); }

// forward declaration
class CPosition;
class CPositionScore;
class CPositionFullScore;
class CPositionScoreDepth;
class CPositionAllScore;

class CPosition
{
public:
	static const std::string FILENAME_EXTENSION; // .pos

	uint64_t P, O;
	
	inline CPosition();
	inline CPosition(uint64_t P, uint64_t O);
	inline CPosition(const bool ETH);
	inline explicit CPosition(const CPositionScore& o);
	inline explicit CPosition(const CPositionFullScore& o);
	inline explicit CPosition(const CPositionScoreDepth& o);
	inline explicit CPosition(const CPositionAllScore& o);
	
	inline void Reset();
	inline void Reset(const bool ETH);
	inline bool Test() const;

	inline uint64_t Empties() const;
	inline uint64_t EmptyCount() const;
	inline uint64_t Parity() const;
	inline uint64_t PossibleMoves() const;
	inline bool HasMoves() const;
	inline void PlayStone(const int move);
	
	std::string to_string_1D() const;
	std::string to_string_2D() const;
	std::string to_string_2D_PM() const;
	
	inline bool operator==(const CPosition& other) const;
	inline bool operator!=(const CPosition& other) const;
	inline bool operator<=(const CPosition& other) const;
	inline bool operator>=(const CPosition& other) const;
	inline bool operator< (const CPosition& other) const;
	inline bool operator> (const CPosition& other) const;
	
	inline bool  comp(const CPosition& other) const { return ::comp (P, O, other.P, other.O); }
	inline bool equiv(const CPosition& other) const { return ::equiv(P, O, other.P, other.O); }
		
	inline void FlipCodiagonal();
	inline void FlipDiagonal();
	inline void FlipHorizontal();
	inline void FlipVertical();
	
	void FlipToMin(); ///< Flip to minimum of all flips
};

/// Position with score
class CPositionScore
{
public:
	static const std::string FILENAME_EXTENSION; // .psc
	static const int8_t DEFAULT_SCORE = -99;

	uint64_t P, O;
	int8_t score;
	
	inline CPositionScore();
	inline CPositionScore(uint64_t P, uint64_t O, int8_t score);
	inline CPositionScore(const bool ETH);
	inline explicit CPositionScore(const CPosition& o);
	inline explicit CPositionScore(const CPositionFullScore& o);
	inline explicit CPositionScore(const CPositionScoreDepth& o);
	inline explicit CPositionScore(const CPositionAllScore& o);
		
	inline void Reset();
	inline void Reset(const bool ETH);
	inline void ResetInformation();
	inline bool Test() const;

	inline uint64_t Empties() const;
	inline uint64_t EmptyCount() const;
	inline uint64_t Parity() const;
	inline uint64_t PossibleMoves() const;
	inline bool HasMoves() const;
	inline void PlayStone(const int move, const int8_t newScore = DEFAULT_SCORE);

	inline bool IsSolved() const;
	
	inline bool  comp(const CPositionScore& other) const { return ::comp (P, O, other.P, other.O); }
	inline bool equiv(const CPositionScore& other) const { return ::equiv(P, O, other.P, other.O); }
	
	std::string to_string_1D() const;
	std::string to_string_2D() const;
	std::string to_string_2D_PM() const;
};

/// Position with score for each depth
class CPositionFullScore
{
public:
	static const std::string FILENAME_EXTENSION; // .pfs
	static const int8_t DEFAULT_SCORE = -99;

	uint64_t P, O;
	int8_t score[61];
	
	inline CPositionFullScore();
	inline CPositionFullScore(uint64_t P, uint64_t O);
	inline CPositionFullScore(const bool ETH);
	inline explicit CPositionFullScore(const CPosition& o);
	inline explicit CPositionFullScore(const CPositionScore& o);
	inline explicit CPositionFullScore(const CPositionScoreDepth& o);
	inline explicit CPositionFullScore(const CPositionAllScore& o);

	inline void Reset();
	inline void Reset(const bool ETH);
	inline void ResetInformation();
	bool Test() const;

	inline uint64_t Empties() const;
	inline uint64_t EmptyCount() const;
	inline uint64_t Parity() const;
	inline uint64_t PossibleMoves() const;
	inline bool HasMoves() const;
	inline void PlayStone(const int move);

	bool IsSolved() const;
	bool IsSolved(const int8_t depth) const;
	int8_t MaxSolvedDepth() const;
	
	inline bool  comp(const CPositionFullScore& other) const { return ::comp (P, O, other.P, other.O); }
	inline bool equiv(const CPositionFullScore& other) const { return ::equiv(P, O, other.P, other.O); }
	
	std::string to_string_1D() const;
	std::string to_string_2D() const;
	std::string to_string_2D_PM() const;
};

/// Position with score, depth and selectivity
class CPositionScoreDepth
{
public:
	static const std::string FILENAME_EXTENSION; // .psd
	static const  int8_t DEFAULT_SCORE = -99;
	static const  int8_t DEFAULT_DEPTH = -1;
	static const uint8_t DEFAULT_SELECTIVITY = 0;

	uint64_t P, O;
	 int8_t score;
	 int8_t depth;
	uint8_t selectivity;
	
	inline CPositionScoreDepth(uint64_t P, uint64_t O, int8_t score, int8_t depth, uint8_t selectivity);
	inline CPositionScoreDepth();
	inline CPositionScoreDepth(const bool ETH);
	inline explicit CPositionScoreDepth(const CPosition& o);
	inline explicit CPositionScoreDepth(const CPositionScore& o);
	inline explicit CPositionScoreDepth(const CPositionFullScore& o);
	inline explicit CPositionScoreDepth(const CPositionAllScore& o);
	
	inline void Reset();
	inline void Reset(const bool ETH);
	inline void ResetInformation();
	inline bool Test() const;

	inline uint64_t Empties() const;
	inline uint64_t EmptyCount() const;
	inline uint64_t Parity() const;
	inline uint64_t PossibleMoves() const;
	inline bool HasMoves() const;
	inline void PlayStone(const int move, const int8_t newScore = DEFAULT_SCORE, const int8_t newDepth = DEFAULT_DEPTH, const uint8_t newSelectivity = DEFAULT_SELECTIVITY);

	inline bool IsSolved() const;
	inline bool IsSolved(const int8_t Depth, const uint8_t Selectivity) const;
	
	inline bool  comp(const CPositionScoreDepth& other) const { return ::comp (P, O, other.P, other.O); }
	inline bool equiv(const CPositionScoreDepth& other) const { return ::equiv(P, O, other.P, other.O); }
	
	std::string to_string_1D() const;
	std::string to_string_2D() const;
	std::string to_string_2D_PM() const;
	
	std::string GetDepthSelectivity() const;
	std::string GetScoreDepthSelectivity() const;
};

/// Position with score for each move
class CPositionAllScore
{
public:
	static const std::string FILENAME_EXTENSION; // .pas
	static const int8_t DEFAULT_SCORE = -99;

	uint64_t P, O;
	int8_t score[64];
	
	inline CPositionAllScore();
	inline CPositionAllScore(uint64_t P, uint64_t O);
	inline CPositionAllScore(const bool ETH);
	inline CPositionAllScore(std::string s);
	inline explicit CPositionAllScore(const CPosition& o);
	inline explicit CPositionAllScore(const CPositionScore& o);
	inline explicit CPositionAllScore(const CPositionFullScore& o);
	inline explicit CPositionAllScore(const CPositionScoreDepth& o);
	
	inline void Reset();
	inline void Reset(const bool ETH);
	inline void ResetInformation();
	bool Test() const;

	inline uint64_t Empties() const;
	inline uint64_t EmptyCount() const;
	inline uint64_t Parity() const;
	inline uint64_t PossibleMoves() const;
	inline bool HasMoves() const;
	inline void PlayStone(const int move);

	inline bool IsSolved() const;
	inline int8_t MaxScore() const;
	
	inline bool  comp(const CPositionAllScore& other) const { return ::comp (P, O, other.P, other.O); }
	inline bool equiv(const CPositionAllScore& other) const { return ::equiv(P, O, other.P, other.O); }

	std::string to_string_1D() const;
	std::string to_string_2D() const;
	std::string to_string_2D_PM() const;
		
private:
	std::vector<std::pair<int,int>> GetMoves() const; ///< std::pair(move, score)
};

inline bool  comp(const CPosition& lhs, const CPosition& rhs) { return  ::comp(lhs.P, lhs.O, rhs.P, rhs.O); }
inline bool equiv(const CPosition& lhs, const CPosition& rhs) { return ::equiv(lhs.P, lhs.O, rhs.P, rhs.O); }
inline bool  comp(const CPositionScore& lhs, const CPositionScore& rhs) { return  ::comp(lhs.P, lhs.O, rhs.P, rhs.O); }
inline bool equiv(const CPositionScore& lhs, const CPositionScore& rhs) { return ::equiv(lhs.P, lhs.O, rhs.P, rhs.O); }
inline bool  comp(const CPositionFullScore& lhs, const CPositionFullScore& rhs) { return  ::comp(lhs.P, lhs.O, rhs.P, rhs.O); }
inline bool equiv(const CPositionFullScore& lhs, const CPositionFullScore& rhs) { return ::equiv(lhs.P, lhs.O, rhs.P, rhs.O); }
inline bool  comp(const CPositionScoreDepth& lhs, const CPositionScoreDepth& rhs) { return  ::comp(lhs.P, lhs.O, rhs.P, rhs.O); }
inline bool equiv(const CPositionScoreDepth& lhs, const CPositionScoreDepth& rhs) { return ::equiv(lhs.P, lhs.O, rhs.P, rhs.O); }
inline bool  comp(const CPositionAllScore& lhs, const CPositionAllScore& rhs) { return  ::comp(lhs.P, lhs.O, rhs.P, rhs.O); }
inline bool equiv(const CPositionAllScore& lhs, const CPositionAllScore& rhs) { return ::equiv(lhs.P, lhs.O, rhs.P, rhs.O); }

bool HasValidFilenameExtension(const std::string& filename);


std::vector<CPositionAllScore> read_vector_OBF(const std::string& filename, std::size_t size = 0xFFFFFFFFFFFFFFFFULL);


// ################################################################################################
// ------------------------------------------------------------------------------------------------
/// Loads a vector of type T from 'filename'. Displays a warning if they don't match.
template <typename T> std::vector<T> LoadVector(const std::string& filename, std::size_t size = 0xFFFFFFFFFFFFFFFFULL)
{
	if (filename.substr(filename.rfind(".") + 1) != T::FILENAME_EXTENSION)
		std::cerr << "WARNING: Filename extension does not match data type when loading." << std::endl;

	return read_vector<T>(filename, size);
}

template <> std::vector<CPositionAllScore> LoadVector(const std::string& filename, std::size_t size);
// ------------------------------------------------------------------------------------------------
// ################################################################################################


// ################################################################################################
// ------------------------------------------------------------------------------------------------
/// Saves a vector of type T to 'filename'. Displays a warning if they don't match.
template <typename T> void SaveVector(const std::string& filename, const std::vector<T>& vec)
{
	if (filename.substr(filename.rfind(".") + 1) != T::FILENAME_EXTENSION)
		std::cerr << "WARNING: Filename extension does not match data type when saving." << std::endl;

	write_to_file(filename, vec);
}

template <> void SaveVector<CPositionAllScore>(const std::string& filename, const std::vector<CPositionAllScore>& vec);
// ------------------------------------------------------------------------------------------------
// ################################################################################################

template <typename ITERATOR>
void SaveTransform(const std::string& filename, ITERATOR begin, ITERATOR end)
{
	std::string filename_extension = filename.substr(filename.rfind(".") + 1);

	if (filename_extension == CPosition          ::FILENAME_EXTENSION)		SaveVector(filename, std::vector<CPosition          >(begin, end));
	if (filename_extension == CPositionScore     ::FILENAME_EXTENSION)		SaveVector(filename, std::vector<CPositionScore     >(begin, end));
	if (filename_extension == CPositionFullScore ::FILENAME_EXTENSION)		SaveVector(filename, std::vector<CPositionFullScore >(begin, end));
	if (filename_extension == CPositionScoreDepth::FILENAME_EXTENSION)		SaveVector(filename, std::vector<CPositionScoreDepth>(begin, end));
	if (filename_extension == CPositionAllScore  ::FILENAME_EXTENSION)		SaveVector(filename, std::vector<CPositionAllScore  >(begin, end));
	if (filename_extension == "obf")										SaveVector(filename, std::vector<CPositionAllScore  >(begin, end));
}

/// Saves a vector to 'filename' and transforms it if needed.
template <typename T> void SaveTransform(const std::string& filename, const std::vector<T>& vec) { SaveTransform(filename, vec.begin(), vec.end()); }

/// Loads a vector from 'filename' and transforms it to type T if needed.
template <typename T> std::vector<T> LoadTransform(const std::string& filename)
{
	std::string filename_extension = filename.substr(filename.rfind(".") + 1);

	if (filename_extension == CPosition          ::FILENAME_EXTENSION)		{ auto vec = LoadVector<CPosition          >(filename); return std::vector<T>(vec.begin(), vec.end()); }
	if (filename_extension == CPositionScore     ::FILENAME_EXTENSION)		{ auto vec = LoadVector<CPositionScore     >(filename); return std::vector<T>(vec.begin(), vec.end()); } 
	if (filename_extension == CPositionFullScore ::FILENAME_EXTENSION)		{ auto vec = LoadVector<CPositionFullScore >(filename); return std::vector<T>(vec.begin(), vec.end()); } 
	if (filename_extension == CPositionScoreDepth::FILENAME_EXTENSION)		{ auto vec = LoadVector<CPositionScoreDepth>(filename); return std::vector<T>(vec.begin(), vec.end()); } 
	if (filename_extension == CPositionAllScore  ::FILENAME_EXTENSION)		{ auto vec = LoadVector<CPositionAllScore  >(filename); return std::vector<T>(vec.begin(), vec.end()); } 
	if (filename_extension == "obf")										{ auto vec = LoadVector<CPositionAllScore  >(filename); return std::vector<T>(vec.begin(), vec.end()); } 
}


// ################################################################################################
// Inline section
// ################################################################################################
// ------------------------------------------------------------------------------------------------

// ################################################################################################
//  CPosition
// ################################################################################################
// ------------------------------------------------------------------------------------------------
CPosition::CPosition() : CPosition(0, 0) {}
CPosition::CPosition(uint64_t P, uint64_t O) : P(P), O(O) {}
CPosition::CPosition(const bool ETH) : CPosition(StartPositionP(ETH), StartPositionO(ETH)) {}
CPosition::CPosition(const CPositionScore& o) : CPosition(o.P, o.O) {}
CPosition::CPosition(const CPositionFullScore& o) : CPosition(o.P, o.O) {}
CPosition::CPosition(const CPositionScoreDepth& o) : CPosition(o.P, o.O) {}
CPosition::CPosition(const CPositionAllScore& o) : CPosition(o.P, o.O) {}

void CPosition::Reset() { P = 0; O = 0; }
void CPosition::Reset(const bool ETH) { ResetPosition(P, O, ETH); }
bool CPosition::Test() const { return (P & O) == 0; }

uint64_t CPosition::Empties() const { return ~(P | O); }
uint64_t CPosition::EmptyCount() const { return PopCount(Empties()); }
uint64_t CPosition::Parity() const { return ::Parity(Empties()); }
uint64_t CPosition::PossibleMoves() const { return ::PossibleMoves(P, O); }
bool CPosition::HasMoves() const { return ::HasMoves(P, O); }
void CPosition::PlayStone(const int move) { ::PlayStone(P, O, move); }

bool CPosition::operator==(const CPosition& other) const { return (this->P == other.P) && (this->O == other.O); }
bool CPosition::operator!=(const CPosition& other) const { return (this->P != other.P) || (this->O != other.O); }
bool CPosition::operator<=(const CPosition& other) const { return (this->P <= other.P) || ((this->P == other.P) && (this->O <= other.O)); }
bool CPosition::operator>=(const CPosition& other) const { return (this->P >= other.P) || ((this->P == other.P) && (this->O >= other.O)); }
bool CPosition::operator< (const CPosition& other) const { return (this->P <  other.P) || ((this->P == other.P) && (this->O <  other.O)); }
bool CPosition::operator> (const CPosition& other) const { return (this->P >  other.P) || ((this->P == other.P) && (this->O >  other.O)); }

void CPosition::FlipCodiagonal() { ::FlipCodiagonal(P); ::FlipCodiagonal(O); }
void CPosition::FlipDiagonal() { ::FlipDiagonal(P); ::FlipDiagonal(O); }
void CPosition::FlipHorizontal() { ::FlipHorizontal(P); ::FlipHorizontal(O); }
void CPosition::FlipVertical() { ::FlipVertical(P); ::FlipVertical(O); }
// ------------------------------------------------------------------------------------------------
// ################################################################################################


// ################################################################################################
//  CPositionScore
// ################################################################################################
// ------------------------------------------------------------------------------------------------
CPositionScore::CPositionScore() : CPositionScore(0, 0, DEFAULT_SCORE) {}
CPositionScore::CPositionScore(uint64_t P, uint64_t O, int8_t score) : P(P), O(O), score(score) {}
CPositionScore::CPositionScore(const bool ETH) : CPositionScore(StartPositionP(ETH), StartPositionO(ETH), DEFAULT_SCORE) {}
CPositionScore::CPositionScore(const CPosition& o) : CPositionScore(o.P, o.O, DEFAULT_SCORE) {}
CPositionScore::CPositionScore(const CPositionFullScore& o) : CPositionScore(o.P, o.O, o.score[o.MaxSolvedDepth()]) {}
CPositionScore::CPositionScore(const CPositionScoreDepth& o) : CPositionScore(o.P, o.O, o.score) {}
CPositionScore::CPositionScore(const CPositionAllScore& o) : CPositionScore(o.P, o.O, o.MaxScore()) {}

void CPositionScore::Reset() { P = 0; O = 0; }
void CPositionScore::Reset(const bool ETH) { ResetPosition(P, O, ETH); ResetInformation(); }
void CPositionScore::ResetInformation() { score = DEFAULT_SCORE; }
bool CPositionScore::Test() const { return ((P & O) == 0) && (((score >= -64) && (score <= 64)) || (score == DEFAULT_SCORE)); }

uint64_t CPositionScore::Empties() const { return ~(P | O); }
uint64_t CPositionScore::EmptyCount() const { return PopCount(Empties()); }
uint64_t CPositionScore::Parity() const { return ::Parity(Empties()); }
uint64_t CPositionScore::PossibleMoves() const { return ::PossibleMoves(P, O); }
bool CPositionScore::HasMoves() const { return ::HasMoves(P, O); }
void CPositionScore::PlayStone(const int move, const int8_t newScore) { ::PlayStone(P, O, move); score = newScore; }

bool CPositionScore::IsSolved() const { return score != DEFAULT_SCORE; }
// ------------------------------------------------------------------------------------------------
// ################################################################################################


// ################################################################################################
//  CPositionFullScore
// ################################################################################################
// ------------------------------------------------------------------------------------------------
CPositionFullScore::CPositionFullScore() : CPositionFullScore(0, 0) {}
CPositionFullScore::CPositionFullScore(uint64_t P, uint64_t O) : P(P), O(O) { ResetInformation(); }
CPositionFullScore::CPositionFullScore(const bool ETH) : CPositionFullScore(StartPositionP(ETH), StartPositionO(ETH)) {}
CPositionFullScore::CPositionFullScore(const CPosition& o) : CPositionFullScore(o.P, o.O) {}
CPositionFullScore::CPositionFullScore(const CPositionScore& o) : CPositionFullScore(o.P, o.O) { score[o.EmptyCount()] = o.score; }
CPositionFullScore::CPositionFullScore(const CPositionScoreDepth& o) : CPositionFullScore(o.P, o.O) { if (o.selectivity == 0) score[o.depth] = o.score; }
CPositionFullScore::CPositionFullScore(const CPositionAllScore& o) : CPositionFullScore(o.P, o.O) { score[o.EmptyCount()] = o.MaxScore(); }

void CPositionFullScore::Reset() { P = 0; O = 0; ResetInformation(); }
void CPositionFullScore::Reset(const bool ETH) { ResetPosition(P, O, ETH); ResetInformation(); }

inline void CPositionFullScore::ResetInformation() { for (int i = 0; i < 61; i++) score[i] = DEFAULT_SCORE; }

uint64_t CPositionFullScore::Empties() const { return ~(P | O); }
uint64_t CPositionFullScore::EmptyCount() const { return PopCount(Empties()); }
uint64_t CPositionFullScore::Parity() const { return ::Parity(Empties()); }
uint64_t CPositionFullScore::PossibleMoves() const { return ::PossibleMoves(P, O); }
bool CPositionFullScore::HasMoves() const { return ::HasMoves(P, O); }
void CPositionFullScore::PlayStone(const int move) { ::PlayStone(P, O, move); ResetInformation(); }

// ------------------------------------------------------------------------------------------------
// ################################################################################################



// ################################################################################################
//  CPositionScoreDepth
// ################################################################################################
// ------------------------------------------------------------------------------------------------
CPositionScoreDepth::CPositionScoreDepth() : CPositionScoreDepth(0, 0, DEFAULT_SCORE, DEFAULT_DEPTH, DEFAULT_SELECTIVITY) {}
CPositionScoreDepth::CPositionScoreDepth(uint64_t P, uint64_t O, int8_t score, int8_t depth, uint8_t selectivity) : P(P), O(O), score(score), depth(depth), selectivity(selectivity) {}
CPositionScoreDepth::CPositionScoreDepth(const bool ETH) : CPositionScoreDepth(StartPositionP(ETH), StartPositionO(ETH), DEFAULT_SCORE, DEFAULT_DEPTH, DEFAULT_SELECTIVITY) {}
CPositionScoreDepth::CPositionScoreDepth(const CPosition& o) : CPositionScoreDepth(o.P, o.O, DEFAULT_SCORE, DEFAULT_DEPTH, DEFAULT_SELECTIVITY) {}
CPositionScoreDepth::CPositionScoreDepth(const CPositionScore& o) : CPositionScoreDepth(o.P, o.O, o.score, o.IsSolved() ? o.EmptyCount() : DEFAULT_DEPTH, o.IsSolved() ? 0 : DEFAULT_SELECTIVITY) {}
CPositionScoreDepth::CPositionScoreDepth(const CPositionFullScore& o) : CPositionScoreDepth(o.P, o.O, o.IsSolved() ? o.score[o.MaxSolvedDepth()] : DEFAULT_SCORE, o.IsSolved() ? o.EmptyCount() : DEFAULT_DEPTH, o.IsSolved() ? 0 : DEFAULT_SELECTIVITY) {}
CPositionScoreDepth::CPositionScoreDepth(const CPositionAllScore& o) : CPositionScoreDepth(o.P, o.O, o.IsSolved() ? o.MaxScore() : DEFAULT_SCORE, o.IsSolved() ? o.EmptyCount() : DEFAULT_DEPTH, o.IsSolved() ? 0 : DEFAULT_SELECTIVITY) {}

void CPositionScoreDepth::Reset() { P = 0; O = 0; ResetInformation(); }
void CPositionScoreDepth::Reset(const bool ETH) { ResetPosition(P, O, ETH); ResetInformation(); }
void CPositionScoreDepth::ResetInformation() { score = DEFAULT_SCORE; depth = DEFAULT_DEPTH; selectivity = DEFAULT_SELECTIVITY; }
bool CPositionScoreDepth::Test() const { return ((P & O) == 0) && (((score >= -64) && (score <= 64)) || (score == DEFAULT_SCORE)) && (depth >= DEFAULT_DEPTH); }

uint64_t CPositionScoreDepth::Empties() const { return ~(P | O); }
uint64_t CPositionScoreDepth::EmptyCount() const { return PopCount(Empties()); }
uint64_t CPositionScoreDepth::Parity() const { return ::Parity(Empties()); }
uint64_t CPositionScoreDepth::PossibleMoves() const { return ::PossibleMoves(P, O); }
bool CPositionScoreDepth::HasMoves() const { return ::HasMoves(P, O); }
void CPositionScoreDepth::PlayStone(const int move, const int8_t newScore, const int8_t newDepth, const uint8_t newSelectivity) { ::PlayStone(P, O, move); score = newScore; depth = newDepth; selectivity = newSelectivity; }

bool CPositionScoreDepth::IsSolved() const { return (depth == EmptyCount()) && (selectivity == 0); }
bool CPositionScoreDepth::IsSolved(const int8_t Depth, const uint8_t Selectivity) const { return (depth >= Depth) || ((depth == Depth) && (selectivity <= Selectivity)); }
// ------------------------------------------------------------------------------------------------
// ################################################################################################



// ################################################################################################
//  CPositionAllScore
// ################################################################################################
// ------------------------------------------------------------------------------------------------
CPositionAllScore::CPositionAllScore() : CPositionAllScore(0, 0) {}
CPositionAllScore::CPositionAllScore(uint64_t P, uint64_t O) : P(P), O(O) { ResetInformation(); }
CPositionAllScore::CPositionAllScore(const bool ETH) : CPositionAllScore(StartPositionP(ETH), StartPositionO(ETH)) {}
CPositionAllScore::CPositionAllScore(const CPosition& o) : CPositionAllScore(o.P, o.O) {}
CPositionAllScore::CPositionAllScore(const CPositionScore& o) : CPositionAllScore(o.P, o.O) {}
CPositionAllScore::CPositionAllScore(const CPositionFullScore& o) : CPositionAllScore(o.P, o.O) {}
CPositionAllScore::CPositionAllScore(const CPositionScoreDepth& o) : CPositionAllScore(o.P, o.O) {}

void CPositionAllScore::Reset() { P = 0; O = 0; ResetInformation(); }
void CPositionAllScore::Reset(const bool ETH) { ResetPosition(P, O, ETH); ResetInformation(); }
void CPositionAllScore::ResetInformation() { for (int i = 0; i < 64; i++) score[i] = DEFAULT_SCORE; }

uint64_t CPositionAllScore::Empties() const { return ~(P | O); }
uint64_t CPositionAllScore::EmptyCount() const { return PopCount(Empties()); }
uint64_t CPositionAllScore::Parity() const { return ::Parity(Empties()); }
uint64_t CPositionAllScore::PossibleMoves() const { return ::PossibleMoves(P, O); }
bool CPositionAllScore::HasMoves() const { return ::HasMoves(P, O); }
void CPositionAllScore::PlayStone(const int move) { ::PlayStone(P, O, move); ResetInformation(); }

bool CPositionAllScore::IsSolved() const { uint64_t pm = PossibleMoves(); while (pm) { if (score[BitScanLSB(pm)] == DEFAULT_SCORE) return false; RemoveLSB(pm); } return true; }
int8_t CPositionAllScore::MaxScore() const { return *std::max_element(score, score + 64); }

// ------------------------------------------------------------------------------------------------
// ################################################################################################

// ------------------------------------------------------------------------------------------------
// ################################################################################################