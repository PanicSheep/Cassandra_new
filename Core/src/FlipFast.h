#pragma once
#include "MacrosHell.h"
#include "Moves.h"
#include "Position.h"

// The fastest flip function there is
namespace flip_fast
{
	extern uint64_t (*flip_fp[64])(const uint64_t, const uint64_t);
}

FORCE_INLINE uint64_t Flip(const CPosition& position, const CMove move) { return flip_fast::flip_fp[move.field](position.GetP(), position.GetO()); }