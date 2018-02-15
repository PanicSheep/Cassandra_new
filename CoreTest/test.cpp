#include "pch.h"
#include "TestHelpers.h"
#include "Position.h"
#include "FlipLoop.h"
#include "FlipFast.h"
#include "Moves.h"

uint64_t line(const CMove& move, const int dX, const int dY)
{
	uint64_t ret = 0;
	int i = (move.field % 8) + dX; // Starting index in x direction
	int j = (move.field / 8) + dY; // Starting index in y direction

	while ((i >= 0) && (i < 8) && (j >= 0) && (j < 8)) // In between boundaries
	{
		ret |= 1ULL << (j * 8 + i);
		i += dX;
		j += dY;
	}

	return ret;
}

void TestFlip(const CMove& move)
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
		CPosition pos = MakeRandomPosition(~(1ULL << move.field));
		ASSERT_EQ(Flip(pos, move), Flip_loop(pos, move));
	}
}

TEST(FlipFastTest, Move_A1) { TestFlip(Field::A1); }
TEST(FlipFastTest, Move_B1) { TestFlip(Field::B1); }
TEST(FlipFastTest, Move_C1) { TestFlip(Field::C1); }
TEST(FlipFastTest, Move_D1) { TestFlip(Field::D1); }
TEST(FlipFastTest, Move_E1) { TestFlip(Field::E1); }
TEST(FlipFastTest, Move_F1) { TestFlip(Field::F1); }
TEST(FlipFastTest, Move_G1) { TestFlip(Field::G1); }
TEST(FlipFastTest, Move_H1) { TestFlip(Field::H1); }
TEST(FlipFastTest, Move_A2) { TestFlip(Field::A2); }
TEST(FlipFastTest, Move_B2) { TestFlip(Field::B2); }
TEST(FlipFastTest, Move_C2) { TestFlip(Field::C2); }
TEST(FlipFastTest, Move_D2) { TestFlip(Field::D2); }
TEST(FlipFastTest, Move_E2) { TestFlip(Field::E2); }
TEST(FlipFastTest, Move_F2) { TestFlip(Field::F2); }
TEST(FlipFastTest, Move_G2) { TestFlip(Field::G2); }
TEST(FlipFastTest, Move_H2) { TestFlip(Field::H2); }
TEST(FlipFastTest, Move_A3) { TestFlip(Field::A3); }
TEST(FlipFastTest, Move_B3) { TestFlip(Field::B3); }
TEST(FlipFastTest, Move_C3) { TestFlip(Field::C3); }
TEST(FlipFastTest, Move_D3) { TestFlip(Field::D3); }
TEST(FlipFastTest, Move_E3) { TestFlip(Field::E3); }
TEST(FlipFastTest, Move_F3) { TestFlip(Field::F3); }
TEST(FlipFastTest, Move_G3) { TestFlip(Field::G3); }
TEST(FlipFastTest, Move_H3) { TestFlip(Field::H3); }
TEST(FlipFastTest, Move_A4) { TestFlip(Field::A4); }
TEST(FlipFastTest, Move_B4) { TestFlip(Field::B4); }
TEST(FlipFastTest, Move_C4) { TestFlip(Field::C4); }
TEST(FlipFastTest, Move_D4) { TestFlip(Field::D4); }
TEST(FlipFastTest, Move_E4) { TestFlip(Field::E4); }
TEST(FlipFastTest, Move_F4) { TestFlip(Field::F4); }
TEST(FlipFastTest, Move_G4) { TestFlip(Field::G4); }
TEST(FlipFastTest, Move_H4) { TestFlip(Field::H4); }
TEST(FlipFastTest, Move_A5) { TestFlip(Field::A5); }
TEST(FlipFastTest, Move_B5) { TestFlip(Field::B5); }
TEST(FlipFastTest, Move_C5) { TestFlip(Field::C5); }
TEST(FlipFastTest, Move_D5) { TestFlip(Field::D5); }
TEST(FlipFastTest, Move_E5) { TestFlip(Field::E5); }
TEST(FlipFastTest, Move_F5) { TestFlip(Field::F5); }
TEST(FlipFastTest, Move_G5) { TestFlip(Field::G5); }
TEST(FlipFastTest, Move_H5) { TestFlip(Field::H5); }
TEST(FlipFastTest, Move_A6) { TestFlip(Field::A6); }
TEST(FlipFastTest, Move_B6) { TestFlip(Field::B6); }
TEST(FlipFastTest, Move_C6) { TestFlip(Field::C6); }
TEST(FlipFastTest, Move_D6) { TestFlip(Field::D6); }
TEST(FlipFastTest, Move_E6) { TestFlip(Field::E6); }
TEST(FlipFastTest, Move_F6) { TestFlip(Field::F6); }
TEST(FlipFastTest, Move_G6) { TestFlip(Field::G6); }
TEST(FlipFastTest, Move_H6) { TestFlip(Field::H6); }
TEST(FlipFastTest, Move_A7) { TestFlip(Field::A7); }
TEST(FlipFastTest, Move_B7) { TestFlip(Field::B7); }
TEST(FlipFastTest, Move_C7) { TestFlip(Field::C7); }
TEST(FlipFastTest, Move_D7) { TestFlip(Field::D7); }
TEST(FlipFastTest, Move_E7) { TestFlip(Field::E7); }
TEST(FlipFastTest, Move_F7) { TestFlip(Field::F7); }
TEST(FlipFastTest, Move_G7) { TestFlip(Field::G7); }
TEST(FlipFastTest, Move_H7) { TestFlip(Field::H7); }
TEST(FlipFastTest, Move_A8) { TestFlip(Field::A8); }
TEST(FlipFastTest, Move_B8) { TestFlip(Field::B8); }
TEST(FlipFastTest, Move_C8) { TestFlip(Field::C8); }
TEST(FlipFastTest, Move_D8) { TestFlip(Field::D8); }
TEST(FlipFastTest, Move_E8) { TestFlip(Field::E8); }
TEST(FlipFastTest, Move_F8) { TestFlip(Field::F8); }
TEST(FlipFastTest, Move_G8) { TestFlip(Field::G8); }
TEST(FlipFastTest, Move_H8) { TestFlip(Field::H8); }

//int main(int argc, char **argv)
//{
//	::testing::InitGoogleTest(&argc, argv);
//	return RUN_ALL_TESTS();
//}
