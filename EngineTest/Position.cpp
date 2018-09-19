#include "pch.h"
#include "Position.h"
#include "Stability.h"

TEST(Position, Stability_Initialize)
{
	CStabilityAnalyzer stab;
	ASSERT_EQ(stab.GetStableEdges(CPosition(0x00, 0xC0)), 0xC0);
	ASSERT_EQ(stab.GetStableEdges(CPosition(0x01, 0x80)), 0x81);
	ASSERT_EQ(stab.GetStableEdges(CPosition(0x00, 0x03)), 0x03);
	ASSERT_EQ(stab.GetStableEdges(CPosition(0xC0, 0x00)), 0xC0);
	ASSERT_EQ(stab.GetStableEdges(CPosition(0x03, 0x00)), 0x03);
	ASSERT_EQ(stab.GetStableEdges(CPosition(0x03, 0xC0)), 0xC3);
	ASSERT_EQ(stab.GetStableEdges(CPosition(0x28, 0xC7)), 0xC7);
	ASSERT_EQ(stab.GetStableEdges(CPosition(0x50, 0xA8)), 0xC0);
	ASSERT_EQ(stab.GetStableEdges(CPosition(0x28, 0x16)), 0x08);
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
	ASSERT_EQ(CPosition().Empties(), 0xFFFFFFFFFFFFFFFFui64);
	ASSERT_EQ(CPosition::StartPosition().Empties(), 0xFFFFFFE7E7FFFFFFui64);
	ASSERT_EQ(CPosition::StartPositionETH().Empties(), 0xFFFFFFE7E7FFFFFFui64);
	ASSERT_EQ(CPosition(0xFFFFFFFFFFFFFFFFui64, 0ui64).Empties(), 0ui64);
}

TEST(Position, EmptyCount)
{
	ASSERT_EQ(CPosition().EmptyCount(), 64u);
	ASSERT_EQ(CPosition::StartPosition().EmptyCount(), 60u);
	ASSERT_EQ(CPosition::StartPositionETH().EmptyCount(), 60u);
	ASSERT_EQ(CPosition(0x0000000000000000ui64, 0x0000000000000000ui64).EmptyCount(), 64u);
	ASSERT_EQ(CPosition(0x0000000000000001ui64, 0x0000000000000000ui64).EmptyCount(), 63u);
	ASSERT_EQ(CPosition(0x0000000000000000ui64, 0x0000000000000001ui64).EmptyCount(), 63u);
	ASSERT_EQ(CPosition(0x0000000000000001ui64, 0x0000000000000001ui64).EmptyCount(), 63u);
	ASSERT_EQ(CPosition(0xFFFFFFFFFFFFFFFFui64, 0x0000000000000000ui64).EmptyCount(), 0u);
	ASSERT_EQ(CPosition(0x0000000000000000ui64, 0xFFFFFFFFFFFFFFFFui64).EmptyCount(), 0u);
	ASSERT_EQ(CPosition(0xFFFFFFFFFFFFFFFFui64, 0xFFFFFFFFFFFFFFFFui64).EmptyCount(), 0u);
}

TEST(Position, Parity)
{
	ASSERT_EQ(CPosition().Parity(), 0u);
	ASSERT_EQ(CPosition::StartPosition().Parity(), 0xFui64);
	ASSERT_EQ(CPosition::StartPositionETH().Parity(), 0xFui64);
	ASSERT_EQ(CPosition(0xFFFFFFFFFFFFFFFFui64, 0ui64).Parity(), 0ui64);
}

TEST(Position, PossibleMoves)
{
	ASSERT_EQ(CPosition().PossibleMoves(), 0ui64);
	ASSERT_EQ(CPosition::StartPosition().PossibleMoves(), 0x0000102004080000ui64);
	ASSERT_EQ(CPosition::StartPositionETH().PossibleMoves(), 0x00000000003C0000ui64);
	ASSERT_EQ(CPosition(0xFFFFFFFFFFFFFFFFui64, 0ui64).PossibleMoves(), 0ui64);
}

TEST(Position, HasMoves)
{
	ASSERT_EQ(CPosition().HasMoves(), false);
	ASSERT_EQ(CPosition::StartPosition().HasMoves(), true);
	ASSERT_EQ(CPosition::StartPositionETH().HasMoves(), true);
	ASSERT_EQ(CPosition(0xFFFFFFFFFFFFFFFFui64, 0ui64).HasMoves(), false);
}

TEST(Position, Play)
{
	CPosition pos = CPosition::StartPosition();
	pos = pos.PlayPass();
	ASSERT_EQ(pos, CPosition(CPosition::StartPosition().GetO(), CPosition::StartPosition().GetP()));
	pos = pos.PlayPass();
	ASSERT_EQ(pos, CPosition::StartPosition());
	pos = pos.Play(Field::D3);
	ASSERT_EQ(pos, CPosition(0x0000001000000000ui64, 0x0000000818080000ui64));
}

TEST(Position, FlipCodiagonal)
{
	CPosition pos1(0x000000000000000Fui64, 0x0ui64);
	CPosition pos5(0x8080808000000000ui64, 0x0ui64);
	pos1.FlipCodiagonal();

	ASSERT_EQ(pos1, pos5);
}

TEST(Position, FlipDiagonal)
{
	CPosition pos1(0x000000000000000Fui64, 0x0ui64);
	CPosition pos7(0x0000000001010101ui64, 0x0ui64);
	pos1.FlipDiagonal();

	ASSERT_EQ(pos1, pos7);
}

TEST(Position, FlipHorizontal)
{
	CPosition pos1(0x000000000000000Fui64, 0x0ui64);
	CPosition pos2(0x00000000000000F0ui64, 0x0ui64);
	pos1.FlipHorizontal();

	ASSERT_EQ(pos1, pos2);
}

TEST(Position, FlipVertical)
{
	CPosition pos1(0x000000000000000Fui64, 0x0ui64);
	CPosition pos4(0x0F00000000000000ui64, 0x0ui64);
	pos1.FlipVertical();

	ASSERT_EQ(pos1, pos4);
}

TEST(Position, FlipToMin)
{
	CPosition pos1(0x000000000000000Fui64, 0x0ui64); pos1.FlipToMin();
	CPosition pos2(0x00000000000000F0ui64, 0x0ui64); pos2.FlipToMin();
	CPosition pos3(0xF000000000000000ui64, 0x0ui64); pos3.FlipToMin();
	CPosition pos4(0x0F00000000000000ui64, 0x0ui64); pos4.FlipToMin();
	CPosition pos5(0x8080808000000000ui64, 0x0ui64); pos5.FlipToMin();
	CPosition pos6(0x0101010100000000ui64, 0x0ui64); pos6.FlipToMin();
	CPosition pos7(0x0000000001010101ui64, 0x0ui64); pos7.FlipToMin();
	CPosition pos8(0x0000000080808080ui64, 0x0ui64); pos8.FlipToMin();

	ASSERT_EQ(pos1, pos2);
	ASSERT_EQ(pos1, pos3);
	ASSERT_EQ(pos1, pos4);
	ASSERT_EQ(pos1, pos5);
	ASSERT_EQ(pos1, pos6);
	ASSERT_EQ(pos1, pos7);
	ASSERT_EQ(pos1, pos8);
}