#include "game.h"
#include "gtest/gtest.h"

TEST (GameTest, EvalGameOver)
{
	ASSERT_EQ (EvalGameOver(0x0000000000000000ULL, 64),   0); // Player: 0   Opponent:  0   Empty: 64   Score: 0-0=0
	ASSERT_EQ (EvalGameOver(0x00000000000000FFULL, 56),  64); // Player: 8   Opponent:  0   Empty: 56   Score: 8-0+56=64
	ASSERT_EQ (EvalGameOver(0x00000000000000FFULL, 48),   0); // Player: 8   Opponent:  8   Empty: 48   Score: 8-8=0
	ASSERT_EQ (EvalGameOver(0x00000000000000FFULL, 47), -48); // Player: 8   Opponent:  9   Empty: 47   Score: 8-9-47=-48
	ASSERT_EQ (EvalGameOver(0x0000000000000000ULL, 63), -64); // Player: 0   Opponent:  1   Empty: 63   Score: 0-1-63=-64
	
	ASSERT_EQ (EvalGameOver<64>(0x0000000000000000ULL),   0); // Player: 0   Opponent:  0   Empty: 64   Score: 0-0=0
	ASSERT_EQ (EvalGameOver<56>(0x00000000000000FFULL),  64); // Player: 8   Opponent:  0   Empty: 56   Score: 8-0+56=64
	ASSERT_EQ (EvalGameOver<48>(0x00000000000000FFULL),   0); // Player: 8   Opponent:  8   Empty: 48   Score: 8-8=0
	ASSERT_EQ (EvalGameOver<47>(0x00000000000000FFULL), -48); // Player: 8   Opponent:  9   Empty: 47   Score: 8-9-47=-48
	ASSERT_EQ (EvalGameOver<63>(0x0000000000000000ULL), -64); // Player: 0   Opponent:  1   Empty: 63   Score: 0-1-63=-64
}

TEST (NegaMaxTest, ZeroEmpties0)
{
	uint64_t NodeCounter = 0ULL;
	int score = Endgame_NegaMax::Eval(0xFF00FF00FF00FF00ULL, 0x00FF00FF00FF00FFULL, NodeCounter);
	ASSERT_EQ (score, 0);
	ASSERT_EQ (NodeCounter, 1u);
}

TEST (NegaMaxTest, ZeroEmpties1)
{
	uint64_t NodeCounter = 0ULL;
	int score = Endgame_NegaMax::Eval(0xFF00FF00FF00FFFFULL, 0x00FF00FF00FF0000ULL, NodeCounter);
	ASSERT_EQ (score, +16);
	ASSERT_EQ (NodeCounter, 1u);
}

TEST (NegaMaxTest, ZeroEmpties2)
{
	uint64_t NodeCounter = 0ULL;
	int score = Endgame_NegaMax::Eval(0xFF00FF00FF000000ULL, 0x00FF00FF00FFFFFFULL, NodeCounter);
	ASSERT_EQ (score, -16);
	ASSERT_EQ (NodeCounter, 1u);
}

// TODO: Implement test for NegaMax One Empties
// TODO: Implement test for NegaMax Two Empties
// TODO: Implement test for NegaMax Three Empties
// TODO: Implement test for NegaMax Four Empties
// TODO: Implement test for AlphaBeta One Empties
// TODO: Implement test for AlphaBeta Two Empties
// TODO: Implement test for AlphaBeta Three Empties
// TODO: Implement test for AlphaBeta Four Empties

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	
	return RUN_ALL_TESTS();
}
