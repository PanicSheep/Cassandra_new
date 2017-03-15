#pragma once
#include <stdint.h>

// Provides a save and easy to understand flip function.
// The function is slow and meant for testing and debugging.

uint64_t flip_loop(const uint64_t P, const uint64_t O, const uint8_t move);