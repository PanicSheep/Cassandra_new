#include "position.h"
#include "gtest/gtest.h"

TEST (PositionTest, SMEAR_BITBOARD) {
	for (int move = 0; move < 64; move++)
	{
		uint64_t board = 0;
		int x = move / 8;
		int y = move % 8;
		
		for (int dx = -1; dx <= 1; dx++)
			for (int dy = -1; dy <= 1; dy++)
				if ((x + dx >= 0) && (x + dx < 8) && (y + dy >= 0) && (y + dy < 8))
					board |= 1ULL << (x * 8 + y);
		
		ASSERT_EQ (board, SMEAR_BITBOARD(1ULL << move));
	}
}

TEST (PositionTest, neighbour) {
	for (int move = 0; move < 64; move++)
		ASSERT_EQ (neighbour[move], SMEAR_BITBOARD(1ULL << move) ^ (1ULL << move));
}

TEST (PositionTest, FlipDiagonal) {
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			ASSERT_EQ (FlipDiagonal(1ULL << (i * 8 + j)), 1ULL << (j * 8 + i));
}

TEST (PositionTest, FlipCodiagonal) {
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			ASSERT_EQ (FlipCodiagonal(1ULL << (i * 8 + j)), 1ULL << (63 - (i * 8 + j)));
}

TEST (PositionTest, FlipVertical) {
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			ASSERT_EQ (FlipVertical(1ULL << (i * 8 + j)), 1ULL << ((7 - i) * 8 + j));
}

TEST (PositionTest, FlipVertical) {
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			ASSERT_EQ (FlipVertical(1ULL << (i * 8 + j)), 1ULL << ((7 - i) * 8 + j));
}

TEST (PositionTest, FlipHorizontal) {
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			ASSERT_EQ (FlipHorizontal(1ULL << (i * 8 + j)), 1ULL << (i * 8 + (7 - j)));
}

TEST (PositionTest, Empties) {
	ASSERT_EQ (Empties(0x0000000000000000ULL, 0x0000000000000000ULL), 64);
	ASSERT_EQ (Empties(0x0000000000000001ULL, 0x0000000000000000ULL), 63);
	ASSERT_EQ (Empties(0x0000000000000000ULL, 0x0000000000000001ULL), 63);
	ASSERT_EQ (Empties(0x0000000000000001ULL, 0x0000000000000001ULL), 63);
	ASSERT_EQ (Empties(0xFFFFFFFFFFFFFFFFULL, 0x0000000000000000ULL),  0);
	ASSERT_EQ (Empties(0x0000000000000000ULL, 0xFFFFFFFFFFFFFFFFULL),  0);
	ASSERT_EQ (Empties(0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL),  0);
}

// TODO: quadrant_id_2_bit
// TODO: quadrant_id_4_bit
// TODO: quadrant_mask
// TODO: quadrant
// TODO: board1D
// TODO: board2D
// TODO: board2D
// TODO: Parity
// TODO: Parity
// TODO: PlayStone
// TODO: StableStonesCornerAndCo
// TODO: StableStonesFullEdges
// TODO: StableStonesFullEdgesSecondOrder
// TODO: StableStonesTriangles
// TODO: FullLineHorizontal
// TODO: FullLineVertival
// TODO: FullLineDiagonal
// TODO: FullLineCodiagonal
// TODO: StableEdges
// TODO: StableStonesPlayer
// TODO: StableStones
// TODO: PlayersBorder
// TODO: OpponentsBorder
// TODO: Borders
// TODO: PlayersExposed
// TODO: OpponentsExposed
// TODO: Exposeds
// TODO: CPositionScore
// TODO: CPosition

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	
	Stability::Initialize();
	
	return RUN_ALL_TESTS();
}