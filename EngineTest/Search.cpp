#include "pch.h"
#include "Search.h"

class Search_Test : public Search::CAlgorithm
{
public:
	static int EvalGameOver(const CPosition& pos) { return CAlgorithm::EvalGameOver(pos); }
};

TEST(CAlgorithm, EvalGameOver)
{
	const auto pos1 = CPosition(0x0000000000000000ui64, 0x0000000000000000ui64); // Player: 0   Opponent: 0   Empty: 64   Score: 0-0=0
	const auto pos2 = CPosition(0x00000000000000FFui64, 0x0000000000000000ui64); // Player: 8   Opponent: 0   Empty: 56   Score: 8-0+56=64
	const auto pos3 = CPosition(0x00000000000000FFui64, 0xFF00000000000000ui64); // Player: 8   Opponent: 8   Empty: 48   Score: 8-8=0
	const auto pos4 = CPosition(0x00000000000000FFui64, 0xFF10000000000000ui64); // Player: 8   Opponent: 9   Empty: 47   Score: 8-9-47=-48
	const auto pos5 = CPosition(0x0000000000000000ui64, 0x1000000000000000ui64); // Player: 0   Opponent: 1   Empty: 63   Score: 0-1-63=-64
	const int score1 = 0;
	const int score2 = 64;
	const int score3 = 0;
	const int score4 = -48;
	const int score5 = -64;
	
	ASSERT_EQ(Search_Test::EvalGameOver(pos1), score1);
	ASSERT_EQ(Search_Test::EvalGameOver(pos2), score2);
	ASSERT_EQ(Search_Test::EvalGameOver(pos3), score3);
	ASSERT_EQ(Search_Test::EvalGameOver(pos4), score4);
	ASSERT_EQ(Search_Test::EvalGameOver(pos5), score5);
}