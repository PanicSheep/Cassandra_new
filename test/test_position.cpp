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
	ASSERT_EQ (SignedInt( -1), "-1");
	ASSERT_EQ (SignedInt( +0), "+0");
	ASSERT_EQ (SignedInt( +1), "+1");
	ASSERT_EQ (SignedInt(+10), "+10");
}

TEST (PositionTest, DoubleDigitSignedInt) {
	ASSERT_EQ (DoubleDigitSignedInt(-10), "-10");
	ASSERT_EQ (DoubleDigitSignedInt( -1), "-01");
	ASSERT_EQ (DoubleDigitSignedInt( +0), "+00");
	ASSERT_EQ (DoubleDigitSignedInt( +1), "+01");
	ASSERT_EQ (DoubleDigitSignedInt(+10), "+10");
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

TEST (PositionTest, StableStonesFullEdges) {
	ASSERT_EQ (StableStonesFullEdges(0x0000000000000000ULL, 0x0000000000000000ULL), 0x0000000000000000ULL);
	ASSERT_EQ (StableStonesFullEdges(0x0000000000000000ULL, 0x00000000000000FFULL), 0x00000000000000FFULL);
	ASSERT_EQ (StableStonesFullEdges(0x0000000000000000ULL, 0xFFFFFFFFFFFFFFFFULL), 0xFF818181818181FFULL);
	ASSERT_EQ (StableStonesFullEdges(0xFFFFFFFFFFFFFFFFULL, 0x0000000000000000ULL), 0x0000000000000000ULL);
}

TEST (PositionTest, Stability_Initialize) {
	
	ASSERT_EQ (StableEdges(0xC0, 0x00), 0xC0);
	ASSERT_EQ (StableEdges(0x80, 0x01), 0x81);
	ASSERT_EQ (StableEdges(0x03, 0x00), 0x03);
	ASSERT_EQ (StableEdges(0x00, 0xC0), 0xC0);
	ASSERT_EQ (StableEdges(0x00, 0x03), 0x03);
	ASSERT_EQ (StableEdges(0xC0, 0x03), 0xC3);
	ASSERT_EQ (StableEdges(0xC7, 0x28), 0xC7);
	ASSERT_EQ (StableEdges(0xA8, 0x50), 0xC0);
	ASSERT_EQ (StableEdges(0x16, 0x28), 0x08);
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
	for (std::size_t i = 0; i < 100000; i++)
	{
		uint64_t P = rnd();
		uint64_t O = rnd();
		uint64_t E = Empties(P, O);
		uint64_t parity1 = Parity(P, O);
		uint64_t parity2 = 0;
		while (E)
		{
			parity2 ^= quadrant_id_4_bit[BitScanLSB(E)];
			RemoveLSB(E);
		}
		ASSERT_EQ (parity1, parity2);
	}
}
// ------------------------------------------------------------------------------------------------
// ################################################################################################


// ################################################################################################
// CPosition Test
// ################################################################################################
// ------------------------------------------------------------------------------------------------
TEST(CPositionTest, DefaultPosition) {
	CPosition pos;
	ASSERT_EQ(pos.EmptyCount(), 64);
}

TEST (CPositionTest, StartPosition_normal) {
	CPosition pos = CPosition::StartPosition();
	pos.FlipDiagonal();
	ASSERT_EQ(pos, CPosition::StartPosition());
}

TEST (CPositionTest, StartPosition_ETH) {
	CPosition pos = CPosition::StartPositionETH();
	pos.FlipHorizontal();
	ASSERT_EQ(pos, CPosition::StartPositionETH());
}

TEST (CPositionTest, Test) {
	ASSERT_EQ (CPosition().Test(), true);
	ASSERT_EQ (CPosition::StartPosition().Test(), true);
	ASSERT_EQ (CPosition::StartPositionETH().Test(), true);
	ASSERT_EQ (CPosition(0xFFULL, 0xFFULL).Test(), false);
}

TEST (CPositionTest, Empties) {
	ASSERT_EQ (CPosition().Empties(), 0xFFFFFFFFFFFFFFFFULL);
	ASSERT_EQ (CPosition::StartPosition().Empties(), 0xFFFFFFE7E7FFFFFFULL);
	ASSERT_EQ (CPosition::StartPositionETH().Empties(), 0xFFFFFFE7E7FFFFFFULL);
	ASSERT_EQ (CPosition(0xFFFFFFFFFFFFFFFFULL, 0ULL).Empties(), 0ULL);
}

TEST (CPositionTest, EmptyCount) {
	ASSERT_EQ (CPosition().EmptyCount(), 64u);
	ASSERT_EQ (CPosition::StartPosition().EmptyCount(), 60u);
	ASSERT_EQ (CPosition::StartPositionETH().EmptyCount(), 60u);
	ASSERT_EQ (CPosition(0xFFFFFFFFFFFFFFFFULL, 0ULL).EmptyCount(), 0u);
}

TEST (CPositionTest, Parity) {
	ASSERT_EQ (CPosition().Parity(), 0u);
	ASSERT_EQ (CPosition::StartPosition().Parity(), 0xFULL);
	ASSERT_EQ (CPosition::StartPositionETH().Parity(), 0xFULL);
	ASSERT_EQ (CPosition(0xFFFFFFFFFFFFFFFFULL, 0ULL).Parity(), 0ULL);
}

TEST (CPositionTest, PossibleMoves) {
	ASSERT_EQ (CPosition().PossibleMoves(), 0ULL);
	ASSERT_EQ (CPosition::StartPosition().PossibleMoves(), 0x0000102004080000ULL);
	ASSERT_EQ (CPosition::StartPositionETH().PossibleMoves(), 0x00000000003C0000ULL);
	ASSERT_EQ (CPosition(0xFFFFFFFFFFFFFFFFULL, 0ULL).PossibleMoves(), 0ULL);
}

TEST (CPositionTest, HasMoves) {
	ASSERT_EQ (CPosition().HasMoves(), false);
	ASSERT_EQ (CPosition::StartPosition().HasMoves(), true);
	ASSERT_EQ (CPosition::StartPositionETH().HasMoves(), true);
	ASSERT_EQ (CPosition(0xFFFFFFFFFFFFFFFFULL, 0ULL).HasMoves(), false);
}

TEST (CPositionTest, PlayStone) {
	CPosition pos = CPosition::StartPosition();
	pos.PlayStone(64);
	ASSERT_EQ (pos, CPosition(CPosition::StartPosition().O, CPosition::StartPosition().P));
	pos.PlayStone(64);
	ASSERT_EQ (pos, CPosition::StartPosition());
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
	CPositionScore pos(CPosition::StartPosition());
	ASSERT_EQ (CPosition::StartPosition(), static_cast<CPosition>(pos));
}

TEST (CPositionTest, ctorCPositionFullScore) {
	CPositionFullScore pos(CPosition::StartPosition());
	ASSERT_EQ (CPosition::StartPosition(), static_cast<CPosition>(pos));
}

TEST (CPositionTest, ctorCPositionScoreDepth) {
	CPositionScoreDepth pos(CPosition::StartPosition());
	ASSERT_EQ (CPosition::StartPosition(), static_cast<CPosition>(pos));
}

TEST (CPositionTest, ctorCPositionAllScore) {
	CPositionAllScore pos(CPosition::StartPosition());
	ASSERT_EQ (CPosition::StartPosition(), static_cast<CPosition>(pos));
}
// ------------------------------------------------------------------------------------------------
// ################################################################################################


// ################################################################################################
// CPositionScore Test
// ################################################################################################
// ------------------------------------------------------------------------------------------------
TEST (CPositionScoreTest, ResetInformation) {
	CPositionScore pos = CPositionScore(0, 0, 0);
	pos.ResetInformation();
	ASSERT_EQ (pos.score, CPositionScore().score);
}

TEST (CPositionScoreTest, Test) {
	ASSERT_EQ (CPositionScore().Test(), true);
	ASSERT_EQ (CPositionScore(CPosition::StartPosition()).Test(), true);
	ASSERT_EQ (CPositionScore(CPosition::StartPositionETH()).Test(), true);
	ASSERT_EQ (CPositionScore(0xFFULL, 0xFFULL, 1).Test(), false);
}

TEST (CPositionScoreTest, Empties) {
	ASSERT_EQ (CPositionScore().Empties(), 0xFFFFFFFFFFFFFFFFULL);
	ASSERT_EQ (CPositionScore(CPosition::StartPosition()).Empties(), 0xFFFFFFE7E7FFFFFFULL);
	ASSERT_EQ (CPositionScore(CPosition::StartPositionETH()).Empties(), 0xFFFFFFE7E7FFFFFFULL);
	ASSERT_EQ (CPositionScore(0xFFFFFFFFFFFFFFFFULL, 0ULL, 1).Empties(), 0ULL);
}

TEST (CPositionScoreTest, EmptyCount) {
	ASSERT_EQ (CPositionScore().EmptyCount(), 64u);
	ASSERT_EQ (CPositionScore(CPosition::StartPosition()).EmptyCount(), 60u);
	ASSERT_EQ (CPositionScore(CPosition::StartPositionETH()).EmptyCount(), 60u);
	ASSERT_EQ (CPositionScore(0xFFFFFFFFFFFFFFFFULL, 0ULL, 1).EmptyCount(), 0u);
}

TEST (CPositionScoreTest, Parity) {
	ASSERT_EQ (CPositionScore().Parity(), 0u);
	ASSERT_EQ (CPositionScore(CPosition::StartPosition()).Parity(), 0xFULL);
	ASSERT_EQ (CPositionScore(CPosition::StartPositionETH()).Parity(), 0xFULL);
	ASSERT_EQ (CPositionScore(0xFFFFFFFFFFFFFFFFULL, 0ULL, 1).Parity(), 0ULL);
}

TEST (CPositionScoreTest, PossibleMoves) {
	ASSERT_EQ (CPositionScore().PossibleMoves(), 0ULL);
	ASSERT_EQ (CPositionScore(CPosition::StartPosition()).PossibleMoves(), 0x0000102004080000ULL);
	ASSERT_EQ (CPositionScore(CPosition::StartPositionETH()).PossibleMoves(), 0x00000000003C0000ULL);
	ASSERT_EQ (CPositionScore(0xFFFFFFFFFFFFFFFFULL, 0ULL, 1).PossibleMoves(), 0ULL);
}

TEST (CPositionScoreTest, HasMoves) {
	ASSERT_EQ (CPositionScore().HasMoves(), false);
	ASSERT_EQ (CPositionScore(CPosition::StartPosition()).HasMoves(), true);
	ASSERT_EQ (CPositionScore(CPosition::StartPositionETH()).HasMoves(), true);
	ASSERT_EQ (CPositionScore(0xFFFFFFFFFFFFFFFFULL, 0ULL, 1).HasMoves(), false);
}

TEST (CPositionScoreTest, PlayStone) {
	CPositionScore pos(CPosition::StartPosition(), -1);
	pos.PlayStone(64);
	ASSERT_EQ (equiv(pos, CPositionScore(CPosition::StartPosition().O, CPosition::StartPosition().P, 1)), true);
	pos.PlayStone(64);
	ASSERT_EQ (equiv(pos, CPositionScore(CPosition::StartPosition())), true);
	pos.PlayStone(19);
	ASSERT_EQ (equiv(pos, CPositionScore(0x0000001000000000ULL, 0x0000000818080000ULL, 1)), true);
}

TEST (CPositionScoreTest, ctorCPosition) {
	CPosition pos = CPosition::StartPositionETH();
	CPositionScore pos_cast = static_cast<CPositionScore>(pos);
	ASSERT_EQ (equiv(CPositionScore(CPosition::StartPositionETH()), pos_cast), true); // Position is copied
	ASSERT_EQ (pos_cast.score, -99); // Score is default
}

//TEST (CPositionScoreTest, ctorCPositionFullScore) {
//	CPositionFullScore pos(CPosition::StartPositionETH());
//	pos.score[0] = 0; // depth 0, score +0
//	pos.score[1] = 3; // depth 1, score +3
//	pos.score[2] = 2; // depth 2, score +2
//	CPositionScore pos_cast = static_cast<CPositionScore>(pos);
//	ASSERT_EQ (equiv(CPositionScore(CPosition::StartPositionETH()), pos_cast), true); // Position is copied
//	ASSERT_EQ (pos_cast.score, 2); // Score of highest depth is copied
//}

TEST (CPositionScoreTest, ctorCPositionScoreDepth) {
	CPositionScoreDepth pos(CPosition::StartPositionETH());
	pos.score = 2;
	CPositionScore pos_cast = static_cast<CPositionScore>(pos);
	ASSERT_EQ (equiv(CPositionScore(CPosition::StartPositionETH()), pos_cast), true); // Position is copied
	ASSERT_EQ (pos_cast.score, 2); // Score is copied
}

//TEST (CPositionScoreTest, ctorCPositionAllScore) {
//	CPositionAllScore pos(CPosition::StartPositionETH());
//	pos.score[0] = 0; // depth 0, score +0
//	pos.score[1] = 3; // depth 1, score +3
//	pos.score[2] = 2; // depth 2, score +2
//	CPositionScore pos_cast = static_cast<CPositionScore>(pos);
//	ASSERT_EQ (equiv(CPositionScore(CPosition::StartPositionETH()), pos_cast), true); // Position is copied
//	ASSERT_EQ (pos_cast.score, 3); // Highest score is copied
//}
// ------------------------------------------------------------------------------------------------
// ################################################################################################


// ################################################################################################
// CPositionFullScore Test
// ################################################################################################
// ------------------------------------------------------------------------------------------------
TEST (CPositionFullScoreTest, ResetInformation) {
	CPositionFullScore pos = CPositionFullScore(0, 0);
	pos.score[3] = 12;
	pos.ResetInformation();
	for (int i = 0; i < 61; i++)
		ASSERT_EQ (pos.score[i], CPositionFullScore().score[i]);
}

TEST (CPositionFullScoreTest, Test1) {
	ASSERT_EQ (CPositionFullScore().Test(), true);
	ASSERT_EQ (CPositionFullScore(CPosition::StartPosition()).Test(), true);
	ASSERT_EQ (CPositionFullScore(CPosition::StartPositionETH()).Test(), true);
	ASSERT_EQ (CPositionFullScore(0xFFULL, 0xFFULL).Test(), false);
}

TEST (CPositionFullScoreTest, Test2) {
	CPositionFullScore pos(0xFFULL, 0xFF00ULL);
	pos.score[59] = 13; // Score outside of range
	ASSERT_EQ (pos.Test(), false);
}

TEST (CPositionFullScoreTest, ctorCPosition) {
	CPosition pos = CPosition::StartPositionETH();
	CPositionFullScore pos_cast = static_cast<CPositionFullScore>(pos);
	ASSERT_EQ (equiv(CPositionFullScore(CPosition::StartPositionETH()), pos_cast), true); // Position is copied
	for (unsigned int i = 0; i < 61; i++)
		ASSERT_EQ (pos_cast.score[i], CPositionFullScore().score[i]); // Score is default
}

//TEST (CPositionFullScoreTest, ctorCPositionScore) {
//	CPositionScore pos(0xFFULL, 0ULL, 10);
//	CPositionFullScore pos_cast = static_cast<CPositionFullScore>(pos);
//	ASSERT_EQ (equiv(CPositionFullScore(0xFFULL, 0ULL), pos_cast), true); // Position is copied
//	for (unsigned int i = 0; i < 61; i++)
//		if (i != pos.EmptyCount())
//			ASSERT_EQ (pos_cast.score[i], CPositionFullScore().score[i]); // Score is default
//	ASSERT_EQ (pos_cast.score[pos.EmptyCount()], pos.score);
//}

//TEST (CPositionFullScoreTest, ctorCPositionScoreDepth1) {
//	CPositionScoreDepth pos(0xFFULL, 0ULL, 10, 3, 0);
//	CPositionFullScore pos_cast = static_cast<CPositionFullScore>(pos);
//	ASSERT_EQ (equiv(CPositionFullScore(0xFFULL, 0ULL), pos_cast), true); // Position is copied
//	for (unsigned int i = 0; i < 61; i++)
//		if (i != 3)
//			ASSERT_EQ (pos_cast.score[i], CPositionFullScore().score[i]); // Score is default
//	ASSERT_EQ (pos_cast.score[3], pos.score); // copied score
//}

TEST (CPositionFullScoreTest, ctorCPositionScoreDepth2) {
	CPositionScoreDepth pos(0xFFULL, 0ULL, 10, 3, 1);
	CPositionFullScore pos_cast = static_cast<CPositionFullScore>(pos);
	ASSERT_EQ (equiv(CPositionFullScore(0xFFULL, 0ULL), pos_cast), true); // Position is copied
	for (unsigned int i = 0; i < 61; i++)
		ASSERT_EQ (pos_cast.score[i], CPositionFullScore().score[i]); // Score is default
}
// ------------------------------------------------------------------------------------------------
// ################################################################################################

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	
	Stability::Initialize();
	
	return RUN_ALL_TESTS();
}