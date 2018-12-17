#pragma once
#include <algorithm>
#include <cstdint>
#include "Position.h"
#include "Engine.h"
#include "HashtablePVS.h"
#include "SearchInput.h"

class CSortedMoves
{
	std::vector<std::pair<int32_t, CMove>> moves;
public:
	CSortedMoves(const Search::CInput&);
	CSortedMoves(const Search::CInput&, CMove filter1, CMove filter2);

	std::size_t size() const { return moves.size(); }
	bool empty() const { return moves.empty(); }

	CMove PeekMove() const { return moves.back().second; }
	CMove ExtractMove();

	auto begin() const { return moves.rbegin(); }
	auto cbegin() const { return moves.crbegin(); }
	auto end() const { return moves.rend(); }
	auto cend() const { return moves.crend(); }
private:
	int32_t Score(CMove, const Search::CInput&);
};

inline CSortedMoves::CSortedMoves(const Search::CInput& in)
{
	CMoves mov = in.pos.PossibleMoves();
	moves.reserve(mov.size());

	while (!mov.empty()) {
		const auto move = mov.ExtractMove();
		moves.emplace_back(Score(move, in), move);
	}

	std::sort(moves.begin(), moves.end(), [](auto& left, auto& right) { return left.first < right.first; });
}

inline CSortedMoves::CSortedMoves(const Search::CInput& in, const CMove filter1, const CMove filter2)
{
	CMoves mov = in.pos.PossibleMoves();
	mov.Remove(filter1);
	mov.Remove(filter2);
	moves.reserve(mov.size());

	while (!mov.empty()) {
		const auto move = mov.ExtractMove();
		moves.emplace_back(Score(move, in), move);
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
	B |= ((B >> 1) & 0x7F7F7F7F7F7F7F7Fui64) | ((B << 1) & 0xFEFEFEFEFEFEFEFEui64);
	return B | (B >> 8) | (B << 8);
}

inline uint64_t OpponentsExposed(const uint64_t P, const uint64_t O) { return SMEAR_BITBOARD(~(P | O)) & O; } // 13 OPs

inline int32_t CSortedMoves::Score(CMove move, const Search::CInput& in)
{
	static const uint64_t QuadrantMask[64] = {
		0x000000000F0F0F0Fui64, 0x000000000F0F0F0Fui64, 0x000000000F0F0F0Fui64, 0x000000000F0F0F0Fui64, 0x00000000F0F0F0F0ui64, 0x00000000F0F0F0F0ui64, 0x00000000F0F0F0F0ui64, 0x00000000F0F0F0F0ui64,
		0x000000000F0F0F0Fui64, 0x000000000F0F0F0Fui64, 0x000000000F0F0F0Fui64, 0x000000000F0F0F0Fui64, 0x00000000F0F0F0F0ui64, 0x00000000F0F0F0F0ui64, 0x00000000F0F0F0F0ui64, 0x00000000F0F0F0F0ui64,
		0x000000000F0F0F0Fui64, 0x000000000F0F0F0Fui64, 0x000000000F0F0F0Fui64, 0x000000000F0F0F0Fui64, 0x00000000F0F0F0F0ui64, 0x00000000F0F0F0F0ui64, 0x00000000F0F0F0F0ui64, 0x00000000F0F0F0F0ui64,
		0x000000000F0F0F0Fui64, 0x000000000F0F0F0Fui64, 0x000000000F0F0F0Fui64, 0x000000000F0F0F0Fui64, 0x00000000F0F0F0F0ui64, 0x00000000F0F0F0F0ui64, 0x00000000F0F0F0F0ui64, 0x00000000F0F0F0F0ui64,
		0x0F0F0F0F00000000ui64, 0x0F0F0F0F00000000ui64, 0x0F0F0F0F00000000ui64, 0x0F0F0F0F00000000ui64, 0xF0F0F0F000000000ui64, 0xF0F0F0F000000000ui64, 0xF0F0F0F000000000ui64, 0xF0F0F0F000000000ui64,
		0x0F0F0F0F00000000ui64, 0x0F0F0F0F00000000ui64, 0x0F0F0F0F00000000ui64, 0x0F0F0F0F00000000ui64, 0xF0F0F0F000000000ui64, 0xF0F0F0F000000000ui64, 0xF0F0F0F000000000ui64, 0xF0F0F0F000000000ui64,
		0x0F0F0F0F00000000ui64, 0x0F0F0F0F00000000ui64, 0x0F0F0F0F00000000ui64, 0x0F0F0F0F00000000ui64, 0xF0F0F0F000000000ui64, 0xF0F0F0F000000000ui64, 0xF0F0F0F000000000ui64, 0xF0F0F0F000000000ui64,
		0x0F0F0F0F00000000ui64, 0x0F0F0F0F00000000ui64, 0x0F0F0F0F00000000ui64, 0x0F0F0F0F00000000ui64, 0xF0F0F0F000000000ui64, 0xF0F0F0F000000000ui64, 0xF0F0F0F000000000ui64, 0xF0F0F0F000000000ui64,
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

	const auto next_pos = in.pos.Play(move);
	const auto next_possible_moves = next_pos.PossibleMoves();
	const int32_t mobility_score = static_cast<int32_t>(next_possible_moves.size()) << 17;
	const int32_t corner_mobility_score = ((next_possible_moves.HasMove(A1) ? 1 : 0) + (next_possible_moves.HasMove(A8) ? 1 : 0) + (next_possible_moves.HasMove(H1) ? 1 : 0) + (next_possible_moves.HasMove(H8) ? 1 : 0)) << 18;
	const int32_t opponents_exposed_score = static_cast<int32_t>(PopCount(OpponentsExposed(next_pos.GetP(), next_pos.GetO()))) << 6;
	const int32_t field_score = FieldValue[move];
	const int32_t parity_score = ParityValue[PopCount(in.pos.Empties() & QuadrantMask[move])];
	return field_score + parity_score - mobility_score - corner_mobility_score - opponents_exposed_score;
}
