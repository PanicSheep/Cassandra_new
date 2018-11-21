#pragma once
#include <cstdint>
#include "Engine.h"
#include "Position.h"
#include "Moves.h"

class CLastFlipCounter : public ILastFlipCounter
{
public:
	CLastFlipCounter();

	uint8_t CountLastFlip(const CPosition&, CMove) const noexcept;

private:
	uint8_t CountFlip(uint8_t P, CMove);
	uint64_t Flip(const CPosition&, CMove);
	uint64_t Flip_dir(const CPosition&, CMove, const int dX);

	uint8_t CLF_0[256];
	uint8_t CLF_1[256];
	uint8_t CLF_2[256];
	uint8_t CLF_3[256];
	uint8_t CLF_4[256];
	uint8_t CLF_5[256];
	uint8_t CLF_6[256];
	uint8_t CLF_7[256];
};