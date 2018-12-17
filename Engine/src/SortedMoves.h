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
	std::vector<std::pair<int32_t, CMove>> m_moves;
public:
	CSortedMoves(const CPosition&, std::function<int32_t(CMove)> score, CMove filter1 = CMove::invalid, CMove filter2 = CMove::invalid);

	std::size_t size() const { return m_moves.size(); }
	bool empty() const { return m_moves.empty(); }

	CMove PeekMove() const { return m_moves.back().second; }
	CMove ExtractMove();

	auto begin() const { return m_moves.rbegin(); }
	auto cbegin() const { return m_moves.crbegin(); }
	auto end() const { return m_moves.rend(); }
	auto cend() const { return m_moves.crend(); }
};

inline CSortedMoves::CSortedMoves(const CPosition& pos, std::function<int32_t(CMove)> score, const CMove filter1, const CMove filter2)
{
	CMoves moves = pos.PossibleMoves();
	moves.Remove(filter1);
	moves.Remove(filter2);
	m_moves.reserve(moves.size());

	while (!moves.empty()) {
		const auto move = moves.ExtractMove();
		m_moves.emplace_back(score(move), move);
	}

	std::sort(m_moves.begin(), m_moves.end(), [](auto& left, auto& right) { return left.first < right.first; });
}

inline CMove CSortedMoves::ExtractMove()
{
	auto back = m_moves.back();
	m_moves.pop_back();
	return back.second;
}