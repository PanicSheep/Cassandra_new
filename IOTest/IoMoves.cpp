#include "pch.h"

#include "IoMoves.h"

TEST(IoMoves, to_string)
{
	for (uint8_t i = 0; i < 65; i++)
	{
		const CMove in_move = static_cast<CMove>(i);
		const std::string str = IO::to_string(in_move);
		const CMove out_move = IO::to_CMove(str);

		ASSERT_EQ(in_move, out_move);
	}
}

TEST(IoMoves, to_CMove)
{
	ASSERT_EQ(IO::to_CMove("--"), CMove::invalid);

	ASSERT_EQ(IO::to_CMove(""), CMove::invalid);
	ASSERT_EQ(IO::to_CMove("??"), CMove::invalid);
}
