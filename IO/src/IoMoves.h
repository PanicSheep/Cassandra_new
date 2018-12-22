#pragma once
#include "Moves.h"
#include <string>

namespace IO
{
	std::string to_string(CMove);

	CMove to_CMove(const std::string&);
}