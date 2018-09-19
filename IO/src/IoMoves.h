#pragma once
#include <string>
#include "Moves.h"

namespace IO
{
	std::string to_string(CMove);

	CMove to_CMove(const std::string&);
}