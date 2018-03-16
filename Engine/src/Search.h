#pragma once
#include <algorithm>
#include <cstdint>
#include "Position.h"
#include "Environment.h"

class Search
{
	uint64_t node_counter;
public:
	Search();

	virtual int Eval(const CPosition&) = 0;

	uint64_t GetNodeCount() const;
	uint64_t GetNodeCount(uint64_t EmptyCount) const;
	void ResetNodeCount();

protected:
	uint64_t& NodeCounter(std::size_t index);

	template <int EmptyCount>
	static int EvalGameOver(const CPosition&);
	static int EvalGameOver(const CPosition&);
	static int EvalGameOver(const CPosition&, const uint64_t EmptyCount);
};

class NegaMaxSearch : public Search
{
public:
	NegaMaxSearch() : Search() {}

	virtual int Eval(const CPosition&) override;
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
	AlphaBetaFailSoftSearch() : Search() {}

	virtual int Eval(const CPosition&) override;
	
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
	AlphaBetaFailHardSearch() : Search() {}

	virtual int Eval(const CPosition&) override;
	
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
	std::shared_ptr<Environment> environment;

	struct ReturnValues
	{
		int8_t alpha;
		int8_t beta;
		int8_t depth;
		uint8_t selectivity;
		CMove PV;
		CMove AV;
		uint64_t NodeCount;

		ReturnValues(int8_t alpha, int8_t beta, int8_t depth, uint8_t selectivity, CMove PV, CMove AV, uint64_t NodeCount)
			: alpha(alpha), beta(beta), depth(depth), selectivity(selectivity), PV(PV), AV(AV), NodeCount(NodeCount)
		{}
		ReturnValues(int8_t score, int8_t depth, uint8_t selectivity, CMove PV, CMove AV, uint64_t NodeCount)
			: ReturnValues(score, score, depth, selectivity, PV, AV, NodeCount)
		{}
		ReturnValues() : ReturnValues(0, 0, -1, 0, Field::invalid, Field::invalid, 0) {}

		bool empty() const { return depth == -1; }
		void Add(const ReturnValues& o)
		{
			if (empty() || (o.alpha > alpha) || ((o.alpha == alpha) && (o.beta < beta))) {
				alpha = o.alpha;
				beta = o.beta;
				depth = o.depth;
				selectivity = o.selectivity;
				//AV = PV;
				//PV = move;
			}
			NodeCount += o.NodeCount;
		}

		friend ReturnValues operator-(ReturnValues o) { std::swap(o.alpha, o.beta); o.alpha = -o.alpha; o.beta = -o.beta; return o; }
	};

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

		InputValues Play(const CMove& move) const { return InputValues(pos.Play(move), -beta, -alpha, depth - 1, selectivity); }
		InputValues Play(const CMove& move, uint64_t flips) const { return InputValues(pos.Play(move, flips), -beta, -alpha, depth-1, selectivity); }
		InputValues PlayPass() const { return InputValues(pos.PlayPass(), -beta, -alpha, depth, selectivity); }

		void ImproveWith(const ReturnValues& r) { alpha = std::max(alpha, r.alpha); }
	};
	struct NodeInfo
	{
		CPosition pos;
		int8_t alpha;
		int8_t beta;
		int8_t depth;
		int8_t empties;
		uint8_t selectivity;

		NodeInfo(CPosition pos, int8_t alpha, int8_t beta, int8_t depth, int8_t empties, uint8_t selectivity)
			: pos(pos), alpha(alpha), beta(beta), depth(depth), empties(empties), selectivity(selectivity)
		{}
		NodeInfo(CPosition pos, int8_t score, int8_t depth, uint8_t selectivity)
			: pos(pos), alpha(score), beta(score), depth(depth), empties(empties), selectivity(selectivity)
		{}
	};

	struct ConfigValues
	{
		bool StabilityCut, TranspositionCut;

		ConfigValues(bool StabilityCut, bool TranspositionCut)
			: StabilityCut(StabilityCut), TranspositionCut(StabilityCut)
		{}
	};
public:
	PVSearch() : PVSearch(std::make_shared<Environment>()) {}
	PVSearch(std::shared_ptr<Environment> environment) : Search(), environment(environment) {}

	virtual int Eval(const CPosition&) override;
private:
	// --- Triage ---------
	int Eval(const CPosition& pos, int alpha, int beta);
	int PVS(const CPosition& pos);
	int ZWS(const CPosition& pos);
	// --------------------
	
	int PVS_N(const CPosition& pos, int alpha, int beta);
	int ZWS_N(const CPosition& pos, int alpha);

	// --- Optimized ------
	int Eval_0(const CPosition& pos);

	int PVS_1(const CPosition& pos, int alpha, int beta);
	int PVS_1(const CPosition& pos, int alpha, int beta, const CMove& move1);

	int ZWS_1(const CPosition& pos, int alpha);
	int ZWS_1(const CPosition& pos, int alpha, const CMove& move1);
	int ZWS_2(const CPosition& pos, int alpha);
	int ZWS_2(const CPosition& pos, int alpha, const CMove& move1, const CMove& move2);
	int ZWS_3(const CPosition& pos, int alpha);
	int ZWS_3(const CPosition& pos, int alpha, const CMove& move1, const CMove& move2, const CMove& move3);
	int ZWS_4(const CPosition& pos, int alpha);
	int ZWS_4(const CPosition& pos, int alpha, const CMove& move1, const CMove& move2, const CMove& move3, const CMove& move4);
	// --------------------

	std::pair<bool, int> StabilityCutoff(const NodeInfo& node);
	std::pair<bool, int> TranspositionTableCutoff(const NodeInfo& node);

	void UpdateTranspositionTable(uint64_t NodeCount, const NodeInfo& node);
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
	std::vector<std::pair<int32_t, CMove>> ScoreMovePairs;
	moves.reserve(mov.size());

	while (!mov.empty()) {
		const auto move = mov.ExtractMove();
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
