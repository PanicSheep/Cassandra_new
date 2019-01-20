#include "pch.h"
#include "Search.h"
#include "SearchInput.h"
#include "SearchOutput.h"
#include "SearchStatusQuo.h"

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


void ExactSearch_NoCut(const int score)
{
	const int8_t depth = 64;
	const uint8_t selectivity = 0;
	CInput input(CPosition(), -infinity, +infinity, depth, selectivity);
	CStatusQuo status_quo(input);

	const auto result = status_quo.ImproveWith(COutput::ExactScore(score, depth - 1, selectivity), CMove::A1);
	ASSERT_FALSE(result.has_value());
}

TEST(StatusQuo, ExactSearch_NoCut_MinScore)
{
	ExactSearch_NoCut(-64);
}

TEST(StatusQuo, ExactSearch_NoCut_ArbitraryScore)
{
	ExactSearch_NoCut(0);
}

TEST(StatusQuo, ExactSearch_NoCut_MaxScore)
{
	ExactSearch_NoCut(+64);
}


void ExactSearch_ExactScore_OneMove(const int score)
{
	const int8_t depth = 64;
	const uint8_t selectivity = 0;
	CInput input(CPosition(), -infinity, +infinity, depth, selectivity);
	CStatusQuo status_quo(input);

	status_quo.ImproveWith(COutput::ExactScore(score, depth - 1, selectivity), CMove::A1);

	const auto result = status_quo.AllMovesTried();
	ASSERT_EQ(result.best_moves.PV, CMove::A1);
	ASSERT_EQ(result.depth, depth);
	ASSERT_EQ(result.selectivity, selectivity);
	ASSERT_EQ(result.min, score);
	ASSERT_EQ(result.max, score);
}

TEST(StatusQuo, ExactSearch_ExactScore_MinScore)
{
	ExactSearch_ExactScore_OneMove(-64);
}

TEST(StatusQuo, ExactSearch_ExactScore_ArbitraryScore)
{
	ExactSearch_ExactScore_OneMove(0);
}

TEST(StatusQuo, ExactSearch_ExactScore_MaxScore)
{
	ExactSearch_ExactScore_OneMove(+64);
}


void ExactSearch_ExactScore_TwoMoves(const int smaller_score, const int bigger_score)
{
	ASSERT_LE(smaller_score, bigger_score); // Input test.

	const int8_t depth = 64;
	const uint8_t selectivity = 0;
	CInput input(CPosition(), -infinity, +infinity, depth, selectivity);
	CStatusQuo status_quo(input);

	status_quo.ImproveWith(COutput::ExactScore(smaller_score, depth - 1, selectivity), CMove::A1);
	status_quo.ImproveWith(COutput::ExactScore(bigger_score, depth - 1, selectivity), CMove::A2);

	const auto result = status_quo.AllMovesTried();

	ASSERT_EQ(result.best_moves.PV, CMove::A2);
	ASSERT_EQ(result.best_moves.AV, CMove::A1);
	ASSERT_EQ(result.depth, depth);
	ASSERT_EQ(result.selectivity, selectivity);
	ASSERT_EQ(result.min, bigger_score);
	ASSERT_EQ(result.max, bigger_score);
}

void ExactSearch_ExactScore_TwoMoves(const int score)
{
	const int8_t depth = 64;
	const uint8_t selectivity = 0;
	CInput input(CPosition(), -infinity, +infinity, depth, selectivity);
	CStatusQuo status_quo(input);

	status_quo.ImproveWith(COutput::ExactScore(score, depth - 1, selectivity), CMove::A1);
	status_quo.ImproveWith(COutput::ExactScore(score, depth - 1, selectivity), CMove::A2);

	const auto result = status_quo.AllMovesTried();

	ASSERT_EQ(result.best_moves.PV, CMove::A1);
	ASSERT_EQ(result.depth, depth);
	ASSERT_EQ(result.selectivity, selectivity);
	ASSERT_EQ(result.min, score);
	ASSERT_EQ(result.max, score);
}

TEST(StatusQuo, ExactSearch_ExactScore_TwoMoves_MinScore)
{
	ExactSearch_ExactScore_TwoMoves(-64, -63);
}

TEST(StatusQuo, ExactSearch_ExactScore_TwoMoves_ArbitraryScore)
{
	ExactSearch_ExactScore_TwoMoves(0, 1);
}

TEST(StatusQuo, ExactSearch_ExactScore_TwoMoves_MaxScore)
{
	ExactSearch_ExactScore_TwoMoves(+63, +64);
}

TEST(StatusQuo, ExactSearch_ExactScore_TwoMoves_SameMinScore)
{
	ExactSearch_ExactScore_TwoMoves(-64);
}

TEST(StatusQuo, ExactSearch_ExactScore_TwoMoves_SameArbitraryScore)
{
	ExactSearch_ExactScore_TwoMoves(0);
}

TEST(StatusQuo, ExactSearch_ExactScore_TwoMoves_SameMaxScore)
{
	ExactSearch_ExactScore_TwoMoves(+64);
}


void WindowSearch_CutTest(const int score)
{
	const int8_t depth = 64;
	const uint8_t selectivity = 0;
	const int alpha = -1;
	const int beta = +1;
	CInput input(CPosition(), alpha, beta, depth, selectivity);
	CStatusQuo status_quo(input);

	const auto result = status_quo.ImproveWith(COutput::ExactScore(score, depth - 1, selectivity), CMove::A1);
	ASSERT_EQ(result.has_value(), (score >= beta));
}

TEST(StatusQuo, WindowSearch_CutTest_MinScore)
{
	WindowSearch_CutTest(-64);
}

TEST(StatusQuo, WindowSearch_CutTest_ArbitraryScore_NoCut)
{
	WindowSearch_CutTest(0);
}
TEST(StatusQuo, WindowSearch_CutTest_ArbitraryScore_Cut)
{
	WindowSearch_CutTest(+1);
}

TEST(StatusQuo, WindowSearch_CutTest_MaxScore)
{
	WindowSearch_CutTest(+64);
}


void WindowSearch_ExactScore_FailLow(const int score)
{
	const int8_t depth = 64;
	const uint8_t selectivity = 0;
	const int alpha = -1;
	const int beta = +1;
	CInput input(CPosition(), alpha, beta, depth, selectivity);
	CStatusQuo status_quo(input);

	status_quo.ImproveWith(COutput::ExactScore(score, depth - 1, selectivity), CMove::A1);

	const auto result = status_quo.AllMovesTried();
	ASSERT_EQ(result.best_moves.PV, CMove::A1);
	ASSERT_EQ(result.depth, depth);
	ASSERT_EQ(result.selectivity, selectivity);
	ASSERT_EQ(result.min, -infinity);
	ASSERT_LE(result.max, alpha);
}

void WindowSearch_ExactScore_FailExtraLow(const int score)
{
	const int8_t depth = 64;
	const uint8_t selectivity = 0;
	const int alpha = -1;
	const int beta = +1;
	CInput input(CPosition(), alpha, beta, depth, selectivity);
	CStatusQuo status_quo(input);

	status_quo.ImproveWith(COutput::ExactScore(score, depth - 1, selectivity), CMove::A1);

	const auto result = status_quo.AllMovesTried();
	ASSERT_EQ(result.best_moves.PV, CMove::A1);
	ASSERT_EQ(result.depth, depth);
	ASSERT_EQ(result.selectivity, selectivity);
	ASSERT_EQ(result.min, -infinity);
	ASSERT_EQ(result.max, score);
}

TEST(StatusQuo, WindowSearch_ExactScore_FailLow_MaxScore)
{
	WindowSearch_ExactScore_FailLow(-1);
}
TEST(StatusQuo, WindowSearch_ExactScore_FailLow_ArbitraryScore)
{
	WindowSearch_ExactScore_FailLow(-10);
}
TEST(StatusQuo, WindowSearch_ExactScore_FailLow_MinScore)
{
	WindowSearch_ExactScore_FailLow(-64);
}

TEST(StatusQuo, WindowSearch_ExactScore_FailExtraLow_MaxScore)
{
	WindowSearch_ExactScore_FailExtraLow(-1);
}
TEST(StatusQuo, WindowSearch_ExactScore_FailExtraLow_ArbitraryScore)
{
	WindowSearch_ExactScore_FailExtraLow(-10);
}
TEST(StatusQuo, WindowSearch_ExactScore_FailExtraLow_MinScore)
{
	WindowSearch_ExactScore_FailExtraLow(-64);
}


void WindowSearch_MinBound(const int score)
{
	const int8_t depth = 64;
	const uint8_t selectivity = 0;
	const int alpha = -1;
	const int beta = +1;
	CInput input(CPosition(), alpha, beta, depth, selectivity);
	CStatusQuo status_quo(input);

	// This simulates a child returning from fail low.
	const auto result = status_quo.ImproveWith(COutput::MinBound(score, depth - 1, selectivity), CMove::A1);
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(result.value().best_moves.PV, CMove::A1);
	ASSERT_EQ(result.value().depth, depth);
	ASSERT_EQ(result.value().selectivity, selectivity);
	ASSERT_EQ(result.value().min, score);
	ASSERT_EQ(result.value().max, +infinity);
}

TEST(StatusQuo, WindowSearch_MinBound_MinScore)
{
	WindowSearch_MinBound(+1);
}

TEST(StatusQuo, WindowSearch_MinBound_ArbitraryScore)
{
	WindowSearch_MinBound(+10);
}

TEST(StatusQuo, WindowSearch_MinBound_MaxScore)
{
	WindowSearch_MinBound(+64);
}


void WindowSearch_MaxBound(const int score)
{
	const int8_t depth = 64;
	const uint8_t selectivity = 0;
	const int alpha = -1;
	const int beta = +1;
	CInput input(CPosition(), alpha, beta, depth, selectivity);
	CStatusQuo status_quo(input);

	// This simulates a child returning from a beta cut.
	status_quo.ImproveWith(COutput::MaxBound(score, depth - 1, selectivity), CMove::A1);

	ASSERT_EQ(status_quo.best_moves.PV, CMove::invalid);
	ASSERT_EQ(status_quo.worst_depth, depth);
	ASSERT_EQ(status_quo.worst_selectivity, selectivity);
	ASSERT_EQ(status_quo.best_score, -infinity);
}

TEST(StatusQuo, WindowSearch_MaxBound_MinScore)
{
	WindowSearch_MaxBound(-1);
}

TEST(StatusQuo, WindowSearch_MaxBound_ArbitraryScore)
{
	WindowSearch_MaxBound(-10);
}

TEST(StatusQuo, WindowSearch_MaxBound_MaxScore)
{
	WindowSearch_MaxBound(-64);
}


void SelectiveSearch_CutTest(const int score)
{
	const int8_t depth = 5;
	const uint8_t selectivity = 10;
	const int alpha = -1;
	const int beta = +1;
	CInput input(CPosition(), alpha, beta, depth, selectivity);
	CStatusQuo status_quo(input);

	const auto result = status_quo.ImproveWith(COutput::ExactScore(score, depth - 1, selectivity), CMove::A1);
	ASSERT_EQ(result.has_value(), (score >= beta));
}

TEST(StatusQuo, SelectiveSearch_CutTest_MinScore)
{
	SelectiveSearch_CutTest(-64);
}

TEST(StatusQuo, SelectiveSearch_CutTest_ArbitraryScore_NoCut)
{
	SelectiveSearch_CutTest(0);
}
TEST(StatusQuo, SelectiveSearch_CutTest_ArbitraryScore_Cut)
{
	SelectiveSearch_CutTest(+1);
}

TEST(StatusQuo, SelectiveSearch_CutTest_MaxScore)
{
	SelectiveSearch_CutTest(+64);
}


TEST(StatusQuo, SelectiveSearch_ExactScore)
{
	const int8_t depth = 5;
	const uint8_t selectivity = 10;
	const int alpha = -1;
	const int beta = +1;
	const int score = 0;
	CInput input(CPosition(), alpha, beta, depth, selectivity);
	CStatusQuo status_quo(input);

	status_quo.ImproveWith(COutput::ExactScore(score, depth - 1, selectivity), CMove::A1);

	const auto result = status_quo.AllMovesTried();
	ASSERT_EQ(result.best_moves.PV, CMove::A1);
	ASSERT_EQ(result.depth, depth);
	ASSERT_EQ(result.selectivity, selectivity);
	ASSERT_EQ(result.min, score);
	ASSERT_EQ(result.max, score);
}


void SelectiveSearch_ExactScore_TwoMoves(const int smaller_score, const int bigger_score)
{
	ASSERT_LE(smaller_score, bigger_score); // Input test.

	const int8_t depth = 5;
	const uint8_t selectivity = 10;
	CInput input(CPosition(), -infinity, +infinity, depth, selectivity);
	CStatusQuo status_quo(input);

	status_quo.ImproveWith(COutput::ExactScore(smaller_score, depth - 1, selectivity), CMove::A1);
	status_quo.ImproveWith(COutput::ExactScore(bigger_score, depth - 1, selectivity), CMove::A2);

	const auto result = status_quo.AllMovesTried();

	ASSERT_EQ(result.best_moves.PV, CMove::A2);
	ASSERT_EQ(result.best_moves.AV, CMove::A1);
	ASSERT_EQ(result.depth, depth);
	ASSERT_EQ(result.selectivity, selectivity);
	ASSERT_EQ(result.min, bigger_score);
	ASSERT_EQ(result.max, bigger_score);
}

void SelectiveSearch_ExactScore_TwoMoves(const int score)
{
	const int8_t depth = 5;
	const uint8_t selectivity = 10;
	CInput input(CPosition(), -infinity, +infinity, depth, selectivity);
	CStatusQuo status_quo(input);

	status_quo.ImproveWith(COutput::ExactScore(score, depth - 1, selectivity), CMove::A1);
	status_quo.ImproveWith(COutput::ExactScore(score, depth - 1, selectivity), CMove::A2);

	const auto result = status_quo.AllMovesTried();

	ASSERT_EQ(result.best_moves.PV, CMove::A1);
	ASSERT_EQ(result.depth, depth);
	ASSERT_EQ(result.selectivity, selectivity);
	ASSERT_EQ(result.min, score);
	ASSERT_EQ(result.max, score);
}

TEST(StatusQuo, SelectiveSearch_ExactScore_TwoMoves_MinScore)
{
	SelectiveSearch_ExactScore_TwoMoves(-64, -63);
}

TEST(StatusQuo, SelectiveSearch_ExactScore_TwoMoves_ArbitraryScore)
{
	SelectiveSearch_ExactScore_TwoMoves(0, 1);
}

TEST(StatusQuo, SelectiveSearch_ExactScore_TwoMoves_MaxScore)
{
	SelectiveSearch_ExactScore_TwoMoves(+63, +64);
}

TEST(StatusQuo, SelectiveSearch_ExactScore_TwoMoves_SameMinScore)
{
	SelectiveSearch_ExactScore_TwoMoves(-64);
}

TEST(StatusQuo, SelectiveSearch_ExactScore_TwoMoves_SameArbitraryScore)
{
	SelectiveSearch_ExactScore_TwoMoves(0);
}

TEST(StatusQuo, SelectiveSearch_ExactScore_TwoMoves_SameMaxScore)
{
	SelectiveSearch_ExactScore_TwoMoves(+64);
}


void SelectiveSearch_ExactScore_TwoMoves_ReturnMoreThanRequested(const int smaller_score, const int bigger_score)
{
	ASSERT_LE(smaller_score, bigger_score); // Input test.

	const int8_t initial_depth = 5;
	const int8_t depth1 = 20;
	const int8_t depth2 = 21;
	const uint8_t initial_selectivity = 10;
	const uint8_t selectivity1 = 5;
	const uint8_t selectivity2 = 6;
	CInput input(CPosition(), -infinity, +infinity, initial_depth, initial_selectivity);
	CStatusQuo status_quo(input);

	status_quo.ImproveWith(COutput::ExactScore(smaller_score, depth1 - 1, selectivity1), CMove::A1);
	status_quo.ImproveWith(COutput::ExactScore(bigger_score, depth2 - 1, selectivity2), CMove::A2);

	const auto result = status_quo.AllMovesTried();

	ASSERT_EQ(result.best_moves.PV, CMove::A2);
	ASSERT_EQ(result.best_moves.AV, CMove::A1);
	ASSERT_EQ(result.depth, std::min(depth1, depth2));
	ASSERT_EQ(result.selectivity, std::max(selectivity1, selectivity2));
	ASSERT_EQ(result.min, bigger_score);
	ASSERT_EQ(result.max, bigger_score);
}

TEST(StatusQuo, SelectiveSearch_ExactScore_TwoMoves_ReturnMoreThanRequested_MinScore)
{
	SelectiveSearch_ExactScore_TwoMoves_ReturnMoreThanRequested(-64, -63);
}

TEST(StatusQuo, SelectiveSearch_ExactScore_TwoMoves_ReturnMoreThanRequested_ArbitraryScore)
{
	SelectiveSearch_ExactScore_TwoMoves_ReturnMoreThanRequested(0, 1);
}

TEST(StatusQuo, SelectiveSearch_ExactScore_TwoMoves_ReturnMoreThanRequested_MaxScore)
{
	SelectiveSearch_ExactScore_TwoMoves_ReturnMoreThanRequested(+63, +64);
}


TEST(StatusQuo, ExactSearch_Analysis_noEffect)
{
	const int8_t depth = 64;
	const uint8_t selectivity = 0;
	const int score = 0;
	CInput input(CPosition(), -infinity, +infinity, depth, selectivity);
	CStatusQuo status_quo(input);

	// This simulates a hash table value.
	const auto ret = status_quo.ImproveWith(CAnalysisOutput(0, 0, 5, 0, CBestMoves(CMove::B1, CMove::B2)));
	ASSERT_FALSE(ret.has_value());

	status_quo.ImproveWith(COutput::ExactScore(score, depth - 1, selectivity), CMove::A2);

	const auto result = status_quo.AllMovesTried();

	ASSERT_EQ(result.best_moves.PV, CMove::A2);
	ASSERT_EQ(result.best_moves.AV, CMove::B1);
	ASSERT_EQ(result.depth, depth);
	ASSERT_EQ(result.selectivity, selectivity);
	ASSERT_EQ(result.min, score);
	ASSERT_EQ(result.max, score);
}

TEST(StatusQuo, ExactSearch_Analysis_BetaCut)
{
	const int8_t depth = 64;
	const uint8_t selectivity = 0;
	const int score = 0;
	const int alpha = -10;
	const int beta = +10;
	CInput input(CPosition(), alpha, beta, depth, selectivity);
	CStatusQuo status_quo(input);

	// This simulates a hash table value.
	const auto ret = status_quo.ImproveWith(CAnalysisOutput::MinBound(beta, depth, 10, CBestMoves(CMove::B1, CMove::B2)));
	ASSERT_FALSE(ret.has_value());

	status_quo.ImproveWith(COutput::ExactScore(score, depth - 1, selectivity), CMove::A2);

	const auto result = status_quo.AllMovesTried();

	ASSERT_EQ(result.best_moves.PV, CMove::A2);
	ASSERT_EQ(result.best_moves.AV, CMove::B1);
	ASSERT_EQ(result.depth, depth);
	ASSERT_EQ(result.selectivity, selectivity);
	ASSERT_EQ(result.min, score);
	ASSERT_EQ(result.max, score);
}