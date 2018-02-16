#pragma once
#include <cstdint>
#include "Moves.h"
#include "Position.h"

uint64_t line(const CMove& move, const int dX, const int dY);

CPosition MakeRandomPosition(uint64_t mask = 0xFFFFFFFFFFFFFFFFULL);
CPosition MakeFullRandomPosition(uint64_t mask = 0xFFFFFFFFFFFFFFFFULL);
