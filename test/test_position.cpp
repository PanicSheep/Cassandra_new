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
	ASSERT_EQ (SignedInt(-10), "-10");
	ASSERT_EQ (SignedInt( -1), "-01");
	ASSERT_EQ (SignedInt( +0), "+00");
	ASSERT_EQ (SignedInt( +1), "+01");
	ASSERT_EQ (SignedInt(+10), "+10");
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

TEST (PositionTest, board1D) {
	ASSERT_EQ (board1D(0xFFULL, 0xFF00000000000001ULL), "OOOOOOOO------------------------------------------------XXXXXXX#");
}

TEST (PositionTest, board2D_PM) {
	ASSERT_EQ (board2D(0xFFULL, 0xFF00000000000001ULL, 0x100ULL), 
			   "  H G F E D C B A  \n"
			   "8 O O O O O O O O 8\n"
			   "7 - - - - - - - - 7\n"
			   "6 - - - - - - - - 6\n"
			   "5 - - - - - - - - 5\n"
			   "4 - - - - - - - - 4\n"
			   "3 - - - - - - - - 3\n"
			   "2 - - - - - - - + 2\n"
			   "1 X X X X X X X # 1\n"
			   "  H G F E D C B A  ");
}

TEST (PositionTest, board2D) {
	ASSERT_EQ (board2D(0xFFULL, 0xFF00000000000001ULL), 
			   "  H G F E D C B A  \n"
			   "8 O O O O O O O O 8\n"
			   "7 - - - - - - - - 7\n"
			   "6 - - - - - - - - 6\n"
			   "5 - - - - - - - - 5\n"
			   "4 - - - - - - - - 4\n"
			   "3 - - - - - - - - 3\n"
			   "2 - - - - - - - - 2\n"
			   "1 X X X X X X X # 1\n"
			   "  H G F E D C B A  ");
}

TEST (PositionTest, StableStonesFullEdges) {
	ASSERT_EQ (StableStonesFullEdges(0x0000000000000000ULL, 0x0000000000000000ULL), 0x0000000000000000ULL);
	ASSERT_EQ (StableStonesFullEdges(0x0000000000000000ULL, 0x00000000000000FFULL), 0x00000000000000FFULL);
	ASSERT_EQ (StableStonesFullEdges(0x0000000000000000ULL, 0xFFFFFFFFFFFFFFFFULL), 0xFF818181818181FFULL);
	ASSERT_EQ (StableStonesFullEdges(0xFFFFFFFFFFFFFFFFULL, 0x0000000000000000ULL), 0x0000000000000000ULL);
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

TEST (PositionTest, HasValidFilenameExtension) {
	ASSERT_EQ (HasValidFilenameExtension("tmp.pos"), true);
	ASSERT_EQ (HasValidFilenameExtension("C:\tmp.psc"), true);
	ASSERT_EQ (HasValidFilenameExtension("tmp.pfs"), true);
	ASSERT_EQ (HasValidFilenameExtension("tmp.psd"), true);
	ASSERT_EQ (HasValidFilenameExtension("tmp.pas"), true);
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

TEST (CPositionTest, FlipCodiagonal) {
	CPosition pos1(0x000000000000000FULL, 0x0ULL);
	CPosition pos5(0x8080808000000000ULL, 0x0ULL);
	pos1.FlipCodiagonal();
	
	ASSERT_EQ (pos1, pos5);
}

TEST (CPositionTest, FlipDiagonal) {
	CPosition pos1(0x000000000000000FULL, 0x0ULL);
	CPosition pos7(0x0000000001010101ULL, 0x0ULL);
	pos1.FlipDiagonal();
	
	ASSERT_EQ (pos1, pos7);
}

TEST (CPositionTest, FlipHorizontal) {
	CPosition pos1(0x000000000000000FULL, 0x0ULL);
	CPosition pos2(0x00000000000000F0ULL, 0x0ULL);
	pos1.FlipHorizontal();
	
	ASSERT_EQ (pos1, pos2);
}

TEST (CPositionTest, FlipVertical) {
	CPosition pos1(0x000000000000000FULL, 0x0ULL);
	CPosition pos4(0x0F00000000000000ULL, 0x0ULL);
	pos1.FlipVertical();
	
	ASSERT_EQ (pos1, pos4);
}

TEST (CPositionTest, FlipToMin) {
	CPosition pos1(0x000000000000000FULL, 0x0ULL); pos1.FlipToMin();
	CPosition pos2(0x00000000000000F0ULL, 0x0ULL); pos2.FlipToMin();
	CPosition pos3(0xF000000000000000ULL, 0x0ULL); pos3.FlipToMin();
	CPosition pos4(0x0F00000000000000ULL, 0x0ULL); pos4.FlipToMin();
	CPosition pos5(0x8080808000000000ULL, 0x0ULL); pos5.FlipToMin();
	CPosition pos6(0x0101010100000000ULL, 0x0ULL); pos6.FlipToMin();
	CPosition pos7(0x0000000001010101ULL, 0x0ULL); pos7.FlipToMin();
	CPosition pos8(0x0000000080808080ULL, 0x0ULL); pos8.FlipToMin();
	
	ASSERT_EQ (pos1, pos2);
	ASSERT_EQ (pos1, pos3);
	ASSERT_EQ (pos1, pos4);
	ASSERT_EQ (pos1, pos5);
	ASSERT_EQ (pos1, pos6);
	ASSERT_EQ (pos1, pos7);
	ASSERT_EQ (pos1, pos8);
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
	ASSERT_EQ (pos.score, CPositionScore().score);
}

TEST (CPositionScoreTest, Reset2) {
	CPositionScore pos = CPositionScore();
	pos.Reset(true);
	ASSERT_EQ (equiv(pos, CPositionScore(true)), true);
	ASSERT_EQ (pos.score, CPositionScore().score);
}

TEST (CPositionScoreTest, ResetInformation) {
	CPositionScore pos = CPositionScore(0, 0, 0);
	pos.ResetInformation();
	ASSERT_EQ (pos.score, CPositionScore().score);
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
	ASSERT_EQ (equiv(CPositionScore(false), pos_cast), true); // Position is copied
	ASSERT_EQ (pos_cast.score, -99); // Score is default
}

TEST (CPositionScoreTest, ctorCPositionFullScore) {
	CPositionFullScore pos(false);
	pos.score[0] = 0; // depth 0, score +0
	pos.score[1] = 3; // depth 1, score +3
	pos.score[2] = 2; // depth 2, score +2
	CPositionScore pos_cast = static_cast<CPositionScore>(pos);
	ASSERT_EQ (equiv(CPositionScore(false), pos_cast), true); // Position is copied
	ASSERT_EQ (pos_cast.score, 2); // Score of highest depth is copied
}

TEST (CPositionScoreTest, ctorCPositionScoreDepth) {
	CPositionScoreDepth pos(false);
	pos.score = 2;
	CPositionScore pos_cast = static_cast<CPositionScore>(pos);
	ASSERT_EQ (equiv(CPositionScore(false), pos_cast), true); // Position is copied
	ASSERT_EQ (pos_cast.score, 2); // Score is copied
}

TEST (CPositionScoreTest, ctorCPositionAllScore) {
	CPositionAllScore pos(false);
	pos.score[0] = 0; // depth 0, score +0
	pos.score[1] = 3; // depth 1, score +3
	pos.score[2] = 2; // depth 2, score +2
	CPositionScore pos_cast = static_cast<CPositionScore>(pos);
	ASSERT_EQ (equiv(CPositionScore(false), pos_cast), true); // Position is copied
	ASSERT_EQ (pos_cast.score, 3); // Highest score is copied
}
// ------------------------------------------------------------------------------------------------
// ################################################################################################


// ################################################################################################
// CPositionFullScore Test
// ################################################################################################
// ------------------------------------------------------------------------------------------------
TEST (CPositionFullScoreTest, Reset1) {
	CPositionFullScore pos = CPositionFullScore(false);
	pos.Reset();
	ASSERT_EQ (equiv(pos, CPositionFullScore()), true);
	for (int i = 0; i < 61; i++)
		ASSERT_EQ (pos.score[i], CPositionFullScore().score[i]);
}

TEST (CPositionFullScoreTest, Reset2) {
	CPositionFullScore pos = CPositionFullScore();
	pos.Reset(true);
	ASSERT_EQ (equiv(pos, CPositionFullScore(true)), true);
	for (int i = 0; i < 61; i++)
		ASSERT_EQ (pos.score[i], CPositionFullScore().score[i]);
}

TEST (CPositionFullScoreTest, ResetInformation) {
	CPositionFullScore pos = CPositionFullScore(0, 0);
	pos.score[3] = 12;
	pos.ResetInformation();
	for (int i = 0; i < 61; i++)
		ASSERT_EQ (pos.score[i], CPositionFullScore().score[i]);
}

TEST (CPositionFullScoreTest, Test1) {
	ASSERT_EQ (CPositionFullScore().Test(), true);
	ASSERT_EQ (CPositionFullScore(true).Test(), true);
	ASSERT_EQ (CPositionFullScore(false).Test(), true);
	ASSERT_EQ (CPositionFullScore(0xFFULL, 0xFFULL).Test(), false);
}

TEST (CPositionFullScoreTest, Test2) {
	CPositionFullScore pos(0xFFULL, 0xFF00ULL);
	pos.score[59] = 13; // Score outside of range
	ASSERT_EQ (pos.Test(), false);
}

TEST (CPositionFullScoreTest, ctorCPosition) {
	CPosition pos(false);
	CPositionFullScore pos_cast = static_cast<CPositionFullScore>(pos);
	ASSERT_EQ (equiv(CPositionFullScore(false), pos_cast), true); // Position is copied
	for (unsigned int i = 0; i < 61; i++)
		ASSERT_EQ (pos_cast.score[i], CPositionFullScore().score[i]); // Score is default
}

TEST (CPositionFullScoreTest, ctorCPositionScore) {
	CPositionScore pos(0xFFULL, 0ULL, 10);
	CPositionFullScore pos_cast = static_cast<CPositionFullScore>(pos);
	ASSERT_EQ (equiv(CPositionFullScore(0xFFULL, 0ULL), pos_cast), true); // Position is copied
	for (unsigned int i = 0; i < 61; i++)
		if (i != pos.EmptyCount())
			ASSERT_EQ (pos_cast.score[i], CPositionFullScore().score[i]); // Score is default
	ASSERT_EQ (pos_cast.score[pos.EmptyCount()], pos.score);
}

TEST (CPositionFullScoreTest, ctorCPositionScoreDepth1) {
	CPositionScoreDepth pos(0xFFULL, 0ULL, 10, 3, 0);
	CPositionFullScore pos_cast = static_cast<CPositionFullScore>(pos);
	ASSERT_EQ (equiv(CPositionFullScore(0xFFULL, 0ULL), pos_cast), true); // Position is copied
	for (unsigned int i = 0; i < 61; i++)
		if (i != 3)
			ASSERT_EQ (pos_cast.score[i], CPositionFullScore().score[i]); // Score is default
	ASSERT_EQ (pos_cast.score[3], pos.score); // copied score
}

TEST (CPositionFullScoreTest, ctorCPositionScoreDepth2) {
	CPositionScoreDepth pos(0xFFULL, 0ULL, 10, 3, 1);
	CPositionFullScore pos_cast = static_cast<CPositionFullScore>(pos);
	ASSERT_EQ (equiv(CPositionFullScore(0xFFULL, 0ULL), pos_cast), true); // Position is copied
	for (unsigned int i = 0; i < 61; i++)
		ASSERT_EQ (pos_cast.score[i], CPositionFullScore().score[i]); // Score is default
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
// TODO: CPositionScoreDepth
// TODO: CPositionAllScore
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