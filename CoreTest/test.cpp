#include "pch.h"
#include "MacrosHell.h"
#include "TestHelpers.h"
#include "Position.h"
#include "FlipLoop.h"
#include "FlipFast.h"
#include "Moves.h"
#include "PositionGenerator.h"
#include "Utility.h"

TEST(Utility, pow_int_1) { ASSERT_EQ(Pow_int(0, 0), 1); }
TEST(Utility, pow_int_2) { ASSERT_EQ(Pow_int(10, 3), 1000); }
TEST(Utility, pow_int_3) { ASSERT_EQ(Pow_int(-10, 3), -1000); }

TEST(Utility, SignedInt) {
	ASSERT_EQ(SignedInt(-10), "-10");
	ASSERT_EQ(SignedInt(-1), "-1");
	ASSERT_EQ(SignedInt(+0), "+0");
	ASSERT_EQ(SignedInt(+1), "+1");
	ASSERT_EQ(SignedInt(+10), "+10");
}

TEST(Utility, DoubleDigitSignedInt) {
	ASSERT_EQ(DoubleDigitSignedInt(-10), "-10");
	ASSERT_EQ(DoubleDigitSignedInt(-1), "-01");
	ASSERT_EQ(DoubleDigitSignedInt(+0), "+00");
	ASSERT_EQ(DoubleDigitSignedInt(+1), "+01");
	ASSERT_EQ(DoubleDigitSignedInt(+10), "+10");
}

TEST(Utility, FlipDiagonal) {
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			ASSERT_EQ(FlipDiagonal(1ULL << (i * 8 + j)), 1ULL << (j * 8 + i));
}

TEST(Utility, FlipCodiagonal) {
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			ASSERT_EQ(FlipCodiagonal(1ULL << (i * 8 + j)), 1ULL << (63 - (j * 8 + i)));
}

TEST(Utility, FlipVertical) {
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			ASSERT_EQ(FlipVertical(1ULL << (i * 8 + j)), 1ULL << ((7 - i) * 8 + j));
}

TEST(Utility, FlipHorizontal) {
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			ASSERT_EQ(FlipHorizontal(1ULL << (i * 8 + j)), 1ULL << (i * 8 + (7 - j)));
}

void TestFlip(const CMove move)
{
	const uint64_t mask = line(move, -1, -1)
	                    | line(move, -1,  0)
	                    | line(move, -1, +1)
	                    | line(move,  0, -1)
	                    | line(move,  0, +1)
	                    | line(move, +1, -1)
	                    | line(move, +1,  0)
	                    | line(move, +1, +1);

	for (unsigned int i = 0; i < 10'000; i++)
	{
		CPosition pos = MakeRandomPosition(mask);
		ASSERT_EQ(Flip(pos, move), Flip_loop(pos, move));
	}

	for (unsigned int i = 0; i < 1'000; i++)
	{
		CPosition pos = MakeRandomPosition(~(1ULL << move));
		ASSERT_EQ(Flip(pos, move), Flip_loop(pos, move));
	}
}

TEST(FlipFast, Move_A1) { TestFlip(A1); }
TEST(FlipFast, Move_B1) { TestFlip(B1); }
TEST(FlipFast, Move_C1) { TestFlip(C1); }
TEST(FlipFast, Move_D1) { TestFlip(D1); }
TEST(FlipFast, Move_E1) { TestFlip(E1); }
TEST(FlipFast, Move_F1) { TestFlip(F1); }
TEST(FlipFast, Move_G1) { TestFlip(G1); }
TEST(FlipFast, Move_H1) { TestFlip(H1); }
TEST(FlipFast, Move_A2) { TestFlip(A2); }
TEST(FlipFast, Move_B2) { TestFlip(B2); }
TEST(FlipFast, Move_C2) { TestFlip(C2); }
TEST(FlipFast, Move_D2) { TestFlip(D2); }
TEST(FlipFast, Move_E2) { TestFlip(E2); }
TEST(FlipFast, Move_F2) { TestFlip(F2); }
TEST(FlipFast, Move_G2) { TestFlip(G2); }
TEST(FlipFast, Move_H2) { TestFlip(H2); }
TEST(FlipFast, Move_A3) { TestFlip(A3); }
TEST(FlipFast, Move_B3) { TestFlip(B3); }
TEST(FlipFast, Move_C3) { TestFlip(C3); }
TEST(FlipFast, Move_D3) { TestFlip(D3); }
TEST(FlipFast, Move_E3) { TestFlip(E3); }
TEST(FlipFast, Move_F3) { TestFlip(F3); }
TEST(FlipFast, Move_G3) { TestFlip(G3); }
TEST(FlipFast, Move_H3) { TestFlip(H3); }
TEST(FlipFast, Move_A4) { TestFlip(A4); }
TEST(FlipFast, Move_B4) { TestFlip(B4); }
TEST(FlipFast, Move_C4) { TestFlip(C4); }
TEST(FlipFast, Move_D4) { TestFlip(D4); }
TEST(FlipFast, Move_E4) { TestFlip(E4); }
TEST(FlipFast, Move_F4) { TestFlip(F4); }
TEST(FlipFast, Move_G4) { TestFlip(G4); }
TEST(FlipFast, Move_H4) { TestFlip(H4); }
TEST(FlipFast, Move_A5) { TestFlip(A5); }
TEST(FlipFast, Move_B5) { TestFlip(B5); }
TEST(FlipFast, Move_C5) { TestFlip(C5); }
TEST(FlipFast, Move_D5) { TestFlip(D5); }
TEST(FlipFast, Move_E5) { TestFlip(E5); }
TEST(FlipFast, Move_F5) { TestFlip(F5); }
TEST(FlipFast, Move_G5) { TestFlip(G5); }
TEST(FlipFast, Move_H5) { TestFlip(H5); }
TEST(FlipFast, Move_A6) { TestFlip(A6); }
TEST(FlipFast, Move_B6) { TestFlip(B6); }
TEST(FlipFast, Move_C6) { TestFlip(C6); }
TEST(FlipFast, Move_D6) { TestFlip(D6); }
TEST(FlipFast, Move_E6) { TestFlip(E6); }
TEST(FlipFast, Move_F6) { TestFlip(F6); }
TEST(FlipFast, Move_G6) { TestFlip(G6); }
TEST(FlipFast, Move_H6) { TestFlip(H6); }
TEST(FlipFast, Move_A7) { TestFlip(A7); }
TEST(FlipFast, Move_B7) { TestFlip(B7); }
TEST(FlipFast, Move_C7) { TestFlip(C7); }
TEST(FlipFast, Move_D7) { TestFlip(D7); }
TEST(FlipFast, Move_E7) { TestFlip(E7); }
TEST(FlipFast, Move_F7) { TestFlip(F7); }
TEST(FlipFast, Move_G7) { TestFlip(G7); }
TEST(FlipFast, Move_H7) { TestFlip(H7); }
TEST(FlipFast, Move_A8) { TestFlip(A8); }
TEST(FlipFast, Move_B8) { TestFlip(B8); }
TEST(FlipFast, Move_C8) { TestFlip(C8); }
TEST(FlipFast, Move_D8) { TestFlip(D8); }
TEST(FlipFast, Move_E8) { TestFlip(E8); }
TEST(FlipFast, Move_F8) { TestFlip(F8); }
TEST(FlipFast, Move_G8) { TestFlip(G8); }
TEST(FlipFast, Move_H8) { TestFlip(H8); }

TEST(FlipLoop, OneFlip) {
	const CPosition pos(0xFF00000000000000ULL, 0x00FFFFFFFFFFFF7EULL);
	const CMove move(A1);
	const uint64_t flip = Flip_loop(pos, move);

	ASSERT_EQ(pos.GetP() & flip, 0ULL);
	ASSERT_EQ(pos.GetO() & flip, flip);
	ASSERT_EQ(flip, 0x0041211109050300ULL);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------------------------------------------------------
TEST(MacrosHell, BitScanLSB_1) {
#ifndef NDEBUG
	ASSERT_DEATH(BitScanLSB(0), ".*");
#endif
	ASSERT_EQ(BitScanLSB(1), 0u);
	ASSERT_EQ(BitScanLSB(2), 1u);
	ASSERT_EQ(BitScanLSB(3), 0u);
	ASSERT_EQ(BitScanLSB(0x8000000000000000ULL), 63u);
}

TEST(MacrosHell, BitScanLSB_2) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanLSB(index, 1);
	ASSERT_NE(ret, 0);
	ASSERT_EQ(*index, 0u);
	delete index;
}

TEST(MacrosHell, BitScanLSB_3) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanLSB(index, 2);
	ASSERT_NE(ret, 0);
	ASSERT_EQ(*index, 1u);
	delete index;
}

TEST(MacrosHell, BitScanLSB_4) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanLSB(index, 3);
	ASSERT_NE(ret, 0);
	ASSERT_EQ(*index, 0u);
	delete index;
}

TEST(MacrosHell, BitScanLSB_5) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanLSB(index, 0x8000000000000000ULL);
	ASSERT_NE(ret, 0);
	ASSERT_EQ(*index, 63u);
	delete index;
}

TEST(MacrosHell, BitScanLSB_6) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanLSB(index, 0);
	ASSERT_EQ(ret, 0u);
	delete index;
}
// --------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------------------------------------------------------
TEST(MacrosHell, BitScanMSB_1) {
#ifndef NDEBUG
	ASSERT_DEATH(BitScanMSB(0), ".*");
#endif
	ASSERT_EQ(BitScanMSB(1), 0u);
	ASSERT_EQ(BitScanMSB(2), 1u);
	ASSERT_EQ(BitScanMSB(3), 1u);
	ASSERT_EQ(BitScanMSB(0x8000000000000000ULL), 63u);
}

TEST(MacrosHell, BitScanMSB_2) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanMSB(index, 1);
	ASSERT_NE(ret, 0);
	ASSERT_EQ(*index, 0u);
	delete index;
}

TEST(MacrosHell, BitScanMSB_3) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanMSB(index, 2);
	ASSERT_NE(ret, 0);
	ASSERT_EQ(*index, 1u);
	delete index;
}

TEST(MacrosHell, BitScanMSB_4) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanMSB(index, 3);
	ASSERT_NE(ret, 0);
	ASSERT_EQ(*index, 1u);
	delete index;
}

TEST(MacrosHell, BitScanMSB_5) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanMSB(index, 0x8000000000000000ULL);
	ASSERT_NE(ret, 0);
	ASSERT_EQ(*index, 63u);
	delete index;
}

TEST(MacrosHell, BitScanMSB_6) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanMSB(index, 0);
	ASSERT_EQ(ret, 0u);
	delete index;
}
// --------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------------------------------------------------------
TEST(MacrosHell, CountLeadingZerosTest) {
#ifndef NDEBUG
	ASSERT_DEATH(CountLeadingZeros(0), ".*");
#endif
	ASSERT_EQ(CountLeadingZeros(1), 63u);
	ASSERT_EQ(CountLeadingZeros(2), 62u);
	ASSERT_EQ(CountLeadingZeros(3), 62u);
	ASSERT_EQ(CountLeadingZeros(0x8000000000000000ULL), 0u);
}

TEST(MacrosHell, CountTrailingZerosTest) {
#ifndef NDEBUG
	ASSERT_DEATH(CountTrailingZeros(0), ".*");
#endif
	ASSERT_EQ(CountTrailingZeros(1), 0u);
	ASSERT_EQ(CountTrailingZeros(2), 1u);
	ASSERT_EQ(CountTrailingZeros(3), 0u);
	ASSERT_EQ(CountTrailingZeros(0x8000000000000000ULL), 63u);
}
// --------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------------------------------------------------------
TEST(MacrosHell, GetLSBTest) {
	ASSERT_EQ(GetLSB(0), 0u);
	ASSERT_EQ(GetLSB(1), 1u);
	ASSERT_EQ(GetLSB(2), 2u);
	ASSERT_EQ(GetLSB(3), 1u);
	ASSERT_EQ(GetLSB(0x8000000000000000ULL), 0x8000000000000000ULL);
}

TEST(MacrosHell, GetMSBTest) {
	ASSERT_EQ(GetMSB(0), 0u);
	ASSERT_EQ(GetMSB(1), 1u);
	ASSERT_EQ(GetMSB(2), 2u);
	ASSERT_EQ(GetMSB(3), 2u);
	ASSERT_EQ(GetMSB(0x8000000000000000ULL), 0x8000000000000000ULL);
}

TEST(MacrosHell, RemoveLSBTest) {
	uint64_t a;
	a = 0; RemoveLSB(a); ASSERT_EQ(a, 0u);
	a = 1; RemoveLSB(a); ASSERT_EQ(a, 0u);
	a = 2; RemoveLSB(a); ASSERT_EQ(a, 0u);
	a = 3; RemoveLSB(a); ASSERT_EQ(a, 2u);
	a = 0x8000000001000000ULL; RemoveLSB(a); ASSERT_EQ(a, 0x8000000000000000ULL);
}

TEST(MacrosHell, RemoveMSBTest) {
	uint64_t a;
	a = 0; RemoveMSB(a); ASSERT_EQ(a, 0u);
	a = 1; RemoveMSB(a); ASSERT_EQ(a, 0u);
	a = 2; RemoveMSB(a); ASSERT_EQ(a, 0u);
	a = 3; RemoveMSB(a); ASSERT_EQ(a, 1u);
	a = 0x8000000001000000ULL; RemoveMSB(a); ASSERT_EQ(a, 0x0000000001000000ULL);
}
// --------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------------------------------------------------------
TEST(MacrosHell, SetBitTest) {
	for (uint8_t i = 0; i < 64; i++)
	{
		uint64_t a = 0ULL;
		SetBit(a, i);
		ASSERT_EQ(a, 1ULL << i);
	}
	{
		uint64_t a = 0ULL;
#ifndef NDEBUG
		ASSERT_DEATH(SetBit(a, 64); , ".*");
#endif
	}
}

TEST(MacrosHell, GetBitTest) {
	uint64_t a = 0x8000000000000001ULL;
	ASSERT_EQ(TestBit(a, 0), true);
	ASSERT_EQ(TestBit(a, 63), true);
	for (uint8_t i = 1; i < 63; i++)
		ASSERT_EQ(TestBit(a, i), false);
#ifndef NDEBUG
	ASSERT_DEATH(TestBit(a, 64); , ".*");
#endif
}

TEST(MacrosHell, MakeBitTest) {
	for (uint8_t i = 0; i < 64; i++)
		ASSERT_EQ(MakeBit(i), 1ULL << i);
}
// --------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------------------------------------------------------
TEST(MacrosHell, PopCountTest) {
	ASSERT_EQ(PopCount(0), 0u);
	ASSERT_EQ(PopCount(1), 1u);
	ASSERT_EQ(PopCount(2), 1u);
	ASSERT_EQ(PopCount(3), 2u);
	ASSERT_EQ(PopCount(0xFFFFFFFFFFFFFFFFULL), 64u);
}

TEST(MacrosHell, PopCount_max15Test) {
	ASSERT_EQ(PopCount_max15(0), 0u);
	ASSERT_EQ(PopCount_max15(1), 1u);
	ASSERT_EQ(PopCount_max15(2), 1u);
	ASSERT_EQ(PopCount_max15(3), 2u);
	ASSERT_EQ(PopCount_max15(0xFF000000000000FEULL), 15u);
}
// --------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////

// TODO: Add test for BExtr
// TODO: Add test for BZHI
// TODO: Add test for PDep
// TODO: Add test for PExt
// TODO: Add test for BSwap

TEST(PossibleMoves, HasMoves)
{
	for (unsigned int i = 0; i < 100'000; i++)
	{
		CPosition pos = MakeRandomPosition();

		auto PotentialMoves = CMoves(pos.Empties());
		bool hasMoves = false;

		while (!PotentialMoves.empty())
		{
			const auto move = PotentialMoves.ExtractMove();
			if (Flip(pos, move) != 0ULL)
			{
				hasMoves = true;
				break;
			}
		}

		ASSERT_EQ(pos.HasMoves(), hasMoves);
	}
}

TEST(PossibleMoves, PossibleMoves_StartPosition)
{
	ASSERT_EQ(CPosition::StartPosition().PossibleMoves(), CMoves(0x0000102004080000ULL));
}

TEST(PossibleMoves, PossibleMoves_StartPositionETH)
{
	ASSERT_EQ(CPosition::StartPositionETH().PossibleMoves(), CMoves(0x00000000003C0000ULL));
}

TEST(PossibleMoves, PossibleMoves_RandomSample)
{
	for (unsigned int i = 0; i < 100'000; i++)
	{
		CPosition pos = MakeRandomPosition();

		auto PotentialMoves = CMoves(pos.Empties());
		uint64_t possibleMoves = 0ULL;

		while (!PotentialMoves.empty())
		{
			const auto move = PotentialMoves.ExtractMove();
			if (Flip(pos, move) != 0ULL)
				possibleMoves |= MakeBit(move);
		}

		ASSERT_EQ(pos.PossibleMoves(), possibleMoves);
	}
}

TEST(PositionGenerator, GenerateRandomPosition_noSeed)
{
	CPositionGenerator pos_gen;

	for (int empty_count = 0; empty_count <= 60; empty_count++)
	{
		const auto pos = pos_gen.GenerateRandomPosition(empty_count);

		ASSERT_EQ(pos.EmptyCount(), empty_count);
		ASSERT_EQ(pos.GetP() & pos.GetO(), 0); // P and O don't overlap.
	}
}

TEST(PositionGenerator, GenerateRandomPosition_Seed)
{
	CPositionGenerator pos_gen1(144);
	CPositionGenerator pos_gen2(144);

	for (int empty_count = 0; empty_count <= 60; empty_count++)
	{
		const auto pos1 = pos_gen1.GenerateRandomPosition(empty_count);
		const auto pos2 = pos_gen2.GenerateRandomPosition(empty_count);

		ASSERT_EQ(pos1, pos2);
	}
}

TEST(PositionGenerator, GenerateRandomPositionSet_SetSize)
{
	CPositionGenerator pos_gen;
	const int empty_count = 25;

	for (int SetSize = 1; SetSize <= 1024; SetSize *= 2)
	{
		const auto set = pos_gen.GenerateRandomPositionSet(empty_count, SetSize);

		ASSERT_EQ(set.size(), SetSize);
		for (const auto& pos : set) {
			ASSERT_EQ(pos.EmptyCount(), empty_count);
			ASSERT_EQ(pos.GetP() & pos.GetO(), 0); // P and O don't overlap.
		}
	}
}

TEST(PositionGenerator, GenerateRandomPositionSet_EmptyCount)
{
	CPositionGenerator pos_gen;
	const int SetSize = 100;

	for (int empty_count = 0; empty_count <= 50; empty_count++)
	{
		const auto set = pos_gen.GenerateRandomPositionSet(empty_count, SetSize);

		ASSERT_EQ(set.size(), SetSize);
		for (const auto& pos : set) {
			ASSERT_EQ(pos.EmptyCount(), empty_count);
			ASSERT_EQ(pos.GetP() & pos.GetO(), 0); // P and O don't overlap.
		}
	}
}

// TODO: Make CPositionGenerator::GenerateRandomPositionSet deterministic and add test.

//TEST(PositionGenerator, GenerateRandomPositionSet_Seed)
//{
//	CPositionGenerator pos_gen1(144);
//	CPositionGenerator pos_gen2(144);
//	const int empty_count = 25;
//
//	for (int SetSize = 1; SetSize <= 1024; SetSize *= 2)
//	{
//		const auto set1 = pos_gen1.GenerateRandomPositionSet(empty_count, SetSize);
//		const auto set2 = pos_gen2.GenerateRandomPositionSet(empty_count, SetSize);
//		ASSERT_EQ(set1.size(), SetSize);
//		ASSERT_EQ(set2.size(), SetSize);
//
//		std::vector<CPosition> vec1(set1.begin(), set1.end());
//		std::vector<CPosition> vec2(set2.begin(), set2.end());
//
//		for (std::size_t i = 0; i < SetSize; i++)
//			ASSERT_EQ(vec1[i], vec2[i]);
//	}
//}

TEST(PositionGenerator, GenerateAllPositions)
{
	CPositionGenerator pos_gen;
	const uint64_t correct[] = { 1, 4, 12, 54, 236, 1'288, 7'092, 42'614, 269'352, 1'723'592, 11'922'566 };

	for (int empty_count = 60; empty_count >= 55; empty_count--)
	{
		const auto set = pos_gen.GenerateAllPositions(empty_count);

		ASSERT_EQ(set.size(), correct[60 - empty_count]);
		for (const auto& pos : set) {
			ASSERT_EQ(pos.EmptyCount(), empty_count);
			ASSERT_EQ(pos.GetP() & pos.GetO(), 0); // P and O don't overlap.
		}
	}
}

TEST(PositionGenerator, GenerateAllPositionsSym)
{
	CPositionGenerator pos_gen;
	const uint64_t correct[] = { 1, 1, 3, 14, 60, 322, 1'773, 10'649, 64'245, 434'029, 2'958'586 };

	for (int empty_count = 60; empty_count >= 55; empty_count--)
	{
		const auto set = pos_gen.GenerateAllPositionsSym(empty_count);

		ASSERT_EQ(set.size(), correct[60 - empty_count]);
		for (const auto& pos : set) {
			ASSERT_EQ(pos.EmptyCount(), empty_count);
			ASSERT_EQ(pos.GetP() & pos.GetO(), 0); // P and O don't overlap.
		}
	}
}