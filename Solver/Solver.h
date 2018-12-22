#pragma once
#include "PuzzleCollection.h"
#include <memory>
#include <utility>
#include <utility>

class CSolver
{
	std::shared_ptr<PuzzleVectorGuard> m_puzzles;
	std::size_t m_node_counter;
	std::size_t m_threads;
	bool m_test;
	bool m_force;

public:
	CSolver(std::shared_ptr<PuzzleVectorGuard> puzzles,
			std::size_t threads,
			bool test, bool force, bool print_each_puzzle)
		: m_puzzles(std::move(puzzles))
		, m_node_counter(0)
		, m_threads(threads)
		, m_test(test)
		, m_force(force)
	{}

	void Solve(const Search::CAlgorithm&);
	std::size_t NodeCount() const;
};
