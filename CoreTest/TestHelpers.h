#pragma once
#include <cstdint>
#include "Moves.h"
#include "Position.h"

CPosition MakeRandomPosition(uint64_t mask = 0xFFFFFFFFFFFFFFFFULL);
CPosition MakeFullRandomPosition(uint64_t mask = 0xFFFFFFFFFFFFFFFFULL);
