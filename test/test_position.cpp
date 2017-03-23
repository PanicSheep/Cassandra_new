#include "macros_hell.h"
#include "position.h"
#include "gtest/gtest.h"
#include <functional>
#include <random>

TEST (PositionTest, SignedInt) {
	ASSERT_EQ (SignedInt(-1), "-1");
	ASSERT_EQ (SignedInt(+0), "+0");
	ASSERT_EQ (SignedInt(+1), "+1");
}

TEST (PositionTest, SMEAR_BITBOARD) {
	for (int move = 0; move < 64; move++)
	{
		uint64_t board = 0;
		int x = move / 8;
		int y = move % 8;
		
		for (int dx = -1; dx <= 1; dx++)
			for (int dy = -1; dy <= 1; dy++)
				if ((x + dx >= 0) && (x + dx < 8) && (y + dy >= 0) && (y + dy < 8))
					board |= 1ULL << ((x + dx) * 8 + (y + dy));
		
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
			ASSERT_EQ (FlipCodiagonal(1ULL << (i * 8 + j)), 1ULL << (63 - (j * 8 + i)));
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

TEST (PositionTest, EmptyCount) {
	ASSERT_EQ (EmptyCount(0x0000000000000000ULL, 0x0000000000000000ULL), 64u);
	ASSERT_EQ (EmptyCount(0x0000000000000001ULL, 0x0000000000000000ULL), 63u);
	ASSERT_EQ (EmptyCount(0x0000000000000000ULL, 0x0000000000000001ULL), 63u);
	ASSERT_EQ (EmptyCount(0x0000000000000001ULL, 0x0000000000000001ULL), 63u);
	ASSERT_EQ (EmptyCount(0xFFFFFFFFFFFFFFFFULL, 0x0000000000000000ULL),  0u);
	ASSERT_EQ (EmptyCount(0x0000000000000000ULL, 0xFFFFFFFFFFFFFFFFULL),  0u);
	ASSERT_EQ (EmptyCount(0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL),  0u);
}

TEST (PositionTest, Paritiy1) {
	static auto rnd = std::bind(std::uniform_int_distribution<uint64_t>(0,0xFFFFFFFFFFFFFFFFULL), std::mt19937_64(11));
	for (std::size_t i = 0; i < 1000000; i++)
	{
		uint64_t E = rnd();
		uint64_t parity1 = Parity(E);
		uint64_t parity2 = 0;
		while (E)
		{
			parity2 ^= quadrant_id_4_bit[BitScanLSB(E)];
			RemoveLSB(E);
		}
		ASSERT_EQ (parity1, parity2);
	}
}

TEST (PositionTest, Paritiy2) {
	for (unsigned int i = 0; i < 64; i++)
		ASSERT_EQ (quadrant_id_4_bit[i], 1ULL << quadrant_id_2_bit[i]);
}

// TODO: quadrant_mask
// TODO: quadrant
// TODO: board1D
// TODO: board2D
// TODO: board2D
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
// TODO: PlayersBoarder
// TODO: OpponentsBoarder
// TODO: Borders
// TODO: PlayersExposed
// TODO: OpponentsExposed
// TODO: Exposeds
// TODO: comp
// TODO: equiv
// TODO: CPosition
// TODO: CPositionScore
// TODO: CPositionFullScore
// TODO: CPositionScoreDepth
// TODO: CPositionAllScore

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	
	Stability::Initialize();
	
	return RUN_ALL_TESTS();
}