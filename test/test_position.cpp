#include "macros_hell.h"
#include "position.h"
#include "gtest/gtest.h"
#include <functional>
#include <random>

// ################################################################################################
// Position Test
// ################################################################################################
// ------------------------------------------------------------------------------------------------
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
// ------------------------------------------------------------------------------------------------
// ################################################################################################


// ################################################################################################
// CPosition Test
// ################################################################################################
// ------------------------------------------------------------------------------------------------
TEST (CPositionTest, Reset1) {
	CPosition pos = CPosition(false);
	pos.Reset();
	ASSERT_EQ (pos, CPosition());
}

TEST (CPositionTest, Reset2) {
	CPosition pos = CPosition();
	pos.Reset(true);
	ASSERT_EQ (pos, CPosition(true));
}

TEST (CPositionTest, Test) {
	ASSERT_EQ (CPosition().Test(), true);
	ASSERT_EQ (CPosition(true).Test(), true);
	ASSERT_EQ (CPosition(false).Test(), true);
	ASSERT_EQ (CPosition(0xFFULL, 0xFFULL).Test(), false);
}

TEST (CPositionTest, Empties) {
	ASSERT_EQ (CPosition().Empties(), 0xFFFFFFFFFFFFFFFFULL);
	ASSERT_EQ (CPosition(true).Empties(), 0xFFFFFFE7E7FFFFFFULL);
	ASSERT_EQ (CPosition(false).Empties(), 0xFFFFFFE7E7FFFFFFULL);
	ASSERT_EQ (CPosition(0xFFFFFFFFFFFFFFFFULL, 0ULL).Empties(), 0ULL);
}

TEST (CPositionTest, EmptyCount) {
	ASSERT_EQ (CPosition().EmptyCount(), 64u);
	ASSERT_EQ (CPosition(true).EmptyCount(), 60u);
	ASSERT_EQ (CPosition(false).EmptyCount(), 60u);
	ASSERT_EQ (CPosition(0xFFFFFFFFFFFFFFFFULL, 0ULL).EmptyCount(), 0u);
}

TEST (CPositionTest, Parity) {
	ASSERT_EQ (CPosition().Parity(), 0u);
	ASSERT_EQ (CPosition(true).Parity(), 0xFULL);
	ASSERT_EQ (CPosition(false).Parity(), 0xFULL);
	ASSERT_EQ (CPosition(0xFFFFFFFFFFFFFFFFULL, 0ULL).Parity(), 0ULL);
}

TEST (CPositionTest, PossibleMoves) {
	ASSERT_EQ (CPosition().PossibleMoves(), 0ULL);
	ASSERT_EQ (CPosition(true).PossibleMoves(), 0x00000000003C0000ULL);
	ASSERT_EQ (CPosition(false).PossibleMoves(), 0x0000102004080000ULL);
	ASSERT_EQ (CPosition(0xFFFFFFFFFFFFFFFFULL, 0ULL).PossibleMoves(), 0ULL);
}

TEST (CPositionTest, HasMoves) {
	ASSERT_EQ (CPosition().HasMoves(), false);
	ASSERT_EQ (CPosition(true).HasMoves(), true);
	ASSERT_EQ (CPosition(false).HasMoves(), true);
	ASSERT_EQ (CPosition(0xFFFFFFFFFFFFFFFFULL, 0ULL).HasMoves(), false);
}

TEST (CPositionTest, PlayStone) {
	CPosition pos(false);
	pos.PlayStone(64);
	ASSERT_EQ (pos, CPosition(START_POSITION_O, START_POSITION_P));
	pos.PlayStone(64);
	ASSERT_EQ (pos, CPosition(false));
	pos.PlayStone(19);
	ASSERT_EQ (pos, CPosition(0x0000001000000000ULL, 0x0000000818080000ULL));
}

TEST (CPositionTest, ctorCPositionScore) {
	CPositionScore pos(false);
	ASSERT_EQ (CPosition(false), static_cast<CPosition>(pos));
}

TEST (CPositionTest, ctorCPositionFullScore) {
	CPositionFullScore pos(false);
	ASSERT_EQ (CPosition(false), static_cast<CPosition>(pos));
}

TEST (CPositionTest, ctorCPositionScoreDepth) {
	CPositionScoreDepth pos(false);
	ASSERT_EQ (CPosition(false), static_cast<CPosition>(pos));
}

TEST (CPositionTest, ctorCPositionAllScore) {
	CPositionAllScore pos(false);
	ASSERT_EQ (CPosition(false), static_cast<CPosition>(pos));
}
// ------------------------------------------------------------------------------------------------
// ################################################################################################


// ################################################################################################
// CPositionScore Test
// ################################################################################################
// ------------------------------------------------------------------------------------------------
TEST (CPositionScoreTest, Reset1) {
	CPositionScore pos = CPositionScore(false);
	pos.Reset();
	ASSERT_EQ (equiv(pos, CPositionScore()), true);
	ASSERT_EQ (pos.score, -99);
}

TEST (CPositionScoreTest, Reset2) {
	CPositionScore pos = CPositionScore();
	pos.Reset(true);
	ASSERT_EQ (equiv(pos, CPositionScore(true)), true);
	ASSERT_EQ (pos.score, -99);
}

TEST (CPositionScoreTest, ResetInformation) {
	CPositionScore pos = CPositionScore(0, 0, 0);
	pos.ResetInformation();
	ASSERT_EQ (pos.score, -99);
}

TEST (CPositionScoreTest, Test) {
	ASSERT_EQ (CPositionScore().Test(), true);
	ASSERT_EQ (CPositionScore(true).Test(), true);
	ASSERT_EQ (CPositionScore(false).Test(), true);
	ASSERT_EQ (CPositionScore(0xFFULL, 0xFFULL, 1).Test(), false);
}

TEST (CPositionScoreTest, Empties) {
	ASSERT_EQ (CPositionScore().Empties(), 0xFFFFFFFFFFFFFFFFULL);
	ASSERT_EQ (CPositionScore(true).Empties(), 0xFFFFFFE7E7FFFFFFULL);
	ASSERT_EQ (CPositionScore(false).Empties(), 0xFFFFFFE7E7FFFFFFULL);
	ASSERT_EQ (CPositionScore(0xFFFFFFFFFFFFFFFFULL, 0ULL, 1).Empties(), 0ULL);
}

TEST (CPositionScoreTest, EmptyCount) {
	ASSERT_EQ (CPositionScore().EmptyCount(), 64u);
	ASSERT_EQ (CPositionScore(true).EmptyCount(), 60u);
	ASSERT_EQ (CPositionScore(false).EmptyCount(), 60u);
	ASSERT_EQ (CPositionScore(0xFFFFFFFFFFFFFFFFULL, 0ULL, 1).EmptyCount(), 0u);
}

TEST (CPositionScoreTest, Parity) {
	ASSERT_EQ (CPositionScore().Parity(), 0u);
	ASSERT_EQ (CPositionScore(true).Parity(), 0xFULL);
	ASSERT_EQ (CPositionScore(false).Parity(), 0xFULL);
	ASSERT_EQ (CPositionScore(0xFFFFFFFFFFFFFFFFULL, 0ULL, 1).Parity(), 0ULL);
}

TEST (CPositionScoreTest, PossibleMoves) {
	ASSERT_EQ (CPositionScore().PossibleMoves(), 0ULL);
	ASSERT_EQ (CPositionScore(true).PossibleMoves(), 0x00000000003C0000ULL);
	ASSERT_EQ (CPositionScore(false).PossibleMoves(), 0x0000102004080000ULL);
	ASSERT_EQ (CPositionScore(0xFFFFFFFFFFFFFFFFULL, 0ULL, 1).PossibleMoves(), 0ULL);
}

TEST (CPositionScoreTest, HasMoves) {
	ASSERT_EQ (CPositionScore().HasMoves(), false);
	ASSERT_EQ (CPositionScore(true).HasMoves(), true);
	ASSERT_EQ (CPositionScore(false).HasMoves(), true);
	ASSERT_EQ (CPositionScore(0xFFFFFFFFFFFFFFFFULL, 0ULL, 1).HasMoves(), false);
}

TEST (CPositionScoreTest, PlayStone) {
	CPositionScore pos(false);
	pos.PlayStone(64);
	ASSERT_EQ (equiv(pos, CPositionScore(START_POSITION_O, START_POSITION_P, 1)), true);
	pos.PlayStone(64);
	ASSERT_EQ (equiv(pos, CPositionScore(false)), true);
	pos.PlayStone(19);
	ASSERT_EQ (equiv(pos, CPositionScore(0x0000001000000000ULL, 0x0000000818080000ULL, 1)), true);
}

TEST (CPositionScoreTest, ctorCPosition) {
	CPosition pos(false);
	CPositionScore pos_cast = static_cast<CPositionScore>(pos);
	ASSERT_EQ (equiv(CPositionScore(false), pos_cast), true);
	ASSERT_EQ (pos_cast.score, -99);
}

TEST (CPositionScoreTest, ctorCPositionFullScore) {
	CPositionFullScore pos(false);
	pos.score[0] = 0;
	pos.score[1] = 1;
	pos.score[2] = 2;
	CPositionScore pos_cast = static_cast<CPositionScore>(pos);
	ASSERT_EQ (equiv(CPositionScore(false), pos_cast), true);
	ASSERT_EQ (pos_cast.score, 2);
}

TEST (CPositionScoreTest, ctorCPositionScoreDepth) {
	CPositionScoreDepth pos(false);
	pos.score = 2;
	CPositionScore pos_cast = static_cast<CPositionScore>(pos);
	ASSERT_EQ (equiv(CPositionScore(false), pos_cast), true);
	ASSERT_EQ (pos_cast.score, 2);
}

TEST (CPositionScoreTest, ctorCPositionAllScore) {
	CPositionAllScore pos(false);
	pos.score[0] = 0;
	pos.score[1] = 1;
	pos.score[2] = 2;
	CPositionScore pos_cast = static_cast<CPositionScore>(pos);
	ASSERT_EQ (equiv(CPositionScore(false), pos_cast), true);
	ASSERT_EQ (pos_cast.score, 2);
}
// ------------------------------------------------------------------------------------------------
// ################################################################################################


// TODO: quadrant_mask
// TODO: quadrant
// TODO: PlayStone
// TODO: StableStonesCornerAndCo
// TODO: StableStonesFullEdges
// TODO: StableStonesFullEdgesSecondOrder
// TODO: StableStonesTriangles
// TODO: StableEdges
// TODO: StableStonesPlayer
// TODO: StableStones
// TODO: FullLineHorizontal
// TODO: FullLineVertival
// TODO: FullLineDiagonal
// TODO: FullLineCodiagonal
// TODO: PlayersBoarder
// TODO: OpponentsBoarder
// TODO: Borders
// TODO: PlayersExposed
// TODO: OpponentsExposed
// TODO: Exposeds
// TODO: CPositionScore
// TODO: CPositionFullScore
// TODO: CPositionScoreDepth
// TODO: CPositionAllScore
// TODO: HasValidFilenameExtension
// TODO: read_vector_OBF
// TODO: LoadVector
// TODO: SaveVector
// TODO: SaveTransform
// TODO: LoadTransform

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	
	Stability::Initialize();
	
	return RUN_ALL_TESTS();
}