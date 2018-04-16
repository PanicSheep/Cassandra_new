#include "pch.h"
#include "Search.h"

TEST(SearchTest, SMEAR_BITBOARD)
{
	for (int move = 0; move < 64; move++)
	{
		uint64_t board = 0;
		int x = move / 8;
		int y = move % 8;

		for (int dx = -1; dx <= 1; dx++)
			for (int dy = -1; dy <= 1; dy++)
				if ((x + dx >= 0) && (x + dx < 8) && (y + dy >= 0) && (y + dy < 8))
					board |= 1ULL << ((x + dx) * 8 + (y + dy));

		ASSERT_EQ(board, SMEAR_BITBOARD(1ULL << move));
	}
}

TEST(SearchTest, neighbour)
{
	for (int move = 0; move < 64; move++)
		ASSERT_EQ(Neighbour(static_cast<Field>(move)), SMEAR_BITBOARD(1ULL << move) ^ (1ULL << move));
}

class Search_Test : public Search
{
public:
	template <int EmptyCount>
	static int EvalGameOver(const CPosition& pos) { return Search::EvalGameOver<EmptyCount>(pos); }
	static int EvalGameOver(const CPosition& pos) { return Search::EvalGameOver(pos); }
	static int EvalGameOver(const CPosition& pos, const uint64_t EmptyCount) { return Search::EvalGameOver(pos, EmptyCount); }
};

TEST(SearchTest, EvalGameOver)
{
	const auto pos1 = CPosition(0x0000000000000000ULL, 0x0000000000000000ULL); // Player: 0   Opponent: 0   Empty: 64   Score: 0-0=0
	const auto pos2 = CPosition(0x00000000000000FFULL, 0x0000000000000000ULL); // Player: 8   Opponent: 0   Empty: 56   Score: 8-0+56=64
	const auto pos3 = CPosition(0x00000000000000FFULL, 0xFF00000000000000ULL); // Player: 8   Opponent: 8   Empty: 48   Score: 8-8=0
	const auto pos4 = CPosition(0x00000000000000FFULL, 0xFF10000000000000ULL); // Player: 8   Opponent: 9   Empty: 47   Score: 8-9-47=-48
	const auto pos5 = CPosition(0x0000000000000000ULL, 0x1000000000000000ULL); // Player: 0   Opponent: 1   Empty: 63   Score: 0-1-63=-64
	const int score1 = 0;
	const int score2 = 64;
	const int score3 = 0;
	const int score4 = -48;
	const int score5 = -64;

	ASSERT_EQ(Search_Test::EvalGameOver(pos1, 64), score1);
	ASSERT_EQ(Search_Test::EvalGameOver(pos2, 56), score2);
	ASSERT_EQ(Search_Test::EvalGameOver(pos3, 48), score3);
	ASSERT_EQ(Search_Test::EvalGameOver(pos4, 47), score4);
	ASSERT_EQ(Search_Test::EvalGameOver(pos5, 63), score5);

	ASSERT_EQ(Search_Test::EvalGameOver<64>(pos1), score1);
	ASSERT_EQ(Search_Test::EvalGameOver<56>(pos2), score2);
	ASSERT_EQ(Search_Test::EvalGameOver<48>(pos3), score3);
	ASSERT_EQ(Search_Test::EvalGameOver<47>(pos4), score4);
	ASSERT_EQ(Search_Test::EvalGameOver<63>(pos5), score5);
}