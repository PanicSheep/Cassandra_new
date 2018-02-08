#pragma once
#include <cstdint>
#include "MacrosHell.h"
#include "Position.h"
#include "Moves.h"

class CLastFlipCounter
{
public:
	CLastFlipCounter();

	uint8_t CountLastFlip(const CPosition&, const CMove&) const;
	
private:
	uint8_t CountFlip(uint8_t P, const CMove& move);
	
	uint8_t CLF_0[256];
	uint8_t CLF_1[256];
	uint8_t CLF_2[256];
	uint8_t CLF_3[256];
	uint8_t CLF_4[256];
	uint8_t CLF_5[256];
	uint8_t CLF_6[256];
	uint8_t CLF_7[256];
};
