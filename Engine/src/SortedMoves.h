#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include "Position.h"
#include "Engine.h"
#include "HashtablePVS.h"
#include "SearchInput.h"

class CSortedMoves
{
	std::vector<std::pair<int32_t, CMove>> moves;
public:
	CSortedMoves(const CPosition&, std::function<int32_t(CMove)> score, CMove filter1 = CMove::invalid, CMove filter2 = CMove::invalid);

	std::size_t size() const { return moves.size(); }
	bool empty() const { return moves.empty(); }

	CMove PeekMove() const { return moves.back().second; }
	CMove ExtractMove();

	auto begin() const { return moves.rbegin(); }
	auto cbegin() const { return moves.crbegin(); }
	auto end() const { return moves.rend(); }
	auto cend() const { return moves.crend(); }
};

inline CSortedMoves::CSortedMoves(const CPosition& pos, std::function<int32_t(CMove)> score, const CMove filter1, const CMove filter2)
{
	CMoves mov = pos.PossibleMoves();
	mov.Remove(filter1);
	mov.Remove(filter2);
	moves.reserve(mov.size());

	while (!mov.empty()) {
		const auto move = mov.ExtractMove();
		moves.emplace_back(score(move), move);
	}

	std::sort(moves.begin(), moves.end(), [](auto& left, auto& right) { return left.first < right.first; });
}

inline CMove CSortedMoves::ExtractMove()
{
	auto back = moves.back();
	moves.pop_back();
	return back.second;
}