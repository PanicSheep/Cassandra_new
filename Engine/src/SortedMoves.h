#pragma once
#include <algorithm>
#include <cstdint>
#include "Position.h"
#include "Environment.h"
#include "HashtablePVS.h"

class CSortedMoves
{
	std::vector<std::pair<int32_t, CMove>> moves;
public:
	CSortedMoves() {}
	CSortedMoves(const CPosition&);
	CSortedMoves(const CPosition&, CMove filter1, CMove filter2);

	std::size_t size() const { return moves.size(); }
	bool empty() const { return moves.empty(); }

	CMove PeekMove() const { return moves.back().second; }
	CMove ExtractMove();

	auto begin() const { return moves.rbegin(); }
	auto end() const { return moves.rend(); }
private:
	int32_t Score(CMove, const CPosition&);
};

inline CSortedMoves::CSortedMoves(const CPosition& pos)
{
	CMoves mov = pos.PossibleMoves();
	moves.reserve(mov.size());

	while (!mov.empty()) {
		const auto move = mov.ExtractMove();
		moves.emplace_back(Score(move, pos), move);
	}

	std::sort(moves.begin(), moves.end(), [](auto& left, auto& right) { return left.first < right.first; });
}

inline CSortedMoves::CSortedMoves(const CPosition& pos, CMove filter1, CMove filter2)
{
	CMoves mov = pos.PossibleMoves();
	moves.reserve(mov.size());

	while (!mov.empty()) {
		const auto move = mov.ExtractMove();
		if (move == filter1) continue;
		if (move == filter2) continue;
		moves.emplace_back(Score(move, pos), move);
	}

	std::sort(moves.begin(), moves.end(), [](auto& left, auto& right) { return left.first < right.first; });
}

inline CMove CSortedMoves::ExtractMove()
{
	auto back = moves.back();
	moves.pop_back();
	return back.second;
}

inline uint64_t SMEAR_BITBOARD(uint64_t B)
{
	// 4x SHIFT, 4x OR, 2x AND
	// = 10 OPs
	B |= ((B >> 1) & 0x7F7F7F7F7F7F7F7FULL) | ((B << 1) & 0xFEFEFEFEFEFEFEFEULL);
	return B | (B >> 8) | (B << 8);
}

inline uint64_t OpponentsExposed(const uint64_t P, const uint64_t O) { return SMEAR_BITBOARD(~(P | O)) & O; } // 13 OPs

inline int32_t CSortedMoves::Score(CMove move, const CPosition& pos)
{
	static const uint64_t quadrant_mask[64] = {
		0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL,
		0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL,
		0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL,
		0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL,
		0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL,
		0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL,
		0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL,
		0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL,
	};
	static const int8_t FieldValue[64] = {
		9, 2, 8, 6, 6, 8, 2, 9,
		2, 1, 3, 4, 4, 3, 1, 2,
		8, 3, 7, 5, 5, 7, 3, 8,
		6, 4, 5, 0, 0, 5, 4, 6,
		6, 4, 5, 0, 0, 5, 4, 6,
		8, 3, 7, 5, 5, 7, 3, 8,
		2, 1, 3, 4, 4, 3, 1, 2,
		9, 2, 8, 6, 6, 8, 2, 9,
	};
	static const uint8_t ParityValue[16] = { 0, 20, 0, 10, 1, 10, 2, 10, 3, 5, 3, 4, 3, 4, 3, 4 };

	const auto NextPos = pos.Play(move);
	const auto PM = NextPos.PossibleMoves();
	const int32_t MobilityScore = static_cast<int32_t>(PM.size()) << 17;
	const int32_t CornerMobilityScore = ((PM.HasMove(A1) ? 1 : 0) + (PM.HasMove(A8) ? 1 : 0) + (PM.HasMove(H1) ? 1 : 0) + (PM.HasMove(H8) ? 1 : 0)) << 18;
	const int32_t OpponentsExposedScore = static_cast<int32_t>(PopCount(OpponentsExposed(NextPos.GetP(), NextPos.GetO()))) << 6;
	const int32_t FieldScore = FieldValue[move];
	const int32_t ParityScore = ParityValue[PopCount(pos.Empties() & quadrant_mask[move])];
	return FieldScore + ParityScore - MobilityScore - CornerMobilityScore - OpponentsExposedScore;
}
