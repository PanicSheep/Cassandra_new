#include "Moves.h"

bool CMoves::HasMove(std::size_t index) const
{
	return TestBit(moves, index);
}

CMove CMoves::PeekMove() const
{
	const auto LSB = BitScanLSB(moves);
	return CMove(static_cast<Field>(LSB));
}

CMove CMoves::ExtractMove()
{
	const auto LSB = BitScanLSB(moves);
	RemoveLSB(moves);
	return CMove(static_cast<Field>(LSB));
}

CMove CMoves::ExtractMove(const std::size_t index)
{
	assert(index < size());
	const uint64_t MoveBit = PDep(MakeBit(index), moves);
	moves ^= MoveBit;
	return CMove(static_cast<Field>(BitScanLSB(MoveBit)));
}