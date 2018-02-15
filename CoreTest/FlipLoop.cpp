#include "pch.h"
#include "FlipLoop.h"

// flips discs in one direction
uint64_t Flip_loop_dir(const CPosition& pos, const CMove& move, const int dX, const int dY)
{
	uint64_t flips = 0;
	int i = (move.field % 8) + dX; // Starting index in x direction
	int j = (move.field / 8) + dY; // Starting index in y direction

	while ((i >= 0) && (i < 8) && (j >= 0) && (j < 8)) // In between boundaries
	{
		const uint64_t bit = 1ULL << (j * 8 + i); // The bit to look at
		if (pos.GetO() & bit) // The bit belongs to the opponent
			flips |= bit; // Add to possible flips
		else if (pos.GetP() & bit) // The bit belongs to the player
			return flips; // All possible flips become real flips
		else // The bit belongs to no player
			return 0; // There are no possible flips
		i += dX; // Advance in direction
		j += dY; // Advance in direction
	}
	return 0;
}

uint64_t Flip_loop(const CPosition& pos, const CMove& move)
{
	return Flip_loop_dir(pos, move, -1, -1)
	     | Flip_loop_dir(pos, move, -1,  0)
	     | Flip_loop_dir(pos, move, -1, +1)
	     | Flip_loop_dir(pos, move,  0, -1)
	     | Flip_loop_dir(pos, move,  0, +1)
	     | Flip_loop_dir(pos, move, +1, -1)
	     | Flip_loop_dir(pos, move, +1,  0)
	     | Flip_loop_dir(pos, move, +1, +1);
}
