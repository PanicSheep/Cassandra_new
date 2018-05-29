#pragma once
#include <cstdint>
#include <memory>
#include <vector>

#include "Puzzles.h"

class PuzzleCollection
{
protected:
	std::vector<std::unique_ptr<CPuzzle>> m_puzzles;
public:
	PuzzleCollection() {}
	template <typename ITERATOR> PuzzleCollection(ITERATOR begin, ITERATOR end) : m_puzzles(begin, end) {}
	virtual ~PuzzleCollection() {}

	virtual void push_back(std::unique_ptr<CPuzzle>&& pos) { m_puzzles.push_back(std::move(pos)); }

	virtual std::unique_ptr<CPuzzle> Get(std::size_t index) const { return m_puzzles[index]->Clone(); }
	virtual void Set(std::size_t index, std::unique_ptr<CPuzzle>&& pos) { m_puzzles[index] = std::move(pos); }

	virtual std::size_t size() const { return m_puzzles.size(); }
};