#pragma once
#include <cstdint>
#include "Moves.h"
#include "Position.h"

uint64_t line(CMove move, const int dX, const int dY);

CPosition MakeRandomPosition(uint64_t mask = 0xFFFFFFFFFFFFFFFFui64);
CPosition MakeFullRandomPosition(uint64_t mask = 0xFFFFFFFFFFFFFFFFui64);
