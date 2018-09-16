#pragma once
#include <cstdint>
#include "Position.h"
#include "Moves.h"

// Provides a save and easy to understand flip function.
// The function is slow and meant for testing and debugging.

uint64_t Flip_loop(const CPosition&, CMove);
