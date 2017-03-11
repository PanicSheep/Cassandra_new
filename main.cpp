#include <iostream>
#include "flip_loop.h"

int main()
{
	std::cout << flip(0xFF00000000000080ULL, 0x00FFFFFFFFFFFF7EULL, 0) << std::endl;
	return 0;
}
