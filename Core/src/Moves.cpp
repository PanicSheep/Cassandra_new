#include "Moves.h"

bool CMoves::HasMove(std::size_t index) const
{
	return TestBit(moves, index);
}

CMove CMoves::PeekMove() const
{
	return static_cast<CMove>(BitScanLSB(moves));
}

CMove CMoves::ExtractMove()
{
	const auto LSB = BitScanLSB(moves);
	RemoveLSB(moves);
	return static_cast<CMove>(LSB);
}

CMove CMoves::ExtractMove(const std::size_t index)
{
	assert(index < size());
	const uint64_t MoveBit = PDep(MakeBit(index), moves);
	moves ^= MoveBit;
	return static_cast<CMove>(BitScanLSB(MoveBit));
}