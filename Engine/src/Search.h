#pragma once
#include <algorithm>
#include <cstdint>
#include "Position.h"
#include "Environment.h"
#include "HashtablePVS.h"

class Search
{
	uint64_t node_counter;
public:
	Search(const std::shared_ptr<Environment>& env);

	virtual int Eval(const CPosition&) = 0;

	uint64_t GetNodeCount() const;
	uint64_t GetNodeCount(uint64_t EmptyCount) const;
	void ResetNodeCount();

protected:
	std::shared_ptr<Environment> environment;

	uint64_t& NodeCounter(std::size_t index);

	template <int EmptyCount>
	static int EvalGameOver(const CPosition&);
	static int EvalGameOver(const CPosition&);
	static int EvalGameOver(const CPosition&, const uint64_t EmptyCount);
};

class NegaMaxSearch : public Search
{
public:
	NegaMaxSearch(const std::shared_ptr<Environment>& env) : Search(env) {}

	int Eval(const CPosition&) override;
private:
	int Eval_1(const CPosition&);
	int Eval_2(const CPosition&);
	int Eval_3(const CPosition&);
	int Eval_4(const CPosition&);

	int Eval_0(const CPosition&);
	int Eval_1(const CPosition&, const CMove& move1);
	int Eval_2(const CPosition&, const CMove& move1, const CMove& move2);
	int Eval_3(const CPosition&, const CMove& move1, const CMove& move2, const CMove& move3);
	int Eval_4(const CPosition&, const CMove& move1, const CMove& move2, const CMove& move3, const CMove& move4);
	int Eval_N(const CPosition&);
};

class AlphaBetaFailSoftSearch : public Search
{
public:
	AlphaBetaFailSoftSearch(const std::shared_ptr<Environment>& env) : Search(env) {}

	int Eval(const CPosition&) override;

	int Eval(const CPosition&, int alpha, int beta);
private:
	int Eval_1(const CPosition&, int alpha);
	int Eval_2(const CPosition&, int alpha, int beta);
	int Eval_3(const CPosition&, int alpha, int beta);
	int Eval_4(const CPosition&, int alpha, int beta);

	int Eval_0(const CPosition&);
	int Eval_1(const CPosition&, int alpha,           const CMove& move1);
	int Eval_2(const CPosition&, int alpha, int beta, const CMove& move1, const CMove& move2);
	int Eval_3(const CPosition&, int alpha, int beta, const CMove& move1, const CMove& move2, const CMove& move3);
	int Eval_4(const CPosition&, int alpha, int beta, const CMove& move1, const CMove& move2, const CMove& move3, const CMove& move4);
	int Eval_N(const CPosition&, int alpha, int beta);
};

class AlphaBetaFailHardSearch : public Search
{
public:
	AlphaBetaFailHardSearch(const std::shared_ptr<Environment>& env) : Search(env) {}

	int Eval(const CPosition&) override;

	int Eval(const CPosition&, int alpha, int beta);
private:
	int Eval_1(const CPosition&, int alpha, int beta);
	int Eval_2(const CPosition&, int alpha, int beta);
	int Eval_3(const CPosition&, int alpha, int beta);
	int Eval_4(const CPosition&, int alpha, int beta);

	int Eval_0(const CPosition&, int alpha, int beta);
	int Eval_1(const CPosition&, int alpha, int beta, const CMove& move1);
	int Eval_2(const CPosition&, int alpha, int beta, const CMove& move1, const CMove& move2);
	int Eval_3(const CPosition&, int alpha, int beta, const CMove& move1, const CMove& move2, const CMove& move3);
	int Eval_4(const CPosition&, int alpha, int beta, const CMove& move1, const CMove& move2, const CMove& move3, const CMove& move4);
	int Eval_N(const CPosition&, int alpha, int beta);
};

class PVSearch : public Search
{
	struct InputValues
	{
		CPosition pos;
		int8_t alpha;
		int8_t beta;
		int8_t depth;
		uint8_t selectivity;

		InputValues(CPosition pos, int8_t alpha, int8_t beta, int8_t depth, uint8_t selectivity)
			: pos(pos), alpha(alpha), beta(beta), depth(depth), selectivity(selectivity)
		{}

		InputValues PlayPass() const { return InputValues(pos.PlayPass(), -beta, -alpha, depth, selectivity); }
		InputValues ToZWS() const { return InputValues(pos, alpha-1, alpha, depth, selectivity); }
	};
	struct AnalysisReturnValues
	{
		int8_t alpha;
		int8_t beta;
		int8_t depth;
		uint8_t selectivity;
		CMove PV;
		CMove AV;

		AnalysisReturnValues() : AnalysisReturnValues(0, 0, -1, 0) {}
		AnalysisReturnValues(int8_t alpha, int8_t beta, int8_t depth, uint8_t selectivity, CMove PV = Field::invalid, CMove AV = Field::invalid)
			: alpha(alpha), beta(beta), depth(depth), selectivity(selectivity), PV(PV), AV(AV)
		{}

		bool empty() const { return depth == -1; }
	};
	struct ReturnValues
	{
		int8_t alpha;
		int8_t beta;
		int8_t depth;
		uint8_t selectivity;

		ReturnValues(int8_t alpha, int8_t beta, int8_t depth, uint8_t selectivity)
			: alpha(alpha), beta(beta), depth(depth), selectivity(selectivity)
		{}
		ReturnValues(int8_t score, int8_t depth, uint8_t selectivity)
			: ReturnValues(score, score, depth, selectivity)
		{}
		ReturnValues() {}

		friend ReturnValues operator-(ReturnValues o) { std::swap(o.alpha, o.beta); o.alpha = -o.alpha; o.beta = -o.beta; return o; }

		int GetScore() const {
			if (alpha == -64)
				return beta;
			return alpha;
		}
	};
	struct AAA
	{
		bool CausesCut;
		ReturnValues Values;

		AAA(bool CausesCut, ReturnValues Values) : CausesCut(CausesCut), Values(Values) {}
	};
	struct StatusValues
	{
		uint64_t InitialNodeCount;
		const CPosition& pos;
		int8_t alpha;
		int8_t beta;
		int8_t depth;
		uint8_t selectivity;
		CMove PV;
		CMove AV;
		bool improved = false;

		StatusValues(const InputValues& in, uint64_t NodeCount)
			: alpha(in.alpha), beta(in.beta), depth(in.depth), selectivity(in.selectivity), PV(Field::invalid), AV(Field::invalid)
			, InitialNodeCount(NodeCount), pos(in.pos)
		{}

		InputValues Play(const CMove& move) const { return InputValues(pos.Play(move), -beta, -alpha, depth-1, selectivity); }
		InputValues PlayZWS(const CMove& move) const { return InputValues(pos.Play(move), -alpha-1, -alpha, depth-1, selectivity); }

		AAA ImproveWith(const AnalysisReturnValues& ret)
		{
			if ((ret.depth >= depth) && (ret.selectivity <= selectivity))
			{
				if (ret.beta <= alpha) // Alpha Cut.
					return AAA(true, ReturnValues(-64, ret.beta, ret.depth, ret.selectivity));
				if (ret.alpha >= beta) // Beta Cut.
					return AAA(true, ReturnValues(ret.alpha, +64, ret.depth, ret.selectivity));
				else if (ret.alpha > alpha)
				{
					alpha = alpha;
					//beta = std::min(beta, ret.beta);
					depth = ret.depth;
					selectivity = ret.selectivity;
					if (ret.PV != Field::invalid) PV = ret.PV; // TODO: Test this, and decide what to do.
					if (ret.AV != Field::invalid) AV = ret.AV; // TODO: Test this, and decide what to do.
				}
			}
			return AAA(false, ReturnValues());
		}
		AAA ImproveWith(const ReturnValues& ret, const CMove& move)
		{
			if ((ret.depth+1 >= depth) && (ret.selectivity <= selectivity))
			{
				if (ret.alpha >= beta) { // Beta Cut.
					alpha = ret.alpha;
					beta = +64;
					depth = ret.depth + 1;
					selectivity = ret.selectivity;
					if (move != PV) {
						AV = PV;
						PV = move;
					}
					improved = true;
					return AAA(true, ReturnValues(alpha, beta, depth, selectivity));
				}
				else if (ret.alpha > alpha)
				{
					alpha = ret.alpha;
					depth = ret.depth + 1;
					selectivity = ret.selectivity;
					if (move != PV) {
						AV = PV;
						PV = move;
					}
					improved = true;
				}
			}
			return AAA(false, ReturnValues());
		}
		ReturnValues AllMovesSearched()
		{
			beta = alpha;
			if (!improved)
				alpha = -64;
			return ReturnValues(alpha, beta, depth, selectivity);
		}
	};

	struct ConfigValues
	{
		bool StabilityCut, TranspositionCut;

		ConfigValues(bool StabilityCut, bool TranspositionCut)
			: StabilityCut(StabilityCut), TranspositionCut(StabilityCut)
		{}
	};
public:
	PVSearch(const std::shared_ptr<Environment>& env) : Search(env) {}

	int Eval(const CPosition&) override;
private:
	// --- Triage ---------
	int Eval(const CPosition& pos, int alpha, int beta);
	ReturnValues PVS(const InputValues&);
	ReturnValues ZWS(const InputValues&);
	// --------------------

	ReturnValues PVS_N(const InputValues&);
	ReturnValues ZWS_N(const InputValues&);

	// --- Optimized ------
	int Eval_0(const CPosition& pos);

	int PVS_1(const CPosition& pos, int alpha, int beta);
	int PVS_1(const CPosition& pos, int alpha, int beta, const CMove& move1);
	int PVS_2(const CPosition& pos, int alpha, int beta);
	int PVS_2(const CPosition& pos, int alpha, int beta, const CMove& move1, const CMove& move2);

	int ZWS_1(const CPosition& pos, int alpha);
	int ZWS_1(const CPosition& pos, int alpha, const CMove& move1);
	int ZWS_2(const CPosition& pos, int alpha);
	int ZWS_2(const CPosition& pos, int alpha, const CMove& move1, const CMove& move2);
	int ZWS_3(const CPosition& pos, int alpha);
	int ZWS_3(const CPosition& pos, int alpha, const CMove& move1, const CMove& move2, const CMove& move3);
	int ZWS_4(const CPosition& pos, int alpha);
	int ZWS_4(const CPosition& pos, int alpha, const CMove& move1, const CMove& move2, const CMove& move3, const CMove& move4);
	ReturnValues ZWS_A(const InputValues&);
	// --------------------

	AnalysisReturnValues StabilityAnalysis(const InputValues&);
	AnalysisReturnValues TranspositionTableAnalysis(const InputValues&);

	void UpdateTranspositionTable(const StatusValues&);
};

uint64_t Neighbour(Field field);

template <int EmptyCount>
inline int Search::EvalGameOver(const CPosition& pos)
{
	const int Diff = 2 * static_cast<int>(PopCount(pos.GetP())) + EmptyCount - 64;
	if (EmptyCount % 2 == 0)
	{
		if (Diff > 0) return Diff + EmptyCount;
		else if (Diff < 0) return Diff - EmptyCount;
		else               return Diff;
	}
	else
	{
		if (Diff > 0) return Diff + EmptyCount;
		else          return Diff - EmptyCount;
	}
}

template <>
inline int Search::EvalGameOver<0>(const CPosition& pos)
{
	return static_cast<int>(2 * PopCount(pos.GetP())) - 64;
}


class CSortedMoves
{
	std::vector<std::pair<int32_t, CMove>> moves;
public:
	CSortedMoves() {}
	CSortedMoves(const CPosition&);
	CSortedMoves(const CPosition&, const CMove& filter1, const CMove& filter2);

	std::size_t size() const { return moves.size(); }
	bool empty() const { return moves.empty(); }

	CMove PeekMove() const { return moves.back().second; }
	CMove ExtractMove();

	auto begin() const { return moves.rbegin(); }
	auto end() const { return moves.rend(); }
private:
	int32_t Score(const CMove&, const CPosition&);
};

inline CSortedMoves::CSortedMoves(const CPosition& pos)
{
	CMoves mov = pos.PossibleMoves();
	moves.reserve(mov.size());

	while (!mov.empty()) {
		const auto move = mov.ExtractMove();
		moves.emplace_back(Score(move, pos), move);
	}

	std::sort(moves.begin(), moves.end(), [](auto& left, auto& right) { return left.first < right.first; });
}

inline CSortedMoves::CSortedMoves(const CPosition& pos, const CMove& filter1, const CMove& filter2)
{
	CMoves mov = pos.PossibleMoves();
	moves.reserve(mov.size());

	while (!mov.empty()) {
		const auto move = mov.ExtractMove();
		if (move == filter1) continue;
		if (move == filter2) continue;
		moves.emplace_back(Score(move, pos), move);
	}

	std::sort(moves.begin(), moves.end(), [](auto& left, auto& right) { return left.first < right.first; });
}

inline CMove CSortedMoves::ExtractMove()
{
	auto back = moves.back();
	moves.pop_back();
	return back.second;
}

inline uint64_t SMEAR_BITBOARD(uint64_t B)
{
	// 4x SHIFT, 4x OR, 2x AND
	// = 10 OPs
	B |= ((B >> 1) & 0x7F7F7F7F7F7F7F7FULL) | ((B << 1) & 0xFEFEFEFEFEFEFEFEULL);
	return B | (B >> 8) | (B << 8);
}

inline uint64_t OpponentsExposed(const uint64_t P, const uint64_t O) { return SMEAR_BITBOARD(~(P | O)) & O; } // 13 OPs

inline int32_t CSortedMoves::Score(const CMove& move, const CPosition& pos)
{
	static const uint64_t quadrant_mask[64] = {
		0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL,
		0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL,
		0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL,
		0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL,
		0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL,
		0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL,
		0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL,
		0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL,
	};
	static const int8_t FieldValue[64] = {
		9, 2, 8, 6, 6, 8, 2, 9,
		2, 1, 3, 4, 4, 3, 1, 2,
		8, 3, 7, 5, 5, 7, 3, 8,
		6, 4, 5, 0, 0, 5, 4, 6,
		6, 4, 5, 0, 0, 5, 4, 6,
		8, 3, 7, 5, 5, 7, 3, 8,
		2, 1, 3, 4, 4, 3, 1, 2,
		9, 2, 8, 6, 6, 8, 2, 9,
	};
	static const uint8_t ParityValue[16] = { 0, 20, 0, 10, 1, 10, 2, 10, 3, 5, 3, 4, 3, 4, 3, 4 };

	const auto NextPos = pos.Play(move);
	const auto PM = NextPos.PossibleMoves();
	const int32_t MobilityScore = static_cast<int32_t>(PM.size()) << 17;
	const int32_t CornerMobilityScore = ((PM.HasMove(A1) ? 1 : 0) + (PM.HasMove(A8) ? 1 : 0) + (PM.HasMove(H1) ? 1 : 0) + (PM.HasMove(H8) ? 1 : 0)) << 18;
	const int32_t OpponentsExposedScore = static_cast<int32_t>(PopCount(OpponentsExposed(NextPos.GetP(), NextPos.GetO()))) << 6;
	const int32_t FieldScore = FieldValue[move.field];
	const int32_t ParityScore = ParityValue[PopCount(pos.Empties() & quadrant_mask[move.field])];
	return FieldScore + ParityScore - MobilityScore - CornerMobilityScore - OpponentsExposedScore;
}
