#include "pch.h"
#include "Search.h"
//#include "SearchInput.h"
//#include "SearchOutput.h"
//#include "SearchStatusQuo.h"

using namespace Search;

class Search_Test : public CAlgorithm
{
public:
	static int EvalGameOver(const CPosition& pos) { return CAlgorithm::EvalGameOver(pos); }
};

TEST(Algorithm, EvalGameOver)
{
	const auto pos1 = CPosition(0x0000000000000000ui64, 0x0000000000000000ui64); // Player: 0   Opponent: 0   Empty: 64   Score: 0-0=0
	const auto pos2 = CPosition(0x00000000000000FFui64, 0x0000000000000000ui64); // Player: 8   Opponent: 0   Empty: 56   Score: 8-0+56=64
	const auto pos3 = CPosition(0x00000000000000FFui64, 0xFF00000000000000ui64); // Player: 8   Opponent: 8   Empty: 48   Score: 8-8=0
	const auto pos4 = CPosition(0x00000000000000FFui64, 0xFF10000000000000ui64); // Player: 8   Opponent: 9   Empty: 47   Score: 8-9-47=-48
	const auto pos5 = CPosition(0x0000000000000000ui64, 0x1000000000000000ui64); // Player: 0   Opponent: 1   Empty: 63   Score: 0-1-63=-64
	const int score1 = 0;
	const int score2 = 64;
	const int score3 = 0;
	const int score4 = -48;
	const int score5 = -64;
	
	ASSERT_EQ(Search_Test::EvalGameOver(pos1), score1);
	ASSERT_EQ(Search_Test::EvalGameOver(pos2), score2);
	ASSERT_EQ(Search_Test::EvalGameOver(pos3), score3);
	ASSERT_EQ(Search_Test::EvalGameOver(pos4), score4);
	ASSERT_EQ(Search_Test::EvalGameOver(pos5), score5);
}
//
//void Improving_on_analysis(int8_t alpha, int8_t beta, int8_t min, int8_t max, int8_t new_alpha, int8_t new_beta)
//{
//	const auto input = CInput(CPosition(), alpha, beta, 64, 0);
//	CStatusQuo status_quo(input);
//
//	const auto analysis = COutput(min, max, 64, 0, { CMove::A1, CMove::B2 });
//	const auto cut = status_quo.ImproveWith(analysis);
//
//	ASSERT_FALSE(cut);
//	ASSERT_EQ(status_quo.alpha, new_alpha);
//	ASSERT_EQ(status_quo.beta, new_beta);
//	ASSERT_EQ(status_quo.best_moves.PV, CMove::A1);
//	ASSERT_EQ(status_quo.best_moves.AV, CMove::B2);
//}
//
//TEST(StatusQuo, Improving_on_analysis_1)
//{
//	// analysis bounds within alpha beta.
//	Improving_on_analysis(-65, +65,  // alpha, beta
//						  -10, +10,  // min, max
//						  -10, +65); // new_alpha, new_beta
//}
//
//TEST(StatusQuo, Improving_on_analysis_2)
//{
//	// analysis bounds match alpha beta.
//	Improving_on_analysis(-10, +10,  // alpha, beta
//						  -10, +10,  // min, max
//						  -10, +10); // new_alpha, new_beta
//}
//
//TEST(StatusQuo, Improving_on_analysis_3)
//{
//	// no analysis bounds.
//	Improving_on_analysis(-10, +10,  // alpha, beta
//						  -64, +64,  // min, max
//						  -10, +10); // new_alpha, new_beta
//}
//
//TEST(StatusQuo, Improving_on_analysis_4)
//{
//	// analysis bounds below alpha and beta.
//	Improving_on_analysis(-10, +10,  // alpha, beta
//						  -20, +01,  // min, max
//						  -10, +10); // new_alpha, new_beta
//}
//
//TEST(StatusQuo, Improving_on_analysis_5)
//{
//	// analysis bounds above alpha and beta.
//	Improving_on_analysis(-10, +10,  // alpha, beta
//						  -01, +20,  // min, max
//						  -01, +10); // new_alpha, new_beta
//}
//
//TEST(StatusQuo, Improving_on_analysis_6)
//{
//	// analysis bounds above alpha and beta.
//	const auto input = CInput(CPosition(), -10, +10, 64, 0);
//	CStatusQuo status_quo(input);
//
//	const auto analysis1 = COutput(-1, +20, 64, 0, CMove::A1, CMove::B2);
//	const auto cut1 = status_quo.ImproveWith(analysis1);
//
//	const auto analysis2 = COutput(-2, +1, 64, 0, CMove::A2, CMove::B3);
//	const auto cut2 = status_quo.ImproveWith(analysis2);
//
//	ASSERT_FALSE(cut1);
//	ASSERT_FALSE(cut2);
//	ASSERT_EQ(status_quo.alpha, -1);
//	ASSERT_EQ(status_quo.beta, +10);
//	ASSERT_EQ(status_quo.PV, CMove::A2);
//	ASSERT_EQ(status_quo.AV, CMove::B3);
//}
//
//void Cutting_on_analysis(int8_t alpha, int8_t beta, int8_t min, int8_t max, int8_t cut_min, int8_t cut_max)
//{
//	const auto input = CInput(CPosition(), alpha, beta, 64, 0);
//	CStatusQuo status_quo(input);
//
//	const auto analysis = COutput(min, max, 64, 0, CMove::A1, CMove::B2);
//	const auto cut = status_quo.ImproveWith(analysis);
//
//	ASSERT_TRUE(cut);
//	ASSERT_EQ(cut.value().min, cut_min);
//	ASSERT_EQ(cut.value().max, cut_max);
//	ASSERT_EQ(status_quo.PV, CMove::A1);
//	ASSERT_EQ(status_quo.AV, CMove::B2);
//}
//
//TEST(StatusQuo, Cutting_on_analysis_1)
//{
//	// analysis bounds below.
//	Cutting_on_analysis(-10, +10,  // alpha, beta
//						-21, -20,  // min, max
//						-64, -20); // cut_min, cut_max
//}
//
//TEST(StatusQuo, Cutting_on_analysis_2)
//{
//	// analysis bounds above.
//	Cutting_on_analysis(-10, +10,  // alpha, beta
//						+20, +21,  // min, max
//						+20, +64); // cut_min, cut_max
//}
//
//TEST(StatusQuo, Cutting_on_analysis_3)
//{
//	// analysis bounds within.
//	Cutting_on_analysis(-10, +10,  // alpha, beta
//						+02, +02,  // min, max
//						+02, +02); // cut_min, cut_max
//}
//
////TEST(StatusQuo, Cutting_on_analysis_4)
////{
////	// analysis bounds within.
////	const auto input = CInput(CPosition(), -10, +10, 64, 0);
////	CStatusQuo status_quo(input);
////
////	const auto analysis1 = COutput(-64, +2, 64, 0, CMove::A1, CMove::B2);
////	const auto cut1 = status_quo.ImproveWith(analysis1);
////
////	ASSERT_FALSE(cut1);
////	ASSERT_EQ(status_quo.alpha, -10);
////	ASSERT_EQ(status_quo.beta, +10);
////
////	const auto analysis2 = COutput(+2, +64, 64, 0, CMove::A2, CMove::B3);
////	const auto cut2 = status_quo.ImproveWith(analysis2);
////
////	ASSERT_TRUE(cut2);
////	ASSERT_EQ(cut2.value().min, +2);
////	ASSERT_EQ(cut2.value().max, +2);
////	ASSERT_EQ(status_quo.PV, CMove::A2);
////	ASSERT_EQ(status_quo.AV, CMove::B3);
////}
//
//TEST(StatusQuo, Improving_move)
//{
//	const auto input = CInput(CPosition(), -10, +10, 64, 0);
//	CStatusQuo status_quo(input);
//
//	const auto analysis = COutput(+2, +60, 64, 0, CMove::A1, CMove::B2);
//	const auto cut1 = status_quo.ImproveWith(analysis);
//	ASSERT_FALSE(cut1);
//
//	const auto move = COutput(+3, +3, 64, 0, CMove::A4, CMove::B4);
//	const auto cut2 = status_quo.ImproveWith(move, CMove::A2);
//	ASSERT_FALSE(cut2);
//
//	ASSERT_EQ(status_quo.alpha, +3);
//	ASSERT_EQ(status_quo.beta, +10);
//	//ASSERT_EQ(status_quo.min, +2);
//	//ASSERT_EQ(status_quo.max, +60);
//	ASSERT_EQ(status_quo.best_score, +3);
//	ASSERT_EQ(status_quo.PV, CMove::A2);
//	ASSERT_EQ(status_quo.AV, CMove::A1);
//}
//
//TEST(StatusQuo, Beta_cut_move)
//{
//	const auto input = CInput(CPosition(), -10, +10, 64, 0);
//	CStatusQuo status_quo(input);
//
//	const auto analysis = COutput(+2, +60, 64, 0, CMove::A1, CMove::B2);
//	const auto cut1 = status_quo.ImproveWith(analysis);
//	ASSERT_FALSE(cut1);
//
//	const auto move = COutput(+11, +64, 64, 0, CMove::A4, CMove::B4);
//	const auto cut2 = status_quo.ImproveWith(move, CMove::A2);
//
//	ASSERT_TRUE(cut2);
//	ASSERT_EQ(cut2.value().min, +11);
//	ASSERT_EQ(cut2.value().max, +64);
//}
//
//TEST(StatusQuo, Max_score_move)
//{
//	const auto input = CInput(CPosition(), -10, +10, 64, 0);
//	CStatusQuo status_quo(input);
//
//	const auto analysis = COutput(+2, +60, 64, 0, CMove::A1, CMove::B2);
//	const auto cut1 = status_quo.ImproveWith(analysis);
//
//	ASSERT_FALSE(cut1);
//	ASSERT_EQ(status_quo.alpha, +2);
//	ASSERT_EQ(status_quo.beta, +10);
//
//	const auto move = COutput(+60, +64, 64, 0, CMove::A4, CMove::B4);
//	const auto cut2 = status_quo.ImproveWith(move, CMove::A2);
//
//	ASSERT_TRUE(cut2);
//	ASSERT_EQ(cut2.value().min, +60);
//	ASSERT_EQ(cut2.value().max, +64);
//}
//
//TEST(StatusQuo, sub_alpha_moves)
//{
//	const auto input = CInput(CPosition(), -10, +10, 64, 0);
//	CStatusQuo status_quo(input);
//
//	const auto move1 = COutput(-64, -20, 64, 0, CMove::A4, CMove::B4);
//	const auto cut1 = status_quo.ImproveWith(move1, CMove::A2);
//
//	ASSERT_FALSE(cut1);
//	ASSERT_EQ(status_quo.alpha, -10);
//	ASSERT_EQ(status_quo.beta, +10);
//	ASSERT_EQ(status_quo.PV, CMove::A2);
//	ASSERT_EQ(status_quo.AV, CMove::invalid);
//
//	const auto move2 = COutput(-15, -11, 64, 0, CMove::A4, CMove::B4);
//	const auto cut2 = status_quo.ImproveWith(move2, CMove::A3);
//
//	ASSERT_FALSE(cut2);
//	ASSERT_EQ(status_quo.alpha, -10);
//	ASSERT_EQ(status_quo.beta, +10);
//	ASSERT_EQ(status_quo.PV, CMove::A3);
//	ASSERT_EQ(status_quo.AV, CMove::A2);
//
//	const auto ret = status_quo.AllMovesTried();
//
//	ASSERT_EQ(ret.min, -15);
//	ASSERT_EQ(ret.max, -10);
//	ASSERT_EQ(ret.PV, CMove::A3);
//	ASSERT_EQ(ret.AV, CMove::A2);
//}
////TEST(StatusQuo, Max_score_move)
////{
////}