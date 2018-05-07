#include "pch.h"
#include "Position.h"
#include "Stability.h"

TEST(Position, Stability_Initialize)
{
	ASSERT_EQ(GetStableEdges(CPosition(0x00, 0xC0)), 0xC0);
	ASSERT_EQ(GetStableEdges(CPosition(0x01, 0x80)), 0x81);
	ASSERT_EQ(GetStableEdges(CPosition(0x00, 0x03)), 0x03);
	ASSERT_EQ(GetStableEdges(CPosition(0xC0, 0x00)), 0xC0);
	ASSERT_EQ(GetStableEdges(CPosition(0x03, 0x00)), 0x03);
	ASSERT_EQ(GetStableEdges(CPosition(0x03, 0xC0)), 0xC3);
	ASSERT_EQ(GetStableEdges(CPosition(0x28, 0xC7)), 0xC7);
	ASSERT_EQ(GetStableEdges(CPosition(0x50, 0xA8)), 0xC0);
	ASSERT_EQ(GetStableEdges(CPosition(0x28, 0x16)), 0x08);
}

TEST(Position, DefaultPosition)
{
	CPosition pos;
	ASSERT_EQ(pos.EmptyCount(), 64);
}

TEST(Position, StartPosition_normal)
{
	CPosition pos = CPosition::StartPosition();
	pos.FlipDiagonal();
	ASSERT_EQ(pos, CPosition::StartPosition());
}

TEST(Position, StartPosition_ETH)
{
	CPosition pos = CPosition::StartPositionETH();
	pos.FlipHorizontal();
	ASSERT_EQ(pos, CPosition::StartPositionETH());
}

TEST(Position, Empties)
{
	ASSERT_EQ(CPosition().Empties(), 0xFFFFFFFFFFFFFFFFULL);
	ASSERT_EQ(CPosition::StartPosition().Empties(), 0xFFFFFFE7E7FFFFFFULL);
	ASSERT_EQ(CPosition::StartPositionETH().Empties(), 0xFFFFFFE7E7FFFFFFULL);
	ASSERT_EQ(CPosition(0xFFFFFFFFFFFFFFFFULL, 0ULL).Empties(), 0ULL);
}

TEST(Position, EmptyCount)
{
	ASSERT_EQ(CPosition().EmptyCount(), 64u);
	ASSERT_EQ(CPosition::StartPosition().EmptyCount(), 60u);
	ASSERT_EQ(CPosition::StartPositionETH().EmptyCount(), 60u);
	ASSERT_EQ(CPosition(0x0000000000000000ULL, 0x0000000000000000ULL).EmptyCount(), 64u);
	ASSERT_EQ(CPosition(0x0000000000000001ULL, 0x0000000000000000ULL).EmptyCount(), 63u);
	ASSERT_EQ(CPosition(0x0000000000000000ULL, 0x0000000000000001ULL).EmptyCount(), 63u);
	ASSERT_EQ(CPosition(0x0000000000000001ULL, 0x0000000000000001ULL).EmptyCount(), 63u);
	ASSERT_EQ(CPosition(0xFFFFFFFFFFFFFFFFULL, 0x0000000000000000ULL).EmptyCount(), 0u);
	ASSERT_EQ(CPosition(0x0000000000000000ULL, 0xFFFFFFFFFFFFFFFFULL).EmptyCount(), 0u);
	ASSERT_EQ(CPosition(0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL).EmptyCount(), 0u);
}

TEST(Position, Parity)
{
	ASSERT_EQ(CPosition().Parity(), 0u);
	ASSERT_EQ(CPosition::StartPosition().Parity(), 0xFULL);
	ASSERT_EQ(CPosition::StartPositionETH().Parity(), 0xFULL);
	ASSERT_EQ(CPosition(0xFFFFFFFFFFFFFFFFULL, 0ULL).Parity(), 0ULL);
}

TEST(Position, PossibleMoves)
{
	ASSERT_EQ(CPosition().PossibleMoves(), 0ULL);
	ASSERT_EQ(CPosition::StartPosition().PossibleMoves(), 0x0000102004080000ULL);
	ASSERT_EQ(CPosition::StartPositionETH().PossibleMoves(), 0x00000000003C0000ULL);
	ASSERT_EQ(CPosition(0xFFFFFFFFFFFFFFFFULL, 0ULL).PossibleMoves(), 0ULL);
}

TEST(Position, HasMoves)
{
	ASSERT_EQ(CPosition().HasMoves(), false);
	ASSERT_EQ(CPosition::StartPosition().HasMoves(), true);
	ASSERT_EQ(CPosition::StartPositionETH().HasMoves(), true);
	ASSERT_EQ(CPosition(0xFFFFFFFFFFFFFFFFULL, 0ULL).HasMoves(), false);
}

TEST(Position, Play)
{
	CPosition pos = CPosition::StartPosition();
	pos = pos.PlayPass();
	ASSERT_EQ(pos, CPosition(CPosition::StartPosition().O, CPosition::StartPosition().P));
	pos = pos.PlayPass();
	ASSERT_EQ(pos, CPosition::StartPosition());
	pos = pos.Play(Field::D3);
	ASSERT_EQ(pos, CPosition(0x0000001000000000ULL, 0x0000000818080000ULL));
}

TEST(Position, FlipCodiagonal)
{
	CPosition pos1(0x000000000000000FULL, 0x0ULL);
	CPosition pos5(0x8080808000000000ULL, 0x0ULL);
	pos1.FlipCodiagonal();

	ASSERT_EQ(pos1, pos5);
}

TEST(Position, FlipDiagonal)
{
	CPosition pos1(0x000000000000000FULL, 0x0ULL);
	CPosition pos7(0x0000000001010101ULL, 0x0ULL);
	pos1.FlipDiagonal();

	ASSERT_EQ(pos1, pos7);
}

TEST(Position, FlipHorizontal)
{
	CPosition pos1(0x000000000000000FULL, 0x0ULL);
	CPosition pos2(0x00000000000000F0ULL, 0x0ULL);
	pos1.FlipHorizontal();

	ASSERT_EQ(pos1, pos2);
}

TEST(Position, FlipVertical)
{
	CPosition pos1(0x000000000000000FULL, 0x0ULL);
	CPosition pos4(0x0F00000000000000ULL, 0x0ULL);
	pos1.FlipVertical();

	ASSERT_EQ(pos1, pos4);
}

TEST(Position, FlipToMin)
{
	CPosition pos1(0x000000000000000FULL, 0x0ULL); pos1.FlipToMin();
	CPosition pos2(0x00000000000000F0ULL, 0x0ULL); pos2.FlipToMin();
	CPosition pos3(0xF000000000000000ULL, 0x0ULL); pos3.FlipToMin();
	CPosition pos4(0x0F00000000000000ULL, 0x0ULL); pos4.FlipToMin();
	CPosition pos5(0x8080808000000000ULL, 0x0ULL); pos5.FlipToMin();
	CPosition pos6(0x0101010100000000ULL, 0x0ULL); pos6.FlipToMin();
	CPosition pos7(0x0000000001010101ULL, 0x0ULL); pos7.FlipToMin();
	CPosition pos8(0x0000000080808080ULL, 0x0ULL); pos8.FlipToMin();

	ASSERT_EQ(pos1, pos2);
	ASSERT_EQ(pos1, pos3);
	ASSERT_EQ(pos1, pos4);
	ASSERT_EQ(pos1, pos5);
	ASSERT_EQ(pos1, pos6);
	ASSERT_EQ(pos1, pos7);
	ASSERT_EQ(pos1, pos8);
}