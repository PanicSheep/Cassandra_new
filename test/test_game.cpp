#include "game.h"
#include "gtest/gtest.h"
#include <cassert>
#include <vector>

TEST (GameTest, EvalGameOver)
{
	ASSERT_EQ (EvalGameOver(0x0000000000000000ULL, 64),   0); // Player: 0   Opponent:  0   Empty: 64   Score: 0-0=0
	ASSERT_EQ (EvalGameOver(0x00000000000000FFULL, 56),  64); // Player: 8   Opponent:  0   Empty: 56   Score: 8-0+56=64
	ASSERT_EQ (EvalGameOver(0x00000000000000FFULL, 48),   0); // Player: 8   Opponent:  8   Empty: 48   Score: 8-8=0
	ASSERT_EQ (EvalGameOver(0x00000000000000FFULL, 47), -48); // Player: 8   Opponent:  9   Empty: 47   Score: 8-9-47=-48
	ASSERT_EQ (EvalGameOver(0x0000000000000000ULL, 63), -64); // Player: 0   Opponent:  1   Empty: 63   Score: 0-1-63=-64
	
	ASSERT_EQ (EvalGameOver<64>(0x0000000000000000ULL),   0); // Player: 0   Opponent:  0   Empty: 64   Score: 0-0=0
	ASSERT_EQ (EvalGameOver<56>(0x00000000000000FFULL),  64); // Player: 8   Opponent:  0   Empty: 56   Score: 8-0+56=64
	ASSERT_EQ (EvalGameOver<48>(0x00000000000000FFULL),   0); // Player: 8   Opponent:  8   Empty: 48   Score: 8-8=0
	ASSERT_EQ (EvalGameOver<47>(0x00000000000000FFULL), -48); // Player: 8   Opponent:  9   Empty: 47   Score: 8-9-47=-48
	ASSERT_EQ (EvalGameOver<63>(0x0000000000000000ULL), -64); // Player: 0   Opponent:  1   Empty: 63   Score: 0-1-63=-64
}

enum eEvalFkt { NegaMax, AlphaBeta, PVS };

class CTestPos
{
public:
	uint64_t P, O;
	int score;
	
	CTestPos(uint64_t P, uint64_t O, int score) : P(P), O(O), score(score) {}
};

std::vector<CTestPos> TestPos = { 
	CTestPos(0xFF00FF00FF00FF00ULL, 0x00FF00FF00FF00FFULL,   0), //  0
	CTestPos(0xFF00FF00FF00FFFFULL, 0x00FF00FF00FF0000ULL, +16), //  1
	CTestPos(0xFF00FF00FF000000ULL, 0x00FF00FF00FFFFFFULL, -16), //  2
	
	CTestPos(0xFFFFFFFFFFFFFFFEULL, 0x0000000000000000ULL, +64), //  3
	CTestPos(0xFFFFFFFFFFFFFCFCULL, 0x0000000000000302ULL, +64), //  4
	CTestPos(0xFFFFFFFFFFFFFF7EULL, 0x0000000000000080ULL, +48), //  5
	CTestPos(0xFFFFFFFFFFFFFE7EULL, 0x0000000000000180ULL, +62), //  6
	
	CTestPos(0xFEFFFFFFFFFFFFFEULL, 0x0000000000000000ULL, +64), //  7
	CTestPos(0xFCFCFFFFFFFFFCFCULL, 0x0203000000000302ULL, +64), //  8
	CTestPos(0xFEFFFFFFFFFFFF7EULL, 0x0000000000000080ULL, +22), //  9
	CTestPos(0xFFFFFFFFFBFEFC7EULL, 0x0000000004010280ULL, +54), // 10
	
	CTestPos(0xFFFFFFFFFFFFFFF8ULL, 0x0000000000000000ULL, +64), // 11
	CTestPos(0xFFFFFFFFFFFFFDF8ULL, 0x0000000000000200ULL, +64), // 12
	CTestPos(0xF8FFFFFFFFFFFFF8ULL, 0x0700000000000000ULL, +16), // 13
	CTestPos(0xFFFFFFFEFBFEFC7EULL, 0x0000000104000280ULL, +58), // 14
	
	CTestPos(0xFFFFFFFFFFFFFFF0ULL, 0x0000000000000000ULL, +64), // 15
	CTestPos(0xFFFFFFFFFFFFE0E0ULL, 0x0000000000001F10ULL, +64), // 16
	CTestPos(0xF0FFFFFFFFFFFFF0ULL, 0x0F00000000000000ULL,  +0), // 17
	CTestPos(0xFFFFFFFFFFE7DBC3ULL, 0x0000000000182400ULL, +56), // 18
	
	// FForum
	CTestPos(0x3E0C0D2B772B7000ULL, 0x0071725488140E7CULL, +18), // 19
	CTestPos(0x7E2059000C19303EULL, 0x001E267FF3E60C00ULL, +10), // 20
	CTestPos(0x040C0D306C707A01ULL, 0x083070CF938F853EULL,  +2), // 21
	CTestPos(0x7EB8B9D127070C2CULL, 0x0006462E58383012ULL,  +0), // 22
	CTestPos(0x0019D6D6C8F0A07CULL, 0x7C242829370D1C00ULL, +32), // 23
	CTestPos(0x1C1C14223F9A047EULL, 0x20E0EADCC0647800ULL, +14), // 24
	CTestPos(0x18BC8F9F877F3018ULL, 0x2440706078000E06ULL,  +8), // 25
	CTestPos(0x0000081F22426064ULL, 0x14BCF7E0DCBC9C98ULL,  +8), // 26
	CTestPos(0x10B070222E942C3CULL, 0x2C0F0F5DD16B1000ULL,  -8), // 27
	CTestPos(0x002054A49716247CULL, 0x781CAB5B68E91800ULL, +10), // 28
};

void TestTestPos(unsigned int emptyCount, unsigned int MobilityPlayer, unsigned int MobilityOpponent, unsigned int index)
{
	ASSERT_TRUE (TestPos[index].score >= -64);
	ASSERT_TRUE (TestPos[index].score <= +64);
	ASSERT_TRUE ((TestPos[index].P & TestPos[index].O) == 0ULL);
	ASSERT_EQ (EmptyCount(TestPos[index].P, TestPos[index].O), emptyCount);
	ASSERT_EQ (PopCount(PossibleMoves(TestPos[index].P, TestPos[index].O)), MobilityPlayer);
	ASSERT_EQ (PopCount(PossibleMoves(TestPos[index].O, TestPos[index].P)), MobilityOpponent);
}

TEST (TestTest, TestPos0) { TestTestPos(0, 0, 0, 0); }
TEST (TestTest, TestPos1) { TestTestPos(0, 0, 0, 1); }
TEST (TestTest, TestPos2) { TestTestPos(0, 0, 0, 2); }
TEST (TestTest, TestPos3) { TestTestPos(1, 0, 0, 3); }
TEST (TestTest, TestPos4) { TestTestPos(1, 1, 0, 4); }
TEST (TestTest, TestPos5) { TestTestPos(1, 0, 1, 5); }
TEST (TestTest, TestPos6) { TestTestPos(1, 1, 1, 6); }
TEST (TestTest, TestPos7) { TestTestPos(2, 0, 0,  7); }
TEST (TestTest, TestPos8) { TestTestPos(2, 2, 0,  8); }
TEST (TestTest, TestPos9) { TestTestPos(2, 0, 2,  9); }
TEST (TestTest, TestPos10) { TestTestPos(2, 2, 2, 10); }
TEST (TestTest, TestPos11) { TestTestPos(3, 0, 0, 11); }
TEST (TestTest, TestPos12) { TestTestPos(3, 3, 0, 12); }
TEST (TestTest, TestPos13) { TestTestPos(3, 0, 3, 13); }
TEST (TestTest, TestPos14) { TestTestPos(3, 3, 3, 14); }
TEST (TestTest, TestPos15) { TestTestPos(4, 0, 0, 15); }
TEST (TestTest, TestPos16) { TestTestPos(4, 4, 0, 16); }
TEST (TestTest, TestPos17) { TestTestPos(4, 0, 4, 17); }
TEST (TestTest, TestPos18) { TestTestPos(4, 4, 4, 18); }

void TestPosition(unsigned int index, eEvalFkt evalFkt)
{
	uint64_t NodeCounter = 0ULL;
	int score;
	switch (evalFkt)
	{
		case NegaMax:
			score = Endgame_NegaMax::Eval(TestPos[index].P, TestPos[index].O, NodeCounter);
			break;
		case AlphaBeta:
			score = Endgame_AlphaBeta::Eval(TestPos[index].P, TestPos[index].O, NodeCounter);
			break;
		case PVS:
			score = Endgame_PVS::Eval(TestPos[index].P, TestPos[index].O, NodeCounter);
			break;
	}
	ASSERT_EQ (score, TestPos[index].score);
}

TEST (NegaMaxTest, ZeroEmpties0) { TestPosition(0, eEvalFkt::NegaMax); }
TEST (NegaMaxTest, ZeroEmpties1) { TestPosition(1, eEvalFkt::NegaMax); }
TEST (NegaMaxTest, ZeroEmpties2) { TestPosition(2, eEvalFkt::NegaMax); }
TEST (NegaMaxTest, OneEmpties0) { TestPosition(3, eEvalFkt::NegaMax); }
TEST (NegaMaxTest, OneEmpties1) { TestPosition(4, eEvalFkt::NegaMax); }
TEST (NegaMaxTest, OneEmpties2) { TestPosition(5, eEvalFkt::NegaMax); }
TEST (NegaMaxTest, OneEmpties3) { TestPosition(6, eEvalFkt::NegaMax); }
TEST (NegaMaxTest, TwoEmpties0) { TestPosition(7, eEvalFkt::NegaMax); }
TEST (NegaMaxTest, TwoEmpties1) { TestPosition(8, eEvalFkt::NegaMax); }
TEST (NegaMaxTest, TwoEmpties2) { TestPosition(9, eEvalFkt::NegaMax); }
TEST (NegaMaxTest, TwoEmpties3) { TestPosition(10, eEvalFkt::NegaMax); }
TEST (NegaMaxTest, ThreeEmpties0) { TestPosition(11, eEvalFkt::NegaMax); }
TEST (NegaMaxTest, ThreeEmpties1) { TestPosition(12, eEvalFkt::NegaMax); }
TEST (NegaMaxTest, ThreeEmpties2) { TestPosition(13, eEvalFkt::NegaMax); }
TEST (NegaMaxTest, ThreeEmpties3) { TestPosition(14, eEvalFkt::NegaMax); }
TEST (NegaMaxTest, FourEmpties0) { TestPosition(15, eEvalFkt::NegaMax); }
TEST (NegaMaxTest, FourEmpties1) { TestPosition(16, eEvalFkt::NegaMax); }
TEST (NegaMaxTest, FourEmpties2) { TestPosition(17, eEvalFkt::NegaMax); }
TEST (NegaMaxTest, FourEmpties3) { TestPosition(18, eEvalFkt::NegaMax); }

//TEST (NegaMaxTest, FForum1) { TestPosition(19, eEvalFkt::NegaMax); }
//TEST (NegaMaxTest, FForum2) { TestPosition(20, eEvalFkt::NegaMax); }
//TEST (NegaMaxTest, FForum3) { TestPosition(21, eEvalFkt::NegaMax); }
//TEST (NegaMaxTest, FForum4) { TestPosition(22, eEvalFkt::NegaMax); }
//TEST (NegaMaxTest, FForum5) { TestPosition(23, eEvalFkt::NegaMax); }
//TEST (NegaMaxTest, FForum6) { TestPosition(24, eEvalFkt::NegaMax); }
//TEST (NegaMaxTest, FForum7) { TestPosition(25, eEvalFkt::NegaMax); }
//TEST (NegaMaxTest, FForum8) { TestPosition(26, eEvalFkt::NegaMax); }
//TEST (NegaMaxTest, FForum9) { TestPosition(27, eEvalFkt::NegaMax); }
//TEST (NegaMaxTest, FForum10) { TestPosition(28, eEvalFkt::NegaMax); }

TEST (AlphaBetaTest, ZeroEmpties0) { TestPosition(0, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, ZeroEmpties1) { TestPosition(1, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, ZeroEmpties2) { TestPosition(2, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, OneEmpties0) { TestPosition(3, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, OneEmpties1) { TestPosition(4, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, OneEmpties2) { TestPosition(5, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, OneEmpties3) { TestPosition(6, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, TwoEmpties0) { TestPosition(7, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, TwoEmpties1) { TestPosition(8, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, TwoEmpties2) { TestPosition(9, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, TwoEmpties3) { TestPosition(10, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, ThreeEmpties0) { TestPosition(11, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, ThreeEmpties1) { TestPosition(12, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, ThreeEmpties2) { TestPosition(13, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, ThreeEmpties3) { TestPosition(14, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, FourEmpties0) { TestPosition(15, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, FourEmpties1) { TestPosition(16, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, FourEmpties2) { TestPosition(17, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, FourEmpties3) { TestPosition(18, eEvalFkt::AlphaBeta); }

TEST (AlphaBetaTest, FForum1) { TestPosition(19, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, FForum2) { TestPosition(20, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, FForum3) { TestPosition(21, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, FForum4) { TestPosition(22, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, FForum5) { TestPosition(23, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, FForum6) { TestPosition(24, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, FForum7) { TestPosition(25, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, FForum8) { TestPosition(26, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, FForum9) { TestPosition(27, eEvalFkt::AlphaBeta); }
TEST (AlphaBetaTest, FForum10) { TestPosition(28, eEvalFkt::AlphaBeta); }

TEST (PvsTest, ZeroEmpties0) { TestPosition(0, eEvalFkt::PVS); }
TEST (PvsTest, ZeroEmpties1) { TestPosition(1, eEvalFkt::PVS); }
TEST (PvsTest, ZeroEmpties2) { TestPosition(2, eEvalFkt::PVS); }
TEST (PvsTest, OneEmpties0) { TestPosition(3, eEvalFkt::PVS); }
TEST (PvsTest, OneEmpties1) { TestPosition(4, eEvalFkt::PVS); }
TEST (PvsTest, OneEmpties2) { TestPosition(5, eEvalFkt::PVS); }
TEST (PvsTest, OneEmpties3) { TestPosition(6, eEvalFkt::PVS); }
TEST (PvsTest, TwoEmpties0) { TestPosition(7, eEvalFkt::PVS); }
TEST (PvsTest, TwoEmpties1) { TestPosition(8, eEvalFkt::PVS); }
TEST (PvsTest, TwoEmpties2) { TestPosition(9, eEvalFkt::PVS); }
TEST (PvsTest, TwoEmpties3) { TestPosition(10, eEvalFkt::PVS); }
TEST (PvsTest, ThreeEmpties0) { TestPosition(11, eEvalFkt::PVS); }
TEST (PvsTest, ThreeEmpties1) { TestPosition(12, eEvalFkt::PVS); }
TEST (PvsTest, ThreeEmpties2) { TestPosition(13, eEvalFkt::PVS); }
TEST (PvsTest, ThreeEmpties3) { TestPosition(14, eEvalFkt::PVS); }
TEST (PvsTest, FourEmpties0) { TestPosition(15, eEvalFkt::PVS); }
TEST (PvsTest, FourEmpties1) { TestPosition(16, eEvalFkt::PVS); }
TEST (PvsTest, FourEmpties2) { TestPosition(17, eEvalFkt::PVS); }
TEST (PvsTest, FourEmpties3) { TestPosition(18, eEvalFkt::PVS); }

TEST (PvsTest, FForum1) { TestPosition(19, eEvalFkt::PVS); }
TEST (PvsTest, FForum2) { TestPosition(20, eEvalFkt::PVS); }
TEST (PvsTest, FForum3) { TestPosition(21, eEvalFkt::PVS); }
TEST (PvsTest, FForum4) { TestPosition(22, eEvalFkt::PVS); }
TEST (PvsTest, FForum5) { TestPosition(23, eEvalFkt::PVS); }
TEST (PvsTest, FForum6) { TestPosition(24, eEvalFkt::PVS); }
TEST (PvsTest, FForum7) { TestPosition(25, eEvalFkt::PVS); }
TEST (PvsTest, FForum8) { TestPosition(26, eEvalFkt::PVS); }
TEST (PvsTest, FForum9) { TestPosition(27, eEvalFkt::PVS); }
TEST (PvsTest, FForum10) { TestPosition(28, eEvalFkt::PVS); }

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	
	Configfile::Initialize(argv[0]);
	CountLastFlip::Initialize();
	Stability::Initialize();
	Pattern::Initialize(false, true);
	gTT = CHashTable(32 * 1024 * 1024);
	gTTPV = CHashTable(1024 * 1024);
	
	return RUN_ALL_TESTS();
}
