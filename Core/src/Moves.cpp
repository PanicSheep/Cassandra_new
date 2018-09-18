#include "Moves.h"

bool CMoves::HasMove(const CMove move) const
{
	return TestBit(m_moves, move);
}

CMove CMoves::PeekMove() const
{
	return static_cast<CMove>(BitScanLSB(m_moves));
}

CMove CMoves::ExtractMove()
{
	const CMove LSB = static_cast<CMove>(BitScanLSB(m_moves));
	RemoveLSB(m_moves);
	return LSB;
}

void CMoves::Remove(const CMove move)
{
	if (move != CMove::invalid)
		ResetBit(m_moves, move);
}

void CMoves::Remove(uint64_t moves)
{
	m_moves &= ~moves;
}

void CMoves::Filter(uint64_t moves)
{
	m_moves &= moves;
}
