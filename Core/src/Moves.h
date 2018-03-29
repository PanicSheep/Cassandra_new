#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include "MacrosHell.h"

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
	invalid
};

class CMove
{
public:
	Field field;
	
	CMove(Field field) : field(field) {}
	bool operator == (const CMove& o) const { return field == o.field; }
	bool operator != (const CMove& o) const { return field != o.field; }
	bool operator == (const Field& o) const { return field == o; }
	bool operator != (const Field& o) const { return field != o; }
};

class CMoves
{
	uint64_t moves;
public:
	CMoves() : moves(0) {}
	CMoves(uint64_t moves) : moves(moves) {}
	CMoves(const CMoves& o, uint64_t filter) : moves(o.moves & filter) {} // TODO: Fix this hack!
	
	bool operator==(const CMoves& o) const { return moves == o.moves; }

	std::size_t size() const { return PopCount(moves); }
	bool empty() const { return moves == 0; }

	bool HasMove(std::size_t index) const;
	CMove PeekMove() const;
	CMove ExtractMove();
	CMove ExtractMove(std::size_t index);
};

inline bool CMoves::HasMove(std::size_t index) const
{
	return TestBit(moves, index);
}

inline CMove CMoves::PeekMove() const
{
	const auto LSB = BitScanLSB(moves);
	return CMove(static_cast<Field>(LSB));
}

inline CMove CMoves::ExtractMove()
{
	const auto LSB = BitScanLSB(moves);
	RemoveLSB(moves);
	return CMove(static_cast<Field>(LSB));
}

inline CMove CMoves::ExtractMove(const std::size_t index)
{
	assert(index < size());
	const uint64_t MoveBit = PDep(MakeBit(index), moves);
	moves ^= MoveBit;
	return CMove(static_cast<Field>(BitScanLSB(MoveBit)));
}